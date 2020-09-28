#pragma once

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <dxmini.h>
#include <d3dkmddi.h>
#include <d3dkmthk.h>
#include "AdapterUtil.h"
#include <initguid.h>
#include "D3D11Test.h"
#include "WGFTestCore.h"
#include "FormatHelper.h"
#include "d3d11internaltest.h"
				
#define	FORMAT_RT		DXGI_FORMAT_R32G32B32A32_FLOAT
#define PRIMITIVE_SIZE	32

typedef struct TEST_COLOR
{
	DOUBLE r;
	DOUBLE g;
	DOUBLE b;
	DOUBLE a;
} 	TEST_COLOR;

class CAsyncConfTest : public CD3D11Test
{
public:
	CAsyncConfTest()
		: m_pDebug(NULL)
		, m_nPrimitiveSize( PRIMITIVE_SIZE )
		, m_nIndexCount( 0 )
		, m_pVS( NULL )
		, m_pGS( NULL )
		, m_pPS( NULL )
		, m_FormatRT( FORMAT_RT )
		, m_pTextureRT( NULL )
		, m_pTextureRTCopy( NULL )
		, m_pTextureDepth( NULL )
		, m_pRTView( NULL )
		, m_pDSView( NULL )
		, m_pRSEnabled( NULL )
		, m_pDSSEnabled( NULL )
		, m_pVertexBuffer( NULL )
		, m_pIndexBuffer( NULL )
		, m_pConstBuffer( NULL )
		, m_pSOBuffer( NULL )
		, m_pLayout( NULL )
		, m_bGpuJobSetup( false )
	{
		m_bDeferredREF = GetFramework()->IsDeferredContextUsed() && GetFramework()->m_D3DOptions.DriverType == D3D_DRIVER_TYPE_REFERENCE;
	}
	virtual TEST_RESULT Setup();
	virtual void Cleanup( );

protected:
	ID3D11Debug			*m_pDebug;

	bool					m_bDeferredREF;
	int						m_nPrimitiveSize;
	int						m_nIndexCount;
	DXGI_FORMAT				m_FormatRT;

	ID3D11VertexShader			*m_pVS;
	ID3D11GeometryShader			*m_pGS;
	ID3D11PixelShader			*m_pPS;
	ID3D11Texture2D				*m_pTextureRT;
	ID3D11Texture2D				*m_pTextureRTCopy;
	ID3D11Texture2D				*m_pTextureDepth;
	ID3D11RenderTargetView		*m_pRTView;
	ID3D11DepthStencilView		*m_pDSView;
	ID3D11RasterizerState		*m_pRSEnabled;
	ID3D11DepthStencilState		*m_pDSSEnabled;
	ID3D11Buffer					*m_pVertexBuffer;
	ID3D11Buffer					*m_pIndexBuffer;
	ID3D11Buffer					*m_pConstBuffer;
	ID3D11Buffer					*m_pSOBuffer;
	ID3D11InputLayout			*m_pLayout;

	HRESULT			CpuBoundJob();
	HRESULT			GpuBoundJob(bool bExtraGPUWork);
	HRESULT			SetupGpuJob();
	HRESULT			ReadBackBufferColor(int x, int y, TEST_COLOR* pColor);
	bool			TestColorsSame(const TEST_COLOR& c1, const TEST_COLOR& c2, double delta);

protected:
	struct TVERTEX
	{
		float vPos[3];
		DWORD Color;
		
	};
	struct SOVERTEX
	{
		float vPos[4];
	};

private:
	TEST_RESULT SetupDirect3D( );
	TEST_RESULT SetupGeometry( bool bIsLevel9 );
	TEST_RESULT SetupRenderTarget( bool bIsLevel9 );
	TEST_RESULT SetupShaders( bool bIsLevel9 );
	void CleanupDirect3D( );

	TEST_RESULT SetupTestCase();

	bool m_bGpuJobSetup;
};
	
class CQueryConfTest : public CAsyncConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	bool CallProcess(LPSTR pszwCommand);
	TEST_RESULT TimingTestCase();
	TEST_RESULT OcclusionTestCase();
	TEST_RESULT StatsTestCase();

		template<typename T>
		TEST_RESULT VerifyStats(const T& value1, const T& value2);

	bool				m_bCanBegin;
	bool				m_bNestQueries;
	bool				m_bOverlapQueries;
	int                 		m_nPrefixGpuIterations;
	int                 		m_nSuffixGpuIterations;
	bool				m_bDoGpuJob;
	D3D11_QUERY_DESC		m_QueryDesc;
};

class CPredicateConfTest : public CAsyncConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	// reads the first pixel from a USAGE_DEFAULT texture 2D
	TEST_RESULT ReadTexture2DColor(ID3D11Texture2D* pTex, UINT uSubresource, void* Out);

	virtual TEST_RESULT InitializeForGpuJob() = 0;
	virtual void CleanupFromGpuJob() = 0;
	virtual void DoPredicatedGpuJob(UINT uIdx) = 0;
	virtual TEST_RESULT VerifyResults(UINT uIdx) = 0;

	bool				m_bHint;
	bool				m_bDoGpuJob;
	bool				m_bPredValue;
	D3D11_QUERY_DESC    m_PredDesc;
};

class CClearRTVPredicateConfTest : public CPredicateConfTest
{
public:
	CClearRTVPredicateConfTest();
	~CClearRTVPredicateConfTest();

protected:
	virtual TEST_RESULT InitializeForGpuJob();
	virtual void CleanupFromGpuJob();
	virtual void DoPredicatedGpuJob(UINT uIdx);
	virtual TEST_RESULT VerifyResults(UINT uIdx);

private:
	FLOAT_COLOR GetClearColor(UINT uIdx);
	ID3D11Texture2D*        m_pRT;
	ID3D11RenderTargetView* m_pRTV;
};

class CClearDSVPredicateConfTest : public CPredicateConfTest
{
public:
	CClearDSVPredicateConfTest();
	~CClearDSVPredicateConfTest();

protected:
	virtual TEST_RESULT InitializeForGpuJob();
	virtual void CleanupFromGpuJob();
	virtual void DoPredicatedGpuJob(UINT uIdx);
	virtual TEST_RESULT VerifyResults(UINT uIdx);

private:
	float GetClearDepth(UINT uIdx);
	ID3D11Texture2D*        m_pDepthBuffer;
	ID3D11DepthStencilView* m_pDSV;
};

class CCopyResourcePredicateConfTest : public CPredicateConfTest
{
public:
	CCopyResourcePredicateConfTest();
	~CCopyResourcePredicateConfTest();

protected:
	virtual TEST_RESULT InitializeForGpuJob();
	virtual void CleanupFromGpuJob();
	virtual void DoPredicatedGpuJob(UINT uIdx);
	virtual TEST_RESULT VerifyResults(UINT uIdx);

	DWORD GetColor(UINT uIdx);
	ID3D11Texture2D* m_pSrc0;
	ID3D11Texture2D* m_pSrc1;
	ID3D11Texture2D* m_pDst;
};

class CCopySubresourceRegionPredicateConfTest : public CCopyResourcePredicateConfTest
{
protected:
	virtual void DoPredicatedGpuJob(UINT uIdx);
};

class CUpdateSubresourcePredicateConfTest : public CPredicateConfTest
{
public:
	CUpdateSubresourcePredicateConfTest();
	~CUpdateSubresourcePredicateConfTest();

protected:
	virtual TEST_RESULT InitializeForGpuJob();
	virtual void CleanupFromGpuJob();
	virtual void DoPredicatedGpuJob(UINT uIdx);
	virtual TEST_RESULT VerifyResults(UINT uIdx);

	DWORD GetColor(UINT uIdx);
	ID3D11Texture2D* m_pDst;
};

class CResolveSubresourcePredicateConfTest : public CPredicateConfTest
{
public:
	CResolveSubresourcePredicateConfTest();
	~CResolveSubresourcePredicateConfTest();

protected:
	virtual TEST_RESULT InitializeForGpuJob();
	virtual void CleanupFromGpuJob();
	virtual void DoPredicatedGpuJob(UINT uIdx);
	virtual TEST_RESULT VerifyResults(UINT uIdx);

	DWORD GetColor(UINT uIdx);
	ID3D11Texture2D* m_pSrc0;
	ID3D11Texture2D* m_pSrc1;
	ID3D11Texture2D* m_pDst;
};

class CGenerateMipsPredicateConfTest : public CPredicateConfTest
{
public:
	CGenerateMipsPredicateConfTest();
	~CGenerateMipsPredicateConfTest();

protected:
	virtual TEST_RESULT InitializeForGpuJob();
	virtual void CleanupFromGpuJob();
	virtual void DoPredicatedGpuJob(UINT uIdx);
	virtual TEST_RESULT VerifyResults(UINT uIdx);

	DWORD GetColor(UINT uIdx);
	ID3D11Texture2D* m_pTex;
	ID3D11ShaderResourceView* m_pSRV;
};

class CDrawBasePredicateConfTest : public CPredicateConfTest
{
public:
	CDrawBasePredicateConfTest();
	~CDrawBasePredicateConfTest();

protected:
	virtual TEST_RESULT InitializeForGpuJob();
	virtual void CleanupFromGpuJob();
	virtual void DoPredicatedGpuJob(UINT uIdx);
	virtual TEST_RESULT VerifyResults(UINT uIdx);

	DWORD GetColor(UINT uIdx);
	ID3D11Buffer* m_pVertexBuffer;
	ID3D11Buffer* m_pIndexBuffer;

	static const UINT VERTEX_COUNT = 8;
	static const UINT INDEX_COUNT  = 4;

	virtual void FillVertexBuffer(ID3D11Buffer* pBuffer, const TVERTEX* pVertices);
	virtual void Draw() = 0;
};

class CDrawPredicateConfTest : public CDrawBasePredicateConfTest
{
protected:
	virtual void Draw();
};

class CDrawInstancedPredicateConfTest : public CDrawBasePredicateConfTest
{
protected:
	virtual void Draw();
};

class CDrawIndexedPredicateConfTest : public CDrawBasePredicateConfTest
{
protected:
	virtual void Draw();
};

class CDrawIndexedInstancedPredicateConfTest : public CDrawBasePredicateConfTest
{
protected:
	virtual void Draw();
};

class CDrawAutoPredicateConfTest : public CDrawBasePredicateConfTest
{
protected:
	virtual void FillVertexBuffer(ID3D11Buffer* pBuffer, const TVERTEX* pVertices);
	virtual void Draw();
};

class CCounterConfTest : public CAsyncConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

	TEST_RESULT Setup();
	void Cleanup();

protected:
	bool			m_bNestQueries;
	bool			m_bOverlapQueries;
	int				m_nGpuJobs;
	int				m_nDeviceDependentNbr;
	D3D11_COUNTER		m_CounterName;
};

class COverAllocTest : public CAsyncConfTest
{
public:
	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	bool				m_bOverAlloc;
	bool				m_bOverAllocIssue;
	D3D11_QUERY_DESC		m_QueryDesc;
};

class CAsyncConfApp : public CD3D11TestFramework
{
public:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	void InitOptionVariables();
	bool InitTestGroups();

	UINT GetDefaultQueryTimeout() { return 30000; }
};
extern CAsyncConfApp  g_App;




// Given the results of query1 and query2 and the values of m_bOverlapQueries and m_bDoGpuJob
// check the statistics returned by the queries for zero.  Fail as approriate.
// This function is used for pipeline stats and so output stats
template<typename T>
TEST_RESULT CQueryConfTest::VerifyStats(const T& value1, const T& value2)
{
	TEST_RESULT tRes = RESULT_PASS;

	if (!m_bOverlapQueries)
	{
		if (value1 == value2)
		{
			WriteToLog( _T("Statistics values are identical.\n") );
			tRes = RESULT_FAIL;
			goto testDone;
		}

		if (m_bDoGpuJob)
		{
			if( value1.IsZero() )
			{
				WriteToLog( _T("Statistics value is zero when it should not be\n") );
				tRes = RESULT_FAIL;
				goto testDone;
			}

			if ( !value2.IsZero() )
			{
				WriteToLog( _T("Statistics value is not zero when it should be\n") );
				tRes = RESULT_FAIL;
				goto testDone;
			}
		}
		else
		{
			if( !value1.IsZero() )
			{
				WriteToLog( _T("Statistics value is not zero when it should be\n") );
				tRes = RESULT_FAIL;
				goto testDone;
			}

			if ( value2.IsZero() )
			{
				WriteToLog( _T("Statistics value is zero when it should not be\n") );
				tRes = RESULT_FAIL;
				goto testDone;
			}
		}
	}
	else
	{
		if ( !(value1 == value2) )
		{
			WriteToLog( _T("Statistics values are different.\n") );
			tRes = RESULT_FAIL;
			goto testDone;
		}

		if (m_bDoGpuJob)
		{
			if( value1.IsZero() )
			{
				WriteToLog( _T("Statistics value is zero when it should not be\n") );
				tRes = RESULT_FAIL;
				goto testDone;
			}
		}
		else
		{
			if( !value1.IsZero() )
			{
				WriteToLog( _T("Statistics value is not zero when it should be\n") );
				tRes = RESULT_FAIL;
				goto testDone;
			}
		}
	}

testDone:
	return tRes;
}
