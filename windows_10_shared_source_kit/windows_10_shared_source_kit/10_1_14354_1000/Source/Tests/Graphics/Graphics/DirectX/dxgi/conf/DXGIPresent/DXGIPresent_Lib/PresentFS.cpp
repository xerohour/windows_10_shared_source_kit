//////////////////////////////////////////////////////////////////////
// File:  PresentFS.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of DXGI present full-screen swap chain tests.
//
// History:
// ?? ??? ????  AndrewLu  Created.
// 05 Oct 2007  SEdmison  Rewrote, cleaned up, and added feature level
//						support.
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

#include "DXGIValueSets.h"
#include "DXGIScanoutFormats.h"
#include "DXGISwapEffects.h"

#include <StrSafe.h>
#include "winuserp.h"

#pragma warning(disable: 4355)


/////////////////////////////////////////////////////////////////
//
// Constants
//
/////////////////////////////////////////////////////////////////

const char * const szScanLineNames[] =
{
	"DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED",
	"DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE",
	"DXGI_MODE_SCANLINE_ORDER_UPPER_FIELD_FIRST",
	"DXGI_MODE_SCANLINE_ORDER_LOWER_FIELD_FIRST"
};

const char * const szScalingNames[] =
{
	"DXGI_MODE_SCALING_UNSPECIFIED",
	"DXGI_MODE_SCALING_CENTERED",
	"DXGI_MODE_SCALING_STRETCHED"
};


/////////////////////////////////////////////////////////////////
//
// CPresentFS implementation
//
/////////////////////////////////////////////////////////////////

CPresentFS::CPresentFS()
:
m_bTestRotatedModes( false ),
m_bOneModeOnly( false ),
m_bIgnoreRefresh( true ),
m_bUseMinimalFormatList(false),
m_bAlwaysFilterLowResolution( false )
{
	m_NumberOfPresents = 1;
	m_TexturesToLoad = m_NumberOfPresents;
	m_MaxBufferCount = 2;

	ZeroMemory( &m_OutputModeSet, MAX_DISPLAYMODES * sizeof( ModeInformation ) );
	ZeroMemory( &m_ModeInformation, sizeof( ModeInformation ) );

};


CPresentFS::~CPresentFS()
{
};

HRESULT CPresentFS::ModeListTransform(
	D3D_DEVICE_POINTER devicePointer,
	__in std::vector< ModeInformation >* pModes,
	__out std::vector< ModeInformation >* pTransformedModes)
{
	HRESULT hr = GetSupportedFormats(
		devicePointer,
		pModes,
		pTransformedModes,
		m_RequiredFormatSupport);

	return hr;
}


void CPresentFS::InitModeParameters()
{
	CDXGITestFramework * pFramework = dynamic_cast< CDXGITestFramework * >( GetFramework() );

	//
	// Enumerate desktop modes.
	//
	RFactor modeFactor;

	std::vector < Adapter > adapterEnumerated;
	std::vector < Adapter > adapterTransformed;

	HRESULT hresult = EnumerateModes(&adapterEnumerated);
	if ( FAILED( hresult ) )
	{
		WriteToLog
			(
			"CPresentFS::InitTestParameters:  EnumerateModes failed with error 0x%x.",
			hresult
			);

		goto Cleanup;
	};

	PrintModes(&adapterEnumerated);
	WriteToLog("*********Enumerated Modes\n");

	if( !g_Application.IsUseDesktopResolution() )
	{
		if(m_bAlwaysFilterLowResolution ||
			g_Application.IsNoLowResolution())
		{
			WriteToLog("*********FilterLowResolutionModes\n");
			FilterLowResolutionModes(&adapterEnumerated);
			PrintModes(&adapterEnumerated);
		}

		if ( m_bIgnoreRefresh )
		{
			RemoveModesThatVaryOnlyByRefresh(&adapterEnumerated);
			PrintModes(&adapterEnumerated);
			WriteToLog("*********RemoveModesThatVaryOnlyByRefresh\n");

		}

		if (m_bUseMinimalFormatList)
		{
			UseMinimalFormatList(&adapterEnumerated);
			PrintModes(&adapterEnumerated);
			WriteToLog("*********UseMinimalFormatList\n");
		}


		if (m_bOneModeOnly)
		{
		FirstModePerFormat(&adapterEnumerated);
		PrintModes(&adapterEnumerated);
		WriteToLog("*********FirstModePerFormat\n\n\n\n");

		}


		if(g_Application.IsMinResolutionSet())
		{
			if(pFramework->GetTestMode() == WGFTEST_MODE_WHQL)
			{
				WriteToLog("*********GetHighLowMidResolutionModes\n\n");
				GetHighLowMidResolutionModes(&adapterEnumerated);    
			} else
			{
				WriteToLog("*********GetHighResolutionModes\n\n");
				GetHighResolutionModes(&adapterEnumerated);      
			}
			PrintModes(&adapterEnumerated);
		}
	}

	for(UINT k = 0; k < adapterEnumerated.size();k++)
	{
		Adapter adapter;
		adapterTransformed.push_back(adapter);

		//
		// TEST_FEATURE_LEVEL is a parameter and is not available in InitParameters.
		//
		TEST_FEATURE_LEVEL testFeatureLevel= TEST_FEATURE_10_0_LEVEL_10_0;
		for(UINT i = 0; i < TEST_FEATURE_COUNT; i++)
		{
			if(pFramework->m_RequestedFeatureLevels[i] == true)
			{
				testFeatureLevel = (TEST_FEATURE_LEVEL)i;
			}
		}

		D3D_DEVICE_POINTER devicePointer;
		ZeroMemory(&devicePointer, sizeof(devicePointer));

		hresult = CreateAdapterDevice(testFeatureLevel, k, &devicePointer);
		if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CPresentFS::InitTestParameters:  EnumerateModes failed with error 0x%x.",
				hresult
			);

			goto Cleanup;
		};



		for(UINT j = 0; j < adapterEnumerated[k].OutputModeContainer.size();j++)
		{
			OutputModeContainer output;
			adapterTransformed[k].OutputModeContainer.push_back(output);

			for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
			{

				hresult = ModeListTransform(
					devicePointer,
					&adapterEnumerated[k].OutputModeContainer[j].RawModes[rotation],
					&adapterTransformed[k].OutputModeContainer[j].RawModes[rotation]);

				if(FAILED(hresult))
				{
					if ( NULL != devicePointer.pDevice)
					{
						devicePointer.pDevice->Release();
						devicePointer.pDevice = NULL;
					};

					goto Cleanup;
				}
			}
		}

		if ( NULL != devicePointer.pDevice)
		{
			devicePointer.pDevice->Release();
			devicePointer.pDevice = NULL;
		};

	}

	m_iTotalModes = 0;

	for(UINT k = 0; k < adapterTransformed.size();k++)
	{
		for(UINT j = 0; j < adapterTransformed[k].OutputModeContainer.size();j++)
		{
			//TCHAR parameterName[ 255 ] = {0};	
			//StringCchPrintf
			//    (
			//    parameterName,
			//    ARRAYSIZE( parameterName ),
			//    _T("Mode_Adapter_%d_output_%d"),
			//    k,
			//    j
			//    );  

			if ( adapterTransformed[k].OutputModeContainer[j].GetTotalModeCount() > 0 )
			{
				for(UINT rotation = 0; rotation < (UINT)(DXGI_MODE_ROTATION_ROTATE270) ;rotation++)
				{
					for(UINT z = 0 ; z < adapterTransformed[k].OutputModeContainer[j].RawModes[rotation].size() 
						&& z < ( MAX_MODES_PER_OUTPUT ) / 4; 
						z++)
					{
						assert( m_iTotalModes < MAX_DISPLAYMODES );
						m_OutputModeSet[ m_iTotalModes ] = adapterTransformed[k].OutputModeContainer[j].RawModes[rotation][z];
						m_iTotalModes++;
					}
				}
			}
			else
			{
				DebugOut.Write("No Modes found for adapter[%d].Output[%d] \n", k,j);
				m_ParameterError = -1;
			}
		}
	}

Cleanup:;

}


void CPresentFS::InitTestParameters()
{
	InitFSTestParameters();

	RFactor testRFactors= IntitTestRFactors();


	if(m_globalRFactor.Valid() && m_FeatureLevelFactor.Valid())
	{

		//
		// If derived test sets some rfactors use them. If there is an error or they dont exist dont use the.
		//
		if(testRFactors.Valid())
		{
			SetRootTestFactor(m_FeatureLevelFactor * m_globalRFactor * testRFactors );
		}
		else
		{
			SetRootTestFactor(m_FeatureLevelFactor * m_globalRFactor );  
		}
	}
	else
	{
		if( m_FeatureLevelFactor.Valid() )
			SetRootTestFactor( m_FeatureLevelFactor );

		WriteToLog("CPresentFS::SetRootTestFactor parameter combination is invalid. m_globalRFactor.Valid=%d m_FeatureLevelFactor.Valid=%d", m_globalRFactor.Valid(), m_FeatureLevelFactor.Valid());
		m_ParameterError = -1;
	}
}


void CPresentFS::InitFSTestParameters()
{

	RFactor rotation, format, width, height, refreshrate[2], scanlineordering, scaling, adapter, output, multisamplecount, multisamplequality, isInitialized;
	//
	// This test group doesn't run under TS.
	//
	if
	(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		goto Cleanup;
	};


	//
	// Call the base class implementation.
	//

	CDXGITest::InitTestParameters();

	//
	// Enumerate\Initailize modes
	//
	InitModeParameters();

	if (m_iTotalModes > 0 )
	{
		rotation = AddParameter(
			_T( "Rotation" ),
			&m_ModeInformation.devmode.dmDisplayOrientation,
			new CTableValueSet< DWORD >(
				&m_OutputModeSet[0].devmode.dmDisplayOrientation,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		format = AddParameter(
			_T( "ModeDesc.Format" ),
			&m_ModeInformation.ModeDesc.Format,
			new CTableValueSet< DXGI_FORMAT >(
				&m_OutputModeSet[0].ModeDesc.Format,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		width = AddParameter(
			_T( "ModeDesc.Width" ),
			&m_ModeInformation.ModeDesc.Width,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].ModeDesc.Width,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		height = AddParameter(
			_T( "ModeDesc.Height" ),
			&m_ModeInformation.ModeDesc.Height,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].ModeDesc.Height,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		refreshrate[0] = AddParameter(
			_T( "ModeDesc.RefreshRate.Numerator" ),
			&m_ModeInformation.ModeDesc.RefreshRate.Numerator,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].ModeDesc.RefreshRate.Numerator,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		refreshrate[1] = AddParameter(
			_T( "ModeDesc.RefreshRate.Denominator" ),
			&m_ModeInformation.ModeDesc.RefreshRate.Denominator,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].ModeDesc.RefreshRate.Denominator,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		scanlineordering = AddParameter(
			_T( "ModeDesc.ScanlineOrdering" ),
			&m_ModeInformation.ModeDesc.ScanlineOrdering,
			new CTableValueSet< DXGI_MODE_SCANLINE_ORDER >(
				&m_OutputModeSet[0].ModeDesc.ScanlineOrdering,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		scaling = AddParameter(
			_T( "ModeDesc.Scaling" ),
			&m_ModeInformation.ModeDesc.Scaling,
			new CTableValueSet< DXGI_MODE_SCALING >(
				&m_OutputModeSet[0].ModeDesc.Scaling,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		adapter = AddParameter(
			_T( "DXGIAdapterOrdinal" ),
			&m_ModeInformation.uiDXGIAdapterOrdinal,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].uiDXGIAdapterOrdinal,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		output = AddParameter(
			_T( "DXGIOutputOrdinal" ),
			&m_ModeInformation.uiDXGIOutputOrdinal,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].uiDXGIOutputOrdinal,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		multisamplecount = AddParameter(
			_T( "MultisampleCount" ),
			&m_ModeInformation.uiMultisampleCount,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].uiMultisampleCount,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		multisamplequality = AddParameter(
			_T( "MultisampleQuality" ),
			&m_ModeInformation.uiMultisampleQuality,
			new CTableValueSet< UINT >(
				&m_OutputModeSet[0].uiMultisampleQuality,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		isInitialized = AddParameter(
			_T( "isInitialized" ),
			&m_ModeInformation.isInitialized,
			new CTableValueSet< BOOL >(
				&m_OutputModeSet[0].isInitialized,
				sizeof( m_OutputModeSet[0] ),
				m_iTotalModes ) );

		m_globalRFactor = rotation % format % width % height % refreshrate[0] % refreshrate[1] % scanlineordering % scaling % adapter % output % multisamplecount % multisamplequality % isInitialized;

		if( m_vecAdapterDescriptors.size() < 2)
			AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 0 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 0 )), WeightFactorLarge);

		AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 1 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 0 )), WeightFactorLarge);
		AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 0 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 1 )), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual< DXGI_FORMAT >(_T("ModeDesc.Format"), DXGI_FORMAT_R8G8B8A8_UNORM ), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual< DXGI_FORMAT >(_T("ModeDesc.Format"), DXGI_FORMAT_B8G8R8A8_UNORM ), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual< DXGI_FORMAT >(_T("ModeDesc.Format"), DXGI_FORMAT_R16G16B16A16_FLOAT ), WeightFactorSmall);
		AddPriorityPatternFilter(FilterEqual< DXGI_MODE_SCANLINE_ORDER >(_T("ModeDesc.ScanlineOrdering"), DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual< DXGI_MODE_SCALING >(_T("ModeDesc.Scaling"), DXGI_MODE_SCALING_STRETCHED ), WeightFactorLarge);
		AddPriorityPatternFilter(FilterEqual< DWORD >(_T("Rotation"), 3), WeightFactorSmall);
		AddPriorityPatternFilter(FilterEqual< DWORD >(_T("Rotation"), 2), WeightFactorSmall);

		DeclarePriorityLevelByPriorityValue(1, 2.0f);
		DeclarePriorityLevelByPriorityValue(2, 1.0f);
	}


Cleanup:
	DebugOut.Write("Exiting InitTestParameters.\n");
};


TEST_RESULT CPresentFS::Setup()
{
	//
	// Initialize locals.
	//

	TEST_RESULT tResult = RESULT_FAIL;
	HRESULT hResult = S_OK;

	//
	// Skip this group if we're testing Terminal Services.
	//

	if
	(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		tResult = RESULT_SKIP;
		goto Cleanup;
	};


	//
	// Initialize our window class.
	//

	bool bInitializeWindowClassResult = InitializeWindowClass();

	if ( bInitializeWindowClassResult != true )
	{
		WriteToLog
		(
			_T( "CPresentFS::Setup:  InitializeWindowClass failed." )
		);

		tResult = RESULT_ABORT;
		goto Cleanup;
	};


	//
	// Create our test window.
	//

	hResult = CreateTestWindows
	(
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT
	);

	if ( FAILED( hResult ) )
	{
		WriteToLog
		(
			"CPresentFS::Setup:  CreateTestWindows failed. hr = 0x%x", hResult
		);

		goto Cleanup;
	};
	//
	// Display the window.
	//

	ActivateWindow
	(
		m_hwndFocusWindow
	);


	m_hwndDeviceWindow = m_hwndFocusWindow;


	//
	// Initialize the device.
	//

	assert( m_DevicePointer.pDevice == NULL );
	hResult = g_Application.CreateFactoryAdapterDevice
	(
		&m_FactoryPointer,
		g_Application.m_DXGIOptions.AdapterIDSrc == ADAPTER_UNDEFINED ? 0 : g_Application.m_DXGIOptions.AdapterIDSrc,
		&m_AdapterPointer,
		g_Application.m_D3DOptions.SrcDriverType,
		D3D10_CREATE_DEVICE_SINGLETHREADED,
		m_TestFeatureLevel,
		0,
		&m_DevicePointer
	);

	if
	(
		hResult == DXGI_ERROR_NOT_FOUND &&
		m_AdapterPointer.pAdapter == NULL
	)
	{
		WriteToLog
		(
			"No adapter found at ordinal %u.",
			g_Application.m_DXGIOptions.AdapterIDSrc == ADAPTER_UNDEFINED ? 0 : g_Application.m_DXGIOptions.AdapterIDSrc
		);

		tResult = RESULT_SKIP;
		goto Cleanup;
	}
	else if
	(
		hResult == DXGI_ERROR_UNSUPPORTED ||
		hResult == E_NOINTERFACE ||
		hResult == E_NOTIMPL ||
		hResult == E_FAIL &&
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

		tResult = RESULT_SKIP;
		goto Cleanup;
	}
	else if ( FAILED( hResult ) )
	{
		WriteToLog
		(
			"CPresentFS::Setup:  CreateFactoryAdapterDevice failed with error 0x%x.",
			hResult
		);

		goto Cleanup;
	};

	if ( m_DevicePointer.pDevice == NULL )
	{
		WriteToLog
		(
			"CPresentFS::Setup:  CreateFactoryAdapterDevice succeeded but returned NULL device pointer."
		);

		goto Cleanup;
	};


	PrimaryAdapterDevice.WrapDevice( m_DevicePointer.pDevice );


	////
	//// Associate the focus window with the factory.
	////

	//hResult = m_FactoryPointer.pFactory->MakeWindowAssociation
	//(
	//	m_hwndFocusWindow,
	//	0
	//);

	//if ( FAILED( hResult ) )
	//{
	//	WriteToLog
	//	(
	//		"CPresentFS::Setup:  MakeWindowAssociation failed with error 0x%x.",
	//		hResult
	//	);

	//	goto Cleanup;
	//};


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


	tResult = RESULT_PASS;


Cleanup:
	DwmIsCompositionEnabled( &m_bDWMState );


	return tResult;
};


void CPresentFS::Cleanup()
{
	DebugOut.Write( "Entering Cleanup.\n" );


	//
	// Release our swap chains.
	//

	if ( m_pSwapChain != NULL )
	{
		if(FAILED(m_pSwapChain->SetFullscreenState( FALSE, NULL )))
		{
			CheckForegroundWindow( m_szFocusWindowClassName );
		}

		m_pSwapChain->Release();
		m_pSwapChain = NULL;
	};

	if ( m_pSwapChainMS != NULL )
	{
		if(FAILED(m_pSwapChainMS->SetFullscreenState( FALSE, NULL )))
		{
			CheckForegroundWindow( m_szFocusWindowClassName );
		}

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
	// Release our adapter pointer.
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


	g_Application.DwmEnableComposition( m_bDWMState );


	//
	// Unregister our window class.
	//

	CleanupWindowClass();


	DebugOut.Write( "Exiting Cleanup.\n" );
};


HRESULT CPresentFS::EnumerateAdapterModes
(
	std::vector < Adapter >* pAdapters,
	IDXGIAdapter * pDXGIAdapterEnumMode,
	UINT uiAdapterOrdinal
)
{
	//
	// Initialize locals.
	//

	HRESULT hresult = E_FAIL;
	IDXGIOutput * pDXGIOutputEnumMode = NULL;
	Adapter nextAdapter;
	pAdapters->push_back( nextAdapter );
	UINT uiAdapterCount = pAdapters->size();

	//
	// Enumerate outputs.
	//
	for
	(
		UINT uiOutputOrdinal = 0;
		uiOutputOrdinal < MAX_OUTPUTS;
		++uiOutputOrdinal
	)
	{
		//
		// Release the output from the prior iteration.
		//

		if ( pDXGIOutputEnumMode != NULL )
		{
			pDXGIOutputEnumMode->Release();
			pDXGIOutputEnumMode = NULL;
		};


		//
		// Enumerate the output at the current ordinal.
		//

		hresult = pDXGIAdapterEnumMode->EnumOutputs
		(
			uiOutputOrdinal,
			&pDXGIOutputEnumMode
		);


		if ( hresult == DXGI_ERROR_NOT_FOUND )
		{
			continue;
		}
		else if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CPresentFS::EnumerateAdapterModes:  EnumOutputs on output %u failed with error 0x%x.",
				uiOutputOrdinal,
				hresult
			);

			goto Cleanup;
		};

		if ( pDXGIOutputEnumMode == NULL )
		{
			WriteToLog
			(
				"CPresentFS::EnumerateAdapterModes:  EnumOutputs succeeded but returned NULL output pointer."
			);

			hresult = E_FAIL;
			goto Cleanup;
		};


		OutputModeContainer output;
		(*pAdapters)[uiAdapterCount-1].OutputModeContainer.push_back(output);

		//
		// Get a descriptor for the output.
		//

		DXGI_OUTPUT_DESC outputDesc;
		hresult = pDXGIOutputEnumMode->GetDesc( &outputDesc );
		if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CPresentFS::EnumerateAdapterModes:  GetDesc on output %u failed with error 0x%x.",
				uiOutputOrdinal,
				hresult
			);

			goto Cleanup;
		};


		//
		// Determine the current mode for the output.
		//

		DEVMODEW Mode;
		ZeroMemory( &Mode, sizeof( Mode ) );
		Mode.dmSize = sizeof( Mode );


		BOOL bGetCurrentModeResult = EnumDisplaySettingsW
		(
			outputDesc.DeviceName,
			ENUM_CURRENT_SETTINGS,
			&Mode
		);

		if ( bGetCurrentModeResult != TRUE )
		{
			DWORD GetCurrentModeError = GetLastError();

			WriteToLog
			(
				"CPresentFS::EnumerateAdapterModes:  EnumDisplaySettingsW failed with error %u.",
				GetCurrentModeError
			);

			if ( GetCurrentModeError != ERROR_SUCCESS )
			{
				hresult = HRESULT_FROM_WIN32( GetCurrentModeError );
			};

			goto Cleanup;
		};


		//
		// Find the mode for this monitor.
		//

		std::map< HMONITOR, DEVMODEW >::const_iterator itMode = m_DesktopMode.find( outputDesc.Monitor );
		if( m_DesktopMode.end() == itMode )
		{
			m_DesktopMode[ outputDesc.Monitor ] = Mode;
		};


		for
		(
			UINT modeRotation = DXGI_MODE_ROTATION_IDENTITY;
			modeRotation <= DXGI_MODE_ROTATION_ROTATE270;
			++modeRotation
		)
		{
			//
			// Only do DXGI_MODE_ROTATION_IDENTITY for non rotated cases
			//
			if
			(
				DXGI_MODE_ROTATION_IDENTITY != modeRotation &&
				!m_bTestRotatedModes
			)
			{
				continue;
			};


			//
			// don't do DXGI_MODE_ROTATION_IDENTITY for rotated cases
			//
			if
			(
				DXGI_MODE_ROTATION_IDENTITY == modeRotation &&
				m_bTestRotatedModes
			)
			{
				continue;
			};

			DEVMODEW rotatedMode = Mode;

			rotatedMode.dmFields = DM_BITSPERPEL |
				DM_PELSWIDTH |
				DM_PELSHEIGHT |
				DM_DISPLAYFLAGS |
				DM_DISPLAYFREQUENCY |
				DM_DISPLAYFIXEDOUTPUT |
				DM_DISPLAYORIENTATION;

			rotatedMode.dmDisplayFixedOutput = DMDFO_DEFAULT;

			//
			// If ENUM_CURRENT_SETTINGS is rotated 90 or 270 Width and Height are already swapped. Switch statement below expects unswapped
			// Width and Height. 
			//

			if(Mode.dmDisplayOrientation == DMDO_90 || Mode.dmDisplayOrientation == DMDO_180)
			{
				std::swap( rotatedMode.dmPelsWidth, rotatedMode.dmPelsHeight );    
			}


			switch ( modeRotation )
			{
				case DXGI_MODE_ROTATION_IDENTITY:
				{
					rotatedMode.dmDisplayOrientation = DMDO_DEFAULT;
				} break;

				case DXGI_MODE_ROTATION_ROTATE90:
				{
					std::swap( rotatedMode.dmPelsWidth, rotatedMode.dmPelsHeight );
					rotatedMode.dmDisplayOrientation = DMDO_90;
				} break;

				case DXGI_MODE_ROTATION_ROTATE180:
				{
					rotatedMode.dmDisplayOrientation = DMDO_180;
				} break;

				case DXGI_MODE_ROTATION_ROTATE270:
				{
					std::swap( rotatedMode.dmPelsWidth, rotatedMode.dmPelsHeight );
					rotatedMode.dmDisplayOrientation = DMDO_270;
				} break;
			};


			//
			// Switch to the mode.
			//
			LONG lResult = ChangeDisplaySettingsExW
			(	
				outputDesc.DeviceName,
				&rotatedMode,
				NULL,
				0,
				NULL
			);

			if(lResult != DISP_CHANGE_SUCCESSFUL)
			{
				WriteToLog
				(
					_T("attempted Mode: Adapter=%u Output=%u dmPelsWidth %d dmPelsHeight %d dmDisplayFrequency %d dmDisplayFlags %d dmFields %d dmDisplayFixedOutput %d dmDisplayOrientation %d"),
					uiAdapterOrdinal,
					uiOutputOrdinal,
					rotatedMode.dmPelsWidth,
					rotatedMode.dmPelsHeight,
					rotatedMode.dmDisplayFrequency,
					rotatedMode.dmDisplayFlags,
					rotatedMode.dmFields, 
					rotatedMode.dmDisplayFixedOutput, 
					rotatedMode.dmDisplayOrientation
				);
			}

			switch ( lResult )
			{
				case DISP_CHANGE_SUCCESSFUL:
				{
					/*WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW returned DISP_CHANGE_SUCCESSFUL."
					);*/

				} break;

				case DISP_CHANGE_BADDUALVIEW:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADDUALVIEW."
					);

					continue;
				} break;

				case DISP_CHANGE_BADFLAGS:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADFLAGS."
					);

					continue;
				} break;

				case DISP_CHANGE_BADMODE:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADMODE."
					);

					continue;
				} break;

				case DISP_CHANGE_BADPARAM:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADPARAM."
					);

					continue;
				} break;

				case DISP_CHANGE_FAILED:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_FAILED."
					);

					continue;
				} break;

				case DISP_CHANGE_NOTUPDATED:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_NOTUPDATED."
					);

					continue;
				} break;

				case DISP_CHANGE_RESTART:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with DISP_CHANGE_RESTART."
					);

					continue;
				} break;

				default:
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsW failed with unexpected error %d.",
						lResult
					);

					continue;
				} break;
			};

			for
			(
				UINT uiFormatIndex = 0;
				uiFormatIndex < clsScanOutFormats::cuiNoOfScanOutFormats;
				++uiFormatIndex
			)
			{
				UINT uiModeCount = 0;

				//
				// Determine the number of supported modes for
				// this format.
				//

				hresult = pDXGIOutputEnumMode->GetDisplayModeList
				(
					clsScanOutFormats::DXGIScanOutFormats[ uiFormatIndex ],
					DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
					&uiModeCount,
					NULL
				);

				if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes: Adapter=%u Output=%u foramt %d GetDisplayModeList (1st call) failed with error 0x%x.",
						uiAdapterOrdinal,
						uiOutputOrdinal,
						clsScanOutFormats::DXGIScanOutFormats[ uiFormatIndex ],
						hresult
					);

					continue;
				};


				if ( uiModeCount == 0 )
				{
					/*
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  GetDisplayModeList returned 0 modes for this format."
					);
					*/

					continue;
				};


				DXGI_MODE_DESC * pModeDescArray = new DXGI_MODE_DESC[ uiModeCount ];

				if( pModeDescArray == NULL )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  Failed to allocate mode descriptor array."
					);

					hresult = E_OUTOFMEMORY;
					goto Cleanup;
				};

				ZeroMemory( pModeDescArray, uiModeCount * sizeof( DXGI_MODE_DESC ) );


				//
				// Get the array of supported modes for this format.
				//

				hresult = pDXGIOutputEnumMode->GetDisplayModeList
				(
					clsScanOutFormats::DXGIScanOutFormats[ uiFormatIndex ],
					DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
					&uiModeCount,
					pModeDescArray
				);

				if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateAdapterModes:  GetDisplayModeList failed with error 0x%x.",
						hresult
					);

					delete [] pModeDescArray;
					pModeDescArray = NULL;

					goto Cleanup;
				};

				for(UINT modes = 0; modes < uiModeCount; modes++)
				{

					// DXGI_MODE_DESC dimensions are rotated
					if( !g_Application.IsUseDesktopResolution() || ( (pModeDescArray + modes)->Width == rotatedMode.dmPelsWidth && (pModeDescArray + modes)->Height == rotatedMode.dmPelsHeight) )
					{
						ModeInformation ModeInit;

						ModeInit.uiDXGIAdapterOrdinal = uiAdapterOrdinal;
						ModeInit.uiDXGIOutputOrdinal = uiOutputOrdinal;
						ModeInit.ModeDesc = *(pModeDescArray + modes);
						ModeInit.uiMultisampleCount = 1;
						ModeInit.uiMultisampleQuality = 0;
						ModeInit.isInitialized = TRUE;
						ModeInit.hmonitor = outputDesc.Monitor;
						ZeroMemory( &ModeInit.devmode, sizeof( ModeInit.devmode ) );
						ModeInit.devmode.dmDisplayOrientation = rotatedMode.dmDisplayOrientation;						
						(*pAdapters)[uiAdapterCount-1].OutputModeContainer[uiOutputOrdinal].RawModes[(UINT)modeRotation - 1].push_back( ModeInit );
					}

					/*DebugOut.Write("output %d DXGIScanOutFormats %d rotation %d ModeDesc.Width %d ModeDesc.Height %d ModeDesc.RefreshRate.Numerator %d ModeDesc.RefreshRate.Denominator %d ModeDesc.ScanlineOrdering %d ModeDesc.Scaling %d uiMultisampleCount %d uiMultisampleQuality %d \n" ,
					uiOutputOrdinal,
					uiFormatIndex,
					modeRotation,
					ModeInit.ModeDesc.Width, 
					ModeInit.ModeDesc.Height,
					ModeInit.ModeDesc.RefreshRate.Numerator,
					ModeInit.ModeDesc.RefreshRate.Denominator,
					ModeInit.ModeDesc.ScanlineOrdering,
					ModeInit.ModeDesc.Scaling,
					ModeInit.uiMultisampleCount,
					ModeInit.uiMultisampleQuality);*/
				}

				delete [] pModeDescArray;
				pModeDescArray = NULL;
			};

		};


		pDXGIOutputEnumMode->Release();
		pDXGIOutputEnumMode = NULL;


		//
		// Restore the output to the original mode.
		//

		LONG lResult = ChangeDisplaySettingsExW
		(
			outputDesc.DeviceName,
			&Mode,
			NULL,
			0,
			NULL
		);

		if ( lResult != DISP_CHANGE_SUCCESSFUL )
		{
			WriteToLog
			(
				"CPresentFS::EnumerateAdapterModes:  ChangeDisplaySettingsExW failed with error %d while restoring the mode.",
				lResult
			);
		};
	};

	//
	// If we made it this far, then we're good to go.
	//

	hresult = S_OK;


Cleanup:
	if ( pDXGIOutputEnumMode != NULL )
	{
		pDXGIOutputEnumMode->Release();
		pDXGIOutputEnumMode = NULL;
	};


	return hresult;
};


HRESULT CPresentFS::EnumerateModes(std::vector < Adapter >* pAdapters)
{
	//
	// Initialize locals.
	//
	HRESULT hresult = E_FAIL;

	DXGI_FACTORY_POINTER FactoryPointer;
	DXGI_ADAPTER_POINTER AdapterPointer;
	AdapterPointer.pAdapter = NULL;

	hresult = g_Application.CreateDXGIFactory
	(
		&FactoryPointer
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::EnumerateModes:  CreateDXGIFactory failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	if ( FactoryPointer.pFactory == NULL )
	{
		WriteToLog
		(
			"CPresentFS::EnumerateModes:  CreateFactory succeeded but returned a NULL factory pointer."
		);

		hresult = E_FAIL;
		goto Cleanup;
	};

	switch ( g_Application.m_D3DOptions.SrcDriverType )
	{
		case D3D10_DRIVER_TYPE_SOFTWARE:
		case D3D10_DRIVER_TYPE_REFERENCE:
		case D3D10_DRIVER_TYPE_NULL:
		{
			HMODULE hRasterizer = NULL;
			if ( g_Application.m_D3DOptions.SrcDriverType == D3D10_DRIVER_TYPE_REFERENCE )
			{
				hRasterizer = LoadLibraryExW
				(
					D3D_REF_DLLNAME,
					NULL,
					0
				);

				if ( hRasterizer == NULL )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateModes:  Failed to load reference rasterizer."
					);

					goto Cleanup;
				};
			}
			else if ( g_Application.m_D3DOptions.SrcDriverType == D3D10_DRIVER_TYPE_SOFTWARE )
			{
				hRasterizer = LoadLibraryExW
				(
					g_Application.m_D3DOptions.SoftwareRasterizerDLLName,
					NULL,
					0
				);

				if ( hRasterizer == NULL )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateModes:  Failed to load software rasterizer."
					);

					goto Cleanup;
				};
			};


			//
			// Create a software adapter.
			//

			hresult = FactoryPointer.pFactory->CreateSoftwareAdapter
			(
				hRasterizer,
				&AdapterPointer.pAdapter
			);


			if ( hRasterizer != NULL )
			{
				FreeLibrary( hRasterizer );
				hRasterizer = NULL;
			};

			if ( FAILED( hresult ) )
			{
				WriteToLog
				(
					"CPresentFS::EnumerateModes:  CreateSoftwareAdapter failed with error 0x%x.",
					hresult
				);

				goto Cleanup;
			};

			if ( AdapterPointer.pAdapter == NULL )
			{
				WriteToLog
				(
					"CPresentFS::EnumerateModes:  CreateSoftwareAdapter succeeded but returned a NULL software adapter pointer."
				);

				goto Cleanup;
			};


			//
			// Enumerate outputs and modes for this adapter.
			//
			hresult = EnumerateAdapterModes
			(
				pAdapters,
				AdapterPointer.pAdapter,
				0
			);

			if ( FAILED( hresult ) )
			{
				WriteToLog
				(
					"CPresentFS::EnumerateModes:  EnumerateAdapterModes failed with error 0x%x.",
					hresult
				);

				goto Cleanup;
			};
		} break;


		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			//
			// Enumerate outputs and modes for all adapters.
			//
			for
			(
				UINT uiAdapterOrdinal = 0;
				uiAdapterOrdinal < MAX_ADAPTERS;
				++uiAdapterOrdinal
			)
			{
				//
				// Release the adapter from the prior iteration.
				//

				if ( AdapterPointer.pAdapter != NULL )
				{
					AdapterPointer.pAdapter->Release();
					AdapterPointer.pAdapter = NULL;
				};

				// 
				// If there is a value for Adapter command line parameter
				// only enumerate modes for that adapter.
				//
				if( g_Application.m_DXGIOptions.AdapterIDSrc != ADAPTER_UNDEFINED &&
					g_Application.m_DXGIOptions.AdapterIDSrc != uiAdapterOrdinal)
				{
					continue;
				}

				//
				// Enumerate the adapter at the specified ordinal.
				//

				hresult = FactoryPointer.pFactory->EnumAdapters
				(
					uiAdapterOrdinal,
					&AdapterPointer.pAdapter
				);

				if ( hresult == DXGI_ERROR_NOT_FOUND )
				{
					break;
				}
				else if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateModes:  EnumAdapters on adapter %u failed with error 0x%x.",
						uiAdapterOrdinal,
						hresult
					);

					goto Cleanup;
				};

				if ( AdapterPointer.pAdapter == NULL )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateModes:  EnumAdapters on adapter %u succeeded but returned a NULL adapter pointer.",
						uiAdapterOrdinal
					);

					hresult = E_FAIL;
					goto Cleanup;
				};

				DXGI_ADAPTER_DESC AdapterDescriptor;

				if ( AdapterPointer.pAdapter->GetDesc( &AdapterDescriptor ) == S_OK )
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
						int cCharactersConverted = WideCharToMultiByte
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
							if( g_Application.IsAdapterOfType( AdapterPointer.pAdapter1, g_Application.m_DXGIOptions.AdapterType[0] )  )
							{
								WriteToLog
								(
									"CFSTestCases::EnumerateModes:  Adding adapter at AdapterOrdinal: %d AdapterDescription: %s",
									uiAdapterOrdinal,
									pszAdapterDesc
								);
							}
							else
							{
								// ignore HW adapters
								delete [] pszAdapterDesc;
								pszAdapterDesc = NULL;

								SAFE_RELEASE( AdapterPointer.pAdapter );

								continue;                    
							}
						}

						delete [] pszAdapterDesc;
						pszAdapterDesc = NULL;
					}
				}

				//
				// Enumerate outputs and modes for this adapter.
				//
				hresult = EnumerateAdapterModes
				(
					pAdapters,	
					AdapterPointer.pAdapter,
					uiAdapterOrdinal
				);

				if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CPresentFS::EnumerateModes:  EnumerateAdapterModes failed with error 0x%x.",
						hresult
					);

					goto Cleanup;
				};


				//
				// Release the adapter.
				//

				if ( AdapterPointer.pAdapter != NULL )
				{
					AdapterPointer.pAdapter->Release();
					AdapterPointer.pAdapter = NULL;
				};
			};


			//
			// Release the factory.
			//

			FactoryPointer.pFactory->Release();
			FactoryPointer.pFactory = NULL;
		} break;


		default:
		{
			assert( FALSE );
		} break;
	};


	hresult = S_OK;


Cleanup:
	if ( AdapterPointer.pAdapter != NULL )
	{
		AdapterPointer.pAdapter->Release();
		AdapterPointer.pAdapter = NULL;
	};


	if ( FactoryPointer.pFactory != NULL )
	{
		FactoryPointer.pFactory->Release();
		FactoryPointer.pFactory = NULL;
	};


	return hresult;
};


HRESULT CPresentFS::SetDisplayMode
(
	IDXGIOutput * pTargetOutput
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Validate parameters.
	//

	if ( pTargetOutput == NULL )
	{
		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Get a descriptor for the output.
	//

	DXGI_OUTPUT_DESC outputDesc;
	HRESULT hGetDescResult = pTargetOutput->GetDesc( &outputDesc );

	if ( FAILED( hGetDescResult ) )
	{
		WriteToLog
		(
			"GetDesc on target output failed with error 0x%x.",
			hGetDescResult
		);

		hOverallResult = hGetDescResult;
		goto Cleanup;
	};

	hOverallResult = hGetDescResult;


	//
	// Rotate the display if necessary.
	//

	if ( outputDesc.Rotation != m_RotationMode  )
	{
		//
		// Initialize our mode structure.
		//

		DEVMODEW dmModeToChangeTo;
		dmModeToChangeTo.dmSize = sizeof( DEVMODEW );
		dmModeToChangeTo.dmSpecVersion = DM_SPECVERSION;


		//
		// Get the current desktop mode.
		//

		BOOL bGetCurrentModeResult = EnumDisplaySettingsW
		(
			outputDesc.DeviceName,
			ENUM_CURRENT_SETTINGS,
			&dmModeToChangeTo
		);

		if ( bGetCurrentModeResult != TRUE )
		{
			DWORD GetCurrentModeError = GetLastError();

			WriteToLog
			(
				"CPresentFS::SetDisplayMode:  EnumDisplaySettingsW failed with error %u.",
				GetCurrentModeError
			);

			if ( GetCurrentModeError != ERROR_SUCCESS )
			{
				hOverallResult = HRESULT_FROM_WIN32( GetCurrentModeError );
			};

			goto Cleanup;
		};

		// Get GDI as close as we know how. GDI rotation should be preserved.
		dmModeToChangeTo.dmBitsPerPel = CFormatInfoMap::Find( m_SCDesc.BufferDesc.Format )->uBitsPerElement;

		switch (dmModeToChangeTo.dmBitsPerPel)
		{
			case 16:
			case 32:
			break;

			default:
				//if we'd confuse GDI, we'll substitute something it'll grok
				dmModeToChangeTo.dmBitsPerPel = 32;
		}


		dmModeToChangeTo.dmPelsWidth = m_ModeInformation.ModeDesc.Width;
		dmModeToChangeTo.dmPelsHeight = m_ModeInformation.ModeDesc.Height;
		dmModeToChangeTo.dmDisplayOrientation = DxgiModeRotationToDisplayOrientation(m_RotationMode);

		dmModeToChangeTo.dmDisplayFrequency = UINT( double( m_SCDesc.BufferDesc.RefreshRate.Numerator ) / m_SCDesc.BufferDesc.RefreshRate.Denominator + 0.5 );
		dmModeToChangeTo.dmDisplayFlags =
			(m_SCDesc.BufferDesc.ScanlineOrdering != DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE ? DM_INTERLACED : 0);

		dmModeToChangeTo.dmFields = (dmModeToChangeTo.dmFields & DM_POSITION) | /* Preserve position if previously specified */
			DM_BITSPERPEL |
			DM_PELSWIDTH |
			DM_PELSHEIGHT |
			DM_DISPLAYFLAGS |
			DM_DISPLAYFIXEDOUTPUT |
			DM_DISPLAYFREQUENCY |
			DM_DISPLAYORIENTATION;

		switch (m_SCDesc.BufferDesc.Scaling)
		{
			case (DXGI_MODE_SCALING_UNSPECIFIED): dmModeToChangeTo.dmDisplayFixedOutput = DMDFO_DEFAULT; break;
			case (DXGI_MODE_SCALING_STRETCHED): dmModeToChangeTo.dmDisplayFixedOutput = DMDFO_STRETCH; break;
			case (DXGI_MODE_SCALING_CENTERED): dmModeToChangeTo.dmDisplayFixedOutput = DMDFO_CENTER; break;
		}

		dmModeToChangeTo.dmDeviceName[0] = 0;


		//
		// Attempt the mode switch.
		//


		hOverallResult = E_FAIL;
		LONG lResult = ChangeDisplaySettingsExW
		(
			outputDesc.DeviceName,
			&dmModeToChangeTo,
			NULL,
			0,
			NULL
		);

		if( lResult != DISP_CHANGE_SUCCESSFUL )
		{        
			WriteToLog(_T("dmModeToChangeTo.dmPelsWidth %d dmModeToChangeTo.dmPelsHeight %d dmModeToChangeTo.dmDisplayFrequency %d dmModeToChangeTo.dmDisplayFlags %d dmModeToChangeTo.dmFields %d dmModeToChangeTo.dmDisplayFixedOutput %d dmModeToChangeTo.dmDisplayOrientation %d"),dmModeToChangeTo.dmPelsWidth,dmModeToChangeTo.dmPelsHeight,dmModeToChangeTo.dmDisplayFrequency,dmModeToChangeTo.dmDisplayFlags,dmModeToChangeTo.dmFields, dmModeToChangeTo.dmDisplayFixedOutput, dmModeToChangeTo.dmDisplayOrientation);

			DXGI_MODE_DESC  matchDesc = { 0 };
			DXGI_MODE_DESC  closestDesc = { 0 };
			memcpy( &matchDesc, &m_ModeInformation.ModeDesc, sizeof(DXGI_MODE_DESC));

			//
			// Find closest match does not work with _SRGB
			//
			if(m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
			{
				matchDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			}
			else if(m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
			{
				matchDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			HRESULT hrFindClosest = pTargetOutput->FindClosestMatchingMode(
				&matchDesc,
				&closestDesc,
				NULL);

			if( SUCCEEDED(hrFindClosest) )
			{
				assert( matchDesc.Format == closestDesc.Format );
				dmModeToChangeTo.dmDisplayFrequency = 0;
				dmModeToChangeTo.dmFields &= ~DM_DISPLAYFIXEDOUTPUT;
				dmModeToChangeTo.dmDisplayFlags =
					(closestDesc.ScanlineOrdering != DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE ? DM_INTERLACED : 0);
				dmModeToChangeTo.dmDisplayFixedOutput = 0;
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  Resorted to use FindClosestMatchingMode: BufferDesc Mode = %d x %d @ %f Hz, %s, %s.",
					closestDesc.Width,
					closestDesc.Height,
					(float) closestDesc.RefreshRate.Numerator /
					(float) closestDesc.RefreshRate.Denominator,
					szScanLineNames[ (UINT) closestDesc.ScanlineOrdering ],
					szScalingNames[ (UINT) closestDesc.Scaling ]
				);

			}

			lResult = ChangeDisplaySettingsExW
			(
				outputDesc.DeviceName,
				&dmModeToChangeTo,
				NULL,
				CDS_TRYCLOSEST,
				NULL
			);

			if( lResult == DISP_CHANGE_SUCCESSFUL )
			{
				if ( SUCCEEDED(hrFindClosest) )
				{
					m_SCDesc.BufferDesc.RefreshRate.Numerator = closestDesc.RefreshRate.Numerator;
					m_SCDesc.BufferDesc.RefreshRate.Denominator = closestDesc.RefreshRate.Denominator;
					m_SCDesc.BufferDesc.Scaling = closestDesc.Scaling;
					m_SCDesc.BufferDesc.ScanlineOrdering = closestDesc.ScanlineOrdering;
				}

				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  Resorted to CDS_TRYCLOSEST mode."
				);
			} 
			else
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode: ChangeDisplaySettingsExW could not set mode using CDS_TRYCLOSEST ."
				);
			}
		}

		switch( lResult )
		{
			case DISP_CHANGE_SUCCESSFUL:
			{
				EnumDisplaySettingsW
				(
					outputDesc.DeviceName,
					ENUM_CURRENT_SETTINGS,
					&m_ModeInformation.devmode
				);

				/*
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW returned DISP_CHANGE_SUCCESSFUL."
				);
				*/

				hOverallResult = S_OK;
			} break;

			case DISP_CHANGE_BADDUALVIEW:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_BADDUALVIEW."
				);
			} break;

			case DISP_CHANGE_BADFLAGS:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_BADFLAGS."
				);
			} break;

			case DISP_CHANGE_BADMODE:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_BADMODE."
				);
			} break;

			case DISP_CHANGE_BADPARAM:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_BADPARAM."
				);
			} break;

			case DISP_CHANGE_FAILED:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_FAILED."
				);
			} break;

			case DISP_CHANGE_NOTUPDATED:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_NOTUPDATED."
				);
			} break;

			case DISP_CHANGE_RESTART:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with error DISP_CHANGE_RESTART."
				);
			} break;

			default:
			{
				WriteToLog
				(
					"CPresentFS::SetDisplayMode:  ChangeDisplaySettingsW failed with unexpected error %d.",
					lResult
				);
			} break;
		};
	};


Cleanup:
	return hOverallResult;
};


TEST_RESULT CPresentFS::CreateDeviceAndSwapChain()
{
	//
	// Initialize locals.
	//

	TEST_RESULT test_result = RESULT_FAIL;
	HRESULT hresult = E_FAIL;
	IDXGIOutput * pTargetOutput = NULL;


	//
	// Release objects that we might be holding.
	//

	ReleaseTestCaseObjects();


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
	// Turn DWM off if necessary.
	//

	BOOL bDWMState = FALSE;
	DwmIsCompositionEnabled( &bDWMState );
	if ( bDWMState == TRUE )
	{
		g_Application.DwmEnableComposition( FALSE );
		Sleep( 1000 );
	};


	//
	// Create our device.
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
		hresult == E_NOTIMPL
	)
	{
		WriteToLog
		(
			"Could not create device."
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	}
	else if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  CreateFactoryAdapterDevice failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	if ( m_DevicePointer.pDevice == NULL )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  CreateFactoryAdapterDevice succeeded but returned NULL device pointer."
		);

		goto Cleanup;
	};


	//
	// Wrap the device.
	//

	PrimaryAdapterDevice.WrapDevice( m_DevicePointer.pDevice );


	//
	// Check for format support.
	//

	UINT FormatSupport = 0;
	hresult = m_DevicePointer.pDevice->CheckFormatSupport
	(
		m_ModeInformation.ModeDesc.Format,
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
	else if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  CheckFormatSupport failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	UINT RequiredFormatSupport = m_RequiredFormatSupport |
		( m_ModeInformation.uiMultisampleCount > 1 ? D3D10_FORMAT_SUPPORT_MULTISAMPLE_RENDERTARGET : 0 );

	//
	// DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM does not support Render target.  
	// Will need to create a DXGI_FORMAT_R10G10B10_A2_UNORM render target.
	//
	if(m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM)
	{
		RequiredFormatSupport = RequiredFormatSupport & ~D3D10_FORMAT_SUPPORT_RENDER_TARGET;
	}


	WriteToLog(_T("D3D10_FORMAT_SUPPORT_TEXTURE2D %d, D3D10_FORMAT_SUPPORT_RENDER_TARGET = %d, D3D10_FORMAT_SUPPORT_CPU_LOCKABLE = %d D3D10_FORMAT_SUPPORT_DISPLAY %d"),
		FormatSupport & D3D10_FORMAT_SUPPORT_TEXTURE2D,
		FormatSupport & D3D10_FORMAT_SUPPORT_RENDER_TARGET,
		FormatSupport & D3D10_FORMAT_SUPPORT_CPU_LOCKABLE,
		FormatSupport & D3D10_FORMAT_SUPPORT_DISPLAY);



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
	// Check for multisample support, and skip if unsupported.
	//

	UINT uSupportedMultisampleQuality = 0;
	if
	(
		m_ModeInformation.uiMultisampleCount > 1 &&
		(
			FAILED
			(
				m_DevicePointer.pDevice->CheckMultisampleQualityLevels
				(
					m_ModeInformation.ModeDesc.Format,
					m_ModeInformation.uiMultisampleCount,
					&uSupportedMultisampleQuality
				)
			) ||
			uSupportedMultisampleQuality <= (UINT) m_ModeInformation.uiMultisampleQuality
		)
	)
	{
		WriteToLog
		(
			"Device does not support sample quality %u.",
			m_ModeInformation.uiMultisampleQuality
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	};


	//
	// Show and activate the focus window.
	//

	ActivateWindow
	(
		m_hwndFocusWindow
	);


	ProcessMessages();


	//
	// Sanity-check our adapter and factory pointers.
	//

	assert( m_AdapterPointer.pAdapter != NULL );
	assert( m_FactoryPointer.pFactory != NULL );


	//
	// Associate our focus window with the DXGI factory.
	//

	DebugOut.Write( "CPresentFS::CreateDeviceAndSwapChain:  Calling MakeWindowAssociation.\n" );
	hresult = m_FactoryPointer.pFactory->MakeWindowAssociation
	(
		m_hwndFocusWindow,
		0
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  MakeWindowAssociation failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};


	//
	// Specify the swap chain properties.
	//

	ZeroMemory((void *)&m_SCDesc, sizeof(DXGI_SWAP_CHAIN_DESC));

	//m_SCDesc.BufferDesc = m_ModeInformation.ModeDesc;
	memcpy( &m_SCDesc.BufferDesc, &m_ModeInformation.ModeDesc, sizeof( DXGI_MODE_DESC ) );


	PrimaryAdapterDevice.SetRotationMode
	(
		m_bNonPrerotated
		? m_RotationMode
		: DXGI_MODE_ROTATION_IDENTITY
	);

	IDXGIOutput * pLocalTargetOutput = NULL;
	DXGI_OUTPUT_DESC desc;
	ZeroMemory(&desc,sizeof(DXGI_OUTPUT_DESC));

	hresult = m_AdapterPointer.pAdapter->EnumOutputs
	(
		m_ModeInformation.uiDXGIOutputOrdinal,
		&pLocalTargetOutput
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  EnumOutputs on adapter %u, output %u failed with error 0x%x.",
			m_ModeInformation.uiDXGIAdapterOrdinal,
			m_ModeInformation.uiDXGIOutputOrdinal,
			hresult
		);

		goto Cleanup;
	};

	pLocalTargetOutput->GetDesc(&desc);
	if ( m_RotationMode != DXGI_MODE_ROTATION_IDENTITY )
	{
		if
		(
			!m_bNonPrerotated &&
			(
				DXGI_MODE_ROTATION_ROTATE90 == m_RotationMode ||
				DXGI_MODE_ROTATION_ROTATE270 == m_RotationMode
			)
		)
		{
			//
			// App doesn't know about rotation. Create the swapchain with unrotated values
			// 
			std::swap
			(
				m_SCDesc.BufferDesc.Width,
				m_SCDesc.BufferDesc.Height
			);

			WriteToLog
			(
				"BufferDesc Mode = %d x %d ",
				m_SCDesc.BufferDesc.Width,
				m_SCDesc.BufferDesc.Height
			);
		};

		//
		// Set Display mode for the test if this is a rotation test
		//
		hresult = SetDisplayMode( pLocalTargetOutput );
		if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CPresentFS::CreateDeviceAndSwapChain: SetDisplayMode failed with error 0x%x.",
				hresult
			);

			test_result = RESULT_SKIP;
			goto Cleanup;
		};
	};

	if (pLocalTargetOutput)
	{
		pLocalTargetOutput->Release();
		pLocalTargetOutput = NULL;
	}


	//
	// Move the window onto the target output so createswapchain can find it, if the window was created on a different
	// adpater.
	//

	if(desc.AttachedToDesktop)
	{
		SetWindowPos
		(
			m_hwndDeviceWindow,
			HWND_TOP,
			desc.DesktopCoordinates.left,
			desc.DesktopCoordinates.top,
			0,
			0,
			SWP_NOSIZE
		);
	}

	m_SCDesc.SampleDesc.Count = 1;
	m_SCDesc.SampleDesc.Quality = 0;

	m_SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	m_SCDesc.BufferCount = m_BufferCount;

	m_SCDesc.OutputWindow = m_hwndDeviceWindow;

	m_SCDesc.Windowed = ( m_ModeInformation.uiMultisampleCount > 1 ) ? TRUE : FALSE;

	m_SCDesc.SwapEffect = m_SwapEffect.dxgiSwapEffect;

	m_SCDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if ( m_bNonPrerotated && !m_SCDesc.Windowed )
	{
		m_SCDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;
	};


	DXGI_SWAP_CHAIN_DESC SCMSDesc;
	memcpy( &SCMSDesc, &m_SCDesc, sizeof( DXGI_SWAP_CHAIN_DESC ) );
	SCMSDesc.SampleDesc.Count = m_ModeInformation.uiMultisampleCount;
	SCMSDesc.SampleDesc.Quality = m_ModeInformation.uiMultisampleQuality;


	//
	// Dump swap chain parameters.
	//

	{
		const DXGIFormatInfo *pEnumeratedModeFormatRecord = CFormatInfoMap::Find
		(
			m_SCDesc.BufferDesc.Format
		);

		WriteToLog
		(
			"BufferDesc Mode = %d x %d @ %f Hz (%s), %s, %s.",
			m_SCDesc.BufferDesc.Width,
			m_SCDesc.BufferDesc.Height,
			(float) m_SCDesc.BufferDesc.RefreshRate.Numerator /
			(float) m_SCDesc.BufferDesc.RefreshRate.Denominator,
			pEnumeratedModeFormatRecord
				? pEnumeratedModeFormatRecord->szDesc
				: "(Unrecognized format)",
			szScanLineNames[ (UINT) m_SCDesc.BufferDesc.ScanlineOrdering ],
			szScalingNames[ (UINT) m_SCDesc.BufferDesc.Scaling ]
		);
	};


	//
	// Attempt the swap chain creation.
	//

	DebugOut.Write("CPresentFS::CreateDeviceAndSwapChain:  Initializing swap chain.\n");

	if
	(
		TestedSwapChain.InitializeSwapChain( &m_SCDesc ) != TRUE
	)
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  InitializeSwapChain failed."
		);

		if( m_bLargeMemoryUsage && TestedSwapChain.GetLastError() == E_OUTOFMEMORY )
		{
			test_result = RESULT_SKIP;
			WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Swapchain creation failure expected. Skipping test case. " );
		}

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
				"CPresentFS::CreateDeviceAndSwapChain:  InitializeSwapChain on multisampled swap chain failed."
			);

			if( m_bLargeMemoryUsage && TestedSwapChainMS.GetLastError() == E_OUTOFMEMORY )
			{
				test_result = RESULT_SKIP;
				WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Swapchain creation failure expected. Skipping test case. " );
			}

			goto Cleanup;
		};
	};


	//
	// Get references to our swap chains.
	//

	assert( m_pSwapChain == NULL );
	TestedSwapChain.GetSwapChainPtr( &m_pSwapChain );
	if ( m_pSwapChain == NULL )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  Swap chain pointer is NULL."
		);

		test_result = RESULT_ABORT;
		goto Cleanup;
	};

	assert( m_pSwapChainMS == NULL );
	TestedSwapChainMS.GetSwapChainPtr( &m_pSwapChainMS );
	if ( m_ModeInformation.uiMultisampleCount > 1 && m_pSwapChainMS == NULL )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  Multisample swap chain pointer is NULL."
		);

		test_result = RESULT_ABORT;
		goto Cleanup;
	};


	//
	// Dump the back buffer format.
	//

	DebugOut.Write("***********************************\n");
	DebugOut.Write("CPresentFS::CreateDeviceAndSwapChain:  InitializeSwapChain succeeded!\n");

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
	// Enumerate the target output.
	//
	hresult = m_AdapterPointer.pAdapter->EnumOutputs
	(
		m_ModeInformation.uiDXGIOutputOrdinal,
		&pTargetOutput
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CPresentFS::CreateDeviceAndSwapChain:  EnumOutputs on adapter %u, output %u failed with error 0x%x.",
			m_ModeInformation.uiDXGIAdapterOrdinal,
			m_ModeInformation.uiDXGIOutputOrdinal,
			hresult
		);

		goto Cleanup;
	};


	//
	// Set the swap chain to full-screen on the target output.
	//

	if
	(
		m_SCDesc.Windowed == FALSE
	)
	{
		hresult = m_pSwapChain->SetFullscreenState
		(
			TRUE,
			pTargetOutput
		);

		if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CPresentFS::CreateDeviceAndSwapChain:  SetFullscreenState failed with error 0x%x.",
				hresult
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed 
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );
			
			goto Cleanup;
		};

		//
		// If m_bNonPrerotated == 0, that means the ChangeDisplaySettingsEx was called to rotate the display.
		// Seems pTargetOutput->GetDesc only returns modes set by ChangeDisplaySettingsEx not SetFullscreenState. 
		//
		if(m_bNonPrerotated)
		{
			//
			// Get a descriptor for the output to check if test is still in the mode requested.
			//
			DXGI_OUTPUT_DESC outputDesc;
			hresult = pTargetOutput->GetDesc( &outputDesc );

			if ( FAILED( hresult ) )
			{
				WriteToLog
				(
					"GetDesc failed with error 0x%x.",
					hresult
				);

				goto Cleanup;
			};


			UINT actualWidth = outputDesc.DesktopCoordinates.right - outputDesc.DesktopCoordinates.left;
			UINT actualHeight = outputDesc.DesktopCoordinates.bottom - outputDesc.DesktopCoordinates.top;
			UINT expectedWidth = m_SCDesc.BufferDesc.Width;
			UINT expectedHeight = m_SCDesc.BufferDesc.Height;

			if
			(
				actualWidth != m_SCDesc.BufferDesc.Width ||
				actualHeight != m_SCDesc.BufferDesc.Height ||
				m_RotationMode != outputDesc.Rotation 
			)
			{	   

				WriteToLog("Mismatched Mode After SetFullscreenState:");
				WriteToLog
				(
					"Actual Mode: %d x %d x Rotation = %d", 
					actualWidth,
					actualHeight,
					outputDesc.Rotation
				); 

				WriteToLog
				(
					"Expected Mode: %d x %d x Rotation = %d", 
					expectedWidth,
					expectedHeight,
					m_RotationMode
				); 

				goto Cleanup;
			} 
		}
	};


	//
	// Initialize the Direct3D device resources needed
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


	//
	// Keep a reference to the target output.
	//

	pDeviceWindowOutput = pTargetOutput;
	pTargetOutput = NULL;


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

#pragma prefast( suppress:  28922 )
	if ( pTargetOutput != NULL )
	{
		pTargetOutput->Release();
		pTargetOutput = NULL;
	};


	return test_result;
};


HRESULT CPresentFS::ResizeBuffers
(
	IDXGISwapChain * pSwapChain,
	DXGI_SWAP_CHAIN_DESC * pSwapChainDesc
)
{
	return S_OK;
};


void CPresentFS::CleanupTestCase()
{
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

};


TEST_RESULT CPresentFS::ExecuteTestCase()
{
	DebugOut.Write("Entering ExecuteTestCase.\n");
	TEST_RESULT test_result = RESULT_FAIL;
	TEST_RESULT CreateDeviceAndSwapChainResult = RESULT_FAIL;
	HRESULT hr = E_FAIL;
	BOOL found = FALSE;
	UINT texturesToLoad = m_TexturesToLoad;

	if(m_ParameterError == -1)
	{
		WriteToLog
		(
			"CPresentFS::ExecuteTestCase m_ParameterError = %d. No supported modes or formats were found",
			m_ParameterError
		); 

		test_result = RESULT_SKIP;
		goto Cleanup;

	}

	if(m_ModeInformation.isInitialized != TRUE )
	{
		WriteToLog
		(
			"CPresentFS::ExecuteTestCase Could not find Adapter-Output format parameter."
		);		

		test_result = RESULT_SKIP;
		goto Cleanup;        
	}


	//
	// Validate Flip Model (DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) compatible parameters
	//

	if (IsFlipModel(m_SwapEffect.dxgiSwapEffect))
	{
		if (m_BufferCount < 2)
		{
			WriteToLog("*** Flip model swapchains requires at least 2 buffers.  Skipping test case. ***");
			test_result = RESULT_SKIP;
			goto Cleanup;
		}

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
					WriteToLog("*** Flip model swapchains not supported with this pixel format.  Skipping test case. ***");                            
					test_result = RESULT_SKIP;
					goto Cleanup;
			} break; // End of DX10 or higher switch case block.

			default:
				// These are the only formats available on feature level 9.
				switch (m_ModeInformation.ModeDesc.Format)
				{
					case DXGI_FORMAT_B8G8R8X8_UNORM:
					case DXGI_FORMAT_B8G8R8A8_UNORM:
					case DXGI_FORMAT_R8G8B8A8_UNORM:
					break;

					default:
						WriteToLog("*** Flip model swapchains not supported with this pixel format.  Skipping test case. ***");
						test_result = RESULT_SKIP;
						goto Cleanup;
				}
		}
	} // End of Flip Model compatibility checks

	m_RotationMode = DisplayOrientationToDxgiModeRotation(m_ModeInformation.devmode.dmDisplayOrientation);


	//
	// Increment the current test case index.
	//

	++m_iCurrentTestCase;

	if(m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_TEST || m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_DO_NOT_SEQUENCE)
	{
		texturesToLoad += 1;
	}


	//
	// Check adapter sys/vid memory and skip cases that might fail due to too large memory usuage
	//
	
	// Backbuffers - in video memory
	SIZE_T backBufferDim = CFormatInfoMap::Find( m_ModeInformation.ModeDesc.Format )->uBitsPerElement;	// get bitsPerElement
	backBufferDim =	( backBufferDim >> 3 ) * m_ModeInformation.ModeDesc.Height * m_ModeInformation.ModeDesc.Width; // in bytes
	
	// In video memory: Backbuffers + textures bound as shader resources
	SIZE_T vidMemSize = backBufferDim * m_BufferCount;
	if( m_bNonPrerotated )
		vidMemSize *= 2;	// NonPrerotated swapchains induces memory cost of 1 extra back buffer
	vidMemSize *= ( m_ModeInformation.uiMultisampleCount + 1 );	// multisample swapchain backbuffers with size*sample count +1 regular swapchain backbuffer

	vidMemSize += backBufferDim * texturesToLoad;

	// Staging textures: loaded reference textures + validation textures for screen grab
	SIZE_T sysMemSize = backBufferDim * texturesToLoad;
	sysMemSize += backBufferDim * 2 ; // 1 each for regular and multisample  

	CDXGITestFramework * pFramework = dynamic_cast< CDXGITestFramework * >( GetFramework() );
	if( m_ModeInformation.uiDXGIAdapterOrdinal < m_vecAdapterDescriptors.size() )	
	{		
		pFramework->WriteToLogF
		(
			L"CPresentFS::ExecuteTestCase: Adapter %u : %s : vidMemReq=%Iu MB, sharedsysMemReq= %Iu MB, DedicatedVideoMemory= %Iu MB, DedicatedSystemMemory= %Iu MB, SharedSystemMemory= %Iu MB",
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
		WriteToLog
		(
			"CPresentFS::ExecuteTestCase: Could not find adapter description."
		);        

		test_result = RESULT_ABORT;
		goto Cleanup;
	}


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

		test_result = CreateDeviceAndSwapChainResult;
		goto Cleanup;
	}
	else if ( CreateDeviceAndSwapChainResult != RESULT_PASS )
	{
		test_result = CreateDeviceAndSwapChainResult;
		goto Cleanup;
	};


	//
	// Get a descriptor for our swap chain.
	//

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	hr = m_pSwapChain->GetDesc( &swapChainDesc );
	if ( FAILED( hr ) )
	{
		WriteToLog
		(
			"GetDesc on swap chain failed with error 0x%x.",
			hr
		);

		goto Cleanup;
	};


	//
	// Make our multisampled swap chain full-screen.
	//

	if ( m_ModeInformation.uiMultisampleCount > 1 )
	{
		hr = m_pSwapChain->SetFullscreenState
		(
			FALSE,
			NULL
		);

		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"SetFullscreenState failed with error 0x%x.",
				hr
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );

			goto Cleanup;
		};


		hr = m_pSwapChainMS->SetFullscreenState
		(
			TRUE,
			pDeviceWindowOutput
		);

		if
		(
			hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE ||
			hr == DXGI_STATUS_OCCLUDED
		)
		{
			WriteToLog
			(
				"Could not make swap chain full-screen.  This may indicate that another window has stolen focus from the test."
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );

			test_result = RESULT_SKIP;
			goto Cleanup;
		}
		else if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"SetFullscreenState on multisampled swap chain failed with error 0x%x.",
				hr
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed even though the swap chain window is in the foreground
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );

			goto Cleanup;
		};
	};

	if ( m_pSwapChain == NULL )
	{
		WriteToLog
		(
			"CPresentFS::ExecuteTestCase:  Swap chain is not initialized."
		);

		// Dump DXGI Journal to get clues as to why Swapchain is NULL
		if( g_Application.m_bLogDXGIJounrnal )
			g_Application.InitiateJournalDump( true );

		goto Cleanup;
	};

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
			"CPresentFS::ExecuteTestCase:  GetWindowInfo failed."
		);

		goto Cleanup;
	};


	if ( !UpdateOutputWindowInformation( m_pSwapChain, deviceWindowInfo ) )
	{
		WriteToLog
		(
			"UpdateOutputWindowInformation failed."
		);

		goto Cleanup;
	};


	//
	// Create our textures.
	//

	HRESULT hrCreateTexturesResult = CreateTextures
	(
		swapChainDesc.BufferDesc.Format,
		rectDeviceWindow.right - rectDeviceWindow.left,
		rectDeviceWindow.bottom - rectDeviceWindow.top,
		uiDeviceOutputWidth,
		uiDeviceOutputHeight,
		texturesToLoad
	);

	if( FAILED( hrCreateTexturesResult ) )
	{
		WriteToLog
		(
			"CreateTextures failed."
		);

		if ( m_bLargeMemoryUsage && ( hrCreateTexturesResult == E_OUTOFMEMORY ) )
		{
			test_result = RESULT_SKIP;
			WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Resource creation failure expected. Skipping test case. " );
		}

		goto Cleanup;
	};


	DXGI_SWAP_CHAIN_DESC swapChainDescMS;

	if ( m_ModeInformation.uiMultisampleCount > 1 )
	{
		UpdateOutputWindowInformation( m_pSwapChainMS, deviceWindowInfo );

		hr = m_pSwapChainMS->GetDesc( &swapChainDescMS );
		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"GetDesc on multisampled swap chain failed with error 0x%x.",
				hr
			);

			goto Cleanup;
		};


		if ( m_bNonPrerotated )
		{
			swapChainDescMS.Flags |= DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;
		}
		else
		{
			swapChainDescMS.Flags &= ~DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;
		};


		hr = ResizeBuffers
		(
			m_pSwapChainMS,
			&swapChainDescMS
		);

		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"ResizeBuffer failed with error 0x%x.",
				hr
			);

			if( hr == E_OUTOFMEMORY  && m_bLargeMemoryUsage )
			{
				test_result = RESULT_SKIP;
				WriteToLog( "ExecuteTestCase: Test case using large amount of memory. ResizeBuffers failure expected. Skipping test case. " );
			}

			goto Cleanup;
		};


		bool bClearResult = TestedSwapChainMS.InitBBViaClear( 0 );
		if ( bClearResult != true )
		{
			WriteToLog
			(
				"Clear on multisampled swap chain failed."
			);

			goto Cleanup;
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

			if( hrInitializeBackBufferResult == E_OUTOFMEMORY && m_bLargeMemoryUsage)
			{
				test_result = RESULT_SKIP;
				WriteToLog( "ExecuteTestCase: Test case using large amount of memory. InitializeBackBuffer failure expected. Skipping test case. " );
			}

			goto Cleanup;
		};

		bool bPerformPresentResult = PerformPresent
		(
			m_pSwapChainMS,
			0,
			0
		);

		if
		(
			g_Application.GetDebugPause() > 0
		)
		{
			Sleep( g_Application.GetDebugPause() );
		};


		if ( bPerformPresentResult != true )
		{
			WriteToLog
			(
				"PerformPresent on multisampled swap chain failed."
			);

			goto Cleanup;
		};

		bool bGetDisplaySurfaceDataResult = GetDisplaySurfaceData
		(
			m_pGetDSDSurfaceMS,
			true,
			false
		);

		if ( bGetDisplaySurfaceDataResult != true )
		{
			WriteToLog
			(
				"GetDisplaySurfaceData on multisampled swap chain failed."
			);

			goto Cleanup;
		};


		hr = m_pSwapChainMS->SetFullscreenState
		(
			FALSE,
			NULL
		);

		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"SetFullscreenState while returning multisampled swap chain to windowed mode failed with error 0x%x.",
				hr
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );

			goto Cleanup;
		};


		//
		// Re-assert the mode.
		//

		hr = SetDisplayMode( pDeviceWindowOutput );
		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"SetDisplayMode  failed with error 0x%x.",
				hr
			);

			goto Cleanup;
		};


		//
		// Restore the original swap chain to full-screen.
		//

		hr = m_pSwapChain->SetFullscreenState
		(
			TRUE,
			pDeviceWindowOutput
		);

		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"SetFullscreenState while returning swap chain to full-screen failed with error 0x%x.",
				hr
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );

			goto Cleanup;
		};

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
				"CPresentFS::ExecuteTestCase:  GetWindowInfo failed."
			);

			goto Cleanup;
		};


		UpdateOutputWindowInformation( m_pSwapChain, deviceWindowInfo );
	};


	if ( m_bNonPrerotated )
	{
		swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;
	}
	else
	{
		swapChainDesc.Flags &= ~DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;
	};


	hr = ResizeBuffers
	(
		m_pSwapChain,
		&swapChainDesc
	);

	if ( FAILED( hr ) )
	{
		WriteToLog
		(
			"ResizeBuffers failed with error 0x%x.",
			hr
		);

		if( hr == E_OUTOFMEMORY  && m_bLargeMemoryUsage )
		{
			test_result = RESULT_SKIP;
			WriteToLog( "ExecuteTestCase: Test case using large amount of memory. ResizeBuffers failure expected. Skipping test case. " );
		}

		goto Cleanup;
	};


	/*bool bClearResult = TestedSwapChain.InitBBViaClear( 0 );
	if ( bClearResult != true )
	{
		WriteToLog
		(
			"Clear on swap chain back buffer failed."
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

				goto Cleanup;
			};

			// Must Present() to rotate to the next backbuffer
			if( !PerformPresent( m_pSwapChain, 0, 0 ) )
			{
				WriteToLog
				(
					"PerformPresent on swap chain failed."
				);

				goto Cleanup;
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

			if ( FAILED(hrInitializeBackBufferResult) )
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

				goto Cleanup;
			};
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

			goto Cleanup;
		};


		if
		(
			g_Application.GetDebugPause() > 0
		)
		{
			Sleep( g_Application.GetDebugPause() );
		};
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
			m_NumberOfPresents % texturesToLoad
		);

		if ( FAILED(hrInitializeBackBufferResult) )
		{
			WriteToLog
			(
				"InitializeBackBuffer failed."
			);

			goto Cleanup;
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

			goto Cleanup;
		};

		if
		(
			g_Application.GetDebugPause() > 0
		)
		{
			Sleep( g_Application.GetDebugPause() );
		};
	}

	bool bGetDisplaySurfaceDataResult = GetDisplaySurfaceData
	(
		m_pGetDSDSurface,
		false,
		false
	);

	if ( bGetDisplaySurfaceDataResult != true )
	{
		WriteToLog
		(
			"GetDisplaySurfaceData on swap chain failed."
		);

		goto Cleanup;
	};


	//
	// For sRGB and fp16  backbuffers, we need to adjust the initial texture.
	//

	if ( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB == swapChainDesc.BufferDesc.Format || 
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB == swapChainDesc.BufferDesc.Format ||
		DXGI_FORMAT_R16G16B16A16_FLOAT == swapChainDesc.BufferDesc.Format )   
	{
		hr = LinearToGamma(pBBInitTextureSys[(m_NumberOfPresents -1) % texturesToLoad]);
		if ( FAILED(hr))
		{
			WriteToLog(_T("LinearToGamma failed. 0x%x"), hr);
			goto Cleanup;
		};

	}

	if 
	(
		(
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB == swapChainDesc.BufferDesc.Format || 
			DXGI_FORMAT_R8G8B8A8_UNORM == swapChainDesc.BufferDesc.Format
		) && 
		m_ModeInformation.uiMultisampleCount > 1 &&
		g_Application.m_D3DOptions.SrcDriverType == D3D10_DRIVER_TYPE_SOFTWARE
	)
	{
		hr = MaskAlphaChannel( m_pGetDSDTexture );
		if ( FAILED(hr))
		{
			WriteToLog(_T("MaskAlphaChannel of Test Texture failed. 0x%x"), hr);
			goto Cleanup;
		};

		hr = MaskAlphaChannel( m_pGetDSDTextureMS );
		if ( FAILED(hr))
		{
			WriteToLog(_T("MaskAlphaChannel of Ref Texture failed. 0x%x"), hr);
			goto Cleanup;
		};
	}


	float fNonConformanceFraction = 0.f;
	bool bEvaluateImageResult = EvaluatePresentedImage
	(
		m_pGetDSDTexture, 
		m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : pBBInitTextureSys[(m_NumberOfPresents -1) % texturesToLoad],
		&fNonConformanceFraction
	);

	if ( bEvaluateImageResult != true )
	{
		WriteToLog
		(
			"EvaluatePresentedImage failed."
		);

		goto Cleanup;
	};


	g_Application.SetLastTestConformance( 1.f - fNonConformanceFraction );
	if ( fNonConformanceFraction > m_fMaxNonConformanceFractionFS)
	{
		WriteToLog
		(
			"FAIL:  Actual conformance of %3.3f%% does not meet expected threshold of %3.3f%%.",
			100.f * ( 1.f - fNonConformanceFraction ),
			100.f * ( 1.f - m_fMaxNonConformanceFractionFS )
		);

		test_result = RESULT_FAIL;


		//
		// Save the result as a DDS file.
		//

		bool bSaveDDS = g_Application.IsSaveDDS();
		if ( bSaveDDS == true )
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
				(ID3DResource *) ( m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : pBBInitTextureSys[(m_NumberOfPresents -1) % texturesToLoad] ),
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
		};


		//
		// Save the result as a bitmap.
		//
		bool bSaveBMP = g_Application.IsSaveBMP();
		if ( bSaveBMP == true )
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
				(ID3DResource *) ( m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : pBBInitTextureSys[((m_NumberOfPresents -1) % texturesToLoad)] ),
				D3DX10_IFF_BMP,
				DumpFileName
			);

			if ( FAILED( hSaveFileResult ) )
			{
				WriteToLog
				(
					"D3DX10SaveTextureToFile saving %s image to bitmap failed with error 0x%x.",
					m_ModeInformation.uiMultisampleCount <= 1 ? "reference" : "test",
					hSaveFileResult
				);


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
					(ID3DResource *) ( m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : pBBInitTextureSys[((m_NumberOfPresents -1) % texturesToLoad)] ),
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
		};
	}
	else
	{
		test_result = RESULT_PASS;
	};


Cleanup:
	ReleaseTestCaseObjects();


	if ( m_DevicePointer.pDevice != NULL )
	{
		// @DX11PORT [SEdmison]:
		// Replace this.
		//D3DX10UnsetAllDeviceObjects( m_DevicePointer.pDevice );

		m_DevicePointer.pDevice->Release();
		m_DevicePointer.pDevice = NULL;
	};

	PrimaryAdapterDevice.ReleaseAllObjects();


	DebugOut.Write("Exiting ExecuteTestCase.\n");

	return test_result;
};


CPresentFSMS::CPresentFSMS()
:
m_uMaxQuality( 32 )
{
	m_MultisampleTest = TRUE;
	m_bIgnoreRefresh = true;
	m_bAlwaysFilterLowResolution = true;
	m_fMaxNonConformanceFractionFS = 0.2f;
	PrimaryAdapterDevice.SetAlternateRenderPath( true );
};


HRESULT CPresentFSMS::ModeListTransform
(
	D3D_DEVICE_POINTER devicePointer,
	__in std::vector< ModeInformation >* pModes,
	__out std::vector< ModeInformation >* pTransformedModes
)
{
	HRESULT hr = GetMultisampleQualityLevels
	(
		devicePointer,
		pModes,
		pTransformedModes,
		D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT,
		g_Application.IsCapMultiSampleQuality()
	);

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
		WriteToLog(_T("CPresentFSMS::ModeListTransform returned 0 modes.  Is Multisampling supported?"));
	}

	return hr;
}


void CPresentFSMS::InitFSMSTestParameters()
{
	//
	// This test group doesn't run under TS.
	//

	if
	(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		goto Cleanup;
	};


	//
	// Call the base class imlementation.
	//
	CPresentFS::InitFSTestParameters();

Cleanup:;
};


void CPresentFSMS::InitTestParameters()
{
	CPresentFSMS::InitFSMSTestParameters();

	RFactor testRFactors = IntitTestRFactors();


	if(m_globalRFactor.Valid() && m_FeatureLevelFactor.Valid())
	{
		//
		// If derived test sets some rfactors use them. If there is an error or they dont exist dont use the.
		//
		if(testRFactors.Valid())
		{
			SetRootTestFactor(m_FeatureLevelFactor * testRFactors * m_globalRFactor );
		}
		else
		{
			SetRootTestFactor(m_FeatureLevelFactor * m_globalRFactor );    
		}
	}
	else
	{
		if( m_FeatureLevelFactor.Valid() )
			SetRootTestFactor( m_FeatureLevelFactor );

		WriteToLog("CPresentFSMS::SetRootTestFactor parameter combination is invalid. m_globalRFactor.Valid=%d m_FeatureLevelFactor.Valid=%d", m_globalRFactor.Valid(), m_FeatureLevelFactor.Valid());
	}
};


TEST_RESULT CPresentFSMS::ExecuteTestCase()
{
	TEST_RESULT retVal = RESULT_PASS;

	retVal = CPresentFS::ExecuteTestCase();

	return retVal;
}


RFactor CPresentFSMS::IntitTestRFactors()
{
	RFactor bufferCount =  AddParameter
	(
		_T("BufferCount"),
		&m_BufferCount,
		RangeValueSet
		(
			(UINT)m_MaxBufferCount,
			(UINT)m_MaxBufferCount,
			(UINT)1
		)
	);

	CUserValueSet< DXGIPresentFlagRecord > *pPresentFlags = new CUserValueSet< DXGIPresentFlagRecord >();

	pPresentFlags->AddValue(DXGIPresentFlags[0]);

	RFactor presentFlags = AddParameter< DXGIPresentFlagRecord >( _T("PresentFlags") , &m_PresentFlag, pPresentFlags);


	CUserValueSet< DXGISwapEffectRecord > *pSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();

	pSwapEffect->AddValue(DXGISwapEffects[0]);

	RFactor swapEffect =  AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

	return bufferCount * presentFlags * swapEffect;
}


CPresentFSRotation::CPresentFSRotation()
{
	m_bIgnoreRefresh = true;
	m_bAlwaysFilterLowResolution = true;
	m_bTestRotatedModes = true;
	m_bUseMinimalFormatList = true;
};


void CPresentFSRotation::InitTestParameters()
{
	//
	// This test group doesn't run under TS.
	//

	if
	(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		return;
	};


	//
	// Call the base class implementation.
	//

	CPresentFS::InitFSTestParameters();

	RFactor testRFactors = IntitTestRFactors();

	RFactor nonPrerotated = AddParameter( _T( "NonPrerotated" ), (int*)(&m_bNonPrerotated), RangeValueSet( 0, 1, 1 ) );

	if(m_globalRFactor.Valid() && m_FeatureLevelFactor.Valid())
	{
		//
		// If derived test sets some rfactors use them. If there is an error or they dont exist dont use the.
		//

		if(testRFactors.Valid())
		{
			SetRootTestFactor(m_FeatureLevelFactor * nonPrerotated * testRFactors * m_globalRFactor );
		}
		else
		{
			SetRootTestFactor(m_FeatureLevelFactor * nonPrerotated * m_globalRFactor );
		}
	}
	else
	{
		if( m_FeatureLevelFactor.Valid() )
			SetRootTestFactor( m_FeatureLevelFactor );

		WriteToLog("CPresentFSRotation::SetRootTestFactor parameter combination is invalid. m_globalRFactor.Valid=%d m_FeatureLevelFactor.Valid=%d", m_globalRFactor.Valid(), m_FeatureLevelFactor.Valid());
	}
};


RFactor CPresentFSRotation::IntitTestRFactors()
{
	RFactor bufferCount =  AddParameter
	(
		_T("BufferCount"),
		&m_BufferCount,
		RangeValueSet
		(
			(UINT)m_MaxBufferCount,
			(UINT)m_MaxBufferCount,
			(UINT)1
		)
	);

	CUserValueSet< DXGIPresentFlagRecord > *pPresentFlags = new CUserValueSet< DXGIPresentFlagRecord >();


	pPresentFlags->AddValue(DXGIPresentFlags[0]);

	RFactor presentFlags = AddParameter< DXGIPresentFlagRecord >( _T("PresentFlags") , &m_PresentFlag, pPresentFlags);

	CUserValueSet< DXGISwapEffectRecord > *pSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();

	pSwapEffect->AddValue(DXGISwapEffects[0]);

	RFactor swapEffect =  AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

	return bufferCount * presentFlags * swapEffect;
}


CPresentFSMulti::CPresentFSMulti()
{
	//PrimaryAdapterDevice.SetAlternateRenderPath( false );
	m_bTestRotatedModes = true;
	m_bOneModeOnly = true;
	m_bUseMinimalFormatList = true;
	// this is not the main test for quality levels, so limit it
	m_uMaxQuality = 2;
};


void CPresentFSMulti::InitTestParameters()
{
	//
	// This test group doesn't run under TS.
	//

	if
	(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		return;
	};


	//
	// Call the base class implementation.
	//

	CPresentFSMS::InitFSMSTestParameters();

	RFactor scaling = AddParameter( _T( "Scaling" ), &m_Scaling, RangeValueSet( 75, 100, 125 ) );
	RFactor scalingMS = AddParameter( _T( "ScalingMS" ), &m_ScalingMS, RangeValueSet( 75, 100, 125 ) );
	RFactor nonPrerotated = AddParameter( _T( "NonPrerotated" ), (int*)(&m_bNonPrerotated), RangeValueSet( 0, 1, 1 ) );


	RFactor testRFactors= IntitTestRFactors();

	if( m_globalRFactor.Valid() && m_FeatureLevelFactor.Valid() )
	{
		//
		// If derived test sets some rfactors use them. If there is an error or they dont exist dont use the.
		//
		if(testRFactors.Valid())
		{
			SetRootTestFactor(m_FeatureLevelFactor * testRFactors * scaling * scalingMS * nonPrerotated * m_globalRFactor );
		}
		else
		{
			SetRootTestFactor(m_FeatureLevelFactor * scaling * scalingMS * nonPrerotated * m_globalRFactor );    
		}
	}
	else
	{
		//
		// Error no modes set.  Execute test case will pick this up
		//
		if( m_FeatureLevelFactor.Valid() )
			SetRootTestFactor( m_FeatureLevelFactor );		        

		WriteToLog("CPresentFSMulti::SetRootTestFactor parameter combination is invalid. m_globalRFactor.Valid=%d m_FeatureLevelFactor.Valid=%d", m_globalRFactor.Valid(), m_FeatureLevelFactor.Valid());
	}
};


HRESULT CPresentFSMulti::ResizeBuffers
(
	IDXGISwapChain * pSwapChain,
	DXGI_SWAP_CHAIN_DESC * pSwapChainDesc
)
{
	int Scaling = pSwapChainDesc->SampleDesc.Count > 1? m_ScalingMS : m_Scaling;

	return pSwapChain->ResizeBuffers
	(
		pSwapChainDesc->BufferCount,
		pSwapChainDesc->BufferDesc.Width * Scaling / 100,
		pSwapChainDesc->BufferDesc.Height * Scaling / 100,
		pSwapChainDesc->BufferDesc.Format,
		pSwapChainDesc->Flags
	);
};


RFactor CPresentFS::IntitTestRFactors()
{
	RFactor bufferCount =  AddParameter
	(
		_T("BufferCount"),
		&m_BufferCount,
		RangeValueSet
		(
			(UINT)m_MaxBufferCount,
			(UINT)m_MaxBufferCount,
			(UINT)1
		)
	);

	CUserValueSet< DXGIPresentFlagRecord > *pPresentFlags = new CUserValueSet< DXGIPresentFlagRecord >();

	pPresentFlags->AddValue(DXGIPresentFlags[0]);

	RFactor presentFlags = AddParameter< DXGIPresentFlagRecord >( _T("PresentFlags") , &m_PresentFlag, pPresentFlags);


	CUserValueSet< DXGISwapEffectRecord > *pSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
	for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
	{
		pSwapEffect->AddValue(DXGISwapEffects[i]);
	}

	RFactor swapEffect =  AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

	return bufferCount * presentFlags * swapEffect;
}
