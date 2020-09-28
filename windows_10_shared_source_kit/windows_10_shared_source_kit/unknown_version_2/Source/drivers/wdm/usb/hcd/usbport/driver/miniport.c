			BYTE resultRendered = pDataBytes[ iB ];
				if( resultExpected != resultRendered && (bPass || bLogAll) )
				{
					WriteToLog( _T( "Test case failed for subresource %d, byte %d at (%d,%d) does not match.\n"
						"	Expected: 0x%02x\n"
						"	Actual:	  0x%02x" ), nSubresource, iB, x, y, resultExpected, resultRendered );
					bPass = false;
				}
			}
			pExpected += size;
			pDataBytes += size;
		}

		if( m_ResourceType == D3D_RESOURCE_SCENARIO_TEXTURE3D )
			pDataBytes = (BYTE*) pResultData + uDepthPitch * (depth - 1) + (y + 1) * uRowPitch;
		else
			pDataBytes = (BYTE*) pResultData + (y + 1) * uRowPitch;
	}
	
	delete [] pYUVPlanarReformattedData;
	return bPass;
}

bool CRenderTargetsTest::IsDenorm( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0) && ((uValue & frac) != 0);
}

bool CRenderTargetsTest::IsNaN( float fValue )
{
	UINT32 frac, exp, uValue;

	uValue = *( (UINT32*) &fValue );
	exp = 0x7f800000;
	frac = 0x007fffff;

	return ((uValue & exp) == 0x7f800000) && ((uValue & frac) != 0);
}

DXGI_FORMAT CRenderTargetsTest::GetRTViewFormat( const DXGI_FORMAT &resourceFormat )
{
	/*
		If the format is a video format, return a supported RTV format. If it is not a video format, return the same format that was passed in.
	*/

	if( CD3D10FormatHelper_Test::YUV( resourceFormat ) )
	{
		// The format is a video format, see if it supports RTVs
		std::vector<DXGI_FORMAT> luminanceDataFormats, chrominanceDataFormats;
		if( CFormatHelper::GetRTViewFormatsForVideoResourceFormat( resourceFormat, luminanceDataFormats, chrominanceDataFormats ) )
		{
			// The format is a video format AND supports RTVs, so we need to determine the correct view format...
			// BUGBUG, we will just get luminance coverage for formats that split the views. Even then, we only get the first format.
			assert( luminanceDataFormats.size() >= 1 );
			return luminanceDataFormats[0];
		}
	}

	return resourceFormat;
}

DXGI_FORMAT CRenderTargetsTest::GetSRViewFormat( const DXGI_FORMAT &resourceFormat )
{
	/*
		If the format is a video format, return a supported SRV format. If it is not a video format, return the same format that was passed in.
	*/

	if( CD3D10FormatHelper_Test::YUV( resourceFormat ) )
	{
		// The format is a video format, see if it supports SRVs
		std::vector<DXGI_FORMAT> luminanceDataFormats, chrominanceDataFormats;
		if( CFormatHelper::GetSRViewFormatsForVideoResourceFormat( resourceFormat, luminanceDataFormats, chrominanceDataFormats ) )
		{
			// The format is a video format AND supports SRVs, so we need to determine the correct view format...
			// BUGBUG, we will just get luminance coverage for formats that split the views. Even then, we only get the first format.
			assert( luminanceDataFormats.size() >= 1 );
			return luminanceDataFormats[0];
		}
	}

	return resourceFormat;
}

bool CRenderTargetsTest::GetMipDimensions( UINT uMipSlice, UINT &uWidth, UINT &uHeight, UINT &uDepth )
{
	if( uWidth == 0 || uHeight == 0 || uDepth == 0 )
		return false;

	bool bSmallestMip = false;

	UINT uCurrMip = 0;
	while( uCurrMip < uMipSlice )
	{
		if( bSmallestMip )
			return false;

		if( uWidth > 1 ) uWidth /= 2;
		if( uHeight > 1 ) uHeight /= 2;
		if( uDepth > 1 ) uDepth /= 2;
		uCurrMip++;

		if( uWidth == 1 && uHeight == 1 && uDepth == 1 )
			bSmallestMip = true;
	}

	return true;
}

bool CRenderTargetsTest::WithinOneULP( float fReference, float fOutput )
{
	// WARNING:
	// This function does not correctly handle edge cases involving negative floats, zeros
	// and NaNs. If future test result discrepancies are discovered to have been masked by this function,
	// please start using CompareFloatULP() function in Framework\Core\FloatUtils.cpp which
	// does not suffer from these problems.

	if( fReference == fOutput )
		return true;

	UINT32 uRef = *( (UINT32*) &fReference );
	UINT32 uOut = *( (UINT32*) &fOutput );
	if( (int)max( uRef, uOut ) - (int)min( uRef, uOut ) <= 1 )
		return true;

	return false;
}

//---------------------------------------------------------------

void CRenderTargetsTestApp::InitOptionVariables( )
{
	CD3D11TestFramework::InitOptionVariables();
	UnRegisterOptionVariable( "SrcOnly" );
	m_D3DOptions.SrcOnly = true;

	// Indicate that this test works on feature level 9
	m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_1;

	int n = AddOptionCategory( _T( "Debug" ) );
}

//---------------------------------------------------------------

bool CRenderTargetsTestApp::InitTestGroups( )
{
	if( !ADD_TEST( "RenderTargets", CRenderTargetsTest ) ) return false;
	if( !ADD_TEST( "Clear", CClearRenderTargetViewTest ) ) return false;

	return true;
}
#ifndef DXGI_BASIC_D3D10_INCLUDED
#define DXGI_BASIC_D3D10_INCLUDED

#include <d3d10_1.h>
#include <d3dx10.h>

#include "WGFTestCore.h"
#include "WGFTestUI.h"

typedef HRESULT (WGFTESTAPI *PFND3D10CREATEDEVICE)
(
	IDXGIAdapter *pAdapter,
	D3D10_DRIVER_TYPE DriverType,
	HMODULE Software,
	UINT Flags,
	UINT32 SDKVersion,
	ID3D10Device **ppDevice
);


class CTestBasicD3D10
{
	public:
		CTestBasicD3D10();
		virtual ~CTestBasicD3D10();


	public:
		bool Initialize_D3D10();


	private:
		static HMODULE hmoduleD3D10;


	protected:
		static const char * const szD3D10Path;
		static PFND3D10CREATEDEVICE pfnD3D10CreateDevice;
		static bool bD3D10Loaded;
};

#endif

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
// Copyright(C) Microsoft.All rights reserved.

// resourcelist.cpp

#include <atlbase.h>
#include <atlstr.h>
#include <audioclient.h>
#include <mmdeviceapi.h>
#include <endpointvolume.h>
#include <strsafe.h>
#include <functiondiscoverykeys_devpkey.h>

#include <basiclog.h>
#include <BasicLogHelper.h>
#include <basicprintf.h>

#include <ITestResource.h>
#include <ResourceList.h>
#include <WexTestClass.h>
#include <devicetopology.h>

#include <debug.h>
#include <doonexit.h>
#include <offloadpath.h>
#include <taefdevice.h>
#include <taefdevicehelper.h>

#include "haptest.h"
#include "log.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;

class CCoUninitializeOnExit {
public:
    CCoUninitializeOnExit() {}
    ~CCoUninitializeOnExit() { CoUninitialize(); }
};

HRESULT __cdecl BuildResourceList(ResourceList& resourceList) {
    Log::Comment(L"Begin BuildResourceList");

    HRESULT hr = S_OK;

    // don't want to see VERIFY spew
    SetVerifyOutput verifySettings(VerifyOutputSettings::LogOnlyFailures);

    if (!VERIFY_SUCCEEDED(hr = CoInitializeEx(NULL, COINIT_MULTITHREADED))) { return hr; }
    CCoUninitializeOnExit cou;
 
    if (!VERIFY_SUCCEEDED(hr = 
        AddAllAudioMMDevices(eAll, TRUE, resourceList))) { return hr; }

    Log::Comment(L"End BuildResourceList");

    return S_OK;
}
#include "LostDev.h"

// ----------------------------------------------------------------------------

void CLostDev::ValidateTestType()
{
#if FAST_TEST
    m_casesTestType1Res.AddCase(TEST_LOSTWHILERENDERING, _T("LostWhileRendering"));
#else //FAST_TEST
	if (m_uCases & TEST_CREATIONWHILELOST)
	{
		m_casesTestType1Res.AddCase(TEST_CREATIONWHILELOST,	"CreationWhileLost");
	}

	if (m_uCases & TEST_CREATIONWHILELOSTFILLAFTER)
	{
		m_casesTestType1Res.AddCase(TEST_CREATIONWHILELOSTFILLAFTER, _T("CreationWhileLostFillAfter"));
	}

	if (m_uCases & TEST_LOCKWHILELOST)
	{
		m_casesTestType1Res.AddCase(TEST_LOCKWHILELOST, _T("LockWhileLost"));
	}

	if (m_uCases & TEST_LOSTWHILELOCKED)
	{
		m_casesTestType1Res.AddCase(TEST_LOSTWHILELOCKED, _T("LostWhileLocked"));
	}

	if (m_uCases & TEST_LOSTWHILERENDERING)
	{
		m_casesTestType1Res.AddCase(TEST_LOSTWHILERENDERING, _T("LostWhileRendering"));
	}
#ifdef USE_FUS
	if ((m_uLostFlags & LOST_FUS) && (m_uCases & TEST_FUSCREATEFUSBACK))
	{
		m_casesTestType1Res.AddCase(TEST_FUSCREATEFUSBACK, _T("FUSCreateFUSBack"));
	}

	if ((m_uLostFlags & LOST_FUS) && (m_uCases & TEST_CREATEFUSDELETEFUSBACK))
	{
		m_casesTestType1Res.AddCase(TEST_CREATEFUSDELETEFUSBACK, _T("CreateFUSDeleteFUSBack"));
	}
#endif
	if (m_uCases & TEST_CREATEAANDBTHENLOSE)
	{
		m_casesTestType2Res.AddCase(TEST_CREATEAANDBTHENLOSE, _T("CreateAAndBThenLose"));
	}

	if (m_uCases & TEST_CREATEALOSECREATEB)
	{
		m_casesTestType2Res.AddCase(TEST_CREATEALOSECREATEB, _T("CreateAThenLoseThenCreateB"));
	}

	if (m_uCases & TEST_LOSETHENCREATEAANDB)
	{
		m_casesTestType2Res.AddCase(TEST_LOSETHENCREATEAANDB, _T("LoseThenCreateAAndB"));
	}
	//...

	if (m_uCases & TEST_DEFAULTPOOLTEST)
	{
		m_casesTestTypeDefault.AddCase(TEST_DEFAULTPOOLTEST, _T("DefaultPoolTest"));
	}

	if ( (m_uLostFlags & LOST_STANDBY) && (m_uCases & TEST_CREATEAANDBTHENSUSPENDSYSTEM) )
	{
		m_casesTestTypeDefault.AddCase(TEST_CREATEAANDBTHENSUSPENDSYSTEM, _T("CreateAAndBThenSuspendSystem(standby)"));
	}

	if ( (m_uLostFlags & LOST_HIBERNATE) && (m_uCases & TEST_CREATEAANDBTHENSUSPENDSYSTEM) )
	{
		m_casesTestTypeDefault.AddCase(TEST_CREATEAANDBTHENSUSPENDSYSTEM, _T("CreateAAndBThenSuspendSystem(hibernate)"));
	}

	if (m_uCases & TEST_RELEASEAFTERRESETFAILED)
	{
		//Do not test until this is debugged
		m_casesTestType1Res.AddCase(TEST_RELEASEAFTERRESETFAILED, _T("ReleaseAfterResetFailed"));
	}
#endif//FAST_TEST
}


void CLostDev::ValidatePools()
{
#if FAST_TEST
    m_casesPools.AddCase(POOL_SYSTEMMEM, _T("SYSTEMMEM"));
#else FAST_TEST//FAST_TEST
	if ( m_bTestSystemMem && (m_uPoolsToTest & TEST_SYSTEMMEM) )
	{
		m_casesPools.AddCase(POOL_SYSTEMMEM, _T("SYSTEMMEM"));
	}

	if ( m_bTestManaged && (m_uPoolsToTest & TEST_MANAGED) )
	{
		m_casesPools.AddCase(POOL_MANAGED, _T("MANAGED"));
	}

	if ( m_bTestDefault && (m_uPoolsToTest & TEST_DEFAULT) )
	{
		m_casesPools.AddCase(POOL_DEFAULT, _T("DEFAULT"));
	}

	if (m_bTestScratch && (m_uPoolsToTest & TEST_SCRATCH) )
	{
		m_casesPools.AddCase(POOL_SCRATCH, _T("SCRATCH"));
	}

	if (m_casesPools.GetNbCases() == 0)
	{
		m_bSkipGroup = true;
	}
#endif
}

void CLostDev::ValidateLostWays()
{
	//if (m_uLostFlags & LOST_CHANGEDISPLAY)
	//{
	//	m_casesLostWays.AddCase(LOST_CHANGEDISPLAY, _T("ChangeDisplay"));
	//}
#if FAST_TEST
    m_casesLostWays.AddCase(LOST_CDSBYOTHERPROCESS, _T("ChangeDisplayByOtherProcess"));
#else//FAST_TEST
	if (m_uLostFlags & LOST_FUS)
	{
		m_casesLostWays.AddCase(LOST_FUS, _T("FUS"));
	}

	if (m_uLostFlags & LOST_CDSBYOTHERPROCESS)
	{
		m_casesLostWays.AddCase(LOST_CDSBYOTHERPROCESS, _T("ChangeDisplayByOtherProcess"));
	}

	if (m_uLostFlags & LOST_STANDBY)
	{
		m_casesLostWays.AddCase(LOST_STANDBY, _T("Standby"));
	}

	if (m_uLostFlags & LOST_HIBERNATE)
	{
		m_casesLostWays.AddCase(LOST_HIBERNATE, _T("Hibernate"));
	}

	if (m_casesLostWays.GetNbCases() == 0)
	{
		//m_casesLostWays.AddCase(LOST_CHANGEDISPLAY, _T("ChangeDisplay"));
		//m_casesLostWays.AddCase(LOST_FUS, _T("FUS"));
		m_casesLostWays.AddCase(LOST_CDSBYOTHERPROCESS, _T("ChangeDisplayByOtherProcess"));
	}
#endif//FAST_TEST
}


void CLostDev::SetTestsToRun()
{
	m_uPoolsToTest = TEST_ALLPOOLS;

	//Determine which pools to test
	if ( KeySet(_T("POOL_SYSTEMMEM")) )
	{
		m_uPoolsToTest = TEST_SYSTEMMEM;
	}
	if ( KeySet(_T("!POOL_SYSTEMMEM")) || KeySet(_T("WHQL")))
	{
		m_uPoolsToTest &= ~TEST_SYSTEMMEM;
	}

	if ( KeySet(_T("POOL_MANAGED")) )
	{
		m_uPoolsToTest = TEST_MANAGED;
	}
	if ( KeySet(_T("!POOL_MANAGED")) )
	{
		m_uPoolsToTest &= ~TEST_MANAGED;
	}

	if ( KeySet(_T("POOL_DEFAULT")) )
	{
		m_uPoolsToTest = TEST_DEFAULT;
	}
	if ( KeySet(_T("!POOL_DEFAULT")) )
	{
		m_uPoolsToTest &= ~TEST_DEFAULT;
	}

	if ( KeySet(_T("POOL_SCRATCH")) )
	{
		m_uPoolsToTest = TEST_SCRATCH;
	}
	if ( KeySet(_T("!POOL_SCRATCH")) || KeySet(_T("WHQL")))
	{
		//Do not test scratch for whql
		m_uPoolsToTest &= ~TEST_SCRATCH;
	}
	if (0 == m_uPoolsToTest)
	{
		m_uPoolsToTest = TEST_ALLPOOLS;
	}

	//Individual test cases to run
	m_uCases = TEST_ALLCASES;
	if ( KeySet(_T("TEST_CREATIONWHILELOST")) )
	{
		m_uCases = TEST_CREATIONWHILELOST;
	}
	if ( KeySet(_T("!TEST_CREATIONWHILELOST")) )
	{
		m_uCases &= ~TEST_CREATIONWHILELOST;
	}

	if ( KeySet(_T("TEST_CREATIONWHILELOSTFILLAFTER")) )
	{
		m_uCases = TEST_CREATIONWHILELOSTFILLAFTER;
	}
	if ( KeySet(_T("!TEST_CREATIONWHILELOSTFILLAFTER")) )
	{
		m_uCases &= ~TEST_CREATIONWHILELOSTFILLAFTER;
	}

	if ( KeySet(_T("TEST_LOCKWHILELOST")) )
	{
		m_uCases = TEST_LOCKWHILELOST;
	}
	if ( KeySet(_T("!TEST_LOCKWHILELOST")) )
	{
		m_uCases &= ~TEST_LOCKWHILELOST;
	}

	if ( KeySet(_T("TEST_LOSTWHILELOCKED")) )
	{
		m_uCases = TEST_LOSTWHILELOCKED;
	}
	if ( KeySet(_T("!TEST_LOSTWHILELOCKED")) )
	{
		m_uCases &= ~TEST_LOSTWHILELOCKED;
	}

	if ( KeySet(_T("TEST_LOSTWHILERENDERING")) )
	{
		m_uCases = TEST_LOSTWHILERENDERING;
	}
	if ( KeySet(_T("!TEST_LOSTWHILERENDERING")) )
	{
		m_uCases &= ~TEST_LOSTWHILERENDERING;
	}

	if ( KeySet(_T("TEST_CREATEAANDBTHENLOSE")) )
	{
		m_uCases = TEST_CREATEAANDBTHENLOSE;
	}
	if ( KeySet(_T("!TEST_CREATEAANDBTHENLOSE")) )
	{
		m_uCases &= ~TEST_CREATEAANDBTHENLOSE;
	}

	if ( KeySet(_T("TEST_CREATEALOSECREATEB")) )
	{
		m_uCases = TEST_CREATEALOSECREATEB;
	}
	if ( KeySet(_T("!TEST_CREATEALOSECREATEB")) )
	{
		m_uCases &= ~TEST_CREATEALOSECREATEB;
	}

	if ( KeySet(_T("TEST_LOSETHENCREATEAANDB")) )
	{
		m_uCases = TEST_LOSETHENCREATEAANDB;
	}
	if ( KeySet(_T("!TEST_LOSETHENCREATEAANDB")) )
	{
		m_uCases &= ~TEST_LOSETHENCREATEAANDB;
	}

	if ( KeySet(_T("TEST_FUSCREATEFUSBACK")) )
	{
		m_uCases = TEST_FUSCREATEFUSBACK;
	}
	if ( KeySet(_T("!TEST_FUSCREATEFUSBACK")) )
	{
		m_uCases &= ~TEST_FUSCREATEFUSBACK;
	}

	if ( KeySet(_T("TEST_CREATEFUSDELETEFUSBACK")) )
	{
		m_uCases = TEST_CREATEFUSDELETEFUSBACK;
	}
	if ( KeySet(_T("!TEST_CREATEFUSDELETEFUSBACK")) )
	{
		m_uCases &= ~TEST_CREATEFUSDELETEFUSBACK;
	}

	if ( KeySet(_T("TEST_CREATEAANDBTHENSUSPENDSYSTEM")) )
	{
		m_uCases = TEST_CREATEAANDBTHENSUSPENDSYSTEM;
	}
	if ( KeySet(_T("!TEST_CREATEAANDBTHENSUSPENDSYSTEM")) )
	{
		m_uCases &= ~TEST_CREATEAANDBTHENSUSPENDSYSTEM;
	}


	if (! KeySet(_T("WHQL")) )
	{
		if ( KeySet(_T("TEST_RELEASEAFTERRESETFAILED")) )
		{
			m_uCases = TEST_RELEASEAFTERRESETFAILED;
		}
		if ( KeySet(_T("!TEST_RELEASEAFTERRESETFAILED")) )
		{
			m_uCases &= ~TEST_RELEASEAFTERRESETFAILED;
		}
	}


/*
	//Flags
	m_uFlags = FLAG_ALLFLAGS;
	if ( KeySet(_T("FLAG_READONLY")) )
	{
		m_uFlags = FLAG_READONLY;
	}
	if ( KeySet(_T("!FLAG_READONLY")) )
	{
		m_uFlags &= ~FLAG_READONLY;
	}

	if ( KeySet(_T("FLAG_READWRITE")) )
	{
		m_uFlags = FLAG_READWRITE;
	}
	if ( KeySet(_T("!FLAG_READWRITE")) )
	{
		m_uFlags &= ~FLAG_READWRITE;
	}
	if (! m_uFlags)
		m_uFlags = FLAG_ALLFLAGS;
*/


	//Ways to lose the device
	m_uLostFlags = LOST_ALLLOSTWAYS;
	if ( KeySet(_T("LOST_CHANGEDISPLAY")) )
	{
		m_uLostFlags = LOST_CHANGEDISPLAY;
	}
	if ( KeySet(_T("!LOST_CHANGEDISPLAY")) || KeySet(_T("WHQL")) )
	{
		m_uLostFlags &= ~LOST_CHANGEDISPLAY;
	}

	if ( KeySet(_T("LOST_FUS")) )
	{
		m_uLostFlags = LOST_FUS;
	}
	if ( KeySet(_T("!LOST_FUS")) )
	{
		m_uLostFlags &= ~LOST_FUS;
	}

	if ( KeySet(_T("LOST_CDSBYOTHERPROCESS")) )
	{
		m_uLostFlags = LOST_CDSBYOTHERPROCESS;
	}
	if ( KeySet(_T("!LOST_CDSBYOTHERPROCESS")) )
	{
		m_uLostFlags &= ~LOST_CDSBYOTHERPROCESS;
	}

	if ( KeySet( _T("LOST_STANDBY" ) ) )
	{
		m_uLostFlags = LOST_STANDBY;
	}
	else
	{
		m_uLostFlags &= ~LOST_STANDBY;
	}

	if ( KeySet( _T("LOST_HIBERNATE") ) )
	{
		m_uLostFlags = LOST_HIBERNATE;
	}
	else
	{
		m_uLostFlags &= ~LOST_HIBERNATE;
	};


	m_bSeeResult = false;
	if (KeySet(_T("SEERESULT")) || KeySet(_T("SEERESULTS")))
	{
		m_bSeeResult = true;
	};


	m_bSeeFailures = false;
	if (KeySet(_T("SEEFAILURE")) || KeySet(_T("SEEFAILURES")))
	{
		m_bSeeFailures = true;
	};


#ifndef USE_FUS
	//Disable FUS if it's not enabled
	m_uLostFlags &= ~LOST_FUS;
#endif
}


bool CLostDev::CreateTestFactors()
{
	ValidatePools();
	ValidateTestType();
	ValidateLostWays();

	UINT uNbDefaultCases = m_casesPools.DoesCaseExist(POOL_DEFAULT) ? m_casesTestTypeDefault.GetNbCases() : 0;
	UINT uNbScratch1ResCases = m_casesPools.DoesCaseExist(POOL_SCRATCH) ? m_casesTestType1Res.GetNbCases() : 0;
	UINT uNbScratch2ResCases = m_casesPools.DoesCaseExist(POOL_SCRATCH) ? m_casesTestType2Res.GetNbCases() : 0;
	UINT uNbManaged1ResCases = m_casesPools.DoesCaseExist(POOL_MANAGED) ? m_casesTestType1Res.GetNbCases() : 0;
	UINT uNbManaged2ResCases = m_casesPools.DoesCaseExist(POOL_MANAGED) ? m_casesTestType2Res.GetNbCases() : 0;
	UINT uNbSystemMem1ResCases = m_casesPools.DoesCaseExist(POOL_SYSTEMMEM) ? m_casesTestType1Res.GetNbCases() : 0;
	UINT uNbSystemMem2ResCases = m_casesPools.DoesCaseExist(POOL_SYSTEMMEM) ? m_casesTestType2Res.GetNbCases() : 0;

	//Create the factors for pools and lost ways
	if(	!CHECKREF(m_rPoolDefault = NewFactor( uNbDefaultCases )) ||
		!CHECKREF(m_rPoolManaged1Res = NewFactor( uNbManaged1ResCases )) ||
		!CHECKREF(m_rPoolManaged2Res = NewFactor( uNbManaged2ResCases )) ||
		!CHECKREF(m_rPoolSystemMem1Res = NewFactor( uNbSystemMem1ResCases )) ||
		!CHECKREF(m_rPoolSystemMem2Res = NewFactor( uNbSystemMem2ResCases )) ||
		!CHECKREF(m_rPoolScratch1Res = NewFactor( uNbScratch1ResCases )) ||
		!CHECKREF(m_rPoolScratch2Res = NewFactor( uNbScratch2ResCases )) ||
		!CHECKREF(m_rLostWays = NewFactor( m_casesLostWays.GetNbCases() )))
	{
		DPF(0, "ERROR setting the cases in SetTestsToRun()");
		return false;
	}

	m_rPoolManaged = m_rPoolManaged1Res + m_rPoolManaged2Res;
	m_rPoolSystemMem = m_rPoolSystemMem1Res + m_rPoolSystemMem2Res;
	m_rPoolScratch = m_rPoolScratch1Res + m_rPoolScratch2Res;
	return true;
}


void CLostDev::SetTestFactors()
{
	m_rCasesManager =
		m_rLostWays *
		(
			m_rPoolManaged + 
			m_rPoolSystemMem +
			m_rPoolScratch +
			m_rPoolDefault 
		);
}

// ----------------------------------------------------------------------------

bool CLostDev::SetTestCase(UINT uTestNumber)
{
//	m_uTestNumber = uTestNumber;

	//Set the pool and test type
	if (m_rPoolDefault->Active())
	{
		m_Pool = POOL_DEFAULT;
		m_szPoolType = _T("Default");
		m_uTestType = m_casesTestTypeDefault[ m_rPoolDefault->Value() ]->GetValue();
		m_szSubTest = m_casesTestTypeDefault[ m_rPoolDefault->Value() ]->GetDesc();
	}
	else if (m_rPoolManaged1Res->Active() || m_rPoolManaged2Res->Active())
	{
		m_Pool = POOL_MANAGED;
		m_szPoolType = _T("Managed");
		if (m_rPoolManaged1Res->Active())
		{
			m_uTestType = m_casesTestType1Res[ m_rPoolManaged1Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType1Res[ m_rPoolManaged1Res->Value() ]->GetDesc();
		}
		else //if (m_rPoolManaged2Res->Active())
		{
			m_uTestType = m_casesTestType2Res[ m_rPoolManaged2Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType2Res[ m_rPoolManaged2Res->Value() ]->GetDesc();
		}
	}
	else if (m_rPoolSystemMem1Res->Active() || m_rPoolSystemMem2Res->Active())
	{
		m_Pool = POOL_SYSTEMMEM;
		m_szPoolType = _T("SystemMem");
		if (m_rPoolSystemMem1Res->Active())
		{
			m_uTestType = m_casesTestType1Res[ m_rPoolSystemMem1Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType1Res[ m_rPoolSystemMem1Res->Value() ]->GetDesc();
		}
		else //if (m_rPoolSystemMem2Res->Active())
		{
			m_uTestType = m_casesTestType2Res[ m_rPoolSystemMem2Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType2Res[ m_rPoolSystemMem2Res->Value() ]->GetDesc();
		}
	}
	else if (m_rPoolScratch1Res->Active() || m_rPoolScratch2Res->Active())
	{
		m_Pool = POOL_SCRATCH;
		m_szPoolType = _T("Scratch");
		if (m_rPoolScratch1Res->Active())
		{
			m_uTestType = m_casesTestType1Res[ m_rPoolScratch1Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType1Res[ m_rPoolScratch1Res->Value() ]->GetDesc();
		}
		else //if (m_rPoolScratch2Res->Active())
		{
			m_uTestType = m_casesTestType2Res[ m_rPoolScratch2Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType2Res[ m_rPoolScratch2Res->Value() ]->GetDesc();
		}
	}
	else //Error 
	{
		DPF(0, "ERROR in lostdev, no pool active");
		return false;
	}

	//Set the lost way
	m_uLostType = (UINT)m_casesLostWays[ m_rLostWays->Value() ]->GetValue();
	m_szLostType = m_casesLostWays[ m_rLostWays->Value() ]->GetDesc();
	return true;
}

// ----------------------------------------------------------------------------

void CLostDev::CommandLineHelp(void)
{
	WriteCommandLineHelp("$gPOOLS");
	WriteCommandLineHelp("$y-POOL_SYSTEMMEM\t$wTest only the SystemMem pool");
	WriteCommandLineHelp("$y-POOL_MANAGED\t$wTest only the Managed pool");
	WriteCommandLineHelp("$y-POOL_DEFAULT\t$wTest only the Default pool");

	WriteCommandLineHelp("$y-!POOL_SYSTEMMEM\t$wDo NOT test the SystemMem pool");
	WriteCommandLineHelp("$y-!POOL_MANAGED\t$wDo NOT test the Managed pool");
	WriteCommandLineHelp("$y-!POOL_DEFAULT\t$wDo NOT test the Default pool");

	WriteCommandLineHelp("\n$gTEST CASES");
	WriteCommandLineHelp("\n$g\tFor all the following flag, if '!' is put before 'TEST', the test case will");
	WriteCommandLineHelp("\n$g\tbe excluded(eg. -!TEST_LOCKWHILELOST).  Many test cases can be excluded.  Otherwise,");
	WriteCommandLineHelp("\n$g\tONLY the test case of the TEST parameter will be executed.\n");
	WriteCommandLineHelp("$y-TEST_CREATIONWHILELOST\t$wTest case that creates a resource while the dev is lost");
	WriteCommandLineHelp("$y-TEST_CREATIONWHILELOSTFILLAFTER\t$wTest case that creates a res while the dev is lost but fills it after the reset");
	WriteCommandLineHelp("$y-TEST_LOCKWHILELOST\t$wTest case that lock a res while the dev is lost");
	WriteCommandLineHelp("$y-TEST_LOSTWHILELOCKED\t$wTest case that lose the dev while a res is locked");
	WriteCommandLineHelp("$y-TEST_LOSTWHILERENDERING\t$wTest case that lose the dev after rendering, then re-render with same res");
	WriteCommandLineHelp("$y-TEST_CREATEAANDBTHENLOSE\t$wTest case that creates res A and res B, loses the dev and copies A to B");
	WriteCommandLineHelp("$y-TEST_CREATEALOSECREATEB\t$wTest case that creates res A, loses the dev, creates res B, then copies A to B");
	WriteCommandLineHelp("$y-TEST_LOSETHENCREATEAANDB\t$wTest case that loses the dev, then creates res A and res B and copies A to B");
	WriteCommandLineHelp("$y-TEST_RELEASEAFTERRESETFAILED\t$wTest case that creates a resource then causes the Reset to fail and try to release after");

/*    WriteCommandLineHelp("\n$gFLAGS");
	WriteCommandLineHelp("$y-FLAG_READONLY\t$wRun test cases with READONLY Flag");
	WriteCommandLineHelp("$y-FLAG_READWRITE\t$wRun test cases with no flag");
	WriteCommandLineHelp("$y-!FLAG_READONLY\t$wDo NOT run test cases with READONLY Flag");
	WriteCommandLineHelp("$y-!FLAG_READWRITE\t$wDo NOT run test cases with no flag");
*/
	WriteCommandLineHelp("\n$gWAYS TO LOSE DEV");
	WriteCommandLineHelp("$y-LOST_CHANGEDISPLAY\t$wLose the device with ChangeDisplaySettings");
	WriteCommandLineHelp("$y-LOST_CDSBYOTHERPROCESS\t$wLose the device by another process calling ChangeDisplaySettings");
	WriteCommandLineHelp("$y-LOST_FUS\t$wLose the device with Fast User Switching");
	WriteCommandLineHelp("$y-!LOST_CHANGEDISPLAY\t$wDo NOT lose the device with ChangeDisplaySettings");
	WriteCommandLineHelp("$y-!LOST_CDSBYOTHERPROCESS\t$wDo NOT lose the device by another process calling ChangeDisplaySettings");
	WriteCommandLineHelp("$y-!LOST_FUS\t$wDo NOT lose the device with Fast User Switching");


	WriteCommandLineHelp("\n$gOPTIONS");
	WriteCommandLineHelp("$y-SEERESULT\t$wWaits for 2 seconds after rending.");
	WriteCommandLineHelp("$y-SEEFAILURE\t$wWaits for 2 seconds after rendering a failure.");
}
//*****************************************************************************
// Copyright (C) Microsoft Corporation. All rights reserved.
//
// File:         EffectsDiscoveryCPPTests.h
//
//*****************************************************************************
#pragma once

#include "EffectsDiscoveryCPPTestCore.h"



class CEffectsDiscoveryCPPTest: public CEffectsDiscoveryCPPTestCore
{
private:
       

public:
    CEffectsDiscoveryCPPTest();
    ~CEffectsDiscoveryCPPTest(); 
    
    void AudioTestsCapture();    
    void AudioTestsRender();  
    void AudioEffectsTests();
    void DiscoverCaptureEffects();
    void DiscoverRenderEffects();
    void VerifyEnhancementsFromUIForCapture();
    void VerifyEnhancementsFromUIForRender();
    void ToggleLoudnessEq();    
    void ToggleLoudnessEqAfterDisable();
    void TriggerEffectsDisableRender();

};/*
Module: MINIPORT.C


name:USB miniport interface (HCDI)

    Copyright (c) Microsoft Corporation. All rights reserved



Abstract:


    Functions used to interface with USB miniport drivers. This file contains both USBX (extended) and standard
    documentation.

    naming convention:

    MINIPORT_Usbx<FuncName> - USBX miniport extensions.
    MINIPORT_<FuncName> - standard and USB2.



<pre>

<USBX miniport extensions>
This is a general name given extensions to the standard USB miniport interface in the Longhorn codebase.  These
extensions will be used in USBEHCI.SYS to move the USB2 bus specific budget code into the miniport.  They provide
a better method for synchronizing with the hardware on more sophisticated controllers.  All extended miniport
functions begin with the preamble (Usbx) and are only called in the miniport sets the OPT_USBX flag on registration
with USBPORT.SYS.  The standard USB 1.1 controller miniports do not use these extensions.  These extension do not
contain any USB2 specific features -- these are isolated with a separate flag OPT_USB2.

Modifications to the miniport interface for WHCI (wireless USB) should occur in this interface.

</pre>


Notes:

    Margin = 120

--*/

#include "common.h"

#ifdef USB2_INTERNAL
// globals
extern ULONG USB2LIB_HcContextSize;
extern ULONG USB2LIB_TtContextSize;
extern ULONG USB2LIB_EndpointContextSize;
#endif

VOID
MPh_AcquireMpSuspendResumeLock(
    PDEVICE_EXTENSION HcFdoExt
    )
{
    ASSERT_PASSIVE();

    KeWaitForSingleObject(&HcFdoExt->Fdo.MpSuspendResumeLock,
                          Executive,
                          KernelMode,
                          FALSE,
                          NULL);
    LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_PNP, 'MPL+',
        HcFdoExt->Fdo.MpResumeThread, HcFdoExt->Fdo.MpSuspendThread, 0);

}

VOID
MPh_ReleaseMpSuspendResumeLock(
    PDEVICE_EXTENSION HcFdoExt
    )
{
    ASSERT_PASSIVE();

    LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_PNP, 'MPL-',
        HcFdoExt->Fdo.MpResumeThread, HcFdoExt->Fdo.MpSuspendThread, 0);

    KeReleaseSemaphore(&HcFdoExt->Fdo.MpSuspendResumeLock,
                       LOW_REALTIME_PRIORITY,
                       1,
                       FALSE);
}


/*

Routine: MINIPORT_CheckHwSync

ULONG MINIPORT_CheckHwSync(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );


Description:

    This routine allows the miniport to manage the hardware synchronization for the removal of endpoints (queue heads)
    internally.  If the OPT_HW_SYNC flag is indicated by the miniport then the endpoint will be polled on each hardware
    interrupt until the miniport indicates the endpoint staructures have been removed from the HC schedule.


HCI_Revison - 3+.

Core_Lock - yes.

Irql - DPC_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Endpoint Context.


Returns:

    non-zero if the endpoint has finished synchronizing with the host adapter.  0 if it is still busy.

*/
ULONG
MPf_CheckHwSync(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    ULONG hc_done = 0;
    KIRQL irql;

    USBPORT_ASSERT(USBPORT_HW_SYNC(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_PollHwSync != NULL);

        if (MPF(HcFdoExt).MINIPORT_PollHwSync == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            hc_done = 1;
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        hc_done = MPF(HcFdoExt).MINIPORT_PollHwSync(HcFdoExt->Fdo.MiniportDeviceData,
                                                    &Endpoint->MiniportEndpointData[0]);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

    return hc_done;
}

/*

Routine: MINIPORT_UsbxOpenEndpoint

USB_MINIPORT_STATUS MINIPORT_UsbxOpenEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_PARAMETERS EpParameters,
    PENDPOINT_DATA EpContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_EP_DATA HsbEpContext,
    PHSB_TT_DATA HsbTtContext
    );


Description:

    Extended version to support USB 2.0 and wireless USB. This version is called at passive level, the miniport may
    block.  This opertion is serialized thru the miniport so it is not reentrant.

HCI_Revison - 3+.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

EpParameters(Epp) - general USB parmeters. This structure is identical to OpenEndpoint (classic version), however,
some fields are no longer used.

EpContext - Endpoint Context.

HsbHcContext, HsbEpContext, HsbTtContext - (optional) high speed bus context structures.

<pre>
------------------------------------------------------------------------------------------------------------------------
Epp.DeviceAddress - input, from endpoint descriptor.
Epp.EndpointAddress - input, from endpoint descriptor.
Epp.MaxPacketSize - input, from endpoint descriptor.
Epp.MaxPeriod - input, max allowable period from endpoint descriptor.
Epp.DeviceSpeed- input, device operating speed.
Epp.TransferType - input, from endpoint descriptor.
Epp.TransferDirection input, from endpoint descriptor.
Epp.CommonBufferVa - input, .
Epp.CommonBufferPhys - input, .
Epp.CommonBufferBytes - input, .
Epp.EndpointFlags - input, endpoint option flags (EP_PARM_FLAG_*).
Epp.TtDeviceAddress - input, .
Epp.TtPortNumber - input, port number (really TT number) of the TT for this device.
Epp.TransactionsPerMicroframe - input, from endpoint descriptor.
Epp.MuxPacketSize - input, from endpoint descriptor.

Epp.MaxTransferSize - output, maximum size in bytes of any single transfer.
Epp.Period -  output, adjusted interrupt period will be one of : 128, 64, 32, 16, 8, 4, 2, 1. miniport must select the
period.
Epp.Bandwidth -  output, relative bandwidth consumed in bits/ms. If the device is on a TT this is the relative bandwdth
consumed on the TT.

Epp.ScheduleOffset - reserved, (do not use).
Epp.Ordinal - reserved,(do not use).
Epp.Pad - reserved,(do not use).
Epp.InterruptScheduleMask - reserved, (do not use).
Epp.SplitCompletionMask - reserved, (do not use).
-----------------------------------------------------------------------------------------------------------------------
</pre>

Returns:

    MP_STATUS_SUCCESS if the endpoint can be initialized. Endpoint should be initially set to the PAUSE state if
    success is returned.

*/
USB_MINIPORT_STATUS
MPf_UsbxOpenEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt,
    __inout_opt USBD_STATUS *UsbdStatus
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;
    PHSB_TT_DATA hsbTt = NULL;

    if (Tt) {
        USBPORT_ASSERT(Endpoint->Tt == Tt);
        hsbTt = Tt->HsbTtContextPtr;

    }

    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxOpenEndpoint!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxOpenEndpoint == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxOpenEndpoint(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->Parameters,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Endpoint->HsbEpContextPtr,
                                                hsbTt);

    } WHILE (0);

    return mps;
}

/*

Routine: MINIPORT_UsbxPokeEndpoint

USB_MINIPORT_STATUS MINIPORT_UsbxPokeEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_PARAMETERS EpParameters,
    PENDPOINT_DATA EpContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_EP_DATA HsbEpContext,
    PHSB_TT_DATA HsbTtContext
    );


Description:

    Use to change the parmeters of an endpoint without removing it from USBPORT internal queues.  This call is only
    valid if the endpoint is in the PAUSE state.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Endpoint Context.

EpParameters - general USB endpoint parmeters.

HsbHcContext, HsbEpContext, HsbTtContext - (optional) high speed bus context structures.

Returns:

    MP_STATUS_SUCCESS if the endpoint can be re-initialized.


*/
USB_MINIPORT_STATUS
MPf_UsbxPokeEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;
    PHSB_TT_DATA hsbTt = NULL;

    if (Tt) {
        USBPORT_ASSERT(Endpoint->Tt == Tt);
        hsbTt = Tt->HsbTtContextPtr;

    }

    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxPokeEndpoint!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxPokeEndpoint == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxPokeEndpoint(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->Parameters,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Endpoint->HsbEpContextPtr,
                                                hsbTt);

    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_UsbxInitTransactionTranslator

VOID MINIPORT_UsbxInitTransactionTranslator(
    PDEVICE_DATA HcContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_TT_DATA HsbTtContext
    );


Description:

    Initializes an instance of a tt in the miniport.  This routine is only called if the OPT_USB20 flag is set.

HCI_Revison - 3+.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

TtContext - Transaction Translator Context.

Returns:

    none.


*/
USB_MINIPORT_STATUS
MPf_UsbxInitHsbTransactionTranslator(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in PTRANSACTION_TRANSLATOR Tt
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxInitHsbTransactionTranslator != NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxInitHsbTransactionTranslator == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxInitHsbTransactionTranslator(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Tt->HsbTtContextPtr);

    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_UsbxInitHsbController

VOID MINIPORT_UsbxInitHsbController(
    PDEVICE_DATA HcContext,
    PHSB_HC_DATA HsbHcContext
    );


Description:

    Initializes an instance of a high speen controller thru the miniport.  This routine is only called if the
    OPT_USB20 flag is set.

HCI_Revison - 3+.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.


Returns:

    none.


*/
USB_MINIPORT_STATUS
MPf_UsbxInitHsbController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxInitHsbController != NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxInitHsbController == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxInitHsbController(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                HcFdoExt->Fdo.HsbControllerContextPtr);

    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_UsbxInitHsbEndpoint

VOID MINIPORT_UsbxInitHsbEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_EP_DATA HsbEpContext
    PHSB_TT_DATA HsbTtContext
    );


Description:

    Initializes an instance of high speed endpoint in the miniport.  This routine is only called if the
    OPT_USB20 flag is set.

HCI_Revison - 3+.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.


Returns:

    none.


*/
USB_MINIPORT_STATUS
MPf_UsbxInitHsbEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in PTRANSACTION_TRANSLATOR Tt
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxInitHsbEndpoint!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxInitHsbEndpoint == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxInitHsbEndpoint(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Endpoint->HsbEpContextPtr,
                                                Tt->HsbTtContextPtr);

    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_UsbxAllocateBandwidth

USB_MINIPORT_STATUS MINIPORT_UsbxAllocateBandwidth(
    PDEVICE_DATA HcContext,
    TT_DATA TtContext,
    PENDPOINT_PARAMETERS EpParameters,
    PENDPOINT_DATA EpContext
    );

Description:

To simplify backward compatibility bandwidth is allocated thru a separate function call into the miniport.  Bandwidth
is allocated in the context of UsbxOpenendpoint and serialized in the same way.  USBPORT will use  'Legacy' USB 11
bandwidth calculations on behalf of the miniport if the OPT_USB11 flag is set.

<pre>
The following table indicates supported values for combinations of USBX, USB11, USB20 miniport flags.

USB_MINIPORT_OPT_USB11                0x00000008
USB_MINIPORT_OPT_USB20                0x00000010
USB_MINIPORT_OPT_USBX                 0x00000400


mask        OPT_USB11       OPT_USB20       OPT_USBX
0x00000000  0               0               0               - Legacy USB 1.1 bw allocation
0x00000400  0               0               1               - Legacy USB 1.1 bw allocation
0x00000010  0               1               0               - not supported (a)
0x00000410  0               1               1               - Allocate thru miniport
0x00000008  1               0               0               - Legacy USB 1.1 bw allocation
0x00000408  1               0               1               - Allocate thru miniport
0x00000018  1               1               0               - not supported, invalid
0x00000418  1               1               1               - not supported, invalid

(a) not supported since the only high speed ehci miniport is usbehci.



</pre>

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

TtContext - Transaction Translator Context (optional).

EpParameters(Epp) - general USB parmeters. This structure is identical to OpenEndpoint (classic version), however,
some fields are no longer used.

<pre>
------------------------------------------------------------------------------------------------------------------------
The following fields are filled in by the port driver:

Epp.DeviceAddress - input, from endpoint descriptor.
Epp.EndpointAddress - input, from endpoint descriptor.
Epp.MaxPacketSize - input, from endpoint descriptor.
Epp.MaxPeriod - input, max allowable period from endpoint descriptor.
Epp.DeviceSpeed- input, device operating speed.
Epp.TransferType - input, from endpoint descriptor.
Epp.TransferDirection input, from endpoint descriptor.
Epp.CommonBufferVa - input, .
Epp.CommonBufferPhys - input, .
Epp.CommonBufferBytes - input, .
Epp.EndpointFlags - input, endpoint option flags (EP_PARM_FLAG_*).
Epp.TtDeviceAddress - input, .
Epp.TtPortNumber - input, port number (really TT number) of the TT for this device.
Epp.TransactionsPerMicroframe - input, from endpoint descriptor.
Epp.MuxPacketSize - input, from endpoint descriptor.

The following fields are filled in by the miniport driver:
Epp.MaxTransferSize - output, maximum size in bytes of any single transfer.
Epp.Period -  output, adjusted interrupt period will be one of : 128, 64, 32, 16, 8, 4, 2, 1. miniport must select the
period.
Epp.Bandwidth -  output, relative bandwidth consumed in bits/ms. If the device is on a TT this is the relative bandwdth
consumed on the TT.

Epp.ScheduleOffset - reserved, (do not use).
Epp.Ordinal - reserved,(do not use).
Epp.Pad - reserved,(do not use).
Epp.InterruptScheduleMask - reserved, (do not use).
Epp.SplitCompletionMask - reserved, (do not use).
-----------------------------------------------------------------------------------------------------------------------
</pre>

EpContext - Endpoint Context.

Returns:

    STATUS_SUCCESS if the bandwidth is allocated.


*/
USB_MINIPORT_STATUS
MPf_UsbxAllocateBandwidth(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt
    )
{
    USB_MINIPORT_STATUS mps;
    PHSB_TT_DATA hsbTt = NULL;

    mps = USBMP_STATUS_FAILURE;

    if (Tt) {
        hsbTt = Tt->HsbTtContextPtr;
    }


    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxAllocateBandwidth != NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxAllocateBandwidth == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_UsbxAllocateBandwidth(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->Parameters,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                hsbTt,
                                                Endpoint->HsbEpContextPtr);
    } WHILE (0);

    return mps;
}


/*

Routine: MINIPORT_UsbxFreeBandwidth

VOID MINIPORT_UsbxFreeBandwidth(
    PDEVICE_DATA HcContext,
    TT_DATA TtContext,
    PENDPOINT_DATA EpContext
    );


Description:

    Miniport should release any bandwidth allocated by MINIPORT_UsbxAllocateBandwidth for this endpoint.  This routine
    is only called if the miniport successfuly allocates bandwidth for an endpoint.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

TtContext - Transaction Translator Context (optional).

EpContext - Endpoint Context.

Returns:

    none.


*/
VOID
MPf_UsbxFreeBandwidth(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt PTRANSACTION_TRANSLATOR Tt
    )
{
    PHSB_TT_DATA hsbTt = NULL;

    USBPORT_ASSERT(Endpoint->EpFlags.USBX_Bandwidth);

    if (Tt) {
        hsbTt = Tt->HsbTtContextPtr;
    }

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxFreeBandwidth != NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxFreeBandwidth == NULL) {
            break;
        }

        // call the engine and attempt to allocate the necessary
        // bus time for this endoint

        MPF(HcFdoExt).MINIPORT_UsbxFreeBandwidth(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                hsbTt,
                                                Endpoint->HsbEpContextPtr);
    } WHILE (0);

    return ;
}

/*

Routine: MINIPORT_UsbxQueryBandwidthData

ULONG MINIPORT_UsbxQueryBandwidthData(
    PDEVICE_DATA HcContext,
    PHSB_HC_DATA HsbHcContext,
    ULONG Frame,
    ULONG Microframe
    );


Description:

    Queries internal bandwidth data from the miniport.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Frame - USB frame number

Microframe - USB microframe number

Returns:

    used bandwidth for the frame,microframe.


*/
ULONG
MPf_UsbxQueryBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG Frame,
    __in ULONG Microframe
    )
{
    ULONG bw = 0;

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxQueryBandwidthData!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxQueryBandwidthData == NULL) {
            break;
        }

        bw = MPF(HcFdoExt).MINIPORT_UsbxQueryBandwidthData(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Frame,
                                                Microframe);

    } WHILE (0);

    return bw;
}


/*

Routine: MINIPORT_UsbxQueryTtBandwidthData

ULONG MINIPORT_UsbxQueryTtBandwidthData(
    PDEVICE_DATA HcContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_TT_DATA HsbTtContext,
    ULONG Frame
    );


Description:

    Queries internal bandwidth data from the miniport for a given TT.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

TtContext - context for tt.

Returns:

     used bandwidth for the frame.


*/
ULONG
MPf_UsbxQueryTtBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PTRANSACTION_TRANSLATOR Tt,
    __in ULONG Frame
    )
{
   ULONG bw = 0;

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxQueryTtBandwidthData!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxQueryTtBandwidthData == NULL) {
            break;
        }

        bw = MPF(HcFdoExt).MINIPORT_UsbxQueryTtBandwidthData(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Tt->HsbTtContextPtr,
                                                Frame);

    } WHILE (0);


    return bw;
}


/*

Routine: MINIPORT_UsbxQueryEpBandwidthData

ULONG MINIPORT_UsbxQueryEpBandwidthData(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EndpointContext,
    PHSB_HC_DATA HsbHcContext,
    PHSB_EP_DATA HsbEpContext
    PHSB_TT_DATA HsbTtContext
    );


Description:

    Queries internal bandwidth data from the miniport for a given endpoint.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EndpointContext - context for endpoint.

Returns:

     used bandwidth for the endpoint.


*/
ULONG
MPf_UsbxQueryEpBandwidthData(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout_opt PTRANSACTION_TRANSLATOR Tt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    PHSB_TT_DATA hsbTt = NULL;
    ULONG bw = 0;

    if (Tt) {
        hsbTt = Tt->HsbTtContextPtr;
    }

    // this routine is the analog to USB2LIB_InitTt
    USBPORT_ASSERT(USBPORT_IS_USBX(HcFdoExt));

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_UsbxQueryEpBandwidthData!= NULL);

        if (MPF(HcFdoExt).MINIPORT_UsbxQueryEpBandwidthData == NULL) {
            break;
        }

        bw = MPF(HcFdoExt).MINIPORT_UsbxQueryEpBandwidthData(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                HcFdoExt->Fdo.HsbControllerContextPtr,
                                                Endpoint->HsbEpContextPtr,
                                                hsbTt);

    } WHILE (0);


    return bw;

}


/*

Routine: MINIPORT_NotifyTransferQueueState

VOID
MINIPORT_NotifyTransferQueueState(
    PDEVICE_DATA HcContext,
    ULONG AsyncTransferCount,
    ULONG PeriodicTransferCount
    );


Description:

    Notifies the miniport of the state of the transfer queue. This is used by the ehci miniport to disble the periodic
    or async lists when not in use.

Core_Lock - yes.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

AsyncTransferCount - current count of async (bulk, control) transfers in the queue.

PeriodicTransferCount - current count of perodic (isoch, interrupt) in the queue.

Returns:

    none.

*/
VOID
MPf_NotifyTransferQueueState(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG AsyncTransferCount,
    __in ULONG PeriodicTransferCount
    )
{
    KIRQL irql;

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_NotifyTransferQueueState == NULL) {
            // treat this one as optional
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_NotifyTransferQueueState(
                               HcFdoExt->Fdo.MiniportDeviceData,
                               AsyncTransferCount,
                               PeriodicTransferCount);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}



/*

Routine: MINIPORT_TakePortControl

VOID MINIPORT_TakePortControl(
    PDEVICE_DATA HcContext
    );


Description:

    This routine sets the owner of a shared USB port. 'Take port control' and chirp_ports are version 3+ specific we
    need them for power management to work properly with EHCI host controllers.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none


*/
VOID
MPf_TakePortControl(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_TakePortControl != NULL);

        if (MPF(HcFdoExt).MINIPORT_TakePortControl == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_TakePortControl(HcFdoExt->Fdo.MiniportDeviceData);

    }  WHILE (0);

}


/*

Routine: MINIPORT_ReleasePortControl

VOID MINIPORT_ReleasePortControl(
    PDEVICE_DATA HcContext
    );


Description:

    Release control of USB 2 ports to CCs. This routine sets the owner of a shared USB ports. This routine is only
    supported if miniport indicates version 3 or higher HCI.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none


*/
VOID
MPf_ReleasePortControl(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }
        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_ReleasePortControl != NULL);

        if (MPF(HcFdoExt).MINIPORT_ReleasePortControl == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        MPF(HcFdoExt).MINIPORT_ReleasePortControl(HcFdoExt->Fdo.MiniportDeviceData);

    } WHILE (0);

}


/*

Routine: MINIPORT_SuspendController

VOID MINIPORT_SuspendController(
    PDEVICE_DATA HcContext
    );


Description:

    Put the host controller in global suspend state.  This routine should only be called once while the
    before MP_ResumeController.

HCI_Revison - all.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none

*/

VOID
MPf_SuspendController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in BOOLEAN EnableRH_IRQ
    )
{
    KIRQL           irql;
    PDEVICE_OBJECT  hcFdo;
    ULONG           mpOptionFlags;

    ASSERT_PASSIVE();

    hcFdo = HcFdoExt->HcFdoDeviceObject;

    mpOptionFlags = REGISTRATION_PACKET(HcFdoExt).OptionFlags;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SuspendController != NULL);

        if (MPF(HcFdoExt).MINIPORT_SuspendController == NULL) {
            break;
        }

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, irql);

        if (!TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_STARTED)) {
            // only suspend if started
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, irql);
            break;
        }


        if (TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED)) {
            // already suspended, bail
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, irql);
            break;
        }

        SET_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED);

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, irql);

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        MPh_AcquireMpSuspendResumeLock(HcFdoExt);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'mpS+', 0, 0, 0);
        USBPORT_ASSERT(HcFdoExt->Fdo.MpResumeThread == NULL);
        HcFdoExt->Fdo.MpSuspendThread = KeGetCurrentThread();

        // introduce a 10ms wait here to allow any port suspends to finish -- according to spec
        USBPORT_Wait(HcFdoExt->HcFdoDeviceObject, 10);

        if (EnableRH_IRQ) {
            MPRH_EnableIrq(HcFdoExt, ROOTHUB_LOCK_NOT_ACQUIRED);
        }

        if (TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_SUSPEND_DISABLE_IRQ)) {
            //
            // Prevent the race condition where the miniport ISR or DPC
            // runs while in the process of suspending the controller.

            // Disable the host controller interrupts.  The miniport
            // will take care of reenabling the RH port status change
            // interrupt when it has finished suspending the controller.
            //
            KeAcquireSpinLock(&HcFdoExt->Fdo.IsrDpcSpin, &irql);

            MPF(HcFdoExt).MINIPORT_DisableInterrupts(HcFdoExt->Fdo.MiniportDeviceData);

            KeReleaseSpinLock(&HcFdoExt->Fdo.IsrDpcSpin, irql);

            // Flush DPCs to flush the ISR DPC
            //
            KeFlushQueuedDpcs();
        }

        // Wait for any outstanding AsyncCallbacks to complete before
        // calling MINIPORT_SuspendController().
        //
        KeWaitForSingleObject(&HcFdoExt->IoRequestAsyncCallbackEvent,
                              Executive,
                              KernelMode,
                              FALSE,
                              NULL);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'mpS1', 0, 0, 0);
        MPF(HcFdoExt).MINIPORT_SuspendController(HcFdoExt->Fdo.MiniportDeviceData);
        LOGENTRY(hcFdo, LOG_TRANSFER, 'mpS2', 0, 0, 0);

        USBPORT_ASSERT(HcFdoExt->Fdo.MpSuspendThread != NULL);
        HcFdoExt->Fdo.MpSuspendThread = NULL;

        MPh_ReleaseMpSuspendResumeLock(HcFdoExt);

    } WHILE (0);

}


/*

Routine: MINIPORT_ResumeController

USB_MINIPORT_STATUS MINIPORT_ResumeController(
    PDEVICE_DATA HcContext
    );


Description:

    Attempts to resume HC HW from the suspend state.  The miniport may fail this in the event the controller has been
    hosed by the BIOS.  In that even the port driver may attempt to power cycle.

HCI_Revison - all.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     miniport status code (success or error)


*/

USB_MINIPORT_STATUS
MPf_ResumeController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_FAILURE;
    KIRQL dm_irql;
    PDEVICE_OBJECT hcFdo;

    hcFdo = HcFdoExt->HcFdoDeviceObject;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_ResumeController != NULL);

        if (MPF(HcFdoExt).MINIPORT_ResumeController == NULL) {
            break;
        }

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dm_irql);

        if (TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED)) {
            CLEAR_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED);
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dm_irql);

        } else {
            // not suspended, assume we are resumed
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dm_irql);
            mps = USBMP_STATUS_SUCCESS;

            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        //
        // MINIPORT_ResumeController would touch the controller hardware registers to resume the
        // controller. At this time, we must ensure that the controller devnode is not in D3.
        // If the controller is in D3 it is possible that the power to the controller and
        // Phys is off. Thus it is not Ok to touch the hardware registers unless the controller
        // is in D0.
        // Thus verify that at the time we are calling the resume controller the
        // Run time PM state should be active a.k.a the controller would be in D0.
        //
        USBPORT_RuntimePMStateActiveAssert(hcFdo);

        MPh_AcquireMpSuspendResumeLock(HcFdoExt);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'mpR+', 0, 0, 0);
        USBPORT_ASSERT(HcFdoExt->Fdo.MpSuspendThread == NULL);
        HcFdoExt->Fdo.MpResumeThread = KeGetCurrentThread();

        mps = MPF(HcFdoExt).MINIPORT_ResumeController(HcFdoExt->Fdo.MiniportDeviceData);

        LOGENTRY(hcFdo, LOG_TRANSFER, 'mpR-', 0, 0, mps);
        USBPORT_ASSERT(HcFdoExt->Fdo.MpResumeThread != NULL);
        HcFdoExt->Fdo.MpResumeThread = NULL;

        MPh_ReleaseMpSuspendResumeLock(HcFdoExt);

    } WHILE (0);


    return mps;
}

/*

Routine: MINIPORT_ReadCfgFlag

ULONG MINIPORT_ReadCfgFlag(
    PDEVICE_DATA HcContext
    );


Description:

    Return the value of the USB2 config flag register.

Core_Lock - no.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     Current value of config flag register. Zero if config flag not supported by this hardware.


*/

ULONG
MPf_ReadCfgFlag(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG cf = 0;

    do {
        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_ReadCfgFlag == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        // only read if the USB20 (ehci) flag is set.
        if (USBPORT_IS_USB20(HcFdoExt)) {
            USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);
            cf = MPF(HcFdoExt).MINIPORT_ReadCfgFlag(HcFdoExt->Fdo.MiniportDeviceData);
        }

    } WHILE (0);

    return cf;
}


/*

Routine: MINIPORT_PollController

VOID MINIPORT_PollController(
    PDEVICE_DATA HcContext
    );


Description:

     Optional Poll routine for miniport this function will be called at the MiniportPollInterval specficeid in
     the registration packet.

     Specifying a value of zero in the registration packet disables polling of the controller.

Core_Lock - no.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none.


*/

VOID
MPf_PollController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        if (MPF(HcFdoExt).MINIPORT_PollController == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        // The Fdo.MpRootHubFunctionSpin spinlock must always be held when
        // USBPORTSVC_InvalidateRootHub() is called and
        // MINIPORT_PollController() can call USBPORTSVC_InvalidateRootHub().
        //
        //
        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject,
                                &HcFdoExt->Fdo.MpRootHubFunctionSpin, &irql);

        MPF(HcFdoExt).MINIPORT_PollController(HcFdoExt->Fdo.MiniportDeviceData);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject,
                                &HcFdoExt->Fdo.MpRootHubFunctionSpin, irql);

    } WHILE (0);

}



/*

Routine: MINIPORT_Get32BitFrameNumber

ULONG MINIPORT_Get32BitFrameNumber(
    PDEVICE_DATA HcContext
    );


Description:

    Return the value of the USB 32 bit frame counter.

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     Current frame number.


*/

ULONG
MPf_Get32BitFrameNumber(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG current_frame = 0;
    KIRQL irql, dmirql;

    do {

        if (MPF(HcFdoExt).MINIPORT_Get32BitFrameNumber == NULL) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            current_frame = 0xFFFFFFFF;
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.PowerFlags.Suspended ||
            HcFdoExt->Fdo.PowerFlags.Off ||
            HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {

            current_frame = 0xFFFFFFFF;
        } else {
            current_frame = MPF(HcFdoExt).MINIPORT_Get32BitFrameNumber(
                                                       HcFdoExt->Fdo.MiniportDeviceData);
        }

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);


    } WHILE (0);

    return current_frame;
}

//#ifdef VISTA_RTM

/*

Routine: MINIPORT_Get32BitMicroFrameNumber

ULONG MINIPORT_Get32BitMicroFrameNumber(
    PDEVICE_DATA HcContext
    );


Description:

    Return the value of the USB 32 bit frame counter and microframe counter.

Core_Lock - yes.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

HsFrameCounter - high speed frame counter with microframes on the end (not shifted off)

Returns:

     high speed frame counter with microframes on the end (not shifted off).


*/

ULONG
MPf_Get32BitMicroFrameNumber(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    ULONG current_hs_frame = 0;
    KIRQL irql, dmirql;

    do {


        if (MPF(HcFdoExt).MINIPORT_Get32BitMicroFrameNumber == NULL) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            current_hs_frame = 0xFFFFFFFF;
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.PowerFlags.Suspended ||
            HcFdoExt->Fdo.PowerFlags.Off ||
            HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {

            current_hs_frame = 0xFFFFFFFF;
        } else {
            current_hs_frame = MPF(HcFdoExt).MINIPORT_Get32BitMicroFrameNumber(
                                                       HcFdoExt->Fdo.MiniportDeviceData);
        }

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);


    } WHILE (0);

    return current_hs_frame;
}

//#endif

/*

Routine: MINIPORT_InterruptService

BOOLEAN
MINIPORT_InterruptService (
    PDEVICE_DATA DeviceData
    );

Description:


    Interrupt service routine for HC hardware interrupt.  If TRUE is returned the ISR DPC is queued to handle the
    interrupt.

Core_Lock - no.

HCI_Revison - all.

Irql - IRQ_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     TRUE if controller generated the interrupt.


*/

BOOLEAN
MPf_InterruptService(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    BOOLEAN usb_int = FALSE;

    do {


        if (MPF(HcFdoExt).MINIPORT_InterruptService == NULL) {
            break;
        }

        usb_int = MPF(HcFdoExt).MINIPORT_InterruptService( HcFdoExt->Fdo.MiniportDeviceData);


    } WHILE (0);

    return usb_int;
}

/*

Routine: MINIPORT_InterruptDpcEx

ULONG
MINIPORT_InterruptDpcEx(
    PDEVICE_DATA DeviceData,
    BOOLEAN EnableInterrupts
    );

Description:

    Called by port in response to an interrupt generated by the HW.

    This function is not serialize with other core functions since which allow it to call services in USBPORT that
    result in immediate calls to core functions.

    The port driver passes a flag to indicate if interrupts need to be re-enabled on completion of this routine.
    This is to handle the rare case where interrupts have been disabled just after the ISR has queued a DPC.

Core_Lock - no.

HCI_Revison - 3+.

Irql - DPC_LEVEL

Parameters:

HcContext - Host Controller Context.

EnableInterrupts -  TRUE if interrupts should be enabled on return.

Returns:

     flags indicating source of irq.


*/

ULONG
MPf_InterruptDpcEx(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in BOOLEAN EnableInterrupts
    )
{
    ULONG flags = 0;

    do {


        if (MPF(HcFdoExt).MINIPORT_InterruptDpcEx == NULL) {
            break;
        }

        flags = MPF(HcFdoExt).MINIPORT_InterruptDpcEx(HcFdoExt->Fdo.MiniportDeviceData,
                                                      EnableInterrupts);


    } WHILE (0);

    return flags;
}

/*

Routine: MINIPORT_CheckController

VOID
MINIPORT_CheckController(
    PDEVICE_DATA DeviceData
    );

Description:

    Entry point called by usbport periodically to check the hardware state, this function is not serialized.
    Typically this is used to detect surprise removal of the hardware.

Core_Lock - no.

HCI_Revison - All.

Irql - DPC_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none.


*/

VOID
MPf_CheckController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        if (MPF(HcFdoExt).MINIPORT_CheckController == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, irql);

        if (!HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {

            USBPORT_RELEASE_DM_LOCK(HcFdoExt, irql);

            MPF(HcFdoExt).MINIPORT_CheckController(HcFdoExt->Fdo.MiniportDeviceData);

        } else {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, irql);
        }

    } WHILE (0);

}


/*

Routine: MINIPORT_InterruptNextSOF

VOID
MINIPORT_InterruptNextSOF(
    PDEVICE_DATA DeviceData
    );

Description:

    Requests a hardware interrupt at the next SOF interval.

Core_Lock - yes.

HCI_Revison - All.

Irql - DPC_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

     none.


*/

VOID
MPf_InterruptNextSOF(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql, dmirql;

    do {


        if (MPF(HcFdoExt).MINIPORT_InterruptNextSOF == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);


        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        //
        // If the controller is suspended or otherwise not running, then there won't be a next SOF,
        // and thus there won't be another interrupt
        //
        if ((!TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_STARTED)) ||
             (TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_SUSPENDED)) ||
             (TEST_FLAG(HcFdoExt->Fdo.MpStateFlags, MP_STATE_IN_RESET_RECOVERY)) ||
             HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

            //
            // This means that the interrupt won't fire, so only the DM Timer
            // can trigger the next operation, which happens to only be an
            // endpoint state change.  However, if the controller is in a
            // low power state, the DM Timer may have been disabled.
            // Instead, we'll just force the DM Timer DPC routine to run now
            //

            KeInsertQueueDpc(&HcFdoExt->Fdo.DM_SimulateTimerDpc,0,0);

            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        MPF(HcFdoExt).MINIPORT_InterruptNextSOF(HcFdoExt->Fdo.MiniportDeviceData);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}

/*

Routine: MINIPORT_SetWakeOnConnect

USB_MINIPORT_STATUS MINIPORT_SetWakeOnConnect(
    PDEVICE_DATA HcContext,
    ULONG EnableFlag
    );


Description:

    This routine will enable or disble wake on connect/disconnect events.

Core_Lock - no.

HCI_Revison - 3+.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

EnableFlag - 1 turns ON wake on connect change. 0 turns OFF wake on connect change.

Returns:

    success if feature is supported by hw.

*/

RHSTATUS
MPf_SetWakeOnConnect(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in ULONG EnableFlag
    )
{
    NTSTATUS nts = STATUS_NOT_SUPPORTED;
    RHSTATUS rhs;

    do {
        USB_MINIPORT_STATUS mps;

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        if (MPF(HcFdoExt).MINIPORT_SetWakeOnConnect == NULL) {
            break;
        }

        mps = MPF(HcFdoExt).MINIPORT_SetWakeOnConnect(HcFdoExt->Fdo.MiniportDeviceData, EnableFlag);

        nts = USBPORT_MiniportStatus_TO_NtStatus(mps);

    } WHILE (0);

    if (NT_ERROR(nts)) {
        rhs = RH_STALL;
    } else {
        rhs = RH_SUCCESS;
    }

    return rhs;
}

/*

Routine: MINIPORT_PreStartController

USB_MINIPORT_STATUS MINIPORT_PreStartController(
    PDEVICE_DATA HcContext,
    PHC_RESOURCES HcResources
    );

Description:

    Pre start callback called by USBPORT in order for the miniport to query
    hardware capabilities (related to DMA, for example), with only the memory
    mapped resources being available at the time of the call.

    This is called once during the start process and not during restart.

Core_Lock - no.

HCI_Revison - all.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

HcResources - Pnp allocated memory resources only.

StartFlags - may be one of: MPF_RESTART, MPF_ON_PNP_THREAD

Returns:

    success if capabilities are queried successfully by the miniport.

*/

USB_MINIPORT_STATUS
MPf_PreStartController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHC_RESOURCES HcResources,
    __in ULONG StartFlags
    )
{
    USB_MINIPORT_STATUS mps;

    ASSERT_PASSIVE();

    if ((MPx_MpRevision(HcFdoExt) >= USB_MINIPORT_HCI_VERSION_5) &&
        (MPF(HcFdoExt).MINIPORT_PreStartController != NULL)) {

        mps = MPF(HcFdoExt).MINIPORT_PreStartController(HcFdoExt->Fdo.MiniportDeviceData, HcResources);

    } else {

        mps = STATUS_NOT_IMPLEMENTED;
    }

    LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_PNP, 'mpPS', mps, 0, 0);
    return mps;
}

/*

Routine: MINIPORT_StartController

USB_MINIPORT_STATUS MINIPORT_StartController(
    PDEVICE_DATA HcContext,
    PHC_RESOURCES HcResources
    );

Description:

    Allow miniport to initialize hardware, allocate memory etc. On return (STATUS_SUCCESS) the device is considered
    started and powered and must handle ALL CORE functions. The miniport should disable any BIOS if present. The last
    thing the driver should do before returning is enable interrupt generation by the controller.

Core_Lock - no.

HCI_Revison - all.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

HcResources - Pnp allocated hardware and memory resources.

StartFlags - may be one of: MPF_RESTART, MPF_ON_PNP_THREAD

Returns:

    success if controller is started.

*/

USB_MINIPORT_STATUS
MPf_StartController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHC_RESOURCES HcResources,
    __in ULONG StartFlags
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_FAILURE;
    ULONG i;

    ASSERT_PASSIVE();

    do {

        if (MPF(HcFdoExt).MINIPORT_StartController == NULL) {
            break;
        }

        HcResources->Restart = FALSE;
        if (StartFlags & MPF_RESTART) {
            // indicate to miniport this is a restart
            HcResources->Restart = TRUE;
        }

        if (StartFlags & MPF_ON_PNP_THREAD) {
            // indicate to miniport this is a restart
            HcFdoExt->Fdo.Flags.OnPnpThread = 1;
        }

        // Zero the miniport common buffer blocks.
        //
        if (HcResources->CommonBufferVa != NULL) {
            RtlSecureZeroMemory(HcResources->CommonBufferVa,
                                REGISTRATION_PACKET(HcFdoExt).CommonBufferBytes);
        }

        for (i = 0; i < HcResources->CommonBufferBlockCount; i++) {
            if (HcResources->CommonBufferBlocksVa[i] != NULL) {
                RtlSecureZeroMemory(HcResources->CommonBufferBlocksVa[i],
                                    REGISTRATION_PACKET(HcFdoExt).CommonBufferBlockBytes[i]);
            }
        }

        // Now call the miniport to start the host controller.
        //
        mps = MPF(HcFdoExt).MINIPORT_StartController(HcFdoExt->Fdo.MiniportDeviceData, HcResources);

    } WHILE (0);

    HcFdoExt->Fdo.Flags.OnPnpThread = 0;
    HcResources->Restart = FALSE;

    LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_PNP, 'mpST', mps, 0, 0);

    return mps;
}


/*

Routine: MINIPORT_StopController

VOID MINIPORT_StopController(
    PDEVICE_DATA HcContext,
    BOOLEAN HardwarePresent
    );

Description:

    Allow miniport to disconnect interrupt, free memory etc. On return
    (non Failable) the device is considered stopped and powered down and
    will no longer receive calls to CORE functions.

<pre>
    NOTES:

    - The miniport will only receive a stop request if it was
      successfuly started.

    Currently Called From:              At Level:
    +-----------------------------------+-------------------------------+
    | USBPORT_StopDevice()              | PASSIVE_LEVEL                 |
    +-----------------------------------+-------------------------------+
    | USBPORT_ResumeUsbController()     | PASSIVE_LEVEL                 |
    +-----------------------------------+-------------------------------+
    | USBPORT_TurnUsbControllerOff()    | DISPATCH_LEVEL (on chk krnl)  |
    +-----------------------------------+-------------------------------+

    MP_DisableInterrupts() is called before MPf_StopController() is
    called.

</pre>

Core_Lock - no.

HCI_Revison - all.

Parameters:

HcContext - Host Controller Context.

HardwarePresent - If Hardware Present is FALSE the miniport should not
                  access memory registers or ioports.

Returns:

    none.

*/

VOID
MPf_StopController(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __in BOOLEAN HardwarePresent
    )
{
    do {

        if (MPF(HcFdoExt).MINIPORT_StopController == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        // Cancel all outstanding AsyncCallbacks before calling
        // MINIPORT_StopController().
        //
        USBPORT_CancelAllAsyncTimerCallbacks(HcFdoExt->HcFdoDeviceObject);

        MPF(HcFdoExt).MINIPORT_StopController(HcFdoExt->Fdo.MiniportDeviceData, HardwarePresent);

    } WHILE (0);

}


/*

Routine: MINIPORT_StopControllerWithInterrupts

VOID MINIPORT_StopControllerWithInterrupts(
    PDEVICE_DATA HcContext
    );

Description:

    Allow miniport to do whatever operations are needed to stop the controller
    while interrupts are enabled.  If the hardware isn't present, this function
    will not be called

    This was added for Wireless USB.

<pre>
    NOTES:
        - The miniport will only receive a stop request if it was successfuly started.
        - Miniport should disable all interrupts from the hardware.
</pre>

Core_Lock - no.

HCI_Revison - all.

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.


Returns:

    none.

*/

VOID
MPf_StopControllerWithInterrupts(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_310) {
            break;
        }
        if (MPF(HcFdoExt).MINIPORT_StopControllerWithInterrupts == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        MPF(HcFdoExt).MINIPORT_StopControllerWithInterrupts(HcFdoExt->Fdo.MiniportDeviceData);

    } WHILE (0);

}

/*

Routine: MINIPORT_FlushInterrupts

VOID MINIPORT_FlushInterrupts(
    PDEVICE_DATA HcContext
    );


Description:

    Miniport should acknowlege any outstanding interrupts on the hardware.  This routine is called just prior to
    disconnecting the irq on unload or suspend.

Core_Lock - no. (serialized thru pnp)

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    None

*/
VOID
MPf_FlushInterrupts(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_FlushInterrupts != NULL);

    if (MPF(HcFdoExt).MINIPORT_FlushInterrupts) {
        MPF(HcFdoExt).MINIPORT_FlushInterrupts(HcFdoExt->Fdo.MiniportDeviceData);
    }
}



/*

Routine: MINIPORT_QueryEndpointRequirements

VOID MINIPORT_QueryEndpointRequirements(
    PDEVICE_DATA DeviceData,
    PENDPOINT_PARAMETERS EndpointParameters,
    PENDPOINT_REQUIREMENTS EndpointRequirements
    );

Description:
Description:

    Allows miniport to indicate how much memory it will need to support a given set of transfers.

<pre>
The following applies if USBX flag is set:

Notes on parameters:

    EndpointParameters.MAXTRANSFERSIZE is largest single transfer this endpoint can handle. If a function driver passes
    down a larger transfer usbport will fail the transfer request.
    default =

    EndpointParameters.MINCOMMONBUFFERBYTES is the minimum size of the common buffer block allocated for this endpoint.

Control:
    This value must be 64k for control endpoints.
    default =

    EndpointParameters.MAXTRANSFERSIZE can never be larger than 64k by spec. The default value is 64k.

Interrupt:
Bulk:
    EndpointParameters.MAXTRANSFERSIZE can be any value. No transfer will be passed in larger than this value.
    default =

Isoch:
    EndpointParameters.MAXTRANSFERSIZE is the largest size of the buffer that encompasses all of the isochronous
    packets.
    default =

----------------------------------------------------------------------------------------------------------------------
The following applies if USBX flag is NOT set (for backward compatibilty with older miniports):

Notes on parameters:

    EndpointParameters.MAXTRANSFERSIZE is largest single transfer this endpoint can handle. If a function driver passes
    down a larger transfer usbport will fail the transfer request.

    EndpointParameters.MINCOMMONBUFFERBYTES is the minimum size of the common buffer block allocated for this endpoint.

Control:
    This value must be 64k for control endpoints.

    EndpointParameters.MAXTRANSFERSIZE can never be larger than 64k by spec. The default value is 64k.

Interrupt:
Bulk:
    EndpointParameters.MAXTRANSFERSIZE can be any value. No transfer will be passed in larger than this value.


Isoch:
    EndpointParameters.MAXTRANSFERSIZE is the largest size of the buffer that encompasses all of the isochronous
    packets.

</pre>

Core_Lock - no.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EndpointParameters - Describes the endpoints.

EndpointRequirements - filled in by miniport with the limits of what the miniport can handle.

Returns:

    none.

*/

VOID
MPf_QueryEndpointRequirements(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout PENDPOINT_REQUIREMENTS Requirements
    )
{
    KIRQL irql;

    USBPORT_ASSERT(!USBPORT_IS_USBX(HcFdoExt));
    USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_QueryEndpointRequirements != NULL);

    USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
    USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);
    MPF(HcFdoExt).MINIPORT_QueryEndpointRequirements(HcFdoExt->Fdo.MiniportDeviceData,
                                                                                        &Endpoint->Parameters,
                                                                                        Requirements);
    USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
}

/*

Routine: MINIPORT_OpenEndpoint

USB_MINIPORT_STATUS MINIPORT_OpenEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_PARAMETERS EpParameters,
    PENDPOINT_DATA EpContext
    );


Description:

    Legacy (old style) open endpoint request.

Core_Lock - no.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpParameters - New parameters for the endpoint.

EpContext - Hc endpoint Context.

Returns:

    miniport status code for the opertaion.

*/
USB_MINIPORT_STATUS
MPf_OpenEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __inout_opt USBD_STATUS *UsbdStatus
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;
    KIRQL irql = PASSIVE_LEVEL;
    ULONG mpOptionFlags;
    BOOLEAN passive;

    USBPORT_ASSERT(!USBPORT_IS_USBX(HcFdoExt));
    USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_OpenEndpoint != NULL);

    mpOptionFlags = REGISTRATION_PACKET(HcFdoExt).OptionFlags;
    passive = TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_EP_OPENCLOSE_PASSIVE);

    if (MPF(HcFdoExt).MINIPORT_OpenEndpoint) {

        //
        // Wireless USB needs Open and Close to be able to block, which means
        // we don't want to call with the spin lock held.  Synchronization is
        // handled by the endpoint state machine and the miniport.
        //
        if (!passive) {

            USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        }

        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        mps = MPF(HcFdoExt).MINIPORT_OpenEndpoint(HcFdoExt->Fdo.MiniportDeviceData,
                                                  &Endpoint->Parameters,
                                                  &Endpoint->MiniportEndpointData[0]);

        if (!passive) {

            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
        }

    }

    if (UsbdStatus) {
        *UsbdStatus = USBPORT_MiniportStatus_TO_USBDStatus(mps);
    }

    return mps;
}


/*

Routine: MINIPORT_DbgFreeEndpoint

VOID MINIPORT_DbgFreeEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );


Description:

    (Optional) Called just prior to releasing endpoint memory.  This is used to sanity check that the endpoint is not
    still active in the hardware.


Core_Lock - yes.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

Returns:

    none


*/
VOID
MPf_DbgFreeEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql;

    do {
        if (Endpoint->MpOpenRef == FALSE) {
            break;
        }
        Endpoint->MpOpenRef = FALSE;

        if (MPF(HcFdoExt).MINIPORT_DbgFreeEndpoint == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        MPF(HcFdoExt).MINIPORT_DbgFreeEndpoint(HcFdoExt->Fdo.MiniportDeviceData,
                                               &Endpoint->MiniportEndpointData[0]);

#if DBG
        // this will flush out cases where the miniport may still be accessing the endpoint after close/free
        RtlFillMemory(&Endpoint->MiniportEndpointData[0], Endpoint->MiniportEndpointDataSize, 'e');
#endif
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: MINIPORT_HaltController

VOID MINIPORT_HaltController(
    PDEVICE_DATA HcContext
    );


Description:

    Halts the controller. This prevents race conditions where the ep structs are freed while the HC is still accessing
    them.

Core_Lock - yes.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    none


*/
VOID
MPf_HaltController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        if (MPF(HcFdoExt).MINIPORT_HaltController == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);
        MPF(HcFdoExt).MINIPORT_HaltController(HcFdoExt->Fdo.MiniportDeviceData);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}



/*

Routine: MINIPORT_PokeEndpoint

USB_MINIPORT_STATUS MINIPORT_PokeEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_PARAMETERS EpParameters,
    PENDPOINT_DATA EpContext
    );


Description:

    Legacy (old style) poke endpoint request. Used to change the address of an endpoint without removing it from
    the schedule.

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpParameters - New parameters for the endpoint.

EpContext - Hc endpoint Context.

Returns:

    miniport status code.

*/
USB_MINIPORT_STATUS
MPf_PokeEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    USB_MINIPORT_STATUS mps = USBMP_STATUS_NOT_SUPPORTED;
    KIRQL irql;

    USBPORT_ASSERT(!USBPORT_IS_USBX(HcFdoExt));
    USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_PokeEndpoint != NULL);

    if (MPF(HcFdoExt).MINIPORT_PokeEndpoint) {
        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);
        mps = MPF(HcFdoExt).MINIPORT_PokeEndpoint(HcFdoExt->Fdo.MiniportDeviceData,
                                                    &Endpoint->Parameters,
                                                    &Endpoint->MiniportEndpointData[0]);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    }

    return mps;
}


/*

Routine: MINIPORT_CloseEndpoint

VOID MINIPORT_CloseEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );


Description:

    Indicates an endpoint is closed to a miniport.  This function has existed since v1 of the interface, however,
    it was never called.  This routine is only called if the miniport specifies miniport v3 interface or higher.

Core_Lock - yes.

HCI_Revison - 3+.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

Returns:

    none.

*/
VOID
MPf_CloseEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql = PASSIVE_LEVEL; // keep the compiler happy
    ULONG mpOptionFlags;
    BOOLEAN passive;

    mpOptionFlags = REGISTRATION_PACKET(HcFdoExt).OptionFlags;
    passive = TEST_FLAG(mpOptionFlags, USB_MINIPORT_OPT_EP_OPENCLOSE_PASSIVE);

    do {

        if (MPx_MpRevision(HcFdoExt) < USB_MINIPORT_HCI_VERSION_3) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_CloseEndpoint != NULL);

        if (MPF(HcFdoExt).MINIPORT_CloseEndpoint == NULL) {
            break;
        }

        if (Endpoint->MpOpenRef == FALSE) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        //
        // Wireless USB needs Open and Close to be able to block, which means
        // we don't want to call with the spin lock held.  Synchronization is
        // handled by the endpoint state machine and the miniport.
        //

        if (!passive) {

            USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        }

        MPF(HcFdoExt).MINIPORT_CloseEndpoint(HcFdoExt->Fdo.MiniportDeviceData,
                                             &Endpoint->MiniportEndpointData[0]);

        if (!passive) {

            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
        }

    } WHILE (0);

}


/*

Routine: MINIPORT_SetEndpointState

VOID MINIPORT_SetEndpointState(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    MP_ENDPOINT_STATE NewEpState
    );


Description:

    Set an endpoint to one of our defined transfer states, the endpoint need not be in the state when the miniport
    returns. For USB11 there is an assumption here that the enpoint will reach the desired state on the next SOF, the
    port will keep track of this and assume that the state transition has occurred after one ms frame has passed.
    If the USBX flag is set tate is transition is left to the miniport and may be asynchronous. The port driver will
    poll the miniport via MP_UsbxCheckHwSync.

<pre>

    These software state changes can only be intiated by a  request thru the MINIPORT_SetEndpointState function.
    Endpoints cannot transition on their own.

    ENDPOINT_IDLE
        The endpoint has no active transfer, set this endpoint to a state that generates minimal activity on the
        contoller (ie remove it from the schedule,set skip bit etc).

    ENDPOINT_PAUSE
        Temporarily stop any bus activity associated with the endpoint, this is a prelude to receiving an AbortTransfer.

    ENDPOINT_ACTIVE
        Enable processing of the enpoint -- ie it is in the schedule and ready for action.

    ENDPOINT_REMOVE
        The endpoint has been removed from the HW schedule.


Not all endpoint changes require an interrupt (hardware sync), by only waiting on those that need it we are able to
decrease the latency associated with openaing an isoch endpoint.


+----------------------+---------------+----------------+----------------+----------------+---------------+
     current/Next      | ENDPOINT_IDLE |ENDPOINT_PAUSE  |ENDPOINT_ACTIVE |ENDPOINT_REMOVE |ENDPOINT_CLOSED|
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_IDLE      |      x        |                |                |                |               |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_PAUSE     |      i        |       x        |        i       |      sync      |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_ACTIVE    |      x        |      sync      |        x       |      sync      |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
    ENDPOINT_REMOVE    |      x        |       x        |                |       x        |      i        |
+----------------------+---------------+----------------+----------------+----------------+---------------+
   *ENDPOINT_CLOSED    |      x        |       x        |        i       |       i        |      x        |
+----------------------+---------------+----------------+----------------+----------------+---------------+

*ENDPOINT_CLOSED is initial state
x       = invalid
sync    = hw sync(ie next sof)
i       = immediate

</pre>

Core_Lock - yes.

HCI_Revison - all.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

NewEpState - new state for the endpoint.

Returns:

    none.

*/
VOID
MPf_SetEndpointState(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATE NewEpState
    )
{
    KIRQL irql;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SetEndpointState != NULL);

        if (MPF(HcFdoExt).MINIPORT_SetEndpointState == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        MPF(HcFdoExt).MINIPORT_SetEndpointState(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                NewEpState);

        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: MINIPORT_GetEndpointState

MPf_ENDPOINT_STATE MINIPORT_GetEndpointState(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );

Description:

    Returns the current hardware (miniport) state for the endpoint.

Core_Lock - yes.

HCI_Revison - All.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

Returns:

    endpoint state.

*/
MP_ENDPOINT_STATE
MPf_GetEndpointState(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql, dmirql;
    MP_ENDPOINT_STATE mp_state = ENDPOINT_ERROR;

    ASSERT_ENDPOINT(Endpoint);

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_GetEndpointState != NULL);

        if (MPF(HcFdoExt).MINIPORT_GetEndpointState == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            mp_state = ENDPOINT_ERROR;
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        mp_state = MPF(HcFdoExt).MINIPORT_GetEndpointState(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0]);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

    return mp_state;
}

/*

Routine: MINIPORT_GetEndpointStatus

MP_ENDPOINT_STATUS MINIPORT_GetEndpointStatus(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );

Description:

    Sets the status of an endpoint (HALTED or RUN) to a specific value. Typically the status of the endpoint is
    controlled by the HW.

Core_Lock - yes.

HCI_Revison - All.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

Returns:

    endpoint status.

    ENDPOINT_STATUS_RUN  - endpoint and controller are running and being accessed by the HC hardware.
    ENDPOINT_STATUS_HALT - endpoint is in the halted state, not being accessed by hw
    ENDPOINT_STATUS_ERROR - endpoint or controller is in an error state
    ENDPOINT_STATUS_ERROR_HC_HALTED - hc hardware is in the halted state.

*/
MP_ENDPOINT_STATUS
MPf_GetEndpointStatus(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql, dmirql;
    MP_ENDPOINT_STATUS mp_status = ENDPOINT_STATUS_ERROR;

    ASSERT_ENDPOINT(Endpoint);

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_GetEndpointStatus != NULL);

        if (MPF(HcFdoExt).MINIPORT_GetEndpointStatus == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        mp_status = MPF(HcFdoExt).MINIPORT_GetEndpointStatus(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0]);

        Endpoint->CurrentStatus = mp_status;
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

    return mp_status;
}


/*

Routine: MINIPORT_SetEndpointStatus

VOID MINIPORT_SetEndpointStatus(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    MP_ENDPOINT_STATUS EpStatus
    );

Description:

    Sets the status of an endpoint (HALTED or RUN) to a specific value. Typically the status of the endpoint is
    controlled by the HW.

Core_Lock - yes.

HCI_Revison - All.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

EpStatus - new endpoint status

Returns:

    none.

*/
VOID
MPf_SetEndpointStatus(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in MP_ENDPOINT_STATUS EpStatus
    )
{
    KIRQL irql, dmirql;

    ASSERT_ENDPOINT(Endpoint);

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SetEndpointStatus != NULL);

        if (MPF(HcFdoExt).MINIPORT_SetEndpointStatus == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        Endpoint->CurrentStatus = EpStatus;
        MPF(HcFdoExt).MINIPORT_SetEndpointStatus(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                EpStatus);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: MINIPORT_SetEndpointDataToggle

VOID MINIPORT_SetEndpointDataToggle(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext,
    ULONG ToggleValue
    );

Description:

    Sets the USB data toggle for a given endpoint to the specified value (0 or 1).

Core_Lock - yes.

HCI_Revison - All.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

ToggleValue - new data toggle value 0 or 1.

Returns:

    none.

*/
VOID
MPf_SetEndpointDataToggle(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint,
    __in ULONG ToggleValue
    )
{
    KIRQL irql, dmirql;

    ASSERT_ENDPOINT(Endpoint);

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_SetEndpointDataToggle != NULL);

        if (MPF(HcFdoExt).MINIPORT_SetEndpointDataToggle == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        if (ToggleValue > 1) {
            ToggleValue = 1;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        MPF(HcFdoExt).MINIPORT_SetEndpointDataToggle(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0],
                                                ToggleValue);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}

/*

Routine: MINIPORT_PollEndpoint

VOID MINIPORT_PollEndpoint(
    PDEVICE_DATA HcContext,
    PENDPOINT_DATA EpContext
    );

Description:

     Poll the endpoint for done transfers or other changes.

HCI_Revison - All.

Core_Lock - yes.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

EpContext - Hc endpoint Context.

Returns:

    none.

*/
VOID
MPf_PollEndpoint(
    __inout PDEVICE_EXTENSION HcFdoExt,
    __inout PHCD_ENDPOINT Endpoint
    )
{
    KIRQL irql, dmirql;

    ASSERT_ENDPOINT(Endpoint);

    do {

        if (Endpoint->EpFlags.RootHub) {
            break;
        }

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_PollEndpoint != NULL);

        if (MPF(HcFdoExt).MINIPORT_PollEndpoint == NULL) {
            break;
        }

        if (HcFdoExt->Fdo.PowerFlags.MpRestartFailed) {
            break;
        }

        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);

        USBPORT_ACQUIRE_DM_LOCK(HcFdoExt, dmirql);

        if (HcFdoExt->Fdo.DmTimerFlags.DecodesOff) {
            USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);
            USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);
            break;
        }

        USBPORT_RELEASE_DM_LOCK(HcFdoExt, dmirql);

        MPF(HcFdoExt).MINIPORT_PollEndpoint(
                                                HcFdoExt->Fdo.MiniportDeviceData,
                                                &Endpoint->MiniportEndpointData[0]);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: MINIPORT_ResetController

VOID MINIPORT_ResetController(
    PDEVICE_DATA HcContext
    );

Description:

     Reset the host controller hardware (slegehammer reset).

     This function is serialized with the ISR and DPC and holds the core function lock in order to provide a
     safe environment for the miniport to reset the host hardware and rebuild the schedule.

Core_Lock - yes.

HCI_Revison - All.

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    none.

*/
VOID
MPf_ResetController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_ResetController != NULL);

        if (MPF(HcFdoExt).MINIPORT_ResetController == NULL) {
            break;
        }

        LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_MISC, 'rset',
            HcFdoExt->HcFdoDeviceObject, HcFdoExt->Fdo.Flags.ul, 0);
        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_ResetController(
                                                HcFdoExt->Fdo.MiniportDeviceData);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}

/*

Routine: MINIPORT_ResetControllerDontReinit

VOID MINIPORT_ResetControllerDontReinit(
    PDEVICE_DATA HcContext
    );

Description:

     Reset the host controller hardware (slegehammer reset), however dont reinitalize it.

     This function is serialized with the ISR and DPC and holds the core function lock in order to provide a
     safe environment for the miniport to reset the host hardware and rebuild the schedule.

Core_Lock - yes.

HCI_Revison - 3.10 +

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    none.

*/
VOID
MPf_ResetControllerDontReinit(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_ResetControllerDontReinit != NULL);

        if (MPF(HcFdoExt).MINIPORT_ResetControllerDontReinit == NULL) {
            break;
        }

        LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_MISC, 'Rset',
            HcFdoExt->HcFdoDeviceObject, HcFdoExt->Fdo.Flags.ul, 0);
        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_ResetControllerDontReinit(
                                                HcFdoExt->Fdo.MiniportDeviceData);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}

/*

Routine: MINIPORT_PostResetController

VOID MINIPORT_ResetController(
    PDEVICE_DATA HcContext
    );

Description:

     A call into miniport at Passive Level after the controller has been
     reset (slegehammer reset) but before it has been reinialized.

Core_Lock - NO.

HCI_Revison - 3.10 +

Irql - PASSIVE_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    none.

*/
VOID
MPf_PostResetController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_PostResetController != NULL);

        if (MPF(HcFdoExt).MINIPORT_PostResetController == NULL) {
            break;
        }

        LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_MISC, 'Rpos',
            HcFdoExt->HcFdoDeviceObject, HcFdoExt->Fdo.Flags.ul, 0);

        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_PostResetController(
                                                HcFdoExt->Fdo.MiniportDeviceData);

    } WHILE (0);

}

/*

Routine: MINIPORT_ReinitController

VOID MINIPORT_ReinitController(
    PDEVICE_DATA HcContext
    );

Description:

     Reinitalize the controller after the host controller hardware has been reset (slegehammer reset)

     This function is serialized with the ISR and DPC and holds the core function lock in order to provide a
     safe environment for the miniport to reset the host hardware and rebuild the schedule.

Core_Lock - yes.

HCI_Revison - 3.10 +

Irql - DISPATCH_LEVEL

Parameters:

HcContext - Host Controller Context.

Returns:

    none.

*/
VOID
MPf_ReinitController(
    __inout PDEVICE_EXTENSION HcFdoExt
    )
{
    KIRQL irql;

    do {

        USBPORT_ASSERT(MPF(HcFdoExt).MINIPORT_ReinitController != NULL);

        if (MPF(HcFdoExt).MINIPORT_ReinitController == NULL) {
            break;
        }

        LOGENTRY(HcFdoExt->HcFdoDeviceObject, LOG_MISC, 'Rint',
            HcFdoExt->HcFdoDeviceObject, HcFdoExt->Fdo.Flags.ul, 0);
        USBPORT_AcquireSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, &irql);
        USBPORT_ASSERT(HcFdoExt->Fdo.DmTimerFlags.DecodesOff == 0);

        MPF(HcFdoExt).MINIPORT_ReinitController(
                                                HcFdoExt->Fdo.MiniportDeviceData);
        USBPORT_ReleaseSpinLock(HcFdoExt->HcFdoDeviceObject, &HcFdoExt->Fdo.CoreFunctionSpin, irql);

    } WHILE (0);

}


/*

Routine: M