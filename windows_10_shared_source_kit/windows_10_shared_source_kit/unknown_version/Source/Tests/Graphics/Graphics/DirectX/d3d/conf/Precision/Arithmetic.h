#pragma once

#include "Precision.h"

enum ARITH_OP
{
	AO_ADD,
	AO_MAX,
	AO_MIN,
	AO_EXP,
	AO_RCP,
	AO_RSQ,
	AO_SQRT,
	AO_MAD,
	AO_IADD,
	AO_IMAX,
	AO_IMIN,
	AO_IMAD,
	AO_UADD,
	AO_UMAX,
	AO_UMIN,
	AO_UMAD,
	AO_MOV,
	AO_MOVC,
	AO_IMOV,
	AO_IMOVC,
	AO_UMOV,
	AO_UMOVC,
	AO_SWAPC,
	AO_ISWAPC,
	AO_USWAPC,
};

DECLARE_NAMED_VALUES( ARITH_OP );

template< class T >
tstring ToStringSafe( T x );

template < class T >
struct Vec4
{
    static const UINT Size = sizeof(float)*4/sizeof(T);
    T values[Size];

	Vec4()
	{
        for( UINT i=0; i<Size; i++ )
			values[i] = 0;
	}
	Vec4( T a, T b, T c, T d )
	{
		assert( Size == 4 );
		values[0] = a; values[1] = b; values[2] = c; values[3] = d;
	}
	Vec4( T a, T b )
	{
		assert( Size == 2 );
		values[0] = a; values[1] = b;
	}

    bool operator == ( const Vec4<T>& comp ) const 
    { 
        for( UINT i=0; i<Size; i++ )
		{
            if( comp[i] != values[i] )
                return false;
		}
        return true;
    }

    T& operator [] (UINT index) { return values[index]; } //non-const
    const T& operator [] (UINT index) const { return values[index]; } //const
};

template < class T >
extern tstring __stdcall ToString( const Vec4<T>& Vector );
template < class T >
extern tstring __stdcall ToString( const Vec4<T>& Vector );
template< class T >
extern bool __stdcall FromString( Vec4<T> *pVector, const tstring &str );


//---------------------------------------------------------------

class CArithmeticPrecisionTest : public CPrecisionTest
{
public:
	struct Vertex9
	{
		float pos[2];
	};
	enum EValueType
	{
		VT_Float = 0,
		VT_Uint = 1,
		VT_Int = 2,
		VT_Double = 3,
	};

	struct SPassThroughShaders
	{
		ATL::CComPtr<ID3D11VertexShader>	VS;
		ATL::CComPtr<ID3D11HullShader>		HS;
		ATL::CComPtr<ID3D11DomainShader>	DS;
		ATL::CComPtr<ID3D11GeometryShader>	GS;
		ATL::CComPtr<ID3D11PixelShader>		PS;
	};

	CArithmeticPrecisionTest() {}

	virtual void			InitTestParameters();
	virtual DXGI_FORMAT		GetRenderTargetFormat();
	virtual bool			CreateResources();
	virtual bool			CreateShaders();
	bool					CreateShaderAndSavePassThroughShader( char* szCurShader, UINT shaderSize, char* pEntrypoint, D3D_SHADER_STAGE Stage, D3D_FEATURE_LEVEL CompileFL, UINT Flags, SPassThroughShaders& PassThroughShaders, ID3D10Blob** ppOutputBlob );
	bool					CreateShadersFromString( char* szCurShader, UINT shaderSize, D3D_FEATURE_LEVEL MinFL, D3D_FEATURE_LEVEL MinFLPS, UINT Flags, SPassThroughShaders& PassThroughShaders );

	virtual TEST_RESULT		SetupTestCase();
	virtual TEST_RESULT		ExecuteTestCase();
	virtual void			CleanupTestCase();
	virtual TEST_RESULT		ValidateResult();

	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor() = 0;
	string OperationToString( string pSrc0, string pSrc1 = "", string pSrc2 = "" );
	virtual void* GetSrcData() ;
	void ModifyFactorsIfNecessary();

	template< class T, UINT NumOperands >
	static testfactor::RFactor InitializeValueFactor( CArithmeticPrecisionTest* pThis, CUserValueSet<T>* pValues, Vec4<T>* BackingStore, CUserValueSet<T>* pValues2Plus, UINT Components, char* pPostFix);
	static EValueType GetValueType( ARITH_OP op );
	static char* GetTypeName( EValueType type );
	template< class T >
	static CMinPrecValue<T> PerformOperation( ARITH_OP op, CMinPrecValue<T>& a, CMinPrecValue<T>& b, CMinPrecValue<T>& c, UINT component );
	template< class T>
	void ApplySourceModifier( SRC_MOD_TYPE ModType, CMinPrecValue<T>& a, CMinPrecValue<T>& b, CMinPrecValue<T>& c );

protected:
	ARITH_OP				m_Op;
	Vec4<float>				m_SrcFloat[3];
	Vec4<UINT>				m_SrcUint[3];
	Vec4<int>				m_SrcInt[3];

	SPassThroughShaders					m_PassThroughShaders[3];
	ATL::CComPtr<ID3D11GeometryShader>	m_pGS;
	ATL::CComPtr<ID3D11HullShader>		m_pHS;
	ATL::CComPtr<ID3D11DomainShader>	m_pDS;
	ATL::CComPtr<ID3D11ComputeShader>	m_pCS;
	ATL::CComPtr<ID3D11Buffer>			m_pCB;
	static const UINT					m_CBSize = sizeof(float)*12;
	ATL::CComPtr<ID3D11Buffer>			m_pStructuredBuffer;
	ATL::CComPtr<ID3D11Buffer>			m_pStructuredStaging;
	ATL::CComPtr<ID3D11UnorderedAccessView> m_pBufUAV;

	static const UINT		m_RTWidth = 1;
	static const UINT		m_RTHeight = 1;
};

DECLARE_NAMED_VALUES( CArithmeticPrecisionTest::EValueType );

class CArithmeticFloatTest : public CArithmeticPrecisionTest
{
public:
	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor();
};

class CArithmeticUintTest : public CArithmeticPrecisionTest
{
public:
	virtual TEST_RESULT Setup();
	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor();
};

class CArithmeticIntTest : public CArithmeticPrecisionTest
{
public:
	virtual TEST_RESULT Setup();
	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor();
};

