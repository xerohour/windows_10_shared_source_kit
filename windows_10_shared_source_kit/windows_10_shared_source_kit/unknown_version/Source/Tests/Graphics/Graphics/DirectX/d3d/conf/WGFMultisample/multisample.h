//////////////////////////////////////////////////////////////////////////
//  Multisample.h
//  created:	2005/07/01
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "D3D11Test.h"
#include <D3DSampleLocation.h>
#include "new_off.h"
#include "d3dx11.h"
#include "d3dx10.h"
#include "new_on.h"

#include <string>
#include <vector>

using namespace std;

#include <WGFTestCore.h>

#define FAILSKIP(x) CFailSkip::Check(x)
#define FAILSKIP_RESULT() CFailSkip::GetPrevResult()

struct VERTEX 
{
	D3DXVECTOR2 pos;
	D3DXVECTOR2 tex;
};

struct SUBPIXEL
{
	UINT x, y;  // actual subpixel position is ( x/256.0, y/256.0 )
	UINT size;  // width and height = size/256.0
};

// struct to organize format families
struct TypelessSubset
{
	DXGI_FORMAT typelessFormat;
	DXGI_FORMAT* pSubset;
	size_t subsetSize;
};

// used for resolution independent pixel positions.
// resolved on CMultisampleTest::ResolvePixPos()
enum PIX_POS {
	PP_CUSTOM,
	PP_TOP_LEFT,
	PP_TOP_RIGHT,
	PP_BOTTOM_LEFT,
	PP_BOTTOM_RIGHT
};

enum MS_READBACK {
	MS_LOAD,
	MS_RESOLVE,
	MS_PRESENT
};

enum QUALITY_LEVEL_SCENARIO
{
	QUALITY_LEVEL_ZERO,
	QUALITY_LEVEL_MAX_SUPPORTED,
	QUALITY_LEVEL_STANDARD,
	QUALITY_LEVEL_CENTER
};
DECLARE_NAMED_VALUES( QUALITY_LEVEL_SCENARIO );

enum SHADER_FREQUENCY { FREQUENCY_PIXEL, FREQUENCY_SAMPLE };

tstring __stdcall ToString( PIX_POS type );
bool __stdcall FromString( PIX_POS *pPos, const tstring &str );

tstring __stdcall ToString( MS_READBACK type );
bool __stdcall FromString( MS_READBACK *pPos, const tstring &str );

tstring __stdcall ToString( SHADER_FREQUENCY x );
bool __stdcall FromString( SHADER_FREQUENCY *x, const tstring &str );
 
// Wacky class to allow binding one param to multiple variables  I.E. keeping RT and swapchain dimensions in sync
template<class Type, UINT size >
class MultiParam
{
public:
	Type *Params[size];

	MultiParam()
	{
		ZeroMemory( Params, sizeof(Params) );
	}
	MultiParam( Type *pParams[size] ) 
	{
		for ( UINT i = 0; i < size; ++i )
		{
			Params[i] = pParams[i];
		}
	}
	MultiParam( const MultiParam<Type, size> &Arg )
	{
		ZeroMemory( Params, sizeof(Params) );
		
		Data = Arg.Data;
	}
	
	MultiParam( const Type &Arg )
	{
		Data = Arg;
	}

	MultiParam& operator= (const Type &Arg)
	{
		SetVal(Arg);
		return *this;
	}
	MultiParam& operator= (const MultiParam<Type, size> &Arg)
	{
		SetVal(Arg.Data);
		return *this;
	}

	friend bool operator== (const MultiParam<Type, size> &Arg1, const MultiParam<Type, size> &Arg2)
	{
		return Arg1.Data == Arg2.Data;
	}
	friend bool operator== (const MultiParam<Type, size> &Arg1, const Type &Arg2)
	{
		return Arg1.Data == Arg2;
	}

	friend bool operator< (const MultiParam<Type, size> &Arg1, const MultiParam<Type, size> &Arg2)
	{
		return Arg1.Data < Arg2.Data;
	}
	friend bool operator< (const MultiParam<Type, size> &Arg1, const Type &Arg2)
	{
		return Arg1.Data < Arg2;
	}

	operator Type() 
	{ 
		return Data;
	}

private:
	Type Data;

	void SetVal( const Type &Arg )
	{
		Data = Arg;
		for ( UINT i = 0; i < size; ++i )
		{
			if ( !Params[i] )
				continue;

			*Params[i] = Arg;
		}
	}
};

template<class Type, UINT size>
tstring __stdcall ToString( MultiParam<Type, size> dp )
{
	return ToString( (Type)dp );
}
template<class Type, UINT size>
bool __stdcall FromString( MultiParam<Type, size> *pDp, const tstring &str )
{
	Type Val;
	bool bRes = FromString( &Val, str );

	if ( bRes )
		*pDp = Val;

	return bRes;
}

typedef MultiParam<UINT, 3> TriUINT;
	
class CMultisampleTest : public CD3D11Test
{
public:
	virtual TEST_RESULT PreSetupTestCase() { return RESULT_PASS; }

protected:
	CMultisampleTest();

	void DecodeTestPath();
	void InitMultisampleParameters();
	virtual TEST_RESULT Setup();
	virtual void Cleanup();
	virtual void CleanupTestCase() = 0;
	int GetNumRenderWindows() const { return GetShell()->IsConsole() ? 0 : 1; }
	bool CreateRTFromSwapChainProps() const { return true; }

	virtual TEST_RESULT SetupD3D();
	virtual TEST_RESULT SetupRenderTarget();
	void ResetStates();
	void CleanupD3D();
	void Present();
	TEST_RESULT SetFormatAndQualityLevel( bool isDSFormat = false );

	// adds geometry to the vertex buffer
	void CoverSample( UINT px, UINT py, UINT uSampleIndex );
	void CoverSample( UINT uSampleIndex );
	void CoverPixel( UINT px, UINT py );
	void CoverPixel( );
	void CoverBuffer( );
	void CoverHalfPixel( UINT px, UINT py );

	void ResolvePixPos( );

	// determine if the related pipeline status need to be updated due to the change of format or SampleDesc
	bool CMultisampleTest::UpdateFormatOrSampleDesc();
	
	SampleFinder		m_Finder;
	DXGI_FORMAT			m_RTFormat;
	DXGI_FORMAT			m_DSFormat;
	DXGI_FORMAT			m_RTVFormat;
	TriUINT				m_RTHeight;
	TriUINT				m_RTWidth;

	ID3D11Device1			*m_pDevice;
	ID3D11DeviceContext1	*m_pDeviceContext;
	ID3D11Texture2D			*m_pRTTexture2D;
	ID3D11RenderTargetView	*m_pRTView;
	ID3D11Buffer			*m_pConstBuffer;
	ID3D11Buffer			*m_pVertexBuffer;
	ID3D11InputLayout		*m_pVertexLayout;
	
	ID3D11DepthStencilState *m_pDepthStateDefault;
	ID3D11DepthStencilState *m_pDepthStateDisabled;
	ID3D11RasterizerState1	*m_pRSEnabled;
	ID3D11BlendState		*m_pBS;

	ID3D11VertexShader		*m_pVS;
	ID3D11PixelShader		*m_pPS;

	UINT m_uEffectiveSampleCount;
	UINT m_uForcedSampleCount;
	UINT m_uSampleCount;
	UINT m_uSampleQuality;
	QUALITY_LEVEL_SCENARIO m_QualityLevelScenario;

	bool m_bMSEnable;
	MS_READBACK m_ReadBack;
	bool m_bA2C;
	bool m_bDepthTest;
	UINT m_uSampleMask;
	UINT m_uVertexCount;

	// test case pixel
	PIX_POS m_PixPosID;
	POINT m_vPixPos;
	D3D11_BOX m_PixBox;

	// previous format and sampleDesc
	DXGI_FORMAT			m_PrevDSFormat;
	DXGI_FORMAT			m_PrevRTVFormat;
	UINT m_uPrevSampleCount;
	UINT m_uPrevForcedSampleCount;
	QUALITY_LEVEL_SCENARIO m_PrevQualityLevelScenario;
	bool m_bUpdateFormatOrSampleDesc;

	// common factors across multisample testsa
	testfactor::RFactor m_rfPixPos;
	testfactor::RFactor m_rfReadBack;
	testfactor::RFactor m_rfMSEnable;
	testfactor::RFactor m_rfRTNonPow2Width, m_rfRTNonPow2Height;
	testfactor::RFactor m_rfSampleDesc;
	testfactor::RFactor m_rfSampleDescStandard;
	testfactor::RFactor m_rfRTFormatTypeless;
	testfactor::RFactor m_rfRTFormatTypedAll;
	testfactor::RFactor m_rfRTFormatTypedUINT;
	testfactor::RFactor m_rfRTFormatTypedSubset;
	CTestCaseParameter< DXGI_FORMAT > *m_pRTFormatParam;
	CTestCaseParameter< DXGI_FORMAT > *m_pRTVFormatParam;
	CTestCaseParameter< UINT > *m_pSampleCountParam;
	CTestCaseParameter< UINT > *m_pForcedSampleCountParam;
	CTestCaseParameter< QUALITY_LEVEL_SCENARIO > *m_pQualityLevelParam;
};

class CDummyTest : public CD3D11Test
{
private:
	void InitTestParameters() {};
};

class CMultisampleTestApp : public CD3D11TestFramework
{
private:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }

	void InitOptionVariables();
	bool InitTestGroups();

protected:
	bool m_bLogShaders;

public:
	bool m_bPresent;

	virtual TEST_RESULT PreSetupTestCase();
	bool InCompactList(DXGI_FORMAT format);
	bool IsDebugRuntime() const { return m_D3DOptions.Debug; }
	bool IsLoggingShaders() const { return m_bLogShaders; }
	D3D_DRIVER_TYPE GetSrcDriverType() const { return m_D3DOptions.DriverType; }
	UINT GetDefaultQueryTimeout() { return 10000; }

};
extern CMultisampleTestApp g_TestApp;

const vector<DXGI_FORMAT>& GetFamily( const DXGI_FORMAT &Fmt );
const DXGI_FORMAT GetTypeless( const DXGI_FORMAT fmt );
