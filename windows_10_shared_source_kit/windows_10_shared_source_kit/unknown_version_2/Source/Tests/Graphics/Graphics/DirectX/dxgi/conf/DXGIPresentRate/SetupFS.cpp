//
// Project headers
//

#include "DXGIPresentRate.h"
#include "TestApp.h"


CFSTestCases::CFSTestCases()
:
	m_iTotalModes( 0 ),
	m_bDWMState( FALSE ),
	m_bIgnoreRefresh( FALSE ),
	m_hEnumerateModesResult( E_FAIL )
{
};


CFSTestCases::~CFSTestCases()
{
};


TEST_RESULT CFSTestCases::Setup()
{
	//
	// Initialize locals.
	//

	TEST_RESULT SetupResult = RESULT_PASS;


	//
	// Determine whether DWM is on.
	//

	DwmIsCompositionEnabled( &m_bDWMState );


	//
	// Determine whether we enumerated modes successfully.
	//

	switch ( m_hEnumerateModesResult )
	{
		case S_OK:
		{
			SetupResult = RESULT_PASS;
		} break;


		case S_FALSE:
		{
			WriteToLog
			(
				_T( "No adapters or no modes found to test." )
			);

			SetupResult = RESULT_SKIP;
		} break;


		default:
		{
			WriteToLog
			(
				_T( "CFSTestCases::SetupTest:  EnumerateModes failed with error 0x%x." ),
				m_hEnumerateModesResult
			);

			SetupResult = RESULT_ABORT;
		} break;
	};

	return SetupResult;
};


void CFSTestCases::Cleanup()
{

};


bool CFSTestCases::EnumerateAdapterModes
(
	IDXGIAdapter * pDXGIAdapterEnumMode,
	UINT uiAdapterOrdinal
)
{
	//
	// Initialize locals.
	//

	bool bOverallResult = false;
	HRESULT hresult = E_FAIL;
	IDXGIOutput * pDXGIOutputEnumMode = NULL;
	DXGI_MODE_DESC * pModeDescArray = NULL;


	//
	// Enumerate the adapter's outputs.
	//

	for
	(
		UINT uiOutputOrdinal = 0;
		uiOutputOrdinal < 10;
		++uiOutputOrdinal
	)
	{
		//
		// Free the output from the prior iteration.
		//

		if ( pDXGIOutputEnumMode != NULL )
		{
			pDXGIOutputEnumMode->Release();
			pDXGIOutputEnumMode = NULL;
		};


		//
		// Enumerate the output.
		//

		hresult = pDXGIAdapterEnumMode->EnumOutputs
		(
			uiOutputOrdinal,
			&pDXGIOutputEnumMode
		);

		if ( hresult == DXGI_ERROR_NOT_FOUND )
		{
			break;
		}
		else if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CFSTestCases::EnumerateAdapterModes:  EnumOutputs on output %u failed with error 0x%x.",
				uiOutputOrdinal,
				hresult
			);

			goto Cleanup;
		};

		if ( pDXGIOutputEnumMode == NULL )
		{
			WriteToLog
			(
				"CFSTestCases::EnumerateAdapterModes:  EnumOutputs on output %u succeeded but returned a NULL output pointer.",
				uiOutputOrdinal
			);

			goto Cleanup;
		};


		//
		// Get a descriptor for the output.
		//

		DXGI_OUTPUT_DESC outputDesc;
		hresult = pDXGIOutputEnumMode->GetDesc( &outputDesc );

		if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CFSTestCases::EnumerateAdapterModes:  GetDesc on output %u failed with error 0x%x.",
				uiOutputOrdinal,
				hresult
			);

			goto Cleanup;
		};


		//
		// Determine the output's current mode.
		//

		DEVMODEW Mode;
		ZeroMemory( &Mode, sizeof( DEVMODEW ) );
		Mode.dmSize = sizeof( DEVMODEW );
		Mode.dmSpecVersion = DM_SPECVERSION;
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
				"CFSTestCases::EnumerateAdapterModes:  EnumDisplaySettings failed with error %u.",
				GetCurrentModeError
			);

			goto Cleanup;
		};
				

		std::map< HMONITOR, DEVMODEW >::const_iterator itMode = m_DesktopMode.find( outputDesc.Monitor );
		if ( m_DesktopMode.end() == itMode )
		{
			m_DesktopMode[ outputDesc.Monitor ] = Mode;
		};


		for
		(
			UINT uiFormatIndex = 0;
			uiFormatIndex < clsScanOutFormats::cuiNoOfScanOutFormats;
			++uiFormatIndex
		)
		{
			DXGI_FORMAT MatchFormat = clsScanOutFormats::DXGIScanOutFormats[ uiFormatIndex ];
			UINT uiModeCount;

			std::vector< DXGI_MODE_DESC > vecResultModes;


			//
			// Determine the number of modes for this format.
			//

			hresult = pDXGIOutputEnumMode->GetDisplayModeList
			(
				MatchFormat,
				DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
				&uiModeCount,
				NULL
			);

			if ( FAILED( hresult ) )
			{
				WriteToLog
				(
					"CFSTestCases::EnumerateAdapterModes:  GetDisplayModeList failed with error 0x%x.",
					hresult
				);

				goto Cleanup;
			};


			if ( uiModeCount == 0 )
			{
				DebugOut.Write( "Found zero modes for this format.\n" );

				continue;
			};


			//
			// Allocate an array to hold the mode list.
			//

			pModeDescArray = new DXGI_MODE_DESC[ uiModeCount ];

			if ( pModeDescArray == NULL )
			{
				WriteToLog
				(
					"CFSTestCases::EnumerateAdapterModes:  Failed to allocate mode descriptor array."
				);

				goto Cleanup;
			};


			//
			// Get the mode list for this format.
			//

			hresult = pDXGIOutputEnumMode->GetDisplayModeList
			(
				MatchFormat,
				DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
				&uiModeCount,
				pModeDescArray
			);

			if ( FAILED( hresult ) )
			{
				WriteToLog
				(
					"CFSTestCases::EnumerateAdapterModes:  GetDisplayModeList failed with error 0x%x.",
					hresult
				);

				goto Cleanup;
			};


			//
			// Insert the modes into our list of modes to test.
			//

			vecResultModes.insert
			(
				vecResultModes.begin(),
				pModeDescArray,
				pModeDescArray + uiModeCount
			);


			//
			// Remove low- and high-res modes.
			//

			for
			(
				UINT uMode = 0;
				uMode < vecResultModes.size();
				++uMode
			)
			{
				DXGI_MODE_DESC dxgiMode = vecResultModes[ uMode ];

				// 800 x 600 and 1024 x 768 should suffice
				if
				(
					( dxgiMode.Width != 800 || dxgiMode.Height != 600 ) &&
					( dxgiMode.Width != 600 || dxgiMode.Height != 800 ) &&
					( dxgiMode.Width != 1024 || dxgiMode.Height != 768 ) &&
					( dxgiMode.Width != 768 || dxgiMode.Height != 1024 )
				)
				{
					vecResultModes.erase( vecResultModes.begin() + uMode-- );
				};
			};


			//
			// If no modes are found, then just select the first mode in the list.
			//

			if ( vecResultModes.empty() )
			{
				vecResultModes.insert
				(
					vecResultModes.begin(),
					pModeDescArray,
					pModeDescArray + 1
				);
			};


			//
			// Remove modes that differ only on refresh rate.
			//

			if ( m_bIgnoreRefresh )
			{
				for
				(
					UINT uMode = 0;
					uMode < vecResultModes.size();
					++uMode
				)
				{
					DXGI_MODE_DESC dxgiMode = vecResultModes[ uMode ];

					// does the mode already exist?
					for
					(
						UINT uExistingMode = 0;
						uExistingMode < vecResultModes.size();
						++uExistingMode
					)
					{
						if ( uMode == uExistingMode )
						{
							continue;
						};

						DXGI_MODE_DESC dxgiExistingMode = vecResultModes[ uExistingMode ];

						if
						(
							dxgiMode.Width == dxgiExistingMode.Width &&
							dxgiMode.Height == dxgiExistingMode.Height &&
							dxgiMode.Format == dxgiExistingMode.Format &&
							dxgiMode.ScanlineOrdering == dxgiExistingMode.ScanlineOrdering &&
							dxgiMode.Scaling == dxgiExistingMode.Scaling
						)
						{
							vecResultModes.erase( vecResultModes.begin() + uExistingMode-- );
							// don't break; we want to eliminate ALL duplicate refresh rates
							// break;
						};
					};
				};
			};


			delete [] pModeDescArray;
			pModeDescArray = NULL;


			UINT uiModeIndex = 0;
			UINT uiModesRemaining = vecResultModes.size();

			if ( uiModesRemaining > 0 )
			{
				do
				{
					FullscreenTestMode ModeInit;

					ModeInit.uiDXGIAdapterOrdinal = uiAdapterOrdinal;
					ModeInit.uiDXGIOutputOrdinal = uiOutputOrdinal;
					ModeInit.ModeDesc = vecResultModes[uiModeIndex++];
					m_TestedModes[ m_iTotalModes ] = ModeInit;
					m_iTotalModes++;
				}
				while ( --uiModesRemaining );
			}
		};


		pDXGIOutputEnumMode->Release();
		pDXGIOutputEnumMode = NULL;
	};


	bOverallResult = true;


Cleanup:
	if ( pModeDescArray != NULL )
	{
		delete [] pModeDescArray;
		pModeDescArray = NULL;
	};

	if ( pDXGIOutputEnumMode != NULL )
	{
		pDXGIOutputEnumMode->Release();
		pDXGIOutputEnumMode = NULL;
	};


	return bOverallResult;
};


HRESULT CFSTestCases::EnumerateModes()
{
	//
	// Initialize locals.
	//

	HRESULT hresult = E_FAIL;

	DXGI_FACTORY_POINTER FactoryPointer;
	ZeroMemory( &FactoryPointer, sizeof( FactoryPointer ) );

	DXGI_ADAPTER_POINTER AdapterPointer;
	ZeroMemory( &AdapterPointer, sizeof( AdapterPointer ) );

	D3D_DEVICE_POINTER DevicePointer;
	ZeroMemory( &DevicePointer, sizeof( DevicePointer ) );

	m_iTotalModes = 0;


	//
	// Enumerate outputs and modes for all adapters.
	//

	for
	(
		UINT uiAdapterOrdinal = 0;
		uiAdapterOrdinal < 10;
		++uiAdapterOrdinal
	)
	{
		//
		// Release the device from the prior iteration.
		//

		if ( DevicePointer.pDevice != NULL )
		{
			DevicePointer.pDevice->Release();
			DevicePointer.pDevice = NULL;
		};


		//
		// Release the adapter from the prior iteration.
		//

		if ( AdapterPointer.pAdapter != NULL )
		{
			AdapterPointer.pAdapter->Release();
			AdapterPointer.pAdapter = NULL;
		};


		//
		// Release the factory from the prior iteration.
		//

		if ( FactoryPointer.pFactory != NULL )
		{
			FactoryPointer.pFactory->Release();
			FactoryPointer.pFactory = NULL;
		};


		//
		// Create our adapter and device.
		//

	    hresult = g_Application.CreateFactoryAdapterDevice
	    (
		    &FactoryPointer,
		    uiAdapterOrdinal,
		    &AdapterPointer,
		    g_Application.m_D3DOptions.SrcDriverType,
		    D3D10_CREATE_DEVICE_SINGLETHREADED,
		    TEST_FEATURE_10_ALL_LEVEL_BESTAVAILABLE,
		    0,
		    &DevicePointer
	    );

		if ( hresult == DXGI_ERROR_NOT_FOUND )
		{
			if ( uiAdapterOrdinal == 0 )
			{
				hresult = S_FALSE;
			};

			break;
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

			hresult = S_FALSE;
			goto Cleanup;
		}
		else if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CFSTestCases::EnumerateModes:  CreateFactoryAdapterDevice failed with error 0x%x.",
				hresult
			);

			goto Cleanup;
		};


        if ( AdapterPointer.pAdapter == NULL )
        {
            WriteToLog
                (
                "CFSTestCases::EnumerateModes:  CreateFactoryAdapterDevice succeeded but returned NULL adapter pointer."
                );

            goto Cleanup;
        };

        DXGI_ADAPTER_DESC AdapterDescriptor;
        if ( AdapterPointer.pAdapter->GetDesc( &AdapterDescriptor ) == S_OK)
            m_vecAdapterDescriptors.push_back( AdapterDescriptor );


		//
		// Enumerate outputs and modes for this adapter.
		//

		bool bEnumerateAdapterModesResult = EnumerateAdapterModes
		(
			AdapterPointer.pAdapter,
			uiAdapterOrdinal
		);

		if ( bEnumerateAdapterModesResult != true )
		{
			WriteToLog
			(
				"CFSTestCases::EnumerateModes:  EnumerateAdapterModes failed."
			);

			goto Cleanup;
		};
	};


	hresult = S_OK;


Cleanup:
	if ( DevicePointer.pDevice != NULL )
	{
		DevicePointer.pDevice->Release();
		DevicePointer.pDevice = NULL;
	};


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

RFactor CFSTestCases::InitModeParameters()
{
    RFactor format, width, height, refreshrate[2], scanlineordering, scaling, adapter, output;

    format = AddParameter( 
        _T( "ModeDesc.Format" ), 
        &m_FSModeInTest.ModeDesc.Format, 
        new CTableValueSet< DXGI_FORMAT >( 
        &m_TestedModes[0].ModeDesc.Format ,
        sizeof( m_TestedModes[0] ),
        m_iTotalModes ) );

    width = AddParameter( 
        _T( "ModeDesc.Width" ), 
        &m_FSModeInTest.ModeDesc.Width, 
        new CTableValueSet< UINT >( 
        &m_TestedModes[0].ModeDesc.Width ,
        sizeof( m_TestedModes[0] ),
        m_iTotalModes ) );

    height = AddParameter( 
        _T( "ModeDesc.Height" ), 
        &m_FSModeInTest.ModeDesc.Height, 
        new CTableValueSet< UINT >( 
        &m_TestedModes[0].ModeDesc.Height ,
        sizeof( m_TestedModes[0] ),
        m_iTotalModes ) );

    refreshrate[0] = AddParameter( 
        _T( "ModeDesc.RefreshRate.Numerator" ), 
        &m_FSModeInTest.ModeDesc.RefreshRate.Numerator, 
        new CTableValueSet< UINT >( 
        &m_TestedModes[0].ModeDesc.RefreshRate.Numerator ,
        sizeof( m_TestedModes[0] ),
        m_iTotalModes ) );

    refreshrate[1] = AddParameter( 
        _T( "ModeDesc.RefreshRate.Denominator" ), 
        &m_FSModeInTest.ModeDesc.RefreshRate.Denominator, 
        new CTableValueSet< UINT >( 
        &m_TestedModes[0].ModeDesc.RefreshRate.Denominator ,
        sizeof( m_TestedModes[0] ),
        m_iTotalModes ) );

    scanlineordering = AddParameter( 
        _T( "ModeDesc.ScanlineOrdering" ), 
        &m_FSModeInTest.ModeDesc.ScanlineOrdering, 
        new CTableValueSet< DXGI_MODE_SCANLINE_ORDER >( 
        &m_TestedModes[0].ModeDesc.ScanlineOrdering ,
        sizeof( m_TestedModes[0] ),
        m_iTotalModes ) );

    scaling = AddParameter( 
        _T( "ModeDesc.Scaling" ), 
        &m_FSModeInTest.ModeDesc.Scaling, 
        new CTableValueSet< DXGI_MODE_SCALING >( 
        &m_TestedModes[0].ModeDesc.Scaling ,
        sizeof( m_TestedModes[0] ),
        m_iTotalModes ) );

    adapter = AddParameter( 
        _T( "DXGIAdapterOrdinal" ), 
        &m_FSModeInTest.uiDXGIAdapterOrdinal, 
        new CTableValueSet< UINT >( 
        &m_TestedModes[0].uiDXGIAdapterOrdinal,
        sizeof( m_TestedModes[0] ),
        m_iTotalModes ) );

    output = AddParameter( 
        _T( "DXGIOutputOrdinal" ), 
        &m_FSModeInTest.uiDXGIOutputOrdinal, 
        new CTableValueSet< UINT >( 
        &m_TestedModes[0].uiDXGIOutputOrdinal,
        sizeof( m_TestedModes[0] ),
        m_iTotalModes ) );


    if( m_vecAdapterDescriptors.size() < 2)
        AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 0 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 0 )), WeightFactorLarge);

    AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 1 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 0 )), WeightFactorLarge);
    AddPriorityPatternFilter((FilterEqual< UINT >(_T("DXGIAdapterOrdinal"), 0 ) && FilterEqual< UINT >(_T("DXGIOutputOrdinal"), 1 )), WeightFactorLarge);
    AddPriorityPatternFilter(FilterEqual< DXGI_FORMAT >(_T("ModeDesc.Format"), DXGI_FORMAT_R8G8B8A8_UNORM ), WeightFactorLarge);
    AddPriorityPatternFilter(FilterEqual< DXGI_FORMAT >(_T("ModeDesc.Format"), DXGI_FORMAT_B8G8R8A8_UNORM ), WeightFactorLarge);
    AddPriorityPatternFilter(FilterEqual< DXGI_FORMAT >(_T("ModeDesc.Format"), DXGI_FORMAT_R16G16B16A16_FLOAT ), WeightFactorSmall);
    AddPriorityPatternFilter(FilterEqual< DXGI_MODE_SCANLINE_ORDER >(_T("ModeDesc.ScanlineOrdering"), DXGI_MODE_SCANLINE_ORDER_PROGRESSIVE), WeightFactorLarge);
    AddPriorityPatternFilter(FilterEqual< DXGI_MODE_SCALING >(_T("ModeDesc.Scaling"), DXGI_MODE_SCALING_STRETCHED ), WeightFactorLarge);

    return format % width % height % refreshrate[0] % refreshrate[1] % scanlineordering % scaling % adapter % output;

}

TEST_RESULT CFSTestCases::CreateDeviceAndSwapChain
(
	int iSwapEffectIndexArg,
	int iNumBuffersIndexArg
)
{
	//
	// Initialize locals.
	//

	TEST_RESULT OverallResult = RESULT_FAIL;
	bool bReturn = false;
	HRESULT hresult = E_FAIL;

	IDXGIOutput * pTargetOutput = NULL;
	IDXGISwapChain* pSwapChain = NULL;


	ReleaseTestCaseObjects();

	//
	// Create our device.
	//

   	assert( m_DevicePointer.pDevice == NULL );
    hresult = g_Application.CreateFactoryAdapterDevice
    (
	    &m_FactoryPointer,
	    m_FSModeInTest.uiDXGIAdapterOrdinal,
	    &m_AdapterPointer,
	    g_Application.m_D3DOptions.SrcDriverType,
	    D3D10_CREATE_DEVICE_SINGLETHREADED,
	    m_TestFeatureLevel,
	    0,
	    &m_DevicePointer
    );

	if
	(
		hresult == DXGI_ERROR_NOT_FOUND
	)
	{
		WriteToLog
		(
			"CFSTestCases::CreateDeviceAndSwapChain:  Could not find an adapter at ordinal %u.",
			m_FSModeInTest.uiDXGIAdapterOrdinal
		);

		OverallResult = RESULT_ABORT;
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
			"CFSTestCases::CreateDeviceAndSwapChain: Could not create device."
		);

		OverallResult = RESULT_SKIP;
		goto Cleanup;
	}
	else if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CFSTestCases::CreateDeviceAndSwapChain:  CreateFactoryAdapterDevice failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	if ( m_DevicePointer.pDevice == NULL )
	{
		WriteToLog
		(
			"CFSTestCases::CreateDeviceAndSwapChain:  CreateFactoryAdapterDevice succeeded but returned NULL device pointer."
		);

		goto Cleanup;
	};


	m_PrimaryAdapterDevice.WrapDevice( m_DevicePointer.pDevice );

	hresult = m_DevicePointer.pDevice->QueryInterface( __uuidof( *m_pDXGIDevice ), reinterpret_cast< void** >( &m_pDXGIDevice ) );
	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CFSTestCases::CreateDeviceAndSwapChain:  QueryInterface for IDXGIDevice failed."
		);

		goto Cleanup;
	}


	//
	// Sanity-check our adapter and factory pointers.
	//

	assert( m_AdapterPointer.pAdapter != NULL );
	assert( m_FactoryPointer.pFactory != NULL );


	//
	// Create our focus window.
	//

	m_hwndFocusWindow = CreateWindowEx
	(
		WS_EX_TOPMOST | WS_EX_APPWINDOW,
		m_szFocusWindowClassName,
		"Focus Window",
		WS_OVERLAPPED,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,
		NULL,
		m_hProgInst,
		NULL
	);

	if ( m_hwndFocusWindow == NULL )
	{
		WriteToLog
		(
			"CFSTestCases::CreateDeviceAndSwapChain:  CreateWindowEx failed."
		);

		goto Cleanup;
	};


	//
	// Display the window.
	//

	ShowWindow
	(
		m_hwndFocusWindow,
		SW_SHOWNORMAL
	);

	UpdateWindow( m_hwndFocusWindow );


	m_hwndDeviceWindow = m_hwndFocusWindow;


	SetForegroundWindow( m_hwndFocusWindow );


	//
	// Associate the focus window with the DXGI factory.
	//

	DebugOut.Write("CFSTestCases::CreateDeviceAndSwapChain:  Calling MakeWindowAssociation in ExecuteTestCase.\n");

	hresult = m_FactoryPointer.pFactory->MakeWindowAssociation
	(
		m_hwndFocusWindow,
		0
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CFSTestCases::CreateDeviceAndSwapChain:  MakeWindowAssociation failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};


	//
	// Specify the swap chain properties.
	//

	DebugOut.Write("CFSTestCases::CreateDeviceAndSwapChain:  MakeWindowAssociation succeeded.\n");


	ZeroMemory( &SCDesc, sizeof( SCDesc ) );

	SCDesc.BufferDesc = m_FSModeInTest.ModeDesc;

	// Skip unsupported formats
	UINT FormatSupport = 0;
	hresult = m_DevicePointer.pDevice->CheckFormatSupport
	(
		SCDesc.BufferDesc.Format,
		&FormatSupport
	);

	if ( hresult == E_FAIL )
	{
		WriteToLog
		(
			"Device does not support this format."
		);

		OverallResult = RESULT_SKIP;
		goto Cleanup;
	}
	else if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CFSTestCases::CreateDeviceAndSwapChain:  CheckFormatSupport failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	C_ASSERT( D3D10_FORMAT_SUPPORT_TEXTURE2D == D3D11_FORMAT_SUPPORT_TEXTURE2D );
	C_ASSERT( D3D10_FORMAT_SUPPORT_RENDER_TARGET == D3D11_FORMAT_SUPPORT_RENDER_TARGET );
	C_ASSERT( D3D10_FORMAT_SUPPORT_CPU_LOCKABLE == D3D11_FORMAT_SUPPORT_CPU_LOCKABLE );
	C_ASSERT( D3D10_FORMAT_SUPPORT_DISPLAY == D3D11_FORMAT_SUPPORT_DISPLAY );
	UINT RequiredFormatSupport =
		D3D10_FORMAT_SUPPORT_TEXTURE2D |
		D3D10_FORMAT_SUPPORT_RENDER_TARGET |
		D3D10_FORMAT_SUPPORT_CPU_LOCKABLE |
		D3D10_FORMAT_SUPPORT_DISPLAY;

	if ( ( FormatSupport & RequiredFormatSupport ) != RequiredFormatSupport )
	{
		WriteToLog
		(
			"Device does not support capabilities with this format needed to test it."
		);

		OverallResult = RESULT_SKIP;
		goto Cleanup;
	};


	SCDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SCDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	SCDesc.SampleDesc.Count = 1;
	SCDesc.SampleDesc.Quality = 0;

	SCDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	SCDesc.BufferCount = iNumBuffersIndexArg;

	// SCDesc.MaxFrameLatency = 1;

	SCDesc.OutputWindow = m_hwndDeviceWindow;

	// SCDesc.DegradationPreference = DXGI_DEGRADE_TIMELINESS;

	SCDesc.Windowed = FALSE;

	SCDesc.SwapEffect = DXGISwapEffects[ iSwapEffectIndexArg ].dxgiSwapEffect;

	// SCDesc.BufferRotation = DXGI_MODE_ROTATION_IDENTITY;

	// SCDesc.RefreshRateInHz = 0.f;

	SCDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	// SCDesc.Flags = 0;


	//
	// Attempt the swap chain creation.
	//

	DebugOut.Write("CFSTestCases::CreateDeviceAndSwapChain:  Initializing swap chain in ExecuteTestCase.\n");

	WriteToLog
	(
		_T("Mode:  Width=%d Height=%d RefreshRate=%d/%d=%d. Frametime=%f ms. Format=%d ScanlineOrdering=%d Scaling=%d"),
		SCDesc.BufferDesc.Width,
		SCDesc.BufferDesc.Height,
		SCDesc.BufferDesc.RefreshRate.Numerator,
		SCDesc.BufferDesc.RefreshRate.Denominator,
        UINT((float)SCDesc.BufferDesc.RefreshRate.Numerator / (float)SCDesc.BufferDesc.RefreshRate.Denominator),
        1000.f / ((float)SCDesc.BufferDesc.RefreshRate.Numerator / (float)SCDesc.BufferDesc.RefreshRate.Denominator),
		SCDesc.BufferDesc.Format,
		SCDesc.BufferDesc.ScanlineOrdering,
		SCDesc.BufferDesc.Scaling
	);


	BOOL bInitializeSwapChainResult = m_TestedSwapChain.InitializeSwapChain( &SCDesc );

	if ( bInitializeSwapChainResult != TRUE )
	{
		WriteToLog
		(
			"CFSTestCases::CreateDeviceAndSwapChain:  InitializeSwapChain failed."
		);

		goto Cleanup;
	};


	//
	// Get a reference to our swap chain.
	//

	assert( m_pSwapChain == NULL );
	m_TestedSwapChain.GetSwapChainPtr( &pSwapChain );

	if ( pSwapChain == NULL )
	{
		WriteToLog
		(
			"GetSwapChainPtr returned NULL swap chain pointer."
		);

		goto Cleanup;
	};

    hresult = pSwapChain->QueryInterface( __uuidof( *m_pSwapChain ), reinterpret_cast< void** >( &m_pSwapChain ) );
    if ( FAILED( hresult ) )
    {
        WriteToLog
        (
            "CFSTestCases::CreateDeviceAndSwapChain:  QueryInterface for IDXGISwapChain1 failed."
        );

        goto Cleanup;
    };

	DebugOut.Write("***********************************\n");
	DebugOut.Write("CreateSwapChain succeeded!\n");


	//
	// Get a descriptor for our swap chain.	
	//

	DXGI_SWAP_CHAIN_DESC desc;
	hresult = m_pSwapChain->GetDesc( &desc );
	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"GetDesc on swap chain failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};


	{
		WriteToLog
		(
			_T("ExecuteTestCase: Actual SwapChain Mode: Width=%d Height=%d RefreshRate=%d/%d=%d. Frametime=%f ms. Format=%s ScanlineOrdering=%d Scaling=%d"),
			desc.BufferDesc.Width,
			desc.BufferDesc.Height,
			desc.BufferDesc.RefreshRate.Numerator,
			desc.BufferDesc.RefreshRate.Denominator,
            UINT((float)desc.BufferDesc.RefreshRate.Numerator / (float)desc.BufferDesc.RefreshRate.Denominator),
            1000.f / ((float)desc.BufferDesc.RefreshRate.Numerator / (float)desc.BufferDesc.RefreshRate.Denominator),
			CFormatInfoMap::Find( desc.BufferDesc.Format )->szDesc,
			desc.BufferDesc.ScanlineOrdering,
			desc.BufferDesc.Scaling
		);

		const DXGIFormatInfo * pBBFormatInfo = NULL;

		if (pBBFormatInfo = CFormatInfoMap::Find((SCDesc.BufferDesc).Format))
		{
			DebugOut.Write(pBBFormatInfo -> szDesc);
			DebugOut.Write("\n");
		}
	}

	DebugOut.Write("***********************************\n");


	//
	// Enumerate the target output.
	//

	hresult = m_AdapterPointer.pAdapter->EnumOutputs
	(
		m_FSModeInTest.uiDXGIOutputOrdinal,
		&pTargetOutput
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CFSTestCases::CreateDeviceAndSwapChain:  EnumOutputs on output %u failed with error 0x%x.",
			m_FSModeInTest.uiDXGIOutputOrdinal,
			hresult
		);

		goto Cleanup;
	};

    
    //
	// Make the swap chain full-screen.
	//

	hresult = m_pSwapChain->SetFullscreenState
	(
		TRUE,
		pTargetOutput
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CFSTestCases::CreateDeviceAndSwapChain:  SetFullscreenState failed with error 0x%x.  This may indicate that the test is occluded by another window.",
            hresult

		);

		// Dump DXGI Journal to get clues as to why FullScreen swapchain is windowed
		if( g_Application.m_bLogDXGIJounrnal )
			g_Application.InitiateJournalDump( true );

		goto Cleanup;
	};


	// Initialize the D3D10Device-owned resources needed
	// for texture rendering.
	// Create the matching textures for back buffer
	// initialization and image verification.

	bool bCreateTexRenderResourcesResult = false;
	switch ( m_TestFeatureLevel )
	{
		case TEST_FEATURE_11_0_LEVEL_9_1:
		case TEST_FEATURE_11_0_LEVEL_9_2:
		case TEST_FEATURE_11_0_LEVEL_9_3:
		case TEST_FEATURE_10_1_LEVEL_9_1:
		case TEST_FEATURE_10_1_LEVEL_9_2:
		case TEST_FEATURE_10_1_LEVEL_9_3:
		{
			bCreateTexRenderResourcesResult = m_PrimaryAdapterDevice.CreateTexRenderResources
			(
				"vs_4_0_level_9_1",
				"ps_4_0_level_9_1"
			);
		} break;


		default:
		{
			bCreateTexRenderResourcesResult = m_PrimaryAdapterDevice.CreateTexRenderResources
			(
				"vs_4_0",
				"ps_4_0"
			);
		} break;
	};

	if ( bCreateTexRenderResourcesResult != true )
	{
		WriteToLog
		(
			"CreateTexRenderResources failed."
		);

		goto Cleanup;
	};


	OverallResult = RESULT_PASS;
    
    
    //
    // Ask GDI what mode we are in to double check
    //
    DXGI_OUTPUT_DESC outputDesc;
    if(SUCCEEDED(hresult))
    {
        hresult = pTargetOutput->GetDesc( &outputDesc );
    }

    if(SUCCEEDED(hresult))
    {
        DEVMODEW Mode;
        ZeroMemory( &Mode, sizeof( DEVMODEW));
        Mode.dmSize = sizeof( DEVMODEW);
        Mode.dmSpecVersion = DM_SPECVERSION;
        BOOL bGetCurrentModeResult = EnumDisplaySettingsW
            (
            outputDesc.DeviceName,
            ENUM_CURRENT_SETTINGS,
            &Mode
            );

        if ( bGetCurrentModeResult != TRUE)
        {
            DWORD GetCurrentModeError = GetLastError();

            WriteToLog
                (
                "EnumDisplaySettingsW failed with error %u.",
                GetCurrentModeError
                );
        }
        else
        {
            WriteToLog
                (
                _T("Double checking mode, EnumDisplaySettingsW Mode: Width=%d Height=%d RefreshRate=%d. Frametime=%f ms."),
                Mode.dmPelsWidth,
                Mode.dmPelsHeight,
                Mode.dmDisplayFrequency,
                1000.f / ((float)Mode.dmDisplayFrequency)
                );   
        }
    }


    BOOL fullscreen = TRUE;
    IDXGIOutput * pTarget = NULL;
    HRESULT hGetFullscreenStateResult = m_pSwapChain->GetFullscreenState
    (
	    &fullscreen,
	    &pTarget
    );

    if ( FAILED( hGetFullscreenStateResult ) )
    {
	    WriteToLog
	    (
		    "GetFullscreenState failed with error 0x%x.",
		    hGetFullscreenStateResult
	    );

		// Dump DXGI Journal to get clues as to why FullScreen swapchain is windowed
		if( g_Application.m_bLogDXGIJounrnal )
			g_Application.InitiateJournalDump( true );

	    goto Cleanup;
    };


    if ( pTarget != NULL )
    {
	    pTarget->Release();
	    pTarget = NULL;
    };


    if ( fullscreen == FALSE )
    {
	    WriteToLog
	    (
            _T("CreateDeviceAndSwapChain::GetFullscreenState returned FALSE. It should be fullscreen by now. Try SetFullScreenState again.")
	    );

        hresult = m_pSwapChain->SetFullscreenState
	    (
		    TRUE,
		    pTargetOutput
	    );

	    if ( FAILED( hresult ) )
	    {
		    WriteToLog
		    (
			    "CFSTestCases::CreateDeviceAndSwapChain:  SetFullscreenState retry failed with error 0x%x.  This may indicate that the test is occluded by another window.",
                hresult
		    );

			// Dump DXGI Journal to get clues as to why FullScreen swapchain is windowed
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );

		    goto Cleanup;
	    };
    }

Cleanup:
	if ( pTargetOutput != NULL )
	{
		pTargetOutput->Release();
		pTargetOutput = NULL;
	};
	SAFE_RELEASE( pSwapChain );


	return OverallResult;
};
