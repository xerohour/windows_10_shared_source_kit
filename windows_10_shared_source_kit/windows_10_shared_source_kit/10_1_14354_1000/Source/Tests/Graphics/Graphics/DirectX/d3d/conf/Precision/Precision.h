#pragma once

#include "D3D11Test.h"
#include <WGFTestCore.h>

#include "FormatHelper.h"
#include "Direct3DFailureCategoryStrings.h"

using namespace std;
using namespace WindowsTest;
using namespace WindowsTest::Graphics::Direct3D;

#include <map>
#include <vector>
using namespace std;

#include <atlbase.h>
#include <atlwin.h>


#if !defined(PASTER2)
#define PASTER2(x,y) x ## y
#endif

#if !defined(PASTER)
#define PASTER(x,y) PASTER2(x,y)
#endif

#if !defined(UNICODIFY)
#define UNICODIFY( x ) PASTER(L,x)
#endif

#if !defined(__FILEW__)
#define __FILEW__ UNICODIFY(__FILE__)
#endif


//---------------------------------------------------------------
// Special values
extern const float g_fMaxFloat16;
extern const float g_fMinFloat16;
extern const float g_fSmallFloat16;
extern const float g_fDenormFloat16;
extern const float g_fINFp32;
extern const float g_fINFn32;
extern const float g_fQNAN;
extern const float g_fMinFloat10;
extern const float g_fMaxFloat10_1;
extern const float g_fMaxFloat10_2;
extern const float g_fMinFloat8;
extern const float g_fMaxFloat8;

//---------------------------------------------------------------
// This class wrapps a Map call and makes sure that Unmap is called when the object is destroyed
// This is similar to how CComPtr wraps AddRef/Release

class CMapHelper
{
public:
	CMapHelper( ID3D11DeviceContext* pContext );
	~CMapHelper();
	HRESULT Map( ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE *pMappedResource );
	void Unmap();
protected:
	ID3D11DeviceContext* m_pContext;
	ID3D11Resource* m_pResource;
	UINT m_Subresource;
};


//---------------------------------------------------------------
// This class represents a min-precision value with associated operations

enum EPrecisionLevel
{
	EPL_8  = 0x1,
	EPL_10 = 0x2,
	EPL_16 = 0x4,
	EPL_32 = 0x8,
};

template< class T >
class CMinPrecValue
{
public:
	typedef T (*PTOP3)(T,T,T);
	typedef T (*PTOP2)(T,T);
	typedef T (*PTOP1)(T);

	CMinPrecValue() : m_Levels(0) {}
	CMinPrecValue( T a ) 
	{ 
		m_Values.push_back( a );
		m_Levels = EPL_32;
	}
	// Move (not copy) constructor
	CMinPrecValue( CMinPrecValue< T >& A )
	{
		m_Values.swap( A.m_Values );
		m_Levels = A.m_Levels;
		A.m_Levels = 0;
	}

	CMinPrecValue< T >& operator=( const CMinPrecValue< T >& A )
	{
		if( this != &A )
		{
			m_Values = A.m_Values;
			m_Levels = A.m_Levels;
		}
		return *this;
	}

	void EnsureMinPrecisionLevel( EPrecisionLevel level, D3D_FEATURE_LEVEL fl );
	void Uniquify();
	bool Matches( T value, EPrecisionLevel level, D3D_FEATURE_LEVEL fl, bool bForceExact, float ULPMultiplier = 1.0f );
	void ApplyRangeClamp( D3D_FEATURE_LEVEL fl );
	tstring PrintValues() const;

	void _neg();
	void _abs();
	void _absneg();
	void Saturate();

	static EPrecisionLevel GetMinPrecisionLevel();
	static EPrecisionLevel ConvertToPrecisionLevel( D3D_MIN_PRECISION level );
	static D3D_MIN_PRECISION ConvertToPrecisionLevel( EPrecisionLevel level );

	static CMinPrecValue< T > op( const CMinPrecValue< T >& A, const CMinPrecValue< T >& B, const CMinPrecValue< T >& C, PTOP3 f );
	static CMinPrecValue< T > op( const CMinPrecValue< T >& A, const CMinPrecValue< T >& B, PTOP2 f );
	static CMinPrecValue< T > op( const CMinPrecValue< T >& A, PTOP1 f );

	template< class TDest >
	static CMinPrecValue< TDest > Convert( const CMinPrecValue< T > & A );

	static T _add( T a, T b ) { return a + b; }
	static T _sub( T a, T b ) { return a - b; }
	static T _mul( T a, T b ) { return a * b; }
	static T _max( T a, T b ) { return a > b ? a : b; }
	static T _min( T a, T b ) { return a > b ? b : a; }
	static float _exp( float a ) { return exp(a); }
	static float _rcp( float a ) { return 1/a; }
	static float _rsq( float a ) { return 1/sqrt(a); }
	static float _sqrt( float a ) { return sqrt(a); }
	static T _mad( T a, T b, T c ) { return a * b + c; }
	static T _movc( T a, T b, T c ) { return a ? b : c; }

	UINT m_Levels;
	vector<T> m_Values;
};

template< class T >
CMinPrecValue<T> _add( const CMinPrecValue<T>& A, const CMinPrecValue<T>& B )
{
	return CMinPrecValue<T>::op( A, B, &CMinPrecValue< T >::_add );
}
template< class T >
CMinPrecValue<T> _sub( const CMinPrecValue<T>& A, const CMinPrecValue<T>& B )
{
	// In bytecode, SUB is performed via add(a,-b)
	CMinPrecValue<T> nB;
	nB = B;
	nB._neg();
	return CMinPrecValue<T>::op( A, nB, &CMinPrecValue<T>::_add );
}
template< class T >
CMinPrecValue<T> _max( const CMinPrecValue<T>& A, const CMinPrecValue<T>& B )
{
	return CMinPrecValue<T>::op( A, B, &CMinPrecValue< T >::_max );
}
template< class T >
CMinPrecValue<T> _min( const CMinPrecValue<T>& A, const CMinPrecValue<T>& B )
{
	return CMinPrecValue<T>::op( A, B, &CMinPrecValue<T>::_min );
}
template< class T >
CMinPrecValue<float> _exp( const CMinPrecValue<T>& A )
{
	return CMinPrecValue<float>::op( A, &CMinPrecValue<float>::_exp );
}
template< class T >
CMinPrecValue<float> _rcp( const CMinPrecValue<T>& A )
{
	return CMinPrecValue<float>::op( A, &CMinPrecValue<float>::_rcp );
}
template< class T >
CMinPrecValue<float> _rsq( const CMinPrecValue<T>& A )
{
	return CMinPrecValue<float>::op( A, &CMinPrecValue<float>::_rsq );
}
template< class T >
CMinPrecValue<float> _sqrt( const CMinPrecValue<T>& A )
{
	return CMinPrecValue<float>::op( A, &CMinPrecValue<float>::_sqrt );
}
template< class T >
CMinPrecValue<T> _mad( const CMinPrecValue<T>& A, const CMinPrecValue<T>& B, const CMinPrecValue<T>& C )
{
	// The precision of mad is fairly low.  It can be as bad as add(mul(a,b),c)
	return CMinPrecValue<T>::op( CMinPrecValue<T>::op( A, B, &CMinPrecValue<T>::_mul ), C, &CMinPrecValue<T>::_add );
}
template< class T >
CMinPrecValue<T> _movc( const CMinPrecValue<T>& A, const CMinPrecValue<T>& B, const CMinPrecValue<T>& C )
{
	return CMinPrecValue<T>::op( A, B, C, &CMinPrecValue<T>::_movc );
}
// No need for a _swapc method because SWAPC is implemented as two calls to _movc


//---------------------------------------------------------------

enum SRC_MOD_TYPE
{
	SMOD_NONE,
	SMOD_NEG,
	SMOD_ABS,
	SMOD_ABSNEG,
};

enum INST_MOD_TYPE
{
	IMOD_NONE,
	IMOD_SAT,
};

enum OPERAND_TYPE
{
	OT_TEMP,
	OT_CONSTANT_BUFFER,
	OT_TEMP_AND_CONSTANT_BUFFER,
	OT_CONSTANT_BUFFER_AND_IMMEDIATE,
	OT_TEMP_AND_IMMEDIATE,
};

enum MIN_PRECISION_LOCATION
{
	MPL_SRC,
	MPL_DEST,
	MPL_BOTH,
};

DECLARE_NAMED_VALUES( SRC_MOD_TYPE );
DECLARE_NAMED_VALUES( INST_MOD_TYPE );
DECLARE_NAMED_VALUES( OPERAND_TYPE );
DECLARE_NAMED_VALUES( MIN_PRECISION_LOCATION );


class CPrecisionTest : public CD3D11Test
{
public:
	CPrecisionTest() :	m_MinPrecision(D3D_MIN_PRECISION_DEFAULT), 
						m_SrcModifier(SMOD_NONE), 
						m_InstructionModifier(IMOD_NONE), 
						m_ShaderStage(D3D_SVT_PIXELSHADER), 
						m_OperandType(OT_TEMP),
						m_ModifierLocation(MPL_SRC) 
	{}

	struct TilingVertex
	{
		float pos[2];
		float tex[2];
	};

	virtual TEST_RESULT		Setup( );
	virtual void			Cleanup( );

	static char* MinPrecisionPrefix( D3D_MIN_PRECISION level );
	
	testfactor::RFactor InitializeMinPrecisionFactor( testfactor::RFactor* pIntFactor = NULL );
	testfactor::RFactor InitializeSrcModifierFactor();
	testfactor::RFactor InitializeInstructionModifierFactor();
	virtual testfactor::RFactor InitializeShaderStageModifierFactor();
	testfactor::RFactor InitializeOperandTypeFactor();
	testfactor::RFactor InitializeLocationFactor( testfactor::RFactor* pDefaultFactor = NULL );

protected:
	template< class T >
	void WriteActualResultAndExpectedResultsToTheLog( const UINT &resultRegister, const UINT &component, const T &actualResult, const CMinPrecValue<T> &expectedResults ) const;
	bool WriteDeviceMinPrecisionSupportToTheLog() const;

protected:
	ATL::CComPtr<ID3D11Texture2D>			m_pRT2D;
	ATL::CComPtr<ID3D11Texture2D>			m_pRT2DStaging;
	ATL::CComPtr<ID3D11ShaderResourceView>	m_pSRV;
	ATL::CComPtr<ID3D11RenderTargetView>	m_pRTV;
	ATL::CComPtr<ID3D11RasterizerState>		m_pRS;
	ATL::CComPtr<ID3D11Buffer>				m_pVB;
	ATL::CComPtr<ID3D11SamplerState>		m_pSampler;
	ATL::CComPtr<ID3D11PixelShader>			m_pPS;
	ATL::CComPtr<ID3D11InputLayout>			m_pIL;
	ATL::CComPtr<ID3D11VertexShader>		m_pVS;

	D3D_MIN_PRECISION						m_MinPrecision;
	SRC_MOD_TYPE							m_SrcModifier;
	INST_MOD_TYPE							m_InstructionModifier;
	D3D_SHADER_VARIABLE_TYPE				m_ShaderStage;
	OPERAND_TYPE							m_OperandType;
	MIN_PRECISION_LOCATION					m_ModifierLocation;
};


//---------------------------------------------------------------

class CPrecisionTestApp : public CD3D11TestFramework
{
private:
	TESTTYPE	GetTestType( ) const { return TESTTYPE_CONF; }
	void		InitOptionVariables( );
	bool		InitTestGroups( );

protected:
	bool		m_bLogShaders;

public:
	bool				IsDebugRuntime( ) const { return m_D3DOptions.Debug; }
	bool				IsLoggingShaders( ) const { return m_bLogShaders; }
	D3D_DRIVER_TYPE	GetSrcDriverType( ) const { return m_D3DOptions.DriverType; }
};

extern CPrecisionTestApp g_TestApp;
