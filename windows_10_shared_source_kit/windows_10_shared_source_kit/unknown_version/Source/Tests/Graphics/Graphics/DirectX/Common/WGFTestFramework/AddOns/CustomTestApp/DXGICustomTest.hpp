//////////////////////////////////////////////////////////////////////
// File:  DXGICustomTest.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for custom D3D/DXGI test group object.
//
// History:
// 10 Mar 2006  SEdmison  Factored from other tests into a shared lib.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CReferenceCountSentinel
//
// Purpose:
// COM reference count sentinel object.
//
// Notes:
// Implements the COM IUnknown interface in a way that
// lets a test case keep track of references to the
// object (and thus to spot places where the reference
// count is higher or lower than expected at a given
// point in the execution of the test).
//
// Source:
// Swiped from Brian Klamik and modified slightly.
///////////////////////////////////////////////////////
class CReferenceCountSentinel
:
    public IUnknown
{
	//
	// Data members
	//

	public:
		LONG m_cReferences;


	//
	// Construction/destruction
	//

	public:
		CReferenceCountSentinel()
		:
			m_cReferences( 0 )
		{
		};


	//
	// IUnknown interface methods
	//

	public:
		STDMETHOD( QueryInterface )
		(
			REFIID riid,
			void** ppvObject
		)
		{
			if ( ppvObject == NULL )
			{
				return E_POINTER;
			};

			*ppvObject = NULL;

			if ( riid == IID_IUnknown )
			{
				AddRef();

				*ppvObject = this;

				return S_OK;
			};

			return E_NOINTERFACE;
		};

		STDMETHOD_( ULONG, AddRef )()
		{
			return (ULONG) InterlockedIncrement( &m_cReferences );
		};

		STDMETHOD_( ULONG, Release )()
		{
			return (ULONG) InterlockedDecrement( &m_cReferences );
		};
};


///////////////////////////////////////////////////////
// Class:  CDXGICustomTest
//
// Purpose:
// DXGI custom test group object.
//
// Notes:
// Adds functionality for initializing DXGI/D3D10 and
// correlating DXGI adapters to GDI display devices.
///////////////////////////////////////////////////////
template < typename AppClass >
class CDXGICustomTest
:
	public CCustomTest< AppClass, CDXGITest >
{
	//
	// Construction/destruction
	//

	public:
		CDXGICustomTest
		(
			AppClass * pApplication
		);

		//virtual ~CDXGICustomTest();


	//
	// Framework override methods
	//

	public:
		virtual TEST_RESULT SetupTestCase();


	//
	// Direct3D initialization and cleanup methods
	//

	public:
		HRESULT LoadD3D10();

		HRESULT UnloadD3D10();


	//
	// Device management functions
	//

	public:
		// Frees Direct3D 10 and DXGI resources.
		HRESULT CleanupResources();


		// Creates a DXGI adapter in the specified adapter slot.
		HRESULT CreateAdapter
		(
			UINT FactorySlot,
			UINT AdapterSlot,
			UINT AdapterOrdinal,
			D3D10_DRIVER_TYPE DriverType,
			HMODULE hRasterizerDLL,
			__out_opt UINT * pcConnectedOutputs
		);


		// Creates a Direct3D 10 device in the specified device slot.
		HRESULT CreateDevice
		(
			UINT FactorySlot,
			UINT AdapterSlot,
			UINT AdapterOrdinal,
			UINT DeviceSlot,
			D3D10_DRIVER_TYPE DriverType,
			TEST_FEATURE_LEVEL TestFeatureLevel,
			bool bIgnoreRequestedFeatureLevels,
			__out_opt bool * pbAdapterIsRemote
		);

		HRESULT CreateDevice
		(
			UINT FactorySlot,
			UINT AdapterSlot,
			UINT AdapterOrdinal,
			UINT DeviceSlot,
			D3D10_DRIVER_TYPE DriverType
		);


		// Creates a Direct3D 10 device in the specified device slot
		// and a DXGI swap chain in the specified swap chain slot.
		HRESULT CreateDeviceAndSwapChain
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
			BOOL bMakeWindowAssociation = FALSE,
			UINT MakeWindowAssociationFlags = 0,
            DXGI_SWAP_EFFECT SwapEffect = DXGI_SWAP_EFFECT_DISCARD
		);


		// Creates a DXGI Swap chain in the specified swap chain slot.
		HRESULT CreateSwapChain
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
            DXGI_SWAP_EFFECT SwapEffect = DXGI_SWAP_EFFECT_DISCARD
		);


/*
		HRESULT FindAdapterAndOutputForMonitorSlot
		(
			D3D10_DRIVER_TYPE DriverType,
			UINT FactorySlot,
			UINT AdapterSlot,
			UINT MonitorSlot,
			__out IDXGIOutput ** ppOutput,
			BOOL bExclusive
		);


		HRESULT GetOutputForMonitorSlot
		(
			D3D10_DRIVER_TYPE DriverType,
			UINT FactorySlot,
			UINT MonitorSlot,
			__out IDXGIOutput ** pOutput
		);
*/


		HRESULT GetFullscreenState
		(
			UINT SwapChainSlot,
			__out BOOL * pbFullScreen,
			__out_opt IDXGIOutput ** ppDXGIOutput
		);


		HRESULT ResizeBackBuffers
		(
			UINT DeviceSlot,
			UINT SwapChainSlot,
			UINT cBuffers,
			UINT Width,
			UINT Height,
			DXGI_FORMAT NewFormat,
			DWORD Flags
		);


		HRESULT SetFullscreenState
		(
			UINT FactorySlot,
			UINT SwapChainSlot,
			BOOL bFullScreen,
			__in_opt IDXGIOutput * pTargetOutput,
			BOOL bSetWindowAssociation = TRUE,
			UINT MakeWindowAssociationFlags = 0
		);


		HRESULT SetRefDeviceMask
		(
			BOOL bEnable,
			DWORD GDIDisplayDeviceMask
		);


		HRESULT SetRenderTarget
		(
			UINT DeviceSlot,
			UINT SwapChainSlot,
			DXGI_FORMAT Format
		);


	//
	// Rendering functions
	//

	public:
		// Boilerplate Present code.
		HRESULT PresentScene
		(
			UINT SwapChainSlot,
			UINT SyncInterval = 0,
			UINT PresentFlags = 0
		);


		// Boilerplate rendering code.
		HRESULT RenderScene
		(
			UINT DeviceSlot,
			UINT SwapChainSlot
		);


	//
	// Window management functions
	//

	public:
		HRESULT CreateDeviceWindow
		(
			UINT WindowSlot,
			int WindowLeft,
			int WindowTop,
			int WindowWidth,
			int WindowHeight,
			BOOL bWindowed,
			BYTE Alpha
		);

		// Boilerplate ShowWindow/UpdateWindow code.
		HRESULT DisplayWindow
		(
			UINT WindowSlot,
			BOOL bWindowed,
			BOOL bBringToFront = TRUE
		);


	//
	// Miscellaneous helper methods
	//

	public:
		virtual int GetAngleFromDXGIOrientation
		(
			DXGI_MODE_ROTATION DisplayOrientation
		);


		virtual __override LPCTSTR HResultToString
		(
			HRESULT hResult
		) const;


		// Log diagnostic information.
		HRESULT LogDiagnostics();

		// Log whether there are any recent TDR events.
		HRESULT LogTimeoutDetectionEvents();

		virtual HRESULT ScanEventLogForEvents
		(
			__in_opt LPCTSTR pEventLogName,
			__in SYSTEMTIME * pScanStartSystemTime,
			UINT cEventIDs,
			__in_ecount( cEventIDs ) DWORD * pEventIDs,
			__out_opt DWORD * pMatchingEventID
		);


	//
	// Data members
	//

	public:
		//
		// GDI members
		//

		// Window class.
		WNDCLASSEX m_WindowClass;

		// Window handles.
		HWND m_phWindows[ 10 ];


		//
		// DXGI members
		//

		// DXGI factory objects.
		DXGI_FACTORY_POINTER m_Factories[ 10 ];

		// DXGI adapter objects.
		DXGI_ADAPTER_POINTER m_Adapters[ 10 ];

		// Direct3D 10 device objects.
		D3D_DEVICE_POINTER m_Devices[ 10 ];

		// DXGI swap chain objects.
		IDXGISwapChain * m_ppSwapChains[ 10 ];


		//
		// Direct3D members
		//

		// Direct3D device reference count sentinel objects.
		CReferenceCountSentinel m_pDeviceReferenceCountSentinels[ 10 ];

		// Direct3D render target view objects.
		ID3DRenderTargetView * m_ppRenderTargetViews[ 10 ];


		//
		// Diagnostic members
		//

		bool m_bCheckForTDRs;

		SYSTEMTIME m_TestCaseStartTime;
};
