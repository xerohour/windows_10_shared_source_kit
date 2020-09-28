#include <d3d8.h>
#include <d3dx8.h>
#include <assert.h>
#include "MVertex.h"

CD3DWindowFramework		theApp;
MVertex					TestOne;


MVertex::MVertex()
{
    for (int i = 0; i < 8; i++)
        m_pTextureArray[i] = NULL;

    m_pVertices = NULL;
    m_dwVertices = 0;
	m_pIndices = NULL;
	m_dwIndices = 0;

    m_dwCurrentTestNumber = 0;
    m_dwNumFailed = 0;

	m_d3dpLastPrimitiveType = (D3DPRIMITIVETYPE)(-1);
	m_dwRPLastAPI = (RENDERPRIMITIVEAPI)(-1);

	SetTestType(TESTTYPE_CONF);
	if (KeySet("MAX"))
		m_Options.D3DOptions.bReference = false;
	m_Options.D3DOptions.fMinDXVersion = 8.f;
	m_szTestName = "MVertex";
	m_szCommandKey = "mvertex";
}

MVertex::~MVertex()
{
    if (m_pVertices)
    {
        delete [] m_pVertices;
        m_pVertices = NULL;
        m_dwVertices = 0;
    }

	if (m_pIndices)
	{
		delete [] m_pIndices;
		m_pIndices = NULL;
		m_dwIndices = 0;
	}
}


UINT MVertex::TestInitialize(void)
{
    m_dwRPAPICyclingFrequency = 1;
#ifdef FAST_TEST
	SetTestRange(1, m_dwRPAPIListSize);
#else
	SetTestRange(1, m_dwRPAPIListSize*5);
#endif

	// Check that the device supports required number of vertices
	if (!KeySet("MAX") && m_pSrcDevice->GetCaps()->dwMaxPrimitiveCount < (320*280))
	{
        WriteToLog("MVertex: Source device does not support required number of primitives\n");
		return D3DTESTINIT_SKIPALL;
	}
	if ( (m_pSrcDevice->GetCaps()->dwMaxVertexIndex < 0x10000) || (m_pRefDevice && m_pRefDevice->GetCaps()->dwMaxVertexIndex < 0x10000) )
	{
        WriteToLog("MVertex: Device does not support 32 bit indexes\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Get number of supported texture stages with textures
    DWORD   dwTexCount = m_pSrcDevice->GetCaps()->wMaxSimultaneousTextures;

    // Loading textures
	if (!KeySet("NOTEXTURES"))
	{
		static DWORD tex[8][8] = 
		{
			{0x00000000, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff},
			{0x00ffffff, 0x000000ff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff},
			{0x00ffffff, 0x00ffffff, 0x0000ff00, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff},
			{0x00ffffff, 0x00ffffff, 0x00ffffff, 0x0000ffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff},
			{0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ff0000, 0x00ffffff, 0x00ffffff, 0x00ffffff},
			{0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ff00ff, 0x00ffffff, 0x00ffffff},
			{0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffff00, 0x00ffffff},
			{0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00ffffff, 0x00f0f0f0}
		};

		for (int i = 0; i < dwTexCount; i++) {
			char fbuf[10];
			sprintf(fbuf, "tex%1d.bmp", i);
			if (!CreateTexture(256, 256, 1, 0, m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], FMWK_DEFAULT, &m_pTextureArray[i]) ||
				!m_pTextureArray[i]->LoadStripes(0, 256, 256, 8, tex[i], true))
			{
				WriteToLog("LineTex: failed to load %s image\n", fbuf);
				return D3DTESTINIT_ABORT;
			}

			SetTexture(i, m_pTextureArray[i]);
			SetTextureStageState(i, D3DTSS_TEXCOORDINDEX, (DWORD)0);
			SetTextureStageState(i, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE);
			if (i) 
			{
				SetTextureStageState(i, D3DTSS_COLORARG2, (DWORD)D3DTA_CURRENT);
				SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_MODULATE);
			}
			else
			{
				SetTextureStageState(i, D3DTSS_COLORARG2, (DWORD)D3DTA_DIFFUSE);
				SetTextureStageState(i, D3DTSS_COLOROP, (DWORD)D3DTOP_SELECTARG1);
			}

		}
	}
	return D3DTESTINIT_RUN;
}


bool MVertex::ExecuteTest(UINT iTest)
{
    DWORD	dwPrimType;

	m_dwCurrentTestNumber = (DWORD)iTest;

	dwPrimType = ((m_dwCurrentTestNumber-1)/(m_dwRPAPIListSize*m_dwRPAPICyclingFrequency)) % 5;
	switch(dwPrimType)
	{
	case 0:
		m_d3dptPrimitiveType = D3DPT_POINTLIST;
		break;
	case 1:		
		m_d3dptPrimitiveType = D3DPT_LINELIST;
		break;
	case 2:
		m_d3dptPrimitiveType = D3DPT_LINESTRIP;
		break;
	case 3:
		m_d3dptPrimitiveType = D3DPT_TRIANGLELIST;
		break;
	case 4:
		m_d3dptPrimitiveType = D3DPT_TRIANGLESTRIP;
		break;
	}

	if (m_pSrcDevice->GetCaps()->dwMaxVertexIndex < m_dwIndices ||
		m_pSrcDevice->GetCaps()->dwMaxPrimitiveCount < m_dwIndices)
	{
		DWORD	dwSkip = m_dwRPAPIListSize*m_dwRPAPICyclingFrequency;
		SkipTests(dwSkip - (m_dwCurrentTestNumber-1)%dwSkip);
		return false;
	}


	if (m_d3dpLastPrimitiveType != m_d3dptPrimitiveType ||
		m_dwRPLastAPI != GetAPI())
	{
		if (!GeneratePrimitive())
		{
			DWORD	dwSkip = m_dwRPAPIListSize*m_dwRPAPICyclingFrequency;
			SkipTests(dwSkip - (m_dwCurrentTestNumber-1)%dwSkip);
			WriteToLog("MVertex: Not enough memory to initialize primitive\n");
			return false;
		}
		m_d3dpLastPrimitiveType = m_d3dptPrimitiveType;
		m_dwRPLastAPI = GetAPI();
	}

    BeginTestCase("MVertex", iTest);
    return true;
}

void MVertex::SceneRefresh(void) 
{
    if (BeginScene())
    {
		RPOPTIONS	Opt;

		Opt.Pool = POOL_DEFAULT;
		Opt.Usage = 0;
		Opt.uIBBitCount = 32;
		Opt.Flags = 0;

        RenderPrimitive(m_d3dptPrimitiveType, FVF_VERTEX, m_pVertices, m_dwVertices, 
						(LPWORD)m_pIndices, m_dwIndices, &Opt);
        EndScene();
    }
}

bool MVertex::ProcessFrame(void)
{
	if (KeySet("MAX"))
	{
		Pass();
	}
	else
	{
		return CD3DTest::ProcessFrame();
	}

	return true;
}

bool MVertex::TestTerminate(void)
{
    for (int i = 0; i < 8; i++)
        RELEASE(m_pTextureArray[i]);

    if (m_pVertices)
    {
        delete [] m_pVertices;
        m_pVertices = NULL;
        m_dwVertices = 0;
    }
	if (m_pIndices)
	{
		delete [] m_pIndices;
		m_pIndices = NULL;
		m_dwIndices = 0;
	}
	return true;
}

bool MVertex::GeneratePrimitive(void) {
	int			iXSegments = 320;
	int			iYSegments = 280;
    float		fStart = 1.f;
	DWORD		i, j, k;
	float		fX = -fStart;
	float		fY = -fStart;
	float		fXInc = (2.f * fStart) / float(iXSegments);
	float		fYInc = (2.f * fStart) / float(iYSegments);
	bool		bMax = KeySet("MAX");


	// Release buffers
	if (m_pVertices)
	{
		delete [] m_pVertices;
		m_pVertices = NULL;
		m_dwVertices = 0;
	}
	if (m_pIndices)
	{
		delete [] m_pIndices;
		m_pIndices = NULL;
		m_dwIndices = 0;
	}

	// Recalculate variables fro max case
	if (bMax)
	{
		iYSegments = (int)sqrt((float)m_pSrcDevice->GetCaps()->dwMaxPrimitiveCount) / 2;
		iXSegments = m_pSrcDevice->GetCaps()->dwMaxPrimitiveCount / iYSegments;
		fXInc = (2.f * fStart) / float(iXSegments);
		fYInc = (2.f * fStart) / float(iYSegments);
	}

	// Generate vertices for indexed or point primitive
	if (m_d3dptPrimitiveType == D3DPT_POINTLIST || GetAPI() == RP_DIP ||
		GetAPI() == RP_DIPS || GetAPI() == RP_DIPVB)
	{
		m_dwVertices = iXSegments * iYSegments;
		m_pVertices = new VERTEX [ m_dwVertices ];
		if (m_pVertices == NULL)
			return false;

		for (i = 0; i < iYSegments; i++) 
		{
    		for (j = 0; j < iXSegments; j++) 
			{
				k = i*iXSegments + j;
				m_pVertices[k].x = fX;
				m_pVertices[k].y = fY; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = (m_pVertices[k].x + fStart) / (2.f * fStart);
				m_pVertices[k].tv = (m_pVertices[k].y + fStart) / (2.f * fStart);

    			fX += fXInc;
			}
			fY += fYInc;
			fX = -fStart;
		}
	}
	// Non-indexed line list
	else if (m_d3dptPrimitiveType == D3DPT_LINELIST)
	{
		m_dwVertices = iXSegments * (iYSegments-1) * 2;
		m_pVertices = new VERTEX [ m_dwVertices ];
		if (m_pVertices == NULL)
			return false;

		for (i = 0, k = 0; i < iYSegments-1; i+=1) 
		{
    		for (j = 0; j < iXSegments; j++) 
			{
				// First vertex (i*iXSegments + j)
				m_pVertices[k].x = fXInc*j-fStart;
				m_pVertices[k].y = fYInc*i-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*j / (2.f * fStart);
				m_pVertices[k].tv = fYInc*i / (2.f * fStart);
				k++;

				// Second vertex ((i+1)*iXSegments + j)
				m_pVertices[k].x = fXInc*j-fStart;
				m_pVertices[k].y = fYInc*(i+1)-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*j / (2.f * fStart);
				m_pVertices[k].tv = fYInc*(i+1) / (2.f * fStart);
				k++;
			}
		}
	}
	// Non-indexed line strip
	else if (m_d3dptPrimitiveType == D3DPT_LINESTRIP)
	{
		m_dwVertices = iXSegments * (iYSegments-1) * 2;
		m_pVertices = new VERTEX [ m_dwVertices ];
		if (m_pVertices == NULL)
			return false;

		for (i = 0, k = 0; i < iYSegments-1; i+=1) 
		{
    		for (j = 0; j < iXSegments; j++) 
			{
				// First vertex (i*iXSegments + j)
				m_pVertices[k].x = fXInc*j-fStart;
				m_pVertices[k].y = fYInc*i-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*j / (2.f * fStart);
				m_pVertices[k].tv = fYInc*i / (2.f * fStart);
				k++;

				// Second vertex ((i+1)*iXSegments + j)
				m_pVertices[k].x = fXInc*j-fStart;
				m_pVertices[k].y = fYInc*(i+1)-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*j / (2.f * fStart);
				m_pVertices[k].tv = fYInc*(i+1) / (2.f * fStart);
				k++;
			}
		}
	}
	// Non-indexed triangle list
	else if (m_d3dptPrimitiveType == D3DPT_TRIANGLELIST)
	{
		m_dwVertices = (iXSegments-1) * (iYSegments-2) * 3 * 2;
		m_pVertices = new VERTEX [ m_dwVertices ];
		if (m_pVertices == NULL)
			return false;

		for (i = 0, k = 0; i < iYSegments-2; i++) 
		{
    		for (j = 0; j < iXSegments-1; j++) 
			{
				// First vertex (i*iXSegments + j)
				m_pVertices[k].x = fXInc*j-fStart;
				m_pVertices[k].y = fYInc*i-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*j / (2.f * fStart);
				m_pVertices[k].tv = fYInc*i / (2.f * fStart);
				k++;

				// Second vertex ((i+2)*iXSegments + j)
				m_pVertices[k].x = fXInc*j-fStart;
				m_pVertices[k].y = fYInc*(i+2)-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*j / (2.f * fStart);
				m_pVertices[k].tv = fYInc*(i+2) / (2.f * fStart);
				k++;

				// Third vertex ((i*iXSegments + (j+1))
				m_pVertices[k].x = fXInc*(j+1)-fStart;
				m_pVertices[k].y = fYInc*i-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*(j+1) / (2.f * fStart);
				m_pVertices[k].tv = fYInc*i / (2.f * fStart);
				k++;
				
				// Second triangle
				// First vertex (i*iXSegments + (j+1))
				m_pVertices[k].x = fXInc*(j+1)-fStart;
				m_pVertices[k].y = fYInc*i-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*(j+1) / (2.f * fStart);
				m_pVertices[k].tv = fYInc*i / (2.f * fStart);
				k++;

				// Second vertex ((i+2)*iXSegments + j)
				m_pVertices[k].x = fXInc*j-fStart;
				m_pVertices[k].y = fYInc*(i+2)-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*j / (2.f * fStart);
				m_pVertices[k].tv = fYInc*(i+2) / (2.f * fStart);
				k++;

				// Third vertex (((i+2)*iXSegments + (j+1))
				m_pVertices[k].x = fXInc*(j+1)-fStart;
				m_pVertices[k].y = fYInc*(i+2)-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*(j+1) / (2.f * fStart);
				m_pVertices[k].tv = fYInc*(i+2) / (2.f * fStart);
				k++;
			}
		}
	}
	// Non-indexed triangle strip
	else if (m_d3dptPrimitiveType == D3DPT_TRIANGLESTRIP)
	{
		m_dwVertices = (iXSegments-1) * (iYSegments-1) * 2;
		m_pVertices = new VERTEX [ m_dwVertices ];
		if (m_pVertices == NULL)
			return false;

		for (i = 0, k = 0; i < iYSegments-1; i+=1) 
		{
    		for (j = 0; j < iXSegments-1; j++) 
			{
				// First vertex (i*iXSegments + j)
				m_pVertices[k].x = fXInc*j-fStart;
				m_pVertices[k].y = fYInc*i-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*j / (2.f * fStart);
				m_pVertices[k].tv = fYInc*i / (2.f * fStart);
				k++;

				// Second vertex ((i+1)*iXSegments + j)
				m_pVertices[k].x = fXInc*j-fStart;
				m_pVertices[k].y = fYInc*(i+1)-fStart; 
				m_pVertices[k].z = 0.f;
				m_pVertices[k].nx = 0.f;
				m_pVertices[k].ny = 0.707f;
				m_pVertices[k].nz = 0.707f;
				m_pVertices[k].tu = fXInc*j / (2.f * fStart);
				m_pVertices[k].tv = fYInc*(i+1) / (2.f * fStart);
				k++;
			}
		}
	}


	if (GetAPI() == RP_DIP || GetAPI() == RP_DIPS || GetAPI() == RP_DIPVB)
	{
		switch(m_d3dptPrimitiveType)
		{
		// Line list indices
		case D3DPT_LINELIST:
			m_dwIndices = (iYSegments-1)*iXSegments*2;
			m_pIndices = new DWORD [m_dwIndices];
			if (m_pIndices == NULL)
				return false;
			for (i = 0, k = 0; i < iYSegments-1; i+=1) 
			{
    			for (j = 0; j < iXSegments; j++) 
				{
					m_pIndices[k++] = i*iXSegments + j;
					m_pIndices[k++] = (i+1)*iXSegments + j;
				}
			}
			break;

		// Line strip indices
		case D3DPT_LINESTRIP:
			m_dwIndices = (iYSegments-1)*iXSegments*2;
			m_pIndices = new DWORD [m_dwIndices];
			if (m_pIndices == NULL)
				return false;
			for (i = 0, k = 0; i < iYSegments-1; i++) 
			{
    			for (j = 0; j < iXSegments; j++) 
				{
					m_pIndices[k++] = i*iXSegments + j;
					m_pIndices[k++] = (i+1)*iXSegments + j;
				}
			}
			break;

		// Triangle list indices
		case D3DPT_TRIANGLELIST:
			m_dwIndices = (iYSegments-2)/2*(iXSegments-1)*3*2;
			m_pIndices = new DWORD [m_dwIndices];
			if (m_pIndices == NULL)
				return false;
			for (i = 0, k = 0; i < iYSegments-2; i+=2) 
			{
    			for (j = 0; j < iXSegments-1; j++) 
				{
					m_pIndices[k++] = i*iXSegments + j;
					assert(m_pIndices[k-1] < m_dwVertices);
					m_pIndices[k++] = (i+3)*iXSegments + j;
					assert(m_pIndices[k-1] < m_dwVertices);
					m_pIndices[k++] = i*iXSegments + (j+1);
					assert(m_pIndices[k-1] < m_dwVertices);
					
					m_pIndices[k++] = i*iXSegments + (j+1);
					assert(m_pIndices[k-1] < m_dwVertices);
					m_pIndices[k++] = (i+3)*iXSegments + j;
					assert(m_pIndices[k-1] < m_dwVertices);
					m_pIndices[k++] = (i+3)*iXSegments + (j+1);
					assert(m_pIndices[k-1] < m_dwVertices);
				}
			}
			break;

		// Triangle strip indices
		case D3DPT_TRIANGLESTRIP:
			m_dwIndices = (iYSegments-1)*iXSegments*2;
			m_pIndices = new DWORD [m_dwIndices];
			if (m_pIndices == NULL)
				return false;
			for (i = 0, k = 0; i < iYSegments-1; i++) 
			{
    			for (j = 0; j < iXSegments; j++) 
				{
					m_pIndices[k++] = i*iXSegments + j;
					m_pIndices[k++] = (i+1)*iXSegments + j;
				}
			}
			break;
		}
	}
	return true;
}


