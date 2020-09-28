//////////////////////////////////////////////////////////////////////
// File:  PresentWinCS.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of DXGI present windowed converting/stretching swap
// chain tests.
//
// History:
// ?? ??? ????  AndrewLu  Created.
// 05 Oct 2007  SEdmison  Rewrote, cleaned up, and added feature level
//                          support.
// 29 July 1010 olanh     Added Flip Model, DirtyRects, and scrolling.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "TestApp.h"

#pragma warning(push, 4)

#include "DXGIBBFMT.H"
#include "DXGIValueSets.h"
#include "BBDims.h"
#include "DCOMPIsolation.H"

#include <StrSafe.h>

#pragma warning(disable: 4355)

// Defines
#define MAX_PRESENT_TIMEOUT 2000

// Globals
// Window sizes
const RECT g_FocusWindowSizes[] =
{
	// left, top, right, bottom
	{0, 0, 801, 600},
	{0, 0, 31, 15},
	{0, 0, 1, 1},
	{0, 0, 0, 0},
	{0, 0, 1, 600},	// veritcal strip
	{0, 0, 800, 1},	// horizontal strip
};

/////////////////////////////////////////////////////////////////
//
// CPresentWinConvertStretch implementation
//
/////////////////////////////////////////////////////////////////

#define TEMPLATE_INVARIANT(ReturnType, Class, MethodProto) template<bool b> ReturnType Class##_<b>::##MethodProto

TEMPLATE_INVARIANT(, CPresentWinConvertStretch, CPresentWinConvertStretch_(DXGI_FORMAT RestrictFormat)) :
	m_bStretchBB( true ),
	m_pBackBufferRendered(NULL),
	m_pBackBufferCleared(NULL),
	m_adapterNoOutputs(false),
	m_iWindow(0),
	m_StartFullScreen(false),
	m_iBBDimIndex(0),
	m_OutputModeIndex(0),
	m_bDWM(true),
	m_LastHmon(0),
	m_iLastAdapterOrdinal(-1),
	m_iLastOutputOrdinal(-1),
	m_SwapChainFlags(DXGISwapChainFlag[0]),
	m_bExpectDeviceLoss(false),
	m_dwPresentTime(0),
	m_bStopNow(false),
	FormatRestriction(RestrictFormat)
{
	m_SwapEffect.dxgiSwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	m_SwapEffect.szSwapEffectName = _T("DXGI_SWAP_EFFECT_DISCARD");

	ZeroMemory( &m_DesktopModes, MAX_DISPLAYMODES * sizeof( clsEnumModesGDI::DesktopModeRecord ) );

	m_DesktopModes[0].uiWidth = 1024;
	m_DesktopModes[0].uiHeight = 768;
	m_DesktopModes[0].uiBPP = 32;

	// Only test bpp 16 modes on Win7+8IP and below
	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);
	if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2) )
	{
		m_DesktopModes[1].uiWidth = 1024;
		m_DesktopModes[1].uiHeight = 768;
		m_DesktopModes[1].uiBPP = 16;
	}

	m_numberOfModes = 2;

	m_NumberOfPresents = 1;
	m_TexturesToLoad = m_NumberOfPresents;
	m_MaxBufferCount = 1;

	ZeroMemory( &m_OutputModeSet, MAX_DISPLAYMODES * sizeof( ModeInformation ) );
	ZeroMemory( &m_ModeInformation, sizeof( ModeInformation ) );

	m_BufferCount=2;  //Override default value of 1.  Flip Model requires at least two back buffers.  Blt model doesn't care.
}

TEMPLATE_INVARIANT(void, CPresentWinConvertStretch, InitModeParameters())
{
	WriteToLog(" CPresentWinConvertStretch::InitModeParameters");

	// software adapters have no outputs.
	if( g_Application.m_DXGIOptions.AdapterType[0] != TEST_DXGI_ADAPTER_TYPE_FULL_HW )
		m_adapterNoOutputs = true;

	HRESULT hresult = S_OK;
	std::vector < ModeInformation > testModeData;
	std::vector < Adapter > adapterModes;
	std::vector < Adapter > dwmOffModes;
	std::vector < Adapter > dwmOnModes;
	std::vector < bool > bTestOrdinals;
	bool bOverallSuccess = false;
	bool bModeParametersHaveBeenAdded = false;

	//
	// Fill the initial mode\monitor with info for all monitors from GDI
	//
	DisplayModeForRemote();
	if(MonitorModePairs.size() == 0)
	{
		WriteToLog("MonitorModePairs size is 0");
		goto Cleanup;
	}


	//
	// Convert to a list of ModeInformations
	//
	Convert(&MonitorModePairs, &testModeData);


	//
	// Get a tree version
	//                      adapters
	//                         / \
	//                   output  output
	//                     |       |
	//  backbuffer format list    backbuffer format list
	//
	FromFlatList(&testModeData, &adapterModes, (UINT) DXGI_MODE_ROTATION_IDENTITY - 1);

	//
	// Get Adapters with no outputs
	//
	DXGI_FACTORY_POINTER factoryPointer;

	hresult = g_Application.CreateDXGIFactory(&factoryPointer);

	for( UINT adapter = 0; adapter < 10; ++adapter)
	{
		DXGI_ADAPTER_POINTER adapterPointer;
		adapterPointer.pAdapter = NULL;

		// EnumAdapter1 is called by EnumAdapters if factory is created by CreateFactory1
		hresult = factoryPointer.pFactory->EnumAdapters(
			adapter,
			&adapterPointer.pAdapter);

		if ( hresult == DXGI_ERROR_NOT_FOUND )
		{
			break;
		}
		else if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CPresentWinConvertStretch::InitModeParameters:  EnumAdapters on adapter %u failed with error 0x%x.",
				adapter,
				hresult
			);

			goto Cleanup;
		};

		if ( adapterPointer.pAdapter == NULL )
		{
			WriteToLog
			(
				"CPresentWinConvertStretch::InitModeParameters:  EnumAdapters on adapter %u succeeded but returned a NULL adapter pointer.",
				adapter
			);

			hresult = E_FAIL;
			goto Cleanup;
		};

		DXGI_ADAPTER_DESC AdapterDescriptor;

		if ( adapterPointer.pAdapter->GetDesc( &AdapterDescriptor ) == S_OK)
		{
			m_vecAdapterDescriptors.push_back( AdapterDescriptor );
			if( AdapterDescriptor.Description != NULL )
			{
				TCHAR* pszAdapterDesc = new TCHAR[ 128 ];

				// copied from CDXGITestFramework::UpdateCurrentDeviceInfo. WCHAR->TCHAR
#ifdef UNICODE
				StringCchCopy
				(
					pszAdapterDesc,
					128,
					AdapterDescriptor.Description
				);
#else

#ifdef DBG
				int cCharactersConverted =
#endif
				WideCharToMultiByte
				(
					CP_ACP,
					WC_NO_BEST_FIT_CHARS,
					AdapterDescriptor.Description,
					-1,
					pszAdapterDesc,
					128,
					NULL,
					NULL
				);

				assert( cCharactersConverted > 0 );
#endif

				if( g_Application.m_DXGIOptions.Version >= TEST_DXGI_VERSION_1_1 && g_Application.m_DXGIOptions.Version < TEST_DXGI_VERSION_BESTAVAILABLE )
				{
					// AdapterType[0] is the Src adapter type
					// Allow renderonly HW if it's the first adapter, since that means it has the output
					// and therefore we are running in a hybrid system
					UINT adapterType = g_Application.m_DXGIOptions.AdapterType[0];
					adapterType |= (adapter == 0 ? TEST_DXGI_ADAPTER_TYPE_RENDERONLY_HW : 0);
					if( g_Application.IsAdapterOfType( adapterPointer.pAdapter1, adapterType )  )
					{
						// Test the adapter ad this ordinal
						bTestOrdinals.push_back( true );
						WriteToLog
						(
							"CPresentWinConvertStretch::InitModeParameters:  Adding adapter at AdapterOrdinal: %d : AdapterDescription: %s",
							adapter,
							pszAdapterDesc
						);
					}
					else
					{
						delete [] pszAdapterDesc;
						pszAdapterDesc = NULL;

						SAFE_RELEASE( adapterPointer.pAdapter );

						bTestOrdinals.push_back( false );

						continue;
					}

					delete [] pszAdapterDesc;
					pszAdapterDesc = NULL;
				}
			}
		}

		for
		(
			UINT uiOutputOrdinal = 0;
			uiOutputOrdinal < MAX_OUTPUTS;
			++uiOutputOrdinal
		)
		{
			IDXGIOutput* pDXGIOutput = NULL;
			HRESULT hresult = adapterPointer.pAdapter->EnumOutputs(uiOutputOrdinal, &pDXGIOutput);

			if ( SUCCEEDED( hresult ) && pDXGIOutput != NULL )
			{
				m_adapterNoOutputs = false;
				pDXGIOutput->Release();
				break;
			}
		};

		if (m_adapterNoOutputs)
		{
			Adapter adapter;
			adapter.outputAttached = false;
			adapterModes.push_back(adapter);
			WriteToLog("adpater with no outputs");
		}

		if(adapterPointer.pAdapter != NULL)
		{
			adapterPointer.pAdapter->Release();
		}
	}

	if ( factoryPointer.pFactory != NULL )
	{
		factoryPointer.pFactory->Release();
		factoryPointer.pFactory = NULL;
	};


	//
	// Can not go fullscreen on an output created on a different adapter.
	// Don't test NoOutputAdapter for remote sessions either
	//
	if( !m_StartFullScreen && (g_Application.IsTerminalServices() == false ||
		GetSystemMetrics( SM_REMOTESESSION) == 0))
	{
		//
		// If there are adapters with no outputs, copy
		// modeinfo from the default monitor
		//
		CopyDefaultOutputToNoOutputAdapters(&adapterModes);
	}

	SeperateDwmOnAndOffModes
	(
		&adapterModes,
		&dwmOnModes,
		&dwmOffModes
	);


	//
	//  Iterate the through the tree and group each output's modes as additive parameter
	//

	//
	// find the TEST_FEATURE_LEVEL it is a parameter and is not available in InitParameters.
	//
	CDXGITestFramework * pFramework = dynamic_cast< CDXGITestFramework * >( GetFramework() );
	TEST_FEATURE_LEVEL testFeatureLevel = TEST_FEATURE_10_0_LEVEL_10_0;
	for(UINT i = 0; i < TEST_FEATURE_COUNT; i++)
	{
		if(pFramework->m_RequestedFeatureLevels[i] == true)
		{
			testFeatureLevel = (TEST_FEATURE_LEVEL)i;
		}
	}

	WriteToLog(_T("InitModeParameters testFeatureLevel %d"),testFeatureLevel);

	int iTotalModes = 0;
	int iModeOffset = 0;

	for(UINT dwm = 0; dwm < 2 ;dwm++)
	{
		adapterModes.clear();

		if(dwm == 0)
		{
			adapterModes = dwmOffModes;
		}
		else
		{
			adapterModes = dwmOnModes;
		}


		for(UINT k = 0; k < adapterModes.size();k++)
		{
			if( k < bTestOrdinals.size() && !bTestOrdinals[k] )
				continue;

			D3D_DEVICE_POINTER devicePointer;
			ZeroMemory(&devicePointer, sizeof(devicePointer));

			hresult = CreateAdapterDevice(testFeatureLevel, k, &devicePointer);
			if ( FAILED( hresult ) )
			{
				WriteToLog
				(
					"CPresentWinConvertStretch::InitModeParameters:  CreateAdapterDevice failed with error 0x%x.",
					hresult
				);

				goto Cleanup;
			};

			for(UINT j = 0; j < adapterModes[k].OutputModeContainer.size();j++)
			{
				//
				// Transform the modes for specific test.  Things like add multisample quality
				// or check the formats
				//

				std::vector < ModeInformation > testedFormats;
				hresult = ModeListTransform(
					devicePointer,
					&adapterModes[k].OutputModeContainer[j].RawModes[(UINT)(DXGI_MODE_ROTATION_IDENTITY) - 1],
					&testedFormats);


				if(FAILED(hresult))
				{
					if ( NULL != devicePointer.pDevice)
					{
						devicePointer.pDevice->Release();
						devicePointer.pDevice = NULL;
					};

					goto Cleanup;
				}

				if(testedFormats.size() == 0)
				{
					WriteToLog
					(
						_T("CPresentWinConvertStretch::ModeListTransform: Returned 0 modes. Adapter %d Output %d"),
						k,
						j
					);

					continue;
				}

				//TCHAR parameterName[ 255 ] = {0};

				//if(adapterModes[k].outputAttached)
				//{
				//    StringCchPrintf
				//    (
				//        parameterName,
				//        ARRAYSIZE( parameterName ),
				//        _T("BBFormat_Adapter_%d_Output_%d_%s"),
				//        testedFormats[0].uiDXGIAdapterOrdinal,
				//        testedFormats[0].uiDXGIOutputOrdinal,
				//        dwm ? _T("dwmOnBpp") : _T("dwmOffBpp")
				//    );
				//}
				//else
				//{
				//    StringCchPrintf
				//    (
				//        parameterName,
				//        ARRAYSIZE( parameterName ),
				//        _T("BBFormat_Adapter_%d_No_Output_Use_Default_%d_%s"),
				//        testedFormats[0].uiDXGIAdapterOrdinal,
				//        testedFormats[0].uiDXGIOutputOrdinal,
				//        dwm ? _T("dwmOnBpp") : _T("dwmOffBpp")
				//    );
				//}


				if ( testedFormats.size() > 0 )
				{
					for(UINT z = 0 ; z < testedFormats.size() && z < MAX_MODES_PER_OUTPUT; z++)
					{
						const auto& Current = testedFormats[z];
						if (FormatRestriction != DXGI_FORMAT_UNKNOWN && Current.ModeDesc.Format != FormatRestriction)
						{
							continue;
						}

						assert( iTotalModes < MAX_DISPLAYMODES );
						m_OutputModeSet[ iTotalModes ] = Current;
						iTotalModes++;
					}
				}
			}

			if ( NULL != devicePointer.pDevice)
			{
				devicePointer.pDevice->Release();
				devicePointer.pDevice = NULL;
			};
		}

		if ( iTotalModes > iModeOffset )
		{
			RFactor hmonitor, devmode, format, width, height, refreshrate[2], scanlineordering, scaling, adapter, output, multisamplecount, multisamplequality, isInitialized;

			// Add the parameters if they've not been added yet
			// We add parameters right before we add value sets to them to ensure they're never empty
			if( !bModeParametersHaveBeenAdded )
			{
				assert( nullptr == CVariationGenerator::GetTestCaseParameter( _T("hmonitor") ) );
				AddParameter( _T( "hmonitor" ), reinterpret_cast< UINT* >(&m_ModeInformation.hmonitor) );
				AddParameter( _T( "devmode" ),&m_ModeInformation.devmode );
				AddParameter( _T( "ModeDesc.Format" ),&m_ModeInformation.ModeDesc.Format );
				AddParameter( _T( "ModeDesc.Width" ), &m_ModeInformation.ModeDesc.Width );
				AddParameter( _T( "ModeDesc.Height" ),&m_ModeInformation.ModeDesc.Height );
				AddParameter( _T( "DXGIAdapterOrdinal" ),&m_ModeInformation.uiDXGIAdapterOrdinal );
				AddParameter( _T( "DXGIOutputOrdinal" ),&m_ModeInformation.uiDXGIOutputOrdinal );
				AddParameter( _T( "MultisampleCount" ),&m_ModeInformation.uiMultisampleCount );
				AddParameter( _T( "MultisampleQuality" ),&m_ModeInformation.uiMultisampleQuality );
				AddParameter( _T( "isInitialized" ),&m_ModeInformation.isInitialized );
				bModeParametersHaveBeenAdded = true;
			}

			hmonitor = AddParameterValueSet(
				"hmonitor",
				new CTableValueSet< UINT >(
					reinterpret_cast< UINT* >(
					&m_OutputModeSet[iModeOffset].hmonitor),
					sizeof( ModeInformation ),
					iTotalModes - iModeOffset) );

			devmode = AddParameterValueSet(
				"devmode",
				new CTableValueSet< DEVMODEW >(
					&m_OutputModeSet[iModeOffset].devmode,
					sizeof( ModeInformation ),
					iTotalModes - iModeOffset) );

			format = AddParameterValueSet(
				_T( "ModeDesc.Format" ),
				new CTableValueSet< DXGI_FORMAT >(
					&m_OutputModeSet[iModeOffset].ModeDesc.Format,
					sizeof( ModeInformation ),
					iTotalModes - iModeOffset) );

			width = AddParameterValueSet(
				_T( "ModeDesc.Width" ),
				new CTableValueSet< UINT >(
					&m_OutputModeSet[iModeOffset].ModeDesc.Width ,
					sizeof( ModeInformation ),
					iTotalModes - iModeOffset) );

			height = AddParameterValueSet(
				_T( "ModeDesc.Height" ),
				new CTableValueSet< UINT >(
					&m_OutputModeSet[iModeOffset].ModeDesc.Height ,
					sizeof( ModeInformation ),
					iTotalModes - iModeOffset) );

			adapter = AddParameterValueSet(
				_T( "DXGIAdapterOrdinal" ),
				new CTableValueSet< UINT >(
					&m_OutputModeSet[iModeOffset].uiDXGIAdapterOrdinal,
					sizeof( ModeInformation ),
					iTotalModes - iModeOffset) );

			output = AddParameterValueSet(
				_T( "DXGIOutputOrdinal" ),
				new CTableValueSet< UINT >(
					&m_OutputModeSet[iModeOffset].uiDXGIOutputOrdinal,
					sizeof( ModeInformation ),
					iTotalModes - iModeOffset) );

			multisamplecount = AddParameterValueSet(
				_T( "MultisampleCount" ),
				new CTableValueSet< UINT >(
					&m_OutputModeSet[iModeOffset].uiMultisampleCount,
					sizeof( ModeInformation ),
					iTotalModes - iModeOffset) );

			multisamplequality = AddParameterValueSet(
				_T( "MultisampleQuality" ),
				new CTableValueSet< UINT >(
					&m_OutputModeSet[iModeOffset].uiMultisampleQuality,
					sizeof( ModeInformation ),
					iTotalModes - iModeOffset) );

			isInitialized = AddParameterValueSet(
				_T( "isInitialized" ),
				new CTableValueSet< BOOL >(
					&m_OutputModeSet[iModeOffset].isInitialized,
					sizeof( ModeInformation ),
					iTotalModes - iModeOffset) );

			RFactor backBufferFormatFactor = hmonitor % devmode % format % width % height % adapter % output % multisamplecount % multisamplequality % isInitialized;

			if(dwm == 0)
			{
				m_globalRFactorDwmOffBpp = backBufferFormatFactor;
				iModeOffset = iTotalModes;
			}
			else
			{
				m_globalRFactorDwmOnBpp = backBufferFormatFactor;
			}
		}

		// Add pattern filters if the mode parameters have been added
		if( bModeParametersHaveBeenAdded )
		{
			if( adapterModes.size() < 2)
				AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 0 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 0 )), WeightFactorLarge);

			AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 1 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 0 )), WeightFactorLarge);
			AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 0 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 1 )), WeightFactorLarge);
			AddPriorityPatternFilter(FilterLess< UINT >(_T("MultisampleQuality"), 2 ), WeightFactorLarge);
			AddPriorityPatternFilter(FilterEqual< DXGI_FORMAT >(_T("ModeDesc.Format"), DXGI_FORMAT_R8G8B8A8_UNORM ), WeightFactorLarge);
			AddPriorityPatternFilter(FilterEqual< DXGI_FORMAT >(_T("ModeDesc.Format"), DXGI_FORMAT_B8G8R8A8_UNORM ), WeightFactorLarge);
		}
	}

	// If we got here, success!
	bOverallSuccess = true;

Cleanup:;
	if( !bOverallSuccess )
	{
		LogError( __FILEW__, __LINE__, L"InitModeParameters() failed.", false, ERRORCODE_TYPE_BOOL );
	}
}


TEMPLATE_INVARIANT
(
	HRESULT,
	CPresentWinConvertStretch,
	ModeListTransform
	(
		D3D_DEVICE_POINTER devicePointer,
		__in std::vector< ModeInformation >* pModes,
		__out std::vector< ModeInformation >* pTransformedModes
	)
)
{
	HRESULT hr = GetSupportedFormats
	(
		devicePointer,
		pModes,
		pTransformedModes,
		m_RequiredFormatSupport
	);

	return hr;
}


TEMPLATE_INVARIANT
(
	void,
	CPresentWinConvertStretch,
	InitTestParameters()
)
{
	//
	// Call the base class implementation.
	//

	CDXGITest::InitTestParameters();

	// If testing on BasicRender device, then enable adapters with no outputs
	m_adapterNoOutputs = ( _wcsicmp( g_Application.m_D3DOptions.SoftwareRasterizerDLLName, L"BasicRender.sys" ) == 0 );


	//
	// Enumerate\Initailize modes
	//

	InitModeParameters();

	InitTestParametersImpl();
}


TEMPLATE_INVARIANT
(
	TEST_RESULT,
	CPresentWinConvertStretch,
	Setup()
)
{
	//
	// Initialize locals.
	//

	TEST_RESULT tResult = RESULT_FAIL;

	//
	// Initialize our window class.
	//

	bool bInitializeWindowClassResult = InitializeWindowClass();

	if ( bInitializeWindowClassResult != true )
	{
		WriteToLog
		(
			_T( "Setup:  InitializeWindowClass failed." )
		);

		tResult = RESULT_ABORT;
		goto Cleanup;
	};


	tResult = RESULT_PASS;


Cleanup:
	return tResult;
};


TEMPLATE_INVARIANT
(
	void,
	CPresentWinConvertStretch,
	Cleanup()
)
{
	DebugOut.Write( "Entering Cleanup.\n" );


	//
	// Unregister our window class.
	//

	CleanupWindowClass();


	DebugOut.Write( "Exiting Cleanup.\n" );
}


TEMPLATE_INVARIANT
(
	TEST_RESULT,
	CPresentWinConvertStretch,
	CreateDeviceAndSwapChain()
)
{
	//
	// Initialize locals.
	//

	TEST_RESULT test_result = RESULT_FAIL;
	HRESULT hresult = E_FAIL;


	//
	// Release objects that we might be holding.
	//

	ReleaseTestCaseObjects();


	//
	// Turn DWM off if necessary.
	//

	if( m_ModeInformation.devmode.dmBitsPerPel >= 32)
	{
		BOOL bDWMState = FALSE;
		DwmIsCompositionEnabled( &bDWMState );
		if
			(
			bDWMState == TRUE && m_bDWM == false ||
			bDWMState == FALSE && m_bDWM == true
			)
		{
			WriteToLog("DwmEnableComposition() : Changing DWM Composition state for 32bbp modes testing.");

			g_Application.DwmEnableComposition( ( m_bDWM == false ) ? FALSE : TRUE );
			Sleep( 1000 );

			//
			// Pump messages while we wait for DWM to shut down.
			//
			ProcessMessages();
		};
	}


	//
	// Initialize the device.
	//

	assert( m_DevicePointer.pDevice == NULL );
	hresult = g_Application.CreateFactoryAdapterDevice
	(
		&m_FactoryPointer,
		m_ModeInformation.uiDXGIAdapterOrdinal,
		&m_AdapterPointer,
		g_Application.m_D3DOptions.SrcDriverType,
		D3D10_CREATE_DEVICE_SINGLETHREADED,
		m_TestFeatureLevel,
		0,
		&m_DevicePointer
	);

	if
	(
		SUCCEEDED (hresult) &&
		m_AdapterPointer.pAdapter != NULL
	)
	{
		WriteToLog
		(
			"CreateFactoryAdapterDevice succeeded on Adapter %u.",
			m_ModeInformation.uiDXGIAdapterOrdinal
		);
	}

	if
	(
		hresult == DXGI_ERROR_NOT_FOUND &&
		m_AdapterPointer.pAdapter == NULL
	)
	{
		WriteToLog
		(
			"No adapter found at ordinal %u.",
			m_ModeInformation.uiDXGIAdapterOrdinal
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	}
	else if
	(
		hresult == DXGI_ERROR_UNSUPPORTED ||
		hresult == E_NOINTERFACE ||
		hresult == E_NOTIMPL ||
		hresult == E_FAIL &&
		(
			m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_1 ||
			m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_2 ||
			m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_3 ||
			m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_1 ||
			m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_2 ||
			m_TestFeatureLevel == TEST_FEATURE_11_0_LEVEL_9_3
		)
	)
	{
		WriteToLog
		(
			"Could not create device."
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	}
	else if(IsDeviceLost(hresult) && m_bExpectDeviceLoss)
	{
		test_result = RESULT_SKIP;
		goto Cleanup;
	}
	else if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentWinConvertStretch::CreateDeviceAndSwapChain:  CreateFactoryAdapterDevice failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	if ( m_DevicePointer.pDevice == NULL )
	{
		WriteToLog
		(
			"CPresentWinConvertStretch::CreateDeviceAndSwapChain:  CreateFactoryAdapterDevice succeeded but returned NULL device pointer."
		);

		goto Cleanup;
	};

	//
	// Wrap this device.
	//

	PrimaryAdapterDevice.WrapDevice( m_DevicePointer.pDevice );

	//
	// Associate the focus window with the factory.
	//

	assert( m_FactoryPointer.pFactory != NULL );
	hresult = m_FactoryPointer.pFactory->MakeWindowAssociation
	(
		m_hwndFocusWindow,
		0
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentWinConvertStretch::CreateDeviceAndSwapChain:  MakeWindowAssociation failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};


	//
	// Specify the swap chain properties.
	//

	{
		const auto bbDim = SwapChainDimension();
		m_SCDesc.BufferDesc.Width = bbDim.x();
		m_SCDesc.BufferDesc.Height = bbDim.y();
	}

	m_SCDesc.BufferDesc.RefreshRate.Numerator = 0;
	m_SCDesc.BufferDesc.RefreshRate.Denominator = 1;
	m_SCDesc.BufferDesc.Format = m_ModeInformation.ModeDesc.Format;

	m_SCDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	m_SCDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	m_SCDesc.SampleDesc.Count = 1;
	m_SCDesc.SampleDesc.Quality = 0;
	m_SCDesc.Windowed = TRUE;


	m_SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	if(!m_SCDesc.Windowed)
	{
		m_SCDesc.BufferUsage |= DXGI_USAGE_BACK_BUFFER;
	}

	m_SCDesc.BufferCount = m_SwapEffect.dxgiSwapEffect == DXGI_SWAP_EFFECT_DISCARD ? 1: m_BufferCount;

	m_SCDesc.OutputWindow = m_tp_bWindowless ? NULL : m_hwndDeviceWindow;


	m_SCDesc.SwapEffect = m_SwapEffect.dxgiSwapEffect;
	m_SCDesc.Flags = m_SwapChainFlags.dxgiSwapChainFlag;

	DXGI_SWAP_CHAIN_DESC SCMSDesc;
	memcpy( &SCMSDesc, &m_SCDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );

	SCMSDesc.SampleDesc.Count = m_ModeInformation.uiMultisampleCount;
	SCMSDesc.SampleDesc.Quality = m_ModeInformation.uiMultisampleQuality;


	//
	// Check for format support.
	//

	UINT FormatSupport = 0;
	hresult = m_DevicePointer.pDevice->CheckFormatSupport
	(
		m_SCDesc.BufferDesc.Format,
		&FormatSupport
	);

	if ( hresult == E_FAIL )
	{
		WriteToLog
		(
			"Device does not support this format."
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	}
	else if(IsDeviceLost(hresult) && m_bExpectDeviceLoss)
	{
		test_result = RESULT_SKIP;
		goto Cleanup;
	}
	else if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentWinConvertStretch::CreateDeviceAndSwapChain:  CheckFormatSupport failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	C_ASSERT( D3D10_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET == D3D11_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET );

	UINT RequiredFormatSupport = m_RequiredFormatSupport |
		( m_ModeInformation.uiMultisampleCount > 1 ? D3D10_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET : 0 );

	if ( ( FormatSupport & RequiredFormatSupport ) != RequiredFormatSupport )
	{
		WriteToLog
		(
			"Device does not support capabilities with this format needed to test it."
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	};


	//
	// Create our swap chains.
	//

	DebugOut.Write("CPresentWinConvertStretch::CreateDeviceAndSwapChain:  Initializing swap chain.\n");

	if
	(
		TestedSwapChain.InitializeSwapChain( &m_SCDesc ) != TRUE
	)
	{
		WriteToLog
		(
			"CPresentWinConvertStretch::CreateDeviceAndSwapChain:  InitializeSwapChain failed."
		);

		if( m_bLargeMemoryUsage && TestedSwapChain.GetLastError() == E_OUTOFMEMORY )
		{
			test_result = RESULT_SKIP;
			WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Swapchain creation failure expected. Skipping test case. " );
		}

		goto Cleanup;
	};

	assert( m_pSwapChain == NULL );
	TestedSwapChain.GetSwapChainPtr(&m_pSwapChain);
	if ( m_pSwapChain == NULL )
	{
		WriteToLog
		(
			"CPresentWinConvertStretch::CreateDeviceAndSwapChain:  Swap chain pointer is NULL."
		);

		goto Cleanup;
	};


	if ( m_ModeInformation.uiMultisampleCount > 1 )
	{
		if
		(
			TestedSwapChainMS.InitializeSwapChain( &SCMSDesc ) != TRUE
		)
		{
			WriteToLog
			(
				"CPresentWinConvertStretch::CreateDeviceAndSwapChain:  InitializeSwapChain on multisampled swap chain failed."
			);

			if( m_bLargeMemoryUsage && TestedSwapChainMS.GetLastError() == E_OUTOFMEMORY )
			{
				test_result = RESULT_SKIP;
				WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Swapchain creation failure expected. Skipping test case. " );
			}

			goto Cleanup;
		};

		//
		// Set for cleanup of TestedSwapChainMS
		//

		m_MultisampleTest = true;
	};


	if ( m_ModeInformation.uiMultisampleCount > 1 )
	{
		assert( m_pSwapChainMS == NULL );
		TestedSwapChainMS.GetSwapChainPtr( &m_pSwapChainMS );
		if ( m_pSwapChainMS == NULL )
		{
			WriteToLog
			(
				"CPresentWinConvertStretch::CreateDeviceAndSwapChain:  Multisample swap chain pointer is NULL."
			);

			goto Cleanup;
		};
	};


	//
	// Dump the back buffer format.
	//

	DebugOut.Write("***********************************\n");
	DebugOut.Write("CPresentWinConvertStretch::CreateDeviceAndSwapChain:  CreateSwapChain succeeded!\n");

	{
		const DXGIFormatInfo *pBBFormatInfo = NULL;
		if (pBBFormatInfo = CFormatInfoMap::Find((m_SCDesc.BufferDesc).Format))
		{
			DebugOut.Write(pBBFormatInfo->szDesc);
			DebugOut.Write("\n");
		}
	}

	DebugOut.Write("***********************************\n");


	//
	// Initialize the D3D 10 device resources needed
	// for texture rendering.
	//

	bool bCreateResourcesResult = false;
	switch ( m_TestFeatureLevel )
	{
		case TEST_FEATURE_11_0_LEVEL_9_1:
		case TEST_FEATURE_11_0_LEVEL_9_2:
		case TEST_FEATURE_11_0_LEVEL_9_3:
		case TEST_FEATURE_10_1_LEVEL_9_1:
		case TEST_FEATURE_10_1_LEVEL_9_2:
		case TEST_FEATURE_10_1_LEVEL_9_3:
		{
			bCreateResourcesResult = PrimaryAdapterDevice.CreateTexRenderResources
			(
				"vs_4_0_level_9_1",
				"ps_4_0_level_9_1"
			);
		} break;

		default:
		{
			bCreateResourcesResult = PrimaryAdapterDevice.CreateTexRenderResources
			(
				"vs_4_0",
				"ps_4_0"
			);
		} break;
	};


	if ( bCreateResourcesResult != true )
	{
		if(IsDeviceLost(PrimaryAdapterDevice.GetLastError()) && m_bExpectDeviceLoss)
		{
			test_result = RESULT_SKIP;
			goto Cleanup;
		}

		WriteToLog
		(
			"CreateTexRenderResources failed."
		);

		if( m_bLargeMemoryUsage && PrimaryAdapterDevice.GetLastError() == E_OUTOFMEMORY )
		{
			test_result = RESULT_SKIP;
			WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Resource creation failure expected. Skipping test case. " );
		}

		goto Cleanup;
	};

	// Instantiate DCOMP if necessary
	if (m_tp_bWindowless)
	{
		if (FAILED(m_DCOMPIsolation.CreateDCompResources(m_pSwapChain, m_hwndDeviceWindow, ScaleToWindow())))
			goto Cleanup;
	}

	//
	// If we made it this far, then we're good to go.
	//

	test_result = RESULT_PASS;


Cleanup:
	if
	(
		m_AdapterPointer.pAdapter != NULL &&
		test_result != RESULT_PASS
	)
	{
		m_AdapterPointer.pAdapter->Release();
		m_AdapterPointer.pAdapter = NULL;
	};


	return test_result;
}


TEMPLATE_INVARIANT
(
	HRESULT,
	CPresentWinConvertStretch,
	UpdateReferenceTexture()
)
{
	HRESULT hr = S_OK;

	if ( m_ModeInformation.uiMultisampleCount <= 1 )
	{
		//
		// Get a descriptor for the swap chain.
		//

		DXGI_SWAP_CHAIN_DESC swapChainDesc;
		hr = m_pSwapChain->GetDesc( &swapChainDesc);
		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"CDXGIPresentSingleSC::UpdatesRGBTexture:  GetDesc on swap chain failed with error 0x%x.",
				hr
			);
		};

		if( SUCCEEDED( hr ))
		{
			//
			// For fp16 backbuffers, we need to adjust the back buffer because LinearToGamma is added at present.
			//

			if ( DXGI_FORMAT_R16G16B16A16_FLOAT == swapChainDesc.BufferDesc.Format )
			{
				hr = LinearToGamma(m_pBackBufferRendered);
				if ( FAILED(hr))
				{
					WriteToLog(_T("LinearToGamma failed. 0x%x"), hr);
				};
			}
		}
	}

	return hr;
}


TEMPLATE_INVARIANT
(
	void,
	CPresentWinConvertStretch,
	CleanupTestCase()
)
{
	// Clean up DirectComposition if necessary
	m_DCOMPIsolation.DestroyDCompDeviceAndVisual();


	//
	// Release our swap chains.
	//

	if ( m_pSwapChain != NULL )
	{
		m_pSwapChain->SetFullscreenState( FALSE, NULL );

		m_pSwapChain->Release();
		m_pSwapChain = NULL;
	};

	if ( m_pSwapChainMS != NULL )
	{
		m_pSwapChainMS->SetFullscreenState( FALSE, NULL );

		m_pSwapChainMS->Release();
		m_pSwapChainMS = NULL;
	};


	//
	// Release device objects and device.
	//

	if ( m_DevicePointer.pDevice != NULL )
	{
		// @DX11PORT [SEdmison]:
		// Replace this.
		//D3DX10UnsetAllDeviceObjects( m_DevicePointer.pDevice );

		m_DevicePointer.pDevice->Release();
		m_DevicePointer.pDevice = NULL;
	};

	PrimaryAdapterDevice.ReleaseAllObjects();


	//
	// Release the adapter.
	//

	if ( m_AdapterPointer.pAdapter != NULL )
	{
		m_AdapterPointer.pAdapter->Release();
		m_AdapterPointer.pAdapter = NULL;
	};


	//
	// Break focus window association and release factory.
	//

	if ( m_FactoryPointer.pFactory != NULL )
	{
		HWND hwndCurrentFocus = NULL;

		m_FactoryPointer.pFactory->GetWindowAssociation( &hwndCurrentFocus );
		if ( hwndCurrentFocus != NULL )
		{
			m_FactoryPointer.pFactory->MakeWindowAssociation
			(
				NULL,
				0
			);
		};

		m_FactoryPointer.pFactory->Release();
		m_FactoryPointer.pFactory = NULL;
	};


	//
	// Destroy our device window.
	//

	if ( m_hwndDeviceWindow != NULL )
	{
		if ( m_hwndDeviceWindow != m_hwndFocusWindow )
		{
			DestroyWindow( m_hwndDeviceWindow );
		};

		m_hwndDeviceWindow = NULL;
	};


	//
	// Destroy our focus window.
	//

	if ( m_hwndFocusWindow != NULL )
	{
		DestroyWindow( m_hwndFocusWindow );
		m_hwndFocusWindow = NULL;
	};

};


class CleanupHelper
{
	public:
		CleanupHelper
		(
			ID3DTexture2D*& t_0,
			ID3DTexture2D*& t_1,
			ModeInformation& mi,
			HMONITOR& hm,
			int& aOrdinal,
			int& oOrdinal,
			CDXGIPresentSingleSC& test
		)
		:
			t0(t_0),
			t1(t_1),
			m(mi),
			h(hm),
			a(aOrdinal),
			o(oOrdinal),
			t(test)
		{
		}


		~CleanupHelper()
		{
			if (t0){ t0->Release(); t0 = nullptr; }

			if (t1){ t1->Release(); t1 = nullptr; }

			if(g_Application.m_bDFlip) ShowCursor(true);

			h = m.hmonitor;

			a = m.uiDXGIAdapterOrdinal;

			o = m.uiDXGIOutputOrdinal;

			t.ReleaseTestCaseObjects();

			DebugOut.Write("Exiting ExecuteTestCase.\n");
		}


	private:
		ID3DTexture2D*& t0;
		ID3DTexture2D*& t1;
		const ModeInformation& m;
		HMONITOR& h;
		int& a;
		int& o;
		CDXGIPresentSingleSC& t;

		CleanupHelper& operator=(const CleanupHelper&);
};


TEMPLATE_INVARIANT
(
	TEST_RESULT,
	CPresentWinConvertStretch,
	ExecuteTestCase()
)
{
	DebugOut.Write("m_NumberOfPresents %d m_TexturesToLoad %d\n", m_NumberOfPresents, m_TexturesToLoad);

	const CleanupHelper c
	(
		m_pBackBufferRendered,
		m_pBackBufferCleared,
		m_ModeInformation,
		m_LastHmon,
		m_iLastAdapterOrdinal,
		m_iLastOutputOrdinal,
		*this
	);


	//
	// Initialize locals.
	//

	TEST_RESULT test_result = RESULT_FAIL;
	TEST_RESULT CreateDeviceAndSwapChainResult = RESULT_FAIL;
	std::map< HMONITOR, DEVMODEW >::iterator itMode;
	UINT texturesToLoad = m_TexturesToLoad;

	// Should we include scrolling in this test case?
	bool bPerformScrollTest = m_ScrollInfo.bPerformScroll;

	if(g_Application.m_bDFlip)
	{
		ShowCursor(FALSE);
		if( m_tp_bWindowless ||
            !IsFlipModel(m_SwapEffect.dxgiSwapEffect) ||
			!m_bDWM)
		{
			WriteToLog("Parameters Not Applicable to DirectFLIP.");
			return RESULT_SKIP;
		}
	}


	if(m_ParameterError == -1)
	{
		WriteToLog
		(
			"CPresentWinConvertStretch::ExecuteTestCase m_ParameterError = %d. No supported modes or formats were found",
			m_ParameterError
		);

		return RESULT_SKIP;
	}

	MONITORINFOEXW monitorinfo;
	ZeroMemory( &monitorinfo, sizeof( monitorinfo ) );
	monitorinfo.cbSize = sizeof( monitorinfo );

	DEVMODEW desktopMode;
	ZeroMemory( &desktopMode, sizeof( DEVMODEW ) );
	desktopMode.dmSize = sizeof( DEVMODEW );

	BOOL bDWMState = FALSE;

	if(!m_ModeInformation.isInitialized)
	{
		WriteToLog
		(
			"CPresentWinConvertStretch::ExecuteTestCase Could not find Adapter-Output format parameter"
		);

		return RESULT_ABORT;
	}


	//
	// Increment the current test case index.
	//

	++m_iCurrentTestCase;

	if(m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_TEST || m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_DO_NOT_SEQUENCE)
	{
		texturesToLoad += 1;
	}

	if(m_bUseDirtyRects)
	{
		texturesToLoad += 1;
	}

	if(g_Application.m_DXGIOptions.AdapterIDSrc != ADAPTER_UNDEFINED)
	{
		if(g_Application.m_DXGIOptions.AdapterIDSrc != m_ModeInformation.uiDXGIAdapterOrdinal)
		{
			WriteToLog
			(
				"Requested adapter %d does not match test mode adapter %d.",
				g_Application.m_DXGIOptions.AdapterIDSrc,
				m_ModeInformation.uiDXGIAdapterOrdinal
			);

			return RESULT_SKIP;
		}
	}


	//
	// Check adapter sys/vid memory and skip cases that might fail due to too large memory usuage
	//

	// Backbuffers - in video memory ( not taking scaling into account )
	const auto d = Helper::Dimensions(FocusWindowRect());
	const size_t backBufferDim = (CFormatInfoMap::Find( m_ModeInformation.ModeDesc.Format )->uBitsPerElement >> 3 ) * d.x() * d.y() ;

	// In video memory: Backbuffers + textures bound as shader resources
	SIZE_T vidMemSize = backBufferDim * m_BufferCount * ( m_ModeInformation.uiMultisampleCount + 1 );	// multisample swapchain backbuffers with size*sample count +1 regular swapchain backbuffer
	vidMemSize += backBufferDim * texturesToLoad;

	// Staging textures: loaded reference textures + validation textures for screen grab
	SIZE_T sysMemSize = backBufferDim * texturesToLoad;
	sysMemSize += backBufferDim * 2 ; // 1 each for regular and multisample

	CDXGITestFramework * pFramework = dynamic_cast< CDXGITestFramework * >( GetFramework() );
	if( m_ModeInformation.uiDXGIAdapterOrdinal < m_vecAdapterDescriptors.size() )
	{
		pFramework->WriteToLogF	// Adapter Description is WCHAR
		(
			L"CPresentWinConvertStretch::ExecuteTestCase: Adapter %u : %s : vidMemReq=%Iu MB, sharedsysMemReq= %Iu MB, DedicatedVideoMemory= %Iu MB, DedicatedSystemMemory= %Iu MB, SharedSystemMemory= %Iu MB",
			m_ModeInformation.uiDXGIAdapterOrdinal,
			(const WCHAR*)m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].Description,
			(vidMemSize >> 20) + 1,
			(sysMemSize >> 20) + 1,
			m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].DedicatedVideoMemory >> 20,
			m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].DedicatedSystemMemory >> 20,
			m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].SharedSystemMemory >> 20
		);

		if( (m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].DedicatedVideoMemory / 2 ) <  vidMemSize || // rough estimate
			(m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].SharedSystemMemory / 2 ) <  (vidMemSize + sysMemSize) )
		{
			WriteToLog( "****WARNING: CPresentFS::ExecuteTestCase: Testcase uses large percentage of available video memory reported by the adapter. Unexpected behavior might occur." );
			m_bLargeMemoryUsage = true;
		}
	}
	else
	{
		WriteToLog(_T("CPresentWinConvertStretch::ExecuteTestCase : Could not find adapter description."));
		return RESULT_ABORT;
	}


	//
	// Validate Flip Model (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) compatible parameters
	//

	if (IsFlipModel(m_SwapEffect.dxgiSwapEffect))
	{
		switch (m_TestFeatureLevel)
		{
			// All levels except feature level 9 should
			// be in this list.  Please add them as they
			// are created.
			case TEST_FEATURE_10_0_LEVEL_10_0:
			case TEST_FEATURE_10_1_LEVEL_10_0:
			case TEST_FEATURE_10_1_LEVEL_10_1:
			case TEST_FEATURE_11_0_LEVEL_10_0:
			case TEST_FEATURE_11_0_LEVEL_10_1:
			case TEST_FEATURE_11_0_LEVEL_11_0:
			//uncomment these when DX11.1 support is added to DXGIFramework helpers.
			//            case TEST_FEATURE_11_1_LEVEL_10_0:
			//            case TEST_FEATURE_11_1_LEVEL_10_1:
			//            case TEST_FEATURE_11_1_LEVEL_11_0:
			//            case TEST_FEATURE_11_1_LEVEL_11_1:
				// Validate back buffer formats compatible with FLIP_SEQUENTIAL on Feature level 10 or greater.
				switch (m_ModeInformation.ModeDesc.Format)
				{
					case DXGI_FORMAT_R16G16B16A16_FLOAT:
					case DXGI_FORMAT_B8G8R8X8_UNORM:
					case DXGI_FORMAT_B8G8R8A8_UNORM:
					case DXGI_FORMAT_R8G8B8A8_UNORM:
						break;
					default:
						WriteToLog("*** Flip Model Not supported with this pixel format. ***");

						return RESULT_SKIP;
				}

				break; // End of DX10 or higher switch case block.

			default:
				// These are the only formats available on feature level 9.
				switch (m_ModeInformation.ModeDesc.Format)
				{
					case DXGI_FORMAT_B8G8R8X8_UNORM:
					case DXGI_FORMAT_B8G8R8A8_UNORM:
					case DXGI_FORMAT_R8G8B8A8_UNORM:
						break;
					default:
						WriteToLog("*** Flip Model Not supported with this pixel format. ***");
						return RESULT_SKIP;
				}
		}
	} // End of Flip Model compatibility checks
	else // If Blt model, we can't support DCOMP (Windowless presents).  This is temporary for M2.   Blt Model support is planned for Win8 M3.
	{
		if (m_tp_bWindowless)
		{
			WriteToLog("*** Windowless (DCOMP) presents require Flip Model.  Skipping this Blt Model test case. ***");
			return RESULT_SKIP;
		}
	}


	//
	// if switch monitors set the old monitor back to its original mode.
	//

	if (m_LastHmon != 0 && m_LastHmon != m_ModeInformation.hmonitor)
	{
		itMode = m_DesktopMode.find( m_LastHmon );

		MONITORINFOEXW monitorinfo;
		ZeroMemory( (LPVOID)&monitorinfo, sizeof(MONITORINFOEXW) );
		monitorinfo.cbSize = sizeof( MONITORINFOEXW );
		GetMonitorInfoW( (HMONITOR)itMode->first, &monitorinfo );

		bDWMState = FALSE;
		DwmIsCompositionEnabled( &bDWMState );

		// Switching Adapters in the following CDS
		LPDEVMODEW lpLastDevMode = (LPDEVMODEW)&( itMode->second );
		if ( bDWMState == TRUE && lpLastDevMode->dmBitsPerPel < 32 )
		{
			WriteToLog("DwmEnableComposition( false ) : Windows 7 bug 627384: Need to turn off DWM Composition before CDS into a 16bbp mode.");
			g_Application.DwmEnableComposition( false );
			Sleep( 1000 );

			//
			// Pump messages while we wait for DWM to shut down.
			//
			ProcessMessages();
		};

		ChangeDisplaySettingsExW( monitorinfo.szDevice, (LPDEVMODEW)&( itMode->second ), NULL, CDS_UPDATEREGISTRY, NULL );
		Sleep(5000);
		WriteToLog
		(
			"ChangeDisplaySettingsExW on Device:%s to restore last monitor to %d bpp %d x %d",
			monitorinfo.szDevice,
			(itMode->second).dmBitsPerPel,
			(itMode->second).dmPelsWidth,
			(itMode->second).dmPelsHeight
		);
	}


	//
	// Switch to the selected mode.
	//

	GetMonitorInfoW( m_ModeInformation.hmonitor, &monitorinfo );

	BOOL bGetCurrentModeResult = EnumDisplaySettingsW
	(
		monitorinfo.szDevice,
		ENUM_CURRENT_SETTINGS,
		&desktopMode
	);

	if ( bGetCurrentModeResult != TRUE )
	{
		DWORD GetCurrentModeError = GetLastError();

		WriteToLog
		(
			"EnumDisplaySettings failed with error %u.",
			GetCurrentModeError
		);

		return test_result;
	};


	{
		const auto& m = monitorinfo.rcMonitor;
		if (static_cast<unsigned>(m.right - m.left) != static_cast<unsigned>(desktopMode.dmPelsWidth) ||
			static_cast<unsigned>(m.bottom - m.top) != static_cast<unsigned>(desktopMode.dmPelsHeight) )
		{
			WriteToLog
			(
				"GetMonitorInfoW and EnumDisplaySettingsW ( ENUM_CURRENT_SETTINGS ) mismatch on Device:%s . desktopmode: %d bpp %d x %d monitorinfo: left %d right %d top %d bottom %d",
				monitorinfo.szDevice,
				desktopMode.dmBitsPerPel,
				desktopMode.dmPelsWidth,
				desktopMode.dmPelsHeight,
				m.left,
				m.right,
				m.top,
				m.bottom
			);
		}
	}


	//
	// Create a list of current modes per hmonitor.  To be restored at end of test
	//

	itMode = m_DesktopMode.find( m_ModeInformation.hmonitor );
	if ( m_DesktopMode.end() == itMode )
	{
		m_DesktopMode[ m_ModeInformation.hmonitor ] = desktopMode;
	};


	if
	(
		(m_ModeInformation.devmode.dmBitsPerPel != desktopMode.dmBitsPerPel ||
		m_ModeInformation.devmode.dmPelsWidth != desktopMode.dmPelsWidth ||
		m_ModeInformation.devmode.dmPelsHeight != desktopMode.dmPelsHeight ||
		m_ModeInformation.devmode.dmDisplayFrequency != desktopMode.dmDisplayFrequency ||
		m_ModeInformation.devmode.dmDisplayOrientation != desktopMode.dmDisplayOrientation ) &&
		m_UseCurrentDesktopMode == FALSE
	)
	{

		/*    WriteToLog
		(
			"ChangeDisplaySettingsExW? requested %d %d %d %d %d != current %d %d %d %d %d ors %d m_UseCurrentDesktopMode %d ors and m_UseCurrentDesktopMode %d ",
			m_ModeInformation.devmode.dmBitsPerPel,
			m_ModeInformation.devmode.dmPelsWidth,
			m_ModeInformation.devmode.dmPelsHeight,
			m_ModeInformation.devmode.dmDisplayFrequency,
			m_ModeInformation.devmode.dmDisplayOrientation,
			desktopMode.dmBitsPerPel,
			desktopMode.dmPelsWidth,
			desktopMode.dmPelsHeight,
			desktopMode.dmDisplayFrequency,
			desktopMode.dmDisplayOrientation,
			(m_ModeInformation.devmode.dmBitsPerPel != desktopMode.dmBitsPerPel ||
			m_ModeInformation.devmode.dmPelsWidth != desktopMode.dmPelsWidth ||
			m_ModeInformation.devmode.dmPelsHeight != desktopMode.dmPelsHeight ||
			m_ModeInformation.devmode.dmDisplayFrequency != desktopMode.dmDisplayFrequency ||
			m_ModeInformation.devmode.dmDisplayOrientation != desktopMode.dmDisplayOrientation ),
			m_UseCurrentDesktopMode,
			((m_ModeInformation.devmode.dmBitsPerPel != desktopMode.dmBitsPerPel ||
			m_ModeInformation.devmode.dmPelsWidth != desktopMode.dmPelsWidth ||
			m_ModeInformation.devmode.dmPelsHeight != desktopMode.dmPelsHeight ||
			m_ModeInformation.devmode.dmDisplayFrequency != desktopMode.dmDisplayFrequency ||
			m_ModeInformation.devmode.dmDisplayOrientation != desktopMode.dmDisplayOrientation ) &&
			m_UseCurrentDesktopMode == FALSE)
		);*/

		WriteToLog
		(
			"ChangeDisplaySettingsExW on Device:%s . From %d bpp %d x %d to %d bpp %d x %d",
			monitorinfo.szDevice,
			desktopMode.dmBitsPerPel,
			desktopMode.dmPelsWidth,
			desktopMode.dmPelsHeight,
			m_ModeInformation.devmode.dmBitsPerPel,
			m_ModeInformation.devmode.dmPelsWidth,
			m_ModeInformation.devmode.dmPelsHeight
		);

		// BUG: 627384 [Windows 7] : DXGIPresent winpresent tests can fail first few when switching adapters
		// To workaround this DWM needs to be turned off before the switching to a mode that turns off dwm (16bpp) on another adapter
		// Then resume DWM State (although DWM should be turned off after CDS to a 16bbp mode )
		bDWMState = FALSE;

		if
		(
			m_iLastAdapterOrdinal >= 0 && static_cast<unsigned>(m_iLastAdapterOrdinal) != m_ModeInformation.uiDXGIAdapterOrdinal ||
			m_iLastOutputOrdinal >= 0 && static_cast<unsigned>(m_iLastOutputOrdinal) != m_ModeInformation.uiDXGIOutputOrdinal ||
			m_ModeInformation.uiDXGIAdapterOrdinal > 0 ||
			m_ModeInformation.uiDXGIOutputOrdinal > 0
		)
		{
			DwmIsCompositionEnabled( &bDWMState );
			// Switching Adapters in the following CDS
			if ( bDWMState == TRUE && m_ModeInformation.devmode.dmBitsPerPel < 32 )
			{
				WriteToLog("Windows 7 bug 627384 : Turning off DWM Composition for 16bbp modes");
				g_Application.DwmEnableComposition( false );
				Sleep( 1000 );

				//
				// Pump messages while we wait for DWM to shut down.
				//
				ProcessMessages();
			};
		}

		LONG ChangeDisplaySettingsResult = ChangeDisplaySettingsExW
		(
			monitorinfo.szDevice,
			&(m_ModeInformation.devmode),
			NULL,
			CDS_UPDATEREGISTRY,
			NULL
		);

		if ( ChangeDisplaySettingsResult != DISP_CHANGE_SUCCESSFUL )
		{
			WriteToLog
			(
				"ChangeDisplaySettingsEx failed with error %d.",
				ChangeDisplaySettingsResult
			);

			return test_result;
		};

		Sleep( 5000 );

		// This is just to make sure that DWM is in the correct state after CDS
		DwmIsCompositionEnabled( &bDWMState );
		if
		(
			bDWMState == TRUE && m_bDWM == false ||
			bDWMState == FALSE && m_bDWM == true
		)
		{
			WriteToLog("DwmEnableComposition() : Correcting DWM Composition State after CDS. ");
			g_Application.DwmEnableComposition( ( m_bDWM == false ) ? FALSE : TRUE );
			Sleep( 1000 );

			//
			// Pump messages while we wait for DWM to shut down.
			//
			ProcessMessages();
		};
	};


	//
	// Get monitor coordinates again after mode change.
	//

	GetMonitorInfoW(m_ModeInformation.hmonitor, &monitorinfo);


	//
	// Log details about the selected test.
	//

	{
		const auto& m = monitorinfo.rcWork; // rcWork here ensures that the test window is not behind the taskbar
		WriteToLog(_T("Monitor (left x top x right x bottom )= %d x %d x %d x %d. Stretch = %.1f x %.1f."), m.left, m.top, m.right, m.bottom, ScaleFactor().x(), ScaleFactor().y());

		//
		// Create the window on the output we are testing
		const auto d = Helper::RotationAdjust(Helper::Dimensions(FocusWindowRect()), GetRotation());
		WriteToLog(_T("CreateTestWindows (left:%d,top:%d,width:%d,height:%d) "), FocusWindowRect().left + m.left, FocusWindowRect().top + m.top, d.x(), d.y());
		HRESULT hresult = CreateTestWindows(FocusWindowRect().left + m.left, FocusWindowRect().top + m.top, d.x(), d.y());
		if ( FAILED(hresult) )
		{
			WriteToLog(_T("CPresentWinConvertStretch::ExecuteTestCase:  CreateTestWindows failed. hr = 0x%x"), hresult);
			return RESULT_ABORT;
		}
	}


	//
	// Display the window.
	//

	ActivateWindow
	(
		m_hwndFocusWindow
	);

	ProcessMessages();

	m_hwndDeviceWindow = m_hwndFocusWindow;

	if(g_Application.m_bDFlip)
	{
		// NOTE: In order for DirectFlip to work, satisfy-
		// 1) Test Window covers Entire Display Area
		// 2) Swapchain Buffers need to match the window size.
		// Resize window to match entire Display area here since
		// Swapchain description is built off of window placement.
		SetWindowPos
		(
			m_hwndDeviceWindow,
			HWND_TOPMOST,
			0,
			0,
			GetSystemMetrics(SM_CXSCREEN),// Desktop Width
			GetSystemMetrics(SM_CYSCREEN),// Desktop Height
			SWP_SHOWWINDOW | SWP_FRAMECHANGED
		);
	}


	//
	// Determine the swap chain window's location
	//

	WINDOWINFO deviceWindowInfo;
	deviceWindowInfo.cbSize = sizeof( deviceWindowInfo );

	BOOL bGetWindowInfoResult = GetWindowInfo
	(
		m_hwndDeviceWindow,
		&deviceWindowInfo
	);

	if ( bGetWindowInfoResult != TRUE )
	{
		WriteToLog
		(
			"CPresentWinConvertStretch::ExecuteTestCase:  GetWindowInfo failed hwnd=0x%x.",
			m_hwndDeviceWindow
		);

		return RESULT_ABORT;
	};

	bool bDeviceLost = false;


	//
	// Set up our device and swap chain.
	//

	CreateDeviceAndSwapChainResult = CreateDeviceAndSwapChain();
	if ( CreateDeviceAndSwapChainResult == RESULT_FAIL )
	{
		WriteToLog
		(
			"CreateDeviceAndSwapChain failed."
		);

		return CreateDeviceAndSwapChainResult;
	}
	else if ( CreateDeviceAndSwapChainResult != RESULT_PASS )
	{
		return CreateDeviceAndSwapChainResult;
	}


	//
	// Initialize our Direct3D 9 device helper object.
	// Causing some failures in stress.  Exculding until
	// time comes to fix
	//

	if((g_Application.GetTestMode() != WGFTEST_MODE_STRESS))
	{
		//
		// Tests that start fullscreen need to recreate the d3d9 screen capture device.
		//
		HRESULT hInitializeD3D9DeviceResult = m_D3D9Device.Initialize
		(
			g_Application.IsTerminalServices(),
			m_StartFullScreen
		);

		if ( FAILED( hInitializeD3D9DeviceResult ) )
		{
			WriteToLog
			(
				"Could not initialize Direct3D 9 device helper object."
			);

			return RESULT_SKIP;
		};
	}


	//
	// Note that the properties of the textures created by
	// the CreateTextures method are dependent on the
	// properties and location of the output window, so
	// these must be established before the call to
	// CreateTextures.

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	HRESULT hGetSwapChainDescriptorResult = m_pSwapChain->GetDesc
	(
		&swapChainDesc
	);

	if ( FAILED( hGetSwapChainDescriptorResult ) )
	{
		WriteToLog
		(
			"GetDesc on swap chain failed with error 0x%x.",
			hGetSwapChainDescriptorResult
		);

		return test_result;
	};

	if ( m_pSwapChain == NULL )
	{
		WriteToLog
		(
			"CPresentWinConvertStretch::ExecuteTestCase:  Swap chain is not initialized."
		);

		return test_result;
	};

	if ( !UpdateOutputWindowInformation(m_pSwapChain, deviceWindowInfo, GetRotation()) )
	{
		WriteToLog
		(
			"UpdateOutputWindowInformation failed."
		);

		return test_result;
	}

	HRESULT hresult;
	if ( !SetSyncTransform(*m_pSwapChain, rectDeviceWindow) )
	{
		return RESULT_FAIL;
	}


	//
	// Create our textures.
	//

	UINT width = rectDeviceWindow.right - rectDeviceWindow.left;

	if( width <= 0)
	{
		width = 1;
	}

	UINT height = rectDeviceWindow.bottom - rectDeviceWindow.top;
	if( height <= 0)
	{
		height = 1;
	}


	HRESULT hrCreateTexturesResult = CreateTextures
	(
		swapChainDesc.BufferDesc.Format,
		width,
		height,
		uiDeviceOutputWidth,
		uiDeviceOutputHeight,
		texturesToLoad
	);

	if ( FAILED( hrCreateTexturesResult ) )
	{
		WriteToLog
		(
			"CreateTextures failed."
		);

		if( m_bLargeMemoryUsage && hrCreateTexturesResult == E_OUTOFMEMORY )
		{
			test_result = RESULT_SKIP;
			WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Resource creation failure expected. Skipping test case. " );
		}
		else if( IsDeviceLost(hrCreateTexturesResult) )
		{
			if(m_bExpectDeviceLoss)
			{
				bDeviceLost = true;
				goto DeviceLost;
			}
		}

		return test_result;
	};

	DWORD Stop = GetTickCount() + m_dwPresentTime;

	do
	{
		HRESULT hr = TransitionBeforePresent();
		if(FAILED(hr))
		{
			if( IsDeviceLost(hr) )
			{
				if(m_bExpectDeviceLoss)
				{
					bDeviceLost = true;
					goto DeviceLost;
				}
			}


			WriteToLog
			(
				_T("TransitionBeforePresent failed. hr=0x%x"),
				hr
			);

			return test_result;
		}

		if ( m_ModeInformation.uiMultisampleCount > 1 )
		{
			bool bClearResult = TestedSwapChainMS.InitBBViaClear( 0 );
			if ( bClearResult != true )
			{
				WriteToLog
				(
					"Clear on multisampled swap chain back buffer failed."
				);

				return test_result;
			};

			HRESULT hrInitializeBackBufferResult = InitializeBackBuffer
			(
				&TestedSwapChainMS,
				0
			);

			if ( FAILED( hrInitializeBackBufferResult ) )
			{
				WriteToLog
				(
					"InitializeBackBuffer failed."
				);

				if( hrInitializeBackBufferResult == E_OUTOFMEMORY && m_bLargeMemoryUsage )
				{
					test_result = RESULT_SKIP;
					WriteToLog( "ExecuteTestCase: Test case using large amount of memory. InitializeBackBuffer failure expected. Skipping test case. " );
				}
				else if( IsDeviceLost(hrInitializeBackBufferResult) )
				{
					if(m_bExpectDeviceLoss)
					{
						bDeviceLost = true;
						goto DeviceLost;
					}
				}

				return test_result;
			};

			bool bPerformPresentResult = PerformPresent
			(
				m_pSwapChainMS,
				0,
				0
			);

			if ( bPerformPresentResult != true )
			{
				WriteToLog
				(
					"PerformPresent on multisampled swap chain failed."
				);

				if(m_bExpectDeviceLoss)
				{
					bDeviceLost = true;
					goto DeviceLost;
				}

				return test_result;
			};


			//
			// Give a user watching at the console a chance to look at the image.
			//

			if
			(
				g_Application.GetDebugPause() > 0
			)
			{
				Sleep( g_Application.GetDebugPause() );
			};

			if ( g_Application.IsTerminalServices() == true||
				GetSystemMetrics( SM_REMOTESESSION) != 0)
			{
				//
				// Sleep to allow the present to finish.
				//
				Sleep(100);
			}

			bool bGetDisplaySurfaceDataResult = GetDisplaySurfaceData
			(
				m_pGetDSDSurfaceMS,
				true,
				true
			);

			if ( bGetDisplaySurfaceDataResult != true )
			{
				WriteToLog
				(
					"GetDisplaySurfaceData on multisampled swap chain failed."
				);

				return test_result;
			}

		};

		/*bool bClearResult = TestedSwapChain.InitBBViaClear( 0 );
		if ( bClearResult != true )
		{
			WriteToLog
			(
				"Clear on swap chain back buffer failed."
			);

			goto Cleanup;
		};

		hr = CreateTextureAndCopyBackBuffer
		(
			&m_pBackBufferCleared,
			dxgifmtBBInitTexture
		);

		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"CreateTextureAndCopyBackBuffer failed with error 0x%x.",
				hr
			);

			goto Cleanup;
		};*/

		bool bPerformPresentResult = false;
		UINT uiCount;
		if( swapChainDesc.SwapEffect != DXGI_SWAP_EFFECT_DISCARD )
		{
			// Render to all of the backbuffers before entering the Present() loop. T
			// This is to test that DXGI_SWAP_EFFECT_SEQUENTIAL* preserves backbuffers
			for
			(
				uiCount = 0;
				uiCount < m_BufferCount;
				++uiCount
			)
			{
				{
					Vector2D<unsigned> RenderDimension;
					const auto pRenderDimension = BBRenderDim(RenderDimension);
					HRESULT hrInitializeBackBufferResult = InitializeBackBuffer(&TestedSwapChain, uiCount % texturesToLoad, pRenderDimension);

					if ( FAILED( hrInitializeBackBufferResult ) )
					{
						WriteToLog(_T("InitializeBackBuffer failed."));
						if( hrInitializeBackBufferResult == E_OUTOFMEMORY && m_bLargeMemoryUsage)
						{
							test_result = RESULT_SKIP;
							WriteToLog( "ExecuteTestCase: Test case using large amount of memory. InitializeBackBuffer failure expected. Skipping test case. " );
						}
						else if(IsDeviceLost(hrInitializeBackBufferResult) && m_bExpectDeviceLoss)
						{
							bDeviceLost = true;
							goto DeviceLost;
						}

						return test_result;
					}


					//
					// Grab the Backbuffer to be presented to use as reference image.
					//

					if(uiCount == ( m_NumberOfPresents - 1 ) % m_BufferCount )
					{
						hr = CreateTextureAndCopyBackBuffer(&m_pBackBufferRendered, dxgifmtBBInitTexture, pRenderDimension);
						if ( FAILED( hr ) )
						{
							WriteToLog
							(
								"CreateTextureAndCopyBackBuffer failed with error 0x%x.",
								hr
							);

							if( hr == E_OUTOFMEMORY && m_bLargeMemoryUsage)
							{
								test_result = RESULT_SKIP;
								WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Texture create failure expected. Skipping test case. " );
							}
							else if (IsDeviceLost(hr))
							{
								if(m_bExpectDeviceLoss)
								{
									bDeviceLost = true;
									goto DeviceLost;
								}
							}

							return test_result;
						}
					}
				}

				// Must Present() to rotate to the next backbuffer
				if( FAILED( m_pSwapChain->Present( 0, 0 ) ) )
				{
					WriteToLog
					(
						"Present() on swap chain failed."
					);

					if(m_bExpectDeviceLoss)
					{
						bDeviceLost = true;
						goto DeviceLost;
					}

					return test_result;
				}
			}
		}

		for
		(
			uiCount = 0;
			uiCount < m_NumberOfPresents;
			++uiCount
		)
		{
			if( swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_DISCARD )
			{
				HRESULT hrInitializeBackBufferResult = InitializeBackBuffer
				(
					&TestedSwapChain,
					uiCount % texturesToLoad
				);

				if ( FAILED( hrInitializeBackBufferResult ) )
				{
					WriteToLog
					(
						"InitializeBackBuffer failed."
					);

					if( hrInitializeBackBufferResult == E_OUTOFMEMORY && m_bLargeMemoryUsage)
					{
						test_result = RESULT_SKIP;
						WriteToLog( "ExecuteTestCase: Test case using large amount of memory. InitializeBackBuffer failure expected. Skipping test case. " );
					}
					else if (IsDeviceLost(hrInitializeBackBufferResult))
					{
						if(m_bExpectDeviceLoss)
						{
							bDeviceLost = true;
							goto DeviceLost;
						}
					}

					return test_result;
				};


				//
				// Grab the Backbuffer before the last present.
				//

				if(uiCount == (m_NumberOfPresents - 1) )
				{
					hr = CreateTextureAndCopyBackBuffer
					(
						&m_pBackBufferRendered,
						dxgifmtBBInitTexture
					);

					if ( FAILED( hr ) )
					{
						WriteToLog
						(
							"CreateTextureAndCopyBackBuffer failed with error 0x%x.",
							hr
						);

						if( hr == E_OUTOFMEMORY && m_bLargeMemoryUsage)
						{
							test_result = RESULT_SKIP;
							WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Texture create failure expected. Skipping test case. " );
						}
						else if (IsDeviceLost(hr))
						{
							if(m_bExpectDeviceLoss)
							{
								bDeviceLost = true;
								goto DeviceLost;
							}
						}

						return test_result;
					};
				}
			}


			bPerformPresentResult = PerformPresent
			(
				m_pSwapChain,
				0,
				0
			);

			if ( bPerformPresentResult != true )
			{
				WriteToLog
				(
					"PerformPresent on swap chain failed."
				);

				if(m_bExpectDeviceLoss)
				{
					bDeviceLost = true;
					goto DeviceLost;
				}

				return test_result;
			};


			//
			// If we're testing Scrolling and Dirty Rects we need to perform a
			// second Present on top of the current one.
			//

			if (m_bUseDirtyRects || bPerformScrollTest)
			{
				DXGI_SWAP_CHAIN_DESC scDesc;
				IDXGISwapChain* pSwapchain = NULL;
				TestedSwapChain.GetSwapChainPtr(&pSwapchain);
				pSwapchain->GetDesc(&scDesc);
				pSwapchain->Release();

				RGNDATA* pDirtyData = CreateDualEllipticRegion(0, 0, scDesc.BufferDesc.Width, scDesc.BufferDesc.Height);

				// Need to initialize a second back buffer to represent on top of
				// the current contents with dirty rect.
				HRESULT hrInitializeBackBufferResult = InitializeBackBufferWithDirtyRects
				(
					&PrimaryAdapterDevice,
					&TestedSwapChain,
					(uiCount+1) % texturesToLoad,
					pDirtyData
				);

				free(pDirtyData);

				if ( FAILED( hrInitializeBackBufferResult ) )
				{
					WriteToLog
					(
						"InitializeBackBuffer for dirtyrects/scroll present failed."
					);

					if( hrInitializeBackBufferResult == E_OUTOFMEMORY && m_bLargeMemoryUsage)
					{
						test_result = RESULT_SKIP;
						WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Texture creation failure expected. Skipping test case. " );
					}
					else if (IsDeviceLost(hrInitializeBackBufferResult))
					{
						if(m_bExpectDeviceLoss)
						{
							bDeviceLost = true;
							goto DeviceLost;
						}
					}

					return test_result;
				};

				bPerformPresentResult = PerformPresentScrollAndDirtyRects
				(
					m_pSwapChain,
					m_bUseDirtyRects,
					bPerformScrollTest, // no scrolling for a moment
					&m_ScrollInfo,
					0,
					0
				);

				if ( bPerformPresentResult != true )
				{
					WriteToLog
					(
						"PerformPresentScrollAndDirtyRects on swap chain failed."
					);

					if(m_bExpectDeviceLoss)
					{
						bDeviceLost = true;
						goto DeviceLost;
					}

					return test_result;
				};


				//
				// Update reference image with scroll/dirtyrect imagery.  This simulates a scroll dirty rect
				// present.
				//
				//      m_pBackBufferRendered is our current reference image minus dirtyrect/scrolling changes.
				//      pBBInitTextureSys[uiCount % texturesToLoad] is the source image for dirty rect changes.
				//
				//      Based on the format of the swapchain backbuffer, we may need to disable gamma correction in our simulation.

				ApplyScrollAndDirtyRectsToReferenceImage( m_pBackBufferRendered, pBBInitTextureSys[(uiCount+1) % texturesToLoad], m_bUseDirtyRects, bPerformScrollTest, &m_ScrollInfo, IsSRGB(m_ModeInformation.ModeDesc.Format));
			}// End of scroll/dirty rect present


			// Give a user watching at the console a chance to look at the image.
			//

			if
			(
				g_Application.GetDebugPause() > 0
			)
			{
				Sleep( g_Application.GetDebugPause() );
			};

			if(uiCount == 0)
			{
				hr = TransitionAfterPresent();
				if(FAILED(hr))
				{
					if( IsDeviceLost(hr) )
					{
						if(m_bExpectDeviceLoss)
						{
							bDeviceLost = true;
							goto DeviceLost;
						}
					}

					WriteToLog
					(
						_T("TransitionAfterPresent failed. hr=0x%x"),
						hr
					);

					return test_result;
				}
			}
		}


		//
		// This is to test DXGI_PRESENT_DO_NOT_SEQUENCE and DXGI_PRESENT_TEST the following present should have no effect
		// on what is displayed
		//

		if(m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_TEST || m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_DO_NOT_SEQUENCE)
		{
			HRESULT hrInitializeBackBufferResult = InitializeBackBuffer
			(
				&TestedSwapChain,
				uiCount % texturesToLoad
			);

			if ( FAILED( hrInitializeBackBufferResult ) )
			{
				WriteToLog
				(
					"InitializeBackBuffer failed."
				);

				if (IsDeviceLost(hrInitializeBackBufferResult))
				{
					if(m_bExpectDeviceLoss)
					{
						bDeviceLost = true;
						goto DeviceLost;
					}
				}

				return test_result;
			};

			bPerformPresentResult = PerformPresent
			(
				m_pSwapChain,
				0,
				m_PresentFlag.dxgiPresentFlag
			);

			if ( bPerformPresentResult != true )
			{
				WriteToLog
				(
					"PerformPresent on swap chain failed."
				);

				if(m_bExpectDeviceLoss)
				{
					bDeviceLost = true;
					goto DeviceLost;
				}

				return test_result;
			};


			//
			// Give a user watching at the console a chance to look at the image.
			//

			if
			(
				g_Application.GetDebugPause() > 0
			)
			{
				Sleep( g_Application.GetDebugPause() );
			};
		}

		hresult = UpdateReferenceTexture();
		if ( FAILED(hresult) )
		{
			WriteToLog
			(
				_T("UpdateReferenceTexture failed. hr = =0x%x"), hresult
			);

			if (IsDeviceLost(hresult))
			{
				if(m_bExpectDeviceLoss)
				{
					bDeviceLost = true;
					goto DeviceLost;
				}
			}

			return test_result;
		};

		DWORD dwEnd = GetTickCount() + MAX_PRESENT_TIMEOUT;
		float fNonConformanceFraction = 0.f;
		UINT loopCount = 0;
		bool bTimedOut = false;

		{ // Extra scope to get around initialization skipped by goto error.
			CHandle<ID3DTexture2D> pReferenceImage = m_pBackBufferRendered;
			{
				HRESULT hr = AdjustForStretchAndRotation(GetStretchFactor(), GetRotation(), pReferenceImage);
				if (FAILED(hr))
				{
					return RESULT_FAIL;
				}
			}

			do
			{
				bool bGetDisplaySurfaceDataResult = bGetDisplaySurfaceDataResult = GetDisplaySurfaceData
				(
					m_pGetDSDSurface,
					false,
					true
				);

				if ( bGetDisplaySurfaceDataResult != true )
				{
					WriteToLog
					(
						"GetDisplaySurfaceData on swap chain failed."
					);

					if(m_bExpectDeviceLoss)
					{
						bDeviceLost = true;
						goto DeviceLost;
					}

					return test_result;
				};

				bool bEvaluateImageResult;
				bTimedOut = GetTickCount() > dwEnd;
				{
					bEvaluateImageResult = EvaluatePresentedImage
					(
						m_pGetDSDTexture,
						m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : pReferenceImage,
						&fNonConformanceFraction,
						( loopCount == 0 || bTimedOut ),
						&hresult,
						GetOffset(), GetRotation()
					);
				}

				if ( bEvaluateImageResult != true )
				{
					WriteToLog
					(
						"EvaluatePresentedImage failed."
					);

					if (IsDeviceLost(hresult))
					{
						if(m_bExpectDeviceLoss)
						{
							bDeviceLost = true;
							goto DeviceLost;
						}
					}

					return test_result;
				};

				loopCount++;

			} while( (fNonConformanceFraction > m_fMaxNonConformanceFractionWin) && !bTimedOut );

			{
				const auto LastTestConformance = 1.f - fNonConformanceFraction;
				g_Application.SetLastTestConformance( LastTestConformance );
				WriteToLog(_T("Conformance: %3.3f%%, Expected: %3.3f%%"), 100.f * ( LastTestConformance ), 100.f * ( 1.f - m_fMaxNonConformanceFractionWin ));
			}

			if ( fNonConformanceFraction > m_fMaxNonConformanceFractionWin )
			{
				WriteToLog
				(
					"FAIL:  Actual conformance of %3.3f%% does not meet expected threshold of %3.3f%%. Retry Count = %u",
					100.f * ( 1.f - fNonConformanceFraction ),
					100.f * ( 1.f - m_fMaxNonConformanceFractionWin ),
					loopCount - 1	// always >= 1 if we get here ( EvaluatePresentedImage() succeeds at least once )
				);

				test_result = RESULT_FAIL;


				//
				// Save the result as a DDS file.
				//

				if ( g_Application.IsSaveDDS() )
				{
					//
					// Save the test image.
					//

					TCHAR DumpFileName[ MAX_PATH ];
					StringCchPrintf
					(
						DumpFileName,
						ARRAYSIZE( DumpFileName ),
						m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Test.dds" ) : _T( "DXGIPresent_%s_%u_Ref.dds" ),
						GetPath().ToString().c_str(),
						m_iCurrentTestCase
					);

					HRESULT hSaveFileResult = D3DX10SaveTextureToFile
					(
						(ID3DResource *) m_pGetDSDTexture,
						D3DX10_IFF_DDS,
						DumpFileName
					);

					if ( FAILED( hSaveFileResult ) )
					{
						WriteToLog
						(
							"D3DX10SaveTextureToFile saving %s image to DDS file failed with error 0x%x.",
							m_ModeInformation.uiMultisampleCount <= 1 ? "test" : "reference",
							hSaveFileResult
						);

						if (IsDeviceLost(hSaveFileResult))
						{
							if(m_bExpectDeviceLoss)
							{
								bDeviceLost = true;
								goto DeviceLost;
							}
						}
					}
					else
					{
						WriteToLog
						(
							_T( "Saved %s image to %s." ),
							m_ModeInformation.uiMultisampleCount <= 1 ? "test" : "reference",
							DumpFileName
						);
					};


					//
					// Save the reference image.
					//

					StringCchPrintf
					(
						DumpFileName,
						ARRAYSIZE( DumpFileName ),
						m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Ref.dds" ) : _T( "DXGIPresent_%s_%u_Test.dds" ),
						GetPath().ToString().c_str(),
						m_iCurrentTestCase
					);

					hSaveFileResult = D3DX10SaveTextureToFile
					(
						(ID3DResource *) ( m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : pReferenceImage ),
						D3DX10_IFF_DDS,
						DumpFileName
					);

					if ( FAILED( hSaveFileResult ) )
					{
						WriteToLog
						(
							"D3DX10SaveTextureToFile saving %s image to DDS file failed with error 0x%x.",
							m_ModeInformation.uiMultisampleCount <= 1 ? "reference" : "test",
							hSaveFileResult
						);

						if (IsDeviceLost(hSaveFileResult))
						{
							if(m_bExpectDeviceLoss)
							{
								bDeviceLost = true;
								goto DeviceLost;
							}
						}
					}
					else
					{
						WriteToLog
						(
							_T( "Saved %s image to %s." ),
							m_ModeInformation.uiMultisampleCount <= 1 ? "reference" : "test",
							DumpFileName
						);
					};
				}


				//
				// Save the result as a bitmap.
				//

				if ( g_Application.IsSaveBMP() )
				{
					//
					// Save the test image.
					//

					TCHAR DumpFileName[ MAX_PATH ];
					StringCchPrintf
					(
						DumpFileName,
						ARRAYSIZE( DumpFileName ),
						m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Test.bmp" ) : _T( "DXGIPresent_%s_%u_Ref.bmp" ),
						GetPath().ToString().c_str(),
						m_iCurrentTestCase
					);

					HRESULT hSaveFileResult = D3DX10SaveTextureToFile
					(
						(ID3DResource *) m_pGetDSDTexture,
						D3DX10_IFF_BMP,
						DumpFileName
					);

					if ( FAILED( hSaveFileResult ) )
					{
						WriteToLog
						(
							"D3DX10SaveTextureToFile saving %s image to bitmap failed with error 0x%x. Try to save as DDS",
							m_ModeInformation.uiMultisampleCount <= 1 ? "test" : "reference",
							hSaveFileResult
						);

						if (IsDeviceLost(hSaveFileResult))
						{
							if(m_bExpectDeviceLoss)
							{
								bDeviceLost = true;
								goto DeviceLost;
							}
						}


						//
						// Added this fallback because D3DX10_IFF_BMP is known to fail with DXGI_FORMAT_R8G8B8A8_UNORM_SRGB.
						//

						StringCchPrintf
						(
							DumpFileName,
							ARRAYSIZE( DumpFileName ),
							m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Test.dds" ) : _T( "DXGIPresent_%s_%u_Ref.dds" ),
							GetPath().ToString().c_str(),
							m_iCurrentTestCase
						);

						hSaveFileResult = D3DX10SaveTextureToFile
						(
							(ID3DResource *) m_pGetDSDTexture,
							D3DX10_IFF_DDS,
							DumpFileName
						);

						if ( FAILED( hSaveFileResult ) )
						{
							WriteToLog
							(
								"D3DX10SaveTextureToFile saving %s image to DDS also failed with error 0x%x. Nothing saved",
								m_ModeInformation.uiMultisampleCount <= 1 ? "test" : "reference",
								hSaveFileResult
							);

							if (IsDeviceLost(hSaveFileResult))
							{
								if(m_bExpectDeviceLoss)
								{
									bDeviceLost = true;
									goto DeviceLost;
								}
							}
						}
					}

					if ( SUCCEEDED( hSaveFileResult ) )
					{
						WriteToLog
						(
							_T( "Saved %s image to %s." ),
							m_ModeInformation.uiMultisampleCount <= 1 ? "test" : "reference",
							DumpFileName
						);
					};


					//
					// Save the reference image.
					//

					StringCchPrintf
					(
						DumpFileName,
						ARRAYSIZE( DumpFileName ),
						m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Ref.bmp" ) : _T( "DXGIPresent_%s_%u_Test.bmp" ),
						GetPath().ToString().c_str(),
						m_iCurrentTestCase
					);

					hSaveFileResult = D3DX10SaveTextureToFile
					(
						(ID3DResource *) ( m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : pReferenceImage ),
						D3DX10_IFF_BMP,
						DumpFileName
					);

					if ( FAILED( hSaveFileResult ) )
					{
						WriteToLog
						(
							"D3DX10SaveTextureToFile saving %s image to bitmap failed with error 0x%x. Try to save as DDS",
							m_ModeInformation.uiMultisampleCount <= 1 ? "reference" : "test",
							hSaveFileResult
						);

						if (IsDeviceLost(hSaveFileResult))
						{
							if(m_bExpectDeviceLoss)
							{
								bDeviceLost = true;
								goto DeviceLost;
							}
						}


						//
						// Added this fallback because D3DX10_IFF_BMP is known to fail with DXGI_FORMAT_R8G8B8A8_UNORM_SRGB.
						//

						StringCchPrintf
						(
							DumpFileName,
							ARRAYSIZE( DumpFileName ),
							m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Ref.dds" ) : _T( "DXGIPresent_%s_%u_Test.dds" ),
							GetPath().ToString().c_str(),
							m_iCurrentTestCase
						);

						hSaveFileResult = D3DX10SaveTextureToFile
						(
							(ID3DResource *) ( m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : m_pBackBufferRendered ),
							D3DX10_IFF_DDS,
							DumpFileName
						);

						if ( FAILED( hSaveFileResult ) )
						{
							WriteToLog
							(
								"D3DX10SaveTextureToFile saving %s image to DDS also failed with error 0x%x. Nothing saved",
								m_ModeInformation.uiMultisampleCount <= 1 ? "reference" : "test",
								hSaveFileResult
							);

							if (IsDeviceLost(hSaveFileResult))
							{
								if(m_bExpectDeviceLoss)
								{
									bDeviceLost = true;
									goto DeviceLost;
								}
							}
						}
					}

					if ( SUCCEEDED( hSaveFileResult ) )
					{
						WriteToLog
						(
							_T( "Saved %s image to %s." ),
							m_ModeInformation.uiMultisampleCount <= 1 ? "reference" : "test",
							DumpFileName
						);
					};
				}
			}
			else
			{
				test_result = RESULT_PASS;
			}
		}

DeviceLost:
		if(bDeviceLost)
		{
			if(m_bExpectDeviceLoss)
			{
				// We expect to get device lost either due to adapter being stopped or
				// a TDR. Introduce a small delay here so that adapter has a chance to recover
				Sleep(5000);
			}

			test_result = ReCreateDeviceAndSwapChain();

			if(test_result != RESULT_PASS)
				break;

			bDeviceLost = false;
		}
	} while(!m_bStopNow && (GetTickCount() < Stop));

	return test_result;
}


/////////////////////////////////////////////////////////////////
//
// CPresentWinConvertStretchMS implementation
//
/////////////////////////////////////////////////////////////////

CPresentWinConvertStretchMS::CPresentWinConvertStretchMS()
{
	m_bStretchBB = false;
	m_fMaxNonConformanceFractionWin = 0.2f;

	PrimaryAdapterDevice.SetAlternateRenderPath( true );
};

HRESULT CPresentWinConvertStretchMS::ModeListTransform(
	D3D_DEVICE_POINTER devicePointer,
	__in std::vector< ModeInformation >* pModes,
	__out std::vector< ModeInformation >* pTransformedModes)
{
	HRESULT hr = GetMultisampleQualityLevels(
		devicePointer,
		pModes,
		pTransformedModes,
		D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT,
		g_Application.IsCapMultiSampleQuality());

	if(SUCCEEDED(hr) && g_Application.IsMinMultiSampleSet())
	{
		//
		// Only test the highest and lowest MultisampleCounts with the highest and lowest MultisampleQuality
		//

		GetHighLowMidMultisampleQuality(pTransformedModes);
		GetHighLowMidMultisampleCount(pTransformedModes);
	}

	if(pTransformedModes->size() == 0)
	{
		WriteToLog(_T("CPresentWinConvertStretchMS::ModeListTransform returned 0 modes.  Is Multisampling supported?"));
	}

	return hr;
}


//****************************************************************************
// CPresentWinConvertStretchMS::InitTestParameters
//
// Overloaded version of the CPresentWinConvertStretch base class InitTestParameters.
//****************************************************************************
void CPresentWinConvertStretchMS::InitTestParametersImpl()
{
	//
	// SwapChain Back Buffer Dimension test factors
	//
	RFactor bbDimensions = AddParameter
	(
		_T("BBDimensions"),
		&m_iBBDimIndex,
		RangeValueSet
		(
			0,
			m_bStretchBB ? (int)(uiNoOfWindowRelativeBBDims - 1) : 2,
			1
		)
	);


	//
	// Presentation Flag test factors.  Add just the zero case. (No flags)
	//

	CUserValueSet< DXGIPresentFlagRecord > *pPresentFlags = new CUserValueSet< DXGIPresentFlagRecord >();
	pPresentFlags->AddValue(DXGIPresentFlags[0]);
	RFactor presentFlags = AddParameter< DXGIPresentFlagRecord >( _T("PresentFlags") , &m_PresentFlag, pPresentFlags);


	//
	// SwapEffect test factors - Add only blt model swap effects.  Flip Model is not supported with multisampling.
	//

	CUserValueSet< DXGISwapEffectRecord > *pSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
	for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
	{
		// Add everything but Flip Model Swap Effect.
		if (!IsFlipModel(DXGISwapEffects[i].dxgiSwapEffect))
			pSwapEffect->AddValue(DXGISwapEffects[i]);
	}
	RFactor swapEffect = AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);


	//
	// Test only regular Windowed Presents. Flip Model is not supported for MS, and DCOMP requires FlipModel.
	//

	CUserValueSet< bool > *pDCOMP = new CUserValueSet< bool >();
	pDCOMP->AddValue(false);
	RFactor WindowlessFactors = AddParameter< bool >( _T("UseDCOMP") , &m_tp_bWindowless, pDCOMP);


	//
	// Establish DWM on/off test factors
	//

	CUserValueSet< bool > *pDwmCompositionOn = new CUserValueSet< bool >();
	if ( g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION) != 0)
	{
		//
		// Dont add the toggle for DWM on and off in the remote case.
		//
	}
	else
	{
		pDwmCompositionOn->AddValue( false );
	}

	// Invlid dwm factor when in the remote case
	RFactor dwmCompositionOff_32bpp = AddParameter< bool >( _T("DWM"), &m_bDWM, pDwmCompositionOn);
	// Always valid dwm factors
	RFactor v_dwmCompositionOn = AddParameterValue< bool >( _T("DWM"), true);
	RFactor v_dwmCompositionOff = AddParameterValue< bool >( _T("DWM"), false);

	// Separate out Bpp32_DwmOn and DwmOff when adding test parameters together so that
	// the order of test cases are preserved (
	RFactor Bpp16_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOffBpp * v_dwmCompositionOff * WindowlessFactors * swapEffect *  bbDimensions;
	RFactor Bpp32_DwmOn = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * v_dwmCompositionOn * WindowlessFactors * swapEffect *  bbDimensions;
	RFactor Bpp32_DwmOff = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * v_dwmCompositionOff * WindowlessFactors * swapEffect *  bbDimensions;

	// Only test dwm off cases on Win7+8IP and below
	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);
	if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2) )
	{
		// Check which set of enumerated mode set parameters are valid
		if( Bpp16_DwmOff.Valid() && Bpp32_DwmOn.Valid())
			SetRootTestFactor( Bpp32_DwmOn + Bpp32_DwmOff + Bpp16_DwmOff );
		else if( !Bpp16_DwmOff.Valid() && Bpp32_DwmOn.Valid())
			SetRootTestFactor(Bpp32_DwmOn + Bpp32_DwmOff);
		else if( Bpp16_DwmOff.Valid() && !Bpp32_DwmOn.Valid())
			SetRootTestFactor( Bpp16_DwmOff );
		else
		{
			SetRootTestFactor( m_FeatureLevelFactor );
			m_ParameterError = static_cast<UINT>(-1);
		}
		AddPriorityPatternFilter(FilterEqual< bool >(_T("DWM"), false ), WeightFactorVerySmall);
	}
	else
	{
		if( Bpp32_DwmOn.Valid())
			SetRootTestFactor( Bpp32_DwmOn );
	}

	AddPriorityPatternFilter(FilterLess< int >(_T("BBDimensions"), 3 ), WeightFactorLarge);
	AddPriorityPatternFilter(( FilterEqual< DXGISwapEffectRecord >(_T("SwapEffect"), DXGISwapEffects[1] ) && FilterEqual< int >(_T("BBDimensions"), 0 )), WeightFactorSmall);
	// DXGI_SWAP_EFFECT_SEQUENTIAL covered in DXGISwap but does not test stretching

	DeclarePriorityLevelByPriorityValue(1, 5.0f);
	DeclarePriorityLevelByPriorityValue(2, 2.0f);
}


TEST_RESULT CPresentWinConvertStretchMS::ExecuteTestCase()
{
	TEST_RESULT retVal = RESULT_PASS;

	// MS is not required for feature levels <= 10. Some devices might not support it.
	if(!m_ModeInformation.isInitialized)
	{
		WriteToLog( _T("CPresentWinConvertStretchMS::ExecuteTestCase Skipping test because no multisample modes were found.") );
		return RESULT_SKIP;
	}

	retVal = CPresentWinConvertStretch::ExecuteTestCase();

	return retVal;
}


TEMPLATE_INVARIANT
(
	HRESULT,
	CPresentWinConvertStretch,
	AdapterAndOutputFromHMon
	(
		HMONITOR monitor,
		__out UINT* adapterID,
		__out UINT* outputID
	)
)
{
	HRESULT hr = S_OK;
	DXGI_FACTORY_POINTER pFactory;

	hr = g_Application.CreateDXGIFactory
	(
		&pFactory
	);

	if(SUCCEEDED(hr))
	{
		IDXGIAdapter * pAdapter = NULL;
		BOOL found =FALSE;
		for( UINT uiAdapterOrdinal = 0; uiAdapterOrdinal < MAX_ADAPTERS; ++uiAdapterOrdinal)
		{
			hr = pFactory.pFactory-> EnumAdapters (uiAdapterOrdinal, &pAdapter);
			if(SUCCEEDED(hr))
			{
				for( UINT uiOutputOrdinal = 0; uiOutputOrdinal < MAX_OUTPUTS; ++uiOutputOrdinal)
				{
					IDXGIOutput* pOutput = NULL;
					hr = pAdapter->EnumOutputs(uiOutputOrdinal, &pOutput);

					DXGI_OUTPUT_DESC desc;
					if(SUCCEEDED(hr))
					{
						hr = pOutput->GetDesc(&desc);
						pOutput->Release();
					}

					if(SUCCEEDED(hr))
					{
						if(desc.Monitor == monitor)
						{
							found = true;
							*adapterID = uiAdapterOrdinal;
							*outputID = uiOutputOrdinal;

						}
					}

					if(hr == DXGI_ERROR_NOT_FOUND)
					{
						continue;
					}
					else if(FAILED(hr) || found)
					{
						break;
					}
				}
			}

			if(pAdapter)
			{
				pAdapter->Release();
			}

			if(hr == DXGI_ERROR_NOT_FOUND)
			{
				continue;
			}
			else if(FAILED(hr) || found)
			{
				break;
			}
		}
	}

	if(pFactory.pFactory)
	{
		pFactory.pFactory->Release();
	}

	return hr;
}


///////////////////////////////////////////////////////////////////////////////
//
//  Convert from a GDI_MONITOR_MODE list to a ModeInformation list
//  bascially for windowed mode tests.  Creates a list of backbuffer formats
//  to test for each mode and each output.
//
///////////////////////////////////////////////////////////////////////////////
TEMPLATE_INVARIANT
(
	void,
	CPresentWinConvertStretch,
	Convert
	(
		__in std::vector<GDI_MONITOR_MODE>* pMonitorModePairs,
		__out std::vector< ModeInformation >* pMultisampleIndex
	)
)
{
	for(UINT i = 0; i < pMonitorModePairs->size(); i++)
	{
		UINT adapterID = 0;
		UINT outputID = 0;
		HRESULT hresult = AdapterAndOutputFromHMon
		(
			(*pMonitorModePairs)[i].hmonitor,
			&adapterID,
			&outputID
		);

		if ( FAILED(hresult) )
		{
			WriteToLog
			(
				"CPresentWinConvertStretch::Convert:  AdapterAndOutputFromHMon failed. hr = 0x%x", hresult
			);

			return;
		};

		for
		(
			UINT uFormat = 0;
			uFormat < clsDXGIRenderTargetFormats::uiNoOfBBFormats;
			++uFormat
		)
		{
			ModeInformation temp = { 0 };
			temp.ModeDesc.Width = (*pMonitorModePairs)[i].devmode.dmPelsWidth;
			temp.ModeDesc.Height = (*pMonitorModePairs)[i].devmode.dmPelsHeight;
			temp.ModeDesc.RefreshRate.Numerator = (*pMonitorModePairs)[i].devmode.dmDisplayFrequency;
			temp.ModeDesc.RefreshRate.Denominator = 1;
			temp.ModeDesc.Format = clsDXGIRenderTargetFormats::DXGI_Back_Buffer_Formats[uFormat];
			temp.uiDXGIAdapterOrdinal = adapterID;
			temp.uiDXGIOutputOrdinal = outputID;
			temp.hmonitor = (*pMonitorModePairs)[i].hmonitor;
			temp.devmode = (*pMonitorModePairs)[i].devmode;
			temp.uiMultisampleCount = 1;
			temp.uiMultisampleQuality = 0;
			temp.isInitialized = TRUE;

			//
			// WARNING:  The equality operator for ModeInformation does not do all fields in devmode.
			//
			if ( pMultisampleIndex->end() == find(
				pMultisampleIndex->begin(),
				pMultisampleIndex->end(),
				temp))
			{
				pMultisampleIndex->push_back( temp );
			}
		}
	}
}


///////////////////////////////////////////////////////////////////////////////
//
//
///////////////////////////////////////////////////////////////////////////////
TEMPLATE_INVARIANT
(
	void,
	CPresentWinConvertStretch,
	DisplayModeForRemote()
)
{
	if
	(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION) != 0 ||
		g_Application.GetTestMode() == WGFTEST_MODE_STRESS
	)
	{
		//
		// Just use modes from the current resolution
		//

		int iTotalModes = 0;
		if ( ( iTotalModes = clsEnumModesGDI::GetAllMontitorModes() ) > 0)
		{
			DebugOut.Write("Total modes: %d\n", iTotalModes);
			m_UseCurrentDesktopMode = TRUE;

		}
		else
		{
			WriteToLog(_T("CPresentWinConvertStretch::DisplayModeForRemote GetAllMontitorModes Found 0 modes."));
		};
	}
	else
	{
		int iTotalModes = 0;
		if ((iTotalModes = clsEnumModesGDI::FindClosestMatchRequireBpp(m_DesktopModes, m_numberOfModes)) > 0)
		{
			DebugOut.Write("Total modes: %d\n", iTotalModes);
		}
		else
		{
			WriteToLog(_T("CPresentWinConvertStretch::DisplayModeForRemote FindClosestMatchRequireBpp Found 0 modes."));
			for(UINT i = 0; i < m_numberOfModes; i++)
			{
				WriteToLog(_T("Attempted to match %dx%d bpp %d"), m_DesktopModes[i].uiWidth, m_DesktopModes[i].uiHeight, m_DesktopModes[i].uiBPP);
			}
		};
	}
}


///////////////////////////////////////////////////////////////////////////////
//
// CreateDualEllipticRegion
//
// Helper function to create the dual elliptical clipping region used by this
// test.  Bounding rect is defined by left, top, right, and
// bottom parameters.   Region created looks something like the following:
//    ------------------
//    |   ---          |
//    |  /   \         |
//    |  \   /         |
//    |   ---     ---  |
//    |          /   \ |
//    |          \   / |
//    |           ---  |
//    ------------------
//
// Caller must call free() on the RGNDATA structure when finished with it.
//
///////////////////////////////////////////////////////////////////////////////
TEMPLATE_INVARIANT
(
	RGNDATA*,
	CPresentWinConvertStretch,
	CreateDualEllipticRegion
	(
		int nLeft,
		int nTop,
		int nRight,
		int nBottom
	)
)
{
	HRGN hRgn1 = NULL;
	HRGN hRgnCombined = NULL;
	int nSize = 0;
	RGNDATA *prgnData = NULL;
	int nRetVal;

	int nHalfX = (nRight - nLeft) / 2 ;
	int nHalfY = (nBottom - nTop) / 2;

	hRgn1 = CreateEllipticRgn( nLeft, nTop, nHalfX, nHalfY);
	if (!hRgn1)
	{
		WriteToLog("CreateDualEllipticRegion: CreateEllipticRgn() failed! ***");
		goto CleanUp;
	}

	hRgnCombined = CreateEllipticRgn( nHalfX, nHalfY, nRight, nBottom);
	if (!hRgnCombined)
	{
		WriteToLog("CreateDualEllipticRegion: CreateEllipticRgn() failed! ***");
		goto CleanUp;
	}

	nRetVal = CombineRgn(hRgnCombined, hRgn1, hRgnCombined, RGN_OR);
	if (nRetVal != COMPLEXREGION)
	{
		WriteToLog("CreateDualEllipticRegion: CombineRgn() failed! ***");
		goto CleanUp;
	}

	nSize = GetRegionData(hRgnCombined, 0, NULL); // Get Size of RGNDATA
	if(nSize == 0)
	{
		WriteToLog("CreateDualEllipticRegion: GetRegionData indicates a region size of zero!");
		goto CleanUp;
	}

	prgnData = (RGNDATA *)malloc(nSize);
	if (!prgnData)
	{
		WriteToLog("CreateDualEllipticRegion: Couldn't allocate memory for region data structure!");
		goto CleanUp;
	}

	if (!GetRegionData(hRgnCombined, nSize, prgnData))
	{
		WriteToLog("CreateDualEllipticRegion: GetRegionData failed!");
		free(prgnData);
		prgnData = NULL;
		goto CleanUp;
	}


CleanUp:
	DeleteObject(hRgn1);
	DeleteObject(hRgnCombined);

	return prgnData;
}


///////////////////////////////////////////////////////////////////////////////
// PerformPresentScrollAndDirtyRects
//
// Helper function to perform scrolling and/or dirty rect present.
///////////////////////////////////////////////////////////////////////////////
TEMPLATE_INVARIANT
(
	bool,
	CPresentWinConvertStretch,
	PerformPresentScrollAndDirtyRects
	(
		IDXGISwapChain *pSwapChain,
		BOOL bUseDirtyRects,
		bool bDoScrolling,
		SCROLLINGINFO *pscrollinfo,
		UINT syncInterval,
		UINT flags
	)
)
{
	IDXGISwapChain1 * pswapchain1 = NULL;
	DXGI_SWAP_CHAIN_DESC1 dscd;
	DXGI_PRESENT_PARAMETERS dxgipp;

	HRESULT hresult = E_FAIL;
	RGNDATA *prgndata = NULL;
	bool    bPresentSucceeded = true;
	RECT    rDest;
	POINT   pointDestOffset;

	//
	// QI for a SWAPCHAIN1 interface.  We'll need it to call the newer
	// Present1 interface which supports dirty rects and scrolling.
	//
	hresult = pSwapChain->QueryInterface(IID_IDXGISwapChain1, (void**)&pswapchain1);
	if (FAILED(hresult))
	{
		WriteToLog("PerformPresentScrollAndDirtyRects: Unable to QI for IID_IDXGISwapChain1.   Must be running on an older OS.");
		return FALSE;
	}

	// Get the swapchain width and hight.
	hresult = pswapchain1->GetDesc1(&dscd);

	ZeroMemory(&dxgipp, sizeof(dxgipp));

	if (bUseDirtyRects)
	{
		//
		// Get the swap chain dimensions so we can create an appropriately sized
		// clipping region.
		//

		prgndata = CreateDualEllipticRegion(0, 0, dscd.Width, dscd.Height);
		if (prgndata == NULL)
		{
			WriteToLog("PerformPresentScrollAndDirtyRects: Unable to create elliptic region.  Might be low on memory.");
			return FALSE;
		}

		dxgipp.DirtyRectsCount = prgndata->rdh.nCount;
		dxgipp.pDirtyRects = (RECT *)prgndata->Buffer;
	}

	if (bDoScrolling)
	{
		// Convert scrollinginfo fractions into dest rect and offset.
		rDest.left   = (long) (pscrollinfo->fLeft   * dscd.Width);
		rDest.top    = (long) (pscrollinfo->fTop    * dscd.Height);
		rDest.right  = (long) (pscrollinfo->fRight  * dscd.Width);
		rDest.bottom = (long) (pscrollinfo->fBottom * dscd.Height);

		pointDestOffset.x = (long) (pscrollinfo->fOffX * dscd.Width);
		pointDestOffset.y = (long) (pscrollinfo->fOffY * dscd.Height);

		dxgipp.pScrollRect = &rDest;
		dxgipp.pScrollOffset = &pointDestOffset;

		// Log the actaul values being used for scrolling.   This is useful since
		// the test parameters that get logged show the scrolling info as normalized
		// values from 0 to 1.
		//
		WriteToLog
		(
			"Scrolling Info:  Rect: [%d, %d, %d, %d], Offset: [%d, %d]",
			dxgipp.pScrollRect->left,
			dxgipp.pScrollRect->top,
			dxgipp.pScrollRect->right,
			dxgipp.pScrollRect->bottom,
			dxgipp.pScrollOffset->x,
			dxgipp.pScrollOffset->y
		);

	}

	hresult = pswapchain1->Present1(syncInterval, flags, &dxgipp);
	DebugOut.Write("+++++ Present1 called. hresult=0x%x +++++\n", hresult);
	if( FAILED(hresult) )
	{
		WriteToLog
		(
			"Present1 failed with error 0x%x.",
			hresult
		);

		bPresentSucceeded = false;
	}
	else if ( hresult == DXGI_STATUS_OCCLUDED)
	{
		WriteToLog
		(
			"Present returned DXGI_STATUS_OCCLUDED.  This is unexpected, "
			"and it usually means that a screensaver or the secure desktop"
			"was active."
		);

		CheckForegroundWindow( m_szFocusWindowClassName );

		// Dump DXGI Journal to get clues as to why DXGI_STATUS_OCCLUDED is returned
		if( g_Application.m_bLogDXGIJounrnal )
			g_Application.InitiateJournalDump( true );

		bPresentSucceeded = false;
	}

	if (prgndata) free(prgndata);
	pswapchain1->Release();

	return bPresentSucceeded;
}


///////////////////////////////////////////////////////////////////////////////
// ApplyScrollAndDirtyRectsToReferenceImage
//
// Updates the reference image with dirty rect contents and scrolling.
//
// Assumptions:
//   *pSourceImage and pRefImage are the same size.
//   *pRefImage is a system memory (CPU accessible) staging surface.
//   *pRefImage already contains the previously presented image that dirtyrects
//    should be applied on top of.
//   *pSourceImage is the source texure used to initialize the back buffer
//    that will be presented as dirty rects.
///////////////////////////////////////////////////////////////////////////////
TEMPLATE_INVARIANT
(
	bool,
	CPresentWinConvertStretch,
	ApplyScrollAndDirtyRectsToReferenceImage
	(
		__in ID3DTexture2D * pRefImage,
		__in ID3DTexture2D * pSourceImage,
		__in bool bApplyDirtyRect,
		__in bool bApplyScrolling,
		__in SCROLLINGINFO *pScrollInfo,
		__in bool bDisableSRGBConversion
	)
)
{
	D3D10_TEXTURE2D_DESC descRef;
	RGNDATA *pDirtyRgn = NULL;
	bool bRet = true;
	RECT rDest;
	POINT pointDestOffset;

	assert(pRefImage != NULL);
	assert(pSourceImage != NULL);
	if (bApplyScrolling) assert(pScrollInfo != NULL);

	pRefImage->GetDesc(&descRef);

	if (bApplyScrolling)
	{
		// Convert scrollinginfo fractions into dest rect and offset.
		rDest.left   = (long)(pScrollInfo->fLeft   * descRef.Width);
		rDest.top    = (long)(pScrollInfo->fTop    * descRef.Height);
		rDest.right  = (long)(pScrollInfo->fRight  * descRef.Width);
		rDest.bottom = (long)(pScrollInfo->fBottom * descRef.Height);

		pointDestOffset.x = (long)(pScrollInfo->fOffX * descRef.Width);
		pointDestOffset.y = (long)(pScrollInfo->fOffY * descRef.Height);

		ScrollRect(pRefImage, &rDest, &pointDestOffset);
	}

	if (bApplyDirtyRect)
	{
		// Create the same elliptic clipping region used in the actual Present.
		pDirtyRgn = CreateDualEllipticRegion(0,0, descRef.Width, descRef.Height);
		if (!pDirtyRgn)
		{
			bRet = false;
			goto CleanUp;
		}

		CopyRegion(pSourceImage, pRefImage, pDirtyRgn, bDisableSRGBConversion);
	}

	//
	// If we got here, pRefImage has now been updated to look something like
	// what we presented, and can be used as a reference image for validation.
	//

CleanUp:
	if (pDirtyRgn) free(pDirtyRgn);

	return bRet;
}


///////////////////////////////////////////////////////////////////////////////
// ScrollRect
//
// Used to simulate a scrolling operation on our reference texture.
// The dest RECT and offset parameters mirror the behavior of the
// scrolling parameters on DXGI's Present1.
///////////////////////////////////////////////////////////////////////////////
TEMPLATE_INVARIANT
(
	bool,
	CPresentWinConvertStretch,
	ScrollRect
	(
		__out ID3D10Texture2D * pFinalImage,  // Destination image
		__in const RECT *pr,    // Dest Rect. Can be NULL.
		__in const POINT *pRelocate // source location offset from dest rect.  May be NULL if pr is NULL.
	)
)
{
	//
	// Initialize locals.
	//

	D3DX10_TEXTURE_LOAD_INFO tlinfo;
	D3D10_BOX srcBox;
	D3D10_BOX dstBox;
	HRESULT hr;
	D3D10_TEXTURE2D_DESC desc;
	bool bRet = true;
	ID3D10Texture2D * pTempImage = NULL;

	if (!pr || !pRelocate || !pFinalImage)
	{
		WriteToLog("ScrollRect: Invalid call with one or more NULL parameters.");
	}


	//
	// Create a temporary texture the same as the FinalImage to assist with simulating the scroll operation.
	//
	pFinalImage->GetDesc( &desc);
	hr = m_DevicePointer.pDevice->CreateTexture2D
	(
		&desc,
		NULL,
		&pTempImage
	);

	if (FAILED(hr))
	{
		WriteToLog("ScrollRect: Failed to create temporary texture for simulating the scroll operation.");
		bRet = false;
		goto CleanUp;
	}


	//
	// Copy current contents of FinalImage to TempImage
	//

	ZeroMemory(&tlinfo, sizeof(tlinfo));

	tlinfo.NumMips         = D3DX10_DEFAULT;
	tlinfo.SrcFirstElement = 0;
	tlinfo.DstFirstElement = 0;
	tlinfo.NumElements     = D3DX10_DEFAULT;
	tlinfo.Filter          = D3DX10_FILTER_LINEAR;
	tlinfo.MipFilter       = D3DX10_FILTER_LINEAR;

	hr = D3DX10LoadTextureFromTexture((ID3D10Resource *)pFinalImage, &tlinfo, (ID3D10Resource *)pTempImage);
	if (FAILED(hr))
	{
		WriteToLog(_T("ScrollRect: D3DX10LoadTextureFromTexture failed with error 0x%x while initializing Temp surface."),hr);
		bRet = false;
	}


	//
	// Copy portion of tempImage back to final, relocating it to simulate a
	// scrolling operation.
	//

	dstBox.left   = pr->left;
	dstBox.top    = pr->top;
	dstBox.front  = 0;
	dstBox.right  = pr->right;
	dstBox.bottom = pr->bottom;
	dstBox.back   = 0;

	srcBox.left   = dstBox.left - pRelocate->x;
	srcBox.top    = dstBox.top  - pRelocate->y;
	srcBox.front  = 0;
	srcBox.right  = dstBox.right  - pRelocate->x;
	srcBox.bottom = dstBox.bottom - pRelocate->y;
	srcBox.back   = 0;

	tlinfo.pDstBox = &dstBox;
	tlinfo.pSrcBox = &srcBox;

	hr = D3DX10LoadTextureFromTexture((ID3D10Resource *)pTempImage, &tlinfo, (ID3D10Resource *)pFinalImage);
	if (FAILED(hr))
	{
		WriteToLog(_T("ScrollRect: D3DX10LoadTextureFromTexture failed with error 0x%x copying back from temp image."),hr);
		bRet = false;
	}

CleanUp:
	if (pTempImage) pTempImage->Release();

	return bRet;
}


TEMPLATE_INVARIANT
(
	TEST_RESULT,
	CPresentWinConvertStretch,
	ReCreateDeviceAndSwapChain()
)
{
	//
	// Initialize locals.
	//

	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	UINT width = 0, height = 0;
	UINT texturesToLoad = m_TexturesToLoad;

	WriteToLog(_T("ReCreateDeviceAndSwapChain:Device had been removed. Attempting to re-create device"));

	ReleaseTextures();

	if ( m_pBackBufferRendered != NULL )
	{
		m_pBackBufferRendered->Release();
		m_pBackBufferRendered = NULL;
	}

	if ( m_pBackBufferCleared != NULL)
	{
		m_pBackBufferCleared->Release();
		m_pBackBufferCleared = NULL;
	}

	// Release Factory
	if(m_FactoryPointer.pFactory)
	{
		m_FactoryPointer.pFactory->Release();
		m_FactoryPointer.pFactory = NULL;
	}

	m_DCOMPIsolation.DestroyDCompDeviceAndVisual();

	// CreateDeviceAndSwapchain (it will release existing objects first)
	tr = CreateDeviceAndSwapChain();

	if(tr == RESULT_SKIP)
	{
		OutputDebugString("ReCreateDeviceAndSwapChain:Couldn't re-create device; it might still be unavailable, try some more\n");
		// Device might still not be available, try to re-create for 4 minutes (in case device had been disabled)
		time_t StartTime = time(NULL);
		time_t StopTime = StartTime + (time_t)4*60;

		do
		{
			Sleep(100);
			tr = CreateDeviceAndSwapChain();
		}while((tr == RESULT_SKIP) && (time(NULL) < StopTime));

		WriteToLog("ReCreateDeviceAndSwapChain: Waited for %d seconds for CreateDeviceAndSwapChain to succeed", (UINT)(time(NULL) - StartTime));

		if(tr != RESULT_PASS)
		{
			WriteToLog("ReCreateDeviceAndSwapChain: Failed to re-create device and swapchain");
			OutputDebugString("ReCreateDeviceAndSwapChain: Failed to re-create device\n");
			return tr;
		}

		OutputDebugString("ReCreateDeviceAndSwapChain:successfully recreated device and swapchain\n");
	}
	else if (tr != RESULT_PASS)
		return tr;

	//
	// Initialize our Direct3D 9 device helper object.
	// Causing some failures in stress.  Exculding until
	// time comes to fix
	//
	if((g_Application.GetTestMode() != WGFTEST_MODE_STRESS))
	{
		//
		// Tests that start fullscreen need to recreate the d3d9 screen capture device.
		//

		hr = m_D3D9Device.Initialize(g_Application.IsTerminalServices(), m_StartFullScreen);
		if ( FAILED( hr ) )
		{
			WriteToLog("ReCreateDeviceAndSwapChain: Could not initialize Direct3D 9 device helper object, error 0x%X.", hr);
			tr = RESULT_FAIL;
		};
	}

	if(tr == RESULT_PASS)
	{
		hr = m_pSwapChain->GetDesc(&swapChainDesc);

		if ( FAILED( hr ) )
		{
			WriteToLog("ReCreateDeviceAndSwapChain:GetDesc on swap chain failed with error 0x%x.", hr);
			tr = RESULT_FAIL;
		};

		if(tr == RESULT_PASS)
		{
			WINDOWINFO deviceWindowInfo;
			deviceWindowInfo.cbSize = sizeof( deviceWindowInfo );

			BOOL bGetWindowInfoResult = GetWindowInfo(m_hwndDeviceWindow, &deviceWindowInfo);

			if ( bGetWindowInfoResult != TRUE )
			{
				WriteToLog("ReCreateDeviceAndSwapChain: GetWindowInfo failed hwnd=0x%x.", m_hwndDeviceWindow);
				tr = RESULT_ABORT;
			};

			if(tr == RESULT_PASS)
			{
				if (!UpdateOutputWindowInformation(m_pSwapChain, deviceWindowInfo, GetRotation()) )
				{
					WriteToLog("ReCreateDeviceAndSwapChain:UpdateOutputWindowInformation failed.");
					tr = RESULT_FAIL;
				}
			}
		}
	}

	if(tr == RESULT_PASS)
	{
		width = rectDeviceWindow.right - rectDeviceWindow.left;

		if( width <= 0)
		{
			width = 1;
		}

		height = rectDeviceWindow.bottom - rectDeviceWindow.top;
		if( height <= 0)
		{
			height = 1;
		}

		if(m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_TEST || m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_DO_NOT_SEQUENCE)
		{
			texturesToLoad += 1;
		}

		if(m_bUseDirtyRects)
		{
			texturesToLoad += 1;
		}

		hr = TransitionBeforePresent();
		if(FAILED(hr))
		{
			WriteToLog(_T("ReCreateDeviceAndSwapChain:TransitionBeforePresent failed. hr=0x%x"), hr );
			tr = RESULT_FAIL;
		}
	}


	if(tr == RESULT_PASS)
	{
		//
		// Create our textures.
		//

		hr = CreateTextures
		(
			swapChainDesc.BufferDesc.Format,
			width,
			height,
			uiDeviceOutputWidth,
			uiDeviceOutputHeight,
			texturesToLoad
		);

		if ( FAILED( hr ) )
		{
			WriteToLog("ReCreateDeviceAndSwapChain:CreateTextures failed with error 0x%x.", hr);
			if( m_bLargeMemoryUsage && hr == E_OUTOFMEMORY )
			{
				tr = RESULT_SKIP;
				WriteToLog( "ReCreateDeviceAndSwapChain: Test case using large amount of memory. Resource creation failure expected. Skipping test case. " );
			}
			else
				tr = RESULT_FAIL;
		}
	}

	if(tr != RESULT_PASS)
		WriteToLog(_T("Failed to re-create device"));
	else
		WriteToLog(_T("Successfully re-created device"));

	return tr;
}


TEMPLATE_INVARIANT
(
	bool,
	CPresentWinConvertStretch,
	IsDeviceLost(HRESULT hr)
)
{
	return (hr == DXGI_ERROR_DEVICE_REMOVED || hr == DXGI_ERROR_DEVICE_RESET);
}


TEMPLATE_INVARIANT
(
	const RELATIVE_BB_DIMS&,
	CPresentWinConvertStretch,
	ScaleFactor() const
)
{
	return WindowRelativeBBDims[m_iBBDimIndex];
}


const RECT& CPresentWinConvertStretch_<false>::FocusWindowRect() const
{
	return g_FocusWindowSizes[m_iWindow];
}


const RECT& CPresentWinConvertStretch_<true>::FocusWindowRect() const
{
	return WindowRect;
}


RECT GetClientRect(HWND hWnd){ RECT r; GetClientRect(hWnd, &r); return r; }


Vector2D<unsigned> CPresentWinConvertStretch_<false>::SwapChainDimension() const
{
	return Helper::Dimensions(Helper::Scale(GetClientRect(m_hwndDeviceWindow), Vector2D<float>(ScaleFactor().x(), ScaleFactor().y())));
}


Vector2D<unsigned> CPresentWinConvertStretch_<true>::SwapChainDimension() const
{
	return Helper::Dimensions(Helper::Scale(Helper::RotationAdjust(GetClientRect(m_hwndDeviceWindow), GetRotation()), GetStretchFactor().inverse().abs()));
}


Vector2D<unsigned>* CPresentWinConvertStretch_<false>::BBRenderDim(Vector2D<unsigned>&) const
{
	return nullptr;
}


Vector2D<unsigned>* CPresentWinConvertStretch_<true>::BBRenderDim(Vector2D<unsigned>& v) const
{
	if (UseSourceSize())
	{
		v = Helper::Scale(GetSourceSize(), GetStretchFactor().abs().inverse());
		return &v;
	}

	return nullptr;
}

template class CPresentWinConvertStretch_<true>;
template class CPresentWinConvertStretch_<false>;

#undef TEMPLATE_INVARIANT

#pragma warning(pop)
