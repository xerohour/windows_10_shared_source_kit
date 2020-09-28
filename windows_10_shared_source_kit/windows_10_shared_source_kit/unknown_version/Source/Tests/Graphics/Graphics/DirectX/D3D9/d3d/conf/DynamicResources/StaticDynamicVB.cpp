// StaticDynamicVB.cpp: implementation of the CStaticDynamicVBTest class.
//
//////////////////////////////////////////////////////////////////////

#include "StaticDynamicVB.h"


typedef struct
{
	float x, y, z;
	float nx, ny, nz;
	D3DCOLOR color;
} D3DVERTEXCOLOR;

typedef struct
{
	float tu, tv;
} D3DVERTEXTEXTURE;


//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CStaticDynamicVBTest::CStaticDynamicVBTest() :
	m_pStaticVertexBuffers( NULL )
{
	m_szTestName	= _T("StaticDynamicVB");
	m_szCommandKey	= _T("StaticDynamicVB");
}

CStaticDynamicVBTest::~CStaticDynamicVBTest()
{
}


//////////////////////////////////////////////////////////////////////
// Name:	BuildTestVectors()
// Desc:	virtual method for each test to initialize its vector list
//////////////////////////////////////////////////////////////////////
void CStaticDynamicVBTest::BuildTestVectors()
{
#if FAST_TEST
    DWORD arraydwLockNo[]			= {1, 2};
    DWORD arraydwFlagsNo[]			= { D3DLOCK_DISCARD, D3DLOCK_NOOVERWRITE, 0 };
    DWORD arraydwOffsetNo[]			= {0, 192};
    DWORD arraydwVerticesNo[]		= {24};
#else//FAST_TEST
	DWORD arraydwLockNo[]			= {1, 2, 3, 4};
	DWORD arraydwFlagsNo[]			= { D3DLOCK_DISCARD, D3DLOCK_NOOVERWRITE, 0 };
	DWORD arraydwOffsetNo[]			= {0, 48, 144, 192};
	DWORD arraydwVerticesNo[]		= {24};
#endif

	m_arraydwLockNo		= vector<DWORD>(arraydwLockNo, arraydwLockNo + sizeof(arraydwLockNo) / sizeof DWORD);
	m_arraydwFlagsNo	= vector<DWORD>(arraydwFlagsNo, arraydwFlagsNo + sizeof(arraydwFlagsNo) / sizeof DWORD);
	m_arraydwOffsetNo	= vector<DWORD>(arraydwOffsetNo, arraydwOffsetNo + sizeof(arraydwOffsetNo) / sizeof DWORD);
	m_arraydwVerticesNo = vector<DWORD>(arraydwVerticesNo, arraydwVerticesNo + sizeof(arraydwVerticesNo) / sizeof DWORD);
}


//////////////////////////////////////////////////////////////////////
// Name:	CopyDynamicVertex( void*, D3DVERTEX*, D3DCOLOR )
// Desc:	copy the shape vertex into the dynamic Vertex
//////////////////////////////////////////////////////////////////////
void CStaticDynamicVBTest::CopyDynamicVertex( void* pDynamicVertex, D3DVERTEX* pShapeVertex, D3DCOLOR color )
{
	D3DVERTEXCOLOR *pMyDynamicVertex = (D3DVERTEXCOLOR *)pDynamicVertex;
	pMyDynamicVertex->x = pShapeVertex->x;
	pMyDynamicVertex->y = pShapeVertex->y;
	pMyDynamicVertex->z = pShapeVertex->z;
	pMyDynamicVertex->nx = pShapeVertex->nx;
	pMyDynamicVertex->ny = pShapeVertex->ny;
	pMyDynamicVertex->nz = pShapeVertex->nz;
	pMyDynamicVertex->color = color;
}


//////////////////////////////////////////////////////////////////////
// Name:	ConfirmDevice( CAPS*, DWORD, FORMAT )
// Desc:	Called during device initialization, this code checks the device for some minimum set of capabilities
//////////////////////////////////////////////////////////////////////
HRESULT CStaticDynamicVBTest::ConfirmDevice( CAPS* pCaps, DWORD dwBehavior, FORMAT Format )
{
	HRESULT hResult = D3D_OK;
	if( SUCCEEDED( hResult = CDynamicVBTest::ConfirmDevice( pCaps, dwBehavior, Format ) ) )
	{
		if( pCaps->MaxStreams < 2 )
		{
			WriteToLog( _T( "Device doesn't support 2 vertex streams, skipping test." ) );
			hResult = D3DERR_NOTAVAILABLE;
		}
	}
	return hResult;
}


//////////////////////////////////////////////////////////////////////
// Name:	OneTimeSceneInit()
// Desc:	Called during initial app startup, this function performs all the permanent initialization
//////////////////////////////////////////////////////////////////////
HRESULT CStaticDynamicVBTest::OneTimeSceneInit()
{
	if( SUCCEEDED( CDynamicVBTest::OneTimeSceneInit() ) )
	{
		m_DynamicStride = sizeof D3DVERTEXCOLOR;
		m_StaticStride = sizeof D3DVERTEXTEXTURE;
		VERTEXELEMENT D3DVERTEX_Elements[] =		// dynamic vertex elements
		{
			// Stream	Offset	Type					Method					Usage					UsageIndex
			{	0,		0,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_POSITION,	0},
			{	0,		12,		D3DDECLTYPE_FLOAT3,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_NORMAL,	0},
			{	0,		24,		D3DDECLTYPE_D3DCOLOR,	D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_COLOR,		0},
			{	1,		0,		D3DDECLTYPE_FLOAT2,		D3DDECLMETHOD_DEFAULT,	D3DDECLUSAGE_TEXCOORD,	0},
			D3DDECL_END()
		};
		m_VertexElements = vector<VERTEXELEMENT>( D3DVERTEX_Elements, D3DVERTEX_Elements + sizeof(D3DVERTEX_Elements) / sizeof VERTEXELEMENT );

		DWORD dwDecl[] =
		{
			D3DVSD_STREAM(0),
			D3DVSD_REG( D3DVSDE_POSITION,	D3DVSDT_FLOAT3 ),
			D3DVSD_REG( D3DVSDE_NORMAL,		D3DVSDT_FLOAT3 ),
			D3DVSD_REG( D3DVSDE_DIFFUSE,	D3DVSDT_D3DCOLOR ),
			D3DVSD_STREAM(1),
			D3DVSD_REG( D3DVSDE_TEXCOORD0,	D3DVSDT_FLOAT2 ),
			D3DVSD_END()
		};
		m_p81VertexDeclaration = vector<DWORD>( dwDecl, dwDecl + sizeof( dwDecl ) / sizeof DWORD );
	}

	return D3D_OK;
}


//////////////////////////////////////////////////////////////////////
// Name:	RestoreDeviceObjects()
// Desc:	Restore device-memory objects and state after a device is created or resized.
//////////////////////////////////////////////////////////////////////
HRESULT CStaticDynamicVBTest::RestoreDeviceObjects()
{
	HRESULT hResult = D3D_OK;
	if( SUCCEEDED( hResult = CDynamicVBTest::RestoreDeviceObjects() ) )
	{
		try
		{
			// create and fill the static vb
			if( !CreateVertexBuffer(	m_AllShapes.m_nVertices * m_StaticStride,
										USAGE_WRITEONLY,
										0,
										POOL_DEFAULT,
										&m_pStaticVertexBuffers ) )
				throw E_FAIL;

			D3DVERTEXTEXTURE *pVertixes;
			if( !m_pStaticVertexBuffers->Lock( 0, 0, (void**)&pVertixes, 0 ) )
				throw E_FAIL;
			for( int iVertex = 0; iVertex < m_AllShapes.m_nVertices; iVertex++ )
			{
				pVertixes->tu = (m_AllShapes.m_pVertices + iVertex)->tu;
				pVertixes->tv = (m_AllShapes.m_pVertices + iVertex)->tv;
				pVertixes++;
			}
			if( !m_pStaticVertexBuffers->Unlock() )
				throw E_FAIL;

			// set the static vb
			if( !SetStreamSource( 1, m_pStaticVertexBuffers, m_StaticStride, 0 ) )
				throw E_FAIL;
		}
		catch( HRESULT hError )
		{
			return hError;
		}
	}

	return hResult;
}


//////////////////////////////////////////////////////////////////////
// Name:	InvalidateDeviceObjects()
// Desc:	Called when the device-dependent objects are about to be lost.
//////////////////////////////////////////////////////////////////////
HRESULT CStaticDynamicVBTest::InvalidateDeviceObjects()
{
	CDynamicVBTest::InvalidateDeviceObjects();
	RELEASE( m_pStaticVertexBuffers );

	return D3D_OK;
}
