#include "SurfacePCH.h"

//#define STRICT
#include "CSurface.h"


// -------------------------------------------------------------------------------------

CnSurface::~CnSurface()
{
	if( !m_pSurfaces )
		return;

	for( UINT i= 0; i< m_uDevices; i++ )
	{
		RELEASE( m_pSurfaces[i] );
	}
	delete[] m_pSurfaces;
}

// -------------------------------------------------------------------------------------

bool CnSurface::Init(UINT uDevices)
{
	m_pSurfaces = new CSurface*[uDevices];
	
	if( m_pSurfaces == NULL )
		return false;

	m_uDevices = uDevices;
	memset( m_pSurfaces, 0, uDevices * sizeof( CSurface*) );

	/*if ( !m_DCHandles.Setup(m_uDevices) )
	{
		//DPF(1, _T("CnSurface::Init() - Failed to setup DC handles.\n"));
		return false;
	}
*/
	return true;
}

// -------------------------------------------------------------------------------------

bool CnSurface::GetSurface(CSurface **ppSurface, UINT n)
{
	if( n >= m_uDevices || ppSurface == NULL )
		return false;

	m_pSurfaces[n]->AddRef();
	*ppSurface = m_pSurfaces[n];
	return true;
}

// -------------------------------------------------------------------------------------

bool CnSurface::SetSurface(CSurface *pSurface, UINT n)
{
	if( n >= m_uDevices )
		return false;

	if( m_pSurfaces[n] != NULL )
		return false;

	m_pSurfaces[n] = pSurface;
	return true;
}

// -------------------------------------------------------------------------------------
/*
bool CnSurface::GetDC(CHandle *pDCHandles)
{
	if( !m_pSurfaces )
		return;

	HDC hDC = NULL;
	for( UINT i= 0; i< m_uDevices; i++ )
	{
		if ( FAILED( m_pSurfaces[i]->GetDC(&hDC) ) )
		{
			return false;
		}
		if ( !m_DCHandles.AddHandle(&hDC) )
		{
			return false;
		}
	}

	*pDCHandles = &m_DCHandles;
	return true;
}

// -------------------------------------------------------------------------------------

bool CnSurface::ReleaseDC(CHandle *pDCHandles)
{
	if( n> m_uDevices )
		return false;

	if( m_pSurfaces[n] != NULL )
		return false;

	m_pSurfaces[n] = pSurface;
	return true;
}*/
