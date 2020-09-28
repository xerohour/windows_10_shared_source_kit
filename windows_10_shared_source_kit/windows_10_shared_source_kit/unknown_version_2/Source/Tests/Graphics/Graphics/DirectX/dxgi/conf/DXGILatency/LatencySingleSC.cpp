//
// Project headers
//

#include "TestApp.h"


#pragma warning(disable: 4355)

const DXGI_FORMAT CDXGILatencySingleSC::s_dxgifmtBBInitTexture = DXGI_FORMAT_R8G8B8A8_UNORM;

CDXGILatencySingleSC::CDXGILatencySingleSC()
:
	m_PrimaryAdapterDevice( (CTest *) this ),
	m_TestedSwapChain( (CTest *) this, &m_PrimaryAdapterDevice ),
	m_hwndFocusWindow( NULL ),
	m_hwndDeviceWindow( NULL ),
	m_pFactory( NULL ),
	m_pD3D10Device( NULL ),
	m_pSwapChain( NULL ),
	m_pDeviceWindowOutput( NULL )
{
    ZeroMemory(&m_SCDesc, sizeof(m_SCDesc));
};


CDXGILatencySingleSC::~CDXGILatencySingleSC()
{
};


void CDXGILatencySingleSC::ReleaseTestCaseObjects()
{
	DebugOut.Write("Entering ReleaseTestCaseObjects.\n");


	if ( m_pDeviceWindowOutput != NULL )
	{
		m_pDeviceWindowOutput->Release();
		m_pDeviceWindowOutput = NULL;
	};


	if ( m_pSwapChain != NULL )
	{
		m_pSwapChain->Release();
		m_pSwapChain = NULL;
	};


	m_TestedSwapChain.ReleaseAllObjects();

	if ( m_pFactory != NULL )
	{
		HWND hwndCurrentFocus = NULL;

		m_pFactory->GetWindowAssociation( &hwndCurrentFocus );
		if ( hwndCurrentFocus != NULL )
		{
			m_pFactory->MakeWindowAssociation
			(
				NULL,
				0
			);
		};

		m_pFactory->Release();
		m_pFactory = NULL;
	};


	if ( m_hwndDeviceWindow != NULL )
	{
		if ( m_hwndDeviceWindow != m_hwndFocusWindow )
		{
			DestroyWindow( m_hwndDeviceWindow );
		};

		m_hwndDeviceWindow = NULL;
	};


	if ( m_hwndFocusWindow != NULL )
	{
		DestroyWindow( m_hwndFocusWindow );
		m_hwndFocusWindow = NULL;
	};


	if ( m_pD3D10Device != NULL )
	{
		m_pD3D10Device->Release();
		m_pD3D10Device = NULL;
	};


	m_PrimaryAdapterDevice.ReleaseAllObjects();

	DebugOut.Write("Exiting ReleaseTestCaseObjects.\n");
};


////////////////////////////////////////////////////////////////////////////////
// clsTestableDevice Methods

clsTestableDevice::~clsTestableDevice()
{
};


bool clsTestableDevice::GetDevicePtr
(
	ID3D10Device ** pDevicePointer )
{
	if ( pD3D10Device != NULL )
	{
		pD3D10Device->AddRef();
	};

	*pDevicePointer = pD3D10Device;

	return true;
};


////////////////////////////////////////////////////////////////////////////////
// clsTestableDXGISwapChain Methods

clsTestableDXGISwapChain::~clsTestableDXGISwapChain()
{
};


bool clsTestableDXGISwapChain::GetSwapChainPtr
(
	IDXGISwapChain ** pSwapChainPointer
)
{
	if ( pSwapChain != NULL )
	{
		pSwapChain->AddRef();
	};

	*pSwapChainPointer = pSwapChain;

	return true;
};
