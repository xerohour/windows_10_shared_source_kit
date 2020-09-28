#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "ResMan.h"

//-----------------------------------------------------------------------------

int ListTest::AddIndexBufferToList(UINT type)
{
	CIndexBuffer  *pIB       = NULL;
	CVertexBuffer *pVB       = NULL;
	RESINFO       *pResInfo  = NULL;
	PWORD          p16bit    = NULL;
	PDWORD         p32bit    = NULL;
	VERTEX        *pVertices = NULL;
	HRESULT        hr        = S_OK;
	DWORD          numInd    = 0;
	int            randCount, randFlag;
	int			   result = RUN;
	
	m_TotalNumberOfSlices = 0;
	if (m_dwVersion >= 0x0800)
		m_CurrentOccupiedMemory = 0;

	pResInfo = new RESINFO;
	
	//read values from input file
	
	if (m_input)
	{
		DWORD temp;

		if ( fscanf( m_input, "%d", &temp) != 1 || temp == 0)
		{
			WriteToLog("%d: AddIndexBufferToList - error reading from file.\n", m_uTestNumber);
			hr = E_FAIL;
			goto Exit;
		}

		numInd = (DWORD)temp;
	}
	else
	{
		// randomly choose the number of indices for index buffer

		numInd = (DWORD)rand()%100000+3;
		numInd += numInd%2;
		numInd = min( numInd, m_pSrcDevice->GetCaps()->dwMaxVertexIndex);
	}

	//create 16 bit index buffer
	
	if (IS_16INDEXBUFFER == type)
	{
		if ( m_output  )
			fprintf(m_output, "%d\n", numInd);

		p16bit = new WORD[numInd];

		if (NULL == p16bit)
		{
			WriteToLog("%d: AddIndexBufferToList - out of memory on allocating 16 bit index array.\n", m_uTestNumber);
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		for (WORD i = 0; i < numInd; i++)
			p16bit[i] = i;

		hr = m_pSrcDevice->CreateIndexBuffer( numInd*sizeof(WORD), 0, FMT_INDEX16, POOL_MANAGED, &pIB);
		if (FAILED(hr))
		{
			WriteToLog("%d: AddIndexBufferToList - CreateIndexBuffer() failed with hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		VOID* pI = NULL;
		randCount = rand()%1000+1;
		randFlag = rand()%2;

		UINT index;
		for (index = 0; index < randCount; index++)
		{
			hr = pIB->Lock( 0, numInd*sizeof(WORD), (BYTE**)&pI, randFlag == 0 ? 0 : LOCK_READONLY ); 
			if (FAILED(hr) || NULL == pI)
			{
				for (UINT i = 0; i < index; i++)
					pIB->Unlock();
				
				WriteToLog("%d: AddIndexBufferToList - %dth Lock() failed on 16 bit index buffer with hr=%s(%x)\n", m_uTestNumber, index, m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}
		}

		ASSERT(pI != NULL);
		memcpy( pI, p16bit, numInd*sizeof(WORD));

		for (index = 0; index < randCount; index++)
		{
			hr = pIB->Unlock();

			if (FAILED(hr))
			{
				WriteToLog("%d: AddIndexBufferToList - %dth Unlock() failed on 16 bit index buffer with hr=%s(%x)\n", m_uTestNumber, index, m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}
		}
	}

	//create 16 bit index buffer

	else 
	{
		if ( m_output )
			fprintf(m_output, "%d\n", numInd);

		p32bit = new DWORD[numInd];

		if (NULL == p32bit)
		{
			WriteToLog("%d: AddIndexBufferToList - out of memory on allocating 32 bit index array\n", m_uTestNumber);
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		for (DWORD i = 0; i < numInd; i++)
			p32bit[i] = i;

		hr = m_pSrcDevice->CreateIndexBuffer( numInd*sizeof(DWORD), 0, FMT_INDEX32, POOL_MANAGED, &pIB);
		if (FAILED(hr))
		{
			WriteToLog("%d: AddIndexBufferToList - CreateIndexBuffer() failed on 32 bit index buffer with hr=%s(%x)\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}

		VOID* pI = NULL;
		randCount = rand()%1000+1;
		randFlag = rand()%2;

		UINT index;
		for (index = 0; index < randCount; index++)
		{
			hr = pIB->Lock( 0, numInd*sizeof(DWORD), (BYTE**)&pI, randFlag == 0 ? 0 : LOCK_READONLY ); 
			
			if (FAILED(hr) || NULL == pI)
			{
				for (UINT i = 0; i < index; i++)
					pIB->Unlock();
				
				WriteToLog("%d: AddIndexBufferToList - %dth Lock() failed on 32 bit index buffer with hr=%s(%x).\n", m_uTestNumber, index, m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}
		}

		ASSERT(pI != NULL);
		memcpy( pI, p32bit, numInd*sizeof(DWORD));

		for (index = 0; index < randCount; index++)
		{
			hr = pIB->Unlock();
			
			if (FAILED(hr))
			{
				WriteToLog("%d: AddIndexBufferToList - %dth Unlock() failed on 32 bit index buffer with hr=%s(%x).\n", m_uTestNumber, index, m_pD3D->HResultToString(hr), hr);
				goto Exit;
			}
		}
	}

	if (m_dwVersion >= 0x0800)
		m_CurrentOccupiedMemory = pIB->GetSize();

	// create vertex buffer to use along with index buffer

	randCount       = rand()%1000 + 1;
	randFlag        = rand()%2;
	UINT  uTriNum   = numInd/2 - 1; //shows how many rectangles will be drawn
	UINT  uVertices = 2 * (uTriNum + 1);   
	FLOAT wcell     = 2.f / uTriNum;

	pVertices = new VERTEX[uVertices];
	
	if (NULL == pVertices)
	{
		WriteToLog("%d: AddIndexBufferToList - out of memory on allocating vertex array.\n", m_uTestNumber);
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	pVertices[0] = VERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   0.f,  0.f);
	pVertices[1] = VERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   0.f,  0.f);

	for (UINT i = 1; i <= uTriNum; i++)
	{
		pVertices[i*2  ] = VERTEX(D3DXVECTOR3( -1.f+i*wcell, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   0.f,  0.f);
		pVertices[i*2+1] = VERTEX(D3DXVECTOR3( -1.f+i*wcell,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   0.f,  0.f);
	}

	hr = m_pSrcDevice->CreateVertexBuffer( uVertices*sizeof(VERTEX), 0, FVF_VERTEX, POOL_MANAGED, &pVB );
	if (FAILED(hr))
	{
		WriteToLog("%d: AddIndexBufferToList - CreateVertexBuffer() failed with hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	VOID* pV = NULL;

	UINT index;
	for (index = 0; index < randCount; index++)
	{
		hr = pVB->Lock( 0, uVertices*sizeof(VERTEX), (BYTE**)&pV, randFlag == 0 ? 0 : LOCK_READONLY );
		
		if (FAILED(hr) || NULL == pV)
		{
			for (UINT i = 0; i < index; i++)
				pVB->Unlock();			
			WriteToLog("%d: AddIndexBufferToList - %d Lock() failed on vertex buffer with hr=%s(%x).\n", m_uTestNumber, index, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

	ASSERT(pV != NULL);
	memcpy( pV, pVertices, uVertices*sizeof(VERTEX));

	for (index = 0; index < randCount; index++)
	{
		hr = pVB->Unlock();

		if (FAILED(hr))
		{
			WriteToLog("%d: AddIndexBufferToList - %d Unlock() failed on vertex buffer with hr=%s(%x).\n", m_uTestNumber, index, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

	m_TotalNumberOfSlices = 1;
	m_IBuffersList.insert(m_IBuffersList.end(), pIB);
	m_VBuffersList.insert(m_VBuffersList.end(), pVB);
	
Exit:
	if (FAILED(hr))
	{
		RELEASE(pIB);
		RELEASE(pVB);

		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			if (!m_FixedEnd)
				m_IsOutOfMemory = true;
			type = IS_NULL;
			result = SKIP;
		}
		else
		{
			//Fail();
			result = FAIL;
			m_IBuffersList.insert(m_IBuffersList.end(), (CIndexBuffer *) NULL);
			m_VBuffersList.insert(m_VBuffersList.end(), (CVertexBuffer *) NULL);
		}
	}

	pResInfo->Type = (PRIM_TYPES)type;
	pResInfo->NumSlices = m_TotalNumberOfSlices;
	if (m_TotalNumberOfSlices > 0)
	{
		pResInfo->pCheckColor = new bool[m_TotalNumberOfSlices];
		pResInfo->pColors = new D3DCOLOR[m_TotalNumberOfSlices];
		memset(pResInfo->pCheckColor, true, m_TotalNumberOfSlices);
		ZeroMemory(pResInfo->pColors, sizeof(D3DCOLOR) * m_TotalNumberOfSlices);
	}
	pResInfo->Format = IS_16INDEXBUFFER==type ? FMT_INDEX16 : FMT_INDEX32;
	m_ResInfosList.insert(m_ResInfosList.end(), pResInfo);

	TCHAR       szBuffer[30];
	FmtToString(FORMAT_TEXTURE, &(pResInfo->Format), szBuffer);

	sprintf(msgString, "%s %s IB %d, ", msgString, IS_16INDEXBUFFER==type ? "INDEX16" : "INDEX32", numInd);
	sprintf(m_Message, "%s %s IB %d\n", m_Message, IS_16INDEXBUFFER==type ? "INDEX16" : "INDEX32", numInd);
	
	delete [] p32bit;
	delete [] p16bit;
	delete [] pVertices;
	
	return result;
}

//-----------------------------------------------------------------------------

int ListTest::AddTLVertexBufferToList()
{
	CVertexBuffer *pVB       = NULL;
	D3DTLVERTEX   *pVertices = NULL;
	RESINFO       *pResInfo  = NULL;
	PRIM_TYPES     type      = IS_TLVERTEXBUFFER;
	int            randCount = rand()%1000+1;
	int            randFlag  = rand()%2;
	int            result    = RUN;
	HRESULT        hr        = S_OK;
	UINT           uTriNum;              //shows how many rectangles will be drawn
	UINT           uVertices = 0;

	pResInfo = new RESINFO;
	m_TotalNumberOfSlices = 0;

	if (m_input)
	{
		if ( fscanf( m_input, "%d", &uTriNum) != 1)
		{
			WriteToLog("%d: AddTLVertexBufferToList - error reading from file.\n", m_uTestNumber);			
			hr = E_FAIL;
			goto Exit;
		}
	}
	else
		uTriNum = 1 << rand()%13; 

	uVertices = 2*(uTriNum + 1); 
	
	if ( m_output )
		fprintf(m_output, "%d\n", uVertices);

	DWORD hcell = m_Options.D3DOptions.nBackBufferHeight;
	FLOAT wcell = (FLOAT)m_Options.D3DOptions.nBackBufferWidth / uTriNum;
	
	pVertices = new D3DTLVERTEX[uVertices];
	
	if (NULL == pVertices)
	{
		WriteToLog("AddTLBufferToList - out of memory on allocating TL vertex array.\n");
		result = E_OUTOFMEMORY;
		goto Exit;
	}

	pVertices[0] = D3DTLVERTEX(D3DXVECTOR3( 0.f, (float)hcell, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	pVertices[1] = D3DTLVERTEX(D3DXVECTOR3( 0.f,   0.f, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);

	for (UINT i = 1; i <= uTriNum; i++)
	{
			pVertices[i*2  ] = D3DTLVERTEX(D3DXVECTOR3( wcell*i, (float)hcell, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
			pVertices[i*2+1] = D3DTLVERTEX(D3DXVECTOR3( wcell*i,   0.f, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	}

    hr = m_pSrcDevice->CreateVertexBuffer( uVertices*sizeof(D3DTLVERTEX), 0, D3DFVF_TLVERTEX, POOL_MANAGED, &pVB );
	if (FAILED(hr))
	{
		WriteToLog("AddTLBufferToList - CreateVertexBuffer() failed with hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	VOID* pV = NULL;
	
	UINT index;
	for (index = 0; index < randCount; index++)
	{
		hr = pVB->Lock( 0, uVertices*sizeof(D3DTLVERTEX), (BYTE**)&pV, randFlag == 0 ? 0 : LOCK_READONLY );
		if (FAILED(hr) || NULL == pV)
		{
			for (UINT i = 0; i < index; i++)
				pVB->Unlock();
			
			WriteToLog("%d: AddTLVertexBufferToList - %d Lock() failed on TL Vertex Buffer with hr=%s(%x).\n", m_uTestNumber, index, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

	ASSERT(pV != NULL);
	memcpy( pV, pVertices, uVertices*sizeof(D3DTLVERTEX));

	for (index = 0; index < randCount; index++)
	{
		hr = pVB->Unlock();
		if (FAILED(hr))
		{
			WriteToLog("AddTLVertexBufferToList - %d Lock() failed on TL Vertex Buffer with hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

	m_TotalNumberOfSlices = 1;
	m_VBuffersList.insert(m_VBuffersList.end(), pVB);

	if (m_dwVersion >= 0x0800)
		m_CurrentOccupiedMemory = pVB->GetSize();
	
Exit:
	if (FAILED(hr))
	{
		RELEASE(pVB);

		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			if (!m_FixedEnd)
				m_IsOutOfMemory = true;
			type = IS_NULL;
			result = SKIP;
		}
		else
		{
			//Fail();
			result = FAIL;
			m_VBuffersList.insert(m_VBuffersList.end(), (CVertexBuffer *) NULL);
		}
	}

	pResInfo->Type = type;
	pResInfo->NumSlices = m_TotalNumberOfSlices;
	if (m_TotalNumberOfSlices > 0)
	{
		pResInfo->pCheckColor = new bool[m_TotalNumberOfSlices];
		pResInfo->pColors = new D3DCOLOR[m_TotalNumberOfSlices];
		memset(pResInfo->pCheckColor, true, m_TotalNumberOfSlices);
		ZeroMemory(pResInfo->pColors, sizeof(D3DCOLOR) * m_TotalNumberOfSlices);
	}
	m_ResInfosList.insert(m_ResInfosList.end(), pResInfo);

	sprintf(msgString, "%sTLVB %d, ", msgString, uVertices);
	sprintf(m_Message, "%sTLVB %d\n", m_Message, uVertices);
	
	delete [] pVertices;
	
	return result;
}

//-----------------------------------------------------------------------------

int ListTest::ChangeTLVertexBufferInList(VBUFFERLIST::iterator vb, WORD row, WORD col, FLOAT hcell, FLOAT wcell)
{
	D3DTLVERTEX *pVertices = NULL;
	HRESULT      hr        = S_OK;
	int          result    = RUN;
	UINT         uVertices = 0;
	UINT         uTriNum   = 0;

	int randCount = rand()%1000+1;
	int randFlag  = rand()%2;
	
	if (NULL == *vb)
	{
		hr = E_FAIL;
		WriteToLog("%d: ChangeTLVertexBufferInList - vb is NULL.\n", m_uTestNumber);
		goto Exit;
	}

	uVertices = (*vb)->GetSize() / sizeof(D3DTLVERTEX);
	uTriNum   = uVertices/2 - 1;

	DWORD Height = m_Options.D3DOptions.nBackBufferHeight;
	DWORD Width  = m_Options.D3DOptions.nBackBufferWidth;
	FLOAT x1, x2, y1, y2;
	/*FLOAT x1     = 1 - (col+0) * wcell;  
	FLOAT x2     = 1 - (col+1) * wcell;
	FLOAT y1     = 1 - (row+0) * hcell;
	FLOAT y2     = 1 - (row+1) * hcell;
	FLOAT width;
	
	x1 = (int)( (Width /2) * (1-x1) + 1.5 );
	x2 = (int)( (Width /2) * (1-x2) + 0.5 );
	y1 = (int)( (Height/2) * (1-y1) + 1.5 );
	y2 = (int)( (Height/2) * (1-y2) + 0.5 );*/

	FLOAT ncell = hcell;
	wcell = Width / ncell;
	hcell = Height / ncell;
	x1 = wcell * col;
	x2 = wcell * (col+1);
	y1 = hcell * row;
	y2 = hcell * (row+1);

	//create a vertex array to store new data for tl vertex buffer
	
	pVertices = new D3DTLVERTEX[uVertices];

	if (NULL == pVertices)
	{
		WriteToLog("%d: ChangeTLVertexBufferInList - out of memory on allocating vertex data array.\n", m_uTestNumber);
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	FLOAT width = (FLOAT)(x2-x1)/uTriNum;
	
	pVertices[0] = D3DTLVERTEX(D3DXVECTOR3( x1, y2, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	pVertices[1] = D3DTLVERTEX(D3DXVECTOR3( x1, y1, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	
	for (UINT i = 1; i <= uTriNum; i++)
	{
		pVertices[i*2  ] = D3DTLVERTEX(D3DXVECTOR3( min(x2, x1+width*i), y2, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
		pVertices[i*2+1] = D3DTLVERTEX(D3DXVECTOR3( min(x2, x1+width*i), y1, 0.5f), 1.f, 0x0000ff00, 0, 0.0f, 0.0f);
	}
	
	VOID* pV = NULL;

	UINT index;
	for (index = 0; index < randCount; index++)
	{
		hr = (*vb)->Lock( 0, uVertices*sizeof(D3DTLVERTEX), (BYTE**)&pV, 0);
		
		if (FAILED(hr) || NULL == pV)
		{
			for (UINT i = 0; i < index; i++)
				(*vb)->Unlock();
			
			WriteToLog("%d: ChangeTLVertexBufferInList - %dth Lock() failed on the buffer with hr=%s(%x).\n", m_uTestNumber, index, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}
	
	if (NULL != pV) memcpy( pV, pVertices, uVertices*sizeof(D3DTLVERTEX));

	for (index = 0; index < randCount; index++)
	{
		hr = (*vb)->Unlock();
		
		if (FAILED(hr))
		{
			WriteToLog("%d: ChangeTLVertexBufferInList - %dth Unlock() failed on the buffer with hr=%s(%x).\n", m_uTestNumber, index, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

Exit:
	if (FAILED(hr))
	{
		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			if (!m_FixedEnd)
				m_IsOutOfMemory = true;
			result = SKIP;
		}
		else
		{
			Fail();
			result = FAIL;
		}
	}

	delete [] pVertices;
	
	return result;
}

//-----------------------------------------------------------------------------

int ListTest::AddVertexBufferToList()
{
	CVertexBuffer *pVB       = NULL;
	VERTEX        *pVertices = NULL;
	RESINFO       *pResInfo  = NULL;
	PRIM_TYPES     type      = IS_VERTEXBUFFER;
	HRESULT        hr        = S_OK;
	int            result    = RUN;
	int            randCount = rand()%1000+1;
	int            randFlag  = rand()%2;
	DWORD          uTriNum; //shows how many rectangles will be drawn
	UINT           uVertices = 0; 
	FLOAT          wcell     = 0.0f;

	pResInfo = new RESINFO;
	m_TotalNumberOfSlices = 0;

	if (m_input)
	{
		if ( fscanf( m_input, "%d", &uTriNum) != 1)
		{
			WriteToLog("AddVertexBufferToList - error reading from file.\n");		
			hr = E_FAIL;
			goto Exit;
		}
	}
	else
		uTriNum = 1 << rand()%13; 

	uVertices = 2*(uTriNum + 1); 
	wcell     = 2.f/uTriNum;
	
	if ( m_output )
		fprintf(m_output, "%d\n", uVertices); 

	pVertices = new VERTEX[uVertices];

	if ( NULL == pVertices )
	{
		WriteToLog("AddVertexBufferToList - out of memory on allocating vertex array.\n");
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	pVertices[0] = VERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   0.f,  0.f);
	pVertices[1] = VERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   0.f,  0.f);

	for (UINT i = 1; i <= uTriNum; i++)
	{
		pVertices[i*2  ] = VERTEX(D3DXVECTOR3( -1.f+i*wcell, -1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   0.f,  0.f);
		pVertices[i*2+1] = VERTEX(D3DXVECTOR3( -1.f+i*wcell,  1.f, 0.5f), D3DXVECTOR3( 0.f, 0.f, -1.f),   0.f,  0.f);
	}

	hr = m_pSrcDevice->CreateVertexBuffer( uVertices*sizeof(VERTEX), 0, FVF_VERTEX, POOL_MANAGED, &pVB );
	if (FAILED(hr))
	{
		WriteToLog("%d: AddVertexBufferToList - CreateVertexBuffer() failed with hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	VOID* pV = NULL;

	UINT index;
	for (index = 0; index < randCount; index++)
	{
		hr = pVB->Lock( 0, uVertices*sizeof(VERTEX), (BYTE**)&pV, randFlag == 0 ? 0 : LOCK_READONLY );
		
		if (FAILED(hr) || NULL == pV)
		{
			for (UINT i = 0; i < index; i++)
				pVB->Unlock();
			
			WriteToLog("%d: AddVertexBufferToList - %dth Lock() failed on the buffer with hr=%s(%x).\n", m_uTestNumber, index, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

	ASSERT(pV != NULL);
	memcpy( pV, pVertices, uVertices*sizeof(VERTEX));

	for (index = 0; index < randCount; index++)
	{
		hr = pVB->Unlock();

		if (FAILED(hr))
		{
			WriteToLog("%d: AddVertexBufferToList - %dth Unlock() failed on the buffer with hr=%s(%x).\n", m_uTestNumber, index, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}
	
	m_TotalNumberOfSlices = 1;
	m_VBuffersList.insert(m_VBuffersList.end(), pVB);

	if (m_dwVersion >= 0x0800)
		m_CurrentOccupiedMemory = pVB->GetSize();
	
Exit:
	if (FAILED(hr))
	{
		RELEASE(pVB);

		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			if (!m_FixedEnd)
				m_IsOutOfMemory = true;
			type = IS_NULL;
			result = SKIP;
		}
		else
		{
			//Fail();
			result = FAIL;
			m_VBuffersList.insert(m_VBuffersList.end(), (CVertexBuffer *) NULL);
		}
	}

	pResInfo->Type = type;
	pResInfo->NumSlices = m_TotalNumberOfSlices;
	if (m_TotalNumberOfSlices > 0)
	{
		pResInfo->pCheckColor = new bool[m_TotalNumberOfSlices];
		pResInfo->pColors = new D3DCOLOR[m_TotalNumberOfSlices];
		memset(pResInfo->pCheckColor, true, m_TotalNumberOfSlices);
		ZeroMemory(pResInfo->pColors, sizeof(D3DCOLOR) * m_TotalNumberOfSlices);
	}
	m_ResInfosList.insert(m_ResInfosList.end(), pResInfo);
	
	sprintf(msgString, "%sVB %d, ", msgString, uVertices);
	sprintf(m_Message, "%sVB %d\n", m_Message, uVertices);

	delete [] pVertices;
	
	return result;
}

//-----------------------------------------------------------------------------

int ListTest::AddLVertexBufferToList()
{
	CVertexBuffer *pVB       = NULL;
	LVERTEX       *pVertices = NULL;
	RESINFO       *pResInfo  = NULL;
	PRIM_TYPES     type      = IS_LVERTEXBUFFER;
	HRESULT        hr        = S_OK;
	int            result    = RUN;
	UINT           uVertices = 0;
	FLOAT          wcell     = 0.f;
	
	int   randCount = rand()%1000+1;
	int   randFlag = rand()%2;
	DWORD uTriNum; //shows how many rectangles will be drawn

	pResInfo = new RESINFO;
	m_TotalNumberOfSlices = 0;

	if (m_input)
	{
		if ( fscanf( m_input, "%d", &uTriNum) != 1)
		{
			WriteToLog("AddLVertexBufferToList - error reading from file.\n");
			hr = E_FAIL;
			goto Exit;
		}
	}
	else
		uTriNum = 1 << rand()%13; 

	uVertices  = 2*(uTriNum + 1);
	wcell      = 2.f/uTriNum;
	
    if ( m_output )
		fprintf(m_output, "%d\n", uVertices); 

	pVertices = new LVERTEX[uVertices];

	if ( NULL == pVertices )
	{
		WriteToLog("AddLVertexBufferToList - out of memory on allocating vertex array.\n");
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	pVertices[0] = LVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xff0000ff, 0, 0.f, 0.f);
	pVertices[1] = LVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xff0000ff, 0, 0.f, 0.f);

	for (UINT i = 1; i <= uTriNum; i++)
	{
		pVertices[i*2  ] = LVERTEX(D3DXVECTOR3( -1.f+i*wcell, -1.f, 0.5f), 0xff0000ff, 0, 0.f,  0.f);
		pVertices[i*2+1] = LVERTEX(D3DXVECTOR3( -1.f+i*wcell,  1.f, 0.5f), 0xff0000ff, 0, 0.f,  0.f);
	}

	hr = m_pSrcDevice->CreateVertexBuffer( uVertices*sizeof(LVERTEX), 0, FVF_LVERTEX, POOL_MANAGED, &pVB);
	if (FAILED(hr))
	{
		WriteToLog("AddLVertexBufferToList - CreateVertexBuffer() failed with hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	VOID* pV = NULL;

	UINT index;
	for (index = 0; index < randCount; index++)
	{
		hr = pVB->Lock( 0, uVertices*sizeof(LVERTEX), (BYTE**)&pV, randFlag == 0 ? 0 : LOCK_READONLY );
		
		if (FAILED(hr) || NULL == pV)
		{
			for (UINT i = 0; i < index; i++)
				pVB->Unlock();
			
			WriteToLog("AddLVertexBufferToList - %dth Lock() failed on the buffer with hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

	ASSERT(pV != NULL);
	memcpy( pV, pVertices, uVertices*sizeof(LVERTEX));

	for (index = 0; index < randCount; index++)
	{
		hr = pVB->Unlock();
		
		if (FAILED(hr))
		{
			WriteToLog("AddLVertexBufferToList - %dth Unlock() failed on the buffer with hr=%s(%x).\n", index, m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
	}

	m_TotalNumberOfSlices = 1;
	m_VBuffersList.insert(m_VBuffersList.end(), pVB);

	if (m_dwVersion >= 0x0800)
		m_CurrentOccupiedMemory = pVB->GetSize();
	
Exit:
	if (FAILED(hr))
	{
		RELEASE(pVB);

		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			if (!m_FixedEnd)
				m_IsOutOfMemory = true;
			type = IS_NULL;
			result = SKIP;
		}
		else
		{
			//Fail();
			result = FAIL;
			m_VBuffersList.insert(m_VBuffersList.end(), (CVertexBuffer *) NULL);
		}
	}

	pResInfo->Type = type;
	pResInfo->NumSlices = m_TotalNumberOfSlices;
	if (m_TotalNumberOfSlices > 0)
	{
		pResInfo->pCheckColor = new bool[m_TotalNumberOfSlices];
		pResInfo->pColors = new D3DCOLOR[m_TotalNumberOfSlices];
		memset(pResInfo->pCheckColor, true, m_TotalNumberOfSlices);
		ZeroMemory(pResInfo->pColors, sizeof(D3DCOLOR) * m_TotalNumberOfSlices);
	}
	m_ResInfosList.insert(m_ResInfosList.end(), pResInfo);
	
	sprintf(msgString, "%sLVB %d, ", msgString, uVertices);
	sprintf(m_Message, "%sLVB %d\n", m_Message, uVertices);

	delete [] pVertices;
	
	return result;
}

//-----------------------------------------------------------------------------

HRESULT ListTest::AddTextureVertexBufferToList()
{
	VERTEX         pVertices[4];
	CVertexBuffer *pVB    = NULL;
	VOID          *pV     = NULL;
	HRESULT        hr     = S_OK;
	int            result = RUN;

	pVertices[0] = VERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f ), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f ); //  tu,  tv);
	pVertices[1] = VERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f ), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f ); //  tu, 0.f);
	pVertices[2] = VERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f ), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f ); // 0.f,  tv);
	pVertices[3] = VERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f ), D3DXVECTOR3( 0.f, 0.f, -1.f), 0.f, 0.f ); // 0.f, 0.f);

	hr = m_pSrcDevice->CreateVertexBuffer( 4*sizeof(VERTEX), 0, FVF_VERTEX, POOL_MANAGED, &pVB );
	if (FAILED(hr))
	{
		WriteToLog("%d: AddTextureVertexBufferToList - CreateVertexBuffer() failed with hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	hr = pVB->Lock( 0, sizeof(pVertices), (BYTE**)&pV, 0 );

	if (FAILED(hr) || NULL == pV)
	{
		WriteToLog("%d: AddTextureVertexBufferToList - Lock() failed on the  buffer hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	memcpy( pV, pVertices, sizeof(pVertices));

	hr = pVB->Unlock();

	if (FAILED(hr))
	{
		WriteToLog("%d: AddTextureVertexBufferToList - Unlock() failed on the buffer with hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}
	
	m_VBuffersList.insert(m_VBuffersList.end(), pVB);

	if (m_dwVersion >= 0x0800)
		m_CurrentOccupiedMemory += pVB->GetSize();
	
Exit:
	if (FAILED(hr))
		RELEASE(pVB);

	return hr;
}

//-----------------------------------------------------------------------------

HRESULT ListTest::AddCubeMapVertexBufferToList()
{
	VOLVERTEX      pVertices[24];
	CVertexBuffer *pVB    = NULL;
	VOID          *pV     = NULL;
	HRESULT        hr     = S_OK;
	int            result = RUN;

	pVertices[0]  = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3( 0.99f,  0.5f, -0.5f));
	pVertices[1]  = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3( 0.99f,  0.5f, -0.5f));
	pVertices[2]  = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3( 0.99f,  0.5f, -0.5f));
	pVertices[3]  = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3( 0.99f,  0.5f, -0.5f));
	pVertices[4]  = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(-0.99f,  0.5f,  0.5f));
	pVertices[5]  = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(-0.99f,  0.5f,  0.5f));
	pVertices[6]  = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(-0.99f,  0.5f,  0.5f));
	pVertices[7]  = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(-0.99f,  0.5f,  0.5f));
	pVertices[8]  = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f, 0.99f, -0.5f));
	pVertices[9]  = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f, 0.99f, -0.5f));
	pVertices[10] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f, 0.99f, -0.5f));
	pVertices[11] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f, 0.99f, -0.5f));
	pVertices[12] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,-0.99f,  0.5f));
	pVertices[13] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,-0.99f,  0.5f));
	pVertices[14] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,-0.99f,  0.5f));
	pVertices[15] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,-0.99f,  0.5f));
	pVertices[16] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f,  0.99f));
	pVertices[17] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f,  0.99f));
	pVertices[18] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f,  0.99f));
	pVertices[19] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f,  0.99f));
	pVertices[20] = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f, -0.99f));
	pVertices[21] = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f, -0.99f));
	pVertices[22] = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f, -0.99f));
	pVertices[23] = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.f), 0xffffffff, D3DXVECTOR3(  0.5f,  0.5f, -0.99f));

	hr = m_pSrcDevice->CreateVertexBuffer( 24*sizeof(VOLVERTEX), 0, FVF_VOLVERTEX, POOL_MANAGED, &pVB );
	if (FAILED(hr))
	{
		WriteToLog("%d: AddCubeMapVertexBufferToList - CreateVertexBuffer() failed with hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	hr = pVB->Lock( 0, 24*sizeof(VOLVERTEX), (BYTE**)&pV, 0 );
	
	if (FAILED(hr) || NULL == pV)
	{
		WriteToLog("%d: AddCubeMapVertexBufferToList - Lock() failed on the buffer with hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	memcpy( pV, pVertices, 24*sizeof(VOLVERTEX));

	hr = pVB->Unlock();
	
	if (FAILED(hr))
	{
		WriteToLog("%d: AddCubeMapVertexBufferToList - Unlock() failed on the buffer with hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	m_VBuffersList.insert(m_VBuffersList.end(), pVB);

	if (m_dwVersion >= 0x0800)
		m_CurrentOccupiedMemory += pVB->GetSize();
	
Exit:
	if (FAILED(hr))
		RELEASE(pVB);

	return hr;
}
	
//-----------------------------------------------------------------------------

HRESULT ListTest::AddVolVertexBufferToList(UINT uLevels, UINT Depth, UINT NumSlices)
{
	CVertexBuffer *pVB       = NULL;
	VOLVERTEX     *pVertices = NULL;
	VOID          *pV        = NULL;
	HRESULT        hr        = S_OK;
	int            result    = RUN;
	UINT           offset    = 0;
	UINT           SliceCount;
	
	pVertices = new VOLVERTEX[4*NumSlices];

	if ( NULL == pVertices )
	{
		WriteToLog("%d: AddVolVertexBufferToList - out of memory on allocating vertex array.\n", m_uTestNumber);
		result = E_OUTOFMEMORY;
		goto Exit;
	}

	SliceCount = Depth;
	for(UINT level = 0; level < uLevels; level++)
	{
		for (UINT sliceIndex = 0; sliceIndex < SliceCount; sliceIndex++)
		{
			FLOAT w = (FLOAT)sliceIndex/SliceCount + 1.0f/(2.0f*SliceCount);
			pVertices[offset+sliceIndex*4+0]  = VOLVERTEX(D3DXVECTOR3(  1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(0.f,  0.f, w));
			pVertices[offset+sliceIndex*4+1]  = VOLVERTEX(D3DXVECTOR3(  1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(0.f,  0.f, w));
			pVertices[offset+sliceIndex*4+2]  = VOLVERTEX(D3DXVECTOR3( -1.f, -1.f, 0.5f), 0xffffffff, D3DXVECTOR3(0.f,  0.f, w));
			pVertices[offset+sliceIndex*4+3]  = VOLVERTEX(D3DXVECTOR3( -1.f,  1.f, 0.5f), 0xffffffff, D3DXVECTOR3(0.f,  0.f, w));
		}
		offset += 4*SliceCount;
		SliceCount = max(1, SliceCount/2);
	}

	hr = m_pSrcDevice->CreateVertexBuffer( 4*NumSlices*sizeof(VOLVERTEX), 0, FVF_VOLVERTEX, POOL_MANAGED, &pVB );
	if (FAILED(hr))
	{
		WriteToLog("%d: AddVolVertexBufferToList - CreateVertexBuffer() failed with hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}
	
	hr = pVB->Lock( 0, 4*NumSlices*sizeof(VOLVERTEX), (BYTE**)&pV, 0 );

	if (FAILED(hr) || NULL == pV)
	{
		WriteToLog("%d: AddVolVertexBufferToList - Lock() failed with hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	memcpy( pV, pVertices, 4*NumSlices*sizeof(VOLVERTEX));

	hr = pVB->Unlock();

	if (FAILED(hr))
	{
		WriteToLog("%d: AddVolVertexBufferToList - Unlock() failed with hr=%s(%x).\n", m_uTestNumber, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	m_VBuffersList.insert(m_VBuffersList.end(), pVB);

	if (m_dwVersion >= 0x0800)
		m_CurrentOccupiedMemory += pVB->GetSize();
	
Exit:
	if (FAILED(hr))
		RELEASE(pVB);

	delete [] pVertices;

	return hr;
}

//-----------------------------------------------------------------------------

int ListTest::AddTextureToList(UINT texIndex, WORD mean, WORD distribution)
{
	SURFACEDESC pDesc;
	int         result   = RUN;
	HRESULT		hr		 = S_OK;
	CTexture   *pTexture = NULL;
	CImage    **ppImages = NULL;
	RESINFO    *pResInfo = NULL;
	PRIM_TYPES  type     = IS_TEXTURE;
	bool        isDXTn   = false;
	bool        isUYV    = false;
	D3DCOLOR    Stripes[7];
	FORMAT      Format; 
	DWORD       color_offset = 100;
	UINT        uWidth;
	UINT        uHeight;
	UINT        uLevels;
	FLOAT       randNumber1, randNumber2;
	bool		is_failed;
	TCHAR       szBuffer[30];

	pResInfo = new RESINFO;
	
	m_CurrentOccupiedMemory = 0;
	m_TotalNumberOfSlices   = 0;
	DWORD color_index = 10*texIndex;
	
	m_pCurrentMode->nTextureFormat = texIndex % m_uCommonTextureFormats;
	Format = m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat];
	FmtToString(FORMAT_TEXTURE, &Format, szBuffer);
	
	if ( m_input )
	{
		if ( fscanf( m_input, "%d %d %d", &uWidth, &uHeight, &uLevels) != 3)
		{
			WriteToLog("%d: AddTextureToList(%s) - error reading from file.\n", m_uTestNumber, szBuffer);
			hr = E_FAIL;
			goto Exit;
		}
	}
	else
	{
		if ( !m_UseMipMaps || !m_bWrapAndMipTex[texIndex%m_uCommonTextureFormats] || Format.d3dfFormat == FMT_A1)
			uLevels = 1;
		else 
			uLevels = rand()%2;

		switch (Format.ddpfPixelFormat.dwFourCC)
		{
			case FMT_DXT1: case FMT_DXT2: case FMT_DXT3: case FMT_DXT4: case FMT_DXT5: 
				isDXTn = true;
				break;
			case FMT_UYVY: case FMT_YUY2: case FMT_G8R8_G8B8: case FMT_R8G8_B8G8:
				isUYV = true;
				break;
		}

		//generation of texture's width and height using normal distribution

		if (mean+distribution > min(m_MaxTextureWidth, m_MaxTextureHeight))
			mean = min(m_MaxTextureWidth, m_MaxTextureHeight)-distribution;
		randNumber1 = (FLOAT)(rand()%m_MaxTextureWidth)/m_MaxTextureWidth;
		randNumber2 = (FLOAT)(rand()%m_MaxTextureHeight)/m_MaxTextureHeight;
		uWidth    = (UINT)(abs( mean + distribution*(FLOAT)cos(2*PI*randNumber2)*sqrt((-2)*log(randNumber1)) ));
		uHeight   = (UINT)(abs( mean + distribution*(FLOAT)cos(2*PI*randNumber1)*sqrt((-2)*log(randNumber2)) ));
		
		int t;
		if (ReadInteger("tw", &t))
			uWidth = t;
		if (ReadInteger("th", &t))
			uHeight = t;
		if (KeySet("onelev"))
			uLevels = 1;

		if ((m_TextureIsPow2 && !m_NonPow2Conditional)       || 
			(m_NonPow2Conditional && (isDXTn || !uLevels))   ||
		    (!uLevels && m_dwVersion < 0x0800)                )//nonpow2 mipmaped textures are broken on dx7
		{
			//need to do this check here in case if m_MaxTextureWidth or m_MaxTextureHeight is nonpowtwo
			uWidth  = min( uWidth,  m_MaxTextureWidth  );
			uHeight = min( uHeight, m_MaxTextureHeight );
			
			int pow1  = (int)(log((float)uWidth)/log(2.f));
			int pow2  = (int)(log((float)uHeight)/log(2.f));
			uWidth  = 1 << pow1; //pow(2, pow1);
			uHeight = 1 << pow2; //pow(2, pow2);
		}

		//adjust width to devidable by 4  number for DXTN formats
		if (isDXTn)
		{
			uWidth  = max((uWidth >>2) << 2, 4); //max((uWidth /4)*4, 4);
			uHeight = max((uHeight>>2) << 2, 4); //max((uHeight/4)*4, 4);
		}

		//adjust width to even number for YUVs and packed RGB formats
		if (isUYV)
		{
			uWidth  = max((uWidth >>1) << 1, 2); //max((uWidth /2)*2, 2);
			//uHeight = max((uHeight>>1) << 1, 2); //max((uHeight/2)*2, 2);
		}

		//adjust width to divisable by 32 for the A1 format
		if (Format.d3dfFormat == FMT_A1)
		{
			uWidth  = max((uWidth >>5) << 5, 32); //max((uWidth / 32)*32, 32);
		}

		if (m_SquareOnly)
			uHeight = uWidth;

		uWidth  = min( uWidth,  m_MaxTextureWidth  );
		uWidth  = max( uWidth,  m_pSrcDevice->GetCaps()->dwMinTextureWidth  );
		uHeight = min( uHeight, m_MaxTextureHeight );
		uHeight = max( uHeight, m_pSrcDevice->GetCaps()->dwMinTextureHeight );

		// The A1 texture format has a height restriction of 2048
		if (Format.d3dfFormat == FMT_A1)
		{
			uHeight = min( 2048, uHeight );
		}

		if ( m_output )
			fprintf(m_output, "%d %d %d\n", uWidth, uHeight, uLevels);
	}

	hr = m_pSrcDevice->CreateTexture(uWidth, uHeight, uLevels, 0, Format, POOL_MANAGED, &pTexture);
	if (FAILED(hr))
	{
		WriteToLog("%d: AddTextureToList(%s) - CreateTexture() failed with hr=%s(%x).\n", m_uTestNumber, szBuffer, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}
	
	if(pTexture->IsPalettized())
		pTexture->SetPalette(NULL);

	uLevels = pTexture->GetLevelCount();

	ppImages = new CImage*[uLevels];

	if ( NULL == ppImages )
	{
		WriteToLog("%d: AddTextureToList(%s) - Out of memory on allocating ppImages.\n", m_uTestNumber, szBuffer);
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	ZeroMemory(ppImages, sizeof(CImage *) * uLevels);

	for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
	{
		ppImages[LevelIndex] = new CImage();

		if ( NULL == ppImages[LevelIndex] )
		{
			WriteToLog("%d: AddTextureToList(%s) - Out of memory on allocating ppImages[%d].\n", m_uTestNumber, szBuffer, LevelIndex);
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		pTexture->GetLevelDesc(LevelIndex, &pDesc);
		WORD index = (WORD)(color_index+LevelIndex*20);
		WORD color = (WORD)(index % (256-color_offset) + color_offset);

		if ((Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
			 Format.d3dfFormat == FMT_A8   || Format.d3dfFormat == FMT_A1 ||
			 Format.d3dfFormat == FMT_R16F || Format.d3dfFormat == FMT_R32F)
		{
			Stripes[0] = Stripes[1] = Stripes[2] = Stripes[3] = 
			Stripes[4] = Stripes[5] = Stripes[6] = RGBA_MAKE(color, color, color, color);
		}
		else
		{
			Stripes[0] = RGBA_MAKE(color, 0, 0, 255); 
			Stripes[1] = RGBA_MAKE(0, color, 0, 255); 
			Stripes[2] = RGBA_MAKE(0, 0, color, 255); 
			Stripes[3] = RGBA_MAKE(0, color, color, 255); 
			Stripes[4] = RGBA_MAKE(color, 0, color, 255); 
			Stripes[5] = RGBA_MAKE(color, color, 0, 255); 
			Stripes[6] = RGBA_MAKE(color, color, color, 255);
		}

		if (!ppImages[LevelIndex]->LoadStripes(pDesc.Width, pDesc.Height, 1, Stripes+texIndex%7, false))
		{
			WriteToLog("%d: AddTextureToList(%s) - out of memory on LoadStripes() on level %d.\n", m_uTestNumber, szBuffer, LevelIndex);
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		switch (Format.d3dfFormat)
		{
			case FMT_A1:
				m_CurrentOccupiedMemory += ((pDesc.Width+7)/8)*pDesc.Height;
				break;
			case FMT_DXT1:
				m_CurrentOccupiedMemory += ((pDesc.Width+3)/4)*((pDesc.Height+3)/4)*8;
				break;
			case FMT_DXT2: case FMT_DXT3: case FMT_DXT4: case FMT_DXT5: 
				m_CurrentOccupiedMemory += ((pDesc.Width+3)/4)*((pDesc.Height+3)/4)*16;
				break;
			case FMT_UYVY: case FMT_YUY2:
				m_CurrentOccupiedMemory += ((pDesc.Width+1)/2)*pDesc.Height*4;
				break;
			default:
				m_CurrentOccupiedMemory += (pDesc.Format.ddpfPixelFormat.dwRGBBitCount/8)*pDesc.Width*pDesc.Height;
		}
	}
			
	hr = pTexture->LoadImage(0, ppImages, TEXLOAD_DEFAULT);

	if (FAILED(hr))
	{
		WriteToLog("%d: AddTextureToList(%s) - LoadImage() failed with hr=%s(%x)\n", m_uTestNumber, szBuffer, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	hr = AddTextureVertexBufferToList();

	if (FAILED(hr))
		goto Exit;

	m_TotalNumberOfSlices = uLevels;
	m_TexturesList.insert(m_TexturesList.end(), pTexture);
		
Exit:
	if (FAILED(hr))
	{
		m_CurrentOccupiedMemory = 0;
		RELEASE(pTexture);

		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			if (!m_FixedEnd)
				m_IsOutOfMemory = true;
			type = IS_NULL;
			result = SKIP;
		}
		else
		{
			//Fail();
			result = FAIL;
			m_TexturesList.insert(m_TexturesList.end(), (CTexture *) NULL);
			m_VBuffersList.insert(m_VBuffersList.end(), (CVertexBuffer *) NULL);
		}
	}

	pResInfo->Type = (PRIM_TYPES)type;
	pResInfo->NumSlices = m_TotalNumberOfSlices;
	if (m_TotalNumberOfSlices > 0)
	{
		pResInfo->pCheckColor = new bool[m_TotalNumberOfSlices];
		pResInfo->pColors = new D3DCOLOR[m_TotalNumberOfSlices];
		memset(pResInfo->pCheckColor, true, m_TotalNumberOfSlices);
		ZeroMemory(pResInfo->pColors, sizeof(D3DCOLOR) * m_TotalNumberOfSlices);
	}
	pResInfo->Format = Format;
	m_ResInfosList.insert(m_ResInfosList.end(), pResInfo);
	
	sprintf(msgString, "%sT %s %dx%d %d, ", msgString, szBuffer, uWidth, uHeight, uLevels);
	sprintf(m_Message, "%sT %s %dx%d %d\n", m_Message, szBuffer, uWidth, uHeight, uLevels);

	if (NULL != ppImages)
	for (UINT k = 0; k < uLevels; k++)
		SAFEDELETE(ppImages[k]);

	SAFEDELETEA(ppImages);
		
	return result;
}

//-----------------------------------------------------------------------------


int ListTest::AddCubeMapToList(UINT cubeIndex, WORD mean, WORD distribution)
{
	CCubeTexture *pCubeMap  = NULL;
	CImage     ***pppImages = NULL;
	RESINFO      *pResInfo  = NULL;
	PRIM_TYPES    type      = IS_CUBEMAP;
	HRESULT       hr        = S_OK;
	int           result    = RUN;
	bool          isDXTn    = false;
	bool          isUYV     = false;
	D3DCOLOR      Stripes[6];
	UINT          uWidth;
	UINT          uLevels;
	FORMAT        Format;
	FLOAT         randNumber1, randNumber2;
	TCHAR         szBuffer[30];
	DWORD         color_offset, color_index;

	pResInfo = new RESINFO;

	m_CurrentOccupiedMemory = 0;
	m_TotalNumberOfSlices  = 0;
	color_offset = 100;
	color_index = cubeIndex*20;

	m_pCurrentMode->nCubeFormat = cubeIndex % m_uCommonCubeFormats;
	Format = m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat];
	FmtToString(FORMAT_TEXTURE, &Format, szBuffer);
		
	if (m_input)
	{
		if ( fscanf( m_input, "%d %d", &uWidth, &uLevels) != 2)
		{
			WriteToLog("%d: AddCubeMapToList(%s) - error reading from file.\n", m_uTestNumber, szBuffer);
			hr = E_FAIL;
			goto Exit;
		}
	}
	else
	{
		if ( !m_UseMipCubeMaps || !m_bWrapAndMipCube[cubeIndex%m_uCommonCubeFormats])
			uLevels = 1;
		else 
			uLevels = rand()%2;

		switch (Format.ddpfPixelFormat.dwFourCC)
		{
			case FMT_DXT1: case FMT_DXT2: case FMT_DXT3: case FMT_DXT4: case FMT_DXT5: 
				isDXTn = true;
				break;
			case FMT_UYVY: case FMT_YUY2: case FMT_G8R8_G8B8: case FMT_R8G8_B8G8:
				isUYV = true;
				break;
		}

		//generation of texture's width and height using normal distribution
		if (mean+distribution > m_MaxCubeWidth)
			mean = m_MaxCubeWidth-distribution;
		randNumber1 = (FLOAT)(rand()%m_MaxCubeWidth)/m_MaxCubeWidth;
		randNumber2 = (FLOAT)(rand()%m_MaxCubeWidth)/m_MaxCubeWidth;
		uWidth      = (UINT)(abs( mean + distribution*(FLOAT)cos(2*PI*randNumber2)*sqrt((-2)*log(randNumber1)) ));
		
		int t;
		if (ReadInteger("cw", &t))
		{ uWidth = t;}
		if (KeySet("onelev"))
			uLevels = 1;

		if ((m_CubeMapIsPow2 /*&& !m_NonPow2Conditional*/) || 
			/*(m_NonPow2Conditional && (isDXTn || !uLevels)) ||*/
		     m_dwVersion < 0x0800) //nonpow2 cube maps are broken on dx7 interfaces
		{
			//need to do this check here in case if m_MaxCubeWidth is nonpowtwo
			uWidth  = min( uWidth,  m_MaxCubeWidth  );
			
			int pow  = (int)(log((float)uWidth)/log(2.f));
			uWidth  = 1 << pow;
		}

		//adjust width to devidable by 4  number for DXTN formats
		if (isDXTn)
			uWidth  = max((uWidth /4)*4, 4);

		//adjust width to even number for YUVs and packed RGB formats
		if (isUYV)
			uWidth  = max((uWidth /2)*2, 2);
		
		uWidth  = min( uWidth,  m_MaxCubeWidth );
		uWidth  = max( uWidth,  m_pSrcDevice->GetCaps()->dwMinTextureWidth );

		if (m_output )
			fprintf(m_output, "%d %d\n", uWidth, uLevels);
	}

	hr = m_pSrcDevice->CreateCubeTexture( uWidth, uLevels, 0, Format, POOL_MANAGED, &pCubeMap );
	if (FAILED(hr))
	{
		//some formats from m_pCommonCubeFormats are not supported as cube maps in dx7 
		//since the list for textures and cube maps is the same in dx7 -> need to skip these cases
		if (m_dwVersion == 0x0700 && E_NOTIMPL == hr)
		{
			type = IS_NULL;
			m_pSrcDevice->GetLastError();
			WriteToLog("%d: AddCubeMapToList - format %s is not supported as cube map. CreateCubeTexture() failed with hr=%s(%x).\n", m_uTestNumber, szBuffer, m_pD3D->HResultToString(hr), hr);
			hr = S_OK;
		}
		else
			WriteToLog("%d: AddCubeMapToList(%s) - CreateCubeTexture() failed with hr=%s(%x).\n", m_uTestNumber, szBuffer, m_pD3D->HResultToString(hr), hr);

		goto Exit;
	}

	if(pCubeMap->IsPalettized())
		pCubeMap->SetPalette(NULL);

	uLevels = pCubeMap->GetLevelCount();

	pppImages = new CImage**[uLevels];

	if ( NULL == pppImages )
	{
		WriteToLog("%d: AddCubeMapToList(%s) - out of memory on allocating pppImages.\n", m_uTestNumber, szBuffer);
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

	for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++ )
	{
		SURFACEDESC pDesc;
		pCubeMap->GetLevelDesc(LevelIndex, &pDesc);
		WORD index = (WORD)(color_index+LevelIndex*20);
		WORD color = (WORD)(index % (256-color_offset) + color_offset);

		pppImages[LevelIndex] = new CImage*[6];

		if ( NULL == pppImages[LevelIndex] )
		{
			WriteToLog("%d: AddCubeMapToList(%s) - out of memory on allocating pppImages[%d].\n", m_uTestNumber, szBuffer, LevelIndex);
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		ZeroMemory(pppImages[LevelIndex], sizeof(CImage *) * 6);

		if ((Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
			 Format.d3dfFormat == FMT_A8   ||
			 Format.d3dfFormat == FMT_R16F || Format.d3dfFormat == FMT_R32F)
		{
			for (UINT i = 0; i < 6; i++)
			{
				index = (WORD)(color_index+LevelIndex*20+i*20);
				color = (WORD)(index % (256-color_offset) + color_offset);
				Stripes[i] = RGBA_MAKE(color, color, color, color); 
			}
		}
		else
		{
			Stripes[0] = RGBA_MAKE(color, 0, 0, 255); 
			Stripes[1] = RGBA_MAKE(0, color, 0, 255); 
			Stripes[2] = RGBA_MAKE(0, 0, color, 255); 
			Stripes[3] = RGBA_MAKE(0, color, color, 255); 
			Stripes[4] = RGBA_MAKE(color, 0, color, 255); 
			Stripes[5] = RGBA_MAKE(color, color, 0, 255); 
		}

		for (UINT FaceIndex = 0; FaceIndex < 6; FaceIndex++)
		{

			pppImages[LevelIndex][FaceIndex] = new CImage();

			if ( NULL == pppImages[LevelIndex][FaceIndex] )
			{
				WriteToLog("%d: AddCubeMapToList(%s) - out of memory on allocating pppImages[%d][%d].\n", m_uTestNumber, szBuffer, LevelIndex, FaceIndex);
				hr = E_OUTOFMEMORY;
				goto Exit;
			}

			if (!pppImages[LevelIndex][FaceIndex]->LoadStripes(pDesc.Width, pDesc.Width, 1, Stripes+(FaceIndex+cubeIndex)%6, false))
			{
				WriteToLog("%d: AddCubeMapToList(%s) - out of memory on LoadStripes() on face %d for level %d\n", m_uTestNumber, szBuffer, FaceIndex, LevelIndex);
				hr = E_OUTOFMEMORY;
				goto Exit;
			}
		}
		switch (Format.ddpfPixelFormat.dwFourCC)
		{
			case FMT_DXT1:
				m_CurrentOccupiedMemory += 6*((pDesc.Width+3)/4)*((pDesc.Height+3)/4)*8;
				break;
			case FMT_DXT2: case FMT_DXT3: case FMT_DXT4: case FMT_DXT5: 
				m_CurrentOccupiedMemory += 6*((pDesc.Width+3)/4)*((pDesc.Height+3)/4)*16;
				break;
			case FMT_UYVY: case FMT_YUY2:
				m_CurrentOccupiedMemory += 6*((pDesc.Width+1)/2)*pDesc.Height*4;
				break;
			default:
				m_CurrentOccupiedMemory += 6*(pDesc.Format.ddpfPixelFormat.dwRGBBitCount/8)*pDesc.Width*pDesc.Height;
		}
	}

	hr = pCubeMap->LoadImage(pppImages, TEXLOAD_DEFAULT);
	if (FAILED(hr))
	{
		WriteToLog("%d: AddCubeMapToList(%s) - LoadImage() failed with hr=%s(%x)\n", m_uTestNumber, szBuffer, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	hr = AddCubeMapVertexBufferToList();

	if (FAILED(hr))
		goto Exit;

	m_TotalNumberOfSlices = 6*uLevels;
	m_CubesList.insert(m_CubesList.end(), pCubeMap);
	

Exit:
	if (FAILED(hr))
	{
		m_CurrentOccupiedMemory = 0;
		RELEASE(pCubeMap);

		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			if (!m_FixedEnd)
				m_IsOutOfMemory = true;
			type = IS_NULL;
			result = SKIP;
		}
		else
		{
			//Fail();
			result = FAIL;
			m_CubesList.insert(m_CubesList.end(), (CCubeTexture *) NULL);
			m_VBuffersList.insert(m_VBuffersList.end(), (CVertexBuffer *) NULL);
		}
	}

	pResInfo->Type = (PRIM_TYPES)type;
	pResInfo->NumSlices = m_TotalNumberOfSlices;
	if (m_TotalNumberOfSlices > 0)
	{
		pResInfo->pCheckColor = new bool[m_TotalNumberOfSlices];
		pResInfo->pColors = new D3DCOLOR[m_TotalNumberOfSlices];
		memset(pResInfo->pCheckColor, true, m_TotalNumberOfSlices);
		ZeroMemory(pResInfo->pColors, sizeof(D3DCOLOR) * m_TotalNumberOfSlices);
	}
	pResInfo->Format = Format;
	m_ResInfosList.insert(m_ResInfosList.end(), pResInfo);
		
	sprintf(msgString, "%sCM %s %d %d, ", msgString, szBuffer, uWidth, uLevels);
	sprintf(m_Message, "%sCM %s %d %d\n", m_Message, szBuffer, uWidth, uLevels);

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


//-----------------------------------------------------------------------------

int ListTest::AddVolTextureToList(UINT volIndex, WORD mean, WORD distribution)
{
	HRESULT      hr          = S_OK;
	int          result      = RUN;
	CVolTexture *pVolTexture = NULL;
	CImage    ***pppImages   = NULL;
	RESINFO     *pResInfo    = NULL;
	PRIM_TYPES   type        = IS_VOLUME;
	bool         isDXTn      = false;
	bool         isUYV       = false;
	VOLUME_DESC  pDesc;
	DWORD        color_offset = 100;
	DWORD        color_index  = volIndex*20;
	D3DCOLOR     Stripe[1];
	UINT         uLevels;	
	UINT         uWidth, uHeight, uDepth;
	FORMAT       Format;
	FLOAT        randNumber1, randNumber2, randNumber3, randNumber4;
	TCHAR        szBuffer[30];

	pResInfo = new RESINFO;

	m_CurrentOccupiedMemory = 0;
	m_TotalNumberOfSlices   = 0;

	m_pCurrentMode->nVolumeFormat = volIndex % m_uCommonVolumeFormats;
	Format = m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat];
	FmtToString(FORMAT_TEXTURE, &Format, szBuffer);

	if (m_input)
	{
		if ( fscanf( m_input, "%d %d %d %d", &uWidth, &uHeight, &uDepth, &uLevels) != 4)
		{
			WriteToLog("%d: AddVolTextureToList(%s) - error reading from file.\n", m_uTestNumber, szBuffer);
			hr = E_FAIL;
			goto Exit;
		}
	}
	else
	{
		if (!m_UseMipVolMaps || !m_bWrapAndMipVol[volIndex%m_uCommonVolumeFormats])
			uLevels = 1; 
		else
			uLevels = rand()%2; 

		switch (Format.ddpfPixelFormat.dwFourCC)
		{
			case FMT_DXT1: case FMT_DXT2: case FMT_DXT3: case FMT_DXT4: case FMT_DXT5: 
				isDXTn = true;
				break;
			case FMT_UYVY: case FMT_YUY2: case FMT_G8R8_G8B8: case FMT_R8G8_B8G8:
				isUYV = true;
				break;
		}

		//generation of texture's width and height using normal distribution
		if (mean+distribution > m_MaxVolumeExtent)
			mean = m_MaxVolumeExtent-distribution;
		randNumber1 = (FLOAT)(rand()%m_MaxVolumeExtent)/m_MaxVolumeExtent;
		randNumber2 = (FLOAT)(rand()%m_MaxVolumeExtent)/m_MaxVolumeExtent;
		uWidth    = (UINT)(abs( mean + distribution*(FLOAT)cos(2*PI*randNumber2)*sqrt((-2)*log(randNumber1)) ));
		uHeight   = (UINT)(abs( mean + distribution*(FLOAT)cos(2*PI*randNumber1)*sqrt((-2)*log(randNumber2)) ));
		mean = 8, distribution = 10;
		randNumber3 = (FLOAT)(rand()%m_MaxVolumeExtent)/m_MaxVolumeExtent;
		randNumber4 = (FLOAT)(rand()%m_MaxVolumeExtent)/m_MaxVolumeExtent;
		uDepth    = (UINT)abs( mean + distribution*(FLOAT)cos(2*PI*randNumber4)*sqrt((-2)*log(randNumber3)) );

		int t;
		if (ReadInteger("vw", &t))
		{ uWidth = t;}
		if (ReadInteger("vh", &t))
		{ uHeight = t;}
		if (ReadInteger("vd", &t))
		{ uDepth = t;}
		if (KeySet("onelev"))
			uLevels = 1;

		if ((m_VolumeMapIsPow2 /*&& !m_NonPow2Conditional*/) /*|| 
			(m_NonPow2Conditional && (isDXTn || !uLevels))*/)
		{
			//need to do this check here in case m_MaxVolumeExtent is nonpowtwo
			uWidth  = min( uWidth,  m_MaxVolumeExtent );
			uHeight = min( uHeight, m_MaxVolumeExtent );

			int pow1  = (int)(log((float)uWidth )/log(2.f));
			int pow2  = (int)(log((float)uHeight)/log(2.f));
			int pow3  = (int)(log((float)uDepth )/log(2.f));
			uWidth    = 1 << pow1;
			uHeight   = 1 << pow2;
			uDepth    = 1 << pow3;
		}

		//adjust width to devidable by 4  number for DXTN formats
		if (isDXTn)
		{
			uWidth  = max((uWidth /4)*4, 4);
			uHeight = max((uHeight/4)*4, 4);
			uDepth  = max((uDepth /4)*4, 4);
		}

		//adjust width to even number for YUVs and packed RGB formats
		if (isUYV)
		{
			uWidth  = max((uWidth /2)*2, 2);
			//uHeight = max((uHeight/2)*2, 2);
			//uDepth  = max((uDepth /2)*2, 2);
		}

		if (m_SquareOnly)
			uHeight = uWidth = uDepth;
				
		uWidth  = min( uWidth,  m_MaxVolumeExtent );
		uWidth  = max( uWidth,  m_pSrcDevice->GetCaps()->dwMinVolumeExtent );
		uHeight = min( uHeight, m_MaxVolumeExtent );
		uHeight = max( uHeight, m_pSrcDevice->GetCaps()->dwMinVolumeExtent );
		uDepth  = min( uDepth,  min(32, m_MaxVolumeExtent) );
		uDepth  = max( uDepth,  m_pSrcDevice->GetCaps()->dwMinVolumeExtent );

		//bug 71530 is not fixed in dx9
		//we don't run the test with driver resource management any more since nobody supports it
		if (/*m_dwVersion < 0x0900 &&*/ /*KeySet(_T("D3DManage"))*/uLevels != 1  && 
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

		if ( m_output )
			fprintf(m_output, "%d %d %d %d\n", uWidth, uHeight, uDepth, uLevels);
	}	

	hr = m_pSrcDevice->CreateVolumeTexture( uWidth, uHeight, uDepth, uLevels, 0, Format, POOL_MANAGED, &pVolTexture );
	if (FAILED(hr))
	{
		WriteToLog("%d: AddVolTexturesToList(%s) - CreateVolumeTexture() failed with hr=%s(%x).\n", m_uTestNumber, szBuffer, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	if(pVolTexture->IsPalettized())
		pVolTexture->SetPalette(NULL);

	pVolTexture->GetLevelDesc(0, &pDesc);
	uLevels = pVolTexture->GetLevelCount();

	pppImages = new CImage**[uLevels];

	if ( NULL == pppImages )
	{
		WriteToLog("%d: AddVolTextureToList(%s) - Out of memory on allocating pppImages.\n", m_uTestNumber, szBuffer);
		hr = E_OUTOFMEMORY;
		goto Exit;
	}

	ZeroMemory(pppImages, sizeof(CImage **) * uLevels);

	for (UINT LevelIndex = 0; LevelIndex < uLevels; LevelIndex++)
	{
		pVolTexture->GetLevelDesc(LevelIndex, &pDesc);
		DWORD temp = (volIndex+LevelIndex)%7;
		m_TotalNumberOfSlices += pDesc.Depth;

        pppImages[LevelIndex] = new CImage*[pDesc.Depth];

        if (NULL == pppImages[LevelIndex])
		{
			WriteToLog("%d: AddVolTextureToList(%s) - Out of memory on allocating pppImages[%d].\n", m_uTestNumber, szBuffer, LevelIndex);
			hr = E_OUTOFMEMORY;
			goto Exit;
		}

		ZeroMemory(pppImages[LevelIndex], sizeof(CImage *) * pDesc.Depth);
    
		for (UINT SliceIndex = 0; SliceIndex < pDesc.Depth; SliceIndex++)
		{
			WORD index = (WORD)(color_index+SliceIndex*20);
			WORD color = (WORD)(index % (256-color_offset) + color_offset);

			if ((Format.ddpfPixelFormat.dwFlags & PF_LUMINANCE) ||
				 Format.d3dfFormat == FMT_A8   ||
				 Format.d3dfFormat == FMT_R16F || Format.d3dfFormat == FMT_R32F)
			{
				index = (WORD)(color_index+SliceIndex*20+temp*20);
				color = (WORD)(index % (256-color_offset) + color_offset);
				Stripe[0] = RGBA_MAKE(color, color, color, color);
			}
			else
			{
				WORD   red = temp%2 == 0 ? color : 0;
				WORD green = (temp+1)%4 >= 2 ? color : 0;
				WORD  blue = temp/3 != 0 ? color : 0;
				Stripe[0] = RGBA_MAKE(red, green, blue, 255);
			}

			pppImages[LevelIndex][SliceIndex] = new CImage();

			if ( NULL == pppImages[LevelIndex][SliceIndex] )
			{
				WriteToLog("%d: AddVolTextureToList(%s) - Out of memory on allocating pppImages[%d][%d].\n", m_uTestNumber, szBuffer, LevelIndex, SliceIndex);
				hr = E_OUTOFMEMORY;
				goto Exit;
			}

           	if (!pppImages[LevelIndex][SliceIndex]->LoadStripes(pDesc.Width, pDesc.Height, 1, Stripe, false))
			{
				WriteToLog("%d: AddVolTextureToList(%s) - out of memory on LoadStripes() on level %d on slice %d.\n", m_uTestNumber, szBuffer, LevelIndex, SliceIndex);
				hr = E_OUTOFMEMORY;
				goto Exit;
			}
		}

		switch (Format.ddpfPixelFormat.dwFourCC)
		{
			case FMT_DXT1:
				m_CurrentOccupiedMemory += pDesc.Depth*((pDesc.Width+3)/4)*((pDesc.Height+3)/4)*8;
				break;
			case FMT_DXT2: case FMT_DXT3: case FMT_DXT4: case FMT_DXT5: 
				m_CurrentOccupiedMemory += pDesc.Depth*((pDesc.Width+3)/4)*((pDesc.Height+3)/4)*16;
				break;
			case FMT_UYVY: case FMT_YUY2:
				m_CurrentOccupiedMemory += pDesc.Depth*((pDesc.Width+1)/2)*pDesc.Height*4;
				break;
			default:
				m_CurrentOccupiedMemory += pDesc.Depth*(pDesc.Format.ddpfPixelFormat.dwRGBBitCount/8)*pDesc.Width*pDesc.Height;
		}
	}

	hr = pVolTexture->LoadImage(pppImages, TEXLOAD_DEFAULT);
	
	if (FAILED(hr))
	{
		WriteToLog("%d: AddVolTextureToList(%s) - LoadImage() failed with hr=%s(%x).\n", m_uTestNumber, szBuffer, m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	hr = AddVolVertexBufferToList(uLevels, uDepth, m_TotalNumberOfSlices);
	
	if (FAILED(hr))
		goto Exit;

	m_VolumesList.insert(m_VolumesList.end(), pVolTexture);
	
Exit:
	if (FAILED(hr))
	{
		m_CurrentOccupiedMemory = 0;
		m_TotalNumberOfSlices   = 0;
		RELEASE(pVolTexture);

		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			if (!m_FixedEnd)
				m_IsOutOfMemory = true;
			type = IS_NULL;
			result = SKIP;
		}
		else
		{
			//Fail();
			result = FAIL;
			m_VolumesList.insert(m_VolumesList.end(), (CVolTexture *) NULL);
			m_VBuffersList.insert(m_VBuffersList.end(), (CVertexBuffer *) NULL);
		}
	}

	pResInfo->Type = (PRIM_TYPES)type;
	pResInfo->NumSlices = m_TotalNumberOfSlices;
	if (m_TotalNumberOfSlices > 0)
	{
		pResInfo->pCheckColor = new bool[m_TotalNumberOfSlices];
		pResInfo->pColors = new D3DCOLOR[m_TotalNumberOfSlices];
		memset(pResInfo->pCheckColor, true, m_TotalNumberOfSlices);
		ZeroMemory(pResInfo->pColors, sizeof(D3DCOLOR) * m_TotalNumberOfSlices);
	}
	pResInfo->Format = Format;
	m_ResInfosList.insert(m_ResInfosList.end(), pResInfo);

	sprintf(msgString, "%sVOL %s %dx%dx%d %d, ", msgString, szBuffer, uWidth, uHeight, uDepth, uLevels);
	sprintf(m_Message, "%sVOL %s %dx%dx%d %d\n", m_Message, szBuffer, uWidth, uHeight, uDepth, uLevels);
	
	UINT nDepth = uDepth;
	if (NULL != pppImages)
	for (UINT i=0; i < uLevels; i++)
	{
		if (NULL != pppImages[i])
		for (UINT j = 0; j < nDepth; j++)
			SAFEDELETE(pppImages[i][j]);

		SAFEDELETEA(pppImages[i]);
		nDepth = max( 1, nDepth>>1 );
	}

	SAFEDELETEA(pppImages);
		
	return result;
}

//-----------------------------------------------------------------------------

void ListTest::DestroyAllVertexBuffersFromList()
{
    VBUFFERLIST::iterator i;

    for (i=m_VBuffersList.begin(); i != m_VBuffersList.end(); ++i)
		RELEASE(*i);

    m_VBuffersList.clear();
}

//-----------------------------------------------------------------------------

void ListTest::DestroyAllIndexBuffersFromList()
{
    IBUFFERLIST::iterator i;

    for (i = m_IBuffersList.begin(); i != m_IBuffersList.end(); ++i)
		RELEASE(*i);

    m_IBuffersList.clear();
}

//-----------------------------------------------------------------------------

void ListTest::DestroyAllTexturesFromList()
{
    TEXTURELIST::iterator i;
    int t=0;

    for (i=m_TexturesList.begin(); i != m_TexturesList.end(); ++i)
		RELEASE(*i);

    m_TexturesList.clear();
}

//-----------------------------------------------------------------------------

void ListTest::DestroyAllCubeMapsFromList()
{
    CUBEMAPLIST::iterator i;
    int t=0;

    for (i=m_CubesList.begin(); i != m_CubesList.end(); ++i)
		RELEASE(*i);

    m_CubesList.clear();
}

//-----------------------------------------------------------------------------

void ListTest::DestroyAllVolTexturesFromList()
{
    VOLTEXLIST::iterator i;
    int t=0;

    for (i=m_VolumesList.begin(); i != m_VolumesList.end(); ++i)
		RELEASE(*i);

    m_VolumesList.clear();
}

//-----------------------------------------------------------------------------

void ListTest::DestroyAllDefaultTexList()
{
    TEXTURELIST::iterator i;
    int t=0;

    for (i=m_DefaultTexList.begin(); i != m_DefaultTexList.end(); ++i)
		RELEASE(*i);

    m_DefaultTexList.clear();
}

//-----------------------------------------------------------------------------

void ListTest::DestroyAllResInfosFromList()
{
	RESINFOLIST::iterator ri;
    int t=0;
    WriteToLog("%d: Destroying all ResInfos from the list\n", m_uTestNumber);

    for (ri = m_ResInfosList.begin(); ri != m_ResInfosList.end(); ++ri)
	{
		if (NULL != (*ri))
		{
			if (NULL != (*ri)->pCheckColor)
				delete [] (*ri)->pCheckColor;
			if (NULL != (*ri)->pColors)
				delete [] (*ri)->pColors;
		}
		delete *ri;
	}

	m_ResInfosList.clear();
}