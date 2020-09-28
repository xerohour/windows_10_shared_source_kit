
//////////////////////////////////////////////////////////////////////////
//  Resolve.cpp
//  created:	2007/07/15
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "resolve.h"
#include "FormatDesc.hpp"

//////////////////////////////////////////////////////////////////////////
// CResolve
//////////////////////////////////////////////////////////////////////////

extern DXGI_FORMAT ResolveFormats[];
extern UINT ResolveFormatsSize;
extern DXGI_FORMAT CompactResolveFormats[];
extern UINT CompactResolveFormatsSize;

void CResolveTest::InitTestParameters()
{
	InitMultisampleParameters();
	CTestCaseParameter< DXGI_FORMAT > *pDstFormatParam = AddParameter( "DstFormat", &m_DstFormat );
	CTestCaseParameter< DXGI_FORMAT > *pResolveFormatParam = AddParameter( "ResolveFormat", &m_ResolveFormat );

	CUserValueSet< DXGI_FORMAT > *pResolveFormatValue = new CUserValueSet< DXGI_FORMAT >;
	
	CUserValueSet< DXGI_FORMAT > *pResolveTypedSelectedValue = new CUserValueSet< DXGI_FORMAT >;
	CUserValueSet< DXGI_FORMAT > *pResolveTypelessValue = new CUserValueSet< DXGI_FORMAT >;
	
	for( size_t i = 0; i < ResolveFormatsSize; i++ )
	{
		pResolveFormatValue->AddValue( ResolveFormats[i] );
	}

	for( size_t i = 0; i < CompactResolveFormatsSize; i++ )
	{
		pResolveTypedSelectedValue->AddValue( CompactResolveFormats[i] );
		pResolveTypelessValue->AddValue( GetTypeless( CompactResolveFormats[i] ) );
	}

	// multisample resolve with the same format
	testfactor::RFactor rfRTResolveFormat = AddParameterValueSet( m_pRTFormatParam, pResolveFormatValue);
	testfactor::RFactor rfDstFormat = AddParameterValueSet( pDstFormatParam, pResolveFormatValue );
	testfactor::RFactor rfResolveFormat = AddParameterValueSet( pResolveFormatParam, pResolveFormatValue );
	testfactor::RFactor rfRTVFormat = AddParameterValueSet( m_pRTVFormatParam, pResolveFormatValue );
	
	testfactor::RFactor rfResolveSameFormat = rfRTResolveFormat % rfRTVFormat % rfDstFormat % rfResolveFormat;


	// multisample resolve can be done between compatible formats (having the same typeless format)
	testfactor::RFactor rfRTResolveFormatTyped = AddParameterValueSet( m_pRTFormatParam, pResolveTypedSelectedValue);
	testfactor::RFactor rfRTResolveFormatTypeless = AddParameterValueSet( m_pRTFormatParam, pResolveTypelessValue);
	testfactor::RFactor rfDstFormatTyped = AddParameterValueSet( pDstFormatParam, pResolveTypedSelectedValue );
	testfactor::RFactor rfDstFormatTypeless = AddParameterValueSet( pDstFormatParam, pResolveTypelessValue );
	// resolve format must be a typed format
	testfactor::RFactor rfResolveFormatTyped = AddParameterValueSet( pResolveFormatParam, pResolveTypedSelectedValue );
	testfactor::RFactor rfRTVFormatSelected = AddParameterValueSet( m_pRTVFormatParam, pResolveTypedSelectedValue );

	testfactor::RFactor rfResolveCompatibleFormat =  
		(
			rfRTResolveFormatTyped % rfDstFormatTyped % rfResolveFormatTyped +			// T, T, T
			rfRTResolveFormatTyped % rfDstFormatTypeless % rfResolveFormatTyped +		// T, L, T
			rfRTResolveFormatTypeless % rfDstFormatTyped % rfResolveFormatTyped +		// L, T, T
			rfRTResolveFormatTypeless % rfDstFormatTypeless % rfResolveFormatTyped		// L, L, T
		) % rfRTVFormatSelected;

	SetRootTestFactor ( ( ( rfResolveSameFormat + rfResolveCompatibleFormat ) % m_rfPixPos ) * m_rfSampleDesc );
}

TEST_RESULT CResolveTest::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;
	UINT caps;

	tRes = SetFormatAndQualityLevel( false );
	if ( tRes != RESULT_PASS )
	{
		return tRes;
	}

	// Multisample Resolve is optional for some formats
	hr = m_pDevice->CheckFormatSupport( m_RTVFormat, &caps );
	if( FAILED( hr ) )
	{
		// If the driver doesn't support the 11.1 DDI then it might fail for some formats
		if( !GetFramework()->Is11_1DDISupported( m_pDevice ) &&			// Check to see if the driver supports the 11.1 DDI
			CD3D10FormatHelper_Test::Is16BitBGRFormat( m_RTVFormat ) )	// The 16 bit formats (B5G6R5, etc...) are only required to pass CheckFormatSupport on the 11.1 DDI
		{
			WriteToLog( _T( "CResolveTest::SetupTestCase(): CheckFormatSupport() failed. However the driver doesn't support the D3D11.1 DDI, so this is OK for the 16-bit formats. " ) );
			return RESULT_SKIP;
		}

		WriteToLog( _T( "CResolveTest::SetupTestCase(): CheckFormatSupport() failed." ) );
		return RESULT_FAIL;
	}

	if ( !(caps & D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE) )
	{
		WriteToLog( _T( "CResolveTest::SetupTestCase(): Format does not support D3D11_FORMAT_SUPPORT_MULTISAMPLE_RESOLVE." ) );
		return RESULT_SKIP;
	}

	try 
	{
		SetupD3D();

		D3D11_TEXTURE2D_DESC rtdesc;
		GetFramework()->GetRTTexture()->GetDesc( &rtdesc );

		D3D11_TEXTURE2D_DESC tex2ddesc;
		tex2ddesc.ArraySize = 1;
		tex2ddesc.BindFlags = 0;	
		tex2ddesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		tex2ddesc.Format = m_DstFormat;
		tex2ddesc.Height = rtdesc.Height;
		tex2ddesc.Width = rtdesc.Width;
		tex2ddesc.MiscFlags = 0;
		tex2ddesc.MipLevels = 1;
		tex2ddesc.SampleDesc.Count = 1;
		tex2ddesc.SampleDesc.Quality = 0;
		tex2ddesc.Usage = D3D11_USAGE_STAGING;
		if ( FAILED( hr = GetFramework()->CreateTexture2DTiledOptional( &tex2ddesc, NULL, &m_pRTStaging )) )
			throw TRException("CreateTexture2D failed for m_pRTResolve.", hr, RESULT_FAIL);

		tex2ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		tex2ddesc.CPUAccessFlags = 0;
		tex2ddesc.Usage = D3D11_USAGE_DEFAULT;
		if ( FAILED( hr = GetFramework()->CreateTexture2DTiledOptional( &tex2ddesc, NULL, &m_pRTResolve )) )
			throw TRException("CreateTexture2D failed for m_pRTResolve.", hr, RESULT_FAIL);
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Samples::SetupTestCase()] ");
		if (tRes == RESULT_FAIL)  // no log on skip
			WriteToLog( exc.GetError().c_str() );
	}
	
	return tRes;
}


TEST_RESULT CResolveTest::ExecuteTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	try 
	{
		m_uVertexCount = 0;
		CoverPixel( );

		// clear
		FLOAT ClearColor[4] = { 0, 0, 0, 0 };
		m_pDeviceContext->ClearRenderTargetView( m_pRTView, ClearColor );

		// render
		ResetStates();
		m_pDeviceContext->Draw(m_uVertexCount, 0);

		//
		// check resulting pixel
		//

		// resolve and copy 
		m_pDeviceContext->ResolveSubresource( m_pRTResolve, 0, GetFramework()->GetRTTexture(), 0, m_ResolveFormat );
		m_pDeviceContext->CopySubresourceRegion( m_pRTStaging, 0, 0, 0, 0, m_pRTResolve, 0, &m_PixBox );

		if ( FAILED( hr = ExecuteEffectiveContext() ) )
			throw TRException("ExecuteEffectiveContext failed.", hr, RESULT_FAIL);
		
		// map
		D3D11_MAPPED_SUBRESOURCE texmap;
		if( FAILED( hr = GetImmediateContext()->Map( m_pRTStaging, 0, D3D11_MAP_READ, 0, &texmap ) ) )
			throw TRException("m_pRTTexture2DCopy->Map() failed.", hr, RESULT_FAIL);

		UINT uRedBits, uMask, uElementBits;

		// pull out the red channel
		uElementBits = GetBitsPerElement( m_ResolveFormat );
		uRedBits = GetBitsPerComponent( 0, m_ResolveFormat );
		uMask = 2 * (1 << (uRedBits -1)) - 1;

		BOOL bResult;
		bResult = (long)(((UINT*)texmap.pData)[0] & uMask);

		// unmap
		GetImmediateContext()->Unmap(m_pRTStaging,0);

		// validate
		if (!bResult)
			throw TRException("Point drawn on pixel center and after using multisample resolve the pixel is still the clear color.");

		ResetStates();
		Present();
	}
	catch (TRException& exc)
	{
		tRes = exc.GetResult();
		exc.Prepend("[CMultisampleTest_Resolve::ExecuteTestCase()] ");
		WriteToLog( exc.GetError().c_str() );
	}

	return tRes;
}

void CResolveTest::CleanupTestCase()
{
	CMultisampleTest::CleanupD3D();
	SAFE_RELEASE( m_pRTStaging );
	SAFE_RELEASE( m_pRTResolve );
}
