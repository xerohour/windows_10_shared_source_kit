#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "ResMan.h"

//-----------------------------------------------------------------------------

CBasicTest::CBasicTest()
{
	m_szTestName = "Basic Test";
	m_szCommandKey = "Basic";

	m_pTLVertexBuffer   = NULL;
	m_pLVertexBuffer    = NULL;
	m_pVertexBuffer     = NULL;
	m_pCubeVertexBuffer = NULL;
	m_pVolVertexBuffer  = NULL;
	
	m_p16bitIndexBuffer1 = NULL;
	m_p16bitIndexBuffer2 = NULL;
	m_p32bitIndexBuffer1 = NULL;
	m_p32bitIndexBuffer2 = NULL;
	
	m_p16bitTexIndexBuffer1  = NULL;
	m_p16bitTexIndexBuffer2  = NULL;
	m_p16bitCubeIndexBuffer1 = NULL;
	m_p16bitCubeIndexBuffer2 = NULL;
	m_p16bitVolIndexBuffer1  = NULL;
	m_p16bitVolIndexBuffer2  = NULL;
	m_p32bitTexIndexBuffer1  = NULL;
	m_p32bitTexIndexBuffer2  = NULL;
	m_p32bitCubeIndexBuffer1 = NULL;
	m_p32bitCubeIndexBuffer2 = NULL;
	m_p32bitVolIndexBuffer1  = NULL;
	m_p32bitVolIndexBuffer2  = NULL;
		
	m_pTexture     = NULL;
	m_pCubeTexture = NULL;
	m_pVolTexture  = NULL;
	
	m_pBackBuffer = NULL;
}

//-----------------------------------------------------------------------------

UINT CBasicTest::TestInitialize(void)
{
	HRESULT hr; 
	srand(10000);

	m_NonPreLoadSum = 0;
	m_PreLoadSum = 0;
	llFreq = 0;

	draw_Textures    = false;
	draw_CubeMaps    = false;
	draw_VolTextures = false;

	SetTestRange(1, 18+12*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) ); 

	UINT result = ResMan::TestInitialize();
	if ( result != D3DTESTINIT_RUN )
		return result;

	//we need it to check if PreLoad() function works correctly for dx7 and up textures	
	if ( m_dwVersion >= 0x0700 )
	{
		if ( !QueryPerformanceFrequency((LARGE_INTEGER*)&llFreq) )
		{
			WriteToLog("QueryPerformanceFrequency failed!\n");
			SkipTestRange( 10+6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats), 18+12*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) );
		}
		else
		{
			if(FAILED(hr = m_pSrcDevice->GetBackBuffer(0, BACKBUFFER_TYPE_MONO, &m_pBackBuffer)))
			{
				WriteToLog("TestInitialize: GetBackBuffer() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				return D3DTESTINIT_ABORT;
			}
		}
	}
	else
	{
		WriteToLog("PreLoad() is not supported on dx6 textures\n");
		SkipTestRange( 10+6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats), 18+12*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) );
	}

	CheckBasicTestCaps();

	return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------

bool CBasicTest::ExecuteTest(UINT iTest)
{
	HRESULT hr;
	DWORD texIndex = 0;
	bool isPreloaded = false;
	TCHAR     szBuffer[30];

	memset(m_bCheckColor, true, sizeof(m_bCheckColor)/sizeof(bool));

	if (m_uTestNumber == 1 || m_uTestNumber == 10+6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats))
	{
		msgString[0] = '\0';
		if ( m_uTestNumber == 10 + 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{

			m_pTLVertexBuffer->PreLoad();
			isPreloaded = true;
			sprintf(msgString, "%sPreloaded TLVB", msgString);
		}
		else
			sprintf(msgString, "%sTLVB", msgString);
	}
	if (m_uTestNumber == 2 || m_uTestNumber == 11+6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats))
	{
		msgString[0] = '\0';
		if ( m_uTestNumber == 11 + 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_pLVertexBuffer->PreLoad();
			isPreloaded = true;
			sprintf(msgString, "%sPreloaded UtLVB", msgString);
		}
		else
			sprintf(msgString, "%sUtLVB", msgString);
	}
	if (m_uTestNumber == 3 || m_uTestNumber == 12+6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats))
	{
		msgString[0] = '\0';
		if ( m_uTestNumber == 12 + 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_pVertexBuffer->PreLoad();
			isPreloaded = true;
			sprintf(msgString, "%sPreloaded UtUlVB", msgString);
		}
		else
			sprintf(msgString, "%s UtUlVB", msgString);
	}
	if ( m_uTestNumber ==  4 + 2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) ||
		 m_uTestNumber == 13 + 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		msgString[0] = '\0';
		if (m_uTestNumber == 13 + 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_pTLVertexBuffer->PreLoad();
			m_p16bitIndexBuffer1->PreLoad();
			m_p16bitIndexBuffer2->PreLoad();
			isPreloaded = true;
			sprintf(msgString, "%sPreloaded TLVB & 16IB", msgString);
		}
		else
			sprintf(msgString, "%s TLVB & 16IB", msgString);
	}
	if ( m_uTestNumber ==  5 + 2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) ||
		 m_uTestNumber == 14 + 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		msgString[0] = '\0';
		if (m_uTestNumber == 14 + 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_pLVertexBuffer->PreLoad();	
			isPreloaded = true;
			sprintf(msgString, "%sPreloaded UtLVB & 16IB", msgString);
		}
		else
			sprintf(msgString, "%sUtLVB & 16IB", msgString);
	}
	if ( m_uTestNumber ==  6 + 2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) ||
		 m_uTestNumber == 15 + 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		msgString[0] = '\0';
		if (m_uTestNumber == 15 + 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_pVertexBuffer->PreLoad();	
			isPreloaded = true;
			sprintf(msgString, "%sPreloaded UtUlVB & 16IB", msgString);
		}
		else
			sprintf(msgString, "%sUtUlVB & 16IB", msgString);
	}
	if ( m_uTestNumber ==  7 + 4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) ||
		 m_uTestNumber == 16 +10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		msgString[0] = '\0';
		if (m_uTestNumber == 16 + 10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_pTLVertexBuffer->PreLoad();
			m_p32bitIndexBuffer1->PreLoad();
			m_p32bitIndexBuffer2->PreLoad();
			isPreloaded = true;
			sprintf(msgString, "%sPreloaded TLVB & 32IB", msgString);
		}
		else
			sprintf(msgString, "%sTLVB & 32IB", msgString);
	}
	if ( m_uTestNumber ==  8 + 4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) ||
		 m_uTestNumber == 17 +10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		msgString[0] = '\0';
		if (m_uTestNumber == 17 + 10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_pLVertexBuffer->PreLoad();
			isPreloaded = true;
			sprintf(msgString, "%sPreloaded UtLVB & 32IB", msgString);
		}
		else
			sprintf(msgString, "%sUtLVB & 32IB", msgString);

	}
	if ( m_uTestNumber ==  9 + 4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) ||
		 m_uTestNumber == 18 +10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		msgString[0] = '\0';
		if (m_uTestNumber == 18 + 10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_pVertexBuffer->PreLoad();
			isPreloaded = true;
			sprintf(msgString, "%sPreloaded UtUlVB & 32IB", msgString);
		}
		else
			sprintf(msgString, "%sUtUlVB & 32IB", msgString);
	}
	if((m_uTestNumber >=  4 && m_uTestNumber <= 3+ 2*m_uCommonTextureFormats) || 
	   (m_uTestNumber >=  7+ 2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) && m_uTestNumber <=  6+ 2*m_uCommonTextureFormats+ 2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)) ||
	   (m_uTestNumber >= 10+ 4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) && m_uTestNumber <=  9+ 2*m_uCommonTextureFormats+ 4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)) ||
	   (m_uTestNumber >= 13+ 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) && m_uTestNumber <= 12+ 2*m_uCommonTextureFormats+ 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)) ||
       (m_uTestNumber >= 16+ 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) && m_uTestNumber <= 15+ 2*m_uCommonTextureFormats+ 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)) ||
       (m_uTestNumber >= 19+10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) && m_uTestNumber <= 18+ 2*m_uCommonTextureFormats+10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats))  )
	{
		draw_Textures = true;
		msgString[0] = '\0';
		if ( m_uTestNumber > 10+6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			sprintf(msgString, "%sPreloaded", msgString);
			isPreloaded = true;
		}
		//determine texture format
		texIndex = m_uTestNumber % (9 + 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats));
		texIndex = (m_uTestNumber-1) % (6*m_uCommonTextureFormats+6*m_uCommonCubeFormats+6*m_uCommonVolumeFormats+9);
		if ( texIndex < 3+2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
			sprintf(msgString, "%s VB & T ", msgString);
		else if ( texIndex < 6+4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
			sprintf(msgString, "%s 16IB & T ", msgString);
		else
			sprintf(msgString, "%s 32IB & T ", msgString);
		texIndex = (texIndex-3)%(3+2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) );
		m_pCurrentMode->nTextureFormat = texIndex % m_uCommonTextureFormats;
		FmtToString(FORMAT_TEXTURE, &m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat], szBuffer);
		sprintf(msgString, "%s %s", msgString, szBuffer);
	}
	if ((m_uTestNumber >=  4+ 2*m_uCommonTextureFormats && m_uTestNumber <= 3+ 2*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats) ||
	    (m_uTestNumber >=  7+ 4*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats && m_uTestNumber <=  6+ 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats) ||
	    (m_uTestNumber >= 10+ 6*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats && m_uTestNumber <=  9+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats) ||
	    (m_uTestNumber >= 13+ 8*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats && m_uTestNumber <= 12+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats) ||
        (m_uTestNumber >= 16+10*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats && m_uTestNumber <= 15+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats) ||
        (m_uTestNumber >= 19+12*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats && m_uTestNumber <= 18+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+10*m_uCommonVolumeFormats)  )
	{
		draw_CubeMaps = true;
		msgString[0] = '\0';
		if ( m_uTestNumber > 10+6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			sprintf(msgString, "%sPreloaded", msgString);
			isPreloaded = true;
		}
		//determine texture format
		texIndex = (m_uTestNumber-1) % ( 9+6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) );
		if (texIndex < 3+2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
			sprintf(msgString, "%s VB & CM ", msgString);
		else if (texIndex < 6+4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
			sprintf(msgString, "%s 16IB & CM ", msgString);
		else
			sprintf(msgString, "%s 32IB & CM ", msgString);
		texIndex = ((texIndex-3)%( 3+2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) ) - 2*m_uCommonTextureFormats);
		m_pCurrentMode->nCubeFormat = texIndex % m_uCommonCubeFormats;
		FmtToString(FORMAT_TEXTURE, &m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat], szBuffer);
		sprintf(msgString, "%s %s", msgString, szBuffer);
	}
	if((m_uTestNumber >=  4+ 2*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats                           && m_uTestNumber <=  3+ 2*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats) ||
	   (m_uTestNumber >=  7+ 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats && m_uTestNumber <=  6+ 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats) ||
	   (m_uTestNumber >= 10+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats && m_uTestNumber <=  9+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats) ||
	   (m_uTestNumber >= 13+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats && m_uTestNumber <= 12+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats) ||
       (m_uTestNumber >= 16+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats && m_uTestNumber <= 15+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats) ||
       (m_uTestNumber >= 19+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+10*m_uCommonVolumeFormats && m_uTestNumber <= 18+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+12*m_uCommonVolumeFormats))
	{
		draw_VolTextures = true;
		msgString[0] = '\0';
		if ( m_uTestNumber > 10+6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			sprintf(msgString, "%sPreloaded", msgString);
			isPreloaded = true;
		}
		texIndex = (m_uTestNumber-1) % ( 9 + 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) );
		if ( texIndex < 3 + 2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
			sprintf(msgString, "%s VB & V", msgString);
		else if ( texIndex < 6 + 4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
			sprintf(msgString, "%s 16IB & V ", msgString);
		else
			sprintf(msgString, "%s 32IB & V ", msgString);
		texIndex = ( (texIndex-3)%(3 + 2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )-2*m_uCommonTextureFormats-2*m_uCommonCubeFormats);
		m_pCurrentMode->nVolumeFormat = texIndex % m_uCommonVolumeFormats;
		FmtToString(FORMAT_TEXTURE, &m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat], szBuffer);
		sprintf(msgString, "%s %s", msgString, szBuffer);
	}

	BeginTestCase(msgString, iTest);

	if (m_uPrevMaxMipLevel != 0)
	{
		m_uPrevMaxMipLevel = 0;
		SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, 0);
	}

	if (draw_Textures)
	{
		if (!GenerateTexture(texIndex))
		{
			draw_Textures = false;
			return false;
		}
		if ( m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_A8 )
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE );
		}
		if ( m_uTestNumber >= 13+ 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) && m_uTestNumber <= 12+ 2*m_uCommonTextureFormats+ 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_pTexture->PreLoad();
			if ( m_dwVersion >= 0x0800)
				m_pVertexBuffer->PreLoad();
		}
		if ( m_uTestNumber >= 16+ 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) && m_uTestNumber <= 15+ 2*m_uCommonTextureFormats+ 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_pTexture->PreLoad();
			if ( m_dwVersion >= 0x0800)
			{
				m_pVertexBuffer->PreLoad();
				m_p16bitTexIndexBuffer1->PreLoad();
				if ( m_uTestNumber >= 16+ m_uCommonTextureFormats+8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) && m_uTestNumber <= 15+ 2*m_uCommonTextureFormats+ 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
					m_p16bitTexIndexBuffer2->PreLoad();
			}
		}
        if ( m_uTestNumber >= 19+10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) && m_uTestNumber <= 18+ 2*m_uCommonTextureFormats+10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_pTexture->PreLoad();
			if ( m_dwVersion >= 0x0800)
			{
				m_pVertexBuffer->PreLoad();
				m_p32bitTexIndexBuffer1->PreLoad();
				if ( m_uTestNumber >= 19+m_uCommonTextureFormats+10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) && m_uTestNumber <= 18+ 2*m_uCommonTextureFormats+10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
					m_p32bitTexIndexBuffer2->PreLoad();
			}
		}
	}
	if( draw_CubeMaps )
	{
		if (!GenerateCubeTexture(texIndex))
		{
			draw_CubeMaps = false;
			return false;
		}
		if (m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].d3dfFormat == FMT_A8)
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE );
		}
		if ( m_uTestNumber >= 13+ 8*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats && m_uTestNumber <= 12+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats )
		{
			m_pCubeTexture->PreLoad();
			if ( m_dwVersion >= 0x0800)
				m_pCubeVertexBuffer->PreLoad();
		}
		if ( m_uTestNumber >= 16+10*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats && m_uTestNumber <= 15+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats )
		{
			m_pCubeTexture->PreLoad();
			if ( m_dwVersion >= 0x0800)
			{
				m_pCubeVertexBuffer->PreLoad();
				m_p16bitCubeIndexBuffer1->PreLoad();
				m_p16bitCubeIndexBuffer2->PreLoad();
			}
		}
		if ( m_uTestNumber >= 19+12*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats && m_uTestNumber <= 18+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+10*m_uCommonVolumeFormats )
		{
			m_pCubeTexture->PreLoad();
			if ( m_dwVersion >= 0x0800)
			{
				m_pCubeVertexBuffer->PreLoad();
				m_p32bitCubeIndexBuffer1->PreLoad();
				m_p32bitCubeIndexBuffer2->PreLoad();
			}
		}
	}
	if( draw_VolTextures )
	{
		if (!GenerateVolumeTexture(texIndex))
		{
			draw_VolTextures = false;
			return false;
		}
		if (m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].d3dfFormat == FMT_A8)
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)TRUE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_DIFFUSE );
		}
		if ( m_uTestNumber >= 13+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats && m_uTestNumber <= 12+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats )
		{
			m_pVolTexture->PreLoad();
			if ( m_dwVersion >= 0x0800)
				m_pVolVertexBuffer->PreLoad();
		}
        if ( m_uTestNumber >= 16+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats && m_uTestNumber <= 15+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats )
		{
			m_pVolTexture->PreLoad();
			if ( m_dwVersion >= 0x0800)
			{
				m_pVolVertexBuffer->PreLoad();
				m_p16bitVolIndexBuffer1->PreLoad();
				if ( m_uTestNumber >= 16+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+ 9*m_uCommonVolumeFormats )
					m_p16bitVolIndexBuffer2->PreLoad();
			}
		}
        if ( m_uTestNumber >= 19+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+10*m_uCommonVolumeFormats && m_uTestNumber <= 18+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+12*m_uCommonVolumeFormats )
		{
			m_pVolTexture->PreLoad();
			if ( m_dwVersion >= 0x0800)
			{
				m_pVolVertexBuffer->PreLoad();
				m_p32bitVolIndexBuffer1->PreLoad();
				if ( m_uTestNumber >= 19+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+11*m_uCommonVolumeFormats )
					m_p32bitVolIndexBuffer2->PreLoad();
			}
		}
	}

	hr = m_pSrcDevice->BeginScene();
	if (FAILED(hr))
	{
		WriteToLog("BeginScene() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	
	//drawing with tl vertex buffers
	if (m_uTestNumber == 1 || m_uTestNumber == 10 + 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats))
	{
		m_pSrcDevice->SetTexture(0, NULL);
		if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, m_pTLVertexBuffer, 
								0, 8, NULL, 0, 0, 0, POOL_MANAGED, false))
		{
			hr = m_pSrcDevice->GetLastError();
			
			WriteToLog("DrawPrimitiveVertexBuffer() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			else
				Fail();
		}
	}
	//drawing with untransformed lit vertex buffers
	if (m_uTestNumber == 2 || m_uTestNumber == 11 + 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats))
	{
		D3DXMATRIX	Matrix;
		D3DXMatrixIdentity( &Matrix );
		m_pSrcDevice->SetTransform(D3DTS_WORLD,(D3DMATRIX*)&Matrix);
		if (m_dwVersion >= 0x0700)
		{
			hr = m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE);
			if (FAILED(hr))
			{
				WriteToLog("SetRenderState(LIGHTING, FALSE) failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
		}
		if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, m_pLVertexBuffer, 
									0, 8, NULL, 0, 0, 0, POOL_MANAGED, false))
		{
			hr = m_pSrcDevice->GetLastError();
			
			WriteToLog("DrawPrimitiveVertexBuffer() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			else
				Fail();
		}
		if ( m_dwVersion >= 0x0700 )
		{
			hr = m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
			if (FAILED(hr))
			{
				WriteToLog("SetRenderState(LIGHTING, TRUE) failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
		}
	}
	//drawing with untransformed unlit vertex buffers
	if (m_uTestNumber == 3 || m_uTestNumber == 12 + 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats))
	{
		D3DXMATRIX	Matrix;
		D3DXMatrixIdentity( &Matrix );
		m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
		if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, m_pVertexBuffer, 
								0, 4, NULL, 0, 0, 0, POOL_MANAGED, false))
		{
			hr = m_pSrcDevice->GetLastError();
			
			WriteToLog("DrawPrimitiveVertexBuffer() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			else
				Fail();
		}
	}
	//drawing with transformed lit vertex and 16 bit index buffers
	if ( m_uTestNumber ==  4+2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) || 
		 m_uTestNumber == 13+8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		m_pSrcDevice->SetTexture(0, NULL);
		hr = m_pSrcDevice->SetVertexShader(D3DFVF_TLVERTEX);
		if (FAILED(hr))
		{
			WriteToLog("SetVertexShader() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->SetStreamSource( 0, m_pTLVertexBuffer, GetVertexSize(D3DFVF_TLVERTEX) );
		if (FAILED(hr))
		{
			WriteToLog("SetStreamSource() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->SetIndices( m_p16bitIndexBuffer1, 0 );
		if (FAILED(hr))
		{
			WriteToLog("SetIndices() failed on m_p16bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 5, 0, 3);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed on m_p16bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
		hr = m_pSrcDevice->SetIndices( m_p16bitIndexBuffer2, 0 );
		if (FAILED(hr))
		{
			WriteToLog("SetIndices() failed on m_p16bitIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 8, 0, 3);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed m_p16bitIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
	}
	//drawing with untransformed lit vertices and 16 bit index buffers
	if ( m_uTestNumber ==  5+2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) || 
		 m_uTestNumber == 14+8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		D3DXMATRIX	Matrix;
		D3DXMatrixIdentity( &Matrix );
		m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
		m_pSrcDevice->SetVertexShader(FVF_LVERTEX);
		if (m_dwVersion >= 0x0700)
			m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE);
		m_pSrcDevice->SetStreamSource( 0, m_pLVertexBuffer, GetVertexSize(FVF_LVERTEX) );
		hr = m_pSrcDevice->SetIndices( m_p16bitIndexBuffer1, 0 );
		if (FAILED(hr))
		{
			WriteToLog("SetIndices() failed on m_p16bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 5, 0, 3);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed on m_p16bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
		hr = m_pSrcDevice->SetIndices( m_p16bitIndexBuffer2, 0 );
		if (FAILED(hr))
		{
			WriteToLog("SetIndices() failed on m_p16bitIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 8, 0, 3);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed on m_p16bitIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
		if (m_dwVersion >= 0x0700)
			m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
	}
	//drawing with untransformed unlit vertices and 16 bit index buffers
	if ( m_uTestNumber ==  6+2*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) || 
		 m_uTestNumber == 15+8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		D3DXMATRIX	Matrix;
		D3DXMatrixIdentity( &Matrix );
		m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
		m_pSrcDevice->SetVertexShader(FVF_VERTEX);
		m_pSrcDevice->SetStreamSource( 0, m_pVertexBuffer, GetVertexSize(FVF_VERTEX) );
		hr = m_pSrcDevice->SetIndices( m_p16bitIndexBuffer1, 0 );
			if (FAILED(hr))
		{
			WriteToLog("SetIndices() failed on m_p16bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 3, 0, 1);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed on m_p16bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 4, 1, 1);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed on m_p16bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
	}
	//drawing with transformed lit vertices and 32 bit index buffers
	if ( m_uTestNumber ==  7+ 4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) || 
		 m_uTestNumber == 16+10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		m_pSrcDevice->SetTexture(0, NULL);
		m_pSrcDevice->SetVertexShader(D3DFVF_TLVERTEX);
		m_pSrcDevice->SetStreamSource( 0, m_pTLVertexBuffer, GetVertexSize(D3DFVF_TLVERTEX) );
		hr = m_pSrcDevice->SetIndices( m_p32bitIndexBuffer1, 0 );
		if (FAILED(hr))
			{
			WriteToLog("SetIndices() failed on m_p32bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 5, 0, 3);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed on m_p16bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
		hr = m_pSrcDevice->SetIndices( m_p32bitIndexBuffer2, 0 );
		if (FAILED(hr))
		{
			WriteToLog("SetIndices() failed on m_p32bitIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 8, 0, 3);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed on m_p16bitIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
	}
	//draw untransformed lit vertex buffers and 32 bit index buffers
	if ( m_uTestNumber ==  8+ 4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) || 
		 m_uTestNumber == 17+10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		D3DXMATRIX	Matrix;
		D3DXMatrixIdentity( &Matrix );
		m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
		if (m_dwVersion >= 0x0700)
		m_pSrcDevice->SetRenderState(D3DRS_LIGHTING, (DWORD)FALSE);
		m_pSrcDevice->SetVertexShader(FVF_LVERTEX);
		m_pSrcDevice->SetStreamSource( 0, m_pLVertexBuffer, GetVertexSize(FVF_LVERTEX) );
		hr = m_pSrcDevice->SetIndices( m_p32bitIndexBuffer1, 0 );
		if (FAILED(hr))
		{
			WriteToLog("SetIndices() failed on m_p32bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 5, 0, 3);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed on m_p32bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
		hr = m_pSrcDevice->SetIndices( m_p32bitIndexBuffer2, 0 );
		if (FAILED(hr))
		{
			WriteToLog("SetIndices() failed on m_p32bitIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 8, 0, 3);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed on m_p32bitIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
		if ( m_dwVersion >= 0x0700 )
			SetRenderState(D3DRS_LIGHTING, (DWORD)TRUE);
	}
	//draw untransformed unlit vertex buffers and 32 bit index buffers
	if ( m_uTestNumber ==  9+ 4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) || 
		 m_uTestNumber == 18+10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)  )
	{
		D3DXMATRIX	Matrix;
		D3DXMatrixIdentity( &Matrix );
		m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
		m_pSrcDevice->SetVertexShader(FVF_VERTEX);
		m_pSrcDevice->SetStreamSource( 0, m_pVertexBuffer, GetVertexSize(FVF_VERTEX) );
		hr = m_pSrcDevice->SetIndices( m_p32bitIndexBuffer1, 0 );
		if (FAILED(hr))
		{
			WriteToLog("SetIndices() failed on m_p32bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 3, 0, 1);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed on m_p32bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
		hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 4, 1, 1);
		if (FAILED(hr))
		{
			WriteToLog("DrawIndexedPrimitive() failed on m_p32bitIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			m_bCheckColor[0] = false;

			if (D3DERR_OUTOFVIDEOMEMORY == hr)
			{
				m_pSrcDevice->GetLastError();
				SkipTestRange(m_uTestNumber, m_uTestNumber);
			}
			else
				Fail();
		}
	}

	//draw textures
	if (draw_Textures)
	{
		D3DXMATRIX	Matrix;
		D3DXMatrixIdentity( &Matrix );
		m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
		hr = m_pSrcDevice->SetTexture(0, m_pTexture);
		if (FAILED(hr))
		{
			WriteToLog("SetTexture() to texture %s failed hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		WORD LevelCount = (WORD)m_pTexture->GetLevelCount();

		if ( m_uTestNumber%(6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)+9) < 4 + 2*m_uCommonTextureFormats+2*m_uCommonCubeFormats+2*m_uCommonVolumeFormats )
		{
			if (LevelCount == 1)
			{
				if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, m_pVertexBuffer, 
				 			0, 4, NULL, 0, 0, 0, POOL_MANAGED, false))
				{
					hr = m_pSrcDevice->GetLastError();
					
					WriteToLog("DrawPrimitiveVertexBuffer() failed on texture %s hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
					m_bCheckColor[0] = false;
							
					if (D3DERR_OUTOFVIDEOMEMORY == hr)
					{
						m_pSrcDevice->GetLastError();
					}
					else
						Fail();
				}
			}
			else
			{
				for (WORD LevelIndex = 0; LevelIndex < LevelCount; LevelIndex++)
				{
					if( !m_UseLODBias || (m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].ddpfPixelFormat.dwFlags & PF_FOURCC) )
					{
						if (m_uPrevMaxMipLevel != LevelIndex)
						{
							m_uPrevMaxMipLevel = LevelIndex;
							SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
						}
					}
					else
						m_pTexture->SetLOD(LevelIndex);

					if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, m_pVertexBuffer, 
				 			4*(LevelIndex+1), 4, NULL, 0, 0, 0, POOL_MANAGED, false))
					{
						hr = m_pSrcDevice->GetLastError();
							
						WriteToLog("DrawPrimitiveVertexBuffer() failed on texture %s on level %d hr=%s(%x).\n", szBuffer, LevelIndex, m_pD3D->HResultToString(hr), hr);
						m_bCheckColor[LevelIndex] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
						}
						else
							Fail();
					}
				}
			}
		}
		else if ( m_uTestNumber%(6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)+9) < 7 + 4*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			hr = m_pSrcDevice->SetIndices( m_p16bitTexIndexBuffer1, 0 );
			if (FAILED(hr))
			{
				WriteToLog("SetIndices() failed on m_p16bitTexIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			hr = m_pSrcDevice->SetStreamSource( 0, m_pVertexBuffer, GetVertexSize(FVF_VERTEX) );
			if (FAILED(hr))
			{
				WriteToLog("SetStreamSource() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			hr = m_pSrcDevice->SetVertexShader( FVF_VERTEX );
			if (FAILED(hr))
			{
				WriteToLog("SetVertexShader() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			if (LevelCount == 1)
			{
				hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 4, 0, 2);
				if (FAILED(hr))
				{
					WriteToLog("DrawIndexedPrimitive() on texture %s failed hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
					m_bCheckColor[0] = false;

					if (D3DERR_OUTOFVIDEOMEMORY == hr)
					{
						m_pSrcDevice->GetLastError();
					}
					else
						Fail();
				}
			}
			else
			{
				for (WORD LevelIndex = 0; LevelIndex < LevelCount; LevelIndex++)
				{
					if( !m_UseLODBias || (m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].ddpfPixelFormat.dwFlags & PF_FOURCC) )
					{
						if (m_uPrevMaxMipLevel != LevelIndex)
						{
							m_uPrevMaxMipLevel = LevelIndex;
							SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
						}
					}
					else
						m_pTexture->SetLOD(LevelIndex);

					if (LevelIndex < 4)
					{
						hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 4*(LevelIndex+2), 4*(LevelIndex+1), 2);
						if (FAILED(hr))
						{
							WriteToLog("DrawIndexedPrimitive() failed on texture %s on level %d hr=%s(%x).\n", szBuffer, LevelIndex, m_pD3D->HResultToString(hr), hr);
							m_bCheckColor[LevelIndex] = false;

							if (D3DERR_OUTOFVIDEOMEMORY == hr)
							{
								m_pSrcDevice->GetLastError();
							}
							else
								Fail();
						}
					}
					else
					{
						if (LevelIndex == 4)
						{
							hr = m_pSrcDevice->SetIndices( m_p16bitTexIndexBuffer2, 0 );
							if (FAILED(hr))
							{
								WriteToLog("SetIndices() failed on m_p16bitTexIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
								Fail();
							}
						}
						hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 4*(LevelIndex+2), 4*(LevelIndex-4), 2);
						if (FAILED(hr))
						{
							WriteToLog("DrawIndexedPrimitive() failed on texture %s level %d hr=%s(%x).\n", szBuffer, LevelIndex, m_pD3D->HResultToString(hr), hr);
							m_bCheckColor[LevelIndex] = false;

							if (D3DERR_OUTOFVIDEOMEMORY == hr)
							{
								m_pSrcDevice->GetLastError();
							}
							else
								Fail();
						}
					}
				}
			}
		}
		else
		{
			hr = m_pSrcDevice->SetIndices( m_p32bitTexIndexBuffer1, 0 );
			if (FAILED(hr))
			{
				WriteToLog("SetIndices() failed on m_p32bitTexIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			hr = m_pSrcDevice->SetStreamSource( 0, m_pVertexBuffer, GetVertexSize(FVF_VERTEX) );
			if (FAILED(hr))
			{
				WriteToLog("SetStreamSource() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			hr = m_pSrcDevice->SetVertexShader( FVF_VERTEX );
			if (FAILED(hr))
			{
				WriteToLog("SetVertexShader() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			if (LevelCount == 1)
			{
				hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 4, 0, 2);
				if (FAILED(hr))
				{
					WriteToLog("DrawIndexedPrimitive() failed on texture %s hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
					m_bCheckColor[0] = false;

					if (D3DERR_OUTOFVIDEOMEMORY == hr)
					{
						m_pSrcDevice->GetLastError();
					}
					else
						Fail();
				}
			}
			else
			{
				for (WORD LevelIndex = 0; LevelIndex < LevelCount; LevelIndex++)
				{
					if( !m_UseLODBias || (m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].ddpfPixelFormat.dwFlags & PF_FOURCC) )
					{
						if (m_uPrevMaxMipLevel != LevelIndex)
						{
							m_uPrevMaxMipLevel = LevelIndex;
							SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
						}
					}
					else
						m_pTexture->SetLOD(LevelIndex);

					if (LevelIndex < 4)
					{
						hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 4*(LevelIndex+2), 4*(LevelIndex+1), 2);
						if (FAILED(hr))
						{
							WriteToLog("DrawIndexedPrimitive() failed on texture %s on level %d ith HResult = %s(%x).\n", szBuffer, LevelIndex, m_pD3D->HResultToString(hr), hr);
							m_bCheckColor[LevelIndex] = false;

							if (D3DERR_OUTOFVIDEOMEMORY == hr)
							{
								m_pSrcDevice->GetLastError();
							}
							else
								Fail();
						}
					}
					else
					{
						if (LevelIndex == 4)
						{
							hr = m_pSrcDevice->SetIndices( m_p32bitTexIndexBuffer2, 0 );
							if (FAILED(hr))
							{
								WriteToLog("SetIndices() failed on m_p32bitTexIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
								Fail();
							}
						}
						hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, 4*(LevelIndex+2), 4*(LevelIndex-4), 2);
						if (FAILED(hr))
						{
							WriteToLog("DrawIndexedPrimitive() failed on texture %s on level %d hr=%s(%x).\n", szBuffer, LevelIndex, m_pD3D->HResultToString(hr), hr);
							m_bCheckColor[LevelIndex] = false;

							if (D3DERR_OUTOFVIDEOMEMORY == hr)
							{
								m_pSrcDevice->GetLastError();
							}
							else
								Fail();
						}
					}
				}
			}
		}
		if ( m_pCommonTextureFormats[m_pCurrentMode->nTextureFormat].d3dfFormat == FMT_A8 )
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE );
		}
	}
	//draw cube textures
	if (draw_CubeMaps)
	{
		hr = m_pSrcDevice->SetTexture(0, m_pCubeTexture);
		if (FAILED(hr))
		{
			WriteToLog("SetTexture() failed on cube map %s hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		WORD LevelCount = (WORD)m_pCubeTexture->GetLevelCount();
		D3DXMATRIX	Matrix, TranslatedM, ScaledM;
		WORD hnum = 2, wnum = 3*LevelCount;
		FLOAT wcell = 2.f/wnum, hcell = 2.f/hnum;
		D3DXMatrixTranslation(&TranslatedM, -1+wcell/2.f, 1+hcell/2.f, 0);
		D3DXMatrixScaling(&ScaledM, wcell/2, hcell/2, 1);
		D3DXMatrixMultiply(&Matrix, &ScaledM, &TranslatedM);
		
		if ( m_uTestNumber%(6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)+9) < 4 + 2*m_uCommonTextureFormats+2*m_uCommonCubeFormats+2*m_uCommonVolumeFormats )
		{
			for (WORD row = 0; row <hnum; row++)
			{
				D3DXMatrixTranslation(&TranslatedM, 2, -hcell, 0);
				D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
				
				for(WORD col = 0; col < wnum; col++)
				{
					if ((row*wnum+col)%6==0)
					{
						if( !m_UseLODBias || (m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].ddpfPixelFormat.dwFlags & PF_FOURCC) )
						{
							if (m_uPrevMaxMipLevel != (row*wnum+col)/6)
							{
								m_uPrevMaxMipLevel = (row*wnum+col)/6;
								SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, (row*wnum+col)/6);
							}
						}
						else  if (m_pCubeTexture->GetLevelCount() > 1)
							m_pCubeTexture->SetLOD((row*wnum+col)/6);
					}

					D3DXMatrixTranslation(&TranslatedM, -wcell, 0, 0);
					D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
					
					m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);

					if (!m_pSrcDevice->DrawPrimitiveVertexBuffer(D3DPT_TRIANGLESTRIP, m_pCubeVertexBuffer, 
				 		((row*wnum+col)%6)*4, 4, NULL, 0, 0, 0, POOL_MANAGED, false))
					{
						hr = m_pSrcDevice->GetLastError();
						
						WriteToLog("DrawPrimitiveVertexBuffer() failed on cube map %s hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
						m_bCheckColor[row*wnum+col] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
						}
						else
							Fail();
					}
				}
			}
		}
		else if ( m_uTestNumber%(6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)+9) < 7 + 4*m_uCommonTextureFormats+4*m_uCommonCubeFormats+4*m_uCommonVolumeFormats )
		{
			hr = m_pSrcDevice->SetStreamSource( 0, m_pCubeVertexBuffer, GetVertexSize(FVF_VOLVERTEX) );
			if (FAILED(hr))
			{
				WriteToLog("SetStreamSource() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			hr = m_pSrcDevice->SetVertexShader( FVF_VOLVERTEX );
			if (FAILED(hr))
			{
				WriteToLog("SetVerteShader() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			for (WORD row = 0; row <hnum; row++)
			{
				D3DXMatrixTranslation(&TranslatedM, 2, -hcell, 0);
				D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
				//Matrix = MatrixMult(TranslateMatrix(D3DVECTOR(2, -hcell, 0)), Matrix);
				for(WORD col = 0; col < wnum; col++)
				{
					if ((row*wnum+col)%6 == 0)
					{
						if( !m_UseLODBias || (m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].ddpfPixelFormat.dwFlags & PF_FOURCC) )
						{
							if (m_uPrevMaxMipLevel != (row*wnum+col)/6)
							{
								m_uPrevMaxMipLevel = (row*wnum+col)/6;
								SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, (row*wnum+col)/6);
							}
						}
						else if (m_pCubeTexture->GetLevelCount() > 1)
							m_pCubeTexture->SetLOD((row*wnum+col)/6);

						hr = m_pSrcDevice->SetIndices( m_p16bitCubeIndexBuffer1, 0 );
						if (FAILED(hr))
						{
							WriteToLog("SetIndices() failed on m_p16bitCubeIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
							Fail();
						}
					}
					if ((row*wnum+col)%6 == 3)
					{
						hr = m_pSrcDevice->SetIndices( m_p16bitCubeIndexBuffer2, 0 );
						if (FAILED(hr))
						{
							WriteToLog("SetIndices() failed  on m_p16bitCubeIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
							Fail();
						}
					}
					D3DXMatrixTranslation(&TranslatedM, -wcell, 0, 0);
					D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
					//Matrix = MatrixMult(TranslateMatrix(D3DVECTOR(-wcell, 0, 0)), Matrix);
					m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
					hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, ((row*wnum+col)%6+1)*4, ((row*wnum+col)%3)*4, 2);
					if (FAILED(hr))
					{
						WriteToLog("DrawIndexedPrimitive() failed  on cube map %s hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
						m_bCheckColor[row*wnum+col] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
						}
						else
							Fail();
					}
				}
			}
		}
		else
		{
			hr = m_pSrcDevice->SetStreamSource( 0, m_pCubeVertexBuffer, GetVertexSize(FVF_VOLVERTEX) );
			if (FAILED(hr))
			{
				WriteToLog("SetStreamSource() failed hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			hr = m_pSrcDevice->SetVertexShader( FVF_VOLVERTEX );
			if (FAILED(hr))
			{
				WriteToLog("SetVerteShader() failed hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			for (WORD row = 0; row <hnum; row++)
			{
				D3DXMatrixTranslation(&TranslatedM, 2, -hcell, 0);
				D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
				//Matrix = MatrixMult(TranslateMatrix(D3DVECTOR(2, -hcell, 0)), Matrix);
				for(WORD col = 0; col < wnum; col++)
				{
					if ((row*wnum+col)%6==0)
					{
						if( !m_UseLODBias || (m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].ddpfPixelFormat.dwFlags & PF_FOURCC) )
						{
							if (m_uPrevMaxMipLevel != (row*wnum+col)/6)
							{
								m_uPrevMaxMipLevel = (row*wnum+col)/6;
								SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, (row*wnum+col)/6);
							}
						}
						else if (m_pCubeTexture->GetLevelCount() > 1)
							m_pCubeTexture->SetLOD((row*wnum+col)/6);

						hr = m_pSrcDevice->SetIndices( m_p32bitCubeIndexBuffer1, 0 );
						if (FAILED(hr))
						{
							WriteToLog("SetIndices() failed on m_p32bitCubeIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
							Fail();
						}
					}
					if ((row*wnum+col)%6 == 3)
					{
						hr = m_pSrcDevice->SetIndices( m_p32bitCubeIndexBuffer2, 0 );
						if (FAILED(hr))
						{
							WriteToLog("SetIndices() failed on m_p32bitCubeIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
							Fail();
						}
					}
					D3DXMatrixTranslation(&TranslatedM, -wcell, 0, 0);
					D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
					//Matrix = MatrixMult(TranslateMatrix(D3DVECTOR(-wcell, 0, 0)), Matrix);
					m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
					hr = m_pSrcDevice->DrawIndexedPrimitive( D3DPT_TRIANGLESTRIP, 0, ((row*wnum+col)%6+1)*4, ((row*wnum+col)%3)*4, 2);
					if (FAILED(hr))
					{
						WriteToLog("DrawIndexedPrimitive() failed on cube map %s hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
						m_bCheckColor[row*wnum+col] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
						}
						else
							Fail();
					}
				}
			}
		}
		if ( m_pCommonCubeFormats[m_pCurrentMode->nCubeFormat].d3dfFormat == FMT_A8 )
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE );
		}
	}

	//draw volume textures
	if (draw_VolTextures)
	{
		hr = m_pSrcDevice->SetTexture(0, m_pVolTexture);
		if (FAILED(hr))
		{
			WriteToLog("SetTexture() failed on volume %s hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->SetStreamSource( 0, m_pVolVertexBuffer, GetVertexSize(FVF_VOLVERTEX) );
		if (FAILED(hr))
		{
			WriteToLog("SetStreamSource() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		hr = m_pSrcDevice->SetVertexShader( FVF_VOLVERTEX );
		if (FAILED(hr))
		{
			WriteToLog("SetVerteShader() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			Fail();
		}
		WORD LevelCount = (WORD)m_pVolTexture->GetLevelCount();
		WORD newLevelIndex = 0, LevelIndex = 0;
		D3DXMATRIX	Matrix, TranslatedM, ScaledM;
		WORD hnum = 4, wnum = (LevelCount == 1 ? 2 : 5);
		FLOAT wcell = 2.f/wnum, hcell = 2.f/hnum;
		D3DXMatrixTranslation(&TranslatedM, -1+wcell/2.f, 1+hcell/2.f, 0);
		D3DXMatrixScaling(&ScaledM, wcell/2, hcell/2, 1);
		D3DXMatrixMultiply(&Matrix, &ScaledM, &TranslatedM);
		
		if ( m_uTestNumber%(6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)+9) < 4 + 2*m_uCommonTextureFormats+2*m_uCommonCubeFormats+2*m_uCommonVolumeFormats )
		{
			for (WORD row = 0; row <hnum; row++)
			{
				D3DXMatrixTranslation(&TranslatedM, 2, -hcell, 0);
				D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
				
				for(WORD col = 0; col < wnum; col++)
				{
					if (row*wnum+col>=m_TotalNumberOfSlices)
						break;
					if (newLevelIndex == row*wnum+col)
					{
						newLevelIndex += (1 << max(0, 3-LevelIndex));
						if( !m_UseLODBias || (m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].ddpfPixelFormat.dwFlags & PF_FOURCC) )
						{
							if (m_uPrevMaxMipLevel != LevelIndex)
							{
								m_uPrevMaxMipLevel = LevelIndex;
								SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
							}
							LevelIndex++;
						}
						else
							m_pVolTexture->SetLOD(LevelIndex++);
					}
					D3DXMatrixTranslation(&TranslatedM, -wcell, 0, 0);
					D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
					
					m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);

					hr = m_pSrcDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP, min((row*wnum+col)*4, 56), 2);
					if (FAILED(hr))
					{
						WriteToLog("DrawPrimitive() failed on volume %s hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
						m_bCheckColor[row*wnum+col] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
						}
						else
							Fail();
					}
				}
			}
		}
		else if ( m_uTestNumber%(6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats)+9) < 7 + 4*m_uCommonTextureFormats+4*m_uCommonCubeFormats+4*m_uCommonVolumeFormats )
		{
			hr = m_pSrcDevice->SetIndices( m_p16bitVolIndexBuffer1, 0 );
			if (FAILED(hr))
			{
				WriteToLog("SetIndices() failed on m_p16bitVolIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			for (WORD row = 0; row <hnum; row++)
			{
				D3DXMatrixTranslation(&TranslatedM, 2, -hcell, 0);
				D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
				
				for(WORD col = 0; col < wnum; col++)
				{
					if (row*wnum+col>=m_TotalNumberOfSlices)
						break;
					if (row*wnum+col == 8)
					{
						hr = m_pSrcDevice->SetIndices( m_p16bitVolIndexBuffer2, 0 );
						if (FAILED(hr))
						{
							WriteToLog("SetIndices() failed on m_p16bitVolIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
							Fail();
						}
					}
					if (newLevelIndex == row*wnum+col)
					{
						newLevelIndex += (1 << max(0, 3-LevelIndex));
						if( !m_UseLODBias || (m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].ddpfPixelFormat.dwFlags & PF_FOURCC) )
						{
							if (m_uPrevMaxMipLevel != LevelIndex)
							{
								m_uPrevMaxMipLevel = LevelIndex;
								SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
							}
							LevelIndex++;
						}
						else
							m_pVolTexture->SetLOD(LevelIndex++);
					}
					D3DXMatrixTranslation(&TranslatedM, -wcell, 0, 0);
					D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
					
					m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);
					
					hr = m_pSrcDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, min((row*wnum+col+1)*4, 60), ((min(row*wnum+col,14))%8)*4, 2);
					if (FAILED(hr))
					{
						WriteToLog("DrawIndexedPrimitive() failed on volume %s hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
						m_bCheckColor[row*wnum+col] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
						}
						else
							Fail();
					}
				}
			}
		}
		else
		{
			hr = m_pSrcDevice->SetIndices( m_p32bitVolIndexBuffer1, 0 );
			if (FAILED(hr))
			{
				WriteToLog("SetIndices() failed on m_p32bitVolIndexBuffer1 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
				Fail();
			}
			for (WORD row = 0; row <hnum; row++)
			{
				D3DXMatrixTranslation(&TranslatedM, 2, -hcell, 0);
				D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
				
				for(WORD col = 0; col < wnum; col++)
				{
					if (row*wnum+col>=m_TotalNumberOfSlices)
						break;
					if (row*wnum+col == 8)
					{
						hr = m_pSrcDevice->SetIndices( m_p32bitVolIndexBuffer2, 0 );
						if (FAILED(hr))
						{
							WriteToLog("SetIndices() failed on m_p32bitVolIndexBuffer2 hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
							Fail();
						}
					}
					if (newLevelIndex == row*wnum+col)
					{
						newLevelIndex += (1 << max(0, 3-LevelIndex));
						if( !m_UseLODBias || (m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].ddpfPixelFormat.dwFlags & PF_FOURCC) )
						{
							if (m_uPrevMaxMipLevel != LevelIndex)
							{
								m_uPrevMaxMipLevel = LevelIndex;
								SetTextureStageState(0, D3DTSS_MAXMIPLEVEL, LevelIndex);
							}
							LevelIndex++;
						}
						else
							m_pVolTexture->SetLOD(LevelIndex++);
					}
					D3DXMatrixTranslation(&TranslatedM, -wcell, 0, 0);
					D3DXMatrixMultiply(&Matrix, &Matrix, &TranslatedM);
					
                    m_pSrcDevice->SetTransform(D3DTS_WORLD, (D3DMATRIX*)&Matrix);

					hr = m_pSrcDevice->DrawIndexedPrimitive(D3DPT_TRIANGLESTRIP, 0, min((row*wnum+col+1)*4, 60), ((min(row*wnum+col,14))%8)*4, 2);
					if (FAILED(hr))
					{
						WriteToLog("DrawIndexedPrimitive() failed on volume %s hr=%s(%x).\n", szBuffer, m_pD3D->HResultToString(hr), hr);
						m_bCheckColor[row*wnum+col] = false;

						if (D3DERR_OUTOFVIDEOMEMORY == hr)
						{
							m_pSrcDevice->GetLastError();
						}
						else
							Fail();
					}
				}
			}
		}
		if ( m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat].d3dfFormat == FMT_A8 )
		{
			SetRenderState(D3DRS_ALPHABLENDENABLE, (DWORD)FALSE);
			SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD)D3DTA_TEXTURE );
		}
	}	

	hr = m_pSrcDevice->EndScene();
	if (FAILED(hr))
	{
		WriteToLog("EndScene failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		Fail();
		return false;
	}
	
	return true;
}

//-----------------------------------------------------------------------------

bool CBasicTest::ProcessFrame(void)
{
	ResMan::ProcessFrame();

	CSurface   *pSrcSurf = NULL;
	HRESULT     hr       = S_OK;
	bool        result   = true;
	void	   *pBuf     = NULL;
	int         nPitch;
    LOCKED_RECT LockRect;
    
	hr = m_pSrcDevice->GetBackBufferCached(0, BACKBUFFER_TYPE_MONO, &pSrcSurf);
	if(FAILED(hr))
	{
		WriteToLog("ProcessFrame: GetBackBufferCached() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

    // Get the Surface data pointer for our subrect
    hr = pSrcSurf->LockRect(&LockRect, NULL, NULL);
    if (FAILED(hr))
	{
		WriteToLog("ProcessFrame: LockRect() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}

	pBuf   = LockRect.pBits;
    nPitch = LockRect.iPitch;

	if (  m_uTestNumber == 1 || 
		  m_uTestNumber == 4 + 2*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats ||
		  m_uTestNumber == 7 + 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats ||
		  m_uTestNumber == 10+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats || 
		  m_uTestNumber == 13+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats ||
		  m_uTestNumber == 16+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats  )
		
		  if ( m_bCheckColor[0] && !CheckColors(pSrcSurf, 0x0000ff00, pBuf, nPitch, NULL, 0, 1, 1) )
			  result = false;
	
	if (  m_uTestNumber == 2 || 
		  m_uTestNumber == 5 + 2*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats ||
		  m_uTestNumber == 8 + 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats ||
		  m_uTestNumber == 11+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats || 
		  m_uTestNumber == 14+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats ||
		  m_uTestNumber == 17+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats  )
		if ( m_bCheckColor[0] && !CheckColors(pSrcSurf, 0x000000ff, pBuf, nPitch, NULL, 0, 1, 1) )
			  result = false;

	if (  m_uTestNumber == 3 || 
		  m_uTestNumber == 6 + 2*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats ||
		  m_uTestNumber == 9 + 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats ||
		  m_uTestNumber == 12+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats || 
		  m_uTestNumber == 15+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats ||
		  m_uTestNumber == 18+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats  )
		if ( m_bCheckColor[0] && !CheckColors(pSrcSurf, 0x00ffffff, pBuf, nPitch, NULL, 0, 1, 1) )
			  result = false;

	if (!result)
		WriteToLog("\n");

	if ( draw_Textures )
	{
		result = CheckTexture(pSrcSurf, pBuf, nPitch, m_pTexture, m_bCheckColor, NULL, 0, m_pTexture->GetLevelCount()==1 ? 1 : 4, m_pTexture->GetLevelCount()==1 ? 1 : 2);
		RELEASE(m_pTexture);
		draw_Textures = false;
	}

	if ( draw_CubeMaps )
	{
		result = CheckCubeMap(pSrcSurf, pBuf, nPitch, m_pCubeTexture, m_bCheckColor, NULL, 0, 3*m_pCubeTexture->GetLevelCount(), 2);
		RELEASE(m_pCubeTexture);
		draw_CubeMaps = false;
	}

	if ( draw_VolTextures )
	{
		result = CheckVolumeTexture(pSrcSurf, pBuf, nPitch, m_pVolTexture, m_bCheckColor, NULL, 0, m_pVolTexture->GetLevelCount()==1 ? 2: 5, 4 );
		RELEASE(m_pVolTexture);
		draw_VolTextures = false;
	}

	if (result)
		Pass();
	else
		Fail();

	hr = pSrcSurf->UnlockRect();
    if (FAILED(hr))
    {
	    WriteToLog("ProcessFrame: UnlockRect() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
		goto Exit;
    }

	//set timer to check PreLoad function
	if ( m_dwVersion >= 0x0700 )
	{
		LOCKED_RECT lock;
		hr = m_pBackBuffer->LockRect(&lock, NULL, 0);
		if (FAILED(hr))
		{
			WriteToLog("ProcessFrame: LockRect() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		hr = m_pBackBuffer->UnlockRect();
		if (FAILED(hr))
		{
			WriteToLog("ProcessFrame: UnlockRect() failed hr=%s(%x).\n", m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		QueryPerformanceCounter((LARGE_INTEGER*)&llStart);
	}

	Exit:
	if (FAILED(hr))
	{
		if (/*DDERR_OUTOFMEMORY == hr ||*/ D3DERR_OUTOFVIDEOMEMORY == hr || E_OUTOFMEMORY == hr)
		{
			m_pSrcDevice->GetLastError();
			SkipTestRange(m_uTestNumber, m_uTestNumber);
		}
		else
		{
			Fail();
			result = false;
		}
	}

	RELEASE(pSrcSurf);

	return result;
}

//-----------------------------------------------------------------------------

void CBasicTest::UpdateStatus(void)
{
	if ( m_dwVersion >= 0x0700 )
	{
		QueryPerformanceCounter((LARGE_INTEGER*)&llEnd);
		if ( m_uTestNumber < 10+6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) )
		{
			m_NonPreLoadSum += QPC_DIFFERENCE(llEnd, llStart); 
		}
		else
		{
			m_PreLoadSum += QPC_DIFFERENCE(llEnd, llStart);
		}
	}
	// Tell the logfile how they did
    CD3DTest::UpdateStatus();
}

//-----------------------------------------------------------------------------

void CBasicTest::CheckBasicTestCaps()
{
	if ( m_dwVersion < 0x0800 )	
	{
        WriteToLog("Index buffers are not supported in dx7 and dx6.\n");
		SkipTestRange( 4 +2*m_uCommonTextureFormats+2*m_uCommonCubeFormats+2*m_uCommonVolumeFormats, 9 + 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats );
		SkipTestRange( 13+8*m_uCommonTextureFormats+8*m_uCommonCubeFormats+8*m_uCommonVolumeFormats, 18+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+12*m_uCommonVolumeFormats );
		
		WriteToLog("PreLoad is not supported for vertex buffers in dx7 and dx6.\n");
		SkipTestRange( 10+ 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats), 12+ 6*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) );
		SkipTestRange( 13+ 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats), 15+ 8*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) );
		SkipTestRange( 16+10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats), 18+10*(m_uCommonTextureFormats+m_uCommonCubeFormats+m_uCommonVolumeFormats) );
	}
	else if ( !m_Use32BitIB )
	{
		WriteToLog("32 bit index buffers are not supported.\n");
		SkipTestRange(  7+ 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats,  9+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats );
		SkipTestRange( 16+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats, 18+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+12*m_uCommonVolumeFormats );
	}

	if ( !m_UseTextures )
	{
		WriteToLog("BasicTest: Texture are skipped.\n");
		if (m_uCommonTextureFormats)
		{
			SkipTestRange( 4, 3+ 2*m_uCommonTextureFormats);
			SkipTestRange( 7+ 2*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats,  6+ 4*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats );
			SkipTestRange(10+ 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats,  9+ 6*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats );
			SkipTestRange(13+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats, 12+ 8*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats );
			SkipTestRange(16+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats, 15+10*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats );
			SkipTestRange(19+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats, 18+12*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats );
		}
	}
	else if ( !m_UseMipMaps )
	{
		WriteToLog("BasicTest: Mipmaping is not supported.\n");
		if (m_uCommonTextureFormats)
		{
			SkipTestRange( 4+   m_uCommonTextureFormats, 3+ 2*m_uCommonTextureFormats);
			SkipTestRange( 7+ 3*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats,  6+ 4*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats );
			SkipTestRange(10+ 5*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats,  9+ 6*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats );
			SkipTestRange(13+ 7*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats, 12+ 8*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats );
			SkipTestRange(16+ 9*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats, 15+10*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats );
			SkipTestRange(19+11*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats, 18+12*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats );
		}
	}
	if ( !m_UseCubeMaps )
	{
		WriteToLog("BasicTest: cube maps are not supported.\n");
		if (m_uCommonCubeFormats)
		{
			SkipTestRange( 4+ 2*m_uCommonTextureFormats,  3+ 2*m_uCommonTextureFormats+2*m_uCommonCubeFormats);
			SkipTestRange( 7+ 4*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats,  6+ 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats);
			SkipTestRange(10+ 6*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats,  9+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats);
			SkipTestRange(13+ 8*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats, 12+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats);
			SkipTestRange(16+10*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats, 15+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats);
			SkipTestRange(19+12*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats, 18+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+10*m_uCommonVolumeFormats);
		}
	}
	else if ( !m_UseMipCubeMaps )
	{
		WriteToLog("BasicTest: mipmap cube maps are not supported.\n");
		if (m_uCommonCubeFormats)
		{
			SkipTestRange( 4+ 2*m_uCommonTextureFormats+   m_uCommonCubeFormats,  3+ 2*m_uCommonTextureFormats+2*m_uCommonCubeFormats);
			SkipTestRange( 7+ 4*m_uCommonTextureFormats+ 3*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats,  6+ 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats);
			SkipTestRange(10+ 6*m_uCommonTextureFormats+ 5*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats,  9+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats);
			SkipTestRange(13+ 8*m_uCommonTextureFormats+ 7*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats, 12+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats);
			SkipTestRange(16+10*m_uCommonTextureFormats+ 9*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats, 15+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats);
			SkipTestRange(19+12*m_uCommonTextureFormats+11*m_uCommonCubeFormats+10*m_uCommonVolumeFormats, 18+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+10*m_uCommonVolumeFormats);
		}
	}
	if ( !m_UseVolMaps )
	{
		WriteToLog("BasicTest: volume textures are not supported.\n");
		if (m_uCommonVolumeFormats)
		{
			SkipTestRange( 4+ 2*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats,  3+ 2*m_uCommonTextureFormats+2*m_uCommonCubeFormats+2*m_uCommonVolumeFormats);
			SkipTestRange( 7+ 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats,  6+ 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats);
			SkipTestRange(10+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats,  9+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats);
			SkipTestRange(13+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats, 12+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats);
			SkipTestRange(16+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats, 15+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats);
			SkipTestRange(19+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+10*m_uCommonVolumeFormats, 18+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+12*m_uCommonVolumeFormats);
		}
	}
	else if ( !m_UseMipVolMaps )
	{
		WriteToLog("BasicTest: mipmap volume textures are not supported.\n");
		if (m_uCommonVolumeFormats)
		{
			SkipTestRange( 4+ 2*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+   m_uCommonVolumeFormats,  3+ 2*m_uCommonTextureFormats+ 2*m_uCommonCubeFormats+ 2*m_uCommonVolumeFormats);
			SkipTestRange( 7+ 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 3*m_uCommonVolumeFormats,  6+ 4*m_uCommonTextureFormats+ 4*m_uCommonCubeFormats+ 4*m_uCommonVolumeFormats);
			SkipTestRange(10+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 5*m_uCommonVolumeFormats,  9+ 6*m_uCommonTextureFormats+ 6*m_uCommonCubeFormats+ 6*m_uCommonVolumeFormats);
			SkipTestRange(13+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 7*m_uCommonVolumeFormats, 12+ 8*m_uCommonTextureFormats+ 8*m_uCommonCubeFormats+ 8*m_uCommonVolumeFormats);
			SkipTestRange(16+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+ 9*m_uCommonVolumeFormats, 15+10*m_uCommonTextureFormats+10*m_uCommonCubeFormats+10*m_uCommonVolumeFormats);
			SkipTestRange(19+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+11*m_uCommonVolumeFormats, 18+12*m_uCommonTextureFormats+12*m_uCommonCubeFormats+12*m_uCommonVolumeFormats);
		}
	}
}

//-----------------------------------------------------------------------------

void CBasicTest::Cleanup()
{
	ResMan::Cleanup();

	RELEASE(m_pTLVertexBuffer);
	RELEASE(m_pLVertexBuffer);
	RELEASE(m_pVertexBuffer);
	RELEASE(m_pCubeVertexBuffer);
	RELEASE(m_pVolVertexBuffer);

	RELEASE(m_p16bitIndexBuffer1);
	RELEASE(m_p16bitIndexBuffer2);
	RELEASE(m_p32bitIndexBuffer1);
	RELEASE(m_p32bitIndexBuffer2);
	RELEASE(m_p16bitTexIndexBuffer1);
	RELEASE(m_p16bitTexIndexBuffer2);
	RELEASE(m_p16bitCubeIndexBuffer1);
	RELEASE(m_p16bitCubeIndexBuffer2);
	RELEASE(m_p16bitVolIndexBuffer1);
	RELEASE(m_p16bitVolIndexBuffer2);
	RELEASE(m_p32bitTexIndexBuffer1);
	RELEASE(m_p32bitTexIndexBuffer2);
	RELEASE(m_p32bitCubeIndexBuffer1);
	RELEASE(m_p32bitCubeIndexBuffer2);
	RELEASE(m_p32bitVolIndexBuffer1);
	RELEASE(m_p32bitVolIndexBuffer2);
}

//-----------------------------------------------------------------------------

bool CBasicTest::TestTerminate()
{
	Cleanup();

	if (llFreq != 0)
	{
		WriteToLog("Total time for NonPreLoad resources is %dus\n", m_NonPreLoadSum*1000000/llFreq);
		WriteToLog("Total time for    PreLoad resources is %dus\n",    m_PreLoadSum*1000000/llFreq);
	}

	RELEASE(m_pTexture);
	RELEASE(m_pCubeTexture);
	RELEASE(m_pVolTexture);
	RELEASE(m_pBackBuffer);

	return true;
}