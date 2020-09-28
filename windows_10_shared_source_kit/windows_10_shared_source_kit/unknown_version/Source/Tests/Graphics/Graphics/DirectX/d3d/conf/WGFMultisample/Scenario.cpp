//////////////////////////////////////////////////////////////////////////
//  Scenario.cpp
//  created:	2006/09/15
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "Scenario.h"
#include <dxgi.h>
#include <d3dx10.h>

tstring __stdcall ToString( SCENARIO Scenario )
{
	switch( Scenario )
	{
	case Array_Resolve:
		return tstring( _T( "Array_Resolve" ) );
		break;

	case MRT_Resolve:
		return tstring( _T( "MRT_Resolve" ) );
		break;

	case Re_Resolve:
		return tstring( _T( "Re_Resolve" ) );
		break;

	case Load_Resolve:
		return tstring( _T( "Load_Resolve" ) );
		break;

	case Resolve_Load:
		return tstring( _T( "Resolve_Load" ) );
		break;

	case Fullscreen_Resolve:
		return tstring( _T( "Fullscreen_Resolve" ) );
		break;

	case Fullscreen_Present_Resolve:
		return tstring( _T( "Fullscreen_Present_Resolve" ) );
		break;

	case Fullscreen_Resolve_Present:
		return tstring( _T( "Fullscreen_Resolve_Present" ) );
		break;

	case Fullscreen_Present_Load:
		return tstring( _T( "Fullscreen_Present_Load" ) );
		break;

	case Fullscreen_Load_Present:
		return tstring( _T( "Fullscreen_Load_Present" ) );
		break;

	case Cast_Resolve:
		return tstring( _T( "Cast_Resolve" ) );
		break;

	case NonPow2_Resolve:
		return tstring( _T( "NonPow2_Resolve" ) );

	default:
		return tstring( _T( "" ) );
		break;
	}
}

bool __stdcall FromString( SCENARIO *pScenario, const tstring &str )
{
	if( tcistring( str ) == _T( "Array_Resolve" ) )
	{
		*pScenario = Array_Resolve;
		return true;
	}

	if( tcistring( str ) == _T( "MRT_Resolve" ) )
	{
		*pScenario = MRT_Resolve;
		return true;
	}

	if( tcistring( str ) == _T( "Re_Resolve" ) )
	{
		*pScenario = Re_Resolve;
		return true;
	}

	if( tcistring( str ) == _T( "Load_Resolve" ) )
	{
		*pScenario = Load_Resolve;
		return true;
	}

	if( tcistring( str ) == _T( "Resolve_Load" ) )
	{
		*pScenario = Resolve_Load;
		return true;
	}

	if( tcistring( str ) == _T( "Fullscreen_Resolve" ) )
	{
		*pScenario = Fullscreen_Resolve;
		return true;
	}

	if( tcistring( str ) == _T( "Fullscreen_Present_Resolve" ) )
	{
		*pScenario = Fullscreen_Present_Resolve;
		return true;
	}

	if( tcistring( str ) == _T( "Fullscreen_Resolve_Present" ) )
	{
		*pScenario = Fullscreen_Resolve_Present;
		return true;
	}

	if( tcistring( str ) == _T( "Fullscreen_Present_Load" ) )
	{
		*pScenario = Fullscreen_Present_Load;
		return true;
	}

	if( tcistring( str ) == _T( "Fullscreen_Load_Present" ) )
	{
		*pScenario = Fullscreen_Load_Present;
		return true;
	}

	if( tcistring( str ) == _T( "Cast_Resolve" ) )
	{
		*pScenario = Cast_Resolve;
		return true;
	}

	if( tcistring( str ) == _T( "NonPow2_Resolve" ) )
	{
		*pScenario = NonPow2_Resolve;
		return true;
	}

	return false;
}
	
void CMultisampleTest_Scenario::InitTestParameters()
{
	DecodeTestPath();

	CUserValueSet< SCENARIO >	*pVSScenario = new CUserValueSet< SCENARIO >();

	pVSScenario->AddValue( Array_Resolve );
	pVSScenario->AddValue( MRT_Resolve );
	pVSScenario->AddValue( Re_Resolve );
	pVSScenario->AddValue( Load_Resolve );
	pVSScenario->AddValue( Resolve_Load );
	pVSScenario->AddValue( Fullscreen_Resolve );
	pVSScenario->AddValue( Fullscreen_Present_Resolve );
	pVSScenario->AddValue( Fullscreen_Resolve_Present );
	pVSScenario->AddValue( Fullscreen_Present_Load );
	pVSScenario->AddValue( Fullscreen_Load_Present );

	pVSScenario->AddValue( Cast_Resolve );
	pVSScenario->AddValue( NonPow2_Resolve );

	testfactor::RFactor rfScenario = AddParameter( _T("Scenario"), &m_Scenario, pVSScenario );

	g_TestApp.m_RenderTargetOptions.SwapChainDesc.SampleDesc.Count = m_uSampleCount;	
	g_TestApp.m_RenderTargetOptions.SwapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_CENTERED;
	g_TestApp.m_RenderTargetOptions.SwapChainDesc.BufferUsage = DXGI_USAGE_SHADER_INPUT | DXGI_USAGE_RENDER_TARGET_OUTPUT;
	g_TestApp.m_RenderTargetOptions.SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	g_TestApp.m_RenderTargetOptions.SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;

	CTestCaseParameter< UINT > *pSIndex = AddParameter( _T( "SampleIndex" ), &m_uSampleIndex );
	pSIndex->SetDefaultValue( 0 );

	testfactor::RFactor sindex = AddParameterValueSet( pSIndex, new CRangeValueSet< UINT > (0, m_uSampleCount-1, 1) );

	if ( m_uSampleCount > 1 )
		SetRootTestFactor ( sindex*rfScenario );
	else
		SetRootTestFactor ( rfScenario );
}
	
TEST_RESULT CMultisampleTest_Scenario::PreSetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;

	g_TestApp.m_RenderTargetOptions.SwapChainDesc.SampleDesc.Quality = 0;
	g_TestApp.m_RenderTargetOptions.SwapChainDesc.BufferDesc.Format = m_RTFormat;

	switch ( m_Scenario )
	{
	case NonPow2_Resolve:
		m_bUseSwapChain = false;
		g_TestApp.m_RenderTargetOptions.SwapChainDesc.Windowed = true;
		m_RTHeight = 3;
		m_RTWidth = 277;

		break;

	case Array_Resolve:
		m_bUseSwapChain = false;

		g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.ArraySize = 10;
		g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
		g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.Texture2DMSArray.ArraySize = 10;
		g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.Texture2DMSArray.FirstArraySlice = 0;
		g_TestApp.m_RenderTargetOptions.SwapChainDesc.Windowed = true;
		m_bUseSwapChain = false;

		break;

	case MRT_Resolve:
		g_TestApp.m_RenderTargetOptions.SwapChainDesc.Windowed = true;
		m_bUseSwapChain = false;
		
		break;

	case Cast_Resolve:
		g_TestApp.m_RenderTargetOptions.RenderTargetTextureDesc.Format = GetTypeless( m_RTFormat );
		g_TestApp.m_RenderTargetOptions.RenderTargetViewDesc.Format = m_RTFormat;
		g_TestApp.m_RenderTargetOptions.SwapChainDesc.Windowed = true;
		m_bUseSwapChain = false;

		break;

	case Fullscreen_Resolve:
	case Fullscreen_Present_Resolve:
	case Fullscreen_Resolve_Present:
	case Fullscreen_Present_Load:
	case Fullscreen_Load_Present:
		g_TestApp.m_RenderTargetOptions.SwapChainDesc.Windowed = false;
		m_bUseSwapChain = true;
		m_RTWidth = 640;
		m_RTHeight = 480;

		break;

	default:
		g_TestApp.m_RenderTargetOptions.SwapChainDesc.Windowed = true;
		m_bUseSwapChain = true;

		break;
	}

	return tRes;
}

TEST_RESULT CMultisampleTest_Scenario::SetupTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	ID3D10Blob *pGSBuf = NULL;

	m_vPixPos.x = m_RTWidth /2;
	m_vPixPos.y = m_RTHeight /2;

	try 
	{
		SetupD3D();
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Scenario::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}
	POINT pix = {m_PixBox.left, m_PixBox.top};

	if ( RESULT_PASS == tRes )
	{
		switch ( m_Scenario )
		{
		case Array_Resolve:
			//Set up GS
			if ( FAILED( D3DX10CompileFromResource(NULL, "gs_Array.gsh", NULL, NULL, NULL, "main", "gs_4_0", 0, 0, NULL, &pGSBuf, NULL, NULL ) ) )
			{
				WriteToLog("CompileShaderFromResource Failed.");
				tRes = RESULT_FAIL;
				goto FAILED;
			}

			if ( FAILED( m_pDevice->CreateGeometryShader( (DWORD*) pGSBuf->GetBufferPointer(), pGSBuf->GetBufferSize(), NULL, &m_pGS ) ) )
			{
				WriteToLog("CreateGeometryShader Failed.");
				SAFE_RELEASE( pGSBuf );
				tRes = RESULT_FAIL;
			}
			SAFE_RELEASE( pGSBuf );
			
			break;

		default:
			break;
		}
	}

	if ( RESULT_PASS == tRes )
	{
		m_Finder.Cleanup(); //Prevent leak
		tRes = m_Finder.Setup( GetFramework(), m_pRTView, m_pRTTexture2D, pix );
	}
	
	return tRes;

FAILED:
	SAFE_RELEASE( pGSBuf );
	SAFE_RELEASE( m_pGS );

	return tRes;
}
TEST_RESULT CMultisampleTest_Scenario::ExecuteTestCase()
{
	BOOL bResult;
	TEST_RESULT tRes = RESULT_PASS;

	try
	{
		if( RESULT_FAIL == m_Finder.FindSamples( ) )
			throw TRException("FindSamples failed.");

		ResetStates();

		CoverSample( m_uSampleIndex );
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Scenario::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	if ( Array_Resolve == m_Scenario )
		m_pDeviceContext->GSSetShader( m_pGS, NULL, 0 );

	if ( RESULT_PASS != tRes )
		return tRes;

	FLOAT ClearColor[4] = { 0, 0, 0, 0 };

	m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );
	
	m_pDeviceContext->Draw( m_uVertexCount, 0 );

	switch( m_Scenario )
	{
	case NonPow2_Resolve:
		tRes = m_Finder.TestPixelNonZero( bResult );

		if ( RESULT_PASS != tRes )
		{
			WriteToLog("Failed to acquire pixel color.");
		}
		else if ( bResult == 0 )
		{
			WriteToLog("Pixel black.");
			tRes = RESULT_FAIL;
		}

		break;

	case Array_Resolve:
		for ( UINT i = 0; i < 10; ++i )
		{
			TEST_RESULT tRTemp = RESULT_PASS;
			tRTemp = m_Finder.TestPixelNonZero( bResult, true, i );
			
			if ( RESULT_PASS != tRTemp )
			{
				WriteToLog("Failed to acquire pixel color in render target %d.", i);
				tRes = RESULT_FAIL;
			}
			else if ( bResult == 0 )
			{
				WriteToLog("Pixel black in render target %d.", i);
				tRes = RESULT_FAIL;
			}
		}
		
		break;
		
	case MRT_Resolve:		
		break;

	case Re_Resolve:
		tRes = m_Finder.TestPixelNonZero( bResult );

		if ( RESULT_PASS != tRes )
		{
			WriteToLog("Failed to acquire pixel color in first resolve.");
		}
		else if ( bResult == 0 )
		{
			WriteToLog("Pixel black from first resolve.");
			tRes = RESULT_FAIL;

			break;
		}

		tRes = m_Finder.TestPixelNonZero( bResult );

		if ( RESULT_PASS != tRes )
		{
			WriteToLog("Failed to acquire pixel color in second resolve.");
		}
		else if ( bResult == 0 )
		{
			WriteToLog("Pixel black from second resolve.");
			tRes = RESULT_FAIL;
		}

		break;

	case Load_Resolve:
		break;
	case Resolve_Load:
		break;

	case Fullscreen_Resolve:
		tRes = m_Finder.TestPixelNonZero( bResult );

		if ( RESULT_PASS != tRes )
		{
			WriteToLog("Failed to acquire pixel color.");
		}
		else if ( bResult == 0 )
		{
			WriteToLog("Pixel black.");
			tRes = RESULT_FAIL;
		}

		break;

	case Fullscreen_Present_Resolve:
		g_TestApp.Present( NULL, NULL, NULL, NULL, NULL, NULL );

		tRes = m_Finder.TestPixelNonZero( bResult );

		if ( RESULT_PASS != tRes )
		{
			WriteToLog("Failed to acquire pixel color.");
		}
		else if ( bResult == 0 )
		{
			WriteToLog("Pixel black.");
			tRes = RESULT_FAIL;
		}

		break;

	case Fullscreen_Resolve_Present:
		break;
	case Fullscreen_Present_Load:
		break;
	case Fullscreen_Load_Present:
		break;

	case Cast_Resolve:

		break;
		/*Resolve to format other than that of the view
		m_pDevice->ResolveSubresource( m_pRTTexture2DResolve, 0, m_pRTTexture2D, 4, DXGI_FORMAT_R16G16B16A16_UNORM );
		m_pDevice->CopySubresourceRegion( m_pRTTexture2DCopy, 0, 0, 0, 0, m_pRTTexture2DResolve, 0, &m_PixBox );

		GetPixelColor( PixColor );
		if ( PixColor == 0.0f )
		{
			WriteToLog("Pixel black.");
			tRes = RESULT_FAIL;
		}

		break;*/

	default:
		break;
	}

	return tRes;
}
BOOL CMultisampleTest_Scenario::Validate( BOOL bResult )
{
	BOOL bRes = 1;

	return bRes;
}
void CMultisampleTest_Scenario::CleanupTestCase()
{
	if ( m_pDevice )
	{
		if ( Array_Resolve == m_Scenario )
		{
			m_pDeviceContext->GSSetShader(NULL, NULL, 0);
			SAFE_RELEASE(m_pGS);
		}
		if ( m_bUseSwapChain )
		{
			m_pRTTexture2D = NULL;
			m_pRTView = NULL;
		}
	}
	
	CMultisampleTest::CleanupD3D();
}