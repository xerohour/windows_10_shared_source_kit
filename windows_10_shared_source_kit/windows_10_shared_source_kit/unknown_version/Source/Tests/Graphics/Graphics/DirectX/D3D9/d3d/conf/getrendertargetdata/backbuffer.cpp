#include "GetRenderTargetData.h"
#include "resource.h"

#include "CBuffers.h"
#include "CTextures.h"
#include "CImageCompare.h"
#include "d3d9.h"

#include "Utils.h"


CBackBuffer::CBackBuffer() :
	m_pVertexBuffer( 0 ),
	m_pTexture( 0 ),
	m_pPlainSurface( 0 )
{
	m_szTestName = _T("BBSUR");
	m_szCommandKey = _T("grtdbb");

	m_Options.D3DOptions.dwWinMode = WINMODE_FULLSCREEN;

	m_Options.ModeOptions.uMaxDisplayModes = MODEOPT_MAXMODES;
	m_Options.ModeOptions.BackBufferFmt = FMT_ALL;
	m_Options.ModeOptions.MultisampType = MULTISAMPLE_ALL;

	addParamNode( "OffscreenPlainSurface", COffscreenPlainSurfaceParams() );
}

bool CBackBuffer::ParseCmdLine()
{
	return CGetRenderTargetData::ParseCmdLine();
}

bool CBackBuffer::CapsCheck()
{
	return CGetRenderTargetData::CapsCheck();
}

bool CBackBuffer::FilterFormats(FORMAT *pFmtIn, DWORD dwInclFlags, DWORD dwExclFlags, FORMAT **pFmtOut, UINT *uCount)
{
	return CGetRenderTargetData::FilterFormats(pFmtIn, dwInclFlags, dwExclFlags, pFmtOut, uCount);
}

bool CBackBuffer::FilterResolution(PDISPLAYMODE pMode)
{
	return CGetRenderTargetData::FilterResolution(pMode);
}

bool CBackBuffer::Setup()
{
	HRESULT hr = E_FAIL;

	struct Vertex
	{
		Vertex( float x, float y, float z, float u, float v ) : x(x), y(y), z(z), nx(0.0f), ny(0.0f), nz(-1.0f), u(u), v(v) {}
		float x;
		float y;
		float z;
		float nx;
		float ny;
		float nz;
		float u;
		float v;
	} *pVertex;

	m_nVertices = 4;

	const UINT fvf = D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1;

	bool bCreateVertexBufferResult = CreateVertexBuffer(m_nVertices * sizeof(Vertex), USAGE_WRITEONLY, fvf, POOL_DEFAULT, &m_pVertexBuffer);
	if (bCreateVertexBufferResult != true)
	{
		WriteToLog("*** Failure in Setup: CreateVertexBuffer failed. ***");
		return FALSE;
	}

	bool bLockVertexBufferResult = m_pVertexBuffer->Lock(0, 0, (VOID**) &pVertex, 0);
	if (bLockVertexBufferResult != true) 
	{
		WriteToLog("*** Failure in Setup: Lock on vertex buffer failed. ***");
		m_pVertexBuffer->Release();
		return FALSE;		
	}

	*pVertex++ = Vertex( -1.0f, 1.0f, 0.0f, 0.0f, 0.0f );
	*pVertex++ = Vertex( 1.0f, 1.0f, 0.0f, 1.0f, 0.0f );
	*pVertex++ = Vertex( 1.0f, -1.0f, 0.0f, 1.0f, 1.0f );
	*pVertex++ = Vertex( -1.0f, -1.0f, 0.0f, 0.0f, 1.0f );

	bool bUnlockVertexBufferResult = m_pVertexBuffer->Unlock();
	if (bUnlockVertexBufferResult != true)
	{
		WriteToLog("*** Failure in Setup: Unlock on vertex buffer failed. ***");
		return FALSE;
	}


	bool bSetStreamSourceResult = SetStreamSource(0, m_pVertexBuffer, sizeof(Vertex));
	if (bSetStreamSourceResult != true)
	{
		WriteToLog("*** Failure in Setup: SetStreamSource failed. ***");

		m_pVertexBuffer->Release();

		return FALSE;		
	}

	bool bSetVertexFormatResult = SetFVF(fvf);
	if (bSetVertexFormatResult != true)
	{
		WriteToLog("*** Failure in Setup: SetFVF failed. ***");

		m_pVertexBuffer->Release();

		return FALSE;		
	}

	bool bCreateTextureResult = D3DXCreateTextureFromResourceA(NULL, MAKEINTRESOURCE(IDB_BITMAP1), &m_pTexture);
	if (bCreateTextureResult != true)
	{
		WriteToLog("*** Failure in Setup: D3DXCreateTextureFromResource failed. ***");

		m_pVertexBuffer->Release();

		return FALSE;		
	}

	bool bSetTextureResult = SetTexture(0, m_pTexture);
	if (bSetTextureResult != true)
	{
		WriteToLog("*** Failure in Setup: SetTexture failed. ***");

		m_pTexture->Release();
		m_pVertexBuffer->Release();

		return FALSE;		
	}

	return CGetRenderTargetData::Setup();
}


UINT CBackBuffer::TestInitialize()
{
	if ( m_bRunInvalid )
	{
		m_szTestName = _T("BBSURAPI");

		setDomain( "OffscreenPlainSurface/Format", BackBufferFmts() );
		setDomain( "OffscreenPlainSurface/Pool", All() );
	}
	else
	{
		setDomain( "OffscreenPlainSurface/Pool", Eq<DWORD>( POOL_SYSTEMMEM ) );
	}

	unsigned nCount = cycleCount();

	SetTestRange(1, nCount );

	return CGetRenderTargetData::TestInitialize();
}

bool CBackBuffer::ClearFrame(void)
{
	return CGetRenderTargetData::ClearFrame();
}

bool CBackBuffer::ExecuteTest(UINT nTest)
{
	HRESULT hr = E_FAIL;

	setState( nTest - 1 );

	if ( !m_bRunInvalid )
	{
		CAutoPtr<CnSurface> pBackBuffer;
		bool bGetBackBufferResult = GetBackBuffer(0, 0, BACKBUFFER_TYPE_MONO, &pBackBuffer);
		if (bGetBackBufferResult != true)
		{
			WriteToLog("*** Failure in ExecuteTest: GetBackBuffer failed. ***");
			return FALSE;
		}

		CAutoPtr<CSurface> pSurface;
		bool bGetSurfaceResult = pBackBuffer->GetSurface(&pSurface, 0);
		if (bGetSurfaceResult != true)
		{
			WriteToLog("*** Failure in ExecuteTest: GetSurface failed. ***");
			return FALSE;
		}

		SURFACEDESC desc;
		hr = pSurface->GetDesc(&desc);
		if (FAILED(hr)) 
		{
			WriteToLog("*** Failure in ExecuteTest: Unexpected error 0x%x from GetSurface. ***", hr);
			return FALSE;
		}

		setParameter( "OffscreenPlainSurface/Format", (FMT) desc.Format.d3dfFormat );
		setParameter( "OffscreenPlainSurface/Dimension", Dimension( desc.dwWidth, desc.dwHeight ) );
	}

	std::string str1;
	getString( "OffscreenPlainSurface", str1 );
	BeginTestCase( str1.c_str(), nTest );

	DWORD pool;
	getParameter( "OffscreenPlainSurface/Pool", pool );
	FMT format;
	getParameter( "OffscreenPlainSurface/Format", format );
	Dimension dimension;
	getParameter( "OffscreenPlainSurface/Dimension", dimension );

	bool bCreatePlainSurfaceResult = CreateOffscreenPlainSurface(dimension.Width, dimension.Height, format, pool, &m_pPlainSurface);
	if (bCreatePlainSurfaceResult != true)
	{
		TCHAR szBuf[80];
		szBuf[0]='\0';

		FmtToString(FORMAT_TEXTURE, &((FORMAT)format), szBuf);

		WriteToLog("*** Failure: CreateOffscreenPlainSurface to create %dx%dx%s surface failed. ***", dimension.Width, dimension.Height, szBuf);
		Fail();
		return FALSE;
	}

	return CGetRenderTargetData::ExecuteTest(nTest);
}


void CBackBuffer::SceneRefresh()
{
	CHRESULT hr;

	try
	{
		hr = BeginScene();

		hr = DrawPrimitive(D3DPT_TRIANGLEFAN, 0, m_nVertices - 2);

		hr = EndScene();
	}
	catch( CHRESULT hr )
	{
		WriteToLog("*** Failure in SceneRefresh 0x%x ***", hr);
		hr.setTestResult(this);
	}

	CGetRenderTargetData::SceneRefresh();
}

bool CBackBuffer::ProcessFrame(void)
{
	HRESULT hr;

	CAutoPtr<CnSurface> pBackBuffer;
	bool bGetBackBufferResult = GetBackBuffer(0, 0, BACKBUFFER_TYPE_MONO, &pBackBuffer);
	if (bGetBackBufferResult != true)
	{
		WriteToLog("*** Failure in ProcessFrame: GetBackBuffer failed. ***");
		Fail();
		return FALSE;
	}

	CAutoPtr<CSurface> pSurfaceBB;
	bool bGetBackBufferSurfaceResult = pBackBuffer->GetSurface(&pSurfaceBB, 0);
	if (bGetBackBufferSurfaceResult != true)
	{
		WriteToLog("*** Failure in ProcessFrame: GetSurface on back buffer failed. ***");
		Fail();
		return FALSE;
	}

	CAutoPtr<CSurface> pSurfacePS;
	bool bGetSurfaceResult = m_pPlainSurface->GetSurface(&pSurfacePS, 0);
	if (bGetSurfaceResult != true)
	{
		WriteToLog("*** Failure in ProcessFrame: GetSurface on plain surface failed. ***");
		Fail();
		return FALSE;
	}

	if ( !GetRenderTargetData(pBackBuffer, m_pPlainSurface) )
	{
		// GetRenderTargetData failed.   Might be OK if this is an invalid paramter test.
		SURFACEDESC descBB;
		hr = pSurfaceBB->GetDesc(&descBB);
		if (FAILED(hr)) 
		{
			WriteToLog("*** Failure in ProcessFrame: Unexpected error 0x%x from GetDesc off of the backbuffer surface. ***", hr);
			Fail();
			return FALSE;
		}

		SURFACEDESC descPS;
		hr = pSurfacePS->GetDesc(&descPS);
		if (FAILED(hr)) 
		{
			WriteToLog("*** Failure in ProcessFrame: Unexpected error 0x%x from GetDesc off of the plain-offscreen surface. ***", hr);
			Fail();
			return FALSE;
		}

		// Check for invalid case.
		if ( descPS.Pool != POOL_SYSTEMMEM || 
				descPS.Format.d3dfFormat != descBB.Format.d3dfFormat ||
				descPS.Width != descBB.Width ||
				descPS.Height != descBB.Height ||
				descBB.MultiSampleType	!= MULTISAMPLE_NONE)
		{
			// Invalid paramter case.  We expect GetRenderTargetData to fail.
			// Eat the error.
			GetLastError();
			WriteToLog("Invalid Parameter case.  GetRenderTargetData failed as expected.", hr);

		}
		else
		{
			WriteToLog("*** Failure: Unexpected error 0x%x from GetRenderTargetData.  Parameters were valid. ***", hr);
			Fail();
			return FALSE;
		}

		GetLastError();
	}
	else
	{
		hr = m_pFramework->GetImageCmp()->CompareImages(pSurfaceBB, pSurfacePS);

		m_fPassPercentage = m_pFramework->GetImageCmp()->GetCompareResult();

		if(m_pFramework->m_DXGLog.InTestCase())
		{
			if(m_fPassPercentage < m_pFramework->GetImageCmp()->GetPassRequired())
			{
/*
				CDevice *pDevice = NULL;
				pSurfaceBB->GetDevice(&pDevice);
				pDevice->D3DXSaveSurfaceToFileA( "C:\\SrcFail.bmp", (D3DXIMAGE_FILEFORMAT)0, pSurfaceBB, NULL, NULL );
				pSurfacePS->GetDevice(&pDevice);
				pDevice->D3DXSaveSurfaceToFileA( "C:\\RefFail.bmp",(D3DXIMAGE_FILEFORMAT)0, pSurfacePS, NULL, NULL );
*/					
				WriteToLog("*** Failure: Surface contents from GetRenderTargetData do not match Backbuffer. ***");
				Fail();
			}
			else
			{
/*
				CDevice *pDevice = NULL;
				pSurfaceBB->GetDevice(&pDevice);
				pDevice->D3DXSaveSurfaceToFileA( "C:\\SrcPass.bmp", (D3DXIMAGE_FILEFORMAT)0, pSurfaceBB, NULL, NULL );
				pSurfacePS->GetDevice(&pDevice);
				pDevice->D3DXSaveSurfaceToFileA( "C:\\RefPass.bmp",(D3DXIMAGE_FILEFORMAT)0, pSurfacePS, NULL, NULL );
*/
				Pass();
			}
		}
	}

	return true; //CGetRenderTargetData::ProcessFrame();
}

void CBackBuffer::UpdateStatus()
{
	CGetRenderTargetData::UpdateStatus();
}

void CBackBuffer::UpdateDetails()
{
	CGetRenderTargetData::UpdateDetails();
}

bool CBackBuffer::DisplayFrame(void)
{
	return CGetRenderTargetData::DisplayFrame();
}

void CBackBuffer::BeginTestCase(LPCTSTR szTestCase, UINT uTestNumber)
{
	m_pPlainSurface = NULL;

	CGetRenderTargetData::BeginTestCase(szTestCase, uTestNumber);
}

void CBackBuffer::EndTestCase(void)
{
	RELEASE( m_pPlainSurface );

	CGetRenderTargetData::EndTestCase();
}

bool CBackBuffer::TestTerminate(void)
{
	return CGetRenderTargetData::TestTerminate();
}

void CBackBuffer::Cleanup()
{
	RELEASE( m_pTexture );

	RELEASE( m_pVertexBuffer );

	CGetRenderTargetData::Cleanup();
}

void CBackBuffer::OnDeviceLost()
{
	CGetRenderTargetData::OnDeviceLost();
}

void CBackBuffer::OnReset()
{
	CGetRenderTargetData::OnReset();
}

bool CBackBuffer::DrawStats(int nHeight, int nWidth, CDevice *pDevice)
{
	return CGetRenderTargetData::DrawStats(nHeight, nWidth, pDevice);
}

bool CBackBuffer::SetDefaultMaterials()
{
	return CGetRenderTargetData::SetDefaultMaterials();
}

bool CBackBuffer::SetDefaultTextures()
{
	return CGetRenderTargetData::SetDefaultTextures();
}

bool CBackBuffer::SetDefaultViewport()
{
	return CGetRenderTargetData::SetDefaultViewport();
}

bool CBackBuffer::SetDefaultLights()
{
	return CGetRenderTargetData::SetDefaultLights();
}

bool CBackBuffer::SetDefaultRenderStates()
{
	return CGetRenderTargetData::SetDefaultRenderStates();
}

bool CBackBuffer::SetDefaultMatrices()
{
	return CGetRenderTargetData::SetDefaultMatrices();
}

void CBackBuffer::CommandLineHelp()
{
	CGetRenderTargetData::CommandLineHelp();
}

void CBackBuffer::SetTestType(TESTTYPE Type)
{
	CGetRenderTargetData::SetTestType(Type);
}

void CBackBuffer::SetTestOptions(DWORD dwTestScenarios, float fMinVersion, bool bPure, bool bRequireRefRast)
{
	CGetRenderTargetData::SetTestOptions(dwTestScenarios, fMinVersion, bPure, bRequireRefRast);
}
