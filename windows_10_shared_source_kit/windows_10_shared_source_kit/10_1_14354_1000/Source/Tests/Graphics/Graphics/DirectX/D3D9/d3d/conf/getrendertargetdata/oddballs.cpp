#include "getrendertargetdata.h"

#include "CSurface.h"
#include "Utils.h"

COddBalls::COddBalls()
{
	SetTestType( TESTTYPE_CONF );

    m_szTestName = _T("Invalid Parameters");
    m_szCommandKey = _T("grtdob");

	m_Options.D3DOptions.fMinDXVersion = 9.f;
}

bool COddBalls::CapsCheck()
{
    return KeySet( _T( "INV" ) ) ? CD3DTest::CapsCheck() : false;
}

UINT COddBalls::TestInitialize(void)
{
	SetTestRange(1, 6);
	return CD3DTest::TestInitialize();
}

bool COddBalls::ExecuteTest(UINT nTest)
{
	CHRESULT hr;

	static const char *desc[] =
	{
		"NULL, NULL",
		"NULL, osps",
		"rt, NULL",
		"osps, rt",
		"rt, rt",
		"osps, osps"
	};

	BeginTestCase( desc[nTest-1], nTest );

	try
	{
		switch ( nTest )
		{
		case 1:
		{
			hr = FAILED( m_pSrcDevice->GetRenderTargetData( NULL, NULL ) );
			break;
		}
		case 2:
		{
			CAutoPtr< CSurface > pPlainSurface;
			hr = m_pSrcDevice->CreateOffscreenPlainSurface( 640, 480, FMT_R5G6B5, POOL_SYSTEMMEM, &pPlainSurface );
			hr = FAILED( m_pSrcDevice->GetRenderTargetData( NULL, pPlainSurface ) );
			break;
		}
		case 3:
		{
			CAutoPtr< CSurface > pRenderTarget;
			hr = m_pSrcDevice->CreateRenderTarget( 640, 480, FMT_R5G6B5, MULTISAMPLE_NONE, 0, FALSE, &pRenderTarget );
			hr = FAILED( m_pSrcDevice->GetRenderTargetData( pRenderTarget, NULL ) );
			break;
		}
		case 4:
		{
			CAutoPtr< CSurface > pRenderTarget;
			hr = m_pSrcDevice->CreateRenderTarget( 640, 480, FMT_R5G6B5, MULTISAMPLE_NONE, 0, FALSE, &pRenderTarget );
			CAutoPtr< CSurface > pPlainSurface;
			hr = m_pSrcDevice->CreateOffscreenPlainSurface( 640, 480, FMT_R5G6B5, POOL_SYSTEMMEM, &pPlainSurface );
			hr = FAILED( m_pSrcDevice->GetRenderTargetData( pPlainSurface, pRenderTarget ) );
			break;
		}
		case 5:
		{
			CAutoPtr< CSurface > pRenderTarget;
			hr = m_pSrcDevice->CreateRenderTarget( 640, 480, FMT_R5G6B5, MULTISAMPLE_NONE, 0, FALSE, &pRenderTarget );
			hr = FAILED( m_pSrcDevice->GetRenderTargetData( pRenderTarget, pRenderTarget ) );
			break;
		}
		case 6:
		{
			CAutoPtr< CSurface > pPlainSurface;
			hr = m_pSrcDevice->CreateOffscreenPlainSurface( 640, 480, FMT_R5G6B5, POOL_SYSTEMMEM, &pPlainSurface );
			hr = FAILED( m_pSrcDevice->GetRenderTargetData( pPlainSurface, pPlainSurface ) );
			break;
		}
		default:
			Abort();
		}
	}
	catch( CHRESULT hr )
	{
		Fail();
	}

	GetLastError();
	Pass();

	return false; //CD3DTest::ExecuteTest( nTest );
}
