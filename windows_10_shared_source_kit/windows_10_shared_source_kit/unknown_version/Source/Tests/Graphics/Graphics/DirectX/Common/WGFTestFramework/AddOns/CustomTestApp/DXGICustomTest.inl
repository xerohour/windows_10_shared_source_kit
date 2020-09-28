//////////////////////////////////////////////////////////////////////
// File:  DXGICustomTest.inl
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a custom D3D/DXGI test group object.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
// 15 Nov 2006  SEdmison  Move code from CCustomTestApp into
//                        CCustomTest.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CDXGICustomTest
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
template < typename AppClass >
CDXGICustomTest< AppClass >::CDXGICustomTest< AppClass >
(
	AppClass * pApplication
)
:
	CCustomTest< AppClass, CDXGITest >
	(
		pApplication
	),
	m_bCheckForTDRs( false )
{
	ZeroMemory( m_phWindows, sizeof( m_phWindows ) );
	ZeroMemory( m_Factories, sizeof( m_Factories ) );
	ZeroMemory( m_Adapters, sizeof( m_Adapters ) );
	ZeroMemory( m_Devices, sizeof( m_Devices ) );
	ZeroMemory( m_ppSwapChains, sizeof( m_ppSwapChains ) );
	ZeroMemory( m_ppRenderTargetViews, sizeof( m_ppRenderTargetViews ) );

	GetSystemTime( &m_TestCaseStartTime );
};


////////////////////////////////////////////////////////////
//
// Framework overrides
//
////////////////////////////////////////////////////////////
template < typename AppClass >
TEST_RESULT CDXGICustomTest< AppClass >::SetupTestCase()
{
	GetSystemTime( &m_TestCaseStartTime );

	m_bCheckForTDRs = true;

	return RESULT_PASS;
};


////////////////////////////////////////////////////////////
//
// Device management
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CleanupResources
//
// Purpose:
// Cleans up DXGI and Direct3D resources created by
// the custom test group layer.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::CleanupResources()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = S_OK;


	//
	// Break window associations.
	//

	for
	(
		UINT iCurrentFactory = 0;
		iCurrentFactory < ARRAYSIZE( m_Factories );
		++iCurrentFactory
	)
	{
		if ( m_Factories[ iCurrentFactory ].pFactory != NULL )
		{
			HRESULT hAssociateWindowResult = m_Factories[ iCurrentFactory ].pFactory->MakeWindowAssociation
			(
				NULL,
				0
			);

			if ( FAILED( hAssociateWindowResult ) )
			{
				WriteToLog
				(
					_T( "CleanupTestCase:  MakeWindowAssociation failed with error %u (%s)." ),
					hAssociateWindowResult,
					HResultToString( hAssociateWindowResult )
				);
			};
		};
	};


	//
	// Unset current render targets.
	//

	for
	(
		UINT iCurrentDevice = 0;
		iCurrentDevice < ARRAYSIZE( m_Devices );
		++iCurrentDevice
	)
	{
		if ( m_Devices[ iCurrentDevice ].pDevice == NULL )
		{
			continue;
		};

		ID3D10RenderTargetView * pNullRenderTarget = NULL;
		m_Devices[ iCurrentDevice ].pDevice->OMSetRenderTargets
		(
			1,
			&pNullRenderTarget,
			NULL
		);
	};


	//
	// Release our render target views.
	//

	for
	(
		UINT iCurrentView = 0;
		iCurrentView < ARRAYSIZE( m_ppRenderTargetViews );
		++iCurrentView
	)
	{
		if ( m_ppRenderTargetViews[ iCurrentView ] != NULL )
		{
			m_ppRenderTargetViews[ iCurrentView ]->Release();
			m_ppRenderTargetViews[ iCurrentView ] = NULL;
		};
	};


	//
	// Release our swap chains.
	//

	for
	(
		UINT iCurrentSwapChain = 0;
		iCurrentSwapChain < ARRAYSIZE( m_ppSwapChains );
		++iCurrentSwapChain
	)
	{
		if ( m_ppSwapChains[ iCurrentSwapChain ] != NULL )
		{
			m_ppSwapChains[ iCurrentSwapChain ]->SetFullscreenState
			(
				FALSE,
				NULL
			);

			m_ppSwapChains[ iCurrentSwapChain ]->Release();
			m_ppSwapChains[ iCurrentSwapChain ] = NULL;
		};
	};


	//
	// Release our devices.
	//

	for
	(
		UINT iCurrentDevice = 0;
		iCurrentDevice < ARRAYSIZE( m_Devices );
		++iCurrentDevice
	)
	{
		if ( m_Devices[ iCurrentDevice ].pDevice != NULL )
		{
			m_Devices[ iCurrentDevice ].pDevice->Release();
			m_Devices[ iCurrentDevice ].pDevice = NULL;
		};
	};


	//
	// Destroy our windows.
	//

	for
	(
		UINT iCurrentWindow = 0;
		iCurrentWindow < ARRAYSIZE( m_phWindows );
		++iCurrentWindow
	)
	{
		if ( m_phWindows[ iCurrentWindow ] != NULL )
		{
			DestroyWindow( m_phWindows[ iCurrentWindow ] );
			m_phWindows[ iCurrentWindow ] = NULL;
		};
	};


	//
	// Release our adapters.
	//

	for
	(
		UINT iCurrentAdapter = 0;
		iCurrentAdapter < ARRAYSIZE( m_Adapters );
		++iCurrentAdapter
	)
	{
		if ( m_Adapters[ iCurrentAdapter ].pAdapter != NULL )
		{
			m_Adapters[ iCurrentAdapter ].pAdapter->Release();
			m_Adapters[ iCurrentAdapter ].pAdapter = NULL;
		};
	};


	//
	// Release our factories.
	//

	for
	(
		UINT iCurrentFactory = 0;
		iCurrentFactory < ARRAYSIZE( m_Factories );
		++iCurrentFactory
	)
	{
		if ( m_Factories[ iCurrentFactory ].pFactory != NULL )
		{
			m_Factories[ iCurrentFactory ].pFactory->Release();
			m_Factories[ iCurrentFactory ].pFactory = NULL;
		};
	};


	//
	// Check device reference count sentinels.
	//

	for
	(
		UINT iCurrentDevice = 0;
		iCurrentDevice < ARRAYSIZE( m_Devices );
		++iCurrentDevice
	)
	{
		// Check the reference count sentinel for reference leaks.
		if ( m_pDeviceReferenceCountSentinels[ iCurrentDevice ].m_cReferences != 0 )
		{
			WriteToLog
			(
				_T( "Device reference count sentinel %u reports a reference count of %d, suggesting a potential leak." ),
				iCurrentDevice,
				m_pDeviceReferenceCountSentinels[ iCurrentDevice ].m_cReferences
			);

			hOverallResult = E_FAIL;
		};
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CreateAdapter
//
// Purpose:
// Creates a Direct3D 10 adapter in the specified
// adapter slot.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::CreateAdapter
(
	UINT FactorySlot,
	UINT AdapterSlot,
	UINT AdapterOrdinal,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE hRasterizerDLL,
	__out_opt UINT * pcConnectedOutputs
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	IDXGIOutput * pCurrentOutput = NULL;
	UINT cConnectedOutputs = 0;


	//
	// Validate parameters.
	//

	// Range-validate the factory slot.
	if ( FactorySlot > ARRAYSIZE( m_Factories ) )
	{
		WriteToLog
		(
			_T( "CreateAdapter:  Invalid factory slot %u." ),
			FactorySlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	// Verify that we have a factory...
	if ( m_Factories[ FactorySlot ].pFactory == NULL )
	{
		// ...and if not, then create one.
		HRESULT hCreateFactoryResult = m_pApplication->CreateDXGIFactory
		(
			&m_Factories[ FactorySlot ]
		);

		if ( FAILED( hCreateFactoryResult ) )
		{
			WriteToLog
			(
				_T( "CreateAdapter:  CreateFactory failed with error 0x%x (%s)." ),
				hCreateFactoryResult,
				HResultToString( hCreateFactoryResult )
			);

			hOverallResult = hCreateFactoryResult;
			goto Cleanup;
		};

		if ( m_Factories[ FactorySlot ].pFactory == NULL )
		{
			WriteToLog
			(
				_T( "CreateAdapter:  CreateDXGIFactory succeeded, but factory pointer is NULL." )
			);

			hOverallResult = E_FAIL;
			goto Cleanup;
		};
	};

	// Range-validate the adapter slot.
	if ( AdapterSlot > ARRAYSIZE( m_Adapters ) )
	{
		WriteToLog
		(
			_T( "CreateAdapter:  Invalid adapter slot %u." ),
			AdapterSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	// Verify that we're not about to leak an adapter.
	if ( m_Adapters[ AdapterSlot ].pAdapter != NULL )
	{
		WriteToLog
		(
			_T( "CreateAdapter:  Adapter slot %u is occupied." ),
			AdapterSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	switch ( DriverType )
	{
		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			//
			// Enumerate the specified adapter from the provided factory.
			//

			HRESULT hEnumerateAdapterResult = m_Factories[ FactorySlot ].pFactory->EnumAdapters
			(
				AdapterOrdinal,
				&m_Adapters[ AdapterSlot ].pAdapter
			);

			if ( hEnumerateAdapterResult == DXGI_ERROR_NOT_FOUND )
			{
				hOverallResult = S_FALSE;
				goto Cleanup;
			}
			else if ( FAILED( hEnumerateAdapterResult ) )
			{
				WriteToLog
				(
					_T( "CreateAdapter:  EnumAdapters on adapter ordinal %u failed with error 0x%x (%s)." ),
					AdapterOrdinal,
					hEnumerateAdapterResult,
					HResultToString( hEnumerateAdapterResult )
				);

				hOverallResult = hEnumerateAdapterResult;
				goto Cleanup;
			};

			if ( m_Adapters[ AdapterSlot ].pAdapter == NULL )
			{
				WriteToLog
				(
					_T( "CreateAdapter:  EnumAdapters succeeded, but adapter pointer is NULL." )
				);

				hOverallResult = E_FAIL;
				goto Cleanup;
			};
		} break;


		case D3D10_DRIVER_TYPE_SOFTWARE:
		case D3D10_DRIVER_TYPE_REFERENCE:
		case D3D10_DRIVER_TYPE_NULL:
		{
			HRESULT hCreateAdapterResult = m_Factories[ FactorySlot ].pFactory->CreateSoftwareAdapter
			(
				hRasterizerDLL,
				&m_Adapters[ AdapterSlot ].pAdapter
			);

			if ( FAILED( hCreateAdapterResult ) )
			{
				WriteToLog
				(
					_T( "CreateAdapter:  CreateSoftwareAdapter on failed with error 0x%x (%s)." ),
					hCreateAdapterResult,
					HResultToString( hCreateAdapterResult )
				);

				hOverallResult = hCreateAdapterResult;
				goto Cleanup;
			};

			if ( m_Adapters[ AdapterSlot ].pAdapter == NULL )
			{
				WriteToLog
				(
					_T( "CreateAdapter:  CreateSoftwareAdapter succeeded, but adapter pointer is NULL." )
				);

				hOverallResult = E_FAIL;
				goto Cleanup;
			};

			HRESULT hSetPrivateDataResult = m_Adapters[ AdapterSlot ].pAdapter->SetPrivateData
			(
				GUID_DeviceType,
				sizeof( DriverType ),
				&DriverType
			);

			if ( FAILED( hSetPrivateDataResult ) )
			{
				WriteToLog
				(
					_T( "CreateAdapter:  SetPrivateData failed with error 0x%x (%s)." ),
					hSetPrivateDataResult,
					HResultToString( hSetPrivateDataResult )
				);

				hOverallResult = hSetPrivateDataResult;
				goto Cleanup;
			};
		} break;


		default:
		{
			assert( FALSE );
		} break;
	};


	if ( (pcConnectedOutputs != NULL) )
	{
		//
		// Determine how many outputs are connected to this adapter.
		//

		for
		(
			UINT iCurrentOutputOrdinal = 0;
			iCurrentOutputOrdinal < 20;
			++iCurrentOutputOrdinal
		)
		{
			HRESULT hEnumerateOutputResult = m_Adapters[ AdapterSlot ].pAdapter->EnumOutputs
			(
				iCurrentOutputOrdinal,
				&pCurrentOutput
			);

			if ( hEnumerateOutputResult == DXGI_ERROR_NOT_FOUND )
			{
				break;
			}
			else if ( FAILED( hEnumerateOutputResult ) )
			{
				WriteToLog
				(
					_T( "CreateAdapter:  EnumOutputs on adapter %u, output 0 failed with error 0x%x (%s)." ),
					AdapterOrdinal,
					hEnumerateOutputResult,
					HResultToString( hEnumerateOutputResult )
				);

				hOverallResult = hEnumerateOutputResult;
				goto Cleanup;
			};

			if ( pCurrentOutput != NULL )
			{
				pCurrentOutput->Release();
				pCurrentOutput = NULL;
			};

			++cConnectedOutputs;
		};

		*pcConnectedOutputs = cConnectedOutputs;
	};


	hOverallResult = S_OK;


Cleanup:
	if ( pCurrentOutput != NULL )
	{
		pCurrentOutput->Release();
		pCurrentOutput = NULL;
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CreateDevice
//
// Purpose:
// Creates a Direct3D 10 device in the specified
// device slot if necessary.
//
// Preconditions:
// On entrance to the function,
// - The DriverType must be set to a valid Direct3D 10
// driver type.
// - DXGI must have already been loaded.
// - The factory slot number, adapter slot number, and
//   device slot number must all be within the correct
//   range.
// - The factory slot, adapter slot, and/or device slot
// can each be empty.
//
// Postconditions:
// On exit from the function, the factory slot,
// adapter slot, and device slot will all be filled in,
// or else an error will be returned indicating what
// failed.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::CreateDevice
(
	UINT FactorySlot,
	UINT AdapterSlot,
	UINT AdapterOrdinal,
	UINT DeviceSlot,
	D3D10_DRIVER_TYPE DriverType,
	TEST_FEATURE_LEVEL TestFeatureLevel,
	bool bIgnoreRequestedFeatureLevels,
	__out_opt bool * pbAdapterIsRemote
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	IDXGIDevice * pDXGIDevice = NULL;


	//
	// Validate parameters.
	//

	// Range-validate the device slot.
	if ( DeviceSlot >= ARRAYSIZE( m_Devices ) )
	{
		WriteToLog
		(
			_T( "CreateDevice:  Invalid device slot %u." ),
			DeviceSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	//
	// Hand off to the helper function on the framework object.
	//

	assert( m_Devices[ DeviceSlot ].pDevice == NULL );
	hOverallResult = m_pApplication->CreateFactoryAdapterDevice
	(
		&m_Factories[ FactorySlot ],
		AdapterOrdinal,
		&m_Adapters[ AdapterSlot ],
		DriverType,
		0,
		TestFeatureLevel,
		0, // TestFeatureLevel == TEST_FEATURE_10_0_LEVEL_10_0 ? D3D10_SDK_VERSION : D3D10_1_SDK_VERSION,
		&m_Devices[ DeviceSlot ],
		bIgnoreRequestedFeatureLevels,
		pbAdapterIsRemote
	);

	if ( FAILED( hOverallResult ) )
	{
		WriteToLog
		(
			_T("CreateDevice:  CreateFactoryAdapterDevice failed with error 0x%x."),
			hOverallResult
		);

		goto Cleanup;
	};

	if ( m_Devices[ DeviceSlot ].pDevice == NULL )
	{
		WriteToLog
		(
			_T("CreateDevice:  CreateFactoryAdapterDevice succeeded but returned NULL device pointer.")
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};


	//
	// Attach a reference count sentinel to the device.
	//

	m_pDeviceReferenceCountSentinels[ DeviceSlot ].m_cReferences = 0;
	HRESULT hAttachRefCountSentinelResult = m_Devices[ DeviceSlot ].pDevice->SetPrivateDataInterface
	(
		IID_IUnknown,
		&m_pDeviceReferenceCountSentinels[ DeviceSlot ]
	);

	if ( FAILED( hAttachRefCountSentinelResult ) )
	{
		WriteToLog
		(
			_T( "CreateDevice:  SetPrivateDataInterface failed while attaching reference count sentinel to device %u." ),
			DeviceSlot
		);

		goto Cleanup;
	};


	//
	// Verify that the device took a reference on the reference count sentinel.
	//

	if ( m_pDeviceReferenceCountSentinels[ DeviceSlot ].m_cReferences < 1 )
	{
		WriteToLog
		(
			_T( "CreateDevice:  Device failed to take a reference on object set using SetPrivateDataInterface." )
		);

		goto Cleanup;
	};


	hOverallResult = S_OK;


Cleanup:
	//
	// Release temporary interface pointers.
	//

#pragma prefast( suppress:  28922 )
	if ( pDXGIDevice != NULL )
	{
		pDXGIDevice->Release();
		pDXGIDevice = NULL;
	};


	return hOverallResult;
};


template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::CreateDevice
(
	UINT FactorySlot,
	UINT AdapterSlot,
	UINT AdapterOrdinal,
	UINT DeviceSlot,
	D3D10_DRIVER_TYPE DriverType
)
{
	return CreateDevice
	(
		FactorySlot,
		AdapterSlot,
		AdapterOrdinal,
		DeviceSlot,
		DriverType,
		m_TestFeatureLevel,
		false,
		NULL
	);
};


///////////////////////////////////////////////////////
// Function:  CreateDeviceAndSwapChain
//
// Purpose:
// Creates a Direct3D 10 device in the specified
// device slot, as well as a DXGI swap chain in the
// specified swap chain slot.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::CreateDeviceAndSwapChain
(
	UINT FactorySlot,
	UINT AdapterSlot,
	UINT AdapterOrdinal,
	UINT DeviceSlot,
	D3D10_DRIVER_TYPE DriverType,
	UINT SwapChainSlot,
	UINT WindowSlot,
	BOOL bWindowed,
	UINT Width,
	UINT Height,
	UINT RefreshRateNumerator,
	UINT RefreshRateDenominator,
	DXGI_FORMAT Format,
	BOOL bMakeWindowAssociation, // = false
	UINT MakeWindowAssociationFlags, // = 0
    DXGI_SWAP_EFFECT SwapEffect
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Create the device.
	//

	assert( m_Devices[ DeviceSlot ].pDevice == NULL );
	HRESULT hCreateDeviceResult = E_FAIL;
	__try
	{
		hCreateDeviceResult = CreateDevice
		(
			FactorySlot,
			AdapterSlot,
			AdapterOrdinal,
			DeviceSlot,
			DriverType
		);
	}
	__except
	(
		(GetExceptionCode() == _FACD3D10 || GetExceptionCode() == _FACD3D11) ?
			EXCEPTION_EXECUTE_HANDLER :
			EXCEPTION_CONTINUE_SEARCH
	)
	{
		WriteToLog
		(
			_T( "D3D runtime exception caught from CreateDevice while attempting to create device %u." ),
			DeviceSlot
		);

		goto Cleanup;
	};

	if ( FAILED( hCreateDeviceResult ) )
	{
		WriteToLog
		(
			_T( "CreateDeviceAndSwapChain:  CreateDevice failed with error 0x%x (%s)." ),
			hCreateDeviceResult,
			HResultToString( hCreateDeviceResult )
		);

		hOverallResult = hCreateDeviceResult;
		goto Cleanup;
	};


	//
	// Call MakeWindowAssociation if requested.
	//

	if ( bMakeWindowAssociation == TRUE )
	{
		WriteToLog
		(
			_T( "CreateDeviceAndSwapChain:  Calling MakeWindowAssociation on factory %u with window slot %u and flags 0x%x." ),
			FactorySlot,
			WindowSlot,
			MakeWindowAssociationFlags
		);

		HRESULT hMakeWindowAssociationResult = m_Factories[ 0 ].pFactory->MakeWindowAssociation
		(
			NULL,
			MakeWindowAssociationFlags
		);

		if ( FAILED( hMakeWindowAssociationResult ) )
		{
			WriteToLog
			(
				_T( "CreateDeviceAndSwapChain:  MakeWindowAssociation on factory %u with NULL window handle and flags 0x%x failed with error 0x%x (%s)." ),
				FactorySlot,
				MakeWindowAssociationFlags,
				hMakeWindowAssociationResult,
				HResultToString( hMakeWindowAssociationResult )
			);

			hOverallResult = hMakeWindowAssociationResult;
			goto Cleanup;
		};


		hMakeWindowAssociationResult = m_Factories[ 0 ].pFactory->MakeWindowAssociation
		(
			m_phWindows[ WindowSlot ],
			MakeWindowAssociationFlags
		);

		if ( FAILED( hMakeWindowAssociationResult ) )
		{
			WriteToLog
			(
				_T( "CreateDeviceAndSwapChain:  MakeWindowAssociation on factory %u with window slot %u and flags 0x%x failed with error 0x%x (%s)." ),
				FactorySlot,
				WindowSlot,
				MakeWindowAssociationFlags,
				hMakeWindowAssociationResult,
				HResultToString( hMakeWindowAssociationResult )
			);

			hOverallResult = hMakeWindowAssociationResult;
			goto Cleanup;
		};
	};


	//
	// Create the swap chain.
	//

	assert( m_ppSwapChains[ SwapChainSlot ] == NULL );
	hOverallResult = CreateSwapChain
	(
		FactorySlot,
		DeviceSlot,
		SwapChainSlot,
		WindowSlot,
		bWindowed,
		Width,
		Height,
		RefreshRateNumerator,
		RefreshRateDenominator,
		Format,
        SwapEffect
	);

	if ( FAILED( hOverallResult ) )
	{
		WriteToLog
		(
			_T( "CreateDeviceAndSwapChain:  CreateSwapChain failed with error 0x%x (%s)." ),
			hOverallResult,
			HResultToString( hOverallResult )
		);

		goto Cleanup;
	};


	//
	// Set the swap chain as the render target for the device.
	//

	HRESULT hSetRenderTargetResult = SetRenderTarget
	(
		DeviceSlot,
		SwapChainSlot,
		Format
	);

	if ( FAILED( hSetRenderTargetResult ) )
	{
		WriteToLog
		(
			_T( "CreateDeviceAndSwapChain:  SetRenderTarget failed with error 0x%x (%s)." ),
			hSetRenderTargetResult,
			HResultToString( hSetRenderTargetResult )
		);

		hOverallResult = hSetRenderTargetResult;
		goto Cleanup;
	};


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  CreateSwapChain
//
// Purpose:
// Creates a DXGI swap chain in the specified swap
// chain slot.
//
// Returns:
// S_OK     Success.
// S_FALSE  Swap chain was created, but the Windowed
//          state was opposite of the caller's request.
// E_xxxx   Error encountered.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::CreateSwapChain
(
	UINT FactorySlot,
	UINT DeviceSlot,
	UINT SwapChainSlot,
	UINT WindowSlot,
	BOOL bWindowed,
	UINT Width,
	UINT Height,
	UINT RefreshRateNumerator,
	UINT RefreshRateDenominator,
	DXGI_FORMAT Format,
    DXGI_SWAP_EFFECT SwapEffect
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	IDXGIDevice * pDXGIDevice = NULL;


	//
	// Validate parameters.
	//

	if ( SwapChainSlot >= ARRAYSIZE( m_ppSwapChains ) )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Invalid swap chain slot %u." ),
			SwapChainSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	if ( DeviceSlot >= ARRAYSIZE( m_Devices ) )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Invalid device slot %u." ),
			DeviceSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	if ( m_Devices[ DeviceSlot ].pDevice == NULL )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Device slot %u is empty." ),
			DeviceSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	if ( m_ppSwapChains[ SwapChainSlot ] != NULL )
	{
/*
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain slot %u is occupied." ),
			SwapChainSlot
		);
*/

		hOverallResult = S_OK;
		goto Cleanup;
	};


	//
	// Check whether the device supports the requested swap
	// chain buffer format.
	//

	UINT FormatSupport = 0;
	HRESULT hCheckFormatSupportResult = m_Devices[ DeviceSlot ].pDevice->CheckFormatSupport
	(
		Format,
		&FormatSupport
	);

	if ( FAILED( hCheckFormatSupportResult ) )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  CheckFormatSupport on format %s failed with error 0x%x (%s)." ),
			ToString( Format ).c_str(),
			hCheckFormatSupportResult,
			HResultToString( hCheckFormatSupportResult )
		);

		hOverallResult = DXGI_ERROR_UNSUPPORTED;
		goto Cleanup;
	};

	UINT RequiredFormatSupport =
		D3D10_FORMAT_SUPPORT_DISPLAY |
		D3D10_FORMAT_SUPPORT_TEXTURE2D |
		D3D10_FORMAT_SUPPORT_RENDER_TARGET;

	if ( ( FormatSupport & RequiredFormatSupport ) != RequiredFormatSupport )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  CheckFormatSupport on format %s reported support of 0x%x rather than required support of 0x%x." ),
			ToString( Format ).c_str(),
			FormatSupport,
			RequiredFormatSupport
		);

		hOverallResult = DXGI_ERROR_UNSUPPORTED;
		goto Cleanup;
	};


	//
	// Query the D3D10 device object for a DXGI device
	// interface pointer.
	//

	HRESULT hQueryForDXGIDeviceResult = m_Devices[ DeviceSlot ].pDevice->QueryInterface
	(
		IID_IDXGIDevice,
		(void **) &pDXGIDevice
	);

	if ( FAILED( hQueryForDXGIDeviceResult ) )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  QueryInterface for IID_IDXGIDevice failed." )
		);

		hOverallResult = hQueryForDXGIDeviceResult;
		goto Cleanup;
	};


	//
	// Create our swap chain for this device.
	//

	DXGI_SWAP_CHAIN_DESC SwapChainDescriptor;
	ZeroMemory( &SwapChainDescriptor, sizeof( SwapChainDescriptor ) );

	SwapChainDescriptor.BufferDesc.Width = Width;
	SwapChainDescriptor.BufferDesc.Height = Height;

	if
	(
		SwapChainDescriptor.BufferDesc.Width == 0 ||
		SwapChainDescriptor.BufferDesc.Height == 0
	)
	{
		if ( bWindowed == TRUE )
		{
			RECT DeviceWindowClientArea;
			GetClientRect( m_phWindows[ WindowSlot ], &DeviceWindowClientArea );

			SwapChainDescriptor.BufferDesc.Width = DeviceWindowClientArea.right - DeviceWindowClientArea.left;
			if ( SwapChainDescriptor.BufferDesc.Width == 0 )
			{
				SwapChainDescriptor.BufferDesc.Width = 640;
			};

			SwapChainDescriptor.BufferDesc.Height = DeviceWindowClientArea.bottom - DeviceWindowClientArea.top;
			if ( SwapChainDescriptor.BufferDesc.Height == 0 )
			{
				SwapChainDescriptor.BufferDesc.Height = 480;
			};
		}
		else
		{
			SwapChainDescriptor.BufferDesc.Width = 1024;
			SwapChainDescriptor.BufferDesc.Height = 768;
		};
	};

	assert( SwapChainDescriptor.BufferDesc.Width > 0 );
	assert( SwapChainDescriptor.BufferDesc.Height > 0 );

	SwapChainDescriptor.BufferDesc.RefreshRate.Numerator = RefreshRateNumerator;
	SwapChainDescriptor.BufferDesc.RefreshRate.Denominator = RefreshRateDenominator;
	SwapChainDescriptor.BufferDesc.Format = Format;
	SwapChainDescriptor.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	SwapChainDescriptor.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	SwapChainDescriptor.SampleDesc.Count = 1;
	SwapChainDescriptor.SampleDesc.Quality = 0;

	SwapChainDescriptor.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDescriptor.BufferCount = 3;
	SwapChainDescriptor.OutputWindow = m_phWindows[ WindowSlot ];
	SwapChainDescriptor.Windowed = bWindowed;
	SwapChainDescriptor.SwapEffect = SwapEffect;

	SwapChainDescriptor.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;


	WriteToLog
	(
		_T( "CreateSwapChain:  Creating swap chain %u of size %u x %u." ),
		SwapChainSlot,
		SwapChainDescriptor.BufferDesc.Width,
		SwapChainDescriptor.BufferDesc.Height
	);


	assert( m_Factories[ FactorySlot ].pFactory != NULL );
	assert( m_ppSwapChains[ SwapChainSlot ] == NULL );
	HRESULT hCreateSwapChainResult = m_Factories[ FactorySlot ].pFactory->CreateSwapChain
	(
		pDXGIDevice,
		&SwapChainDescriptor,
		&m_ppSwapChains[ SwapChainSlot ]
	);

	if ( FAILED( hCreateSwapChainResult ) )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  CreateSwapChain failed with error 0x%x (%s)." ),
			hCreateSwapChainResult,
			HResultToString( hCreateSwapChainResult )
		);

		hOverallResult = hCreateSwapChainResult;
		goto Cleanup;
	};

	hOverallResult = hCreateSwapChainResult;


	//
	// Turn around and call GetDesc on the swap chain.
	//

	DXGI_SWAP_CHAIN_DESC ReturnedSwapChainDescriptor;
	ZeroMemory( &ReturnedSwapChainDescriptor, sizeof( ReturnedSwapChainDescriptor ) );

	HRESULT hGetSwapChainDescResult = m_ppSwapChains[ SwapChainSlot ]->GetDesc
	(
		&ReturnedSwapChainDescriptor
	);

	if ( FAILED( hGetSwapChainDescResult ) )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain GetDesc failed with error 0x%x (%s)." ),
			hGetSwapChainDescResult,
			HResultToString( hGetSwapChainDescResult )
		);

		hOverallResult = hGetSwapChainDescResult;
		goto Cleanup;
	};


	//
	// Verify that DXGI gave us what we asked for.
	//

	if ( ReturnedSwapChainDescriptor.BufferDesc.Width != SwapChainDescriptor.BufferDesc.Width )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field BufferDesc.Width (%u) than requested (%u)." ),
			DeviceSlot,
			ReturnedSwapChainDescriptor.BufferDesc.Width,
			SwapChainDescriptor.BufferDesc.Width
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.BufferDesc.Height != SwapChainDescriptor.BufferDesc.Height )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field BufferDesc.Height (%u) than requested (%u)." ),
			DeviceSlot,
			ReturnedSwapChainDescriptor.BufferDesc.Height,
			SwapChainDescriptor.BufferDesc.Height
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.BufferDesc.RefreshRate.Numerator != SwapChainDescriptor.BufferDesc.RefreshRate.Numerator )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field BufferDesc.RefreshRate.Numerator than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.BufferDesc.RefreshRate.Denominator != SwapChainDescriptor.BufferDesc.RefreshRate.Denominator )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field BufferDesc.RefreshRate.Denominator than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.BufferDesc.Format != SwapChainDescriptor.BufferDesc.Format )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field BufferDesc.Format than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.BufferDesc.ScanlineOrdering != SwapChainDescriptor.BufferDesc.ScanlineOrdering )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field BufferDesc.ScanlineOrdering than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.BufferDesc.Scaling != SwapChainDescriptor.BufferDesc.Scaling )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field BufferDesc.Scaling than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.SampleDesc.Count != SwapChainDescriptor.SampleDesc.Count )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field SampleDesc.Count than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.SampleDesc.Quality != SwapChainDescriptor.SampleDesc.Quality )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field SampleDesc.Quality than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.BufferUsage != SwapChainDescriptor.BufferUsage )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field BufferUsage than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.BufferCount != SwapChainDescriptor.BufferCount )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field BufferCount than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.OutputWindow != SwapChainDescriptor.OutputWindow )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field OutputWindow than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.Windowed != SwapChainDescriptor.Windowed )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field Windowed than requested." ),
			DeviceSlot
		);

		hOverallResult = S_FALSE;
	};

	if ( ReturnedSwapChainDescriptor.SwapEffect != SwapChainDescriptor.SwapEffect )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field SwapEffect than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};

	if ( ReturnedSwapChainDescriptor.Flags != SwapChainDescriptor.Flags )
	{
		WriteToLog
		(
			_T( "CreateSwapChain:  Swap chain %u descriptor has different field Flags than requested." ),
			DeviceSlot
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};


Cleanup:
	if ( pDXGIDevice != NULL )
	{
		pDXGIDevice->Release();
		pDXGIDevice = NULL;
	};


	return hOverallResult;
};


/*
///////////////////////////////////////////////////////
// Function:  FindAdapterAndOutputForMonitorSlot
//
// Purpose:
// Finds the adapter and output that correspond to the
// specified GDI display device.
//
// Notes:
// It's a bit brute-force, but it gets the job done.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::FindAdapterAndOutputForMonitorSlot
(
	D3D10_DRIVER_TYPE DriverType,
	UINT FactorySlot,
	UINT AdapterSlot,
	UINT MonitorSlot,
	__out IDXGIOutput ** ppOutput,
	BOOL bExclusive
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	IDXGIAdapter * pCurrentAdapter = NULL;
	IDXGIOutput * pCurrentOutput = NULL;


	//
	// Validate parameters and state.
	//

	if ( FactorySlot >= ARRAYSIZE( m_Factories ) )
	{
		WriteToLog
		(
			_T( "FindAdapterAndOutputForMonitorSlot:  Invalid factory slot %u." ),
			FactorySlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	if ( AdapterSlot >= ARRAYSIZE( m_Adapters ) )
	{
		WriteToLog
		(
			_T( "FindAdapterAndOutputForMonitorSlot:  Invalid adapter slot %u." ),
			AdapterSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	if ( m_Adapters[ AdapterSlot ].pAdapter != NULL )
	{
		WriteToLog
		(
			_T( "FindAdapterAndOutputForMonitorSlot:  Adapter slot %u is occupied." ),
			AdapterSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	if ( MonitorSlot >= MONITOR_SLOTS )
	{
		WriteToLog
		(
			_T( "FindAdapterAndOutputForMonitorSlot:  Invalid monitor slot %u." ),
			MonitorSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	if ( m_Factories[ FactorySlot ].pFactory == NULL )
	{
		WriteToLog
		(
			_T( "FindAdapterAndOutputForMonitorSlot:  Creating factory %u." ),
			FactorySlot
		);

		HRESULT hCreateFactoryResult = CreateDXGIFactory
		(
			IID_IDXGIFactory,
			&m_Factories[ FactorySlot ]
		);

		if ( FAILED( hCreateFactoryResult ) )
		{
			WriteToLog
			(
				_T( "FindAdapterAndOutputForMonitorSlot:  CreateDXGIFactory failed with error 0x%x (%s)." ),
				hCreateFactoryResult,
				HResultToString( hCreateFactoryResult )
			);

			hOverallResult = hCreateFactoryResult;
			goto Cleanup;
		};

		if ( m_Factories[ FactorySlot ].pFactory == NULL )
		{
			WriteToLog
			(
				_T( "FindAdapterAndOutputForMonitorSlot:  CreateDXGIFactory succeeded, but factory pointer is NULL." )
			);

			hOverallResult = E_FAIL;
			goto Cleanup;
		};
	};


	switch ( DriverType )
	{
		case D3D10_DRIVER_TYPE_HARDWARE:
		{
			// For each adapter...
			for
			(
				UINT iCurrentAdapter = 0;
				iCurrentAdapter < 10;
				++iCurrentAdapter
			)
			{
				// ...enumerate the adapter...
				HRESULT hEnumerateAdapterResult = m_Factories[ FactorySlot ].pFactory->EnumAdapters
				(
					iCurrentAdapter,
					&pCurrentAdapter
				);

				// ...and if there are no more...
				if ( hEnumerateAdapterResult == DXGI_ERROR_NOT_FOUND )
				{
					// ...then fail, because we never found what we were looking for...
					hOverallResult = hEnumerateAdapterResult;
					goto Cleanup;
				}
				// ...and if we failed for some other reason...
				else if ( FAILED( hEnumerateAdapterResult ) )
				{
					// ...then fail.
					WriteToLog
					(
						_T( "FindAdapterAndOutputForMonitorSlot:  EnumAdapters on adapter %u failed with error 0x%x (%s)." ),
						iCurrentAdapter,
						hEnumerateAdapterResult,
						HResultToString( hEnumerateAdapterResult )
					);

					hOverallResult = hEnumerateAdapterResult;
					goto Cleanup;
				};


				if ( pCurrentAdapter == NULL )
				{
					WriteToLog
					(
						_T( "FindAdapterAndOutputForMonitorSlot:  EnumAdapters succeeded, but adapter pointer is NULL." )
					);

					hOverallResult = E_FAIL;
					goto Cleanup;
				};


				for
				(
					UINT iCurrentOutput = 0;
					iCurrentOutput < 10;
					++iCurrentOutput
				)
				{
					HRESULT hEnumerateOutputResult = pCurrentAdapter->EnumOutputs
					(
						iCurrentOutput,
						&pCurrentOutput
					);

					if ( hEnumerateOutputResult == DXGI_ERROR_NOT_FOUND )
					{
						break;
					}
					else if ( FAILED( hEnumerateOutputResult ) )
					{
						WriteToLog
						(
							_T( "FindAdapterAndOutputForMonitorSlot:  EnumOutputs on adapter %u, output %u failed with error 0x%x (%s)." ),
							iCurrentAdapter,
							iCurrentOutput,
							hEnumerateOutputResult,
							HResultToString( hEnumerateOutputResult )
						);

						hOverallResult = hEnumerateOutputResult;
						goto Cleanup;
					};


					DXGI_OUTPUT_DESC OutputDescriptor;
					ZeroMemory( &OutputDescriptor, sizeof( OutputDescriptor ) );
					HRESULT hGetDescriptorResult = pCurrentOutput->GetDesc
					(
						&OutputDescriptor
					);

					if ( FAILED( hGetDescriptorResult ) )
					{
						WriteToLog
						(
							_T( "FindAdapterAndOutputForMonitorSlot:  GetDesc on adapter %u, output %u failed with error 0x%x (%s)." ),
							iCurrentAdapter,
							iCurrentOutput,
							hGetDescriptorResult,
							HResultToString( hGetDescriptorResult )
						);

						hOverallResult = hGetDescriptorResult;
						goto Cleanup;
					};


					if
					(
						_wcsicmp
						(
							m_pDisplayDevices[ MonitorSlot ].DeviceName,
							OutputDescriptor.DeviceName
						) == 0
					)
					{
						m_Adapters[ AdapterSlot ].pAdapter = pCurrentAdapter;
						pCurrentAdapter = NULL;

						if ( ppOutput != NULL )
						{
							*ppOutput = pCurrentOutput;
							pCurrentOutput = NULL;
						};

						hOverallResult = S_OK;
						goto Cleanup;
					};


					pCurrentOutput->Release();
					pCurrentOutput = NULL;
				};


				pCurrentAdapter->Release();
				pCurrentAdapter = NULL;
			};
		} break;


		case D3D10_DRIVER_TYPE_SOFTWARE:
		case D3D10_DRIVER_TYPE_REFERENCE:
		case D3D10_DRIVER_TYPE_NULL:
		{
			//
			// Load the software rasterizer.
			//

			if
			(
				DriverType == D3D10_DRIVER_TYPE_SOFTWARE &&
				m_hRasterizerDLL == NULL
			)
			{
				m_hRasterizerDLL = LoadLibrary( m_pRasterizerDLLName );

				if ( m_hRasterizerDLL == NULL )
				{
					WriteToLog
					(
						_T( "CreateDevice:  Failed to load software rasterizer %s." ),
						m_pRasterizerDLLName
					);

					goto Cleanup;
				};
			}
			else if
			(
				(
					DriverType == D3D10_DRIVER_TYPE_REFERENCE ||
					DriverType == D3D10_DRIVER_TYPE_NULL
				) &&
				m_hReferenceDLL == NULL
			)
			{
				m_hReferenceDLL = LoadLibraryA( "D3D10Ref.dll" );

				if ( m_hReferenceDLL == NULL )
				{
					WriteToLog
					(
						_T( "CreateDevice:  Failed to load D3D10Ref.dll." )
					);

					goto Cleanup;
				};
			};


			if ( bExclusive == TRUE )
			{
				WriteToLog
				(
					_T( "FindAdapterAndOutputForMonitorSlot:  Setting REF device mask." )
				);

				DWORD GDIDisplaysControlledByThisAdapter = 1 << m_GDIDisplayDeviceOrdinals[ MonitorSlot ];

				HRESULT hSetGDIDisplayMaskResult = SetRefDeviceMask
				(
					GDIDisplaysControlledByThisAdapter,
					TRUE
				);

				if ( FAILED( hSetGDIDisplayMaskResult ) )
				{
					WriteToLog
					(
						_T( "FindAdapterAndOutputForMonitorSlot:  SetRefDeviceMask failed with error 0x%x (%s)." ),
						hSetGDIDisplayMaskResult,
						HResultToString( hSetGDIDisplayMaskResult )
					);
				};
			};


			HRESULT hCreateAdapterResult = E_FAIL;
			switch ( DriverType )
			{
				case D3D10_DRIVER_TYPE_SOFTWARE:
				{
					hCreateAdapterResult = m_Factories[ FactorySlot ].pFactory->CreateSoftwareAdapter
					(
						m_hRasterizerDLL,
						&pCurrentAdapter
					);
				} break;


				case D3D10_DRIVER_TYPE_REFERENCE:
				case D3D10_DRIVER_TYPE_NULL:
				{
					hCreateAdapterResult = m_Factories[ FactorySlot ].pFactory->CreateSoftwareAdapter
					(
						m_hReferenceDLL,
						&pCurrentAdapter
					);
				} break;
			};

			if ( FAILED( hCreateAdapterResult ) )
			{
				WriteToLog
				(
					_T( "FindAdapterAndOutputForMonitorSlot:  CreateSoftwareAdapter failed with error 0x%x (%s)." ),
					hCreateAdapterResult,
					HResultToString( hCreateAdapterResult )
				);

				hOverallResult = hCreateAdapterResult;
				goto Cleanup;
			};

			if ( pCurrentAdapter == NULL )
			{
				WriteToLog
				(
					_T( "FindAdapterAndOutputForMonitorSlot:  CreateSoftwareAdapter succeeded, but adapter pointer is NULL." )
				);

				hOverallResult = E_FAIL;
				goto Cleanup;
			};


			HRESULT hSetPrivateDataResult = pCurrentAdapter->SetPrivateData
			(
				GUID_DeviceType,
				sizeof( DriverType ),
				&DriverType
			);

			if ( FAILED( hSetPrivateDataResult ) )
			{
				WriteToLog
				(
					_T( "FindAdapterAndOutputForMonitorSlot:  SetPrivateData failed with error 0x%x (%s)." ),
					hSetPrivateDataResult,
					HResultToString( hSetPrivateDataResult )
				);

				hOverallResult = hSetPrivateDataResult;
				goto Cleanup;
			};


			if ( bExclusive == TRUE )
			{
				WriteToLog
				(
					_T( "FindAdapterAndOutputForMonitorSlot:  Clearing REF device mask." )
				);

				HRESULT hClearGDIDisplayMaskResult = SetRefDeviceMask
				(
					0,
					FALSE
				);

				if ( FAILED( hClearGDIDisplayMaskResult ) )
				{
					WriteToLog
					(
						_T( "FindAdapterAndOutputForMonitorSlot:  SetRefDeviceMask failed with error 0x%x (%s)." ),
						hClearGDIDisplayMaskResult,
						HResultToString( hClearGDIDisplayMaskResult )
					);
				};
			};


			for
			(
				UINT iCurrentOutput = 0;
				iCurrentOutput < 10;
				++iCurrentOutput
			)
			{
				HRESULT hEnumerateOutputResult = pCurrentAdapter->EnumOutputs
				(
					iCurrentOutput,
					&pCurrentOutput
				);

				if ( hEnumerateOutputResult == DXGI_ERROR_NOT_FOUND )
				{
					break;
				}
				else if ( FAILED( hEnumerateOutputResult ) )
				{
					WriteToLog
					(
						_T( "FindAdapterAndOutputForMonitorSlot:  EnumOutputs on output %u failed with error 0x%x (%s)." ),
						iCurrentOutput,
						hEnumerateOutputResult,
						HResultToString( hEnumerateOutputResult )
					);

					hOverallResult = hEnumerateOutputResult;
					goto Cleanup;
				};


				DXGI_OUTPUT_DESC OutputDescriptor;
				ZeroMemory( &OutputDescriptor, sizeof( OutputDescriptor ) );
				HRESULT hGetDescriptorResult = pCurrentOutput->GetDesc
				(
					&OutputDescriptor
				);

				if ( FAILED( hGetDescriptorResult ) )
				{
					WriteToLog
					(
						_T( "FindAdapterAndOutputForMonitorSlot:  GetDesc on output %u failed with error 0x%x (%s)." ),
						iCurrentOutput,
						hGetDescriptorResult,
						HResultToString( hGetDescriptorResult )
					);

					hOverallResult = hGetDescriptorResult;
					goto Cleanup;
				};


				if
				(
					_wcsicmp
					(
						m_pDisplayDevices[ MonitorSlot ].DeviceName,
						OutputDescriptor.DeviceName
					) == 0
				)
				{
					m_Adapters[ AdapterSlot ].pAdapter = pCurrentAdapter;
					pCurrentAdapter = NULL;

					if ( ppOutput != NULL )
					{
						*ppOutput = pCurrentOutput;
						pCurrentOutput = NULL;
					};

					hOverallResult = S_OK;
					goto Cleanup;
				};

				pCurrentOutput->Release();
				pCurrentOutput = NULL;
			};
		} break;


		default:
		{
			assert( FALSE );
		} break;
	};


Cleanup:
	if ( pCurrentOutput != NULL )
	{
		pCurrentOutput->Release();
		pCurrentOutput = NULL;
	};

	if ( pCurrentAdapter != NULL )
	{
		pCurrentAdapter->Release();
		pCurrentAdapter = NULL;
	};


	return hOverallResult;
};
*/


///////////////////////////////////////////////////////
// Function:  GetFullscreenState
//
// Purpose:
// Determines whether the swap chain in the specified
// swap chain slot is full-screen, and, if so, on which
// output.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::GetFullscreenState
(
	UINT SwapChainSlot,
	__out BOOL * pbFullScreen,
	__out_opt IDXGIOutput ** ppDXGIOutput
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	IDXGIOutput * pOutput = NULL;


	//
	// Validate parameters.
	//

	if ( SwapChainSlot >= ARRAYSIZE( m_ppSwapChains ) )
	{
		WriteToLog
		(
			_T( "GetFullscreenState:  Invalid swap chain slot %u." ),
			SwapChainSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	if ( m_ppSwapChains[ SwapChainSlot ] == NULL )
	{
		WriteToLog
		(
			_T( "GetFullscreenState:  Swap chain slot %u is empty." ),
			SwapChainSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};


	if ( pbFullScreen == NULL )
	{
		hOverallResult = E_POINTER;
		goto Cleanup;
	};


	//
	// Hand off to the swap chain method.
	//

	HRESULT hGetFullscreenStateResult = m_ppSwapChains[ SwapChainSlot ]->GetFullscreenState
	(
		pbFullScreen,
		&pOutput
	);

	if ( FAILED( hGetFullscreenStateResult ) )
	{
		WriteToLog
		(
			_T( "GetFullscreenState:  GetFullscreenState failed with error 0x%x (%s)." ),
			hGetFullscreenStateResult,
			HResultToString( hGetFullscreenStateResult )
		);

		hOverallResult = hGetFullscreenStateResult;
		goto Cleanup;
	};


	if
	(
		ppDXGIOutput != NULL &&
		pOutput == NULL
	)
	{
		HRESULT hGetContainingOutputResult = m_ppSwapChains[ SwapChainSlot ]->GetContainingOutput
		(
			&pOutput
		);

		if ( FAILED( hGetContainingOutputResult ) )
		{
			WriteToLog
			(
				_T( "GetFullscreenState:  GetContainingOutput failed with error 0x%x (%s)." ),
				hGetContainingOutputResult,
				HResultToString( hGetContainingOutputResult )
			);

			hOverallResult = hGetContainingOutputResult;
			goto Cleanup;
		};
	};


	hOverallResult = S_OK;


Cleanup:
	if ( ppDXGIOutput != NULL )
	{
		*ppDXGIOutput = pOutput;
	}
	else
	{
		if ( pOutput != NULL )
		{
			pOutput->Release();
			pOutput = NULL;
		};
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  ResizeBackBuffers
//
// Purpose:
// Resizes the specified swap chain's buffers to the
// specified size and format.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::ResizeBackBuffers
(
	UINT DeviceSlot,
	UINT SwapChainSlot,
	UINT cBuffers,
	UINT Width,
	UINT Height,
	DXGI_FORMAT NewFormat,
	DWORD Flags
)
{
	if ( m_ppSwapChains[ SwapChainSlot ] == NULL )
	{
		return S_FALSE;
	};


	//
	// Unset the current render target.
	//

	assert( m_Devices[ DeviceSlot ].pDevice != NULL );

	ID3D10RenderTargetView * pNullRenderTarget = NULL;
	m_Devices[ DeviceSlot ].pDevice->OMSetRenderTargets
	(
		1,
		&pNullRenderTarget,
		NULL
	);


	//
	// Release the current render target view.
	//

	if ( m_ppRenderTargetViews[ SwapChainSlot ] != NULL )
	{
		m_ppRenderTargetViews[ SwapChainSlot ]->Release();
		m_ppRenderTargetViews[ SwapChainSlot ] = NULL;
	};


	//
	// Actually resize the back buffer.
	//

	HRESULT hResizeBackBufferResult = m_ppSwapChains[ SwapChainSlot ]->ResizeBuffers
	(
		cBuffers,
		Width,
		Height,
		NewFormat,
		Flags
	);

	if ( FAILED( hResizeBackBufferResult ) )
	{
		WriteToLog
		(
			_T( "ResizeBackBuffers:  ResizeBuffers failed with error 0x%x (%s)." ),
			hResizeBackBufferResult,
			HResultToString( hResizeBackBufferResult )
		);

		return hResizeBackBufferResult;
	};


	//
	// Re-create the render target view, and re-set the render target.
	//

	return SetRenderTarget
	(
		DeviceSlot,
		SwapChainSlot,
		NewFormat
	);
};


///////////////////////////////////////////////////////
// Function:  SetFullscreenState
//
// Purpose:
// Sets the specified swap chain to be windowed or
// full-screen on the specified output.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::SetFullscreenState
(
	UINT FactorySlot,
	UINT SwapChainSlot,
	BOOL bFullScreen,
	__in_opt IDXGIOutput * pTargetOutput,
	BOOL bMakeWindowAssociation, // = TRUE
	UINT MakeWindowAssociationFlags // = 0
)
{
	//
	// Validate parameters.
	//

	assert( FactorySlot < ARRAYSIZE( m_Factories ) );
	if ( FactorySlot >= ARRAYSIZE( m_Factories ) )
	{
		WriteToLog
		(
			_T( "SetFullscreenState:  Invalid factory slot %u." ),
			FactorySlot
		);

		return E_INVALIDARG;
	};

	assert( SwapChainSlot < ARRAYSIZE( m_ppSwapChains ) );
	if ( SwapChainSlot >= ARRAYSIZE( m_ppSwapChains ) )
	{
		WriteToLog
		(
			_T( "SetFullscreenState:  Invalid swap chain slot %u." ),
			SwapChainSlot
		);

		return E_INVALIDARG;
	};


	//
	// Verify that we have a swap chain.
	//

	assert( m_ppSwapChains[ SwapChainSlot ] != NULL );
	if ( m_ppSwapChains[ SwapChainSlot ] == NULL )
	{
		WriteToLog
		(
			_T( "SetFullscreenState:  Swap chain slot %u is empty." ),
			SwapChainSlot
		);

		return E_INVALIDARG;
	};


	if
	(
		bFullScreen == TRUE &&
		bMakeWindowAssociation == TRUE
	)
	{
		//
		// Make the swap chain's window our focus window.
		//
		// N.B. [SEdmison]:
		// We have to do this, because DXGI expects the focus
		// window to be the top window in the z-order so that
		// it will receive window messages about interesting
		// things like app activation.  If we fail to change
		// the window association, DXGI will yank the focus
		// window to the top of the z-order, thus making our
		// swap chain's device window occluded.
		//

		assert( m_Factories[ FactorySlot ].pFactory != NULL );

		HWND hWindow = NULL;
		HRESULT hGetWindowAssociationResult = m_Factories[ FactorySlot ].pFactory->GetWindowAssociation
		(
			&hWindow
		);

		if ( FAILED( hGetWindowAssociationResult ) )
		{
			WriteToLog
			(
				_T( "GetWindowAssociation failed with error 0x%x (%s)." ),
				hGetWindowAssociationResult,
				HResultToString( hGetWindowAssociationResult )
			);

			return hGetWindowAssociationResult;
		};


		if ( hWindow != m_phWindows[ SwapChainSlot ] )
		{
			HRESULT hAssociateWindowResult = m_Factories[ FactorySlot ].pFactory->MakeWindowAssociation
			(
				m_phWindows[ SwapChainSlot ],
				MakeWindowAssociationFlags
			);

			if ( FAILED( hAssociateWindowResult ) )
			{
				WriteToLog
				(
					_T( "MakeWindowAssociation failed with error 0x%x (%s)." ),
					hAssociateWindowResult,
					HResultToString( hAssociateWindowResult )
				);

				return hAssociateWindowResult;
			};
		};
	};


	//
	// Hand off to the swap chain method.
	//

	assert( m_ppSwapChains[ SwapChainSlot ] != NULL );
	if ( m_ppSwapChains[ SwapChainSlot ] == NULL )
	{
		WriteToLog
		(
			_T( "m_ppSwapChains[ SwapChainSlot ] == NULL." )
		);

		return E_INVALIDARG;
	};


	HRESULT hPresentResult = m_ppSwapChains[ SwapChainSlot ]->SetFullscreenState
	(
		bFullScreen,
		pTargetOutput
	);

	return hPresentResult;
};


///////////////////////////////////////////////////////
// Function:  SetRefDeviceMask
//
// Purpose:
// Sets a mask in the registry indicating to d3d10ref
// which outputs it should enumerate and drive, which
// is specified as a mask of GDI device ordinals.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::SetRefDeviceMask
(
	BOOL bEnable,
	DWORD DeviceMask
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	HKEY hReferenceDeviceKey = NULL;


	//
	// Open the registry key.
	//

	LONG OpenKeyResult = RegCreateKeyEx
	(
		HKEY_CURRENT_USER,
		_T( "Software\\Microsoft\\Direct3D\\ReferenceDevice" ),
		0,
		NULL,
		0,
		KEY_READ | KEY_WRITE,
		NULL,
		&hReferenceDeviceKey,
		NULL
	);

	if ( OpenKeyResult != ERROR_SUCCESS )
	{
		WriteToLog
		(
			_T( "SetRefDeviceMask:  RegCreateKeyEx failed with error %u." ),
			OpenKeyResult
		);

		hOverallResult = HRESULT_FROM_WIN32( OpenKeyResult );
		goto Cleanup;
	};


	//
	// Set or delete the value.
	//

	if ( bEnable == TRUE )
	{
		LONG SetValueResult = RegSetValueEx
		(
			hReferenceDeviceKey,
			_T( "D3D10RefGdiDisplayMask" ),
			0,
			REG_DWORD,
			(BYTE *) &DeviceMask,
			sizeof( DeviceMask )
		);

		if ( SetValueResult != ERROR_SUCCESS )
		{
			WriteToLog
			(
				_T( "SetRefDeviceMask:  RegSetValueEx failed with error %u." ),
				SetValueResult
			);

			hOverallResult = HRESULT_FROM_WIN32( SetValueResult );
			goto Cleanup;
		};
	}
	else
	{
		LONG DeleteValueResult = RegDeleteValue
		(
			hReferenceDeviceKey,
			_T( "D3D10RefGdiDisplayMask" )
		);

		if ( DeleteValueResult != ERROR_SUCCESS )
		{
			WriteToLog
			(
				_T( "SetRefDeviceMask:  RegDeleteValue failed with error %u." ),
				DeleteValueResult
			);

			hOverallResult = HRESULT_FROM_WIN32( DeleteValueResult );
			goto Cleanup;
		};
	};


	hOverallResult = S_OK;


Cleanup:
	//
	// Close the key.
	//

	if ( hReferenceDeviceKey != NULL )
	{
		RegCloseKey( hReferenceDeviceKey );
		hReferenceDeviceKey = NULL;
	};

	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  SetRenderTarget
//
// Purpose:
// Set the specified swap chain as the render target
// for the specified device.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::SetRenderTarget
(
	UINT DeviceSlot,
	UINT SwapChainSlot,
	DXGI_FORMAT Format
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	ID3D10Texture2D * pDXGIBackBuffer = NULL;


	//
	// Validate parameters.
	//

	if ( DeviceSlot >= ARRAYSIZE( m_Devices ) )
	{
		WriteToLog
		(
			_T( "SetRenderTarget:  Invalid device slot %u." ),
			DeviceSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	if ( SwapChainSlot >= ARRAYSIZE( m_ppSwapChains ) )
	{
		WriteToLog
		(
			_T( "SetRenderTarget:  Invalid swap chain slot %u." ),
			SwapChainSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	if ( m_ppSwapChains[ SwapChainSlot ] == NULL )
	{
		WriteToLog
		(
			_T( "SetRenderTarget:  Swap chain slot %u is empty." ),
			SwapChainSlot
		);

		hOverallResult = E_INVALIDARG;
		goto Cleanup;
	};

	// Verify that we're not about to leak a render target view.
	if ( m_ppRenderTargetViews[ SwapChainSlot ] != NULL )
	{
		ID3D10RenderTargetView * pNullRenderTargetView = NULL;

		m_Devices[ DeviceSlot ].pDevice->OMSetRenderTargets
		(
			1,
			&pNullRenderTargetView,
			NULL // depth stencil view
		);

		m_ppRenderTargetViews[ SwapChainSlot ]->Release();
		m_ppRenderTargetViews[ SwapChainSlot ] = NULL;
	};


	//
	// Get the backbuffer from the swapchain for this device.
	//

	HRESULT hGetBufferResult = m_ppSwapChains[ SwapChainSlot ]->GetBuffer
	(
		0,
		IID_ID3D10Texture2D,
		(void **) &pDXGIBackBuffer
	);

	if ( FAILED( hGetBufferResult ) )
	{
		WriteToLog
		(
			_T( "SetRenderTarget:  GetBuffer failed with error 0x%x (%s)." ),
			hGetBufferResult,
			HResultToString( hGetBufferResult )
		);

		hOverallResult = hGetBufferResult;
		goto Cleanup;
	};


	//
	// Create a render target view of the backbuffer.
	//

	D3D10_RENDER_TARGET_VIEW_DESC RenderTargetViewDescriptor;
	ZeroMemory( &RenderTargetViewDescriptor, sizeof( RenderTargetViewDescriptor ) );
	RenderTargetViewDescriptor.Format = Format;
	RenderTargetViewDescriptor.ViewDimension = D3D10_RTV_DIMENSION_TEXTURE2D;

	RenderTargetViewDescriptor.Texture2D.MipSlice = 0;

	HRESULT hCreateRenderTargetViewResult = m_Devices[ DeviceSlot ].pDevice->CreateRenderTargetView
	(
		pDXGIBackBuffer,
		&RenderTargetViewDescriptor,
		&m_ppRenderTargetViews[ SwapChainSlot ]
	);

	if ( FAILED( hCreateRenderTargetViewResult ) )
	{
		WriteToLog
		(
			_T( "SetRenderTarget:  CreateRenderTargetView failed with error 0x%x (%s)." ),
			hCreateRenderTargetViewResult,
			HResultToString( hCreateRenderTargetViewResult )
		);

		hOverallResult = hCreateRenderTargetViewResult;
		goto Cleanup;
	};


	//
	// Set our render target.
	//

	m_Devices[ DeviceSlot ].pDevice->OMSetRenderTargets
	(
		1,
		&m_ppRenderTargetViews[ SwapChainSlot ],
		NULL // depth stencil view
	);


	hOverallResult = S_OK;


Cleanup:
	//
	// Release the backbuffer.
	//

	if ( pDXGIBackBuffer != NULL )
	{
		pDXGIBackBuffer->Release();
		pDXGIBackBuffer = NULL;
	};


	return hOverallResult;
};


////////////////////////////////////////////////////////////
//
// Rendering
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  PresentScene
//
// Purpose:
// Presents the scene to the hardware.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::PresentScene
(
	UINT SwapChainSlot,
	UINT SyncInterval, // = 0
	UINT PresentFlags // = 0
)
{
	//
	// Verify that we have a swap chain.
	//

	assert( m_ppSwapChains[ SwapChainSlot ] != NULL );
	if ( m_ppSwapChains[ SwapChainSlot ] == NULL )
	{
		return E_FAIL;
	};


	HRESULT hPresentResult = m_ppSwapChains[ SwapChainSlot ]->Present
	(
		SyncInterval,
		PresentFlags
	);

	//
	// Reset our render target.
	//

	DXGI_SWAP_CHAIN_DESC scDesc;
	m_ppSwapChains[ SwapChainSlot ]->GetDesc( &scDesc );

	if( scDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL )
	{
		if( m_ppRenderTargetViews[ SwapChainSlot ] )
		{
			ID3D10Device *pDevice = NULL;
			m_ppRenderTargetViews[ SwapChainSlot ]->GetDevice( &pDevice );
			if( pDevice )
			{
				pDevice->OMSetRenderTargets
					(
					1,
					&m_ppRenderTargetViews[ SwapChainSlot ],
					NULL // depth stencil view
					);
				SAFE_RELEASE( pDevice );
			}
			else
			{
				WriteToLog
					(
					_T( "PresentScene:  Unable to obtain D3D10 Device after Present() to reset render targets" )
					);
			}
		}
	}

	return hPresentResult;
};


///////////////////////////////////////////////////////
// Function:  RenderScene
//
// Purpose:
// Helper method to render an empty scene on the
// specified device.  That is, we just clear, without
// actually rendering anything.  (Why bother?)
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::RenderScene
(
	UINT DeviceSlot,
	UINT SwapChainSlot
)
{
	//
	// Validate parameters.
	//

	assert( m_Devices[ DeviceSlot ].pDevice != NULL );
	if ( m_Devices[ DeviceSlot ].pDevice == NULL )
	{
		return E_INVALIDARG;
	};


	assert( m_ppRenderTargetViews[ SwapChainSlot ] != NULL );
	if ( m_ppRenderTargetViews[ SwapChainSlot ] == NULL )
	{
		return E_INVALIDARG;
	};


	//
	// Clear the render target to blue.
	//

	FLOAT BlueRGBA[ 4 ] = { 0.0, 0.0, 1.0, 1.0 };
	m_Devices[ DeviceSlot ].pDevice->ClearRenderTargetView
	(
		m_ppRenderTargetViews[ SwapChainSlot ],
		BlueRGBA
	);


	//
	// Render something.
	//
	// @TODO [SEdmison]:
	//


	return S_OK;
};


////////////////////////////////////////////////////////////
//
// Helper methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  GetAngleFromDXGIOrientation
//
// Purpose:
// Returns the angle in degrees corresponding to a
// DEVMODE orientation mode.
///////////////////////////////////////////////////////
template < typename AppClass >
int CDXGICustomTest< AppClass >::GetAngleFromDXGIOrientation
(
	DXGI_MODE_ROTATION DisplayOrientation
)
{
	switch ( DisplayOrientation )
	{
		case DXGI_MODE_ROTATION_IDENTITY:
		{
			return 0;
		};

		case DXGI_MODE_ROTATION_ROTATE90:
		{
			return 90;
		};

		case DXGI_MODE_ROTATION_ROTATE180:
		{
			return 180;
		};

		case DXGI_MODE_ROTATION_ROTATE270:
		{
			return 270;
		};

		default:
		{
			assert( FALSE );
			return 0;
		} break;
	};
};


///////////////////////////////////////////////////////
// Function:  HResultToString
//
// Purpose:
// Returns a stringized HRESULT.
///////////////////////////////////////////////////////
template < typename AppClass >
__override LPCTSTR CDXGICustomTest< AppClass >::HResultToString
(
	HRESULT hResult
) const
{
	switch ( hResult )
	{
		//
		// DXGI status codes
		//

		HANDLE_HRESULT( DXGI_STATUS_OCCLUDED );
		HANDLE_HRESULT( DXGI_STATUS_CLIPPED );
		HANDLE_HRESULT( DXGI_STATUS_NO_REDIRECTION );

		HANDLE_HRESULT( DXGI_ERROR_DEVICE_HUNG );
		HANDLE_HRESULT( DXGI_ERROR_DEVICE_RESET );
		HANDLE_HRESULT( DXGI_ERROR_DRIVER_INTERNAL_ERROR );
		HANDLE_HRESULT( DXGI_ERROR_DEVICE_REMOVED );
		HANDLE_HRESULT( DXGI_ERROR_INVALID_CALL );
		HANDLE_HRESULT( DXGI_ERROR_UNSUPPORTED );
		HANDLE_HRESULT( DXGI_ERROR_NOT_FOUND );
		HANDLE_HRESULT( DXGI_ERROR_MORE_DATA );
		HANDLE_HRESULT( DXGI_ERROR_WAS_STILL_DRAWING );
		HANDLE_HRESULT( DXGI_ERROR_NONEXCLUSIVE );
		HANDLE_HRESULT( DXGI_ERROR_NOT_CURRENTLY_AVAILABLE );


		//
		// D3D DDI status codes
		//

		HANDLE_HRESULT( D3DDDIERR_WASSTILLDRAWING );
		HANDLE_HRESULT( D3DDDIERR_NOTAVAILABLE );
		HANDLE_HRESULT( D3DDDIERR_DEVICEREMOVED );
		HANDLE_HRESULT( D3DDDIERR_PRIVILEGEDINSTRUCTION );
		HANDLE_HRESULT( D3DDDIERR_INVALIDINSTRUCTION );
		HANDLE_HRESULT( D3DDDIERR_INVALIDHANDLE );
		HANDLE_HRESULT( D3DDDIERR_CANTEVICTPINNEDALLOCATION );
		HANDLE_HRESULT( D3DDDIERR_CANTRENDERLOCKEDALLOCATION );
		HANDLE_HRESULT( D3DDDIERR_INVALIDUSERBUFFER );
		HANDLE_HRESULT( D3DDDIERR_INCOMPATIBLEPRIVATEFORMAT );
		HANDLE_HRESULT( D3DDDIERR_UNSUPPORTEDOVERLAY );
		HANDLE_HRESULT( D3DDDIERR_UNSUPPORTEDOVERLAYFORMAT );


		//
		// All other status codes
		//

		default:
		{
			return CCustomTest::HResultToString( hResult );
		} break;
	};
};


////////////////////////////////////////////////////////////
//
// Diagnostic functions
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  LogDiagnostics
//
// Purpose:
// Logs information that might be handy in trying to
// troubleshoot a failure.
///////////////////////////////////////////////////////
template< typename AppClass >
HRESULT CDXGICustomTest< AppClass >::LogDiagnostics()
{
	//
	// Log whether there are any recent TDR events.
	//

	if ( m_bCheckForTDRs == true )
	{
		LogTimeoutDetectionEvents();
	};


	//
	// Call the base class implementation.
	//

	return CCustomTest::LogDiagnostics();
};


///////////////////////////////////////////////////////
// Function:  LogTimeoutDetectionEvents
//
// Purpose:
// Logs whether any Timeout Detection and Recovery
// (TDR) events have been logged since the start of the
// test case.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::LogTimeoutDetectionEvents()
{
	//
	// Check for a TDR in the event log.
	//

	DWORD TDREventIDs[] =
	{
		4097,
		4099,
		4101
	};

	DWORD MatchingEventID = 0;

	HRESULT hScanForTDREventResult = ScanEventLogForEvents
	(
		_T( "System" ),
		&m_TestCaseStartTime,
		ARRAYSIZE( TDREventIDs ),
		TDREventIDs,
		&MatchingEventID
	);

	if ( hScanForTDREventResult == S_OK )
	{
		WriteToLog
		(
			_T( "A timeout detection and recovery (TDR) event (%u) was detected in the System event log during since the start of the most recent test case.\n" ),
			MatchingEventID
		);
	}
	else if ( hScanForTDREventResult == S_FALSE )
	{
		WriteToLog
		(
			_T( "No timeout detection and recovery (TDR) events were detected during this test case." )
		);
	};


	return hScanForTDREventResult;
};


////////////////////////////////////////////////////////////
//
// Event log helper functions
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  ScanEventLogForEvents
//
// Purpose:
// Scans through the specified event log in search of
// an event with the specified event ID that occurs
// after the specified start time.
//
// Returns:
// S_OK     A matching event was found.
// S_FALSE  No matching event was found.
// E_xxxx   An error occurred.
///////////////////////////////////////////////////////
template < typename AppClass >
HRESULT CDXGICustomTest< AppClass >::ScanEventLogForEvents
(
	__in_opt LPCTSTR pEventLogName,
	__in SYSTEMTIME * pScanStartSystemTime,
	UINT cEventIDs,
	__in_ecount( cEventIDs ) DWORD * pEventIDs,
	__out_opt DWORD * pMatchingEventID
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	FILETIME ScanStartFileTime;
	SYSTEMTIME ScanStartLocalTime;

	HANDLE hEventLog = NULL;
	HANDLE hEventLogChangeEvent = NULL;
	BYTE * pEventRecordBuffer = NULL;

	const UINT nEventRecordBuffer = 4 * 1024;


	//
	// Validate parameters.
	//

	if ( pScanStartSystemTime == NULL )
	{
		hOverallResult = E_INVALIDARG;

		goto Cleanup;
	};


	//
	// Convert the starting system time to a file time.
	//

	SystemTimeToFileTime
	(
		pScanStartSystemTime,
		&ScanStartFileTime
	);

	SystemTimeToTzSpecificLocalTime
	(
		NULL,
		pScanStartSystemTime,
		&ScanStartLocalTime
	);


//#define FULL_DEBUG 1
#if defined(FULL_DEBUG)
	WriteToLog
	(
		_T( "CGraphicsTest::ScanEventLogForEvents:  Scanning for an event on or after %2.2u/%2.2u/%4.4u %2.2u:%2.2u:%2.2u." ),
		ScanStartLocalTime.wMonth,
		ScanStartLocalTime.wDay,
		ScanStartLocalTime.wYear,
		ScanStartLocalTime.wHour,
		ScanStartLocalTime.wMinute,
		ScanStartLocalTime.wSecond
	);
#endif


	//
	// Create an event handle to use for signalling updates
	// to the event log.
	//
	hEventLogChangeEvent = CreateEvent
	(
		NULL,
		TRUE,
		FALSE,
		NULL
	);

	if
	(
		hEventLogChangeEvent == NULL ||
		hEventLogChangeEvent == INVALID_HANDLE_VALUE
	)
	{
		DWORD CreateEventLogChangeEventError = ::GetLastError();

		WriteToLog
		(
			_T( "CGraphicsTest::ScanEventLogForEvents:  CreateEvent failed with error %u." ),
			CreateEventLogChangeEventError
		);

		if ( CreateEventLogChangeEventError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( CreateEventLogChangeEventError );
		};

		goto Cleanup;
	};


	//
	// Allocate an event record buffer.
	//

	pEventRecordBuffer = new BYTE[ nEventRecordBuffer ];

	if ( pEventRecordBuffer == NULL )
	{
		hOverallResult = E_OUTOFMEMORY;

		goto Cleanup;
	};

	ZeroMemory( pEventRecordBuffer, nEventRecordBuffer );


	//
	// Open the event log.
	//

	hEventLog = OpenEventLog
	(
		NULL,
		pEventLogName
	);

	if ( hEventLog == NULL )
	{
		DWORD OpenEventLogError = ::GetLastError();

		WriteToLog
		(
			1,
			_T( "CGraphicsTest::ScanEventLogForEvents:  OpenEventLog failed with error %u.\n" ),
			OpenEventLogError
		);

		if ( OpenEventLogError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( OpenEventLogError );
		};

		goto Cleanup;
	};


	//
	// Notify the event log that we want to be notified about
	// changes.
	//

	BOOL bNotifyChangeEventLogResult = NotifyChangeEventLog
	(
		hEventLog,
		hEventLogChangeEvent
	);

	if ( bNotifyChangeEventLogResult != TRUE )
	{
		DWORD NotifyChangeEventLogError = ::GetLastError();

		WriteToLog
		(
			_T( "CGraphicsTest::ScanEventLogForEvents:  NotifyChangeEventLog failed with error %u." ),
			NotifyChangeEventLogError
		);

		if ( NotifyChangeEventLogError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( NotifyChangeEventLogError );
		};

		goto Cleanup;
	};


	//
	// Wait around for changes to the event log.
	//
	// @N.B. [SEdmison]:
	// It may be the case that the target event hasn't been
	// logged yet.  So we'll vamp for a little while in order
	// to give the event log a chance to catch up.
	//

	/* DWORD WaitResult = */ WaitForSingleObject
	(
		hEventLogChangeEvent,
		5 * 1000
	);


	//
	// Determine how many events there are in the event log.
	//

	DWORD cEventLogRecords = 0;
	BOOL bGetEventCountResult = GetNumberOfEventLogRecords
	(
		hEventLog,
		&cEventLogRecords
	);

	if ( bGetEventCountResult != TRUE )
	{
		DWORD GetNumberOfEventLogRecordsError = ::GetLastError();

		WriteToLog
		(
			_T( "CGraphicsTest::ScanEventLogForEvents:  GetNumberOfEventLogRecords failed with error %u." )
		);

		if ( GetNumberOfEventLogRecordsError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( GetNumberOfEventLogRecordsError );
		};

		goto Cleanup;
	};

#if defined(FULL_DEBUG)
	WriteToLog
	(
		_T( "CGraphicsTest::ScanEventLogForEvents:  Found %u events in System event log." ),
		cEventLogRecords
	);
#endif


	//
	// Determine how which event record in the event log is the oldest.
	//

	DWORD iOldestEventLogRecord = 0;
	BOOL bGetOldestEventRecordResult = GetOldestEventLogRecord
	(
		hEventLog,
		&iOldestEventLogRecord
	);

	if ( bGetOldestEventRecordResult != TRUE )
	{
		DWORD GetOldestEventRecordError = ::GetLastError();

		WriteToLog
		(
			_T( "CGraphicsTest::ScanEventLogForEvents:  GetOldestEventLogRecord failed with error %u." )
		);

		if ( GetOldestEventRecordError != ERROR_SUCCESS )
		{
			hOverallResult = HRESULT_FROM_WIN32( GetOldestEventRecordError );
		};

		goto Cleanup;
	};

#if defined(FULL_DEBUG)
	WriteToLog
	(
		_T( "CGraphicsTest::ScanEventLogForEvents:  Oldest event record is %u." ),
		iOldestEventLogRecord
	);
#endif


	//
	// Starting from the last record in the log, scan backward
	// until we find an event that matches our target event ID,
	// until we pass the specified time stamp, or until we run
	// out of events to process (that is, we reached the
	// beginning of the log).
	//

	DWORD cBytesRead = 0;
	DWORD cBytesNeeded = 0;

	UINT cEventLogRecordsProcessed = 0;

	BOOL bContinue = TRUE;

	for
	(
		BOOL bReadEventResult = ReadEventLog
		(
			hEventLog,
			EVENTLOG_SEEK_READ |
				EVENTLOG_FORWARDS_READ,
			iOldestEventLogRecord,
			pEventRecordBuffer,
			nEventRecordBuffer,
			&cBytesRead,
			&cBytesNeeded
		);
		bContinue == TRUE;
		bReadEventResult = ReadEventLog
		(
			hEventLog,
			EVENTLOG_SEQUENTIAL_READ |
				EVENTLOG_FORWARDS_READ,
			0,
			pEventRecordBuffer,
			nEventRecordBuffer,
			&cBytesRead,
			&cBytesNeeded
		)
	)
	{
		if ( bReadEventResult == FALSE )
		{
			DWORD ReadEventLogError = ::GetLastError();

			if ( ReadEventLogError == ERROR_HANDLE_EOF )
			{
				// We've reached the end of the event log.

#if defined(FULL_DEBUG)
				WriteToLog
				(
					_T( "CGraphicsTest::ScanEventLogForEvents:  Reached end of event log after %u events." ),
					cEventLogRecordsProcessed
				);
#endif

				bContinue = FALSE;
				break;
			}
			else if ( ReadEventLogError == ERROR_INSUFFICIENT_BUFFER )
			{
				WriteToLog
				(
					1,
					_T( "CGraphicsTest::ScanEventLogForEvents:  ReadEventLog failed with error ERROR_INSUFFICIENT_BUFFER.  Needed %u bytes.\n" ),
					cBytesNeeded
				);

				hOverallResult = HRESULT_FROM_WIN32( ReadEventLogError );
				goto Cleanup;
			}
			else
			{
				WriteToLog
				(
					1,
					_T( "CGraphicsTest::ScanEventLogForEvents:  ReadEventLog failed with error %u.\n" ),
					ReadEventLogError
				);

				if ( ReadEventLogError != ERROR_SUCCESS )
				{
					hOverallResult = HRESULT_FROM_WIN32( ReadEventLogError );
				};

				goto Cleanup;
			};
		};


		if ( cBytesRead == 0 )
		{
			hOverallResult = S_FALSE;

			goto Cleanup;
		};


		for
		(
			EVENTLOGRECORD * pEventRecord = (EVENTLOGRECORD *) pEventRecordBuffer;
			pEventRecord != NULL &&
				(BYTE *) pEventRecord < ( pEventRecordBuffer + cBytesRead ) &&
				cEventLogRecordsProcessed <= cEventLogRecords;
			pEventRecord = (EVENTLOGRECORD *) ( (BYTE *) pEventRecord + pEventRecord->Length )
		)
		{
			//
			// Convert the event record's time offset to a FILETIME.
			//
			// @N.B. [SEdmison]:
			// Look a little strange?  Does to me, too.  This code
			// comes from MSDN.
			//
			// I sat down and thought about the math here, and I'm
			// not entirely sure how they're accounting for the
			// discontinuities in UTC (like the fact that it didn't
			// exist prior to 1972, much less all the way back to
			// 1601, or the addition and removal of leap-seconds).
			// In fact, I'm not sure how the rest of the OS deals
			// with these issues (if at all).
			//
			// For anyone who's read this far, my calculation was
			// ( 369 years between 1970 and 1601
			// * 365.25 days per year, which includes leap-days
			// - minus the three nonexistent leap-days for 1700, 1800, and 1900 ),
			// * 24 hours per day
			// * 60 minutes per hour
			// * 60 seconds per minute (ignoring leap-seconds)
			// * 1000 milliseconds per second
			// * 1000 microseconds per millisecond
			// * 10 100-nanosecond intervals per microsecond
			// for a grand total of
			//   116444952000000000
			// which differs from MSDN's value of
			//   116444736000000000
            // by 6 hours.  Not bad for a quick, back-of-the-
			// envelope calculation, but I'm still curious
			// where those other 6 hours went....
			//
			// I guess I'm going to have to add some fudge
			// factors in order to account for these differences.
			//

			FILETIME EventRecordFileTime;

			__int64 NumberOf100nsIntervalsFrom_1_1_1601_to_1_1_1970 = 116444736000000000;
			__int64 NumberOf100nsIntervalsFrom_1_1_1601_to_TimeGenerated = Int32x32To64
			(
				pEventRecord->TimeGenerated, // in seconds from 1/1/1970
				10000000 // 1000 milliseconds per second * 1000 microseconds per millisecond * 10 100-nanosecond intervals per microsecond
			) + NumberOf100nsIntervalsFrom_1_1_1601_to_1_1_1970;

			EventRecordFileTime.dwLowDateTime = (DWORD) NumberOf100nsIntervalsFrom_1_1_1601_to_TimeGenerated;
			EventRecordFileTime.dwHighDateTime = (DWORD) ( NumberOf100nsIntervalsFrom_1_1_1601_to_TimeGenerated >> 32 );

			SYSTEMTIME EventRecordSystemTime;
			SYSTEMTIME EventRecordLocalTime;

			FileTimeToSystemTime
			(
				&EventRecordFileTime,
				&EventRecordSystemTime
			);

			SystemTimeToTzSpecificLocalTime
			(
				NULL,
				&EventRecordSystemTime,
				&EventRecordLocalTime
			);


			//
			// Dump information about the record.
			//

#if defined(FULL_DEBUG)
			WriteToLog
			(
				_T( "CGraphicsTest::ScanEventLogForEvents:  Event record %u:  ID %u, generated at %2.2u/%2.2u/%4.4u %2.2u:%2.2u:%2.2u." ),
				pEventRecord->RecordNumber,
				pEventRecord->EventID,
				EventRecordLocalTime.wMonth,
				EventRecordLocalTime.wDay,
				EventRecordLocalTime.wYear,
				EventRecordLocalTime.wHour,
				EventRecordLocalTime.wMinute,
				EventRecordLocalTime.wSecond
			);
#endif


			//
			// Check whether the event falls prior to the start
			// of our scan interval by more than 10 seconds.
			//
			// N.B. [SEdmison]:
			// That 10 second difference is the fudge factor I
			// mentioned above.
			//

			if
			(
				EventRecordFileTime.dwHighDateTime < ScanStartFileTime.dwHighDateTime ||
				EventRecordFileTime.dwHighDateTime == ScanStartFileTime.dwHighDateTime &&
				EventRecordFileTime.dwLowDateTime < ScanStartFileTime.dwLowDateTime - 10 * 10000000
			)
			{
#if 0
#if defined(FULL_DEBUG)
				WriteToLog
				(
					_T( "CGraphicsTest::ScanEventLogForEvents:  Found an event earlier than our scan window after processing %u records." ),
					cEventLogRecordsProcessed
				);
#endif

				bContinue = FALSE;
				break;
#else
				++cEventLogRecordsProcessed;
				continue;
#endif
			};


			//
			// Check whether the event matches the set for
			// which we're scanning.
			//

			for
			(
				UINT iCurrentEventID = 0;
				iCurrentEventID < cEventIDs;
				++iCurrentEventID
			)
			{
				if ( pEventRecord->EventID == pEventIDs[ iCurrentEventID ] )
				{
					if ( pMatchingEventID != NULL )
					{
						*pMatchingEventID = pEventIDs[ iCurrentEventID ];
					};

					hOverallResult = S_OK;

					goto Cleanup;
				};
			};


			//
			// Keep a count of how many records we've scanned.
			//

			++cEventLogRecordsProcessed;
		};
	};


	hOverallResult = S_FALSE;


Cleanup:
	if ( pEventRecordBuffer != NULL )
	{
		delete [] pEventRecordBuffer;
		pEventRecordBuffer = NULL;
	};


	if
	(
		hEventLogChangeEvent != NULL &&
		hEventLogChangeEvent != INVALID_HANDLE_VALUE
	)
	{
		CloseHandle( hEventLogChangeEvent );
		hEventLogChangeEvent = NULL;
	};


	if ( hEventLog != NULL )
	{
		CloseEventLog( hEventLog );
		hEventLog = NULL;
	};


	return hOverallResult;
};
