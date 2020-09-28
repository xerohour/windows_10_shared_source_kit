#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "ResMan.h"

bool CBasicTest::Setup()
{
	DWORD   uHeight = m_Options.D3DOptions.nBackBufferHeight;
	DWORD   uWidth  = m_Options.D3DOptions.nBackBufferWidth;
	HRESULT hr;
	WORD    index;
	DWORD   dindex;
	DWORD   priority, dwTemp;
	WORD    randCount, randFlag;

	ResMan::Setup();

	D3DTLVERTEX V1[8];    
	V1[0] = D3DTLVERTEX(D3DXVECTOR3(          0.f, (float)uHeight, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	V1[1] = D3DTLVERTEX(D3DXVECTOR3(          0.f,       0, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	V1[2] = D3DTLVERTEX(D3DXVECTOR3(   uWidth/3.f, (float)uHeight, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	V1[3] = D3DTLVERTEX(D3DXVECTOR3(   uWidth/3.f,       0, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	V1[4] = D3DTLVERTEX(D3DXVECTOR3( 2*uWidth/3.f, (float)uHeight, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	V1[5] = D3DTLVERTEX(D3DXVECTOR3( 2*uWidth/3.f,       0, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	V1[6] = D3DTLVERTEX(D3DXVECTOR3(       (float)uWidth, (float)uHeight, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	V1[7] = D3DTLVERTEX(D3DXVECTOR3(       (float)uWidth,	   0, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	hr = m_pSrcDevice->CreateVertexBuffer( 8*sizeof(D3DTLVERTEX), 0, D3DFVF_TLVERTEX, POOL_MANAGED, &m_pTLVertexBuffer);
	if(FAILED(hr))
	{
		WriteToLog("Failed to create m_pTLVertexBuffer hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* pV1 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		hr = m_pTLVertexBuffer->Lock( 0, sizeof(V1), (BYTE**)&pV1, randFlag == 0 ? 0 : LOCK_READONLY );
		if (FAILED(hr))
		{
			for (WORD i = 0; i < index; i++)
				m_pTLVertexBuffer->Unlock();
			WriteToLog("Failed to lock m_pTLVertexBuffer on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	
	ASSERT(pV1 != NULL);
	memcpy( pV1, V1, sizeof(V1) );

	for (index = 0; index < randCount; index++)
	{
		hr = m_pTLVertexBuffer->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_pTLVertexBuffer on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if (m_dwVersion >= 0x0800)
	if( !PrioritiesVB( m_pTLVertexBuffer, &priority) || (dwTemp = m_pTLVertexBuffer->GetPriority()) != priority )
    {
        WriteToLog("GetPriority() on m_pTLVertexBuffer returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	LVERTEX V2[8];    
	V2[0] = LVERTEX(D3DXVECTOR3(    1.f, -1.f, 0.5f), 0x000000ff, 0, 0.0f, 0.0f);
	V2[1] = LVERTEX(D3DXVECTOR3(    1.f,  1.f, 0.5f), 0x000000ff, 0, 0.0f, 0.0f);
	V2[2] = LVERTEX(D3DXVECTOR3(  0.33f, -1.f, 0.5f), 0x000000ff, 0, 0.0f, 0.0f);
	V2[3] = LVERTEX(D3DXVECTOR3(  0.33f,  1.f, 0.5f), 0x000000ff, 0, 0.0f, 0.0f);
	V2[4] = LVERTEX(D3DXVECTOR3( -0.33f, -1.f, 0.5f), 0x000000ff, 0, 0.0f, 0.0f);
	V2[5] = LVERTEX(D3DXVECTOR3( -0.33f,  1.f, 0.5f), 0x000000ff, 0, 0.0f, 0.0f);
	V2[6] = LVERTEX(D3DXVECTOR3(   -1.f, -1.f, 0.5f), 0x000000ff, 0, 0.0f, 0.0f);
	V2[7] = LVERTEX(D3DXVECTOR3(   -1.f,  1.f, 0.5f), 0x000000ff, 0, 0.0f, 0.0f);
	hr = m_pSrcDevice->CreateVertexBuffer( 8*GetVertexSize(FVF_LVERTEX), 0, FVF_LVERTEX, POOL_MANAGED, &m_pLVertexBuffer);
	if( FAILED(hr) )
	{
		WriteToLog("Failed to create m_pLVertexBuffer hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* pV2 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		hr = m_pLVertexBuffer->Lock( 0, sizeof(V2), (BYTE**)&pV2, randFlag == 0 ? 0 : LOCK_READONLY );
		if (FAILED(hr))
		{
			for (WORD i = 0; i < index; i++)
				m_pLVertexBuffer->Unlock();
			WriteToLog("Failed to lock m_pLVertexBuffer on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}

	ASSERT(pV2 != NULL);
	memcpy( pV2, V2, sizeof(V2) );

	for (index = 0; index < randCount; index++)
	{
		hr = m_pLVertexBuffer->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_pLVertexBuffer on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if (m_dwVersion >= 0x0800)
	if( !PrioritiesVB( m_pLVertexBuffer, &priority) || (dwTemp = m_pLVertexBuffer->GetPriority()) != priority )
    {
        WriteToLog("GetPriority() on m_pLVertexBuffer returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	VERTEX V3[36]; 
	FLOAT tu = 0.f;//min( 1, 0.5f*m_Options.D3DOptions.nBackBufferWidth/256);
	FLOAT tv = 0.f;//min( 1, 1.f*m_Options.D3DOptions.nBackBufferHeight/256);
	V3[0]  = VERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   0.f,  1.f);
	V3[1]  = VERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   0.f,  0.f);
	V3[2]  = VERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   1.f,  1.f);
	V3[3]  = VERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   1.f,  0.f);
	V3[4]  = VERTEX(D3DXVECTOR3(  1.f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f,  tv);//0.5f);
	V3[5]  = VERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f);
	V3[6]  = VERTEX(D3DXVECTOR3( 0.5f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu,  tv);//0.25f, 0.5f);
	V3[7]  = VERTEX(D3DXVECTOR3( 0.5f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu, 0.f);//0.25f,  0.f);
	V3[8]  = VERTEX(D3DXVECTOR3( 0.5f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f,  tv); //0.25f, 0.5f);
	V3[9]  = VERTEX(D3DXVECTOR3( 0.5f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f);// 0.25f,  0.f);
	V3[10] = VERTEX(D3DXVECTOR3(  0.f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu,  tv);//  0.5f, 0.5f);
	V3[11] = VERTEX(D3DXVECTOR3(  0.f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu, 0.f);//  0.5f,  0.f);
	V3[12] = VERTEX(D3DXVECTOR3(  0.f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f,  tv);
	V3[13] = VERTEX(D3DXVECTOR3(  0.f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f);
	V3[14] = VERTEX(D3DXVECTOR3(-0.5f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu,  tv);
	V3[15] = VERTEX(D3DXVECTOR3(-0.5f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu, 0.f);
	V3[16] = VERTEX(D3DXVECTOR3(-0.5f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f,  tv);
	V3[17] = VERTEX(D3DXVECTOR3(-0.5f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f);
	V3[18] = VERTEX(D3DXVECTOR3( -1.f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu,  tv);
	V3[19] = VERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu, 0.f);
	V3[20] = VERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f,  tv);
	V3[21] = VERTEX(D3DXVECTOR3(  1.f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f);
	V3[22] = VERTEX(D3DXVECTOR3( 0.5f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu,  tv);
	V3[23] = VERTEX(D3DXVECTOR3( 0.5f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu, 0.f);
	V3[24] = VERTEX(D3DXVECTOR3( 0.5f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f,  tv);
	V3[25] = VERTEX(D3DXVECTOR3( 0.5f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f);
	V3[26] = VERTEX(D3DXVECTOR3(  0.f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu,  tv);
	V3[27] = VERTEX(D3DXVECTOR3(  0.f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu, 0.f);
	V3[28] = VERTEX(D3DXVECTOR3(  0.f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f,  tv);
	V3[29] = VERTEX(D3DXVECTOR3(  0.f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f);
	V3[30] = VERTEX(D3DXVECTOR3(-0.5f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu,  tv);
	V3[31] = VERTEX(D3DXVECTOR3(-0.5f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu, 0.f);
	V3[32] = VERTEX(D3DXVECTOR3(-0.5f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f,  tv);
	V3[33] = VERTEX(D3DXVECTOR3(-0.5f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f);
	V3[34] = VERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu,  tv);
	V3[35] = VERTEX(D3DXVECTOR3( -1.f,  0.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),  tu, 0.f);

	hr = m_pSrcDevice->CreateVertexBuffer( 36*GetVertexSize(FVF_VERTEX), 0, FVF_VERTEX, POOL_MANAGED, &m_pVertexBuffer);
    if( FAILED(hr) )
	{
		WriteToLog("Failed to create m_pVertexBuffer hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	
	VOID* pV3 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		hr = m_pVertexBuffer->Lock( 0, sizeof(V3), (BYTE**)&pV3, randFlag == 0 ? 0 : LOCK_READONLY );
		if (FAILED(hr))
		{
			for (WORD i = 0; i < index; i++)
				m_pVertexBuffer->Unlock();
			WriteToLog("Failed to lock m_pVertexBuffer on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}

	ASSERT(pV3 != NULL);
	memcpy( pV3, V3, sizeof(V3) );

	for (index = 0; index < randCount; index++)
	{
		hr = m_pVertexBuffer->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_pVertexBuffer on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if (m_dwVersion >= 0x0800)
	if( !PrioritiesVB( m_pVertexBuffer, &priority) || (dwTemp = m_pVertexBuffer->GetPriority()) != priority )
    {
        WriteToLog("GetPriority() on m_pVertexBuffer returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	if ( m_dwVersion < 0x0700 ) //dx6 doesn't support cube maps, volumes and index buffers -> end settings
		return true;

	VOLVERTEX cubeV[24];
	cubeV[0]  = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3( 0.99f,  0.5f, -0.5f));
	cubeV[1]  = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3( 0.99f,  0.5f, -0.5f));
	cubeV[2]  = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3( 0.99f,  0.5f, -0.5f));
	cubeV[3]  = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3( 0.99f,  0.5f, -0.5f));
	cubeV[4]  = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(-0.99f,  0.5f,  0.5f));
	cubeV[5]  = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(-0.99f,  0.5f,  0.5f));
	cubeV[6]  = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(-0.99f,  0.5f,  0.5f));
	cubeV[7]  = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(-0.99f,  0.5f,  0.5f));
	cubeV[8]  = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f, 0.99f, -0.5f));
	cubeV[9]  = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f, 0.99f, -0.5f));
	cubeV[10] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f, 0.99f, -0.5f));
	cubeV[11] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f, 0.99f, -0.5f));
	cubeV[12] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,-0.99f,  0.5f));
	cubeV[13] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,-0.99f,  0.5f));
	cubeV[14] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,-0.99f,  0.5f));
	cubeV[15] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,-0.99f,  0.5f));
	cubeV[16] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f,  0.99f));
	cubeV[17] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f,  0.99f));
	cubeV[18] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f,  0.99f));
	cubeV[19] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f,  0.99f));
	cubeV[20] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f, -0.99f));
	cubeV[21] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f, -0.99f));
	cubeV[22] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f, -0.99f));
	cubeV[23] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f, -0.99f));

	hr = m_pSrcDevice->CreateVertexBuffer( 24*GetVertexSize(FVF_VOLVERTEX), 0, FVF_VOLVERTEX, POOL_MANAGED, &m_pCubeVertexBuffer);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_pCubeVertexBuffer hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	
	VOID* pV4 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		hr = m_pCubeVertexBuffer->Lock( 0, sizeof(cubeV), (BYTE**)&pV4, randFlag == 0 ? 0 : LOCK_READONLY );
		if (FAILED(hr))
		{
			for (WORD i = 0; i < index; i++)
				m_pCubeVertexBuffer->Unlock();
			WriteToLog("Failed to lock m_pCubeVertexBuffer on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}

	ASSERT(pV4 != NULL);
	memcpy( pV4, cubeV, sizeof(cubeV) );

	for (index = 0; index < randCount; index++)
	{
		hr = m_pCubeVertexBuffer->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_pCubeVertexBuffer on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if (m_dwVersion >= 0x0800)
	if( !PrioritiesVB( m_pCubeVertexBuffer, &priority) || (dwTemp = m_pCubeVertexBuffer->GetPriority()) != priority )
    {
        WriteToLog("GetPriority() on m_pCubeVertexBuffer returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	if ( m_dwVersion < 0x0800 )
		return true;    //dx6 and dx7 don't support volumes and index buffers -> end settings

	VOLVERTEX volV[60];
	tu = min( 1, 0.2f*m_Options.D3DOptions.nBackBufferWidth/256);
	tv = min( 1, 0.2f*m_Options.D3DOptions.nBackBufferHeight/256);
	tu = tv = 0;
	volV[0]  = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.0625f));
	volV[1]  = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.0625f));
	volV[2]  = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.0625f));
	volV[3]  = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.0625f));
	volV[4]  = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.1875f));
	volV[5]  = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.1875f));
	volV[6]  = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.1875f));
	volV[7]  = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.1875f));
	volV[8]  = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.3125f));
	volV[9]  = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.3125f));
	volV[10] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.3125f));
	volV[11] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.3125f));
	volV[12] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.4375f));
	volV[13] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.4375f));
	volV[14] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.4375f)); 
	volV[15] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.4375f));
	volV[16] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.5625f));
	volV[17] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.5625f));
	volV[18] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.5625f));
	volV[19] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.5625f));
	volV[20] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.6875f));
	volV[21] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.6875f));
	volV[22] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.6875f));
	volV[23] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.6875f));
	volV[24] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.8125f));
	volV[25] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.8125f));
	volV[26] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.8125f));
	volV[27] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.8125f));
	volV[28] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.9375f));
	volV[29] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.9375f));
	volV[30] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.9375f)); 
	volV[31] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.9375f));
	volV[32] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.125f));
	volV[33] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.125f));
	volV[34] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.125f));
	volV[35] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.125f));
	volV[36] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.375f));
	volV[37] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.375f));
	volV[38] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.375f));
	volV[39] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.375f));
	volV[40] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.625f));
	volV[41] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.625f));
	volV[42] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.625f));
	volV[43] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.625f));
	volV[44] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.875f));
	volV[45] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.875f));
	volV[46] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.875f)); 
	volV[47] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.875f));
	volV[48] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.25f));
	volV[49] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.25f));
	volV[50] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.25f));
	volV[51] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.25f));
	volV[52] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.75f));
	volV[53] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.75f));
	volV[54] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.75f));
	volV[55] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.75f));
	volV[56] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu, 0.f, 0.5f));
	volV[57] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(  tu,  tv, 0.5f));
	volV[58] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f, 0.f, 0.5f));
	volV[59] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3( 0.f,  tv, 0.5f));

	hr = m_pSrcDevice->CreateVertexBuffer( 60*GetVertexSize(FVF_VOLVERTEX), 0, FVF_VOLVERTEX, POOL_MANAGED, &m_pVolVertexBuffer);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_pVolVertexBuffer hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	
	VOID* pV5 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_pVolVertexBuffer->Lock( 0, sizeof(volV), (BYTE**)&pV5, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_pVolVertexBuffer->Unlock();
			WriteToLog("Failed to lock m_pVolVertexBuffer on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( pV5, volV, sizeof(volV) );
	for (index = 0; index < randCount; index++)
	{
		hr = m_pVolVertexBuffer->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_pVolVertexBuffer on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesVB( m_pVolVertexBuffer, &priority) || (dwTemp = m_pVolVertexBuffer->GetPriority()) != priority )
    {
        WriteToLog("GetPriority() on m_pVolVertexBuffer returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	//m_p16bitIndexBuffer1 and m_p16bitIndexBuffer2: two index buffers for TLVertexBuffer and LVertexBuffer
	WORD g_16bitI1[] = {0,1,2,3,4};
	hr = m_pSrcDevice->CreateIndexBuffer( 5*sizeof(WORD), 0, FMT_INDEX16, POOL_MANAGED, &m_p16bitIndexBuffer1);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p16bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p16bitI1 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p16bitIndexBuffer1->Lock( 0, sizeof(g_16bitI1), (BYTE**)&p16bitI1, randFlag == 0 ? 0 : LOCK_READONLY );            
		 if (FAILED(hr))
		 {
			 for (WORD i = 0; i < index; i++)
				m_p16bitIndexBuffer1->Unlock();
			WriteToLog("Failed to lock m_p16bitIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p16bitI1, g_16bitI1, sizeof(g_16bitI1));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p16bitIndexBuffer1->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p16bitIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p16bitIndexBuffer1, &priority) || (dwTemp = m_p16bitIndexBuffer1->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p16bitIndexBuffer1 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	WORD g_16bitI2[] = {3,4,5,6,7};
	hr = m_pSrcDevice->CreateIndexBuffer( 8*sizeof(WORD), 0, FMT_INDEX16, POOL_MANAGED, &m_p16bitIndexBuffer2);
	if ( FAILED(hr) )
	{
		WriteToLog("Failed to create m_p16bitIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p16bitI2 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p16bitIndexBuffer2->Lock( 0, sizeof(g_16bitI2), (BYTE**)&p16bitI2, randFlag == 0 ? 0 : LOCK_READONLY ); 
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p16bitIndexBuffer2->Unlock();
			WriteToLog("Failed to lock m_p16bitIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p16bitI2, g_16bitI2, sizeof(g_16bitI2));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p16bitIndexBuffer2->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p16bitIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p16bitIndexBuffer2, &priority) || (dwTemp = m_p16bitIndexBuffer2->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p16bitIndexBuffer2 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }
	
	//m_p16bitTexIndexBuffer1 and m_p16bitTexIndexBuffer2: creating 16 bit index buffers used with textures
	WORD g_16bitIndexBuffer1[20];
	for (index = 0; index < 20; index++)
		g_16bitIndexBuffer1[ index ] = index;
	hr = m_pSrcDevice->CreateIndexBuffer( 20*sizeof(WORD), 0, FMT_INDEX16, POOL_MANAGED, &m_p16bitTexIndexBuffer1);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p16bittexIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p16bitIndexBuffer1 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p16bitTexIndexBuffer1->Lock( 0, sizeof(g_16bitIndexBuffer1), (BYTE**)&p16bitIndexBuffer1, randFlag == 0 ? 0 : LOCK_READONLY ); 
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p16bitTexIndexBuffer1->Unlock();
			WriteToLog("Failed to lock m_p16bitTexIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p16bitIndexBuffer1, g_16bitIndexBuffer1, sizeof(g_16bitIndexBuffer1));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p16bitTexIndexBuffer1->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p16bitTexIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p16bitTexIndexBuffer1, &priority) || (dwTemp = m_p16bitTexIndexBuffer1->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p16bitTexIndexBuffer1 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	WORD g_16bitIndexBuffer2[16];
	for ( index = 0; index < 16; index++ )
		g_16bitIndexBuffer2[ index ] = 20 + index;
	hr = m_pSrcDevice->CreateIndexBuffer( 36*sizeof(WORD), 0, FMT_INDEX16, POOL_MANAGED, &m_p16bitTexIndexBuffer2);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p16bitTexIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p16bitIndexBuffer2 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p16bitTexIndexBuffer2->Lock( 0, sizeof(g_16bitIndexBuffer2), (BYTE**)&p16bitIndexBuffer2, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p16bitTexIndexBuffer2->Unlock();
			WriteToLog("Failed to lock m_p16bitTexIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p16bitIndexBuffer2, g_16bitIndexBuffer2, sizeof(g_16bitIndexBuffer2));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p16bitTexIndexBuffer2->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p16bitTexIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p16bitTexIndexBuffer2, &priority) || (dwTemp = m_p16bitTexIndexBuffer2->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p16bitTexIndexBuffer2 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }
	
	//m_p16bitCubeIndexBuffer1 and m_p16bitCubeIndexBuffer2: creating 16 bit index buffers used with cube textures
	WORD g_16bitCubeIndexBuffer1[12];
	for ( index = 0; index < 12; index++ )
		g_16bitCubeIndexBuffer1[ index ] = index;
	hr = m_pSrcDevice->CreateIndexBuffer( 12*sizeof(WORD), 0, FMT_INDEX16, POOL_MANAGED, &m_p16bitCubeIndexBuffer1);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p16bitCubeIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p16bitCubeIndexBuffer1 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p16bitCubeIndexBuffer1->Lock( 0, sizeof(g_16bitCubeIndexBuffer1), (BYTE**)&p16bitCubeIndexBuffer1, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p16bitCubeIndexBuffer1->Unlock();
			WriteToLog("Failed to lock m_p16bitCubeIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p16bitCubeIndexBuffer1, g_16bitCubeIndexBuffer1, sizeof(g_16bitCubeIndexBuffer1));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p16bitCubeIndexBuffer1->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p16bitCubeIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p16bitCubeIndexBuffer1, &priority) || (dwTemp = m_p16bitCubeIndexBuffer1->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p16bitCubeIndexBuffer1 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	WORD g_16bitCubeIndexBuffer2[12];
	for ( index = 0; index < 12; index++ )
		g_16bitCubeIndexBuffer2[ index ] = 12 + index;
	hr = m_pSrcDevice->CreateIndexBuffer( 24*sizeof(WORD), 0, FMT_INDEX16, POOL_MANAGED, &m_p16bitCubeIndexBuffer2);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p16bitCubeIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p16bitCubeIndexBuffer2 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p16bitCubeIndexBuffer2->Lock( 0, sizeof(g_16bitCubeIndexBuffer2), (BYTE**)&p16bitCubeIndexBuffer2, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p16bitCubeIndexBuffer2->Unlock();
			WriteToLog("Failed to lock m_p16bitCubeIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p16bitCubeIndexBuffer2, g_16bitCubeIndexBuffer2, sizeof(g_16bitCubeIndexBuffer2));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p16bitCubeIndexBuffer2->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p16bitCubeIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p16bitCubeIndexBuffer2, &priority) || (dwTemp = m_p16bitCubeIndexBuffer2->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p16bitCubeIndexBuffer2 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	//m_p16bitVolIndexBuffer1 and m_p16bitVol: creating 16 bit index buffers used with volume textures
	WORD g_16bitVolIndexBuffer1[32];
	for ( index = 0; index < 32; index++ )
		g_16bitVolIndexBuffer1[ index ] = index;
	hr = m_pSrcDevice->CreateIndexBuffer( 32*sizeof(WORD), 0, FMT_INDEX16, POOL_MANAGED, &m_p16bitVolIndexBuffer1);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p16bitVolIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p16bitVolIndexBuffer1 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p16bitVolIndexBuffer1->Lock( 0, sizeof(g_16bitVolIndexBuffer1), (BYTE**)&p16bitVolIndexBuffer1, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p16bitVolIndexBuffer1->Unlock();
			WriteToLog("Failed to lock m_p16bitVolIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p16bitVolIndexBuffer1, g_16bitVolIndexBuffer1, sizeof(g_16bitVolIndexBuffer1));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p16bitVolIndexBuffer1->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p16bitVolIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p16bitVolIndexBuffer1, &priority) || (dwTemp = m_p16bitVolIndexBuffer1->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p16bitVolIndexBuffer1 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	WORD g_16bitVolIndexBuffer2[28];
	for ( index = 0; index < 28; index++ )
		g_16bitVolIndexBuffer2[ index ] = 32 + index;
	hr = m_pSrcDevice->CreateIndexBuffer( 60*sizeof(WORD), 0, FMT_INDEX16, POOL_MANAGED, &m_p16bitVolIndexBuffer2);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p16bitVolIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p16bitVolIndexBuffer2 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p16bitVolIndexBuffer2->Lock( 0, sizeof(g_16bitVolIndexBuffer2), (BYTE**)&p16bitVolIndexBuffer2, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p16bitVolIndexBuffer2->Unlock();
			WriteToLog("Failed to lock m_p16bitVolIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p16bitVolIndexBuffer2, g_16bitVolIndexBuffer2, sizeof(g_16bitVolIndexBuffer2));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p16bitVolIndexBuffer2->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p16bitVolIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p16bitVolIndexBuffer2, &priority) || (dwTemp = m_p16bitVolIndexBuffer2->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p16bitVolIndexBuffer2 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	if ( !m_Use32BitIB )
		return true;
	
	//m_p32bitIndexBuffer1 and m_p32bitIndexBuffer2: two index buffers for TLVertexBuffer and LVertexBuffer
	DWORD g_32bitI1[] = {0,1,2,3,4};
	hr = m_pSrcDevice->CreateIndexBuffer( 5*sizeof(DWORD), 0, FMT_INDEX32, POOL_MANAGED, &m_p32bitIndexBuffer1);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p32bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p32bitI1 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p32bitIndexBuffer1->Lock( 0, sizeof(g_32bitI1), (BYTE**)&p32bitI1, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			 for (WORD i = 0; i < index; i++)
				m_p32bitIndexBuffer1->Unlock();
			WriteToLog("Failed to lock m_p32bitIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p32bitI1, g_32bitI1, sizeof(g_32bitI1));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p32bitIndexBuffer1->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p32bitIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p32bitIndexBuffer1, &priority) || (dwTemp = m_p32bitIndexBuffer1->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p32bitIndexBuffer1 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	DWORD g_32bitI2[] = {3,4,5,6,7};
	hr = m_pSrcDevice->CreateIndexBuffer( 8*sizeof(DWORD), 0, FMT_INDEX32, POOL_MANAGED, &m_p32bitIndexBuffer2);
	if ( FAILED(hr) )
	{
		WriteToLog("Failed to create m_p32bitIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p32bitI2 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p32bitIndexBuffer2->Lock( 0, sizeof(g_32bitI2), (BYTE**)&p32bitI2, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p32bitIndexBuffer2->Unlock();
			WriteToLog("Failed to lock m_p32bitIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p32bitI2, g_32bitI2, sizeof(g_32bitI2));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p32bitIndexBuffer2->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p32bitIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p32bitIndexBuffer2, &priority) || (dwTemp = m_p32bitIndexBuffer2->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p32bitIndexBuffer2 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }
	
	//m_p32bitIndexBuffer1 and m_p32bitIndexBuffer2: creating 32 bit index buffers used with textures
	DWORD g_32bitIndexBuffer1[20];
	for (dindex = 0; dindex < 20; dindex++)
		g_32bitIndexBuffer1[ dindex ] = dindex;
	hr = m_pSrcDevice->CreateIndexBuffer( 20*sizeof(DWORD), 0, FMT_INDEX32, POOL_MANAGED, &m_p32bitTexIndexBuffer1);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p32bitTexIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p32bitIndexBuffer1 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p32bitTexIndexBuffer1->Lock( 0, sizeof(g_32bitIndexBuffer1), (BYTE**)&p32bitIndexBuffer1, randFlag == 0 ? 0 : LOCK_READONLY ); 
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p32bitTexIndexBuffer1->Unlock();
			WriteToLog("Failed to lock m_p32bitTexIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p32bitIndexBuffer1, g_32bitIndexBuffer1, sizeof(g_32bitIndexBuffer1));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p32bitTexIndexBuffer1->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p32bitTexIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p32bitTexIndexBuffer1, &priority) || (dwTemp = m_p32bitTexIndexBuffer1->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p32bitTexIndexBuffer1 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	DWORD g_32bitIndexBuffer2[16];
	for ( dindex = 0; dindex < 16; dindex++ )
		g_32bitIndexBuffer2[ dindex ] = (DWORD)20 + dindex;
	hr = m_pSrcDevice->CreateIndexBuffer( 36*sizeof(DWORD), 0, FMT_INDEX32, POOL_MANAGED, &m_p32bitTexIndexBuffer2);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p32bitTexIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p32bitIndexBuffer2 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p32bitTexIndexBuffer2->Lock( 0, sizeof(g_32bitIndexBuffer2), (BYTE**)&p32bitIndexBuffer2, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p32bitTexIndexBuffer2->Unlock();
			WriteToLog("Failed to lock m_p32bitTexIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p32bitIndexBuffer2, g_32bitIndexBuffer2, sizeof(g_32bitIndexBuffer2));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p32bitTexIndexBuffer2->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p32bitTexIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p32bitTexIndexBuffer2, &priority) || (dwTemp = m_p32bitTexIndexBuffer2->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p32bitTexIndexBuffer2 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }
	
	//m_p32bitCubeIndexBuffer1 and m_p32bitCubeIndexBuffer2: creating 32 bit index buffers used with cube textures
	DWORD g_32bitCubeIndexBuffer1[12];
	for ( dindex = 0; dindex < 12; dindex++ )
		g_32bitCubeIndexBuffer1[ dindex ] = dindex;
	hr = m_pSrcDevice->CreateIndexBuffer( 12*sizeof(DWORD), 0, FMT_INDEX32, POOL_MANAGED, &m_p32bitCubeIndexBuffer1);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p32bitCubeIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p32bitCubeIndexBuffer1 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p32bitCubeIndexBuffer1->Lock( 0, sizeof(g_32bitCubeIndexBuffer1), (BYTE**)&p32bitCubeIndexBuffer1, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p32bitCubeIndexBuffer1->Unlock();
			WriteToLog("Failed to lock m_p32bitCubeIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p32bitCubeIndexBuffer1, g_32bitCubeIndexBuffer1, sizeof(g_32bitCubeIndexBuffer1));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p32bitCubeIndexBuffer1->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p32bitCubeIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p32bitCubeIndexBuffer1, &priority) || (dwTemp = m_p32bitCubeIndexBuffer1->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p32bitCubeIndexBuffer1 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	DWORD g_32bitCubeIndexBuffer2[12];
	for ( dindex = 0; dindex < 12; dindex++ )
		g_32bitCubeIndexBuffer2[ dindex ] = (DWORD)12 + dindex;
	hr = m_pSrcDevice->CreateIndexBuffer( 24*sizeof(DWORD), 0, FMT_INDEX32, POOL_MANAGED, &m_p32bitCubeIndexBuffer2);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p32bitCubeIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p32bitCubeIndexBuffer2 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p32bitCubeIndexBuffer2->Lock( 0, sizeof(g_32bitCubeIndexBuffer2), (BYTE**)&p32bitCubeIndexBuffer2, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p32bitCubeIndexBuffer2->Unlock();
			WriteToLog("Failed to lock m_p32bitCubeIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p32bitCubeIndexBuffer2, g_32bitCubeIndexBuffer2, sizeof(g_32bitCubeIndexBuffer2));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p32bitCubeIndexBuffer2->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p32bitCubeIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p32bitCubeIndexBuffer2, &priority) || (dwTemp = m_p32bitCubeIndexBuffer2->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p32bitCubeIndexBuffer2 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	//m_p32bitVolIndexBuffer1 and m_p32bitVol: creating 32 bit index buffers used with volume textures
	DWORD g_32bitVolIndexBuffer1[32];
	for ( dindex = 0; dindex < 32; dindex++ )
		g_32bitVolIndexBuffer1[ dindex ] = dindex;
	hr = m_pSrcDevice->CreateIndexBuffer( 32*sizeof(DWORD), 0, FMT_INDEX32, POOL_MANAGED, &m_p32bitVolIndexBuffer1);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p32bitVolIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p32bitVolIndexBuffer1 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p32bitVolIndexBuffer1->Lock( 0, sizeof(g_32bitVolIndexBuffer1), (BYTE**)&p32bitVolIndexBuffer1, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p32bitVolIndexBuffer1->Unlock();
			WriteToLog("Failed to lock m_p32bitVolIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p32bitVolIndexBuffer1, g_32bitVolIndexBuffer1, sizeof(g_32bitVolIndexBuffer1));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p32bitVolIndexBuffer1->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p32bitVolIndexBuffer1 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p32bitVolIndexBuffer1, &priority) || (dwTemp = m_p32bitVolIndexBuffer1->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p32bitVolIndexBuffer1 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	DWORD g_32bitVolIndexBuffer2[28];
	for ( dindex = 0; dindex < 28; dindex++ )
		g_32bitVolIndexBuffer2[ dindex ] = (DWORD)32 + dindex;
	hr = m_pSrcDevice->CreateIndexBuffer( 60*sizeof(DWORD), 0, FMT_INDEX32, POOL_MANAGED, &m_p32bitVolIndexBuffer2);
	if ( FAILED(hr) )
    {
		WriteToLog("Failed to create m_p32bitVolIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	VOID* p32bitVolIndexBuffer2 = NULL;
	randCount = rand()%1000+1;
	randFlag = rand()%2;
	for (index = 0; index < randCount; index++)
	{
		 hr = m_p32bitVolIndexBuffer2->Lock( 0, sizeof(g_32bitVolIndexBuffer2), (BYTE**)&p32bitVolIndexBuffer2, randFlag == 0 ? 0 : LOCK_READONLY );
		 if (FAILED(hr))
		 {
			for (WORD i = 0; i < index; i++)
				m_p32bitVolIndexBuffer2->Unlock();
			WriteToLog("Failed to lock m_p32bitVolIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		 }
	}
	memcpy( p32bitVolIndexBuffer2, g_32bitVolIndexBuffer2, sizeof(g_32bitVolIndexBuffer2));
	for (index = 0; index < randCount; index++)
	{
		hr = m_p32bitVolIndexBuffer2->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("Failed to unlock m_p32bitVolIndexBuffer2 on index %d hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			Fail();
			return false;
		}
	}
	if( !PrioritiesIB( m_p32bitVolIndexBuffer2, &priority) || (dwTemp = m_p32bitVolIndexBuffer2->GetPriority()) != priority )
    {
        WriteToLog("GetPriority on m_p32bitVolIndexBuffer2 returned %d, expected %d.\n", dwTemp, priority);
        Fail();
    }

	return true;
}

bool CBasicTest::GenerateTexture(DWORD texIndex)
{
	D3DCOLOR  Stripe[1];
	CImage  **ppImages = NULL;
	HRESULT   hr       = S_OK;
	bool      result   = true;
	UINT      uWidth   = 128;
	UINT      uHeight  = 128;
	UINT      uLevels;
	DWORD     color_offset = 150;
	DWORD     color_index;
		
	//determine texture format and number of levels

	if (texIndex < m_uCommonTextureFormats || !m_bWrapAndMipTex[(texIndex = texIndex%m_uCommonTextureFormats)])
		uLevels = 1; //single textures
	else 
		uLevels = 0; //mipmaped textures
	
	color_index = (texIndex>>2)*30;
	
	DWORD pool = POOL_MANAGED;
	if (KeySet("default_pool"))
		pool = POOL_DEFAULT;

	hr = m_pSrcDevice->CreateTexture(uWidth, uHeight, uLevels, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], pool, &m_pTexture);
	if (FAILED(hr))
	{
		WriteToLog("GenerateTexture: CreateTexture() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}

	if(m_pTexture->IsPalettized())
		m_pTexture->SetPalette(NULL);

	uLevels = m_pTexture->GetLevelCount();

	ppImages = new CImage*[uLevels];

	if ( NULL == ppImages )
	{
		WriteToLog("GenerateTexture: out of memory allocating ppImages.\n");
		result = false;
		goto Exit;
	}

	ZeroMemory(ppImages, sizeof(CImage *) * uLevels);

	for(DWORD LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
	{
		ppImages[LevelIndex] = new CImage;

		if ( NULL == ppImages[LevelIndex] )
		{
			WriteToLog("GenerateTexture: out of memory allocating ppImages[%d].\n", LevelIndex);
			result =  false;
			goto Exit;
		}

		WORD color = (WORD)(color_index+LevelIndex*15);
		color = (WORD)(color % (256-color_offset) + color_offset);

		if ((m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
			 m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_R16F || 
			 m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_R32F ||
			 m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_A8    )
				Stripe[0] = RGBA_MAKE(color, color, color, color);
		else
		{
			WORD   red = (texIndex+LevelIndex)%4 == 0 || (texIndex+LevelIndex)%4 ==3 ? color : 0;
			WORD green = (texIndex+LevelIndex)%4 == 1 || (texIndex+LevelIndex)%4 ==3 ? color : 0;
			WORD  blue = (texIndex+LevelIndex)%4 == 2 || (texIndex+LevelIndex)%4 ==3 ? color : 0;
			Stripe[0] = RGBA_MAKE(red, green, blue, 0xff);
		}
		
		if (!ppImages[LevelIndex]->LoadStripes( uWidth, uHeight, 1, Stripe, false, false ))
		{
			WriteToLog("GenerateTexture: out of memory on LoadStripes() on level %d.\n", LevelIndex);
			result =  false;
			goto Exit;
		}

		uWidth  = max( 1, uWidth >>1 );
		uHeight = max( 1, uHeight>>1 );
	}
		
	hr = m_pTexture->LoadImage( 0, ppImages, TEXLOAD_DEFAULT);
	if (FAILED(hr))
	{
		WriteToLog("GenerateTexture: LoadImage() failed with hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		result =  false;
		goto Exit;
	}	

	DWORD priority, dwTemp;
	if( m_dwVersion >= 0x0700 && (!PrioritiesT( m_pTexture, &priority) || (dwTemp = m_pTexture->GetPriority()) != priority) )
    {
        WriteToLog("GetPriority() on texture returned %d, expected %d.\n", dwTemp, priority);
		Fail();
    }

Exit:
	if (!result)
	{
		Fail();
		RELEASE(m_pTexture);
	}

	if (NULL != ppImages)
	for(UINT i = 0; i < uLevels; i++)
		SAFEDELETE(ppImages[i]);
	SAFEDELETEA(ppImages);

	return result;
}

bool CBasicTest::GenerateCubeTexture(DWORD texIndex)
{
	D3DCOLOR  Stripes[6];
	CImage ***pppImages = NULL;
	HRESULT   hr        = S_OK;
	bool      result    = true;
	UINT      uWidth    = 128;
	UINT      uLevels;
	

	//determine texture format and number of levels
	if (texIndex < m_uCommonCubeFormats || !m_bWrapAndMipCube[(texIndex = texIndex%m_uCommonCubeFormats)])
		uLevels = 1; //single textures
	else 
		uLevels = 0; //mipmaped textures
	
	DWORD pool = POOL_MANAGED;
	if (KeySet("default_pool"))
		pool = POOL_DEFAULT;

	hr = m_pSrcDevice->CreateCubeTexture( uWidth, uLevels, 0, m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat], pool, &m_pCubeTexture );
	if ( m_dwVersion < 0x0800 && E_NOTIMPL == hr )
	{ 
		WriteToLog("GenerateCubeTexture: cube maps of this format are not implemented on this driver.\n");
		SkipTestRange(m_uTestNumber, m_uTestNumber);
		return true;
	}
	else if (FAILED(hr))
	{
		WriteToLog("GenerateCubeTexture: CreateCubeTexture() failed with hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}

	if(m_pCubeTexture->IsPalettized())
		m_pCubeTexture->SetPalette(NULL);

	uLevels = m_pCubeTexture->GetLevelCount();

	pppImages = new CImage**[uLevels];

	if ( NULL == pppImages )
	{
		WriteToLog("GenerateCubeTexture: out of memory allocating pppImage.\n");
		result = false;
		goto Exit;
	}

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

	for(DWORD LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
	{
		pppImages[LevelIndex] = new CImage*[6];

		if ( NULL == pppImages[LevelIndex] )
		{
			WriteToLog("GenerateCubesTexture: out of memory allocating pppImage[%d].\n", LevelIndex);
			result = false;
			goto Exit;
		}

		ZeroMemory(pppImages[LevelIndex], sizeof(CImage *) * 6);

		if ((m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
			 m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].d3dfFormat == FMT_R16F || 
			 m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].d3dfFormat == FMT_R32F ||
		     m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].d3dfFormat == FMT_A8    )
		{
			WORD color = 255 - (WORD)LevelIndex*15;
			for (int i = 0; i < 6; i++)
			{
				Stripes[i] = RGBA_MAKE(color, color, color, color); 
				color -= 15;
			}
		}
		else
		{
			Stripes[0] = RGBA_MAKE(255 - LevelIndex*15, 0, 0, 255);
			Stripes[1] = RGBA_MAKE(0, 255 - LevelIndex*15, 0, 255);
			Stripes[2] = RGBA_MAKE(0, 0, 255 - LevelIndex*15, 255);
			Stripes[3] = RGBA_MAKE(0, 255 - LevelIndex*15, 255 - LevelIndex*15, 255);
			Stripes[4] = RGBA_MAKE(255 - LevelIndex*15, 0, 255 - LevelIndex*15, 255);
			Stripes[5] = RGBA_MAKE(255 - LevelIndex*15, 255 - LevelIndex*15, 0, 255);
		}
		
		for ( int FaceIndex = 0; FaceIndex < 6; FaceIndex++)
		{
			pppImages[LevelIndex][FaceIndex] = new CImage();

			if ( NULL == pppImages[LevelIndex][FaceIndex] )
			{
				WriteToLog("GenerateCubesTexture: out of memory allocating pppImage[%d][%d].\n", LevelIndex, FaceIndex);
				result = false;
				goto Exit;
			}

			if (!pppImages[LevelIndex][FaceIndex]->LoadStripes( uWidth, uWidth, 1, Stripes+(texIndex+FaceIndex)%6, false, false ))
			{
				WriteToLog("GenerateCubeTexture: out of memory on LoadStripes() on face %d for level %d hr=%s(%x).\n", FaceIndex, LevelIndex, m_pD3D->HResultToString(hr), hr);
				result = false;
				goto Exit;
			}
		}
		uWidth  >>= 1;
	}

	hr = m_pCubeTexture->LoadImage(pppImages, TEXLOAD_DEFAULT );
	if (FAILED(hr))
	{
		WriteToLog("GenerateCubeTexture: LoadImage() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		result = false;
		goto Exit;
	}

	DWORD priority, dwTemp;
	if( !PrioritiesCT( m_pCubeTexture, &priority) || (dwTemp = m_pCubeTexture->GetPriority()) != priority )
    {
        WriteToLog("GetPriority() on cube texture returned %d, expected %d.\n", dwTemp, priority);
		Fail();
    }
	
Exit:
	if (!result)
	{
		Fail();
		RELEASE(m_pCubeTexture);
	}

	if (NULL != pppImages)
	for (UINT k = 0; k < uLevels; k++)
	{
		if (NULL != pppImages[k])
		for (UINT j = 0; j < 6; j++)
			SAFEDELETE(pppImages[k][j]);
		SAFEDELETEA(pppImages[k]);
	}
	SAFEDELETEA(pppImages);

	return result;
}

bool CBasicTest::GenerateVolumeTexture(DWORD texIndex)
{
	D3DCOLOR  Stripe[1];
	CImage ***pppImages = NULL;
	HRESULT   hr        = S_OK;
	bool      result    = true;
	UINT      uLevels;
	UINT      uWidth = 128;
	UINT      depth, uDepth = depth = 8;
	m_TotalNumberOfSlices = 0;

	//determine texture format and number of levels
	if (texIndex < m_uCommonVolumeFormats || !m_bWrapAndMipVol[(texIndex = texIndex%m_uCommonVolumeFormats)])
		uLevels = 1; //single textures
	else
		uLevels = 0; //mipmaped textures

	FORMAT Format = m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat];
	//bug 71530 is not fixed in dx9
	//we don't run the test with driver resource management any more since nobody supports it
	if (/*m_dwVersion < 0x0900 &&*/ /*KeySet(_T("D3DManage"))*/ uLevels != 1  && 
		(Format.ddpfPixelFormat.dwFourCC == FMT_G8R8_G8B8 || Format.ddpfPixelFormat.dwFourCC == FMT_R8G8_B8G8))
	{
		UINT tempLevels = 0;
		UINT tempWidth = uWidth;
		while (tempWidth > 1)
		{
			tempLevels++;
			tempWidth = tempWidth >> 1;
		}
		uLevels = tempLevels;
	}
	//bug 52173 got fixed only for dx9
	if (m_dwVersion < 0x0900 && uLevels != 1 && (Format.ddpfPixelFormat.dwFourCC == FMT_UYVY || Format.ddpfPixelFormat.dwFourCC == FMT_YUY2))
	{
		UINT tempLevels = 0;
		UINT tempWidth = uWidth;
		while (tempWidth > 1)
		{
			tempLevels++;
			tempWidth = tempWidth >> 1;
		}
		uLevels = tempLevels;
	}
	
	DWORD pool = POOL_MANAGED;
	if (KeySet("default_pool"))
		pool = POOL_DEFAULT;

	hr = m_pSrcDevice->CreateVolumeTexture( uWidth, uWidth, uDepth, uLevels, 0, m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat], pool, &m_pVolTexture );
	if (FAILED(hr))
	{
		WriteToLog("GenerateVolumeTexture: CreateVolumeTexture() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}

	if(m_pVolTexture->IsPalettized())
		m_pVolTexture->SetPalette(NULL);

	uLevels = m_pVolTexture->GetLevelCount();
	
	pppImages = new CImage**[uLevels];

	if (NULL == pppImages)
	{
		WriteToLog("GenerateVolTexture: Out of memory allocating pppImage.\n");
		result = false;
		goto Exit;
	}

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

	for (DWORD LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
	{
		DWORD temp = (texIndex + LevelIndex)%8;
		m_TotalNumberOfSlices += depth;

        pppImages[LevelIndex] = new CImage*[depth];

        if (NULL == pppImages[LevelIndex])
		{
			WriteToLog("GenerateVolTexture: Out of memory allocating pppImage[%d].\n", LevelIndex);
			result = false;
			goto Exit;
		}

		ZeroMemory(pppImages[LevelIndex], sizeof(CImage *) * depth);
    
		for (DWORD SliceIndex = 0; SliceIndex < depth; SliceIndex++)
		{
			pppImages[LevelIndex][SliceIndex] = new CImage();

			if (NULL == pppImages[LevelIndex][SliceIndex])
			{
				WriteToLog("GenerateVolTexture: Out of memory allocating pppImage[%d][%d].\n", LevelIndex, SliceIndex);
				result = false;
				goto Exit;
			}

			if ((m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
				 m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].d3dfFormat == FMT_R16F || 
				 m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].d3dfFormat == FMT_R32F ||
				 m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].d3dfFormat == FMT_A8    )
			{
				WORD color = (WORD)(255-15*SliceIndex-temp*15);
				Stripe[0] = RGBA_MAKE(color, color, color, color);
			}
			else
			{
				WORD color = 255-20*(WORD)SliceIndex;
				WORD   red = temp%2 == 0 ? color : 0;
				WORD green = (temp+1)%4 >= 2 ? color : 0;
				WORD  blue = temp/3 != 0 ? color : 0;
				Stripe[0] = RGBA_MAKE(red, green, blue, 255);
			}
			
            if (!pppImages[LevelIndex][SliceIndex]->LoadStripes(uWidth, uWidth, 1, Stripe, false))
			{
				WriteToLog("GenerateVolTexture: out of memory on LoadStripes() on level %d and slice %d.\n", LevelIndex, SliceIndex);
				result = false;
				goto Exit;
			}
		}
		uWidth  = max( 1, uWidth>>1 );
		depth  = max( 1, depth>>1 );
	}

	hr = m_pVolTexture->LoadImage(pppImages, TEXLOAD_DEFAULT);
	
	if (FAILED(hr))
	{
		WriteToLog("GenerateVolTexture: LoadImage() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		result = false;
		goto Exit;
	}

	DWORD priority, dwTemp;
	if( !PrioritiesVT( m_pVolTexture, &priority) || (dwTemp = m_pVolTexture->GetPriority()) != priority )
    {
        WriteToLog("GetPriority() on volume texture returned %d, expected %d.\n", dwTemp, priority);
		Fail();
    }
	

Exit:
	if (!result)
	{
		Fail();
		RELEASE(m_pVolTexture);
	}

	if (NULL != pppImages)
	for (UINT i = 0; i < uLevels; i++)
	{
		if (NULL != pppImages[i])
		for (UINT j = 0; j < uDepth; j++)
			SAFEDELETE(pppImages[i][j]);
		SAFEDELETEA(pppImages[i]);
		uDepth = max( 1, uDepth>>1 );
	}
	SAFEDELETEA(pppImages);

	return result;
}

bool CBasicTest::PrioritiesVB(CVertexBuffer *pVB, DWORD *priority)
{
	DWORD dwOldPriority = 0; //default priority for managed resources is 0
	DWORD dwNewPriority = rand() % MAXPRIORITY;
    DWORD dwTemp;

	if( (dwTemp = pVB->GetPriority()) != dwOldPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwTemp;
        return false;
    }
	if( (dwTemp = pVB->SetPriority( dwNewPriority)) != dwOldPriority )
    {
        WriteToLog("SetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwNewPriority;
        return false;
    }
	if( (dwTemp = pVB->GetPriority()) != dwNewPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwNewPriority);
		*priority = dwTemp;
        return false;
    }
	dwOldPriority = dwNewPriority;
	dwNewPriority = rand() % MAXPRIORITY;
	if( (dwTemp = pVB->SetPriority( dwNewPriority)) != dwOldPriority )
    {
        WriteToLog("SetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwNewPriority;
        return false;
    }
	if( (dwTemp = pVB->GetPriority()) != dwNewPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwNewPriority);
		*priority = dwTemp;
        return false;
    }
	*priority = dwNewPriority;
	return true;
}

bool CBasicTest::PrioritiesIB(CIndexBuffer *pIB, DWORD *priority)
{
	DWORD dwOldPriority = 0; //default priority for managed resources is 0
	DWORD dwNewPriority = rand() % MAXPRIORITY;
    DWORD dwTemp;

	if( (dwTemp = pIB->GetPriority()) != dwOldPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwTemp;
        return false;
    }
	if( (dwTemp = pIB->SetPriority( dwNewPriority)) != dwOldPriority )
    {
        WriteToLog("SetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwNewPriority;
        return false;
    }
	if( (dwTemp = pIB->GetPriority()) != dwNewPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwNewPriority);
		*priority = dwTemp;
        return false;
    }
	dwOldPriority = dwNewPriority;
	dwNewPriority = rand() % MAXPRIORITY;
	if( (dwTemp = pIB->SetPriority( dwNewPriority)) != dwOldPriority )
    {
        WriteToLog("SetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwNewPriority;
        return false;
    }
	if( (dwTemp = pIB->GetPriority()) != dwNewPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwNewPriority);
		*priority = dwTemp;
        return false;
    }
	*priority = dwNewPriority;
	return true;
}

bool CBasicTest::PrioritiesT(CTexture *pTexture, DWORD *priority)
{
	DWORD dwOldPriority = 0; //default priority for managed resources is 0
	DWORD dwNewPriority = rand() % MAXPRIORITY;
    DWORD dwTemp;

	if( (dwTemp = pTexture->GetPriority()) != dwOldPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwTemp;
        return false;
    }
	if( (dwTemp = pTexture->SetPriority( dwNewPriority)) != dwOldPriority )
    {
        WriteToLog("SetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwNewPriority;
        return false;
    }
	if( (dwTemp = pTexture->GetPriority()) != dwNewPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwNewPriority);
		*priority = dwTemp;
        return false;
    }
	dwOldPriority = dwNewPriority;
	dwNewPriority = rand() % MAXPRIORITY;
	if( (dwTemp = pTexture->SetPriority( dwNewPriority)) != dwOldPriority )
    {
        WriteToLog("SetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwNewPriority;
        return false;
    }
	if( (dwTemp = pTexture->GetPriority()) != dwNewPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwNewPriority);
		*priority = dwTemp;
        return false;
    }
	*priority = dwNewPriority;
	return true;
}

bool CBasicTest::PrioritiesCT(CCubeTexture *pCubeTexture, DWORD *priority)
{
	DWORD dwOldPriority = 0; //default priority for managed resources is 0
	DWORD dwNewPriority = rand() % MAXPRIORITY;
    DWORD dwTemp;

	if( (dwTemp = pCubeTexture->GetPriority()) != dwOldPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwTemp;
        return false;
    }
	if( (dwTemp = pCubeTexture->SetPriority( dwNewPriority)) != dwOldPriority )
    {
        WriteToLog("SetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwNewPriority;
        return false;
    }
	if( (dwTemp = pCubeTexture->GetPriority()) != dwNewPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwNewPriority);
		*priority = dwTemp;
        return false;
    }
	dwOldPriority = dwNewPriority;
	dwNewPriority = rand() % MAXPRIORITY;
	if( (dwTemp = pCubeTexture->SetPriority( dwNewPriority)) != dwOldPriority )
    {
        WriteToLog("SetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwNewPriority;
        return false;
    }
	if( (dwTemp = pCubeTexture->GetPriority()) != dwNewPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwNewPriority);
		*priority = dwTemp;
        return false;
    }
	*priority = dwNewPriority;
	return true;
}

bool CBasicTest::PrioritiesVT(CVolTexture *pVolTexture, DWORD *priority)
{
	DWORD dwOldPriority = 0; //default priority for managed resources is 0
	DWORD dwNewPriority = rand() % MAXPRIORITY;
    DWORD dwTemp;

	if( (dwTemp = pVolTexture->GetPriority()) != dwOldPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwTemp;
        return false;
    }
	if( (dwTemp = pVolTexture->SetPriority( dwNewPriority)) != dwOldPriority )
    {
        WriteToLog("SetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwNewPriority;
        return false;
    }
	if( (dwTemp = pVolTexture->GetPriority()) != dwNewPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwNewPriority);
		*priority = dwTemp;
        return false;
    }
	dwOldPriority = dwNewPriority;
	dwNewPriority = rand() % MAXPRIORITY;
	if( (dwTemp = pVolTexture->SetPriority( dwNewPriority)) != dwOldPriority )
    {
        WriteToLog("SetPriority() returned %d, expected %d.\n", dwTemp, dwOldPriority);
		*priority = dwNewPriority;
        return false;
    }
	if( (dwTemp = pVolTexture->GetPriority()) != dwNewPriority )
    {
        WriteToLog("GetPriority() returned %d, expected %d.\n", dwTemp, dwNewPriority);
		*priority = dwTemp;
        return false;
    }
	*priority = dwNewPriority;
	return true;
}


