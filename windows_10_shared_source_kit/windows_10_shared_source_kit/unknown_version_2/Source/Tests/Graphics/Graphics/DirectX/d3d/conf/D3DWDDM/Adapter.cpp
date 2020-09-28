// Includes
#include "Adapter.h"

////////////////////////

CAdapter::CAdapter(IDXGIAdapter1 *pAdapter)
	: m_pAdapter1( nullptr )
	, m_pAdapterDescriptor (nullptr)
{
	// Setup
	Setup( pAdapter );
}

/////

CAdapter::~CAdapter()
{
	Cleanup();
}

/////

bool CAdapter::IsValid() const
{
	// Local variables
	bool bValid = true;

	if( !m_pAdapter1 )
	{
		bValid = false;
	}

	if( !m_pAdapterDescriptor )
	{
		bValid = false;
	}

	return bValid;
}

/////

tstring CAdapter::GetUserModeDriverVersionStr( const LARGE_INTEGER &userModeDriverVersion )
{
	// Local variables
	tstring userModeDriverVersionStr = _T("");
	userModeDriverVersionStr += ToString( HIWORD( userModeDriverVersion.HighPart ) ) + _T(".");	// Product
	userModeDriverVersionStr += ToString( LOWORD( userModeDriverVersion.HighPart ) ) + _T(".");	// Version
	userModeDriverVersionStr += ToString( HIWORD( userModeDriverVersion.LowPart ) ) + _T(".");	// Sub-Version
	userModeDriverVersionStr += ToString( LOWORD( userModeDriverVersion.LowPart ) );			// Build
	return userModeDriverVersionStr;
}

/////

void CAdapter::Setup(IDXGIAdapter1 *pAdapter)
{
	// Local variables
	HRESULT hr = S_OK;

	// Cleanup first
	Cleanup();

	// Setup the Adapter (automatically cleaned up if already set: smart pointer)
	m_pAdapter1 = pAdapter;

	// Setup the AdapterDescriptor
	assert( m_pAdapterDescriptor == nullptr );
	m_pAdapterDescriptor = DevX::GRFX::AdapterDescriptorCore::GetAdapterDescriptor( m_pAdapter1 );

	// Setup the User Mode Driver Version
	ZeroMemory( &m_userModeDriverVersion, sizeof(m_userModeDriverVersion) );
	hr = m_pAdapter1->CheckInterfaceSupport( __uuidof( IDXGIDevice ), &m_userModeDriverVersion );
}

/////

void CAdapter::Cleanup()
{
	// Cleanup the AdapterDescriptor
	if( m_pAdapterDescriptor )
	{
		delete m_pAdapterDescriptor;
		m_pAdapterDescriptor = nullptr;
	}

	// No need to cleanup the Adapter (smart pointer)
}