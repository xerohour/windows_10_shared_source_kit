//////////////////////////////////////////////////////////////////////////
//  DoubleArith.cpp
//  created:	2009/03/02
//
//  purpose: Test correctness of calculations performed at double precision. 
//
//  4/24/11:        D3D11.1 Shader Model Improvements
//  Calculations:   ddiv, drcp, dfma
//  Conversions:    dtoi, dtou, itod, utod
//////////////////////////////////////////////////////////////////////////
#include "DoublesArith.h"
#include "AsmHelper.h"

#define WGF11SHADER5X
#include "DoublesDDivData.h"
#include "DoublesDRCPData.h"
#include "DoublesDFMAData.h"

#define UnWritten 0xBAADF00Dl

CShader5xTest_DoubleArith::CShader5xTest_DoubleArith()
	:CShader5xTest(DXGI_FORMAT_R32G32B32A32_UINT)
{}

bool CShader5xTest_DoubleArith::Binary64::operator ==( Binary64 const& b ) const
{
	return (IsNaN(b) && IsNaN(*this)) || ( b.m_AsULongLong == m_AsULongLong );
}

bool CShader5xTest_DoubleArith::Binary64::operator !=( Binary64 const& b ) const
{
	return !(*this == b);
}

bool CShader5xTest_DoubleArith::Binary64Vec2::operator ==( Binary64Vec2 const& b ) const
{
	for( unsigned int i(0); i < 2; ++i )
	{
		if( (m_Vec[i] == b.m_Vec[i]) == false )
		{
			return false;
		}
	}
	return true;
}

bool CShader5xTest_DoubleArith::Binary32Vec2::operator ==( Binary32Vec2 const& b ) const
{
	for( unsigned int i(0); i < 2; ++i )
	{
		if( (m_Vec[i] == b.m_Vec[i]) == false )
		{
			return false;
		}
	}
	return true;
}

CShader5xTest_DoubleArith::Binary64& CShader5xTest_DoubleArith::Binary64Vec2::operator []( int i )
{
	return m_Vec[i];
}	

CShader5xTest_DoubleArith::Binary64 const& CShader5xTest_DoubleArith::Binary64Vec2::operator []( int i ) const
{
	return m_Vec[i];
}	

float& CShader5xTest_DoubleArith::Binary32Vec2::operator []( int i )
{
	return m_Vec[i];
}	

float const& CShader5xTest_DoubleArith::Binary32Vec2::operator []( int i ) const
{
	return m_Vec[i];
}	

//DOUBLE_CLASSIFICATION CShader5xTest_DoubleArith::Binary64::GetClassification() const
//{
//	if( IsNegINF(*this) )
//	{
//		return DC_INFn;
//	}
//	else if( IsPosINF(*this) )
//	{
//		return DC_INFp;
//	}
//	else if( IsNaN(*this) )
//	{
//		return DC_NAN;
//	}
//}

bool CShader5xTest_DoubleArith::IsINF( Binary64 const& d )
{
	return ((d.m_AsULongLong & s_Fraction64.m_AsULongLong) == 0ull) && 
		(((d.m_AsULongLong & s_Exponent64.m_AsULongLong)  ^ s_Exponent64.m_AsULongLong) == 0ull );
} 

bool CShader5xTest_DoubleArith::IsNegINF( Binary64 const& d )
{
	return IsINF(d) && (d.m_AsULongLong & s_Sign64.m_AsULongLong);
}

bool CShader5xTest_DoubleArith::IsPosINF( Binary64 const& d )
{
	return IsINF(d) && ((d.m_AsULongLong & s_Sign64.m_AsULongLong) == 0x0ull );
}

bool CShader5xTest_DoubleArith::IsNaN( Binary64 const& d )
{
	return ((d.m_AsULongLong & s_Fraction64.m_AsULongLong) != 0ull ) &&
		(((d.m_AsULongLong & s_Exponent64.m_AsULongLong) ^ s_Exponent64.m_AsULongLong) == 0ull );
}

bool CShader5xTest_DoubleArith::IsZero( Binary64 const& d )
{
	return (d.m_AsULongLong == s_Zero64.m_AsULongLong) || (d.m_AsULongLong == s_NegZero64.m_AsULongLong);
}

bool CShader5xTest_DoubleArith::IsNegZero( Binary64 const& d )
{
	return IsZero(d) && (d.m_AsULongLong & s_Sign64.m_AsULongLong);
}

bool CShader5xTest_DoubleArith::IsPosZero( Binary64 const& d )
{
	return d.m_AsULongLong == s_Zero64.m_AsULongLong;
}

bool CShader5xTest_DoubleArith::IsPosOrNegZero( Binary64 const& d )
{
	return IsPosZero(d) || IsNegZero(d);
}

bool CShader5xTest_DoubleArith::IsDNorm( Binary64 const& d )
{
	return ((d.m_AsULongLong & s_Exponent64.m_AsULongLong) == 0) && (d.m_AsULongLong & s_Fraction64.m_AsULongLong);
}

bool CShader5xTest_DoubleArith::IsPosDNorm( Binary64 const& d )
{
    return ((d.m_AsULongLong & s_Exponent64.m_AsULongLong) == 0) && (d.m_AsULongLong & s_Fraction64.m_AsULongLong)
		&& ((d.m_AsULongLong & s_Sign64.m_AsULongLong) == 0);
}

bool CShader5xTest_DoubleArith::IsNegDNorm( Binary64 const& d )
{
    return ((d.m_AsULongLong & s_Exponent64.m_AsULongLong) == 0) && (d.m_AsULongLong & s_Fraction64.m_AsULongLong)
		&& (d.m_AsULongLong & s_Sign64.m_AsULongLong);
}

bool CShader5xTest_DoubleArith::IsDNorm( float const& d )
{
	typedef union { float m_AsFloat; unsigned int m_AsULong; } Helper;
	Helper const v = { d };
	return ((v.m_AsULong & 0x7F800000u) == 0) && (v.m_AsULong & 0x007FFFFFu);
}

bool CShader5xTest_DoubleArith::IsNorm( Binary64 const& d )
{
	return (IsNaN(d) == false )  && (IsINF(d) == false ) && (IsDNorm(d) == false) && (IsZero(d) == false );
}

bool CShader5xTest_DoubleArith::IsReal( Binary64 const& d )
{
	return (IsNaN(d) == false ) && (IsINF(d) == false );
}

bool CShader5xTest_DoubleArith::IsNegReal( Binary64 const& d )
{
	return IsReal(d) && (d.m_AsULongLong & s_Sign64.m_AsULongLong);
}

bool CShader5xTest_DoubleArith::IsPosReal( Binary64 const& d )
{
	return IsReal(d) && ((d.m_AsULongLong & s_Sign64.m_AsULongLong) == 0 );
}

bool CShader5xTest_DoubleArith::IsPosOne( Binary64 const& d )
{
	return d.m_AsULongLong == s_One64.m_AsULongLong;
}

bool CShader5xTest_DoubleArith::IsNegOne( Binary64 const& d )
{
	return d.m_AsULongLong == s_NegOne64.m_AsULongLong;
}

bool CShader5xTest_DoubleArith::IsSaturatedReal( Binary64 const& d )
{
	return IsReal(d) && (d.m_AsDouble >= s_Zero64.m_AsDouble) && (d.m_AsDouble <= s_One64.m_AsDouble);
}

const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_Sign64		=		    { 0x8000000000000000ull }; // 1 bit (sign)
const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_Exponent64	=		    { 0x7FF0000000000000ull }; // 11 bits (exponent)
const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_Fraction64	=		    { 0x000FFFFFFFFFFFFFull }; // 52 bits (mantisa)
const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_OneULP64		=		    { 0x0000000000000001ull }; // This plus any sign and any exponent.
const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_NegINF64		=		    { 0xFFF0000000000000ull }; // value for negative infinity
const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_PosINF64		=		    { 0x7FF0000000000000ull }; // value for positive infinity
const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_One64		=		    { 0x3FF0000000000000ull }; // value for positive 1.0
const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_NegOne64		=		    { 0xBFF0000000000000ull }; // value for negative 1.0
const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_Zero64		=		    { 0x0000000000000000ull }; // value for zero 0.0
const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_NegZero64	=		    { 0x8000000000000000ull }; // value for negative zero -0.0
const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_BadFood64	=		    { 0xBAADF00DBAADF00Dull }; // value for badfood :)
const CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::s_NegNextToSmallestNorm =	{ 0x0010000000000001ull }; // value for negative next-to-smallest normal

//////////////////////////////////////////////////////////////////////////
// Functions for choosing which src value to use as a result.
CShader5xTest_DoubleArith::Binary64Vec2 ReturnSrc0( CShader5xTest_DoubleArith::Binary64Vec2 const& src0,
													CShader5xTest_DoubleArith::Binary64Vec2 const& )
{
	return src0;
}
CShader5xTest_DoubleArith::Binary64Vec2 ReturnSrc1( CShader5xTest_DoubleArith::Binary64Vec2 const&, 
													CShader5xTest_DoubleArith::Binary64Vec2 const& src1 )
{
	return src1;
}
typedef CShader5xTest_DoubleArith::Binary64Vec2 (*SrcGetFunc)( CShader5xTest_DoubleArith::Binary64Vec2 const&, CShader5xTest_DoubleArith::Binary64Vec2 const& );
template<SrcGetFunc pSGF> 
CShader5xTest_DoubleArith::Binary64Vec2 NegateResult(	CShader5xTest_DoubleArith::Binary64Vec2 const& src0,
														CShader5xTest_DoubleArith::Binary64Vec2 const& src1 )
{
	CShader5xTest_DoubleArith::Binary64Vec2 result = (*pSGF)( src0, src1 );
	result.Negate();
	return result;
}
template<SrcGetFunc pSGF> 
CShader5xTest_DoubleArith::Binary64Vec2 SaturateResult( CShader5xTest_DoubleArith::Binary64Vec2 const& src0,
														CShader5xTest_DoubleArith::Binary64Vec2 const& src1 )
{
	CShader5xTest_DoubleArith::Binary64Vec2 result = (*pSGF)( src0, src1 );
	result.Saturate();
	return result;
}
//////////////////////////////////////////////////////////////////////////
// Set the right src value
template<typename _Type>
_Type ReturnValue( _Type const& src )
{
	return src;
}
//typedef CShader5xTest_DoubleArith::Binary64Vec2 (*SrcModFunc)( CShader5xTest_DoubleArith::Binary64Vec2 const& );
template<typename _Type, _Type (*SrcModFunc)( _Type const&)>
_Type NegateValue( _Type const& src )
{
	_Type result = (*SrcModFunc)( src );
	result.Negate();
	return result;
}
template<typename _Type, _Type (*SrcModFunc)( _Type const&)>
_Type AbsValue( _Type const& src )
{
	_Type result = (*SrcModFunc)( src );
	result.Abs();
	return result;
}
//////////////////////////////////////////////////////////////////////////
// Group of information for a specific double classification
struct DoubleClassInfo
{
	// Name of the double classification
	tstring const m_Name;
	// Value of the double (if one exists)
	CShader5xTest_DoubleArith::Binary64 const*const m_pValue;
	// Function for checking if a double is this classification.
	CShader5xTest_DoubleArith::IsA const m_pCatFunc;
	// Function for retrieving the correct src input for a result.
	typedef CShader5xTest_DoubleArith::Binary64Vec2 (*SrcPicFunc)( CShader5xTest_DoubleArith::Binary64Vec2 const&, CShader5xTest_DoubleArith::Binary64Vec2 const& );
	SrcPicFunc const m_pSrcFunc;
};

// This must match enum DOUBLE_CLASSIFICATION in Shader5x.h
DoubleClassInfo const  DoubleClassInfoTable[] = 
{
	{ _T("-INF"),   &CShader5xTest_DoubleArith::s_NegINF64, &CShader5xTest_DoubleArith::IsNegINF, NULL },
	{ _T("-REAL"),  NULL, &CShader5xTest_DoubleArith::IsNegReal, NULL },
	{ _T("-ONE"),   &CShader5xTest_DoubleArith::s_NegOne64, &CShader5xTest_DoubleArith::IsNegOne, NULL },
	{ _T("-ZERO"),  &CShader5xTest_DoubleArith::s_NegZero64, &CShader5xTest_DoubleArith::IsNegZero, NULL },
	{ _T("+ZERO"),  &CShader5xTest_DoubleArith::s_Zero64, &CShader5xTest_DoubleArith::IsPosZero, NULL },
	{ _T("+ONE"),   &CShader5xTest_DoubleArith::s_One64, &CShader5xTest_DoubleArith::IsPosOne, NULL },
	{ _T("+REAL"),  NULL, &CShader5xTest_DoubleArith::IsPosReal, NULL },
	{ _T("+INF"),   &CShader5xTest_DoubleArith::s_PosINF64, &CShader5xTest_DoubleArith::IsPosINF, NULL },
	{ _T("NAN"),    NULL, &CShader5xTest_DoubleArith::IsNaN, NULL },
	{ _T("-SRC0"),  NULL, NULL, &NegateResult<&ReturnSrc0> },
	{ _T("SRC0"),   NULL, NULL, &ReturnSrc0 },
	{ _T("-SRC1"),  NULL, NULL, &NegateResult<&ReturnSrc1> },
	{ _T("SRC1"),   NULL, NULL, &ReturnSrc1 },
	{ _T("REAL"),   NULL, &CShader5xTest_DoubleArith::IsReal, NULL },
	{ _T("NORM"),   NULL, &CShader5xTest_DoubleArith::IsNorm, NULL },
	{ _T("-NORM"),  NULL, NULL, NULL },
	{ _T("+NORM"),  NULL, NULL, NULL },
	{ _T("DENORM"), NULL, &CShader5xTest_DoubleArith::IsDNorm, NULL },
	{ _T("-DENORM"), NULL, &CShader5xTest_DoubleArith::IsNegDNorm, NULL },
	{ _T("+DENORM"), NULL, &CShader5xTest_DoubleArith::IsPosDNorm, NULL },
	{ _T("ResultTypeCount"), NULL, NULL, NULL },
	{ _T("REAL_SAT"), NULL, &CShader5xTest_DoubleArith::IsSaturatedReal, NULL },
	{ _T("SRC0_SAT"), NULL, NULL, &SaturateResult<&ReturnSrc0> },
	{ _T("SRC0n_SAT"), NULL, NULL, &SaturateResult<&NegateResult<&ReturnSrc0> > },
	{ _T("SRC1_SAT"), NULL, NULL, &SaturateResult<&ReturnSrc1> },
	{ _T("SRC1n_SAT"), NULL, NULL, &SaturateResult<&NegateResult<&ReturnSrc1> > },
	{ _T("DC_ZEROpn"), NULL, &CShader5xTest_DoubleArith::IsPosOrNegZero, NULL },
};

//////////////////////////////////////////////////////////////////////////
tstring ToString( CShader5xTest_DoubleArith::Binary64 const& d )
{ 
	//TCHAR buf[32] = _T("\0");
	//sprintf_s( buf, 32, "%+.17f", d.m_AsDouble );
	//return tstring( buf );
	return FormatString( "%+Lg (0x%016llX)", d.m_AsDouble, d.m_AsULongLong );
}

tstring ToString( CShader5xTest_DoubleArith::Binary64Vec2 const& v )
{
	return ToString( v[0] ) + ' ' + ToString( v[1] );
}

tstring ToString( DOUBLE_CLASSIFICATION const& dc )
{
	return DoubleClassInfoTable[dc].m_Name;
}

tstring ToString( CShader5xTest_DoubleArith::Binary32Vec2 const& v )
{
	return ToString( v[0] ) + ' ' + ToString( v[1] );
}

bool FromString( CShader5xTest_DoubleArith::Binary64* pVal, tstring const& str )
{
	return sscanf_s( str.c_str(), "%lg (0x%*llX)", &(pVal->m_AsDouble) ) == 1;
}

bool FromString( CShader5xTest_DoubleArith::Binary64Vec2* pVal, tstring const& str )
{
	return sscanf_s( str.c_str(), "%lg (0x%*llX) %lg", (double *) &(pVal->m_X), (double *) &(pVal->m_Y) ) == 2;
}

bool FromString( CShader5xTest_DoubleArith::Binary32Vec2* pVal, tstring const& str )
{
	return sscanf_s( str.c_str(), "%g %g", &(pVal->m_X), &(pVal->m_Y) ) == 2;
}

bool FromString( DOUBLE_CLASSIFICATION* pVal, tstring const& str )
{
	for( unsigned int i(0); i < sizeof(DoubleClassInfoTable) / sizeof(DoubleClassInfo); ++i )
	{	
		if( str == DoubleClassInfoTable[i].m_Name )
		{	
			*pVal = static_cast<DOUBLE_CLASSIFICATION>(i);
			return true;
		}
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////
DOUBLE_CLASSIFICATION const DoubleSrcMulTable[9] = 
{ 
	DC_INFn,
	DC_REALn,
	DC_ONEn,
	DC_ZEROn,
	DC_ZEROp,
	DC_ONEp,
	DC_REALp,
	DC_INFp,
	DC_NAN 
};
//////////////////////////////////////////////////////////////////////////
DOUBLE_CLASSIFICATION const DoubleMulTable[9][9] = 
{
	/* SRC0	SRC1->	-INF		-Real		-1.0		-0.0		+0.0		+1.0		+Real		+INF		NaN */
	/* -INF  */	{	DC_INFp,	DC_INFp,	DC_INFp,	DC_NAN,		DC_NAN,		DC_INFn,	DC_INFn,	DC_INFn,	DC_NAN },
	/* -Real */ {	DC_INFp,	DC_REALp,	DC_SRC0n,	DC_ZEROp,	DC_ZEROn,	DC_SRC0,	DC_REALn,	DC_INFn,	DC_NAN },
	/* -One  */ {	DC_INFp,	DC_SRC1n,	DC_ONEp,	DC_ZEROp,	DC_ZEROn,	DC_ONEn,	DC_SRC1n,	DC_INFn,	DC_NAN },
	/* -Zero */ {	DC_NAN,		DC_ZEROp,	DC_ZEROp,	DC_ZEROp,	DC_ZEROn,	DC_ZEROn,	DC_ZEROn,	DC_NAN,		DC_NAN },
	/* +Zero */ {	DC_NAN,		DC_ZEROn,	DC_ZEROn,	DC_ZEROn,	DC_ZEROp,	DC_ZEROp,	DC_ZEROp,	DC_NAN,		DC_NAN },
	/* +One  */ {	DC_INFn,	DC_SRC1,	DC_ONEn,	DC_ZEROn,	DC_ZEROp,	DC_ONEp,	DC_SRC1,	DC_INFp,	DC_NAN },
	/* +Real */ {	DC_INFn,	DC_REALn,	DC_SRC0n,	DC_ZEROn,	DC_ZEROp,	DC_SRC0,	DC_REALp,	DC_INFp,	DC_NAN },
	/* +INF  */ {	DC_INFn,	DC_INFn,	DC_INFn,	DC_NAN,		DC_NAN,		DC_INFp,	DC_INFp,	DC_INFp,	DC_NAN },
	/* NaN	 */ {	DC_NAN,		DC_NAN,		DC_NAN,		DC_NAN,		DC_NAN,		DC_NAN,		DC_NAN,		DC_NAN,		DC_NAN }
};

//////////////////////////////////////////////////////////////////////////
// Classifications for Values_ddiv:
DOUBLE_CLASSIFICATION const DoubleDivSrcTable[12] = 
{ 
	DC_INFn,
	DC_REALn,
	DC_REALn,
	DC_ONEn,
	DC_ZEROn,
	DC_ZEROp,
	DC_ONEp,
	DC_REALp,
	DC_REALp,
	DC_INFp,
	DC_NAN,
	DC_NAN 
};

//////////////////////////////////////////////////////////////////////////
DOUBLE_CLASSIFICATION const DoubleDivResultsTable[9][9][3] = 
{
// In some division cases, i.e. REAL / REAL, there is more than one possible result: REAL, INF.  The extra dimension is provided for this. 
/* SRC0	SRC1-> -INF                            -Real                           -1.0                           -0.0                             +0.0  */
/* -INF  */	{ {DC_NAN, DC_NAN, DC_NAN},       {DC_INFp, DC_INFp, DC_INFp},    {DC_INFp, DC_INFp, DC_INFp},    {DC_INFp, DC_INFp, DC_INFp},    {DC_INFn, DC_INFn, DC_INFn},
/* SRC0	SRC1-> +1.0                            +Real                           +INF                            NaN  */
/* -INF  */	  {DC_INFn, DC_INFn, DC_INFn},    {DC_INFn, DC_INFn, DC_INFn},    {DC_NAN, DC_NAN, DC_NAN},       {DC_NAN, DC_NAN, DC_NAN} },

/* SRC0	SRC1-> -INF                            -Real                           -1.0                            -0.0                            +0.0  */
/* -Real */ { {DC_ZEROp, DC_ZEROp, DC_ZEROp}, {DC_REALp, DC_INFp, DC_INFp},   {DC_SRC0n, DC_SRC0n, DC_SRC0n}, {DC_INFp, DC_INFp, DC_INFp},    {DC_INFn, DC_INFn, DC_INFn},
/* SRC0	SRC1-> +1.0                            +Real                           +INF                            NaN  */
/* -Real */   {DC_SRC0, DC_SRC0, DC_SRC0},    {DC_INFn, DC_ONEn, DC_ZEROn},   {DC_INFn, DC_ZEROn, DC_ZEROn}, {DC_NAN, DC_NAN, DC_NAN} },

/* SRC0	SRC1-> -INF                            -Real                           -1.0                            -0.0                            +0.0  */
/* -One  */ { {DC_ZEROp, DC_ZEROp, DC_ZEROp}, {DC_REALp, DC_REALp, DC_REALp}, {DC_ONEp, DC_ONEp, DC_ONEp},    {DC_INFp, DC_INFp, DC_INFp},   {DC_INFn, DC_INFn, DC_INFn},
/* SRC0	SRC1-> +1.0                            +Real                           +INF                            NaN  */
/* -One  */   {DC_ONEn, DC_ONEn, DC_ONEn},    {DC_REALn, DC_REALn, DC_REALn}, {DC_ZEROn, DC_ZEROn, DC_ZEROn}, {DC_NAN, DC_NAN, DC_NAN} },

/* SRC0	SRC1-> -INF                            -Real                           -1.0                            -0.0                            +0.0  */
/* -Zero */ { {DC_ZEROp, DC_ZEROp, DC_ZEROp}, {DC_ZEROp, DC_ZEROp, DC_ZEROp}, {DC_ZEROp, DC_ZEROp, DC_ZEROp}, {DC_NAN, DC_NAN, DC_NAN},      {DC_NAN, DC_NAN, DC_NAN},
/* SRC0	SRC1-> +1.0                            +Real                           +INF                            NaN  */
/* -Zero */   {DC_ZEROn, DC_ZEROn, DC_ZEROn}, {DC_ZEROn, DC_ZEROn, DC_ZEROn}, {DC_ZEROn, DC_ZEROn, DC_ZEROn}, {DC_NAN, DC_NAN, DC_NAN} },

/* SRC0	SRC1-> -INF                            -Real                           -1.0                            -0.0                            +0.0  */
/* +Zero */ { {DC_ZEROn, DC_ZEROn, DC_ZEROn}, {DC_ZEROn, DC_ZEROn, DC_ZEROn}, {DC_ZEROn, DC_ZEROn, DC_ZEROn}, {DC_NAN, DC_NAN, DC_NAN},       {DC_NAN, DC_NAN, DC_NAN},
/* SRC0	SRC1-> +1.0                            +Real                           +INF                            NaN  */
/* +Zero */   {DC_ZEROp, DC_ZEROp, DC_ZEROp}, {DC_ZEROp, DC_ZEROp, DC_ZEROp}, {DC_ZEROp, DC_ZEROp, DC_ZEROp}, {DC_NAN, DC_NAN, DC_NAN} },

/* SRC0	SRC1-> -INF                            -Real                           -1.0                            -0.0                            +0.0  */
/* +One  */ { {DC_ZEROn, DC_ZEROn, DC_ZEROn}, {DC_REALn, DC_REALn, DC_REALn}, {DC_ONEn, DC_ONEn, DC_ONEn},    {DC_INFn, DC_INFn, DC_INFn},    {DC_INFp, DC_INFp, DC_INFp},
/* SRC0	SRC1-> +1.0                            +Real                           +INF                            NaN  */
/* +One  */   {DC_ONEp, DC_ONEp, DC_ONEp},    {DC_REALp, DC_REALp, DC_REALp}, {DC_ZEROp, DC_ZEROp, DC_ZEROp}, {DC_NAN, DC_NAN, DC_NAN} },

/* SRC0	SRC1-> -INF                            -Real                           -1.0                            -0.0                            +0.0  */
/* +Real */ { {DC_ZEROn, DC_ZEROn, DC_ZEROn}, {DC_INFn, DC_ONEn, DC_ZEROn},  {DC_REALn, DC_REALn, DC_REALn}, {DC_INFn, DC_INFn, DC_INFn},   {DC_INFp, DC_INFp, DC_INFp},
/* SRC0	SRC1-> +1.0                            +Real                           +INF                            NaN  */
/* +Real */   {DC_REALp, DC_REALp, DC_REALp}, {DC_REALp, DC_INFp, DC_INFp},  {DC_ZEROp, DC_ZEROp, DC_ZEROp}, {DC_NAN, DC_NAN, DC_NAN} },

/* SRC0	SRC1-> -INF                            -Real                           -1.0                            -0.0                            +0.0  */
/* +INF  */ { {DC_NAN, DC_NAN, DC_NAN},       {DC_INFn, DC_ZEROn, DC_ZEROn},  {DC_INFn, DC_INFn, DC_INFn},    {DC_INFn, DC_INFn, DC_INFn},     {DC_INFp, DC_INFp, DC_INFp},
/* SRC0	SRC1-> +1.0                            +Real                           +INF                            NaN  */
/* +INF  */   {DC_INFp, DC_INFp, DC_INFp},    {DC_INFp, DC_INFp, DC_INFp},    {DC_NAN, DC_NAN, DC_NAN},       {DC_NAN, DC_NAN, DC_NAN} },

/* SRC0	SRC1-> -INF                            -Real                           -1.0                            -0.0                            +0.0  */
/* NaN	 */ { {DC_NAN, DC_NAN, DC_NAN},       {DC_NAN, DC_NAN, DC_NAN},       {DC_NAN, DC_NAN, DC_NAN},       {DC_NAN, DC_NAN, DC_NAN},       {DC_NAN, DC_NAN, DC_NAN},
/* SRC0	SRC1-> +1.0                            +Real                           +INF                            NaN  */
/* NaN	 */   {DC_NAN, DC_NAN, DC_NAN},       {DC_NAN, DC_NAN, DC_NAN},       {DC_NAN, DC_NAN, DC_NAN},       {DC_NAN, DC_NAN, DC_NAN} }
};

//////////////////////////////////////////////////////////////////////////
// Classifications for Values_drcp:
DOUBLE_CLASSIFICATION const DoubleSrcRCPTable[12] = 
{ 
	DC_INFn,
	DC_REALn,
	DC_REALn,
	DC_ONEn,
	DC_ZEROn,
	DC_ZEROp,
	DC_ONEp,
	DC_REALp,
	DC_REALp,
	DC_INFp,
	DC_NAN,
	DC_NAN 
};

//////////////////////////////////////////////////////////////////////////
DOUBLE_CLASSIFICATION const DoubleRCPTable[9] = 
{
	/* SRC0	 */
	/* -INF  */	DC_ZEROn,
	/* -Real */ DC_REALn,
	/* -One  */ DC_ONEn,
	/* -Zero */ DC_INFn,
	/* +Zero */ DC_INFp,
	/* +One  */ DC_ONEp,
	/* +Real */ DC_REALp,
	/* +INF  */ DC_ZEROp,
	/* NaN	 */ DC_NAN	
};

//////////////////////////////////////////////////////////////////////////
// Classifications for Values_dfma:
DOUBLE_CLASSIFICATION const DoubleSrcFMATable[11] = 
{ 
	DC_INFn,    // 0
	DC_DENORMn, // 1
	DC_REALn,   // 2
	DC_ONEn,    // 3
	DC_ZEROn,   // 4
	DC_ZEROp,   // 5 
	DC_ONEp,    // 6
	DC_REALp,   // 7
	DC_DENORMp, // 8
	DC_INFp,    // 9
	DC_NAN,     // 10
};

// The enum value from DoubleSrcFMATable is used to select a slot in DoubleFMACrossIndex.
// That slot contains the index to be used on DoubleFMATable. 
static unsigned int const DoubleFMACrossIndex[] = 
{
	0,  //DC_INFn,
	2,  //DC_REALn,
	3,  //DC_ONEn,
	4,  //DC_ZEROn,
	5,  //DC_ZEROp,
	6,  //DC_ONEp,
	7,  //DC_REALp,
	9,  //DC_INFp,
	10,  //DC_NAN,
	-1, //DC_SRC0n,
	-1, //DC_SRC0,
	-1, //DC_SRC1n,
	-1, //DC_SRC1,
	-1, //DC_REAL,
	-1, //DC_NORM,
	-1, //DC_NORMn,
	-1, //DC_NORMp,
	-1, //DC_DENORM,
	1,  //DC_DENORMn,
	8,  //DC_DENORMp
    -1, //NUM_DOUBLE_CLASSIFICATION_RESULT_TYPES
};

//////////////////////////////////////////////////////////////////////////
static unsigned int const DFMAIndexValues[] =
{ 
    DC_INFn,
    DC_DENORMn,
    DC_REALn,
    DC_ONEn,
    DC_ZEROn,
    DC_ZEROp,
    DC_ONEp,
    DC_REALp,
    DC_DENORMp,
    DC_INFp,
    DC_NAN,
};
static unsigned int const DFMA_numVals = sizeof(DFMAIndexValues)/sizeof(DFMAIndexValues[0]);

DOUBLE_CLASSIFICATION const DoubleFMATable[DFMA_numVals][DFMA_numVals][DFMA_numVals] =  // SRC1 * SRC2 + SRC3 = DC_REALp, etc.
{                   
#include "DoubleFMATable.h"       // Generated by \DirectX\Tools\CalcDoubles
};

//////////////////////////////////////////////////////////////////////////
DOUBLE_CLASSIFICATION const DoubleSrcAddTable[7] = 
{ 
	DC_INFn,
	DC_REALn,
	DC_ZEROn,
	DC_ZEROp,
	DC_REALp,
	DC_INFp,
	DC_NAN 
};
//////////////////////////////////////////////////////////////////////////
DOUBLE_CLASSIFICATION const DoubleAddTable[7][7] = 
{
	/* SRC0	SRC1->	-INF		-Real		-0.0		+0.0		+Real		+INF		NaN */
	/* -INF  */	{	DC_INFn,	DC_INFn,	DC_INFn,	DC_INFn,	DC_INFn,	DC_NAN, 	DC_NAN },
	/* -Real */ {	DC_INFn,	DC_REALn,	DC_SRC0,	DC_SRC0,	DC_REAL,	DC_INFp,	DC_NAN },
	/* -Zero */ {	DC_INFn,	DC_SRC1,	DC_ZEROn,	DC_ZEROp,	DC_SRC1,	DC_INFp,	DC_NAN },
	/* +Zero */ {	DC_INFn,	DC_SRC1,	DC_ZEROp,	DC_ZEROp,	DC_SRC1,	DC_INFp,	DC_NAN },
	/* +Real */ {	DC_INFn,	DC_REAL,	DC_SRC0,	DC_SRC0,	DC_REALp,	DC_INFp,	DC_NAN },
	/* +INF  */ {	DC_NAN,		DC_INFp,	DC_INFp,	DC_INFp,	DC_INFp,	DC_INFp,	DC_NAN },
	/* NaN	 */ {	DC_NAN,		DC_NAN,		DC_NAN,		DC_NAN,		DC_NAN,		DC_NAN,		DC_NAN }
};

//////////////////////////////////////////////////////////////////////////
CShader5xTest_DoubleArith::~CShader5xTest_DoubleArith()
{

}
//////////////////////////////////////////////////////////////////////////
CShader5xTest_dadd::~CShader5xTest_dadd()
{

}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_DoubleArith::Setup()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;

	// Call the base setup
	tRes = CShader5xTest::Setup();

	if( RESULT_PASS != tRes )
		goto setupDone;

	// Doubles are FL11+ only
	if( !g_TestApp.IsFL11OrGreater() )
	{
		WriteToLog( _T("Double instructions are only for Feature Level 11 or greater.") );
		tRes = RESULT_SKIP;
		goto setupDone;
	}

	// Doubles are optional. We should only run double tests if they are supported
	if( !g_TestApp.DoublesAreSupported() )
	{
		tRes = RESULT_SKIP;
		goto setupDone;
	}

setupDone:
	return tRes;
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_DoubleArith::SetupTestCase()
{
	// Local variables
	TEST_RESULT tRes = RESULT_PASS;

	// Call the Shader5x SetupTestCase to see if we should skip based on common test factors (shader stage, etc...)
	tRes = CShader5xTest::SetupTestCase();

	if( RESULT_PASS != tRes )
		goto setupDone;

setupDone:
	return tRes;
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_DoubleArith::Binary64Vec2 const Values_dadd[] = 
{ 
	CShader5xTest_DoubleArith::s_NegINF64, CShader5xTest_DoubleArith::s_NegINF64,
	{ 0xDD0306CED8869A6Bull, 0x800F4A8B6037B1F5ull }, // -Norm, -DeNorm
	CShader5xTest_DoubleArith::s_NegZero64, CShader5xTest_DoubleArith::s_NegZero64,
	CShader5xTest_DoubleArith::s_Zero64, CShader5xTest_DoubleArith::s_Zero64,
	{ 0x000F4A8B6037B1F5ull, 0x5D0306CED8869A6Bull }, // +DeNorm, +Norm
	CShader5xTest_DoubleArith::s_PosINF64, CShader5xTest_DoubleArith::s_PosINF64,
	{ CShader5xTest_DoubleArith::s_Exponent64.m_AsULongLong | CShader5xTest_DoubleArith::s_OneULP64.m_AsULongLong, // NaN
	0x7FF72172155D4FDBull }, // NaN
};

//////////////////////////////////////////////////////////////////////////
void CShader5xTest_dadd::InitTestParameters()
{
	CTestCaseParameter<Binary64Vec2>* pSrc0 = AddParameter<Binary64Vec2>( _T("Src0"), &m_Src0 );
	
	testfactor::RFactor src0_t0 = AddParameterValueSet( pSrc0, new CTableValueSet<Binary64Vec2>( Values_dadd, sizeof(Binary64Vec2), sizeof(Values_dadd) / sizeof(Binary64Vec2) ) );
	testfactor::RFactor src0_t1 = AddParameterValueSet( pSrc0, new CTableValueSet<Binary64Vec2>( s_DAddSrc0, sizeof(Binary64Vec2), sizeof(s_DAddSrc0) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<Binary64Vec2>* pSrc1 = AddParameter<Binary64Vec2>( _T("Src1"), &m_Src1 );

	testfactor::RFactor src1_t0 = AddParameterValueSet( pSrc1, new CTableValueSet<Binary64Vec2>( Values_dadd, sizeof(Binary64Vec2), sizeof(Values_dadd) / sizeof(Binary64Vec2) ) );
	testfactor::RFactor src1_t1 = AddParameterValueSet( pSrc1, new CTableValueSet<Binary64Vec2>( s_DAddSrc1, sizeof(Binary64Vec2), sizeof(s_DAddSrc1) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<Binary64Vec2>* pResultParam = AddParameter<Binary64Vec2>( _T("Result"), &m_Result );
	{
		Binary64Vec2 def = { s_Zero64, s_Zero64 };
		SetParameterDefaultValue( pResultParam, def );
	}
	testfactor::RFactor res_t1 = AddParameterValueSet(pResultParam, new CTableValueSet<Binary64Vec2>( s_DAddRes, sizeof(Binary64Vec2), sizeof(s_DAddRes) / sizeof(Binary64Vec2) ) );

	testfactor::RFactor src0Class = AddParameter<DOUBLE_CLASSIFICATION>( _T("Src0Class"), &m_Src0Class, new CTableValueSet<DOUBLE_CLASSIFICATION>( DoubleSrcAddTable, sizeof(DOUBLE_CLASSIFICATION), sizeof(DoubleSrcAddTable) / sizeof(DOUBLE_CLASSIFICATION) ) );
	testfactor::RFactor src1Class = AddParameter<DOUBLE_CLASSIFICATION>( _T("Src1Class"), &m_Src1Class, new CTableValueSet<DOUBLE_CLASSIFICATION>( DoubleSrcAddTable, sizeof(DOUBLE_CLASSIFICATION), sizeof(DoubleSrcAddTable) / sizeof(DOUBLE_CLASSIFICATION) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xyzw") );
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("xyzw"));
	pMaskValueSet->AddValue(_T(""));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("zw"));
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xyzw") );
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xyzw") );
	pSwizzleValueSet->AddValue( _T("zwxy") );
	pSwizzleValueSet->AddValue( _T("xyxy") ); // stands for .xy or .xyxy
	pSwizzleValueSet->AddValue( _T("zwzw") ); // stands for .zw or .zwzw 
	pSwizzleValueSet->AddValue( _T("") );
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );

	CTestCaseParameter<bool>* pGenParam = AddPrivateParameter<bool>( _T("Generated"), &m_Generated );
	SetParameterDefaultValue( pGenParam, false );
	testfactor::RFactor genFactor = AddParameterValue<bool>( pGenParam, true );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrc0Mod = AddParameter<SRC_MOD_TYPE>( _T("Src0Mod"), &m_Src0Modifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrc0Mod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrc0ModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrc0ModValueSet->AddValue( SMOD_NONE );
	pSrc0ModValueSet->AddValue( SMOD_NEG );
	pSrc0ModValueSet->AddValue( SMOD_ABS );
	pSrc0ModValueSet->AddValue( SMOD_ABSNEG );
	testfactor::RFactor src0Mod = AddParameterValueSet( pSrc0Mod, pSrc0ModValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrc1Mod = AddParameter<SRC_MOD_TYPE>( _T("Src1Mod"), &m_Src1Modifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrc1Mod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrc1ModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrc1ModValueSet->AddValue( SMOD_NONE );
	pSrc1ModValueSet->AddValue( SMOD_NEG );
	pSrc1ModValueSet->AddValue( SMOD_ABS );
	pSrc1ModValueSet->AddValue( SMOD_ABSNEG );
	testfactor::RFactor src1Mod = AddParameterValueSet( pSrc1Mod, pSrc1ModValueSet );

	CTestCaseParameter<INST_MOD_TYPE>* pInstMod = AddParameter<INST_MOD_TYPE>( _T("InstructionModifier"), &m_InstModifier );
	SetParameterDefaultValue<INST_MOD_TYPE>( pInstMod, IMOD_NONE );
	CUserValueSet<INST_MOD_TYPE>* pInstModValueSet = new CUserValueSet<INST_MOD_TYPE>( IMOD_NONE, IMOD_SAT );
	testfactor::RFactor instMod = AddParameterValueSet( pInstMod, pInstModValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc1Type = AddParameter<OPERAND_TYPE>( _T("Operand1Type"), &m_OperandType[0] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc1Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand1TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src1Type = AddParameterValueSet( pSrc1Type, pOperand1TypeValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc2Type = AddParameter<OPERAND_TYPE>( _T("Operand2Type"), &m_OperandType[1] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc2Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand2TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src2Type = AddParameterValueSet( pSrc2Type, pOperand2TypeValueSet );
	
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::RFactor tableFactor = (src0_t0 % src0Class) * (src1_t0 % src1Class);

	testfactor::XFACTOR pairwiseGroup1[] = 
	{
		{ instMod, 1, NULL, 0 },
		{ maskFactor, 1, NULL, 0 },
		{ src0Mod, 1, NULL, 0 },
		{ src1Mod, 1, NULL, 0 },
		{ src1Type, 1, NULL, 0 },
		{ src2Type, 1, NULL, 0 },
	};

	testfactor::RFactor exp2[] = 
	{
		tableFactor,
		NewCrossFactor( pairwiseGroup1, sizeof(pairwiseGroup1) / sizeof(testfactor::XFACTOR) )
	};

	testfactor::RFactor exp3[] = 
	{
		src0_t1 % src1_t1 %	res_t1,
		swizzleFactor * maskFactor * genFactor
	};

	// Add the shader stages to each factor
	testfactor::RFactor rFactor1 =  ((src0_t0 % src0Class) * (src1_t0 % src1Class) * instMod * maskFactor);
	testfactor::RFactor rfFactor1WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor1 );

	testfactor::RFactor rFactor2 = NewSetFactor( exp2, sizeof(exp2) / sizeof(testfactor::RFactor) );
	testfactor::RFactor rfFactor2WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor2 );

	testfactor::RFactor rFactor3 = NewSetFactor( exp3, sizeof(exp3) / sizeof(testfactor::RFactor) );
	testfactor::RFactor rfFactor3WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor3 );

	SetRootTestFactor( rfFactor1WithShaderStages + rfFactor2WithShaderStages + rfFactor3WithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstructionModifier"), IMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_TEMP), WeightFactorLarge);	// Mix and match Temp vs CB for high pri to get extra coverage
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_CONSTANT_BUFFER), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_CONSTANT_BUFFER), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_TEMP), WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 5.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dadd::SetupTestCase()
{
	// Local variables
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	tRes = CShader5xTest_DoubleArith::SetupTestCase();

	if( RESULT_PASS != tRes )
		return tRes;

	const tstring neg0String = (SMOD_NEG & m_Src0Modifier) ? _T("-") : _T("");
	const tstring neg1String = (SMOD_NEG & m_Src1Modifier) ? _T("-") : _T("");
	const tstring abs0String_pre = (SMOD_ABS & m_Src0Modifier) ? _T("abs(") : _T("");
	const tstring abs1String_pre = (SMOD_ABS & m_Src1Modifier) ? _T("abs(") : _T("");
	const tstring abs0String_suf = (SMOD_ABS & m_Src0Modifier) ? _T(")") : _T("");
	const tstring abs1String_suf = (SMOD_ABS & m_Src1Modifier) ? _T(")") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;
	const tstring modString = (IMOD_SAT & m_InstModifier) ? _T("_sat") : _T("");

	ID3D10Blob* pShaderBuf(NULL);

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);

	assert((OT_TEMP == m_OperandType[0]) || (OT_CONSTANT_BUFFER == m_OperandType[0]));
	assert((OT_TEMP == m_OperandType[1]) || (OT_CONSTANT_BUFFER == m_OperandType[1]));

	if(OT_TEMP == m_OperandType[0])
	{
		asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	}

	if(OT_TEMP == m_OperandType[1])
	{
		asmHelper.SetupMovToTempFromCB( 1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	}

	asmHelper.SetupMovToTempFromLiteral( 2, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	asmHelper.SetupMovToRawOutputFromTemp( 0, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();

	tstring instructionCode = FormatString( _T("%s%s %s%s, %s%s%s%s%s, %s%s%s%s%s"), 
											_T("dadd"),
											modString.c_str(),

											_T("r2"),
											maskString.c_str(),

											neg0String.c_str(),
											abs0String_pre.c_str(),
											OT_TEMP == m_OperandType[0] ? _T("r0") : _T("c0[0]"),
											swizString.c_str(),
											abs0String_suf.c_str(),

											neg1String.c_str(),
											abs1String_pre.c_str(),
											OT_TEMP == m_OperandType[1] ? _T("r1") : _T("c0[1]"),
											swizString.c_str(),
											abs1String_suf.c_str()
									);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
												GetEffectiveContext(),
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												instructionCode,
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);
	return tRes;
}

//////////////////////////////////////////////////////////////////////////
// Compare Binary64 values.
bool CShader5xTest_DoubleArith::CompareBinary64(Binary64 const& gold,
												Binary64 const& res,
												bool bAllowDeNormFlush,
                                                bool bAllowOneULPDifference )
{
	// Check for Nan first.
	if( IsNaN(res) && IsNaN(gold) )
	{
		// no-op
	}
	else if( (res == gold) == false )
	{
		if( bAllowDeNormFlush && IsDNorm(gold) )
		{
			if( res == s_Zero64 || res == s_NegZero64 )
			{
				return true;
			}
		}

        if ( bAllowOneULPDifference == true 
            && CompareDoubleULP( gold.m_AsDouble, res.m_AsDouble, 1 ) == true )
        {
            return true;
        }

		return false;
	}

	return true;
}

//////////////////////////////////////////////////////////////////////////
bool CShader5xTest_DoubleArith::CompareBinary64Vec2(	Binary64Vec2 const& gold,
														Binary64Vec2 const& res,
														tstring const& reg,
														tstring const& mask, 
														tstring const& swizzle,
														bool bAllowDeNormFlush,
														bool bSuppressErrorMsg,
                                                        bool bAllowOneULPDifference )
{
	bool tr = true;
	tstring const allowedMasks[] = { _T("xy"), _T("zw") };
	tstring const allowedSwizzles[] = { _T("xy"), _T("zw") };
	tstring const tMask = mask.empty() ? _T("xyzw") : mask;
	tstring const tSwizzle = swizzle.empty() ? _T("xyzw") : swizzle;
	unsigned int maskWindow(0);
	unsigned int swizzleWindow(0);

	for( unsigned int m(0); m < sizeof(allowedMasks) / sizeof(tstring); ++m )
	{
		if( (tMask.size() > maskWindow * 2) &&
			( tMask.substr( maskWindow * 2, (maskWindow + 1) * 2 ) == allowedMasks[m] ) )
		{
			// Compare to swizzled result
			for( unsigned int s(0); s < sizeof(allowedSwizzles) / sizeof(tstring); ++s )
			{
				if( tSwizzle.substr( swizzleWindow * 2, (swizzleWindow + 1) * 2 ) == allowedSwizzles[s] )
				{
					if( CompareBinary64( gold[s], res[m], bAllowDeNormFlush, bAllowOneULPDifference ) == false )
					{
						if ( bSuppressErrorMsg == false )
						{
							WriteToLog( 1, _T("CShader5xTest_DoubleArith::CompareBinary64Vec2() %s.%s Expected: %+.18Lg  Result: %+.18Lg"), reg.c_str(), allowedMasks[m].c_str(), gold[s].m_AsDouble, res[m].m_AsDouble );
							WriteToLog( 1, _T("CShader5xTest_DoubleArith::CompareBinary64Vec2() %s.%s Expected: 0x%016llX  Result: 0x%016llX"), reg.c_str(), allowedMasks[m].c_str(), gold[s].m_AsULongLong, res[m].m_AsULongLong );
						}

						tr = false;
					}
					break;
				}
			}

			++maskWindow;
		}
		else
		{
			// compare to UnWritten
			if( res[m].m_AsULong[0] != UnWritten ||
				res[m].m_AsULong[1] != UnWritten )
			{
				WriteToLog( 1, _T("CShader5xTest_DoubleArith::CompareBinary64() %s.%s was masked off.  Expected: 0x%08X%08X Result: 0x%016llX"), reg.c_str(), allowedMasks[m].c_str(), UnWritten, UnWritten, res[m].m_AsULongLong );
				tr = false;
			}
		}
		
		if( tSwizzle.size() > 2 )
			++swizzleWindow;
	}
	return tr;
}

//////////////////////////////////////////////////////////////////////////
// Check result using a function.
bool CShader5xTest_DoubleArith::CheckBinary64Vec2Result(	CShader5xTest_DoubleArith::Binary64Vec2 const& res,
															tstring const& reg,
															tstring const& mask,
															IsA const pIsAFunc,
															tstring const& message,
															bool bAllowDeNormFlush,
															bool bSuppressErrorMsg )
{
	bool tr = true;
	tstring const allowedMasks[] = { _T("xy"), _T("zw") };
	unsigned int maskWindow(0);
	tstring const tMask = mask.empty() ? _T("xyzw") : mask;

	for( unsigned int m(0); m < sizeof(allowedMasks) / sizeof(tstring); ++m )
	{	
		if( (tMask.size() > maskWindow * 2) &&
			( tMask.substr( maskWindow * 2, (maskWindow + 1) * 2 ) == allowedMasks[m] ) )
		{
			// Compare to swizzled result
			if ( bAllowDeNormFlush && ( IsDNorm( res[m] ) || IsPosOrNegZero( res[m] ) ) )
			{
				// valid
			}
			else if( (*pIsAFunc)(res[m]) == false )
			{
				if ( bSuppressErrorMsg == false )
				{
					WriteToLog( 1, _T("CheckBinary64Vec2Result() %s.%s Expected: %s Result: %+.18Lg  0x%016llX"), reg.c_str(), allowedMasks[m].c_str(), message.c_str(), res[m].m_AsDouble, res[m].m_AsULongLong );
				}
				
				tr = false;
			}

			++maskWindow;
		}
		else
		{
			// compare to UnWritten
			if( res[m].m_AsULong[0] != UnWritten ||
				res[m].m_AsULong[1] != UnWritten )
			{
				WriteToLog( 1, _T("CheckBinary64Vec2Result() %s.%s was masked off.  Expected: 0x%08X%08X Result: 0x%016llX"), reg.c_str(), allowedMasks[m].c_str(), UnWritten, UnWritten, res[m].m_AsULongLong );
				tr = false;
			}
		}
		
	}
	return tr;
}

//////////////////////////////////////////////////////////////////////////
DOUBLE_CLASSIFICATION CShader5xTest_DoubleArith::GetModifiedClass( DOUBLE_CLASSIFICATION const& dClass, SRC_MOD_TYPE const& mod )
{
	assert( dClass < NUM_DOUBLE_CLASSIFICATION_RESULT_TYPES );

	static DOUBLE_CLASSIFICATION const helper[NUM_SRC_MOD_TYPES][NUM_DOUBLE_CLASSIFICATION_RESULT_TYPES] = 
	{
		{ // SMOD_NONE   This must match enum DOUBLE_CLASSIFICATION in Shader5x.h
			DC_INFn, DC_REALn, DC_ONEn, DC_ZEROn, DC_ZEROp, DC_ONEp, DC_REALp, DC_INFp, DC_NAN, DC_SRC0n, DC_SRC0,  DC_SRC1n, DC_SRC1,  DC_REAL, DC_NORM, DC_NORMn, DC_NORMp, DC_DENORM, DC_DENORMn, DC_DENORMp
		},
		{ // SMOD_NEG
			DC_INFp, DC_REALp, DC_ONEp, DC_ZEROp, DC_ZEROn, DC_ONEn, DC_REALn, DC_INFn, DC_NAN, DC_SRC0,  DC_SRC0n, DC_SRC1,  DC_SRC1n, DC_REAL, DC_NORM, DC_NORMp, DC_NORMn, DC_DENORM, DC_DENORMp, DC_DENORMn
		},
		{ // SMOD_ABS
			DC_INFp, DC_REALp, DC_ONEp, DC_ZEROp, DC_ZEROp, DC_ONEp, DC_REALp, DC_INFp, DC_NAN, DC_SRC0,  DC_SRC0,  DC_SRC1,  DC_SRC1,  DC_REAL, DC_NORM, DC_NORMp, DC_NORMp, DC_DENORM, DC_DENORMp, DC_DENORMp
		},
		{ // SMOD_ABSNEG
			DC_INFn, DC_REALn, DC_ONEn, DC_ZEROn, DC_ZEROn, DC_ONEn, DC_REALn, DC_INFn, DC_NAN, DC_SRC0n, DC_SRC0n, DC_SRC1n, DC_SRC1n, DC_REAL, DC_NORM, DC_NORMn, DC_NORMn, DC_DENORM, DC_DENORMn, DC_DENORMn
		}
	};

	return helper[mod][dClass];
}
//////////////////////////////////////////////////////////////////////////
DOUBLE_CLASSIFICATION CShader5xTest_DoubleArith::GetModifiedClass( DOUBLE_CLASSIFICATION const& dClass, INST_MOD_TYPE const& mod )
{
	// For the _SAT modifier
	// A result of -0 can be saturated to either +0 or -0
	// The spec allows for either.
	// -1 falls into 2 classes: DC_REALn and DC_ZEROn
	// So either of these can map to +0 or -0
	assert( dClass < NUM_DOUBLE_CLASSIFICATION_RESULT_TYPES );
	static DOUBLE_CLASSIFICATION const helper[NUM_INST_MOD_TYPES][NUM_DOUBLE_CLASSIFICATION_RESULT_TYPES] = 
	{
		{ // IMOD_NONE	 This must match enum DOUBLE_CLASSIFICATION in Shader5x.h
			DC_INFn,  DC_REALn,  DC_ONEn,  DC_ZEROn,  DC_ZEROp, DC_ONEp, DC_REALp,          DC_INFp, DC_NAN,   DC_SRC0n,           DC_SRC0,           DC_SRC1n,           DC_SRC1,           DC_REAL,           DC_NORM,           DC_NORMn,          DC_NORMp,          DC_DENORM, DC_DENORMn, DC_DENORMp
		},
		{ // IMOD_SAT
			DC_ZEROp, DC_ZEROpn, DC_ZEROp, DC_ZEROpn, DC_ZEROp, DC_ONEp, DC_SATURATED_REAL, DC_ONEp, DC_ZEROp, DC_SATURATED_SRC0n, DC_SATURATED_SRC0, DC_SATURATED_SRC1n, DC_SATURATED_SRC1, DC_SATURATED_REAL, DC_SATURATED_REAL, DC_SATURATED_REAL, DC_SATURATED_REAL, DC_DENORM, DC_DENORM,  DC_DENORM  
		}
	};

	return helper[mod][dClass];
}

//////////////////////////////////////////////////////////////////////////
template<typename _Type>
_Type CShader5xTest_DoubleArith::GetModifiedValue( _Type const& v, SRC_MOD_TYPE const& mod )
{
	typedef _Type (*SrcModFunc)( _Type const& );
	static SrcModFunc const modifiers[] = 
	{
		&ReturnValue<_Type>,
		&NegateValue<_Type, &ReturnValue<_Type> >, 
		&AbsValue<_Type, &ReturnValue<_Type> >, 
		&NegateValue<_Type, &AbsValue<_Type, &ReturnValue<_Type> > > 
	};

	return (*(modifiers[mod]))(v);
}

//////////////////////////////////////////////////////////////////////////
void CShader5xTest_DoubleArith::Binary64::Saturate()
{
	for( unsigned int i(0); i < 2; ++i )
	{
		if( IsNegZero(*this) )
		{
			*this = s_Zero64;
		}
		else if( IsNaN(*this) )
		{
			*this = s_Zero64;
		}
		else if( IsPosINF(*this) )
		{
			*this = s_One64;
		}
		else if( IsNegINF(*this) )
		{
			*this = s_Zero64;
		}
		else
		{
			if( m_AsDouble > s_One64.m_AsDouble )
			{
				*this = s_One64;
			}
			else if( IsNegZero(*this) )
			{
				// no-op
			}
			else if( m_AsDouble < s_Zero64.m_AsDouble )
			{
				*this = s_Zero64;
			}
		}
	}
}
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_DoubleArith::Binary64Vec2::Saturate()
{
	m_X.Saturate();
	m_Y.Saturate();
}
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_DoubleArith::Binary64::Negate()
{
	m_AsULongLong ^= s_Sign64.m_AsULongLong;
}
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_DoubleArith::Binary64Vec2::Negate()
{
	m_X.Negate();
	m_Y.Negate();
}
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_DoubleArith::Binary64::Abs()
{
	m_AsULongLong &= ( s_Exponent64.m_AsULongLong | s_Fraction64.m_AsULongLong);
}
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_DoubleArith::Binary64Vec2::Abs()
{
	m_X.Abs();
	m_Y.Abs();
}
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_DoubleArith::Binary32Vec2::Negate()
{
	m_VecAsUInt[0] ^= 0x80000000u;
	m_VecAsUInt[1] ^= 0x80000000u;
}
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_DoubleArith::Binary32Vec2::Abs()
{
	m_VecAsUInt[0] &= 0x7FFFFFFFu;
	m_VecAsUInt[1] &= 0x7FFFFFFFu;
}
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_DoubleArith::SignedBinary32Vec2::Negate()
{
	m_VecAsInt[0] *= -1;
	m_VecAsInt[1] *= -1;
}
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_DoubleArith::SignedBinary32Vec2::Abs()
{
	m_VecAsInt[0] = abs(m_VecAsInt[0]);
	m_VecAsInt[1] = abs(m_VecAsInt[1]);
}
//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dadd::ExecuteTestCase()
{
	HRESULT hr;
	{
		struct CBufferLayout
		{
			Binary64Vec2 m_Src0;
			Binary64Vec2 m_Src1;
		};

		CBufferLayout cb = { m_Src0, m_Src1 };

		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, cb ) ) )
		{
			WriteToLog( _T("CShader5xTest_dadd::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	if( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->Draw( 1, 0 );
	}

	GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	{
		Binary64Vec2 result;
		TEST_RESULT tr = RESULT_PASS;
		if( FAILED( hr = MapAndRead( GetImmediateContext(), m_pRTBufferCopy, result ) ) )
		{
			WriteToLog( _T("CShader5xTest_dadd::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s."), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		if( m_Generated )
		{
			if( CompareBinary64Vec2( m_Result, result, _T("o0"), m_Mask, m_Swizzle ) == false )
			{
				tr = RESULT_FAIL;
			}
		}
		else
		{
			static unsigned int const IndexTable[] = 
			{
				0,//DC_INFn,
				1,//DC_REALn,
				-1,//DC_ONEn,
				2,//DC_ZEROn,
				3,//DC_ZEROp,
				-1,//DC_ONEp,
				4,//DC_REALp,
				5,//DC_INFp,
				6,//DC_NAN,
				-1,//DC_SRC0n,
				-1,//DC_SRC0,
				-1,//DC_SRC1n,
				-1,//DC_SRC1,
				-1,//DC_REAL,
				-1,//DC_NORM,
				-1,//DC_NORMn,
				-1,//DC_NORMp,
				-1,//DC_DENORM,
				-1,//DC_DENORMn,
				-1,//DC_DENORMp
			};
			// Determine what the result should be.
			DOUBLE_CLASSIFICATION const src0Class = GetModifiedClass( m_Src0Class, m_Src0Modifier );
			DOUBLE_CLASSIFICATION const src1Class = GetModifiedClass( m_Src1Class, m_Src1Modifier );
			DOUBLE_CLASSIFICATION const resClass = GetModifiedClass( DoubleAddTable[IndexTable[src0Class]][IndexTable[src1Class]], m_InstModifier );
			DoubleClassInfo dci = DoubleClassInfoTable[resClass];

			if( dci.m_pValue )
			{
				// Do a value compare
				Binary64Vec2 const gold = { *(dci.m_pValue), *(dci.m_pValue) };
				if( CompareBinary64Vec2( gold, result, _T("o0"), m_Mask, m_Swizzle ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else if( dci.m_pCatFunc )
			{
				// Use a function to check the result.
				if( CheckBinary64Vec2Result( result, _T("o0"), m_Mask, dci.m_pCatFunc, dci.m_Name ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else if( dci.m_pSrcFunc )
			{
				// Use a src value as the gold result.
				// Do a value compare.
				Binary64Vec2 const gold = (*(dci.m_pSrcFunc))(	GetModifiedValue( m_Src0, m_Src0Modifier), 
																GetModifiedValue( m_Src1, m_Src1Modifier) );
				if( CompareBinary64Vec2( gold, result, _T("o0"), m_Mask, m_Swizzle ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else
			{
				WriteToLog( 1, _T("CShader5xTest_dadd::ExecuteTestCase() No method for checking the result of %s"), dci.m_Name.c_str() );
				tr = RESULT_FAIL;
			}
		}
		return tr;
	}
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_DoubleArith::Binary64Vec2 const Values_dmul[] = 
{ 
	CShader5xTest_DoubleArith::s_NegINF64, CShader5xTest_DoubleArith::s_NegINF64,
	{ 0xDD0306CED8869A6Bull, 0x800F4A8B6037B1F5ull }, // -Norm, -DeNorm
	CShader5xTest_DoubleArith::s_NegOne64, CShader5xTest_DoubleArith::s_NegOne64,
	CShader5xTest_DoubleArith::s_NegZero64, CShader5xTest_DoubleArith::s_NegZero64,
	CShader5xTest_DoubleArith::s_Zero64, CShader5xTest_DoubleArith::s_Zero64,
	CShader5xTest_DoubleArith::s_One64, CShader5xTest_DoubleArith::s_One64,
	{ 0x000F4A8B6037B1F5ull, 0x5D0306CED8869A6Bull }, // +DeNorm, +Norm
	CShader5xTest_DoubleArith::s_PosINF64, CShader5xTest_DoubleArith::s_PosINF64,
	{ CShader5xTest_DoubleArith::s_Exponent64.m_AsULongLong | CShader5xTest_DoubleArith::s_OneULP64.m_AsULongLong, // NaN
	0x7FF72172155D4FDBull }, // NaN
};
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_dmul::InitTestParameters()
{
	CTestCaseParameter<Binary64Vec2>* pSrc0 = AddParameter<Binary64Vec2>( _T("Src0"), &m_Src0 );
	
	testfactor::RFactor src0_t0 = AddParameterValueSet( pSrc0, new CTableValueSet<Binary64Vec2>( Values_dmul, sizeof(Binary64Vec2), sizeof(Values_dmul) / sizeof(Binary64Vec2) ) );
	testfactor::RFactor src0_t1 = AddParameterValueSet( pSrc0, new CTableValueSet<Binary64Vec2>( s_DMulSrc0, sizeof(Binary64Vec2), sizeof(s_DMulSrc0) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<Binary64Vec2>* pSrc1 = AddParameter<Binary64Vec2>( _T("Src1"), &m_Src1 );

	testfactor::RFactor src1_t0 = AddParameterValueSet( pSrc1, new CTableValueSet<Binary64Vec2>( Values_dmul, sizeof(Binary64Vec2), sizeof(Values_dmul) / sizeof(Binary64Vec2) ) );
	testfactor::RFactor src1_t1 = AddParameterValueSet( pSrc1, new CTableValueSet<Binary64Vec2>( s_DMulSrc1, sizeof(Binary64Vec2), sizeof(s_DMulSrc1) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<Binary64Vec2>* pResultParam = AddParameter<Binary64Vec2>( _T("Result"), &m_Result );
	{
		Binary64Vec2 def = { s_Zero64, s_Zero64 };
		SetParameterDefaultValue( pResultParam, def );
	}
	testfactor::RFactor res_t1 = AddParameterValueSet(pResultParam, new CTableValueSet<Binary64Vec2>( s_DMulRes, sizeof(Binary64Vec2), sizeof(s_DMulRes) / sizeof(Binary64Vec2) ) );

	testfactor::RFactor src0Class = AddParameter<DOUBLE_CLASSIFICATION>( _T("Src0Class"), &m_Src0Class, new CTableValueSet<DOUBLE_CLASSIFICATION>( DoubleSrcMulTable, sizeof(DOUBLE_CLASSIFICATION), sizeof(DoubleSrcMulTable) / sizeof(DOUBLE_CLASSIFICATION) ) );
	testfactor::RFactor src1Class = AddParameter<DOUBLE_CLASSIFICATION>( _T("Src1Class"), &m_Src1Class, new CTableValueSet<DOUBLE_CLASSIFICATION>( DoubleSrcMulTable, sizeof(DOUBLE_CLASSIFICATION), sizeof(DoubleSrcMulTable) / sizeof(DOUBLE_CLASSIFICATION) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xyzw") );
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("xyzw"));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("zw"));
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xyzw") );
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xyzw") );
	pSwizzleValueSet->AddValue( _T("zwxy") );
	pSwizzleValueSet->AddValue( _T("xyxy") ); // stands for .xy or .xyxy
	pSwizzleValueSet->AddValue( _T("zwzw") ); // stands for .zw or .zwzw 
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );

	CTestCaseParameter<bool>* pGenParam = AddPrivateParameter<bool>( _T("Generated"), &m_Generated );
	SetParameterDefaultValue( pGenParam, false );
	testfactor::RFactor genFactor = AddParameterValue<bool>( pGenParam, true );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrc0Mod = AddParameter<SRC_MOD_TYPE>( _T("Src0Mod"), &m_Src0Modifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrc0Mod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrc0ModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrc0ModValueSet->AddValue( SMOD_NONE );
	pSrc0ModValueSet->AddValue( SMOD_NEG );
	pSrc0ModValueSet->AddValue( SMOD_ABS );
	pSrc0ModValueSet->AddValue( SMOD_ABSNEG );
	testfactor::RFactor src0Mod = AddParameterValueSet( pSrc0Mod, pSrc0ModValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrc1Mod = AddParameter<SRC_MOD_TYPE>( _T("Src1Mod"), &m_Src1Modifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrc1Mod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrc1ModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrc1ModValueSet->AddValue( SMOD_NONE );
	pSrc1ModValueSet->AddValue( SMOD_NEG );
	pSrc1ModValueSet->AddValue( SMOD_ABS );
	pSrc1ModValueSet->AddValue( SMOD_ABSNEG );
	testfactor::RFactor src1Mod = AddParameterValueSet( pSrc1Mod, pSrc1ModValueSet );

	CTestCaseParameter<INST_MOD_TYPE>* pInstMod = AddParameter<INST_MOD_TYPE>( _T("InstructionModifier"), &m_InstModifier );
	SetParameterDefaultValue<INST_MOD_TYPE>( pInstMod, IMOD_NONE );
	CUserValueSet<INST_MOD_TYPE>* pInstModValueSet = new CUserValueSet<INST_MOD_TYPE>( IMOD_NONE, IMOD_SAT );
	testfactor::RFactor instMod = AddParameterValueSet( pInstMod, pInstModValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc1Type = AddParameter<OPERAND_TYPE>( _T("Operand1Type"), &m_OperandType[0] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc1Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand1TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src1Type = AddParameterValueSet( pSrc1Type, pOperand1TypeValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc2Type = AddParameter<OPERAND_TYPE>( _T("Operand2Type"), &m_OperandType[1] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc2Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand2TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src2Type = AddParameterValueSet( pSrc2Type, pOperand2TypeValueSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::RFactor tableFactor = (src0_t0 % src0Class) * (src1_t0 % src1Class);

	testfactor::XFACTOR pairwiseGroup1[] = 
	{
		{ instMod, 1, NULL, 0 },
		{ maskFactor, 1, NULL, 0 },
		{ src0Mod, 1, NULL, 0 },
		{ src1Mod, 1, NULL, 0 },
		{ src1Type, 1, NULL, 0 },
		{ src2Type, 1, NULL, 0 },
	};

	testfactor::RFactor exp2[] = 
	{
		tableFactor,
		NewCrossFactor( pairwiseGroup1, sizeof(pairwiseGroup1) / sizeof(testfactor::XFACTOR) )
	};

	testfactor::RFactor exp3[] = 
	{
		src0_t1 % src1_t1 %	res_t1,
		swizzleFactor * maskFactor * genFactor
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rFactor1 = ((src0_t0 % src0Class) * (src1_t0 % src1Class) * instMod * maskFactor);
	testfactor::RFactor rfFactor1WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor1 );

	testfactor::RFactor rFactor2 = NewSetFactor( exp2, sizeof(exp2) / sizeof(testfactor::RFactor) );
	testfactor::RFactor rfFactor2WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor2 );

	testfactor::RFactor rFactor3 = NewSetFactor( exp3, sizeof(exp3) / sizeof(testfactor::RFactor) );
	testfactor::RFactor rfFactor3WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor3 );

	SetRootTestFactor( rfFactor1WithShaderStages + rfFactor2WithShaderStages + rfFactor3WithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstructionModifier"), IMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_TEMP), WeightFactorLarge);	// Mix and match Temp vs CB for high pri to get extra coverage
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_CONSTANT_BUFFER), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_CONSTANT_BUFFER), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_TEMP), WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 5.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dmul::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	tRes = CShader5xTest_DoubleArith::SetupTestCase();

	if( RESULT_PASS != tRes )
		return tRes;

	const tstring neg0String = (SMOD_NEG & m_Src0Modifier) ? _T("-") : _T("");
	const tstring neg1String = (SMOD_NEG & m_Src1Modifier) ? _T("-") : _T("");
	const tstring abs0String_pre = (SMOD_ABS & m_Src0Modifier) ? _T("abs(") : _T("");
	const tstring abs1String_pre = (SMOD_ABS & m_Src1Modifier) ? _T("abs(") : _T("");
	const tstring abs0String_suf = (SMOD_ABS & m_Src0Modifier) ? _T(")") : _T("");
	const tstring abs1String_suf = (SMOD_ABS & m_Src1Modifier) ? _T(")") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;
	const tstring modString = (IMOD_SAT & m_InstModifier) ? _T("_sat") : _T("");

	ID3D10Blob* pShaderBuf(NULL);

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);

	assert((OT_TEMP == m_OperandType[0]) || (OT_CONSTANT_BUFFER == m_OperandType[0]));
	assert((OT_TEMP == m_OperandType[1]) || (OT_CONSTANT_BUFFER == m_OperandType[1]));

	if(OT_TEMP == m_OperandType[0])
	{
		asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	}

	if(OT_TEMP == m_OperandType[1])
	{
		asmHelper.SetupMovToTempFromCB( 1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	}

	asmHelper.SetupMovToTempFromLiteral( 2, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	asmHelper.SetupMovToRawOutputFromTemp( 0, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();

	tstring instructionCode = FormatString( _T("%s%s %s%s, %s%s%s%s%s, %s%s%s%s%s"), 
											_T("dmul"),
											modString.c_str(),

											_T("r2"),
											maskString.c_str(),

											neg0String.c_str(),
											abs0String_pre.c_str(),
											OT_TEMP == m_OperandType[0] ? _T("r0") : _T("c0[0]"),
											swizString.c_str(),
											abs0String_suf.c_str(),

											neg1String.c_str(),
											abs1String_pre.c_str(),
											OT_TEMP == m_OperandType[1] ? _T("r1") : _T("c0[1]"),
											swizString.c_str(),
											abs1String_suf.c_str()
									);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
												GetEffectiveContext(),
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												instructionCode,
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);
	return tRes;
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_dmul::~CShader5xTest_dmul()
{
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dmul::ExecuteTestCase()
{
	HRESULT hr;
	{
		struct CBufferLayout
		{
			Binary64Vec2 m_Src0;
			Binary64Vec2 m_Src1;
		};

		CBufferLayout cb = { m_Src0, m_Src1 };

		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, cb ) ) )
		{
			WriteToLog( _T("CShader5xTest_dmul::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	if( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->Draw( 1, 0 );
	}

	GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	{
		Binary64Vec2 result;
		TEST_RESULT tr = RESULT_PASS;
		if( FAILED( hr = MapAndRead( GetImmediateContext(), m_pRTBufferCopy, result ) ) )
		{
			WriteToLog( _T("CShader5xTest_dmul::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s."), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		if( m_Generated )
		{
			if( CompareBinary64Vec2( m_Result, result, _T("o0"), m_Mask, m_Swizzle ) == false )
			{
				tr = RESULT_FAIL;
			}
		}
		else
		{
			// Determine what the result should be.
			DOUBLE_CLASSIFICATION const src0Class = GetModifiedClass( m_Src0Class, m_Src0Modifier );
			DOUBLE_CLASSIFICATION const src1Class = GetModifiedClass( m_Src1Class, m_Src1Modifier );
			DOUBLE_CLASSIFICATION const resClass = GetModifiedClass( DoubleMulTable[src0Class][src1Class], m_InstModifier );
			DoubleClassInfo dci = DoubleClassInfoTable[resClass];

			if( dci.m_pValue )
			{
				// Do a value compare
				Binary64Vec2 const gold = { *(dci.m_pValue), *(dci.m_pValue) };
				if( CompareBinary64Vec2( gold, result, _T("o0"), m_Mask, m_Swizzle ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else if( dci.m_pCatFunc )
			{
				// Use a function to check the result.
				if( CheckBinary64Vec2Result( result, _T("o0"), m_Mask, dci.m_pCatFunc, dci.m_Name ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else if( dci.m_pSrcFunc )
			{
				// Use a src value as the gold result.
				// Do a value compare.
				Binary64Vec2 const gold = (*(dci.m_pSrcFunc))(	GetModifiedValue( m_Src0, m_Src0Modifier), 
																GetModifiedValue( m_Src1, m_Src1Modifier) );
				if( CompareBinary64Vec2( gold, result, _T("o0"), m_Mask, m_Swizzle ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else
			{
				WriteToLog( 1, _T("CShader5xTest_dmul::ExecuteTestCase() No method for checking the result of %s"), dci.m_Name.c_str() );
				tr = RESULT_FAIL;
			}
		}
		return tr;
	}
}

//////////////////////////////////////////////////////////////////////////
//  ftod
//////////////////////////////////////////////////////////////////////////
//typedef struct 
//{
//	CShader5xTest_DoubleArith::Binary32Vec2 m_FtoDInput;
//	DOUBLE_CLASSIFICATION m_InputClass;
//} FtoDValueInputTable;
const CShader5xTest_DoubleArith::Binary32Vec2 ftodInputTable[] = 
{
	/*{*/ { 0xFF800000u, 0xFF800000u }, //DC_INFn },
	/*{*/ { 0x86A05FB1u, 0x8059AF1Bu }, //DC_REALn },// negative real norm , negative real denorm
	/*{*/ { 0xBF800000u, 0xBF800000u }, //DC_ONEn },
	/*{*/ { 0x80000000u, 0x80000000u }, //DC_ZEROn },
	/*{*/ { 0x00000000u, 0x00000000u }, //DC_ZEROp },
	/*{*/ { 0x3F800000u, 0x3F800000u }, //DC_ONEp },
	/*{*/ { 0x00004823u, 0x5A4CD285u }, //DC_REALp },// real denorm, real norm 
	/*{*/ { 0x7F800000u, 0x7F800000u }, //DC_INFp },
	/*{*/ { 0x7F800001u, 0x7F800001u }//, DC_NAN }// nan
};
//////////////////////////////////////////////////////////////////////////
const CShader5xTest_DoubleArith::Binary64Vec2 ftodOutputTable[] = 
{
	CShader5xTest_DoubleArith::s_NegINF64, CShader5xTest_DoubleArith::s_NegZero64,
	
};
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_ftod::InitTestParameters()
{
	CTestCaseParameter<Binary32Vec2>* pSrc = AddParameter<Binary32Vec2>( _T("Src"), &m_Src );
	
	//testfactor::RFactor src_t0 = AddParameterValueSet( pSrc, new CTableValueSet<Binary32Vec2>( ftodInputTable, sizeof(FtoDValueInputTable), sizeof(ftodInputTable) / sizeof(FtoDValueInputTable) ) );
	testfactor::RFactor src_t0 = AddParameterValueSet( pSrc, new CTableValueSet<Binary32Vec2>( ftodInputTable, sizeof(Binary32Vec2), sizeof(ftodInputTable) / sizeof(Binary32Vec2) ) );

	//testfactor::RFactor res_Class = AddPrivateParameter( _T("ResultClass"), &m_ResultClass, new CTableValueSet<DOUBLE_CLASSIFICATION>( &(ftodInputTable[0].m_InputClass), sizeof(FtoDValueInputTable), sizeof(ftodInputTable) / sizeof(FtoDValueInputTable) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xyzw") );
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("xyzw"));
	pMaskValueSet->AddValue(_T(""));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("zw"));
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xy") );
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xy") );
	pSwizzleValueSet->AddValue( _T("yx") );
	pSwizzleValueSet->AddValue( _T("xx") ); 
	pSwizzleValueSet->AddValue( _T("yy") );  
	pSwizzleValueSet->AddValue( _T("") );
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrcMod = AddParameter<SRC_MOD_TYPE>( _T("SrcMod"), &m_SrcModifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrcMod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrcModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrcModValueSet->AddValue( SMOD_NONE );
	pSrcModValueSet->AddValue( SMOD_NEG );
	testfactor::RFactor srcMod = AddParameterValueSet( pSrcMod, pSrcModValueSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src_t0 * srcMod * swizzleFactor;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("SrcMod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xy")), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_ftod::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	tRes = CShader5xTest_DoubleArith::SetupTestCase();

	if( RESULT_PASS != tRes )
		return tRes;

	const tstring negString = (SMOD_NEG & m_SrcModifier) ? _T("-") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;

	ID3D10Blob* pShaderBuf(NULL);

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);

	asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	asmHelper.SetupMovToTempFromLiteral( 1, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	// my code here
	asmHelper.SetupMovToRawOutputFromTemp( 0, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();

	tstring instructionCode = FormatString( _T("%s %s%s, %s%s%s"), 
											_T("ftod"),

											_T("r1"),
											maskString.c_str(),

											negString.c_str(),
											_T("r0"),
											swizString.c_str()
									);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
												GetEffectiveContext(),
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												instructionCode,
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);
	return tRes;
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_DoubleArith::Binary64 CShader5xTest_DoubleArith::ConvertToDouble( float const& v )
{
	Binary64 value;
	value.m_AsDouble = static_cast<double>(v);
	return value;
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_ftod::ExecuteTestCase()
{
	HRESULT hr;
	{
		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, m_Src ) ) )
		{
			WriteToLog( _T("CShader5xTest_ftod::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	if( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->Draw( 1, 0 );
	}

	GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	{
		Binary64Vec2 result;
		TEST_RESULT tr = RESULT_PASS;
		if( FAILED( hr = MapAndRead( GetImmediateContext(), m_pRTBufferCopy, result ) ) )
		{
			WriteToLog( _T("CShader5xTest_ftod::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s."), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		{
			// Determine what the result should be.
			Binary32Vec2 const srcValue = GetModifiedValue( m_Src, m_SrcModifier );
			Binary64Vec2 const srcAsDouble = { ConvertToDouble( srcValue.m_X ), ConvertToDouble( srcValue.m_Y ) };

			// Translate the swizzle. (from float to double meaning)
			tstring swizzle;
			if( m_Swizzle.empty() )
			{
				swizzle = _T("xyzw");
			}
			else
			{
				for( unsigned int i(0); i < m_Swizzle.size(); ++i )
				{
					switch( m_Swizzle[i] )
					{
						case _T('x'):
							swizzle += _T("xy");
							break;
						case _T('y'):
							swizzle += _T("zw");
							break;
					}
				}
			}
	
			if( CompareBinary64Vec2( srcAsDouble, result, _T("o0"), m_Mask, swizzle, true ) == false )
			{
				const bool v0 = IsDNorm(srcValue.m_X);
				const bool v1 = IsDNorm(srcValue.m_Y);
				if( v0 || v1 )
				{
					Binary64Vec2 const newSrcAsDouble = { v0 ? s_Zero64 : srcAsDouble.m_X, v1 ? s_Zero64 : srcAsDouble.m_Y };
					if( CompareBinary64Vec2( newSrcAsDouble, result, _T("o0"), m_Mask, swizzle ) == false )
					{
						Binary64Vec2 const new2SrcAsDouble = { v0 ? s_NegZero64 : srcAsDouble.m_X, v1 ? s_NegZero64 : srcAsDouble.m_Y };
						if( CompareBinary64Vec2( new2SrcAsDouble, result, _T("o0"), m_Mask, swizzle ) == false )
						{
							tr = RESULT_FAIL;
						}
					}
				}
				else
				{
					tr = RESULT_FAIL;
				}
			}
		}
		return tr;
	}
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_ftod::~CShader5xTest_ftod()
{
}

//////////////////////////////////////////////////////////////////////////
//  dtof
//////////////////////////////////////////////////////////////////////////
const CShader5xTest_DoubleArith::Binary64Vec2 dtofInputTable[] = 
{
	/*{*/ { 0xFFF0000000000000ull, 0xFFF0000000000000ull }, //DC_INFn },
	/*{*/ { 0xC00921FB54442D18ull, 0x800F4A8B6037B1F5ull }, //DC_REALn },// negative real norm , negative real denorm
	/*{*/ { 0xBFF0000000000000ull, 0xBFF0000000000000ull }, //DC_ONEn },
	/*{*/ { 0x8000000000000000ull, 0x8000000000000000ull }, //DC_ZEROn },
	/*{*/ { 0x0000000000000000ull, 0x0000000000000000ull }, //DC_ZEROp },
	/*{*/ { 0x3FF0000000000000ull, 0x3FF0000000000000ull }, //DC_ONEp },
	/*{*/ { 0x000F4A8B6037B1F5ull, 0x000F4A8B6037B1F5ull }, //DC_REALp },// real denorm, real norm 
	/*{*/ { 0x7FF0000000000000ull, 0x7FF0000000000000ull }, //DC_INFp },
	/*{*/ { 0x7FFFFFFFFFFFFFFFull, 0x7FFFFFFFFFFFFFFFull }//, DC_NAN }// nan
};
//////////////////////////////////////////////////////////////////////////
const CShader5xTest_DoubleArith::Binary32Vec2 dtofOutputTable[] = 
{
	0xFF800000u, 0x80000000u,//INFn, ZEROn
};

//////////////////////////////////////////////////////////////////////////
void CShader5xTest_dtof::InitTestParameters()
{
	CTestCaseParameter<Binary64Vec2>* pSrc = AddParameter<Binary64Vec2>( _T("Src"), &m_Src );
	
	
	testfactor::RFactor src_t0 = AddParameterValueSet( pSrc, new CTableValueSet<Binary64Vec2>( dtofInputTable, sizeof(Binary64Vec2), sizeof(dtofInputTable) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xy") );
	
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("x"));
	pMaskValueSet->AddValue(_T("y"));
	pMaskValueSet->AddValue(_T("z"));
	pMaskValueSet->AddValue(_T("w"));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("xz"));
	pMaskValueSet->AddValue(_T("xw"));
	pMaskValueSet->AddValue(_T("yz"));
	pMaskValueSet->AddValue(_T("yw"));
	pMaskValueSet->AddValue(_T("zw"));
	
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xyzw") );
	
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xyzw") );
	pSwizzleValueSet->AddValue( _T("xyxy") );
	pSwizzleValueSet->AddValue( _T("zwxy") ); 
	pSwizzleValueSet->AddValue( _T("zwzw") );  
	
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrcMod = AddParameter<SRC_MOD_TYPE>( _T("SrcMod"), &m_SrcModifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrcMod, SMOD_NONE );
	
	CUserValueSet<SRC_MOD_TYPE> *pSrcModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrcModValueSet->AddValue( SMOD_NONE );
	pSrcModValueSet->AddValue( SMOD_NEG );

	CTestCaseParameter<OPERAND_TYPE>* pSrcType = AddParameter<OPERAND_TYPE>( _T("OperandType"), &m_OperandType );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrcType, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperandTypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor srcType = AddParameterValueSet( pSrcType, pOperandTypeValueSet );
	
	testfactor::RFactor srcMod = AddParameterValueSet( pSrcMod, pSrcModValueSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src_t0 * srcMod * srcType * swizzleFactor * maskFactor;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("SrcMod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"), _T("xy")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("OperandType"), OT_TEMP), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dtof::SetupTestCase()
{
	HRESULT hr = 0;
	TEST_RESULT trResult = RESULT_PASS;
	const tstring negString = (SMOD_NEG & m_SrcModifier) ? _T("-") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;

	trResult = CShader5xTest_DoubleArith::SetupTestCase();

	if( RESULT_PASS != trResult )
		return trResult;

	ID3D10Blob* pShaderBuf(NULL);

	assert((OT_TEMP == m_OperandType) || (OT_CONSTANT_BUFFER == m_OperandType));

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);

	if(OT_TEMP == m_OperandType)
	{
		asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	}

	asmHelper.SetupMovToTempFromLiteral( 1, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	asmHelper.SetupMovToRawOutputFromTemp( 0, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();

	tstring instructionCode = FormatString( _T("%s %s%s, %s%s%s"), 
											_T("dtof"),

											_T("r1"),
											maskString.c_str(),

											negString.c_str(),
											OT_TEMP == m_OperandType ? _T("r0") : _T("c0[0]"),
											swizString.c_str()
									);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
												GetEffectiveContext(),
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												instructionCode,
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		trResult = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		trResult = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);

	return trResult;
}

//////////////////////////////////////////////////////////////////////////
float CShader5xTest_DoubleArith::ConvertToFloat( Binary64 const& v )
{
	return static_cast<float>(v.m_AsDouble);
}

CShader5xTest_DoubleArith::Binary64Vec2 CShader5xTest_dtof::ReCalcSrcSwizzle(CShader5xTest_DoubleArith::Binary64Vec2 const& x)
{
	if(m_Swizzle.size() < 4)
	{
		return x;
	}
	CShader5xTest_DoubleArith::Binary64Vec2 retValue;
	//if swizzle[0] is z (ie zw) src component [1] was used
	switch(m_Swizzle[0])
	{
		case 'z'://zw
			retValue.m_X = x.m_Y;
			break;
		default://xy implied
			retValue.m_X = x.m_X;
			break;
	}
	//if swizzle[2] is x (ie xy) src component [0] was used
	switch(m_Swizzle[2])
	{
		case 'x'://xy
			retValue.m_Y = x.m_X;
			break;
		default://zw implied
			retValue.m_Y = x.m_Y;
			break;
	}
	return retValue;
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dtof::ExecuteTestCase()
{
	HRESULT hr = 0;
	TEST_RESULT trResult = RESULT_PASS;

	if(FAILED(hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, m_Src)))
	{
		WriteToLog(_T("CShader5xTest_dtof::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s."), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	if(m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
	{
		GetEffectiveContext()->Dispatch(1, 1, 1);
	}
	else
	{
		GetEffectiveContext()->Draw(1, 0);
	}

	GetEffectiveContext()->CopySubresourceRegion(m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	UINT result[4];//0=x 1=y 2=z 3=w
	if(FAILED(hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result)))
	{
		WriteToLog(_T("CShader5xTest_dtof::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s."), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	//Get "Modified" Value
	//Re Order based on swizzle (so we will be 0;1 based).  
	CShader5xTest_DoubleArith::Binary64Vec2 const srcValue = ReCalcSrcSwizzle(GetModifiedValue( m_Src, m_SrcModifier ));
	//Our source index should now map to the result index, based off the swizzle used
	struct FLOAT_UINT
	{
		union
		{
			float m_AsFloat;
			UINT  m_AsUint;
		};
	};

	for(UINT i = 0; i < m_Mask.size(); ++i)
	{
		UINT resIndex = 0;
		//Figure out result index from mask value
		switch(m_Mask[i])
		{
			case 'y':
				resIndex = 1;
				break;
			case 'z':
				resIndex = 2;
				break;
			case 'w':
				resIndex = 3;
				break;
			default://x is default, resIndex init to 0
				break;
		}
		
		FLOAT_UINT srcResult; 
		srcResult.m_AsFloat = CShader5xTest_DoubleArith::ConvertToFloat(srcValue.m_Vec[i]);
		if( srcResult.m_AsUint != result[resIndex])
		{
			// nan check, avoid -nan != nan
			if( CShader5xTest_DoubleArith::IsNaN(srcValue.m_Vec[i]) &&
				( (srcResult.m_AsUint & 0x7F800000 ) != 0x0) && 
				( ((srcResult.m_AsUint & 0x007FFFFF) ^ 0x007FFFFF) == 0x0 ) )
			{
				continue;
			}

			if(CShader5xTest_DoubleArith::IsDNorm(srcValue.m_Vec[i]) || 
				CShader5xTest_DoubleArith::IsZero(srcValue.m_Vec[i]))
			{
				if((result[resIndex] == 0x00000000) ||
					result[resIndex] == 0x800000000)
				{
					continue;
				}
			}
			WriteToLog( 1, "Expected (for component %d) 0x%0I64X 0x%I32X : result 0x%I32X.", i, srcValue.m_Vec[i], srcResult.m_AsUint, result[resIndex] );
			trResult = RESULT_FAIL;
		}
	}
	
	return trResult;
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_dtof::~CShader5xTest_dtof()
{
}

//////////////////////////////////////////////////////////////////////////
//  ddiv
//////////////////////////////////////////////////////////////////////////
CShader5xTest_DoubleArith::Binary64Vec2 const  Values_ddiv[] =      /* Must match classifications in DoubleDivSrcTable */  
{ 
	{ CShader5xTest_DoubleArith::s_NegINF64,    CShader5xTest_DoubleArith::s_NegINF64 },
	{ 0xDD0306CED8869A6Bull,                    0xDD0306CED8869A6Bull },                    // -Norm
	{ 0x800F4A8B6037B1F5ull,                    0x800F4A8B6037B1F5ull },                    // -DeNorm
	{ CShader5xTest_DoubleArith::s_NegOne64,    CShader5xTest_DoubleArith::s_NegOne64 },
	{ CShader5xTest_DoubleArith::s_NegZero64,   CShader5xTest_DoubleArith::s_NegZero64 },
	{ CShader5xTest_DoubleArith::s_Zero64,      CShader5xTest_DoubleArith::s_Zero64 },
	{ CShader5xTest_DoubleArith::s_One64,       CShader5xTest_DoubleArith::s_One64 },
	{ 0x000F4A8B6037B1F5ull,                    0x000F4A8B6037B1F5ull },                    // +DeNorm
	{ 0x5D0306CED8869A6Bull,                    0x5D0306CED8869A6Bull },                    // +Norm
	{ CShader5xTest_DoubleArith::s_PosINF64,    CShader5xTest_DoubleArith::s_PosINF64 },
	{ CShader5xTest_DoubleArith::s_Exponent64.m_AsULongLong | CShader5xTest_DoubleArith::s_OneULP64.m_AsULongLong,
	  CShader5xTest_DoubleArith::s_Exponent64.m_AsULongLong | CShader5xTest_DoubleArith::s_OneULP64.m_AsULongLong }, // NaN
	{ 0x7FF72172155D4FDBull,                    0x7FF72172155D4FDBull }                     // NaN
};
//////////////////////////////////////////////////////////////////////////
void CShader5xTest_ddiv::InitTestParameters()
{
	CTestCaseParameter<Binary64Vec2>* pSrc0 = AddParameter<Binary64Vec2>( _T("Src0"), &m_Src0 );
	
	testfactor::RFactor src0_t0 = AddParameterValueSet( pSrc0, new CTableValueSet<Binary64Vec2>( Values_ddiv, sizeof(Binary64Vec2), sizeof(Values_ddiv) / sizeof(Binary64Vec2) ) );
	testfactor::RFactor src0_t1 = AddParameterValueSet( pSrc0, new CTableValueSet<Binary64Vec2>( CShader5xTest_ddiv::s_DDivSrc0, sizeof(Binary64Vec2), sizeof(CShader5xTest_ddiv::s_DDivSrc0) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<Binary64Vec2>* pSrc1 = AddParameter<Binary64Vec2>( _T("Src1"), &m_Src1 );

	testfactor::RFactor src1_t0 = AddParameterValueSet( pSrc1, new CTableValueSet<Binary64Vec2>( Values_ddiv, sizeof(Binary64Vec2), sizeof(Values_ddiv) / sizeof(Binary64Vec2) ) );
	testfactor::RFactor src1_t1 = AddParameterValueSet( pSrc1, new CTableValueSet<Binary64Vec2>( CShader5xTest_ddiv::s_DDivSrc1, sizeof(Binary64Vec2), sizeof(CShader5xTest_ddiv::s_DDivSrc1) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<Binary64Vec2>* pResultParam = AddParameter<Binary64Vec2>( _T("Result"), &m_Result );
	{
		Binary64Vec2 def = { s_Zero64, s_Zero64 };
		SetParameterDefaultValue( pResultParam, def );
	}
	testfactor::RFactor res_t1 = AddParameterValueSet(pResultParam, new CTableValueSet<Binary64Vec2>( CShader5xTest_ddiv::s_DDivRes, sizeof(Binary64Vec2), sizeof(CShader5xTest_ddiv::s_DDivRes) / sizeof(Binary64Vec2) ) );

	testfactor::RFactor src0Class = AddParameter<DOUBLE_CLASSIFICATION>( _T("Src0Class"), &m_Src0Class, new CTableValueSet<DOUBLE_CLASSIFICATION>( DoubleDivSrcTable, sizeof(DOUBLE_CLASSIFICATION), sizeof(DoubleDivSrcTable) / sizeof(DOUBLE_CLASSIFICATION) ) );
	testfactor::RFactor src1Class = AddParameter<DOUBLE_CLASSIFICATION>( _T("Src1Class"), &m_Src1Class, new CTableValueSet<DOUBLE_CLASSIFICATION>( DoubleDivSrcTable, sizeof(DOUBLE_CLASSIFICATION), sizeof(DoubleDivSrcTable) / sizeof(DOUBLE_CLASSIFICATION) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xyzw") );
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("xyzw"));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("zw"));
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xyzw") );
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xyzw") );
	pSwizzleValueSet->AddValue( _T("zwxy") );
	pSwizzleValueSet->AddValue( _T("xyxy") ); // stands for .xy or .xyxy
	pSwizzleValueSet->AddValue( _T("zwzw") ); // stands for .zw or .zwzw 
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );

	CTestCaseParameter<bool>* pGenParam = AddPrivateParameter<bool>( _T("Generated"), &m_Generated );
	SetParameterDefaultValue( pGenParam, false );
	testfactor::RFactor genFactor = AddParameterValue<bool>( pGenParam, true );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrc0Mod = AddParameter<SRC_MOD_TYPE>( _T("Src0Mod"), &m_Src0Modifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrc0Mod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrc0ModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrc0ModValueSet->AddValue( SMOD_NONE );
	pSrc0ModValueSet->AddValue( SMOD_NEG );
	pSrc0ModValueSet->AddValue( SMOD_ABS );
	pSrc0ModValueSet->AddValue( SMOD_ABSNEG );
	testfactor::RFactor src0Mod = AddParameterValueSet( pSrc0Mod, pSrc0ModValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrc1Mod = AddParameter<SRC_MOD_TYPE>( _T("Src1Mod"), &m_Src1Modifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrc1Mod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrc1ModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrc1ModValueSet->AddValue( SMOD_NONE );
	pSrc1ModValueSet->AddValue( SMOD_NEG );
	pSrc1ModValueSet->AddValue( SMOD_ABS );
	pSrc1ModValueSet->AddValue( SMOD_ABSNEG );
	testfactor::RFactor src1Mod = AddParameterValueSet( pSrc1Mod, pSrc1ModValueSet );

	CTestCaseParameter<INST_MOD_TYPE>* pInstMod = AddParameter<INST_MOD_TYPE>( _T("InstructionModifier"), &m_InstModifier );
	SetParameterDefaultValue<INST_MOD_TYPE>( pInstMod, IMOD_NONE );
	CUserValueSet<INST_MOD_TYPE>* pInstModValueSet = new CUserValueSet<INST_MOD_TYPE>( IMOD_NONE, IMOD_SAT );
	testfactor::RFactor instMod = AddParameterValueSet( pInstMod, pInstModValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc1Type = AddParameter<OPERAND_TYPE>( _T("Operand1Type"), &m_OperandType[0] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc1Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand1TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src1Type = AddParameterValueSet( pSrc1Type, pOperand1TypeValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc2Type = AddParameter<OPERAND_TYPE>( _T("Operand2Type"), &m_OperandType[1] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc2Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand2TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src2Type = AddParameterValueSet( pSrc2Type, pOperand2TypeValueSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::RFactor tableFactor = (src0_t0 % src0Class) * (src1_t0 % src1Class);

	testfactor::XFACTOR pairwiseGroup1[] = 
	{
		{ instMod, 1, NULL, 0 },
		{ maskFactor, 1, NULL, 0 },
		{ src0Mod, 1, NULL, 0 },
		{ src1Mod, 1, NULL, 0 },
		{ src1Type, 1, NULL, 0 },
		{ src2Type, 1, NULL, 0 },
	};

	testfactor::RFactor exp2[] = 
	{
		tableFactor,
		NewCrossFactor( pairwiseGroup1, sizeof(pairwiseGroup1) / sizeof(testfactor::XFACTOR) )
	};

	testfactor::RFactor exp3[] = 
	{
		src0_t1 % src1_t1 %	res_t1,
		swizzleFactor * maskFactor * genFactor
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rFactor1 = ((src0_t0 % src0Class) * (src1_t0 % src1Class) * instMod * maskFactor);
	testfactor::RFactor rfFactor1WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor1 );

	testfactor::RFactor rFactor2 = NewSetFactor( exp2, sizeof(exp2) / sizeof(testfactor::RFactor) );
	testfactor::RFactor rfFactor2WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor2 );

	testfactor::RFactor rFactor3 = NewSetFactor( exp3, sizeof(exp3) / sizeof(testfactor::RFactor) );
	testfactor::RFactor rfFactor3WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor3 );

	SetRootTestFactor( rfFactor1WithShaderStages + rfFactor2WithShaderStages + rfFactor3WithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstructionModifier"), IMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_TEMP), WeightFactorLarge);	// Mix and match Temp vs CB for high pri to get extra coverage
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_CONSTANT_BUFFER), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_CONSTANT_BUFFER), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_TEMP), WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 5.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_ddiv::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	if ( g_TestApp.Is11_1DDIOrGreater() == false
	    || g_TestApp.IsFL11OrGreater() == false
	    || g_TestApp.DoublesAreSupported() == false )
		return RESULT_SKIP;

	tRes = CShader5xTest_DoubleArith::SetupTestCase();

	if( RESULT_PASS != tRes )
		return tRes;

	const tstring neg0String = (SMOD_NEG & m_Src0Modifier) ? _T("-") : _T("");
	const tstring neg1String = (SMOD_NEG & m_Src1Modifier) ? _T("-") : _T("");
	const tstring abs0String_pre = (SMOD_ABS & m_Src0Modifier) ? _T("abs(") : _T("");
	const tstring abs1String_pre = (SMOD_ABS & m_Src1Modifier) ? _T("abs(") : _T("");
	const tstring abs0String_suf = (SMOD_ABS & m_Src0Modifier) ? _T(")") : _T("");
	const tstring abs1String_suf = (SMOD_ABS & m_Src1Modifier) ? _T(")") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;
	const tstring modString = (IMOD_SAT & m_InstModifier) ? _T("_sat") : _T("");

	ID3D10Blob* pShaderBuf(NULL);

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);

	assert((OT_TEMP == m_OperandType[0]) || (OT_CONSTANT_BUFFER == m_OperandType[0]));
	assert((OT_TEMP == m_OperandType[1]) || (OT_CONSTANT_BUFFER == m_OperandType[1]));

	if(OT_TEMP == m_OperandType[0])
	{
		asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	}

	if(OT_TEMP == m_OperandType[1])
	{
		asmHelper.SetupMovToTempFromCB( 1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	}

	asmHelper.SetupMovToTempFromLiteral( 2, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	asmHelper.SetupMovToRawOutputFromTemp( 0, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();
	asmHelper.EnableDoubleExtensionsSupport();

	m_InstructionCode = FormatString( _T("%s%s %s%s, %s%s%s%s%s, %s%s%s%s%s"), 
  									  _T("ddiv"),
									  modString.c_str(),

									  _T("r2"),
									  maskString.c_str(),

									  neg0String.c_str(),
									  abs0String_pre.c_str(),
									  OT_TEMP == m_OperandType[0] ? _T("r0") : _T("c0[0]"),
									  swizString.c_str(),
									  abs0String_suf.c_str(),

									  neg1String.c_str(),
									  abs1String_pre.c_str(),
									  OT_TEMP == m_OperandType[1] ? _T("r1") : _T("c0[1]"),
									  swizString.c_str(),
									  abs1String_suf.c_str()
							        );

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
												GetEffectiveContext(),
												this,
												m_ShaderStage,
												g_TestApp.m_D3DOptions,
												m_InstructionCode.c_str(),
												&m_pRTBufferView,
												&m_pRTUABufferView,
												1,
												&pShaderBuf,
												&m_pVS,
												&m_pGS,
												&m_pPS,
												&m_pHS,
												&m_pDS,
												&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);
	return tRes;
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_ddiv::~CShader5xTest_ddiv()
{
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_ddiv::ExecuteTestCase()
{
	HRESULT hr;
	{
		struct CBufferLayout
		{
			Binary64Vec2 m_Src0;
			Binary64Vec2 m_Src1;
		};

		CBufferLayout cb = { m_Src0, m_Src1 };

		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, cb ) ) )
		{
			WriteToLog( _T("CShader5xTest_ddiv::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	if( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->Draw( 1, 0 );
	}

	GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	{
		Binary64Vec2 result;
		TEST_RESULT tr = RESULT_PASS;
		if( FAILED( hr = MapAndRead( GetImmediateContext(), m_pRTBufferCopy, result ) ) )
		{
			WriteToLog( _T("CShader5xTest_ddiv::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s."), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		if( m_Generated )
		{
			if( CompareBinary64Vec2( m_Result, result, _T("o0"), m_Mask, m_Swizzle, true ) == false )
			{
				tr = RESULT_FAIL;
			}
		}
		else
		{
			// Determine what the result should be.
			DOUBLE_CLASSIFICATION const src0Class = GetModifiedClass( m_Src0Class, m_Src0Modifier );
			DOUBLE_CLASSIFICATION const src1Class = GetModifiedClass( m_Src1Class, m_Src1Modifier );
			DOUBLE_CLASSIFICATION const expectedResultClass[3] = { GetModifiedClass( DoubleDivResultsTable[src0Class][src1Class][0], m_InstModifier),
                                                                   GetModifiedClass( DoubleDivResultsTable[src0Class][src1Class][1], m_InstModifier),
                                                                   GetModifiedClass( DoubleDivResultsTable[src0Class][src1Class][2], m_InstModifier) };
			DoubleClassInfo dci[3] = { DoubleClassInfoTable[expectedResultClass[0]], DoubleClassInfoTable[expectedResultClass[1]], DoubleClassInfoTable[expectedResultClass[2]] };

			if( dci[0].m_pValue && dci[1].m_pValue && dci[2].m_pValue )
			{
 				// Do a value compare.  Check alternative correct results, i.e. +Real / +Real may give +Real or +INF.
				Binary64Vec2 const gold[3] = { { *(dci[0].m_pValue), *(dci[0].m_pValue) },
                                               { *(dci[1].m_pValue), *(dci[1].m_pValue) },
                                               { *(dci[2].m_pValue), *(dci[2].m_pValue) } };

				if ( ( CompareBinary64Vec2( gold[0], result, _T("o0"), m_Mask, m_Swizzle, true, true ) == false )
				    && ( CompareBinary64Vec2( gold[1], result, _T("o0"), m_Mask, m_Swizzle, true, true ) == false )
				    && ( CompareBinary64Vec2( gold[2], result, _T("o0"), m_Mask, m_Swizzle, true, true ) == false ) )
				{
					tr = RESULT_FAIL;
				}
			}
			else if( dci[0].m_pCatFunc && dci[1].m_pCatFunc && dci[2].m_pCatFunc )
			{
   				// Use a function.  Check alternative correct results, i.e. +Real / +Real may give +Real or +INF.
				if ( ( CheckBinary64Vec2Result( result, _T("o0"), m_Mask, dci[0].m_pCatFunc, dci[0].m_Name, false, true ) == false )
				    && ( CheckBinary64Vec2Result( result, _T("o0"), m_Mask, dci[1].m_pCatFunc, dci[1].m_Name, false, true ) == false )
				    && ( CheckBinary64Vec2Result( result, _T("o0"), m_Mask, dci[2].m_pCatFunc, dci[2].m_Name, false, true ) == false ) )
				{
					tr = RESULT_FAIL;
				}
			}
			else if( dci[0].m_pSrcFunc )
			{
				// Use a src value as the gold result.
				// Do a value compare.
				Binary64Vec2 const gold = (*(dci[0].m_pSrcFunc))( GetModifiedValue( m_Src0, m_Src0Modifier), 
													                GetModifiedValue( m_Src1, m_Src1Modifier) );
				if( CompareBinary64Vec2( gold, result, _T("o0"), m_Mask, m_Swizzle ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else
			{
				WriteToLog( 1, _T("CShader5xTest_ddiv::ExecuteTestCase() No method for checking the result of %s"), dci[0].m_Name.c_str() );
				tr = RESULT_FAIL;
			}
		}

		if ( tr == RESULT_FAIL )
		{																		
			const tstring neg0String = (SMOD_NEG & m_Src0Modifier) ? _T("-") : _T("");
			const tstring neg1String = (SMOD_NEG & m_Src1Modifier) ? _T("-") : _T("");
			const tstring abs0String_pre = (SMOD_ABS & m_Src0Modifier) ? _T("abs(") : _T("");
			const tstring abs1String_pre = (SMOD_ABS & m_Src1Modifier) ? _T("abs(") : _T("");
			const tstring abs0String_suf = (SMOD_ABS & m_Src0Modifier) ? _T(")") : _T("");
			const tstring abs1String_suf = (SMOD_ABS & m_Src1Modifier) ? _T(")") : _T("");

			WriteToLog( 1, _T("DDIV instruction code:    %s"), m_InstructionCode.c_str() );
			WriteToLog( 1, _T("            .xy Input:    %s%s%+.18Lg%s / %s%s%+.18Lg%s"),
						neg0String.c_str(),	abs0String_pre.c_str(), m_Src0.m_X.m_AsDouble, abs0String_suf.c_str(),
						neg1String.c_str(),	abs1String_pre.c_str(), m_Src1.m_X.m_AsDouble, abs1String_suf.c_str() );
			WriteToLog( 1, _T("            .xy Input:    %s%s0x%016llX%s / %s%s0x%016llX%s"),
						neg0String.c_str(),	abs0String_pre.c_str(), m_Src0.m_X.m_AsULongLong, abs0String_suf.c_str(),
						neg1String.c_str(),	abs1String_pre.c_str(), m_Src1.m_X.m_AsULongLong, abs1String_suf.c_str() );
			WriteToLog( 1, _T("            .zw Input:    %s%s%+.18Lg%s / %s%s%+.18Lg%s"),
						neg0String.c_str(),	abs0String_pre.c_str(), m_Src0.m_Y.m_AsDouble, abs0String_suf.c_str(),
						neg1String.c_str(),	abs1String_pre.c_str(), m_Src1.m_Y.m_AsDouble, abs1String_suf.c_str() );
			WriteToLog( 1, _T("            .zw Input:    %s%s0x%016llX%s / %s%s0x%016llX%s"),
						neg0String.c_str(),	abs0String_pre.c_str(), m_Src0.m_Y.m_AsULongLong, abs0String_suf.c_str(),
						neg1String.c_str(),	abs1String_pre.c_str(), m_Src1.m_Y.m_AsULongLong, abs1String_suf.c_str() );
		}

		return tr;
	}
}

//////////////////////////////////////////////////////////////////////////
//  drcp
//////////////////////////////////////////////////////////////////////////
CShader5xTest_DoubleArith::Binary64Vec2 const  Values_drcp[] =      /* Must match classifications in DoubleRCPSrcTable */  
{ 
	{ CShader5xTest_DoubleArith::s_NegINF64,    CShader5xTest_DoubleArith::s_NegINF64 },
	{ 0xDD0306CED8869A6Bull,                    0xDD0306CED8869A6Bull },                    // -Norm
	{ 0x800F4A8B6037B1F5ull,                    0x800F4A8B6037B1F5ull },                    // -DeNorm
	{ CShader5xTest_DoubleArith::s_NegOne64,    CShader5xTest_DoubleArith::s_NegOne64 },
	{ CShader5xTest_DoubleArith::s_NegZero64,   CShader5xTest_DoubleArith::s_NegZero64 },
	{ CShader5xTest_DoubleArith::s_Zero64,      CShader5xTest_DoubleArith::s_Zero64 },
	{ CShader5xTest_DoubleArith::s_One64,       CShader5xTest_DoubleArith::s_One64 },
	{ 0x000F4A8B6037B1F5ull,                    0x000F4A8B6037B1F5ull },                    // +DeNorm
	{ 0x5D0306CED8869A6Bull,                    0x5D0306CED8869A6Bull },                    // +Norm
	{ CShader5xTest_DoubleArith::s_PosINF64,    CShader5xTest_DoubleArith::s_PosINF64 },
	{ CShader5xTest_DoubleArith::s_Exponent64.m_AsULongLong | CShader5xTest_DoubleArith::s_OneULP64.m_AsULongLong, 
	  CShader5xTest_DoubleArith::s_Exponent64.m_AsULongLong | CShader5xTest_DoubleArith::s_OneULP64.m_AsULongLong }, // NaN
	{ 0x7FF72172155D4FDBull,                    0x7FF72172155D4FDBull }                     // NaN
};

//////////////////////////////////////////////////////////////////////////
void CShader5xTest_drcp::InitTestParameters()
{
	CTestCaseParameter<Binary64Vec2>* pSrc0 = AddParameter<Binary64Vec2>( _T("Src0"), &m_Src0 );
	
	testfactor::RFactor src0_t0 = AddParameterValueSet( pSrc0, new CTableValueSet<Binary64Vec2>( Values_drcp, sizeof(Binary64Vec2), sizeof(Values_drcp) / sizeof(Binary64Vec2) ) );
	testfactor::RFactor src0_t1 = AddParameterValueSet( pSrc0, new CTableValueSet<Binary64Vec2>( CShader5xTest_drcp::s_DRCPSrc0, sizeof(Binary64Vec2), sizeof(CShader5xTest_drcp::s_DRCPSrc0) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<Binary64Vec2>* pResultParam = AddParameter<Binary64Vec2>( _T("Result"), &m_Result );
	{
		Binary64Vec2 def = { s_Zero64, s_Zero64 };
		SetParameterDefaultValue( pResultParam, def );
	}
	testfactor::RFactor res_t1 = AddParameterValueSet(pResultParam, new CTableValueSet<Binary64Vec2>( CShader5xTest_drcp::s_DRCPRes, sizeof(Binary64Vec2), sizeof(CShader5xTest_drcp::s_DRCPRes) / sizeof(Binary64Vec2) ) );

	testfactor::RFactor src0Class = AddParameter<DOUBLE_CLASSIFICATION>( _T("Src0Class"), &m_Src0Class, new CTableValueSet<DOUBLE_CLASSIFICATION>( DoubleSrcRCPTable, sizeof(DOUBLE_CLASSIFICATION), sizeof(DoubleSrcRCPTable) / sizeof(DOUBLE_CLASSIFICATION) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xyzw") );
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("xyzw"));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("zw"));
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xyzw") );
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xyzw") );
	pSwizzleValueSet->AddValue( _T("zwxy") );
	pSwizzleValueSet->AddValue( _T("xyxy") ); // stands for .xy or .xyxy
	pSwizzleValueSet->AddValue( _T("zwzw") ); // stands for .zw or .zwzw 
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );

	CTestCaseParameter<bool>* pGenParam = AddPrivateParameter<bool>( _T("Generated"), &m_Generated );
	SetParameterDefaultValue( pGenParam, false );
	testfactor::RFactor genFactor = AddParameterValue<bool>( pGenParam, true );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrc0Mod = AddParameter<SRC_MOD_TYPE>( _T("Src0Mod"), &m_Src0Modifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrc0Mod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrc0ModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrc0ModValueSet->AddValue( SMOD_NONE );
	pSrc0ModValueSet->AddValue( SMOD_NEG );
	pSrc0ModValueSet->AddValue( SMOD_ABS );
	pSrc0ModValueSet->AddValue( SMOD_ABSNEG );
	testfactor::RFactor src0Mod = AddParameterValueSet( pSrc0Mod, pSrc0ModValueSet );

	CTestCaseParameter<INST_MOD_TYPE>* pInstMod = AddParameter<INST_MOD_TYPE>( _T("InstructionModifier"), &m_InstModifier );
	SetParameterDefaultValue<INST_MOD_TYPE>( pInstMod, IMOD_NONE );
	CUserValueSet<INST_MOD_TYPE>* pInstModValueSet = new CUserValueSet<INST_MOD_TYPE>( IMOD_NONE, IMOD_SAT );
	testfactor::RFactor instMod = AddParameterValueSet( pInstMod, pInstModValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc1Type = AddParameter<OPERAND_TYPE>( _T("Operand1Type"), &m_OperandType[0] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc1Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand1TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src1Type = AddParameterValueSet( pSrc1Type, pOperand1TypeValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc2Type = AddParameter<OPERAND_TYPE>( _T("Operand2Type"), &m_OperandType[1] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc2Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand2TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src2Type = AddParameterValueSet( pSrc2Type, pOperand2TypeValueSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::RFactor tableFactor = src0_t0 % src0Class;

	testfactor::XFACTOR pairwiseGroup1[] = 
	{
		{ instMod, 1, NULL, 0 },
		{ maskFactor, 1, NULL, 0 },
		{ src0Mod, 1, NULL, 0 },
		{ src1Type, 1, NULL, 0 },
		{ src2Type, 1, NULL, 0 },
	};

	testfactor::RFactor exp2[] = 
	{
		tableFactor,
		NewCrossFactor( pairwiseGroup1, sizeof(pairwiseGroup1) / sizeof(testfactor::XFACTOR) )
	};

	testfactor::RFactor exp3[] = 
	{
		src0_t1 % res_t1,
		swizzleFactor * maskFactor * genFactor
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rFactor1 = ((src0_t0 % src0Class) * instMod * maskFactor);
	testfactor::RFactor rfFactor1WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor1 );

	testfactor::RFactor rFactor2 = NewSetFactor( exp2, sizeof(exp2) / sizeof(testfactor::RFactor) );
	testfactor::RFactor rfFactor2WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor2 );

	testfactor::RFactor rFactor3 = NewSetFactor( exp3, sizeof(exp3) / sizeof(testfactor::RFactor) );
	testfactor::RFactor rfFactor3WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor3 );

	SetRootTestFactor( rfFactor1WithShaderStages + rfFactor2WithShaderStages + rfFactor3WithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_ABSNEG), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstructionModifier"), IMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_TEMP), WeightFactorLarge);	// Mix and match Temp vs CB for high pri to get extra coverage
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_CONSTANT_BUFFER), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_CONSTANT_BUFFER), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_TEMP), WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 5.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_drcp::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	if ( g_TestApp.Is11_1DDIOrGreater() == false
	    || g_TestApp.IsFL11OrGreater() == false
	    || g_TestApp.DoublesAreSupported() == false )
		return RESULT_SKIP;

	tRes = CShader5xTest_DoubleArith::SetupTestCase();

	if( RESULT_PASS != tRes )
		return tRes;

	const tstring neg0String = (SMOD_NEG & m_Src0Modifier) ? _T("-") : _T("");
	const tstring abs0String_pre = (SMOD_ABS & m_Src0Modifier) ? _T("abs(") : _T("");
	const tstring abs0String_suf = (SMOD_ABS & m_Src0Modifier) ? _T(")") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;
	const tstring modString = (IMOD_SAT & m_InstModifier) ? _T("_sat") : _T("");

	ID3D10Blob* pShaderBuf(NULL);

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);

	assert((OT_TEMP == m_OperandType[0]) || (OT_CONSTANT_BUFFER == m_OperandType[0]));
	assert((OT_TEMP == m_OperandType[1]) || (OT_CONSTANT_BUFFER == m_OperandType[1]));

	if(OT_TEMP == m_OperandType[0])
	{
		asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	}

	if(OT_TEMP == m_OperandType[1])
	{
		asmHelper.SetupMovToTempFromCB( 1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	}

	asmHelper.SetupMovToTempFromLiteral( 2, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	asmHelper.SetupMovToRawOutputFromTemp( 0, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();
	asmHelper.EnableDoubleExtensionsSupport();

	tstring instructionCode = FormatString( _T("%s%s %s%s, %s%s%s%s%s"), 
						_T("drcp"),
						modString.c_str(),

						_T("r2"),
						maskString.c_str(),

						neg0String.c_str(),
						abs0String_pre.c_str(),
						OT_TEMP == m_OperandType[0] ? _T("r0") : _T("c0[0]"),
						swizString.c_str(),
						abs0String_suf.c_str()
	);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
						GetEffectiveContext(),
						this,
						m_ShaderStage,
						g_TestApp.m_D3DOptions,
						instructionCode,
						&m_pRTBufferView,
						&m_pRTUABufferView,
						1,
						&pShaderBuf,
						&m_pVS,
						&m_pGS,
						&m_pPS,
						&m_pHS,
						&m_pDS,
						&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);
	return tRes;
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_drcp::~CShader5xTest_drcp()
{
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_drcp::ExecuteTestCase()
{
	HRESULT hr;
	{
		struct CBufferLayout
		{
			Binary64Vec2 m_Src0;
		};

		CBufferLayout cb = { m_Src0 };

		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, cb ) ) )
		{
			WriteToLog( _T("CShader5xTest_drcp::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	if( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->Draw( 1, 0 );
	}

	GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	{
		Binary64Vec2 result;
		TEST_RESULT tr = RESULT_PASS;
		if( FAILED( hr = MapAndRead( GetImmediateContext(), m_pRTBufferCopy, result ) ) )
		{
			WriteToLog( _T("CShader5xTest_drcp::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s."), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		if( m_Generated )
		{
			if( CompareBinary64Vec2( m_Result, result, _T("o0"), m_Mask, m_Swizzle, false, false, true ) == false )
			{
				tr = RESULT_FAIL;
			}
		}
		else
		{
			// Determine what the result should be.
			DOUBLE_CLASSIFICATION const src0Class = GetModifiedClass( m_Src0Class, m_Src0Modifier );
			DOUBLE_CLASSIFICATION const expectedResultClass = GetModifiedClass( DoubleRCPTable[src0Class], m_InstModifier );
			DoubleClassInfo dci = DoubleClassInfoTable[expectedResultClass];

			if( dci.m_pValue )
			{
				// Do a value compare
				Binary64Vec2 const gold = { *(dci.m_pValue), *(dci.m_pValue) };
				if( CompareBinary64Vec2( gold, result, _T("o0"), m_Mask, m_Swizzle ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else if( dci.m_pCatFunc )
			{
				// Use a function to check the result.
				if( CheckBinary64Vec2Result( result, _T("o0"), m_Mask, dci.m_pCatFunc, dci.m_Name ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else if( dci.m_pSrcFunc )
			{
				// Use a src value as the gold result.
				// Do a value compare.
				Binary64Vec2 const gold = (*(dci.m_pSrcFunc))( GetModifiedValue( m_Src0, m_Src0Modifier), GetModifiedValue( m_Src0, m_Src0Modifier) ); 
				if( CompareBinary64Vec2( gold, result, _T("o0"), m_Mask, m_Swizzle ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else
			{
				WriteToLog( 1, _T("CShader5xTest_drcp::ExecuteTestCase() No method for checking the result of %s"), dci.m_Name.c_str() );
				tr = RESULT_FAIL;
			}
		}
		return tr;
	}
}

//////////////////////////////////////////////////////////////////////////
//  dfma
//////////////////////////////////////////////////////////////////////////
CShader5xTest_DoubleArith::Binary64Vec2 const Values_dfma[] =      /* Must match classifications in DoubleSrcFMATable */   
{ 
	{ CShader5xTest_DoubleArith::s_NegINF64,    CShader5xTest_DoubleArith::s_NegINF64 },
	{ 0x800F06CED8869A6Bull,                    0x800806CED8869A6Bull },                    // -DeNorm
	{ 0xDD0306CED8869A6Bull,                    0xDAA34A8B6037B1F5ull },                    // -Norm
	{ CShader5xTest_DoubleArith::s_NegOne64,    CShader5xTest_DoubleArith::s_NegOne64 },
	{ CShader5xTest_DoubleArith::s_NegZero64,   CShader5xTest_DoubleArith::s_NegZero64 },
	{ CShader5xTest_DoubleArith::s_Zero64,      CShader5xTest_DoubleArith::s_Zero64 },
	{ CShader5xTest_DoubleArith::s_One64,       CShader5xTest_DoubleArith::s_One64 },
	{ 0x01064A8B6037B1F5ull,                    0x011306CED8869A6Bull },                    // +Norm
	{ 0x000F06CEA8869A6Bull,                    0x000806CEA8869A6Bull },                    // +DeNorm
	{ CShader5xTest_DoubleArith::s_PosINF64,    CShader5xTest_DoubleArith::s_PosINF64 },
	{ CShader5xTest_DoubleArith::s_Exponent64.m_AsULongLong | CShader5xTest_DoubleArith::s_OneULP64.m_AsULongLong,
	                                            0x7FF72172155D4FDBull },                    // NaN, NaN
};

//////////////////////////////////////////////////////////////////////////
void CShader5xTest_dfma::InitTestParameters()
{
	CTestCaseParameter<Binary64Vec2>* pSrc0 = AddParameter<Binary64Vec2>( _T("Src0"), &m_Src0 );
	
	testfactor::RFactor src0_t0 = AddParameterValueSet( pSrc0, new CTableValueSet<Binary64Vec2>( Values_dfma, sizeof(Binary64Vec2), sizeof(Values_dfma) / sizeof(Binary64Vec2) ) );
	testfactor::RFactor src0_t1 = AddParameterValueSet( pSrc0, new CTableValueSet<Binary64Vec2>( CShader5xTest_dfma::s_DFMASrc0, sizeof(Binary64Vec2), sizeof(CShader5xTest_dfma::s_DFMASrc0) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<Binary64Vec2>* pSrc1 = AddParameter<Binary64Vec2>( _T("Src1"), &m_Src1 );

	testfactor::RFactor src1_t0 = AddParameterValueSet( pSrc1, new CTableValueSet<Binary64Vec2>( Values_dfma, sizeof(Binary64Vec2), sizeof(Values_dfma) / sizeof(Binary64Vec2) ) );
	testfactor::RFactor src1_t1 = AddParameterValueSet( pSrc1, new CTableValueSet<Binary64Vec2>( CShader5xTest_dfma::s_DFMASrc1, sizeof(Binary64Vec2), sizeof(CShader5xTest_dfma::s_DFMASrc1) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<Binary64Vec2>* pSrc2 = AddParameter<Binary64Vec2>( _T("Src2"), &m_Src2 );

	testfactor::RFactor src2_t0 = AddParameterValueSet( pSrc2, new CTableValueSet<Binary64Vec2>( Values_dfma, sizeof(Binary64Vec2), sizeof(Values_dfma) / sizeof(Binary64Vec2) ) );
	testfactor::RFactor src2_t1 = AddParameterValueSet( pSrc2, new CTableValueSet<Binary64Vec2>( CShader5xTest_dfma::s_DFMASrc2, sizeof(Binary64Vec2), sizeof(CShader5xTest_dfma::s_DFMASrc2) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<Binary64Vec2>* pResultParam = AddParameter<Binary64Vec2>( _T("Result"), &m_Result );
	{
		Binary64Vec2 def = { s_Zero64, s_Zero64 };
		SetParameterDefaultValue( pResultParam, def );
	}
	testfactor::RFactor res_t1 = AddParameterValueSet(pResultParam, new CTableValueSet<Binary64Vec2>( CShader5xTest_dfma::s_DFMARes, sizeof(Binary64Vec2), sizeof(CShader5xTest_dfma::s_DFMARes) / sizeof(Binary64Vec2) ) );

	testfactor::RFactor src0Class = AddParameter<DOUBLE_CLASSIFICATION>( _T("Src0Class"), &m_Src0Class, new CTableValueSet<DOUBLE_CLASSIFICATION>( DoubleSrcFMATable, sizeof(DOUBLE_CLASSIFICATION), sizeof(DoubleSrcFMATable) / sizeof(DOUBLE_CLASSIFICATION) ) );
	testfactor::RFactor src1Class = AddParameter<DOUBLE_CLASSIFICATION>( _T("Src1Class"), &m_Src1Class, new CTableValueSet<DOUBLE_CLASSIFICATION>( DoubleSrcFMATable, sizeof(DOUBLE_CLASSIFICATION), sizeof(DoubleSrcFMATable) / sizeof(DOUBLE_CLASSIFICATION) ) );
	testfactor::RFactor src2Class = AddParameter<DOUBLE_CLASSIFICATION>( _T("Src2Class"), &m_Src2Class, new CTableValueSet<DOUBLE_CLASSIFICATION>( DoubleSrcFMATable, sizeof(DOUBLE_CLASSIFICATION), sizeof(DoubleSrcFMATable) / sizeof(DOUBLE_CLASSIFICATION) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xyzw") );
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("xyzw"));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("zw")); 
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xyzw") );
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xyzw") );
	pSwizzleValueSet->AddValue( _T("zwxy") );
	pSwizzleValueSet->AddValue( _T("xyxy") ); // stands for .xy or .xyxy
	pSwizzleValueSet->AddValue( _T("zwzw") ); // stands for .zw or .zwzw 
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );
                                         
	CTestCaseParameter<bool>* pGenParam = AddPrivateParameter<bool>( _T("Generated"), &m_Generated );
	SetParameterDefaultValue( pGenParam, false );
	testfactor::RFactor genFactor = AddParameterValue<bool>( pGenParam, true );

	// Specify SMOD_NONE only as DoubleFMATable.h does not handle modifier combinations.
	CTestCaseParameter<SRC_MOD_TYPE>* pSrc0Mod = AddParameter<SRC_MOD_TYPE>( _T("Src0Mod"), &m_Src0Modifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrc0Mod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrc0ModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrc0ModValueSet->AddValue( SMOD_NONE );
	testfactor::RFactor src0Mod = AddParameterValueSet( pSrc0Mod, pSrc0ModValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrc1Mod = AddParameter<SRC_MOD_TYPE>( _T("Src1Mod"), &m_Src1Modifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrc1Mod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrc1ModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrc1ModValueSet->AddValue( SMOD_NONE );
	testfactor::RFactor src1Mod = AddParameterValueSet( pSrc1Mod, pSrc1ModValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrc2Mod = AddParameter<SRC_MOD_TYPE>( _T("Src2Mod"), &m_Src2Modifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrc2Mod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrc2ModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrc2ModValueSet->AddValue( SMOD_NONE );
	testfactor::RFactor src2Mod = AddParameterValueSet( pSrc2Mod, pSrc2ModValueSet );

	CTestCaseParameter<INST_MOD_TYPE>* pInstMod = AddParameter<INST_MOD_TYPE>( _T("InstructionModifier"), &m_InstModifier );
	SetParameterDefaultValue<INST_MOD_TYPE>( pInstMod, IMOD_NONE );
	CUserValueSet<INST_MOD_TYPE>* pInstModValueSet = new CUserValueSet<INST_MOD_TYPE>( IMOD_NONE, IMOD_SAT );
	testfactor::RFactor instMod = AddParameterValueSet( pInstMod, pInstModValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc1Type = AddParameter<OPERAND_TYPE>( _T("Operand1Type"), &m_OperandType[0] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc1Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand1TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src1Type = AddParameterValueSet( pSrc1Type, pOperand1TypeValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc2Type = AddParameter<OPERAND_TYPE>( _T("Operand2Type"), &m_OperandType[1] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc2Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand2TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src2Type = AddParameterValueSet( pSrc2Type, pOperand2TypeValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrc3Type = AddParameter<OPERAND_TYPE>( _T("Operand3Type"), &m_OperandType[2] );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrc3Type, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperand3TypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor src3Type = AddParameterValueSet( pSrc3Type, pOperand3TypeValueSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	testfactor::RFactor tableFactor = (src0_t0 % src0Class) * (src1_t0 % src1Class) * (src2_t0 % src2Class);

	SetRootTestFactor( tableFactor );
	testfactor::XFACTOR pairwiseGroup1[] = 
	{
		{ instMod, 1, NULL, 0 },
		{ maskFactor, 1, NULL, 0 },
		{ src0Mod, 1, NULL, 0 },
		{ src1Mod, 1, NULL, 0 },
		{ src2Mod, 1, NULL, 0 },
		{ src1Type, 1, NULL, 0 },
		{ src2Type, 1, NULL, 0 },
		{ src3Type, 1, NULL, 0 },
	};

	testfactor::RFactor exp2[] = 
	{
		tableFactor,
		NewCrossFactor( pairwiseGroup1, sizeof(pairwiseGroup1) / sizeof(testfactor::XFACTOR) )
	};

	testfactor::RFactor exp3[] = 
	{
		src0_t1 % src1_t1 % src2_t1 % res_t1,
		swizzleFactor * maskFactor * genFactor
	};

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rFactor1 = ((src0_t0 % src0Class) * (src1_t0 % src1Class) * (src2_t0 % src2Class) * instMod * maskFactor);
	testfactor::RFactor rfFactor1WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor1 );

	testfactor::RFactor rFactor2 = NewSetFactor( exp2, sizeof(exp2) / sizeof(testfactor::RFactor) );
	testfactor::RFactor rfFactor2WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor2 );

	testfactor::RFactor rFactor3 = NewSetFactor( exp3, sizeof(exp3) / sizeof(testfactor::RFactor) );
	testfactor::RFactor rfFactor3WithShaderStages = GetTestFactorWithDefaultStageCoverage( rFactor3 );

	SetRootTestFactor( rfFactor1WithShaderStages + rfFactor2WithShaderStages + rfFactor3WithShaderStages );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src0Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src1Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("Src2Mod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<INST_MOD_TYPE>(_T("InstructionModifier"), IMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_TEMP), WeightFactorLarge);	// Mix and match Temp vs CB for high pri to get extra coverage
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand1Type"), OT_CONSTANT_BUFFER), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_CONSTANT_BUFFER), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand2Type"), OT_TEMP), WeightFactorSmall);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand3Type"), OT_CONSTANT_BUFFER), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("Operand3Type"), OT_TEMP), WeightFactorSmall);

	DeclarePriorityLevelByPriorityValue( 1, 5.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dfma::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	if ( g_TestApp.Is11_1DDIOrGreater() == false
	    || g_TestApp.IsFL11OrGreater() == false
	    || g_TestApp.DoublesAreSupported() == false )
		return RESULT_SKIP;

	tRes = CShader5xTest_DoubleArith::SetupTestCase();

	if( RESULT_PASS != tRes )
		return tRes;

	const tstring neg0String = (SMOD_NEG & m_Src0Modifier) ? _T("-") : _T("");
	const tstring neg1String = (SMOD_NEG & m_Src1Modifier) ? _T("-") : _T("");
	const tstring neg2String = (SMOD_NEG & m_Src2Modifier) ? _T("-") : _T("");
	const tstring abs0String_pre = (SMOD_ABS & m_Src0Modifier) ? _T("abs(") : _T("");
	const tstring abs1String_pre = (SMOD_ABS & m_Src1Modifier) ? _T("abs(") : _T("");
	const tstring abs2String_pre = (SMOD_ABS & m_Src2Modifier) ? _T("abs(") : _T("");
	const tstring abs0String_suf = (SMOD_ABS & m_Src0Modifier) ? _T(")") : _T("");
	const tstring abs1String_suf = (SMOD_ABS & m_Src1Modifier) ? _T(")") : _T("");
	const tstring abs2String_suf = (SMOD_ABS & m_Src2Modifier) ? _T(")") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;
	const tstring modString = (IMOD_SAT & m_InstModifier) ? _T("_sat") : _T("");

	ID3D10Blob* pShaderBuf(NULL);

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);

	assert((OT_TEMP == m_OperandType[0]) || (OT_CONSTANT_BUFFER == m_OperandType[0]));
	assert((OT_TEMP == m_OperandType[1]) || (OT_CONSTANT_BUFFER == m_OperandType[1]));
	assert((OT_TEMP == m_OperandType[2]) || (OT_CONSTANT_BUFFER == m_OperandType[2]));

	if(OT_TEMP == m_OperandType[0])
	{
		asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	}

	if(OT_TEMP == m_OperandType[1])
	{
		asmHelper.SetupMovToTempFromCB( 1, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	}

	if(OT_TEMP == m_OperandType[2])
	{
		asmHelper.SetupMovToTempFromCB( 2, AssemblyHelper::xyzw, 2, AssemblyHelper::swizXYZW );
	}

	asmHelper.SetupMovToTempFromLiteral( 3, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	asmHelper.SetupMovToRawOutputFromTemp( 0, AssemblyHelper::xyzw, 3, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();
	asmHelper.EnableDoubleExtensionsSupport();

	m_InstructionCode = FormatString( _T("%s%s %s%s, %s%s%s%s%s, %s%s%s%s%s, %s%s%s%s%s"), 
  					_T("dfma"),
					modString.c_str(),

					_T("r3"),
					maskString.c_str(),

					neg0String.c_str(),
					abs0String_pre.c_str(),
					OT_TEMP == m_OperandType[0] ? _T("r0") : _T("c0[0]"),
					swizString.c_str(),
					abs0String_suf.c_str(),

					neg1String.c_str(),
					abs1String_pre.c_str(),
					OT_TEMP == m_OperandType[1] ? _T("r1") : _T("c0[1]"),
					swizString.c_str(),
					abs1String_suf.c_str(),

					neg2String.c_str(),
					abs2String_pre.c_str(),
					OT_TEMP == m_OperandType[2] ? _T("r2") : _T("c0[2]"),
					swizString.c_str(),
					abs2String_suf.c_str()
					);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
						GetEffectiveContext(),
						this,
						m_ShaderStage,
						g_TestApp.m_D3DOptions,
						m_InstructionCode.c_str(),
						&m_pRTBufferView,
						&m_pRTUABufferView,
						1,
						&pShaderBuf,
						&m_pVS,
						&m_pGS,
						&m_pPS,
						&m_pHS,
						&m_pDS,
						&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);
	return tRes;
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_dfma::~CShader5xTest_dfma()
{
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dfma::ExecuteTestCase()
{
	HRESULT hr;
	{
		struct CBufferLayout
		{
			Binary64Vec2 m_Src0;
			Binary64Vec2 m_Src1;
			Binary64Vec2 m_Src2;
		};

		CBufferLayout cb = { m_Src0, m_Src1, m_Src2 };

		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, cb ) ) )
		{
			WriteToLog( _T("CShader5xTest_dfma::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	if( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->Draw( 1, 0 );
	}

	GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	{
		Binary64Vec2 result;
		TEST_RESULT tr = RESULT_PASS;
		if( FAILED( hr = MapAndRead( GetImmediateContext(), m_pRTBufferCopy, result ) ) )
		{
			WriteToLog( _T("CShader5xTest_dfma::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s."), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		if( m_Generated )
		{
			if( CompareBinary64Vec2( m_Result, result, _T("o0"), m_Mask, m_Swizzle ) == false )
			{
				tr = RESULT_FAIL;
			}
		}
		else
		{
			// Determine what the result should be.
			DOUBLE_CLASSIFICATION const src0Class = GetModifiedClass( m_Src0Class, m_Src0Modifier );
			DOUBLE_CLASSIFICATION const src1Class = GetModifiedClass( m_Src1Class, m_Src1Modifier );
			DOUBLE_CLASSIFICATION const src2Class = GetModifiedClass( m_Src2Class, m_Src2Modifier );
			DOUBLE_CLASSIFICATION const nonModResultClass = DoubleFMATable[DoubleFMACrossIndex[src0Class]][DoubleFMACrossIndex[src1Class]][DoubleFMACrossIndex[src2Class]];
			DOUBLE_CLASSIFICATION const expectedResultClass = GetModifiedClass( nonModResultClass, m_InstModifier );
			DoubleClassInfo dci = DoubleClassInfoTable[expectedResultClass];

			if( dci.m_pValue )
			{
				// Do a value compare
				Binary64Vec2 const gold = { *(dci.m_pValue), *(dci.m_pValue) };
				if( CompareBinary64Vec2( gold, result, _T("o0"), m_Mask, m_Swizzle, false, false ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else if( dci.m_pCatFunc )
			{
				// Use a function to check the result.
				if( CheckBinary64Vec2Result( result, _T("o0"), m_Mask, dci.m_pCatFunc, dci.m_Name, true ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else if( dci.m_pSrcFunc )
			{
				// Use a src value as the gold result.
				// Do a value compare.
				Binary64Vec2 const gold = (*(dci.m_pSrcFunc))(	GetModifiedValue( m_Src0, m_Src0Modifier), 
																GetModifiedValue( m_Src1, m_Src1Modifier) );
				if( CompareBinary64Vec2( gold, result, _T("o0"), m_Mask, m_Swizzle ) == false )
				{
					tr = RESULT_FAIL;
				}
			}
			else
			{
				WriteToLog( 1, _T("CShader5xTest_dfma::ExecuteTestCase() No method for checking the result of %s"), dci.m_Name.c_str() );
				tr = RESULT_FAIL;
			}
		}

		if ( tr == RESULT_FAIL )
		{																		
			const tstring neg0String = (SMOD_NEG & m_Src0Modifier) ? _T("-") : _T("");
			const tstring neg1String = (SMOD_NEG & m_Src1Modifier) ? _T("-") : _T("");
			const tstring neg2String = (SMOD_NEG & m_Src2Modifier) ? _T("-") : _T("");
			const tstring abs0String_pre = (SMOD_ABS & m_Src0Modifier) ? _T("abs(") : _T("");
			const tstring abs1String_pre = (SMOD_ABS & m_Src1Modifier) ? _T("abs(") : _T("");
			const tstring abs2String_pre = (SMOD_ABS & m_Src2Modifier) ? _T("abs(") : _T("");
			const tstring abs0String_suf = (SMOD_ABS & m_Src0Modifier) ? _T(")") : _T("");
			const tstring abs1String_suf = (SMOD_ABS & m_Src1Modifier) ? _T(")") : _T("");
			const tstring abs2String_suf = (SMOD_ABS & m_Src2Modifier) ? _T(")") : _T("");

			WriteToLog( 1, _T("DFMA instruction code:    %s"), m_InstructionCode.c_str() );
			WriteToLog( 1, _T("            .xy Input:    %s%s%+.18Lg%s * %s%s%+.18Lg%s + %s%s%+.18Lg%s"),
						neg0String.c_str(),	abs0String_pre.c_str(), m_Src0.m_X.m_AsDouble, abs0String_suf.c_str(),
						neg1String.c_str(),	abs1String_pre.c_str(), m_Src1.m_X.m_AsDouble, abs1String_suf.c_str(),
						neg2String.c_str(),	abs2String_pre.c_str(), m_Src2.m_X.m_AsDouble, abs2String_suf.c_str() );
			WriteToLog( 1, _T("            .xy Input:    %s%s0x%016llX%s * %s%s0x%016llX%s + %s%s0x%016llX%s"),
						neg0String.c_str(),	abs0String_pre.c_str(), m_Src0.m_X.m_AsULongLong, abs0String_suf.c_str(),
						neg1String.c_str(),	abs1String_pre.c_str(), m_Src1.m_X.m_AsULongLong, abs1String_suf.c_str(),
						neg2String.c_str(),	abs2String_pre.c_str(), m_Src2.m_X.m_AsULongLong, abs2String_suf.c_str() );
			WriteToLog( 1, _T("            .zw Input:    %s%s%+.18Lg%s * %s%s%+.18Lg%s + %s%s%+.18Lg%s"),
						neg0String.c_str(),	abs0String_pre.c_str(), m_Src0.m_Y.m_AsDouble, abs0String_suf.c_str(),
						neg1String.c_str(),	abs1String_pre.c_str(), m_Src1.m_Y.m_AsDouble, abs1String_suf.c_str(),
						neg2String.c_str(),	abs2String_pre.c_str(), m_Src2.m_Y.m_AsDouble, abs2String_suf.c_str() );
			WriteToLog( 1, _T("            .zw Input:    %s%s0x%016llX%s * %s%s0x%016llX%s + %s%s0x%016llX%s"),
						neg0String.c_str(),	abs0String_pre.c_str(), m_Src0.m_Y.m_AsULongLong, abs0String_suf.c_str(),
						neg1String.c_str(),	abs1String_pre.c_str(), m_Src1.m_Y.m_AsULongLong, abs1String_suf.c_str(),
						neg2String.c_str(),	abs2String_pre.c_str(), m_Src2.m_Y.m_AsULongLong, abs2String_suf.c_str() );
		}

		return tr;
	}
}

//////////////////////////////////////////////////////////////////////////
//  dtoi
//////////////////////////////////////////////////////////////////////////
const CShader5xTest_DoubleArith::Binary64Vec2 dtoiInputTable[] = 
{
	/*{*/ { 0xFFF0000000000000ull, 0xFFF0000000000000ull }, //DC_INFn },
	/*{*/ { 0xC00921FB54442D18ull, 0x800F4A8B6037B1F5ull }, //DC_REALn },// negative real norm , negative real denorm
	/*{*/ { 0xBFF0000000000000ull, 0xBFF0000000000000ull }, //DC_ONEn },
	/*{*/ { 0x8000000000000000ull, 0x8000000000000000ull }, //DC_ZEROn },
	/*{*/ { 0x0000000000000000ull, 0x0000000000000000ull }, //DC_ZEROp },
	/*{*/ { 0x3FF0000000000000ull, 0x3FF0000000000000ull }, //DC_ONEp },
	/*{*/ { 0x000F4A8B6037B1F5ull, 0x000F4A8B6037B1F5ull }, //DC_REALp },// real denorm, real norm 
	/*{*/ { 0x7FF0000000000000ull, 0x7FF0000000000000ull }, //DC_INFp },
	/*{*/ { 0x7FFFFFFFFFFFFFFFull, 0x7FFFFFFFFFFFFFFFull }//, DC_NAN }// nan
};

//////////////////////////////////////////////////////////////////////////
void CShader5xTest_dtoi::InitTestParameters()
{
	CTestCaseParameter<Binary64Vec2>* pSrc = AddParameter<Binary64Vec2>( _T("Src"), &m_Src );
	
	
	testfactor::RFactor src_t0 = AddParameterValueSet( pSrc, new CTableValueSet<Binary64Vec2>( dtoiInputTable, sizeof(Binary64Vec2), sizeof(dtoiInputTable) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xy") );
	
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("x"));
	pMaskValueSet->AddValue(_T("y"));
	pMaskValueSet->AddValue(_T("z"));
	pMaskValueSet->AddValue(_T("w"));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("xz"));
	pMaskValueSet->AddValue(_T("xw"));
	pMaskValueSet->AddValue(_T("yz"));
	pMaskValueSet->AddValue(_T("yw"));
	pMaskValueSet->AddValue(_T("zw"));
	
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xyzw") );
	
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xyzw") );
	pSwizzleValueSet->AddValue( _T("xyxy") );
	pSwizzleValueSet->AddValue( _T("zwxy") ); 
	pSwizzleValueSet->AddValue( _T("zwzw") );  
	
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrcMod = AddParameter<SRC_MOD_TYPE>( _T("SrcMod"), &m_SrcModifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrcMod, SMOD_NONE );
	
	CUserValueSet<SRC_MOD_TYPE> *pSrcModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrcModValueSet->AddValue( SMOD_NONE );
	pSrcModValueSet->AddValue( SMOD_NEG );

	CTestCaseParameter<OPERAND_TYPE>* pSrcType = AddParameter<OPERAND_TYPE>( _T("OperandType"), &m_OperandType );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrcType, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperandTypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor srcType = AddParameterValueSet( pSrcType, pOperandTypeValueSet );
	
	testfactor::RFactor srcMod = AddParameterValueSet( pSrcMod, pSrcModValueSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src_t0 * srcMod * srcType * swizzleFactor * maskFactor;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("SrcMod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"), _T("xy")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("OperandType"), OT_TEMP), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dtoi::SetupTestCase()
{
	HRESULT hr = 0;
	TEST_RESULT trResult = RESULT_PASS;
	const tstring negString = (SMOD_NEG & m_SrcModifier) ? _T("-") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;

	if ( g_TestApp.Is11_1DDIOrGreater() == false
	    || g_TestApp.IsFL11OrGreater() == false
	    || g_TestApp.DoublesAreSupported() == false )
		return RESULT_SKIP;

	trResult = CShader5xTest_DoubleArith::SetupTestCase();

	if( RESULT_PASS != trResult )
		return trResult;

	ID3D10Blob* pShaderBuf(NULL);

	assert((OT_TEMP == m_OperandType) || (OT_CONSTANT_BUFFER == m_OperandType));

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);

	if(OT_TEMP == m_OperandType)
	{
		asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	}

	asmHelper.SetupMovToTempFromLiteral( 1, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	asmHelper.SetupMovToRawOutputFromTemp( 0, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();
	asmHelper.EnableDoubleExtensionsSupport();

	tstring instructionCode = FormatString( _T("%s %s%s, %s%s%s"), 
						_T("dtoi"),

						_T("r1"),
						maskString.c_str(),

						negString.c_str(),
						OT_TEMP == m_OperandType ? _T("r0") : _T("c0[0]"),
						swizString.c_str()
						);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
						GetEffectiveContext(),
						this,
						m_ShaderStage,
						g_TestApp.m_D3DOptions,
						instructionCode,
						&m_pRTBufferView,
						&m_pRTUABufferView,
						1,
						&pShaderBuf,
						&m_pVS,
						&m_pGS,
						&m_pPS,
						&m_pHS,
						&m_pDS,
						&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		trResult = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		trResult = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);

	return trResult;
}

CShader5xTest_DoubleArith::Binary64Vec2 CShader5xTest_dtoi::ReCalcSrcSwizzle(CShader5xTest_DoubleArith::Binary64Vec2 const& x)
{
	if(m_Swizzle.size() < 4)
	{
		return x;
	}
	CShader5xTest_DoubleArith::Binary64Vec2 retValue;
	//if swizzle[0] is z (ie zw) src component [1] was used
	switch(m_Swizzle[0])
	{
		case 'z'://zw
			retValue.m_X = x.m_Y;
			break;
		default://xy implied
			retValue.m_X = x.m_X;
			break;
	}
	//if swizzle[2] is x (ie xy) src component [0] was used
	switch(m_Swizzle[2])
	{
		case 'x'://xy
			retValue.m_Y = x.m_X;
			break;
		default://zw implied
			retValue.m_Y = x.m_Y;
			break;
	}
	return retValue;
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dtoi::ExecuteTestCase()
{
	HRESULT hr = 0;
	TEST_RESULT trResult = RESULT_PASS;

	if(FAILED(hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, m_Src)))
	{
		WriteToLog(_T("CShader5xTest_dtoi::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s."), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	if(m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
	{
		GetEffectiveContext()->Dispatch(1, 1, 1);
	}
	else
	{
		GetEffectiveContext()->Draw(1, 0);
	}

	GetEffectiveContext()->CopySubresourceRegion(m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	INT result[4];//0=x 1=y 2=z 3=w
	if(FAILED(hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result)))
	{
		WriteToLog(_T("CShader5xTest_dtoi::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s."), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	// Get "Modified" Value
	// Re-order based on swizzle (so we will be 0:1 based).  
	CShader5xTest_DoubleArith::Binary64Vec2 const srcValue = ReCalcSrcSwizzle(GetModifiedValue( m_Src, m_SrcModifier ));

	// Our source index should now map to the result index, based off the swizzle used
	for(UINT i = 0; i < m_Mask.size(); ++i)
	{
		UINT resIndex = 0;
		//Figure out result index from mask value
		switch(m_Mask[i])
		{
			case 'y':
				resIndex = 1;
				break;
			case 'z':
				resIndex = 2;
				break;
			case 'w':
				resIndex = 3;
				break;
			default://x is default, resIndex init to 0
				break;
		}
		
        // Calculate the expected result.
		int  srcResult = 0;

        if ( IsNaN( srcValue.m_Vec[i] )
            || IsZero( srcValue.m_Vec[i] )
            || IsDNorm( srcValue.m_Vec[i] ) )
        {
            srcResult = 0;
        }
        else
        if ( IsNegINF( srcValue.m_Vec[i] ) )
        {
            srcResult = INT_MIN;
        }
        else
        if ( IsPosINF( srcValue.m_Vec[i] ) )
        {
            srcResult = INT_MAX;
        }
        else
        {
    		srcResult = (int)(double)srcValue.m_Vec[i].m_AsDouble;
        }

		if( srcResult != result[resIndex])
		{
			WriteToLog( 1, "Expected (for component %d) 0x%0I64X 0x%I32X : result 0x%I32X.", i, srcValue.m_Vec[i], srcResult, result[resIndex] );
			trResult = RESULT_FAIL;
		}
	}
	
	return trResult;
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_dtoi::~CShader5xTest_dtoi()
{
}

//////////////////////////////////////////////////////////////////////////
//  dtou
//////////////////////////////////////////////////////////////////////////
const CShader5xTest_DoubleArith::Binary64Vec2 dtouInputTable[] = 
{
	/*{*/ { 0xFFF0000000000000ull, 0xFFF0000000000000ull }, //DC_INFn },
	/*{*/ { 0xC00921FB54442D18ull, 0x800F4A8B6037B1F5ull }, //DC_REALn },// negative real norm , negative real denorm
	/*{*/ { 0xBFF0000000000000ull, 0xBFF0000000000000ull }, //DC_ONEn },
	/*{*/ { 0x8000000000000000ull, 0x8000000000000000ull }, //DC_ZEROn },
	/*{*/ { 0x0000000000000000ull, 0x0000000000000000ull }, //DC_ZEROp },
	/*{*/ { 0x3FF0000000000000ull, 0x3FF0000000000000ull }, //DC_ONEp },
	/*{*/ { 0x000F4A8B6037B1F5ull, 0x000F4A8B6037B1F5ull }, //DC_REALp },// real denorm, real norm 
	/*{*/ { 0x7FF0000000000000ull, 0x7FF0000000000000ull }, //DC_INFp },
	/*{*/ { 0x7FFFFFFFFFFFFFFFull, 0x7FFFFFFFFFFFFFFFull }//, DC_NAN }// nan
};

//////////////////////////////////////////////////////////////////////////
void CShader5xTest_dtou::InitTestParameters()
{
	CTestCaseParameter<Binary64Vec2>* pSrc = AddParameter<Binary64Vec2>( _T("Src"), &m_Src );
	
	testfactor::RFactor src_t0 = AddParameterValueSet( pSrc, new CTableValueSet<Binary64Vec2>( dtouInputTable, sizeof(Binary64Vec2), sizeof(dtouInputTable) / sizeof(Binary64Vec2) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xy") );
	
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("x"));
	pMaskValueSet->AddValue(_T("y"));
	pMaskValueSet->AddValue(_T("z"));
	pMaskValueSet->AddValue(_T("w"));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("xz"));
	pMaskValueSet->AddValue(_T("xw"));
	pMaskValueSet->AddValue(_T("yz"));
	pMaskValueSet->AddValue(_T("yw"));
	pMaskValueSet->AddValue(_T("zw"));
	
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xyzw") );
	
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xyzw") );
	pSwizzleValueSet->AddValue( _T("xyxy") );
	pSwizzleValueSet->AddValue( _T("zwxy") ); 
	pSwizzleValueSet->AddValue( _T("zwzw") );  
	
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrcMod = AddParameter<SRC_MOD_TYPE>( _T("SrcMod"), &m_SrcModifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrcMod, SMOD_NONE );
	
	CUserValueSet<SRC_MOD_TYPE> *pSrcModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrcModValueSet->AddValue( SMOD_NONE );
	pSrcModValueSet->AddValue( SMOD_NEG );

	CTestCaseParameter<OPERAND_TYPE>* pSrcType = AddParameter<OPERAND_TYPE>( _T("OperandType"), &m_OperandType );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrcType, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperandTypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor srcType = AddParameterValueSet( pSrcType, pOperandTypeValueSet );
	
	testfactor::RFactor srcMod = AddParameterValueSet( pSrcMod, pSrcModValueSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src_t0 * srcMod * srcType * swizzleFactor * maskFactor;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("SrcMod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"), _T("xy")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<OPERAND_TYPE>(_T("OperandType"), OT_TEMP), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.0f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dtou::SetupTestCase()
{
	HRESULT hr = 0;
	TEST_RESULT trResult = RESULT_PASS;
	const tstring negString = (SMOD_NEG & m_SrcModifier) ? _T("-") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;

	if ( g_TestApp.Is11_1DDIOrGreater() == false
	    || g_TestApp.IsFL11OrGreater() == false
	    || g_TestApp.DoublesAreSupported() == false )
		return RESULT_SKIP;

	trResult = CShader5xTest_DoubleArith::SetupTestCase();

	if( RESULT_PASS != trResult )
		return trResult;

	ID3D10Blob* pShaderBuf(NULL);

	assert((OT_TEMP == m_OperandType) || (OT_CONSTANT_BUFFER == m_OperandType));

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);

	if(OT_TEMP == m_OperandType)
	{
		asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	}

	asmHelper.SetupMovToTempFromLiteral( 1, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	asmHelper.SetupMovToRawOutputFromTemp( 0, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();
	asmHelper.EnableDoubleExtensionsSupport();

	tstring instructionCode = FormatString( _T("%s %s%s, %s%s%s"), 
						_T("dtou"),

						_T("r1"),
						maskString.c_str(),

						negString.c_str(),
						OT_TEMP == m_OperandType ? _T("r0") : _T("c0[0]"),
						swizString.c_str()
						);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
						GetEffectiveContext(),
						this,
						m_ShaderStage,
						g_TestApp.m_D3DOptions,
						instructionCode,
						&m_pRTBufferView,
						&m_pRTUABufferView,
						1,
						&pShaderBuf,
						&m_pVS,
						&m_pGS,
						&m_pPS,
						&m_pHS,
						&m_pDS,
						&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		trResult = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		trResult = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);

	return trResult;
}

CShader5xTest_DoubleArith::Binary64Vec2 CShader5xTest_dtou::ReCalcSrcSwizzle(CShader5xTest_DoubleArith::Binary64Vec2 const& x)
{
	if(m_Swizzle.size() < 4)
	{
		return x;
	}

	CShader5xTest_DoubleArith::Binary64Vec2 retValue;

	//if swizzle[0] is z (ie zw) src component [1] was used
	switch(m_Swizzle[0])
	{
		case 'z'://zw
			retValue.m_X = x.m_Y;
			break;
		default://xy implied
			retValue.m_X = x.m_X;
			break;
	}

	//if swizzle[2] is x (ie xy) src component [0] was used
	switch(m_Swizzle[2])
	{
		case 'x'://xy
			retValue.m_Y = x.m_X;
			break;
		default://zw implied
			retValue.m_Y = x.m_Y;
			break;
	}

	return retValue;
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_dtou::ExecuteTestCase()
{
	HRESULT hr = 0;
	TEST_RESULT trResult = RESULT_PASS;

	if(FAILED(hr = MapAndWrite(GetEffectiveContext(), m_pConstBuffer, m_Src)))
	{
		WriteToLog(_T("CShader5xTest_dtou::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s."), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	if(m_ShaderStage == D3D_SHADER_STAGE_COMPUTE)
	{
		GetEffectiveContext()->Dispatch(1, 1, 1);
	}
	else
	{
		GetEffectiveContext()->Draw(1, 0);
	}

	GetEffectiveContext()->CopySubresourceRegion(m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL);

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	UINT result[4];//0=x 1=y 2=z 3=w
	if(FAILED(hr = MapAndRead(GetImmediateContext(), m_pRTBufferCopy, result)))
	{
		WriteToLog(_T("CShader5xTest_dtou::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s."), D3DHResultToString(hr).c_str());
		return RESULT_FAIL;
	}

	//Get "Modified" Value
	//Re-order based on swizzle (so we will be 0:1 based).  
	CShader5xTest_DoubleArith::Binary64Vec2 const srcValue = ReCalcSrcSwizzle(GetModifiedValue( m_Src, m_SrcModifier ));

	//Our source index should now map to the result index, based off the swizzle used
	for(UINT i = 0; i < m_Mask.size(); ++i)
	{
		UINT resIndex = 0;
		//Figure out result index from mask value
		switch(m_Mask[i])
		{
			case 'y':
				resIndex = 1;
				break;
			case 'z':
				resIndex = 2;
				break;
			case 'w':
				resIndex = 3;
				break;
			default://x is default, resIndex init to 0
				break;
		}
		
        // Calculate the expected result.
		UINT  srcResult = 0; 

        if ( IsNaN( srcValue.m_Vec[i] )
            || IsNegINF( srcValue.m_Vec[i] )
            || IsZero( srcValue.m_Vec[i] )
            || IsDNorm( srcValue.m_Vec[i] )
            || IsNegReal( srcValue.m_Vec[i] ) )
        {
            srcResult = 0;
        }
        else
        if ( IsPosINF( srcValue.m_Vec[i] ) )
        {
            srcResult = UINT_MAX;
        }
        else
        {
    		srcResult = (UINT)(double)srcValue.m_Vec[i].m_AsDouble;
        }

		if( srcResult != result[resIndex])
		{
			WriteToLog( 1, "Expected (for component %d) 0x%0I64X 0x%I32X : result 0x%I32X.", i, srcValue.m_Vec[i], srcResult, result[resIndex] );
			trResult = RESULT_FAIL;
		}
	}
	
	return trResult;
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_dtou::~CShader5xTest_dtou()
{
}

//////////////////////////////////////////////////////////////////////////
//  itod
//////////////////////////////////////////////////////////////////////////
const CShader5xTest_DoubleArith::Binary32Vec2 itodInputTable[] = 
{
	 { 0xFF800000u, 0xFF800000u }, 
	 { 0x86A05FB1u, 0x8059AF1Bu }, 
	 { 0xBF800000u, 0xBF800000u }, 
	 { 0x80000000u, 0x80000000u }, 
	 { 0x00000000u, 0x00000000u }, 
	 { 0x3F800000u, 0x3F800000u }, 
	 { 0x00004823u, 0x5A4CD285u }, 
	 { 0x7F800000u, 0x7F800000u }, 
	 { 0x7F800001u, 0x7F800001u },
	 { 0x0000000Fu, 0x00000000u },
	 { 0x000000F0u, 0x0000000Fu },
	 { 0x00000F00u, 0x000000F0u },
	 { 0x0000F000u, 0x00000F00u },
	 { 0x000F0000u, 0x0000F000u },
	 { 0x00F00000u, 0x000F0000u },
	 { 0x0F000000u, 0x00F00000u },
	 { 0xF0000000u, 0x0F000000u },
};

//////////////////////////////////////////////////////////////////////////
void CShader5xTest_itod::InitTestParameters()
{
	CTestCaseParameter<Binary32Vec2>* pSrc = AddParameter<Binary32Vec2>( _T("Src"), &m_Src );
	
	testfactor::RFactor src_t0 = AddParameterValueSet( pSrc, new CTableValueSet<Binary32Vec2>( itodInputTable, sizeof(Binary32Vec2), sizeof(itodInputTable) / sizeof(Binary32Vec2) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xyzw") );
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("xyzw"));
	pMaskValueSet->AddValue(_T(""));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("zw"));
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xy") );
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xy") );
	pSwizzleValueSet->AddValue( _T("yx") );
	pSwizzleValueSet->AddValue( _T("xx") ); 
	pSwizzleValueSet->AddValue( _T("yy") );  
	pSwizzleValueSet->AddValue( _T("") );
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrcMod = AddParameter<SRC_MOD_TYPE>( _T("SrcMod"), &m_SrcModifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrcMod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrcModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrcModValueSet->AddValue( SMOD_NONE );
	pSrcModValueSet->AddValue( SMOD_NEG );
	testfactor::RFactor srcMod = AddParameterValueSet( pSrcMod, pSrcModValueSet );

	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src_t0 * srcMod * swizzleFactor;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("SrcMod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xy")), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_itod::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	if ( g_TestApp.Is11_1DDIOrGreater() == false
	    || g_TestApp.IsFL11OrGreater() == false
	    || g_TestApp.DoublesAreSupported() == false )
		return RESULT_SKIP;

	tRes = CShader5xTest_DoubleArith::SetupTestCase();

	if( RESULT_PASS != tRes )
		return tRes;

	const tstring negString = (SMOD_NEG & m_SrcModifier) ? _T("-") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;

	ID3D10Blob* pShaderBuf(NULL);

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);

	asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	asmHelper.SetupMovToTempFromLiteral( 1, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	asmHelper.SetupMovToRawOutputFromTemp( 0, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();
	asmHelper.EnableDoubleExtensionsSupport();

	tstring instructionCode = FormatString( _T("%s %s%s, %s%s%s"), 
						_T("itod"),

						_T("r1"),
						maskString.c_str(),

						negString.c_str(),
						_T("r0"),
						swizString.c_str()
						);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
						GetEffectiveContext(),
						this,
						m_ShaderStage,
						g_TestApp.m_D3DOptions,
						instructionCode,
						&m_pRTBufferView,
						&m_pRTUABufferView,
						1,
						&pShaderBuf,
						&m_pVS,
						&m_pGS,
						&m_pPS,
						&m_pHS,
						&m_pDS,
						&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);
	return tRes;
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_itod::ExecuteTestCase()
{
	HRESULT hr;
	{
		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, m_Src ) ) )
		{
			WriteToLog( _T("CShader5xTest_itod::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	if( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->Draw( 1, 0 );
	}

	GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	{
		Binary64Vec2 result;
		TEST_RESULT tr = RESULT_PASS;
		if( FAILED( hr = MapAndRead( GetImmediateContext(), m_pRTBufferCopy, result ) ) )
		{
			WriteToLog( _T("CShader5xTest_itod::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s."), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		{
			// Determine what the result should be.
			SignedBinary32Vec2  srcValue;
			Binary64Vec2  srcAsDouble;

			srcValue.m_VecAsInt[0] = m_Src.m_VecAsUInt[0];
			srcValue.m_VecAsInt[1] = m_Src.m_VecAsUInt[1];
			srcValue = GetModifiedValue( srcValue, m_SrcModifier );

			srcAsDouble.m_Vec[0].m_AsDouble = (double)(int)srcValue.m_VecAsInt[0];
			srcAsDouble.m_Vec[1].m_AsDouble = (double)(int)srcValue.m_VecAsInt[1];

			// Translate the swizzle. (from int to double meaning)
			tstring swizzle;
			if( m_Swizzle.empty() )
			{
				swizzle = _T("xyzw");
			}
			else
			{
				for( unsigned int i(0); i < m_Swizzle.size(); ++i )
				{
					switch( m_Swizzle[i] )
					{
						case _T('x'):
							swizzle += _T("xy");
							break;
						case _T('y'):
							swizzle += _T("zw");
							break;
					}
				}
			}
	
			if( CompareBinary64Vec2( srcAsDouble, result, _T("o0"), m_Mask, swizzle, true ) == false )
			{
				const bool v0 = IsDNorm(srcValue.m_X);
				const bool v1 = IsDNorm(srcValue.m_Y);
				if( v0 || v1 )
				{
					Binary64Vec2 const newSrcAsDouble = { v0 ? s_Zero64 : srcAsDouble.m_X, v1 ? s_Zero64 : srcAsDouble.m_Y };
					if( CompareBinary64Vec2( newSrcAsDouble, result, _T("o0"), m_Mask, swizzle ) == false )
					{
						Binary64Vec2 const new2SrcAsDouble = { v0 ? s_NegZero64 : srcAsDouble.m_X, v1 ? s_NegZero64 : srcAsDouble.m_Y };
						if( CompareBinary64Vec2( new2SrcAsDouble, result, _T("o0"), m_Mask, swizzle ) == false )
						{
							tr = RESULT_FAIL;
						}
					}
				}
				else
				{
					tr = RESULT_FAIL;
				}
			}
		}

		if ( tr == RESULT_FAIL )
		{
        		const tstring negString = (SMOD_NEG & m_SrcModifier) ? _T("Negate") : _T("None");

    			WriteToLog( 1, _T("CShader5xTest_itod::ExecuteTestCase()  ITOD        .x Input:    %d  Modifier: %s"), (int)m_Src.m_VecAsUInt[0], negString.c_str() );
    			WriteToLog( 1, _T("CShader5xTest_itod::ExecuteTestCase()              .x Input:    0x%08X"), m_Src.m_VecAsUInt[0] );
    			WriteToLog( 1, _T("CShader5xTest_itod::ExecuteTestCase()  ITOD        .y Input:    %d  Modifier: %s"), (int)m_Src.m_VecAsUInt[1], negString.c_str() );
    			WriteToLog( 1, _T("CShader5xTest_itod::ExecuteTestCase()              .y Input:    0x%08X"), m_Src.m_VecAsUInt[1] );
		}

		return tr;
	}
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_itod::~CShader5xTest_itod()
{
}

//////////////////////////////////////////////////////////////////////////
//  utod
//////////////////////////////////////////////////////////////////////////
const CShader5xTest_DoubleArith::Binary32Vec2 utodInputTable[] = 
{
	 { 0xFF800000u, 0xFF800000u }, 
	 { 0x86A05FB1u, 0x8059AF1Bu }, 
	 { 0xBF800000u, 0xBF800000u }, 
	 { 0x80000000u, 0x80000000u }, 
	 { 0x00000000u, 0x00000000u }, 
	 { 0x3F800000u, 0x3F800000u }, 
	 { 0x00004823u, 0x5A4CD285u }, 
	 { 0x7F800000u, 0x7F800000u }, 
	 { 0x7F800001u, 0x7F800001u },
	 { 0x0000000Fu, 0x00000000u },
	 { 0x000000F0u, 0x0000000Fu },
	 { 0x00000F00u, 0x000000F0u },
	 { 0x0000F000u, 0x00000F00u },
	 { 0x000F0000u, 0x0000F000u },
	 { 0x00F00000u, 0x000F0000u },
	 { 0x0F000000u, 0x00F00000u },
	 { 0xF0000000u, 0x0F000000u },
};

//////////////////////////////////////////////////////////////////////////
void CShader5xTest_utod::InitTestParameters()
{
	CTestCaseParameter<Binary32Vec2>* pSrc = AddParameter<Binary32Vec2>( _T("Src"), &m_Src );
	
	testfactor::RFactor src_t0 = AddParameterValueSet( pSrc, new CTableValueSet<Binary32Vec2>( utodInputTable, sizeof(Binary32Vec2), sizeof(utodInputTable) / sizeof(Binary32Vec2) ) );

	CTestCaseParameter<tstring>* pMaskParam = AddParameter<tstring>( _T("Mask"), &m_Mask );
	SetParameterDefaultValue<tstring>( pMaskParam, _T("xyzw") );
	CUserValueSet<tstring> *pMaskValueSet = new CUserValueSet<tstring>();
	pMaskValueSet->AddValue(_T("xyzw"));
	pMaskValueSet->AddValue(_T(""));
	pMaskValueSet->AddValue(_T("xy"));
	pMaskValueSet->AddValue(_T("zw"));
	testfactor::RFactor maskFactor = AddParameterValueSet( pMaskParam, pMaskValueSet );

	CTestCaseParameter<tstring>* pSwizzleParam = AddParameter<tstring>( _T("Swizzle"), &m_Swizzle );
	SetParameterDefaultValue<tstring>( pSwizzleParam, _T("xy") );
	CUserValueSet<tstring> *pSwizzleValueSet = new CUserValueSet<tstring>();
	pSwizzleValueSet->AddValue( _T("xy") );
	pSwizzleValueSet->AddValue( _T("yx") );
	pSwizzleValueSet->AddValue( _T("xx") ); 
	pSwizzleValueSet->AddValue( _T("yy") );  
	pSwizzleValueSet->AddValue( _T("") );
	testfactor::RFactor swizzleFactor = AddParameterValueSet( pSwizzleParam, pSwizzleValueSet );

	CTestCaseParameter<SRC_MOD_TYPE>* pSrcMod = AddParameter<SRC_MOD_TYPE>( _T("SrcMod"), &m_SrcModifier );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pSrcMod, SMOD_NONE );
	CUserValueSet<SRC_MOD_TYPE> *pSrcModValueSet = new CUserValueSet<SRC_MOD_TYPE>();
	pSrcModValueSet->AddValue( SMOD_NONE );
	testfactor::RFactor srcMod = AddParameterValueSet( pSrcMod, pSrcModValueSet );

	CTestCaseParameter<OPERAND_TYPE>* pSrcType = AddParameter<OPERAND_TYPE>( _T("OperandType"), &m_OperandType );
	SetParameterDefaultValue<OPERAND_TYPE>( pSrcType, OT_TEMP );
	CUserValueSet<OPERAND_TYPE>* pOperandTypeValueSet = new CUserValueSet<OPERAND_TYPE>( OT_TEMP, OT_CONSTANT_BUFFER );
	testfactor::RFactor srcType = AddParameterValueSet( pSrcType, pOperandTypeValueSet );
	
	// Adds common parameters like "ShaderStage"
	AddCommonParameters();

	// Utilize a base class function to add the shader stages to the "important" testcases
	testfactor::RFactor rfTestCases = src_t0 * srcMod * swizzleFactor;
	testfactor::RFactor rfTestCasesWithShaderStages = GetTestFactorWithDefaultStageCoverage( rfTestCases );

	SetRootTestFactor( rfTestCasesWithShaderStages  );

	AddPriorityPatternFilter(FilterEqual<SRC_MOD_TYPE>(_T("SrcMod"), SMOD_NONE), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Mask"), _T("xyzw")), WeightFactorLarge);
	AddPriorityPatternFilter(FilterEqual<tstring>(_T("Swizzle"), _T("xy")), WeightFactorLarge);

	DeclarePriorityLevelByPriorityValue( 1, 2.25f );
	DeclarePriorityLevelByPriorityValue( 2, 1.5f );
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_utod::SetupTestCase()
{
	HRESULT hr;
	TEST_RESULT tRes = RESULT_PASS;

	if ( g_TestApp.Is11_1DDIOrGreater() == false
	    || g_TestApp.IsFL11OrGreater() == false
	    || g_TestApp.DoublesAreSupported() == false )
		return RESULT_SKIP;

	tRes = CShader5xTest_DoubleArith::SetupTestCase();

	if( RESULT_PASS != tRes )
		return tRes;

	const tstring negString = (SMOD_NEG & m_SrcModifier) ? _T("-") : _T("");
	const tstring absString_pre = (SMOD_ABS & m_SrcModifier) ? _T("abs(") : _T("");
	const tstring absString_suf = (SMOD_ABS & m_SrcModifier) ? _T(")") : _T("");
	const tstring maskString = m_Mask.empty() ? _T("") : _T(".") + m_Mask;
	const tstring swizString = m_Swizzle.empty() ? _T("") : _T(".") + m_Swizzle;

	ID3D10Blob* pShaderBuf(NULL);

	assert((OT_TEMP == m_OperandType) || (OT_CONSTANT_BUFFER == m_OperandType));

	AssemblyHelper asmHelper(20, 200, m_RTFormat, m_viewPort);

	asmHelper.SetupMovToTempFromCB( 0, AssemblyHelper::xyzw, 0, AssemblyHelper::swizXYZW );
	asmHelper.SetupMovToTempFromLiteral( 1, AssemblyHelper::xyzw, tstring("l( 0xBAADF00D, 0xBAADF00D, 0xBAADF00D, 0xBAADF00D )") );
	asmHelper.SetupMovToRawOutputFromTemp( 0, AssemblyHelper::xyzw, 1, AssemblyHelper::swizXYZW );
	asmHelper.SetOutputRegMaskRaw(0, AssemblyHelper::xyzw);
	asmHelper.EnableDoubleSupport();
	asmHelper.EnableDoubleExtensionsSupport();

	tstring instructionCode = FormatString( _T("%s %s%s, %s%s%s%s%s"), 
						_T("utod"),

						_T("r1"),
						maskString.c_str(),

						negString.c_str(),
						absString_pre.c_str(),
						OT_TEMP == m_OperandType ? _T("r0") : _T("c0[0]"),
						swizString.c_str(),
						absString_suf.c_str()
						);

	hr = asmHelper.SetupPipelineWithShaderCode( GetDevice(),
						GetEffectiveContext(),
						this,
						m_ShaderStage,
						g_TestApp.m_D3DOptions,
						instructionCode,
						&m_pRTBufferView,
						&m_pRTUABufferView,
						1,
						&pShaderBuf,
						&m_pVS,
						&m_pGS,
						&m_pPS,
						&m_pHS,
						&m_pDS,
						&m_pCS );

	if( FAILED(hr) )
	{
		WriteError( _T("SetupPipelineWithShaderCode() failed: hr = %s"), D3DHResultToString(hr).c_str() );
		tRes = RESULT_FAIL;
	}

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		tRes = RESULT_FAIL;
	}

	SAFE_RELEASE(pShaderBuf);
	return tRes;
}

//////////////////////////////////////////////////////////////////////////
TEST_RESULT CShader5xTest_utod::ExecuteTestCase()
{
	HRESULT hr;
	{
		if( FAILED( hr = MapAndWrite( GetEffectiveContext(), m_pConstBuffer, m_Src ) ) )
		{
			WriteToLog( _T("CShader5xTest_utod::ExecuteTestCase() failed to map m_pConstBuffer, hr = %s." ), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}
	}

	if( m_ShaderStage == D3D_SHADER_STAGE_COMPUTE )
	{
		GetEffectiveContext()->Dispatch( 1, 1, 1 );
	}
	else
	{
		GetEffectiveContext()->Draw( 1, 0 );
	}

	GetEffectiveContext()->CopySubresourceRegion( m_pRTBufferCopy, 0, 0, 0, 0, m_pRTBuffer, 0, NULL );

	if( FAILED( ExecuteEffectiveContext() ) )
	{
		WriteToLog("ExecuteEffectiveContext() failed");
		return RESULT_FAIL;
	}

	{
		Binary64Vec2 result;
		TEST_RESULT tr = RESULT_PASS;
		if( FAILED( hr = MapAndRead( GetImmediateContext(), m_pRTBufferCopy, result ) ) )
		{
			WriteToLog( _T("CShader5xTest_utod::ExecuteTestCase() failed to map m_pRTBufferCopy, hr = %s."), D3DHResultToString(hr).c_str() );
			return RESULT_FAIL;
		}

		{
			// Determine what the result should be.
			Binary32Vec2 const srcValue = GetModifiedValue( m_Src, m_SrcModifier );
			Binary64Vec2 srcAsDouble;

			srcAsDouble.m_Vec[0].m_AsDouble = (double)(UINT)srcValue.m_VecAsUInt[0];
			srcAsDouble.m_Vec[1].m_AsDouble = (double)(UINT)srcValue.m_VecAsUInt[1];

			// Translate the swizzle. (from UINT to double meaning)
			tstring swizzle;
			if( m_Swizzle.empty() )
			{
				swizzle = _T("xyzw");
			}
			else
			{
				for( unsigned int i(0); i < m_Swizzle.size(); ++i )
				{
					switch( m_Swizzle[i] )
					{
						case _T('x'):
							swizzle += _T("xy");
							break;
						case _T('y'):
							swizzle += _T("zw");
							break;
					}
				}
			}
	
			if( CompareBinary64Vec2( srcAsDouble, result, _T("o0"), m_Mask, swizzle, true ) == false )
			{
				const bool v0 = IsDNorm(srcValue.m_X);
				const bool v1 = IsDNorm(srcValue.m_Y);
				if( v0 || v1 )
				{
					Binary64Vec2 const newSrcAsDouble = { v0 ? s_Zero64 : srcAsDouble.m_X, v1 ? s_Zero64 : srcAsDouble.m_Y };
					if( CompareBinary64Vec2( newSrcAsDouble, result, _T("o0"), m_Mask, swizzle ) == false )
					{
						Binary64Vec2 const new2SrcAsDouble = { v0 ? s_NegZero64 : srcAsDouble.m_X, v1 ? s_NegZero64 : srcAsDouble.m_Y };
						if( CompareBinary64Vec2( new2SrcAsDouble, result, _T("o0"), m_Mask, swizzle ) == false )
						{
							tr = RESULT_FAIL;
						}
					}
				}
				else
				{
					tr = RESULT_FAIL;
				}
			}
		}
		return tr;
	}
}

//////////////////////////////////////////////////////////////////////////
CShader5xTest_utod::~CShader5xTest_utod()
{
}

//////////////////////////////////////////////////////////////////////////
// Test data from here to the end.
//////////////////////////////////////////////////////////////////////////
CShader5xTest_DoubleArith::Binary64Vec2 const CShader5xTest_dmul::s_DMulSrc0[] = 
{
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xBFF0000000000000ull, 0xBFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FFFCB8D9A5D4659ull, 0x7FFFCB8D9A5D4659ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x7FF6B0586EF53833ull, 0x7FF6B0586EF53833ull },
	{ 0xE832B30C2F906709ull, 0xE832B30C2F906709ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xD3DC7C4B89F05653ull, 0xD3DC7C4B89F05653ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xC4DA8FD9E6AA3677ull, 0xC4DA8FD9E6AA3677ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xC8CB013B39FB3E20ull, 0xC8CB013B39FB3E20ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x5C01D1EEEABC7754ull, 0x5C01D1EEEABC7754ull },
	{ 0x41E21AD82491B346ull, 0x41E21AD82491B346ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF84E1BFCCABC0Bull, 0x7FF84E1BFCCABC0Bull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x8000000000000864ull, 0x8000000000000864ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x800000000010DD88ull, 0x800000000010DD88ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x800000000001F606ull, 0x800000000001F606ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x0000046A7CD1A70Cull, 0x0000046A7CD1A70Cull },
	{ 0x000000000003952Full, 0x000000000003952Full },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x0D0656509E98A510ull, 0x0D0656509E98A510ull },
	{ 0xCD4086C5BFCDA2BBull, 0xCD4086C5BFCDA2BBull },
	{ 0xE495B5F9B32EFE04ull, 0xE495B5F9B32EFE04ull },
	{ 0x3031A2686B267CF5ull, 0x3031A2686B267CF5ull },
	{ 0x0F381FC69ED1CCAEull, 0x0F381FC69ED1CCAEull },
	{ 0x744BABA648DF8839ull, 0x744BABA648DF8839ull },
	{ 0xDB9CAB673523DFAFull, 0xDB9CAB673523DFAFull },
	{ 0xE456D172A0AADA0Bull, 0xE456D172A0AADA0Bull },
	{ 0xCECA2AF0E92E414Dull, 0xCECA2AF0E92E414Dull },
	{ 0x1D8555184AB52152ull, 0x1D8555184AB52152ull },
	{ 0x9BFE5AC3AF82F4F2ull, 0x9BFE5AC3AF82F4F2ull },
	{ 0x873FBA508FDC665Bull, 0x873FBA508FDC665Bull },
	{ 0x1CA71232DE87B82Cull, 0x1CA71232DE87B82Cull },
	{ 0xCAFAF77E0A8ECB55ull, 0xCAFAF77E0A8ECB55ull },
	{ 0xC89572920CA8C32Dull, 0xC89572920CA8C32Dull },
	{ 0x4CB8A54B86514DD4ull, 0x4CB8A54B86514DD4ull },
	{ 0xADCB17E5F1B387CBull, 0xADCB17E5F1B387CBull },
	{ 0x22812F0EDF2E83D1ull, 0x22812F0EDF2E83D1ull },
	{ 0x686148F945D970FFull, 0x686148F945D970FFull },
	{ 0x8CD802AFE20D5EA7ull, 0x8CD802AFE20D5EA7ull },
	{ 0x9B1A2675A39AA4E8ull, 0x9B1A2675A39AA4E8ull },
	{ 0xEE29C0532D6FE815ull, 0xEE29C0532D6FE815ull },
	{ 0xE67BDC206518C2C8ull, 0xE67BDC206518C2C8ull },
	{ 0xC3746AE9109A05C0ull, 0xC3746AE9109A05C0ull },
	{ 0xE185D03B8651A483ull, 0xE185D03B8651A483ull },
	{ 0x9BC997A06BA03630ull, 0x9BC997A06BA03630ull },
	{ 0x119FD2EA83C21E25ull, 0x119FD2EA83C21E25ull },
	{ 0x908A9E7D8FDE51D1ull, 0x908A9E7D8FDE51D1ull },
	{ 0xE2034F7B3BB0BD5Aull, 0xE2034F7B3BB0BD5Aull },
	{ 0x3D4EC9751F564B72ull, 0x3D4EC9751F564B72ull },
	{ 0x9CBE7B72CC608975ull, 0x9CBE7B72CC608975ull },
	{ 0x37F3858AED76EF47ull, 0x37F3858AED76EF47ull },
	{ 0x619186BE756BC3EDull, 0x619186BE756BC3EDull },
	{ 0x08B69214DE0AA469ull, 0x08B69214DE0AA469ull },
	{ 0x1EE5CC0877C6A959ull, 0x1EE5CC0877C6A959ull },
	{ 0x21CA30A1C70E2BD2ull, 0x21CA30A1C70E2BD2ull },
	{ 0x891F0050F52A2E79ull, 0x891F0050F52A2E79ull },
	{ 0xD7E355A44E2F5FE3ull, 0xD7E355A44E2F5FE3ull },
	{ 0x92F6632AD066C32Full, 0x92F6632AD066C32Full },
	{ 0x59E1D801CAB3F561ull, 0x59E1D801CAB3F561ull },
	{ 0x657FEE9C8CDF1180ull, 0x657FEE9C8CDF1180ull },
	{ 0xA8D2A0E524C83AF1ull, 0xA8D2A0E524C83AF1ull },
	{ 0xD7D987252F6ABD15ull, 0xD7D987252F6ABD15ull },
	{ 0x103BDC35B5F5D8A2ull, 0x103BDC35B5F5D8A2ull },
	{ 0x67A42BE51DE123BEull, 0x67A42BE51DE123BEull },
	{ 0x9D83283228029261ull, 0x9D83283228029261ull },
	{ 0xB087266AFECC1CECull, 0xB087266AFECC1CECull },
	{ 0x18ADC56F56770404ull, 0x18ADC56F56770404ull },
	{ 0x649D3AA49A42B9B4ull, 0x649D3AA49A42B9B4ull },
	{ 0xFBCAC79B2D4D64C3ull, 0xFBCAC79B2D4D64C3ull },
	{ 0xC3FBD753B8AC06D1ull, 0xC3FBD753B8AC06D1ull },
	{ 0xEB3744238A0844B8ull, 0xEB3744238A0844B8ull },
	{ 0x1534454703E9C9A1ull, 0x1534454703E9C9A1ull },
	{ 0xAD9C83A11AF14EE4ull, 0xAD9C83A11AF14EE4ull },
	{ 0x9D1ED836E66B4124ull, 0x9D1ED836E66B4124ull },
	{ 0x23B830C63EC007ABull, 0x23B830C63EC007ABull },
	{ 0x96801E266C27E802ull, 0x96801E266C27E802ull },
	{ 0xC0DE88F1E6288ED4ull, 0xC0DE88F1E6288ED4ull },
	{ 0x2AC80F8821C23184ull, 0x2AC80F8821C23184ull },
	{ 0x82468BA672125B0Cull, 0x82468BA672125B0Cull },
	{ 0x695B3F86F5124B99ull, 0x695B3F86F5124B99ull },
	{ 0xE7902B6494F70163ull, 0xE7902B6494F70163ull },
	{ 0x3FF60B931498DDD0ull, 0x3FF60B931498DDD0ull },
	{ 0xEAB47D0A31A8ECCCull, 0xEAB47D0A31A8ECCCull },
	{ 0xF2B9CBD2D059C962ull, 0xF2B9CBD2D059C962ull },
	{ 0x77BFE7D53C82241Full, 0x77BFE7D53C82241Full },
	{ 0x93B402D778DEEA27ull, 0x93B402D778DEEA27ull },
	{ 0xE4F6558A9B7B0918ull, 0xE4F6558A9B7B0918ull },
	{ 0xFB8F8D763FA2D61Eull, 0xFB8F8D763FA2D61Eull },
	{ 0x6B11626103A915D7ull, 0x6B11626103A915D7ull },
	{ 0xC0DFD36517A39D68ull, 0xC0DFD36517A39D68ull },
	{ 0x9CF88D4FDCE59E5Full, 0x9CF88D4FDCE59E5Full },
	{ 0xB0B5F6DB949D88CBull, 0xB0B5F6DB949D88CBull },
	{ 0x5FB367621F5C921Aull, 0x5FB367621F5C921Aull },
	{ 0xC6500417CB6DDC32ull, 0xC6500417CB6DDC32ull },
	{ 0xE2B5C4CD340339D2ull, 0xE2B5C4CD340339D2ull },
	{ 0xA3322247315D95A9ull, 0xA3322247315D95A9ull },
	{ 0x9795EB6BD5B6F7B7ull, 0x9795EB6BD5B6F7B7ull },
	{ 0x8719C2F37D942C63ull, 0x8719C2F37D942C63ull },
	{ 0xAA2CC45CEEC408DDull, 0xAA2CC45CEEC408DDull },
	{ 0x47FAD21387725236ull, 0x47FAD21387725236ull },
	{ 0xF8800AA67C1443BDull, 0xF8800AA67C1443BDull },
	{ 0xCA16E66027D4B31Cull, 0xCA16E66027D4B31Cull },
	{ 0x8B167C7C6A85D932ull, 0x8B167C7C6A85D932ull },
	{ 0xF7EE70CD187EB532ull, 0xF7EE70CD187EB532ull },
	{ 0x0C8609AB78AA13FCull, 0x0C8609AB78AA13FCull },
	{ 0x3A4BECE5D42D36BAull, 0x3A4BECE5D42D36BAull },
	{ 0x419C0548152C183Eull, 0x419C0548152C183Eull },
	{ 0x700910AE7651529Bull, 0x700910AE7651529Bull },
	{ 0x818F54A140DFA501ull, 0x818F54A140DFA501ull },
	{ 0x668DFCC89C331C5Dull, 0x668DFCC89C331C5Dull },
	{ 0x2D9C9F976C77D3B1ull, 0x2D9C9F976C77D3B1ull },
	{ 0x513A6C3C42FC60B0ull, 0x513A6C3C42FC60B0ull },
	{ 0xA97C80175AA5D59Dull, 0xA97C80175AA5D59Dull },
	{ 0x41D8E464A96B68E4ull, 0x41D8E464A96B68E4ull },
	{ 0x52F3B358FE1FB8F9ull, 0x52F3B358FE1FB8F9ull },
	{ 0xA06BE5812FADB571ull, 0xA06BE5812FADB571ull },
	{ 0x7A3D48425B8F22DEull, 0x7A3D48425B8F22DEull },
	{ 0x23151A2A35E2C26Eull, 0x23151A2A35E2C26Eull },
	{ 0x59AED0EE67E81546ull, 0x59AED0EE67E81546ull },
	{ 0x46868749FE46C51Cull, 0x46868749FE46C51Cull },
	{ 0x8DD691B7E967A993ull, 0x8DD691B7E967A993ull },
	{ 0x454DBC798B456DDAull, 0x454DBC798B456DDAull },
	{ 0x108903A180F4A630ull, 0x108903A180F4A630ull },
	{ 0x2228688ABA0D284Bull, 0x2228688ABA0D284Bull },
	{ 0xC756919F03B751C7ull, 0xC756919F03B751C7ull },
	{ 0x5ED633E28AFF896Aull, 0x5ED633E28AFF896Aull },
	{ 0xAF5DB52C99080DEDull, 0xAF5DB52C99080DEDull },
	{ 0x993401669346FBD5ull, 0x993401669346FBD5ull },
	{ 0xAC32080F086BF294ull, 0xAC32080F086BF294ull },
	{ 0x612E61DFCF735C5Full, 0x612E61DFCF735C5Full },
	{ 0x37E09E26388D51D1ull, 0x37E09E26388D51D1ull },
	{ 0x78D1B88C4ADE21C1ull, 0x78D1B88C4ADE21C1ull },
	{ 0x5AC73602133E76DFull, 0x5AC73602133E76DFull },
	{ 0xCC9E6B2603011C15ull, 0xCC9E6B2603011C15ull },
	{ 0x233568FB6164669Cull, 0x233568FB6164669Cull },
	{ 0xEB07124DC675375Cull, 0xEB07124DC675375Cull },
	{ 0x9DF7DA30ADC9A289ull, 0x9DF7DA30ADC9A289ull },
	{ 0xFD08A87A13D9377Aull, 0xFD08A87A13D9377Aull },
	{ 0x4E2D67E72498E34Cull, 0x4E2D67E72498E34Cull },
	{ 0xBDBCBB09FBA07AB7ull, 0xBDBCBB09FBA07AB7ull },
	{ 0x24D65A8F727FDCAFull, 0x24D65A8F727FDCAFull },
	{ 0xF27599FBFF09BA3Eull, 0xF27599FBFF09BA3Eull },
	{ 0x799D956EA4B9FC20ull, 0x799D956EA4B9FC20ull },
	{ 0x62D48748F07DC920ull, 0x62D48748F07DC920ull },
	{ 0x823BC5CB0A432A32ull, 0x823BC5CB0A432A32ull },
	{ 0x50C9E802D2D65350ull, 0x50C9E802D2D65350ull },
	{ 0xCA9493FD12F28798ull, 0xCA9493FD12F28798ull },
	{ 0x705F7377BA8C9FA6ull, 0x705F7377BA8C9FA6ull },
	{ 0xAD1DD2242F7D302Full, 0xAD1DD2242F7D302Full },
	{ 0xDFEC6318AC904F5Eull, 0xDFEC6318AC904F5Eull },
	{ 0x4A00ADBFB160FA72ull, 0x4A00ADBFB160FA72ull },
	{ 0x0054DE0BAB773506ull, 0x0054DE0BAB773506ull },
	{ 0x0BDC0396BA6E9C58ull, 0x0BDC0396BA6E9C58ull },
	{ 0xE4E5D521D16F6A2Eull, 0xE4E5D521D16F6A2Eull },
	{ 0xD032645A7574FC85ull, 0xD032645A7574FC85ull },
	{ 0x3AF1EFDE58321DD6ull, 0x3AF1EFDE58321DD6ull },
	{ 0x6475E9A139A5C76Cull, 0x6475E9A139A5C76Cull },
	{ 0x6C03A4C8D52B8C8Aull, 0x6C03A4C8D52B8C8Aull },
	{ 0x826E29FDE18D9E3Dull, 0x826E29FDE18D9E3Dull },
	{ 0x7FD4B5C11F7E6F7Dull, 0x7FD4B5C11F7E6F7Dull },
	{ 0x9DCB5DE3787FFE0Bull, 0x9DCB5DE3787FFE0Bull },
	{ 0x147B5ED02E5EB705ull, 0x147B5ED02E5EB705ull },
	{ 0xDCB090E81A05FDCDull, 0xDCB090E81A05FDCDull },
	{ 0x5601867CFA94501Cull, 0x5601867CFA94501Cull },
	{ 0x8FA7D0ABD4350E61ull, 0x8FA7D0ABD4350E61ull },
	{ 0xF705EDA55E65DF5Full, 0xF705EDA55E65DF5Full },
	{ 0x29205D8384ADB906ull, 0x29205D8384ADB906ull },
	{ 0x2847601AF42B8511ull, 0x2847601AF42B8511ull },
	{ 0xB606DA27BAC264E0ull, 0xB606DA27BAC264E0ull },
	{ 0x90C9E3A3F7499A96ull, 0x90C9E3A3F7499A96ull },
	{ 0x55877C2796C60EF6ull, 0x55877C2796C60EF6ull },
	{ 0xC5B5E900251373FAull, 0xC5B5E900251373FAull },
	{ 0x30EA3B1FAA030FB5ull, 0x30EA3B1FAA030FB5ull },
	{ 0x42917B519AD61EEAull, 0x42917B519AD61EEAull },
	{ 0x73D90134D699DAECull, 0x73D90134D699DAECull },
	{ 0xE1236FE9B74B2238ull, 0xE1236FE9B74B2238ull },
	{ 0x442FDB732FC7BC4Full, 0x442FDB732FC7BC4Full },
	{ 0xD16096D9FAB340D9ull, 0xD16096D9FAB340D9ull },
	{ 0xBA072910D9D59B77ull, 0xBA072910D9D59B77ull },
	{ 0x1192EAC8E5193568ull, 0x1192EAC8E5193568ull },
	{ 0xD7C7CBD4E9AEB85Full, 0xD7C7CBD4E9AEB85Full },
	{ 0xE62AC0CAD8BC7622ull, 0xE62AC0CAD8BC7622ull },
	{ 0x0209580245DF6DE6ull, 0x0209580245DF6DE6ull },
	{ 0xD1E9F83EF7A3F2EAull, 0xD1E9F83EF7A3F2EAull },
	{ 0x77AA4486857BF0D2ull, 0x77AA4486857BF0D2ull },
	{ 0xC9C21AACD0AF1049ull, 0xC9C21AACD0AF1049ull },
	{ 0xD3BDA614C0B0F3D7ull, 0xD3BDA614C0B0F3D7ull },
	{ 0xB940B0EF65931200ull, 0xB940B0EF65931200ull },
	{ 0xB325252D3FE5DF6Dull, 0xB325252D3FE5DF6Dull },
	{ 0xE2168C2D42B6A2A0ull, 0xE2168C2D42B6A2A0ull },
	{ 0xBFFC78B36964EE5Full, 0xBFFC78B36964EE5Full },
	{ 0xBD87D404E8318353ull, 0xBD87D404E8318353ull },
	{ 0xB44B6CB95D5CAE17ull, 0xB44B6CB95D5CAE17ull },
	{ 0x537CBF1E8F8E1E7Bull, 0x537CBF1E8F8E1E7Bull },
	{ 0x26C2AB48332E9A79ull, 0x26C2AB48332E9A79ull },
	{ 0x392F3E88267DDDD1ull, 0x392F3E88267DDDD1ull },
	{ 0x4C9F5ABC89E2A0EEull, 0x4C9F5ABC89E2A0EEull },
	{ 0x980B44386E2E787Aull, 0x980B44386E2E787Aull },
	{ 0xA8D057D5926B60BEull, 0xA8D057D5926B60BEull },
	{ 0x2921683E33D8E1BEull, 0x2921683E33D8E1BEull },
	{ 0x48E342CDED5ED314ull, 0x48E342CDED5ED314ull },
	{ 0x0C22DDD8A51BC70Cull, 0x0C22DDD8A51BC70Cull },
	{ 0xBC6AADB68D150A81ull, 0xBC6AADB68D150A81ull },
	{ 0x3EA4A28430EE50F7ull, 0x3EA4A28430EE50F7ull },
	{ 0x0D2E4D95952EF3F8ull, 0x0D2E4D95952EF3F8ull },
	{ 0x14CCAE6B66BADFB2ull, 0x14CCAE6B66BADFB2ull },
	{ 0x3745296939451551ull, 0x3745296939451551ull },
	{ 0xB0A9241BD721D09Aull, 0xB0A9241BD721D09Aull },
	{ 0x2129CB2E9EFE4FC8ull, 0x2129CB2E9EFE4FC8ull },
	{ 0x7AD1807EF3E4382Bull, 0x7AD1807EF3E4382Bull },
	{ 0x93786F4CBC4E9DFEull, 0x93786F4CBC4E9DFEull },
	{ 0x9A11CB16F036A687ull, 0x9A11CB16F036A687ull },
	{ 0x30F73443389DD475ull, 0x30F73443389DD475ull },
	{ 0xEB5FC61F9F3BE5F5ull, 0xEB5FC61F9F3BE5F5ull },
	{ 0x6EE84BBD4C166195ull, 0x6EE84BBD4C166195ull },
	{ 0xF9DA1DF355E7B8DDull, 0xF9DA1DF355E7B8DDull },
	{ 0x771199D3CA099DCFull, 0x771199D3CA099DCFull },
	{ 0x32EC4B36F23F8C7Bull, 0x32EC4B36F23F8C7Bull },
	{ 0x4F38C18FB91BB182ull, 0x4F38C18FB91BB182ull },
	{ 0x9A71FF7A1EB2FCB5ull, 0x9A71FF7A1EB2FCB5ull },
	{ 0x18669CFA756823F1ull, 0x18669CFA756823F1ull },
	{ 0xC40EBBD75192D935ull, 0xC40EBBD75192D935ull },
	{ 0x309435BBC4EBAD4Bull, 0x309435BBC4EBAD4Bull },
	{ 0x20BAE9A40174EB25ull, 0x20BAE9A40174EB25ull },
	{ 0x43CE493809527CF6ull, 0x43CE493809527CF6ull },
	{ 0x1A79711248D805DDull, 0x1A79711248D805DDull },
	{ 0x363E7CF743D5F608ull, 0x363E7CF743D5F608ull },
	{ 0x7AB41DDDF697B5EFull, 0x7AB41DDDF697B5EFull },
	{ 0x8BADFE358286FA4Aull, 0x8BADFE358286FA4Aull },
	{ 0x74B59AA4D6D465A5ull, 0x74B59AA4D6D465A5ull },
	{ 0xC81A347C59360DB4ull, 0xC81A347C59360DB4ull },
	{ 0x6A453DE085020245ull, 0x6A453DE085020245ull },
	{ 0xE811ACAF83A3410Dull, 0xE811ACAF83A3410Dull },
	{ 0x17EB92B8540F45C0ull, 0x17EB92B8540F45C0ull },
	{ 0xD23AB2C1ADC42C90ull, 0xD23AB2C1ADC42C90ull },
	{ 0x34FE0A062463BA82ull, 0x34FE0A062463BA82ull },
	{ 0xF558DC9CA8474F38ull, 0xF558DC9CA8474F38ull },
	{ 0x6E9B05994FDFF37Eull, 0x6E9B05994FDFF37Eull },
	{ 0x742FB2F64880A78Bull, 0x742FB2F64880A78Bull },
	{ 0xA0E118A0F6CFABDCull, 0xA0E118A0F6CFABDCull },
	{ 0x6C9DCDE873809749ull, 0x6C9DCDE873809749ull },
	{ 0x7F915A2DFFD182B1ull, 0x7F915A2DFFD182B1ull },
	{ 0xFE8B2790E6B38F2Bull, 0xFE8B2790E6B38F2Bull },
	{ 0x5A1E5A13B876409Bull, 0x5A1E5A13B876409Bull },
	{ 0xE86BA14D0DB41B92ull, 0xE86BA14D0DB41B92ull },
	{ 0x656104FE7CAEC455ull, 0x656104FE7CAEC455ull },
	{ 0xB1FB764119C75624ull, 0xB1FB764119C75624ull },
	{ 0xD31DBC6106E31A96ull, 0xD31DBC6106E31A96ull },
	{ 0xEDBAD34B070702EEull, 0xEDBAD34B070702EEull },
	{ 0x5BF138C752D5CEF5ull, 0x5BF138C752D5CEF5ull },
	{ 0x1981459591B01554ull, 0x1981459591B01554ull },
	{ 0xA0D5A73BB6BA1CC9ull, 0xA0D5A73BB6BA1CC9ull },
	{ 0x1D8E06CAE2777E52ull, 0x1D8E06CAE2777E52ull },
	{ 0x17A20B7A4D90F028ull, 0x17A20B7A4D90F028ull },
	{ 0xAC123A01B0831FC2ull, 0xAC123A01B0831FC2ull },
	{ 0x062508FA61FF5587ull, 0x062508FA61FF5587ull },
	{ 0x956DEA6E6C7EA5D2ull, 0x956DEA6E6C7EA5D2ull },
	{ 0x3524DEF38EE841DFull, 0x3524DEF38EE841DFull },
	{ 0x109110B739A41B95ull, 0x109110B739A41B95ull },
	{ 0xAFA088F477C0C6D6ull, 0xAFA088F477C0C6D6ull },
	{ 0x874B0772C833DC4Bull, 0x874B0772C833DC4Bull },
	{ 0x0F857A572D095695ull, 0x0F857A572D095695ull },
	{ 0x8273A2F1EA3DE141ull, 0x8273A2F1EA3DE141ull },
	{ 0x30C2F6E01088B548ull, 0x30C2F6E01088B548ull },
	{ 0x9EC3733AF4D76AF0ull, 0x9EC3733AF4D76AF0ull },
	{ 0xA307F43A40FA1860ull, 0xA307F43A40FA1860ull },
	{ 0xB93727E544874311ull, 0xB93727E544874311ull },
	{ 0x908144EDF2AD272Bull, 0x908144EDF2AD272Bull },
	{ 0x867AEEC6459C1FDCull, 0x867AEEC6459C1FDCull },
	{ 0x0766459B4204F603ull, 0x0766459B4204F603ull },
	{ 0x9CCA42EBD0420960ull, 0x9CCA42EBD0420960ull },
	{ 0x01AF7DE61D6AA0F3ull, 0x01AF7DE61D6AA0F3ull },
	{ 0x092A6042A29F6B74ull, 0x092A6042A29F6B74ull },
	{ 0x2E166E5978AF4C8Aull, 0x2E166E5978AF4C8Aull },
	{ 0x8DC4C6306650CCA2ull, 0x8DC4C6306650CCA2ull },
	{ 0x29FAED8F94B315AFull, 0x29FAED8F94B315AFull },
	{ 0x3EA43EA36EDE13DBull, 0x3EA43EA36EDE13DBull },
	{ 0x3DF4A1254BC39F3Bull, 0x3DF4A1254BC39F3Bull },
	{ 0x2B7872920351F0DDull, 0x2B7872920351F0DDull },
	{ 0x291B9A45DCF9EB89ull, 0x291B9A45DCF9EB89ull },
	{ 0x9BC1CBCB76FB7689ull, 0x9BC1CBCB76FB7689ull },
	{ 0x988B5C3356CF8688ull, 0x988B5C3356CF8688ull },
	{ 0x9CBDC1D48CCB5298ull, 0x9CBDC1D48CCB5298ull },
	{ 0x2EE8BF113D6CAFF0ull, 0x2EE8BF113D6CAFF0ull },
	{ 0x2D0B61CECC718F06ull, 0x2D0B61CECC718F06ull },
	{ 0xA04DD83CDB215AB2ull, 0xA04DD83CDB215AB2ull },
	{ 0x1D6C0D7AF4738E71ull, 0x1D6C0D7AF4738E71ull },
	{ 0x0F47D6A0F1C6C223ull, 0x0F47D6A0F1C6C223ull },
	{ 0x8B10730F409ACBCFull, 0x8B10730F409ACBCFull },
	{ 0xA242B764DA359BB5ull, 0xA242B764DA359BB5ull },
	{ 0xAC1D739FC69D5265ull, 0xAC1D739FC69D5265ull },
	{ 0x1E48D4713E29B904ull, 0x1E48D4713E29B904ull },
	{ 0x8928DA0FFD93A904ull, 0x8928DA0FFD93A904ull },
	{ 0x157A18B4A28F3E13ull, 0x157A18B4A28F3E13ull },
	{ 0x2D7EC387F143684Dull, 0x2D7EC387F143684Dull },
	{ 0xB803631FCA59ED4Dull, 0xB803631FCA59ED4Dull },
	{ 0x322066949217508Aull, 0x322066949217508Aull },
	{ 0x1250AD333C9C6706ull, 0x1250AD333C9C6706ull },
	{ 0x8B108A35C6A396DFull, 0x8B108A35C6A396DFull },
	{ 0x2EF43E040FF04664ull, 0x2EF43E040FF04664ull },
	{ 0x32B0D1341ABE44D3ull, 0x32B0D1341ABE44D3ull },
	{ 0x88F67E7CFCD88E17ull, 0x88F67E7CFCD88E17ull },
	{ 0x83B0E75F4AEB3EC7ull, 0x83B0E75F4AEB3EC7ull },
	{ 0x09FA75E61E4AEF54ull, 0x09FA75E61E4AEF54ull },
	{ 0x08D1F3AE8A374265ull, 0x08D1F3AE8A374265ull },
	{ 0x0F4E517FE97993DDull, 0x0F4E517FE97993DDull },
	{ 0xAD2ADB544AD754CAull, 0xAD2ADB544AD754CAull },
	{ 0x2A712F723AFF42E6ull, 0x2A712F723AFF42E6ull },
	{ 0x8C643811D763E9BFull, 0x8C643811D763E9BFull },
	{ 0x3863BF66257C4457ull, 0x3863BF66257C4457ull },
	{ 0x866D8A46C1C1D47Bull, 0x866D8A46C1C1D47Bull },
	{ 0x23BB139BDC035FDDull, 0x23BB139BDC035FDDull },
	{ 0x8F9364142920EED8ull, 0x8F9364142920EED8ull },
	{ 0x8FB99FDEE0A6BF63ull, 0x8FB99FDEE0A6BF63ull },
	{ 0x29FBE990A5859B0Eull, 0x29FBE990A5859B0Eull },
	{ 0x290D7305C10C752Aull, 0x290D7305C10C752Aull },
	{ 0xA5AE85DA92E0A914ull, 0xA5AE85DA92E0A914ull },
	{ 0x8000000000000577ull, 0x8000000000000577ull },
	{ 0x00000000019AA91Aull, 0x00000000019AA91Aull },
	{ 0x00000000000016DFull, 0x00000000000016DFull },
	{ 0x00000000000000DBull, 0x00000000000000DBull },
	{ 0x80000E0838C74EBBull, 0x80000E0838C74EBBull },
	{ 0x8000000000000077ull, 0x8000000000000077ull },
	{ 0x800000000001141Eull, 0x800000000001141Eull },
	{ 0x800006E9F26ECD66ull, 0x800006E9F26ECD66ull },
	{ 0x0000000003AD81A9ull, 0x0000000003AD81A9ull },
	{ 0x80000000042FB495ull, 0x80000000042FB495ull },
	{ 0x8000E0AFD45E2CD3ull, 0x8000E0AFD45E2CD3ull },
	{ 0x80000000000000E4ull, 0x80000000000000E4ull },
	{ 0x8000000076E98EA1ull, 0x8000000076E98EA1ull },
	{ 0x0000000000000067ull, 0x0000000000000067ull },
	{ 0x8000000000000005ull, 0x8000000000000005ull },
	{ 0x800000000809CDE6ull, 0x800000000809CDE6ull },
	{ 0x000000000647D91Eull, 0x000000000647D91Eull },
	{ 0x0000000000091640ull, 0x0000000000091640ull },
	{ 0x00003FEE414FECA7ull, 0x00003FEE414FECA7ull },
	{ 0x8000000006CC636Aull, 0x8000000006CC636Aull },
	{ 0x00000000001F7C97ull, 0x00000000001F7C97ull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x8000065F8EA9A9A8ull, 0x8000065F8EA9A9A8ull },
	{ 0x00000000000012D6ull, 0x00000000000012D6ull },
	{ 0x8000000000452189ull, 0x8000000000452189ull },
	{ 0x00040D69802EC1E3ull, 0x00040D69802EC1E3ull },
	{ 0x0000000000691F70ull, 0x0000000000691F70ull },
	{ 0x8000001AD0E5C0F8ull, 0x8000001AD0E5C0F8ull },
	{ 0x000003166F733F57ull, 0x000003166F733F57ull },
	{ 0x0000001B88B3E173ull, 0x0000001B88B3E173ull },
	{ 0x0000000000000A37ull, 0x0000000000000A37ull },
	{ 0x000000000000004Eull, 0x000000000000004Eull },
	{ 0x0000001EC97A5624ull, 0x0000001EC97A5624ull },
	{ 0x000000904BC6BC0Full, 0x000000904BC6BC0Full },
	{ 0x80000000022356E1ull, 0x80000000022356E1ull },
	{ 0x00000002EC6DFD11ull, 0x00000002EC6DFD11ull },
	{ 0x00000000003AB52Full, 0x00000000003AB52Full },
	{ 0x800000008C301BACull, 0x800000008C301BACull },
	{ 0x800000000009A17Dull, 0x800000000009A17Dull },
	{ 0x000000294549A362ull, 0x000000294549A362ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x00000027E5601BC9ull, 0x00000027E5601BC9ull },
	{ 0x80079436665C3B0Cull, 0x80079436665C3B0Cull },
	{ 0x8000000000000027ull, 0x8000000000000027ull },
	{ 0x8000000000000017ull, 0x8000000000000017ull },
	{ 0x800000000703979Full, 0x800000000703979Full },
	{ 0x0003FD127856072Dull, 0x0003FD127856072Dull },
	{ 0x0005A2E3841F0467ull, 0x0005A2E3841F0467ull },
	{ 0x8000000000000260ull, 0x8000000000000260ull },
	{ 0x0000000000000449ull, 0x0000000000000449ull },
	{ 0x800000000009DA43ull, 0x800000000009DA43ull },
	{ 0x0000000000DE491Full, 0x0000000000DE491Full },
	{ 0x000000178325FADBull, 0x000000178325FADBull },
	{ 0x800000000009F6CDull, 0x800000000009F6CDull },
	{ 0x0000196A650713E9ull, 0x0000196A650713E9ull },
	{ 0x8000000000662662ull, 0x8000000000662662ull },
	{ 0x8000001BB7ACCC93ull, 0x8000001BB7ACCC93ull },
	{ 0x0000000000000B5Aull, 0x0000000000000B5Aull },
	{ 0x8000000004BD89F2ull, 0x8000000004BD89F2ull },
	{ 0x0000007534319DA4ull, 0x0000007534319DA4ull },
	{ 0x800000C625EFEFD9ull, 0x800000C625EFEFD9ull },
	{ 0x0000000016E1994Full, 0x0000000016E1994Full },
	{ 0x0000000325B9AE76ull, 0x0000000325B9AE76ull },
	{ 0x800000000002DA23ull, 0x800000000002DA23ull },
	{ 0x0000000000164488ull, 0x0000000000164488ull },
	{ 0x80000000000A2119ull, 0x80000000000A2119ull },
	{ 0x000000057BD92A6Dull, 0x000000057BD92A6Dull },
	{ 0x80000C63B951CF29ull, 0x80000C63B951CF29ull },
	{ 0x00077C19CA25C7B8ull, 0x00077C19CA25C7B8ull },
	{ 0x000000000DEC9159ull, 0x000000000DEC9159ull },
	{ 0x80000000000000FAull, 0x80000000000000FAull },
	{ 0x8000003D783DAA21ull, 0x8000003D783DAA21ull },
	{ 0x00000000000000A8ull, 0x00000000000000A8ull },
	{ 0x00000000000001E6ull, 0x00000000000001E6ull },
	{ 0x8005C64DD3F7D2CFull, 0x8005C64DD3F7D2CFull },
	{ 0x000000000000066Dull, 0x000000000000066Dull },
	{ 0x00000000000578AAull, 0x00000000000578AAull },
	{ 0x0000000000A3F965ull, 0x0000000000A3F965ull },
	{ 0x800000000B90DE02ull, 0x800000000B90DE02ull },
	{ 0x8000000000004F30ull, 0x8000000000004F30ull },
	{ 0x000002A306F5B15Aull, 0x000002A306F5B15Aull },
	{ 0x80000000000001E0ull, 0x80000000000001E0ull },
	{ 0x80000000000003A8ull, 0x80000000000003A8ull },
	{ 0x8000000000000007ull, 0x8000000000000007ull },
	{ 0x8000000001B61B74ull, 0x8000000001B61B74ull },
	{ 0x800012E84E0D05CFull, 0x800012E84E0D05CFull },
	{ 0x000000000000004Cull, 0x000000000000004Cull },
	{ 0x800017CEC12E1ADAull, 0x800017CEC12E1ADAull },
	{ 0x80000000000032F4ull, 0x80000000000032F4ull },
	{ 0x0000000000001890ull, 0x0000000000001890ull },
	{ 0x000000000000005Dull, 0x000000000000005Dull },
	{ 0x0000000FBB08DA4Dull, 0x0000000FBB08DA4Dull },
	{ 0x000000000000001Full, 0x000000000000001Full },
	{ 0x8000011CFD8B298Dull, 0x8000011CFD8B298Dull },
	{ 0x80000000007CD81Dull, 0x80000000007CD81Dull },
	{ 0x8000000000BD86E9ull, 0x8000000000BD86E9ull },
	{ 0x000000000CEEBF9Eull, 0x000000000CEEBF9Eull },
	{ 0x8000EB4C1D1E4BD7ull, 0x8000EB4C1D1E4BD7ull },
	{ 0x80000000000001C7ull, 0x80000000000001C7ull },
	{ 0x80000000000000CAull, 0x80000000000000CAull },
	{ 0x4026BF0356F867C5ull, 0x4026BF0356F867C5ull },
	{ 0x5731EAC77A63F5E1ull, 0x5731EAC77A63F5E1ull },
	{ 0xA94CF7C3C3FC1107ull, 0xA94CF7C3C3FC1107ull },
	{ 0x7C5B900A6282944Eull, 0x7C5B900A6282944Eull },
	{ 0x1B715EFABF9A824Full, 0x1B715EFABF9A824Full },
	{ 0xB4BFDE1AABE8EE49ull, 0xB4BFDE1AABE8EE49ull },
	{ 0xDAC94BB09EEF8176ull, 0xDAC94BB09EEF8176ull },
	{ 0xEAB6474E06C69FABull, 0xEAB6474E06C69FABull },
	{ 0x15A397C8A6BD2F36ull, 0x15A397C8A6BD2F36ull },
	{ 0xCEB598B20543FE74ull, 0xCEB598B20543FE74ull },
	{ 0xDCA6CFABED2AC730ull, 0xDCA6CFABED2AC730ull },
	{ 0x4F8F29E8FA14D9C3ull, 0x4F8F29E8FA14D9C3ull },
	{ 0x1A996218396B5DE8ull, 0x1A996218396B5DE8ull },
	{ 0x12140D0AD8B33B60ull, 0x12140D0AD8B33B60ull },
	{ 0x8CB3D032E3CCA0C6ull, 0x8CB3D032E3CCA0C6ull },
	{ 0xE9A43E6A15C22733ull, 0xE9A43E6A15C22733ull },
	{ 0xBA4CDDFFB5ED9995ull, 0xBA4CDDFFB5ED9995ull },
	{ 0x4FC7D57C87985B4Cull, 0x4FC7D57C87985B4Cull },
	{ 0xECFFC1BBC95D6B06ull, 0xECFFC1BBC95D6B06ull },
	{ 0xDC12334844330E5Cull, 0xDC12334844330E5Cull },
	{ 0xA89C541C69A211ACull, 0xA89C541C69A211ACull },
	{ 0x3CBF34348390B2B8ull, 0x3CBF34348390B2B8ull },
	{ 0x23514071F327247Full, 0x23514071F327247Full },
	{ 0x2DCA5EDE7EB3B45Bull, 0x2DCA5EDE7EB3B45Bull },
	{ 0xBD80348EB294935Bull, 0xBD80348EB294935Bull },
	{ 0xF1D013F24ECD3862ull, 0xF1D013F24ECD3862ull },
	{ 0x79CC10D1C68D6900ull, 0x79CC10D1C68D6900ull },
	{ 0x4998BF27CF0FDE90ull, 0x4998BF27CF0FDE90ull },
	{ 0x74E117F5FE568B12ull, 0x74E117F5FE568B12ull },
	{ 0x6C1E06C37AEA80CAull, 0x6C1E06C37AEA80CAull },
	{ 0x5E271F44BA257799ull, 0x5E271F44BA257799ull },
	{ 0x7E89FB1B4F2AF29Aull, 0x7E89FB1B4F2AF29Aull },
	{ 0xEEDDBE8CCAD70700ull, 0xEEDDBE8CCAD70700ull },
	{ 0x0C70473FA988C1AEull, 0x0C70473FA988C1AEull },
	{ 0xD193926345DDB0F3ull, 0xD193926345DDB0F3ull },
	{ 0x82EE31BB8FA7BAE1ull, 0x82EE31BB8FA7BAE1ull },
	{ 0x01D113AACBF6FAA5ull, 0x01D113AACBF6FAA5ull },
	{ 0xA8D8049EF2F5BF1Cull, 0xA8D8049EF2F5BF1Cull },
	{ 0x5EB4FF4D8DFB99D6ull, 0x5EB4FF4D8DFB99D6ull },
	{ 0x18DF66FB76E07C32ull, 0x18DF66FB76E07C32ull },
	{ 0xC60AC05CDC538896ull, 0xC60AC05CDC538896ull },
	{ 0x849F93431B314AF1ull, 0x849F93431B314AF1ull },
	{ 0x5A2E1C589554AAFDull, 0x5A2E1C589554AAFDull },
	{ 0x6F77E5731AA08636ull, 0x6F77E5731AA08636ull },
	{ 0x8FD1C813DD37E699ull, 0x8FD1C813DD37E699ull },
	{ 0xAD27D6FBFA3AD421ull, 0xAD27D6FBFA3AD421ull },
	{ 0xA7B4AECC907DCF7Eull, 0xA7B4AECC907DCF7Eull },
	{ 0x19A5B0796711E397ull, 0x19A5B0796711E397ull },
	{ 0x66FDA20525136043ull, 0x66FDA20525136043ull },
	{ 0x511F38CE17D52D42ull, 0x511F38CE17D52D42ull },
	{ 0xCEC10A028A5EC081ull, 0xCEC10A028A5EC081ull },
	{ 0x67D46563AC12B50Full, 0x67D46563AC12B50Full },
	{ 0x54D0958E08340059ull, 0x54D0958E08340059ull },
	{ 0xA2BC0A2E8C1FC7ADull, 0xA2BC0A2E8C1FC7ADull },
	{ 0x1AD4E99B1F71D8EEull, 0x1AD4E99B1F71D8EEull },
	{ 0x2E828995C969BC93ull, 0x2E828995C969BC93ull },
	{ 0x26E151D169C66F65ull, 0x26E151D169C66F65ull },
	{ 0xD882849EFEAEB4F8ull, 0xD882849EFEAEB4F8ull },
	{ 0x1B41709A7ADC10E9ull, 0x1B41709A7ADC10E9ull },
	{ 0xC6178A7E2EDE5B5Aull, 0xC6178A7E2EDE5B5Aull },
	{ 0xEE90EE53BE6DF6ABull, 0xEE90EE53BE6DF6ABull },
	{ 0xACFAC949A722A5FAull, 0xACFAC949A722A5FAull },
	{ 0x239D2A6458C502DCull, 0x239D2A6458C502DCull },
	{ 0x0AF9BD51DC50935Cull, 0x0AF9BD51DC50935Cull },
	{ 0x0150EA9708DC80B4ull, 0x0150EA9708DC80B4ull },
	{ 0x5E66E1BD4D00ECC9ull, 0x5E66E1BD4D00ECC9ull },
	{ 0xCB2A0AB8052EE76Cull, 0xCB2A0AB8052EE76Cull },
	{ 0xDDA45FA86151054Full, 0xDDA45FA86151054Full },
	{ 0xF8992DB6DDAA96C5ull, 0xF8992DB6DDAA96C5ull },
	{ 0xAF58BF2948CB7ABEull, 0xAF58BF2948CB7ABEull },
	{ 0xED7B6CAC5B4E9882ull, 0xED7B6CAC5B4E9882ull },
	{ 0xA27D96B7DDB8F3ABull, 0xA27D96B7DDB8F3ABull },
	{ 0x31CE0724602F61E8ull, 0x31CE0724602F61E8ull },
	{ 0x66383B3981D8E16Full, 0x66383B3981D8E16Full },
	{ 0x29EC13ABC7238FC2ull, 0x29EC13ABC7238FC2ull },
	{ 0x41FC6FF100C23F27ull, 0x41FC6FF100C23F27ull },
	{ 0xCE562E824418ABE1ull, 0xCE562E824418ABE1ull },
	{ 0x4D7916F26FAC5233ull, 0x4D7916F26FAC5233ull },
	{ 0xE3F42B10440FE696ull, 0xE3F42B10440FE696ull },
	{ 0x289EE0BB0B1F69B9ull, 0x289EE0BB0B1F69B9ull },
	{ 0x33C0C2E0CD42DDB9ull, 0x33C0C2E0CD42DDB9ull },
	{ 0xB370FBED1A0D9EA1ull, 0xB370FBED1A0D9EA1ull },
	{ 0x430F465F5EB056A4ull, 0x430F465F5EB056A4ull },
	{ 0xC6AFC64FCC139498ull, 0xC6AFC64FCC139498ull },
	{ 0x8B6B4BD9CF9101B6ull, 0x8B6B4BD9CF9101B6ull },
	{ 0x6ACD7B9319213810ull, 0x6ACD7B9319213810ull },
	{ 0xB29260A631F43952ull, 0xB29260A631F43952ull },
	{ 0x90BCE637A30A813Cull, 0x90BCE637A30A813Cull },
	{ 0xC2B7B88CAEC4C0DFull, 0xC2B7B88CAEC4C0DFull },
	{ 0xC9BA0F3A94403097ull, 0xC9BA0F3A94403097ull },
	{ 0xE201DDB569B587C8ull, 0xE201DDB569B587C8ull },
	{ 0x7F2CEDC5807091DCull, 0x7F2CEDC5807091DCull },
	{ 0x8B22609A5E56657Bull, 0x8B22609A5E56657Bull },
	{ 0xAAC618024390390Cull, 0xAAC618024390390Cull },
	{ 0xC4D7889D41D0DBEDull, 0xC4D7889D41D0DBEDull },
	{ 0x0E657272E38AC6EEull, 0x0E657272E38AC6EEull },
	{ 0xDA380321624CD712ull, 0xDA380321624CD712ull },
	{ 0xEC5ADFAD6E9BA308ull, 0xEC5ADFAD6E9BA308ull },
	{ 0x927894C9867D6EE7ull, 0x927894C9867D6EE7ull },
	{ 0xBEF0F17BDC4EC229ull, 0xBEF0F17BDC4EC229ull },
	{ 0x693BC7D9CE55A16Aull, 0x693BC7D9CE55A16Aull },
	{ 0x70F1977FED2960E1ull, 0x70F1977FED2960E1ull },
	{ 0xC64B9DBD92E51A1Eull, 0xC64B9DBD92E51A1Eull },
	{ 0xE0DD24951366BAC6ull, 0xE0DD24951366BAC6ull },
	{ 0xD885CA8755BF0A3Bull, 0xD885CA8755BF0A3Bull },
	{ 0xD0A98367B7E0CD80ull, 0xD0A98367B7E0CD80ull },
	{ 0x628A38DE8216EE43ull, 0x628A38DE8216EE43ull },
	{ 0xEE0779C9C946D5A3ull, 0xEE0779C9C946D5A3ull },
	{ 0xDDE46A93B48C5373ull, 0xDDE46A93B48C5373ull },
	{ 0xCBF1AC78030620C7ull, 0xCBF1AC78030620C7ull },
	{ 0xE6956EE915EE452Aull, 0xE6956EE915EE452Aull },
	{ 0xBEE64665728260D7ull, 0xBEE64665728260D7ull },
	{ 0x5832BA5E355E76EEull, 0x5832BA5E355E76EEull },
	{ 0x73101B92BE052042ull, 0x73101B92BE052042ull },
	{ 0xF96B634F6E5D2408ull, 0xF96B634F6E5D2408ull },
	{ 0x4D55F4B9ADA07924ull, 0x4D55F4B9ADA07924ull },
	{ 0xF5D3398CB112FEA0ull, 0xF5D3398CB112FEA0ull },
	{ 0x4D9D4D225F927085ull, 0x4D9D4D225F927085ull },
	{ 0xC9145248B4921E2Cull, 0xC9145248B4921E2Cull },
	{ 0xD0F7D3347D48F240ull, 0xD0F7D3347D48F240ull },
	{ 0x564B69C07045E06Cull, 0x564B69C07045E06Cull },
	{ 0x473EBFC3656D0DDAull, 0x473EBFC3656D0DDAull },
	{ 0xC2AE3E7C0AAFDD99ull, 0xC2AE3E7C0AAFDD99ull },
	{ 0xDE4DA255E451150Full, 0xDE4DA255E451150Full },
	{ 0xF46E0307A03672CBull, 0xF46E0307A03672CBull },
	{ 0x706DDF3B3F5FC491ull, 0x706DDF3B3F5FC491ull },
	{ 0xF62D0F3990E8F2F1ull, 0xF62D0F3990E8F2F1ull },
	{ 0x5F370A3334B4D6F5ull, 0x5F370A3334B4D6F5ull },
	{ 0xDEDB7B16F675FE4Bull, 0xDEDB7B16F675FE4Bull },
	{ 0xC767A54395CCB230ull, 0xC767A54395CCB230ull },
	{ 0xFFA383F1ACEDAEDFull, 0xFFA383F1ACEDAEDFull },
	{ 0xC90FC949FA590B99ull, 0xC90FC949FA590B99ull },
	{ 0xFB8A3CDF58112983ull, 0xFB8A3CDF58112983ull },
	{ 0x4DDE446CD7657FDBull, 0x4DDE446CD7657FDBull },
	{ 0xD25125C9F6327D65ull, 0xD25125C9F6327D65ull },
	{ 0x6E8AF6B982E2A296ull, 0x6E8AF6B982E2A296ull },
	{ 0xD62EA9846EDF2C74ull, 0xD62EA9846EDF2C74ull },
	{ 0xE3916B19EA5B8D4Eull, 0xE3916B19EA5B8D4Eull },
	{ 0xC498520D50DB02E2ull, 0xC498520D50DB02E2ull },
	{ 0xEC8679FD45DD4B45ull, 0xEC8679FD45DD4B45ull },
	{ 0xF127C7B286460CCDull, 0xF127C7B286460CCDull },
	{ 0x5CBE7683430C3EAEull, 0x5CBE7683430C3EAEull },
	{ 0x474E813A26C1D146ull, 0x474E813A26C1D146ull },
	{ 0x525BE4EC9CEA240Dull, 0x525BE4EC9CEA240Dull },
	{ 0xEE3FF96E1B3B1766ull, 0xEE3FF96E1B3B1766ull },
	{ 0x5EA5F89205021DF1ull, 0x5EA5F89205021DF1ull },
	{ 0x5FF17D224E8FE580ull, 0x5FF17D224E8FE580ull },
	{ 0xFAE64FA96720A609ull, 0xFAE64FA96720A609ull },
	{ 0xC22E4A0A05CFBD96ull, 0xC22E4A0A05CFBD96ull },
	{ 0xF94A45915247B969ull, 0xF94A45915247B969ull },
	{ 0x542C9DA625CB06FEull, 0x542C9DA625CB06FEull },
	{ 0x78FEFECABB3C7BC3ull, 0x78FEFECABB3C7BC3ull },
	{ 0x75F5130C14823391ull, 0x75F5130C14823391ull },
	{ 0x41ABE3BB886C2F7Eull, 0x41ABE3BB886C2F7Eull },
	{ 0xCA39BA3FB5034D0Eull, 0xCA39BA3FB5034D0Eull },
	{ 0xDCC3EBF91511ED46ull, 0xDCC3EBF91511ED46ull },
	{ 0xC1020D60B9A19F87ull, 0xC1020D60B9A19F87ull },
	{ 0x604813F0772D342Cull, 0x604813F0772D342Cull },
	{ 0x7E7329A8A20780CDull, 0x7E7329A8A20780CDull },
	{ 0x7AD701B575789B95ull, 0x7AD701B575789B95ull },
	{ 0xE7B8B199360AF504ull, 0xE7B8B199360AF504ull },
	{ 0x5CCD5E0B1AC9586Eull, 0x5CCD5E0B1AC9586Eull },
	{ 0x6B6700373276F3E3ull, 0x6B6700373276F3E3ull },
	{ 0xE4721246C3FCF499ull, 0xE4721246C3FCF499ull },
	{ 0xD4587E8C662B8263ull, 0xD4587E8C662B8263ull },
	{ 0x5AB5308508CB9A61ull, 0x5AB5308508CB9A61ull },
	{ 0xE2D6C84034C38FF4ull, 0xE2D6C84034C38FF4ull },
	{ 0xFF66CDE1CD5C603Bull, 0xFF66CDE1CD5C603Bull },
	{ 0xE70E2A1D39487DEDull, 0xE70E2A1D39487DEDull },
	{ 0x67BF2390DE8D050Full, 0x67BF2390DE8D050Full },
	{ 0x5EFD24DE7B4EE176ull, 0x5EFD24DE7B4EE176ull },
	{ 0xCC9B2B7FD22B8276ull, 0xCC9B2B7FD22B8276ull },
	{ 0x59169E07206518A6ull, 0x59169E07206518A6ull },
	{ 0xF7D9F076614DD8C7ull, 0xF7D9F076614DD8C7ull },
	{ 0xD9EDA9B65669AFFEull, 0xD9EDA9B65669AFFEull },
	{ 0xE29E164B573E7E71ull, 0xE29E164B573E7E71ull },
	{ 0x6955ABDD425332E8ull, 0x6955ABDD425332E8ull },
	{ 0xF8857F3FA63A2D68ull, 0xF8857F3FA63A2D68ull },
	{ 0xE03F7CFE9CC64D73ull, 0xE03F7CFE9CC64D73ull },
	{ 0xD5A613217D28040Full, 0xD5A613217D28040Full },
	{ 0xE1B2FE96142FFA0Full, 0xE1B2FE96142FFA0Full },
	{ 0x7C12C4614614ACEAull, 0x7C12C4614614ACEAull },
	{ 0x753FBEF9F104D070ull, 0x753FBEF9F104D070ull },
	{ 0x5728DB4545EA3A4Dull, 0x5728DB4545EA3A4Dull },
	{ 0xC26785A364F1B0A8ull, 0xC26785A364F1B0A8ull },
	{ 0x57DF294305F53F8Bull, 0x57DF294305F53F8Bull },
	{ 0x7DC56607D2BD3622ull, 0x7DC56607D2BD3622ull },
	{ 0xBEA4181B1C083CFEull, 0xBEA4181B1C083CFEull },
	{ 0x55B4BC0BDE1DF551ull, 0x55B4BC0BDE1DF551ull },
	{ 0x40F452F21553E4CEull, 0x40F452F21553E4CEull },
	{ 0x65A4A5962D9C6280ull, 0x65A4A5962D9C6280ull },
	{ 0xD0B9C7AFE5C0B53Cull, 0xD0B9C7AFE5C0B53Cull },
	{ 0x5F4A2F375E723FCBull, 0x5F4A2F375E723FCBull },
	{ 0x61FF03B1B8B802B6ull, 0x61FF03B1B8B802B6ull },
	{ 0xE2E72311CE047965ull, 0xE2E72311CE047965ull },
	{ 0xFB6D6C11A1F82829ull, 0xFB6D6C11A1F82829ull },
	{ 0x6F7486E66CCC9CA4ull, 0x6F7486E66CCC9CA4ull },
	{ 0xF9DE1E7C3D034CA1ull, 0xF9DE1E7C3D034CA1ull },
	{ 0x59FC147EDFCAF23Bull, 0x59FC147EDFCAF23Bull },
	{ 0xE55D31D6A5D10764ull, 0xE55D31D6A5D10764ull },
	{ 0x561C657510C8A3C2ull, 0x561C657510C8A3C2ull },
	{ 0x6C204F028161801Full, 0x6C204F028161801Full },
	{ 0x525C93BF4B242676ull, 0x525C93BF4B242676ull },
	{ 0xE1DC679EDEAD4F0Dull, 0xE1DC679EDEAD4F0Dull },
	{ 0xD5121E5F28A1C467ull, 0xD5121E5F28A1C467ull },
	{ 0x641663B37CD9F29Eull, 0x641663B37CD9F29Eull },
	{ 0x5A246BF6DF89E653ull, 0x5A246BF6DF89E653ull },
	{ 0x560F0431615809A9ull, 0x560F0431615809A9ull },
	{ 0x4158191AFD0FA9FFull, 0x4158191AFD0FA9FFull },
	{ 0xEBDF2A30E89E1A5Bull, 0xEBDF2A30E89E1A5Bull },
	{ 0xE2ECFC1E02E53977ull, 0xE2ECFC1E02E53977ull },
	{ 0x66FAA6A09047D893ull, 0x66FAA6A09047D893ull },
	{ 0x6C2DE479A1CA3A51ull, 0x6C2DE479A1CA3A51ull },
	{ 0x7A1835FC39256CAAull, 0x7A1835FC39256CAAull },
	{ 0x7DDF8732428B4D54ull, 0x7DDF8732428B4D54ull },
	{ 0xBD4E64F75275212Cull, 0xBD4E64F75275212Cull },
	{ 0xF1F07BEC85FD5AF7ull, 0xF1F07BEC85FD5AF7ull },
	{ 0xE2027F5ED5FFB8C1ull, 0xE2027F5ED5FFB8C1ull },
	{ 0x56B3E8007523E8E2ull, 0x56B3E8007523E8E2ull },
	{ 0xCF45CCEA86946600ull, 0xCF45CCEA86946600ull },
	{ 0x669F8E0B12C16C04ull, 0x669F8E0B12C16C04ull },
	{ 0xEFA0B731CE473A42ull, 0xEFA0B731CE473A42ull },
	{ 0x64B4A0474A0764D6ull, 0x64B4A0474A0764D6ull },
	{ 0xF63F7C63DEDD4F60ull, 0xF63F7C63DEDD4F60ull },
	{ 0xF956A6AEA48AF864ull, 0xF956A6AEA48AF864ull },
	{ 0xC6DF8DCB1797F51Eull, 0xC6DF8DCB1797F51Eull },
	{ 0x44FB34BCE7BA792Full, 0x44FB34BCE7BA792Full },
	{ 0xE434A7C5ACDDAADEull, 0xE434A7C5ACDDAADEull },
	{ 0x744403E34B4028EAull, 0x744403E34B4028EAull },
	{ 0x7063A91AA76D3FA5ull, 0x7063A91AA76D3FA5ull },
	{ 0x51DDC38747A4A4A7ull, 0x51DDC38747A4A4A7ull },
	{ 0x7F2AA641A7240793ull, 0x7F2AA641A7240793ull },
	{ 0xE5C90541F4ABE953ull, 0xE5C90541F4ABE953ull },
	{ 0xFD041C15CDB16A86ull, 0xFD041C15CDB16A86ull },
	{ 0x7B59226BACD26BD1ull, 0x7B59226BACD26BD1ull },
	{ 0xFDC7C411EF478F00ull, 0xFDC7C411EF478F00ull },
	{ 0xC84C8C9D804FEFD2ull, 0xC84C8C9D804FEFD2ull },
	{ 0x66B5CBB2214841D8ull, 0x66B5CBB2214841D8ull },
	{ 0x5D7DD5C4D62B1E94ull, 0x5D7DD5C4D62B1E94ull },
	{ 0xCC3D0335521DCB4Eull, 0xCC3D0335521DCB4Eull },
	{ 0xF9C9D2C87DF2A9F7ull, 0xF9C9D2C87DF2A9F7ull },
	{ 0xCAE48FEEA8277985ull, 0xCAE48FEEA8277985ull },
	{ 0x5F85A15A611FB9C1ull, 0x5F85A15A611FB9C1ull },
	{ 0x4EB6B37DC36550D7ull, 0x4EB6B37DC36550D7ull },
	{ 0x3F289E18A1AFDF1Aull, 0x3F289E18A1AFDF1Aull },
	{ 0xBF06165AA740DCFBull, 0xBF06165AA740DCFBull },
	{ 0xBF767BB8EF44AA92ull, 0xBF767BB8EF44AA92ull },
	{ 0x40BD43D97FF8FFC8ull, 0x40BD43D97FF8FFC8ull },
	{ 0xBFF51F86FE8E0666ull, 0xBFF51F86FE8E0666ull },
	{ 0x41A3507967F099E8ull, 0x41A3507967F099E8ull },
	{ 0x4032F3605EA7AC02ull, 0x4032F3605EA7AC02ull },
	{ 0xBE02564B6BF4E8CEull, 0xBE02564B6BF4E8CEull },
	{ 0x419E614614796E09ull, 0x419E614614796E09ull },
	{ 0xC185872FEB645D04ull, 0xC185872FEB645D04ull },
	{ 0x41965237E43324B1ull, 0x41965237E43324B1ull },
	{ 0xC132E312671922DDull, 0xC132E312671922DDull },
	{ 0xC133B4EDB5C91857ull, 0xC133B4EDB5C91857ull },
	{ 0xC16CF6A35D67653Cull, 0xC16CF6A35D67653Cull },
	{ 0xBF1E90A593E4885Aull, 0xBF1E90A593E4885Aull },
	{ 0xC06B9C86B5EA02D4ull, 0xC06B9C86B5EA02D4ull },
	{ 0x406070BC60E28520ull, 0x406070BC60E28520ull },
	{ 0x3F4DC21BBD58A3C3ull, 0x3F4DC21BBD58A3C3ull },
	{ 0xBD8A35164F235552ull, 0xBD8A35164F235552ull },
	{ 0xBF4B0C04AE2486E2ull, 0xBF4B0C04AE2486E2ull },
	{ 0xC0D426430941AD7Cull, 0xC0D426430941AD7Cull },
	{ 0xC1ADBBA5FBB0ECB4ull, 0xC1ADBBA5FBB0ECB4ull },
	{ 0x3F977F46CA0D9E1Aull, 0x3F977F46CA0D9E1Aull },
	{ 0x3DD472A604EA4D2Eull, 0x3DD472A604EA4D2Eull },
	{ 0xC00435589B0BCA47ull, 0xC00435589B0BCA47ull },
	{ 0x40790523EBDC03B4ull, 0x40790523EBDC03B4ull },
	{ 0xBEDA5FF81963E67Dull, 0xBEDA5FF81963E67Dull },
	{ 0x402C94D819B88801ull, 0x402C94D819B88801ull },
	{ 0xC0455326F394C1DAull, 0xC0455326F394C1DAull },
	{ 0xBD88CCC5301B88ADull, 0xBD88CCC5301B88ADull },
	{ 0xC12E5C3D14C1E52Bull, 0xC12E5C3D14C1E52Bull },
	{ 0xC032E9056B3D2D6Cull, 0xC032E9056B3D2D6Cull },
	{ 0xBF30D65C29149628ull, 0xBF30D65C29149628ull },
	{ 0xC0FBE08BF5B25F1Bull, 0xC0FBE08BF5B25F1Bull },
	{ 0xBE777221F7B2E5F8ull, 0xBE777221F7B2E5F8ull },
	{ 0xC00EF624CB86FD00ull, 0xC00EF624CB86FD00ull },
	{ 0xBEC54A28C04962AEull, 0xBEC54A28C04962AEull },
	{ 0x3EFB46489CC5637Bull, 0x3EFB46489CC5637Bull },
	{ 0x3DEA660C6744AC0Eull, 0x3DEA660C6744AC0Eull },
	{ 0x412F96B8550AA78Bull, 0x412F96B8550AA78Bull },
	{ 0x3F690B80764C83FEull, 0x3F690B80764C83FEull },
	{ 0xBF04E61EFE3B1AB0ull, 0xBF04E61EFE3B1AB0ull },
	{ 0x3EF784FB0D7B108Aull, 0x3EF784FB0D7B108Aull },
	{ 0x403CC3F4ACF23746ull, 0x403CC3F4ACF23746ull },
	{ 0xC0F747DD03F7D9F2ull, 0xC0F747DD03F7D9F2ull },
	{ 0x42B77776AD205D81ull, 0x42B77776AD205D81ull },
	{ 0x40B0EEFBB624FD36ull, 0x40B0EEFBB624FD36ull },
	{ 0xBEF2D08D3A50014Cull, 0xBEF2D08D3A50014Cull },
	{ 0xC213852329BE50CDull, 0xC213852329BE50CDull },
	{ 0x3EC0AE0ABA5FAE97ull, 0x3EC0AE0ABA5FAE97ull },
	{ 0x3EA1582CD5FE1C5Cull, 0x3EA1582CD5FE1C5Cull },
	{ 0x3DF0B43ACB08A4A2ull, 0x3DF0B43ACB08A4A2ull },
	{ 0xBE41D02F50F331BFull, 0xBE41D02F50F331BFull },
	{ 0x3EE2C177A96A996Aull, 0x3EE2C177A96A996Aull },
	{ 0x3EF8624F5188E138ull, 0x3EF8624F5188E138ull },
	{ 0x40FBBF3089420BD5ull, 0x40FBBF3089420BD5ull },
	{ 0xBFED582445C334E4ull, 0xBFED582445C334E4ull },
	{ 0xC06FC6B624CA7677ull, 0xC06FC6B624CA7677ull },
	{ 0x3F3B4353F3465843ull, 0x3F3B4353F3465843ull },
	{ 0x41FAB0D72028B92Cull, 0x41FAB0D72028B92Cull },
	{ 0xBE78AA2EB7BDC220ull, 0xBE78AA2EB7BDC220ull },
	{ 0x3E58A02D943A972Aull, 0x3E58A02D943A972Aull },
	{ 0x3FA364C8EEB6340Full, 0x3FA364C8EEB6340Full },
	{ 0xBEDD907A8FEF2101ull, 0xBEDD907A8FEF2101ull },
	{ 0x3FBF577DBB6D5851ull, 0x3FBF577DBB6D5851ull },
	{ 0xC012A93FB2BB358Cull, 0xC012A93FB2BB358Cull },
	{ 0xC056EB1E2C22B27Full, 0xC056EB1E2C22B27Full },
	{ 0x3DD67C0EA84EDA6Dull, 0x3DD67C0EA84EDA6Dull },
	{ 0x3F5A684E606F01ACull, 0x3F5A684E606F01ACull },
	{ 0x41965733EB58825Cull, 0x41965733EB58825Cull },
	{ 0x3E9AE49223437A0Bull, 0x3E9AE49223437A0Bull },
	{ 0x3E254545CAE545F1ull, 0x3E254545CAE545F1ull },
	{ 0x3EAE10E1883B9B3Full, 0x3EAE10E1883B9B3Full },
	{ 0x41273A5CA7BBE316ull, 0x41273A5CA7BBE316ull },
	{ 0xC15E08060E17AB18ull, 0xC15E08060E17AB18ull },
	{ 0x429A2FC5C1234AE9ull, 0x429A2FC5C1234AE9ull },
	{ 0xBFE80AE42C518FD4ull, 0xBFE80AE42C518FD4ull },
	{ 0xC19F4E9EB8CDC189ull, 0xC19F4E9EB8CDC189ull },
	{ 0xC1C1F6AC26C8E2E0ull, 0xC1C1F6AC26C8E2E0ull },
	{ 0x4196D9684CD2B901ull, 0x4196D9684CD2B901ull },
	{ 0xC198A91B066A1F74ull, 0xC198A91B066A1F74ull },
	{ 0x404BEFF34F7C3469ull, 0x404BEFF34F7C3469ull },
	{ 0x424E7D801E002E84ull, 0x424E7D801E002E84ull },
	{ 0x3FFF4378646DBF40ull, 0x3FFF4378646DBF40ull },
	{ 0x432371000126C62Dull, 0x432371000126C62Dull },
	{ 0x42C81855ABD2867Cull, 0x42C81855ABD2867Cull },
	{ 0xC203C961494F59A4ull, 0xC203C961494F59A4ull },
	{ 0x4119A9619C53D7D3ull, 0x4119A9619C53D7D3ull },
	{ 0x420F6FF230DCDF9Dull, 0x420F6FF230DCDF9Dull },
	{ 0x42B9475551391AA4ull, 0x42B9475551391AA4ull },
	{ 0x8000000000001058ull, 0x8000000000001058ull },
	{ 0x000000000050F83Dull, 0x000000000050F83Dull },
	{ 0x800000000000000Aull, 0x800000000000000Aull },
	{ 0x000000000097CB65ull, 0x000000000097CB65ull },
	{ 0x8003AD9E4E2C68AAull, 0x8003AD9E4E2C68AAull },
	{ 0x0000000000000114ull, 0x0000000000000114ull },
	{ 0x0000000000000082ull, 0x0000000000000082ull },
	{ 0x800000000000000Eull, 0x800000000000000Eull },
	{ 0x0000000000AE5B26ull, 0x0000000000AE5B26ull },
	{ 0x0000000000018F36ull, 0x0000000000018F36ull },
	{ 0x00062F3A0E6049D1ull, 0x00062F3A0E6049D1ull },
	{ 0x0000000FEBFCBD2Bull, 0x0000000FEBFCBD2Bull },
	{ 0x800000000000B0F4ull, 0x800000000000B0F4ull },
	{ 0x8000FF49301F91E6ull, 0x8000FF49301F91E6ull },
	{ 0x0000000000000005ull, 0x0000000000000005ull },
	{ 0x00000000000B3D34ull, 0x00000000000B3D34ull },
	{ 0x0002884DCB6A28D0ull, 0x0002884DCB6A28D0ull },
	{ 0x8000000086D75388ull, 0x8000000086D75388ull },
	{ 0x8000000000019AABull, 0x8000000000019AABull },
	{ 0x800000000000000Full, 0x800000000000000Full },
	{ 0x800000005D13055Aull, 0x800000005D13055Aull },
	{ 0x8000000000000A52ull, 0x8000000000000A52ull },
	{ 0x8006A50E14E6275Cull, 0x8006A50E14E6275Cull },
	{ 0x00000013CC323849ull, 0x00000013CC323849ull },
	{ 0x800000003A5926F6ull, 0x800000003A5926F6ull },
	{ 0x80000000C05AA880ull, 0x80000000C05AA880ull },
	{ 0x00000002E4E75B57ull, 0x00000002E4E75B57ull },
	{ 0x80000000000868F7ull, 0x80000000000868F7ull },
	{ 0x800000101F75E560ull, 0x800000101F75E560ull },
	{ 0x8000000FB29654AEull, 0x8000000FB29654AEull },
	{ 0x0000000003AE70EFull, 0x0000000003AE70EFull },
	{ 0x000000002E1D4635ull, 0x000000002E1D4635ull },
	{ 0x0000000000002EA5ull, 0x0000000000002EA5ull },
	{ 0x00000039CF4B90F1ull, 0x00000039CF4B90F1ull },
	{ 0x8000000000000ECEull, 0x8000000000000ECEull },
	{ 0x8000000091BBBEE7ull, 0x8000000091BBBEE7ull },
	{ 0x000000000000130Cull, 0x000000000000130Cull },
	{ 0x8000000608235BD7ull, 0x8000000608235BD7ull },
	{ 0x00003184C1A4A0A2ull, 0x00003184C1A4A0A2ull },
	{ 0x00000000002CDCF2ull, 0x00000000002CDCF2ull },
	{ 0x800003146049E5C6ull, 0x800003146049E5C6ull },
	{ 0x00000003CFBA48B2ull, 0x00000003CFBA48B2ull },
	{ 0x0000000000BE41DCull, 0x0000000000BE41DCull },
	{ 0x000011A84A2BCA65ull, 0x000011A84A2BCA65ull },
	{ 0x00000000001CF732ull, 0x00000000001CF732ull },
	{ 0x8000000054231040ull, 0x8000000054231040ull },
	{ 0x00001D62BDC79D4Cull, 0x00001D62BDC79D4Cull },
	{ 0x80000000003341A3ull, 0x80000000003341A3ull },
	{ 0x0000008061EE394Bull, 0x0000008061EE394Bull },
	{ 0x8000000045074245ull, 0x8000000045074245ull },
	{ 0x00000000ACE9F1C6ull, 0x00000000ACE9F1C6ull },
	{ 0x80000079E6B5D83Bull, 0x80000079E6B5D83Bull },
	{ 0x80000000000001D1ull, 0x80000000000001D1ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x0000000000000619ull, 0x0000000000000619ull },
	{ 0x800047AB8CE3AF3Cull, 0x800047AB8CE3AF3Cull },
	{ 0x8000000000000F54ull, 0x8000000000000F54ull },
	{ 0x8000000017F27C04ull, 0x8000000017F27C04ull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x80000000002B016Eull, 0x80000000002B016Eull },
	{ 0x8000000004AE73BDull, 0x8000000004AE73BDull },
	{ 0x80000000006CBDBDull, 0x80000000006CBDBDull },
	{ 0x000000000001D495ull, 0x000000000001D495ull },
	{ 0x800000000002FDFEull, 0x800000000002FDFEull },
	{ 0x80004C2A63AD6F60ull, 0x80004C2A63AD6F60ull },
	{ 0x800000E7AB819760ull, 0x800000E7AB819760ull },
	{ 0x800000000000003Bull, 0x800000000000003Bull },
	{ 0x000000000015C42Bull, 0x000000000015C42Bull },
	{ 0x80007297231CF225ull, 0x80007297231CF225ull },
	{ 0x0002DEF0CE737634ull, 0x0002DEF0CE737634ull },
	{ 0x00000000000A5257ull, 0x00000000000A5257ull },
	{ 0x80000000000002D5ull, 0x80000000000002D5ull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x00000110978FFB5Aull, 0x00000110978FFB5Aull },
	{ 0x80000000119F9C16ull, 0x80000000119F9C16ull },
	{ 0x8000000000000D95ull, 0x8000000000000D95ull },
	{ 0x0000000000020F1Aull, 0x0000000000020F1Aull },
	{ 0x0004759D65CB393Full, 0x0004759D65CB393Full },
	{ 0x000000DC65A8F3A4ull, 0x000000DC65A8F3A4ull },
	{ 0x0000000000003CE0ull, 0x0000000000003CE0ull },
	{ 0x00000000000545C0ull, 0x00000000000545C0ull },
	{ 0x80000000000B4AA7ull, 0x80000000000B4AA7ull },
	{ 0x80000C1C29FBF2A4ull, 0x80000C1C29FBF2A4ull },
	{ 0x0000000000000101ull, 0x0000000000000101ull },
	{ 0x800000000000B988ull, 0x800000000000B988ull },
	{ 0x00000000000F472Eull, 0x00000000000F472Eull },
	{ 0x00000F72229BA88Full, 0x00000F72229BA88Full },
	{ 0x0000000000000017ull, 0x0000000000000017ull },
	{ 0x800000000006917Dull, 0x800000000006917Dull },
	{ 0x8000000000000434ull, 0x8000000000000434ull },
	{ 0x000147C7612E0C02ull, 0x000147C7612E0C02ull },
	{ 0x0001C79F104A2318ull, 0x0001C79F104A2318ull },
	{ 0x8000000000000421ull, 0x8000000000000421ull },
	{ 0x8000002519BA8E13ull, 0x8000002519BA8E13ull },
	{ 0x000000000128FF95ull, 0x000000000128FF95ull },
	{ 0x800000003BBBF9E5ull, 0x800000003BBBF9E5ull },
	{ 0x000000003C7F1656ull, 0x000000003C7F1656ull },
	{ 0x0000000001EAEEF8ull, 0x0000000001EAEEF8ull },
	{ 0x8001E8D05885A92Full, 0x8001E8D05885A92Full },
	{ 0x0000000000000006ull, 0x0000000000000006ull },
	{ 0x8000000091DF26AFull, 0x8000000091DF26AFull },
	{ 0x8000000000001528ull, 0x8000000000001528ull },
	{ 0x8000003A8F0FDF30ull, 0x8000003A8F0FDF30ull },
	{ 0x80000006A40E1255ull, 0x80000006A40E1255ull },
	{ 0x0000000000000024ull, 0x0000000000000024ull },
	{ 0x00000000000001F4ull, 0x00000000000001F4ull },
	{ 0x0000000B806A3587ull, 0x0000000B806A3587ull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x0000000347FB5DC9ull, 0x0000000347FB5DC9ull },
	{ 0x000001CDC24DC993ull, 0x000001CDC24DC993ull },
	{ 0x80000000028C911Dull, 0x80000000028C911Dull },
	{ 0x800000000014477Eull, 0x800000000014477Eull },
	{ 0x00000000000000DDull, 0x00000000000000DDull },
	{ 0x00000000001EDEAFull, 0x00000000001EDEAFull },
	{ 0x800000FA3C6917C8ull, 0x800000FA3C6917C8ull },
	{ 0x000000000D5ADDF0ull, 0x000000000D5ADDF0ull },
	{ 0x8000000007E79B4Dull, 0x8000000007E79B4Dull },
	{ 0x0000000029CF4577ull, 0x0000000029CF4577ull },
	{ 0x8000000000064989ull, 0x8000000000064989ull },
	{ 0x80000000000008ABull, 0x80000000000008ABull },
	{ 0x0000000000000011ull, 0x0000000000000011ull },
	{ 0x800064E47E4A645Cull, 0x800064E47E4A645Cull },
	{ 0x000000011E9CF0E1ull, 0x000000011E9CF0E1ull },
	{ 0x800000000000C05Bull, 0x800000000000C05Bull },
	{ 0x0000000000063B96ull, 0x0000000000063B96ull },
	{ 0x0000003DE9B12B6Aull, 0x0000003DE9B12B6Aull },
	{ 0x800000001083F4FFull, 0x800000001083F4FFull },
	{ 0x8000000065DEFB5Aull, 0x8000000065DEFB5Aull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x0000009F05571856ull, 0x0000009F05571856ull },
	{ 0x000000000000000Bull, 0x000000000000000Bull },
	{ 0x8000A5460CAD01EFull, 0x8000A5460CAD01EFull },
	{ 0x0000000000000072ull, 0x0000000000000072ull },
	{ 0x800000000527E7DEull, 0x800000000527E7DEull },
	{ 0x8000000000000008ull, 0x8000000000000008ull },
	{ 0x0000000025F9E344ull, 0x0000000025F9E344ull },
	{ 0x800000FD042E7F20ull, 0x800000FD042E7F20ull },
	{ 0x8000C5B5B8FF361Cull, 0x8000C5B5B8FF361Cull },
	{ 0x800000000000134Eull, 0x800000000000134Eull },
	{ 0x000000000001ABCFull, 0x000000000001ABCFull },
	{ 0x000000000048F264ull, 0x000000000048F264ull },
	{ 0x8000000EBDCC278Dull, 0x8000000EBDCC278Dull },
	{ 0x800000000894599Bull, 0x800000000894599Bull },
	{ 0x8000000000000B98ull, 0x8000000000000B98ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x8000000000001ED1ull, 0x8000000000001ED1ull },
	{ 0x000E28DEB65085A2ull, 0x000E28DEB65085A2ull },
	{ 0x0000000000000010ull, 0x0000000000000010ull },
	{ 0x000000001862A486ull, 0x000000001862A486ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x80000026EBF55E0Eull, 0x80000026EBF55E0Eull },
	{ 0x8000002CF4CE690Full, 0x8000002CF4CE690Full },
	{ 0x000000000000000Aull, 0x000000000000000Aull },
	{ 0x000000000000009Dull, 0x000000000000009Dull },
	{ 0x8000000001B5067Full, 0x8000000001B5067Full },
	{ 0x8000000000000040ull, 0x8000000000000040ull },
	{ 0x00000360E398A24Aull, 0x00000360E398A24Aull },
	{ 0x8001E1B744C35CF8ull, 0x8001E1B744C35CF8ull },
	{ 0x00000AEC565498D6ull, 0x00000AEC565498D6ull },
	{ 0x8000C7045CE11540ull, 0x8000C7045CE11540ull },
	{ 0x0004D69E48478331ull, 0x0004D69E48478331ull },
	{ 0x800038093BED09B4ull, 0x800038093BED09B4ull },
	{ 0x800000001C9A020Eull, 0x800000001C9A020Eull },
	{ 0x0000000038E5B692ull, 0x0000000038E5B692ull },
	{ 0x80008996EAE46244ull, 0x80008996EAE46244ull },
	{ 0x8000000000000076ull, 0x8000000000000076ull },
	{ 0x80000000B8B5DFE8ull, 0x80000000B8B5DFE8ull },
	{ 0x800000ABB5659344ull, 0x800000ABB5659344ull },
	{ 0x0000000072D98434ull, 0x0000000072D98434ull },
	{ 0x8000000000B24F2Cull, 0x8000000000B24F2Cull },
	{ 0x00000570A0046D88ull, 0x00000570A0046D88ull },
	{ 0x800001E5128BA2A0ull, 0x800001E5128BA2A0ull },
	{ 0x00000000061A55E3ull, 0x00000000061A55E3ull },
	{ 0x800001A5A659D8BBull, 0x800001A5A659D8BBull },
	{ 0x80000000041536C7ull, 0x80000000041536C7ull },
	{ 0x8000000BF13A1E90ull, 0x8000000BF13A1E90ull },
	{ 0x00000000003A6A78ull, 0x00000000003A6A78ull },
	{ 0x000000000000003Cull, 0x000000000000003Cull },
	{ 0x0000000000000029ull, 0x0000000000000029ull },
	{ 0x000000003F5DC697ull, 0x000000003F5DC697ull },
	{ 0x000000000002E7FDull, 0x000000000002E7FDull },
	{ 0x000000C9AB739315ull, 0x000000C9AB739315ull },
	{ 0x8000000DDCFBBBB6ull, 0x8000000DDCFBBBB6ull },
	{ 0x0000000000000007ull, 0x0000000000000007ull },
	{ 0x000000012B65BF42ull, 0x000000012B65BF42ull },
	{ 0x800000000103FF2Full, 0x800000000103FF2Full },
	{ 0x00000000012E4E62ull, 0x00000000012E4E62ull },
	{ 0x00000000000324C0ull, 0x00000000000324C0ull },
	{ 0x8000005EE1B5988Full, 0x8000005EE1B5988Full },
	{ 0x0000000000000275ull, 0x0000000000000275ull },
	{ 0x00000000010DDEC9ull, 0x00000000010DDEC9ull },
	{ 0x8000000000000015ull, 0x8000000000000015ull },
	{ 0x0000000000000C9Dull, 0x0000000000000C9Dull },
	{ 0x000000000012B528ull, 0x000000000012B528ull },
	{ 0x000000000000000Cull, 0x000000000000000Cull },
	{ 0x800000031C75986Eull, 0x800000031C75986Eull },
	{ 0x8000000000000F3Full, 0x8000000000000F3Full },
	{ 0x8000000000118BD3ull, 0x8000000000118BD3ull },
	{ 0x0000000000000A41ull, 0x0000000000000A41ull },
	{ 0x800D9A11AA6A400Bull, 0x800D9A11AA6A400Bull },
	{ 0x000000009F10DFF2ull, 0x000000009F10DFF2ull },
	{ 0x8000000000000048ull, 0x8000000000000048ull },
	{ 0x000001B407FE6504ull, 0x000001B407FE6504ull },
	{ 0x8000000295C1FC28ull, 0x8000000295C1FC28ull },
	{ 0x8000000E40DF75D7ull, 0x8000000E40DF75D7ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x80000000000000D9ull, 0x80000000000000D9ull },
	{ 0x0000000000B04D59ull, 0x0000000000B04D59ull },
	{ 0x00008C5CC2C8BB6Dull, 0x00008C5CC2C8BB6Dull },
	{ 0x800000000000000Eull, 0x800000000000000Eull },
	{ 0x00000019ADFC8F90ull, 0x00000019ADFC8F90ull },
	{ 0x0000000000BBBAD8ull, 0x0000000000BBBAD8ull },
	{ 0x0000000000000397ull, 0x0000000000000397ull },
	{ 0x000C00BDB69BB76Aull, 0x000C00BDB69BB76Aull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x80000001F8A2F366ull, 0x80000001F8A2F366ull },
	{ 0x8000000D119BD779ull, 0x8000000D119BD779ull },
	{ 0x800000000000000Eull, 0x800000000000000Eull },
	{ 0x8000000000014CE8ull, 0x8000000000014CE8ull },
	{ 0x800000000000002Dull, 0x800000000000002Dull },
	{ 0x0000001A3DF52890ull, 0x0000001A3DF52890ull },
	{ 0x0000000006006659ull, 0x0000000006006659ull },
	{ 0x8005A730999DFF83ull, 0x8005A730999DFF83ull },
	{ 0x800000036686F8E4ull, 0x800000036686F8E4ull },
	{ 0x800006C5321DFBFAull, 0x800006C5321DFBFAull },
	{ 0x000B77BA9477CED3ull, 0x000B77BA9477CED3ull },
	{ 0x00010599D8ECEA57ull, 0x00010599D8ECEA57ull },
	{ 0x0000175147CCA07Full, 0x0000175147CCA07Full },
	{ 0x800000004D0F1A1Aull, 0x800000004D0F1A1Aull },
	{ 0x00000000D01F7A58ull, 0x00000000D01F7A58ull },
	{ 0x80000091CB28B352ull, 0x80000091CB28B352ull },
	{ 0x8000000000AA715Dull, 0x8000000000AA715Dull },
	{ 0x0002C892054531A4ull, 0x0002C892054531A4ull },
	{ 0x0000000008CE899Cull, 0x0000000008CE899Cull },
	{ 0x0000000040283EF8ull, 0x0000000040283EF8ull },
	{ 0x8000004C927676E9ull, 0x8000004C927676E9ull },
	{ 0x800000079B4F512Full, 0x800000079B4F512Full },
	{ 0x0000000A929BFC51ull, 0x0000000A929BFC51ull },
	{ 0x80000000001CA414ull, 0x80000000001CA414ull },
	{ 0x0000BA5BE109DBB6ull, 0x0000BA5BE109DBB6ull },
	{ 0x8000006E8B24F295ull, 0x8000006E8B24F295ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x8000000001272038ull, 0x8000000001272038ull },
	{ 0x0000000000007CBCull, 0x0000000000007CBCull },
	{ 0x0000012B508FC86Cull, 0x0000012B508FC86Cull },
	{ 0x80000000000000AAull, 0x80000000000000AAull },
	{ 0x8000000000000018ull, 0x8000000000000018ull },
	{ 0x000000000000002Dull, 0x000000000000002Dull },
	{ 0x000000003F50902Dull, 0x000000003F50902Dull },
	{ 0x0000D4A470056051ull, 0x0000D4A470056051ull },
	{ 0x00000000000005B0ull, 0x00000000000005B0ull },
	{ 0x80092E7F39E3083Cull, 0x80092E7F39E3083Cull },
	{ 0x800000000410811Cull, 0x800000000410811Cull },
	{ 0x80000F96A851E3EBull, 0x80000F96A851E3EBull },
	{ 0x000001CFF2F07447ull, 0x000001CFF2F07447ull },
	{ 0x800ABF1BC2854460ull, 0x800ABF1BC2854460ull },
	{ 0x80032701F0D1A11Eull, 0x80032701F0D1A11Eull },
	{ 0x80000001906502F9ull, 0x80000001906502F9ull },
	{ 0x00068D6C22F2814Cull, 0x00068D6C22F2814Cull },
	{ 0x000000000000000Eull, 0x000000000000000Eull },
	{ 0x800000000750FE30ull, 0x800000000750FE30ull },
	{ 0x00000000000001AEull, 0x00000000000001AEull },
	{ 0x800002DE3BF9BF4Cull, 0x800002DE3BF9BF4Cull },
	{ 0x800000000DF5A44Dull, 0x800000000DF5A44Dull },
	{ 0x80000000003936F3ull, 0x80000000003936F3ull },
	{ 0x0000000000AA9F90ull, 0x0000000000AA9F90ull },
	{ 0x0000000AFA64F531ull, 0x0000000AFA64F531ull },
	{ 0x0000078ED0BF92B7ull, 0x0000078ED0BF92B7ull },
	{ 0x000059E02B2E9C2Eull, 0x000059E02B2E9C2Eull },
	{ 0x800000000007979Aull, 0x800000000007979Aull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x00000000C21532B2ull, 0x00000000C21532B2ull },
	{ 0x000000000007B3FCull, 0x000000000007B3FCull },
	{ 0x80005844F0704CE3ull, 0x80005844F0704CE3ull },
	{ 0x000000000E3952E7ull, 0x000000000E3952E7ull },
	{ 0x00000E6CD8DF1B93ull, 0x00000E6CD8DF1B93ull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x000001E56E7E2D3Full, 0x000001E56E7E2D3Full },
	{ 0x0000134DDBDB4FD8ull, 0x0000134DDBDB4FD8ull },
	{ 0x0000000000000563ull, 0x0000000000000563ull },
	{ 0x80000000126B051Cull, 0x80000000126B051Cull },
	{ 0x8000000000001736ull, 0x8000000000001736ull },
	{ 0x000000000000068Eull, 0x000000000000068Eull },
	{ 0x8000000000008146ull, 0x8000000000008146ull },
	{ 0x8000000000000027ull, 0x8000000000000027ull },
	{ 0x000000000001C9FEull, 0x000000000001C9FEull },
	{ 0x8000000DACD2EDA0ull, 0x8000000DACD2EDA0ull },
	{ 0x80000001D097773Bull, 0x80000001D097773Bull },
	{ 0x000013EA3250E08Eull, 0x000013EA3250E08Eull },
	{ 0x800000177643D0A2ull, 0x800000177643D0A2ull },
	{ 0x0000036311AB1C05ull, 0x0000036311AB1C05ull },
	{ 0x0000000000000017ull, 0x0000000000000017ull },
	{ 0x80000000013CF620ull, 0x80000000013CF620ull },
	{ 0x000000000FA5A3F3ull, 0x000000000FA5A3F3ull },
	{ 0x800000017B05B2D5ull, 0x800000017B05B2D5ull },
	{ 0x800477F44D5AE9A6ull, 0x800477F44D5AE9A6ull },
	{ 0x80007E1CC1D83AD5ull, 0x80007E1CC1D83AD5ull },
	{ 0x00000000005E24D1ull, 0x00000000005E24D1ull },
	{ 0x800000000000001Aull, 0x800000000000001Aull },
	{ 0x80002EE806BE1224ull, 0x80002EE806BE1224ull },
	{ 0x800000000069CC5Dull, 0x800000000069CC5Dull },
	{ 0x000000000000000Eull, 0x000000000000000Eull },
	{ 0x8000000002346671ull, 0x8000000002346671ull },
	{ 0x0001A1C22A8407B8ull, 0x0001A1C22A8407B8ull },
	{ 0x8000000000000027ull, 0x8000000000000027ull },
	{ 0x0000000FA574F5E0ull, 0x0000000FA574F5E0ull },
	{ 0x8000000000000066ull, 0x8000000000000066ull },
	{ 0x0000000C4CDE4E89ull, 0x0000000C4CDE4E89ull },
	{ 0x00000000000003BDull, 0x00000000000003BDull },
	{ 0x000000000169D892ull, 0x000000000169D892ull },
	{ 0x800000000000001Full, 0x800000000000001Full },
	{ 0x0000000002E699E5ull, 0x0000000002E699E5ull },
	{ 0x000000000003F481ull, 0x000000000003F481ull },
	{ 0x0000901BED3866F6ull, 0x0000901BED3866F6ull },
	{ 0x00000000A4B0A5B7ull, 0x00000000A4B0A5B7ull },
	{ 0x0000000000000B61ull, 0x0000000000000B61ull },
	{ 0x00000F3385EB831Eull, 0x00000F3385EB831Eull },
	{ 0x0001740091F0175Aull, 0x0001740091F0175Aull },
	{ 0x0000018BC72B707Eull, 0x0000018BC72B707Eull },
	{ 0x8000000002D0C319ull, 0x8000000002D0C319ull },
	{ 0x8000006024C20E94ull, 0x8000006024C20E94ull },
	{ 0x00000000AA560C19ull, 0x00000000AA560C19ull },
	{ 0x800007EA1936D3F7ull, 0x800007EA1936D3F7ull },
	{ 0x0000000000000004ull, 0x0000000000000004ull },
	{ 0x800CCD4212E8B310ull, 0x800CCD4212E8B310ull },
	{ 0x80000000193DEE5Dull, 0x80000000193DEE5Dull },
	{ 0x00000CA850522375ull, 0x00000CA850522375ull },
	{ 0x00000000003A6225ull, 0x00000000003A6225ull },
	{ 0x0000000000000632ull, 0x0000000000000632ull },
	{ 0x000000000000000Aull, 0x000000000000000Aull },
	{ 0x800027875839F473ull, 0x800027875839F473ull },
	{ 0x8000000000001DCEull, 0x8000000000001DCEull },
	{ 0x000008CE8E3E61AFull, 0x000008CE8E3E61AFull },
	{ 0x000000003E75AAB4ull, 0x000000003E75AAB4ull },
	{ 0x8000000000000050ull, 0x8000000000000050ull }
};

CShader5xTest_DoubleArith::Binary64Vec2 const CShader5xTest_dmul::s_DMulSrc1[] = 
{
	{ 0x3FF0000000000000ull, 0x3FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x7FF3F89B51837A9Dull, 0x7FF3F89B51837A9Dull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xB7854F713C689E44ull, 0xB7854F713C689E44ull },
	{ 0x7FF9436DD07BF2E5ull, 0x7FF9436DD07BF2E5ull },
	{ 0x4F57F53BE90A6D77ull, 0x4F57F53BE90A6D77ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xBCAEA0E7896C803Dull, 0xBCAEA0E7896C803Dull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xDDF4900A52A14BA1ull, 0xDDF4900A52A14BA1ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x351E5198B30372FDull, 0x351E5198B30372FDull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x5744B484274AA4EEull, 0x5744B484274AA4EEull },
	{ 0x8000000003F9D285ull, 0x8000000003F9D285ull },
	{ 0x7FFB37D2241C3B1Dull, 0x7FFB37D2241C3B1Dull },
	{ 0x0000001983B94C92ull, 0x0000001983B94C92ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x80009443E584F4A5ull, 0x80009443E584F4A5ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x8000000855A82C2Cull, 0x8000000855A82C2Cull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x000000000005B6ABull, 0x000000000005B6ABull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x00000000B58DC413ull, 0x00000000B58DC413ull },
	{ 0xB3E6B297EC29791Dull, 0xB3E6B297EC29791Dull },
	{ 0x538FB3BC5CFA6759ull, 0x538FB3BC5CFA6759ull },
	{ 0xBA000797F4C7DEA1ull, 0xBA000797F4C7DEA1ull },
	{ 0x0C1C8FF2F41F16DAull, 0x0C1C8FF2F41F16DAull },
	{ 0x30B5C1676C56DD62ull, 0x30B5C1676C56DD62ull },
	{ 0x5CCB5C56E7C5AC0Bull, 0x5CCB5C56E7C5AC0Bull },
	{ 0x63068E39A09E6158ull, 0x63068E39A09E6158ull },
	{ 0x07B3FB8E47099176ull, 0x07B3FB8E47099176ull },
	{ 0x184F31A158F1807Dull, 0x184F31A158F1807Dull },
	{ 0x8758FF530896B7E7ull, 0x8758FF530896B7E7ull },
	{ 0x4C0A36D2B4A33B50ull, 0x4C0A36D2B4A33B50ull },
	{ 0xC64C5756ED525E6Full, 0xC64C5756ED525E6Full },
	{ 0x66199E890D233BD4ull, 0x66199E890D233BD4ull },
	{ 0x578E020B5EEBCB63ull, 0x578E020B5EEBCB63ull },
	{ 0xF28A90D0D4429690ull, 0xF28A90D0D4429690ull },
	{ 0x2E44BC8D4F15105Cull, 0x2E44BC8D4F15105Cull },
	{ 0xD9830B957560898Full, 0xD9830B957560898Full },
	{ 0xD5F0B21519E3C836ull, 0xD5F0B21519E3C836ull },
	{ 0x17D9939333BF3CE0ull, 0x17D9939333BF3CE0ull },
	{ 0x2A282C8A6399D814ull, 0x2A282C8A6399D814ull },
	{ 0xF616E3ED0C808295ull, 0xF616E3ED0C808295ull },
	{ 0x8EEE447BF9D63059ull, 0x8EEE447BF9D63059ull },
	{ 0x41719E46964A9745ull, 0x41719E46964A9745ull },
	{ 0x285390BBB32D882Eull, 0x285390BBB32D882Eull },
	{ 0x0333FB01DF54E08Cull, 0x0333FB01DF54E08Cull },
	{ 0x12E0D6DE4EAB2400ull, 0x12E0D6DE4EAB2400ull },
	{ 0xB8DB78A14FDCAF88ull, 0xB8DB78A14FDCAF88ull },
	{ 0x9B3BBA9F54F99101ull, 0x9B3BBA9F54F99101ull },
	{ 0x8C428C6D88C1FDDDull, 0x8C428C6D88C1FDDDull },
	{ 0xF6556E68F7B36624ull, 0xF6556E68F7B36624ull },
	{ 0x0B7451FF444C2F30ull, 0x0B7451FF444C2F30ull },
	{ 0xF943635FF0DC04A4ull, 0xF943635FF0DC04A4ull },
	{ 0x60BA3B4D3516D22Dull, 0x60BA3B4D3516D22Dull },
	{ 0x51A1F8B3648057FCull, 0x51A1F8B3648057FCull },
	{ 0x0AA3C17DE7265F82ull, 0x0AA3C17DE7265F82ull },
	{ 0xBCF12C70BEFF94EEull, 0xBCF12C70BEFF94EEull },
	{ 0x9408133D9FA4F8E4ull, 0x9408133D9FA4F8E4ull },
	{ 0xC14B4AFB957BFA7Eull, 0xC14B4AFB957BFA7Eull },
	{ 0xCBF1C6783C472A07ull, 0xCBF1C6783C472A07ull },
	{ 0xE4B59F4986B39276ull, 0xE4B59F4986B39276ull },
	{ 0xFDA487571458AA28ull, 0xFDA487571458AA28ull },
	{ 0x65BB22A219C0EF62ull, 0x65BB22A219C0EF62ull },
	{ 0xA8BCC882D7B11885ull, 0xA8BCC882D7B11885ull },
	{ 0xF2F12D229FC3EE93ull, 0xF2F12D229FC3EE93ull },
	{ 0xB8F973296F10BFE2ull, 0xB8F973296F10BFE2ull },
	{ 0xE356252611F2779Eull, 0xE356252611F2779Eull },
	{ 0xBE6F96E7DCA55404ull, 0xBE6F96E7DCA55404ull },
	{ 0x6E1A2F90F2513BDCull, 0x6E1A2F90F2513BDCull },
	{ 0xA5681A181B76B3B7ull, 0xA5681A181B76B3B7ull },
	{ 0x6291DFC12CD4766Aull, 0x6291DFC12CD4766Aull },
	{ 0x6E876922FF3BA6CAull, 0x6E876922FF3BA6CAull },
	{ 0xE092E7C1F5A8AB28ull, 0xE092E7C1F5A8AB28ull },
	{ 0xE67F277615169F0Full, 0xE67F277615169F0Full },
	{ 0x8FFAC7E3AAA36B3Cull, 0x8FFAC7E3AAA36B3Cull },
	{ 0x5E69DE7D833A79DDull, 0x5E69DE7D833A79DDull },
	{ 0xD8EF7F37B1DC0E0Eull, 0xD8EF7F37B1DC0E0Eull },
	{ 0x6390AE81E8373B90ull, 0x6390AE81E8373B90ull },
	{ 0x9BCA3AF05AEC78CCull, 0x9BCA3AF05AEC78CCull },
	{ 0x779AFE4A3BEED607ull, 0x779AFE4A3BEED607ull },
	{ 0xE96C0724BE59D6E5ull, 0xE96C0724BE59D6E5ull },
	{ 0x48282AC1AFF1E2A3ull, 0x48282AC1AFF1E2A3ull },
	{ 0x3A9C7E589DA56010ull, 0x3A9C7E589DA56010ull },
	{ 0xE76337E18D84664Cull, 0xE76337E18D84664Cull },
	{ 0x28927928434515C4ull, 0x28927928434515C4ull },
	{ 0x83A07FBF184A8DEDull, 0x83A07FBF184A8DEDull },
	{ 0xA4D6BF58628482C0ull, 0xA4D6BF58628482C0ull },
	{ 0x9F6562DD6BDD72F5ull, 0x9F6562DD6BDD72F5ull },
	{ 0xB0F0B669F80D7C82ull, 0xB0F0B669F80D7C82ull },
	{ 0x4979F8835D07D6D6ull, 0x4979F8835D07D6D6ull },
	{ 0x16DD13522958E2D2ull, 0x16DD13522958E2D2ull },
	{ 0x54E4BE755A6AE588ull, 0x54E4BE755A6AE588ull },
	{ 0x20CE889624875DB7ull, 0x20CE889624875DB7ull },
	{ 0x910048B34A0AF605ull, 0x910048B34A0AF605ull },
	{ 0x512877AB01A220FEull, 0x512877AB01A220FEull },
	{ 0x017AF1FA6CC4484Cull, 0x017AF1FA6CC4484Cull },
	{ 0x1AA3A0B9A083A938ull, 0x1AA3A0B9A083A938ull },
	{ 0x15D08B6B3B27C660ull, 0x15D08B6B3B27C660ull },
	{ 0x982CD1C68B287EBAull, 0x982CD1C68B287EBAull },
	{ 0xE26E0CD3473FC4C9ull, 0xE26E0CD3473FC4C9ull },
	{ 0xC4ED983DD3E7F21Dull, 0xC4ED983DD3E7F21Dull },
	{ 0x575F445B1BBCC090ull, 0x575F445B1BBCC090ull },
	{ 0xB45EEF22F5D4DE3Eull, 0xB45EEF22F5D4DE3Eull },
	{ 0xF94686781CCA2344ull, 0xF94686781CCA2344ull },
	{ 0x4A05F0A4B5C668BDull, 0x4A05F0A4B5C668BDull },
	{ 0x18EE5E82678AFEFAull, 0x18EE5E82678AFEFAull },
	{ 0x720A85150148C080ull, 0x720A85150148C080ull },
	{ 0x0B653E2EB23ECE47ull, 0x0B653E2EB23ECE47ull },
	{ 0xD8781344CEF5DF2Full, 0xD8781344CEF5DF2Full },
	{ 0x62032DAC2ACE3946ull, 0x62032DAC2ACE3946ull },
	{ 0x21C830C3FDB647B8ull, 0x21C830C3FDB647B8ull },
	{ 0x22597B665A3FCF16ull, 0x22597B665A3FCF16ull },
	{ 0xA2A751653669C6CBull, 0xA2A751653669C6CBull },
	{ 0x7E1C6CEB0114CADAull, 0x7E1C6CEB0114CADAull },
	{ 0x97F377C5CA38345Cull, 0x97F377C5CA38345Cull },
	{ 0x8B66ED93F90DCFB9ull, 0x8B66ED93F90DCFB9ull },
	{ 0xDEF4E4B993CA3126ull, 0xDEF4E4B993CA3126ull },
	{ 0xFC983FB515D7AC5Full, 0xFC983FB515D7AC5Full },
	{ 0xAD10C77AD6E9E824ull, 0xAD10C77AD6E9E824ull },
	{ 0x582BABEE02A018F8ull, 0x582BABEE02A018F8ull },
	{ 0x47F5EE3E1F4BCF18ull, 0x47F5EE3E1F4BCF18ull },
	{ 0xB0EA316024D2773Aull, 0xB0EA316024D2773Aull },
	{ 0xBD83752A237F6507ull, 0xBD83752A237F6507ull },
	{ 0xDDF635E87A7C93D9ull, 0xDDF635E87A7C93D9ull },
	{ 0x3281359D7A1B6513ull, 0x3281359D7A1B6513ull },
	{ 0xA9B3BE5D3F5D4CBBull, 0xA9B3BE5D3F5D4CBBull },
	{ 0xB63B72F0F40FCAA4ull, 0xB63B72F0F40FCAA4ull },
	{ 0x1D6E71BB6F948EDEull, 0x1D6E71BB6F948EDEull },
	{ 0xDCDDA755F17B4FD0ull, 0xDCDDA755F17B4FD0ull },
	{ 0x5614C83C4AF0E037ull, 0x5614C83C4AF0E037ull },
	{ 0xBE7CB24A93C4E75Full, 0xBE7CB24A93C4E75Full },
	{ 0x12D9B693739C329Full, 0x12D9B693739C329Full },
	{ 0xCF574A05F645B01Aull, 0xCF574A05F645B01Aull },
	{ 0xAB1AA029F5480133ull, 0xAB1AA029F5480133ull },
	{ 0x980DAB6A0CDDB250ull, 0x980DAB6A0CDDB250ull },
	{ 0xC69C0732247610D4ull, 0xC69C0732247610D4ull },
	{ 0x6D63495984B0A056ull, 0x6D63495984B0A056ull },
	{ 0x15303BDF7EA930A5ull, 0x15303BDF7EA930A5ull },
	{ 0x287A7D91A34A9581ull, 0x287A7D91A34A9581ull },
	{ 0x07280CE28A3DFA92ull, 0x07280CE28A3DFA92ull },
	{ 0x5E8A3848282CD92Cull, 0x5E8A3848282CD92Cull },
	{ 0x04DD7828B6AB90C5ull, 0x04DD7828B6AB90C5ull },
	{ 0x192DB04D2AC29AB6ull, 0x192DB04D2AC29AB6ull },
	{ 0xA6F859603C6C6131ull, 0xA6F859603C6C6131ull },
	{ 0x8E1E1290FC7DB483ull, 0x8E1E1290FC7DB483ull },
	{ 0x6E6D1AFFFCCEE30Aull, 0x6E6D1AFFFCCEE30Aull },
	{ 0x578632EE071B6B78ull, 0x578632EE071B6B78ull },
	{ 0xFC7565D362AE56C9ull, 0xFC7565D362AE56C9ull },
	{ 0x2B903B56AD512C81ull, 0x2B903B56AD512C81ull },
	{ 0xCA62D14A4153882Aull, 0xCA62D14A4153882Aull },
	{ 0x94FD5CA72BE1508Dull, 0x94FD5CA72BE1508Dull },
	{ 0xE20B936CB53D8CAFull, 0xE20B936CB53D8CAFull },
	{ 0x2A777E1B76D7D98Eull, 0x2A777E1B76D7D98Eull },
	{ 0xC70CED291CEF347Cull, 0xC70CED291CEF347Cull },
	{ 0x36212BE17B38547Bull, 0x36212BE17B38547Bull },
	{ 0x42160E1D249CF651ull, 0x42160E1D249CF651ull },
	{ 0xCF4FB68853557399ull, 0xCF4FB68853557399ull },
	{ 0xA7150B9DEDC69F8Dull, 0xA7150B9DEDC69F8Dull },
	{ 0xDB4C60678CB99E85ull, 0xDB4C60678CB99E85ull },
	{ 0xEDEF86D1121E5B31ull, 0xEDEF86D1121E5B31ull },
	{ 0xFD00C816D0AE9C15ull, 0xFD00C816D0AE9C15ull },
	{ 0x3292476542E7BC45ull, 0x3292476542E7BC45ull },
	{ 0xB7C94A7151B0FDE2ull, 0xB7C94A7151B0FDE2ull },
	{ 0x6D6EEB802B2481D5ull, 0x6D6EEB802B2481D5ull },
	{ 0xA6F7B2C5AAD0DE49ull, 0xA6F7B2C5AAD0DE49ull },
	{ 0x2AE397F04F2953EDull, 0x2AE397F04F2953EDull },
	{ 0xB63FEB49C17AA36Bull, 0xB63FEB49C17AA36Bull },
	{ 0xC3FCA7A2EBDA8423ull, 0xC3FCA7A2EBDA8423ull },
	{ 0xD97EAC12A227BB2Cull, 0xD97EAC12A227BB2Cull },
	{ 0x95905C9DD6D0CF8Cull, 0x95905C9DD6D0CF8Cull },
	{ 0xC4EA2C5063B76035ull, 0xC4EA2C5063B76035ull },
	{ 0x39148EBD5FA61BC3ull, 0x39148EBD5FA61BC3ull },
	{ 0x24FCD14E05EF54F4ull, 0x24FCD14E05EF54F4ull },
	{ 0x3C0E5CD62DAA3967ull, 0x3C0E5CD62DAA3967ull },
	{ 0xD659DEF550A4A619ull, 0xD659DEF550A4A619ull },
	{ 0xD83BDBFB223FA19Cull, 0xD83BDBFB223FA19Cull },
	{ 0x1DEC27B6B5CC6C19ull, 0x1DEC27B6B5CC6C19ull },
	{ 0x2BA7C7C737203987ull, 0x2BA7C7C737203987ull },
	{ 0x60FEBC952FF4872Cull, 0x60FEBC952FF4872Cull },
	{ 0xF0B7328B5F4A0FD4ull, 0xF0B7328B5F4A0FD4ull },
	{ 0xDCCF9CD622246351ull, 0xDCCF9CD622246351ull },
	{ 0x35E1350067141BB2ull, 0x35E1350067141BB2ull },
	{ 0x5F2C667538E5B447ull, 0x5F2C667538E5B447ull },
	{ 0x96009E5CD32AFF54ull, 0x96009E5CD32AFF54ull },
	{ 0xFDF905FC4A353A93ull, 0xFDF905FC4A353A93ull },
	{ 0x703FA3EEC1B5C770ull, 0x703FA3EEC1B5C770ull },
	{ 0x4F60661F33DB7F7Full, 0x4F60661F33DB7F7Full },
	{ 0x21EF207BBDAD9498ull, 0x21EF207BBDAD9498ull },
	{ 0xB4282CDD7835D8ADull, 0xB4282CDD7835D8ADull },
	{ 0xA904E9E41CE3E903ull, 0xA904E9E41CE3E903ull },
	{ 0x6D1391A59C84B2D4ull, 0x6D1391A59C84B2D4ull },
	{ 0x9EB58C1AD41B447Dull, 0x9EB58C1AD41B447Dull },
	{ 0x13D713F6EDA95366ull, 0x13D713F6EDA95366ull },
	{ 0x1184E8913A6C3640ull, 0x1184E8913A6C3640ull },
	{ 0x78DC5D0B0C572003ull, 0x78DC5D0B0C572003ull },
	{ 0x7C9939A61D1BAAE1ull, 0x7C9939A61D1BAAE1ull },
	{ 0x9EC8961DEBA76093ull, 0x9EC8961DEBA76093ull },
	{ 0xB2CD9B2ACC847BE8ull, 0xB2CD9B2ACC847BE8ull },
	{ 0x71B9312DDBA5109Aull, 0x71B9312DDBA5109Aull },
	{ 0x51B0F0B3510D008Bull, 0x51B0F0B3510D008Bull },
	{ 0x6228FBE36B249343ull, 0x6228FBE36B249343ull },
	{ 0xEAF240D8E2A9A9AAull, 0xEAF240D8E2A9A9AAull },
	{ 0x90C6A203F3159601ull, 0x90C6A203F3159601ull },
	{ 0x3C5488D0F22D9126ull, 0x3C5488D0F22D9126ull },
	{ 0x86735E197513CF88ull, 0x86735E197513CF88ull },
	{ 0xE7D26C57094F3AE8ull, 0xE7D26C57094F3AE8ull },
	{ 0x6C8BA8DE7B1FC251ull, 0x6C8BA8DE7B1FC251ull },
	{ 0x2BDBE57AAB955559ull, 0x2BDBE57AAB955559ull },
	{ 0x4439E9CDF93F7818ull, 0x4439E9CDF93F7818ull },
	{ 0x9ADDF51134EB78E9ull, 0x9ADDF51134EB78E9ull },
	{ 0x1F5326F72B0746E0ull, 0x1F5326F72B0746E0ull },
	{ 0xD3B4520BB99EE70Aull, 0xD3B4520BB99EE70Aull },
	{ 0x57CDB4B576CA8D6Bull, 0x57CDB4B576CA8D6Bull },
	{ 0x39651A70E8434CB4ull, 0x39651A70E8434CB4ull },
	{ 0xC7C1E5CC4AF474C4ull, 0xC7C1E5CC4AF474C4ull },
	{ 0xAB56821AE7FF8062ull, 0xAB56821AE7FF8062ull },
	{ 0x81A4BBE6FE48B037ull, 0x81A4BBE6FE48B037ull },
	{ 0x3CFE83B938024091ull, 0x3CFE83B938024091ull },
	{ 0xEE2C90B691DEF9C6ull, 0xEE2C90B691DEF9C6ull },
	{ 0x62FD4062C7527E06ull, 0x62FD4062C7527E06ull },
	{ 0xADD0FD1938EFC8D9ull, 0xADD0FD1938EFC8D9ull },
	{ 0x6E9C1C75858E879Cull, 0x6E9C1C75858E879Cull },
	{ 0xF747BF6A98C71410ull, 0xF747BF6A98C71410ull },
	{ 0x4510B9B0DB8B205Bull, 0x4510B9B0DB8B205Bull },
	{ 0xE008F97004989A95ull, 0xE008F97004989A95ull },
	{ 0x6D9FDBDA718A0B3Full, 0x6D9FDBDA718A0B3Full },
	{ 0x086E1E517D348804ull, 0x086E1E517D348804ull },
	{ 0xB64962CBA402306Bull, 0xB64962CBA402306Bull },
	{ 0x9A86D959EC5A82A1ull, 0x9A86D959EC5A82A1ull },
	{ 0xEC1CE074FE1B6E19ull, 0xEC1CE074FE1B6E19ull },
	{ 0xFF26EACEC4F307B7ull, 0xFF26EACEC4F307B7ull },
	{ 0x3D0D13EC1205FD0Eull, 0x3D0D13EC1205FD0Eull },
	{ 0x60B478814647C781ull, 0x60B478814647C781ull },
	{ 0xA3DA5CB1149A68B0ull, 0xA3DA5CB1149A68B0ull },
	{ 0xC361598FAA4691B2ull, 0xC361598FAA4691B2ull },
	{ 0x7F206B283FC634CAull, 0x7F206B283FC634CAull },
	{ 0xDF4024749D006AE0ull, 0xDF4024749D006AE0ull },
	{ 0x711847F760D09E32ull, 0x711847F760D09E32ull },
	{ 0xEBE6F595E742462Bull, 0xEBE6F595E742462Bull },
	{ 0xFD593744FEC02C13ull, 0xFD593744FEC02C13ull },
	{ 0x3D2AD79F636FB22Eull, 0x3D2AD79F636FB22Eull },
	{ 0x52C18D9D90D9770Eull, 0x52C18D9D90D9770Eull },
	{ 0x7F06164A8E84928Bull, 0x7F06164A8E84928Bull },
	{ 0x5623837A1EA0F74Dull, 0x5623837A1EA0F74Dull },
	{ 0x7F1948767F3C7D71ull, 0x7F1948767F3C7D71ull },
	{ 0x76642F3676FB0B34ull, 0x76642F3676FB0B34ull },
	{ 0x6B77EFB22B3865DCull, 0x6B77EFB22B3865DCull },
	{ 0xFEAD56A8D2A8EEA2ull, 0xFEAD56A8D2A8EEA2ull },
	{ 0x0B7A0605FE9716CBull, 0x0B7A0605FE9716CBull },
	{ 0x7807942AEFE28A7Aull, 0x7807942AEFE28A7Aull },
	{ 0x67E8464F2A6CDD95ull, 0x67E8464F2A6CDD95ull },
	{ 0x7F3CFC016FB7AFBFull, 0x7F3CFC016FB7AFBFull },
	{ 0x2585ECC6432AEBB1ull, 0x2585ECC6432AEBB1ull },
	{ 0x9DEF3515B19916A5ull, 0x9DEF3515B19916A5ull },
	{ 0x224431BEECD4A9AEull, 0x224431BEECD4A9AEull },
	{ 0x27A2D5D0C9755C84ull, 0x27A2D5D0C9755C84ull },
	{ 0x10A07D8918A070B3ull, 0x10A07D8918A070B3ull },
	{ 0x373EAB981556BF3Aull, 0x373EAB981556BF3Aull },
	{ 0x27E78AAC88657CABull, 0x27E78AAC88657CABull },
	{ 0x887577EE641A6E2Cull, 0x887577EE641A6E2Cull },
	{ 0xAEDD7223C0500686ull, 0xAEDD7223C0500686ull },
	{ 0x8E0137305F85593Full, 0x8E0137305F85593Full },
	{ 0xB84C642C630279F8ull, 0xB84C642C630279F8ull },
	{ 0x2F23DF8753B53A5Cull, 0x2F23DF8753B53A5Cull },
	{ 0x3A6F715B393C1D38ull, 0x3A6F715B393C1D38ull },
	{ 0x0E5372D8308B39F2ull, 0x0E5372D8308B39F2ull },
	{ 0xA0380A0B4690CDA7ull, 0xA0380A0B4690CDA7ull },
	{ 0x1BFB493D59B0687Cull, 0x1BFB493D59B0687Cull },
	{ 0x857A69431226DEC1ull, 0x857A69431226DEC1ull },
	{ 0xAECB6DDADEA9C6DCull, 0xAECB6DDADEA9C6DCull },
	{ 0x37877A72EB782690ull, 0x37877A72EB782690ull },
	{ 0xB62D4C34F7A7F757ull, 0xB62D4C34F7A7F757ull },
	{ 0x21066482BD27A692ull, 0x21066482BD27A692ull },
	{ 0x3D77C3B186915232ull, 0x3D77C3B186915232ull },
	{ 0x346C2386FEF215E2ull, 0x346C2386FEF215E2ull },
	{ 0x8EE70DEA78D847A4ull, 0x8EE70DEA78D847A4ull },
	{ 0xB02463A646BB941Cull, 0xB02463A646BB941Cull },
	{ 0x92BCE23D7BF947B1ull, 0x92BCE23D7BF947B1ull },
	{ 0x81208EEFEB87646Cull, 0x81208EEFEB87646Cull },
	{ 0x81B9FBB41E057ABFull, 0x81B9FBB41E057ABFull },
	{ 0x147BEF1A81B3F2DEull, 0x147BEF1A81B3F2DEull },
	{ 0x13BBF8369033C039ull, 0x13BBF8369033C039ull },
	{ 0x216BBA95B2BDC553ull, 0x216BBA95B2BDC553ull },
	{ 0xA65897C7511317E3ull, 0xA65897C7511317E3ull },
	{ 0xA07A73FBE340EA37ull, 0xA07A73FBE340EA37ull },
	{ 0x904A621F1FCFD441ull, 0x904A621F1FCFD441ull },
	{ 0x12636942884DA213ull, 0x12636942884DA213ull },
	{ 0x9D2606A8F54684E3ull, 0x9D2606A8F54684E3ull },
	{ 0xA14F77FFC248DE22ull, 0xA14F77FFC248DE22ull },
	{ 0xAEE21EA1F7AB4AC7ull, 0xAEE21EA1F7AB4AC7ull },
	{ 0x32465FBD9EFF4AFAull, 0x32465FBD9EFF4AFAull },
	{ 0x1D1EDC8A4C94326Eull, 0x1D1EDC8A4C94326Eull },
	{ 0x113BC956FF05C93Bull, 0x113BC956FF05C93Bull },
	{ 0x213C621327450D29ull, 0x213C621327450D29ull },
	{ 0x364F41B3E183D913ull, 0x364F41B3E183D913ull },
	{ 0x27AA9A5297F5F6D2ull, 0x27AA9A5297F5F6D2ull },
	{ 0x1134216A9B07AD10ull, 0x1134216A9B07AD10ull },
	{ 0x85F70AA190098446ull, 0x85F70AA190098446ull },
	{ 0x8AF82498C5949E80ull, 0x8AF82498C5949E80ull },
	{ 0xACE83D5A57E54979ull, 0xACE83D5A57E54979ull },
	{ 0xB2A9EE9419760B2Bull, 0xB2A9EE9419760B2Bull },
	{ 0x11004C71908C6720ull, 0x11004C71908C6720ull },
	{ 0x8D4B0C15D5B9E9ADull, 0x8D4B0C15D5B9E9ADull },
	{ 0xB702F05163B26A36ull, 0xB702F05163B26A36ull },
	{ 0x3C3EEA440C04B56Aull, 0x3C3EEA440C04B56Aull },
	{ 0x3600476751ABC298ull, 0x3600476751ABC298ull },
	{ 0x3727188344DB837Aull, 0x3727188344DB837Aull },
	{ 0x30AE61B08C2F1920ull, 0x30AE61B08C2F1920ull },
	{ 0x92D213D790AED017ull, 0x92D213D790AED017ull },
	{ 0x9589F9EDE4C17E13ull, 0x9589F9EDE4C17E13ull },
	{ 0x3390FE8B608782A3ull, 0x3390FE8B608782A3ull },
	{ 0x8790BE26492774EDull, 0x8790BE26492774EDull },
	{ 0xB98DDC832ED859C3ull, 0xB98DDC832ED859C3ull },
	{ 0x1C35DE8C9C6A7DCAull, 0x1C35DE8C9C6A7DCAull },
	{ 0x305BF973C5314DD7ull, 0x305BF973C5314DD7ull },
	{ 0x3034A29E24C35613ull, 0x3034A29E24C35613ull },
	{ 0x95F3243DCF28DAA1ull, 0x95F3243DCF28DAA1ull },
	{ 0x96E55865C552B028ull, 0x96E55865C552B028ull },
	{ 0x1A46B71555AFCD64ull, 0x1A46B71555AFCD64ull },
	{ 0x000002CDD8F4A2BAull, 0x000002CDD8F4A2BAull },
	{ 0x000000026CFAB210ull, 0x000000026CFAB210ull },
	{ 0x000EA5DD919B356Cull, 0x000EA5DD919B356Cull },
	{ 0x0001147EB2545912ull, 0x0001147EB2545912ull },
	{ 0x000000003EF20726ull, 0x000000003EF20726ull },
	{ 0x0000000000027AB5ull, 0x0000000000027AB5ull },
	{ 0x000000000000E251ull, 0x000000000000E251ull },
	{ 0x800000000000000Full, 0x800000000000000Full },
	{ 0x0000000000010CD1ull, 0x0000000000010CD1ull },
	{ 0x8000000000000D7Bull, 0x8000000000000D7Bull },
	{ 0x800000000002366Full, 0x800000000002366Full },
	{ 0x8000000000000051ull, 0x8000000000000051ull },
	{ 0x8000000000000002ull, 0x8000000000000002ull },
	{ 0x0000058786E782ECull, 0x0000058786E782ECull },
	{ 0x00000000300D5C5Bull, 0x00000000300D5C5Bull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x00000000A453CD9Cull, 0x00000000A453CD9Cull },
	{ 0x80002A6FD740C9E7ull, 0x80002A6FD740C9E7ull },
	{ 0x8000000000000011ull, 0x8000000000000011ull },
	{ 0x00000377B18BD0A3ull, 0x00000377B18BD0A3ull },
	{ 0x800000000000078Dull, 0x800000000000078Dull },
	{ 0x80000000000017F5ull, 0x80000000000017F5ull },
	{ 0x800000000019F989ull, 0x800000000019F989ull },
	{ 0x00000442AF84BB53ull, 0x00000442AF84BB53ull },
	{ 0x00058F9A696D7761ull, 0x00058F9A696D7761ull },
	{ 0x0000000000000D28ull, 0x0000000000000D28ull },
	{ 0x0000000010C834C9ull, 0x0000000010C834C9ull },
	{ 0x8000002C06F8C3E0ull, 0x8000002C06F8C3E0ull },
	{ 0x0000000D18136CDAull, 0x0000000D18136CDAull },
	{ 0x0000000000000674ull, 0x0000000000000674ull },
	{ 0x800000000685837Aull, 0x800000000685837Aull },
	{ 0x000016EFAF8E1252ull, 0x000016EFAF8E1252ull },
	{ 0x800006F2C793B324ull, 0x800006F2C793B324ull },
	{ 0x8000052EA0692A19ull, 0x8000052EA0692A19ull },
	{ 0x8000012D06D2BA69ull, 0x8000012D06D2BA69ull },
	{ 0x000024A242B24418ull, 0x000024A242B24418ull },
	{ 0x8000000898A7B3D8ull, 0x8000000898A7B3D8ull },
	{ 0x8000000000000053ull, 0x8000000000000053ull },
	{ 0x00000000031E9784ull, 0x00000000031E9784ull },
	{ 0x00000000000031D3ull, 0x00000000000031D3ull },
	{ 0x80015D52CB68E331ull, 0x80015D52CB68E331ull },
	{ 0x00000735DF11D7A7ull, 0x00000735DF11D7A7ull },
	{ 0x8000000000000024ull, 0x8000000000000024ull },
	{ 0x80000002DEAC1A3Bull, 0x80000002DEAC1A3Bull },
	{ 0x8000000000000E5Dull, 0x8000000000000E5Dull },
	{ 0x000000000323F913ull, 0x000000000323F913ull },
	{ 0x00000000194BD424ull, 0x00000000194BD424ull },
	{ 0x800000FD63DB3F44ull, 0x800000FD63DB3F44ull },
	{ 0x00000046D8F1D0A4ull, 0x00000046D8F1D0A4ull },
	{ 0x80005978ED086304ull, 0x80005978ED086304ull },
	{ 0x0000000160BF5EA7ull, 0x0000000160BF5EA7ull },
	{ 0x800013221AD288F1ull, 0x800013221AD288F1ull },
	{ 0x80000001F08FBC40ull, 0x80000001F08FBC40ull },
	{ 0x800000C3C0239782ull, 0x800000C3C0239782ull },
	{ 0x0000887DAB1EC9B3ull, 0x0000887DAB1EC9B3ull },
	{ 0x0000000000000CACull, 0x0000000000000CACull },
	{ 0x800000000005F75Eull, 0x800000000005F75Eull },
	{ 0x800104474FB90E6Cull, 0x800104474FB90E6Cull },
	{ 0x80000003FD3A30F1ull, 0x80000003FD3A30F1ull },
	{ 0x8002E562E67F6A5Bull, 0x8002E562E67F6A5Bull },
	{ 0x00000000000001F5ull, 0x00000000000001F5ull },
	{ 0x0000000026620DC4ull, 0x0000000026620DC4ull },
	{ 0x00040DCD31AB7AB3ull, 0x00040DCD31AB7AB3ull },
	{ 0x000000000017F117ull, 0x000000000017F117ull },
	{ 0x8000000B461046D4ull, 0x8000000B461046D4ull },
	{ 0x0003035883E7F263ull, 0x0003035883E7F263ull },
	{ 0x0000000074D6C48Eull, 0x0000000074D6C48Eull },
	{ 0x80005C0D18A1F637ull, 0x80005C0D18A1F637ull },
	{ 0x00000000019985BEull, 0x00000000019985BEull },
	{ 0x0000001E963BBF53ull, 0x0000001E963BBF53ull },
	{ 0x00000000000EE202ull, 0x00000000000EE202ull },
	{ 0x800000000000B104ull, 0x800000000000B104ull },
	{ 0x000000C6B9561C11ull, 0x000000C6B9561C11ull },
	{ 0x000000000032C97Eull, 0x000000000032C97Eull },
	{ 0x0000000B641FADDCull, 0x0000000B641FADDCull },
	{ 0x80000000000000A1ull, 0x80000000000000A1ull },
	{ 0x0000000000001F9Cull, 0x0000000000001F9Cull },
	{ 0x0000009DE2CCB50Full, 0x0000009DE2CCB50Full },
	{ 0x8000000000002779ull, 0x8000000000002779ull },
	{ 0x00008871904FAA05ull, 0x00008871904FAA05ull },
	{ 0x8002E445B3687355ull, 0x8002E445B3687355ull },
	{ 0x0005D28E0783A6AFull, 0x0005D28E0783A6AFull },
	{ 0x8000009A5F03FCE4ull, 0x8000009A5F03FCE4ull },
	{ 0x80000000000001B3ull, 0x80000000000001B3ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x00000000000019DEull, 0x00000000000019DEull },
	{ 0x8000000000010542ull, 0x8000000000010542ull },
	{ 0x00000000001B7C58ull, 0x00000000001B7C58ull },
	{ 0x00000000000018CBull, 0x00000000000018CBull },
	{ 0x00070E8E4A859180ull, 0x00070E8E4A859180ull },
	{ 0x00000000000000DEull, 0x00000000000000DEull },
	{ 0x800000000000032Aull, 0x800000000000032Aull },
	{ 0x8000000000000045ull, 0x8000000000000045ull },
	{ 0x800000000000088Full, 0x800000000000088Full },
	{ 0x800066B451E6FFC6ull, 0x800066B451E6FFC6ull },
	{ 0x8000000000000031ull, 0x8000000000000031ull },
	{ 0x000000002C049D54ull, 0x000000002C049D54ull },
	{ 0x00000236220A6AF1ull, 0x00000236220A6AF1ull },
	{ 0x80000001A884ADC4ull, 0x80000001A884ADC4ull },
	{ 0x8000000000009B78ull, 0x8000000000009B78ull },
	{ 0x00000000000336EBull, 0x00000000000336EBull },
	{ 0x00000001EB4971DDull, 0x00000001EB4971DDull },
	{ 0x0000000000007EDDull, 0x0000000000007EDDull },
	{ 0x00000001C532482Aull, 0x00000001C532482Aull },
	{ 0x0000000F1E212955ull, 0x0000000F1E212955ull },
	{ 0x0000000000000E7Bull, 0x0000000000000E7Bull },
	{ 0x000000000000000Cull, 0x000000000000000Cull },
	{ 0x800002AB1FD23851ull, 0x800002AB1FD23851ull },
	{ 0x80000013F3BE3D82ull, 0x80000013F3BE3D82ull },
	{ 0x800000040C088F2Eull, 0x800000040C088F2Eull },
	{ 0x80000016AE1D6071ull, 0x80000016AE1D6071ull },
	{ 0x00000B6902D67F9Cull, 0x00000B6902D67F9Cull },
	{ 0x00012699FC5E07F4ull, 0x00012699FC5E07F4ull },
	{ 0x000000000000001Cull, 0x000000000000001Cull },
	{ 0x800000000013351Bull, 0x800000000013351Bull },
	{ 0x000117B7C2FB8459ull, 0x000117B7C2FB8459ull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x80000000007FC092ull, 0x80000000007FC092ull },
	{ 0x8000002E93EBE321ull, 0x8000002E93EBE321ull },
	{ 0x000000008A5342C3ull, 0x000000008A5342C3ull },
	{ 0x00000002E595D68Aull, 0x00000002E595D68Aull },
	{ 0x800001F26AE6FF61ull, 0x800001F26AE6FF61ull },
	{ 0x8000009CBDBF0632ull, 0x8000009CBDBF0632ull },
	{ 0x00000000002D1468ull, 0x00000000002D1468ull },
	{ 0x8000000000000005ull, 0x8000000000000005ull },
	{ 0x0000000000000005ull, 0x0000000000000005ull },
	{ 0x00000000009A4813ull, 0x00000000009A4813ull },
	{ 0x8000000000033018ull, 0x8000000000033018ull },
	{ 0x00000000007689D5ull, 0x00000000007689D5ull },
	{ 0x000B3226FB4156DFull, 0x000B3226FB4156DFull },
	{ 0x0000000180DC7649ull, 0x0000000180DC7649ull },
	{ 0x80017238D79B926Bull, 0x80017238D79B926Bull },
	{ 0x80001F0DCEBC58D7ull, 0x80001F0DCEBC58D7ull },
	{ 0x8000952BF1414019ull, 0x8000952BF1414019ull },
	{ 0x00000000008D8BBFull, 0x00000000008D8BBFull },
	{ 0x800000000000006Bull, 0x800000000000006Bull },
	{ 0x00000000000738D3ull, 0x00000000000738D3ull },
	{ 0x8000000000D61674ull, 0x8000000000D61674ull },
	{ 0x80000006CE9E627Aull, 0x80000006CE9E627Aull },
	{ 0x800000000000F6F7ull, 0x800000000000F6F7ull },
	{ 0x0000000000000C89ull, 0x0000000000000C89ull },
	{ 0x800000F43A59C6F3ull, 0x800000F43A59C6F3ull },
	{ 0x800000000702B24Eull, 0x800000000702B24Eull },
	{ 0x800000000000374Cull, 0x800000000000374Cull },
	{ 0x000767A6B1E4E329ull, 0x000767A6B1E4E329ull },
	{ 0x00000001F26E9A1Dull, 0x00000001F26E9A1Dull },
	{ 0x800000000000CD7Eull, 0x800000000000CD7Eull },
	{ 0x0000000074C49B18ull, 0x0000000074C49B18ull },
	{ 0x0000000000000007ull, 0x0000000000000007ull },
	{ 0x000000000063FB34ull, 0x000000000063FB34ull },
	{ 0x00000670541BB5C8ull, 0x00000670541BB5C8ull },
	{ 0x8000BF125ACD97EAull, 0x8000BF125ACD97EAull },
	{ 0x800000000000000Full, 0x800000000000000Full },
	{ 0x00000000000312CBull, 0x00000000000312CBull },
	{ 0x00000000000004B9ull, 0x00000000000004B9ull },
	{ 0x80000031EA35834Dull, 0x80000031EA35834Dull },
	{ 0x80000000000BD049ull, 0x80000000000BD049ull },
	{ 0x80000000DBEC9FEDull, 0x80000000DBEC9FEDull },
	{ 0x0000000000000026ull, 0x0000000000000026ull },
	{ 0x00000000033227BFull, 0x00000000033227BFull },
	{ 0x8000075B1529480Full, 0x8000075B1529480Full },
	{ 0x8000018296905BEEull, 0x8000018296905BEEull },
	{ 0x00016B1390638429ull, 0x00016B1390638429ull },
	{ 0x000000016C8E97FCull, 0x000000016C8E97FCull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x800000018FC1BF67ull, 0x800000018FC1BF67ull },
	{ 0x800000000000B26Eull, 0x800000000000B26Eull },
	{ 0x80031FFAD7665759ull, 0x80031FFAD7665759ull },
	{ 0x8000000000001FACull, 0x8000000000001FACull },
	{ 0x80000001354C9157ull, 0x80000001354C9157ull },
	{ 0x000000000000421Full, 0x000000000000421Full },
	{ 0x000005128452EF21ull, 0x000005128452EF21ull },
	{ 0x00000000000007B3ull, 0x00000000000007B3ull },
	{ 0x000000D43CF66B0Aull, 0x000000D43CF66B0Aull },
	{ 0x000000002726A58Dull, 0x000000002726A58Dull },
	{ 0x00000003886C3147ull, 0x00000003886C3147ull },
	{ 0x0000000003897113ull, 0x0000000003897113ull },
	{ 0x8000000177856717ull, 0x8000000177856717ull },
	{ 0x80000000000031E1ull, 0x80000000000031E1ull },
	{ 0x800000102E74E307ull, 0x800000102E74E307ull },
	{ 0x8000000000000BE6ull, 0x8000000000000BE6ull },
	{ 0x000015EE09A1C2EDull, 0x000015EE09A1C2EDull },
	{ 0x0000000000003762ull, 0x0000000000003762ull },
	{ 0x8000740A8CD3309Aull, 0x8000740A8CD3309Aull },
	{ 0x8000000000000167ull, 0x8000000000000167ull },
	{ 0x0000000000033BBAull, 0x0000000000033BBAull },
	{ 0x8000208FEFD553D2ull, 0x8000208FEFD553D2ull },
	{ 0x000000000000E6FBull, 0x000000000000E6FBull },
	{ 0x8000000007FE6B79ull, 0x8000000007FE6B79ull },
	{ 0x000000000000150Dull, 0x000000000000150Dull },
	{ 0x00000006FA634E96ull, 0x00000006FA634E96ull },
	{ 0x000000002F60B8D7ull, 0x000000002F60B8D7ull },
	{ 0x0000000002C9303Cull, 0x0000000002C9303Cull },
	{ 0x80024DCC0AADAECAull, 0x80024DCC0AADAECAull },
	{ 0x8000000000034D1Full, 0x8000000000034D1Full },
	{ 0x0000000A0C03FBC6ull, 0x0000000A0C03FBC6ull },
	{ 0x000000001BAA0560ull, 0x000000001BAA0560ull },
	{ 0x80000000000006B1ull, 0x80000000000006B1ull },
	{ 0x0000000000000087ull, 0x0000000000000087ull },
	{ 0x800000012CAD1530ull, 0x800000012CAD1530ull },
	{ 0x000000000000005Cull, 0x000000000000005Cull },
	{ 0x800000022A82FFE6ull, 0x800000022A82FFE6ull },
	{ 0x800000001AE76E60ull, 0x800000001AE76E60ull },
	{ 0x0000FEF8183D8459ull, 0x0000FEF8183D8459ull },
	{ 0x0000000000B2EA0Full, 0x0000000000B2EA0Full },
	{ 0x0000000007AC5335ull, 0x0000000007AC5335ull },
	{ 0x8000000000169BCEull, 0x8000000000169BCEull },
	{ 0x00014BDB4942BA1Full, 0x00014BDB4942BA1Full },
	{ 0x0002B93E168D4532ull, 0x0002B93E168D4532ull },
	{ 0x0003904D6DCBCDD2ull, 0x0003904D6DCBCDD2ull },
	{ 0x8000000000671640ull, 0x8000000000671640ull },
	{ 0x80013A2564A1D6BEull, 0x80013A2564A1D6BEull },
	{ 0x000000000000045Bull, 0x000000000000045Bull },
	{ 0x000000000000086Bull, 0x000000000000086Bull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x80000131514DA8E4ull, 0x80000131514DA8E4ull },
	{ 0x8003071752CD6E85ull, 0x8003071752CD6E85ull },
	{ 0x800000000009401Bull, 0x800000000009401Bull },
	{ 0x0000000000000004ull, 0x0000000000000004ull },
	{ 0x000000000000010Cull, 0x000000000000010Cull },
	{ 0x800000000065D617ull, 0x800000000065D617ull },
	{ 0x8000000000453541ull, 0x8000000000453541ull },
	{ 0x80000000000000ABull, 0x80000000000000ABull },
	{ 0x000000130BE2E0DCull, 0x000000130BE2E0DCull },
	{ 0x000000000010F85Cull, 0x000000000010F85Cull },
	{ 0x800000000000002Aull, 0x800000000000002Aull },
	{ 0x0000000001A7C098ull, 0x0000000001A7C098ull },
	{ 0x0000000001027F30ull, 0x0000000001027F30ull },
	{ 0x8000000000000010ull, 0x8000000000000010ull },
	{ 0x8000001CB253CC9Full, 0x8000001CB253CC9Full },
	{ 0x000000043BE39F7Dull, 0x000000043BE39F7Dull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x0000000001D2AC12ull, 0x0000000001D2AC12ull },
	{ 0x8000000DF7007E9Bull, 0x8000000DF7007E9Bull },
	{ 0x000000102E884086ull, 0x000000102E884086ull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x80000000000006F0ull, 0x80000000000006F0ull },
	{ 0x80011EC3A41DD56Dull, 0x80011EC3A41DD56Dull },
	{ 0x800000EC3146F12Aull, 0x800000EC3146F12Aull },
	{ 0x0000000000000722ull, 0x0000000000000722ull },
	{ 0x000000036157CD0Cull, 0x000000036157CD0Cull },
	{ 0x8000000000000F0Dull, 0x8000000000000F0Dull },
	{ 0x80000000C6488435ull, 0x80000000C6488435ull },
	{ 0x000031CD12EAE039ull, 0x000031CD12EAE039ull },
	{ 0x800000000052E0BFull, 0x800000000052E0BFull },
	{ 0x000000000000F6B9ull, 0x000000000000F6B9ull },
	{ 0x800003A03444D2E2ull, 0x800003A03444D2E2ull },
	{ 0x800000055B9DD6C1ull, 0x800000055B9DD6C1ull },
	{ 0x8000000000000029ull, 0x8000000000000029ull },
	{ 0x800000001844C451ull, 0x800000001844C451ull },
	{ 0x0000000000000F77ull, 0x0000000000000F77ull },
	{ 0x00000000000F27B6ull, 0x00000000000F27B6ull },
	{ 0x000000000000000Eull, 0x000000000000000Eull },
	{ 0x0000000001BC90A2ull, 0x0000000001BC90A2ull },
	{ 0x00000000000003EAull, 0x00000000000003EAull },
	{ 0x00001C673B99FCACull, 0x00001C673B99FCACull },
	{ 0x0000000002B61B8Bull, 0x0000000002B61B8Bull },
	{ 0x8000000000000052ull, 0x8000000000000052ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x800027F18F11DEF3ull, 0x800027F18F11DEF3ull },
	{ 0x0000000000000006ull, 0x0000000000000006ull },
	{ 0x000001917B4822E3ull, 0x000001917B4822E3ull },
	{ 0x8000040E0505D097ull, 0x8000040E0505D097ull },
	{ 0x00000001DEACC588ull, 0x00000001DEACC588ull },
	{ 0x800000000001343Cull, 0x800000000001343Cull },
	{ 0x00000000005E71C0ull, 0x00000000005E71C0ull },
	{ 0x800000000000001Aull, 0x800000000000001Aull },
	{ 0x80000000000002C6ull, 0x80000000000002C6ull },
	{ 0x8000000000264CBCull, 0x8000000000264CBCull },
	{ 0x0000000000000070ull, 0x0000000000000070ull },
	{ 0x8005C6CC64DC08D0ull, 0x8005C6CC64DC08D0ull },
	{ 0x00000000000000D4ull, 0x00000000000000D4ull },
	{ 0x0000038ED6BDAE9Dull, 0x0000038ED6BDAE9Dull },
	{ 0x00000004F0F66D92ull, 0x00000004F0F66D92ull },
	{ 0x8000000000000063ull, 0x8000000000000063ull },
	{ 0x80000000009AEA2Full, 0x80000000009AEA2Full },
	{ 0x8000000000002A41ull, 0x8000000000002A41ull },
	{ 0x0000000000000089ull, 0x0000000000000089ull },
	{ 0x80000005932AE4DCull, 0x80000005932AE4DCull },
	{ 0x0000000000000005ull, 0x0000000000000005ull },
	{ 0x0000000000008771ull, 0x0000000000008771ull },
	{ 0x80000016B0C3523Eull, 0x80000016B0C3523Eull },
	{ 0x80071F83B5077BC2ull, 0x80071F83B5077BC2ull },
	{ 0x00000169F391D74Full, 0x00000169F391D74Full },
	{ 0x0000000121B0D915ull, 0x0000000121B0D915ull },
	{ 0x80000C6BCA41DE00ull, 0x80000C6BCA41DE00ull },
	{ 0x8000000015E9B383ull, 0x8000000015E9B383ull },
	{ 0x80000001C98013A4ull, 0x80000001C98013A4ull },
	{ 0x0000000017C5B30Bull, 0x0000000017C5B30Bull },
	{ 0x00000000000000FEull, 0x00000000000000FEull },
	{ 0x000000800C2991F5ull, 0x000000800C2991F5ull },
	{ 0x8000000000007E8Dull, 0x8000000000007E8Dull },
	{ 0x0000000007BD03EEull, 0x0000000007BD03EEull },
	{ 0x8000000000000DEFull, 0x8000000000000DEFull },
	{ 0x800D7787B545379Dull, 0x800D7787B545379Dull },
	{ 0x800000000000004Full, 0x800000000000004Full },
	{ 0x0000000000000147ull, 0x0000000000000147ull },
	{ 0x80000000CAFC3C6Cull, 0x80000000CAFC3C6Cull },
	{ 0x000063C7B3574D49ull, 0x000063C7B3574D49ull },
	{ 0x80000000001B88BAull, 0x80000000001B88BAull },
	{ 0x000B4C72A978965Full, 0x000B4C72A978965Full },
	{ 0x8000000000000024ull, 0x8000000000000024ull },
	{ 0x0000000000000B60ull, 0x0000000000000B60ull },
	{ 0x8000001827CC2E77ull, 0x8000001827CC2E77ull },
	{ 0x80000000000000B3ull, 0x80000000000000B3ull },
	{ 0x000002562FD6B879ull, 0x000002562FD6B879ull },
	{ 0x0000005174EEB699ull, 0x0000005174EEB699ull },
	{ 0x800000000000008Eull, 0x800000000000008Eull },
	{ 0x00000006D3589455ull, 0x00000006D3589455ull },
	{ 0x8000000000000011ull, 0x8000000000000011ull },
	{ 0x80000000000000C2ull, 0x80000000000000C2ull },
	{ 0x80000000000002A6ull, 0x80000000000002A6ull },
	{ 0x000000000A0B4CB8ull, 0x000000000A0B4CB8ull },
	{ 0x800000000000A1B2ull, 0x800000000000A1B2ull },
	{ 0x8007E9FAF8AE552Bull, 0x8007E9FAF8AE552Bull },
	{ 0x00000000000000DDull, 0x00000000000000DDull },
	{ 0x80000000003DAE83ull, 0x80000000003DAE83ull },
	{ 0x000000000001EA61ull, 0x000000000001EA61ull },
	{ 0x000000000205C1DAull, 0x000000000205C1DAull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x000471C90C7CDB14ull, 0x000471C90C7CDB14ull },
	{ 0x8000000000032DF6ull, 0x8000000000032DF6ull },
	{ 0x800000022F86D04Eull, 0x800000022F86D04Eull },
	{ 0x800001F3625A6088ull, 0x800001F3625A6088ull },
	{ 0x000000000017E493ull, 0x000000000017E493ull },
	{ 0x8003FB2FE5FB6C2Dull, 0x8003FB2FE5FB6C2Dull },
	{ 0x00000D8840B8EC98ull, 0x00000D8840B8EC98ull },
	{ 0x00000000029261DBull, 0x00000000029261DBull },
	{ 0x8000000000D60B89ull, 0x8000000000D60B89ull },
	{ 0x0000000000B80C77ull, 0x0000000000B80C77ull },
	{ 0x0000000000E3CB1Cull, 0x0000000000E3CB1Cull },
	{ 0x0000000001956172ull, 0x0000000001956172ull },
	{ 0x000020BD381B5170ull, 0x000020BD381B5170ull },
	{ 0x8000000000005C20ull, 0x8000000000005C20ull },
	{ 0x00000CE274585AD1ull, 0x00000CE274585AD1ull },
	{ 0x8000000D5104DCD3ull, 0x8000000D5104DCD3ull },
	{ 0x00000F431E047ECDull, 0x00000F431E047ECDull },
	{ 0x000000000000CE7Bull, 0x000000000000CE7Bull },
	{ 0x8000000000000555ull, 0x8000000000000555ull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x00000000234FBCA5ull, 0x00000000234FBCA5ull },
	{ 0x80000000001A2E1Eull, 0x80000000001A2E1Eull },
	{ 0x000000014B442FE1ull, 0x000000014B442FE1ull },
	{ 0x80001ABD6625202Bull, 0x80001ABD6625202Bull },
	{ 0x8001F31ED168C878ull, 0x8001F31ED168C878ull },
	{ 0x00000000000BBD25ull, 0x00000000000BBD25ull },
	{ 0x000000001CABDCFBull, 0x000000001CABDCFBull },
	{ 0x0000000000005861ull, 0x0000000000005861ull },
	{ 0x8000000001491890ull, 0x8000000001491890ull },
	{ 0x80000000040A5892ull, 0x80000000040A5892ull },
	{ 0x800000400BF745A3ull, 0x800000400BF745A3ull },
	{ 0x8000000000000D34ull, 0x8000000000000D34ull },
	{ 0x0000000000016EFAull, 0x0000000000016EFAull },
	{ 0x800000000198FBFEull, 0x800000000198FBFEull },
	{ 0x8000000000007327ull, 0x8000000000007327ull },
	{ 0x8000000000000002ull, 0x8000000000000002ull },
	{ 0x8000000000000081ull, 0x8000000000000081ull },
	{ 0x800002795372E376ull, 0x800002795372E376ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x8000000000000050ull, 0x8000000000000050ull },
	{ 0x80000000000067BAull, 0x80000000000067BAull },
	{ 0x000013ECF2ABE024ull, 0x000013ECF2ABE024ull },
	{ 0x800001BF935FBBF9ull, 0x800001BF935FBBF9ull },
	{ 0x0000000000003406ull, 0x0000000000003406ull },
	{ 0x000000000002460Bull, 0x000000000002460Bull },
	{ 0x8000000DE9457EA6ull, 0x8000000DE9457EA6ull },
	{ 0x8000003FC3F0B6BBull, 0x8000003FC3F0B6BBull },
	{ 0x00000000354D75A9ull, 0x00000000354D75A9ull },
	{ 0x00000000001DD2C3ull, 0x00000000001DD2C3ull },
	{ 0x0000000042DA2230ull, 0x0000000042DA2230ull },
	{ 0x8000000004E7605Aull, 0x8000000004E7605Aull },
	{ 0x0000000090B0CEF9ull, 0x0000000090B0CEF9ull },
	{ 0x800212606D115383ull, 0x800212606D115383ull },
	{ 0x8000000000022C01ull, 0x8000000000022C01ull },
	{ 0x800000000049F853ull, 0x800000000049F853ull },
	{ 0x80000001E0F35365ull, 0x80000001E0F35365ull },
	{ 0x000000000000CBBAull, 0x000000000000CBBAull },
	{ 0x8000000002FDCA1Aull, 0x8000000002FDCA1Aull },
	{ 0x800000000BBCB3F5ull, 0x800000000BBCB3F5ull },
	{ 0x000000614D49C850ull, 0x000000614D49C850ull },
	{ 0x000059E8A1169D6Cull, 0x000059E8A1169D6Cull },
	{ 0x00000065080B529Eull, 0x00000065080B529Eull },
	{ 0x00000000000001E8ull, 0x00000000000001E8ull },
	{ 0x800000001730513Cull, 0x800000001730513Cull },
	{ 0x800CE317FADC8591ull, 0x800CE317FADC8591ull },
	{ 0x8000001A8B68E338ull, 0x8000001A8B68E338ull },
	{ 0x80000CBBF06DB357ull, 0x80000CBBF06DB357ull },
	{ 0x000000009B054413ull, 0x000000009B054413ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x000000000001F568ull, 0x000000000001F568ull },
	{ 0x0000000002538E9Full, 0x0000000002538E9Full },
	{ 0x80000000000008C1ull, 0x80000000000008C1ull },
	{ 0x000002B38D0B5A48ull, 0x000002B38D0B5A48ull },
	{ 0x00000038631E3581ull, 0x00000038631E3581ull },
	{ 0x000000671E2DDD76ull, 0x000000671E2DDD76ull },
	{ 0x8000000139EDC8B8ull, 0x8000000139EDC8B8ull },
	{ 0x8000000020F7C1A4ull, 0x8000000020F7C1A4ull },
	{ 0x0000000388F47D54ull, 0x0000000388F47D54ull },
	{ 0x000000000000000Bull, 0x000000000000000Bull },
	{ 0x8000000000000011ull, 0x8000000000000011ull },
	{ 0x8000000001A0960Bull, 0x8000000001A0960Bull },
	{ 0x800000000016B417ull, 0x800000000016B417ull },
	{ 0x8000000000011C27ull, 0x8000000000011C27ull },
	{ 0x0000005AE5FC22CCull, 0x0000005AE5FC22CCull },
	{ 0x8000000119616C00ull, 0x8000000119616C00ull },
	{ 0x80003FE6C78C5339ull, 0x80003FE6C78C5339ull },
	{ 0x000000749A1F9C45ull, 0x000000749A1F9C45ull },
	{ 0x000000000185FCBAull, 0x000000000185FCBAull },
	{ 0x8000000000000218ull, 0x8000000000000218ull },
	{ 0x000000000003B756ull, 0x000000000003B756ull },
	{ 0x00000001C7A0EAACull, 0x00000001C7A0EAACull },
	{ 0x8000000063D992F3ull, 0x8000000063D992F3ull },
	{ 0x0000000000000065ull, 0x0000000000000065ull },
	{ 0x800000002B8A0752ull, 0x800000002B8A0752ull },
	{ 0x00000000299BF3D5ull, 0x00000000299BF3D5ull },
	{ 0x00000001C913E15Dull, 0x00000001C913E15Dull },
	{ 0x800000010751D720ull, 0x800000010751D720ull },
	{ 0x000000001D6ED2A7ull, 0x000000001D6ED2A7ull },
	{ 0x000000000000015Full, 0x000000000000015Full },
	{ 0x800DB2F9206D1791ull, 0x800DB2F9206D1791ull },
	{ 0x8000000001E9FACCull, 0x8000000001E9FACCull },
	{ 0x80000000003CF764ull, 0x80000000003CF764ull },
	{ 0x0000000002682EB8ull, 0x0000000002682EB8ull },
	{ 0x800000000167C7CBull, 0x800000000167C7CBull },
	{ 0x8000329433A40661ull, 0x8000329433A40661ull },
	{ 0x80000000000028C3ull, 0x80000000000028C3ull },
	{ 0x8004CA92A76F2FF4ull, 0x8004CA92A76F2FF4ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x000000000000002Bull, 0x000000000000002Bull },
	{ 0x800000000004CD1Aull, 0x800000000004CD1Aull },
	{ 0x000000025B4CD5BDull, 0x000000025B4CD5BDull },
	{ 0x000000000002E753ull, 0x000000000002E753ull },
	{ 0x8000000000000083ull, 0x8000000000000083ull },
	{ 0x925D23D73313116Bull, 0x925D23D73313116Bull },
	{ 0x5A03641B4A9DCFF4ull, 0x5A03641B4A9DCFF4ull },
	{ 0xCAC40DA1ABC84504ull, 0xCAC40DA1ABC84504ull },
	{ 0x3867AFA5D9343E3Dull, 0x3867AFA5D9343E3Dull },
	{ 0xB26C12E63924A805ull, 0xB26C12E63924A805ull },
	{ 0xB8D5FD7583ED0C8Aull, 0xB8D5FD7583ED0C8Aull },
	{ 0x24D0BE397BD13395ull, 0x24D0BE397BD13395ull },
	{ 0x858F0001FA92DD8Eull, 0x858F0001FA92DD8Eull },
	{ 0xB0D866334A4BA3D0ull, 0xB0D866334A4BA3D0ull },
	{ 0xC8076ECED1D0F52Eull, 0xC8076ECED1D0F52Eull },
	{ 0xF0831EB50F403545ull, 0xF0831EB50F403545ull },
	{ 0x80E7F54BE330F983ull, 0x80E7F54BE330F983ull },
	{ 0x6FDDFD26B8191F0Aull, 0x6FDDFD26B8191F0Aull },
	{ 0x212445BFD2990988ull, 0x212445BFD2990988ull },
	{ 0x513838B41BF0CDCBull, 0x513838B41BF0CDCBull },
	{ 0x194F8CCD787C075Bull, 0x194F8CCD787C075Bull },
	{ 0xE3C53183B5B35B06ull, 0xE3C53183B5B35B06ull },
	{ 0x89E39D6AB9CF5FFEull, 0x89E39D6AB9CF5FFEull },
	{ 0x95AC42D1C1851F32ull, 0x95AC42D1C1851F32ull },
	{ 0xE749AE02AFC23F6Cull, 0xE749AE02AFC23F6Cull },
	{ 0x7F41CA606CAEC92Bull, 0x7F41CA606CAEC92Bull },
	{ 0x2BE5CFF3541A8BC2ull, 0x2BE5CFF3541A8BC2ull },
	{ 0x5B355882B51C2431ull, 0x5B355882B51C2431ull },
	{ 0x50821E4A6107A443ull, 0x50821E4A6107A443ull },
	{ 0xFE59E02B4A42D568ull, 0xFE59E02B4A42D568ull },
	{ 0x4403EF86331D22D3ull, 0x4403EF86331D22D3ull },
	{ 0xCFB7E67F6CB11B57ull, 0xCFB7E67F6CB11B57ull },
	{ 0xE3CB041AA6759AFEull, 0xE3CB041AA6759AFEull },
	{ 0xC0D9B30DCCC48E6Bull, 0xC0D9B30DCCC48E6Bull },
	{ 0x734AB6D9F8BD5A71ull, 0x734AB6D9F8BD5A71ull },
	{ 0x453180896D48E0F3ull, 0x453180896D48E0F3ull },
	{ 0x142D2D81A8E5287Eull, 0x142D2D81A8E5287Eull },
	{ 0x4AB9ABA881BEA024ull, 0x4AB9ABA881BEA024ull },
	{ 0x3E70880B567D079Aull, 0x3E70880B567D079Aull },
	{ 0x318EE3A74D49F315ull, 0x318EE3A74D49F315ull },
	{ 0x387E1101A276F360ull, 0x387E1101A276F360ull },
	{ 0xC3F45A2F064959C1ull, 0xC3F45A2F064959C1ull },
	{ 0x8E9B6D990E51A08Eull, 0x8E9B6D990E51A08Eull },
	{ 0xAEA1F3C0B2946F0Aull, 0xAEA1F3C0B2946F0Aull },
	{ 0x8BA5CD2CDD2B4005ull, 0x8BA5CD2CDD2B4005ull },
	{ 0xF87C77240922A833ull, 0xF87C77240922A833ull },
	{ 0x30A719DFF23C3B4Bull, 0x30A719DFF23C3B4Bull },
	{ 0xFA87BD9A525A12E3ull, 0xFA87BD9A525A12E3ull },
	{ 0x2CC52821B279F50Aull, 0x2CC52821B279F50Aull },
	{ 0x5832E2D5478618A7ull, 0x5832E2D5478618A7ull },
	{ 0x558AE736E3A18C10ull, 0x558AE736E3A18C10ull },
	{ 0xB0892D87A6555DC6ull, 0xB0892D87A6555DC6ull },
	{ 0xE0B405DF6D7D7EFCull, 0xE0B405DF6D7D7EFCull },
	{ 0x4C5E6DC51A50A3B2ull, 0x4C5E6DC51A50A3B2ull },
	{ 0x67B5E1C67F6375E1ull, 0x67B5E1C67F6375E1ull },
	{ 0x9C6464D520F47110ull, 0x9C6464D520F47110ull },
	{ 0x621490AE3423BEF9ull, 0x621490AE3423BEF9ull },
	{ 0xC0032F5D7DDA5997ull, 0xC0032F5D7DDA5997ull },
	{ 0x91DC5922F7C51EF0ull, 0x91DC5922F7C51EF0ull },
	{ 0xC650D2C9D2222890ull, 0xC650D2C9D2222890ull },
	{ 0x5B602717D81C6708ull, 0x5B602717D81C6708ull },
	{ 0x1DB327FA69B5D529ull, 0x1DB327FA69B5D529ull },
	{ 0x6C70587800D64D01ull, 0x6C70587800D64D01ull },
	{ 0x34ABA29E9CFB2745ull, 0x34ABA29E9CFB2745ull },
	{ 0xA3DAEF958DEFE6A7ull, 0xA3DAEF958DEFE6A7ull },
	{ 0xDFF1B4D90C532070ull, 0xDFF1B4D90C532070ull },
	{ 0x07C25F4FA45EE1B9ull, 0x07C25F4FA45EE1B9ull },
	{ 0xB67211EDE2C3B824ull, 0xB67211EDE2C3B824ull },
	{ 0xAC9CAFDA0F6A4D2Dull, 0xAC9CAFDA0F6A4D2Dull },
	{ 0x6C8230D838032231ull, 0x6C8230D838032231ull },
	{ 0x0B48BC398C4273BEull, 0x0B48BC398C4273BEull },
	{ 0xF5670CBA0886329Dull, 0xF5670CBA0886329Dull },
	{ 0xE7C29BA76F04239Dull, 0xE7C29BA76F04239Dull },
	{ 0x5C05153C95FE1F21ull, 0x5C05153C95FE1F21ull },
	{ 0xA984926BFF5A6CECull, 0xA984926BFF5A6CECull },
	{ 0x1A221E84C6DE412Bull, 0x1A221E84C6DE412Bull },
	{ 0x330A010CDA685F43ull, 0x330A010CDA685F43ull },
	{ 0xF12951BB86E9D2DCull, 0xF12951BB86E9D2DCull },
	{ 0xBA34542F523ACF2Dull, 0xBA34542F523ACF2Dull },
	{ 0x64918879F6397D21ull, 0x64918879F6397D21ull },
	{ 0xF29164578A4C78EDull, 0xF29164578A4C78EDull },
	{ 0xE2152CFF241CECA4ull, 0xE2152CFF241CECA4ull },
	{ 0xFE5A160FEF187E80ull, 0xFE5A160FEF187E80ull },
	{ 0xBC05189FD2655DD6ull, 0xBC05189FD2655DD6ull },
	{ 0x8380195B6C67EAD1ull, 0x8380195B6C67EAD1ull },
	{ 0x6AFB88D856AD2564ull, 0x6AFB88D856AD2564ull },
	{ 0x45800A4738163A72ull, 0x45800A4738163A72ull },
	{ 0x45DFA408A8F3313Cull, 0x45DFA408A8F3313Cull },
	{ 0xF4DDF5F7E51ABB8Aull, 0xF4DDF5F7E51ABB8Aull },
	{ 0xBCBEF98E4EED219Cull, 0xBCBEF98E4EED219Cull },
	{ 0xC743C992C31050D3ull, 0xC743C992C31050D3ull },
	{ 0x7104F0F7BAD3089Bull, 0x7104F0F7BAD3089Bull },
	{ 0x8FC1C31B3F452B9Bull, 0x8FC1C31B3F452B9Bull },
	{ 0xCD743A4BAC332A1Full, 0xCD743A4BAC332A1Full },
	{ 0x56B0E0A4431291C5ull, 0x56B0E0A4431291C5ull },
	{ 0x9DD23E248F0DBBE6ull, 0x9DD23E248F0DBBE6ull },
	{ 0xF187515595FC99C6ull, 0xF187515595FC99C6ull },
	{ 0xA7648DDFDCF8A57Full, 0xA7648DDFDCF8A57Full },
	{ 0x9AFEE4653AD9ECABull, 0x9AFEE4653AD9ECABull },
	{ 0x407052817BCB4150ull, 0x407052817BCB4150ull },
	{ 0x0B3D79C4D3438590ull, 0x0B3D79C4D3438590ull },
	{ 0xE8BBBECB1FD21B10ull, 0xE8BBBECB1FD21B10ull },
	{ 0x8F5F71C2FD0C7128ull, 0x8F5F71C2FD0C7128ull },
	{ 0x61017D07A88AB14Cull, 0x61017D07A88AB14Cull },
	{ 0x22041D40B2C18E3Aull, 0x22041D40B2C18E3Aull },
	{ 0xC3A6206A83A73365ull, 0xC3A6206A83A73365ull },
	{ 0x4F3A24A6BEFB7448ull, 0x4F3A24A6BEFB7448ull },
	{ 0xFE5E8D63E76AB4D0ull, 0xFE5E8D63E76AB4D0ull },
	{ 0xC07329058AFFBEC4ull, 0xC07329058AFFBEC4ull },
	{ 0x7621B6D238DE657Aull, 0x7621B6D238DE657Aull },
	{ 0x46D7AFBF24E4B5C6ull, 0x46D7AFBF24E4B5C6ull },
	{ 0xCE781C67D92BBAB5ull, 0xCE781C67D92BBAB5ull },
	{ 0xE2912719673A576Bull, 0xE2912719673A576Bull },
	{ 0xCEA37A032E800B67ull, 0xCEA37A032E800B67ull },
	{ 0x73A66A49A9B6E154ull, 0x73A66A49A9B6E154ull },
	{ 0xCA8336F3C939012Dull, 0xCA8336F3C939012Dull },
	{ 0xC0DA967C2BF5F463ull, 0xC0DA967C2BF5F463ull },
	{ 0xED86A8E1E53D6A4Cull, 0xED86A8E1E53D6A4Cull },
	{ 0x555AC9026B0EDBA0ull, 0x555AC9026B0EDBA0ull },
	{ 0xF2DE463D56F09581ull, 0xF2DE463D56F09581ull },
	{ 0x57E68E869B09F305ull, 0x57E68E869B09F305ull },
	{ 0x41FF27167CE64EBAull, 0x41FF27167CE64EBAull },
	{ 0xC3D14E994F81417Bull, 0xC3D14E994F81417Bull },
	{ 0x4442A3CA20CA3522ull, 0x4442A3CA20CA3522ull },
	{ 0x736749E9A521031Cull, 0x736749E9A521031Cull },
	{ 0x5AA9BEBB7837D4DDull, 0x5AA9BEBB7837D4DDull },
	{ 0x412CA52E2F8F32EBull, 0x412CA52E2F8F32EBull },
	{ 0x553408BFA44E1D46ull, 0x553408BFA44E1D46ull },
	{ 0x46AA140DF7F3B70Full, 0x46AA140DF7F3B70Full },
	{ 0xDA841D3DAFD03044ull, 0xDA841D3DAFD03044ull },
	{ 0xBF7B23FF1D5E7C8Aull, 0xBF7B23FF1D5E7C8Aull },
	{ 0xE341DA0669DC092Eull, 0xE341DA0669DC092Eull },
	{ 0xE76BC5C9B679A8B3ull, 0xE76BC5C9B679A8B3ull },
	{ 0xC6732744A62F74E6ull, 0xC6732744A62F74E6ull },
	{ 0xC56DD6CE51F83A7Full, 0xC56DD6CE51F83A7Full },
	{ 0xCDF4912743D56FFAull, 0xCDF4912743D56FFAull },
	{ 0xFA7AEFFB5F09F33Aull, 0xFA7AEFFB5F09F33Aull },
	{ 0x747A14F84AA2D63Aull, 0x747A14F84AA2D63Aull },
	{ 0xFE18D3F7ED24BC4Full, 0xFE18D3F7ED24BC4Full },
	{ 0xFC2D091E94AC2A20ull, 0xFC2D091E94AC2A20ull },
	{ 0x55757E5376DF6F4Bull, 0x55757E5376DF6F4Bull },
	{ 0xCA96D7E7378B4744ull, 0xCA96D7E7378B4744ull },
	{ 0xCF81299C12D145FFull, 0xCF81299C12D145FFull },
	{ 0x4D2976BDF99479D9ull, 0x4D2976BDF99479D9ull },
	{ 0xF1CE31B03FC5C3C2ull, 0xF1CE31B03FC5C3C2ull },
	{ 0xE6346B005DED2A00ull, 0xE6346B005DED2A00ull },
	{ 0xFE31D34A4CCE2C09ull, 0xFE31D34A4CCE2C09ull },
	{ 0x7CA01E2032D1BF8Dull, 0x7CA01E2032D1BF8Dull },
	{ 0xD9128DB987DCB5D4ull, 0xD9128DB987DCB5D4ull },
	{ 0xE6F543799F6EA951ull, 0xE6F543799F6EA951ull },
	{ 0x503FA78BADD6A0E6ull, 0x503FA78BADD6A0E6ull },
	{ 0xBF4F7473E4BA9D8Eull, 0xBF4F7473E4BA9D8Eull },
	{ 0xD5BAA3D66446B6EBull, 0xD5BAA3D66446B6EBull },
	{ 0xE46556266682EA75ull, 0xE46556266682EA75ull },
	{ 0xFD63480EFC0D420Eull, 0xFD63480EFC0D420Eull },
	{ 0xDCB1A3444AF2D678ull, 0xDCB1A3444AF2D678ull },
	{ 0xF87AF67CDEDDF629ull, 0xF87AF67CDEDDF629ull },
	{ 0x73DB68F135FB68D0ull, 0x73DB68F135FB68D0ull },
	{ 0xD796D0CF122DFA80ull, 0xD796D0CF122DFA80ull },
	{ 0xFE7961AC80725138ull, 0xFE7961AC80725138ull },
	{ 0x45C0967AAF689063ull, 0x45C0967AAF689063ull },
	{ 0xD2994F32474B1398ull, 0xD2994F32474B1398ull },
	{ 0xEFCE375DFBB9DD85ull, 0xEFCE375DFBB9DD85ull },
	{ 0xD4F18409D8229CCFull, 0xD4F18409D8229CCFull },
	{ 0xDE3E433E46726FAAull, 0xDE3E433E46726FAAull },
	{ 0xF94D65653C43D82Aull, 0xF94D65653C43D82Aull },
	{ 0x5F438E9708782157ull, 0x5F438E9708782157ull },
	{ 0xF0724B9E704887A2ull, 0xF0724B9E704887A2ull },
	{ 0xF7699CC4667D1283ull, 0xF7699CC4667D1283ull },
	{ 0x64FC4CB71FA96A5Full, 0x64FC4CB71FA96A5Full },
	{ 0xC46693EABD01C19Eull, 0xC46693EABD01C19Eull },
	{ 0xCF55F844303422ABull, 0xCF55F844303422ABull },
	{ 0xE99EAB54CB638FFBull, 0xE99EAB54CB638FFBull },
	{ 0x4AFF67210BC99FA5ull, 0x4AFF67210BC99FA5ull },
	{ 0x6D33074B17E4478Full, 0x6D33074B17E4478Full },
	{ 0xD15F4F1E0824B252ull, 0xD15F4F1E0824B252ull },
	{ 0xF750590FE7AAAFBDull, 0xF750590FE7AAAFBDull },
	{ 0xFDA499106315A563ull, 0xFDA499106315A563ull },
	{ 0xD8D6B9C0B68CFEB3ull, 0xD8D6B9C0B68CFEB3ull },
	{ 0xD5035D5346312667ull, 0xD5035D5346312667ull },
	{ 0xD1C3867CDE21DC3Dull, 0xD1C3867CDE21DC3Dull },
	{ 0x6673B5A8D980C6D0ull, 0x6673B5A8D980C6D0ull },
	{ 0xC03EF781142524EDull, 0xC03EF781142524EDull },
	{ 0xF723690D051FFCDFull, 0xF723690D051FFCDFull },
	{ 0x54E2A704DEF96F21ull, 0x54E2A704DEF96F21ull },
	{ 0xE02B6DB62A3CD955ull, 0xE02B6DB62A3CD955ull },
	{ 0xFB8B065BE935EE01ull, 0xFB8B065BE935EE01ull },
	{ 0x5BE346748B0E32BBull, 0x5BE346748B0E32BBull },
	{ 0x6653AD666D83F8DDull, 0x6653AD666D83F8DDull },
	{ 0x442F1E1E8543A624ull, 0x442F1E1E8543A624ull },
	{ 0x5CD8BA67E89BE12Bull, 0x5CD8BA67E89BE12Bull },
	{ 0x7BC33D2D5A9C9E74ull, 0x7BC33D2D5A9C9E74ull },
	{ 0x476EE6F531E80701ull, 0x476EE6F531E80701ull },
	{ 0x4E235B523A62A0D9ull, 0x4E235B523A62A0D9ull },
	{ 0xC8ADCD97F647E30Full, 0xC8ADCD97F647E30Full },
	{ 0xCDD134FC016F12CCull, 0xCDD134FC016F12CCull },
	{ 0xF9FFF5DA13A33D7Full, 0xF9FFF5DA13A33D7Full },
	{ 0x654AB36E88E470D6ull, 0x654AB36E88E470D6ull },
	{ 0xE9FDE9CAC3416D04ull, 0xE9FDE9CAC3416D04ull },
	{ 0x5A0493B159A89F4Dull, 0x5A0493B159A89F4Dull },
	{ 0x7F808F9D97866F1Aull, 0x7F808F9D97866F1Aull },
	{ 0x522045675018F3F2ull, 0x522045675018F3F2ull },
	{ 0x64559A2CD4AE8806ull, 0x64559A2CD4AE8806ull },
	{ 0x48ECD6CD88837370ull, 0x48ECD6CD88837370ull },
	{ 0xFAB19432B7CC3755ull, 0xFAB19432B7CC3755ull },
	{ 0xE4A9CF2ABEED7BECull, 0xE4A9CF2ABEED7BECull },
	{ 0x5C90546C4FBBD241ull, 0x5C90546C4FBBD241ull },
	{ 0xFCDC1CDC126463CAull, 0xFCDC1CDC126463CAull },
	{ 0xCD272F94975F6639ull, 0xCD272F94975F6639ull },
	{ 0x67F81EA4B85992F3ull, 0x67F81EA4B85992F3ull },
	{ 0x456EED08C96683C4ull, 0x456EED08C96683C4ull },
	{ 0x68BB39B2294B997Dull, 0x68BB39B2294B997Dull },
	{ 0xF142001C89C9F057ull, 0xF142001C89C9F057ull },
	{ 0x5550DCA31002FF08ull, 0x5550DCA31002FF08ull },
	{ 0x4367784327BEBE40ull, 0x4367784327BEBE40ull },
	{ 0xC52D85763D4C459Cull, 0xC52D85763D4C459Cull },
	{ 0x4DCE5B0FF2401C52ull, 0x4DCE5B0FF2401C52ull },
	{ 0x6102FCDA78CA32A2ull, 0x6102FCDA78CA32A2ull },
	{ 0xC7F0E3BAFD48A374ull, 0xC7F0E3BAFD48A374ull },
	{ 0xC26DF58EDFDFE171ull, 0xC26DF58EDFDFE171ull },
	{ 0x59ECCDA8509EF5A6ull, 0x59ECCDA8509EF5A6ull },
	{ 0xE95B99B4F1EC7313ull, 0xE95B99B4F1EC7313ull },
	{ 0xC9B1A7A9ADE80B43ull, 0xC9B1A7A9ADE80B43ull },
	{ 0x7933C2D5AAB4C2DEull, 0x7933C2D5AAB4C2DEull },
	{ 0x5A3B9AFCD426CA6Dull, 0x5A3B9AFCD426CA6Dull },
	{ 0xD4F2393B116C2450ull, 0xD4F2393B116C2450ull },
	{ 0xC46892EA630491EAull, 0xC46892EA630491EAull },
	{ 0xC93953A2DCAAA435ull, 0xC93953A2DCAAA435ull },
	{ 0xFD39E6E5D52B26E4ull, 0xFD39E6E5D52B26E4ull },
	{ 0xF500A79584527C7Full, 0xF500A79584527C7Full },
	{ 0x44CC06E105AFE4ADull, 0x44CC06E105AFE4ADull },
	{ 0x5E121860D3CAA097ull, 0x5E121860D3CAA097ull },
	{ 0x7E3432A0440A8C83ull, 0x7E3432A0440A8C83ull },
	{ 0xE7972D0FE5DAA189ull, 0xE7972D0FE5DAA189ull },
	{ 0xFE94B3398F48B22Aull, 0xFE94B3398F48B22Aull },
	{ 0x496B23BB23D5AA28ull, 0x496B23BB23D5AA28ull },
	{ 0x4A8B6A59DDD9E7DEull, 0x4A8B6A59DDD9E7DEull },
	{ 0x796093F283492A37ull, 0x796093F283492A37ull },
	{ 0xEF6A2E3EDF1E0994ull, 0xEF6A2E3EDF1E0994ull },
	{ 0xC2943ECE510ED5AEull, 0xC2943ECE510ED5AEull },
	{ 0xCBD8D2F6429190B9ull, 0xCBD8D2F6429190B9ull },
	{ 0x5437F40EF747CB50ull, 0x5437F40EF747CB50ull },
	{ 0xBEF0EBC216010359ull, 0xBEF0EBC216010359ull },
	{ 0x599A826199DFCB36ull, 0x599A826199DFCB36ull },
	{ 0xFC9C24A88261726Bull, 0xFC9C24A88261726Bull },
	{ 0x7013CFBC20BEBCB0ull, 0x7013CFBC20BEBCB0ull },
	{ 0x4B3AFC8891E2F585ull, 0x4B3AFC8891E2F585ull },
	{ 0x7F40F583F2252C6Full, 0x7F40F583F2252C6Full },
	{ 0xC3106E656F3A33C1ull, 0xC3106E656F3A33C1ull },
	{ 0xBD8D2C1702F279F4ull, 0xBD8D2C1702F279F4ull },
	{ 0xC06B993626858507ull, 0xC06B993626858507ull },
	{ 0x404A03A3C297C3F3ull, 0x404A03A3C297C3F3ull },
	{ 0x4276743D6C307FCFull, 0x4276743D6C307FCFull },
	{ 0xC0C3F3DD90BD7D57ull, 0xC0C3F3DD90BD7D57ull },
	{ 0xC01C5F8DDA6F7A10ull, 0xC01C5F8DDA6F7A10ull },
	{ 0x4160E7F4E701F501ull, 0x4160E7F4E701F501ull },
	{ 0xBE65C0D3CAE0F0BFull, 0xBE65C0D3CAE0F0BFull },
	{ 0xC05D0D8E746700C8ull, 0xC05D0D8E746700C8ull },
	{ 0x3EF4B9726634A103ull, 0x3EF4B9726634A103ull },
	{ 0x3FA3301A2F31E739ull, 0x3FA3301A2F31E739ull },
	{ 0x3E4D13910B039FEFull, 0x3E4D13910B039FEFull },
	{ 0xBD2ECF9EEBBB7948ull, 0xBD2ECF9EEBBB7948ull },
	{ 0x3FF553E9826AAFCEull, 0x3FF553E9826AAFCEull },
	{ 0xBF304545BB796E91ull, 0xBF304545BB796E91ull },
	{ 0x415B13D7322E34DAull, 0x415B13D7322E34DAull },
	{ 0x40874F6AEBFAAD36ull, 0x40874F6AEBFAAD36ull },
	{ 0xC1A769569DC453A3ull, 0xC1A769569DC453A3ull },
	{ 0x407C4518EB172CD8ull, 0x407C4518EB172CD8ull },
	{ 0xC11E7EFFF9DB2E9Aull, 0xC11E7EFFF9DB2E9Aull },
	{ 0xBF08DBCBD968083Cull, 0xBF08DBCBD968083Cull },
	{ 0x404E3E3D3C108809ull, 0x404E3E3D3C108809ull },
	{ 0x3F842E82A89E12ACull, 0x3F842E82A89E12ACull },
	{ 0xBF3509049B9F1C8Full, 0xBF3509049B9F1C8Full },
	{ 0x3FBEB16A811186C9ull, 0x3FBEB16A811186C9ull },
	{ 0xBF9CB7C5C83BEE33ull, 0xBF9CB7C5C83BEE33ull },
	{ 0xC237A798B0E0343Dull, 0xC237A798B0E0343Dull },
	{ 0xBE5EED188B14D5BBull, 0xBE5EED188B14D5BBull },
	{ 0xC1ED798CC57A77ABull, 0xC1ED798CC57A77ABull },
	{ 0xBD106E6432D17DEFull, 0xBD106E6432D17DEFull },
	{ 0xC0E9CAAE06367E79ull, 0xC0E9CAAE06367E79ull },
	{ 0xBF561DD69566AF74ull, 0xBF561DD69566AF74ull },
	{ 0x4218D477FFEDDA68ull, 0x4218D477FFEDDA68ull },
	{ 0x40867C1BC7186776ull, 0x40867C1BC7186776ull },
	{ 0xBE35538B10F44403ull, 0xBE35538B10F44403ull },
	{ 0xC07627EF28DB2039ull, 0xC07627EF28DB2039ull },
	{ 0xBEA837158357E320ull, 0xBEA837158357E320ull },
	{ 0x424D92B9E57B4484ull, 0x424D92B9E57B4484ull },
	{ 0x3F99A061EDD32946ull, 0x3F99A061EDD32946ull },
	{ 0x413F16270D5FDEDAull, 0x413F16270D5FDEDAull },
	{ 0x424D21CDB22E72A0ull, 0x424D21CDB22E72A0ull },
	{ 0xC03DC2084B35DDD5ull, 0xC03DC2084B35DDD5ull },
	{ 0xBE081A1DAA91E551ull, 0xBE081A1DAA91E551ull },
	{ 0x3E71D1CE6E9A403Cull, 0x3E71D1CE6E9A403Cull },
	{ 0x3E7E5DBB5DE25BD0ull, 0x3E7E5DBB5DE25BD0ull },
	{ 0x40B424FCE77CC546ull, 0x40B424FCE77CC546ull },
	{ 0x3F2111FBDDF4B992ull, 0x3F2111FBDDF4B992ull },
	{ 0x4066AEC6D6E7FF50ull, 0x4066AEC6D6E7FF50ull },
	{ 0x4110F15D0843E536ull, 0x4110F15D0843E536ull },
	{ 0xC00CA2103964C4A4ull, 0xC00CA2103964C4A4ull },
	{ 0xBDEFA259E5786F78ull, 0xBDEFA259E5786F78ull },
	{ 0xBECF57C3B97E26EAull, 0xBECF57C3B97E26EAull },
	{ 0xBD67D89A9D54F054ull, 0xBD67D89A9D54F054ull },
	{ 0x3F82B919EC88EA7Full, 0x3F82B919EC88EA7Full },
	{ 0xC1BD609F36213122ull, 0xC1BD609F36213122ull },
	{ 0x3F12AC68DA39DBE3ull, 0x3F12AC68DA39DBE3ull },
	{ 0x41A28C2E70A0B730ull, 0x41A28C2E70A0B730ull },
	{ 0x3FD9C53BE777E744ull, 0x3FD9C53BE777E744ull },
	{ 0x40356494579065E4ull, 0x40356494579065E4ull },
	{ 0x3CFA6969864FCE41ull, 0x3CFA6969864FCE41ull },
	{ 0x409A7B21B781CF60ull, 0x409A7B21B781CF60ull },
	{ 0x3DE8F609BF2FC19Full, 0x3DE8F609BF2FC19Full },
	{ 0xC19A79294F5D22B1ull, 0xC19A79294F5D22B1ull },
	{ 0x3EACBF98CDA310C7ull, 0x3EACBF98CDA310C7ull },
	{ 0xC22CB6E88614C42Full, 0xC22CB6E88614C42Full },
	{ 0xBF02330F4905531Full, 0xBF02330F4905531Full },
	{ 0xC218626AA28A78C6ull, 0xC218626AA28A78C6ull },
	{ 0xBFC8F7D64DEB5ECCull, 0xBFC8F7D64DEB5ECCull },
	{ 0xC0E6B212CB470E69ull, 0xC0E6B212CB470E69ull },
	{ 0xBEF978B1000923FFull, 0xBEF978B1000923FFull },
	{ 0x3EDC27B8821052C0ull, 0x3EDC27B8821052C0ull },
	{ 0x426FAFE27DBC213Aull, 0x426FAFE27DBC213Aull },
	{ 0xC062A53ADFC77834ull, 0xC062A53ADFC77834ull },
	{ 0xC01A3738BD6083B0ull, 0xC01A3738BD6083B0ull },
	{ 0x409BA6A8E25A793Eull, 0x409BA6A8E25A793Eull },
	{ 0xC19377A1464A6D88ull, 0xC19377A1464A6D88ull },
	{ 0xC0C214EA13065689ull, 0xC0C214EA13065689ull },
	{ 0xC13458EAC80891A3ull, 0xC13458EAC80891A3ull },
	{ 0x407BC53200ED9077ull, 0x407BC53200ED9077ull },
	{ 0xC30390D5AE0359FBull, 0xC30390D5AE0359FBull },
	{ 0xBFF1D1DA1FBA8541ull, 0xBFF1D1DA1FBA8541ull },
	{ 0x415EB176E978CA04ull, 0x415EB176E978CA04ull },
	{ 0x4064920AF08654B1ull, 0x4064920AF08654B1ull },
	{ 0x41C6B18A080434EEull, 0x41C6B18A080434EEull },
	{ 0x427D69E36AACE925ull, 0x427D69E36AACE925ull },
	{ 0xC2F10AAC0F9512FAull, 0xC2F10AAC0F9512FAull },
	{ 0x404C32B1D236D864ull, 0x404C32B1D236D864ull },
	{ 0x42562D9694E58BD4ull, 0x42562D9694E58BD4ull },
	{ 0xC06FF62ABB0FDD40ull, 0xC06FF62ABB0FDD40ull },
	{ 0x41482D2DDBA7A0D0ull, 0x41482D2DDBA7A0D0ull },
	{ 0x42C88433A0A1C883ull, 0x42C88433A0A1C883ull }
};

CShader5xTest_DoubleArith::Binary64Vec2 const CShader5xTest_dmul::s_DMulRes[] = 
{
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FFF80D73C0502D6ull, 0x7FFF80D73C0502D6ull },
	{ 0x7FF553481B85E7DCull, 0x7FF553481B85E7DCull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x7FF674C8AB439310ull, 0x7FF674C8AB439310ull },
	{ 0x7FF5AD80CAFB1580ull, 0x7FF5AD80CAFB1580ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF36CB88B7C4318ull, 0x7FF36CB88B7C4318ull },
	{ 0x7FF4732FF8DB11B0ull, 0x7FF4732FF8DB11B0ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x80FFB0033342294Aull, 0x80FFB0033342294Aull },
	{ 0xE0E05F62BBC6AD94ull, 0xE0E05F62BBC6AD94ull },
	{ 0x5EA5C04781F842F4ull, 0x5EA5C04781F842F4ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x00083353FB0DCCC2ull, 0x00083353FB0DCCC2ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFEB435432FBB392Full, 0xFEB435432FBB392Full },
	{ 0xAC1C7F78BD2BBF02ull, 0xAC1C7F78BD2BBF02ull },
	{ 0xA729822F13918641ull, 0xA729822F13918641ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xA818DDC007D02721ull, 0xA818DDC007D02721ull },
	{ 0x0D9C199F38EE0B1Full, 0x0D9C199F38EE0B1Full },
	{ 0x42D2788468675A69ull, 0x42D2788468675A69ull },
	{ 0xE29949BF36D79258ull, 0xE29949BF36D79258ull },
	{ 0x7B31CE26327B1116ull, 0x7B31CE26327B1116ull },
	{ 0x3B0FF10E49CA0355ull, 0x3B0FF10E49CA0355ull },
	{ 0x47601FFF493578A8ull, 0x47601FFF493578A8ull },
	{ 0xB881EE510E33181Eull, 0xB881EE510E33181Eull },
	{ 0x404BA172F5AEE37Cull, 0x404BA172F5AEE37Cull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x5142B4B36A0996BDull, 0x5142B4B36A0996BDull },
	{ 0x3D285B6A6FA61255ull, 0x3D285B6A6FA61255ull },
	{ 0xE7FEAD7B140FEEF3ull, 0xE7FEAD7B140FEEF3ull },
	{ 0xABD8F7A67B766CF1ull, 0xABD8F7A67B766CF1ull },
	{ 0xA4CB3D7BE2C9C180ull, 0xA4CB3D7BE2C9C180ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8A8B51ED3B31F3DBull, 0x8A8B51ED3B31F3DBull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x2E5662E64BF65EC0ull, 0x2E5662E64BF65EC0ull },
	{ 0xF3B49E6E5C389934ull, 0xF3B49E6E5C389934ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xF147A7D4546303C9ull, 0xF147A7D4546303C9ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x1A695A0B7F6C76EBull, 0x1A695A0B7F6C76EBull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x9ECC1C6A2D72C2E0ull, 0x9ECC1C6A2D72C2E0ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x59407D90AFFF394Eull, 0x59407D90AFFF394Eull },
	{ 0x1EF8DF10EB58B57Eull, 0x1EF8DF10EB58B57Eull },
	{ 0xFEA81D26EB9C0E27ull, 0xFEA81D26EB9C0E27ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xCE9F97D5500C28DCull, 0xCE9F97D5500C28DCull },
	{ 0x40A6F63592FE1A1Dull, 0x40A6F63592FE1A1Dull },
	{ 0xC33DE890B2889D42ull, 0xC33DE890B2889D42ull },
	{ 0xE0B00AE2DCFBA4F4ull, 0xE0B00AE2DCFBA4F4ull },
	{ 0x40EA83BF7FF432B7ull, 0x40EA83BF7FF432B7ull },
	{ 0x2F06DA6379543295ull, 0x2F06DA6379543295ull },
	{ 0x46D85CAB4BAD6499ull, 0x46D85CAB4BAD6499ull },
	{ 0xCA1603D11E80BC78ull, 0xCA1603D11E80BC78ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xF2945E3D2A4A56DAull, 0xF2945E3D2A4A56DAull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xBBC3BC1C0728E9ADull, 0xBBC3BC1C0728E9ADull },
	{ 0x0000000000002FBAull, 0x0000000000002FBAull },
	{ 0xBB98EF5FDBF64524ull, 0xBB98EF5FDBF64524ull },
	{ 0xBCB7CF6BB9DBEFE4ull, 0xBCB7CF6BB9DBEFE4ull },
	{ 0xBA20CDF12B02B2C3ull, 0xBA20CDF12B02B2C3ull },
	{ 0x1CB907821B4B2953ull, 0x1CB907821B4B2953ull },
	{ 0x62744BD1B4F4C325ull, 0x62744BD1B4F4C325ull },
	{ 0x2BC3BF3A1370BC17ull, 0x2BC3BF3A1370BC17ull },
	{ 0x7194940D85E6B844ull, 0x7194940D85E6B844ull },
	{ 0xE23CCB9F4ABF7A6Eull, 0xE23CCB9F4ABF7A6Eull },
	{ 0xE76A7ABD37CD131Dull, 0xE76A7ABD37CD131Dull },
	{ 0xD357A7D0A3B325D6ull, 0xD357A7D0A3B325D6ull },
	{ 0x366A99C8C1FE18FAull, 0x366A99C8C1FE18FAull },
	{ 0xDCA6AE2A7A1F23B9ull, 0xDCA6AE2A7A1F23B9ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x55F7542B8FC1F4AEull, 0x55F7542B8FC1F4AEull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x41FF974DEEF4AF43ull, 0x41FF974DEEF4AF43ull },
	{ 0xD5D4A18B0DD9191Full, 0xD5D4A18B0DD9191Full },
	{ 0x80000000000176D4ull, 0x80000000000176D4ull },
	{ 0x01C65AA890179FE8ull, 0x01C65AA890179FE8ull },
	{ 0x70EDAC337BB7DFB9ull, 0x70EDAC337BB7DFB9ull },
	{ 0x87DAF8DEFDCE924Aull, 0x87DAF8DEFDCE924Aull },
	{ 0xBD6AB4603FB3C3E4ull, 0xBD6AB4603FB3C3E4ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x2998311777DB67B0ull, 0x2998311777DB67B0ull },
	{ 0x2F2A9AD086A03A44ull, 0x2F2A9AD086A03A44ull },
	{ 0x5F6A34CDB15D8FD2ull, 0x5F6A34CDB15D8FD2ull },
	{ 0x6CEF03BA4C9DB69Dull, 0x6CEF03BA4C9DB69Dull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x952ED596117FDFD3ull, 0x952ED596117FDFD3ull },
	{ 0xD0ECE3A7AE2F3EBEull, 0xD0ECE3A7AE2F3EBEull },
	{ 0x3EA24381F9F2A1FFull, 0x3EA24381F9F2A1FFull },
	{ 0x05C289BA909FB1F2ull, 0x05C289BA909FB1F2ull },
	{ 0xDA2514D573201D41ull, 0xDA2514D573201D41ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x48F7E12047728473ull, 0x48F7E12047728473ull },
	{ 0x9054DB8355A28AD2ull, 0x9054DB8355A28AD2ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x018156BA5D55F7CCull, 0x018156BA5D55F7CCull },
	{ 0x8D51D5D3D8EAF388ull, 0x8D51D5D3D8EAF388ull },
	{ 0xF1F9B9CF717A9BF8ull, 0xF1F9B9CF717A9BF8ull },
	{ 0x5D1523AA97457CC8ull, 0x5D1523AA97457CC8ull },
	{ 0xE75EB555E52E6DECull, 0xE75EB555E52E6DECull },
	{ 0x3B523F74635C6E80ull, 0x3B523F74635C6E80ull },
	{ 0x61B51E8A34466D78ull, 0x61B51E8A34466D78ull },
	{ 0xB78270AEF7B1365Cull, 0xB78270AEF7B1365Cull },
	{ 0x0B6B724F669B198Bull, 0x0B6B724F669B198Bull },
	{ 0xE354A3ABC151EB59ull, 0xE354A3ABC151EB59ull },
	{ 0x031AE7ADD2865FF7ull, 0x031AE7ADD2865FF7ull },
	{ 0x8BEE1E8C6CA14C75ull, 0x8BEE1E8C6CA14C75ull },
	{ 0x3DA35BEEE1EA059Cull, 0x3DA35BEEE1EA059Cull },
	{ 0x3C551F8DA6E95060ull, 0x3C551F8DA6E95060ull },
	{ 0x4C4B8789B4DB5781ull, 0x4C4B8789B4DB5781ull },
	{ 0xAF59FC1D1D3E0225ull, 0xAF59FC1D1D3E0225ull },
	{ 0x2AC02B8400E408FBull, 0x2AC02B8400E408FBull },
	{ 0x341869CF825E2AC6ull, 0x341869CF825E2AC6ull },
	{ 0xC748303893449174ull, 0xC748303893449174ull },
	{ 0xE3FD7D47ED26C80Aull, 0xE3FD7D47ED26C80Aull },
	{ 0xB2E5854768C36060ull, 0xB2E5854768C36060ull },
	{ 0x534AA4985D05CB61ull, 0x534AA4985D05CB61ull },
	{ 0x50A9CED1481854E9ull, 0x50A9CED1481854E9ull },
	{ 0xC04768A38B01218Full, 0xC04768A38B01218Full },
	{ 0x8683BEE069BE4322ull, 0x8683BEE069BE4322ull },
	{ 0xC44288494D0ED7F1ull, 0xC44288494D0ED7F1ull },
	{ 0x6CC81823F0EF6DF0ull, 0x6CC81823F0EF6DF0ull },
	{ 0x82AA7562FF4755CAull, 0x82AA7562FF4755CAull },
	{ 0x000000000014BD3Aull, 0x000000000014BD3Aull },
	{ 0x59806FD23AE27DA6ull, 0x59806FD23AE27DA6ull },
	{ 0xC7CBCD41D289E82Cull, 0xC7CBCD41D289E82Cull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x99D3442F398F62DBull, 0x99D3442F398F62DBull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xB634E04E4281328Full, 0xB634E04E4281328Full },
	{ 0xFAD27EA67A23042Bull, 0xFAD27EA67A23042Bull },
	{ 0x022B5CC8BCA32CA9ull, 0x022B5CC8BCA32CA9ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x34887D3875A62650ull, 0x34887D3875A62650ull },
	{ 0x8772DCF18BD373CEull, 0x8772DCF18BD373CEull },
	{ 0x020E10A495606448ull, 0x020E10A495606448ull },
	{ 0xE70E1850C1D37952ull, 0xE70E1850C1D37952ull },
	{ 0x5F923A20BEAB38D0ull, 0x5F923A20BEAB38D0ull },
	{ 0xA21797D9C8E3E3C1ull, 0xA21797D9C8E3E3C1ull },
	{ 0xFFD36E70DE7A2DF7ull, 0xFFD36E70DE7A2DF7ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x3F7FA335CA3931F8ull, 0x3F7FA335CA3931F8ull },
	{ 0x7277A8E2DE56C5B1ull, 0x7277A8E2DE56C5B1ull },
	{ 0x15A5A103BD92ED44ull, 0x15A5A103BD92ED44ull },
	{ 0x41FA72511474AE15ull, 0x41FA72511474AE15ull },
	{ 0x43B88966648FC3ECull, 0x43B88966648FC3ECull },
	{ 0x40F57620DD14F329ull, 0x40F57620DD14F329ull },
	{ 0x05F7C141C777992Eull, 0x05F7C141C777992Eull },
	{ 0x7B13A2D08E51E8EDull, 0x7B13A2D08E51E8EDull },
	{ 0xC2AF5F56EDA772B6ull, 0xC2AF5F56EDA772B6ull },
	{ 0x800000000002FCEDull, 0x800000000002FCEDull },
	{ 0x3B02B0E5A6F7EF58ull, 0x3B02B0E5A6F7EF58ull },
	{ 0x89F0A1C1D809F5E5ull, 0x89F0A1C1D809F5E5ull },
	{ 0x3A95263EC97995FCull, 0x3A95263EC97995FCull },
	{ 0xC1D4CA00A8A201ECull, 0xC1D4CA00A8A201ECull },
	{ 0xC75534F3FF430D7Cull, 0xC75534F3FF430D7Cull },
	{ 0xDADE7074063C677Full, 0xDADE7074063C677Full },
	{ 0x51D6001679173EA3ull, 0x51D6001679173EA3ull },
	{ 0xCCDCE391E536687Dull, 0xCCDCE391E536687Dull },
	{ 0x653E9979C7120D84ull, 0x653E9979C7120D84ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x56E6E14B808388B2ull, 0x56E6E14B808388B2ull },
	{ 0x0784581FA6C0DFBCull, 0x0784581FA6C0DFBCull },
	{ 0xF7051E8B2EB3B9FAull, 0xF7051E8B2EB3B9FAull },
	{ 0x3C3BC995E17D7178ull, 0x3C3BC995E17D7178ull },
	{ 0xC013D17F56C90C6Eull, 0xC013D17F56C90C6Eull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xABC19C3818D249F0ull, 0xABC19C3818D249F0ull },
	{ 0x47F666211F6DAE9Dull, 0x47F666211F6DAE9Dull },
	{ 0x2255D129D4A5CFF0ull, 0x2255D129D4A5CFF0ull },
	{ 0xE049DCA1843087B7ull, 0xE049DCA1843087B7ull },
	{ 0x40DE5D6BC248C4AEull, 0x40DE5D6BC248C4AEull },
	{ 0x93E48884519EF1E6ull, 0x93E48884519EF1E6ull },
	{ 0x8F1F2360B1D2E40Eull, 0x8F1F2360B1D2E40Eull },
	{ 0xED384EDFA45B9650ull, 0xED384EDFA45B9650ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x859CB014FB0F9A43ull, 0x859CB014FB0F9A43ull },
	{ 0xAC0CE82C101F3417ull, 0xAC0CE82C101F3417ull },
	{ 0x7E68AF132B3C0917ull, 0x7E68AF132B3C0917ull },
	{ 0xA9CCDE6951FA083Aull, 0xA9CCDE6951FA083Aull },
	{ 0xCB09850A8DA0D33Dull, 0xCB09850A8DA0D33Dull },
	{ 0xD423DBD2E90421B0ull, 0xD423DBD2E90421B0ull },
	{ 0x99BB3EE44C8CF29Dull, 0x99BB3EE44C8CF29Dull },
	{ 0x088836A2E16FAF2Eull, 0x088836A2E16FAF2Eull },
	{ 0x02F02597E55531E1ull, 0x02F02597E55531E1ull },
	{ 0xE687C28DF306C689ull, 0xE687C28DF306C689ull },
	{ 0x39CA31603604D971ull, 0x39CA31603604D971ull },
	{ 0x00B900D8C2083448ull, 0x00B900D8C2083448ull },
	{ 0x3B9122F7F1054245ull, 0x3B9122F7F1054245ull },
	{ 0x0B97894334E037A3ull, 0x0B97894334E037A3ull },
	{ 0x008EE018618519A3ull, 0x008EE018618519A3ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xAB56AEDA2A89980Aull, 0xAB56AEDA2A89980Aull },
	{ 0x938777F5B07A4936ull, 0x938777F5B07A4936ull },
	{ 0xB8C9F4CBB54F53C8ull, 0xB8C9F4CBB54F53C8ull },
	{ 0x56C659655545A95Bull, 0x56C659655545A95Bull },
	{ 0xB09F7C00966722D1ull, 0xB09F7C00966722D1ull },
	{ 0xF6E8E795E78DF2F7ull, 0xF6E8E795E78DF2F7ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x55F9DD1790030A28ull, 0x55F9DD1790030A28ull },
	{ 0xBD1A492B7043C1C2ull, 0xBD1A492B7043C1C2ull },
	{ 0xC91F9F18FDD7EEFAull, 0xC91F9F18FDD7EEFAull },
	{ 0xCFC0C8184F439865ull, 0xCFC0C8184F439865ull },
	{ 0xC930104379946ED3ull, 0xC930104379946ED3ull },
	{ 0xD0AF8BAB5F7AAAC6ull, 0xD0AF8BAB5F7AAAC6ull },
	{ 0x4E6ACB3D92FDAA0Full, 0x4E6ACB3D92FDAA0Full },
	{ 0x0C4C81564D1BA4AFull, 0x0C4C81564D1BA4AFull },
	{ 0x90D42EE278991386ull, 0x90D42EE278991386ull },
	{ 0x90D5C4FF1826AC3Eull, 0x90D5C4FF1826AC3Eull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x4AE57AD97E1890BFull, 0x4AE57AD97E1890BFull },
	{ 0x71D3A198953AC253ull, 0x71D3A198953AC253ull },
	{ 0xE8E0C37C673332BDull, 0xE8E0C37C673332BDull },
	{ 0xCE317FCDDD7F12EBull, 0xCE317FCDDD7F12EBull },
	{ 0x6B832A69BFA31703ull, 0x6B832A69BFA31703ull },
	{ 0x571C4B62E1758676ull, 0x571C4B62E1758676ull },
	{ 0x718AEF9677821092ull, 0x718AEF9677821092ull },
	{ 0x6626CB13112CDA1Full, 0x6626CB13112CDA1Full },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x716A97006557AFFEull, 0x716A97006557AFFEull },
	{ 0xB3B2C17C6D2D2875ull, 0xB3B2C17C6D2D2875ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x8000000000000B2Bull, 0x8000000000000B2Bull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x00000017AAECB2F9ull, 0x00000017AAECB2F9ull },
	{ 0x0000000151DED81Dull, 0x0000000151DED81Dull },
	{ 0x00025E5D8B587762ull, 0x00025E5D8B587762ull },
	{ 0x00000153E0DDBD62ull, 0x00000153E0DDBD62ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x000000000000050Aull, 0x000000000000050Aull },
	{ 0x80000000000002C0ull, 0x80000000000002C0ull },
	{ 0x8000000000003802ull, 0x8000000000003802ull },
	{ 0x800007DA0254E8B9ull, 0x800007DA0254E8B9ull },
	{ 0x000000000000472Aull, 0x000000000000472Aull },
	{ 0x00002FF642677BF1ull, 0x00002FF642677BF1ull },
	{ 0x000000006AB55934ull, 0x000000006AB55934ull },
	{ 0x8000000000000005ull, 0x8000000000000005ull },
	{ 0x0000002E1A929CE5ull, 0x0000002E1A929CE5ull },
	{ 0x0000000E9C87148Eull, 0x0000000E9C87148Eull },
	{ 0x800000146CE4386Aull, 0x800000146CE4386Aull },
	{ 0x0000000098E4ADC5ull, 0x0000000098E4ADC5ull },
	{ 0x000001D9AE35A072ull, 0x000001D9AE35A072ull },
	{ 0x800000000013C2AEull, 0x800000000013C2AEull },
	{ 0x80000000000028C8ull, 0x80000000000028C8ull },
	{ 0x8000000000024C10ull, 0x8000000000024C10ull },
	{ 0x0000005D8C4D30DCull, 0x0000005D8C4D30DCull },
	{ 0x0000000000002E63ull, 0x0000000000002E63ull },
	{ 0x8000000000000010ull, 0x8000000000000010ull },
	{ 0x0000000000069E48ull, 0x0000000000069E48ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x80014F37EEB029EEull, 0x80014F37EEB029EEull },
	{ 0x8000860149EC3C02ull, 0x8000860149EC3C02ull },
	{ 0x000AABAB92C1F47Bull, 0x000AABAB92C1F47Bull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x800000000000007Bull, 0x800000000000007Bull },
	{ 0x0000000541BAFF84ull, 0x0000000541BAFF84ull },
	{ 0x00000000000000C5ull, 0x00000000000000C5ull },
	{ 0x800000A338F9C338ull, 0x800000A338F9C338ull },
	{ 0x0000084E12532772ull, 0x0000084E12532772ull },
	{ 0x0000000000000A45ull, 0x0000000000000A45ull },
	{ 0x80000000DCB20B9Eull, 0x80000000DCB20B9Eull },
	{ 0x80000000006BFC63ull, 0x80000000006BFC63ull },
	{ 0x80000000000002E0ull, 0x80000000000002E0ull },
	{ 0x800004833B1166E7ull, 0x800004833B1166E7ull },
	{ 0x8000000000000332ull, 0x8000000000000332ull },
	{ 0x00001605FC8DCE9Dull, 0x00001605FC8DCE9Dull },
	{ 0x80000C232310879Cull, 0x80000C232310879Cull },
	{ 0x0000000000000057ull, 0x0000000000000057ull },
	{ 0x000000009AD2A8BEull, 0x000000009AD2A8BEull },
	{ 0x00000000000DF5B0ull, 0x00000000000DF5B0ull },
	{ 0x8000000000000031ull, 0x8000000000000031ull },
	{ 0x800000CA1DFAC0F5ull, 0x800000CA1DFAC0F5ull },
	{ 0x000000000000D675ull, 0x000000000000D675ull },
	{ 0x000A4F5D280C38F7ull, 0x000A4F5D280C38F7ull },
	{ 0x800E36DDE3B6FA0Eull, 0x800E36DDE3B6FA0Eull },
	{ 0x000D50142DE34509ull, 0x000D50142DE34509ull },
	{ 0x80082A54988A1163ull, 0x80082A54988A1163ull },
	{ 0x000D75FE14BDD4D9ull, 0x000D75FE14BDD4D9ull },
	{ 0x000CF4E5DA72DF57ull, 0x000CF4E5DA72DF57ull },
	{ 0x000E647B1DD73332ull, 0x000E647B1DD73332ull },
	{ 0x000F2C068F3198F3ull, 0x000F2C068F3198F3ull },
	{ 0x800DF34A31327B39ull, 0x800DF34A31327B39ull },
	{ 0x800ABCDE09DAF225ull, 0x800ABCDE09DAF225ull },
	{ 0x800A550B21AAFAF7ull, 0x800A550B21AAFAF7ull },
	{ 0x000DC86FEFADF223ull, 0x000DC86FEFADF223ull },
	{ 0x00094096A5E6120Bull, 0x00094096A5E6120Bull },
	{ 0x800879CCF35D210Full, 0x800879CCF35D210Full },
	{ 0x80084311A3C31F6Full, 0x80084311A3C31F6Full },
	{ 0x800859254777E41Eull, 0x800859254777E41Eull },
	{ 0x8009D26ADF92B65Full, 0x8009D26ADF92B65Full },
	{ 0x800AD5540C428AF5ull, 0x800AD5540C428AF5ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000248F1Bull, 0x0000000000248F1Bull },
	{ 0x16213138415CAD29ull, 0x16213138415CAD29ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x3B4865A623B06DACull, 0x3B4865A623B06DACull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x97E2F8C47733A118ull, 0x97E2F8C47733A118ull },
	{ 0x2A2DB9810CF5B29Dull, 0x2A2DB9810CF5B29Dull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0DB5D9AA51C30D33ull, 0x0DB5D9AA51C30D33ull },
	{ 0x1BD02AE45ADA6199ull, 0x1BD02AE45ADA6199ull },
	{ 0x0F263958BBDDB660ull, 0x0F263958BBDDB660ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0xA9861E8DA6BC343Bull, 0xA9861E8DA6BC343Bull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8E17C9ACF8D699C8ull, 0x8E17C9ACF8D699C8ull },
	{ 0x2C371CA5001EB4D4ull, 0x2C371CA5001EB4D4ull },
	{ 0x9AE3AB20A88EA7D9ull, 0x9AE3AB20A88EA7D9ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0xAED418EEE280867Aull, 0xAED418EEE280867Aull },
	{ 0x3820EA0501B07AACull, 0x3820EA0501B07AACull },
	{ 0x8793B8E833EF7FBAull, 0x8793B8E833EF7FBAull },
	{ 0x332FA8FDBB42B423ull, 0x332FA8FDBB42B423ull },
	{ 0x2C3502C4AEF65264ull, 0x2C3502C4AEF65264ull },
	{ 0x1D11616852A4E1FFull, 0x1D11616852A4E1FFull },
	{ 0xBE72C95924099C90ull, 0xBE72C95924099C90ull },
	{ 0x2E8CDD6DF76F1EEEull, 0x2E8CDD6DF76F1EEEull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8FE5A49F873435EBull, 0x8FE5A49F873435EBull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x9DC1DDC7EC3821ADull, 0x9DC1DDC7EC3821ADull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x83A4F554C05D31B2ull, 0x83A4F554C05D31B2ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x98BA62F3B516C514ull, 0x98BA62F3B516C514ull },
	{ 0xAD34A58F639191F6ull, 0xAD34A58F639191F6ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x2409EDC48070F43Bull, 0x2409EDC48070F43Bull },
	{ 0x0F68633579A556B6ull, 0x0F68633579A556B6ull },
	{ 0x8E4B6D825EEFC1A3ull, 0x8E4B6D825EEFC1A3ull },
	{ 0xA7AE723774D4C852ull, 0xA7AE723774D4C852ull },
	{ 0x91EF186A4F6180A7ull, 0x91EF186A4F6180A7ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x175FD123AFEB8A7Full, 0x175FD123AFEB8A7Full },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8492CF0BB6D3F22Full, 0x8492CF0BB6D3F22Full },
	{ 0x2E1F229A106038D9ull, 0x2E1F229A106038D9ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x9D51DD93AB2E472Full, 0x9D51DD93AB2E472Full },
	{ 0x090226A48EACD711ull, 0x090226A48EACD711ull },
	{ 0x1D9FD5428B53318Dull, 0x1D9FD5428B53318Dull },
	{ 0x3648EB9EDDA4B6F9ull, 0x3648EB9EDDA4B6F9ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0xAB3C555A61A54758ull, 0xAB3C555A61A54758ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x259416D0D40CFFA5ull, 0x259416D0D40CFFA5ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x00F91DD3EF6A9B7Cull, 0x00F91DD3EF6A9B7Cull },
	{ 0x8CD39D0DABEBBC64ull, 0x8CD39D0DABEBBC64ull },
	{ 0x8C6266D8140305D3ull, 0x8C6266D8140305D3ull },
	{ 0x21AF6FBFB296E8E3ull, 0x21AF6FBFB296E8E3ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x00CB105DB768887Aull, 0x00CB105DB768887Aull },
	{ 0x067CCE56C481EF8Aull, 0x067CCE56C481EF8Aull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x28C7D4E7FEC3A490ull, 0x28C7D4E7FEC3A490ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0147B3DD36BA010Aull, 0x0147B3DD36BA010Aull },
	{ 0x87612494A11659F7ull, 0x87612494A11659F7ull },
	{ 0xA10F2AEC772A6140ull, 0xA10F2AEC772A6140ull },
	{ 0x3DE56A553F59E8B7ull, 0x3DE56A553F59E8B7ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x02D36C322752578Eull, 0x02D36C322752578Eull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x98E4C238D4D1A931ull, 0x98E4C238D4D1A931ull },
	{ 0x29E67A56F023EDBDull, 0x29E67A56F023EDBDull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000013E67ull, 0x0000000000013E67ull },
	{ 0x2683F7A48C4D8C04ull, 0x2683F7A48C4D8C04ull },
	{ 0xAFE30D6DADF4426Bull, 0xAFE30D6DADF4426Bull },
	{ 0x04F737E3F8EB6CB9ull, 0x04F737E3F8EB6CB9ull },
	{ 0xA0BD068A1C197B2Bull, 0xA0BD068A1C197B2Bull },
	{ 0x96DE757D00624785ull, 0x96DE757D00624785ull },
	{ 0x8F38788D5330B7F1ull, 0x8F38788D5330B7F1ull },
	{ 0xA0B286C53D4E4854ull, 0xA0B286C53D4E4854ull },
	{ 0xADEE6E93CEB6C792ull, 0xADEE6E93CEB6C792ull },
	{ 0x9DDBCD8FE1625963ull, 0x9DDBCD8FE1625963ull },
	{ 0x8BEF7DE1FFAFE7D6ull, 0x8BEF7DE1FFAFE7D6ull },
	{ 0x24E142F9572A0B93ull, 0x24E142F9572A0B93ull },
	{ 0x00000000DAACCAE6ull, 0x00000000DAACCAE6ull },
	{ 0x15B4646E149C9C00ull, 0x15B4646E149C9C00ull },
	{ 0x30A0F30388B2A4D1ull, 0x30A0F30388B2A4D1ull },
	{ 0x364B634F6E5D2408ull, 0x364B634F6E5D2408ull },
	{ 0x8CBA2F8A4FB42123ull, 0x8CBA2F8A4FB42123ull },
	{ 0x35CD1A7CA7C86E52ull, 0x35CD1A7CA7C86E52ull },
	{ 0x8BB0F0C9516AAA54ull, 0x8BB0F0C9516AAA54ull },
	{ 0x86145248B4921E2Cull, 0x86145248B4921E2Cull },
	{ 0x8E58F11AF3285D9Bull, 0x8E58F11AF3285D9Bull },
	{ 0x9495CF4B93536C15ull, 0x9495CF4B93536C15ull },
	{ 0x8580A02A58F60221ull, 0x8580A02A58F60221ull },
	{ 0x000A19DE6D91BC83ull, 0x000A19DE6D91BC83ull },
	{ 0x9D71A364E7D1522Aull, 0x9D71A364E7D1522Aull },
	{ 0xB28FD4E327D597F7ull, 0xB28FD4E327D597F7ull },
	{ 0xADA39A7EE196D8FFull, 0xADA39A7EE196D8FFull },
	{ 0xB4980D026553E316ull, 0xB4980D026553E316ull },
	{ 0x1D9743B9FC6F003Bull, 0x1D9743B9FC6F003Bull },
	{ 0x1BFB7B16F675FE4Bull, 0x1BFB7B16F675FE4Bull },
	{ 0x0695346056DEB717ull, 0x0695346056DEB717ull },
	{ 0xBEA4A8226396183Dull, 0xBEA4A8226396183Dull },
	{ 0x85FFC949FA590B99ull, 0x85FFC949FA590B99ull },
	{ 0xB9F7EA38A6530EA5ull, 0xB9F7EA38A6530EA5ull },
	{ 0x8CFA6AD9AEBFB6C6ull, 0x8CFA6AD9AEBFB6C6ull },
	{ 0x917157A8A131BF8Full, 0x917157A8A131BF8Full },
	{ 0x2B84390B2229F9F0ull, 0x2B84390B2229F9F0ull },
	{ 0x931EA9846EDF2C74ull, 0x931EA9846EDF2C74ull },
	{ 0x211E35C0F276C913ull, 0x211E35C0F276C913ull },
	{ 0x047B3E40F7DF5DFAull, 0x047B3E40F7DF5DFAull },
	{ 0x2BE4BCC9106A294Cull, 0x2BE4BCC9106A294Cull },
	{ 0xAEB533CCEC37F4EAull, 0xAEB533CCEC37F4EAull },
	{ 0x1BB9BE390C9F34F3ull, 0x1BB9BE390C9F34F3ull },
	{ 0x84ECB1EF839531ACull, 0x84ECB1EF839531ACull },
	{ 0x91359AF5CBB99FA1ull, 0x91359AF5CBB99FA1ull },
	{ 0xADF8E16CB5076EE9ull, 0xADF8E16CB5076EE9ull },
	{ 0x9CEC73A2A75004B8ull, 0x9CEC73A2A75004B8ull },
	{ 0x1DD0DAE265490C86ull, 0x1DD0DAE265490C86ull },
	{ 0x3A64395510F255CCull, 0x3A64395510F255CCull },
	{ 0x013449260FBFE9E4ull, 0x013449260FBFE9E4ull },
	{ 0x3680D49118B5F2C7ull, 0x3680D49118B5F2C7ull },
	{ 0x92D5B3BB2B7148B9ull, 0x92D5B3BB2B7148B9ull },
	{ 0x369DF565135945DFull, 0x369DF565135945DFull },
	{ 0x3413F629795576FFull, 0x3413F629795576FFull },
	{ 0x00000000C33A20BBull, 0x00000000C33A20BBull },
	{ 0x88A656C7C4D1EE06ull, 0x88A656C7C4D1EE06ull },
	{ 0x9A437E673B1E0AADull, 0x9A437E673B1E0AADull },
	{ 0x80B005F1514BC2EBull, 0x80B005F1514BC2EBull },
	{ 0x1EC0522944112B7Aull, 0x1EC0522944112B7Aull },
	{ 0xBBB88D600F999D07ull, 0xBBB88D600F999D07ull },
	{ 0x37B701B575789B95ull, 0x37B701B575789B95ull },
	{ 0x276ED2DAAB2B6BA3ull, 0x276ED2DAAB2B6BA3ull },
	{ 0x19D6068854170252ull, 0x19D6068854170252ull },
	{ 0x2AD2093505E1C2F2ull, 0x2AD2093505E1C2F2ull },
	{ 0x23F2519D6CC897F4ull, 0x23F2519D6CC897F4ull },
	{ 0x9346E66909BF93F3ull, 0x9346E66909BF93F3ull },
	{ 0x989983576DC30575ull, 0x989983576DC30575ull },
	{ 0xA120CF4E1BCE8182ull, 0xA120CF4E1BCE8182ull },
	{ 0x3C92874776DB0E30ull, 0x3C92874776DB0E30ull },
	{ 0x2484EA334337C350ull, 0x2484EA334337C350ull },
	{ 0xA5F2A273B8A59035ull, 0xA5F2A273B8A59035ull },
	{ 0x1C498042ABE50547ull, 0x1C498042ABE50547ull },
	{ 0x0CA39E5AAAB35DA8ull, 0x0CA39E5AAAB35DA8ull },
	{ 0x1672BADDE6D3B869ull, 0x1672BADDE6D3B869ull },
	{ 0xB75712A1C244339Bull, 0xB75712A1C244339Bull },
	{ 0x98F252504241C96Aull, 0x98F252504241C96Aull },
	{ 0x1FE7453E457A55CBull, 0x1FE7453E457A55CBull },
	{ 0xA7AA3A6C596497B3ull, 0xA7AA3A6C596497B3ull },
	{ 0x363C62AE037601C5ull, 0x363C62AE037601C5ull },
	{ 0x9D90D9E441E61F73ull, 0x9D90D9E441E61F73ull },
	{ 0x14AEC415A309B6DCull, 0x14AEC415A309B6DCull },
	{ 0x9EB7BE3B993BF893ull, 0x9EB7BE3B993BF893ull },
	{ 0x39E3DBAFF5C9AC9Full, 0x39E3DBAFF5C9AC9Full },
	{ 0xB46682A7F9BAE0FBull, 0xB46682A7F9BAE0FBull },
	{ 0x973621C782A27731ull, 0x973621C782A27731ull },
	{ 0x81D0A0EA54F5D716ull, 0x81D0A0EA54F5D716ull },
	{ 0x16C1A18DBA2B13F2ull, 0x16C1A18DBA2B13F2ull },
	{ 0xBD609CAEB29965FDull, 0xBD609CAEB29965FDull },
	{ 0x00000000000000DCull, 0x00000000000000DCull },
	{ 0x94A287086657F17Full, 0x94A287086657F17Full },
	{ 0x00001E325CB13C87ull, 0x00001E325CB13C87ull },
	{ 0x23047C4B014129BBull, 0x23047C4B014129BBull },
	{ 0x9009CA22FD9A245Eull, 0x9009CA22FD9A245Eull },
	{ 0x9D19E35283F680A6ull, 0x9D19E35283F680A6ull },
	{ 0x208E0002045D8A1Cull, 0x208E0002045D8A1Cull },
	{ 0x2084261A51590577ull, 0x2084261A51590577ull },
	{ 0x3B88C39BA3E8DCB2ull, 0x3B88C39BA3E8DCB2ull },
	{ 0xACB956846E4C915Aull, 0xACB956846E4C915Aull },
	{ 0xB7433C7858F79B71ull, 0xB7433C7858F79B71ull },
	{ 0x98D643D6EA20D0E3ull, 0x98D643D6EA20D0E3ull },
	{ 0xA526C21867912319ull, 0xA526C21867912319ull },
	{ 0x94486EEF05691072ull, 0x94486EEF05691072ull },
	{ 0x2C37087BC911CAB7ull, 0x2C37087BC911CAB7ull },
	{ 0x8F90131B9A4455A2ull, 0x8F90131B9A4455A2ull },
	{ 0x9F7431AAF24F3633ull, 0x9F7431AAF24F3633ull },
	{ 0x143B5A9FFCC10A70ull, 0x143B5A9FFCC10A70ull },
	{ 0xA16F4F6D0098C949ull, 0xA16F4F6D0098C949ull },
	{ 0x1997DC01D0BB64D6ull, 0x1997DC01D0BB64D6ull },
	{ 0x1553BCFCE464C81Full, 0x1553BCFCE464C81Full },
	{ 0x800000003577B3E1ull, 0x800000003577B3E1ull },
	{ 0xAAEA96F68A5F859Eull, 0xAAEA96F68A5F859Eull },
	{ 0x200ECBDFE3138D0Eull, 0x200ECBDFE3138D0Eull },
	{ 0xA4543245AD56721Full, 0xA4543245AD56721Full },
	{ 0xA9A3CAC6889F659Dull, 0xA9A3CAC6889F659Dull },
	{ 0x38AE65ADAFF32350ull, 0x38AE65ADAFF32350ull },
	{ 0xBBB3E9F29CCBE07Eull, 0xBBB3E9F29CCBE07Eull },
	{ 0x00000000000001E1ull, 0x00000000000001E1ull },
	{ 0xAF4C75F65F576F0Eull, 0xAF4C75F65F576F0Eull },
	{ 0x2041D3D68A83A95Eull, 0x2041D3D68A83A95Eull },
	{ 0x14A310CDE4318F2Eull, 0x14A310CDE4318F2Eull },
	{ 0x8DB60BABD78C6F3Dull, 0x8DB60BABD78C6F3Dull },
	{ 0xA37F8E0B12C16C04ull, 0xA37F8E0B12C16C04ull },
	{ 0xAFA292B13700ABDFull, 0xAFA292B13700ABDFull },
	{ 0xA2B065351AA6A796ull, 0xA2B065351AA6A796ull },
	{ 0x3531344CB3A53CEBull, 0x3531344CB3A53CEBull },
	{ 0x38C617CD1C5CE7A1ull, 0x38C617CD1C5CE7A1ull },
	{ 0x85078F4D335A4742ull, 0x85078F4D335A4742ull },
	{ 0x84FB1400C37DBEC9ull, 0x84FB1400C37DBEC9ull },
	{ 0xA3D178363A82FA99ull, 0xA3D178363A82FA99ull },
	{ 0x32B9BCD267EEEEECull, 0x32B9BCD267EEEEECull },
	{ 0xAEC0703F1199C87Full, 0xAEC0703F1199C87Full },
	{ 0x103565FC3BB9945Dull, 0x103565FC3BB9945Dull },
	{ 0x3D87B690F773AA91ull, 0x3D87B690F773AA91ull },
	{ 0xA433CF6BBD34861Cull, 0xA433CF6BBD34861Cull },
	{ 0xBCB492FEF0C95239ull, 0xBCB492FEF0C95239ull },
	{ 0xB9221705FF227219ull, 0xB9221705FF227219ull },
	{ 0xBD63236BC188CCC5ull, 0xBD63236BC188CCC5ull },
	{ 0x0767C2D082C7DB9Eull, 0x0767C2D082C7DB9Eull },
	{ 0x2654CA64DD2CC29Bull, 0x2654CA64DD2CC29Bull },
	{ 0x1B58105A19E99953ull, 0x1B58105A19E99953ull },
	{ 0x09C3564369DB1BA1ull, 0x09C3564369DB1BA1ull },
	{ 0xB6C35E165E75FF79ull, 0xB6C35E165E75FF79ull },
	{ 0x8996B0A994BA90D8ull, 0x8996B0A994BA90D8ull },
	{ 0x9DB1B245A81A1F96ull, 0x9DB1B245A81A1F96ull },
	{ 0x0D9D601F87F3BED8ull, 0x0D9D601F87F3BED8ull },
	{ 0x8000000149228595ull, 0x8000000149228595ull },
	{ 0x0000000562076C7Eull, 0x0000000562076C7Eull },
	{ 0x800000000000107Full, 0x800000000000107Full },
	{ 0x00000347115F4828ull, 0x00000347115F4828ull },
	{ 0x80000000000074ADull, 0x80000000000074ADull },
	{ 0x800C6A23379D1F68ull, 0x800C6A23379D1F68ull },
	{ 0x800000004C9191B6ull, 0x800000004C9191B6ull },
	{ 0x0000000000000093ull, 0x0000000000000093ull },
	{ 0x8000006447145195ull, 0x8000006447145195ull },
	{ 0x800003DB8990EAACull, 0x800003DB8990EAACull },
	{ 0x8008EA4075846D42ull, 0x8008EA4075846D42ull },
	{ 0x000000087EE1DC1Eull, 0x000000087EE1DC1Eull },
	{ 0x00000000002769DBull, 0x00000000002769DBull },
	{ 0x0000000074C24291ull, 0x0000000074C24291ull },
	{ 0x0000000012E76802ull, 0x0000000012E76802ull },
	{ 0x80000000000000DDull, 0x80000000000000DDull },
	{ 0x800000000000291Aull, 0x800000000000291Aull },
	{ 0x8000000000000018ull, 0x8000000000000018ull },
	{ 0x8000000000000041ull, 0x8000000000000041ull },
	{ 0x000000005E931C60ull, 0x000000005E931C60ull },
	{ 0x800000001060FA0Eull, 0x800000001060FA0Eull },
	{ 0x800021CCF1352B05ull, 0x800021CCF1352B05ull },
	{ 0x8000000051B7F428ull, 0x8000000051B7F428ull },
	{ 0x8000000000000014ull, 0x8000000000000014ull },
	{ 0x8000000086A515AFull, 0x8000000086A515AFull },
	{ 0x000000002EA2E582ull, 0x000000002EA2E582ull },
	{ 0x8000000000001B8Dull, 0x8000000000001B8Dull },
	{ 0x80000000461438A2ull, 0x80000000461438A2ull },
	{ 0x800000181B00A2E5ull, 0x800000181B00A2E5ull },
	{ 0x000000000000066Cull, 0x000000000000066Cull },
	{ 0x00000020F83982A9ull, 0x00000020F83982A9ull },
	{ 0x000000000576CA6Aull, 0x000000000576CA6Aull },
	{ 0x00000000001FA1EEull, 0x00000000001FA1EEull },
	{ 0x8000000162F50218ull, 0x8000000162F50218ull },
	{ 0x0000000000000004ull, 0x0000000000000004ull },
	{ 0x000000002D6CC370ull, 0x000000002D6CC370ull },
	{ 0x8000000000102F0Eull, 0x8000000000102F0Eull },
	{ 0x000000009943819Full, 0x000000009943819Full },
	{ 0x0000000000000053ull, 0x0000000000000053ull },
	{ 0x000000001E1BA7B1ull, 0x000000001E1BA7B1ull },
	{ 0x8000000000122615ull, 0x8000000000122615ull },
	{ 0x00000021AA6ED1B7ull, 0x00000021AA6ED1B7ull },
	{ 0x8000000000270505ull, 0x8000000000270505ull },
	{ 0x80016E4D9FDEA7A1ull, 0x80016E4D9FDEA7A1ull },
	{ 0x8000E18FD69892F4ull, 0x8000E18FD69892F4ull },
	{ 0x8000177776AD205Eull, 0x8000177776AD205Eull },
	{ 0x00000000212A93B2ull, 0x00000000212A93B2ull },
	{ 0x80000000000002BCull, 0x80000000000002BCull },
	{ 0x00002AB81E341AC0ull, 0x00002AB81E341AC0ull },
	{ 0x00000000005A1DEFull, 0x00000000005A1DEFull },
	{ 0x000000000001E900ull, 0x000000000001E900ull },
	{ 0x000000000000006Cull, 0x000000000000006Cull },
	{ 0x000000000000002Cull, 0x000000000000002Cull },
	{ 0x8000000000001353ull, 0x8000000000001353ull },
	{ 0x0000000000056327ull, 0x0000000000056327ull },
	{ 0x0000000000131371ull, 0x0000000000131371ull },
	{ 0x0000000000000010ull, 0x0000000000000010ull },
	{ 0x000000019DAC3E40ull, 0x000000019DAC3E40ull },
	{ 0x800000000000026Bull, 0x800000000000026Bull },
	{ 0x8001DA03F987313Aull, 0x8001DA03F987313Aull },
	{ 0x8000000000008C20ull, 0x8000000000008C20ull },
	{ 0x800000000000006Cull, 0x800000000000006Cull },
	{ 0x8000026BA48E9F76ull, 0x8000026BA48E9F76ull },
	{ 0x800000000035DD10ull, 0x800000000035DD10ull },
	{ 0x00000000002FBEE3ull, 0x00000000002FBEE3ull },
	{ 0x00000000000009C5ull, 0x00000000000009C5ull },
	{ 0x800000000154AC80ull, 0x800000000154AC80ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x8000000000293320ull, 0x8000000000293320ull },
	{ 0x000000023419DEF6ull, 0x000000023419DEF6ull },
	{ 0x8000000000000125ull, 0x8000000000000125ull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x0000000000001AD7ull, 0x0000000000001AD7ull },
	{ 0x800BF23116471A99ull, 0x800BF23116471A99ull },
	{ 0x800DCFA3617BC03Full, 0x800DCFA3617BC03Full },
	{ 0x0008F9E008F3D8EDull, 0x0008F9E008F3D8EDull },
	{ 0x000A4AE47453FBF3ull, 0x000A4AE47453FBF3ull },
	{ 0x000EFAF63A29FEB8ull, 0x000EFAF63A29FEB8ull },
	{ 0x00088E58BB17804Cull, 0x00088E58BB17804Cull },
	{ 0x000DBFD39D76F490ull, 0x000DBFD39D76F490ull },
	{ 0x0008AA18FA064F2Cull, 0x0008AA18FA064F2Cull },
	{ 0x800B0A13C1C831EAull, 0x800B0A13C1C831EAull },
	{ 0x8009B5AB3A8DC2D0ull, 0x8009B5AB3A8DC2D0ull },
	{ 0x80095CB077E36826ull, 0x80095CB077E36826ull },
	{ 0x0009B88000936316ull, 0x0009B88000936316ull },
	{ 0x0008182CC7B8B92Eull, 0x0008182CC7B8B92Eull },
	{ 0x000BDFF9501A6C6Dull, 0x000BDFF9501A6C6Dull },
	{ 0x000F1E6B28450A99ull, 0x000F1E6B28450A99ull },
	{ 0x000B690326EADC94ull, 0x000B690326EADC94ull },
	{ 0x800CEF80A89038A2ull, 0x800CEF80A89038A2ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x1848885882706A95ull, 0x1848885882706A95ull },
	{ 0x07D9110A16BA5645ull, 0x07D9110A16BA5645ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x85F24552DC52781Bull, 0x85F24552DC52781Bull },
	{ 0xB08D8FCDE84C9B03ull, 0xB08D8FCDE84C9B03ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xADB4BA9FE778BD48ull, 0xADB4BA9FE778BD48ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0E3E46E122ED013Eull, 0x0E3E46E122ED013Eull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0xA3BAD5E30FCC7BF6ull, 0xA3BAD5E30FCC7BF6ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x2468132284C61B75ull, 0x2468132284C61B75ull },
	{ 0xBE09DF5DAF62604Dull, 0xBE09DF5DAF62604Dull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x9B41BAC922549CF8ull, 0x9B41BAC922549CF8ull },
	{ 0x0FA66B3365F3523Dull, 0x0FA66B3365F3523Dull },
	{ 0x3D179732B80F30D4ull, 0x3D179732B80F30D4ull },
	{ 0x82DDF567F5574E02ull, 0x82DDF567F5574E02ull },
	{ 0x8EB14AF8768E69F2ull, 0x8EB14AF8768E69F2ull },
	{ 0x21DC66921FA0674Cull, 0x21DC66921FA0674Cull },
	{ 0x000679658550C71Cull, 0x000679658550C71Cull },
	{ 0xB26A359991BEB824ull, 0xB26A359991BEB824ull },
	{ 0x03B01BAC90D0922Aull, 0x03B01BAC90D0922Aull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0872B58FABAF828Full, 0x0872B58FABAF828Full },
	{ 0x0000000000003BBBull, 0x0000000000003BBBull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x81983A5B7ABBD199ull, 0x81983A5B7ABBD199ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x37F5EA5BF449FE2Eull, 0x37F5EA5BF449FE2Eull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0xB8E1A4D4143E61A9ull, 0xB8E1A4D4143E61A9ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x1661185EFF42A862ull, 0x1661185EFF42A862ull },
	{ 0x9451AF1AA8BEB9FDull, 0x9451AF1AA8BEB9FDull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x1EF0093701203F81ull, 0x1EF0093701203F81ull },
	{ 0x0BAE850CEB67C7D3ull, 0x0BAE850CEB67C7D3ull },
	{ 0xA67799E55B794757ull, 0xA67799E55B794757ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xA16395D5DDA37125ull, 0xA16395D5DDA37125ull },
	{ 0x000000000000045Bull, 0x000000000000045Bull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x83D9A55428949256ull, 0x83D9A55428949256ull },
	{ 0x9B2216AA766FF98Bull, 0x9B2216AA766FF98Bull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xAB1876E549F8DC7Bull, 0xAB1876E549F8DC7Bull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x1E74B91441EC724Full, 0x1E74B91441EC724Full },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0xAC45A60D2542A1C5ull, 0xAC45A60D2542A1C5ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x32A53FBB7FDBB6A9ull, 0x32A53FBB7FDBB6A9ull },
	{ 0xA5E9507087175BCDull, 0xA5E9507087175BCDull },
	{ 0x9BD2DFCECBA25E05ull, 0x9BD2DFCECBA25E05ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xAE1951BB86E9D2DCull, 0xAE1951BB86E9D2DCull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xA3334FE8DC5CB8A2ull, 0xA3334FE8DC5CB8A2ull },
	{ 0x302D86FB1F2C14CDull, 0x302D86FB1F2C14CDull },
	{ 0xA005CCE3E6A09B0Bull, 0xA005CCE3E6A09B0Bull },
	{ 0xBE5D1517BAF5DAC9ull, 0xBE5D1517BAF5DAC9ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x29022598545FECFCull, 0x29022598545FECFCull },
	{ 0x8396A3CFD67410C9ull, 0x8396A3CFD67410C9ull },
	{ 0x8577F2B894715CA1ull, 0x8577F2B894715CA1ull },
	{ 0xB23E13EDDCFFD646ull, 0xB23E13EDDCFFD646ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8562E500F5A6D4FBull, 0x8562E500F5A6D4FBull },
	{ 0x30A4374A700261E4ull, 0x30A4374A700261E4ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0B809B956B95E192ull, 0x0B809B956B95E192ull },
	{ 0x9431BC0C9A7A832Cull, 0x9431BC0C9A7A832Cull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xB174C0060EC915CFull, 0xB174C0060EC915CFull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x000000012EFB1662ull, 0x000000012EFB1662ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xA77A39F6B7C65230ull, 0xA77A39F6B7C65230ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xA0F0B2491DCEBC2Aull, 0xA0F0B2491DCEBC2Aull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0279374BD294B63Aull, 0x0279374BD294B63Aull },
	{ 0x8CE148BB3DC3BEA1ull, 0x8CE148BB3DC3BEA1ull },
	{ 0x3D9BF46E08A0C2ABull, 0x3D9BF46E08A0C2ABull },
	{ 0x000007F3D766C96Full, 0x000007F3D766C96Full },
	{ 0x3353EDAC7FFA3229ull, 0x3353EDAC7FFA3229ull },
	{ 0x044721A0AA075983ull, 0x044721A0AA075983ull },
	{ 0x8D91550AB0F0F49Full, 0x8D91550AB0F0F49Full },
	{ 0x1F89BAA61AD78320ull, 0x1F89BAA61AD78320ull },
	{ 0x8D9FF400189118A7ull, 0x8D9FF400189118A7ull },
	{ 0x331437350107B5A3ull, 0x331437350107B5A3ull },
	{ 0x08F87D6C98E5E30Aull, 0x08F87D6C98E5E30Aull },
	{ 0x000000086CBA17C2ull, 0x000000086CBA17C2ull },
	{ 0xAAE38FCB02E604C4ull, 0xAAE38FCB02E604C4ull },
	{ 0x1389D6D74069DE73ull, 0x1389D6D74069DE73ull },
	{ 0x323D97BCCAE6716Eull, 0x323D97BCCAE6716Eull },
	{ 0x1682D3E7F61D4DFAull, 0x1682D3E7F61D4DFAull },
	{ 0x808EC81917CBEB64ull, 0x808EC81917CBEB64ull },
	{ 0x82869CCE6F6EC594ull, 0x82869CCE6F6EC594ull },
	{ 0x824D4C5AC3D09D0Aull, 0x824D4C5AC3D09D0Aull },
	{ 0xB0F93BB5874AA4BEull, 0xB0F93BB5874AA4BEull },
	{ 0x17CB5AA72FBB522Bull, 0x17CB5AA72FBB522Bull },
	{ 0x80F6942E92AC0202ull, 0x80F6942E92AC0202ull },
	{ 0x14166E0E4826BEC7ull, 0x14166E0E4826BEC7ull },
	{ 0x8483984F9AEDECEDull, 0x8483984F9AEDECEDull },
	{ 0x988F577E1E22C82Dull, 0x988F577E1E22C82Dull },
	{ 0x800000006905A55Bull, 0x800000006905A55Bull },
	{ 0x21E26D40D7C3574Cull, 0x21E26D40D7C3574Cull },
	{ 0x26361A72BD3517C5ull, 0x26361A72BD3517C5ull },
	{ 0x036CBAE6F9472F59ull, 0x036CBAE6F9472F59ull },
	{ 0x84C289097DC70D8Aull, 0x84C289097DC70D8Aull },
	{ 0x8B0C4795FD4579F8ull, 0x8B0C4795FD4579F8ull },
	{ 0x3A51640BF8718FE7ull, 0x3A51640BF8718FE7ull },
	{ 0x31C73AAD227906CCull, 0x31C73AAD227906CCull },
	{ 0x3CA00053E66C0841ull, 0x3CA00053E66C0841ull },
	{ 0x393D091E94AC2A20ull, 0x393D091E94AC2A20ull },
	{ 0x142981E820B3D7E6ull, 0x142981E820B3D7E6ull },
	{ 0x09F693BF07A50648ull, 0x09F693BF07A50648ull },
	{ 0x0F5A8267D9B132F1ull, 0x0F5A8267D9B132F1ull },
	{ 0x8ACEB92476810484ull, 0x8ACEB92476810484ull },
	{ 0xAFB93AA593F13737ull, 0xAFB93AA593F13737ull },
	{ 0xA47745B4EA3E8AC9ull, 0xA47745B4EA3E8AC9ull },
	{ 0x3D506C5452E3184Full, 0x3D506C5452E3184Full },
	{ 0xBB3149020DEBCD1Bull, 0xBB3149020DEBCD1Bull },
	{ 0x16AAE363DFE4DB82ull, 0x16AAE363DFE4DB82ull },
	{ 0x23D543799F6EA951ull, 0x23D543799F6EA951ull },
	{ 0x8DEE7BD13B4098A2ull, 0x8DEE7BD13B4098A2ull },
	{ 0x8000037AC7CC0243ull, 0x8000037AC7CC0243ull },
	{ 0x92DAA3D66446B6EBull, 0x92DAA3D66446B6EBull },
	{ 0xA31042625559AEF2ull, 0xA31042625559AEF2ull },
	{ 0x3A43480EFC0D420Eull, 0x3A43480EFC0D420Eull },
	{ 0x1BE573EF5460F3E3ull, 0x1BE573EF5460F3E3ull },
	{ 0x37B2F0988AEEEB74ull, 0x37B2F0988AEEEB74ull },
	{ 0x30F12196C1BD2182ull, 0x30F12196C1BD2182ull },
	{ 0x94EBFC1DFC4C6541ull, 0x94EBFC1DFC4C6541ull },
	{ 0x3CE5AA302C615D86ull, 0x3CE5AA302C615D86ull },
	{ 0x8300967AAF689063ull, 0x8300967AAF689063ull },
	{ 0x921560728050F8AFull, 0x921560728050F8AFull },
	{ 0x2FBC6DD4A3F5A3DAull, 0x2FBC6DD4A3F5A3DAull },
	{ 0x9487EA8075361D2Dull, 0x9487EA8075361D2Dull },
	{ 0x1E1786C970820CB9ull, 0x1E1786C970820CB9ull },
	{ 0xB951C7508E54BE39ull, 0xB951C7508E54BE39ull },
	{ 0x9F011F9684888839ull, 0x9F011F9684888839ull },
	{ 0x2F205A37B976D010ull, 0x2F205A37B976D010ull },
	{ 0xB626C519D8A84B36ull, 0xB626C519D8A84B36ull },
	{ 0xA4CE6B79D5399439ull, 0xA4CE6B79D5399439ull },
	{ 0x01B4D05C663D9E7Eull, 0x01B4D05C663D9E7Eull },
	{ 0x0E2FB419840FE321ull, 0x0E2FB419840FE321ull },
	{ 0x28F4922CF20C4A03ull, 0x28F4922CF20C4A03ull },
	{ 0x09CC2D36B3A18794ull, 0x09CC2D36B3A18794ull },
	{ 0xAB8A81E97814029Dull, 0xAB8A81E97814029Dull },
	{ 0x90E54A38A935CD50ull, 0x90E54A38A935CD50ull },
	{ 0x36BEF9EA4FC2DF76ull, 0x36BEF9EA4FC2DF76ull },
	{ 0xBC2F6D35C4AF204Bull, 0xBC2F6D35C4AF204Bull },
	{ 0x1842B71F2C7EC905ull, 0x1842B71F2C7EC905ull },
	{ 0x1383C4066AA8CAECull, 0x1383C4066AA8CAECull },
	{ 0x10DD25ACFCB35E5Dull, 0x10DD25ACFCB35E5Dull },
	{ 0x24B1FD6AD9618947ull, 0x24B1FD6AD9618947ull },
	{ 0x8000000000000742ull, 0x8000000000000742ull },
	{ 0xB458DE98AE90FBFEull, 0xB458DE98AE90FBFEull },
	{ 0x13A277BD5F4AAD2Cull, 0x13A277BD5F4AAD2Cull },
	{ 0x9E23EDA1C867963Aull, 0x9E23EDA1C867963Aull },
	{ 0xBAE54A179CE246F4ull, 0xBAE54A179CE246F4ull },
	{ 0x9B00B376B6DA8567ull, 0x9B00B376B6DA8567ull },
	{ 0x236137B99FD379C1ull, 0x236137B99FD379C1ull },
	{ 0x03123245E3979004ull, 0x03123245E3979004ull },
	{ 0x9B391D3D580F7BC8ull, 0x9B391D3D580F7BC8ull },
	{ 0x3A26B80F8221BB55ull, 0x3A26B80F8221BB55ull },
	{ 0x05684921D2288901ull, 0x05684921D2288901ull },
	{ 0x8D6CB264BF3ECF72ull, 0x8D6CB264BF3ECF72ull },
	{ 0x86224E899807A839ull, 0x86224E899807A839ull },
	{ 0x8C3223A642A4D233ull, 0x8C3223A642A4D233ull },
	{ 0x3724F9571CE3205Bull, 0x3724F9571CE3205Bull },
	{ 0x22E50C93934A9634ull, 0x22E50C93934A9634ull },
	{ 0xA8217CD9D4F2AB75ull, 0xA8217CD9D4F2AB75ull },
	{ 0x171EDD8A067CEEF4ull, 0x171EDD8A067CEEF4ull },
	{ 0xBE79C314C147C4CAull, 0xBE79C314C147C4CAull },
	{ 0x8FBF024443C58DE7ull, 0x8FBF024443C58DE7ull },
	{ 0xA277B097E8A05182ull, 0xA277B097E8A05182ull },
	{ 0x06827B69184CBE2Bull, 0x06827B69184CBE2Bull },
	{ 0x3ACDE35E0C6069C1ull, 0x3ACDE35E0C6069C1ull },
	{ 0xA380095F131D8189ull, 0xA380095F131D8189ull },
	{ 0x99D25EF9D9B34C89ull, 0x99D25EF9D9B34C89ull },
	{ 0xBC47F103C488D41Dull, 0xBC47F103C488D41Dull },
	{ 0x0C1DF7B6302A930Bull, 0x0C1DF7B6302A930Bull },
	{ 0xA7157C9B90EBC08Aull, 0xA7157C9B90EBC08Aull },
	{ 0x824EED08C96683C4ull, 0x824EED08C96683C4ull },
	{ 0xA61713E80501151Bull, 0xA61713E80501151Bull },
	{ 0xAF98CB07D2B465C1ull, 0xAF98CB07D2B465C1ull },
	{ 0x15227D8A978F5CD8ull, 0x15227D8A978F5CD8ull },
	{ 0x8084893AC2C6E678ull, 0x8084893AC2C6E678ull },
	{ 0x8457B0C6E6935E69ull, 0x8457B0C6E6935E69ull },
	{ 0x0C2642AC6C604BABull, 0x0C2642AC6C604BABull },
	{ 0x1E810A6D11E776F1ull, 0x1E810A6D11E776F1ull },
	{ 0x8809572901943284ull, 0x8809572901943284ull },
	{ 0x000002CF0564FCFDull, 0x000002CF0564FCFDull },
	{ 0x98DC639C72F656E7ull, 0x98DC639C72F656E7ull },
	{ 0x28768B43298C3A38ull, 0x28768B43298C3A38ull },
	{ 0x06CEE568F05613B5ull, 0x06CEE568F05613B5ull },
	{ 0xB719B295AB052089ull, 0xB719B295AB052089ull },
	{ 0x977368F9C52B4655ull, 0x977368F9C52B4655ull },
	{ 0x941DE391680DB8D4ull, 0x941DE391680DB8D4ull },
	{ 0x82F26F6A2CD24B77ull, 0x82F26F6A2CD24B77ull },
	{ 0x0941E5918DCDF9C6ull, 0x0941E5918DCDF9C6ull },
	{ 0x3C360516475FA17Dull, 0x3C360516475FA17Dull },
	{ 0x348C306E75D5B009ull, 0x348C306E75D5B009ull },
	{ 0x04E41674B18C9A71ull, 0x04E41674B18C9A71ull },
	{ 0x1DF27DBA9F1CE639ull, 0x1DF27DBA9F1CE639ull },
	{ 0x3DDD6F61501EE34Aull, 0x3DDD6F61501EE34Aull },
	{ 0x265BE7AF23FDAD6Dull, 0x265BE7AF23FDAD6Dull },
	{ 0xBD70D42A5CEA74A2ull, 0xBD70D42A5CEA74A2ull },
	{ 0x88BEE98D4A92B568ull, 0x88BEE98D4A92B568ull },
	{ 0x88E240C394A046C1ull, 0x88E240C394A046C1ull },
	{ 0x395712779A689881ull, 0x395712779A689881ull },
	{ 0xADFCD2280889C5ECull, 0xADFCD2280889C5ECull },
	{ 0x81544B897D4CBCF4ull, 0x81544B897D4CBCF4ull },
	{ 0x0B1DB353A3EA18BDull, 0x0B1DB353A3EA18BDull },
	{ 0x9346C693377D7C2Eull, 0x9346C693377D7C2Eull },
	{ 0x80000000000B2E66ull, 0x80000000000B2E66ull },
	{ 0x97C7BA022C556CEDull, 0x97C7BA022C556CEDull },
	{ 0xBC747CBC33EEB1A8ull, 0xBC747CBC33EEB1A8ull },
	{ 0xAF611C0EFA6831FDull, 0xAF611C0EFA6831FDull },
	{ 0x881AFC8891E2F585ull, 0x881AFC8891E2F585ull },
	{ 0xBDA38D0D723BF7D2ull, 0xBDA38D0D723BF7D2ull },
	{ 0x80E0031468BBBF9Full, 0x80E0031468BBBF9Full },
	{ 0x8000000000000004ull, 0x8000000000000004ull },
	{ 0x000000000000929Eull, 0x000000000000929Eull },
	{ 0x80000000000004E1ull, 0x80000000000004E1ull },
	{ 0x00003F26ECC04867ull, 0x00003F26ECC04867ull },
	{ 0x800009DE95F114B9ull, 0x800009DE95F114B9ull },
	{ 0x8005E456C68AB6A9ull, 0x8005E456C68AB6A9ull },
	{ 0x00000003013A070Full, 0x00000003013A070Full },
	{ 0x00000000063DD73Eull, 0x00000000063DD73Eull },
	{ 0x00000001D856E6EDull, 0x00000001D856E6EDull },
	{ 0x800000001430FFD6ull, 0x800000001430FFD6ull },
	{ 0x00000011631A6CF6ull, 0x00000011631A6CF6ull },
	{ 0x800000000270F0D6ull, 0x800000000270F0D6ull },
	{ 0x0000000000000031ull, 0x0000000000000031ull },
	{ 0x8000000215B87365ull, 0x8000000215B87365ull },
	{ 0x8000006A9CA93F23ull, 0x8000006A9CA93F23ull },
	{ 0x0000000005EC5713ull, 0x0000000005EC5713ull },
	{ 0x80000015517C6112ull, 0x80000015517C6112ull },
	{ 0x80000013A977BE80ull, 0x80000013A977BE80ull },
	{ 0x8005114BCF31FCFBull, 0x8005114BCF31FCFBull },
	{ 0x00006A6D874C9BCCull, 0x00006A6D874C9BCCull },
	{ 0x00000000000000B2ull, 0x00000000000000B2ull },
	{ 0x0000000028505CAAull, 0x0000000028505CAAull },
	{ 0x000000001BB1CF76ull, 0x000000001BB1CF76ull },
	{ 0x800000009EFB47E1ull, 0x800000009EFB47E1ull },
	{ 0x00000AC68E717386ull, 0x00000AC68E717386ull },
	{ 0x0000000000003682ull, 0x0000000000003682ull },
	{ 0x00000046F6CA12A1ull, 0x00000046F6CA12A1ull },
	{ 0x800000000000005Eull, 0x800000000000005Eull },
	{ 0x8007185ED3D8025Full, 0x8007185ED3D8025Full },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x80000B76DFFA3FF1ull, 0x80000B76DFFA3FF1ull },
	{ 0x80000004FC243C98ull, 0x80000004FC243C98ull },
	{ 0x0000000C6A3BFFF7ull, 0x0000000C6A3BFFF7ull },
	{ 0x0005545AA125F9BDull, 0x0005545AA125F9BDull },
	{ 0x8000000000019BB0ull, 0x8000000000019BB0ull },
	{ 0x8000000000077592ull, 0x8000000000077592ull },
	{ 0x00000000000000DFull, 0x00000000000000DFull },
	{ 0x80055CD74DA8F651ull, 0x80055CD74DA8F651ull },
	{ 0x000000000000002Aull, 0x000000000000002Aull },
	{ 0x8000000FB2A9BC6Bull, 0x8000000FB2A9BC6Bull },
	{ 0x800008E04CAC4A27ull, 0x800008E04CAC4A27ull },
	{ 0x8000000000353CE7ull, 0x8000000000353CE7ull },
	{ 0x0000000000000029ull, 0x0000000000000029ull },
	{ 0x8000000000000205ull, 0x8000000000000205ull },
	{ 0x000000000025CBC9ull, 0x000000000025CBC9ull },
	{ 0x8001D8A11B77E6B6ull, 0x8001D8A11B77E6B6ull },
	{ 0x000000001CE88ACEull, 0x000000001CE88ACEull },
	{ 0x000000000000104Eull, 0x000000000000104Eull },
	{ 0x8000053E8E38F158ull, 0x8000053E8E38F158ull },
	{ 0x800000003800B7C6ull, 0x800000003800B7C6ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x00000001181D8801ull, 0x00000001181D8801ull },
	{ 0x000000000000005Eull, 0x000000000000005Eull },
	{ 0x000000000000DC55ull, 0x000000000000DC55ull },
	{ 0x00000002FBD02B7Full, 0x00000002FBD02B7Full },
	{ 0x80000000DAF9E509ull, 0x80000000DAF9E509ull },
	{ 0x8003D526BF104510ull, 0x8003D526BF104510ull },
	{ 0x0000000000000006ull, 0x0000000000000006ull },
	{ 0x800000002F2A2655ull, 0x800000002F2A2655ull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x8000000000010231ull, 0x8000000000010231ull },
	{ 0x000000000000000Cull, 0x000000000000000Cull },
	{ 0x00000002A3119D68ull, 0x00000002A3119D68ull },
	{ 0x000000000000B0CFull, 0x000000000000B0CFull },
	{ 0x800035ABE19E9DD1ull, 0x800035ABE19E9DD1ull },
	{ 0x8000000000000337ull, 0x8000000000000337ull },
	{ 0x000000BCFABA6BB1ull, 0x000000BCFABA6BB1ull },
	{ 0x800000000090DAB3ull, 0x800000000090DAB3ull },
	{ 0x80000002CE1B1038ull, 0x80000002CE1B1038ull },
	{ 0x80000000E56AAE51ull, 0x80000000E56AAE51ull },
	{ 0x0000000000004873ull, 0x0000000000004873ull },
	{ 0x000B448301C5CDB1ull, 0x000B448301C5CDB1ull },
	{ 0x8008DB78F6B0A694ull, 0x8008DB78F6B0A694ull },
	{ 0x8009861359436300ull, 0x8009861359436300ull },
	{ 0x000AAFEA74EB9C56ull, 0x000AAFEA74EB9C56ull },
	{ 0x000DB3D2E70E27A7ull, 0x000DB3D2E70E27A7ull },
	{ 0x000D94E0D9DC114Dull, 0x000D94E0D9DC114Dull },
	{ 0x800D89E2BDDC93E5ull, 0x800D89E2BDDC93E5ull },
	{ 0x800DBC959EFD1103ull, 0x800DBC959EFD1103ull },
	{ 0x8009C86AD701ACFEull, 0x8009C86AD701ACFEull },
	{ 0x000E41FDE9356C49ull, 0x000E41FDE9356C49ull },
	{ 0x800C1B05E915CB0Full, 0x800C1B05E915CB0Full },
	{ 0x000822F675898149ull, 0x000822F675898149ull },
	{ 0x000A59D908A1F186ull, 0x000A59D908A1F186ull },
	{ 0x000B63A02EAE33C6ull, 0x000B63A02EAE33C6ull },
	{ 0x800AA6AB89BD2BDCull, 0x800AA6AB89BD2BDCull },
	{ 0x8008B54315371975ull, 0x8008B54315371975ull },
	{ 0x800A540AF8274636ull, 0x800A540AF8274636ull },
	{ 0x8008CBD974844F4Dull, 0x8008CBD974844F4Dull },
	{ 0x000BCC1BC0506955ull, 0x000BCC1BC0506955ull },
	{ 0x800F52A044651D52ull, 0x800F52A044651D52ull }
};

//////////////////////////////////////////////////////////////////////////
CShader5xTest_DoubleArith::Binary64Vec2 const CShader5xTest_dadd::s_DAddSrc0[] = 
{
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xBFF0000000000000ull, 0xBFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FFFCB8D9A5D4659ull, 0x7FFFCB8D9A5D4659ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x7FF6B0586EF53833ull, 0x7FF6B0586EF53833ull },
	{ 0xE832B30C2F906709ull, 0xE832B30C2F906709ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xD3DC7C4B89F05653ull, 0xD3DC7C4B89F05653ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xC4DA8FD9E6AA3677ull, 0xC4DA8FD9E6AA3677ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xC8CB013B39FB3E20ull, 0xC8CB013B39FB3E20ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x5C01D1EEEABC7754ull, 0x5C01D1EEEABC7754ull },
	{ 0x41E21AD82491B346ull, 0x41E21AD82491B346ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF84E1BFCCABC0Bull, 0x7FF84E1BFCCABC0Bull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x8000000000000864ull, 0x8000000000000864ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x800000000010DD88ull, 0x800000000010DD88ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x800000000001F606ull, 0x800000000001F606ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x0000046A7CD1A70Cull, 0x0000046A7CD1A70Cull },
	{ 0x000000000003952Full, 0x000000000003952Full },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xF80D73C050176B53ull, 0xF80D73C050176B53ull },
	{ 0xDAD80CAFB14EC06Cull, 0xDAD80CAFB14EC06Cull },
	{ 0x0D0656509E98A510ull, 0x0D0656509E98A510ull },
	{ 0xCD4086C5BFCDA2BBull, 0xCD4086C5BFCDA2BBull },
	{ 0xE495B5F9B32EFE04ull, 0xE495B5F9B32EFE04ull },
	{ 0x3031A2686B267CF5ull, 0x3031A2686B267CF5ull },
	{ 0x0F381FC69ED1CCAEull, 0x0F381FC69ED1CCAEull },
	{ 0x744BABA648DF8839ull, 0x744BABA648DF8839ull },
	{ 0xDB9CAB673523DFAFull, 0xDB9CAB673523DFAFull },
	{ 0xE456D172A0AADA0Bull, 0xE456D172A0AADA0Bull },
	{ 0xCECA2AF0E92E414Dull, 0xCECA2AF0E92E414Dull },
	{ 0x1D8555184AB52152ull, 0x1D8555184AB52152ull },
	{ 0x9BFE5AC3AF82F4F2ull, 0x9BFE5AC3AF82F4F2ull },
	{ 0x873FBA508FDC665Bull, 0x873FBA508FDC665Bull },
	{ 0x1CA71232DE87B82Cull, 0x1CA71232DE87B82Cull },
	{ 0xCAFAF77E0A8ECB55ull, 0xCAFAF77E0A8ECB55ull },
	{ 0xC89572920CA8C32Dull, 0xC89572920CA8C32Dull },
	{ 0x4CB8A54B86514DD4ull, 0x4CB8A54B86514DD4ull },
	{ 0xADCB17E5F1B387CBull, 0xADCB17E5F1B387CBull },
	{ 0x22812F0EDF2E83D1ull, 0x22812F0EDF2E83D1ull },
	{ 0x686148F945D970FFull, 0x686148F945D970FFull },
	{ 0x8CD802AFE20D5EA7ull, 0x8CD802AFE20D5EA7ull },
	{ 0x9B1A2675A39AA4E8ull, 0x9B1A2675A39AA4E8ull },
	{ 0xEE29C0532D6FE815ull, 0xEE29C0532D6FE815ull },
	{ 0xE67BDC206518C2C8ull, 0xE67BDC206518C2C8ull },
	{ 0xC3746AE9109A05C0ull, 0xC3746AE9109A05C0ull },
	{ 0xE185D03B8651A483ull, 0xE185D03B8651A483ull },
	{ 0x9BC997A06BA03630ull, 0x9BC997A06BA03630ull },
	{ 0x119FD2EA83C21E25ull, 0x119FD2EA83C21E25ull },
	{ 0x908A9E7D8FDE51D1ull, 0x908A9E7D8FDE51D1ull },
	{ 0xE2034F7B3BB0BD5Aull, 0xE2034F7B3BB0BD5Aull },
	{ 0x3D4EC9751F564B72ull, 0x3D4EC9751F564B72ull },
	{ 0x9CBE7B72CC608975ull, 0x9CBE7B72CC608975ull },
	{ 0x37F3858AED76EF47ull, 0x37F3858AED76EF47ull },
	{ 0x619186BE756BC3EDull, 0x619186BE756BC3EDull },
	{ 0x08B69214DE0AA469ull, 0x08B69214DE0AA469ull },
	{ 0x1EE5CC0877C6A959ull, 0x1EE5CC0877C6A959ull },
	{ 0x21CA30A1C70E2BD2ull, 0x21CA30A1C70E2BD2ull },
	{ 0x891F0050F52A2E79ull, 0x891F0050F52A2E79ull },
	{ 0xD7E355A44E2F5FE3ull, 0xD7E355A44E2F5FE3ull },
	{ 0x92F6632AD066C32Full, 0x92F6632AD066C32Full },
	{ 0x59E1D801CAB3F561ull, 0x59E1D801CAB3F561ull },
	{ 0x657FEE9C8CDF1180ull, 0x657FEE9C8CDF1180ull },
	{ 0xA8D2A0E524C83AF1ull, 0xA8D2A0E524C83AF1ull },
	{ 0xD7D987252F6ABD15ull, 0xD7D987252F6ABD15ull },
	{ 0x103BDC35B5F5D8A2ull, 0x103BDC35B5F5D8A2ull },
	{ 0x67A42BE51DE123BEull, 0x67A42BE51DE123BEull },
	{ 0x9D83283228029261ull, 0x9D83283228029261ull },
	{ 0xB087266AFECC1CECull, 0xB087266AFECC1CECull },
	{ 0x18ADC56F56770404ull, 0x18ADC56F56770404ull },
	{ 0x649D3AA49A42B9B4ull, 0x649D3AA49A42B9B4ull },
	{ 0xFBCAC79B2D4D64C3ull, 0xFBCAC79B2D4D64C3ull },
	{ 0xC3FBD753B8AC06D1ull, 0xC3FBD753B8AC06D1ull },
	{ 0xEB3744238A0844B8ull, 0xEB3744238A0844B8ull },
	{ 0x1534454703E9C9A1ull, 0x1534454703E9C9A1ull },
	{ 0xAD9C83A11AF14EE4ull, 0xAD9C83A11AF14EE4ull },
	{ 0x9D1ED836E66B4124ull, 0x9D1ED836E66B4124ull },
	{ 0x23B830C63EC007ABull, 0x23B830C63EC007ABull },
	{ 0x96801E266C27E802ull, 0x96801E266C27E802ull },
	{ 0xC0DE88F1E6288ED4ull, 0xC0DE88F1E6288ED4ull },
	{ 0x2AC80F8821C23184ull, 0x2AC80F8821C23184ull },
	{ 0x82468BA672125B0Cull, 0x82468BA672125B0Cull },
	{ 0x695B3F86F5124B99ull, 0x695B3F86F5124B99ull },
	{ 0xE7902B6494F70163ull, 0xE7902B6494F70163ull },
	{ 0x3FF60B931498DDD0ull, 0x3FF60B931498DDD0ull },
	{ 0xEAB47D0A31A8ECCCull, 0xEAB47D0A31A8ECCCull },
	{ 0xF2B9CBD2D059C962ull, 0xF2B9CBD2D059C962ull },
	{ 0x77BFE7D53C82241Full, 0x77BFE7D53C82241Full },
	{ 0x93B402D778DEEA27ull, 0x93B402D778DEEA27ull },
	{ 0xE4F6558A9B7B0918ull, 0xE4F6558A9B7B0918ull },
	{ 0xFB8F8D763FA2D61Eull, 0xFB8F8D763FA2D61Eull },
	{ 0x6B11626103A915D7ull, 0x6B11626103A915D7ull },
	{ 0xC0DFD36517A39D68ull, 0xC0DFD36517A39D68ull },
	{ 0x9CF88D4FDCE59E5Full, 0x9CF88D4FDCE59E5Full },
	{ 0xB0B5F6DB949D88CBull, 0xB0B5F6DB949D88CBull },
	{ 0x5FB367621F5C921Aull, 0x5FB367621F5C921Aull },
	{ 0xC6500417CB6DDC32ull, 0xC6500417CB6DDC32ull },
	{ 0xE2B5C4CD340339D2ull, 0xE2B5C4CD340339D2ull },
	{ 0xA3322247315D95A9ull, 0xA3322247315D95A9ull },
	{ 0x9795EB6BD5B6F7B7ull, 0x9795EB6BD5B6F7B7ull },
	{ 0x8719C2F37D942C63ull, 0x8719C2F37D942C63ull },
	{ 0xAA2CC45CEEC408DDull, 0xAA2CC45CEEC408DDull },
	{ 0x47FAD21387725236ull, 0x47FAD21387725236ull },
	{ 0xF8800AA67C1443BDull, 0xF8800AA67C1443BDull },
	{ 0xCA16E66027D4B31Cull, 0xCA16E66027D4B31Cull },
	{ 0x8B167C7C6A85D932ull, 0x8B167C7C6A85D932ull },
	{ 0xF7EE70CD187EB532ull, 0xF7EE70CD187EB532ull },
	{ 0x0C8609AB78AA13FCull, 0x0C8609AB78AA13FCull },
	{ 0x3A4BECE5D42D36BAull, 0x3A4BECE5D42D36BAull },
	{ 0x419C0548152C183Eull, 0x419C0548152C183Eull },
	{ 0x700910AE7651529Bull, 0x700910AE7651529Bull },
	{ 0x818F54A140DFA501ull, 0x818F54A140DFA501ull },
	{ 0x668DFCC89C331C5Dull, 0x668DFCC89C331C5Dull },
	{ 0x2D9C9F976C77D3B1ull, 0x2D9C9F976C77D3B1ull },
	{ 0x513A6C3C42FC60B0ull, 0x513A6C3C42FC60B0ull },
	{ 0xA97C80175AA5D59Dull, 0xA97C80175AA5D59Dull },
	{ 0x41D8E464A96B68E4ull, 0x41D8E464A96B68E4ull },
	{ 0x52F3B358FE1FB8F9ull, 0x52F3B358FE1FB8F9ull },
	{ 0xA06BE5812FADB571ull, 0xA06BE5812FADB571ull },
	{ 0x7A3D48425B8F22DEull, 0x7A3D48425B8F22DEull },
	{ 0x23151A2A35E2C26Eull, 0x23151A2A35E2C26Eull },
	{ 0x59AED0EE67E81546ull, 0x59AED0EE67E81546ull },
	{ 0x46868749FE46C51Cull, 0x46868749FE46C51Cull },
	{ 0x8DD691B7E967A993ull, 0x8DD691B7E967A993ull },
	{ 0x454DBC798B456DDAull, 0x454DBC798B456DDAull },
	{ 0x0DA3B10857BAD607ull, 0x0DA3B10857BAD607ull },
	{ 0x108903A180F4A630ull, 0x108903A180F4A630ull },
	{ 0x2228688ABA0D284Bull, 0x2228688ABA0D284Bull },
	{ 0xC756919F03B751C7ull, 0xC756919F03B751C7ull },
	{ 0x5ED633E28AFF896Aull, 0x5ED633E28AFF896Aull },
	{ 0xAF5DB52C99080DEDull, 0xAF5DB52C99080DEDull },
	{ 0x993401669346FBD5ull, 0x993401669346FBD5ull },
	{ 0xAC32080F086BF294ull, 0xAC32080F086BF294ull },
	{ 0x612E61DFCF735C5Full, 0x612E61DFCF735C5Full },
	{ 0x37E09E26388D51D1ull, 0x37E09E26388D51D1ull },
	{ 0x78D1B88C4ADE21C1ull, 0x78D1B88C4ADE21C1ull },
	{ 0x5AC73602133E76DFull, 0x5AC73602133E76DFull },
	{ 0xCC9E6B2603011C15ull, 0xCC9E6B2603011C15ull },
	{ 0x233568FB6164669Cull, 0x233568FB6164669Cull },
	{ 0xEB07124DC675375Cull, 0xEB07124DC675375Cull },
	{ 0x9DF7DA30ADC9A289ull, 0x9DF7DA30ADC9A289ull },
	{ 0xFD08A87A13D9377Aull, 0xFD08A87A13D9377Aull },
	{ 0x4E2D67E72498E34Cull, 0x4E2D67E72498E34Cull },
	{ 0xBDBCBB09FBA07AB7ull, 0xBDBCBB09FBA07AB7ull },
	{ 0x24D65A8F727FDCAFull, 0x24D65A8F727FDCAFull },
	{ 0xF27599FBFF09BA3Eull, 0xF27599FBFF09BA3Eull },
	{ 0x799D956EA4B9FC20ull, 0x799D956EA4B9FC20ull },
	{ 0x62D48748F07DC920ull, 0x62D48748F07DC920ull },
	{ 0x823BC5CB0A432A32ull, 0x823BC5CB0A432A32ull },
	{ 0x50C9E802D2D65350ull, 0x50C9E802D2D65350ull },
	{ 0xCA9493FD12F28798ull, 0xCA9493FD12F28798ull },
	{ 0x705F7377BA8C9FA6ull, 0x705F7377BA8C9FA6ull },
	{ 0xAD1DD2242F7D302Full, 0xAD1DD2242F7D302Full },
	{ 0xDFEC6318AC904F5Eull, 0xDFEC6318AC904F5Eull },
	{ 0x4A00ADBFB160FA72ull, 0x4A00ADBFB160FA72ull },
	{ 0xA023A3847F861C98ull, 0xA023A3847F861C98ull },
	{ 0x0054DE0BAB773506ull, 0x0054DE0BAB773506ull },
	{ 0x0BDC0396BA6E9C58ull, 0x0BDC0396BA6E9C58ull },
	{ 0xE4E5D521D16F6A2Eull, 0xE4E5D521D16F6A2Eull },
	{ 0xD032645A7574FC85ull, 0xD032645A7574FC85ull },
	{ 0x3AF1EFDE58321DD6ull, 0x3AF1EFDE58321DD6ull },
	{ 0x6475E9A139A5C76Cull, 0x6475E9A139A5C76Cull },
	{ 0x6C03A4C8D52B8C8Aull, 0x6C03A4C8D52B8C8Aull },
	{ 0x826E29FDE18D9E3Dull, 0x826E29FDE18D9E3Dull },
	{ 0x7FD4B5C11F7E6F7Dull, 0x7FD4B5C11F7E6F7Dull },
	{ 0x9DCB5DE3787FFE0Bull, 0x9DCB5DE3787FFE0Bull },
	{ 0x147B5ED02E5EB705ull, 0x147B5ED02E5EB705ull },
	{ 0xDCB090E81A05FDCDull, 0xDCB090E81A05FDCDull },
	{ 0x5601867CFA94501Cull, 0x5601867CFA94501Cull },
	{ 0x8FA7D0ABD4350E61ull, 0x8FA7D0ABD4350E61ull },
	{ 0xF705EDA55E65DF5Full, 0xF705EDA55E65DF5Full },
	{ 0x29205D8384ADB906ull, 0x29205D8384ADB906ull },
	{ 0x2847601AF42B8511ull, 0x2847601AF42B8511ull },
	{ 0xB606DA27BAC264E0ull, 0xB606DA27BAC264E0ull },
	{ 0x90C9E3A3F7499A96ull, 0x90C9E3A3F7499A96ull },
	{ 0x55877C2796C60EF6ull, 0x55877C2796C60EF6ull },
	{ 0xC5B5E900251373FAull, 0xC5B5E900251373FAull },
	{ 0x30EA3B1FAA030FB5ull, 0x30EA3B1FAA030FB5ull },
	{ 0x42917B519AD61EEAull, 0x42917B519AD61EEAull },
	{ 0x73D90134D699DAECull, 0x73D90134D699DAECull },
	{ 0xE1236FE9B74B2238ull, 0xE1236FE9B74B2238ull },
	{ 0x442FDB732FC7BC4Full, 0x442FDB732FC7BC4Full },
	{ 0xD16096D9FAB340D9ull, 0xD16096D9FAB340D9ull },
	{ 0xBA072910D9D59B77ull, 0xBA072910D9D59B77ull },
	{ 0x1192EAC8E5193568ull, 0x1192EAC8E5193568ull },
	{ 0xD7C7CBD4E9AEB85Full, 0xD7C7CBD4E9AEB85Full },
	{ 0xE62AC0CAD8BC7622ull, 0xE62AC0CAD8BC7622ull },
	{ 0x0209580245DF6DE6ull, 0x0209580245DF6DE6ull },
	{ 0xD1E9F83EF7A3F2EAull, 0xD1E9F83EF7A3F2EAull },
	{ 0x77AA4486857BF0D2ull, 0x77AA4486857BF0D2ull },
	{ 0x0DD92B500618D4C5ull, 0x0DD92B500618D4C5ull },
	{ 0xC9C21AACD0AF1049ull, 0xC9C21AACD0AF1049ull },
	{ 0x0116DE9948C9405Dull, 0x0116DE9948C9405Dull },
	{ 0xD3BDA614C0B0F3D7ull, 0xD3BDA614C0B0F3D7ull },
	{ 0xB940B0EF65931200ull, 0xB940B0EF65931200ull },
	{ 0xB325252D3FE5DF6Dull, 0xB325252D3FE5DF6Dull },
	{ 0xE2168C2D42B6A2A0ull, 0xE2168C2D42B6A2A0ull },
	{ 0xBFFC78B36964EE5Full, 0xBFFC78B36964EE5Full },
	{ 0xBD87D404E8318353ull, 0xBD87D404E8318353ull },
	{ 0x043D53F56966D8E3ull, 0x043D53F56966D8E3ull },
	{ 0x9BCA97545E814052ull, 0x9BCA97545E814052ull },
	{ 0xB44B6CB95D5CAE17ull, 0xB44B6CB95D5CAE17ull },
	{ 0x537CBF1E8F8E1E7Bull, 0x537CBF1E8F8E1E7Bull },
	{ 0x26C2AB48332E9A79ull, 0x26C2AB48332E9A79ull },
	{ 0x392F3E88267DDDD1ull, 0x392F3E88267DDDD1ull },
	{ 0x4C9F5ABC89E2A0EEull, 0x4C9F5ABC89E2A0EEull },
	{ 0x980B44386E2E787Aull, 0x980B44386E2E787Aull },
	{ 0xA8D057D5926B60BEull, 0xA8D057D5926B60BEull },
	{ 0x2921683E33D8E1BEull, 0x2921683E33D8E1BEull },
	{ 0x48E342CDED5ED314ull, 0x48E342CDED5ED314ull },
	{ 0x0C22DDD8A51BC70Cull, 0x0C22DDD8A51BC70Cull },
	{ 0xBC6AADB68D150A81ull, 0xBC6AADB68D150A81ull },
	{ 0x3EA4A28430EE50F7ull, 0x3EA4A28430EE50F7ull },
	{ 0x0D2E4D95952EF3F8ull, 0x0D2E4D95952EF3F8ull },
	{ 0x14CCAE6B66BADFB2ull, 0x14CCAE6B66BADFB2ull },
	{ 0x3745296939451551ull, 0x3745296939451551ull },
	{ 0xB0A9241BD721D09Aull, 0xB0A9241BD721D09Aull },
	{ 0x2129CB2E9EFE4FC8ull, 0x2129CB2E9EFE4FC8ull },
	{ 0x7AD1807EF3E4382Bull, 0x7AD1807EF3E4382Bull },
	{ 0x93786F4CBC4E9DFEull, 0x93786F4CBC4E9DFEull },
	{ 0x9A11CB16F036A687ull, 0x9A11CB16F036A687ull },
	{ 0x30F73443389DD475ull, 0x30F73443389DD475ull },
	{ 0xEB5FC61F9F3BE5F5ull, 0xEB5FC61F9F3BE5F5ull },
	{ 0x6EE84BBD4C166195ull, 0x6EE84BBD4C166195ull },
	{ 0xF9DA1DF355E7B8DDull, 0xF9DA1DF355E7B8DDull },
	{ 0x8C862908A01C0EABull, 0x8C862908A01C0EABull },
	{ 0x771199D3CA099DCFull, 0x771199D3CA099DCFull },
	{ 0x8B79B2F02EAABE67ull, 0x8B79B2F02EAABE67ull },
	{ 0x32EC4B36F23F8C7Bull, 0x32EC4B36F23F8C7Bull },
	{ 0x27AA721B22DD2D5Cull, 0x27AA721B22DD2D5Cull },
	{ 0x9322BDF7E610B639ull, 0x9322BDF7E610B639ull },
	{ 0x4F38C18FB91BB182ull, 0x4F38C18FB91BB182ull },
	{ 0x8C2728EBBEB04680ull, 0x8C2728EBBEB04680ull },
	{ 0x9A71FF7A1EB2FCB5ull, 0x9A71FF7A1EB2FCB5ull },
	{ 0x18669CFA756823F1ull, 0x18669CFA756823F1ull },
	{ 0xC40EBBD75192D935ull, 0xC40EBBD75192D935ull },
	{ 0x309435BBC4EBAD4Bull, 0x309435BBC4EBAD4Bull },
	{ 0x20BAE9A40174EB25ull, 0x20BAE9A40174EB25ull },
	{ 0x43CE493809527CF6ull, 0x43CE493809527CF6ull },
	{ 0x17C80E2F6E2B718Full, 0x17C80E2F6E2B718Full },
	{ 0xAF2D97ED1F3B25FBull, 0xAF2D97ED1F3B25FBull },
	{ 0x1A79711248D805DDull, 0x1A79711248D805DDull },
	{ 0x363E7CF743D5F608ull, 0x363E7CF743D5F608ull },
	{ 0x9C7A4CC993CA59DFull, 0x9C7A4CC993CA59DFull },
	{ 0x7AB41DDDF697B5EFull, 0x7AB41DDDF697B5EFull },
	{ 0x8BADFE358286FA4Aull, 0x8BADFE358286FA4Aull },
	{ 0x74B59AA4D6D465A5ull, 0x74B59AA4D6D465A5ull },
	{ 0xC81A347C59360DB4ull, 0xC81A347C59360DB4ull },
	{ 0x6A453DE085020245ull, 0x6A453DE085020245ull },
	{ 0x1C2B21314C7A159Bull, 0x1C2B21314C7A159Bull },
	{ 0xE811ACAF83A3410Dull, 0xE811ACAF83A3410Dull },
	{ 0x3471A91F632EAC5Eull, 0x3471A91F632EAC5Eull },
	{ 0xB8C1169714DA515Eull, 0xB8C1169714DA515Eull },
	{ 0x17EB92B8540F45C0ull, 0x17EB92B8540F45C0ull },
	{ 0x5D6665641C639D3Eull, 0x5D6665641C639D3Eull },
	{ 0x1F95B74767C5F1EBull, 0x1F95B74767C5F1EBull },
	{ 0x05106E0FFF388857ull, 0x05106E0FFF388857ull },
	{ 0xEED832C857A21868ull, 0xEED832C857A21868ull },
	{ 0x05731F4982DA3077ull, 0x05731F4982DA3077ull },
	{ 0xEB5C93622D773C29ull, 0xEB5C93622D773C29ull },
	{ 0xD23AB2C1ADC42C90ull, 0xD23AB2C1ADC42C90ull },
	{ 0x34FE0A062463BA82ull, 0x34FE0A062463BA82ull },
	{ 0xF558DC9CA8474F38ull, 0xF558DC9CA8474F38ull },
	{ 0x9DC399D288FF89A9ull, 0x9DC399D288FF89A9ull },
	{ 0x071BFB269ACC63A8ull, 0x071BFB269ACC63A8ull },
	{ 0xA23448B99872F9BCull, 0xA23448B99872F9BCull },
	{ 0x25E0478C8F3EEFBFull, 0x25E0478C8F3EEFBFull },
	{ 0x60C54EEC60B576B9ull, 0x60C54EEC60B576B9ull },
	{ 0x6E9B05994FDFF37Eull, 0x6E9B05994FDFF37Eull },
	{ 0x92624749A1344406ull, 0x92624749A1344406ull },
	{ 0x742FB2F64880A78Bull, 0x742FB2F64880A78Bull },
	{ 0x823F5F10A9E14169ull, 0x823F5F10A9E14169ull },
	{ 0xB7BF3850666540B1ull, 0xB7BF3850666540B1ull },
	{ 0x059586183CA9A68Eull, 0x059586183CA9A68Eull },
	{ 0xA0E118A0F6CFABDCull, 0xA0E118A0F6CFABDCull },
	{ 0x74289E896AD4C686ull, 0x74289E896AD4C686ull },
	{ 0xF566A1DA88725120ull, 0xF566A1DA88725120ull },
	{ 0x1FB7AD7E784ACEC4ull, 0x1FB7AD7E784ACEC4ull },
	{ 0xD1281B99CB0AD231ull, 0xD1281B99CB0AD231ull },
	{ 0x4C6709E5B90384A0ull, 0x4C6709E5B90384A0ull },
	{ 0x1E8BF87A6FA4CAE9ull, 0x1E8BF87A6FA4CAE9ull },
	{ 0xA8A48DC8721D09D7ull, 0xA8A48DC8721D09D7ull },
	{ 0x8DE805B007849047ull, 0x8DE805B007849047ull },
	{ 0x4505C575BA1A9887ull, 0x4505C575BA1A9887ull },
	{ 0x8E1C9AA2E5F9F2EDull, 0x8E1C9AA2E5F9F2EDull },
	{ 0x0C501F91580B4635ull, 0x0C501F91580B4635ull },
	{ 0xC409C970FDDAFB1Full, 0xC409C970FDDAFB1Full },
	{ 0xBBDB1D86A418C049ull, 0xBBDB1D86A418C049ull },
	{ 0x82FE8EB2C4A6AE49ull, 0x82FE8EB2C4A6AE49ull },
	{ 0xDACC831F677E100Eull, 0xDACC831F677E100Eull },
	{ 0xC47201440FCFC978ull, 0xC47201440FCFC978ull },
	{ 0xB2DE867EBC6658B5ull, 0xB2DE867EBC6658B5ull },
	{ 0x202682F0F62F835Eull, 0x202682F0F62F835Eull },
	{ 0xC7D0009FEE739651ull, 0xC7D0009FEE739651ull },
	{ 0xC426EFA6AF204ED0ull, 0xC426EFA6AF204ED0ull },
	{ 0xCFFE9CB9592D5FDAull, 0xCFFE9CB9592D5FDAull },
	{ 0x6C9DCDE873809749ull, 0x6C9DCDE873809749ull },
	{ 0xAB7B08A34A17A5ACull, 0xAB7B08A34A17A5ACull },
	{ 0x5C067EC85D5D8266ull, 0x5C067EC85D5D8266ull },
	{ 0xDD1724A2DFF17287ull, 0xDD1724A2DFF17287ull },
	{ 0xD3286DE446C5AFEAull, 0xD3286DE446C5AFEAull },
	{ 0x01AE3398E91BAB8Full, 0x01AE3398E91BAB8Full },
	{ 0x890C4126AC43F8BAull, 0x890C4126AC43F8BAull },
	{ 0x48470A94C627CF4Bull, 0x48470A94C627CF4Bull },
	{ 0x22DB094B8A0231E0ull, 0x22DB094B8A0231E0ull },
	{ 0xEA5041AB484EB5A9ull, 0xEA5041AB484EB5A9ull },
	{ 0x2C1771093DD3E78Aull, 0x2C1771093DD3E78Aull },
	{ 0x236662F1920F5276ull, 0x236662F1920F5276ull },
	{ 0x8E90EE5D66A9230Aull, 0x8E90EE5D66A9230Aull },
	{ 0xA9541B7BF67571E8ull, 0xA9541B7BF67571E8ull },
	{ 0x0A30F0FDC3E92555ull, 0x0A30F0FDC3E92555ull },
	{ 0xA45C6A20D9487A16ull, 0xA45C6A20D9487A16ull },
	{ 0x2D42138D16752986ull, 0x2D42138D16752986ull },
	{ 0xA6BED11F90222CFCull, 0xA6BED11F90222CFCull },
	{ 0x4C414B33FFD325DBull, 0x4C414B33FFD325DBull },
	{ 0xA515848B416D6479ull, 0xA515848B416D6479ull },
	{ 0x79E9175DE3008A35ull, 0x79E9175DE3008A35ull },
	{ 0x6E6D9B39C40CD4D3ull, 0x6E6D9B39C40CD4D3ull },
	{ 0x7F36B219C3ACFF9Aull, 0x7F36B219C3ACFF9Aull },
	{ 0x23A93E768069CD30ull, 0x23A93E768069CD30ull },
	{ 0xD3693FFF265D2AB5ull, 0xD3693FFF265D2AB5ull },
	{ 0x303BD9F94FAAF69Dull, 0x303BD9F94FAAF69Dull },
	{ 0x93FB0016F1356852ull, 0x93FB0016F1356852ull },
	{ 0xC1D04C215BD31389ull, 0xC1D04C215BD31389ull },
	{ 0x7CA47BC84A6C90E4ull, 0x7CA47BC84A6C90E4ull },
	{ 0x8AEA0023E060248Aull, 0x8AEA0023E060248Aull },
	{ 0xDBB4BCF35ABDC0CCull, 0xDBB4BCF35ABDC0CCull },
	{ 0x452FD4A01A114A26ull, 0x452FD4A01A114A26ull },
	{ 0xB0BA643BF084F165ull, 0xB0BA643BF084F165ull },
	{ 0x7F915A2DFFD182B1ull, 0x7F915A2DFFD182B1ull },
	{ 0x8150AFC76D9A333Bull, 0x8150AFC76D9A333Bull },
	{ 0xFE8B2790E6B38F2Bull, 0xFE8B2790E6B38F2Bull },
	{ 0x17701523F12387C8ull, 0x17701523F12387C8ull },
	{ 0x373EAC9310C8A0E9ull, 0x373EAC9310C8A0E9ull },
	{ 0x5A1E5A13B876409Bull, 0x5A1E5A13B876409Bull },
	{ 0xE86BA14D0DB41B92ull, 0xE86BA14D0DB41B92ull },
	{ 0x656104FE7CAEC455ull, 0x656104FE7CAEC455ull },
	{ 0xB1FB764119C75624ull, 0xB1FB764119C75624ull },
	{ 0x2311CCB6D550482Dull, 0x2311CCB6D550482Dull },
	{ 0xAD0EC4CA7E6557EFull, 0xAD0EC4CA7E6557EFull },
	{ 0x61C5089395EC96EDull, 0x61C5089395EC96EDull },
	{ 0x03FCBE40F1AE9A17ull, 0x03FCBE40F1AE9A17ull },
	{ 0x6D82207D2F53C9F5ull, 0x6D82207D2F53C9F5ull },
	{ 0x0A849CEFF734CA15ull, 0x0A849CEFF734CA15ull },
	{ 0xA7ADFCAF0F4C0EB6ull, 0xA7ADFCAF0F4C0EB6ull },
	{ 0x557113423F3684B4ull, 0x557113423F3684B4ull },
	{ 0x1EA7762D01116436ull, 0x1EA7762D01116436ull },
	{ 0xD31DBC6106E31A96ull, 0xD31DBC6106E31A96ull },
	{ 0x2655C64C8C1D5B10ull, 0x2655C64C8C1D5B10ull },
	{ 0xE2AE8C2778E64D2Eull, 0xE2AE8C2778E64D2Eull },
	{ 0xEFA8F6D25576D773ull, 0xEFA8F6D25576D773ull },
	{ 0xECE93E6A0CBC10C5ull, 0xECE93E6A0CBC10C5ull },
	{ 0x20FC52237EC9CCA0ull, 0x20FC52237EC9CCA0ull },
	{ 0xFBB0C7F1E5CC4AF8ull, 0xFBB0C7F1E5CC4AF8ull },
	{ 0xEDBAD34B070702EEull, 0xEDBAD34B070702EEull },
	{ 0x5232C5AC368E93B7ull, 0x5232C5AC368E93B7ull },
	{ 0x2EA595732A0BD248ull, 0x2EA595732A0BD248ull },
	{ 0x84D6ED4D9CF5F545ull, 0x84D6ED4D9CF5F545ull },
	{ 0x79EC4472C0D1E5ABull, 0x79EC4472C0D1E5ABull },
	{ 0x11307D0983E4AB3Cull, 0x11307D0983E4AB3Cull },
	{ 0xBAB78CAEA0E6FAABull, 0xBAB78CAEA0E6FAABull },
	{ 0x7631AB5D8427E388ull, 0x7631AB5D8427E388ull },
	{ 0x49958B0EFD6CA06Eull, 0x49958B0EFD6CA06Eull },
	{ 0x4BF11770C33B7F6Aull, 0x4BF11770C33B7F6Aull },
	{ 0x04483CE8C5DFF62Dull, 0x04483CE8C5DFF62Dull },
	{ 0x5BF138C752D5CEF5ull, 0x5BF138C752D5CEF5ull },
	{ 0xE5D5F03B076970B8ull, 0xE5D5F03B076970B8ull },
	{ 0xB8C9CFA9933B5612ull, 0xB8C9CFA9933B5612ull },
	{ 0x1E8BB3004BF494F0ull, 0x1E8BB3004BF494F0ull },
	{ 0xD653549F8FDA8C7Dull, 0xD653549F8FDA8C7Dull },
	{ 0xABE1C473F9CFB84Cull, 0xABE1C473F9CFB84Cull },
	{ 0xA1D468A062AF98C7ull, 0xA1D468A062AF98C7ull },
	{ 0x70A203CBB553C4D7ull, 0x70A203CBB553C4D7ull },
	{ 0x1981459591B01554ull, 0x1981459591B01554ull },
	{ 0x4CE060E4BF13F4AFull, 0x4CE060E4BF13F4AFull },
	{ 0x653D2D52707ECDDDull, 0x653D2D52707ECDDDull },
	{ 0xB5D54C4555559606ull, 0xB5D54C4555559606ull },
	{ 0x6DBDDA9F7F27806Eull, 0x6DBDDA9F7F27806Eull },
	{ 0x4E1A2692124FC723ull, 0x4E1A2692124FC723ull },
	{ 0xF530F079CB229CE7ull, 0xF530F079CB229CE7ull },
	{ 0xFD57B1374E7338DFull, 0xFD57B1374E7338DFull },
	{ 0xAF136A664EE1057Dull, 0xAF136A664EE1057Dull },
	{ 0x88967DE67E39EDAAull, 0x88967DE67E39EDAAull },
	{ 0xD7FB0CF9519FCD37ull, 0xD7FB0CF9519FCD37ull },
	{ 0x2A315F6793BEFB06ull, 0x2A315F6793BEFB06ull },
	{ 0x4DD1D431C783FB73ull, 0x4DD1D431C783FB73ull },
	{ 0xA9A9D83E5D5E4888ull, 0xA9A9D83E5D5E4888ull },
	{ 0xA7B365A8B2BE46E1ull, 0xA7B365A8B2BE46E1ull },
	{ 0xF3BE8BC3E75C4E5Cull, 0xF3BE8BC3E75C4E5Cull },
	{ 0xCE1C7EB48076DA05ull, 0xCE1C7EB48076DA05ull },
	{ 0x3273AE13DA55D740ull, 0x3273AE13DA55D740ull },
	{ 0x129C65606C811138ull, 0x129C65606C811138ull },
	{ 0x651D734DDC77C086ull, 0x651D734DDC77C086ull },
	{ 0xC81858B7DFD739A5ull, 0xC81858B7DFD739A5ull },
	{ 0x782C808AF07EB915ull, 0x782C808AF07EB915ull },
	{ 0xDAC0FF65CEDE534Dull, 0xDAC0FF65CEDE534Dull },
	{ 0xDD6C5AE6D5FF01E7ull, 0xDD6C5AE6D5FF01E7ull },
	{ 0xE809D9281C71D086ull, 0xE809D9281C71D086ull },
	{ 0xA3F3D85D69F42D8Aull, 0xA3F3D85D69F42D8Aull },
	{ 0x5682AE36F7F75576ull, 0x5682AE36F7F75576ull },
	{ 0x8FF78F3C0618E120ull, 0x8FF78F3C0618E120ull },
	{ 0x6E12FEE03D7A771Bull, 0x6E12FEE03D7A771Bull },
	{ 0xB30E4666E1869461ull, 0xB30E4666E1869461ull },
	{ 0x6FF774B6D6107E44ull, 0x6FF774B6D6107E44ull },
	{ 0x03E76552701D4F16ull, 0x03E76552701D4F16ull },
	{ 0xAE7150F618C42417ull, 0xAE7150F618C42417ull },
	{ 0x769363B0C2736C9Eull, 0x769363B0C2736C9Eull },
	{ 0xA547DDD928C9558Bull, 0xA547DDD928C9558Bull },
	{ 0x145E39F2E64D5A6Cull, 0x145E39F2E64D5A6Cull },
	{ 0x914BDDE65678F06Dull, 0x914BDDE65678F06Dull },
	{ 0x13CBD11E471D5605ull, 0x13CBD11E471D5605ull },
	{ 0x88ACFC6E7E54835Dull, 0x88ACFC6E7E54835Dull },
	{ 0x709672360C0B3203ull, 0x709672360C0B3203ull },
	{ 0x12D13CC06BD21150ull, 0x12D13CC06BD21150ull },
	{ 0x83DF37387A041096ull, 0x83DF37387A041096ull },
	{ 0xC05E6B7035EFCE0Eull, 0xC05E6B7035EFCE0Eull },
	{ 0x1BC07AB053922680ull, 0x1BC07AB053922680ull },
	{ 0xC3358CB2A08FE033ull, 0xC3358CB2A08FE033ull },
	{ 0x13F2473A36BC7B17ull, 0x13F2473A36BC7B17ull },
	{ 0x06804DAC7D9C1EB1ull, 0x06804DAC7D9C1EB1ull },
	{ 0xED10C85DFFAEA746ull, 0xED10C85DFFAEA746ull },
	{ 0xC6A074160B71D3CAull, 0xC6A074160B71D3CAull },
	{ 0x6544B9DC2728950Aull, 0x6544B9DC2728950Aull },
	{ 0x29414E1254D57B18ull, 0x29414E1254D57B18ull },
	{ 0x8FBDD82F237642F6ull, 0x8FBDD82F237642F6ull },
	{ 0x555B223A92AE8386ull, 0x555B223A92AE8386ull },
	{ 0xED0F5000C6AF7EB1ull, 0xED0F5000C6AF7EB1ull },
	{ 0x8AE89F7D89CB0C56ull, 0x8AE89F7D89CB0C56ull },
	{ 0x82EE2EF3A13FA9F5ull, 0x82EE2EF3A13FA9F5ull },
	{ 0x3B6E4D92ED2BA71Cull, 0x3B6E4D92ED2BA71Cull },
	{ 0x7451D2A05F757691ull, 0x7451D2A05F757691ull },
	{ 0x2183FCA2BB9718BFull, 0x2183FCA2BB9718BFull },
	{ 0x99B659872B9AAAE6ull, 0x99B659872B9AAAE6ull },
	{ 0x59203560A1F6167Dull, 0x59203560A1F6167Dull },
	{ 0x187112950C93DD6Bull, 0x187112950C93DD6Bull },
	{ 0x7765A8BB59370866ull, 0x7765A8BB59370866ull },
	{ 0x7B66ECFD4608362Aull, 0x7B66ECFD4608362Aull },
	{ 0xA2A2A0400779C85Eull, 0xA2A2A0400779C85Eull },
	{ 0xCA94E886B8D13143ull, 0xCA94E886B8D13143ull },
	{ 0xAAA26E71A083621Eull, 0xAAA26E71A083621Eull },
	{ 0xA33A85CD42F25AE6ull, 0xA33A85CD42F25AE6ull },
	{ 0xA020DD7444B9D431ull, 0xA020DD7444B9D431ull },
	{ 0x4FD635971ED9125Bull, 0x4FD635971ED9125Bull },
	{ 0xED7FA08CA127CE79ull, 0xED7FA08CA127CE79ull },
	{ 0xE033C95E44E4487Cull, 0xE033C95E44E4487Cull },
	{ 0xEDDDC37350487229ull, 0xEDDDC37350487229ull },
	{ 0xC16CE28DC9744038ull, 0xC16CE28DC9744038ull },
	{ 0x86011A1C395B144Aull, 0x86011A1C395B144Aull },
	{ 0xD5BB64133E824E10ull, 0xD5BB64133E824E10ull },
	{ 0x3BE1B131F0FEF9B9ull, 0x3BE1B131F0FEF9B9ull },
	{ 0x8C8DE04413409A1Bull, 0x8C8DE04413409A1Bull },
	{ 0xCE973DC518171AA5ull, 0xCE973DC518171AA5ull },
	{ 0xFACE0AF6F341DC04ull, 0xFACE0AF6F341DC04ull },
	{ 0xCE888DAFBD75E194ull, 0xCE888DAFBD75E194ull },
	{ 0x726927D127691F89ull, 0x726927D127691F89ull },
	{ 0x354CF6CCBC10EA5Dull, 0x354CF6CCBC10EA5Dull },
	{ 0x1ABA7B07F56181FBull, 0x1ABA7B07F56181FBull },
	{ 0xFDDFC7A31CBCC22Dull, 0xFDDFC7A31CBCC22Dull },
	{ 0x8837B9B6001DF0C3ull, 0x8837B9B6001DF0C3ull },
	{ 0xBC55B7A57430A984ull, 0xBC55B7A57430A984ull },
	{ 0x54AF7904A760EE59ull, 0x54AF7904A760EE59ull },
	{ 0xB6145625451252B7ull, 0xB6145625451252B7ull },
	{ 0x37199F09684C4C4Dull, 0x37199F09684C4C4Dull },
	{ 0xF9EB7B7A5EC9A0EEull, 0xF9EB7B7A5EC9A0EEull },
	{ 0x1BC9D2173CB8F0C0ull, 0x1BC9D2173CB8F0C0ull },
	{ 0xF780B81E3DDC6A25ull, 0xF780B81E3DDC6A25ull },
	{ 0x3C33E8A3FAD69269ull, 0x3C33E8A3FAD69269ull },
	{ 0x982CC4FB6B1236ADull, 0x982CC4FB6B1236ADull },
	{ 0xB7425A81B7537993ull, 0xB7425A81B7537993ull },
	{ 0x562AF3F0DD6000AAull, 0x562AF3F0DD6000AAull },
	{ 0x28DEAF8C228C419Cull, 0x28DEAF8C228C419Cull },
	{ 0x554A9F29579BF315ull, 0x554A9F29579BF315ull },
	{ 0x425BEFC9EA8397F6ull, 0x425BEFC9EA8397F6ull },
	{ 0xFA529906CA792D3Aull, 0xFA529906CA792D3Aull },
	{ 0x622314421A21FAA7ull, 0x622314421A21FAA7ull },
	{ 0x03869CC1B30B7E39ull, 0x03869CC1B30B7E39ull },
	{ 0x2C21766578FD7ACBull, 0x2C21766578FD7ACBull },
	{ 0x203EBE1C7BD32786ull, 0x203EBE1C7BD32786ull },
	{ 0xA149400BEC297B8Eull, 0xA149400BEC297B8Eull },
	{ 0x8900D8CADC109E6Bull, 0x8900D8CADC109E6Bull },
	{ 0x37E3DB85D50E72B5ull, 0x37E3DB85D50E72B5ull },
	{ 0x97EC01CD38604783ull, 0x97EC01CD38604783ull },
	{ 0x737663817349A594ull, 0x737663817349A594ull },
	{ 0x6005F62001DE3BBFull, 0x6005F62001DE3BBFull },
	{ 0xFCC813C44214EE17ull, 0xFCC813C44214EE17ull },
	{ 0xC5AA8B5F16D20462ull, 0xC5AA8B5F16D20462ull },
	{ 0xB9EAB7D85AB676BCull, 0xB9EAB7D85AB676BCull },
	{ 0x169D1F9D22E25D87ull, 0x169D1F9D22E25D87ull },
	{ 0x66051C0DD2B87D15ull, 0x66051C0DD2B87D15ull },
	{ 0xA1BC096EFDD0F518ull, 0xA1BC096EFDD0F518ull },
	{ 0x29BF7D901CE90F49ull, 0x29BF7D901CE90F49ull },
	{ 0xCF0607EB0DBE2BD7ull, 0xCF0607EB0DBE2BD7ull },
	{ 0x32B1F96F6921CE92ull, 0x32B1F96F6921CE92ull },
	{ 0xB811B2FCA474D257ull, 0xB811B2FCA474D257ull },
	{ 0xCC14FDBE0457AEBEull, 0xCC14FDBE0457AEBEull },
	{ 0xBF86EA4B617DED01ull, 0xBF86EA4B617DED01ull },
	{ 0x7613B881B8B2B32Full, 0x7613B881B8B2B32Full },
	{ 0xA92A46B690FC7212ull, 0xA92A46B690FC7212ull },
	{ 0x7E4089192C6DB45Dull, 0x7E4089192C6DB45Dull },
	{ 0xB23A89448C510999ull, 0xB23A89448C510999ull },
	{ 0x976B6E7543FE19D1ull, 0x976B6E7543FE19D1ull },
	{ 0x29000BA61A45CD7Cull, 0x29000BA61A45CD7Cull },
	{ 0xF17A78867F839F2Aull, 0xF17A78867F839F2Aull },
	{ 0x054E300DCDC208F2ull, 0x054E300DCDC208F2ull },
	{ 0xD1C437FE5D40111Aull, 0xD1C437FE5D40111Aull },
	{ 0xFFD6D0DB6745FA88ull, 0xFFD6D0DB6745FA88ull },
	{ 0x2ACC2D24B8570F68ull, 0x2ACC2D24B8570F68ull },
	{ 0x2AEFB13B342E921Eull, 0x2AEFB13B342E921Eull },
	{ 0xB45139C12815CA6Eull, 0xB45139C12815CA6Eull },
	{ 0x09D9ECD96E8131E9ull, 0x09D9ECD96E8131E9ull },
	{ 0x837E0FB861C2C41Aull, 0x837E0FB861C2C41Aull },
	{ 0xA95B67C99E7F6D71ull, 0xA95B67C99E7F6D71ull },
	{ 0xA26BA13D96F5936Full, 0xA26BA13D96F5936Full },
	{ 0xB73A3EA1F702CA12ull, 0xB73A3EA1F702CA12ull },
	{ 0x28B59117D6769D83ull, 0x28B59117D6769D83ull },
	{ 0x9ABA368ABAE080FDull, 0x9ABA368ABAE080FDull },
	{ 0x5310A2BB6C66DC00ull, 0x5310A2BB6C66DC00ull },
	{ 0x8C2C2D2197953B36ull, 0x8C2C2D2197953B36ull },
	{ 0x9BA329F7412F9A25ull, 0x9BA329F7412F9A25ull },
	{ 0xAD4F889108B8D516ull, 0xAD4F889108B8D516ull },
	{ 0x800000B467DED58Eull, 0x800000B467DED58Eull },
	{ 0x800320D5A73BB69Cull, 0x800320D5A73BB69Cull },
	{ 0x000000000000062Eull, 0x000000000000062Eull },
	{ 0x00000E462EA0F7ACull, 0x00000E462EA0F7ACull },
	{ 0x8000000000001BF4ull, 0x8000000000001BF4ull },
	{ 0x000000000000217Cull, 0x000000000000217Cull },
	{ 0x8000000000000005ull, 0x8000000000000005ull },
	{ 0x80000001693A04A1ull, 0x80000001693A04A1ull },
	{ 0x0000000000000005ull, 0x0000000000000005ull },
	{ 0x00000000049C4543ull, 0x00000000049C4543ull },
	{ 0x0000B7D416734428ull, 0x0000B7D416734428ull },
	{ 0x00000000C9672D71ull, 0x00000000C9672D71ull },
	{ 0x8000000007DE9F43ull, 0x8000000007DE9F43ull },
	{ 0x80000000000000DEull, 0x80000000000000DEull },
	{ 0x8000000000784620ull, 0x8000000000784620ull },
	{ 0x800000000003515Cull, 0x800000000003515Cull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x000034851A257CDDull, 0x000034851A257CDDull },
	{ 0x000189CB402479D0ull, 0x000189CB402479D0ull },
	{ 0x000000000887513Bull, 0x000000000887513Bull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x800000000515AA1Full, 0x800000000515AA1Full },
	{ 0x8000000000000005ull, 0x8000000000000005ull },
	{ 0x000000000000001Full, 0x000000000000001Full },
	{ 0x000000000000005Dull, 0x000000000000005Dull },
	{ 0x8000000000579E22ull, 0x8000000000579E22ull },
	{ 0x0000000020C35288ull, 0x0000000020C35288ull },
	{ 0x000000000D7B06FCull, 0x000000000D7B06FCull },
	{ 0x000006EDCBBD8F8Dull, 0x000006EDCBBD8F8Dull },
	{ 0x80000003C0F9998Aull, 0x80000003C0F9998Aull },
	{ 0x0000000003FBDE1Full, 0x0000000003FBDE1Full },
	{ 0x0000000035931F19ull, 0x0000000035931F19ull },
	{ 0x800000000A7B16B1ull, 0x800000000A7B16B1ull },
	{ 0x800000176D49849Bull, 0x800000176D49849Bull },
	{ 0x8000000002FF9413ull, 0x8000000002FF9413ull },
	{ 0x0000000029877510ull, 0x0000000029877510ull },
	{ 0x800000593C19C681ull, 0x800000593C19C681ull },
	{ 0x0000000BA432D160ull, 0x0000000BA432D160ull },
	{ 0x000000000000005Eull, 0x000000000000005Eull },
	{ 0x800000002C9604C2ull, 0x800000002C9604C2ull },
	{ 0x0000000000000158ull, 0x0000000000000158ull },
	{ 0x000000002F928345ull, 0x000000002F928345ull },
	{ 0x00000000000192D4ull, 0x00000000000192D4ull },
	{ 0x000000000000001Full, 0x000000000000001Full },
	{ 0x000000000000B51Aull, 0x000000000000B51Aull },
	{ 0x0000000027ACD56Dull, 0x0000000027ACD56Dull },
	{ 0x8000000000B7C3A3ull, 0x8000000000B7C3A3ull },
	{ 0x00000028AF0DFB68ull, 0x00000028AF0DFB68ull },
	{ 0x8000000000089380ull, 0x8000000000089380ull },
	{ 0x80000022BBA66FD7ull, 0x80000022BBA66FD7ull },
	{ 0x80000000003B4BC5ull, 0x80000000003B4BC5ull },
	{ 0x80000000000076DFull, 0x80000000000076DFull },
	{ 0x800000000000000Aull, 0x800000000000000Aull },
	{ 0x8000000000000015ull, 0x8000000000000015ull },
	{ 0x80000EF3B409BA0Full, 0x80000EF3B409BA0Full },
	{ 0x0000000019F30397ull, 0x0000000019F30397ull },
	{ 0x800000012A29B44Dull, 0x800000012A29B44Dull },
	{ 0x0000000000000159ull, 0x0000000000000159ull },
	{ 0x8000000000000EAAull, 0x8000000000000EAAull },
	{ 0x00000000076BFFD9ull, 0x00000000076BFFD9ull },
	{ 0x8000000000000062ull, 0x8000000000000062ull },
	{ 0x000000407DFE535Cull, 0x000000407DFE535Cull },
	{ 0x80000000000002A4ull, 0x80000000000002A4ull },
	{ 0x0000000000106500ull, 0x0000000000106500ull },
	{ 0x8000000000000F38ull, 0x8000000000000F38ull },
	{ 0x0000006FABD8835Dull, 0x0000006FABD8835Dull },
	{ 0x0005045FD0FC3CA6ull, 0x0005045FD0FC3CA6ull },
	{ 0x800000030B2F0389ull, 0x800000030B2F0389ull },
	{ 0x000000000000004Bull, 0x000000000000004Bull },
	{ 0x00000000004C11B4ull, 0x00000000004C11B4ull },
	{ 0x0000000C0A68DB9Full, 0x0000000C0A68DB9Full },
	{ 0x800000000001A8E5ull, 0x800000000001A8E5ull },
	{ 0x8000000000000007ull, 0x8000000000000007ull },
	{ 0x0000000000000023ull, 0x0000000000000023ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x8000654FB0ABA37Dull, 0x8000654FB0ABA37Dull },
	{ 0x80000A1D8A298CEBull, 0x80000A1D8A298CEBull },
	{ 0x00000088D4987A89ull, 0x00000088D4987A89ull },
	{ 0x8000000000FC54E5ull, 0x8000000000FC54E5ull },
	{ 0x00000000031488FDull, 0x00000000031488FDull },
	{ 0x8000000000000006ull, 0x8000000000000006ull },
	{ 0x000000000000069Bull, 0x000000000000069Bull },
	{ 0x80000000000000F8ull, 0x80000000000000F8ull },
	{ 0x80000098B0DE276Eull, 0x80000098B0DE276Eull },
	{ 0x0000000003BE0602ull, 0x0000000003BE0602ull },
	{ 0x800C6DBF44FBBF5Aull, 0x800C6DBF44FBBF5Aull },
	{ 0x0000000000000BFCull, 0x0000000000000BFCull },
	{ 0x00000000000003D7ull, 0x00000000000003D7ull },
	{ 0x8000000000000030ull, 0x8000000000000030ull },
	{ 0x00000000002C4362ull, 0x00000000002C4362ull },
	{ 0x80004057D72F0D8Aull, 0x80004057D72F0D8Aull },
	{ 0x800000000002F492ull, 0x800000000002F492ull },
	{ 0x000000000000000Full, 0x000000000000000Full },
	{ 0x000000000017D033ull, 0x000000000017D033ull },
	{ 0x0000C15FD8870031ull, 0x0000C15FD8870031ull },
	{ 0x00000000035C76D5ull, 0x00000000035C76D5ull },
	{ 0x000000004A76E3C5ull, 0x000000004A76E3C5ull },
	{ 0x0000000000000010ull, 0x0000000000000010ull },
	{ 0x800000000017901Full, 0x800000000017901Full },
	{ 0x80000000006DE34Dull, 0x80000000006DE34Dull },
	{ 0x8000000000049699ull, 0x8000000000049699ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x800000000000000Eull, 0x800000000000000Eull },
	{ 0x8000000000DFAF10ull, 0x8000000000DFAF10ull },
	{ 0x80001FBE6A4C09DEull, 0x80001FBE6A4C09DEull },
	{ 0x80072C11AA5CC2BBull, 0x80072C11AA5CC2BBull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x8000000389FA8B62ull, 0x8000000389FA8B62ull },
	{ 0x0000000000002C8Bull, 0x0000000000002C8Bull },
	{ 0x0000000000004D33ull, 0x0000000000004D33ull },
	{ 0x8000000000034163ull, 0x8000000000034163ull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x000000000000006Eull, 0x000000000000006Eull },
	{ 0x000005FA0DC8B067ull, 0x000005FA0DC8B067ull },
	{ 0x800000001049156Bull, 0x800000001049156Bull },
	{ 0x800000000000001Full, 0x800000000000001Full },
	{ 0x0000230D9DE5F18Dull, 0x0000230D9DE5F18Dull },
	{ 0x80004C4B3DEE354Full, 0x80004C4B3DEE354Full },
	{ 0x0000000006C4FE4Eull, 0x0000000006C4FE4Eull },
	{ 0x8000000000000035ull, 0x8000000000000035ull },
	{ 0x00000000000007EBull, 0x00000000000007EBull },
	{ 0x00000000058981B7ull, 0x00000000058981B7ull },
	{ 0x800000001C6DA295ull, 0x800000001C6DA295ull },
	{ 0x8000000000000087ull, 0x8000000000000087ull },
	{ 0x80000331F3770D89ull, 0x80000331F3770D89ull },
	{ 0x0000000000AB6466ull, 0x0000000000AB6466ull },
	{ 0x0000012CA7E9B30Eull, 0x0000012CA7E9B30Eull },
	{ 0x00000000000012DFull, 0x00000000000012DFull },
	{ 0x00000003B0B8FA17ull, 0x00000003B0B8FA17ull },
	{ 0x0000009239CD2051ull, 0x0000009239CD2051ull },
	{ 0x0000000000006F36ull, 0x0000000000006F36ull },
	{ 0x8000000BB0456D6Full, 0x8000000BB0456D6Full },
	{ 0x800000000000008Aull, 0x800000000000008Aull },
	{ 0x00003199D6082BBBull, 0x00003199D6082BBBull },
	{ 0x8000000000000002ull, 0x8000000000000002ull },
	{ 0x00000B06296D4B2Eull, 0x00000B06296D4B2Eull },
	{ 0x800000000000000Full, 0x800000000000000Full },
	{ 0x000000000023C894ull, 0x000000000023C894ull },
	{ 0x0000059AD4360D4Aull, 0x0000059AD4360D4Aull },
	{ 0x0000000000000024ull, 0x0000000000000024ull },
	{ 0x8000000000000118ull, 0x8000000000000118ull },
	{ 0x80000000000000BEull, 0x80000000000000BEull },
	{ 0x0000000000791681ull, 0x0000000000791681ull },
	{ 0x8000A6D6A54DCD09ull, 0x8000A6D6A54DCD09ull },
	{ 0x0000000019455383ull, 0x0000000019455383ull },
	{ 0x00000000000000DCull, 0x00000000000000DCull },
	{ 0x00000000000325A3ull, 0x00000000000325A3ull },
	{ 0x80000084936B30CAull, 0x80000084936B30CAull },
	{ 0x800000000000506Dull, 0x800000000000506Dull },
	{ 0x800000000E90B6F5ull, 0x800000000E90B6F5ull },
	{ 0x80000002A8993415ull, 0x80000002A8993415ull },
	{ 0x80000000000000B0ull, 0x80000000000000B0ull },
	{ 0x000001557BEEE5EAull, 0x000001557BEEE5EAull },
	{ 0x80000007650BF89Full, 0x80000007650BF89Full },
	{ 0x000000000A244E5Eull, 0x000000000A244E5Eull },
	{ 0x800000000083415Eull, 0x800000000083415Eull },
	{ 0x000000004F24AE16ull, 0x000000004F24AE16ull },
	{ 0x800000000003A5C0ull, 0x800000000003A5C0ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x80003B11837079B2ull, 0x80003B11837079B2ull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x800003612401C73Dull, 0x800003612401C73Dull },
	{ 0x00000000000018D7ull, 0x00000000000018D7ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x000000000018C683ull, 0x000000000018C683ull },
	{ 0x0000001AFD20126Bull, 0x0000001AFD20126Bull },
	{ 0x800000004144FB6Aull, 0x800000004144FB6Aull },
	{ 0x8000073B5B1780E4ull, 0x8000073B5B1780E4ull },
	{ 0x8000017BF5FB3DACull, 0x8000017BF5FB3DACull },
	{ 0x000000000015DC5Bull, 0x000000000015DC5Bull },
	{ 0x800002861BCB1870ull, 0x800002861BCB1870ull },
	{ 0x800006586B0073E4ull, 0x800006586B0073E4ull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x800000011DFA10E2ull, 0x800000011DFA10E2ull },
	{ 0x800410905D9F7097ull, 0x800410905D9F7097ull },
	{ 0x8000000C9BEDFA6Aull, 0x8000000C9BEDFA6Aull },
	{ 0x000000000000586Full, 0x000000000000586Full },
	{ 0x8000000000010989ull, 0x8000000000010989ull },
	{ 0x00000000006CF54Bull, 0x00000000006CF54Bull },
	{ 0x0000000000000A98ull, 0x0000000000000A98ull },
	{ 0x00000000000E4A03ull, 0x00000000000E4A03ull },
	{ 0x000000001FA79175ull, 0x000000001FA79175ull },
	{ 0x00000DAB8CAD5F0Dull, 0x00000DAB8CAD5F0Dull },
	{ 0x0000814126FE64E2ull, 0x0000814126FE64E2ull },
	{ 0x80000000000000E9ull, 0x80000000000000E9ull },
	{ 0x0000000004F1D588ull, 0x0000000004F1D588ull },
	{ 0x000000000006FCA9ull, 0x000000000006FCA9ull },
	{ 0x800000000374B69Full, 0x800000000374B69Full },
	{ 0x800000010C72CB9Cull, 0x800000010C72CB9Cull },
	{ 0x80000F40C5DC3102ull, 0x80000F40C5DC3102ull },
	{ 0x0000149DA4351BF4ull, 0x0000149DA4351BF4ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x0000000000000035ull, 0x0000000000000035ull },
	{ 0x800000000000ABBBull, 0x800000000000ABBBull },
	{ 0x8000000000000086ull, 0x8000000000000086ull },
	{ 0x80000001EDEF613Bull, 0x80000001EDEF613Bull },
	{ 0x800000000000000Bull, 0x800000000000000Bull },
	{ 0x00000000835B6DD4ull, 0x00000000835B6DD4ull },
	{ 0x80000000090E6C3Aull, 0x80000000090E6C3Aull },
	{ 0x000036056B8761A6ull, 0x000036056B8761A6ull },
	{ 0x0000000000000038ull, 0x0000000000000038ull },
	{ 0x8000000000000010ull, 0x8000000000000010ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x000000000005EB96ull, 0x000000000005EB96ull },
	{ 0x0000000D8132C2BCull, 0x0000000D8132C2BCull },
	{ 0x8000000000001F30ull, 0x8000000000001F30ull },
	{ 0x0000000480D5E06Full, 0x0000000480D5E06Full },
	{ 0x8000004B6706F987ull, 0x8000004B6706F987ull },
	{ 0x8000000000007F1Cull, 0x8000000000007F1Cull },
	{ 0x0000000000000F90ull, 0x0000000000000F90ull },
	{ 0x8000000003272044ull, 0x8000000003272044ull },
	{ 0x800005BE1F96CEC1ull, 0x800005BE1F96CEC1ull },
	{ 0x00000000002AAE57ull, 0x00000000002AAE57ull },
	{ 0x0000000019A159B2ull, 0x0000000019A159B2ull },
	{ 0x80000000000E3BAFull, 0x80000000000E3BAFull },
	{ 0x800000000000015Eull, 0x800000000000015Eull },
	{ 0x00000000774BB2A8ull, 0x00000000774BB2A8ull },
	{ 0x80000000051A7553ull, 0x80000000051A7553ull },
	{ 0x000000003E967186ull, 0x000000003E967186ull },
	{ 0x000000000085E503ull, 0x000000000085E503ull },
	{ 0x80010E57CAD263ADull, 0x80010E57CAD263ADull },
	{ 0x80001C70A859A002ull, 0x80001C70A859A002ull },
	{ 0x000648CCF4258529ull, 0x000648CCF4258529ull },
	{ 0x00000000000000D4ull, 0x00000000000000D4ull },
	{ 0x000D86A9C500FA89ull, 0x000D86A9C500FA89ull },
	{ 0x00000000000002C8ull, 0x00000000000002C8ull },
	{ 0x80000000000000EAull, 0x80000000000000EAull },
	{ 0x0001D0427C812973ull, 0x0001D0427C812973ull },
	{ 0x00000000000124DBull, 0x00000000000124DBull },
	{ 0x80000013B42FE7F5ull, 0x80000013B42FE7F5ull },
	{ 0x8000000000000035ull, 0x8000000000000035ull },
	{ 0x80001058BC861CCAull, 0x80001058BC861CCAull },
	{ 0x0000000000000007ull, 0x0000000000000007ull },
	{ 0x000000000003CD91ull, 0x000000000003CD91ull },
	{ 0x8000000000E6FFE7ull, 0x8000000000E6FFE7ull },
	{ 0x8000190F24E0F9E3ull, 0x8000190F24E0F9E3ull },
	{ 0x800000016A421AFFull, 0x800000016A421AFFull },
	{ 0x80001103D3B08F50ull, 0x80001103D3B08F50ull },
	{ 0x8000000002F1AC62ull, 0x8000000002F1AC62ull },
	{ 0x800000000DA9351Dull, 0x800000000DA9351Dull },
	{ 0x8000000147B88512ull, 0x8000000147B88512ull },
	{ 0x800001082B630B96ull, 0x800001082B630B96ull },
	{ 0x80000019F296B0D8ull, 0x80000019F296B0D8ull },
	{ 0x000000000003DF3Aull, 0x000000000003DF3Aull },
	{ 0x80000000000000C8ull, 0x80000000000000C8ull },
	{ 0x00000000001DF434ull, 0x00000000001DF434ull },
	{ 0x8000000000031A1Dull, 0x8000000000031A1Dull },
	{ 0x80000E94DACAC756ull, 0x80000E94DACAC756ull },
	{ 0x8001DC14FED5799Eull, 0x8001DC14FED5799Eull },
	{ 0x80005046EC1A3ECAull, 0x80005046EC1A3ECAull },
	{ 0x8000000000000005ull, 0x8000000000000005ull },
	{ 0x800000009144AE80ull, 0x800000009144AE80ull },
	{ 0x800000000000000Full, 0x800000000000000Full },
	{ 0x800000000001749Bull, 0x800000000001749Bull },
	{ 0x0000000000037285ull, 0x0000000000037285ull },
	{ 0x8000000000002158ull, 0x8000000000002158ull },
	{ 0x00000000000014C1ull, 0x00000000000014C1ull },
	{ 0x80000000240316B5ull, 0x80000000240316B5ull },
	{ 0x000000002C6CE47Cull, 0x000000002C6CE47Cull },
	{ 0x80000000000AE0FEull, 0x80000000000AE0FEull },
	{ 0x00001E7133206041ull, 0x00001E7133206041ull },
	{ 0x80000000000030E5ull, 0x80000000000030E5ull },
	{ 0x8000001B429103D8ull, 0x8000001B429103D8ull },
	{ 0x0000000000011FF9ull, 0x0000000000011FF9ull },
	{ 0x800000000067E6DFull, 0x800000000067E6DFull },
	{ 0x8000000000013F78ull, 0x8000000000013F78ull },
	{ 0x80000345CDD45839ull, 0x80000345CDD45839ull },
	{ 0x8000027EF9895E69ull, 0x8000027EF9895E69ull },
	{ 0x0000000000000004ull, 0x0000000000000004ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x000000000000066Eull, 0x000000000000066Eull },
	{ 0x8000000000000028ull, 0x8000000000000028ull },
	{ 0x80008ABBD7332E54ull, 0x80008ABBD7332E54ull },
	{ 0x000000000000001Cull, 0x000000000000001Cull },
	{ 0x8000000000000009ull, 0x8000000000000009ull },
	{ 0x800000000018D4A6ull, 0x800000000018D4A6ull },
	{ 0x0000000000000B2Cull, 0x0000000000000B2Cull },
	{ 0x800000AE51EF5156ull, 0x800000AE51EF5156ull },
	{ 0x800079E3A32C5405ull, 0x800079E3A32C5405ull },
	{ 0x8000000000000019ull, 0x8000000000000019ull },
	{ 0x80000000000EAC6Dull, 0x80000000000EAC6Dull },
	{ 0x80000000000001AEull, 0x80000000000001AEull },
	{ 0x8000045154194958ull, 0x8000045154194958ull },
	{ 0x8000001D0957FC01ull, 0x8000001D0957FC01ull },
	{ 0x8000000000000292ull, 0x8000000000000292ull },
	{ 0x800000000000EE3Aull, 0x800000000000EE3Aull },
	{ 0x00001A0F4516B6AFull, 0x00001A0F4516B6AFull },
	{ 0x00000000000542C7ull, 0x00000000000542C7ull },
	{ 0x00000000000001E6ull, 0x00000000000001E6ull },
	{ 0x0000000962EE499Dull, 0x0000000962EE499Dull },
	{ 0x8000000156AF7961ull, 0x8000000156AF7961ull },
	{ 0x0000000000C3143Dull, 0x0000000000C3143Dull },
	{ 0x8000676E7CDA772Bull, 0x8000676E7CDA772Bull },
	{ 0x00000000002605C9ull, 0x00000000002605C9ull },
	{ 0x80000000B8932E6Dull, 0x80000000B8932E6Dull },
	{ 0x00000000000E0112ull, 0x00000000000E0112ull },
	{ 0x00000003E622E304ull, 0x00000003E622E304ull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x800000000000A6F3ull, 0x800000000000A6F3ull },
	{ 0x80000324D9EF77ECull, 0x80000324D9EF77ECull },
	{ 0x800000000007A86Bull, 0x800000000007A86Bull },
	{ 0x000000000440B2B9ull, 0x000000000440B2B9ull },
	{ 0x00000000A79BB479ull, 0x00000000A79BB479ull },
	{ 0x800000000000002Bull, 0x800000000000002Bull },
	{ 0x000006C0CCB1A6C9ull, 0x000006C0CCB1A6C9ull },
	{ 0x000000000000F840ull, 0x000000000000F840ull },
	{ 0x8000D55B81F3C708ull, 0x8000D55B81F3C708ull },
	{ 0x0000000000025385ull, 0x0000000000025385ull },
	{ 0x80000000092FC8DFull, 0x80000000092FC8DFull },
	{ 0x800000000309A5BBull, 0x800000000309A5BBull },
	{ 0x80004FFB5657BC3Bull, 0x80004FFB5657BC3Bull },
	{ 0x80000FCD6426D0BEull, 0x80000FCD6426D0BEull },
	{ 0x000000000003570Bull, 0x000000000003570Bull },
	{ 0x000106BE0F5BFB84ull, 0x000106BE0F5BFB84ull },
	{ 0x8000000000000006ull, 0x8000000000000006ull },
	{ 0x8000000001D07FAEull, 0x8000000001D07FAEull },
	{ 0x00000000EAC16053ull, 0x00000000EAC16053ull },
	{ 0x8000000000000399ull, 0x8000000000000399ull },
	{ 0x00000000000FA019ull, 0x00000000000FA019ull },
	{ 0x00000B8E62D17FDAull, 0x00000B8E62D17FDAull },
	{ 0x000000000001A77Bull, 0x000000000001A77Bull },
	{ 0x0000000000000616ull, 0x0000000000000616ull },
	{ 0x8000E305C3353C61ull, 0x8000E305C3353C61ull },
	{ 0x80000056C2F4EE24ull, 0x80000056C2F4EE24ull },
	{ 0x0000000000000014ull, 0x0000000000000014ull },
	{ 0x000000000001DD84ull, 0x000000000001DD84ull },
	{ 0x000000000000A30Eull, 0x000000000000A30Eull },
	{ 0x000000537E5E0778ull, 0x000000537E5E0778ull },
	{ 0x000000000000E3F0ull, 0x000000000000E3F0ull },
	{ 0x8000033E4BF20230ull, 0x8000033E4BF20230ull },
	{ 0x000001CD11EF7CEEull, 0x000001CD11EF7CEEull },
	{ 0x8000001EDC551EC6ull, 0x8000001EDC551EC6ull },
	{ 0x0000000023BBAA03ull, 0x0000000023BBAA03ull },
	{ 0x00000000B12C14CDull, 0x00000000B12C14CDull },
	{ 0x800000000222B64Cull, 0x800000000222B64Cull },
	{ 0x8000000000046DB9ull, 0x8000000000046DB9ull },
	{ 0x0001302C36E1C36Dull, 0x0001302C36E1C36Dull },
	{ 0x000302DDBEEE8C4Bull, 0x000302DDBEEE8C4Bull },
	{ 0x0000013BBD14BBFFull, 0x0000013BBD14BBFFull },
	{ 0x80000000000000FEull, 0x80000000000000FEull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x00000002F5406B45ull, 0x00000002F5406B45ull },
	{ 0x0000001FC1C8A812ull, 0x0000001FC1C8A812ull },
	{ 0x000001BDA28B5608ull, 0x000001BDA28B5608ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x8000042E69E98BD0ull, 0x8000042E69E98BD0ull },
	{ 0x0007E353BFE4FF28ull, 0x0007E353BFE4FF28ull },
	{ 0x000004D3F9028CC2ull, 0x000004D3F9028CC2ull },
	{ 0x8000000000001011ull, 0x8000000000001011ull },
	{ 0x000C368DF77CD3A7ull, 0x000C368DF77CD3A7ull },
	{ 0x0000004E28808AF7ull, 0x0000004E28808AF7ull },
	{ 0x800241DA3E1F096Aull, 0x800241DA3E1F096Aull },
	{ 0x0000000167F06369ull, 0x0000000167F06369ull },
	{ 0x80000000000025D0ull, 0x80000000000025D0ull },
	{ 0x8000000000525EEEull, 0x8000000000525EEEull },
	{ 0x0000000007CAFCEEull, 0x0000000007CAFCEEull },
	{ 0x8000000000000375ull, 0x8000000000000375ull },
	{ 0x800034E69474D67Bull, 0x800034E69474D67Bull },
	{ 0x80000000011AC07Bull, 0x80000000011AC07Bull },
	{ 0x80003037797A85D8ull, 0x80003037797A85D8ull },
	{ 0x00000B9DE0DEFA8Dull, 0x00000B9DE0DEFA8Dull },
	{ 0x0000001FA78EF48Aull, 0x0000001FA78EF48Aull },
	{ 0x800000000000006Bull, 0x800000000000006Bull },
	{ 0x8000051EE172E8FAull, 0x8000051EE172E8FAull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x000002ADBCED74F5ull, 0x000002ADBCED74F5ull },
	{ 0x80000000001D6D58ull, 0x80000000001D6D58ull },
	{ 0x8005B1891DAF0FC4ull, 0x8005B1891DAF0FC4ull },
	{ 0x800000000000003Full, 0x800000000000003Full },
	{ 0x00000039FD64974Full, 0x00000039FD64974Full },
	{ 0x00000000000408E4ull, 0x00000000000408E4ull },
	{ 0x8000000000023E51ull, 0x8000000000023E51ull },
	{ 0x800000000357DA89ull, 0x800000000357DA89ull },
	{ 0x800000000000023Cull, 0x800000000000023Cull },
	{ 0x0001EF390B66E1B4ull, 0x0001EF390B66E1B4ull },
	{ 0x800000000000037Aull, 0x800000000000037Aull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x800057CBF24A409Dull, 0x800057CBF24A409Dull },
	{ 0x0000000000005CAAull, 0x0000000000005CAAull },
	{ 0x0000000000017B4Aull, 0x0000000000017B4Aull },
	{ 0x0000000000000191ull, 0x0000000000000191ull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x00000026CA64A501ull, 0x00000026CA64A501ull },
	{ 0x8001A0CC220AE5AEull, 0x8001A0CC220AE5AEull },
	{ 0x80000008F64E805Full, 0x80000008F64E805Full },
	{ 0x800000000D453ED3ull, 0x800000000D453ED3ull },
	{ 0x80000000000F1F90ull, 0x80000000000F1F90ull },
	{ 0x0000000000006544ull, 0x0000000000006544ull },
	{ 0x000000001D7205CEull, 0x000000001D7205CEull },
	{ 0x000000529B17E4F4ull, 0x000000529B17E4F4ull },
	{ 0x00000CE78028CA74ull, 0x00000CE78028CA74ull },
	{ 0x000000000000934Dull, 0x000000000000934Dull },
	{ 0x000000000743C6CFull, 0x000000000743C6CFull },
	{ 0x00000000003F96CEull, 0x00000000003F96CEull },
	{ 0x00000000DF9202E0ull, 0x00000000DF9202E0ull },
	{ 0x800000000000000Eull, 0x800000000000000Eull },
	{ 0x80000002946BB8D3ull, 0x80000002946BB8D3ull },
	{ 0x000000000000020Bull, 0x000000000000020Bull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x8000000000000309ull, 0x8000000000000309ull },
	{ 0x000000000000107Aull, 0x000000000000107Aull },
	{ 0x800000000FDDC6D0ull, 0x800000000FDDC6D0ull },
	{ 0x80003031F23BF5F9ull, 0x80003031F23BF5F9ull },
	{ 0x8006A0666340F4FDull, 0x8006A0666340F4FDull },
	{ 0x8000000000000006ull, 0x8000000000000006ull },
	{ 0x00000000000017EAull, 0x00000000000017EAull },
	{ 0x80000000B7F6F17Bull, 0x80000000B7F6F17Bull },
	{ 0x0000000000936334ull, 0x0000000000936334ull },
	{ 0x8000000000CCA3B0ull, 0x8000000000CCA3B0ull },
	{ 0x0000000000196126ull, 0x0000000000196126ull },
	{ 0x0000000000000C37ull, 0x0000000000000C37ull },
	{ 0x8000000000011D36ull, 0x8000000000011D36ull },
	{ 0x0000197089916ECFull, 0x0000197089916ECFull },
	{ 0x800003D4F97281B4ull, 0x800003D4F97281B4ull },
	{ 0x800000851701BC22ull, 0x800000851701BC22ull },
	{ 0x00000000000541F2ull, 0x00000000000541F2ull },
	{ 0x000000000D6149F7ull, 0x000000000D6149F7ull },
	{ 0x0000000000861048ull, 0x0000000000861048ull },
	{ 0x800000000003D92Cull, 0x800000000003D92Cull },
	{ 0x8000000000E551FCull, 0x8000000000E551FCull },
	{ 0x800000000000001Dull, 0x800000000000001Dull },
	{ 0x80000000EF8891FCull, 0x80000000EF8891FCull },
	{ 0x8001F28EBDFF9B2Bull, 0x8001F28EBDFF9B2Bull },
	{ 0x8000000CD58152F9ull, 0x8000000CD58152F9ull },
	{ 0x0000000005B5D5C2ull, 0x0000000005B5D5C2ull },
	{ 0x00000000EE4700E0ull, 0x00000000EE4700E0ull },
	{ 0x000A2602990B6A4Dull, 0x000A2602990B6A4Dull },
	{ 0x80000016C97D6B61ull, 0x80000016C97D6B61ull },
	{ 0x8000000551E16772ull, 0x8000000551E16772ull },
	{ 0x80000000000A3457ull, 0x80000000000A3457ull },
	{ 0x00000000005D5B73ull, 0x00000000005D5B73ull },
	{ 0x8000000000000018ull, 0x8000000000000018ull },
	{ 0x80000068E393F01Aull, 0x80000068E393F01Aull },
	{ 0x8000000000000018ull, 0x8000000000000018ull },
	{ 0x00000000000B0134ull, 0x00000000000B0134ull },
	{ 0x80001689758BFBC0ull, 0x80001689758BFBC0ull },
	{ 0x0000000000A95324ull, 0x0000000000A95324ull },
	{ 0x00000082B836577Aull, 0x00000082B836577Aull },
	{ 0x000000026666B41Eull, 0x000000026666B41Eull },
	{ 0x0001A4D1F82D444Aull, 0x0001A4D1F82D444Aull },
	{ 0x80000000005354EAull, 0x80000000005354EAull },
	{ 0x00000C898CC4A78Cull, 0x00000C898CC4A78Cull },
	{ 0x000007ECB220E817ull, 0x000007ECB220E817ull },
	{ 0x0000005A58B4ADC7ull, 0x0000005A58B4ADC7ull },
	{ 0x8000000006AB3D47ull, 0x8000000006AB3D47ull },
	{ 0x00000067D721AEBDull, 0x00000067D721AEBDull },
	{ 0x800000F7EBAF3D82ull, 0x800000F7EBAF3D82ull },
	{ 0x0000002CCF18396Dull, 0x0000002CCF18396Dull },
	{ 0x800000000000BBBFull, 0x800000000000BBBFull },
	{ 0x00000000007FF6EDull, 0x00000000007FF6EDull },
	{ 0x000000000000557Dull, 0x000000000000557Dull },
	{ 0x00000000001D9DCFull, 0x00000000001D9DCFull },
	{ 0x000000000B18A7D7ull, 0x000000000B18A7D7ull },
	{ 0x80000704B8CDD3F7ull, 0x80000704B8CDD3F7ull },
	{ 0x0000000001D7AA1Full, 0x0000000001D7AA1Full },
	{ 0x80000000000005C0ull, 0x80000000000005C0ull },
	{ 0x80000000379557C6ull, 0x80000000379557C6ull },
	{ 0x800000000000EDB8ull, 0x800000000000EDB8ull },
	{ 0x800197B290E65E20ull, 0x800197B290E65E20ull },
	{ 0x80000000000D05E7ull, 0x80000000000D05E7ull },
	{ 0x800000D3F78E53BBull, 0x800000D3F78E53BBull },
	{ 0x8000000000000009ull, 0x8000000000000009ull },
	{ 0x0000000000000127ull, 0x0000000000000127ull },
	{ 0x800000000001F04Cull, 0x800000000001F04Cull },
	{ 0x80000004FA6EF5AEull, 0x80000004FA6EF5AEull },
	{ 0x80000901F94213AFull, 0x80000901F94213AFull },
	{ 0x0000000029B335CBull, 0x0000000029B335CBull },
	{ 0x000000028BD31EEFull, 0x000000028BD31EEFull },
	{ 0x000000000004DE27ull, 0x000000000004DE27ull },
	{ 0x000000033FF85FBFull, 0x000000033FF85FBFull },
	{ 0x00000000041B55C4ull, 0x00000000041B55C4ull },
	{ 0x0000000000003EFAull, 0x0000000000003EFAull },
	{ 0x00000000000001E6ull, 0x00000000000001E6ull },
	{ 0x8000000000518E09ull, 0x8000000000518E09ull },
	{ 0x800000000054D3C3ull, 0x800000000054D3C3ull },
	{ 0x0000000000000011ull, 0x0000000000000011ull },
	{ 0x00000000000000E7ull, 0x00000000000000E7ull },
	{ 0x000020D710A0806Eull, 0x000020D710A0806Eull },
	{ 0x00000C4EB6A40C1Cull, 0x00000C4EB6A40C1Cull },
	{ 0x0000000000000014ull, 0x0000000000000014ull },
	{ 0x0000000000000092ull, 0x0000000000000092ull },
	{ 0x8000000000000052ull, 0x8000000000000052ull },
	{ 0x0000000000000016ull, 0x0000000000000016ull },
	{ 0x000000000000003Cull, 0x000000000000003Cull },
	{ 0x800000069D0E3FBCull, 0x800000069D0E3FBCull },
	{ 0x0000000000000015ull, 0x0000000000000015ull },
	{ 0x00000FFE12E7F57Cull, 0x00000FFE12E7F57Cull },
	{ 0x0000000003AEFF67ull, 0x0000000003AEFF67ull },
	{ 0x8000000000007217ull, 0x8000000000007217ull },
	{ 0x000006313E5F652Eull, 0x000006313E5F652Eull },
	{ 0x8000000000000056ull, 0x8000000000000056ull },
	{ 0x00000165D0E2E2F2ull, 0x00000165D0E2E2F2ull },
	{ 0x0000000000000046ull, 0x0000000000000046ull },
	{ 0x000000000264122Dull, 0x000000000264122Dull },
	{ 0x80000001560C0040ull, 0x80000001560C0040ull },
	{ 0x8000000000000059ull, 0x8000000000000059ull },
	{ 0x8000000000000059ull, 0x8000000000000059ull },
	{ 0x80000000000024ECull, 0x80000000000024ECull },
	{ 0x00007EF9BDE4DF24ull, 0x00007EF9BDE4DF24ull },
	{ 0x80000000E5928BBEull, 0x80000000E5928BBEull },
	{ 0xAB1370A14CC3EDB7ull, 0xAB1370A14CC3EDB7ull },
	{ 0xCF57B5752B4AB460ull, 0xCF57B5752B4AB460ull },
	{ 0x70CAF04CE31AAD99ull, 0x70CAF04CE31AAD99ull },
	{ 0x19000E28036FD666ull, 0x19000E28036FD666ull },
	{ 0xA21EB927EB85C918ull, 0xA21EB927EB85C918ull },
	{ 0x9F06E3F686C75D14ull, 0x9F06E3F686C75D14ull },
	{ 0xB9159DC5566C6365ull, 0xB9159DC5566C6365ull },
	{ 0x4026AD9FCC238404ull, 0x4026AD9FCC238404ull },
	{ 0xAF66759BF7463FE6ull, 0xAF66759BF7463FE6ull },
	{ 0x666F9E847E350946ull, 0x666F9E847E350946ull },
	{ 0x55980C06E89985B4ull, 0x55980C06E89985B4ull },
	{ 0x76115EA74448D174ull, 0x76115EA74448D174ull },
	{ 0x062DFE74F4AD7AF9ull, 0x062DFE74F4AD7AF9ull },
	{ 0x63AB6D67E84AA8B7ull, 0x63AB6D67E84AA8B7ull },
	{ 0x41F031E91F465246ull, 0x41F031E91F465246ull },
	{ 0x7BFD32839D8C30E1ull, 0x7BFD32839D8C30E1ull },
	{ 0x96FCA672150C2C06ull, 0x96FCA672150C2C06ull },
	{ 0xF6122FC6F3864CF4ull, 0xF6122FC6F3864CF4ull },
	{ 0x697E8432743E4761ull, 0x697E8432743E4761ull },
	{ 0xD0D29B8B82DE2B5Dull, 0xD0D29B8B82DE2B5Dull },
	{ 0x1219E80E6F963200ull, 0x1219E80E6F963200ull },
	{ 0x245B151470E7AC5Aull, 0x245B151470E7AC5Aull },
	{ 0x6E86CF76F7290E9Bull, 0x6E86CF76F7290E9Bull },
	{ 0xBA141401CFF12181ull, 0xBA141401CFF12181ull },
	{ 0x79D1880717B74F02ull, 0x79D1880717B74F02ull },
	{ 0xA4AB5857FED70FBAull, 0xA4AB5857FED70FBAull },
	{ 0xE44D20955B047896ull, 0xE44D20955B047896ull },
	{ 0xD7FF5ADB0A9FEBC1ull, 0xD7FF5ADB0A9FEBC1ull },
	{ 0x2B67D7792836E24Full, 0x2B67D7792836E24Full },
	{ 0xC13CBC5E0C5FDE27ull, 0xC13CBC5E0C5FDE27ull },
	{ 0x31E18C4422BF74B6ull, 0x31E18C4422BF74B6ull },
	{ 0xDEBDB6A68A648051ull, 0xDEBDB6A68A648051ull },
	{ 0xD7E42EEF8B896B69ull, 0xD7E42EEF8B896B69ull },
	{ 0xD81B8BE3D8289EF5ull, 0xD81B8BE3D8289EF5ull },
	{ 0x92AE2FBDA0B2121Cull, 0x92AE2FBDA0B2121Cull },
	{ 0x1651F6BA75CFF7A2ull, 0x1651F6BA75CFF7A2ull },
	{ 0x849EEF6EFC888915ull, 0x849EEF6EFC888915ull },
	{ 0xDB949A0370EAF9B7ull, 0xDB949A0370EAF9B7ull },
	{ 0x15952F8BF79580ADull, 0x15952F8BF79580ADull },
	{ 0xDB3671BFC3828869ull, 0xDB3671BFC3828869ull },
	{ 0x0A9F81E206AAFC54ull, 0x0A9F81E206AAFC54ull },
	{ 0xD1E3419FB56BB6BFull, 0xD1E3419FB56BB6BFull },
	{ 0x1DFEB8D51AFC0F8Cull, 0x1DFEB8D51AFC0F8Cull },
	{ 0x9D7B067E3A638A97ull, 0x9D7B067E3A638A97ull },
	{ 0x172A572503BCA2EAull, 0x172A572503BCA2EAull },
	{ 0x551F65505472BC21ull, 0x551F65505472BC21ull },
	{ 0xE2F70046CC892E9Aull, 0xE2F70046CC892E9Aull },
	{ 0xE1331BE8711975E1ull, 0xE1331BE8711975E1ull },
	{ 0xAB7EE8121E967DDAull, 0xAB7EE8121E967DDAull },
	{ 0x137B702DCC431230ull, 0x137B702DCC431230ull },
	{ 0xEFDC42C29FE96CFCull, 0xEFDC42C29FE96CFCull },
	{ 0x3EB3A0E7CE81E0B9ull, 0x3EB3A0E7CE81E0B9ull },
	{ 0x1CA5B56D5296AB67ull, 0x1CA5B56D5296AB67ull },
	{ 0xCF22551D6C92E0FEull, 0xCF22551D6C92E0FEull },
	{ 0xA3ACC549F70A7A15ull, 0xA3ACC549F70A7A15ull },
	{ 0x62210C4889298452ull, 0x62210C4889298452ull },
	{ 0x13FE48506A186B94ull, 0x13FE48506A186B94ull },
	{ 0x5E658EDD53836EDEull, 0x5E658EDD53836EDEull },
	{ 0xB1B4D4AA05532586ull, 0xB1B4D4AA05532586ull },
	{ 0x0BBD5D05A804361Full, 0x0BBD5D05A804361Full },
	{ 0x8E8D3176FFEC1EA3ull, 0x8E8D3176FFEC1EA3ull },
	{ 0x2B5B20506FD923E4ull, 0x2B5B20506FD923E4ull },
	{ 0xA12CC5C7CD5B5F34ull, 0xA12CC5C7CD5B5F34ull },
	{ 0x8F961A1501A4EFD3ull, 0x8F961A1501A4EFD3ull },
	{ 0xDD9A09AA7E8F4319ull, 0xDD9A09AA7E8F4319ull },
	{ 0xB42B946A80B38763ull, 0xB42B946A80B38763ull },
	{ 0x4D26F56C21A436C2ull, 0x4D26F56C21A436C2ull },
	{ 0xD5E252133D35C761ull, 0xD5E252133D35C761ull },
	{ 0x264D71EE216478B7ull, 0x264D71EE216478B7ull },
	{ 0x0B1FFE1C144540EDull, 0x0B1FFE1C144540EDull },
	{ 0x64DDC88FA54FDA91ull, 0x64DDC88FA54FDA91ull },
	{ 0x5AF09C0FD0AFF4FCull, 0x5AF09C0FD0AFF4FCull },
	{ 0xE1DF132AF2AB7E81ull, 0xE1DF132AF2AB7E81ull },
	{ 0x563FF81E898C17D6ull, 0x563FF81E898C17D6ull },
	{ 0x6B81AC06CAD99AC6ull, 0x6B81AC06CAD99AC6ull },
	{ 0xAED516D90353CF98ull, 0xAED516D90353CF98ull },
	{ 0xDD739E3BCCF93539ull, 0xDD739E3BCCF93539ull },
	{ 0x4FEFA5600F88F3B1ull, 0x4FEFA5600F88F3B1ull },
	{ 0x385714CDD4CDE744ull, 0x385714CDD4CDE744ull },
	{ 0x75C76947EC20CF66ull, 0x75C76947EC20CF66ull },
	{ 0x3A4ACBD95BAF9BE4ull, 0x3A4ACBD95BAF9BE4ull },
	{ 0xC62DB0CFA4B7DCD0ull, 0xC62DB0CFA4B7DCD0ull },
	{ 0x31F72BAE64DF89D9ull, 0x31F72BAE64DF89D9ull },
	{ 0x5988F57E505E7FEEull, 0x5988F57E505E7FEEull },
	{ 0xEF40A1D91BA11DBBull, 0xEF40A1D91BA11DBBull },
	{ 0x3C8D7797297CC714ull, 0x3C8D7797297CC714ull },
	{ 0xE43DC2E5FCECA440ull, 0xE43DC2E5FCECA440ull },
	{ 0xF6CF5F10FF14F372ull, 0xF6CF5F10FF14F372ull },
	{ 0xC9F9A771D7990384ull, 0xC9F9A771D7990384ull },
	{ 0x54AA514F5159C775ull, 0x54AA514F5159C775ull },
	{ 0xCE36ADA1D3D30C25ull, 0xCE36ADA1D3D30C25ull },
	{ 0xD2CAD244686D504Eull, 0xD2CAD244686D504Eull },
	{ 0xCAA82B29527A3846ull, 0xCAA82B29527A3846ull },
	{ 0xD62CF47935B1A573ull, 0xD62CF47935B1A573ull },
	{ 0xFD661B73CDA56B8Eull, 0xFD661B73CDA56B8Eull },
	{ 0x63F5089933C1A79Bull, 0x63F5089933C1A79Bull },
	{ 0x40BAD092B7CFB4A7ull, 0x40BAD092B7CFB4A7ull },
	{ 0x59924E024611C244ull, 0x59924E024611C244ull },
	{ 0x432AA34F5DED0BC2ull, 0x432AA34F5DED0BC2ull },
	{ 0xA4BBA3189938AB2Full, 0xA4BBA3189938AB2Full },
	{ 0xBF95A2C0C3E71290ull, 0xBF95A2C0C3E71290ull },
	{ 0x7B5633DE826020E0ull, 0x7B5633DE826020E0ull },
	{ 0xB50344C58841D648ull, 0xB50344C58841D648ull },
	{ 0x13862B50605DAC20ull, 0x13862B50605DAC20ull },
	{ 0x1EC81561C2288BA6ull, 0x1EC81561C2288BA6ull },
	{ 0xDBD962E47A73617Dull, 0xDBD962E47A73617Dull },
	{ 0xAC656740E1225468ull, 0xAC656740E1225468ull },
	{ 0x32DC12AEE0F89CC7ull, 0x32DC12AEE0F89CC7ull },
	{ 0xF9B303BB8E2EF951ull, 0xF9B303BB8E2EF951ull },
	{ 0x9F8780C94DAAC592ull, 0x9F8780C94DAAC592ull },
	{ 0xC6B2DA218B4BAFA8ull, 0xC6B2DA218B4BAFA8ull },
	{ 0x091AB27F02A60DB9ull, 0x091AB27F02A60DB9ull },
	{ 0xAFCEB10793DFD537ull, 0xAFCEB10793DFD537ull },
	{ 0xEEED1AFCABC62FD8ull, 0xEEED1AFCABC62FD8ull },
	{ 0x656FD4D93B51B1CFull, 0x656FD4D93B51B1CFull },
	{ 0x6E3C080992EA7A89ull, 0x6E3C080992EA7A89ull },
	{ 0xF7100591A2D6F547ull, 0xF7100591A2D6F547ull },
	{ 0x1CED749B4B8E4E22ull, 0x1CED749B4B8E4E22ull },
	{ 0x941EA15FA110E5C7ull, 0x941EA15FA110E5C7ull },
	{ 0xC4A341597C46FC22ull, 0xC4A341597C46FC22ull },
	{ 0xFF1F37C8E70430EAull, 0xFF1F37C8E70430EAull },
	{ 0x650720A3D63B1DEFull, 0x650720A3D63B1DEFull },
	{ 0x463E241F349F18A2ull, 0x463E241F349F18A2ull },
	{ 0xB0E091AB3ED20EE4ull, 0xB0E091AB3ED20EE4ull },
	{ 0x76FEC4EE2E1C8293ull, 0x76FEC4EE2E1C8293ull },
	{ 0xE653CFD8320DAC52ull, 0xE653CFD8320DAC52ull },
	{ 0xF60B6E9BBD41B498ull, 0xF60B6E9BBD41B498ull },
	{ 0x26A2C3051F6C15FBull, 0x26A2C3051F6C15FBull },
	{ 0xDC9B1DA7562480FCull, 0xDC9B1DA7562480FCull },
	{ 0xE689299FD29E6135ull, 0xE689299FD29E6135ull },
	{ 0x589E75D9A5AD2235ull, 0x589E75D9A5AD2235ull },
	{ 0xBB913B30E73FAEB1ull, 0xBB913B30E73FAEB1ull },
	{ 0x1F6EA93126909E44ull, 0x1F6EA93126909E44ull },
	{ 0xDC6B5B3EAB26C0C5ull, 0xDC6B5B3EAB26C0C5ull },
	{ 0xB96E765FC1954064ull, 0xB96E765FC1954064ull },
	{ 0x354CEF251BC1688Bull, 0x354CEF251BC1688Bull },
	{ 0xB4F2766040140DF3ull, 0xB4F2766040140DF3ull },
	{ 0x54DD8F67079C9084ull, 0x54DD8F67079C9084ull },
	{ 0x9EDC4CF32D4B86F2ull, 0x9EDC4CF32D4B86F2ull },
	{ 0xE3FF36FCED8AFD94ull, 0xE3FF36FCED8AFD94ull },
	{ 0xFA1FD7FFB49A6407ull, 0xFA1FD7FFB49A6407ull },
	{ 0x9DC072C7DDF40E03ull, 0x9DC072C7DDF40E03ull },
	{ 0xB7F25FF882A25AFCull, 0xB7F25FF882A25AFCull },
	{ 0xC7A892C859CA7AFCull, 0xC7A892C859CA7AFCull },
	{ 0xAFD3C7E4A592D73Full, 0xAFD3C7E4A592D73Full },
	{ 0x292DD99933AB1610ull, 0x292DD99933AB1610ull },
	{ 0x1791BD8C6B9BC262ull, 0x1791BD8C6B9BC262ull },
	{ 0x0435AB6E6E638DB2ull, 0x0435AB6E6E638DB2ull },
	{ 0x7B14E8EA44C83C1Bull, 0x7B14E8EA44C83C1Bull },
	{ 0x0CEEC0181EA1273Aull, 0x0CEEC0181EA1273Aull },
	{ 0xB4A81C7FA1C16244ull, 0xB4A81C7FA1C16244ull },
	{ 0x61955109494783E4ull, 0x61955109494783E4ull },
	{ 0xE4C9852DD4390959ull, 0xE4C9852DD4390959ull },
	{ 0x82F14AA2C4AF5DD0ull, 0x82F14AA2C4AF5DD0ull },
	{ 0xEA35DAD2EEFC80FFull, 0xEA35DAD2EEFC80FFull },
	{ 0x4BCE7BE217CD4A82ull, 0x4BCE7BE217CD4A82ull },
	{ 0x59ED8F2AA56F5379ull, 0x59ED8F2AA56F5379ull },
	{ 0xE93EC5F45B407861ull, 0xE93EC5F45B407861ull },
	{ 0xF713FF132CAE0333ull, 0xF713FF132CAE0333ull },
	{ 0x52A64C8D18096FBAull, 0x52A64C8D18096FBAull },
	{ 0x2B48A1561A18CFB9ull, 0x2B48A1561A18CFB9ull },
	{ 0xC1B1A60128A8D33Dull, 0xC1B1A60128A8D33Dull },
	{ 0x7E3A379544866F3Eull, 0x7E3A379544866F3Eull },
	{ 0xAA360B4597E6207Cull, 0xAA360B4597E6207Cull },
	{ 0x8DE3FDDBA41ED418ull, 0x8DE3FDDBA41ED418ull },
	{ 0xB24B8792823E6D71ull, 0xB24B8792823E6D71ull },
	{ 0x8694DAB2311DE9C1ull, 0x8694DAB2311DE9C1ull },
	{ 0xAA512889F4BB2A77ull, 0xAA512889F4BB2A77ull },
	{ 0x1AC08F38C57E5656ull, 0x1AC08F38C57E5656ull },
	{ 0x04FB2EA2CDB0E2CEull, 0x04FB2EA2CDB0E2CEull },
	{ 0xFB2AE6C2FB4C389Aull, 0xFB2AE6C2FB4C389Aull },
	{ 0xCEFB3CB79C65FB1Aull, 0xCEFB3CB79C65FB1Aull },
	{ 0x4ECCE7B90F1FEBF1ull, 0x4ECCE7B90F1FEBF1ull },
	{ 0xA79287D9812171E2ull, 0xA79287D9812171E2ull },
	{ 0x2AD4025CBD40C1E7ull, 0x2AD4025CBD40C1E7ull },
	{ 0x381E07E70CE1A013ull, 0x381E07E70CE1A013ull },
	{ 0x8ADB41D223AFC929ull, 0x8ADB41D223AFC929ull },
	{ 0x2C2CA76721FAF87Cull, 0x2C2CA76721FAF87Cull },
	{ 0x488E7BACA0DA8A08ull, 0x488E7BACA0DA8A08ull },
	{ 0x299F72C7D35EC6D1ull, 0x299F72C7D35EC6D1ull },
	{ 0x13E77FFFB4B5C57Cull, 0x13E77FFFB4B5C57Cull },
	{ 0xC893C9A145C9F2ABull, 0xC893C9A145C9F2ABull },
	{ 0xEB354913DF40371Dull, 0xEB354913DF40371Dull },
	{ 0x0B778DDC8E5BBC85ull, 0x0B778DDC8E5BBC85ull },
	{ 0x12ED2C5F836D54A9ull, 0x12ED2C5F836D54A9ull },
	{ 0xD6D7535E92D47F3Eull, 0xD6D7535E92D47F3Eull },
	{ 0x0DC60903C3380E83ull, 0x0DC60903C3380E83ull },
	{ 0x69969116EAFC6F1Cull, 0x69969116EAFC6F1Cull },
	{ 0x94DD794D621D8DB0ull, 0x94DD794D621D8DB0ull },
	{ 0xD4C1D052BEB25B45ull, 0xD4C1D052BEB25B45ull },
	{ 0x8F3701D9A57FF5DAull, 0x8F3701D9A57FF5DAull },
	{ 0xD11F4405A3889497ull, 0xD11F4405A3889497ull },
	{ 0x39A89AD18ED136A4ull, 0x39A89AD18ED136A4ull },
	{ 0xE6DB3E093BC006D4ull, 0xE6DB3E093BC006D4ull },
	{ 0xC7AD8A80B9DF3E44ull, 0xC7AD8A80B9DF3E44ull },
	{ 0x7FB1FD29240C1C17ull, 0x7FB1FD29240C1C17ull },
	{ 0xC4EA161D2F067A66ull, 0xC4EA161D2F067A66ull },
	{ 0xE77766D87987FEF3ull, 0xE77766D87987FEF3ull },
	{ 0xA6B3719003DE6217ull, 0xA6B3719003DE6217ull },
	{ 0x39BA09BA24F3D27Aull, 0x39BA09BA24F3D27Aull },
	{ 0x03251D93D2674210ull, 0x03251D93D2674210ull },
	{ 0x88EDE7E578276556ull, 0x88EDE7E578276556ull },
	{ 0x6FBD098E5E26444Eull, 0x6FBD098E5E26444Eull },
	{ 0x3B6B0A6D9FDB70B8ull, 0x3B6B0A6D9FDB70B8ull },
	{ 0x121AC36FB3AADD91ull, 0x121AC36FB3AADD91ull },
	{ 0x442F3037812655D1ull, 0x442F3037812655D1ull },
	{ 0xE675F2560EA57C88ull, 0xE675F2560EA57C88ull },
	{ 0x4842AD78B93C0D83ull, 0x4842AD78B93C0D83ull },
	{ 0xBF09D63C4C424BA2ull, 0xBF09D63C4C424BA2ull },
	{ 0x91198D0F616C7B39ull, 0x91198D0F616C7B39ull },
	{ 0xF9CED3418B8B7CF2ull, 0xF9CED3418B8B7CF2ull },
	{ 0x4690DEBB65548BDCull, 0x4690DEBB65548BDCull },
	{ 0x2F36B06CA7DD2AC4ull, 0x2F36B06CA7DD2AC4ull },
	{ 0xA3395224322B4DEDull, 0xA3395224322B4DEDull },
	{ 0xB4D14D54621B1F19ull, 0xB4D14D54621B1F19ull },
	{ 0xEB8343149B0D43A4ull, 0xEB8343149B0D43A4ull },
	{ 0x7CECE0233D88EF7Full, 0x7CECE0233D88EF7Full },
	{ 0x9A5A09D296264F4Eull, 0x9A5A09D296264F4Eull },
	{ 0x62F9335ADC5A8D45ull, 0x62F9335ADC5A8D45ull },
	{ 0x395C5BC041CB77BFull, 0x395C5BC041CB77BFull },
	{ 0x3D0C338C11B0C59Full, 0x3D0C338C11B0C59Full },
	{ 0x288AE947DE68FEEBull, 0x288AE947DE68FEEBull },
	{ 0x530C2019C187FFA5ull, 0x530C2019C187FFA5ull },
	{ 0x161F8CB0ABC520ECull, 0x161F8CB0ABC520ECull },
	{ 0xD773BADFC1CEFBD4ull, 0xD773BADFC1CEFBD4ull },
	{ 0xF69E7B17CCAAD201ull, 0xF69E7B17CCAAD201ull },
	{ 0x5ED0781DB825940Aull, 0x5ED0781DB825940Aull },
	{ 0xFCC1740D22FC838Cull, 0xFCC1740D22FC838Cull },
	{ 0xD82AAE85FBC37D8Cull, 0xD82AAE85FBC37D8Cull },
	{ 0x119CF19432AEDE92ull, 0x119CF19432AEDE92ull },
	{ 0x0477C79075470782ull, 0x0477C79075470782ull },
	{ 0xCC5F5849006B86BCull, 0xCC5F5849006B86BCull },
	{ 0x6F166EF17DABDA77ull, 0x6F166EF17DABDA77ull },
	{ 0x6A6A2728F21BD9DAull, 0x6A6A2728F21BD9DAull },
	{ 0xE5B4C2F4BEE9B8DCull, 0xE5B4C2F4BEE9B8DCull },
	{ 0x47A527DFAE9BAF5Dull, 0x47A527DFAE9BAF5Dull },
	{ 0xF020847C14BB3E84ull, 0xF020847C14BB3E84ull },
	{ 0x55FB9E37FD1418DAull, 0x55FB9E37FD1418DAull },
	{ 0xC89146A26ACCA3A7ull, 0xC89146A26ACCA3A7ull },
	{ 0x15B378F0A6FE220Cull, 0x15B378F0A6FE220Cull },
	{ 0xD35C9F629ED27861ull, 0xD35C9F629ED27861ull },
	{ 0x0D4E8115550D91D1ull, 0x0D4E8115550D91D1ull },
	{ 0x970457025EC86732ull, 0x970457025EC86732ull },
	{ 0x41658A3B7303A5A9ull, 0x41658A3B7303A5A9ull },
	{ 0xB48E98410A17F4FEull, 0xB48E98410A17F4FEull },
	{ 0xAD2DA2230C29D93Cull, 0xAD2DA2230C29D93Cull },
	{ 0xDEE8206A90A73F8Dull, 0xDEE8206A90A73F8Dull },
	{ 0xB9B741BBACF49F54ull, 0xB9B741BBACF49F54ull },
	{ 0x53A06F6E538FC10Aull, 0x53A06F6E538FC10Aull },
	{ 0xC38164F9B2112503ull, 0xC38164F9B2112503ull },
	{ 0xEDBD4428FB3834C9ull, 0xEDBD4428FB3834C9ull },
	{ 0x8A7AB30C6E97135Aull, 0x8A7AB30C6E97135Aull },
	{ 0x5CABB5C26F188A76ull, 0x5CABB5C26F188A76ull },
	{ 0x88DEA70798408B5Bull, 0x88DEA70798408B5Bull },
	{ 0x6BE72FDCF19F0371ull, 0x6BE72FDCF19F0371ull },
	{ 0x05C1C62EF0FDC4B5ull, 0x05C1C62EF0FDC4B5ull },
	{ 0x384935B04E1798E6ull, 0x384935B04E1798E6ull },
	{ 0x19BDAE0BA6466A6Eull, 0x19BDAE0BA6466A6Eull },
	{ 0x9BC4DB3EEE369592ull, 0x9BC4DB3EEE369592ull },
	{ 0xC6328483B44D56DDull, 0xC6328483B44D56DDull },
	{ 0x45B134B736D056C9ull, 0x45B134B736D056C9ull },
	{ 0x8E4A6865433C592Full, 0x8E4A6865433C592Full },
	{ 0xD295C708EB590B71ull, 0xD295C708EB590B71ull },
	{ 0x8E0EE0C607F7F0E5ull, 0x8E0EE0C607F7F0E5ull },
	{ 0x6428F78E8BAC7383ull, 0x6428F78E8BAC7383ull },
	{ 0x92D04D56A52B1052ull, 0x92D04D56A52B1052ull },
	{ 0xB9707C4AB41DA30Full, 0xB9707C4AB41DA30Full },
	{ 0x335F560A0A5ADA9Full, 0x335F560A0A5ADA9Full },
	{ 0xC6C3CB797F0EBF36ull, 0xC6C3CB797F0EBF36ull },
	{ 0x67785B90D3426648ull, 0x67785B90D3426648ull },
	{ 0xCDA08BD5E350BDB0ull, 0xCDA08BD5E350BDB0ull },
	{ 0x2060E6E8AA367B1Eull, 0x2060E6E8AA367B1Eull },
	{ 0xF4C104F3148B28C6ull, 0xF4C104F3148B28C6ull },
	{ 0x607719FFA4515346ull, 0x607719FFA4515346ull },
	{ 0xE0448E89E3ABDA57ull, 0xE0448E89E3ABDA57ull },
	{ 0x53431E8BA6265A37ull, 0x53431E8BA6265A37ull },
	{ 0xC95A011A1E5EBF55ull, 0xC95A011A1E5EBF55ull },
	{ 0x65EA9775BF3DEEBDull, 0x65EA9775BF3DEEBDull },
	{ 0xBF1A512CA9D3C3ECull, 0xBF1A512CA9D3C3ECull },
	{ 0x0E1DBA4F8FE89488ull, 0x0E1DBA4F8FE89488ull },
	{ 0xF81535EACDE0A994ull, 0xF81535EACDE0A994ull },
	{ 0x353FB0AF64B6B7FDull, 0x353FB0AF64B6B7FDull },
	{ 0x46F67F972185231Bull, 0x46F67F972185231Bull },
	{ 0x13FBB97A87FA94A9ull, 0x13FBB97A87FA94A9ull },
	{ 0x4764F8BC450D3001ull, 0x4764F8BC450D3001ull },
	{ 0x351C04913A02699Bull, 0x351C04913A02699Bull },
	{ 0x8116E29C11417B48ull, 0x8116E29C11417B48ull },
	{ 0xCB80D04365067CAAull, 0xCB80D04365067CAAull },
	{ 0x2025A4D9782C1876ull, 0x2025A4D9782C1876ull },
	{ 0x6A6919DC7B21E06Dull, 0x6A6919DC7B21E06Dull },
	{ 0x2CEB7DA664854518ull, 0x2CEB7DA664854518ull },
	{ 0xBD354C7357B32E42ull, 0xBD354C7357B32E42ull },
	{ 0x43CB31B59A882ABBull, 0x43CB31B59A882ABBull },
	{ 0xBF93F1FE1F254CCAull, 0xBF93F1FE1F254CCAull },
	{ 0x4DB1BC30976F9B73ull, 0x4DB1BC30976F9B73ull },
	{ 0x757862EA1EE02A6Aull, 0x757862EA1EE02A6Aull },
	{ 0xB7C1FEB1705AD1D6ull, 0xB7C1FEB1705AD1D6ull },
	{ 0xA31B7657ADA180C9ull, 0xA31B7657ADA180C9ull },
	{ 0x2B2A7160B51A35FFull, 0x2B2A7160B51A35FFull },
	{ 0x84C6317A0ABD9356ull, 0x84C6317A0ABD9356ull },
	{ 0xC574D3374D64168Eull, 0xC574D3374D64168Eull },
	{ 0x8746785B3E87EE41ull, 0x8746785B3E87EE41ull },
	{ 0xD08FD8E459F16EA4ull, 0xD08FD8E459F16EA4ull },
	{ 0x7F8DBD23FAE429FFull, 0x7F8DBD23FAE429FFull },
	{ 0xFA52DE7714C2A2EAull, 0xFA52DE7714C2A2EAull },
	{ 0xE2A9B12779FEA6C9ull, 0xE2A9B12779FEA6C9ull },
	{ 0x0D4B95ECB14C400Aull, 0x0D4B95ECB14C400Aull },
	{ 0x7E26F3935F6D4F9Cull, 0x7E26F3935F6D4F9Cull },
	{ 0x27E5B8023B0BBFB2ull, 0x27E5B8023B0BBFB2ull },
	{ 0x2FADC26397385AB8ull, 0x2FADC26397385AB8ull },
	{ 0x74ECB3EA77044315ull, 0x74ECB3EA77044315ull },
	{ 0x1D18A9FA374B06A3ull, 0x1D18A9FA374B06A3ull },
	{ 0x63775F47C3A356ECull, 0x63775F47C3A356ECull },
	{ 0x6CB5355B5E0C5CA5ull, 0x6CB5355B5E0C5CA5ull },
	{ 0xDEDDA629F70DB0EFull, 0xDEDDA629F70DB0EFull },
	{ 0x7CDE9DCD131FF04Cull, 0x7CDE9DCD131FF04Cull },
	{ 0x86883B704213F1DFull, 0x86883B704213F1DFull },
	{ 0x1C327B5B268E9DE7ull, 0x1C327B5B268E9DE7ull },
	{ 0xE67A482F0D0F637Dull, 0xE67A482F0D0F637Dull },
	{ 0x41CF747F12574D0Dull, 0x41CF747F12574D0Dull },
	{ 0x2E951C19DA66B992ull, 0x2E951C19DA66B992ull },
	{ 0xC4BDEDEBD867AF95ull, 0xC4BDEDEBD867AF95ull },
	{ 0xEB35DD3124BAD3E8ull, 0xEB35DD3124BAD3E8ull },
	{ 0x9452BB3DE47D001Eull, 0x9452BB3DE47D001Eull },
	{ 0x3CFD3854403B78CDull, 0x3CFD3854403B78CDull },
	{ 0x708BCE2EEB2CC387ull, 0x708BCE2EEB2CC387ull },
	{ 0x109110B739A41B95ull, 0x109110B739A41B95ull },
	{ 0x2106E76DC99589F5ull, 0x2106E76DC99589F5ull },
	{ 0xD7382FDCB7609816ull, 0xD7382FDCB7609816ull },
	{ 0x2E1245DA6677AB52ull, 0x2E1245DA6677AB52ull },
	{ 0xBE0FF5F3D7D9F4ABull, 0xBE0FF5F3D7D9F4ABull },
	{ 0x9837568591360BCAull, 0x9837568591360BCAull },
	{ 0x66AC09BA136B2134ull, 0x66AC09BA136B2134ull },
	{ 0x0FFF65F4E27ED7CEull, 0x0FFF65F4E27ED7CEull },
	{ 0x9EE008411AFDB813ull, 0x9EE008411AFDB813ull },
	{ 0xAE524E0A9B1C4793ull, 0xAE524E0A9B1C4793ull },
	{ 0x18D33578B84EBDAEull, 0x18D33578B84EBDAEull },
	{ 0xAC7F2811866D5B12ull, 0xAC7F2811866D5B12ull },
	{ 0xA2BE2B5CAE9B5FF2ull, 0xA2BE2B5CAE9B5FF2ull },
	{ 0x9BDEFB2CD7069E88ull, 0x9BDEFB2CD7069E88ull },
	{ 0xDF128A8699F5B6CCull, 0xDF128A8699F5B6CCull },
	{ 0x9E116D6C0A7AE775ull, 0x9E116D6C0A7AE775ull },
	{ 0x7CDCAA43CEC184B0ull, 0x7CDCAA43CEC184B0ull },
	{ 0x1D337342C50C131Full, 0x1D337342C50C131Full },
	{ 0xEEFF482F3B98F614ull, 0xEEFF482F3B98F614ull },
	{ 0x834FFF84B603CC0Eull, 0x834FFF84B603CC0Eull },
	{ 0xA4B9388F47196174ull, 0xA4B9388F47196174ull },
	{ 0x72DAB3C574B44590ull, 0x72DAB3C574B44590ull },
	{ 0xC1C515D7B15805CDull, 0xC1C515D7B15805CDull },
	{ 0xFE748FDC6160FCB4ull, 0xFE748FDC6160FCB4ull },
	{ 0xD5E2F16F733ECDA4ull, 0xD5E2F16F733ECDA4ull },
	{ 0x612AA20C858F7851ull, 0x612AA20C858F7851ull },
	{ 0x151C02DA9ED10E7Full, 0x151C02DA9ED10E7Full },
	{ 0x360EB79073C009FFull, 0x360EB79073C009FFull },
	{ 0x0FA058683F1844E9ull, 0x0FA058683F1844E9ull },
	{ 0xB09A0B8A278E8C1Aull, 0xB09A0B8A278E8C1Aull },
	{ 0xEA3E86A437F8D970ull, 0xEA3E86A437F8D970ull },
	{ 0x5479F816E57B2544ull, 0x5479F816E57B2544ull },
	{ 0x158558CD266EDEA8ull, 0x158558CD266EDEA8ull },
	{ 0xADA6A5871A5B0361ull, 0xADA6A5871A5B0361ull },
	{ 0x06BF808E4072D127ull, 0x06BF808E4072D127ull },
	{ 0x0833BADA3D711F9Bull, 0x0833BADA3D711F9Bull },
	{ 0x6EF9461D34C65608ull, 0x6EF9461D34C65608ull },
	{ 0x28BB0D7EADEEFEDEull, 0x28BB0D7EADEEFEDEull },
	{ 0x73E037FC0BA8FE6Bull, 0x73E037FC0BA8FE6Bull },
	{ 0x83B15135941D6C5Bull, 0x83B15135941D6C5Bull },
	{ 0xE75CDB7F06350472ull, 0xE75CDB7F06350472ull },
	{ 0xF634C89AC1AD3F87ull, 0xF634C89AC1AD3F87ull },
	{ 0x7F565C367DE40B44ull, 0x7F565C367DE40B44ull },
	{ 0x7D55F47090F01A9Cull, 0x7D55F47090F01A9Cull },
	{ 0x0E203C5CC63BE18Bull, 0x0E203C5CC63BE18Bull },
	{ 0x6E9A444FC5792814ull, 0x6E9A444FC5792814ull },
	{ 0x3A5204A80AA340F8ull, 0x3A5204A80AA340F8ull },
	{ 0x2F98C63B6BE5DDB3ull, 0x2F98C63B6BE5DDB3ull },
	{ 0x36C8F420309E893Bull, 0x36C8F420309E893Bull },
	{ 0x6E50D638B90EB9FCull, 0x6E50D638B90EB9FCull },
	{ 0x7A73B0D8B5B4880Eull, 0x7A73B0D8B5B4880Eull },
	{ 0x58EACF6AEC600638ull, 0x58EACF6AEC600638ull },
	{ 0x78B7F66E913B33A9ull, 0x78B7F66E913B33A9ull },
	{ 0xD5D2BB732C9A95F5ull, 0xD5D2BB732C9A95F5ull },
	{ 0xD91247CA128C6BCDull, 0xD91247CA128C6BCDull },
	{ 0xBBBD01F46A7C8602ull, 0xBBBD01F46A7C8602ull },
	{ 0x2F5F9E44C3A2BABCull, 0x2F5F9E44C3A2BABCull },
	{ 0x0985197B3F11FDF6ull, 0x0985197B3F11FDF6ull },
	{ 0x0888986A2B24A9A6ull, 0x0888986A2B24A9A6ull },
	{ 0xB730E6861FED1403ull, 0xB730E6861FED1403ull },
	{ 0x59EBDE98CF96D671ull, 0x59EBDE98CF96D671ull },
	{ 0x2891961154E9AA05ull, 0x2891961154E9AA05ull },
	{ 0xA35612ED68101397ull, 0xA35612ED68101397ull },
	{ 0x82DDA6DB3BA43F8Full, 0x82DDA6DB3BA43F8Full },
	{ 0x1D459281A556FFEEull, 0x1D459281A556FFEEull },
	{ 0x7F1C72299B4EEA1Full, 0x7F1C72299B4EEA1Full },
	{ 0xF77DEF73FCFD97FEull, 0xF77DEF73FCFD97FEull },
	{ 0x599D3E36AFC90128ull, 0x599D3E36AFC90128ull },
	{ 0x30D1DFAC0946FC85ull, 0x30D1DFAC0946FC85ull },
	{ 0xA9F22D6F8A68DD94ull, 0xA9F22D6F8A68DD94ull },
	{ 0x6FAF2DF2EA1530F5ull, 0x6FAF2DF2EA1530F5ull },
	{ 0xBB2D2EC6710D9BBCull, 0xBB2D2EC6710D9BBCull },
	{ 0x594CA97FA41ED8F3ull, 0x594CA97FA41ED8F3ull },
	{ 0x2CF82894378C4997ull, 0x2CF82894378C4997ull },
	{ 0x8AA0F624D5235A67ull, 0x8AA0F624D5235A67ull },
	{ 0x18D84AC13CC87039ull, 0x18D84AC13CC87039ull },
	{ 0xFD8D68425FEA8498ull, 0xFD8D68425FEA8498ull },
	{ 0xEB903696EAF8E003ull, 0xEB903696EAF8E003ull },
	{ 0x87D3CE4685882BF8ull, 0x87D3CE4685882BF8ull },
	{ 0xCA0BF700FDB2AD44ull, 0xCA0BF700FDB2AD44ull },
	{ 0x3F669741A99BF570ull, 0x3F669741A99BF570ull },
	{ 0xA1982B009C21AE70ull, 0xA1982B009C21AE70ull },
	{ 0x815AC474A5E7890Dull, 0x815AC474A5E7890Dull },
	{ 0xC52712192A0C4C12ull, 0xC52712192A0C4C12ull },
	{ 0x3C16EF7AC07600B6ull, 0x3C16EF7AC07600B6ull },
	{ 0x8498FBDDA84341CAull, 0x8498FBDDA84341CAull },
	{ 0x8734BA210C8FA8A3ull, 0x8734BA210C8FA8A3ull },
	{ 0xC546B8B214D95C49ull, 0xC546B8B214D95C49ull },
	{ 0x3BB43F91CAB5BEDFull, 0x3BB43F91CAB5BEDFull },
	{ 0x99CE0980CAA33AD5ull, 0x99CE0980CAA33AD5ull },
	{ 0x0BB18293C826324Cull, 0x0BB18293C826324Cull },
	{ 0x59B00FA3E0FF0DC9ull, 0x59B00FA3E0FF0DC9ull },
	{ 0x1A8BDEBDBF67661Eull, 0x1A8BDEBDBF67661Eull },
	{ 0xBA31BD228D245814ull, 0xBA31BD228D245814ull },
	{ 0x94EF783DB75BEDDCull, 0x94EF783DB75BEDDCull },
	{ 0x659E442280FBAF37ull, 0x659E442280FBAF37ull },
	{ 0x48B0ABB4651E4F64ull, 0x48B0ABB4651E4F64ull },
	{ 0x934B878D4B3E79C9ull, 0x934B878D4B3E79C9ull },
	{ 0x3D297E248964C265ull, 0x3D297E248964C265ull },
	{ 0x20108AC9B79EB0F5ull, 0x20108AC9B79EB0F5ull },
	{ 0xCA790D5652ACF0E5ull, 0xCA790D5652ACF0E5ull },
	{ 0x0353951722AD1246ull, 0x0353951722AD1246ull },
	{ 0x3B72910E5E653767ull, 0x3B72910E5E653767ull },
	{ 0xF72D93C5C8043B12ull, 0xF72D93C5C8043B12ull },
	{ 0x40C01D09EF757BC8ull, 0x40C01D09EF757BC8ull },
	{ 0x0919664844938C44ull, 0x0919664844938C44ull },
	{ 0xE895A1FE86D9288Full, 0xE895A1FE86D9288Full },
	{ 0xCDD8025AF58F48C1ull, 0xCDD8025AF58F48C1ull },
	{ 0xF7D0118A85C54111ull, 0xF7D0118A85C54111ull },
	{ 0xC0367CA0857B18C4ull, 0xC0367CA0857B18C4ull },
	{ 0x41800961ECE9533Cull, 0x41800961ECE9533Cull },
	{ 0x34756A126B8AAC78ull, 0x34756A126B8AAC78ull },
	{ 0x0057D854C231FA6Bull, 0x0057D854C231FA6Bull },
	{ 0xC015F9A96CEA2781ull, 0xC015F9A96CEA2781ull },
	{ 0xFA8C89E6A07E546Full, 0xFA8C89E6A07E546Full },
	{ 0x4FC64F7492E8113Aull, 0x4FC64F7492E8113Aull },
	{ 0xD966D6291801C00Aull, 0xD966D6291801C00Aull },
	{ 0xFF73736D8C3B10B0ull, 0xFF73736D8C3B10B0ull },
	{ 0x0176116A0E2D9FF4ull, 0x0176116A0E2D9FF4ull },
	{ 0x727D42810C0FB1A3ull, 0x727D42810C0FB1A3ull },
	{ 0xEA951CD61CE31658ull, 0xEA951CD61CE31658ull },
	{ 0x2B2C5DEB2B9E200Aull, 0x2B2C5DEB2B9E200Aull },
	{ 0x77D155CEF32CC8D6ull, 0x77D155CEF32CC8D6ull },
	{ 0xE34A1B27C807EA0Dull, 0xE34A1B27C807EA0Dull },
	{ 0x4FA38655B4C09F80ull, 0x4FA38655B4C09F80ull },
	{ 0x63BA74D6E43DC10Bull, 0x63BA74D6E43DC10Bull },
	{ 0x337AD4E95B8983DFull, 0x337AD4E95B8983DFull },
	{ 0x5CC7F95507AD3614ull, 0x5CC7F95507AD3614ull },
	{ 0x5849B4791579206Eull, 0x5849B4791579206Eull },
	{ 0xBDE1BC079EB67EEEull, 0xBDE1BC079EB67EEEull },
	{ 0xBE04D48AA4E1A31Bull, 0xBE04D48AA4E1A31Bull },
	{ 0x0DCB44BD5B2A3092ull, 0x0DCB44BD5B2A3092ull },
	{ 0x08141114C2577BCBull, 0x08141114C2577BCBull },
	{ 0xC26E7F82938F052Dull, 0xC26E7F82938F052Dull },
	{ 0x5525628D7B1B1B55ull, 0x5525628D7B1B1B55ull },
	{ 0x18EFAD29A3AF95A2ull, 0x18EFAD29A3AF95A2ull },
	{ 0x16E1CE0C910AB504ull, 0x16E1CE0C910AB504ull },
	{ 0x6DD353074BF01F05ull, 0x6DD353074BF01F05ull },
	{ 0xEA845501DB9BFF18ull, 0xEA845501DB9BFF18ull },
	{ 0x94E62AC3122C4020ull, 0x94E62AC3122C4020ull },
	{ 0x7052E57FA65DF043ull, 0x7052E57FA65DF043ull },
	{ 0x30F712BBA05CB970ull, 0x30F712BBA05CB970ull },
	{ 0xA3E949B912C2822Eull, 0xA3E949B912C2822Eull },
	{ 0x0C52FD7F26C52C27ull, 0x0C52FD7F26C52C27ull },
	{ 0x341618D476F070F3ull, 0x341618D476F070F3ull },
	{ 0xF072E237B9A9DE29ull, 0xF072E237B9A9DE29ull },
	{ 0xDAF9A651BD9CF823ull, 0xDAF9A651BD9CF823ull },
	{ 0xFE0BACABB28D7510ull, 0xFE0BACABB28D7510ull },
	{ 0xB5D6F244C72D9940ull, 0xB5D6F244C72D9940ull },
	{ 0x04F02D5210B6B92Bull, 0x04F02D5210B6B92Bull },
	{ 0x1D469903C861D4C1ull, 0x1D469903C861D4C1ull },
	{ 0xDC9508BAD6BE55F9ull, 0xDC9508BAD6BE55F9ull },
	{ 0xFD3BC389ABC4EE13ull, 0xFD3BC389ABC4EE13ull },
	{ 0x3B36A82B6DA496B0ull, 0x3B36A82B6DA496B0ull },
	{ 0x2CBB178C7067AB15ull, 0x2CBB178C7067AB15ull },
	{ 0x8EB1294F036F2ABAull, 0x8EB1294F036F2ABAull },
	{ 0x4603A6A78A221319ull, 0x4603A6A78A221319ull },
	{ 0xD3EA4E7AE9CEE1B5ull, 0xD3EA4E7AE9CEE1B5ull },
	{ 0x7EC9DFBA425D2F68ull, 0x7EC9DFBA425D2F68ull },
	{ 0xEF1D6E46F94D6EF1ull, 0xEF1D6E46F94D6EF1ull },
	{ 0x7FE47D7A16A4CABCull, 0x7FE47D7A16A4CABCull },
	{ 0x03B96419EE0325F3ull, 0x03B96419EE0325F3ull },
	{ 0xBD5B77791A8133C6ull, 0xBD5B77791A8133C6ull },
	{ 0x4C707F89C994BD76ull, 0x4C707F89C994BD76ull },
	{ 0xCE78F1A055D8F928ull, 0xCE78F1A055D8F928ull },
	{ 0x178B8B1AEAF423E7ull, 0x178B8B1AEAF423E7ull },
	{ 0x99DA528BADD05663ull, 0x99DA528BADD05663ull },
	{ 0x42F20299167E0F44ull, 0x42F20299167E0F44ull },
	{ 0x6160317F0B255EDCull, 0x6160317F0B255EDCull },
	{ 0xEEC593FFFB2FA3A1ull, 0xEEC593FFFB2FA3A1ull },
	{ 0x1143F33D1C2E1305ull, 0x1143F33D1C2E1305ull },
	{ 0x815EC9DEBA1FDC97ull, 0x815EC9DEBA1FDC97ull },
	{ 0x13B4098B961DF35Bull, 0x13B4098B961DF35Bull },
	{ 0xA70A893E55526FE5ull, 0xA70A893E55526FE5ull },
	{ 0x00000000000018D5ull, 0x00000000000018D5ull },
	{ 0x80000000000000CAull, 0x80000000000000CAull },
	{ 0x800041D4AA8268F3ull, 0x800041D4AA8268F3ull },
	{ 0x800000000000051Aull, 0x800000000000051Aull },
	{ 0x800000288D0A08A1ull, 0x800000288D0A08A1ull },
	{ 0x00000033DC0301AEull, 0x00000033DC0301AEull },
	{ 0x00000003B87BB586ull, 0x00000003B87BB586ull },
	{ 0x000000000009481Eull, 0x000000000009481Eull },
	{ 0x8000520024D860C1ull, 0x8000520024D860C1ull },
	{ 0x80000000026EDC81ull, 0x80000000026EDC81ull },
	{ 0x000000000069A0FEull, 0x000000000069A0FEull },
	{ 0x000182AD0C95E873ull, 0x000182AD0C95E873ull },
	{ 0x0000000000000036ull, 0x0000000000000036ull },
	{ 0x000015BD93CB3403ull, 0x000015BD93CB3403ull },
	{ 0x800000000000009Dull, 0x800000000000009Dull },
	{ 0x000018A001694A3Full, 0x000018A001694A3Full },
	{ 0x000001845D1E4BFEull, 0x000001845D1E4BFEull },
	{ 0x800002952385212Eull, 0x800002952385212Eull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x8000000009CE711Aull, 0x8000000009CE711Aull },
	{ 0x8000000AFACDBC51ull, 0x8000000AFACDBC51ull },
	{ 0x00000E51A4AC8F85ull, 0x00000E51A4AC8F85ull },
	{ 0x00006E532C5A05E2ull, 0x00006E532C5A05E2ull },
	{ 0x8000954B988ED117ull, 0x8000954B988ED117ull },
	{ 0x800000000000349Aull, 0x800000000000349Aull },
	{ 0x800047EE392A7502ull, 0x800047EE392A7502ull },
	{ 0x0000B1B729870737ull, 0x0000B1B729870737ull },
	{ 0x80000000000000F1ull, 0x80000000000000F1ull },
	{ 0x0000000000000006ull, 0x0000000000000006ull },
	{ 0x000000014141287Dull, 0x000000014141287Dull },
	{ 0x800000000001D3BCull, 0x800000000001D3BCull },
	{ 0x8000000000018050ull, 0x8000000000018050ull },
	{ 0x0000000D2F6AC6F1ull, 0x0000000D2F6AC6F1ull },
	{ 0x8000000000000021ull, 0x8000000000000021ull },
	{ 0x00000000000E591Bull, 0x00000000000E591Bull },
	{ 0x0000000016056729ull, 0x0000000016056729ull },
	{ 0x80000FDB9E653910ull, 0x80000FDB9E653910ull },
	{ 0x8000000000000005ull, 0x8000000000000005ull },
	{ 0x8000000000000917ull, 0x8000000000000917ull },
	{ 0x0000002AA4CF918Aull, 0x0000002AA4CF918Aull },
	{ 0x000000EDE0E77DC8ull, 0x000000EDE0E77DC8ull },
	{ 0x00000025635CC7C5ull, 0x00000025635CC7C5ull },
	{ 0x800000000018A507ull, 0x800000000018A507ull },
	{ 0x8000000000123CC1ull, 0x8000000000123CC1ull },
	{ 0x8000000000001870ull, 0x8000000000001870ull },
	{ 0x00000000CB4B303Eull, 0x00000000CB4B303Eull },
	{ 0x800000000286D903ull, 0x800000000286D903ull },
	{ 0x00000000000000DCull, 0x00000000000000DCull },
	{ 0x0000456051AA989Eull, 0x0000456051AA989Eull },
	{ 0x80000000000000D2ull, 0x80000000000000D2ull },
	{ 0x00000000000FFDB2ull, 0x00000000000FFDB2ull },
	{ 0x000000000C15A7C9ull, 0x000000000C15A7C9ull },
	{ 0x800000026F339C8Dull, 0x800000026F339C8Dull },
	{ 0x0000000003E14E03ull, 0x0000000003E14E03ull },
	{ 0x0000000000000007ull, 0x0000000000000007ull },
	{ 0x0000000000003DAEull, 0x0000000000003DAEull },
	{ 0x0000000001042675ull, 0x0000000001042675ull },
	{ 0x80000000000D17B6ull, 0x80000000000D17B6ull },
	{ 0x800000000009BD2Aull, 0x800000000009BD2Aull },
	{ 0x80000000000181C4ull, 0x80000000000181C4ull },
	{ 0x80002E5E4EFF9EC4ull, 0x80002E5E4EFF9EC4ull },
	{ 0x000000000000000Dull, 0x000000000000000Dull },
	{ 0x8001CA49F30498C4ull, 0x8001CA49F30498C4ull },
	{ 0x000000000000CEA5ull, 0x000000000000CEA5ull },
	{ 0x8000003F83BDB203ull, 0x8000003F83BDB203ull },
	{ 0x0000000000000009ull, 0x0000000000000009ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x000000131D4406E8ull, 0x000000131D4406E8ull },
	{ 0x000011D52CB4B58Aull, 0x000011D52CB4B58Aull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x0007F73C3F66A41Cull, 0x0007F73C3F66A41Cull },
	{ 0x00000000024B7C51ull, 0x00000000024B7C51ull },
	{ 0x000000002F4B385Cull, 0x000000002F4B385Cull },
	{ 0x80000000000000CCull, 0x80000000000000CCull },
	{ 0x000092F9705B8A3Eull, 0x000092F9705B8A3Eull },
	{ 0x0000F6BAC064785Full, 0x0000F6BAC064785Full },
	{ 0x80000000000009F8ull, 0x80000000000009F8ull },
	{ 0x800000000000001Cull, 0x800000000000001Cull },
	{ 0x0000000000000049ull, 0x0000000000000049ull },
	{ 0x80000000024E191Bull, 0x80000000024E191Bull },
	{ 0x0000014F4CF9A360ull, 0x0000014F4CF9A360ull },
	{ 0x800CFA340C3579BBull, 0x800CFA340C3579BBull },
	{ 0x000000000000649Eull, 0x000000000000649Eull },
	{ 0x0000000000007227ull, 0x0000000000007227ull },
	{ 0x800271D0BEDAFFEFull, 0x800271D0BEDAFFEFull },
	{ 0x00000000000005E2ull, 0x00000000000005E2ull },
	{ 0x800000F56255F18Full, 0x800000F56255F18Full },
	{ 0x0000000000000005ull, 0x0000000000000005ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x8000000000000174ull, 0x8000000000000174ull },
	{ 0x000000000000001Full, 0x000000000000001Full },
	{ 0x8001DC7EA9752E4Eull, 0x8001DC7EA9752E4Eull },
	{ 0x0000DC0CC5A704DBull, 0x0000DC0CC5A704DBull },
	{ 0x8000000000021B9Full, 0x8000000000021B9Full },
	{ 0x00000000002D4198ull, 0x00000000002D4198ull },
	{ 0x00000B0B57EE0B5Dull, 0x00000B0B57EE0B5Dull },
	{ 0x00000C150B08FB07ull, 0x00000C150B08FB07ull },
	{ 0x0000000000000136ull, 0x0000000000000136ull },
	{ 0x8000000000000023ull, 0x8000000000000023ull },
	{ 0x0000000007833F28ull, 0x0000000007833F28ull },
	{ 0x00007BF88C823A20ull, 0x00007BF88C823A20ull },
	{ 0x80000102E91C440Cull, 0x80000102E91C440Cull },
	{ 0x80000000001D5CA5ull, 0x80000000001D5CA5ull },
	{ 0x0000000000005483ull, 0x0000000000005483ull },
	{ 0x800000000000014Dull, 0x800000000000014Dull },
	{ 0x8000006A94EB41C2ull, 0x8000006A94EB41C2ull },
	{ 0x8000000002F89675ull, 0x8000000002F89675ull },
	{ 0x800000075FC1A90Full, 0x800000075FC1A90Full },
	{ 0x0000000000049403ull, 0x0000000000049403ull },
	{ 0x000001F5372C1C9Bull, 0x000001F5372C1C9Bull },
	{ 0x0000000076878B11ull, 0x0000000076878B11ull },
	{ 0x8001C79E520619DFull, 0x8001C79E520619DFull },
	{ 0x8001ADBEBC1B2A88ull, 0x8001ADBEBC1B2A88ull },
	{ 0x80001E6CB1F4AED1ull, 0x80001E6CB1F4AED1ull },
	{ 0x8000000000000005ull, 0x8000000000000005ull },
	{ 0x0000000000000138ull, 0x0000000000000138ull },
	{ 0x0000000000000058ull, 0x0000000000000058ull },
	{ 0x8000000004B335FFull, 0x8000000004B335FFull },
	{ 0x000000000735DFB8ull, 0x000000000735DFB8ull },
	{ 0x0000000001594336ull, 0x0000000001594336ull },
	{ 0x80000000006F85E7ull, 0x80000000006F85E7ull },
	{ 0x80000000000013C6ull, 0x80000000000013C6ull },
	{ 0x8000000000000004ull, 0x8000000000000004ull },
	{ 0x000001FEFAF80853ull, 0x000001FEFAF80853ull },
	{ 0x80000000B864FD3Aull, 0x80000000B864FD3Aull },
	{ 0x0000000000016AB3ull, 0x0000000000016AB3ull },
	{ 0x8000000000000079ull, 0x8000000000000079ull },
	{ 0x80000000B2836D5Eull, 0x80000000B2836D5Eull },
	{ 0x80000000000EA3F0ull, 0x80000000000EA3F0ull },
	{ 0x0000000000003C51ull, 0x0000000000003C51ull },
	{ 0x0000000000000011ull, 0x0000000000000011ull },
	{ 0x80015952A2D69919ull, 0x80015952A2D69919ull },
	{ 0x000000031F48F7BCull, 0x000000031F48F7BCull },
	{ 0x800000000002E3B7ull, 0x800000000002E3B7ull },
	{ 0x80007B23AC805001ull, 0x80007B23AC805001ull },
	{ 0x000000000191B063ull, 0x000000000191B063ull },
	{ 0x0000000000230CB4ull, 0x0000000000230CB4ull },
	{ 0x00005D0FB514EA80ull, 0x00005D0FB514EA80ull },
	{ 0x0000AFCCC515A68Dull, 0x0000AFCCC515A68Dull },
	{ 0x000011656C4C7B5Bull, 0x000011656C4C7B5Bull },
	{ 0x0000000000001C35ull, 0x0000000000001C35ull },
	{ 0x8000017F938DEF93ull, 0x8000017F938DEF93ull },
	{ 0x8000004BE1F7FC3Aull, 0x8000004BE1F7FC3Aull },
	{ 0x8000000000000CA9ull, 0x8000000000000CA9ull },
	{ 0x8000002D53AAEC52ull, 0x8000002D53AAEC52ull },
	{ 0x8000000000000504ull, 0x8000000000000504ull },
	{ 0x00000000005F1563ull, 0x00000000005F1563ull },
	{ 0x8000000000000313ull, 0x8000000000000313ull },
	{ 0x000000000000351Eull, 0x000000000000351Eull },
	{ 0x8000000000033627ull, 0x8000000000033627ull },
	{ 0x80000000082939B0ull, 0x80000000082939B0ull },
	{ 0x80000000000007B5ull, 0x80000000000007B5ull },
	{ 0x00002DC5318DFF7Aull, 0x00002DC5318DFF7Aull },
	{ 0x80000000E86B6B7Full, 0x80000000E86B6B7Full },
	{ 0x00000000001C52D6ull, 0x00000000001C52D6ull },
	{ 0x8000001003F7B6DFull, 0x8000001003F7B6DFull },
	{ 0x000000237234938Full, 0x000000237234938Full },
	{ 0x8000000000000203ull, 0x8000000000000203ull },
	{ 0x8000000000001563ull, 0x8000000000001563ull },
	{ 0x0000000176946E6Dull, 0x0000000176946E6Dull },
	{ 0x0000000FDD09CB91ull, 0x0000000FDD09CB91ull },
	{ 0x000000000ECFEE28ull, 0x000000000ECFEE28ull },
	{ 0x8000000000000067ull, 0x8000000000000067ull },
	{ 0x80000002CA19D9F1ull, 0x80000002CA19D9F1ull },
	{ 0x8003882E02AE054Eull, 0x8003882E02AE054Eull },
	{ 0x8000000429176326ull, 0x8000000429176326ull },
	{ 0x8000000000001F1Eull, 0x8000000000001F1Eull },
	{ 0x80000000338B1558ull, 0x80000000338B1558ull },
	{ 0x8001B1D5480FCFA4ull, 0x8001B1D5480FCFA4ull },
	{ 0x8000000000027EFCull, 0x8000000000027EFCull },
	{ 0x80000000000003DAull, 0x80000000000003DAull },
	{ 0x80000000000003E3ull, 0x80000000000003E3ull },
	{ 0x00000000104E6252ull, 0x00000000104E6252ull },
	{ 0x8000016E1AC5DC2Eull, 0x8000016E1AC5DC2Eull },
	{ 0x800044CF05B2E89Dull, 0x800044CF05B2E89Dull },
	{ 0x800000001BE124B3ull, 0x800000001BE124B3ull },
	{ 0x0000000000000543ull, 0x0000000000000543ull },
	{ 0x800000000000000Cull, 0x800000000000000Cull },
	{ 0x8000000000000087ull, 0x8000000000000087ull },
	{ 0x80001A7661210286ull, 0x80001A7661210286ull },
	{ 0x80000002300678D7ull, 0x80000002300678D7ull },
	{ 0x800000FBCCFB57CDull, 0x800000FBCCFB57CDull },
	{ 0x00000000AE22E904ull, 0x00000000AE22E904ull },
	{ 0x000000000000FEA9ull, 0x000000000000FEA9ull },
	{ 0x800001D51B523DF5ull, 0x800001D51B523DF5ull },
	{ 0x00001A94EA270014ull, 0x00001A94EA270014ull },
	{ 0x8000000006DF6027ull, 0x8000000006DF6027ull },
	{ 0x800000000000013Dull, 0x800000000000013Dull },
	{ 0x0000000049B81206ull, 0x0000000049B81206ull },
	{ 0x80019AA0765E56F7ull, 0x80019AA0765E56F7ull },
	{ 0x8000000000023827ull, 0x8000000000023827ull },
	{ 0x80000000000000F4ull, 0x80000000000000F4ull },
	{ 0x0000000000000004ull, 0x0000000000000004ull },
	{ 0x0000A7B837DA0C1Full, 0x0000A7B837DA0C1Full },
	{ 0x8000000000000913ull, 0x8000000000000913ull },
	{ 0x80000000000BA696ull, 0x80000000000BA696ull },
	{ 0x00000002A99AB7AFull, 0x00000002A99AB7AFull },
	{ 0x00000000000016A6ull, 0x00000000000016A6ull },
	{ 0x80000000000021EFull, 0x80000000000021EFull },
	{ 0x0000000016CF571Cull, 0x0000000016CF571Cull },
	{ 0x00091432D9FB9926ull, 0x00091432D9FB9926ull },
	{ 0x800000000029C8A8ull, 0x800000000029C8A8ull },
	{ 0x800000000000003Aull, 0x800000000000003Aull },
	{ 0x0000001F71F0E78Cull, 0x0000001F71F0E78Cull },
	{ 0x800000021E8EDD6Bull, 0x800000021E8EDD6Bull },
	{ 0x8000000000000006ull, 0x8000000000000006ull },
	{ 0x80000000000B7B9Aull, 0x80000000000B7B9Aull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x000000002F241751ull, 0x000000002F241751ull },
	{ 0x80000000001D3F14ull, 0x80000000001D3F14ull },
	{ 0x000000000000048Bull, 0x000000000000048Bull },
	{ 0x8000000000000E59ull, 0x8000000000000E59ull },
	{ 0x0000000000004BF7ull, 0x0000000000004BF7ull },
	{ 0x80000002E7C3F5CFull, 0x80000002E7C3F5CFull },
	{ 0x000000C440D83F82ull, 0x000000C440D83F82ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x800000084E2871DAull, 0x800000084E2871DAull },
	{ 0x80000000058FD834ull, 0x80000000058FD834ull },
	{ 0x8000001B5F2364C9ull, 0x8000001B5F2364C9ull },
	{ 0x8000000000000033ull, 0x8000000000000033ull },
	{ 0x0000000000000046ull, 0x0000000000000046ull },
	{ 0x8000000000069107ull, 0x8000000000069107ull },
	{ 0x8001B4EFC9C8DCDAull, 0x8001B4EFC9C8DCDAull },
	{ 0x80000000000001EDull, 0x80000000000001EDull },
	{ 0x00000000000001DFull, 0x00000000000001DFull },
	{ 0x8000000000000276ull, 0x8000000000000276ull },
	{ 0x0000000000005CEEull, 0x0000000000005CEEull },
	{ 0x800000000000000Full, 0x800000000000000Full },
	{ 0x8000000000000026ull, 0x8000000000000026ull },
	{ 0x0000000E1C5B1E19ull, 0x0000000E1C5B1E19ull },
	{ 0x00000000000038DBull, 0x00000000000038DBull },
	{ 0x80000002661B70BCull, 0x80000002661B70BCull },
	{ 0x8000000053052249ull, 0x8000000053052249ull },
	{ 0x80000000000B8F23ull, 0x80000000000B8F23ull },
	{ 0x000000095B821786ull, 0x000000095B821786ull },
	{ 0x800000000000321Eull, 0x800000000000321Eull },
	{ 0x8000000007691271ull, 0x8000000007691271ull },
	{ 0x800015EFA86BA7FEull, 0x800015EFA86BA7FEull },
	{ 0x00000000003BE56Dull, 0x00000000003BE56Dull },
	{ 0x0000002B5424583Full, 0x0000002B5424583Full },
	{ 0x00000007BAE48624ull, 0x00000007BAE48624ull },
	{ 0x800000000000350Full, 0x800000000000350Full },
	{ 0x000000C11A4175D8ull, 0x000000C11A4175D8ull },
	{ 0x00000000000008FFull, 0x00000000000008FFull },
	{ 0x8000000002DB8C8Eull, 0x8000000002DB8C8Eull },
	{ 0x8000000040B1BF5Full, 0x8000000040B1BF5Full },
	{ 0x0000007BC2C4CDB3ull, 0x0000007BC2C4CDB3ull },
	{ 0x80005802EE68A270ull, 0x80005802EE68A270ull },
	{ 0x800000000000017Cull, 0x800000000000017Cull },
	{ 0x000000000013446Bull, 0x000000000013446Bull },
	{ 0x8000000002C71ECFull, 0x8000000002C71ECFull },
	{ 0x00001397B51B1F41ull, 0x00001397B51B1F41ull },
	{ 0x80002C0C664AB793ull, 0x80002C0C664AB793ull },
	{ 0x0000001C29AB7CD6ull, 0x0000001C29AB7CD6ull },
	{ 0x000000003CB8CDF1ull, 0x000000003CB8CDF1ull },
	{ 0x00000019387DE850ull, 0x00000019387DE850ull },
	{ 0x80000000695104A3ull, 0x80000000695104A3ull },
	{ 0x80000095F1B595BFull, 0x80000095F1B595BFull },
	{ 0x800000000002D3A6ull, 0x800000000002D3A6ull },
	{ 0x0000000000007AA4ull, 0x0000000000007AA4ull },
	{ 0x000001560C2652ADull, 0x000001560C2652ADull },
	{ 0x0000016CD1C0C5EEull, 0x0000016CD1C0C5EEull },
	{ 0x000000000000025Bull, 0x000000000000025Bull },
	{ 0x8000000000001ED2ull, 0x8000000000001ED2ull },
	{ 0x000213D9AD188B61ull, 0x000213D9AD188B61ull },
	{ 0x800000000030EF7Full, 0x800000000030EF7Full },
	{ 0x800000000003960Full, 0x800000000003960Full },
	{ 0x8000000000000060ull, 0x8000000000000060ull },
	{ 0x000000000363BEE7ull, 0x000000000363BEE7ull },
	{ 0x800005518D9C66ACull, 0x800005518D9C66ACull },
	{ 0x80000047E713E296ull, 0x80000047E713E296ull },
	{ 0x800000029346FB95ull, 0x800000029346FB95ull },
	{ 0x800000007303F19Bull, 0x800000007303F19Bull },
	{ 0x00001835B7DA4471ull, 0x00001835B7DA4471ull },
	{ 0x0000000000000069ull, 0x0000000000000069ull },
	{ 0x8000000000000022ull, 0x8000000000000022ull },
	{ 0x0000000000030375ull, 0x0000000000030375ull },
	{ 0x0000047DED1C8E80ull, 0x0000047DED1C8E80ull },
	{ 0x80000000000D7CD2ull, 0x80000000000D7CD2ull },
	{ 0x8000000000001D6Full, 0x8000000000001D6Full },
	{ 0x000006CEF79B3E93ull, 0x000006CEF79B3E93ull },
	{ 0x0000000049DFB2D3ull, 0x0000000049DFB2D3ull },
	{ 0x0000000F5D15C856ull, 0x0000000F5D15C856ull },
	{ 0x000000000000001Aull, 0x000000000000001Aull },
	{ 0x000000000030DF06ull, 0x000000000030DF06ull },
	{ 0x0000000000095504ull, 0x0000000000095504ull },
	{ 0x00000011DEBB394Aull, 0x00000011DEBB394Aull },
	{ 0x800000000007CA3Dull, 0x800000000007CA3Dull },
	{ 0x8000000000000D6Bull, 0x8000000000000D6Bull },
	{ 0x800015F63DF3FF19ull, 0x800015F63DF3FF19ull },
	{ 0x000000000000134Dull, 0x000000000000134Dull },
	{ 0x00000003F76326FFull, 0x00000003F76326FFull },
	{ 0x0000000000092F46ull, 0x0000000000092F46ull },
	{ 0x0000000000A18F08ull, 0x0000000000A18F08ull },
	{ 0x8000000069BB6C5Dull, 0x8000000069BB6C5Dull },
	{ 0x8000000000000007ull, 0x8000000000000007ull },
	{ 0x8000000FE39CEADCull, 0x8000000FE39CEADCull },
	{ 0x000284245A07C451ull, 0x000284245A07C451ull },
	{ 0x800000000038AF98ull, 0x800000000038AF98ull },
	{ 0x000000000000000Aull, 0x000000000000000Aull },
	{ 0x0000000000000539ull, 0x0000000000000539ull },
	{ 0x800007B8FD05CF36ull, 0x800007B8FD05CF36ull },
	{ 0x000000000000023Bull, 0x000000000000023Bull },
	{ 0x00000000316D6740ull, 0x00000000316D6740ull },
	{ 0x00000000000182C0ull, 0x00000000000182C0ull },
	{ 0x0000000EE3F36D1Full, 0x0000000EE3F36D1Full },
	{ 0x8000027E27433A0Bull, 0x8000027E27433A0Bull },
	{ 0x0000000000000005ull, 0x0000000000000005ull },
	{ 0x0000000000011E10ull, 0x0000000000011E10ull },
	{ 0x800000000000001Aull, 0x800000000000001Aull },
	{ 0x00000000002A1619ull, 0x00000000002A1619ull },
	{ 0x000000000000135Eull, 0x000000000000135Eull },
	{ 0x00001A7523D733B1ull, 0x00001A7523D733B1ull },
	{ 0x8000000000030DB7ull, 0x8000000000030DB7ull },
	{ 0x80000000570696D8ull, 0x80000000570696D8ull },
	{ 0x800000003591DB9Dull, 0x800000003591DB9Dull },
	{ 0x800000000004248Eull, 0x800000000004248Eull },
	{ 0x00002C8ED206E1A3ull, 0x00002C8ED206E1A3ull },
	{ 0x8000000F3F03B1C7ull, 0x8000000F3F03B1C7ull },
	{ 0x0000000000000872ull, 0x0000000000000872ull },
	{ 0x000000000000008Cull, 0x000000000000008Cull },
	{ 0x00000021A5C76068ull, 0x00000021A5C76068ull },
	{ 0x800334F8E23BE1B3ull, 0x800334F8E23BE1B3ull },
	{ 0x8000D492BCB8EEBCull, 0x8000D492BCB8EEBCull },
	{ 0x000000000126B614ull, 0x000000000126B614ull },
	{ 0x000000000035E857ull, 0x000000000035E857ull },
	{ 0x8006A36B3945B1C6ull, 0x8006A36B3945B1C6ull },
	{ 0x8000000000000026ull, 0x8000000000000026ull },
	{ 0x0000026B46BECA73ull, 0x0000026B46BECA73ull },
	{ 0x000000A62279269Aull, 0x000000A62279269Aull },
	{ 0x80015DF79FC28335ull, 0x80015DF79FC28335ull },
	{ 0x0000000BAF678903ull, 0x0000000BAF678903ull },
	{ 0x0000000000000111ull, 0x0000000000000111ull },
	{ 0x8000000048449765ull, 0x8000000048449765ull },
	{ 0x00000005CEF1AF75ull, 0x00000005CEF1AF75ull },
	{ 0x8000001D5F15FAD6ull, 0x8000001D5F15FAD6ull },
	{ 0x800014A904FE6429ull, 0x800014A904FE6429ull },
	{ 0x000000001A56C8D2ull, 0x000000001A56C8D2ull },
	{ 0x80007CCB3ED30F00ull, 0x80007CCB3ED30F00ull },
	{ 0x8000A0F90662C971ull, 0x8000A0F90662C971ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x80000003BC3DD009ull, 0x80000003BC3DD009ull },
	{ 0x00008F91D55A751Bull, 0x00008F91D55A751Bull },
	{ 0x0000000000001453ull, 0x0000000000001453ull },
	{ 0x00000000000002D4ull, 0x00000000000002D4ull },
	{ 0x0000000001B91AC2ull, 0x0000000001B91AC2ull },
	{ 0x0000000094AEABC2ull, 0x0000000094AEABC2ull },
	{ 0x0006CC1507683A3Full, 0x0006CC1507683A3Full },
	{ 0x800000027C6D304Bull, 0x800000027C6D304Bull },
	{ 0x80000008534081D2ull, 0x80000008534081D2ull },
	{ 0x800000000000022Cull, 0x800000000000022Cull },
	{ 0x800000000000000Aull, 0x800000000000000Aull },
	{ 0x800000000043C309ull, 0x800000000043C309ull },
	{ 0x8000000000000014ull, 0x8000000000000014ull },
	{ 0x000032A1A17C7214ull, 0x000032A1A17C7214ull },
	{ 0x00000032A5C561AEull, 0x00000032A5C561AEull },
	{ 0x800000000000035Aull, 0x800000000000035Aull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x00000000075F27D8ull, 0x00000000075F27D8ull },
	{ 0x0000000000000073ull, 0x0000000000000073ull },
	{ 0x00000004738D9338ull, 0x00000004738D9338ull },
	{ 0x800000000120DE8Eull, 0x800000000120DE8Eull },
	{ 0x0000000003F10059ull, 0x0000000003F10059ull },
	{ 0x0000000009665F9Bull, 0x0000000009665F9Bull },
	{ 0x000000000005A0D6ull, 0x000000000005A0D6ull },
	{ 0x8000000000717108ull, 0x8000000000717108ull },
	{ 0x000000000000017Full, 0x000000000000017Full },
	{ 0x8000011B5480EA55ull, 0x8000011B5480EA55ull },
	{ 0x80000000000A7DEEull, 0x80000000000A7DEEull },
	{ 0x0002274A4E040F56ull, 0x0002274A4E040F56ull },
	{ 0x000000000000002Full, 0x000000000000002Full },
	{ 0x80000185D1D51A0Dull, 0x80000185D1D51A0Dull },
	{ 0x0000005649348F5Bull, 0x0000005649348F5Bull },
	{ 0x80000000000000BCull, 0x80000000000000BCull },
	{ 0x800000000000053Dull, 0x800000000000053Dull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x000000000000007Full, 0x000000000000007Full },
	{ 0x0000071ECDC2B70Aull, 0x0000071ECDC2B70Aull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x80000000000002F8ull, 0x80000000000002F8ull },
	{ 0x800008870BA85C9Dull, 0x800008870BA85C9Dull },
	{ 0x800000000000006Full, 0x800000000000006Full },
	{ 0x00000000005A3486ull, 0x00000000005A3486ull },
	{ 0x80000000061A8358ull, 0x80000000061A8358ull },
	{ 0x00000003EC40E10Bull, 0x00000003EC40E10Bull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x000009ED38373450ull, 0x000009ED38373450ull },
	{ 0x0000FE8A93D56685ull, 0x0000FE8A93D56685ull },
	{ 0x0000000000010EAAull, 0x0000000000010EAAull },
	{ 0x8000000000000132ull, 0x8000000000000132ull },
	{ 0x800029F385C081A2ull, 0x800029F385C081A2ull },
	{ 0x800000000000000Dull, 0x800000000000000Dull },
	{ 0x000000000004676Aull, 0x000000000004676Aull },
	{ 0x8000000000000074ull, 0x8000000000000074ull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x000000000000000Bull, 0x000000000000000Bull },
	{ 0x0000000C0CF616A9ull, 0x0000000C0CF616A9ull },
	{ 0x000000000000044Bull, 0x000000000000044Bull },
	{ 0x000000000008A765ull, 0x000000000008A765ull },
	{ 0x0000000000000138ull, 0x0000000000000138ull },
	{ 0x8000000000018098ull, 0x8000000000018098ull },
	{ 0x000004CB2A120CA3ull, 0x000004CB2A120CA3ull },
	{ 0x000000000006081Dull, 0x000000000006081Dull },
	{ 0x00000000000E44BBull, 0x00000000000E44BBull },
	{ 0x00000000006BA30Dull, 0x00000000006BA30Dull },
	{ 0x0000000000000632ull, 0x0000000000000632ull },
	{ 0x000003B2C436AFCFull, 0x000003B2C436AFCFull },
	{ 0x80001DC5069B9C37ull, 0x80001DC5069B9C37ull },
	{ 0x8000000000002D28ull, 0x8000000000002D28ull },
	{ 0x800000080E4A6549ull, 0x800000080E4A6549ull },
	{ 0x000000000000B199ull, 0x000000000000B199ull },
	{ 0x00000000001D9BA0ull, 0x00000000001D9BA0ull },
	{ 0x0000023DDE7DCD38ull, 0x0000023DDE7DCD38ull },
	{ 0x80000000073AD731ull, 0x80000000073AD731ull },
	{ 0x800000CD123389FFull, 0x800000CD123389FFull },
	{ 0x80000004EFEDCA0Full, 0x80000004EFEDCA0Full },
	{ 0x80000DC3A862B8A2ull, 0x80000DC3A862B8A2ull },
	{ 0x0000000000000273ull, 0x0000000000000273ull },
	{ 0x80000A04AB5D6D0Full, 0x80000A04AB5D6D0Full },
	{ 0x8000000000007EEFull, 0x8000000000007EEFull },
	{ 0x8000072E9E9D4012ull, 0x8000072E9E9D4012ull },
	{ 0x000000000DF28B48ull, 0x000000000DF28B48ull },
	{ 0x80000001C298E571ull, 0x80000001C298E571ull },
	{ 0x800000005377645Aull, 0x800000005377645Aull },
	{ 0x800033B09BAAFB25ull, 0x800033B09BAAFB25ull },
	{ 0x8000000004906447ull, 0x8000000004906447ull },
	{ 0x000430096876F7E7ull, 0x000430096876F7E7ull },
	{ 0x80000001BEAE21F3ull, 0x80000001BEAE21F3ull },
	{ 0x8000000000016FE5ull, 0x8000000000016FE5ull },
	{ 0x000001D362498B8Full, 0x000001D362498B8Full },
	{ 0x00000F50C167B486ull, 0x00000F50C167B486ull },
	{ 0x0000000000346196ull, 0x0000000000346196ull },
	{ 0x000000C5CF841F05ull, 0x000000C5CF841F05ull },
	{ 0x8000000000002B83ull, 0x8000000000002B83ull },
	{ 0x000000000352DEE6ull, 0x000000000352DEE6ull },
	{ 0x000000046428B973ull, 0x000000046428B973ull },
	{ 0x8000000000000014ull, 0x8000000000000014ull },
	{ 0x000000005DC7DF22ull, 0x000000005DC7DF22ull },
	{ 0x0000000056A32EFFull, 0x0000000056A32EFFull },
	{ 0x00018EAB67BB6171ull, 0x00018EAB67BB6171ull },
	{ 0x000000000001CB8Dull, 0x000000000001CB8Dull },
	{ 0x0000000002AB98E5ull, 0x0000000002AB98E5ull },
	{ 0x0005662209E739F8ull, 0x0005662209E739F8ull },
	{ 0x80000358AAB17A2Eull, 0x80000358AAB17A2Eull },
	{ 0x00000000058D808Cull, 0x00000000058D808Cull },
	{ 0x00000000000002D4ull, 0x00000000000002D4ull },
	{ 0x800000AE99794974ull, 0x800000AE99794974ull },
	{ 0x00000000000001F0ull, 0x00000000000001F0ull },
	{ 0x0000F5BEF428E8F0ull, 0x0000F5BEF428E8F0ull },
	{ 0x80000019BA523E15ull, 0x80000019BA523E15ull },
	{ 0x80000000358A9FE1ull, 0x80000000358A9FE1ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x0000000542B4E4F0ull, 0x0000000542B4E4F0ull },
	{ 0x00058978FD4C133Bull, 0x00058978FD4C133Bull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x800000265CCBE7DCull, 0x800000265CCBE7DCull },
	{ 0x80000021BA5D355Cull, 0x80000021BA5D355Cull },
	{ 0x0000000408424382ull, 0x0000000408424382ull },
	{ 0x8003A0925CD6393Eull, 0x8003A0925CD6393Eull },
	{ 0x8000016226D40B4Full, 0x8000016226D40B4Full },
	{ 0x0000000078D45F48ull, 0x0000000078D45F48ull },
	{ 0x8000160FC9EFD7B1ull, 0x8000160FC9EFD7B1ull },
	{ 0x00003AA6C407657Dull, 0x00003AA6C407657Dull },
	{ 0x0000006C7407DF09ull, 0x0000006C7407DF09ull },
	{ 0x00000040E1D0C3ADull, 0x00000040E1D0C3ADull },
	{ 0x0000000025A7A796ull, 0x0000000025A7A796ull },
	{ 0x8000000003F6A957ull, 0x8000000003F6A957ull },
	{ 0x800000000000029Aull, 0x800000000000029Aull },
	{ 0x8000000000000CF0ull, 0x8000000000000CF0ull },
	{ 0x00052E654A00A4FBull, 0x00052E654A00A4FBull },
	{ 0x800034DD602425C9ull, 0x800034DD602425C9ull },
	{ 0x8000000000079752ull, 0x8000000000079752ull },
	{ 0x800001FE1EC0BDC7ull, 0x800001FE1EC0BDC7ull },
	{ 0x000000000000001Eull, 0x000000000000001Eull },
	{ 0x80000000000040A2ull, 0x80000000000040A2ull },
	{ 0x80000000000015A1ull, 0x80000000000015A1ull },
	{ 0x8000000000000014ull, 0x8000000000000014ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x80000000034164B6ull, 0x80000000034164B6ull },
	{ 0x0000000000002DBCull, 0x0000000000002DBCull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x80000000000007FAull, 0x80000000000007FAull },
	{ 0x000000000000001Full, 0x000000000000001Full },
	{ 0x0007EE43E0B03917ull, 0x0007EE43E0B03917ull },
	{ 0x800000002C7B5DC6ull, 0x800000002C7B5DC6ull },
	{ 0x8000018AA42F222Aull, 0x8000018AA42F222Aull },
	{ 0x8000000000000594ull, 0x8000000000000594ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x80000000380CCB09ull, 0x80000000380CCB09ull },
	{ 0x8000000000000013ull, 0x8000000000000013ull },
	{ 0x0000122454D88D3Aull, 0x0000122454D88D3Aull },
	{ 0x80005B174F222D24ull, 0x80005B174F222D24ull },
	{ 0x00000EA1F64238EFull, 0x00000EA1F64238EFull },
	{ 0x8000000000006CF3ull, 0x8000000000006CF3ull },
	{ 0x00002C44995ABAA0ull, 0x00002C44995ABAA0ull },
	{ 0x80001DB8DE34E865ull, 0x80001DB8DE34E865ull },
	{ 0x80000000004C4E2Aull, 0x80000000004C4E2Aull },
	{ 0x0000005114395CDAull, 0x0000005114395CDAull },
	{ 0x0000000000000070ull, 0x0000000000000070ull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull }
};

CShader5xTest_DoubleArith::Binary64Vec2 const CShader5xTest_dadd::s_DAddSrc1[] = 
{
	{ 0x3FF0000000000000ull, 0x3FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x7FF3F89B51837A9Dull, 0x7FF3F89B51837A9Dull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0xB7854F713C689E44ull, 0xB7854F713C689E44ull },
	{ 0x7FF9436DD07BF2E5ull, 0x7FF9436DD07BF2E5ull },
	{ 0x4F57F53BE90A6D77ull, 0x4F57F53BE90A6D77ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xBCAEA0E7896C803Dull, 0xBCAEA0E7896C803Dull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xDDF4900A52A14BA1ull, 0xDDF4900A52A14BA1ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x351E5198B30372FDull, 0x351E5198B30372FDull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x5744B484274AA4EEull, 0x5744B484274AA4EEull },
	{ 0x8000000003F9D285ull, 0x8000000003F9D285ull },
	{ 0x7FFB37D2241C3B1Dull, 0x7FFB37D2241C3B1Dull },
	{ 0x0000001983B94C92ull, 0x0000001983B94C92ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x80009443E584F4A5ull, 0x80009443E584F4A5ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x8000000855A82C2Cull, 0x8000000855A82C2Cull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x000000000005B6ABull, 0x000000000005B6ABull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x00000000B58DC413ull, 0x00000000B58DC413ull },
	{ 0x17937B959D322AE4ull, 0x17937B959D322AE4ull },
	{ 0x710CE3011CCBFE44ull, 0x710CE3011CCBFE44ull },
	{ 0xB3E6B297EC29791Dull, 0xB3E6B297EC29791Dull },
	{ 0x538FB3BC5CFA6759ull, 0x538FB3BC5CFA6759ull },
	{ 0xBA000797F4C7DEA1ull, 0xBA000797F4C7DEA1ull },
	{ 0x0C1C8FF2F41F16DAull, 0x0C1C8FF2F41F16DAull },
	{ 0x30B5C1676C56DD62ull, 0x30B5C1676C56DD62ull },
	{ 0x5CCB5C56E7C5AC0Bull, 0x5CCB5C56E7C5AC0Bull },
	{ 0x63068E39A09E6158ull, 0x63068E39A09E6158ull },
	{ 0x07B3FB8E47099176ull, 0x07B3FB8E47099176ull },
	{ 0x184F31A158F1807Dull, 0x184F31A158F1807Dull },
	{ 0x8758FF530896B7E7ull, 0x8758FF530896B7E7ull },
	{ 0x4C0A36D2B4A33B50ull, 0x4C0A36D2B4A33B50ull },
	{ 0xC64C5756ED525E6Full, 0xC64C5756ED525E6Full },
	{ 0x66199E890D233BD4ull, 0x66199E890D233BD4ull },
	{ 0x578E020B5EEBCB63ull, 0x578E020B5EEBCB63ull },
	{ 0xF28A90D0D4429690ull, 0xF28A90D0D4429690ull },
	{ 0x2E44BC8D4F15105Cull, 0x2E44BC8D4F15105Cull },
	{ 0xD9830B957560898Full, 0xD9830B957560898Full },
	{ 0xD5F0B21519E3C836ull, 0xD5F0B21519E3C836ull },
	{ 0x17D9939333BF3CE0ull, 0x17D9939333BF3CE0ull },
	{ 0x2A282C8A6399D814ull, 0x2A282C8A6399D814ull },
	{ 0xF616E3ED0C808295ull, 0xF616E3ED0C808295ull },
	{ 0x8EEE447BF9D63059ull, 0x8EEE447BF9D63059ull },
	{ 0x41719E46964A9745ull, 0x41719E46964A9745ull },
	{ 0x285390BBB32D882Eull, 0x285390BBB32D882Eull },
	{ 0x0333FB01DF54E08Cull, 0x0333FB01DF54E08Cull },
	{ 0x12E0D6DE4EAB2400ull, 0x12E0D6DE4EAB2400ull },
	{ 0xB8DB78A14FDCAF88ull, 0xB8DB78A14FDCAF88ull },
	{ 0x9B3BBA9F54F99101ull, 0x9B3BBA9F54F99101ull },
	{ 0x8C428C6D88C1FDDDull, 0x8C428C6D88C1FDDDull },
	{ 0xF6556E68F7B36624ull, 0xF6556E68F7B36624ull },
	{ 0x0B7451FF444C2F30ull, 0x0B7451FF444C2F30ull },
	{ 0xF943635FF0DC04A4ull, 0xF943635FF0DC04A4ull },
	{ 0x60BA3B4D3516D22Dull, 0x60BA3B4D3516D22Dull },
	{ 0x51A1F8B3648057FCull, 0x51A1F8B3648057FCull },
	{ 0x0AA3C17DE7265F82ull, 0x0AA3C17DE7265F82ull },
	{ 0xBCF12C70BEFF94EEull, 0xBCF12C70BEFF94EEull },
	{ 0x9408133D9FA4F8E4ull, 0x9408133D9FA4F8E4ull },
	{ 0xC14B4AFB957BFA7Eull, 0xC14B4AFB957BFA7Eull },
	{ 0xCBF1C6783C472A07ull, 0xCBF1C6783C472A07ull },
	{ 0xE4B59F4986B39276ull, 0xE4B59F4986B39276ull },
	{ 0xFDA487571458AA28ull, 0xFDA487571458AA28ull },
	{ 0x65BB22A219C0EF62ull, 0x65BB22A219C0EF62ull },
	{ 0xA8BCC882D7B11885ull, 0xA8BCC882D7B11885ull },
	{ 0xF2F12D229FC3EE93ull, 0xF2F12D229FC3EE93ull },
	{ 0xB8F973296F10BFE2ull, 0xB8F973296F10BFE2ull },
	{ 0xE356252611F2779Eull, 0xE356252611F2779Eull },
	{ 0xBE6F96E7DCA55404ull, 0xBE6F96E7DCA55404ull },
	{ 0x6E1A2F90F2513BDCull, 0x6E1A2F90F2513BDCull },
	{ 0xA5681A181B76B3B7ull, 0xA5681A181B76B3B7ull },
	{ 0x6291DFC12CD4766Aull, 0x6291DFC12CD4766Aull },
	{ 0x6E876922FF3BA6CAull, 0x6E876922FF3BA6CAull },
	{ 0xE092E7C1F5A8AB28ull, 0xE092E7C1F5A8AB28ull },
	{ 0xE67F277615169F0Full, 0xE67F277615169F0Full },
	{ 0x8FFAC7E3AAA36B3Cull, 0x8FFAC7E3AAA36B3Cull },
	{ 0x5E69DE7D833A79DDull, 0x5E69DE7D833A79DDull },
	{ 0xD8EF7F37B1DC0E0Eull, 0xD8EF7F37B1DC0E0Eull },
	{ 0x6390AE81E8373B90ull, 0x6390AE81E8373B90ull },
	{ 0x9BCA3AF05AEC78CCull, 0x9BCA3AF05AEC78CCull },
	{ 0x779AFE4A3BEED607ull, 0x779AFE4A3BEED607ull },
	{ 0xE96C0724BE59D6E5ull, 0xE96C0724BE59D6E5ull },
	{ 0x48282AC1AFF1E2A3ull, 0x48282AC1AFF1E2A3ull },
	{ 0x3A9C7E589DA56010ull, 0x3A9C7E589DA56010ull },
	{ 0xE76337E18D84664Cull, 0xE76337E18D84664Cull },
	{ 0x28927928434515C4ull, 0x28927928434515C4ull },
	{ 0x83A07FBF184A8DEDull, 0x83A07FBF184A8DEDull },
	{ 0xA4D6BF58628482C0ull, 0xA4D6BF58628482C0ull },
	{ 0x9F6562DD6BDD72F5ull, 0x9F6562DD6BDD72F5ull },
	{ 0xB0F0B669F80D7C82ull, 0xB0F0B669F80D7C82ull },
	{ 0x4979F8835D07D6D6ull, 0x4979F8835D07D6D6ull },
	{ 0x16DD13522958E2D2ull, 0x16DD13522958E2D2ull },
	{ 0x54E4BE755A6AE588ull, 0x54E4BE755A6AE588ull },
	{ 0x20CE889624875DB7ull, 0x20CE889624875DB7ull },
	{ 0x910048B34A0AF605ull, 0x910048B34A0AF605ull },
	{ 0x512877AB01A220FEull, 0x512877AB01A220FEull },
	{ 0x017AF1FA6CC4484Cull, 0x017AF1FA6CC4484Cull },
	{ 0x1AA3A0B9A083A938ull, 0x1AA3A0B9A083A938ull },
	{ 0x15D08B6B3B27C660ull, 0x15D08B6B3B27C660ull },
	{ 0x982CD1C68B287EBAull, 0x982CD1C68B287EBAull },
	{ 0xE26E0CD3473FC4C9ull, 0xE26E0CD3473FC4C9ull },
	{ 0xC4ED983DD3E7F21Dull, 0xC4ED983DD3E7F21Dull },
	{ 0x575F445B1BBCC090ull, 0x575F445B1BBCC090ull },
	{ 0xB45EEF22F5D4DE3Eull, 0xB45EEF22F5D4DE3Eull },
	{ 0xF94686781CCA2344ull, 0xF94686781CCA2344ull },
	{ 0x4A05F0A4B5C668BDull, 0x4A05F0A4B5C668BDull },
	{ 0x18EE5E82678AFEFAull, 0x18EE5E82678AFEFAull },
	{ 0x720A85150148C080ull, 0x720A85150148C080ull },
	{ 0x0B653E2EB23ECE47ull, 0x0B653E2EB23ECE47ull },
	{ 0xD8781344CEF5DF2Full, 0xD8781344CEF5DF2Full },
	{ 0x62032DAC2ACE3946ull, 0x62032DAC2ACE3946ull },
	{ 0x21C830C3FDB647B8ull, 0x21C830C3FDB647B8ull },
	{ 0x22597B665A3FCF16ull, 0x22597B665A3FCF16ull },
	{ 0xA2A751653669C6CBull, 0xA2A751653669C6CBull },
	{ 0x7E1C6CEB0114CADAull, 0x7E1C6CEB0114CADAull },
	{ 0x97F377C5CA38345Cull, 0x97F377C5CA38345Cull },
	{ 0x8B66ED93F90DCFB9ull, 0x8B66ED93F90DCFB9ull },
	{ 0xDEF4E4B993CA3126ull, 0xDEF4E4B993CA3126ull },
	{ 0xFC983FB515D7AC5Full, 0xFC983FB515D7AC5Full },
	{ 0xAD10C77AD6E9E824ull, 0xAD10C77AD6E9E824ull },
	{ 0x582BABEE02A018F8ull, 0x582BABEE02A018F8ull },
	{ 0x47F5EE3E1F4BCF18ull, 0x47F5EE3E1F4BCF18ull },
	{ 0xB0EA316024D2773Aull, 0xB0EA316024D2773Aull },
	{ 0xBD83752A237F6507ull, 0xBD83752A237F6507ull },
	{ 0xDDF635E87A7C93D9ull, 0xDDF635E87A7C93D9ull },
	{ 0x1BF97773A0C7F3B9ull, 0x1BF97773A0C7F3B9ull },
	{ 0x3281359D7A1B6513ull, 0x3281359D7A1B6513ull },
	{ 0xA9B3BE5D3F5D4CBBull, 0xA9B3BE5D3F5D4CBBull },
	{ 0xB63B72F0F40FCAA4ull, 0xB63B72F0F40FCAA4ull },
	{ 0x1D6E71BB6F948EDEull, 0x1D6E71BB6F948EDEull },
	{ 0xDCDDA755F17B4FD0ull, 0xDCDDA755F17B4FD0ull },
	{ 0x5614C83C4AF0E037ull, 0x5614C83C4AF0E037ull },
	{ 0xBE7CB24A93C4E75Full, 0xBE7CB24A93C4E75Full },
	{ 0x12D9B693739C329Full, 0x12D9B693739C329Full },
	{ 0xCF574A05F645B01Aull, 0xCF574A05F645B01Aull },
	{ 0xAB1AA029F5480133ull, 0xAB1AA029F5480133ull },
	{ 0x980DAB6A0CDDB250ull, 0x980DAB6A0CDDB250ull },
	{ 0xC69C0732247610D4ull, 0xC69C0732247610D4ull },
	{ 0x6D63495984B0A056ull, 0x6D63495984B0A056ull },
	{ 0x15303BDF7EA930A5ull, 0x15303BDF7EA930A5ull },
	{ 0x287A7D91A34A9581ull, 0x287A7D91A34A9581ull },
	{ 0x07280CE28A3DFA92ull, 0x07280CE28A3DFA92ull },
	{ 0x5E8A3848282CD92Cull, 0x5E8A3848282CD92Cull },
	{ 0x04DD7828B6AB90C5ull, 0x04DD7828B6AB90C5ull },
	{ 0x192DB04D2AC29AB6ull, 0x192DB04D2AC29AB6ull },
	{ 0xA6F859603C6C6131ull, 0xA6F859603C6C6131ull },
	{ 0x8E1E1290FC7DB483ull, 0x8E1E1290FC7DB483ull },
	{ 0x6E6D1AFFFCCEE30Aull, 0x6E6D1AFFFCCEE30Aull },
	{ 0x578632EE071B6B78ull, 0x578632EE071B6B78ull },
	{ 0xFC7565D362AE56C9ull, 0xFC7565D362AE56C9ull },
	{ 0x2B903B56AD512C81ull, 0x2B903B56AD512C81ull },
	{ 0xCA62D14A4153882Aull, 0xCA62D14A4153882Aull },
	{ 0x94FD5CA72BE1508Dull, 0x94FD5CA72BE1508Dull },
	{ 0xE20B936CB53D8CAFull, 0xE20B936CB53D8CAFull },
	{ 0x2A777E1B76D7D98Eull, 0x2A777E1B76D7D98Eull },
	{ 0x153130B30392829Dull, 0x153130B30392829Dull },
	{ 0xC70CED291CEF347Cull, 0xC70CED291CEF347Cull },
	{ 0x36212BE17B38547Bull, 0x36212BE17B38547Bull },
	{ 0x42160E1D249CF651ull, 0x42160E1D249CF651ull },
	{ 0xCF4FB68853557399ull, 0xCF4FB68853557399ull },
	{ 0xA7150B9DEDC69F8Dull, 0xA7150B9DEDC69F8Dull },
	{ 0xDB4C60678CB99E85ull, 0xDB4C60678CB99E85ull },
	{ 0xEDEF86D1121E5B31ull, 0xEDEF86D1121E5B31ull },
	{ 0xFD00C816D0AE9C15ull, 0xFD00C816D0AE9C15ull },
	{ 0x3292476542E7BC45ull, 0x3292476542E7BC45ull },
	{ 0xB7C94A7151B0FDE2ull, 0xB7C94A7151B0FDE2ull },
	{ 0x6D6EEB802B2481D5ull, 0x6D6EEB802B2481D5ull },
	{ 0xA6F7B2C5AAD0DE49ull, 0xA6F7B2C5AAD0DE49ull },
	{ 0x2AE397F04F2953EDull, 0x2AE397F04F2953EDull },
	{ 0xB63FEB49C17AA36Bull, 0xB63FEB49C17AA36Bull },
	{ 0xC3FCA7A2EBDA8423ull, 0xC3FCA7A2EBDA8423ull },
	{ 0xD97EAC12A227BB2Cull, 0xD97EAC12A227BB2Cull },
	{ 0x95905C9DD6D0CF8Cull, 0x95905C9DD6D0CF8Cull },
	{ 0xC4EA2C5063B76035ull, 0xC4EA2C5063B76035ull },
	{ 0x39148EBD5FA61BC3ull, 0x39148EBD5FA61BC3ull },
	{ 0x24FCD14E05EF54F4ull, 0x24FCD14E05EF54F4ull },
	{ 0x3C0E5CD62DAA3967ull, 0x3C0E5CD62DAA3967ull },
	{ 0xD659DEF550A4A619ull, 0xD659DEF550A4A619ull },
	{ 0xD83BDBFB223FA19Cull, 0xD83BDBFB223FA19Cull },
	{ 0x1DEC27B6B5CC6C19ull, 0x1DEC27B6B5CC6C19ull },
	{ 0x2BA7C7C737203987ull, 0x2BA7C7C737203987ull },
	{ 0x60FEBC952FF4872Cull, 0x60FEBC952FF4872Cull },
	{ 0xF0B7328B5F4A0FD4ull, 0xF0B7328B5F4A0FD4ull },
	{ 0xDCCF9CD622246351ull, 0xDCCF9CD622246351ull },
	{ 0x35E1350067141BB2ull, 0x35E1350067141BB2ull },
	{ 0x5F2C667538E5B447ull, 0x5F2C667538E5B447ull },
	{ 0x96009E5CD32AFF54ull, 0x96009E5CD32AFF54ull },
	{ 0xFDF905FC4A353A93ull, 0xFDF905FC4A353A93ull },
	{ 0x703FA3EEC1B5C770ull, 0x703FA3EEC1B5C770ull },
	{ 0x4F60661F33DB7F7Full, 0x4F60661F33DB7F7Full },
	{ 0x9FAD9239C922A9C1ull, 0x9FAD9239C922A9C1ull },
	{ 0x21EF207BBDAD9498ull, 0x21EF207BBDAD9498ull },
	{ 0x33FE7BD8D9DBC706ull, 0x33FE7BD8D9DBC706ull },
	{ 0xB4282CDD7835D8ADull, 0xB4282CDD7835D8ADull },
	{ 0xA904E9E41CE3E903ull, 0xA904E9E41CE3E903ull },
	{ 0x6D1391A59C84B2D4ull, 0x6D1391A59C84B2D4ull },
	{ 0x9EB58C1AD41B447Dull, 0x9EB58C1AD41B447Dull },
	{ 0x13D713F6EDA95366ull, 0x13D713F6EDA95366ull },
	{ 0x1184E8913A6C3640ull, 0x1184E8913A6C3640ull },
	{ 0x87E8E70596D27600ull, 0x87E8E70596D27600ull },
	{ 0x11DF0C0263A0079Bull, 0x11DF0C0263A0079Bull },
	{ 0x78DC5D0B0C572003ull, 0x78DC5D0B0C572003ull },
	{ 0x7C9939A61D1BAAE1ull, 0x7C9939A61D1BAAE1ull },
	{ 0x9EC8961DEBA76093ull, 0x9EC8961DEBA76093ull },
	{ 0xB2CD9B2ACC847BE8ull, 0xB2CD9B2ACC847BE8ull },
	{ 0x71B9312DDBA5109Aull, 0x71B9312DDBA5109Aull },
	{ 0x51B0F0B3510D008Bull, 0x51B0F0B3510D008Bull },
	{ 0x6228FBE36B249343ull, 0x6228FBE36B249343ull },
	{ 0xEAF240D8E2A9A9AAull, 0xEAF240D8E2A9A9AAull },
	{ 0x90C6A203F3159601ull, 0x90C6A203F3159601ull },
	{ 0x3C5488D0F22D9126ull, 0x3C5488D0F22D9126ull },
	{ 0x86735E197513CF88ull, 0x86735E197513CF88ull },
	{ 0xE7D26C57094F3AE8ull, 0xE7D26C57094F3AE8ull },
	{ 0x6C8BA8DE7B1FC251ull, 0x6C8BA8DE7B1FC251ull },
	{ 0x2BDBE57AAB955559ull, 0x2BDBE57AAB955559ull },
	{ 0x4439E9CDF93F7818ull, 0x4439E9CDF93F7818ull },
	{ 0x9ADDF51134EB78E9ull, 0x9ADDF51134EB78E9ull },
	{ 0x1F5326F72B0746E0ull, 0x1F5326F72B0746E0ull },
	{ 0xD3B4520BB99EE70Aull, 0xD3B4520BB99EE70Aull },
	{ 0x57CDB4B576CA8D6Bull, 0x57CDB4B576CA8D6Bull },
	{ 0x39651A70E8434CB4ull, 0x39651A70E8434CB4ull },
	{ 0xC7C1E5CC4AF474C4ull, 0xC7C1E5CC4AF474C4ull },
	{ 0xAB56821AE7FF8062ull, 0xAB56821AE7FF8062ull },
	{ 0x81A4BBE6FE48B037ull, 0x81A4BBE6FE48B037ull },
	{ 0x3CFE83B938024091ull, 0x3CFE83B938024091ull },
	{ 0x082C96DCB1CC3DD6ull, 0x082C96DCB1CC3DD6ull },
	{ 0xEE2C90B691DEF9C6ull, 0xEE2C90B691DEF9C6ull },
	{ 0x062FE4F82F1623F4ull, 0x062FE4F82F1623F4ull },
	{ 0x62FD4062C7527E06ull, 0x62FD4062C7527E06ull },
	{ 0x86FDD323C80833ACull, 0x86FDD323C80833ACull },
	{ 0x9BA80140A3AFCD5Dull, 0x9BA80140A3AFCD5Dull },
	{ 0xADD0FD1938EFC8D9ull, 0xADD0FD1938EFC8D9ull },
	{ 0xACD6C7F7CA74CE5Aull, 0xACD6C7F7CA74CE5Aull },
	{ 0x6E9C1C75858E879Cull, 0x6E9C1C75858E879Cull },
	{ 0xF747BF6A98C71410ull, 0xF747BF6A98C71410ull },
	{ 0x4510B9B0DB8B205Bull, 0x4510B9B0DB8B205Bull },
	{ 0xE008F97004989A95ull, 0xE008F97004989A95ull },
	{ 0x6D9FDBDA718A0B3Full, 0x6D9FDBDA718A0B3Full },
	{ 0x086E1E517D348804ull, 0x086E1E517D348804ull },
	{ 0x1C64B83866EE4C34ull, 0x1C64B83866EE4C34ull },
	{ 0x85F01C8CC9F3ECFDull, 0x85F01C8CC9F3ECFDull },
	{ 0xB64962CBA402306Bull, 0xB64962CBA402306Bull },
	{ 0x9A86D959EC5A82A1ull, 0x9A86D959EC5A82A1ull },
	{ 0x08157E17BA880E59ull, 0x08157E17BA880E59ull },
	{ 0xEC1CE074FE1B6E19ull, 0xEC1CE074FE1B6E19ull },
	{ 0xFF26EACEC4F307B7ull, 0xFF26EACEC4F307B7ull },
	{ 0x3D0D13EC1205FD0Eull, 0x3D0D13EC1205FD0Eull },
	{ 0x60B478814647C781ull, 0x60B478814647C781ull },
	{ 0xA3DA5CB1149A68B0ull, 0xA3DA5CB1149A68B0ull },
	{ 0x029A95320B654379ull, 0x029A95320B654379ull },
	{ 0xC361598FAA4691B2ull, 0xC361598FAA4691B2ull },
	{ 0x82B1FD0E09F579A4ull, 0x82B1FD0E09F579A4ull },
	{ 0x82B41067118AC54Aull, 0x82B41067118AC54Aull },
	{ 0x7F206B283FC634CAull, 0x7F206B283FC634CAull },
	{ 0x450F9D6800C678B3ull, 0x450F9D6800C678B3ull },
	{ 0x4E6349199509C6F8ull, 0x4E6349199509C6F8ull },
	{ 0x220BE336886D0D2Dull, 0x220BE336886D0D2Dull },
	{ 0x37245E44B9FFCD00ull, 0x37245E44B9FFCD00ull },
	{ 0x1D653AEFF1628CF9ull, 0x1D653AEFF1628CF9ull },
	{ 0xA85881BE0785EF75ull, 0xA85881BE0785EF75ull },
	{ 0xDF4024749D006AE0ull, 0xDF4024749D006AE0ull },
	{ 0x711847F760D09E32ull, 0x711847F760D09E32ull },
	{ 0xEBE6F595E742462Bull, 0xEBE6F595E742462Bull },
	{ 0xF8E2B490114BD552ull, 0xF8E2B490114BD552ull },
	{ 0xDBE18B48035AA72Aull, 0xDBE18B48035AA72Aull },
	{ 0x7769E968AD23D9B7ull, 0x7769E968AD23D9B7ull },
	{ 0x9406F8B7DEAFBFB0ull, 0x9406F8B7DEAFBFB0ull },
	{ 0x2C89D6FAF1B5FA05ull, 0x2C89D6FAF1B5FA05ull },
	{ 0xFD593744FEC02C13ull, 0xFD593744FEC02C13ull },
	{ 0x9878EC15A5E950A5ull, 0x9878EC15A5E950A5ull },
	{ 0x3D2AD79F636FB22Eull, 0x3D2AD79F636FB22Eull },
	{ 0x33B2C63C78EF7FC5ull, 0x33B2C63C78EF7FC5ull },
	{ 0xED0EAFC963BC0261ull, 0xED0EAFC963BC0261ull },
	{ 0xA6DCE14BE32D74D5ull, 0xA6DCE14BE32D74D5ull },
	{ 0x52C18D9D90D9770Eull, 0x52C18D9D90D9770Eull },
	{ 0x82C547A1064C268Aull, 0x82C547A1064C268Aull },
	{ 0x187DE43894F7D41Bull, 0x187DE43894F7D41Bull },
	{ 0xF8DB42658CB157DDull, 0xF8DB42658CB157DDull },
	{ 0x04597E0712EF0875ull, 0x04597E0712EF0875ull },
	{ 0x924208FF89E0518Bull, 0x924208FF89E0518Bull },
	{ 0xB0FD2BD786D3EA89ull, 0xB0FD2BD786D3EA89ull },
	{ 0x68FD70E55A02AA94ull, 0x68FD70E55A02AA94ull },
	{ 0xC52674B02574FECAull, 0xC52674B02574FECAull },
	{ 0x5EA89F358319013Eull, 0x5EA89F358319013Eull },
	{ 0x2C034608BCFF2FB1ull, 0x2C034608BCFF2FB1ull },
	{ 0xD596B6329353BF11ull, 0xD596B6329353BF11ull },
	{ 0x4CD7A468940A9436ull, 0x4CD7A468940A9436ull },
	{ 0x683D87A30213D75Aull, 0x683D87A30213D75Aull },
	{ 0x431D5A484B0F2C5Aull, 0x431D5A484B0F2C5Aull },
	{ 0xA13A4395101386ABull, 0xA13A4395101386ABull },
	{ 0x96BDA8F7BAC1A21Eull, 0x96BDA8F7BAC1A21Eull },
	{ 0x33802826A6771557ull, 0x33802826A6771557ull },
	{ 0x73A4F7E7D5180A0Bull, 0x73A4F7E7D5180A0Bull },
	{ 0xB251325137C59379ull, 0xB251325137C59379ull },
	{ 0xE23085988536A22Aull, 0xE23085988536A22Aull },
	{ 0x48F0CDB99FC99CECull, 0x48F0CDB99FC99CECull },
	{ 0x7F06164A8E84928Bull, 0x7F06164A8E84928Bull },
	{ 0x7330840402DA1652ull, 0x7330840402DA1652ull },
	{ 0x32D5A5956EE8ADC1ull, 0x32D5A5956EE8ADC1ull },
	{ 0xB506ADCBAE0AEE8Full, 0xB506ADCBAE0AEE8Full },
	{ 0x6AB514C13BAE2FE2ull, 0x6AB514C13BAE2FE2ull },
	{ 0xD27F21DEF845E04Dull, 0xD27F21DEF845E04Dull },
	{ 0x4FF5DC4483917E8Aull, 0x4FF5DC4483917E8Aull },
	{ 0x8805E5BF20CB2DF8ull, 0x8805E5BF20CB2DF8ull },
	{ 0x8034BAC62F74EAD6ull, 0x8034BAC62F74EAD6ull },
	{ 0x427BDD44305A63C1ull, 0x427BDD44305A63C1ull },
	{ 0x451365435F7D6DEBull, 0x451365435F7D6DEBull },
	{ 0xD6857AE4D8031A1Dull, 0xD6857AE4D8031A1Dull },
	{ 0x4CDC37B94DD76CEFull, 0x4CDC37B94DD76CEFull },
	{ 0x39936FA2529DB1C5ull, 0x39936FA2529DB1C5ull },
	{ 0x61AFE3F32DE3750Cull, 0x61AFE3F32DE3750Cull },
	{ 0xB8F55AD843AC1635ull, 0xB8F55AD843AC1635ull },
	{ 0xA21D22BC0B0AFFF2ull, 0xA21D22BC0B0AFFF2ull },
	{ 0xB2567A7097227C30ull, 0xB2567A7097227C30ull },
	{ 0x39026354AAFD2DD4ull, 0x39026354AAFD2DD4ull },
	{ 0xCBA45ADB62982339ull, 0xCBA45ADB62982339ull },
	{ 0x95797B6D6A938CE8ull, 0x95797B6D6A938CE8ull },
	{ 0xAB1F86EAC4E01419ull, 0xAB1F86EAC4E01419ull },
	{ 0xA2AD572421DED1EAull, 0xA2AD572421DED1EAull },
	{ 0xAF263812C549E25Dull, 0xAF263812C549E25Dull },
	{ 0x8049A926001B9D97ull, 0x8049A926001B9D97ull },
	{ 0xA669066D344A6B57ull, 0xA669066D344A6B57ull },
	{ 0x3A049A956E8F3A34ull, 0x3A049A956E8F3A34ull },
	{ 0x87D998218B8C959Bull, 0x87D998218B8C959Bull },
	{ 0x3D99F7A62DC383CFull, 0x3D99F7A62DC383CFull },
	{ 0xA9386695098680CFull, 0xA9386695098680CFull },
	{ 0x3887FE8FF0412D03ull, 0x3887FE8FF0412D03ull },
	{ 0x0432B2460F37539Dull, 0x0432B2460F37539Dull },
	{ 0x948B7612C855FECCull, 0x948B7612C855FECCull },
	{ 0x5623837A1EA0F74Dull, 0x5623837A1EA0F74Dull },
	{ 0x430564826F7061B2ull, 0x430564826F7061B2ull },
	{ 0x7F1948767F3C7D71ull, 0x7F1948767F3C7D71ull },
	{ 0xA8FE1D2AD315812Full, 0xA8FE1D2AD315812Full },
	{ 0x1EAAF4915D459ACEull, 0x1EAAF4915D459ACEull },
	{ 0x76642F3676FB0B34ull, 0x76642F3676FB0B34ull },
	{ 0x6B77EFB22B3865DCull, 0x6B77EFB22B3865DCull },
	{ 0xFEAD56A8D2A8EEA2ull, 0xFEAD56A8D2A8EEA2ull },
	{ 0x0B7A0605FE9716CBull, 0x0B7A0605FE9716CBull },
	{ 0x10696C2CB3A31CD3ull, 0x10696C2CB3A31CD3ull },
	{ 0xEBE95827F3F7C87Aull, 0xEBE95827F3F7C87Aull },
	{ 0x4D5754B69BDB4991ull, 0x4D5754B69BDB4991ull },
	{ 0x99424B9D8B5A3680ull, 0x99424B9D8B5A3680ull },
	{ 0x82ABF19B226AAA9Bull, 0x82ABF19B226AAA9Bull },
	{ 0x973D79C70A8580A6ull, 0x973D79C70A8580A6ull },
	{ 0x009B10F95353B6A5ull, 0x009B10F95353B6A5ull },
	{ 0xB8B9A103DC2DE6E9ull, 0xB8B9A103DC2DE6E9ull },
	{ 0x72D25D390FCBE7D7ull, 0x72D25D390FCBE7D7ull },
	{ 0x7807942AEFE28A7Aull, 0x7807942AEFE28A7Aull },
	{ 0xB8264D5D6E0D78CBull, 0xB8264D5D6E0D78CBull },
	{ 0xCE982A7719C3313Full, 0xCE982A7719C3313Full },
	{ 0x3AB114CB19312711ull, 0x3AB114CB19312711ull },
	{ 0x8D0C2F67743E00D1ull, 0x8D0C2F67743E00D1ull },
	{ 0x47C393A2B08FF32Dull, 0x47C393A2B08FF32Dull },
	{ 0x33765393BD15427Eull, 0x33765393BD15427Eull },
	{ 0x67E8464F2A6CDD95ull, 0x67E8464F2A6CDD95ull },
	{ 0x48FF1B74B86D1FC7ull, 0x48FF1B74B86D1FC7ull },
	{ 0xB70835E83026AA36ull, 0xB70835E83026AA36ull },
	{ 0xB40AD0408EA16561ull, 0xB40AD0408EA16561ull },
	{ 0x3C21E7C880B19DEEull, 0x3C21E7C880B19DEEull },
	{ 0x92616FFB2D6341B6ull, 0x92616FFB2D6341B6ull },
	{ 0x41A24DF850994011ull, 0x41A24DF850994011ull },
	{ 0x91189C9AA71F05E3ull, 0x91189C9AA71F05E3ull },
	{ 0xAD1FB6EBA8CA1B67ull, 0xAD1FB6EBA8CA1B67ull },
	{ 0xC42F8CDF93E9E3BAull, 0xC42F8CDF93E9E3BAull },
	{ 0xD756BDB4C76379AAull, 0xD756BDB4C76379AAull },
	{ 0x7F3CFC016FB7AFBFull, 0x7F3CFC016FB7AFBFull },
	{ 0x647939AA7F91298Aull, 0x647939AA7F91298Aull },
	{ 0x2CDF1A0EFC58A02Dull, 0x2CDF1A0EFC58A02Dull },
	{ 0x1BCF2EC2992D3A2Bull, 0x1BCF2EC2992D3A2Bull },
	{ 0x271D7AE2A0BF0DF0ull, 0x271D7AE2A0BF0DF0ull },
	{ 0x3FE9BDC03169BCA3ull, 0x3FE9BDC03169BCA3ull },
	{ 0x0F2D076AC80D32ABull, 0x0F2D076AC80D32ABull },
	{ 0xE6A813761A82847Full, 0xE6A813761A82847Full },
	{ 0x2585ECC6432AEBB1ull, 0x2585ECC6432AEBB1ull },
	{ 0x512E595D3B76E73Dull, 0x512E595D3B76E73Dull },
	{ 0x21DB8CF4A763770Eull, 0x21DB8CF4A763770Eull },
	{ 0xC3AEA55EF04B7C51ull, 0xC3AEA55EF04B7C51ull },
	{ 0x8B166DDAAF8B357Dull, 0x8B166DDAAF8B357Dull },
	{ 0x5DD3EA136A13DD22ull, 0x5DD3EA136A13DD22ull },
	{ 0x0F702A3A9B2D68F1ull, 0x0F702A3A9B2D68F1ull },
	{ 0x727EE6540F04650Cull, 0x727EE6540F04650Cull },
	{ 0x5C8161808EC1F60Full, 0x5C8161808EC1F60Full },
	{ 0xDFA515C1DA4BF25Eull, 0xDFA515C1DA4BF25Eull },
	{ 0x76D1A762F42B2330ull, 0x76D1A762F42B2330ull },
	{ 0x66C1A05FBBF5A35Cull, 0x66C1A05FBBF5A35Cull },
	{ 0x12797416D9BB5962ull, 0x12797416D9BB5962ull },
	{ 0x04584A80F8D299BCull, 0x04584A80F8D299BCull },
	{ 0x6A9614385302E266ull, 0x6A9614385302E266ull },
	{ 0x484A654B8B6BB8AAull, 0x484A654B8B6BB8AAull },
	{ 0x19BF9926D8E5AA31ull, 0x19BF9926D8E5AA31ull },
	{ 0xAD8DBE0E827B66C6ull, 0xAD8DBE0E827B66C6ull },
	{ 0x7160C895A8F900EDull, 0x7160C895A8F900EDull },
	{ 0x72958EEC622F4A23ull, 0x72958EEC622F4A23ull },
	{ 0x52D10CD025295576ull, 0x52D10CD025295576ull },
	{ 0x70FD6E7983B20D49ull, 0x70FD6E7983B20D49ull },
	{ 0x8600647235E7FA60ull, 0x8600647235E7FA60ull },
	{ 0x75863F4E753D1B32ull, 0x75863F4E753D1B32ull },
	{ 0x128552B4A760E4EEull, 0x128552B4A760E4EEull },
	{ 0x1855212E59B260CEull, 0x1855212E59B260CEull },
	{ 0x971CCF8D8B386A9Bull, 0x971CCF8D8B386A9Bull },
	{ 0x14B4602148FE0E00ull, 0x14B4602148FE0E00ull },
	{ 0x219C36E498110493ull, 0x219C36E498110493ull },
	{ 0xA1366482B08054A0ull, 0xA1366482B08054A0ull },
	{ 0x073539DB89590DB8ull, 0x073539DB89590DB8ull },
	{ 0x47CAA363B2BF27FAull, 0x47CAA363B2BF27FAull },
	{ 0xBC9BCD4A8020811Full, 0xBC9BCD4A8020811Full },
	{ 0xBD748E21859FFC4Bull, 0xBD748E21859FFC4Bull },
	{ 0xAD3E1B106176BE13ull, 0xAD3E1B106176BE13ull },
	{ 0xBA8482EFD6698AC9ull, 0xBA8482EFD6698AC9ull },
	{ 0x943E71973810448Full, 0x943E71973810448Full },
	{ 0xD292BC17258D8B9Aull, 0xD292BC17258D8B9Aull },
	{ 0x53CF334C93F477C6ull, 0x53CF334C93F477C6ull },
	{ 0xCC8C4087297A7ADAull, 0xCC8C4087297A7ADAull },
	{ 0x4D40C7ADE9335B9Cull, 0x4D40C7ADE9335B9Cull },
	{ 0xFA61D3FD2CF55598ull, 0xFA61D3FD2CF55598ull },
	{ 0xB9718A68F1A3562Cull, 0xB9718A68F1A3562Cull },
	{ 0x1198E63E707B5B57ull, 0x1198E63E707B5B57ull },
	{ 0x805FBBB00BA6F0C3ull, 0x805FBBB00BA6F0C3ull },
	{ 0x829E83C688ABCD90ull, 0x829E83C688ABCD90ull },
	{ 0x911D70229B3A9561ull, 0x911D70229B3A9561ull },
	{ 0x514C45C9BFF9B02Cull, 0x514C45C9BFF9B02Cull },
	{ 0xBA268129684A4FBDull, 0xBA268129684A4FBDull },
	{ 0x6CCF418B7565880Cull, 0x6CCF418B7565880Cull },
	{ 0x01487C67014F8FC0ull, 0x01487C67014F8FC0ull },
	{ 0x8760F9087CFF1C82ull, 0x8760F9087CFF1C82ull },
	{ 0xFB5C943C1876E487ull, 0xFB5C943C1876E487ull },
	{ 0xEEA24AF57FEF37D8ull, 0xEEA24AF57FEF37D8ull },
	{ 0x24B38F1FE50DC163ull, 0x24B38F1FE50DC163ull },
	{ 0x638862115C0964C6ull, 0x638862115C0964C6ull },
	{ 0x38B3B88C84383A5Aull, 0x38B3B88C84383A5Aull },
	{ 0x71FBA5EB84E5B07Eull, 0x71FBA5EB84E5B07Eull },
	{ 0x8A18D01F572EC625ull, 0x8A18D01F572EC625ull },
	{ 0xC968AA9E62026AA2ull, 0xC968AA9E62026AA2ull },
	{ 0x5AD5FA0D6483FCB2ull, 0x5AD5FA0D6483FCB2ull },
	{ 0x1463207DB12DE54Aull, 0x1463207DB12DE54Aull },
	{ 0x3419AF4BB83258A2ull, 0x3419AF4BB83258A2ull },
	{ 0x50BAC478E3B83301ull, 0x50BAC478E3B83301ull },
	{ 0x3E63ACCBC271F6CBull, 0x3E63ACCBC271F6CBull },
	{ 0x27925031816CEA47ull, 0x27925031816CEA47ull },
	{ 0x997CE46BB8965535ull, 0x997CE46BB8965535ull },
	{ 0xC538698585D4E10Full, 0xC538698585D4E10Full },
	{ 0xB4316C9AF634149Eull, 0xB4316C9AF634149Eull },
	{ 0xB132932DC27FF240ull, 0xB132932DC27FF240ull },
	{ 0x24DC6FB259CEB777ull, 0x24DC6FB259CEB777ull },
	{ 0xA67A1BD03CD6B8B5ull, 0xA67A1BD03CD6B8B5ull },
	{ 0x80D61D00A88E6167ull, 0x80D61D00A88E6167ull },
	{ 0x6B0B0F2690F4513Full, 0x6B0B0F2690F4513Full },
	{ 0x34E3988AEC3D99C6ull, 0x34E3988AEC3D99C6ull },
	{ 0x283525384E081C20ull, 0x283525384E081C20ull },
	{ 0xDD08EA40D2CF08A1ull, 0xDD08EA40D2CF08A1ull },
	{ 0xC2AEB22959BF7A90ull, 0xC2AEB22959BF7A90ull },
	{ 0x9C5CEF25106B38BAull, 0x9C5CEF25106B38BAull },
	{ 0xB229977C4DF7903Aull, 0xB229977C4DF7903Aull },
	{ 0x756F4849BD465607ull, 0x756F4849BD465607ull },
	{ 0xD9CA3208D9B7023Eull, 0xD9CA3208D9B7023Eull },
	{ 0xA1E04B40B77DEF30ull, 0xA1E04B40B77DEF30ull },
	{ 0xDFF04C4620EFB7AFull, 0xDFF04C4620EFB7AFull },
	{ 0x6ABAF2A000FEB494ull, 0x6ABAF2A000FEB494ull },
	{ 0x86320CA91D089A93ull, 0x86320CA91D089A93ull },
	{ 0x0030CCFA21B23943ull, 0x0030CCFA21B23943ull },
	{ 0x8158E5F6FC2F56E8ull, 0x8158E5F6FC2F56E8ull },
	{ 0x51A7ED6A8674B083ull, 0x51A7ED6A8674B083ull },
	{ 0x517087CB8172181Cull, 0x517087CB8172181Cull },
	{ 0x65EADBD5E052B1CDull, 0x65EADBD5E052B1CDull },
	{ 0x99F7CE1364614F92ull, 0x99F7CE1364614F92ull },
	{ 0xB2EA847E864EF4CFull, 0xB2EA847E864EF4CFull },
	{ 0xF953B15AB2B96E21ull, 0xF953B15AB2B96E21ull },
	{ 0x75D227ECD3F014E9ull, 0x75D227ECD3F014E9ull },
	{ 0xE184368F2E14A716ull, 0xE184368F2E14A716ull },
	{ 0x0B1D4B6B89964D33ull, 0x0B1D4B6B89964D33ull },
	{ 0x6F8B5F56AE24B030ull, 0x6F8B5F56AE24B030ull },
	{ 0xC68FA8F82D193D73ull, 0xC68FA8F82D193D73ull },
	{ 0xC3A619797E7480A1ull, 0xC3A619797E7480A1ull },
	{ 0x4AFF1E87692DA529ull, 0x4AFF1E87692DA529ull },
	{ 0x59A72F60C0721192ull, 0x59A72F60C0721192ull },
	{ 0x62BF9F1D6CD32706ull, 0x62BF9F1D6CD32706ull },
	{ 0xE0243848C9C01E9Eull, 0xE0243848C9C01E9Eull },
	{ 0xD731219A4D4A07EEull, 0xD731219A4D4A07EEull },
	{ 0x1164878785CFE5D3ull, 0x1164878785CFE5D3ull },
	{ 0xC92F91DA63D0EDFCull, 0xC92F91DA63D0EDFCull },
	{ 0x0AA21D46D502E5B6ull, 0x0AA21D46D502E5B6ull },
	{ 0x0884C02AAF2AA17Cull, 0x0884C02AAF2AA17Cull },
	{ 0xA48C95D2EBD4A03Eull, 0xA48C95D2EBD4A03Eull },
	{ 0xEC7C515132EBCCF0ull, 0xEC7C515132EBCCF0ull },
	{ 0x4ADE1A1CB4CC58D6ull, 0x4ADE1A1CB4CC58D6ull },
	{ 0x2B89AE895750BC8Eull, 0x2B89AE895750BC8Eull },
	{ 0xD4A24FD02EF6D4DDull, 0xD4A24FD02EF6D4DDull },
	{ 0xB35FF5744A5B1F3Eull, 0xB35FF5744A5B1F3Eull },
	{ 0xDC5F48BECE281B28ull, 0xDC5F48BECE281B28ull },
	{ 0x80EBE85D5EB7C51Eull, 0x80EBE85D5EB7C51Eull },
	{ 0x9780737DDB85367Cull, 0x9780737DDB85367Cull },
	{ 0xF87FDDF26BAC6100ull, 0xF87FDDF26BAC6100ull },
	{ 0xAEF38AF8D595F117ull, 0xAEF38AF8D595F117ull },
	{ 0x3D71B1702D8D4769ull, 0x3D71B1702D8D4769ull },
	{ 0x16C786B7CEDE9926ull, 0x16C786B7CEDE9926ull },
	{ 0x8A9AB2A2A6FB2D92ull, 0x8A9AB2A2A6FB2D92ull },
	{ 0x664D8AB9CFC1BDCEull, 0x664D8AB9CFC1BDCEull },
	{ 0x09C995D97D68EDE8ull, 0x09C995D97D68EDE8ull },
	{ 0xB02BD76335A9F025ull, 0xB02BD76335A9F025ull },
	{ 0xAC5663905D29408Eull, 0xAC5663905D29408Eull },
	{ 0xDA12B9E011FB8438ull, 0xDA12B9E011FB8438ull },
	{ 0x8298694B56448458ull, 0x8298694B56448458ull },
	{ 0xF5437B9A8C0E4F06ull, 0xF5437B9A8C0E4F06ull },
	{ 0x945E28873F1675CFull, 0x945E28873F1675CFull },
	{ 0x15F95245406F6801ull, 0x15F95245406F6801ull },
	{ 0x389747DE1173F5B2ull, 0x389747DE1173F5B2ull },
	{ 0x2BD450D99F6AE912ull, 0x2BD450D99F6AE912ull },
	{ 0x6D70819F4D58C7F1ull, 0x6D70819F4D58C7F1ull },
	{ 0xD9B655EA4FDDAD93ull, 0xD9B655EA4FDDAD93ull },
	{ 0x3061951C575D4EB2ull, 0x3061951C575D4EB2ull },
	{ 0xCD91FD298C4F12D4ull, 0xCD91FD298C4F12D4ull },
	{ 0x563D3648D934524Full, 0x563D3648D934524Full },
	{ 0x18EB8E638512B799ull, 0x18EB8E638512B799ull },
	{ 0x62C2FD2F20F0BA54ull, 0x62C2FD2F20F0BA54ull },
	{ 0x883BEE6CBF723E18ull, 0x883BEE6CBF723E18ull },
	{ 0xE15356B882935103ull, 0xE15356B882935103ull },
	{ 0xFB458DE4751F09FEull, 0xFB458DE4751F09FEull },
	{ 0xBC6573C9B6A083D1ull, 0xBC6573C9B6A083D1ull },
	{ 0xBD5B5A8AF5ABFFE9ull, 0xBD5B5A8AF5ABFFE9ull },
	{ 0x959F3A933AFE1FE8ull, 0x959F3A933AFE1FE8ull },
	{ 0xDC99AE2C048F476Eull, 0xDC99AE2C048F476Eull },
	{ 0x9DEF3515B19916A5ull, 0x9DEF3515B19916A5ull },
	{ 0x82A13F7D3BAC0A12ull, 0x82A13F7D3BAC0A12ull },
	{ 0xED9881FA4293399Aull, 0xED9881FA4293399Aull },
	{ 0x4C64111068643452ull, 0x4C64111068643452ull },
	{ 0xDC10C49EFA78FE8Cull, 0xDC10C49EFA78FE8Cull },
	{ 0x21E462D9EDF2321Eull, 0x21E462D9EDF2321Eull },
	{ 0x580014B72ABF3AF3ull, 0x580014B72ABF3AF3ull },
	{ 0x0E75A08B24CAB3D7ull, 0x0E75A08B24CAB3D7ull },
	{ 0xBF72EE5FC988E6FFull, 0xBF72EE5FC988E6FFull },
	{ 0x98BF52A0B9866A57ull, 0x98BF52A0B9866A57ull },
	{ 0xB7AE2267D374DD8Full, 0xB7AE2267D374DD8Full },
	{ 0x9A720E450ED3C967ull, 0x9A720E450ED3C967ull },
	{ 0x04E4C62CA8739B3Aull, 0x04E4C62CA8739B3Aull },
	{ 0x98DE64E69B70C648ull, 0x98DE64E69B70C648ull },
	{ 0x6300234A6E24FE48ull, 0x6300234A6E24FE48ull },
	{ 0x9C1DD4624B6C98AEull, 0x9C1DD4624B6C98AEull },
	{ 0xDCF9A8006F3E0738ull, 0xDCF9A8006F3E0738ull },
	{ 0x87EDB741DFF179BEull, 0x87EDB741DFF179BEull },
	{ 0x24F6D7297B36983Aull, 0x24F6D7297B36983Aull },
	{ 0xF30C348E4F166495ull, 0xF30C348E4F166495ull },
	{ 0x6C6737C846BF3634ull, 0x6C6737C846BF3634ull },
	{ 0xFF972FD623F8D7C2ull, 0xFF972FD623F8D7C2ull },
	{ 0xBC1445C7CB7EC73Full, 0xBC1445C7CB7EC73Full },
	{ 0xB2FD38C7DE9E91C9ull, 0xB2FD38C7DE9E91C9ull },
	{ 0x4BE3614FA7D853A9ull, 0x4BE3614FA7D853A9ull },
	{ 0x4DB07C20541F54A0ull, 0x4DB07C20541F54A0ull },
	{ 0x5B19BF0C7FB8C673ull, 0x5B19BF0C7FB8C673ull },
	{ 0xA39BB2ED0ED6A334ull, 0xA39BB2ED0ED6A334ull },
	{ 0x0812584C5B6CAB52ull, 0x0812584C5B6CAB52ull },
	{ 0xF3C91282AD9D8363ull, 0xF3C91282AD9D8363ull },
	{ 0xA8A5DAEF08BDF130ull, 0xA8A5DAEF08BDF130ull },
	{ 0x409B3CC7444C3B81ull, 0x409B3CC7444C3B81ull },
	{ 0xA1B498EA79D9A82Aull, 0xA1B498EA79D9A82Aull },
	{ 0xA7983070BC8416D5ull, 0xA7983070BC8416D5ull },
	{ 0x3F82792326FDC311ull, 0x3F82792326FDC311ull },
	{ 0x39A6387734011F9Bull, 0x39A6387734011F9Bull },
	{ 0x0D74E6D86C2ED66Bull, 0x0D74E6D86C2ED66Bull },
	{ 0x52CADC1B5E33E382ull, 0x52CADC1B5E33E382ull },
	{ 0x2E52C1A4EB75D7F2ull, 0x2E52C1A4EB75D7F2ull },
	{ 0xABAF5B56D019382Cull, 0xABAF5B56D019382Cull },
	{ 0x8A3603FA4C478C7Cull, 0x8A3603FA4C478C7Cull },
	{ 0x9E00F59FDF0491DCull, 0x9E00F59FDF0491DCull },
	{ 0xBCCD24700AB7CF14ull, 0xBCCD24700AB7CF14ull },
	{ 0x02D47BB1C808897Eull, 0x02D47BB1C808897Eull },
	{ 0xD662CD28E8FF6DEFull, 0xD662CD28E8FF6DEFull },
	{ 0xBDA5C8724A3564E9ull, 0xBDA5C8724A3564E9ull },
	{ 0x4A277B00D83D7E7Full, 0x4A277B00D83D7E7Full },
	{ 0x573AD74E60D7FE07ull, 0x573AD74E60D7FE07ull },
	{ 0x5C99C3E336DC8B86ull, 0x5C99C3E336DC8B86ull },
	{ 0x958C3407A5297FD8ull, 0x958C3407A5297FD8ull },
	{ 0xBB37C5E230C6519Eull, 0xBB37C5E230C6519Eull },
	{ 0x13DF6B15ACC726ECull, 0x13DF6B15ACC726ECull },
	{ 0x950297621DDF7E33ull, 0x950297621DDF7E33ull },
	{ 0xDAC3793B6B3201EBull, 0xDAC3793B6B3201EBull },
	{ 0xB213B7B8E34B7105ull, 0xB213B7B8E34B7105ull },
	{ 0xF7FF3BB08F56B20Full, 0xF7FF3BB08F56B20Full },
	{ 0x0BE47E4153F7FCA8ull, 0x0BE47E4153F7FCA8ull },
	{ 0x9691E0BEE69D2A27ull, 0x9691E0BEE69D2A27ull },
	{ 0x65078B628EF12406ull, 0x65078B628EF12406ull },
	{ 0x16D55653BA806F94ull, 0x16D55653BA806F94ull },
	{ 0x6154378B607CDE5Aull, 0x6154378B607CDE5Aull },
	{ 0xA632BB65326E594Dull, 0xA632BB65326E594Dull },
	{ 0x9E1403C1A1BED235ull, 0x9E1403C1A1BED235ull },
	{ 0x5798CF8BB16F4FDDull, 0x5798CF8BB16F4FDDull },
	{ 0xC9A209E599641AFBull, 0xC9A209E599641AFBull },
	{ 0xBAD4624F39160DE1ull, 0xBAD4624F39160DE1ull },
	{ 0xBBAC6BC95AD70365ull, 0xBBAC6BC95AD70365ull },
	{ 0x755AC448C6F7608Dull, 0x755AC448C6F7608Dull },
	{ 0x8CE7C45C263AC97Eull, 0x8CE7C45C263AC97Eull },
	{ 0xD9963657B5A9E7A6ull, 0xD9963657B5A9E7A6ull },
	{ 0x2FB49669C8EF5DA8ull, 0x2FB49669C8EF5DA8ull },
	{ 0xFF6459AB1D4DD308ull, 0xFF6459AB1D4DD308ull },
	{ 0x0766BDADFD5D2216ull, 0x0766BDADFD5D2216ull },
	{ 0xDBB6A03F32DCA61Full, 0xDBB6A03F32DCA61Full },
	{ 0x009EDE5BCA88AA52ull, 0x009EDE5BCA88AA52ull },
	{ 0xD8E4BC4EA7C0F5EFull, 0xD8E4BC4EA7C0F5EFull },
	{ 0x2935D4F8E5AE7C37ull, 0x2935D4F8E5AE7C37ull },
	{ 0xEE6110F80B76288Full, 0xEE6110F80B76288Full },
	{ 0xC86A268D0F26CFF5ull, 0xC86A268D0F26CFF5ull },
	{ 0xB90F213728F03828ull, 0xB90F213728F03828ull },
	{ 0xF69F70C575914E92ull, 0xF69F70C575914E92ull },
	{ 0x92CC013269CB6F78ull, 0x92CC013269CB6F78ull },
	{ 0x355FD9CC1488D5E5ull, 0x355FD9CC1488D5E5ull },
	{ 0xAAF8C53C33842A53ull, 0xAAF8C53C33842A53ull },
	{ 0x07AF8053145F319Bull, 0x07AF8053145F319Bull },
	{ 0x9E06F2084936989Full, 0x9E06F2084936989Full },
	{ 0x09A1EB342A20E3B5ull, 0x09A1EB342A20E3B5ull },
	{ 0x0DE6ED352A267B55ull, 0x0DE6ED352A267B55ull },
	{ 0x7D54F6D8F4D01644ull, 0x7D54F6D8F4D01644ull },
	{ 0x30300D32B10C7F20ull, 0x30300D32B10C7F20ull },
	{ 0x11E0F8B0D97BDC5Aull, 0x11E0F8B0D97BDC5Aull },
	{ 0xC370914EAAF8F6E9ull, 0xC370914EAAF8F6E9ull },
	{ 0xB26A0384A52B8820ull, 0xB26A0384A52B8820ull },
	{ 0x009062B32180B26Cull, 0x009062B32180B26Cull },
	{ 0x6FCD21D779E4114Bull, 0x6FCD21D779E4114Bull },
	{ 0x1198F522BF2F760Full, 0x1198F522BF2F760Full },
	{ 0xFB389C5901A73954ull, 0xFB389C5901A73954ull },
	{ 0xBCDF12D026F9313Eull, 0xBCDF12D026F9313Eull },
	{ 0xAC689FEA73AA3150ull, 0xAC689FEA73AA3150ull },
	{ 0x52E1192EC8088908ull, 0x52E1192EC8088908ull },
	{ 0xFEA9128B61DFA371ull, 0xFEA9128B61DFA371ull },
	{ 0xBC4FCD4F64B4A0F2ull, 0xBC4FCD4F64B4A0F2ull },
	{ 0x9152D7196AD05835ull, 0x9152D7196AD05835ull },
	{ 0xA19B26D4D5C77C39ull, 0xA19B26D4D5C77C39ull },
	{ 0x78EA63162B8860B8ull, 0x78EA63162B8860B8ull },
	{ 0xB925CEA585D4613Aull, 0xB925CEA585D4613Aull },
	{ 0x6BE45ED6122AE77Aull, 0x6BE45ED6122AE77Aull },
	{ 0x37DD78799F4C0FF8ull, 0x37DD78799F4C0FF8ull },
	{ 0x3B2DD54F3E18F0DEull, 0x3B2DD54F3E18F0DEull },
	{ 0xC8A0123BEE378098ull, 0xC8A0123BEE378098ull },
	{ 0x49EE63AC5F001BADull, 0x49EE63AC5F001BADull },
	{ 0x84CAF46EBE9AA8E3ull, 0x84CAF46EBE9AA8E3ull },
	{ 0x056469C397116093ull, 0x056469C397116093ull },
	{ 0x6BB41234BEFF3549ull, 0x6BB41234BEFF3549ull },
	{ 0xDD5B39F157DFD320ull, 0xDD5B39F157DFD320ull },
	{ 0xDB3D293DDD674D5Bull, 0xDB3D293DDD674D5Bull },
	{ 0xBB8B48F744A75EC6ull, 0xBB8B48F744A75EC6ull },
	{ 0x0740F1382A28534Eull, 0x0740F1382A28534Eull },
	{ 0xFC245E1F11BB8B5Full, 0xFC245E1F11BB8B5Full },
	{ 0xF22A4D04B56DA2FDull, 0xF22A4D04B56DA2FDull },
	{ 0xE581C301641E3826ull, 0xE581C301641E3826ull },
	{ 0x6455799071235166ull, 0x6455799071235166ull },
	{ 0xA0487450B27E60BAull, 0xA0487450B27E60BAull },
	{ 0x648B3E711008EE2Aull, 0x648B3E711008EE2Aull },
	{ 0xAF4F5068239EDA21ull, 0xAF4F5068239EDA21ull },
	{ 0xAFB61D85E62C440Full, 0xAFB61D85E62C440Full },
	{ 0xE7A1491E70251241ull, 0xE7A1491E70251241ull },
	{ 0x313183BCC7CC167Full, 0x313183BCC7CC167Full },
	{ 0x68C991BCC0CFD266ull, 0x68C991BCC0CFD266ull },
	{ 0xFA72F7BAEAA2E188ull, 0xFA72F7BAEAA2E188ull },
	{ 0x9439D0B99015FCC1ull, 0x9439D0B99015FCC1ull },
	{ 0xBB3D4B29C5D99EDCull, 0xBB3D4B29C5D99EDCull },
	{ 0x05DA4EF7896D4E66ull, 0x05DA4EF7896D4E66ull },
	{ 0xB97DCA47F0A27F51ull, 0xB97DCA47F0A27F51ull },
	{ 0x8BD427A868791D4Bull, 0x8BD427A868791D4Bull },
	{ 0xC9636B7E03FBAEE0ull, 0xC9636B7E03FBAEE0ull },
	{ 0x9E9578B3DAE11B4Eull, 0x9E9578B3DAE11B4Eull },
	{ 0x45AFFFB7799A12A8ull, 0x45AFFFB7799A12A8ull },
	{ 0xD89F92425ED0122Dull, 0xD89F92425ED0122Dull },
	{ 0x86C3629F8C3C0AEAull, 0x86C3629F8C3C0AEAull },
	{ 0x6ECF274C235AA70Dull, 0x6ECF274C235AA70Dull },
	{ 0x5BF0AA221764310Cull, 0x5BF0AA221764310Cull },
	{ 0xA84E7CEBE8CE18F6ull, 0xA84E7CEBE8CE18F6ull },
	{ 0x824D571077FC1898ull, 0x824D571077FC1898ull },
	{ 0xD6719B35E0A6FF54ull, 0xD6719B35E0A6FF54ull },
	{ 0x9D5A844A6B9F103Dull, 0x9D5A844A6B9F103Dull },
	{ 0xCCFC7C128CB00DF6ull, 0xCCFC7C128CB00DF6ull },
	{ 0xAED8176AF10CDE4Bull, 0xAED8176AF10CDE4Bull },
	{ 0xD9933C9DA008D110ull, 0xD9933C9DA008D110ull },
	{ 0x8EB1F07928048539ull, 0x8EB1F07928048539ull },
	{ 0x3C744BE8DF546FBAull, 0x3C744BE8DF546FBAull },
	{ 0x635719FD31870020ull, 0x635719FD31870020ull },
	{ 0x9FFC9C02FF906A70ull, 0x9FFC9C02FF906A70ull },
	{ 0x991AF8D90EC205C0ull, 0x991AF8D90EC205C0ull },
	{ 0x10C5D10287535B13ull, 0x10C5D10287535B13ull },
	{ 0xDAB07CC582E2C578ull, 0xDAB07CC582E2C578ull },
	{ 0x75BE71D4AD33B7E5ull, 0x75BE71D4AD33B7E5ull },
	{ 0x094F4FF2F150A7CFull, 0x094F4FF2F150A7CFull },
	{ 0x87DC17B039D68E0Eull, 0x87DC17B039D68E0Eull },
	{ 0x340306AE3F101170ull, 0x340306AE3F101170ull },
	{ 0xCA979E4C6666479Bull, 0xCA979E4C6666479Bull },
	{ 0x90545029B14D2028ull, 0x90545029B14D2028ull },
	{ 0x8393543BBCF166FCull, 0x8393543BBCF166FCull },
	{ 0xF21C25C9CF8E6B68ull, 0xF21C25C9CF8E6B68ull },
	{ 0xB2AB29EAF8F14784ull, 0xB2AB29EAF8F14784ull },
	{ 0x4320FBD640CFC4C8ull, 0x4320FBD640CFC4C8ull },
	{ 0x09FEE2E3E678E16Bull, 0x09FEE2E3E678E16Bull },
	{ 0x7BDDF08EAE44F7FFull, 0x7BDDF08EAE44F7FFull },
	{ 0x65C64514428B844Dull, 0x65C64514428B844Dull },
	{ 0x9AA3FD23A1448AEEull, 0x9AA3FD23A1448AEEull },
	{ 0xC4DF48789B9B9BAAull, 0xC4DF48789B9B9BAAull },
	{ 0x9C45255364DA7C94ull, 0x9C45255364DA7C94ull },
	{ 0xBFF647EB31316C66ull, 0xBFF647EB31316C66ull },
	{ 0x73C9A6EAE4830799ull, 0x73C9A6EAE4830799ull },
	{ 0x024830F6D2F9D3C8ull, 0x024830F6D2F9D3C8ull },
	{ 0x825F2A6A8D4D5FC6ull, 0x825F2A6A8D4D5FC6ull },
	{ 0xC11EB30DC27D0BFDull, 0xC11EB30DC27D0BFDull },
	{ 0x0F71F30F2C667189ull, 0x0F71F30F2C667189ull },
	{ 0x341670C491BD6695ull, 0x341670C491BD6695ull },
	{ 0xC0F00E5ED10BA5F4ull, 0xC0F00E5ED10BA5F4ull },
	{ 0x8D2F2EF103FE1182ull, 0x8D2F2EF103FE1182ull },
	{ 0x12B68459AAA59DBDull, 0x12B68459AAA59DBDull },
	{ 0xF056048CED71D29Eull, 0xF056048CED71D29Eull },
	{ 0xCF08876EEA0AF4BDull, 0xCF08876EEA0AF4BDull },
	{ 0x6C098BBD1351CA24ull, 0x6C098BBD1351CA24ull },
	{ 0x0660A4D5A0EB0172ull, 0x0660A4D5A0EB0172ull },
	{ 0x70D8107C0C5C3434ull, 0x70D8107C0C5C3434ull },
	{ 0x078BF3E9A1E0927Full, 0x078BF3E9A1E0927Full },
	{ 0xC9A7C1C91F3B2451ull, 0xC9A7C1C91F3B2451ull },
	{ 0xD76F46EC60ECB1A9ull, 0xD76F46EC60ECB1A9ull },
	{ 0x1398E0DB23D34965ull, 0x1398E0DB23D34965ull },
	{ 0xA9565A69D299635Bull, 0xA9565A69D299635Bull },
	{ 0xB601F197643AABB9ull, 0xB601F197643AABB9ull },
	{ 0x56CD05214C8464A1ull, 0x56CD05214C8464A1ull },
	{ 0xDA85EA2C7A176F7Full, 0xDA85EA2C7A176F7Full },
	{ 0xD77269126591EDAEull, 0xD77269126591EDAEull },
	{ 0x6504639130E68F4Bull, 0x6504639130E68F4Bull },
	{ 0xB96621EDD5477058ull, 0xB96621EDD5477058ull },
	{ 0x6B93C8C7646AA314ull, 0x6B93C8C7646AA314ull },
	{ 0x18DF766A54CE5718ull, 0x18DF766A54CE5718ull },
	{ 0xAE9587E2E2429C35ull, 0xAE9587E2E2429C35ull },
	{ 0x983E877D7D5EC90Dull, 0x983E877D7D5EC90Dull },
	{ 0x0E7833D7A50B8A5Aull, 0x0E7833D7A50B8A5Aull },
	{ 0xE9D8A9BFF4B31D86ull, 0xE9D8A9BFF4B31D86ull },
	{ 0x86917C15BCCE0266ull, 0x86917C15BCCE0266ull },
	{ 0x631CEA943CC455B5ull, 0x631CEA943CC455B5ull },
	{ 0x181536BAE7CB7F7Aull, 0x181536BAE7CB7F7Aull },
	{ 0x236CE377E73F1D6Full, 0x236CE377E73F1D6Full },
	{ 0x06BCFCEE6E531230ull, 0x06BCFCEE6E531230ull },
	{ 0x7A48E7BBDD8CB527ull, 0x7A48E7BBDD8CB527ull },
	{ 0x738F388EB5111D36ull, 0x738F388EB5111D36ull },
	{ 0xC037139257B691A6ull, 0xC037139257B691A6ull },
	{ 0xF4C194029DE7164Full, 0xF4C194029DE7164Full },
	{ 0x6F4EBBAA39421908ull, 0x6F4EBBAA39421908ull },
	{ 0x2E0D6A3EE78B4351ull, 0x2E0D6A3EE78B4351ull },
	{ 0x3A85DD1E749463BFull, 0x3A85DD1E749463BFull },
	{ 0xE806367F8E307E27ull, 0xE806367F8E307E27ull },
	{ 0xA6508D59671DFA8Dull, 0xA6508D59671DFA8Dull },
	{ 0x28C204952A75F14Eull, 0x28C204952A75F14Eull },
	{ 0xA0726A8C3C8E9A0Cull, 0xA0726A8C3C8E9A0Cull },
	{ 0xCB83E0E651DED9DAull, 0xCB83E0E651DED9DAull },
	{ 0x17A20B7A4D90F028ull, 0x17A20B7A4D90F028ull },
	{ 0x98E84864F7924670ull, 0x98E84864F7924670ull },
	{ 0x943E6F387ECE5CA1ull, 0x943E6F387ECE5CA1ull },
	{ 0x5E839857CBA2D84Bull, 0x5E839857CBA2D84Bull },
	{ 0xCF666D00A33AB10Cull, 0xCF666D00A33AB10Cull },
	{ 0x07927EB79A9E8239ull, 0x07927EB79A9E8239ull },
	{ 0xCBF22598D9E7F2D0ull, 0xCBF22598D9E7F2D0ull },
	{ 0x344C6980AE66489Dull, 0x344C6980AE66489Dull },
	{ 0x6DCE710EEEE2162Bull, 0x6DCE710EEEE2162Bull },
	{ 0xD217F8382DCE03EEull, 0xD217F8382DCE03EEull },
	{ 0x91FA50A7BC12C3ADull, 0x91FA50A7BC12C3ADull },
	{ 0x33E169857CC41BB4ull, 0x33E169857CC41BB4ull },
	{ 0x26F65E2287AD15BAull, 0x26F65E2287AD15BAull },
	{ 0xA9930DDF9C014B90ull, 0xA9930DDF9C014B90ull },
	{ 0xC4563B9064A1570Bull, 0xC4563B9064A1570Bull },
	{ 0x188FB52E89806033ull, 0x188FB52E89806033ull },
	{ 0x4314037F93D8C72Bull, 0x4314037F93D8C72Bull },
	{ 0x933F2080A03DF863ull, 0x933F2080A03DF863ull },
	{ 0x5555B8DAE279557Cull, 0x5555B8DAE279557Cull },
	{ 0xF17C7153F780457Bull, 0xF17C7153F780457Bull },
	{ 0x9E64B79D0AB162B1ull, 0x9E64B79D0AB162B1ull },
	{ 0xE8369933C4E6C9EAull, 0xE8369933C4E6C9EAull },
	{ 0x4B072398138B84D4ull, 0x4B072398138B84D4ull },
	{ 0x21D2CAA9B9821AB1ull, 0x21D2CAA9B9821AB1ull },
	{ 0xA91B5970B20A3FC0ull, 0xA91B5970B20A3FC0ull },
	{ 0xD5C0E88364F39F67ull, 0xD5C0E88364F39F67ull },
	{ 0x9B4D6BC0E2F2BB0Full, 0x9B4D6BC0E2F2BB0Full },
	{ 0x6C83B0DA2ABE0A06ull, 0x6C83B0DA2ABE0A06ull },
	{ 0x542C11ADFEB8DC9Cull, 0x542C11ADFEB8DC9Cull },
	{ 0xA836E7AE13223012ull, 0xA836E7AE13223012ull },
	{ 0xDA0009FA692FB66Dull, 0xDA0009FA692FB66Dull },
	{ 0x754DCEF3455A0B32ull, 0x754DCEF3455A0B32ull },
	{ 0xA491F65FB72D8CE3ull, 0xA491F65FB72D8CE3ull },
	{ 0xF87DF9FFAED5CE39ull, 0xF87DF9FFAED5CE39ull },
	{ 0x27ED960E2DFEFF0Aull, 0x27ED960E2DFEFF0Aull },
	{ 0xC042C3C3CCE1FB8Eull, 0xC042C3C3CCE1FB8Eull },
	{ 0xEB6FCC3F85D3010Full, 0xEB6FCC3F85D3010Full },
	{ 0xA2DA9C8525EF1655ull, 0xA2DA9C8525EF1655ull },
	{ 0xA1413A8225FBBD3Eull, 0xA1413A8225FBBD3Eull },
	{ 0xBF61987AB530B3CDull, 0xBF61987AB530B3CDull },
	{ 0x046E300A13CDCE72ull, 0x046E300A13CDCE72ull },
	{ 0x2B42E6813F7AF89Aull, 0x2B42E6813F7AF89Aull },
	{ 0x5AD5B83EEB50527Aull, 0x5AD5B83EEB50527Aull },
	{ 0xD336AACBCE446C17ull, 0xD336AACBCE446C17ull },
	{ 0x4DEA8AD63C1EA819ull, 0x4DEA8AD63C1EA819ull },
	{ 0x5323ED7810ABB650ull, 0x5323ED7810ABB650ull },
	{ 0x4235FCA1EB123187ull, 0x4235FCA1EB123187ull },
	{ 0x4E648AF6B9856108ull, 0x4E648AF6B9856108ull },
	{ 0xAF9CEE9292F4126Full, 0xAF9CEE9292F4126Full },
	{ 0xC75B24C7E2319932ull, 0xC75B24C7E2319932ull },
	{ 0xF926BE0DE236EF70ull, 0xF926BE0DE236EF70ull },
	{ 0x77AB10AB67B9EDFAull, 0x77AB10AB67B9EDFAull },
	{ 0xC5263518FCF8AF9Full, 0xC5263518FCF8AF9Full },
	{ 0xA86248A54CB70E3Bull, 0xA86248A54CB70E3Bull },
	{ 0x55447A16DC8C3D82ull, 0x55447A16DC8C3D82ull },
	{ 0x89B55BB618FE8D89ull, 0x89B55BB618FE8D89ull },
	{ 0xDE4E001AAF0B4494ull, 0xDE4E001AAF0B4494ull },
	{ 0x8B1D5CE73918A026ull, 0x8B1D5CE73918A026ull },
	{ 0x605176AE37EFF146ull, 0x605176AE37EFF146ull },
	{ 0x3337D40B5EBDDC0Full, 0x3337D40B5EBDDC0Full },
	{ 0xFEA0B9CC2E76B3FAull, 0xFEA0B9CC2E76B3FAull },
	{ 0x7818AD72E01CFA6Eull, 0x7818AD72E01CFA6Eull },
	{ 0xD7F3C1CEA007FD08ull, 0xD7F3C1CEA007FD08ull },
	{ 0x96DE29B419289527ull, 0x96DE29B419289527ull },
	{ 0x5DFD93D14F2A013Bull, 0x5DFD93D14F2A013Bull },
	{ 0xEC06CE07CA6CE8CEull, 0xEC06CE07CA6CE8CEull },
	{ 0x2DD3312812BA7552ull, 0x2DD3312812BA7552ull },
	{ 0x4B495459796D93ADull, 0x4B495459796D93ADull },
	{ 0xE7E4880936705002ull, 0xE7E4880936705002ull },
	{ 0xD7AE9A8BD0CB5641ull, 0xD7AE9A8BD0CB5641ull },
	{ 0x07B762CBDBE58CF1ull, 0x07B762CBDBE58CF1ull },
	{ 0xDB4D93D10031D3BBull, 0xDB4D93D10031D3BBull },
	{ 0xBC24587A58E8E4BAull, 0xBC24587A58E8E4BAull },
	{ 0x1FA937171C0D4F02ull, 0x1FA937171C0D4F02ull },
	{ 0x3F35BC98985796EEull, 0x3F35BC98985796EEull },
	{ 0xC57070287E08702Dull, 0xC57070287E08702Dull },
	{ 0xAE688EFE7B3F218Full, 0xAE688EFE7B3F218Full },
	{ 0x1CA00F4024D1FE8Bull, 0x1CA00F4024D1FE8Bull },
	{ 0xE21F6C2635470714ull, 0xE21F6C2635470714ull },
	{ 0xFC76B932142AA51Eull, 0xFC76B932142AA51Eull },
	{ 0xC6477D4EB1E28AEAull, 0xC6477D4EB1E28AEAull },
	{ 0x9B6AD707B43AB019ull, 0x9B6AD707B43AB019ull },
	{ 0x320F67B1EF4A71F0ull, 0x320F67B1EF4A71F0ull },
	{ 0x672286B032AFCDE7ull, 0x672286B032AFCDE7ull },
	{ 0x3F903D6F623EC378ull, 0x3F903D6F623EC378ull },
	{ 0x5C9D8A44E262CEA6ull, 0x5C9D8A44E262CEA6ull },
	{ 0xA4986CEA5310894Bull, 0xA4986CEA5310894Bull },
	{ 0x600138A19AC565A6ull, 0x600138A19AC565A6ull },
	{ 0x89E8B13C40B38F23ull, 0x89E8B13C40B38F23ull },
	{ 0x94E66CF11857E6ECull, 0x94E66CF11857E6ECull },
	{ 0xE39301BB3D9821B1ull, 0xE39301BB3D9821B1ull },
	{ 0x35D976ED5D270833ull, 0x35D976ED5D270833ull },
	{ 0x490B827451CCD116ull, 0x490B827451CCD116ull },
	{ 0x6CFC0AD40F11A368ull, 0x6CFC0AD40F11A368ull },
	{ 0xCCBB73F9DB282F6Cull, 0xCCBB73F9DB282F6Cull },
	{ 0x2B59316EDA56712Bull, 0x2B59316EDA56712Bull },
	{ 0x54862305EAFA8E3Dull, 0x54862305EAFA8E3Dull },
	{ 0x018B37CFCF29CF53ull, 0x018B37CFCF29CF53ull },
	{ 0xF19FD970A887C388ull, 0xF19FD970A887C388ull },
	{ 0x77239FE1C3537C69ull, 0x77239FE1C3537C69ull },
	{ 0xB836CCFAB048E842ull, 0xB836CCFAB048E842ull },
	{ 0xE3350FCCB114572Aull, 0xE3350FCCB114572Aull },
	{ 0x17FE102B72F61450ull, 0x17FE102B72F61450ull },
	{ 0xC3FF424E18942387ull, 0xC3FF424E18942387ull },
	{ 0x33AA820396D8238Full, 0x33AA820396D8238Full },
	{ 0x96DCF6A55E174626ull, 0x96DCF6A55E174626ull },
	{ 0xB971BE0356E2734Full, 0xB971BE0356E2734Full },
	{ 0xB083E0942B7C84E0ull, 0xB083E0942B7C84E0ull },
	{ 0x3AAB0C24E314A351ull, 0x3AAB0C24E314A351ull },
	{ 0x14F69832CE82C7C3ull, 0x14F69832CE82C7C3ull },
	{ 0x83C250CAC0E4554Full, 0x83C250CAC0E4554Full },
	{ 0x4F688A789BD6D990ull, 0x4F688A789BD6D990ull },
	{ 0xF4CCFD222A6EEBF3ull, 0xF4CCFD222A6EEBF3ull },
	{ 0xCA53E8784B69273Dull, 0xCA53E8784B69273Dull },
	{ 0xD8A0FE796E334CDCull, 0xD8A0FE796E334CDCull },
	{ 0xA76197F05B0B7E72ull, 0xA76197F05B0B7E72ull },
	{ 0x3DCDAEEA50939F1Eull, 0x3DCDAEEA50939F1Eull },
	{ 0x9793230F6D56CF8Dull, 0x9793230F6D56CF8Dull },
	{ 0xCDB6C4D86B1F0D41ull, 0xCDB6C4D86B1F0D41ull },
	{ 0x332EA8ACA9D8EE20ull, 0x332EA8ACA9D8EE20ull },
	{ 0x255143438B63823Full, 0x255143438B63823Full },
	{ 0xD17CCC141D0A4DEFull, 0xD17CCC141D0A4DEFull },
	{ 0x2CB269AE14476B08ull, 0x2CB269AE14476B08ull },
	{ 0xE158A4E718CBC575ull, 0xE158A4E718CBC575ull },
	{ 0x6F30A4375EEB7780ull, 0x6F30A4375EEB7780ull },
	{ 0x40A1AF70936C4C5Eull, 0x40A1AF70936C4C5Eull },
	{ 0xF3567B0A1714597Dull, 0xF3567B0A1714597Dull },
	{ 0x14AFBC4D88B6BE0Dull, 0x14AFBC4D88B6BE0Dull },
	{ 0x92F7837E9FDC834Cull, 0x92F7837E9FDC834Cull },
	{ 0x21D8E0C959079695ull, 0x21D8E0C959079695ull },
	{ 0xD3274ED16FEDEB2Bull, 0xD3274ED16FEDEB2Bull },
	{ 0x23D966AE2A6AB4C2ull, 0x23D966AE2A6AB4C2ull },
	{ 0xBC525B0D7343C84Eull, 0xBC525B0D7343C84Eull },
	{ 0x25ADBC2946C1188Full, 0x25ADBC2946C1188Full },
	{ 0xB1F902AA6B7454DCull, 0xB1F902AA6B7454DCull },
	{ 0xD97E63E37E6EA4AFull, 0xD97E63E37E6EA4AFull },
	{ 0xCB0C6CDF4E028AF2ull, 0xCB0C6CDF4E028AF2ull },
	{ 0xF0DDE54D854FDD87ull, 0xF0DDE54D854FDD87ull },
	{ 0xA97D7CA8A677E89Dull, 0xA97D7CA8A677E89Dull },
	{ 0xF458BC0055B7A9B1ull, 0xF458BC0055B7A9B1ull },
	{ 0xD2BDC282F4862DE1ull, 0xD2BDC282F4862DE1ull },
	{ 0x9C1748518DB51178ull, 0x9C1748518DB51178ull },
	{ 0x07426AB90DB11B37ull, 0x07426AB90DB11B37ull },
	{ 0xA721BEDBD0E0FEE0ull, 0xA721BEDBD0E0FEE0ull },
	{ 0x9B192E067BC03508ull, 0x9B192E067BC03508ull },
	{ 0xB40618FB2833FE9Dull, 0xB40618FB2833FE9Dull },
	{ 0xCD2F426DDE387FB5ull, 0xCD2F426DDE387FB5ull },
	{ 0x9870043D60E3FF19ull, 0x9870043D60E3FF19ull },
	{ 0x1FCA4983485E4391ull, 0x1FCA4983485E4391ull },
	{ 0x3962D2E66D8A11F1ull, 0x3962D2E66D8A11F1ull },
	{ 0xB0B341EAA43EC9E4ull, 0xB0B341EAA43EC9E4ull },
	{ 0xE3156BE8C61027F7ull, 0xE3156BE8C61027F7ull },
	{ 0x1093768A0F021DE8ull, 0x1093768A0F021DE8ull },
	{ 0xA0E03A71C726D5AFull, 0xA0E03A71C726D5AFull },
	{ 0x9BA36EF03C84CCCDull, 0x9BA36EF03C84CCCDull },
	{ 0x959D1E0A11C31253ull, 0x959D1E0A11C31253ull },
	{ 0x512BDF0ED4F9A636ull, 0x512BDF0ED4F9A636ull },
	{ 0x53295B09ECE3F652ull, 0x53295B09ECE3F652ull },
	{ 0xABB89768D4937BBBull, 0xABB89768D4937BBBull },
	{ 0x32C98A21A4607B4Bull, 0x32C98A21A4607B4Bull },
	{ 0xF92D7514EE36E0E8ull, 0xF92D7514EE36E0E8ull },
	{ 0x2CEF88D5E87D3E94ull, 0x2CEF88D5E87D3E94ull },
	{ 0x328D5153E873EABBull, 0x328D5153E873EABBull },
	{ 0x314268902E8A3E38ull, 0x314268902E8A3E38ull },
	{ 0x5442F23AFFC7F128ull, 0x5442F23AFFC7F128ull },
	{ 0x7ED85F8311889AF2ull, 0x7ED85F8311889AF2ull },
	{ 0xB5DBFA5245274D16ull, 0xB5DBFA5245274D16ull },
	{ 0xFBD2B8A2B1585635ull, 0xFBD2B8A2B1585635ull },
	{ 0x6225D886FE8F1AE1ull, 0x6225D886FE8F1AE1ull },
	{ 0x0DABC33111EE15A6ull, 0x0DABC33111EE15A6ull },
	{ 0x6F4CB872075EF6D8ull, 0x6F4CB872075EF6D8ull },
	{ 0xF106C2BE7D10DF1Eull, 0xF106C2BE7D10DF1Eull },
	{ 0x569C71F22F7AC0BEull, 0x569C71F22F7AC0BEull },
	{ 0x91F1DF9DE19FDB2Aull, 0x91F1DF9DE19FDB2Aull },
	{ 0x4C93799A9BF45A4Bull, 0x4C93799A9BF45A4Bull },
	{ 0xE3AA166D3320140Dull, 0xE3AA166D3320140Dull },
	{ 0x1927D03526BD66ABull, 0x1927D03526BD66ABull },
	{ 0x38812891C9D236BBull, 0x38812891C9D236BBull },
	{ 0x6F6DF378BF210BFBull, 0x6F6DF378BF210BFBull },
	{ 0x9A558B88C390515Eull, 0x9A558B88C390515Eull },
	{ 0x45A1CD53C945B0D1ull, 0x45A1CD53C945B0D1ull },
	{ 0x220C59EB633393D2ull, 0x220C59EB633393D2ull },
	{ 0xAD71A40A7E76BEB3ull, 0xAD71A40A7E76BEB3ull },
	{ 0xCFCD42476F82112Bull, 0xCFCD42476F82112Bull },
	{ 0xABAB0AE94C76619Dull, 0xABAB0AE94C76619Dull },
	{ 0xE31077CB96247DACull, 0xE31077CB96247DACull },
	{ 0x1069D14B361746FEull, 0x1069D14B361746FEull },
	{ 0x32BEA227C7E3F04Full, 0x32BEA227C7E3F04Full },
	{ 0x4EF8F334341E5D39ull, 0x4EF8F334341E5D39ull },
	{ 0x0482CDC19BEFA042ull, 0x0482CDC19BEFA042ull },
	{ 0x55E588FF932C96A6ull, 0x55E588FF932C96A6ull },
	{ 0x65BE5B5CAD2AA966ull, 0x65BE5B5CAD2AA966ull },
	{ 0x6723B7A255B0A711ull, 0x6723B7A255B0A711ull },
	{ 0x02DEEFF6FCC1C6D0ull, 0x02DEEFF6FCC1C6D0ull },
	{ 0xEFB5A0E19260C337ull, 0xEFB5A0E19260C337ull },
	{ 0xFF9665995009184Bull, 0xFF9665995009184Bull },
	{ 0xDFF4529CD57C624Eull, 0xDFF4529CD57C624Eull },
	{ 0xCEE4BA3B91E1DD52ull, 0xCEE4BA3B91E1DD52ull },
	{ 0x038AB70C8015FEFCull, 0x038AB70C8015FEFCull },
	{ 0x8AF4009D31683817ull, 0x8AF4009D31683817ull },
	{ 0xD92285942864D2DDull, 0xD92285942864D2DDull },
	{ 0x4D46503183A8EE01ull, 0x4D46503183A8EE01ull },
	{ 0x6AF33168F95E9C80ull, 0x6AF33168F95E9C80ull },
	{ 0x8685B460221F2528ull, 0x8685B460221F2528ull },
	{ 0x433CA5D515AD00C8ull, 0x433CA5D515AD00C8ull },
	{ 0x51E3EE1B12EE5D1Bull, 0x51E3EE1B12EE5D1Bull },
	{ 0xD10436EA38C7B46Cull, 0xD10436EA38C7B46Cull },
	{ 0x3FB8D30AC49E91A4ull, 0x3FB8D30AC49E91A4ull },
	{ 0x0E788E2D5BD68AF2ull, 0x0E788E2D5BD68AF2ull },
	{ 0x3C8BFAB13C642566ull, 0x3C8BFAB13C642566ull },
	{ 0x99D0E6FB7AF95B50ull, 0x99D0E6FB7AF95B50ull },
	{ 0xEC987B4E5271C3DBull, 0xEC987B4E5271C3DBull },
	{ 0x3B557F1F83045664ull, 0x3B557F1F83045664ull },
	{ 0xFA68461CC0E0D2A0ull, 0xFA68461CC0E0D2A0ull },
	{ 0x5DBDC6BD32FEC66Dull, 0x5DBDC6BD32FEC66Dull },
	{ 0x8EAF568F02043277ull, 0x8EAF568F02043277ull },
	{ 0x85B890BFF909D00Eull, 0x85B890BFF909D00Eull },
	{ 0x4850E2B03222FB46ull, 0x4850E2B03222FB46ull },
	{ 0xCA9E426DD6AF32D1ull, 0xCA9E426DD6AF32D1ull },
	{ 0xB4CE8B28ED9B4218ull, 0xB4CE8B28ED9B4218ull },
	{ 0xCD9D039E3BD8069Full, 0xCD9D039E3BD8069Full },
	{ 0x47618AE8322CD49Aull, 0x47618AE8322CD49Aull },
	{ 0xB25EEE94EBEB7E4Aull, 0xB25EEE94EBEB7E4Aull },
	{ 0x51FDE7903EABF99Dull, 0x51FDE7903EABF99Dull },
	{ 0x96DA4000D915A686ull, 0x96DA4000D915A686ull },
	{ 0x7C7FA107725C3398ull, 0x7C7FA107725C3398ull },
	{ 0x0B0B879F08762669ull, 0x0B0B879F08762669ull },
	{ 0xBD11E20D2EB2FE25ull, 0xBD11E20D2EB2FE25ull },
	{ 0x0969D6816E76FDF1ull, 0x0969D6816E76FDF1ull },
	{ 0xD0702F92B4828E82ull, 0xD0702F92B4828E82ull },
	{ 0xEE84F199D18B487Cull, 0xEE84F199D18B487Cull },
	{ 0x31319918078C950Full, 0x31319918078C950Full },
	{ 0xF14D7F68A995FA4Full, 0xF14D7F68A995FA4Full },
	{ 0x905DE8ACCAFCFAD4ull, 0x905DE8ACCAFCFAD4ull },
	{ 0x1AB7394FFDF8A192ull, 0x1AB7394FFDF8A192ull },
	{ 0xCD1CD315234CA3FAull, 0xCD1CD315234CA3FAull },
	{ 0xBE551AEA484A2851ull, 0xBE551AEA484A2851ull },
	{ 0x6D0F26F2968C2DCFull, 0x6D0F26F2968C2DCFull },
	{ 0xF1AF92C552B5907Bull, 0xF1AF92C552B5907Bull },
	{ 0x93D1FE30EB56AFC5ull, 0x93D1FE30EB56AFC5ull },
	{ 0x0484B1A11840B664ull, 0x0484B1A11840B664ull },
	{ 0xFB8BECB709AC7C71ull, 0xFB8BECB709AC7C71ull },
	{ 0xF9225AB2A5BB1144ull, 0xF9225AB2A5BB1144ull },
	{ 0x6C4430FBD9F5DE8Eull, 0x6C4430FBD9F5DE8Eull },
	{ 0x7BDA73CAF7947137ull, 0x7BDA73CAF7947137ull },
	{ 0xB4A3E43F25ADDCFAull, 0xB4A3E43F25ADDCFAull },
	{ 0x5B311521DC9BC1BEull, 0x5B311521DC9BC1BEull },
	{ 0xA16129C67834F5B6ull, 0xA16129C67834F5B6ull },
	{ 0x0024B9C4D3E5C73Bull, 0x0024B9C4D3E5C73Bull },
	{ 0x75745FFDFC8BE468ull, 0x75745FFDFC8BE468ull },
	{ 0xE5C06EC9ED5BE076ull, 0xE5C06EC9ED5BE076ull },
	{ 0x62174F1D60365ADAull, 0x62174F1D60365ADAull },
	{ 0x11780406AAB2C51Dull, 0x11780406AAB2C51Dull },
	{ 0xFEDE5A30B005C87Aull, 0xFEDE5A30B005C87Aull },
	{ 0x0ACE3BCADD8949BDull, 0x0ACE3BCADD8949BDull },
	{ 0xC15BAEE53B4C135Aull, 0xC15BAEE53B4C135Aull },
	{ 0xD3A4FAFF89EF194Cull, 0xD3A4FAFF89EF194Cull },
	{ 0xF5BC757A71F65E30ull, 0xF5BC757A71F65E30ull },
	{ 0x60277889C3537B21ull, 0x60277889C3537B21ull },
	{ 0xB6100016C40FC4D5ull, 0xB6100016C40FC4D5ull },
	{ 0x7530E99F4D8652C7ull, 0x7530E99F4D8652C7ull },
	{ 0xE60FED7BF129FFE7ull, 0xE60FED7BF129FFE7ull },
	{ 0x9CBA46D3BEF1A766ull, 0x9CBA46D3BEF1A766ull },
	{ 0x0E042F6D9A128B1Eull, 0x0E042F6D9A128B1Eull },
	{ 0x0379838ED3C1DDDBull, 0x0379838ED3C1DDDBull },
	{ 0x3E37F27A0E936A45ull, 0x3E37F27A0E936A45ull },
	{ 0xEDD034A3EFD5658Dull, 0xEDD034A3EFD5658Dull },
	{ 0x3AEDCD40A4575C5Bull, 0x3AEDCD40A4575C5Bull },
	{ 0x25BCCF9D32AA3FF6ull, 0x25BCCF9D32AA3FF6ull },
	{ 0x795D2FC79D879435ull, 0x795D2FC79D879435ull },
	{ 0x921B1A18D8ADBF25ull, 0x921B1A18D8ADBF25ull },
	{ 0xB84BD203895F757Aull, 0xB84BD203895F757Aull },
	{ 0xCF269A7A9BD4483Bull, 0xCF269A7A9BD4483Bull },
	{ 0x97992492A48855A8ull, 0x97992492A48855A8ull },
	{ 0xC69F079113B912EFull, 0xC69F079113B912EFull },
	{ 0xB06BC06C383D3D0Full, 0xB06BC06C383D3D0Full },
	{ 0x470EB8EF1305E68Cull, 0x470EB8EF1305E68Cull },
	{ 0xAD10D763FA76A5D3ull, 0xAD10D763FA76A5D3ull },
	{ 0x00000000038E418Bull, 0x00000000038E418Bull },
	{ 0x8000000000000EC2ull, 0x8000000000000EC2ull },
	{ 0x0000000000002943ull, 0x0000000000002943ull },
	{ 0x800016212D76B45Dull, 0x800016212D76B45Dull },
	{ 0x0000FE4FD9CDBED1ull, 0x0000FE4FD9CDBED1ull },
	{ 0x80000000000000EBull, 0x80000000000000EBull },
	{ 0x000004E2996CBC45ull, 0x000004E2996CBC45ull },
	{ 0x0004F2D53B6AFDB0ull, 0x0004F2D53B6AFDB0ull },
	{ 0x800000000415AB22ull, 0x800000000415AB22ull },
	{ 0x800000000000000Aull, 0x800000000000000Aull },
	{ 0x000075142F9C4B7Eull, 0x000075142F9C4B7Eull },
	{ 0x800000165EA2BC31ull, 0x800000165EA2BC31ull },
	{ 0x0000002788D33121ull, 0x0000002788D33121ull },
	{ 0x8000000C720C1613ull, 0x8000000C720C1613ull },
	{ 0x000000000000003Full, 0x000000000000003Full },
	{ 0x0000000000000007ull, 0x0000000000000007ull },
	{ 0x800000000000546Dull, 0x800000000000546Dull },
	{ 0x00000001127C2CA5ull, 0x00000001127C2CA5ull },
	{ 0x8000000000000036ull, 0x8000000000000036ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x000000000001A5D0ull, 0x000000000001A5D0ull },
	{ 0x0000000000000EC6ull, 0x0000000000000EC6ull },
	{ 0x00000000000008BAull, 0x00000000000008BAull },
	{ 0x8000000000013F81ull, 0x8000000000013F81ull },
	{ 0x8000000000338B13ull, 0x8000000000338B13ull },
	{ 0x80000000034D9B0Aull, 0x80000000034D9B0Aull },
	{ 0x0000000000066266ull, 0x0000000000066266ull },
	{ 0x000000016F5A213Cull, 0x000000016F5A213Cull },
	{ 0x8000000000124745ull, 0x8000000000124745ull },
	{ 0x8000000D74D30C6Cull, 0x8000000D74D30C6Cull },
	{ 0x8000000000000010ull, 0x8000000000000010ull },
	{ 0x0000000000139ACDull, 0x0000000000139ACDull },
	{ 0x0000000000000023ull, 0x0000000000000023ull },
	{ 0x0000000000000018ull, 0x0000000000000018ull },
	{ 0x8000006B02375722ull, 0x8000006B02375722ull },
	{ 0x80000560D03BA65Eull, 0x80000560D03BA65Eull },
	{ 0x8000000000000004ull, 0x8000000000000004ull },
	{ 0x0000000000000193ull, 0x0000000000000193ull },
	{ 0x0000009ECCEBD4BBull, 0x0000009ECCEBD4BBull },
	{ 0x00000017CAB224EEull, 0x00000017CAB224EEull },
	{ 0x8000000000000078ull, 0x8000000000000078ull },
	{ 0x0000000000000FDBull, 0x0000000000000FDBull },
	{ 0x000000000000F471ull, 0x000000000000F471ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x80001B2E24BD10EFull, 0x80001B2E24BD10EFull },
	{ 0x80014B514DE5E2D9ull, 0x80014B514DE5E2D9ull },
	{ 0x800000000000E2CEull, 0x800000000000E2CEull },
	{ 0x80000000000010B1ull, 0x80000000000010B1ull },
	{ 0x0000000001688A4Bull, 0x0000000001688A4Bull },
	{ 0x8000000003943E8Cull, 0x8000000003943E8Cull },
	{ 0x800000000000000Aull, 0x800000000000000Aull },
	{ 0x000000000052C5E1ull, 0x000000000052C5E1ull },
	{ 0x0000000000004D72ull, 0x0000000000004D72ull },
	{ 0x000000000000001Dull, 0x000000000000001Dull },
	{ 0x800000000000000Full, 0x800000000000000Full },
	{ 0x0000000000000044ull, 0x0000000000000044ull },
	{ 0x00000000011F8353ull, 0x00000000011F8353ull },
	{ 0x000000000DF35020ull, 0x000000000DF35020ull },
	{ 0x0006A6228768FD16ull, 0x0006A6228768FD16ull },
	{ 0x800000001FCFBF64ull, 0x800000001FCFBF64ull },
	{ 0x80000000002D3346ull, 0x80000000002D3346ull },
	{ 0x800422B51AB4679Cull, 0x800422B51AB4679Cull },
	{ 0x0000002C45AD6DB0ull, 0x0000002C45AD6DB0ull },
	{ 0x000000000000006Cull, 0x000000000000006Cull },
	{ 0x000000060222820Eull, 0x000000060222820Eull },
	{ 0x0000000003BF6D60ull, 0x0000000003BF6D60ull },
	{ 0x00064AF5B035666Aull, 0x00064AF5B035666Aull },
	{ 0x00000000000001FAull, 0x00000000000001FAull },
	{ 0x000000000000002Full, 0x000000000000002Full },
	{ 0x8000002C6683679Eull, 0x8000002C6683679Eull },
	{ 0x0000000000000059ull, 0x0000000000000059ull },
	{ 0x000000000001E987ull, 0x000000000001E987ull },
	{ 0x800001CF0D46ECB9ull, 0x800001CF0D46ECB9ull },
	{ 0x000000000000004Full, 0x000000000000004Full },
	{ 0x80000000010810CCull, 0x80000000010810CCull },
	{ 0x00001F0ADA9AF172ull, 0x00001F0ADA9AF172ull },
	{ 0x0002E2E3405F9DCDull, 0x0002E2E3405F9DCDull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x00000000C81BAE0Dull, 0x00000000C81BAE0Dull },
	{ 0x8001787F088C908Dull, 0x8001787F088C908Dull },
	{ 0x0000000000000377ull, 0x0000000000000377ull },
	{ 0x0000000000002D92ull, 0x0000000000002D92ull },
	{ 0x0000029215CFE184ull, 0x0000029215CFE184ull },
	{ 0x80002572CDB1ABA3ull, 0x80002572CDB1ABA3ull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x80000000013479A5ull, 0x80000000013479A5ull },
	{ 0x0000000280C85189ull, 0x0000000280C85189ull },
	{ 0x80000000037479C0ull, 0x80000000037479C0ull },
	{ 0x0000000000000005ull, 0x0000000000000005ull },
	{ 0x0000000060C652EFull, 0x0000000060C652EFull },
	{ 0x00000007E18D2591ull, 0x00000007E18D2591ull },
	{ 0x80000ADDAB3FE355ull, 0x80000ADDAB3FE355ull },
	{ 0x0000000000031174ull, 0x0000000000031174ull },
	{ 0x000000000000000Cull, 0x000000000000000Cull },
	{ 0x000000000000000Eull, 0x000000000000000Eull },
	{ 0x800000000062DB11ull, 0x800000000062DB11ull },
	{ 0x0000D8D69562DF42ull, 0x0000D8D69562DF42ull },
	{ 0x0000000FF0C2DFCCull, 0x0000000FF0C2DFCCull },
	{ 0x8000000E99F4064Dull, 0x8000000E99F4064Dull },
	{ 0x800D9D06F952BA24ull, 0x800D9D06F952BA24ull },
	{ 0x8000699C717A3F44ull, 0x8000699C717A3F44ull },
	{ 0x8000000000002A59ull, 0x8000000000002A59ull },
	{ 0x8000000000009FEDull, 0x8000000000009FEDull },
	{ 0x00000000000AF1FFull, 0x00000000000AF1FFull },
	{ 0x000000E1110DFAFFull, 0x000000E1110DFAFFull },
	{ 0x0000000001780802ull, 0x0000000001780802ull },
	{ 0x8000000002D84143ull, 0x8000000002D84143ull },
	{ 0x80000000D3715908ull, 0x80000000D3715908ull },
	{ 0x80000000DA8333F9ull, 0x80000000DA8333F9ull },
	{ 0x0000000000001CACull, 0x0000000000001CACull },
	{ 0x000000038DD3335Full, 0x000000038DD3335Full },
	{ 0x80000023902236A1ull, 0x80000023902236A1ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x80000000625A375Eull, 0x80000000625A375Eull },
	{ 0x80000720D20AB4C3ull, 0x80000720D20AB4C3ull },
	{ 0x8000000000000007ull, 0x8000000000000007ull },
	{ 0x80000000000001B4ull, 0x80000000000001B4ull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x0001EA0CC40C848Cull, 0x0001EA0CC40C848Cull },
	{ 0x000000000000008Eull, 0x000000000000008Eull },
	{ 0x80000DCAC1992604ull, 0x80000DCAC1992604ull },
	{ 0x8000000000000005ull, 0x8000000000000005ull },
	{ 0x80000174A668DB7Dull, 0x80000174A668DB7Dull },
	{ 0x000000008AAFD5A2ull, 0x000000008AAFD5A2ull },
	{ 0x0000000C29A5CCCFull, 0x0000000C29A5CCCFull },
	{ 0x000000004BB144C2ull, 0x000000004BB144C2ull },
	{ 0x0000000000001467ull, 0x0000000000001467ull },
	{ 0x8000000223A0D757ull, 0x8000000223A0D757ull },
	{ 0x8000000000033EA0ull, 0x8000000000033EA0ull },
	{ 0x00007C4036C6521Bull, 0x00007C4036C6521Bull },
	{ 0x8000000000001B07ull, 0x8000000000001B07ull },
	{ 0x800AC1D833FF9DDEull, 0x800AC1D833FF9DDEull },
	{ 0x00000000013FD3F8ull, 0x00000000013FD3F8ull },
	{ 0x00000475561AF9C5ull, 0x00000475561AF9C5ull },
	{ 0x80000000296D0154ull, 0x80000000296D0154ull },
	{ 0x0000000000C51E5Eull, 0x0000000000C51E5Eull },
	{ 0x800000000000063Eull, 0x800000000000063Eull },
	{ 0x00000000000000D5ull, 0x00000000000000D5ull },
	{ 0x000015AE1E8D81C1ull, 0x000015AE1E8D81C1ull },
	{ 0x0000000000016DC0ull, 0x0000000000016DC0ull },
	{ 0x80000DBECE259FC6ull, 0x80000DBECE259FC6ull },
	{ 0x000000002AF2D7FDull, 0x000000002AF2D7FDull },
	{ 0x80000000000005A8ull, 0x80000000000005A8ull },
	{ 0x00000000000000C0ull, 0x00000000000000C0ull },
	{ 0x800000000367C421ull, 0x800000000367C421ull },
	{ 0x000000000004ED67ull, 0x000000000004ED67ull },
	{ 0x000000000000FF61ull, 0x000000000000FF61ull },
	{ 0x8000000B77144643ull, 0x8000000B77144643ull },
	{ 0x8000000000000004ull, 0x8000000000000004ull },
	{ 0x0000000000000046ull, 0x0000000000000046ull },
	{ 0x8000000000000014ull, 0x8000000000000014ull },
	{ 0x000000000000011Dull, 0x000000000000011Dull },
	{ 0x8000004E28C5596Dull, 0x8000004E28C5596Dull },
	{ 0x00000029E54BDFCFull, 0x00000029E54BDFCFull },
	{ 0x8000058B0F0CEA0Dull, 0x8000058B0F0CEA0Dull },
	{ 0x000000000000070Dull, 0x000000000000070Dull },
	{ 0x00000000001AD0FCull, 0x00000000001AD0FCull },
	{ 0x00000002E5EA3069ull, 0x00000002E5EA3069ull },
	{ 0x8000046A1B3F18FCull, 0x8000046A1B3F18FCull },
	{ 0x00000002671F517Cull, 0x00000002671F517Cull },
	{ 0x0000000000000A08ull, 0x0000000000000A08ull },
	{ 0x0000000000000346ull, 0x0000000000000346ull },
	{ 0x0000000026B8D921ull, 0x0000000026B8D921ull },
	{ 0x8000000000000267ull, 0x8000000000000267ull },
	{ 0x8000000000300C13ull, 0x8000000000300C13ull },
	{ 0x80000004932B6184ull, 0x80000004932B6184ull },
	{ 0x000000067B4E3523ull, 0x000000067B4E3523ull },
	{ 0x00000000000001F5ull, 0x00000000000001F5ull },
	{ 0x0000036B77772C7Aull, 0x0000036B77772C7Aull },
	{ 0x8000000000000511ull, 0x8000000000000511ull },
	{ 0x80000000DAAD766Cull, 0x80000000DAAD766Cull },
	{ 0x00000000A1AE761Bull, 0x00000000A1AE761Bull },
	{ 0x00018C4353DC7112ull, 0x00018C4353DC7112ull },
	{ 0x80010CD2267EE7DDull, 0x80010CD2267EE7DDull },
	{ 0x0000000000023CA1ull, 0x0000000000023CA1ull },
	{ 0x8000000064C96A79ull, 0x8000000064C96A79ull },
	{ 0x8000000000000005ull, 0x8000000000000005ull },
	{ 0x800000000014FBBEull, 0x800000000014FBBEull },
	{ 0x0001A24D3D27BC60ull, 0x0001A24D3D27BC60ull },
	{ 0x80000120828B7ECDull, 0x80000120828B7ECDull },
	{ 0x800000000010A2A8ull, 0x800000000010A2A8ull },
	{ 0x00000000000001A6ull, 0x00000000000001A6ull },
	{ 0x0003674BF4FAEE37ull, 0x0003674BF4FAEE37ull },
	{ 0x80001751F379C464ull, 0x80001751F379C464ull },
	{ 0x80000000149F11ECull, 0x80000000149F11ECull },
	{ 0x00000000000078EEull, 0x00000000000078EEull },
	{ 0x000000006C7B6F8Full, 0x000000006C7B6F8Full },
	{ 0x800000001A240D12ull, 0x800000001A240D12ull },
	{ 0x800000000154F604ull, 0x800000000154F604ull },
	{ 0x80001B9EBAD82BADull, 0x80001B9EBAD82BADull },
	{ 0x80000000086605CFull, 0x80000000086605CFull },
	{ 0x800000766C4150A8ull, 0x800000766C4150A8ull },
	{ 0x80000000000E8B73ull, 0x80000000000E8B73ull },
	{ 0x8000001109F64FA4ull, 0x8000001109F64FA4ull },
	{ 0x0000000000000026ull, 0x0000000000000026ull },
	{ 0x0000000039C2AA06ull, 0x0000000039C2AA06ull },
	{ 0x8000000000001A49ull, 0x8000000000001A49ull },
	{ 0x800000000344BFF1ull, 0x800000000344BFF1ull },
	{ 0x8000000000044998ull, 0x8000000000044998ull },
	{ 0x00000000002D922Eull, 0x00000000002D922Eull },
	{ 0x0000000000001D98ull, 0x0000000000001D98ull },
	{ 0x800000000000EBD0ull, 0x800000000000EBD0ull },
	{ 0x00000000000009B2ull, 0x00000000000009B2ull },
	{ 0x800000000000008Full, 0x800000000000008Full },
	{ 0x8000000000000E52ull, 0x8000000000000E52ull },
	{ 0x8006797B60589EA1ull, 0x8006797B60589EA1ull },
	{ 0x000000019984ED14ull, 0x000000019984ED14ull },
	{ 0x8000000000010A20ull, 0x8000000000010A20ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x00000005E02B00F4ull, 0x00000005E02B00F4ull },
	{ 0x80000331BABD4138ull, 0x80000331BABD4138ull },
	{ 0x0000CD6BC9E44D2Aull, 0x0000CD6BC9E44D2Aull },
	{ 0x0000000032CA5D11ull, 0x0000000032CA5D11ull },
	{ 0x80000000EA5C0923ull, 0x80000000EA5C0923ull },
	{ 0x8000000000000289ull, 0x8000000000000289ull },
	{ 0x80000000005C45BDull, 0x80000000005C45BDull },
	{ 0x8000000128773254ull, 0x8000000128773254ull },
	{ 0x0000000000000011ull, 0x0000000000000011ull },
	{ 0x000000000000C192ull, 0x000000000000C192ull },
	{ 0x000001F42717CEBBull, 0x000001F42717CEBBull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x800000000000A8EBull, 0x800000000000A8EBull },
	{ 0x8002C3DF101FF7C2ull, 0x8002C3DF101FF7C2ull },
	{ 0x000000000000005Eull, 0x000000000000005Eull },
	{ 0x0000000000E27B52ull, 0x0000000000E27B52ull },
	{ 0x0000000F5256EF8Aull, 0x0000000F5256EF8Aull },
	{ 0x8000000000000146ull, 0x8000000000000146ull },
	{ 0x800068C44F05539Aull, 0x800068C44F05539Aull },
	{ 0x80000009FF01E3CAull, 0x80000009FF01E3CAull },
	{ 0x0000193D92E4422Cull, 0x0000193D92E4422Cull },
	{ 0x80000000000006BBull, 0x80000000000006BBull },
	{ 0x8000012AAEDA2343ull, 0x8000012AAEDA2343ull },
	{ 0x0000000000001EDCull, 0x0000000000001EDCull },
	{ 0x000000000000000Eull, 0x000000000000000Eull },
	{ 0x0000000001631326ull, 0x0000000001631326ull },
	{ 0x80000000000CB41Bull, 0x80000000000CB41Bull },
	{ 0x00000C75E13D5A87ull, 0x00000C75E13D5A87ull },
	{ 0x00000001FBB52366ull, 0x00000001FBB52366ull },
	{ 0x8000000000073724ull, 0x8000000000073724ull },
	{ 0x000000000000003Full, 0x000000000000003Full },
	{ 0x00027DFE97B5600Eull, 0x00027DFE97B5600Eull },
	{ 0x000000030D846915ull, 0x000000030D846915ull },
	{ 0x000000000000000Bull, 0x000000000000000Bull },
	{ 0x000E6A850283547Dull, 0x000E6A850283547Dull },
	{ 0x80000000351F1D48ull, 0x80000000351F1D48ull },
	{ 0x800000000000B809ull, 0x800000000000B809ull },
	{ 0x800000000000004Full, 0x800000000000004Full },
	{ 0x000000000001F006ull, 0x000000000001F006ull },
	{ 0x8000000106206D4Eull, 0x8000000106206D4Eull },
	{ 0x000000096342927Full, 0x000000096342927Full },
	{ 0x0000000A091E8CD9ull, 0x0000000A091E8CD9ull },
	{ 0x8001E8D7CA5C2684ull, 0x8001E8D7CA5C2684ull },
	{ 0x0000048D843A50C1ull, 0x0000048D843A50C1ull },
	{ 0x80000000000066B5ull, 0x80000000000066B5ull },
	{ 0x000000000014011Dull, 0x000000000014011Dull },
	{ 0x80000005FAF4FF3Dull, 0x80000005FAF4FF3Dull },
	{ 0x0000006DD21F82E2ull, 0x0000006DD21F82E2ull },
	{ 0x800002BDB16E15CDull, 0x800002BDB16E15CDull },
	{ 0x8000000004F340FDull, 0x8000000004F340FDull },
	{ 0x8000000EFF3B00CEull, 0x8000000EFF3B00CEull },
	{ 0x8000000001B2B0C7ull, 0x8000000001B2B0C7ull },
	{ 0x00000003D1A280B0ull, 0x00000003D1A280B0ull },
	{ 0x800000001136637Eull, 0x800000001136637Eull },
	{ 0x8000010B01934AFCull, 0x8000010B01934AFCull },
	{ 0x800000000000000Cull, 0x800000000000000Cull },
	{ 0x80001231CA4CAE84ull, 0x80001231CA4CAE84ull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x800000026EF64D51ull, 0x800000026EF64D51ull },
	{ 0x00000F50F0E6837Bull, 0x00000F50F0E6837Bull },
	{ 0x0000000000000017ull, 0x0000000000000017ull },
	{ 0x80000000000001B1ull, 0x80000000000001B1ull },
	{ 0x000000001157DD83ull, 0x000000001157DD83ull },
	{ 0x0000E971A13CCBA7ull, 0x0000E971A13CCBA7ull },
	{ 0x00002B3FCBAF83F1ull, 0x00002B3FCBAF83F1ull },
	{ 0x8000000004DE12A7ull, 0x8000000004DE12A7ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x800000026691B779ull, 0x800000026691B779ull },
	{ 0x000000000000570Aull, 0x000000000000570Aull },
	{ 0x0000001DBF12F230ull, 0x0000001DBF12F230ull },
	{ 0x00000000023FE9E7ull, 0x00000000023FE9E7ull },
	{ 0x0000000001CA24F3ull, 0x0000000001CA24F3ull },
	{ 0x0000000000000100ull, 0x0000000000000100ull },
	{ 0x8000019CE4CF30AAull, 0x8000019CE4CF30AAull },
	{ 0x0000000000003871ull, 0x0000000000003871ull },
	{ 0x8000000210F035F7ull, 0x8000000210F035F7ull },
	{ 0x0000074BA54DDB74ull, 0x0000074BA54DDB74ull },
	{ 0x80000000000003DDull, 0x80000000000003DDull },
	{ 0x80001B9A01532CEDull, 0x80001B9A01532CEDull },
	{ 0x800000000F3893FCull, 0x800000000F3893FCull },
	{ 0x0001616677AD113Aull, 0x0001616677AD113Aull },
	{ 0x8000000000003DAFull, 0x8000000000003DAFull },
	{ 0x8000000000000016ull, 0x8000000000000016ull },
	{ 0x80000000000040E2ull, 0x80000000000040E2ull },
	{ 0x80000C8F8EE0ABC8ull, 0x80000C8F8EE0ABC8ull },
	{ 0x800000000000701Cull, 0x800000000000701Cull },
	{ 0x000000EE0844A26Eull, 0x000000EE0844A26Eull },
	{ 0x0000000000078A67ull, 0x0000000000078A67ull },
	{ 0x0000038F01952DE4ull, 0x0000038F01952DE4ull },
	{ 0x00000000000F12FBull, 0x00000000000F12FBull },
	{ 0x000001A7F8B94B21ull, 0x000001A7F8B94B21ull },
	{ 0x80000000000029CBull, 0x80000000000029CBull },
	{ 0x800000006BC5F87Bull, 0x800000006BC5F87Bull },
	{ 0x00000000001D1D9Aull, 0x00000000001D1D9Aull },
	{ 0x0000000A7BE0944Cull, 0x0000000A7BE0944Cull },
	{ 0x8000000000002D6Cull, 0x8000000000002D6Cull },
	{ 0x8000000003CA5915ull, 0x8000000003CA5915ull },
	{ 0x8000000000000077ull, 0x8000000000000077ull },
	{ 0x800000014EE4C1D6ull, 0x800000014EE4C1D6ull },
	{ 0x800000000000001Cull, 0x800000000000001Cull },
	{ 0x80000D04281A8B2Bull, 0x80000D04281A8B2Bull },
	{ 0x000000000000000Dull, 0x000000000000000Dull },
	{ 0x80000006FDC17818ull, 0x80000006FDC17818ull },
	{ 0x8000000000000014ull, 0x8000000000000014ull },
	{ 0x80000000034E3D50ull, 0x80000000034E3D50ull },
	{ 0x80002F6B6440C8E5ull, 0x80002F6B6440C8E5ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x0000000B30D8C188ull, 0x0000000B30D8C188ull },
	{ 0x000095A7BC121BBFull, 0x000095A7BC121BBFull },
	{ 0x8000001EEC8DB11Bull, 0x8000001EEC8DB11Bull },
	{ 0x00003CA87B0DE0BDull, 0x00003CA87B0DE0BDull },
	{ 0x80000000000171ACull, 0x80000000000171ACull },
	{ 0x800000D65FE74CF2ull, 0x800000D65FE74CF2ull },
	{ 0x00000000000001B5ull, 0x00000000000001B5ull },
	{ 0x000001CE92335F4Dull, 0x000001CE92335F4Dull },
	{ 0x800000355833D85Full, 0x800000355833D85Full },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x800000000AEDD722ull, 0x800000000AEDD722ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x800000000000001Full, 0x800000000000001Full },
	{ 0x0000000000001FE9ull, 0x0000000000001FE9ull },
	{ 0x0000000000001F57ull, 0x0000000000001F57ull },
	{ 0x000000000E7D3241ull, 0x000000000E7D3241ull },
	{ 0x80000000002A9C25ull, 0x80000000002A9C25ull },
	{ 0x0000000000DA21F0ull, 0x0000000000DA21F0ull },
	{ 0x80001FF1CE90A7F2ull, 0x80001FF1CE90A7F2ull },
	{ 0x800000089ED0CD12ull, 0x800000089ED0CD12ull },
	{ 0x8000000E7F6C34B4ull, 0x8000000E7F6C34B4ull },
	{ 0x00000000000001CEull, 0x00000000000001CEull },
	{ 0x8000000000000009ull, 0x8000000000000009ull },
	{ 0x800000000000015Aull, 0x800000000000015Aull },
	{ 0x0000000004240C90ull, 0x0000000004240C90ull },
	{ 0x8000000000913E91ull, 0x8000000000913E91ull },
	{ 0x0000000000038732ull, 0x0000000000038732ull },
	{ 0x800000063226C173ull, 0x800000063226C173ull },
	{ 0x0000044F11AEB52Eull, 0x0000044F11AEB52Eull },
	{ 0x0000000000000022ull, 0x0000000000000022ull },
	{ 0x00000000003BE0C9ull, 0x00000000003BE0C9ull },
	{ 0x80000005FCE79245ull, 0x80000005FCE79245ull },
	{ 0x0003F4BA2937CF2Aull, 0x0003F4BA2937CF2Aull },
	{ 0x800000000000000Dull, 0x800000000000000Dull },
	{ 0x8000000000000002ull, 0x8000000000000002ull },
	{ 0x8000000000000005ull, 0x8000000000000005ull },
	{ 0x80000000000002DAull, 0x80000000000002DAull },
	{ 0x8000000000C33782ull, 0x8000000000C33782ull },
	{ 0x8000000000001703ull, 0x8000000000001703ull },
	{ 0x00000000009EFA0Dull, 0x00000000009EFA0Dull },
	{ 0x8000000000A12F67ull, 0x8000000000A12F67ull },
	{ 0x8000000A33F594B5ull, 0x8000000A33F594B5ull },
	{ 0x00000000003E2630ull, 0x00000000003E2630ull },
	{ 0x00000F9DA00E9362ull, 0x00000F9DA00E9362ull },
	{ 0x800000000000CEE3ull, 0x800000000000CEE3ull },
	{ 0x800000003854423Dull, 0x800000003854423Dull },
	{ 0x00000004E519752Dull, 0x00000004E519752Dull },
	{ 0x8000000038F3672Cull, 0x8000000038F3672Cull },
	{ 0x8000000000004D39ull, 0x8000000000004D39ull },
	{ 0x0000036C44CB29A0ull, 0x0000036C44CB29A0ull },
	{ 0x800003B373875D31ull, 0x800003B373875D31ull },
	{ 0x8000000000000278ull, 0x8000000000000278ull },
	{ 0x80000005AEC6C29Dull, 0x80000005AEC6C29Dull },
	{ 0x00000001701FBEB2ull, 0x00000001701FBEB2ull },
	{ 0x800000000000002Cull, 0x800000000000002Cull },
	{ 0x800000006202A420ull, 0x800000006202A420ull },
	{ 0x00000005AE932ABBull, 0x00000005AE932ABBull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x800000000B534B76ull, 0x800000000B534B76ull },
	{ 0x00000DE9741BAAECull, 0x00000DE9741BAAECull },
	{ 0x80000000003320B4ull, 0x80000000003320B4ull },
	{ 0x8000000000B991F5ull, 0x8000000000B991F5ull },
	{ 0x800000070D0E5DB4ull, 0x800000070D0E5DB4ull },
	{ 0x80000000016603EFull, 0x80000000016603EFull },
	{ 0x80000001390432A1ull, 0x80000001390432A1ull },
	{ 0x80000210F42DE156ull, 0x80000210F42DE156ull },
	{ 0x8000000000000593ull, 0x8000000000000593ull },
	{ 0x800000004739A672ull, 0x800000004739A672ull },
	{ 0x8000000913F4F3B1ull, 0x8000000913F4F3B1ull },
	{ 0x00000000002113CFull, 0x00000000002113CFull },
	{ 0x8000000013B6571Eull, 0x8000000013B6571Eull },
	{ 0x000000001DCDE95Cull, 0x000000001DCDE95Cull },
	{ 0x000007903597125Full, 0x000007903597125Full },
	{ 0x0000000000000006ull, 0x0000000000000006ull },
	{ 0x8000000000001EA5ull, 0x8000000000001EA5ull },
	{ 0x800000000000FFB3ull, 0x800000000000FFB3ull },
	{ 0x800ADF1645AA564Aull, 0x800ADF1645AA564Aull },
	{ 0x800000000009CCE1ull, 0x800000000009CCE1ull },
	{ 0x0000000000002F48ull, 0x0000000000002F48ull },
	{ 0x8000000000000034ull, 0x8000000000000034ull },
	{ 0x0000000008643B0Aull, 0x0000000008643B0Aull },
	{ 0x0000002884275448ull, 0x0000002884275448ull },
	{ 0x000000000000003Cull, 0x000000000000003Cull },
	{ 0x000000000362A3D4ull, 0x000000000362A3D4ull },
	{ 0x80000003B7340B81ull, 0x80000003B7340B81ull },
	{ 0x80000000AB22E7F3ull, 0x80000000AB22E7F3ull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x8000000000000193ull, 0x8000000000000193ull },
	{ 0x000000000001B1CFull, 0x000000000001B1CFull },
	{ 0x8000000000000004ull, 0x8000000000000004ull },
	{ 0x00000000013F1C52ull, 0x00000000013F1C52ull },
	{ 0x0000000000001703ull, 0x0000000000001703ull },
	{ 0x800000000000175Dull, 0x800000000000175Dull },
	{ 0x00000000000A99CEull, 0x00000000000A99CEull },
	{ 0x80000330CE0AABF5ull, 0x80000330CE0AABF5ull },
	{ 0x00000DDA6C63A645ull, 0x00000DDA6C63A645ull },
	{ 0x000014182A929DCAull, 0x000014182A929DCAull },
	{ 0x00012B0C4BD97E2Cull, 0x00012B0C4BD97E2Cull },
	{ 0x000003A7F610F248ull, 0x000003A7F610F248ull },
	{ 0x000000000000082Eull, 0x000000000000082Eull },
	{ 0x00000000000F75A6ull, 0x00000000000F75A6ull },
	{ 0x00003731303589B0ull, 0x00003731303589B0ull },
	{ 0x0000000007CC0ED5ull, 0x0000000007CC0ED5ull },
	{ 0x0000000005382803ull, 0x0000000005382803ull },
	{ 0x8000000000000004ull, 0x8000000000000004ull },
	{ 0x800000397AA5C160ull, 0x800000397AA5C160ull },
	{ 0x00000000000000F9ull, 0x00000000000000F9ull },
	{ 0x8000000000000BF8ull, 0x8000000000000BF8ull },
	{ 0x800000000000F63Eull, 0x800000000000F63Eull },
	{ 0x80000000003EB5D3ull, 0x80000000003EB5D3ull },
	{ 0x80000006E4C54C1Bull, 0x80000006E4C54C1Bull },
	{ 0x8001444BB274FF3Cull, 0x8001444BB274FF3Cull },
	{ 0x800000000000001Cull, 0x800000000000001Cull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x80000000000008DFull, 0x80000000000008DFull },
	{ 0x000000001C3C6795ull, 0x000000001C3C6795ull },
	{ 0x0000D95F588D618Full, 0x0000D95F588D618Full },
	{ 0x800000000C89788Cull, 0x800000000C89788Cull },
	{ 0x8000001688867D10ull, 0x8000001688867D10ull },
	{ 0x00000D59AAB12CE5ull, 0x00000D59AAB12CE5ull },
	{ 0x0000000010EC179Cull, 0x0000000010EC179Cull },
	{ 0x80000010E2713B8Eull, 0x80000010E2713B8Eull },
	{ 0x00000000000000D9ull, 0x00000000000000D9ull },
	{ 0x80000000B86DB083ull, 0x80000000B86DB083ull },
	{ 0x80000261A8623A3Eull, 0x80000261A8623A3Eull },
	{ 0x80000002DD66C8E7ull, 0x80000002DD66C8E7ull },
	{ 0x800000000000A4EBull, 0x800000000000A4EBull },
	{ 0x0000000000000008ull, 0x0000000000000008ull },
	{ 0x000000000000021Aull, 0x000000000000021Aull },
	{ 0x0000000000015AB0ull, 0x0000000000015AB0ull },
	{ 0x800000032F9F20B3ull, 0x800000032F9F20B3ull },
	{ 0x80000000000002B7ull, 0x80000000000002B7ull },
	{ 0x0000001552C9DA03ull, 0x0000001552C9DA03ull },
	{ 0x80000000000083DAull, 0x80000000000083DAull },
	{ 0x000000004C784F82ull, 0x000000004C784F82ull },
	{ 0x8000672FAD129E37ull, 0x8000672FAD129E37ull },
	{ 0x80069EE8803430DDull, 0x80069EE8803430DDull },
	{ 0x800001F9023067D4ull, 0x800001F9023067D4ull },
	{ 0x8000000000000250ull, 0x8000000000000250ull },
	{ 0x00000000A6391E04ull, 0x00000000A6391E04ull },
	{ 0x00000000007CDB4Aull, 0x00000000007CDB4Aull },
	{ 0x8000000000001C09ull, 0x8000000000001C09ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x80007B97B9124784ull, 0x80007B97B9124784ull },
	{ 0x8000000B94DC3A14ull, 0x8000000B94DC3A14ull },
	{ 0x000000000000005Cull, 0x000000000000005Cull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x0000000000000004ull, 0x0000000000000004ull },
	{ 0x0000000000000071ull, 0x0000000000000071ull },
	{ 0x8000000012DFD43Aull, 0x8000000012DFD43Aull },
	{ 0x0000062A8C1EBEE9ull, 0x0000062A8C1EBEE9ull },
	{ 0x80000000A63C59BBull, 0x80000000A63C59BBull },
	{ 0x000000000005F627ull, 0x000000000005F627ull },
	{ 0x800041D0180445DAull, 0x800041D0180445DAull },
	{ 0x800000000000C019ull, 0x800000000000C019ull },
	{ 0x000001C498A427E0ull, 0x000001C498A427E0ull },
	{ 0x0000000002BBA3E7ull, 0x0000000002BBA3E7ull },
	{ 0x80000000108E3FB6ull, 0x80000000108E3FB6ull },
	{ 0x0000000046D59193ull, 0x0000000046D59193ull },
	{ 0x000000000001AAD8ull, 0x000000000001AAD8ull },
	{ 0x0000000B84238361ull, 0x0000000B84238361ull },
	{ 0x0000000000943539ull, 0x0000000000943539ull },
	{ 0x000000024068C323ull, 0x000000024068C323ull },
	{ 0x000000036D6B1AB8ull, 0x000000036D6B1AB8ull },
	{ 0x80000000000001CDull, 0x80000000000001CDull },
	{ 0x8000000000007E85ull, 0x8000000000007E85ull },
	{ 0x0000000000000238ull, 0x0000000000000238ull },
	{ 0x0000007BCF3B1F0Dull, 0x0000007BCF3B1F0Dull },
	{ 0x8000000000000D6Eull, 0x8000000000000D6Eull },
	{ 0x8001F686694274CCull, 0x8001F686694274CCull },
	{ 0x000186A965DBBFC7ull, 0x000186A965DBBFC7ull },
	{ 0x80000018C858C604ull, 0x80000018C858C604ull },
	{ 0x000002D2B80EA046ull, 0x000002D2B80EA046ull },
	{ 0x800199B2EB2F9BE2ull, 0x800199B2EB2F9BE2ull },
	{ 0x8000000000000090ull, 0x8000000000000090ull },
	{ 0x800000069B8B17B0ull, 0x800000069B8B17B0ull },
	{ 0x000029050D2D099Eull, 0x000029050D2D099Eull },
	{ 0x00000000019A8184ull, 0x00000000019A8184ull },
	{ 0x000000000060EEAFull, 0x000000000060EEAFull },
	{ 0x8000000000001862ull, 0x8000000000001862ull },
	{ 0x0000000000002E62ull, 0x0000000000002E62ull },
	{ 0x8000000000000028ull, 0x8000000000000028ull },
	{ 0x800000000001A883ull, 0x800000000001A883ull },
	{ 0x80000000000184D3ull, 0x80000000000184D3ull },
	{ 0x00000000008BB0ECull, 0x00000000008BB0ECull },
	{ 0x8000000012A4E53Cull, 0x8000000012A4E53Cull },
	{ 0x8000001316A3AD39ull, 0x8000001316A3AD39ull },
	{ 0x00000000DAA73448ull, 0x00000000DAA73448ull },
	{ 0x0000000000000692ull, 0x0000000000000692ull },
	{ 0x80007CED17C370EFull, 0x80007CED17C370EFull },
	{ 0x0000000000000067ull, 0x0000000000000067ull },
	{ 0x00001DCF0BAF095Aull, 0x00001DCF0BAF095Aull },
	{ 0x800000000485CED6ull, 0x800000000485CED6ull },
	{ 0x0000000000001179ull, 0x0000000000001179ull },
	{ 0x0000000D9AF93152ull, 0x0000000D9AF93152ull },
	{ 0x000000074C1B9C71ull, 0x000000074C1B9C71ull },
	{ 0x0000000000230512ull, 0x0000000000230512ull },
	{ 0x80000B7FE6C835ACull, 0x80000B7FE6C835ACull },
	{ 0x0000000000059DC2ull, 0x0000000000059DC2ull },
	{ 0x800000000853C9E4ull, 0x800000000853C9E4ull },
	{ 0x80000000000003ADull, 0x80000000000003ADull },
	{ 0x0000000000000053ull, 0x0000000000000053ull },
	{ 0x80000001E6DE7C52ull, 0x80000001E6DE7C52ull },
	{ 0x0000000003ADD8DCull, 0x0000000003ADD8DCull },
	{ 0x8000000000000B9Full, 0x8000000000000B9Full },
	{ 0x0000000000000D5Aull, 0x0000000000000D5Aull },
	{ 0x8000084EC98BC9FEull, 0x8000084EC98BC9FEull },
	{ 0x000000000000049Cull, 0x000000000000049Cull },
	{ 0x800004ADD9623163ull, 0x800004ADD9623163ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x8003294C448549BEull, 0x8003294C448549BEull },
	{ 0x8000000A4B278E02ull, 0x8000000A4B278E02ull },
	{ 0x800001D28FCEAC3Full, 0x800001D28FCEAC3Full },
	{ 0x8000004F7E1A926Dull, 0x8000004F7E1A926Dull },
	{ 0x8000000000B44112ull, 0x8000000000B44112ull },
	{ 0x800000000000003Eull, 0x800000000000003Eull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x800000006E95844Eull, 0x800000006E95844Eull },
	{ 0x8000009BF629D48Full, 0x8000009BF629D48Full },
	{ 0x0000000000000262ull, 0x0000000000000262ull },
	{ 0x000131E9EE70A4E8ull, 0x000131E9EE70A4E8ull },
	{ 0x0000062A1F6F0320ull, 0x0000062A1F6F0320ull },
	{ 0x8000000015B34A33ull, 0x8000000015B34A33ull },
	{ 0x80000000012B5499ull, 0x80000000012B5499ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x0000000000111FE5ull, 0x0000000000111FE5ull },
	{ 0x000000000175388Cull, 0x000000000175388Cull },
	{ 0x800000F08E357882ull, 0x800000F08E357882ull },
	{ 0x80000000000024A0ull, 0x80000000000024A0ull },
	{ 0x8000002AA8072374ull, 0x8000002AA8072374ull },
	{ 0x0000000000AB9951ull, 0x0000000000AB9951ull },
	{ 0x00000D8245783561ull, 0x00000D8245783561ull },
	{ 0x00000000000D8A74ull, 0x00000000000D8A74ull },
	{ 0x000000000000B7CAull, 0x000000000000B7CAull },
	{ 0x80087CEE4F2C312Dull, 0x80087CEE4F2C312Dull },
	{ 0x0000036BCE70ED42ull, 0x0000036BCE70ED42ull },
	{ 0x00000000001B8DC8ull, 0x00000000001B8DC8ull },
	{ 0x0000003DE16C4177ull, 0x0000003DE16C4177ull },
	{ 0x8000000000002FFFull, 0x8000000000002FFFull },
	{ 0x800000000004FDC7ull, 0x800000000004FDC7ull },
	{ 0x80000000068011B4ull, 0x80000000068011B4ull },
	{ 0x00035CEC4AAC8615ull, 0x00035CEC4AAC8615ull },
	{ 0x00006D82C0397DBFull, 0x00006D82C0397DBFull },
	{ 0x8000000000000030ull, 0x8000000000000030ull },
	{ 0x80000000005DB8D2ull, 0x80000000005DB8D2ull },
	{ 0x00000001725685CDull, 0x00000001725685CDull },
	{ 0x000000178F2ECC9Dull, 0x000000178F2ECC9Dull },
	{ 0x80000CC57734DC51ull, 0x80000CC57734DC51ull },
	{ 0x800000018931F92Aull, 0x800000018931F92Aull },
	{ 0x800000000000003Cull, 0x800000000000003Cull },
	{ 0x800000000C747817ull, 0x800000000C747817ull },
	{ 0x000000000000574Eull, 0x000000000000574Eull },
	{ 0x000000B1FF976812ull, 0x000000B1FF976812ull },
	{ 0x0000000000000030ull, 0x0000000000000030ull },
	{ 0x80000000149D6FF4ull, 0x80000000149D6FF4ull },
	{ 0x8000000008B33B34ull, 0x8000000008B33B34ull },
	{ 0x8000000000348383ull, 0x8000000000348383ull },
	{ 0x80000000077873ADull, 0x80000000077873ADull },
	{ 0x0000000000475E2Full, 0x0000000000475E2Full },
	{ 0x80002A00BBF4F20Bull, 0x80002A00BBF4F20Bull },
	{ 0x8000126AB73075D4ull, 0x8000126AB73075D4ull },
	{ 0x0000000000000009ull, 0x0000000000000009ull },
	{ 0x0000000000000007ull, 0x0000000000000007ull },
	{ 0x0000000000004959ull, 0x0000000000004959ull },
	{ 0x8000000000003C2Full, 0x8000000000003C2Full },
	{ 0x8000000BBC231E1Aull, 0x8000000BBC231E1Aull },
	{ 0x000014B865BEC13Full, 0x000014B865BEC13Full },
	{ 0x0000171039D41107ull, 0x0000171039D41107ull },
	{ 0x800000000000005Bull, 0x800000000000005Bull },
	{ 0x8000000164C1C6A9ull, 0x8000000164C1C6A9ull },
	{ 0x0000000000024200ull, 0x0000000000024200ull },
	{ 0x0000001E77258938ull, 0x0000001E77258938ull },
	{ 0x00000001575E5FD2ull, 0x00000001575E5FD2ull },
	{ 0x0000000000000524ull, 0x0000000000000524ull },
	{ 0x800000C090131A36ull, 0x800000C090131A36ull },
	{ 0x80000457E4FFF460ull, 0x80000457E4FFF460ull },
	{ 0x00000225A7C64F1Full, 0x00000225A7C64F1Full },
	{ 0x0000000000020100ull, 0x0000000000020100ull },
	{ 0x8000000000013BABull, 0x8000000000013BABull },
	{ 0x8000000000000008ull, 0x8000000000000008ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x80001C669F8B05E2ull, 0x80001C669F8B05E2ull },
	{ 0x800000000003B030ull, 0x800000000003B030ull },
	{ 0x0000000072F40BCCull, 0x0000000072F40BCCull },
	{ 0x800000000C6FC4EDull, 0x800000000C6FC4EDull },
	{ 0x000036CBDA2216A2ull, 0x000036CBDA2216A2ull },
	{ 0x00000000DBE8AACEull, 0x00000000DBE8AACEull },
	{ 0x000000000000001Full, 0x000000000000001Full },
	{ 0x00000079006A90A3ull, 0x00000079006A90A3ull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x000000000000003Cull, 0x000000000000003Cull },
	{ 0x0000000000AB6B45ull, 0x0000000000AB6B45ull },
	{ 0x000000000000DC84ull, 0x000000000000DC84ull },
	{ 0x0000000000000DF7ull, 0x0000000000000DF7ull },
	{ 0x000000F856313F1Eull, 0x000000F856313F1Eull },
	{ 0x0000000000000E36ull, 0x0000000000000E36ull },
	{ 0x0000000000000003ull, 0x0000000000000003ull },
	{ 0x00003A3EB42DF3E5ull, 0x00003A3EB42DF3E5ull },
	{ 0x8000303C4103E7BFull, 0x8000303C4103E7BFull },
	{ 0x80000000002AE9AFull, 0x80000000002AE9AFull },
	{ 0x00000000000007C0ull, 0x00000000000007C0ull },
	{ 0x000098166EDDDDC5ull, 0x000098166EDDDDC5ull },
	{ 0x800000000000003Eull, 0x800000000000003Eull },
	{ 0x8000000000000891ull, 0x8000000000000891ull },
	{ 0x800000000000003Bull, 0x800000000000003Bull },
	{ 0x8000000000013CF7ull, 0x8000000000013CF7ull },
	{ 0x8000000000000F4Eull, 0x8000000000000F4Eull },
	{ 0x00000003022819EBull, 0x00000003022819EBull },
	{ 0x8000000000C64EA6ull, 0x8000000000C64EA6ull },
	{ 0x8000000000000C73ull, 0x8000000000000C73ull },
	{ 0x0000000001DF1F06ull, 0x0000000001DF1F06ull },
	{ 0x8000000370B4AF13ull, 0x8000000370B4AF13ull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x00000002AC9B88E6ull, 0x00000002AC9B88E6ull },
	{ 0x80000000000000B0ull, 0x80000000000000B0ull },
	{ 0x000000000000000Eull, 0x000000000000000Eull },
	{ 0x800000BE9DB32AD8ull, 0x800000BE9DB32AD8ull },
	{ 0x800000000FDE8101ull, 0x800000000FDE8101ull },
	{ 0x8000000372F01356ull, 0x8000000372F01356ull },
	{ 0x0000000000000016ull, 0x0000000000000016ull },
	{ 0x800051A1ED8E91E5ull, 0x800051A1ED8E91E5ull },
	{ 0x800000995939E802ull, 0x800000995939E802ull },
	{ 0x800000004E9D4F0Dull, 0x800000004E9D4F0Dull },
	{ 0x80000000000D6857ull, 0x80000000000D6857ull },
	{ 0x0000001DFD36D8A3ull, 0x0000001DFD36D8A3ull },
	{ 0x80000000003C7078ull, 0x80000000003C7078ull },
	{ 0x0000006683D684A9ull, 0x0000006683D684A9ull },
	{ 0x800000000000069Cull, 0x800000000000069Cull },
	{ 0x00000000000001DCull, 0x00000000000001DCull },
	{ 0x800000000000006Aull, 0x800000000000006Aull },
	{ 0x00000000000000C2ull, 0x00000000000000C2ull },
	{ 0x00005209221FED74ull, 0x00005209221FED74ull },
	{ 0x000000000000F7D4ull, 0x000000000000F7D4ull },
	{ 0x00000019CDA67B24ull, 0x00000019CDA67B24ull },
	{ 0x00000000000003FDull, 0x00000000000003FDull },
	{ 0x000000000166B472ull, 0x000000000166B472ull },
	{ 0x000000000000116Eull, 0x000000000000116Eull },
	{ 0x0000000076FEBEF5ull, 0x0000000076FEBEF5ull },
	{ 0x0000023838D163B4ull, 0x0000023838D163B4ull },
	{ 0x0000000000303969ull, 0x0000000000303969ull },
	{ 0x8000000000002A28ull, 0x8000000000002A28ull },
	{ 0x0000000000002A8Cull, 0x0000000000002A8Cull },
	{ 0x80000000000BD99Eull, 0x80000000000BD99Eull },
	{ 0x800000000003ED2Bull, 0x800000000003ED2Bull },
	{ 0x8000B15070E03703ull, 0x8000B15070E03703ull },
	{ 0x00000000012F3531ull, 0x00000000012F3531ull },
	{ 0x8000000007244EF2ull, 0x8000000007244EF2ull },
	{ 0x800000000000168Eull, 0x800000000000168Eull },
	{ 0x8000000003A04908ull, 0x8000000003A04908ull },
	{ 0x800000000025514Full, 0x800000000025514Full },
	{ 0x80000000001C81D6ull, 0x80000000001C81D6ull },
	{ 0x800000000044D406ull, 0x800000000044D406ull },
	{ 0x00000D504AB129A5ull, 0x00000D504AB129A5ull },
	{ 0x800000036E965636ull, 0x800000036E965636ull },
	{ 0x8000FECA65C30623ull, 0x8000FECA65C30623ull },
	{ 0x80000000002B2F63ull, 0x80000000002B2F63ull },
	{ 0x000000000A76E4A4ull, 0x000000000A76E4A4ull },
	{ 0x00000577F37634DDull, 0x00000577F37634DDull },
	{ 0x00001C31F4EE5605ull, 0x00001C31F4EE5605ull },
	{ 0x00000000000003CEull, 0x00000000000003CEull },
	{ 0x0000000000000016ull, 0x0000000000000016ull },
	{ 0x0000000011620417ull, 0x0000000011620417ull },
	{ 0x8000000000006F04ull, 0x8000000000006F04ull },
	{ 0x0000000000000131ull, 0x0000000000000131ull },
	{ 0x0000000000009849ull, 0x0000000000009849ull },
	{ 0x000B39BB8C21F12Dull, 0x000B39BB8C21F12Dull },
	{ 0x0000000031C2D90Bull, 0x0000000031C2D90Bull },
	{ 0x8000000000083BB7ull, 0x8000000000083BB7ull },
	{ 0x00000000000001ABull, 0x00000000000001ABull },
	{ 0x800003CF6DF31577ull, 0x800003CF6DF31577ull },
	{ 0x0000000004BA819Full, 0x0000000004BA819Full },
	{ 0x80000000006AD886ull, 0x80000000006AD886ull },
	{ 0x00000001AE5C5692ull, 0x00000001AE5C5692ull },
	{ 0x8000000000000004ull, 0x8000000000000004ull },
	{ 0x800000000151273Full, 0x800000000151273Full },
	{ 0x000001C4AD5E4209ull, 0x000001C4AD5E4209ull },
	{ 0x800001C048CC60C6ull, 0x800001C048CC60C6ull },
	{ 0x000000000260688Full, 0x000000000260688Full },
	{ 0x0000000000038A61ull, 0x0000000000038A61ull },
	{ 0x800028B17E8E6F3Full, 0x800028B17E8E6F3Full },
	{ 0x80000000005B3CC1ull, 0x80000000005B3CC1ull },
	{ 0x00000000000003D0ull, 0x00000000000003D0ull },
	{ 0x00000013D46CB91Dull, 0x00000013D46CB91Dull },
	{ 0x000000001761F229ull, 0x000000001761F229ull },
	{ 0x00000000000000EFull, 0x00000000000000EFull },
	{ 0x00000000000000E3ull, 0x00000000000000E3ull },
	{ 0x8000000000000018ull, 0x8000000000000018ull },
	{ 0x8000000000002AC7ull, 0x8000000000002AC7ull },
	{ 0x800038B2ED08FB23ull, 0x800038B2ED08FB23ull },
	{ 0x000000000000002Aull, 0x000000000000002Aull },
	{ 0x8000000006C0E530ull, 0x8000000006C0E530ull },
	{ 0x800000002DC3B4ADull, 0x800000002DC3B4ADull },
	{ 0x0000000000037B3Dull, 0x0000000000037B3Dull },
	{ 0x8004BD5BB584589Eull, 0x8004BD5BB584589Eull },
	{ 0x800000000547ED71ull, 0x800000000547ED71ull },
	{ 0x80000000013B1D98ull, 0x80000000013B1D98ull },
	{ 0x800130945C8E1B4Eull, 0x800130945C8E1B4Eull },
	{ 0x80000000000B6F01ull, 0x80000000000B6F01ull },
	{ 0x0000000000000009ull, 0x0000000000000009ull },
	{ 0x800000000001EF84ull, 0x800000000001EF84ull },
	{ 0x80000000000003ECull, 0x80000000000003ECull },
	{ 0x00000000007DE93Dull, 0x00000000007DE93Dull },
	{ 0x00000000005F743Full, 0x00000000005F743Full },
	{ 0x0000000000000013ull, 0x0000000000000013ull },
	{ 0x800000000000008Dull, 0x800000000000008Dull },
	{ 0x8000000000084EBEull, 0x8000000000084EBEull },
	{ 0x800BC621E0C0E4B8ull, 0x800BC621E0C0E4B8ull },
	{ 0x800007F2E85D8CA0ull, 0x800007F2E85D8CA0ull },
	{ 0x800000000000000Eull, 0x800000000000000Eull },
	{ 0x80000000000000BBull, 0x80000000000000BBull },
	{ 0x800000000BD7ABC3ull, 0x800000000BD7ABC3ull },
	{ 0x0000000000000018ull, 0x0000000000000018ull },
	{ 0x00000000000057B1ull, 0x00000000000057B1ull },
	{ 0x8000000000000351ull, 0x8000000000000351ull },
	{ 0x800000000000039Full, 0x800000000000039Full },
	{ 0x000000000D9F4D40ull, 0x000000000D9F4D40ull },
	{ 0x0000004803F8DA4Eull, 0x0000004803F8DA4Eull },
	{ 0x8000000000000010ull, 0x8000000000000010ull },
	{ 0x00093B476DA37659ull, 0x00093B476DA37659ull },
	{ 0x8000000000012547ull, 0x8000000000012547ull },
	{ 0x8000000000001570ull, 0x8000000000001570ull },
	{ 0x000000000009061Dull, 0x000000000009061Dull },
	{ 0x800000000000B597ull, 0x800000000000B597ull },
	{ 0x0000000006A6BD3Aull, 0x0000000006A6BD3Aull },
	{ 0x0000000A8083642Eull, 0x0000000A8083642Eull },
	{ 0x00000000005973F5ull, 0x00000000005973F5ull },
	{ 0x8000010BADCF3E69ull, 0x8000010BADCF3E69ull },
	{ 0x80031C0D3A214DEAull, 0x80031C0D3A214DEAull },
	{ 0x000000000016B531ull, 0x000000000016B531ull },
	{ 0x8000000000002998ull, 0x8000000000002998ull },
	{ 0x0000000085F88DBAull, 0x0000000085F88DBAull },
	{ 0x8000000000000003ull, 0x8000000000000003ull },
	{ 0x00000000004D56C5ull, 0x00000000004D56C5ull },
	{ 0x80007666159FB751ull, 0x80007666159FB751ull },
	{ 0x0000045C5587C7EAull, 0x0000045C5587C7EAull },
	{ 0x80000000108D2559ull, 0x80000000108D2559ull },
	{ 0x0000000000CE6FCBull, 0x0000000000CE6FCBull },
	{ 0x00046B457A581E7Bull, 0x00046B457A581E7Bull },
	{ 0x800000000000379Bull, 0x800000000000379Bull },
	{ 0x80000000CACCDE96ull, 0x80000000CACCDE96ull },
	{ 0x0000000180B2190Full, 0x0000000180B2190Full },
	{ 0x0000000000002BF3ull, 0x0000000000002BF3ull },
	{ 0x00000001A5513060ull, 0x00000001A5513060ull },
	{ 0x00000008EC737BFEull, 0x00000008EC737BFEull },
	{ 0x0000089505360C20ull, 0x0000089505360C20ull },
	{ 0x8000000000311680ull, 0x8000000000311680ull },
	{ 0x80000E0F696B444Aull, 0x80000E0F696B444Aull },
	{ 0x8001D9DA137C9B83ull, 0x8001D9DA137C9B83ull },
	{ 0x8000000000000821ull, 0x8000000000000821ull },
	{ 0x800734DE44B9B861ull, 0x800734DE44B9B861ull },
	{ 0x0000000004886A83ull, 0x0000000004886A83ull },
	{ 0x80000000000182DCull, 0x80000000000182DCull },
	{ 0x80000000001104AFull, 0x80000000001104AFull },
	{ 0x0000003A58B319A5ull, 0x0000003A58B319A5ull },
	{ 0x000000881D65FBD5ull, 0x000000881D65FBD5ull },
	{ 0x800000000000057Eull, 0x800000000000057Eull },
	{ 0x800000000038EA09ull, 0x800000000038EA09ull },
	{ 0x000007818392F900ull, 0x000007818392F900ull },
	{ 0x0000000000000005ull, 0x0000000000000005ull },
	{ 0x000199FB6567B6A1ull, 0x000199FB6567B6A1ull },
	{ 0x80000000CDABFED1ull, 0x80000000CDABFED1ull },
	{ 0x0000000000000006ull, 0x0000000000000006ull },
	{ 0x8000000000000020ull, 0x8000000000000020ull },
	{ 0x8000000AFBA2B599ull, 0x8000000AFBA2B599ull },
	{ 0x8000000000000006ull, 0x8000000000000006ull },
	{ 0x000001F90FF97A2Full, 0x000001F90FF97A2Full },
	{ 0x80000000197E60D3ull, 0x80000000197E60D3ull },
	{ 0x0000000001C36064ull, 0x0000000001C36064ull },
	{ 0x0000000002FFD9F1ull, 0x0000000002FFD9F1ull },
	{ 0x0000004F212C3912ull, 0x0000004F212C3912ull },
	{ 0x8000000000000002ull, 0x8000000000000002ull },
	{ 0x000045B557C2C467ull, 0x000045B557C2C467ull },
	{ 0x8000000000000006ull, 0x8000000000000006ull },
	{ 0x800000048C62DF8Eull, 0x800000048C62DF8Eull },
	{ 0x800000000077B5CCull, 0x800000000077B5CCull },
	{ 0x80000030D13F5486ull, 0x80000030D13F5486ull },
	{ 0x000002FD27C2CA5Cull, 0x000002FD27C2CA5Cull },
	{ 0x80000002D92B0780ull, 0x80000002D92B0780ull },
	{ 0x80000000A8CDCBADull, 0x80000000A8CDCBADull },
	{ 0x000000000052013Eull, 0x000000000052013Eull },
	{ 0x800000002D795A5Bull, 0x800000002D795A5Bull },
	{ 0x800000000032F27Eull, 0x800000000032F27Eull },
	{ 0x00000001C2041ABEull, 0x00000001C2041ABEull },
	{ 0x0000000000000030ull, 0x0000000000000030ull },
	{ 0x0000000000000098ull, 0x0000000000000098ull },
	{ 0x800000006E3382A4ull, 0x800000006E3382A4ull },
	{ 0x0000000000000196ull, 0x0000000000000196ull },
	{ 0x00000B525B43D2B0ull, 0x00000B525B43D2B0ull },
	{ 0x0000000000000001ull, 0x0000000000000001ull },
	{ 0x000000033B2ADF00ull, 0x000000033B2ADF00ull },
	{ 0x000000039C6117F1ull, 0x000000039C6117F1ull },
	{ 0x800004058AE56166ull, 0x800004058AE56166ull },
	{ 0x80000E0CABACA4EEull, 0x80000E0CABACA4EEull },
	{ 0x00000000000005EAull, 0x00000000000005EAull },
	{ 0x00000000010EE8A0ull, 0x00000000010EE8A0ull },
	{ 0x80000000000001A8ull, 0x80000000000001A8ull },
	{ 0x000000000000003Cull, 0x000000000000003Cull },
	{ 0x800002A9CE7BA883ull, 0x800002A9CE7BA883ull },
	{ 0x00000000000007BBull, 0x00000000000007BBull },
	{ 0x0000000000000D86ull, 0x0000000000000D86ull },
	{ 0x8000FDE474B8EFFEull, 0x8000FDE474B8EFFEull },
	{ 0x80000000001852AAull, 0x80000000001852AAull },
	{ 0x0000000000009DB0ull, 0x0000000000009DB0ull },
	{ 0x800000000001AEEEull, 0x800000000001AEEEull },
	{ 0x800000000000EA6Cull, 0x800000000000EA6Cull },
	{ 0x80000000001C4CC4ull, 0x80000000001C4CC4ull },
	{ 0x0002B777849B8470ull, 0x0002B777849B8470ull },
	{ 0x8000000000FE166Cull, 0x8000000000FE166Cull },
	{ 0x8000167E833CE0F4ull, 0x8000167E833CE0F4ull },
	{ 0x800000036B8AE2F6ull, 0x800000036B8AE2F6ull },
	{ 0x0000000000033353ull, 0x0000000000033353ull },
	{ 0x0000028FC382EFA8ull, 0x0000028FC382EFA8ull },
	{ 0x800000033DC479D2ull, 0x800000033DC479D2ull },
	{ 0x8000000004C662CFull, 0x8000000004C662CFull },
	{ 0x80000000009B87C9ull, 0x80000000009B87C9ull },
	{ 0x80000000000004C7ull, 0x80000000000004C7ull },
	{ 0x800000000000D833ull, 0x800000000000D833ull },
	{ 0x800000000000000Bull, 0x800000000000000Bull },
	{ 0x80000E658BE8C489ull, 0x80000E658BE8C489ull },
	{ 0x80000000FCFDA79Cull, 0x80000000FCFDA79Cull },
	{ 0x00000D3EF0DA058Cull, 0x00000D3EF0DA058Cull },
	{ 0x80000000006A4B31ull, 0x80000000006A4B31ull },
	{ 0x800000000000000Eull, 0x800000000000000Eull },
	{ 0x0000002DF2923249ull, 0x0000002DF2923249ull },
	{ 0x800000002002D24Full, 0x800000002002D24Full },
	{ 0x8000000000000006ull, 0x8000000000000006ull },
	{ 0x000001A33247852Eull, 0x000001A33247852Eull },
	{ 0x800000000001E8E7ull, 0x800000000001E8E7ull },
	{ 0x80000000000143F3ull, 0x80000000000143F3ull },
	{ 0x80000000000B5165ull, 0x80000000000B5165ull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x000D2D59A66412A2ull, 0x000D2D59A66412A2ull },
	{ 0x0000B1AF7ADFA512ull, 0x0000B1AF7ADFA512ull },
	{ 0x8001478F535B3539ull, 0x8001478F535B3539ull },
	{ 0x000000000074944Full, 0x000000000074944Full },
	{ 0x0000001570E3ADDFull, 0x0000001570E3ADDFull },
	{ 0x00000005DDFE56A4ull, 0x00000005DDFE56A4ull },
	{ 0x000005785F65F8B5ull, 0x000005785F65F8B5ull },
	{ 0x800000000009D30Full, 0x800000000009D30Full },
	{ 0x800003D204D8AF99ull, 0x800003D204D8AF99ull },
	{ 0x0000000692123D68ull, 0x0000000692123D68ull },
	{ 0x000000000000002Bull, 0x000000000000002Bull },
	{ 0x00001C245F006000ull, 0x00001C245F006000ull },
	{ 0x8000033A44FAC960ull, 0x8000033A44FAC960ull },
	{ 0x800000001F5D412Aull, 0x800000001F5D412Aull },
	{ 0x0000000005540B21ull, 0x0000000005540B21ull },
	{ 0x0000000561542194ull, 0x0000000561542194ull },
	{ 0x800000000009F248ull, 0x800000000009F248ull },
	{ 0x800000091B68F581ull, 0x800000091B68F581ull },
	{ 0x800000589C684B5Dull, 0x800000589C684B5Dull },
	{ 0x8000000000C39304ull, 0x8000000000C39304ull },
	{ 0x800000001DF2E85Full, 0x800000001DF2E85Full },
	{ 0x800000036D2B5ED6ull, 0x800000036D2B5ED6ull },
	{ 0x8000000359AC6558ull, 0x8000000359AC6558ull },
	{ 0x0000000000003470ull, 0x0000000000003470ull },
	{ 0x000000000001C011ull, 0x000000000001C011ull },
	{ 0x80000672F776E40Dull, 0x80000672F776E40Dull },
	{ 0x000003EB09677D95ull, 0x000003EB09677D95ull },
	{ 0x0000000001F04930ull, 0x0000000001F04930ull },
	{ 0x8000003D6604BF77ull, 0x8000003D6604BF77ull },
	{ 0x800035A88CDA1444ull, 0x800035A88CDA1444ull },
	{ 0x8000000000000B7Dull, 0x8000000000000B7Dull },
	{ 0x0000000000003EC1ull, 0x0000000000003EC1ull },
	{ 0x0000000000000104ull, 0x0000000000000104ull },
	{ 0x000004A4632DFCCBull, 0x000004A4632DFCCBull },
	{ 0x000000000000006Bull, 0x000000000000006Bull },
	{ 0x00000002FA0DB0A5ull, 0x00000002FA0DB0A5ull },
	{ 0x800000000000944Bull, 0x800000000000944Bull },
	{ 0x0000000000028D79ull, 0x0000000000028D79ull },
	{ 0x800000000041DD57ull, 0x800000000041DD57ull },
	{ 0x800000065F4DBB55ull, 0x800000065F4DBB55ull },
	{ 0x0000000000009C18ull, 0x0000000000009C18ull },
	{ 0x00004ACF46D24B35ull, 0x00004ACF46D24B35ull },
	{ 0x80000000000082A0ull, 0x80000000000082A0ull },
	{ 0x8000000AA0759693ull, 0x8000000AA0759693ull },
	{ 0x00000000003C733Dull, 0x00000000003C733Dull },
	{ 0x0000000051BFB66Dull, 0x0000000051BFB66Dull },
	{ 0x000000000000046Bull, 0x000000000000046Bull },
	{ 0x00000014CFEE9D95ull, 0x00000014CFEE9D95ull },
	{ 0x800000000B6FAD36ull, 0x800000000B6FAD36ull },
	{ 0x000000000000012Eull, 0x000000000000012Eull },
	{ 0x0000000003CC22FBull, 0x0000000003CC22FBull },
	{ 0x8000003415AE3CBCull, 0x8000003415AE3CBCull },
	{ 0x80000000002DD2BBull, 0x80000000002DD2BBull },
	{ 0x00000000CA10F0E2ull, 0x00000000CA10F0E2ull },
	{ 0x0000000000005D3Bull, 0x0000000000005D3Bull },
	{ 0x8000000000001B89ull, 0x8000000000001B89ull },
	{ 0x000000000000AE8Bull, 0x000000000000AE8Bull },
	{ 0x8000000000007BF5ull, 0x8000000000007BF5ull },
	{ 0x000000003E668DFDull, 0x000000003E668DFDull },
	{ 0x80000000000024BAull, 0x80000000000024BAull },
	{ 0x800000000FF5AB4Eull, 0x800000000FF5AB4Eull },
	{ 0x800F82A5358BD1BCull, 0x800F82A5358BD1BCull },
	{ 0x8000000000000009ull, 0x8000000000000009ull },
	{ 0x800000009F9C9115ull, 0x800000009F9C9115ull },
	{ 0x800000000000A716ull, 0x800000000000A716ull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x80002E920BE15342ull, 0x80002E920BE15342ull },
	{ 0x00000000000009FBull, 0x00000000000009FBull },
	{ 0x00000000002E39F0ull, 0x00000000002E39F0ull },
	{ 0x000006B121E9E6FDull, 0x000006B121E9E6FDull },
	{ 0x00000000000001C5ull, 0x00000000000001C5ull },
	{ 0x00000000000001C5ull, 0x00000000000001C5ull },
	{ 0x00000000004ADE99ull, 0x00000000004ADE99ull },
	{ 0x000000000006E931ull, 0x000000000006E931ull },
	{ 0x000001B6685BB81Dull, 0x000001B6685BB81Dull },
	{ 0x800048A1EFAE0913ull, 0x800048A1EFAE0913ull },
	{ 0x00000000AAB11187ull, 0x00000000AAB11187ull },
	{ 0x0000038823FE26C8ull, 0x0000038823FE26C8ull },
	{ 0x80000002419B7380ull, 0x80000002419B7380ull },
	{ 0x00000000000AAEBDull, 0x00000000000AAEBDull },
	{ 0x8000000B269C3859ull, 0x8000000B269C3859ull },
	{ 0x80000000CC8E0122ull, 0x80000000CC8E0122ull },
	{ 0x0000000016C78500ull, 0x0000000016C78500ull },
	{ 0x8000000029DFEC93ull, 0x8000000029DFEC93ull },
	{ 0x00007AD01F8AED2Bull, 0x00007AD01F8AED2Bull },
	{ 0x80000000000C75A2ull, 0x80000000000C75A2ull },
	{ 0x000000000000001Eull, 0x000000000000001Eull },
	{ 0x0001152ED2A83342ull, 0x0001152ED2A83342ull },
	{ 0x80017F8C0BD24C07ull, 0x80017F8C0BD24C07ull },
	{ 0x80000000000002C4ull, 0x80000000000002C4ull },
	{ 0x00000000021AC057ull, 0x00000000021AC057ull },
	{ 0x80000001E7625D53ull, 0x80000001E7625D53ull },
	{ 0x800000166AA82C05ull, 0x800000166AA82C05ull },
	{ 0x000000000D84BEFDull, 0x000000000D84BEFDull },
	{ 0x8000000000000031ull, 0x8000000000000031ull },
	{ 0x800003D5ED3C61F5ull, 0x800003D5ED3C61F5ull },
	{ 0x80002D0C7E87B0EBull, 0x80002D0C7E87B0EBull },
	{ 0x000000044BA0E967ull, 0x000000044BA0E967ull },
	{ 0x0000000000000179ull, 0x0000000000000179ull },
	{ 0x80000000000005D5ull, 0x80000000000005D5ull },
	{ 0x800000000000004Full, 0x800000000000004Full },
	{ 0x8000000007D7B110ull, 0x8000000007D7B110ull },
	{ 0x800001CACF201572ull, 0x800001CACF201572ull },
	{ 0x8000CA571B0B931Cull, 0x8000CA571B0B931Cull },
	{ 0x000000000A0B5F14ull, 0x000000000A0B5F14ull },
	{ 0x00000CF88EB0E274ull, 0x00000CF88EB0E274ull },
	{ 0x8000000000009A8Bull, 0x8000000000009A8Bull },
	{ 0x0000000028F49F9Aull, 0x0000000028F49F9Aull },
	{ 0x00062FD02C6AABC3ull, 0x00062FD02C6AABC3ull },
	{ 0x80000000000DF5CCull, 0x80000000000DF5CCull },
	{ 0x8000000000091613ull, 0x8000000000091613ull },
	{ 0x80000000D0D0DC8Aull, 0x80000000D0D0DC8Aull },
	{ 0x80000A92E332645Aull, 0x80000A92E332645Aull },
	{ 0x0000000000008C89ull, 0x0000000000008C89ull },
	{ 0x000000084D3FB425ull, 0x000000084D3FB425ull },
	{ 0x00005670540CDC4Dull, 0x00005670540CDC4Dull },
	{ 0x800000000000001Cull, 0x800000000000001Cull },
	{ 0x00000003E3A6EEDFull, 0x00000003E3A6EEDFull },
	{ 0x8000000205ACAB22ull, 0x8000000205ACAB22ull },
	{ 0x8000000000000E4Full, 0x8000000000000E4Full },
	{ 0x80000042E5210667ull, 0x80000042E5210667ull },
	{ 0x0000000000000020ull, 0x0000000000000020ull },
	{ 0x00000000001A02FBull, 0x00000000001A02FBull },
	{ 0x000000015A11D85Bull, 0x000000015A11D85Bull },
	{ 0x8000000000000001ull, 0x8000000000000001ull },
	{ 0x000000000E22C16Aull, 0x000000000E22C16Aull },
	{ 0x0000C0EBBF245659ull, 0x0000C0EBBF245659ull },
	{ 0x8000028B1FE87450ull, 0x8000028B1FE87450ull },
	{ 0x8000000028E289ADull, 0x8000000028E289ADull },
	{ 0x0000000000000002ull, 0x0000000000000002ull },
	{ 0x0000000000001390ull, 0x0000000000001390ull },
	{ 0x80000000098F9B2Full, 0x80000000098F9B2Full },
	{ 0x000000A559DEB64Dull, 0x000000A559DEB64Dull },
	{ 0x800052026B501A95ull, 0x800052026B501A95ull },
	{ 0x000212E11EFD11F5ull, 0x000212E11EFD11F5ull },
	{ 0x00000000000000B5ull, 0x00000000000000B5ull },
	{ 0x000009D02BA084ABull, 0x000009D02BA084ABull },
	{ 0x000000000004D6CDull, 0x000000000004D6CDull },
	{ 0x00000BCB7F50328Eull, 0x00000BCB7F50328Eull },
	{ 0x800000000000C6EBull, 0x800000000000C6EBull },
	{ 0x8000000000003EB6ull, 0x8000000000003EB6ull },
	{ 0x8000000000003925ull, 0x8000000000003925ull },
	{ 0x8000000000015C9Cull, 0x8000000000015C9Cull },
	{ 0x80000000005F00D5ull, 0x80000000005F00D5ull },
	{ 0x000000000DDA150Eull, 0x000000000DDA150Eull },
	{ 0x8000287EE65B9325ull, 0x8000287EE65B9325ull },
	{ 0x8000000017BD8A4Bull, 0x8000000017BD8A4Bull },
	{ 0x000000000B8EE154ull, 0x000000000B8EE154ull },
	{ 0x0000E35273BDDF9Dull, 0x0000E35273BDDF9Dull },
	{ 0x00000001AA12F897ull, 0x00000001AA12F897ull },
	{ 0x80000000000202ACull, 0x80000000000202ACull },
	{ 0x8000000002ADF65Full, 0x8000000002ADF65Full },
	{ 0x000000236D395B72ull, 0x000000236D395B72ull },
	{ 0x3FF0000000000001ull, 0x3FF0000000000001ull },
	{ 0x3FF0000000000002ull, 0x3FF0000000000002ull },
	{ 0x3FF0000000000004ull, 0x3FF0000000000004ull },
	{ 0x3FF0000000000008ull, 0x3FF0000000000008ull },
	{ 0x3FF0000000000010ull, 0x3FF0000000000010ull },
	{ 0x3FF0000000000020ull, 0x3FF0000000000020ull },
	{ 0x3FF0000000000040ull, 0x3FF0000000000040ull },
	{ 0x3FF0000000000080ull, 0x3FF0000000000080ull },
	{ 0x3FF0000000000100ull, 0x3FF0000000000100ull },
	{ 0x3FF0000000000200ull, 0x3FF0000000000200ull },
	{ 0x3FF0000000000400ull, 0x3FF0000000000400ull },
	{ 0x3FF0000000000800ull, 0x3FF0000000000800ull },
	{ 0x3FF0000000001000ull, 0x3FF0000000001000ull },
	{ 0x3FF0000000002000ull, 0x3FF0000000002000ull },
	{ 0x3FF0000000004000ull, 0x3FF0000000004000ull },
	{ 0x3FF0000000008000ull, 0x3FF0000000008000ull },
	{ 0x3FF0000000010000ull, 0x3FF0000000010000ull },
	{ 0x3FF0000000020000ull, 0x3FF0000000020000ull },
	{ 0x3FF0000000040000ull, 0x3FF0000000040000ull },
	{ 0x3FF0000000080000ull, 0x3FF0000000080000ull },
	{ 0x3FF0000000100000ull, 0x3FF0000000100000ull },
	{ 0x3FF0000000200000ull, 0x3FF0000000200000ull },
	{ 0x3FF0000000400000ull, 0x3FF0000000400000ull },
	{ 0x3FF0000000800000ull, 0x3FF0000000800000ull },
	{ 0x3FF0000001000000ull, 0x3FF0000001000000ull },
	{ 0x3FF0000002000000ull, 0x3FF0000002000000ull },
	{ 0x3FF0000004000000ull, 0x3FF0000004000000ull },
	{ 0x3FF0000008000000ull, 0x3FF0000008000000ull },
	{ 0x3FF0000010000000ull, 0x3FF0000010000000ull },
	{ 0x3FF0000020000000ull, 0x3FF0000020000000ull },
	{ 0x3FF0000040000000ull, 0x3FF0000040000000ull },
	{ 0x3FF0000080000000ull, 0x3FF0000080000000ull },
	{ 0x3FF0000100000000ull, 0x3FF0000100000000ull },
	{ 0x3FF0000200000000ull, 0x3FF0000200000000ull },
	{ 0x3FF0000400000000ull, 0x3FF0000400000000ull },
	{ 0x3FF0000800000000ull, 0x3FF0000800000000ull },
	{ 0x3FF0001000000000ull, 0x3FF0001000000000ull },
	{ 0x3FF0002000000000ull, 0x3FF0002000000000ull },
	{ 0x3FF0004000000000ull, 0x3FF0004000000000ull },
	{ 0x3FF0008000000000ull, 0x3FF0008000000000ull },
	{ 0x3FF0010000000000ull, 0x3FF0010000000000ull },
	{ 0x3FF0020000000000ull, 0x3FF0020000000000ull },
	{ 0x3FF0040000000000ull, 0x3FF0040000000000ull },
	{ 0x3FF0080000000000ull, 0x3FF0080000000000ull },
	{ 0x3FF0100000000000ull, 0x3FF0100000000000ull },
	{ 0x3FF0200000000000ull, 0x3FF0200000000000ull },
	{ 0x3FF0400000000000ull, 0x3FF0400000000000ull },
	{ 0x3FF0800000000000ull, 0x3FF0800000000000ull },
	{ 0x3FF1000000000000ull, 0x3FF1000000000000ull },
	{ 0x3FF2000000000000ull, 0x3FF2000000000000ull },
	{ 0x3FF4000000000000ull, 0x3FF4000000000000ull },
	{ 0x3FF8000000000000ull, 0x3FF8000000000000ull },
	{ 0x1FF0000000000001ull, 0x1FF0000000000001ull },
	{ 0x1FF0000000000002ull, 0x1FF0000000000002ull },
	{ 0x1FF0000000000004ull, 0x1FF0000000000004ull },
	{ 0x1FF0000000000008ull, 0x1FF0000000000008ull },
	{ 0x1FF0000000000010ull, 0x1FF0000000000010ull },
	{ 0x1FF0000000000020ull, 0x1FF0000000000020ull },
	{ 0x1FF0000000000040ull, 0x1FF0000000000040ull },
	{ 0x1FF0000000000080ull, 0x1FF0000000000080ull },
	{ 0x1FF0000000000100ull, 0x1FF0000000000100ull },
	{ 0x1FF0000000000200ull, 0x1FF0000000000200ull },
	{ 0x1FF0000000000400ull, 0x1FF0000000000400ull },
	{ 0x1FF0000000000800ull, 0x1FF0000000000800ull },
	{ 0x1FF0000000001000ull, 0x1FF0000000001000ull },
	{ 0x1FF0000000002000ull, 0x1FF0000000002000ull },
	{ 0x1FF0000000004000ull, 0x1FF0000000004000ull },
	{ 0x1FF0000000008000ull, 0x1FF0000000008000ull },
	{ 0x1FF0000000010000ull, 0x1FF0000000010000ull },
	{ 0x1FF0000000020000ull, 0x1FF0000000020000ull },
	{ 0x1FF0000000040000ull, 0x1FF0000000040000ull },
	{ 0x1FF0000000080000ull, 0x1FF0000000080000ull },
	{ 0x1FF0000000100000ull, 0x1FF0000000100000ull },
	{ 0x1FF0000000200000ull, 0x1FF0000000200000ull },
	{ 0x1FF0000000400000ull, 0x1FF0000000400000ull },
	{ 0x1FF0000000800000ull, 0x1FF0000000800000ull },
	{ 0x1FF0000001000000ull, 0x1FF0000001000000ull },
	{ 0x1FF0000002000000ull, 0x1FF0000002000000ull },
	{ 0x1FF0000004000000ull, 0x1FF0000004000000ull },
	{ 0x1FF0000008000000ull, 0x1FF0000008000000ull },
	{ 0x1FF0000010000000ull, 0x1FF0000010000000ull },
	{ 0x1FF0000020000000ull, 0x1FF0000020000000ull },
	{ 0x1FF0000040000000ull, 0x1FF0000040000000ull },
	{ 0x1FF0000080000000ull, 0x1FF0000080000000ull },
	{ 0x1FF0000100000000ull, 0x1FF0000100000000ull },
	{ 0x1FF0000200000000ull, 0x1FF0000200000000ull },
	{ 0x1FF0000400000000ull, 0x1FF0000400000000ull },
	{ 0x1FF0000800000000ull, 0x1FF0000800000000ull },
	{ 0x1FF0001000000000ull, 0x1FF0001000000000ull },
	{ 0x1FF0002000000000ull, 0x1FF0002000000000ull },
	{ 0x1FF0004000000000ull, 0x1FF0004000000000ull },
	{ 0x1FF0008000000000ull, 0x1FF0008000000000ull },
	{ 0x1FF0010000000000ull, 0x1FF0010000000000ull },
	{ 0x1FF0020000000000ull, 0x1FF0020000000000ull },
	{ 0x1FF0040000000000ull, 0x1FF0040000000000ull },
	{ 0x1FF0080000000000ull, 0x1FF0080000000000ull },
	{ 0x1FF0100000000000ull, 0x1FF0100000000000ull },
	{ 0x1FF0200000000000ull, 0x1FF0200000000000ull },
	{ 0x1FF0400000000000ull, 0x1FF0400000000000ull },
	{ 0x1FF0800000000000ull, 0x1FF0800000000000ull },
	{ 0x1FF1000000000000ull, 0x1FF1000000000000ull },
	{ 0x1FF2000000000000ull, 0x1FF2000000000000ull },
	{ 0x1FF4000000000000ull, 0x1FF4000000000000ull },
	{ 0x1FF8000000000000ull, 0x1FF8000000000000ull }
};

CShader5xTest_DoubleArith::Binary64Vec2 const CShader5xTest_dadd::s_DAddRes[] = 
{
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FFFCB8D9A5D4659ull, 0x7FFFCB8D9A5D4659ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x0000000000000000ull, 0x0000000000000000ull },
	{ 0x8000000000000000ull, 0x8000000000000000ull },
	{ 0x7FFEB0586EF53833ull, 0x7FFEB0586EF53833ull },
	{ 0x7FF9436DD07BF2E5ull, 0x7FF9436DD07BF2E5ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF84E1BFCCABC0Bull, 0x7FF84E1BFCCABC0Bull },
	{ 0x7FFB37D2241C3B1Dull, 0x7FFB37D2241C3B1Dull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0x7FF0000000000000ull, 0x7FF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xFFF0000000000000ull, 0xFFF0000000000000ull },
	{ 0xF80D73C050176B53ull, 0xF80D73C050176B53ull },
	{ 0x710CE3011CCBFE44ull, 0x710CE3011CCBFE44ull },
	{ 0xB3E6B297EC29791Dull, 0xB3E6B297EC29791Dull },
	{ 0x538FB3BC5CFA6759ull, 0x538FB3BC5CFA6759ull },
	{ 0xE495B5F9B32EFE04ull, 0xE495B5F9B32EFE04ull },
	{ 0x3031A2686B267CF5ull, 0x3031A2686B267CF5ull },
	{ 0x30B5C1676C56DD62ull, 0x30B5C1676C56DD62ull },
	{ 0x744BABA648DF8839ull, 0x744BABA648DF8839ull },
	{ 0x63068E39A09E6158ull, 0x63068E39A09E6158ull },
	{ 0xE456D172A0AADA0Bull, 0xE456D172A0AADA0Bull },
	{ 0xCECA2AF0E92E414Dull, 0xCECA2AF0E92E414Dull },
	{ 0x1D8555184AB52152ull, 0x1D8555184AB52152ull },
	{ 0x4C0A36D2B4A33B50ull, 0x4C0A36D2B4A33B50ull },
	{ 0xC64C5756ED525E6Full, 0xC64C5756ED525E6Full },
	{ 0x66199E890D233BD4ull, 0x66199E890D233BD4ull },
	{ 0x578E020B5EEBCB63ull, 0x578E020B5EEBCB63ull },
	{ 0xF28A90D0D4429690ull, 0xF28A90D0D4429690ull },
	{ 0x4CB8A54B86514DD4ull, 0x4CB8A54B86514DD4ull },
	{ 0xD9830B957560898Full, 0xD9830B957560898Full },
	{ 0xD5F0B21519E3C836ull, 0xD5F0B21519E3C836ull },
	{ 0x686148F945D970FFull, 0x686148F945D970FFull },
	{ 0x2A282C8A6399D814ull, 0x2A282C8A6399D814ull },
	{ 0xF616E3ED0C808295ull, 0xF616E3ED0C808295ull },
	{ 0xEE29C0532D6FE815ull, 0xEE29C0532D6FE815ull },
	{ 0xE67BDC206518C2C8ull, 0xE67BDC206518C2C8ull },
	{ 0xC3746AE9109A05C0ull, 0xC3746AE9109A05C0ull },
	{ 0xE185D03B8651A483ull, 0xE185D03B8651A483ull },
	{ 0x9BC997A06BA03630ull, 0x9BC997A06BA03630ull },
	{ 0xB8DB78A14FDCAF88ull, 0xB8DB78A14FDCAF88ull },
	{ 0x9B3BBA9F54F99101ull, 0x9B3BBA9F54F99101ull },
	{ 0xE2034F7B3BB0BD5Aull, 0xE2034F7B3BB0BD5Aull },
	{ 0xF6556E68F7B36624ull, 0xF6556E68F7B36624ull },
	{ 0x9CBE7B72CC608975ull, 0x9CBE7B72CC608975ull },
	{ 0xF943635FF0DC04A4ull, 0xF943635FF0DC04A4ull },
	{ 0x6191872762A09848ull, 0x6191872762A09848ull },
	{ 0x51A1F8B3648057FCull, 0x51A1F8B3648057FCull },
	{ 0x1EE5CC0877C6A959ull, 0x1EE5CC0877C6A959ull },
	{ 0xBCF12C70BEFF94EEull, 0xBCF12C70BEFF94EEull },
	{ 0x9408133D9FA4F8E4ull, 0x9408133D9FA4F8E4ull },
	{ 0xD7E355A44E2F5FE3ull, 0xD7E355A44E2F5FE3ull },
	{ 0xCBF1C6783C472A07ull, 0xCBF1C6783C472A07ull },
	{ 0xE4B59F4986B39276ull, 0xE4B59F4986B39276ull },
	{ 0xFDA487571458AA28ull, 0xFDA487571458AA28ull },
	{ 0x65BB22A219C0EF62ull, 0x65BB22A219C0EF62ull },
	{ 0xD7D987252F6ABD15ull, 0xD7D987252F6ABD15ull },
	{ 0xF2F12D229FC3EE93ull, 0xF2F12D229FC3EE93ull },
	{ 0x67A42BE51DE123BEull, 0x67A42BE51DE123BEull },
	{ 0xE356252611F2779Eull, 0xE356252611F2779Eull },
	{ 0xBE6F96E7DCA55404ull, 0xBE6F96E7DCA55404ull },
	{ 0x6E1A2F90F2513BDCull, 0x6E1A2F90F2513BDCull },
	{ 0x649D3AA49A42B9B4ull, 0x649D3AA49A42B9B4ull },
	{ 0xFBCAC79B2D4D64C3ull, 0xFBCAC79B2D4D64C3ull },
	{ 0x6E876922FF3BA6CAull, 0x6E876922FF3BA6CAull },
	{ 0xEB3744238A0844B8ull, 0xEB3744238A0844B8ull },
	{ 0xE67F277615169F0Full, 0xE67F277615169F0Full },
	{ 0xAD9C83A11AF14EE4ull, 0xAD9C83A11AF14EE4ull },
	{ 0x5E69DE7D833A79DDull, 0x5E69DE7D833A79DDull },
	{ 0xD8EF7F37B1DC0E0Eull, 0xD8EF7F37B1DC0E0Eull },
	{ 0x6390AE81E8373B90ull, 0x6390AE81E8373B90ull },
	{ 0xC0DE88F1E6288ED4ull, 0xC0DE88F1E6288ED4ull },
	{ 0x779AFE4A3BEED607ull, 0x779AFE4A3BEED607ull },
	{ 0xE96C0724BE59D6E5ull, 0xE96C0724BE59D6E5ull },
	{ 0x695B3F86F5124B99ull, 0x695B3F86F5124B99ull },
	{ 0xE7902B6494F70163ull, 0xE7902B6494F70163ull },
	{ 0xE76337E18D84664Cull, 0xE76337E18D84664Cull },
	{ 0xEAB47D0A31A8ECCCull, 0xEAB47D0A31A8ECCCull },
	{ 0xF2B9CBD2D059C962ull, 0xF2B9CBD2D059C962ull },
	{ 0x77BFE7D53C82241Full, 0x77BFE7D53C82241Full },
	{ 0x9F6562DD6BDD72F5ull, 0x9F6562DD6BDD72F5ull },
	{ 0xE4F6558A9B7B0918ull, 0xE4F6558A9B7B0918ull },
	{ 0xFB8F8D763FA2D61Eull, 0xFB8F8D763FA2D61Eull },
	{ 0x6B11626103A915D7ull, 0x6B11626103A915D7ull },
	{ 0x54E4BE755A6AE588ull, 0x54E4BE755A6AE588ull },
	{ 0x20CE889624875DB7ull, 0x20CE889624875DB7ull },
	{ 0xB0B5F6DB949D88CBull, 0xB0B5F6DB949D88CBull },
	{ 0x5FB367621F5C921Aull, 0x5FB367621F5C921Aull },
	{ 0xC6500417CB6DDC32ull, 0xC6500417CB6DDC32ull },
	{ 0xE2B5C4CD340339D2ull, 0xE2B5C4CD340339D2ull },
	{ 0xA3322247315D95A9ull, 0xA3322247315D95A9ull },
	{ 0x982CDCBC41135A36ull, 0x982CDCBC41135A36ull },
	{ 0xE26E0CD3473FC4C9ull, 0xE26E0CD3473FC4C9ull },
	{ 0xC4ED983DD3E7F21Dull, 0xC4ED983DD3E7F21Dull },
	{ 0x575F445B1BBCC090ull, 0x575F445B1BBCC090ull },
	{ 0xF8800AA67C1443BDull, 0xF8800AA67C1443BDull },
	{ 0xF94686781CCA2344ull, 0xF94686781CCA2344ull },
	{ 0x4A05F0A4B5C668BDull, 0x4A05F0A4B5C668BDull },
	{ 0xF7EE70CD187EB532ull, 0xF7EE70CD187EB532ull },
	{ 0x720A85150148C080ull, 0x720A85150148C080ull },
	{ 0x3A4BECE5D42D36BAull, 0x3A4BECE5D42D36BAull },
	{ 0xD8781344CEF5DF2Full, 0xD8781344CEF5DF2Full },
	{ 0x700910AE7651529Bull, 0x700910AE7651529Bull },
	{ 0x21C830C3FDB647B8ull, 0x21C830C3FDB647B8ull },
	{ 0x668DFCC89C331C5Dull, 0x668DFCC89C331C5Dull },
	{ 0x2D9C9F976C77D3B1ull, 0x2D9C9F976C77D3B1ull },
	{ 0x7E1C6CEB0114CADAull, 0x7E1C6CEB0114CADAull },
	{ 0xA97C80175AA5D59Dull, 0xA97C80175AA5D59Dull },
	{ 0x41D8E464A96B68E4ull, 0x41D8E464A96B68E4ull },
	{ 0xDEF4E4B993CA3126ull, 0xDEF4E4B993CA3126ull },
	{ 0xFC983FB515D7AC5Full, 0xFC983FB515D7AC5Full },
	{ 0x7A3D48425B8F22DEull, 0x7A3D48425B8F22DEull },
	{ 0x582BABEE02A018F8ull, 0x582BABEE02A018F8ull },
	{ 0x59AED0EE67E81546ull, 0x59AED0EE67E81546ull },
	{ 0x46868749FE46C51Cull, 0x46868749FE46C51Cull },
	{ 0xBD83752A237F6507ull, 0xBD83752A237F6507ull },
	{ 0xDDF635E87A7C93D9ull, 0xDDF635E87A7C93D9ull },
	{ 0x1BF97773A0C7F3B9ull, 0x1BF97773A0C7F3B9ull },
	{ 0x3281359D7A1B6513ull, 0x3281359D7A1B6513ull },
	{ 0xA9B3BE5D3F5D4CBBull, 0xA9B3BE5D3F5D4CBBull },
	{ 0xC756919F03B751C7ull, 0xC756919F03B751C7ull },
	{ 0x5ED633E28AFF896Aull, 0x5ED633E28AFF896Aull },
	{ 0xDCDDA755F17B4FD0ull, 0xDCDDA755F17B4FD0ull },
	{ 0x5614C83C4AF0E037ull, 0x5614C83C4AF0E037ull },
	{ 0xBE7CB24A93C4E75Full, 0xBE7CB24A93C4E75Full },
	{ 0x612E61DFCF735C5Full, 0x612E61DFCF735C5Full },
	{ 0xCF574A05F645B01Aull, 0xCF574A05F645B01Aull },
	{ 0x78D1B88C4ADE21C1ull, 0x78D1B88C4ADE21C1ull },
	{ 0x5AC73602133E76DFull, 0x5AC73602133E76DFull },
	{ 0xCC9E6B2603011C15ull, 0xCC9E6B2603011C15ull },
	{ 0x6D63495984B0A056ull, 0x6D63495984B0A056ull },
	{ 0xEB07124DC675375Cull, 0xEB07124DC675375Cull },
	{ 0x287A7D91A34A9581ull, 0x287A7D91A34A9581ull },
	{ 0xFD08A87A13D9377Aull, 0xFD08A87A13D9377Aull },
	{ 0x5E8A3848282CD92Cull, 0x5E8A3848282CD92Cull },
	{ 0xBDBCBB09FBA07AB7ull, 0xBDBCBB09FBA07AB7ull },
	{ 0x24D65A8F727FDCAFull, 0x24D65A8F727FDCAFull },
	{ 0xF27599FBFF09BA3Eull, 0xF27599FBFF09BA3Eull },
	{ 0x799D956EA4B9FC20ull, 0x799D956EA4B9FC20ull },
	{ 0x6E6D1AFFFCCEE30Aull, 0x6E6D1AFFFCCEE30Aull },
	{ 0x578632EE071B6B78ull, 0x578632EE071B6B78ull },
	{ 0xFC7565D362AE56C9ull, 0xFC7565D362AE56C9ull },
	{ 0xCA9493FD12F28798ull, 0xCA9493FD12F28798ull },
	{ 0x705F7377BA8C9FA6ull, 0x705F7377BA8C9FA6ull },
	{ 0xAD1DD2242F7D302Full, 0xAD1DD2242F7D302Full },
	{ 0xE20B936CB544A575ull, 0xE20B936CB544A575ull },
	{ 0x4A00ADBFB160FA72ull, 0x4A00ADBFB160FA72ull },
	{ 0xA023A3847F861C98ull, 0xA023A3847F861C98ull },
	{ 0xC70CED291CEF347Cull, 0xC70CED291CEF347Cull },
	{ 0x36212BE17B38547Bull, 0x36212BE17B38547Bull },
	{ 0xE4E5D521D16F6A2Eull, 0xE4E5D521D16F6A2Eull },
	{ 0xD0326499E285A330ull, 0xD0326499E285A330ull },
	{ 0x3AF1EFDE58321DD6ull, 0x3AF1EFDE58321DD6ull },
	{ 0x6475E9A139A5C76Cull, 0x6475E9A139A5C76Cull },
	{ 0xEDEF86D111CFC80Eull, 0xEDEF86D111CFC80Eull },
	{ 0xFD00C816D0AE9C15ull, 0xFD00C816D0AE9C15ull },
	{ 0x7FD4B5C11F7E6F7Dull, 0x7FD4B5C11F7E6F7Dull },
	{ 0xB7C94A7151B0FDE2ull, 0xB7C94A7151B0FDE2ull },
	{ 0x6D6EEB802B2481D5ull, 0x6D6EEB802B2481D5ull },
	{ 0xDCB090E81A05FDCDull, 0xDCB090E81A05FDCDull },
	{ 0x5601867CFA94501Cull, 0x5601867CFA94501Cull },
	{ 0xB63FEB49C17AA36Bull, 0xB63FEB49C17AA36Bull },
	{ 0xF705EDA55E65DF5Full, 0xF705EDA55E65DF5Full },
	{ 0xD97EAC12A227BB2Cull, 0xD97EAC12A227BB2Cull },
	{ 0x2847601AF42B8511ull, 0x2847601AF42B8511ull },
	{ 0xC4EA2C5063B76035ull, 0xC4EA2C5063B76035ull },
	{ 0x39148EBD5FA61BC3ull, 0x39148EBD5FA61BC3ull },
	{ 0x55877C2796C60EF6ull, 0x55877C2796C60EF6ull },
	{ 0xC5B5E900251373FAull, 0xC5B5E900251373FAull },
	{ 0xD659DEF550A4A619ull, 0xD659DEF550A4A619ull },
	{ 0xD83BDBFB223FA19Cull, 0xD83BDBFB223FA19Cull },
	{ 0x73D90134D699DAECull, 0x73D90134D699DAECull },
	{ 0xE1236FE9B74B2238ull, 0xE1236FE9B74B2238ull },
	{ 0x60FEBC952FF4872Cull, 0x60FEBC952FF4872Cull },
	{ 0xF0B7328B5F4A0FD4ull, 0xF0B7328B5F4A0FD4ull },
	{ 0xDCCF9CD622246351ull, 0xDCCF9CD622246351ull },
	{ 0x35E1350067141BB2ull, 0x35E1350067141BB2ull },
	{ 0x5F2C667538E5B447ull, 0x5F2C667538E5B447ull },
	{ 0xE62AC0CAD8BC7622ull, 0xE62AC0CAD8BC7622ull },
	{ 0xFDF905FC4A353A93ull, 0xFDF905FC4A353A93ull },
	{ 0x703FA3EEC1B5C770ull, 0x703FA3EEC1B5C770ull },
	{ 0x77AA4486857BF0D2ull, 0x77AA4486857BF0D2ull },
	{ 0x9FAD9239C922A9C1ull, 0x9FAD9239C922A9C1ull },
	{ 0xC9C21AACD0AF1049ull, 0xC9C21AACD0AF1049ull },
	{ 0x33FE7BD8D9DBC706ull, 0x33FE7BD8D9DBC706ull },
	{ 0xD3BDA614C0B0F3D7ull, 0xD3BDA614C0B0F3D7ull },
	{ 0xB940B0EF65931200ull, 0xB940B0EF65931200ull },
	{ 0x6D1391A59C84B2D4ull, 0x6D1391A59C84B2D4ull },
	{ 0xE2168C2D42B6A2A0ull, 0xE2168C2D42B6A2A0ull },
	{ 0xBFFC78B36964EE5Full, 0xBFFC78B36964EE5Full },
	{ 0xBD87D404E8318353ull, 0xBD87D404E8318353ull },
	{ 0x87E8E70596D27600ull, 0x87E8E70596D27600ull },
	{ 0x9BCA97545E814052ull, 0x9BCA97545E814052ull },
	{ 0x78DC5D0B0C572003ull, 0x78DC5D0B0C572003ull },
	{ 0x7C9939A61D1BAAE1ull, 0x7C9939A61D1BAAE1ull },
	{ 0x26C2AB48332E9A79ull, 0x26C2AB48332E9A79ull },
	{ 0x392F3E88267DDDD1ull, 0x392F3E88267DDDD1ull },
	{ 0x71B9312DDBA5109Aull, 0x71B9312DDBA5109Aull },
	{ 0x51B0F0B3510D008Bull, 0x51B0F0B3510D008Bull },
	{ 0x6228FBE36B249343ull, 0x6228FBE36B249343ull },
	{ 0xEAF240D8E2A9A9AAull, 0xEAF240D8E2A9A9AAull },
	{ 0x48E342CDED5ED314ull, 0x48E342CDED5ED314ull },
	{ 0x3C5488D0F22D9126ull, 0x3C5488D0F22D9126ull },
	{ 0xBC6AADB68D150A81ull, 0xBC6AADB68D150A81ull },
	{ 0xE7D26C57094F3AE8ull, 0xE7D26C57094F3AE8ull },
	{ 0x6C8BA8DE7B1FC251ull, 0x6C8BA8DE7B1FC251ull },
	{ 0x2BDBE57AAB955559ull, 0x2BDBE57AAB955559ull },
	{ 0x4439E9CDF93F7818ull, 0x4439E9CDF93F7818ull },
	{ 0xB0A9241BD721D09Aull, 0xB0A9241BD721D09Aull },
	{ 0x2129CB2E9F978781ull, 0x2129CB2E9F978781ull },
	{ 0x7AD1807EF3E4382Bull, 0x7AD1807EF3E4382Bull },
	{ 0x57CDB4B576CA8D6Bull, 0x57CDB4B576CA8D6Bull },
	{ 0x39651A70E8434CB4ull, 0x39651A70E8434CB4ull },
	{ 0xC7C1E5CC4AF474C4ull, 0xC7C1E5CC4AF474C4ull },
	{ 0xEB5FC61F9F3BE5F5ull, 0xEB5FC61F9F3BE5F5ull },
	{ 0x6EE84BBD4C166195ull, 0x6EE84BBD4C166195ull },
	{ 0xF9DA1DF355E7B8DDull, 0xF9DA1DF355E7B8DDull },
	{ 0x8C862908A01C0EABull, 0x8C862908A01C0EABull },
	{ 0x771199D3CA099DCFull, 0x771199D3CA099DCFull },
	{ 0x8B79B2F02EAABE67ull, 0x8B79B2F02EAABE67ull },
	{ 0x62FD4062C7527E06ull, 0x62FD4062C7527E06ull },
	{ 0x27AA721B22DD2D5Cull, 0x27AA721B22DD2D5Cull },
	{ 0x9BA80140A3AFCD5Dull, 0x9BA80140A3AFCD5Dull },
	{ 0x4F38C18FB91BB182ull, 0x4F38C18FB91BB182ull },
	{ 0xACD6C7F7CA74CE5Aull, 0xACD6C7F7CA74CE5Aull },
	{ 0x6E9C1C75858E879Cull, 0x6E9C1C75858E879Cull },
	{ 0xF747BF6A98C71410ull, 0xF747BF6A98C71410ull },
	{ 0x4510B9A17D9F7792ull, 0x4510B9A17D9F7792ull },
	{ 0xE008F97004989A95ull, 0xE008F97004989A95ull },
	{ 0x6D9FDBDA718A0B3Full, 0x6D9FDBDA718A0B3Full },
	{ 0x43CE493809527CF6ull, 0x43CE493809527CF6ull },
	{ 0x1C64B83866EE4C34ull, 0x1C64B83866EE4C34ull },
	{ 0xAF2D97ED1F3B25FBull, 0xAF2D97ED1F3B25FBull },
	{ 0xB64962CBA402306Bull, 0xB64962CBA402306Bull },
	{ 0x363E7CF743D5F608ull, 0x363E7CF743D5F608ull },
	{ 0x9C7A4CC993CA59DFull, 0x9C7A4CC993CA59DFull },
	{ 0x7AB41DDDF697B5EFull, 0x7AB41DDDF697B5EFull },
	{ 0xFF26EACEC4F307B7ull, 0xFF26EACEC4F307B7ull },
	{ 0x74B59AA4D6D465A5ull, 0x74B59AA4D6D465A5ull },
	{ 0x60B478814647C781ull, 0x60B478814647C781ull },
	{ 0x6A453DE085020245ull, 0x6A453DE085020245ull },
	{ 0x1C2B21314C7A159Bull, 0x1C2B21314C7A159Bull },
	{ 0xE811ACAF83A3410Dull, 0xE811ACAF83A3410Dull },
	{ 0x3471A91F632EAC5Eull, 0x3471A91F632EAC5Eull },
	{ 0xB8C1169714DA515Eull, 0xB8C1169714DA515Eull },
	{ 0x7F206B283FC634CAull, 0x7F206B283FC634CAull },
	{ 0x5D6665641C639D3Eull, 0x5D6665641C639D3Eull },
	{ 0x4E6349199509C6F8ull, 0x4E6349199509C6F8ull },
	{ 0x220BE336886D0D2Dull, 0x220BE336886D0D2Dull },
	{ 0xEED832C857A21868ull, 0xEED832C857A21868ull },
	{ 0x1D653AEFF1628CF9ull, 0x1D653AEFF1628CF9ull },
	{ 0xEB5C93622D773C29ull, 0xEB5C93622D773C29ull },
	{ 0xDF4024749D006AE0ull, 0xDF4024749D006AE0ull },
	{ 0x711847F760D09E32ull, 0x711847F760D09E32ull },
	{ 0xF558DC9CA8474F38ull, 0xF558DC9CA8474F38ull },
	{ 0xF8E2B490114BD552ull, 0xF8E2B490114BD552ull },
	{ 0xDBE18B48035AA72Aull, 0xDBE18B48035AA72Aull },
	{ 0x7769E968AD23D9B7ull, 0x7769E968AD23D9B7ull },
	{ 0x25E0478C8F3EEFBFull, 0x25E0478C8F3EEFBFull },
	{ 0x60C54EEC60B576B9ull, 0x60C54EEC60B576B9ull },
	{ 0xFD593744FEC02C13ull, 0xFD593744FEC02C13ull },
	{ 0x9878EC15A5E950A5ull, 0x9878EC15A5E950A5ull },
	{ 0x742FB2F64880A78Bull, 0x742FB2F64880A78Bull },
	{ 0x33B2C63C78EF7FC5ull, 0x33B2C63C78EF7FC5ull },
	{ 0xED0EAFC963BC0261ull, 0xED0EAFC963BC0261ull },
	{ 0xA6DCE14BE32D74D5ull, 0xA6DCE14BE32D74D5ull },
	{ 0x52C18D9D90D9770Eull, 0x52C18D9D90D9770Eull },
	{ 0x74289E896AD4C686ull, 0x74289E896AD4C686ull },
	{ 0xF566A1DA88725120ull, 0xF566A1DA88725120ull },
	{ 0xF8DB42658CB157DDull, 0xF8DB42658CB157DDull },
	{ 0xD1281B99CB0AD231ull, 0xD1281B99CB0AD231ull },
	{ 0x4C6709E5B90384A0ull, 0x4C6709E5B90384A0ull },
	{ 0xB0FD2BD786D3EA89ull, 0xB0FD2BD786D3EA89ull },
	{ 0x68FD70E55A02AA94ull, 0x68FD70E55A02AA94ull },
	{ 0xC52674B02574FECAull, 0xC52674B02574FECAull },
	{ 0x5EA89F358319013Eull, 0x5EA89F358319013Eull },
	{ 0x2C034608BCFF2FB1ull, 0x2C034608BCFF2FB1ull },
	{ 0xD596B6329353BF11ull, 0xD596B6329353BF11ull },
	{ 0x4CD7A468940A9436ull, 0x4CD7A468940A9436ull },
	{ 0x683D87A30213D75Aull, 0x683D87A30213D75Aull },
	{ 0x431D5A484B0F2C5Aull, 0x431D5A484B0F2C5Aull },
	{ 0xDACC831F677E100Eull, 0xDACC831F677E100Eull },
	{ 0xC47201440FCFC978ull, 0xC47201440FCFC978ull },
	{ 0x33802455D69F888Cull, 0x33802455D69F888Cull },
	{ 0x73A4F7E7D5180A0Bull, 0x73A4F7E7D5180A0Bull },
	{ 0xC7D0009FEE739651ull, 0xC7D0009FEE739651ull },
	{ 0xE23085988536A22Aull, 0xE23085988536A22Aull },
	{ 0xCFFE9CB9592D5FDAull, 0xCFFE9CB9592D5FDAull },
	{ 0x7F06164A8E84928Bull, 0x7F06164A8E84928Bull },
	{ 0x7330840402DA1652ull, 0x7330840402DA1652ull },
	{ 0x5C067EC85D5D8266ull, 0x5C067EC85D5D8266ull },
	{ 0xDD1724A2DFF17287ull, 0xDD1724A2DFF17287ull },
	{ 0x6AB514C13BAE2FE2ull, 0x6AB514C13BAE2FE2ull },
	{ 0xD27F21DEF845E04Dull, 0xD27F21DEF845E04Dull },
	{ 0x4FF5DC4483917E8Aull, 0x4FF5DC4483917E8Aull },
	{ 0x48470A94C627CF4Bull, 0x48470A94C627CF4Bull },
	{ 0x22DB094B8A0231E0ull, 0x22DB094B8A0231E0ull },
	{ 0xEA5041AB484EB5A9ull, 0xEA5041AB484EB5A9ull },
	{ 0x451365435F7D6DEBull, 0x451365435F7D6DEBull },
	{ 0xD6857AE4D8031A1Dull, 0xD6857AE4D8031A1Dull },
	{ 0x4CDC37B94DD76CEFull, 0x4CDC37B94DD76CEFull },
	{ 0x39936FA2529DB1C5ull, 0x39936FA2529DB1C5ull },
	{ 0x61AFE3F32DE3750Cull, 0x61AFE3F32DE3750Cull },
	{ 0xB8F55AD843AC1635ull, 0xB8F55AD843AC1635ull },
	{ 0x2D42138D16752986ull, 0x2D42138D16752986ull },
	{ 0xB2567A7097227C30ull, 0xB2567A7097227C30ull },
	{ 0x4C414B33FFD325DBull, 0x4C414B33FFD325DBull },
	{ 0xCBA45ADB62982339ull, 0xCBA45ADB62982339ull },
	{ 0x79E9175DE3008A35ull, 0x79E9175DE3008A35ull },
	{ 0x6E6D9B39C40CD4D3ull, 0x6E6D9B39C40CD4D3ull },
	{ 0x7F36B219C3ACFF9Aull, 0x7F36B219C3ACFF9Aull },
	{ 0xAF263812C549E25Dull, 0xAF263812C549E25Dull },
	{ 0xD3693FFF265D2AB5ull, 0xD3693FFF265D2AB5ull },
	{ 0x303BD9F94FAAF69Dull, 0x303BD9F94FAAF69Dull },
	{ 0x3A049A956E8F3A34ull, 0x3A049A956E8F3A34ull },
	{ 0xC1D04C215BD31389ull, 0xC1D04C215BD31389ull },
	{ 0x7CA47BC84A6C90E4ull, 0x7CA47BC84A6C90E4ull },
	{ 0xA9386695098680CFull, 0xA9386695098680CFull },
	{ 0xDBB4BCF35ABDC0CCull, 0xDBB4BCF35ABDC0CCull },
	{ 0x452FD4A01A114A26ull, 0x452FD4A01A114A26ull },
	{ 0xB0BA643BF084F165ull, 0xB0BA643BF084F165ull },
	{ 0x7F915A2DFFD182B1ull, 0x7F915A2DFFD182B1ull },
	{ 0x430564826F7061B2ull, 0x430564826F7061B2ull },
	{ 0x7F193AE2B6C923A9ull, 0x7F193AE2B6C923A9ull },
	{ 0xA8FE1D2AD315812Full, 0xA8FE1D2AD315812Full },
	{ 0x373EAC9310C8A0E9ull, 0x373EAC9310C8A0E9ull },
	{ 0x76642F3676FB0B34ull, 0x76642F3676FB0B34ull },
	{ 0x6B77EFB22B3865CEull, 0x6B77EFB22B3865CEull },
	{ 0xFEAD56A8D2A8EEA2ull, 0xFEAD56A8D2A8EEA2ull },
	{ 0xB1FB764119C75624ull, 0xB1FB764119C75624ull },
	{ 0x2311CCB6D550482Dull, 0x2311CCB6D550482Dull },
	{ 0xEBE95827F3F7C87Aull, 0xEBE95827F3F7C87Aull },
	{ 0x61C5089395EC96EDull, 0x61C5089395EC96EDull },
	{ 0x99424B9D8B5A3680ull, 0x99424B9D8B5A3680ull },
	{ 0x6D82207D2F53C9F5ull, 0x6D82207D2F53C9F5ull },
	{ 0x973D79C70A8580A6ull, 0x973D79C70A8580A6ull },
	{ 0xA7ADFCAF0F4C0EB6ull, 0xA7ADFCAF0F4C0EB6ull },
	{ 0x557113423F3684B4ull, 0x557113423F3684B4ull },
	{ 0x72D25D390FCBE7D7ull, 0x72D25D390FCBE7D7ull },
	{ 0x7807942AEFE28A7Aull, 0x7807942AEFE28A7Aull },
	{ 0xB8264D5D6E0D78CBull, 0xB8264D5D6E0D78CBull },
	{ 0xE2AE8C2778E64D2Eull, 0xE2AE8C2778E64D2Eull },
	{ 0xEFA8F6D25576D773ull, 0xEFA8F6D25576D773ull },
	{ 0xECE93E6A0CBC10C5ull, 0xECE93E6A0CBC10C5ull },
	{ 0x47C393A2B08FF32Dull, 0x47C393A2B08FF32Dull },
	{ 0xFBB0C7F1E5CC4AF8ull, 0xFBB0C7F1E5CC4AF8ull },
	{ 0xEDBAD34B070702EEull, 0xEDBAD34B070702EEull },
	{ 0x5232C5AC368E93B7ull, 0x5232C5AC368E93B7ull },
	{ 0xB70835E83026AA36ull, 0xB70835E83026AA36ull },
	{ 0xB40AD0408EA16561ull, 0xB40AD0408EA16561ull },
	{ 0x79EC4472C0D1E5ABull, 0x79EC4472C0D1E5ABull },
	{ 0x92616FF91DC21139ull, 0x92616FF91DC21139ull },
	{ 0x41A24DF850994011ull, 0x41A24DF850994011ull },
	{ 0x7631AB5D8427E388ull, 0x7631AB5D8427E388ull },
	{ 0x49958B0EFD6CA06Eull, 0x49958B0EFD6CA06Eull },
	{ 0x4BF11770C33B7F6Aull, 0x4BF11770C33B7F6Aull },
	{ 0xD756BDB4C76379AAull, 0xD756BDB4C76379AAull },
	{ 0x7F3CFC016FB7AFBFull, 0x7F3CFC016FB7AFBFull },
	{ 0xE5D5F03AA282C6BAull, 0xE5D5F03AA282C6BAull },
	{ 0xB8C9CFA9933B5612ull, 0xB8C9CFA9933B5612ull },
	{ 0x1E8BB3004BF496E3ull, 0x1E8BB3004BF496E3ull },
	{ 0xD653549F8FDA8C7Dull, 0xD653549F8FDA8C7Dull },
	{ 0x3FE9BDC03169BCA3ull, 0x3FE9BDC03169BCA3ull },
	{ 0xA1D468A062AF98C7ull, 0xA1D468A062AF98C7ull },
	{ 0x70A203CBB553C4D7ull, 0x70A203CBB553C4D7ull },
	{ 0x2585ECC6432AEBB1ull, 0x2585ECC6432AEBB1ull },
	{ 0x512E595D3B76E73Dull, 0x512E595D3B76E73Dull },
	{ 0x653D2D52707ECDDDull, 0x653D2D52707ECDDDull },
	{ 0xC3AEA55EF04B7C51ull, 0xC3AEA55EF04B7C51ull },
	{ 0x6DBDDA9F7F27806Eull, 0x6DBDDA9F7F27806Eull },
	{ 0x5DD3EA136A13DD22ull, 0x5DD3EA136A13DD22ull },
	{ 0xF530F079CB229CE7ull, 0xF530F079CB229CE7ull },
	{ 0xFD57B1374E7338DFull, 0xFD57B1374E7338DFull },
	{ 0x5C8161808EC1F60Full, 0x5C8161808EC1F60Full },
	{ 0xDFA515C1DA4BF25Eull, 0xDFA515C1DA4BF25Eull },
	{ 0x76D1A762F42B2330ull, 0x76D1A762F42B2330ull },
	{ 0x66C1A05FBBF5A35Cull, 0x66C1A05FBBF5A35Cull },
	{ 0x4DD1D431C783FB73ull, 0x4DD1D431C783FB73ull },
	{ 0xA9A9D83E5D5E4888ull, 0xA9A9D83E5D5E4888ull },
	{ 0x6A9614385302E266ull, 0x6A9614385302E266ull },
	{ 0xF3BE8BC3E75C4E5Cull, 0xF3BE8BC3E75C4E5Cull },
	{ 0xCE1C7EB48076DA05ull, 0xCE1C7EB48076DA05ull },
	{ 0x3273AE13DA55D740ull, 0x3273AE13DA55D740ull },
	{ 0x7160C895A8F900EDull, 0x7160C895A8F900EDull },
	{ 0x72958EEC622F4A23ull, 0x72958EEC622F4A23ull },
	{ 0x52D10CD025295576ull, 0x52D10CD025295576ull },
	{ 0x782C808AF07EB915ull, 0x782C808AF07EB915ull },
	{ 0xDAC0FF65CEDE534Dull, 0xDAC0FF65CEDE534Dull },
	{ 0x75863F4E753D1B32ull, 0x75863F4E753D1B32ull },
	{ 0xE809D9281C71D086ull, 0xE809D9281C71D086ull },
	{ 0xA3F3D85D69F42D8Aull, 0xA3F3D85D69F42D8Aull },
	{ 0x5682AE36F7F75576ull, 0x5682AE36F7F75576ull },
	{ 0x14B4602148FE0E00ull, 0x14B4602148FE0E00ull },
	{ 0x6E12FEE03D7A771Bull, 0x6E12FEE03D7A771Bull },
	{ 0xB30E4666E1869461ull, 0xB30E4666E1869461ull },
	{ 0x6FF774B6D6107E44ull, 0x6FF774B6D6107E44ull },
	{ 0x47CAA363B2BF27FAull, 0x47CAA363B2BF27FAull },
	{ 0xBC9BCD4A8020811Full, 0xBC9BCD4A8020811Full },
	{ 0x769363B0C2736C9Eull, 0x769363B0C2736C9Eull },
	{ 0xAD3E1B106176BE13ull, 0xAD3E1B106176BE13ull },
	{ 0xBA8482EFD6698AC9ull, 0xBA8482EFD6698AC9ull },
	{ 0x943E7197381044C7ull, 0x943E7197381044C7ull },
	{ 0xD292BC17258D8B9Aull, 0xD292BC17258D8B9Aull },
	{ 0x53CF334C93F477C6ull, 0x53CF334C93F477C6ull },
	{ 0x709672360C0B3203ull, 0x709672360C0B3203ull },
	{ 0x4D40C7ADE9335B9Cull, 0x4D40C7ADE9335B9Cull },
	{ 0xFA61D3FD2CF55598ull, 0xFA61D3FD2CF55598ull },
	{ 0xC05E6B7035EFCE0Eull, 0xC05E6B7035EFCE0Eull },
	{ 0x1BC07AB053922680ull, 0x1BC07AB053922680ull },
	{ 0xC3358CB2A08FE033ull, 0xC3358CB2A08FE033ull },
	{ 0x13F2473A36BC7B17ull, 0x13F2473A36BC7B17ull },
	{ 0x911D70229B3A9561ull, 0x911D70229B3A9561ull },
	{ 0xED10C85DFFAEA746ull, 0xED10C85DFFAEA746ull },
	{ 0xC6A074160B71D3CAull, 0xC6A074160B71D3CAull },
	{ 0x6CCF418B7565880Cull, 0x6CCF418B7565880Cull },
	{ 0x29414E1254D57B18ull, 0x29414E1254D57B18ull },
	{ 0x8FBDD82F237642F6ull, 0x8FBDD82F237642F6ull },
	{ 0xFB5C943C1876E487ull, 0xFB5C943C1876E487ull },
	{ 0xEEA24AF587C3380Aull, 0xEEA24AF587C3380Aull },
	{ 0x24B38F1FE50DC163ull, 0x24B38F1FE50DC163ull },
	{ 0x638862115C0964C6ull, 0x638862115C0964C6ull },
	{ 0x3B6E4D92ED2BA993ull, 0x3B6E4D92ED2BA993ull },
	{ 0x7451D2A05F75E529ull, 0x7451D2A05F75E529ull },
	{ 0x2183FCA2BB9718BFull, 0x2183FCA2BB9718BFull },
	{ 0xC968AA9E62026AA2ull, 0xC968AA9E62026AA2ull },
	{ 0x5AD5FA0D668AA8C6ull, 0x5AD5FA0D668AA8C6ull },
	{ 0x187112950C93DD6Bull, 0x187112950C93DD6Bull },
	{ 0x7765A8BB59370866ull, 0x7765A8BB59370866ull },
	{ 0x7B66ECFD4608362Aull, 0x7B66ECFD4608362Aull },
	{ 0x3E63ACCBC271F6CBull, 0x3E63ACCBC271F6CBull },
	{ 0xCA94E886B8D13143ull, 0xCA94E886B8D13143ull },
	{ 0xAAA26E71A083621Eull, 0xAAA26E71A083621Eull },
	{ 0xC538698585D4E10Full, 0xC538698585D4E10Full },
	{ 0xB4316C9AF634149Eull, 0xB4316C9AF634149Eull },
	{ 0x4FD635971ED9125Bull, 0x4FD635971ED9125Bull },
	{ 0xED7FA08CA127CE79ull, 0xED7FA08CA127CE79ull },
	{ 0xE033C95E44E4487Cull, 0xE033C95E44E4487Cull },
	{ 0xEDDDC37350487229ull, 0xEDDDC37350487229ull },
	{ 0x6B0B0F2690F4513Full, 0x6B0B0F2690F4513Full },
	{ 0x34E3988AEC3D99C6ull, 0x34E3988AEC3D99C6ull },
	{ 0xD5BB64133E824E10ull, 0xD5BB64133E824E10ull },
	{ 0xDD08EA40D2CF08A1ull, 0xDD08EA40D2CF08A1ull },
	{ 0xC2AEB22959BF7A90ull, 0xC2AEB22959BF7A90ull },
	{ 0xCE973DC518171AA5ull, 0xCE973DC518171AA5ull },
	{ 0xFACE0AF6F341DC04ull, 0xFACE0AF6F341DC04ull },
	{ 0x756F4849BD465607ull, 0x756F4849BD465607ull },
	{ 0x726927D127691F89ull, 0x726927D127691F89ull },
	{ 0x354CF6CCBC10EA5Dull, 0x354CF6CCBC10EA5Dull },
	{ 0xDFF04C4620EFB7AFull, 0xDFF04C4620EFB7AFull },
	{ 0xFDDFC7A31CBCC22Dull, 0xFDDFC7A31CBCC22Dull },
	{ 0x8837B9B6002FFD6Cull, 0x8837B9B6002FFD6Cull },
	{ 0xBC55B7A57430A984ull, 0xBC55B7A57430A984ull },
	{ 0x54AF7904A760EE59ull, 0x54AF7904A760EE59ull },
	{ 0x51A7ED6A8674B083ull, 0x51A7ED6A8674B083ull },
	{ 0x517087CB8172181Cull, 0x517087CB8172181Cull },
	{ 0xF9EB7B7A5EC9A0EEull, 0xF9EB7B7A5EC9A0EEull },
	{ 0x1BC9D2173BFA8025ull, 0x1BC9D2173BFA8025ull },
	{ 0xF780B81E3DDC6A25ull, 0xF780B81E3DDC6A25ull },
	{ 0xF953B15AB2B96E21ull, 0xF953B15AB2B96E21ull },
	{ 0x75D227ECD3F014E9ull, 0x75D227ECD3F014E9ull },
	{ 0xE184368F2E14A716ull, 0xE184368F2E14A716ull },
	{ 0x562AF3F0DD6000AAull, 0x562AF3F0DD6000AAull },
	{ 0x6F8B5F56AE24B030ull, 0x6F8B5F56AE24B030ull },
	{ 0x554A9F29579BF315ull, 0x554A9F29579BF315ull },
	{ 0xC3A619789EF6314Dull, 0xC3A619789EF6314Dull },
	{ 0xFA529906CA792D3Aull, 0xFA529906CA792D3Aull },
	{ 0x622314421A21FAA7ull, 0x622314421A21FAA7ull },
	{ 0x62BF9F1D6CD32706ull, 0x62BF9F1D6CD32706ull },
	{ 0xE0243848C9C01E9Eull, 0xE0243848C9C01E9Eull },
	{ 0xD731219A4D4A07EEull, 0xD731219A4D4A07EEull },
	{ 0xA149400BEC297B8Eull, 0xA149400BEC297B8Eull },
	{ 0xC92F91DA63D0EDFCull, 0xC92F91DA63D0EDFCull },
	{ 0x37E3DB85D50E72B5ull, 0x37E3DB85D50E72B5ull },
	{ 0x97EC01CD38604783ull, 0x97EC01CD38604783ull },
	{ 0x737663817349A594ull, 0x737663817349A594ull },
	{ 0xEC7C515132EBCCF0ull, 0xEC7C515132EBCCF0ull },
	{ 0xFCC813C44214EE17ull, 0xFCC813C44214EE17ull },
	{ 0xC5AA8B5F16D20462ull, 0xC5AA8B5F16D20462ull },
	{ 0xD4A24FD02EF6D4DDull, 0xD4A24FD02EF6D4DDull },
	{ 0xB35FF5744A5B1F3Eull, 0xB35FF5744A5B1F3Eull },
	{ 0x66051C0DD2B87D15ull, 0x66051C0DD2B87D15ull },
	{ 0xA1BC096EFDD0F518ull, 0xA1BC096EFDD0F518ull },
	{ 0x29BF7D901CE90F49ull, 0x29BF7D901CE90F49ull },
	{ 0xF87FDDF26BAC6100ull, 0xF87FDDF26BAC6100ull },
	{ 0x32B1F96F6921CE92ull, 0x32B1F96F6921CE92ull },
	{ 0x3D71B1702D8D4769ull, 0x3D71B1702D8D4769ull },
	{ 0xCC14FDBE0457AEBEull, 0xCC14FDBE0457AEBEull },
	{ 0xBF86EA4B617DED01ull, 0xBF86EA4B617DED01ull },
	{ 0x7613B881B8B2B32Full, 0x7613B881B8B2B32Full },
	{ 0xA92A46B690FC7212ull, 0xA92A46B690FC7212ull },
	{ 0x7E4089192C6DB45Dull, 0x7E4089192C6DB45Dull },
	{ 0xB23A89448C510999ull, 0xB23A89448C510999ull },
	{ 0xDA12B9E011FB8438ull, 0xDA12B9E011FB8438ull },
	{ 0x29000BA61A45CD7Cull, 0x29000BA61A45CD7Cull },
	{ 0xF5437B9A8C0E4F06ull, 0xF5437B9A8C0E4F06ull },
	{ 0x945E28873F1675CFull, 0x945E28873F1675CFull },
	{ 0xD1C437FE5D40111Aull, 0xD1C437FE5D40111Aull },
	{ 0xFFD6D0DB6745FA88ull, 0xFFD6D0DB6745FA88ull },
	{ 0x2BD450E7B5FD453Eull, 0x2BD450E7B5FD453Eull },
	{ 0x6D70819F4D58C7F1ull, 0x6D70819F4D58C7F1ull },
	{ 0xD9B655EA4FDDAD93ull, 0xD9B655EA4FDDAD93ull },
	{ 0x3061951C575D4EB2ull, 0x3061951C575D4EB2ull },
	{ 0xCD91FD298C4F12D4ull, 0xCD91FD298C4F12D4ull },
	{ 0x563D3648D934524Full, 0x563D3648D934524Full },
	{ 0xA26BA13D96F5936Full, 0xA26BA13D96F5936Full },
	{ 0x62C2FD2F20F0BA54ull, 0x62C2FD2F20F0BA54ull },
	{ 0x28B59117D6769D83ull, 0x28B59117D6769D83ull },
	{ 0xE15356B882935103ull, 0xE15356B882935103ull },
	{ 0xFB458DE4751F09FEull, 0xFB458DE4751F09FEull },
	{ 0xBC6573C9B6A083D1ull, 0xBC6573C9B6A083D1ull },
	{ 0xBD5B5A8AF5ABFFE9ull, 0xBD5B5A8AF5ABFFE9ull },
	{ 0xAD4F889108B8D516ull, 0xAD4F889108B8D516ull },
	{ 0xDC99AE2C048F476Eull, 0xDC99AE2C048F476Eull },
	{ 0x9DEF3515B19916A5ull, 0x9DEF3515B19916A5ull },
	{ 0x82A13F7D3BAC0A12ull, 0x82A13F7D3BAC0A12ull },
	{ 0xED9881FA4293399Aull, 0xED9881FA4293399Aull },
	{ 0x4C64111068643452ull, 0x4C64111068643452ull },
	{ 0xDC10C49EFA78FE8Cull, 0xDC10C49EFA78FE8Cull },
	{ 0x21E462D9EDF2321Eull, 0x21E462D9EDF2321Eull },
	{ 0x580014B72ABF3AF3ull, 0x580014B72ABF3AF3ull },
	{ 0x0E75A08B24CAB3D7ull, 0x0E75A08B24CAB3D7ull },
	{ 0xBF72EE5FC988E6FFull, 0xBF72EE5FC988E6FFull },
	{ 0x98BF52A0B9866A57ull, 0x98BF52A0B9866A57ull },
	{ 0xB7AE2267D374DD8Full, 0xB7AE2267D374DD8Full },
	{ 0x9A720E450ED3C967ull, 0x9A720E450ED3C967ull },
	{ 0x04E4C62CA8739B3Aull, 0x04E4C62CA8739B3Aull },
	{ 0x98DE64E69B70C648ull, 0x98DE64E69B70C648ull },
	{ 0x6300234A6E24FE48ull, 0x6300234A6E24FE48ull },
	{ 0x9C1DD4624B6C98AEull, 0x9C1DD4624B6C98AEull },
	{ 0xDCF9A8006F3E0738ull, 0xDCF9A8006F3E0738ull },
	{ 0x87EDB741DFF179BEull, 0x87EDB741DFF179BEull },
	{ 0x24F6D7297B36983Aull, 0x24F6D7297B36983Aull },
	{ 0xF30C348E4F166495ull, 0xF30C348E4F166495ull },
	{ 0x6C6737C846BF3634ull, 0x6C6737C846BF3634ull },
	{ 0xFF972FD623F8D7C2ull, 0xFF972FD623F8D7C2ull },
	{ 0xBC1445C7CB7EC73Full, 0xBC1445C7CB7EC73Full },
	{ 0xB2FD38C7DE9E91C9ull, 0xB2FD38C7DE9E91C9ull },
	{ 0x4BE3614FA7D853A9ull, 0x4BE3614FA7D853A9ull },
	{ 0x4DB07C20541F54A0ull, 0x4DB07C20541F54A0ull },
	{ 0x5B19BF0C7FB8C673ull, 0x5B19BF0C7FB8C673ull },
	{ 0xA39BB2ED0ED6A334ull, 0xA39BB2ED0ED6A334ull },
	{ 0x0812584C5B6CAB52ull, 0x0812584C5B6CAB52ull },
	{ 0xF3C91282AD9D8363ull, 0xF3C91282AD9D8363ull },
	{ 0xA8A5DAEF08BDF130ull, 0xA8A5DAEF08BDF130ull },
	{ 0x409B3CC7444C3B81ull, 0x409B3CC7444C3B81ull },
	{ 0xA1B498EA79D9A82Aull, 0xA1B498EA79D9A82Aull },
	{ 0xA7983070BC8416D5ull, 0xA7983070BC8416D5ull },
	{ 0x3F82792326FDC311ull, 0x3F82792326FDC311ull },
	{ 0x39A6387734011F9Bull, 0x39A6387734011F9Bull },
	{ 0x0D74E6D86C2ED66Bull, 0x0D74E6D86C2ED66Bull },
	{ 0x52CADC1B5E33E382ull, 0x52CADC1B5E33E382ull },
	{ 0x2E52C1A4EB75D7F2ull, 0x2E52C1A4EB75D7F2ull },
	{ 0xABAF5B56D019382Cull, 0xABAF5B56D019382Cull },
	{ 0x8A3603FA4C478C7Cull, 0x8A3603FA4C478C7Cull },
	{ 0x9E00F59FDF0491DCull, 0x9E00F59FDF0491DCull },
	{ 0xBCCD24700AB7CF14ull, 0xBCCD24700AB7CF14ull },
	{ 0x02D47BB1C808897Eull, 0x02D47BB1C808897Eull },
	{ 0xD662CD28E8FF6DEFull, 0xD662CD28E8FF6DEFull },
	{ 0xBDA5C8724A3564E9ull, 0xBDA5C8724A3564E9ull },
	{ 0x4A277B00D83D7E7Full, 0x4A277B00D83D7E7Full },
	{ 0x573AD74E60D7FE07ull, 0x573AD74E60D7FE07ull },
	{ 0x5C99C3E336DC8B86ull, 0x5C99C3E336DC8B86ull },
	{ 0x958C3407A5297FD8ull, 0x958C3407A5297FD8ull },
	{ 0xBB37C5E230C6519Eull, 0xBB37C5E230C6519Eull },
	{ 0x13DF6B15ACC726ECull, 0x13DF6B15ACC726ECull },
	{ 0x950297621DDF7E33ull, 0x950297621DDF7E33ull },
	{ 0xDAC3793B6B3201EBull, 0xDAC3793B6B3201EBull },
	{ 0xB213B7B8E34B7105ull, 0xB213B7B8E34B7105ull },
	{ 0xF7FF3BB08F56B20Full, 0xF7FF3BB08F56B20Full },
	{ 0x0BE47E4153F7FCA8ull, 0x0BE47E4153F7FCA8ull },
	{ 0x9691E0BEE69D2A27ull, 0x9691E0BEE69D2A27ull },
	{ 0x65078B628EF12406ull, 0x65078B628EF12406ull },
	{ 0x16D55653BA806F94ull, 0x16D55653BA806F94ull },
	{ 0x6154378B607CDE5Aull, 0x6154378B607CDE5Aull },
	{ 0xA632BB65326E594Dull, 0xA632BB65326E594Dull },
	{ 0x9E1403C1A1BED235ull, 0x9E1403C1A1BED235ull },
	{ 0x5798CF8BB16F4FDDull, 0x5798CF8BB16F4FDDull },
	{ 0xC9A209E599641AFBull, 0xC9A209E599641AFBull },
	{ 0xBAD4624F39160DE1ull, 0xBAD4624F39160DE1ull },
	{ 0xBBAC6BC95AD70365ull, 0xBBAC6BC95AD70365ull },
	{ 0x755AC448C6F7608Dull, 0x755AC448C6F7608Dull },
	{ 0x8CE7C45C263AC97Eull, 0x8CE7C45C263AC97Eull },
	{ 0xD9963657B5A9E7A6ull, 0xD9963657B5A9E7A6ull },
	{ 0x2FB49669C8EF5DA8ull, 0x2FB49669C8EF5DA8ull },
	{ 0xFF6459AB1D4DD308ull, 0xFF6459AB1D4DD308ull },
	{ 0x0766BDADFD5D2216ull, 0x0766BDADFD5D2216ull },
	{ 0xDBB6A03F32DCA61Full, 0xDBB6A03F32DCA61Full },
	{ 0x009EDDF67AD7FEAFull, 0x009EDDF67AD7FEAFull },
	{ 0xD8E4BC4EA7C0F5EFull, 0xD8E4BC4EA7C0F5EFull },
	{ 0x2935D4F8E5AE7C37ull, 0x2935D4F8E5AE7C37ull },
	{ 0xEE6110F80B76288Full, 0xEE6110F80B76288Full },
	{ 0xC86A268D0F26CFF5ull, 0xC86A268D0F26CFF5ull },
	{ 0xB90F213728F03828ull, 0xB90F213728F03828ull },
	{ 0xF69F70C575914E92ull, 0xF69F70C575914E92ull },
	{ 0x92CC013269CB6F78ull, 0x92CC013269CB6F78ull },
	{ 0x355FD9CC1488D5E5ull, 0x355FD9CC1488D5E5ull },
	{ 0xAAF8C53C33842A53ull, 0xAAF8C53C33842A53ull },
	{ 0x07AF8053145F319Bull, 0x07AF8053145F319Bull },
	{ 0x9E06F2084936989Full, 0x9E06F2084936989Full },
	{ 0x09A1EB342A20E3B5ull, 0x09A1EB342A20E3B5ull },
	{ 0x0DE6ED352A267B55ull, 0x0DE6ED352A267B55ull },
	{ 0x7D54F6D8F4D01644ull, 0x7D54F6D8F4D01644ull },
	{ 0x30300D32B10C7F20ull, 0x30300D32B10C7F20ull },
	{ 0x11E0F8B0D97BDC5Aull, 0x11E0F8B0D97BDC5Aull },
	{ 0xC370914EAAF8F6E9ull, 0xC370914EAAF8F6E9ull },
	{ 0xB26A0384A52B8820ull, 0xB26A0384A52B8820ull },
	{ 0x009063748159396Cull, 0x009063748159396Cull },
	{ 0x6FCD21D779E4114Bull, 0x6FCD21D779E4114Bull },
	{ 0x1198F522BF2F760Full, 0x1198F522BF2F760Full },
	{ 0xFB389C5901A73954ull, 0xFB389C5901A73954ull },
	{ 0xBCDF12D026F9313Eull, 0xBCDF12D026F9313Eull },
	{ 0xAC689FEA73AA3150ull, 0xAC689FEA73AA3150ull },
	{ 0x52E1192EC8088908ull, 0x52E1192EC8088908ull },
	{ 0xFEA9128B61DFA371ull, 0xFEA9128B61DFA371ull },
	{ 0xBC4FCD4F64B4A0F2ull, 0xBC4FCD4F64B4A0F2ull },
	{ 0x9152D7196AD05835ull, 0x9152D7196AD05835ull },
	{ 0xA19B26D4D5C77C39ull, 0xA19B26D4D5C77C39ull },
	{ 0x78EA63162B8860B8ull, 0x78EA63162B8860B8ull },
	{ 0xB925CEA585D4613Aull, 0xB925CEA585D4613Aull },
	{ 0x6BE45ED6122AE77Aull, 0x6BE45ED6122AE77Aull },
	{ 0x37DD78799F4C0FF8ull, 0x37DD78799F4C0FF8ull },
	{ 0x3B2DD54F3E18F0DEull, 0x3B2DD54F3E18F0DEull },
	{ 0xC8A0123BEE378098ull, 0xC8A0123BEE378098ull },
	{ 0x49EE63AC5F001BADull, 0x49EE63AC5F001BADull },
	{ 0x84CAF46EBE9AA8E3ull, 0x84CAF46EBE9AA8E3ull },
	{ 0x056469C397116093ull, 0x056469C397116093ull },
	{ 0x6BB41234BEFF3549ull, 0x6BB41234BEFF3549ull },
	{ 0xDD5B39F157DFD320ull, 0xDD5B39F157DFD320ull },
	{ 0xDB3D293DDD674D5Bull, 0xDB3D293DDD674D5Bull },
	{ 0xBB8B48F744A75EC6ull, 0xBB8B48F744A75EC6ull },
	{ 0x0740F1382A28534Eull, 0x0740F1382A28534Eull },
	{ 0xFC245E1F11BB8B5Full, 0xFC245E1F11BB8B5Full },
	{ 0xF22A4D04B56DA2FDull, 0xF22A4D04B56DA2FDull },
	{ 0xE581C301641E3826ull, 0xE581C301641E3826ull },
	{ 0x6455799071235166ull, 0x6455799071235166ull },
	{ 0xA0487450B27E60BAull, 0xA0487450B27E60BAull },
	{ 0x648B3E711008EE2Aull, 0x648B3E711008EE2Aull },
	{ 0xAF4F5068239EDA21ull, 0xAF4F5068239EDA21ull },
	{ 0xAFB61D85E62C440Full, 0xAFB61D85E62C440Full },
	{ 0xE7A1491E70251241ull, 0xE7A1491E70251241ull },
	{ 0x313183BCC7CC167Full, 0x313183BCC7CC167Full },
	{ 0x68C991BCC0CFD266ull, 0x68C991BCC0CFD266ull },
	{ 0xFA72F7BAEAA2E188ull, 0xFA72F7BAEAA2E188ull },
	{ 0x9439D0B99015FCC1ull, 0x9439D0B99015FCC1ull },
	{ 0xBB3D4B29C5D99EDCull, 0xBB3D4B29C5D99EDCull },
	{ 0x05DA4EF7896D4E66ull, 0x05DA4EF7896D4E66ull },
	{ 0xB97DCA47F0A27F51ull, 0xB97DCA47F0A27F51ull },
	{ 0x8BD427A868791D4Bull, 0x8BD427A868791D4Bull },
	{ 0xC9636B7E03FBAEE0ull, 0xC9636B7E03FBAEE0ull },
	{ 0x9E9578B3DAE11B4Eull, 0x9E9578B3DAE11B4Eull },
	{ 0x45AFFFB7799A12A8ull, 0x45AFFFB7799A12A8ull },
	{ 0xD89F92425ED0122Dull, 0xD89F92425ED0122Dull },
	{ 0x86C3629F8C3C0AEAull, 0x86C3629F8C3C0AEAull },
	{ 0x6ECF274C235AA70Dull, 0x6ECF274C235AA70Dull },
	{ 0x5BF0AA221764310Cull, 0x5BF0AA221764310Cull },
	{ 0xA84E7CEBE8CE18F6ull, 0xA84E7CEBE8CE18F6ull },
	{ 0x824D571077FC1898ull, 0x824D571077FC1898ull },
	{ 0xD6719B35E0A6FF54ull, 0xD6719B35E0A6FF54ull },
	{ 0x9D5A844A6B9F103Dull, 0x9D5A844A6B9F103Dull },
	{ 0xCCFC7C128CB00DF6ull, 0xCCFC7C128CB00DF6ull },
	{ 0xAED8176AF10CDE4Bull, 0xAED8176AF10CDE4Bull },
	{ 0xD9933C9DA008D110ull, 0xD9933C9DA008D110ull },
	{ 0x8EB1F07928048539ull, 0x8EB1F07928048539ull },
	{ 0x3C744BE8DF546FBAull, 0x3C744BE8DF546FBAull },
	{ 0x635719FD31870020ull, 0x635719FD31870020ull },
	{ 0x9FFC9C02FF906A70ull, 0x9FFC9C02FF906A70ull },
	{ 0x991AF8D90EC205C0ull, 0x991AF8D90EC205C0ull },
	{ 0x10C5D10287535B13ull, 0x10C5D10287535B13ull },
	{ 0xDAB07CC582E2C578ull, 0xDAB07CC582E2C578ull },
	{ 0x75BE71D4AD33B7E5ull, 0x75BE71D4AD33B7E5ull },
	{ 0x094F4FF2F150A7CFull, 0x094F4FF2F150A7CFull },
	{ 0x87DC17B039D68E0Eull, 0x87DC17B039D68E0Eull },
	{ 0x340306AE3F101170ull, 0x340306AE3F101170ull },
	{ 0xCA979E4C6666479Bull, 0xCA979E4C6666479Bull },
	{ 0x90545029B14D2028ull, 0x90545029B14D2028ull },
	{ 0x8393543BBCF166FCull, 0x8393543BBCF166FCull },
	{ 0xF21C25C9CF8E6B68ull, 0xF21C25C9CF8E6B68ull },
	{ 0xB2AB29EAF8F14784ull, 0xB2AB29EAF8F14784ull },
	{ 0x4320FBD640CFC4C8ull, 0x4320FBD640CFC4C8ull },
	{ 0x09FEE2E3E678E16Bull, 0x09FEE2E3E678E16Bull },
	{ 0x7BDDF08EAE44F7FFull, 0x7BDDF08EAE44F7FFull },
	{ 0x65C64514428B844Dull, 0x65C64514428B844Dull },
	{ 0x9AA3FD23A1448AEEull, 0x9AA3FD23A1448AEEull },
	{ 0xC4DF48789B9B9BAAull, 0xC4DF48789B9B9BAAull },
	{ 0x9C45255364DA7C94ull, 0x9C45255364DA7C94ull },
	{ 0xBFF647EB31316C66ull, 0xBFF647EB31316C66ull },
	{ 0x73C9A6EAE4830799ull, 0x73C9A6EAE4830799ull },
	{ 0x024830F6D2F9D3C6ull, 0x024830F6D2F9D3C6ull },
	{ 0x825F2A6A8D4D5FC6ull, 0x825F2A6A8D4D5FC6ull },
	{ 0xC11EB30DC27D0BFDull, 0xC11EB30DC27D0BFDull },
	{ 0x0F71F30F2C667189ull, 0x0F71F30F2C667189ull },
	{ 0x341670C491BD6695ull, 0x341670C491BD6695ull },
	{ 0xC0F00E5ED10BA5F4ull, 0xC0F00E5ED10BA5F4ull },
	{ 0x8D2F2EF103FE1182ull, 0x8D2F2EF103FE1182ull },
	{ 0x12B68459AAA59DBDull, 0x12B68459AAA59DBDull },
	{ 0xF056048CED71D29Eull, 0xF056048CED71D29Eull },
	{ 0xCF08876EEA0AF4BDull, 0xCF08876EEA0AF4BDull },
	{ 0x6C098BBD1351CA24ull, 0x6C098BBD1351CA24ull },
	{ 0x0660A4D5A0EB0172ull, 0x0660A4D5A0EB0172ull },
	{ 0x70D8107C0C5C3434ull, 0x70D8107C0C5C3434ull },
	{ 0x078BF3E9A1E0927Full, 0x078BF3E9A1E0927Full },
	{ 0xC9A7C1C91F3B2451ull, 0xC9A7C1C91F3B2451ull },
	{ 0xD76F46EC60ECB1A9ull, 0xD76F46EC60ECB1A9ull },
	{ 0x1398E0DB23D34965ull, 0x1398E0DB23D34965ull },
	{ 0xA9565A69D299635Bull, 0xA9565A69D299635Bull },
	{ 0xB601F197643AABB9ull, 0xB601F197643AABB9ull },
	{ 0x56CD05214C8464A1ull, 0x56CD05214C8464A1ull },
	{ 0xDA85EA2C7A176F7Full, 0xDA85EA2C7A176F7Full },
	{ 0xD77269126591EDAEull, 0xD77269126591EDAEull },
	{ 0x6504639130E68F4Bull, 0x6504639130E68F4Bull },
	{ 0xB96621EDD5477058ull, 0xB96621EDD5477058ull },
	{ 0x6B93C8C7646AA314ull, 0x6B93C8C7646AA314ull },
	{ 0x18DF766A54CE5718ull, 0x18DF766A54CE5718ull },
	{ 0xAE9587E2E2429C35ull, 0xAE9587E2E2429C35ull },
	{ 0x983E877D7D5EC90Dull, 0x983E877D7D5EC90Dull },
	{ 0x0E7833D7A50B8A5Aull, 0x0E7833D7A50B8A5Aull },
	{ 0xE9D8A9BFF4B31D86ull, 0xE9D8A9BFF4B31D86ull },
	{ 0x86917C15BCCE0266ull, 0x86917C15BCCE0266ull },
	{ 0x631CEA943CC455B5ull, 0x631CEA943CC455B5ull },
	{ 0x181536BAE7CB7F7Aull, 0x181536BAE7CB7F7Aull },
	{ 0x236CE377E73F1D6Full, 0x236CE377E73F1D6Full },
	{ 0x06BCFCEE6E531230ull, 0x06BCFCEE6E531230ull },
	{ 0x7A48E7BBDD8CB527ull, 0x7A48E7BBDD8CB527ull },
	{ 0x738F388EB5111D36ull, 0x738F388EB5111D36ull },
	{ 0xC037139257B691A6ull, 0xC037139257B691A6ull },
	{ 0xF4C194029DE7164Full, 0xF4C194029DE7164Full },
	{ 0x6F4EBBAA39421908ull, 0x6F4EBBAA39421908ull },
	{ 0x2E0D6A3EE78B4351ull, 0x2E0D6A3EE78B4351ull },
	{ 0x3A85DD1E749463BFull, 0x3A85DD1E749463BFull },
	{ 0xE806367F8E307E27ull, 0xE806367F8E307E27ull },
	{ 0xA6508D59671DFA8Dull, 0xA6508D59671DFA8Dull },
	{ 0x28C204952A75F14Eull, 0x28C204952A75F14Eull },
	{ 0xA0726A8C3C8E9A0Cull, 0xA0726A8C3C8E9A0Cull },
	{ 0xCB83E0E651DED9DAull, 0xCB83E0E651DED9DAull },
	{ 0x17A20B7A4D90F028ull, 0x17A20B7A4D90F028ull },
	{ 0x98E84864F7924670ull, 0x98E84864F7924670ull },
	{ 0x943E6F387ECE5CA1ull, 0x943E6F387ECE5CA1ull },
	{ 0x5E839857CBA2D84Bull, 0x5E839857CBA2D84Bull },
	{ 0xCF666D00A33AB10Cull, 0xCF666D00A33AB10Cull },
	{ 0x07927EB79A9E8239ull, 0x07927EB79A9E8239ull },
	{ 0xCBF22598D9E7F2D0ull, 0xCBF22598D9E7F2D0ull },
	{ 0x344C6980AE66489Dull, 0x344C6980AE66489Dull },
	{ 0x6DCE710EEEE2162Bull, 0x6DCE710EEEE2162Bull },
	{ 0xD217F8382DCE03EEull, 0xD217F8382DCE03EEull },
	{ 0x91FA50A7BC12C3ADull, 0x91FA50A7BC12C3ADull },
	{ 0x33E169857CC41BB4ull, 0x33E169857CC41BB4ull },
	{ 0x26F65E2287AD15BAull, 0x26F65E2287AD15BAull },
	{ 0xA9930DDF9C014B90ull, 0xA9930DDF9C014B90ull },
	{ 0xC4563B9064A1570Bull, 0xC4563B9064A1570Bull },
	{ 0x188FB52E89806033ull, 0x188FB52E89806033ull },
	{ 0x4314037F93D8C72Bull, 0x4314037F93D8C72Bull },
	{ 0x933F2080A03DF863ull, 0x933F2080A03DF863ull },
	{ 0x5555B8DAE279557Cull, 0x5555B8DAE279557Cull },
	{ 0xF17C7153F780457Bull, 0xF17C7153F780457Bull },
	{ 0x9E64B79D0AB162B1ull, 0x9E64B79D0AB162B1ull },
	{ 0xE8369933C4E6C9EAull, 0xE8369933C4E6C9EAull },
	{ 0x4B072398138B84D4ull, 0x4B072398138B84D4ull },
	{ 0x21D2CAA9B9821AB1ull, 0x21D2CAA9B9821AB1ull },
	{ 0xA91B5970B20A3FC0ull, 0xA91B5970B20A3FC0ull },
	{ 0xD5C0E88364F39F67ull, 0xD5C0E88364F39F67ull },
	{ 0x9B4D6BC0E2F2BB0Full, 0x9B4D6BC0E2F2BB0Full },
	{ 0x6C83B0DA2ABE0A06ull, 0x6C83B0DA2ABE0A06ull },
	{ 0x542C11ADFEB8DC9Cull, 0x542C11ADFEB8DC9Cull },
	{ 0xA836E7AE13223012ull, 0xA836E7AE13223012ull },
	{ 0xDA0009FA692FB66Dull, 0xDA0009FA692FB66Dull },
	{ 0x754DCEF3455A0B32ull, 0x754DCEF3455A0B32ull },
	{ 0xA491F65FB72D8CE3ull, 0xA491F65FB72D8CE3ull },
	{ 0xF87DF9FFAED5CE39ull, 0xF87DF9FFAED5CE39ull },
	{ 0x27ED960E2DFEFF0Aull, 0x27ED960E2DFEFF0Aull },
	{ 0xC042C3C3CCE1FB8Eull, 0xC042C3C3CCE1FB8Eull },
	{ 0xEB6FCC3F85D3010Full, 0xEB6FCC3F85D3010Full },
	{ 0xA2DA9C8525EF1655ull, 0xA2DA9C8525EF1655ull },
	{ 0xA1413A8225FBBD3Eull, 0xA1413A8225FBBD3Eull },
	{ 0xBF61987AB530B3CDull, 0xBF61987AB530B3CDull },
	{ 0x046E300A13CDCE72ull, 0x046E300A13CDCE72ull },
	{ 0x2B42E6813F7AF89Aull, 0x2B42E6813F7AF89Aull },
	{ 0x5AD5B83EEB50527Aull, 0x5AD5B83EEB50527Aull },
	{ 0xD336AACBCE446C17ull, 0xD336AACBCE446C17ull },
	{ 0x4DEA8AD63C1EA819ull, 0x4DEA8AD63C1EA819ull },
	{ 0x5323ED7810ABB650ull, 0x5323ED7810ABB650ull },
	{ 0x4235FCA1EB123187ull, 0x4235FCA1EB123187ull },
	{ 0x4E648AF6B9856108ull, 0x4E648AF6B9856108ull },
	{ 0xAF9CEE9292F4126Full, 0xAF9CEE9292F4126Full },
	{ 0xC75B24C7E2319932ull, 0xC75B24C7E2319932ull },
	{ 0xF926BE0DE236EF70ull, 0xF926BE0DE236EF70ull },
	{ 0x77AB10AB67B9EDFAull, 0x77AB10AB67B9EDFAull },
	{ 0xC5263518FCF8AF9Full, 0xC5263518FCF8AF9Full },
	{ 0xA86248A54CB70E3Bull, 0xA86248A54CB70E3Bull },
	{ 0x55447A16DC8C3D82ull, 0x55447A16DC8C3D82ull },
	{ 0x89B55BB618FE8D89ull, 0x89B55BB618FE8D89ull },
	{ 0xDE4E001AAF0B4494ull, 0xDE4E001AAF0B4494ull },
	{ 0x8B1D5CE73918A026ull, 0x8B1D5CE73918A026ull },
	{ 0x605176AE37EFF146ull, 0x605176AE37EFF146ull },
	{ 0x3337D40B5EBDDC0Full, 0x3337D40B5EBDDC0Full },
	{ 0xFEA0B9CC2E76B3FAull, 0xFEA0B9CC2E76B3FAull },
	{ 0x7818AD72E01CFA6Eull, 0x7818AD72E01CFA6Eull },
	{ 0xD7F3C1CEA007FD08ull, 0xD7F3C1CEA007FD08ull },
	{ 0x96DE29B419289527ull, 0x96DE29B419289527ull },
	{ 0x5DFD93D14F2A013Bull, 0x5DFD93D14F2A013Bull },
	{ 0xEC06CE07CA6CE8CEull, 0xEC06CE07CA6CE8CEull },
	{ 0x2DD3312812BA7552ull, 0x2DD3312812BA7552ull },
	{ 0x4B495459796D93ADull, 0x4B495459796D93ADull },
	{ 0xE7E4880936705002ull, 0xE7E4880936705002ull },
	{ 0xD7AE9A8BD0CB5641ull, 0xD7AE9A8BD0CB5641ull },
	{ 0x07B762CBDBE58CF1ull, 0x07B762CBDBE58CF1ull },
	{ 0xDB4D93D10031D3BBull, 0xDB4D93D10031D3BBull },
	{ 0xBC24587A58E8E4BAull, 0xBC24587A58E8E4BAull },
	{ 0x1FA937171C0D4F02ull, 0x1FA937171C0D4F02ull },
	{ 0x3F35BC98985796EEull, 0x3F35BC98985796EEull },
	{ 0xC57070287E08702Dull, 0xC57070287E08702Dull },
	{ 0xAE688EFE7B3F218Full, 0xAE688EFE7B3F218Full },
	{ 0x1CA00F4024D1FE8Bull, 0x1CA00F4024D1FE8Bull },
	{ 0xE21F6C2635470714ull, 0xE21F6C2635470714ull },
	{ 0xFC76B932142AA51Eull, 0xFC76B932142AA51Eull },
	{ 0xC6477D4EB1E28AEAull, 0xC6477D4EB1E28AEAull },
	{ 0x9B6AD707B43AB019ull, 0x9B6AD707B43AB019ull },
	{ 0x320F67B1EF4A71F0ull, 0x320F67B1EF4A71F0ull },
	{ 0x672286B032AFCDE7ull, 0x672286B032AFCDE7ull },
	{ 0x3F903D6F623EC378ull, 0x3F903D6F623EC378ull },
	{ 0x5C9D8A44E262CEA6ull, 0x5C9D8A44E262CEA6ull },
	{ 0xA4986CEA5310894Bull, 0xA4986CEA5310894Bull },
	{ 0x600138A19AC565A6ull, 0x600138A19AC565A6ull },
	{ 0x89E8B13C40B38F23ull, 0x89E8B13C40B38F23ull },
	{ 0x94E66CF11857E6ECull, 0x94E66CF11857E6ECull },
	{ 0xE39301BB3D9821B1ull, 0xE39301BB3D9821B1ull },
	{ 0x35D976ED5D270833ull, 0x35D976ED5D270833ull },
	{ 0x490B827451CCD116ull, 0x490B827451CCD116ull },
	{ 0x6CFC0AD40F11A368ull, 0x6CFC0AD40F11A368ull },
	{ 0xCCBB73F9DB282F6Cull, 0xCCBB73F9DB282F6Cull },
	{ 0x2B59316EDA56712Bull, 0x2B59316EDA56712Bull },
	{ 0x54862305EAFA8E3Dull, 0x54862305EAFA8E3Dull },
	{ 0x018B37CFCF40EC19ull, 0x018B37CFCF40EC19ull },
	{ 0xF19FD970A887C388ull, 0xF19FD970A887C388ull },
	{ 0x77239FE1C3537C69ull, 0x77239FE1C3537C69ull },
	{ 0xB836CCFAB048E842ull, 0xB836CCFAB048E842ull },
	{ 0xE3350FCCB114572Aull, 0xE3350FCCB114572Aull },
	{ 0x17FE102B72F61450ull, 0x17FE102B72F61450ull },
	{ 0xC3FF424E18942387ull, 0xC3FF424E18942387ull },
	{ 0x33AA820396D8238Full, 0x33AA820396D8238Full },
	{ 0x96DCF6A55E174626ull, 0x96DCF6A55E174626ull },
	{ 0xB971BE0356E2734Full, 0xB971BE0356E2734Full },
	{ 0xB083E0942B7C84E0ull, 0xB083E0942B7C84E0ull },
	{ 0x3AAB0C24E314A351ull, 0x3AAB0C24E314A351ull },
	{ 0x14F69832CE82C7C3ull, 0x14F69832CE82C7C3ull },
	{ 0x83C250CAC0E4554Full, 0x83C250CAC0E4554Full },
	{ 0x4F688A789BD6D990ull, 0x4F688A789BD6D990ull },
	{ 0xF4CCFD222A6EEBF3ull, 0xF4CCFD222A6EEBF3ull },
	{ 0xCA53E8784B69273Dull, 0xCA53E8784B69273Dull },
	{ 0xD8A0FE796E334CDCull, 0xD8A0FE796E334CDCull },
	{ 0xA76197F05B0B7E72ull, 0xA76197F05B0B7E72ull },
	{ 0x3DCDAEEA50939F1Eull, 0x3DCDAEEA50939F1Eull },
	{ 0x9793230F6D56CF8Dull, 0x9793230F6D56CF8Dull },
	{ 0xCDB6C4D86B1F0D41ull, 0xCDB6C4D86B1F0D41ull },
	{ 0x332EA8ACA9D8EE20ull, 0x332EA8ACA9D8EE20ull },
	{ 0x255143438B63823Full, 0x255143438B63823Full },
	{ 0xD17CCC141D0A4DEFull, 0xD17CCC141D0A4DEFull },
	{ 0x2CB269AE14476B08ull, 0x2CB269AE14476B08ull },
	{ 0xE158A4E718CBC575ull, 0xE158A4E718CBC575ull },
	{ 0x6F30A4375EEB7780ull, 0x6F30A4375EEB7780ull },
	{ 0x40A1AF70936C4C5Eull, 0x40A1AF70936C4C5Eull },
	{ 0xF3567B0A1714597Dull, 0xF3567B0A1714597Dull },
	{ 0x14AFBC4D88B6BE0Dull, 0x14AFBC4D88B6BE0Dull },
	{ 0x92F7837E9FDC834Cull, 0x92F7837E9FDC834Cull },
	{ 0x21D8E0C959079695ull, 0x21D8E0C959079695ull },
	{ 0xD3274ED16FEDEB2Bull, 0xD3274ED16FEDEB2Bull },
	{ 0x23D966AE2A6AB4C2ull, 0x23D966AE2A6AB4C2ull },
	{ 0xBC525B0D7343C84Eull, 0xBC525B0D7343C84Eull },
	{ 0x25ADBC2946C1188Full, 0x25ADBC2946C1188Full },
	{ 0xB1F902AA6B7454DCull, 0xB1F902AA6B7454DCull },
	{ 0xD97E63E37E6EA4AFull, 0xD97E63E37E6EA4AFull },
	{ 0xCB0C6CDF4E028AF2ull, 0xCB0C6CDF4E028AF2ull },
	{ 0xF0DDE54D854FDD87ull, 0xF0DDE54D854FDD87ull },
	{ 0xA97D7CA8A677E89Dull, 0xA97D7CA8A677E89Dull },
	{ 0xF458BC0055B7A9B1ull, 0xF458BC0055B7A9B1ull },
	{ 0xD2BDC282F4862DE1ull, 0xD2BDC282F4862DE1ull },
	{ 0x9C1748518DB51178ull, 0x9C1748518DB51178ull },
	{ 0x07426AB90DB11B37ull, 0x07426AB90DB11B37ull },
	{ 0xA721BEDBD0E0FEE0ull, 0xA721BEDBD0E0FEE0ull },
	{ 0x9B192E067BC03508ull, 0x9B192E067BC03508ull },
	{ 0xB40618FB2833FE9Dull, 0xB40618FB2833FE9Dull },
	{ 0xCD2F426DDE387FB5ull, 0xCD2F426DDE387FB5ull },
	{ 0x9870043D60E3FF19ull, 0x9870043D60E3FF19ull },
	{ 0x1FCA4983485E4391ull, 0x1FCA4983485E4391ull },
	{ 0x3962D2E66D8A11F1ull, 0x3962D2E66D8A11F1ull },
	{ 0xB0B341EAA43EC9E4ull, 0xB0B341EAA43EC9E4ull },
	{ 0xE3156BE8C61027F7ull, 0xE3156BE8C61027F7ull },
	{ 0x1093768A0F021DE8ull, 0x1093768A0F021DE8ull },
	{ 0xA0E03A71C726D5AFull, 0xA0E03A71C726D5AFull },
	{ 0x9BA36EF03C84CCCDull, 0x9BA36EF03C84CCCDull },
	{ 0x959D1E0A11C31253ull, 0x959D1E0A11C31253ull },
	{ 0x512BDF0ED4F9A636ull, 0x512BDF0ED4F9A636ull },
	{ 0x53295B09ECE3F652ull, 0x53295B09ECE3F652ull },
	{ 0xABB89768D4937BBBull, 0xABB89768D4937BBBull },
	{ 0x32C98A21A4607B4Bull, 0x32C98A21A4607B4Bull },
	{ 0xF92D7514EE36E0E8ull, 0xF92D7514EE36E0E8ull },
	{ 0x2CEF88D5E87D3E94ull, 0x2CEF88D5E87D3E94ull },
	{ 0x328D5153E873EABBull, 0x328D5153E873EABBull },
	{ 0x314268902E8A3E38ull, 0x314268902E8A3E38ull },
	{ 0x5442F23AFFC7F128ull, 0x5442F23AFFC7F128ull },
	{ 0x7ED85F8311889AF2ull, 0x7ED85F8311889AF2ull },
	{ 0xB5DBFA5245274D16ull, 0xB5DBFA5245274D16ull },
	{ 0xFBD2B8A2B1585635ull, 0xFBD2B8A2B1585635ull },
	{ 0x6225D886FE8F1AE1ull, 0x6225D886FE8F1AE1ull },
	{ 0x0DABC33111EE15A6ull, 0x0DABC33111EE15A6ull },
	{ 0x6F4CB872075EF6D8ull, 0x6F4CB872075EF6D8ull },
	{ 0xF106C2BE7D10DF1Eull, 0xF106C2BE7D10DF1Eull },
	{ 0x569C71F22F7AC0BEull, 0x569C71F22F7AC0BEull },
	{ 0x91F1DF9DE19FDB2Aull, 0x91F1DF9DE19FDB2Aull },
	{ 0x4C93799A9BF45A4Bull, 0x4C93799A9BF45A4Bull },
	{ 0xE3AA166D3320140Dull, 0xE3AA166D3320140Dull },
	{ 0x1927D03526BD66ABull, 0x1927D03526BD66ABull },
	{ 0x38812891C9D236BBull, 0x38812891C9D236BBull },
	{ 0x6F6DF378BF210BFBull, 0x6F6DF378BF210BFBull },
	{ 0x9A558B88C390515Eull, 0x9A558B88C390515Eull },
	{ 0x45A1CD53C945B0D1ull, 0x45A1CD53C945B0D1ull },
	{ 0x220C59EB633393D2ull, 0x220C59EB633393D2ull },
	{ 0xAD71A40A7E76BEB3ull, 0xAD71A40A7E76BEB3ull },
	{ 0xCFCD42476F82112Bull, 0xCFCD42476F82112Bull },
	{ 0xABAB0AE94C76619Dull, 0xABAB0AE94C76619Dull },
	{ 0xE31077CB96247DACull, 0xE31077CB96247DACull },
	{ 0x1069D14B361746FEull, 0x1069D14B361746FEull },
	{ 0x32BEA227C7E3F04Full, 0x32BEA227C7E3F04Full },
	{ 0x4EF8F334341E5D39ull, 0x4EF8F334341E5D39ull },
	{ 0x0482CDC19BEFA042ull, 0x0482CDC19BEFA042ull },
	{ 0x55E588FF932C96A6ull, 0x55E588FF932C96A6ull },
	{ 0x65BE5B5CAD2AA966ull, 0x65BE5B5CAD2AA966ull },
	{ 0x6723B7A255B0A711ull, 0x6723B7A255B0A711ull },
	{ 0x02DEEFF6FCC1C6D0ull, 0x02DEEFF6FCC1C6D0ull },
	{ 0xEFB5A0E19260C337ull, 0xEFB5A0E19260C337ull },
	{ 0xFF9665995009184Bull, 0xFF9665995009184Bull },
	{ 0xDFF4529CD57C624Eull, 0xDFF4529CD57C624Eull },
	{ 0xCEE4BA3B91E1DD52ull, 0xCEE4BA3B91E1DD52ull },
	{ 0x038AB70C8015FEFCull, 0x038AB70C8015FEFCull },
	{ 0x8AF4009D31683817ull, 0x8AF4009D31683817ull },
	{ 0xD92285942864D2DDull, 0xD92285942864D2DDull },
	{ 0x4D46503183A8EE01ull, 0x4D46503183A8EE01ull },
	{ 0x6AF33168F95E9C80ull, 0x6AF33168F95E9C80ull },
	{ 0x8685B460221F2528ull, 0x8685B460221F2528ull },
	{ 0x433CA5D515AD00C8ull, 0x433CA5D515AD00C8ull },
	{ 0x51E3EE1B12EE5D1Bull, 0x51E3EE1B12EE5D1Bull },
	{ 0xD10436EA38C7B46Cull, 0xD10436EA38C7B46Cull },
	{ 0x3FB8D30AC49E91A4ull, 0x3FB8D30AC49E91A4ull },
	{ 0x0E788E2D5BD68AF2ull, 0x0E788E2D5BD68AF2ull },
	{ 0x3C8BFAB13C642566ull, 0x3C8BFAB13C642566ull },
	{ 0x99D0E6FB7AF95B50ull, 0x99D0E6FB7AF95B50ull },
	{ 0xEC987B4E5271C3DBull, 0xEC987B4E5271C3DBull },
	{ 0x3B557F1F83045664ull, 0x3B557F1F83045664ull },
	{ 0xFA68461CC0E0D2A0ull, 0xFA68461CC0E0D2A0ull },
	{ 0x5DBDC6BD32FEC66Dull, 0x5DBDC6BD32FEC66Dull },
	{ 0x8EAF568F02043277ull, 0x8EAF568F02043277ull },
	{ 0x85B890BFF909D00Eull, 0x85B890BFF909D00Eull },
	{ 0x4850E2B03222FB46ull, 0x4850E2B03222FB46ull },
	{ 0xCA9E426DD6AF32D1ull, 0xCA9E426DD6AF32D1ull },
	{ 0xB4CE8B28ED9B4218ull, 0xB4CE8B28ED9B4218ull },
	{ 0xCD9D039E3BD8069Full, 0xCD9D039E3BD8069Full },
	{ 0x47618AE8322CD49Aull, 0x47618AE8322CD49Aull },
	{ 0xB25EEE94EBEB7E4Aull, 0xB25EEE94EBEB7E4Aull },
	{ 0x51FDE7903EABF99Dull, 0x51FDE7903EABF99Dull },
	{ 0x96DA4000D915A686ull, 0x96DA4000D915A686ull },
	{ 0x7C7FA107725C3398ull, 0x7C7FA107725C3398ull },
	{ 0x0B0B879F08762669ull, 0x0B0B879F08762669ull },
	{ 0xBD11E20D2EB2FE25ull, 0xBD11E20D2EB2FE25ull },
	{ 0x0969D6816E76FDF1ull, 0x0969D6816E76FDF1ull },
	{ 0xD0702F92B4828E82ull, 0xD0702F92B4828E82ull },
	{ 0xEE84F199D18B487Cull, 0xEE84F199D18B487Cull },
	{ 0x31319918078C950Full, 0x31319918078C950Full },
	{ 0xF14D7F68A995FA4Full, 0xF14D7F68A995FA4Full },
	{ 0x905DE8ACCAFCFAD4ull, 0x905DE8ACCAFCFAD4ull },
	{ 0x1AB7394FFDF8A192ull, 0x1AB7394FFDF8A192ull },
	{ 0xCD1CD315234CA3FAull, 0xCD1CD315234CA3FAull },
	{ 0xBE551AEA484A2851ull, 0xBE551AEA484A2851ull },
	{ 0x6D0F26F2968C2DCFull, 0x6D0F26F2968C2DCFull },
	{ 0xF1AF92C552B5907Bull, 0xF1AF92C552B5907Bull },
	{ 0x93D1FE30EB56AFC5ull, 0x93D1FE30EB56AFC5ull },
	{ 0x0484B1A11840B664ull, 0x0484B1A11840B664ull },
	{ 0xFB8BECB709AC7C71ull, 0xFB8BECB709AC7C71ull },
	{ 0xF9225AB2A5BB1144ull, 0xF9225AB2A5BB1144ull },
	{ 0x6C4430FBD9F5DE8Eull, 0x6C4430FBD9F5DE8Eull },
	{ 0x7BDA73CAF7947137ull, 0x7BDA73CAF7947137ull },
	{ 0xB4A3E43F25ADDCFAull, 0xB4A3E43F25ADDCFAull },
	{ 0x5B311521DC9BC1BEull, 0x5B311521DC9BC1BEull },
	{ 0xA16129C67834F5B6ull, 0xA16129C67834F5B6ull },
	{ 0x0024B9C4D5F3721Dull, 0x0024B9C4D5F3721Dull },
	{ 0x75745FFDFC8BE468ull, 0x75745FFDFC8BE468ull },
	{ 0xE5C06EC9ED5BE076ull, 0xE5C06EC9ED5BE076ull },
	{ 0x62174F1D60365ADAull, 0x62174F1D60365ADAull },
	{ 0x11780406AAB2C51Dull, 0x11780406AAB2C51Dull },
	{ 0xFEDE5A30B005C87Aull, 0xFEDE5A30B005C87Aull },
	{ 0x0ACE3BCADD8949BDull, 0x0ACE3BCADD8949BDull },
	{ 0xC15BAEE53B4C135Aull, 0xC15BAEE53B4C135Aull },
	{ 0xD3A4FAFF89EF194Cull, 0xD3A4FAFF89EF194Cull },
	{ 0xF5BC757A71F65E30ull, 0xF5BC757A71F65E30ull },
	{ 0x60277889C3537B21ull, 0x60277889C3537B21ull },
	{ 0xB6100016C40FC4D5ull, 0xB6100016C40FC4D5ull },
	{ 0x7530E99F4D8652C7ull, 0x7530E99F4D8652C7ull },
	{ 0xE60FED7BF129FFE7ull, 0xE60FED7BF129FFE7ull },
	{ 0x9CBA46D3BEF1A766ull, 0x9CBA46D3BEF1A766ull },
	{ 0x0E042F6D9A128B1Eull, 0x0E042F6D9A128B1Eull },
	{ 0x0379838ED3C1DDDBull, 0x0379838ED3C1DDDBull },
	{ 0x3E37F27A0E936A45ull, 0x3E37F27A0E936A45ull },
	{ 0xEDD034A3EFD5658Dull, 0xEDD034A3EFD5658Dull },
	{ 0x3AEDCD40A4575C5Bull, 0x3AEDCD40A4575C5Bull },
	{ 0x25BCCF9D32AA3FF6ull, 0x25BCCF9D32AA3FF6ull },
	{ 0x795D2FC79D879435ull, 0x795D2FC79D879435ull },
	{ 0x921B1A18D8ADBF25ull, 0x921B1A18D8ADBF25ull },
	{ 0xB84BD203895F757Aull, 0xB84BD203895F757Aull },
	{ 0xCF269A7A9BD4483Bull, 0xCF269A7A9BD4483Bull },
	{ 0x97992492A48855A8ull, 0x97992492A48855A8ull },
	{ 0xC69F079113B912EFull, 0xC69F079113B912EFull },
	{ 0xB06BC06C383D3D0Full, 0xB06BC06C383D3D0Full },
	{ 0x470EB8EF1305E68Cull, 0x470EB8EF1305E68Cull },
	{ 0xAD10D763FA76A5D3ull, 0xAD10D763FA76A5D3ull },
	{ 0xAB1370A14CC3EDB7ull, 0xAB1370A14CC3EDB7ull },
	{ 0xCF57B5752B4AB460ull, 0xCF57B5752B4AB460ull },
	{ 0x70CAF04CE31AAD99ull, 0x70CAF04CE31AAD99ull },
	{ 0x19000E28036FD666ull, 0x19000E28036FD666ull },
	{ 0xA21EB927EB85C918ull, 0xA21EB927EB85C918ull },
	{ 0x9F06E3F686C75D14ull, 0x9F06E3F686C75D14ull },
	{ 0xB9159DC5566C6365ull, 0xB9159DC5566C6365ull },
	{ 0x4026AD9FCC238404ull, 0x4026AD9FCC238404ull },
	{ 0xAF66759BF7463FE6ull, 0xAF66759BF7463FE6ull },
	{ 0x666F9E847E350946ull, 0x666F9E847E350946ull },
	{ 0x55980C06E89985B4ull, 0x55980C06E89985B4ull },
	{ 0x76115EA74448D174ull, 0x76115EA74448D174ull },
	{ 0x062DFE74F4AD7AF9ull, 0x062DFE74F4AD7AF9ull },
	{ 0x63AB6D67E84AA8B7ull, 0x63AB6D67E84AA8B7ull },
	{ 0x41F031E91F465246ull, 0x41F031E91F465246ull },
	{ 0x7BFD32839D8C30E1ull, 0x7BFD32839D8C30E1ull },
	{ 0x96FCA672150C2C06ull, 0x96FCA672150C2C06ull },
	{ 0xF6122FC6F3864CF4ull, 0xF6122FC6F3864CF4ull },
	{ 0x697E8432743E4761ull, 0x697E8432743E4761ull },
	{ 0xD0D29B8B82DE2B5Dull, 0xD0D29B8B82DE2B5Dull },
	{ 0x1219E80E6F963200ull, 0x1219E80E6F963200ull },
	{ 0x245B151470E7AC5Aull, 0x245B151470E7AC5Aull },
	{ 0x6E86CF76F7290E9Bull, 0x6E86CF76F7290E9Bull },
	{ 0xBA141401CFF12181ull, 0xBA141401CFF12181ull },
	{ 0x79D1880717B74F02ull, 0x79D1880717B74F02ull },
	{ 0xA4AB5857FED70FBAull, 0xA4AB5857FED70FBAull },
	{ 0xE44D20955B047896ull, 0xE44D20955B047896ull },
	{ 0xD7FF5ADB0A9FEBC1ull, 0xD7FF5ADB0A9FEBC1ull },
	{ 0x2B67D7792836E24Full, 0x2B67D7792836E24Full },
	{ 0xC13CBC5E0C5FDE27ull, 0xC13CBC5E0C5FDE27ull },
	{ 0x31E18C4422BF74B6ull, 0x31E18C4422BF74B6ull },
	{ 0xDEBDB6A68A648051ull, 0xDEBDB6A68A648051ull },
	{ 0xD7E42EEF8B896B69ull, 0xD7E42EEF8B896B69ull },
	{ 0xD81B8BE3D8289EF5ull, 0xD81B8BE3D8289EF5ull },
	{ 0x92AE2FBDA0B2121Cull, 0x92AE2FBDA0B2121Cull },
	{ 0x1651F6BA75CFF7A2ull, 0x1651F6BA75CFF7A2ull },
	{ 0x849EEF6EFC888915ull, 0x849EEF6EFC888915ull },
	{ 0xDB949A0370EAF9B7ull, 0xDB949A0370EAF9B7ull },
	{ 0x15952F8BF79580ADull, 0x15952F8BF79580ADull },
	{ 0xDB3671BFC3828869ull, 0xDB3671BFC3828869ull },
	{ 0x0A9F81E206AAFC54ull, 0x0A9F81E206AAFC54ull },
	{ 0xD1E3419FB56BB6BFull, 0xD1E3419FB56BB6BFull },
	{ 0x1DFEB8D51AFC0F8Cull, 0x1DFEB8D51AFC0F8Cull },
	{ 0x9D7B067E3A638A97ull, 0x9D7B067E3A638A97ull },
	{ 0x172A572503BCA2EAull, 0x172A572503BCA2EAull },
	{ 0x551F65505472BC21ull, 0x551F65505472BC21ull },
	{ 0xE2F70046CC892E9Aull, 0xE2F70046CC892E9Aull },
	{ 0xE1331BE8711975E1ull, 0xE1331BE8711975E1ull },
	{ 0xAB7EE8121E967DDAull, 0xAB7EE8121E967DDAull },
	{ 0x137B702DCC431230ull, 0x137B702DCC431230ull },
	{ 0xEFDC42C29FE96CFCull, 0xEFDC42C29FE96CFCull },
	{ 0x3EB3A0E7CE81E0B9ull, 0x3EB3A0E7CE81E0B9ull },
	{ 0x1CA5B56D5296AB67ull, 0x1CA5B56D5296AB67ull },
	{ 0xCF22551D6C92E0FEull, 0xCF22551D6C92E0FEull },
	{ 0xA3ACC549F70A7A15ull, 0xA3ACC549F70A7A15ull },
	{ 0x62210C4889298452ull, 0x62210C4889298452ull },
	{ 0x13FE48506A186B94ull, 0x13FE48506A186B94ull },
	{ 0x5E658EDD53836EDEull, 0x5E658EDD53836EDEull },
	{ 0xB1B4D4AA05532586ull, 0xB1B4D4AA05532586ull },
	{ 0x0BBD5D05A804361Full, 0x0BBD5D05A804361Full },
	{ 0x8E8D3176FFEC1EA3ull, 0x8E8D3176FFEC1EA3ull },
	{ 0x2B5B20506FD923E4ull, 0x2B5B20506FD923E4ull },
	{ 0xA12CC5C7CD5B5F34ull, 0xA12CC5C7CD5B5F34ull },
	{ 0x8F961A1501A4EFD3ull, 0x8F961A1501A4EFD3ull },
	{ 0xDD9A09AA7E8F4319ull, 0xDD9A09AA7E8F4319ull },
	{ 0xB42B946A80B38763ull, 0xB42B946A80B38763ull },
	{ 0x4D26F56C21A436C2ull, 0x4D26F56C21A436C2ull },
	{ 0xD5E252133D35C761ull, 0xD5E252133D35C761ull },
	{ 0x264D71EE216478B7ull, 0x264D71EE216478B7ull },
	{ 0x0B1FFE1C144540EDull, 0x0B1FFE1C144540EDull },
	{ 0x64DDC88FA54FDA91ull, 0x64DDC88FA54FDA91ull },
	{ 0x5AF09C0FD0AFF4FCull, 0x5AF09C0FD0AFF4FCull },
	{ 0xE1DF132AF2AB7E81ull, 0xE1DF132AF2AB7E81ull },
	{ 0x563FF81E898C17D6ull, 0x563FF81E898C17D6ull },
	{ 0x6B81AC06CAD99AC6ull, 0x6B81AC06CAD99AC6ull },
	{ 0xAED516D90353CF98ull, 0xAED516D90353CF98ull },
	{ 0xDD739E3BCCF93539ull, 0xDD739E3BCCF93539ull },
	{ 0x4FEFA5600F88F3B1ull, 0x4FEFA5600F88F3B1ull },
	{ 0x385714CDD4CDE744ull, 0x385714CDD4CDE744ull },
	{ 0x75C76947EC20CF66ull, 0x75C76947EC20CF66ull },
	{ 0x3A4ACBD95BAF9BE4ull, 0x3A4ACBD95BAF9BE4ull },
	{ 0xC62DB0CFA4B7DCD0ull, 0xC62DB0CFA4B7DCD0ull },
	{ 0x31F72BAE64DF89D9ull, 0x31F72BAE64DF89D9ull },
	{ 0x5988F57E505E7FEEull, 0x5988F57E505E7FEEull },
	{ 0xEF40A1D91BA11DBBull, 0xEF40A1D91BA11DBBull },
	{ 0x3C8D7797297CC714ull, 0x3C8D7797297CC714ull },
	{ 0xE43DC2E5FCECA440ull, 0xE43DC2E5FCECA440ull },
	{ 0xF6CF5F10FF14F372ull, 0xF6CF5F10FF14F372ull },
	{ 0xC9F9A771D7990384ull, 0xC9F9A771D7990384ull },
	{ 0x54AA514F5159C775ull, 0x54AA514F5159C775ull },
	{ 0xCE36ADA1D3D30C25ull, 0xCE36ADA1D3D30C25ull },
	{ 0xD2CAD244686D504Eull, 0xD2CAD244686D504Eull },
	{ 0xCAA82B29527A3846ull, 0xCAA82B29527A3846ull },
	{ 0xD62CF47935B1A573ull, 0xD62CF47935B1A573ull },
	{ 0xFD661B73CDA56B8Eull, 0xFD661B73CDA56B8Eull },
	{ 0x63F5089933C1A79Bull, 0x63F5089933C1A79Bull },
	{ 0x40BAD092B7CFB4A7ull, 0x40BAD092B7CFB4A7ull },
	{ 0x59924E024611C244ull, 0x59924E024611C244ull },
	{ 0x432AA34F5DED0BC2ull, 0x432AA34F5DED0BC2ull },
	{ 0xA4BBA3189938AB2Full, 0xA4BBA3189938AB2Full },
	{ 0xBF95A2C0C3E71290ull, 0xBF95A2C0C3E71290ull },
	{ 0x7B5633DE826020E0ull, 0x7B5633DE826020E0ull },
	{ 0xB50344C58841D648ull, 0xB50344C58841D648ull },
	{ 0x13862B50605DAC20ull, 0x13862B50605DAC20ull },
	{ 0x1EC81561C2288BA6ull, 0x1EC81561C2288BA6ull },
	{ 0xDBD962E47A73617Dull, 0xDBD962E47A73617Dull },
	{ 0xAC656740E1225468ull, 0xAC656740E1225468ull },
	{ 0x32DC12AEE0F89CC7ull, 0x32DC12AEE0F89CC7ull },
	{ 0xF9B303BB8E2EF951ull, 0xF9B303BB8E2EF951ull },
	{ 0x9F8780C94DAAC592ull, 0x9F8780C94DAAC592ull },
	{ 0xC6B2DA218B4BAFA8ull, 0xC6B2DA218B4BAFA8ull },
	{ 0x091AB27F02A60DB9ull, 0x091AB27F02A60DB9ull },
	{ 0xAFCEB10793DFD537ull, 0xAFCEB10793DFD537ull },
	{ 0xEEED1AFCABC62FD8ull, 0xEEED1AFCABC62FD8ull },
	{ 0x656FD4D93B51B1CFull, 0x656FD4D93B51B1CFull },
	{ 0x6E3C080992EA7A89ull, 0x6E3C080992EA7A89ull },
	{ 0xF7100591A2D6F547ull, 0xF7100591A2D6F547ull },
	{ 0x1CED749B4B8E4E22ull, 0x1CED749B4B8E4E22ull },
	{ 0x941EA15FA110E5C7ull, 0x941EA15FA110E5C7ull },
	{ 0xC4A341597C46FC22ull, 0xC4A341597C46FC22ull },
	{ 0xFF1F37C8E70430EAull, 0xFF1F37C8E70430EAull },
	{ 0x650720A3D63B1DEFull, 0x650720A3D63B1DEFull },
	{ 0x463E241F349F18A2ull, 0x463E241F349F18A2ull },
	{ 0xB0E091AB3ED20EE4ull, 0xB0E091AB3ED20EE4ull },
	{ 0x76FEC4EE2E1C8293ull, 0x76FEC4EE2E1C8293ull },
	{ 0xE653CFD8320DAC52ull, 0xE653CFD8320DAC52ull },
	{ 0xF60B6E9BBD41B498ull, 0xF60B6E9BBD41B498ull },
	{ 0x26A2C3051F6C15FBull, 0x26A2C3051F6C15FBull },
	{ 0xDC9B1DA7562480FCull, 0xDC9B1DA7562480FCull },
	{ 0xE689299FD29E6135ull, 0xE689299FD29E6135ull },
	{ 0x589E75D9A5AD2235ull, 0x589E75D9A5AD2235ull },
	{ 0xBB913B30E73FAEB1ull, 0xBB913B30E73FAEB1ull },
	{ 0x1F6EA93126909E44ull, 0x1F6EA93126909E44ull },
	{ 0xDC6B5B3EAB26C0C5ull, 0xDC6B5B3EAB26C0C5ull },
	{ 0xB96E765FC1954064ull, 0xB96E765FC1954064ull },
	{ 0x354CEF251BC1688Bull, 0x354CEF251BC1688Bull },
	{ 0xB4F2766040140DF3ull, 0xB4F2766040140DF3ull },
	{ 0x54DD8F67079C9084ull, 0x54DD8F67079C9084ull },
	{ 0x9EDC4CF32D4B86F2ull, 0x9EDC4CF32D4B86F2ull },
	{ 0xE3FF36FCED8AFD94ull, 0xE3FF36FCED8AFD94ull },
	{ 0xFA1FD7FFB49A6407ull, 0xFA1FD7FFB49A6407ull },
	{ 0x9DC072C7DDF40E03ull, 0x9DC072C7DDF40E03ull },
	{ 0xB7F25FF882A25AFCull, 0xB7F25FF882A25AFCull },
	{ 0xC7A892C859CA7AFCull, 0xC7A892C859CA7AFCull },
	{ 0xAFD3C7E4A592D73Full, 0xAFD3C7E4A592D73Full },
	{ 0x292DD99933AB1610ull, 0x292DD99933AB1610ull },
	{ 0x1791BD8C6B9BC262ull, 0x1791BD8C6B9BC262ull },
	{ 0x0435AB6E6E638DB2ull, 0x0435AB6E6E638DB2ull },
	{ 0x7B14E8EA44C83C1Bull, 0x7B14E8EA44C83C1Bull },
	{ 0x0CEEC0181EA1273Aull, 0x0CEEC0181EA1273Aull },
	{ 0xB4A81C7FA1C16244ull, 0xB4A81C7FA1C16244ull },
	{ 0x61955109494783E4ull, 0x61955109494783E4ull },
	{ 0xE4C9852DD4390959ull, 0xE4C9852DD4390959ull },
	{ 0x82F14AA2C4AF5DD0ull, 0x82F14AA2C4AF5DD0ull },
	{ 0xEA35DAD2EEFC80FFull, 0xEA35DAD2EEFC80FFull },
	{ 0x4BCE7BE217CD4A82ull, 0x4BCE7BE217CD4A82ull },
	{ 0x59ED8F2AA56F5379ull, 0x59ED8F2AA56F5379ull },
	{ 0xE93EC5F45B407861ull, 0xE93EC5F45B407861ull },
	{ 0xF713FF132CAE0333ull, 0xF713FF132CAE0333ull },
	{ 0x52A64C8D18096FBAull, 0x52A64C8D18096FBAull },
	{ 0x2B48A1561A18CFB9ull, 0x2B48A1561A18CFB9ull },
	{ 0xC1B1A60128A8D33Dull, 0xC1B1A60128A8D33Dull },
	{ 0x7E3A379544866F3Eull, 0x7E3A379544866F3Eull },
	{ 0xAA360B4597E6207Cull, 0xAA360B4597E6207Cull },
	{ 0x8DE3FDDBA41ED418ull, 0x8DE3FDDBA41ED418ull },
	{ 0xB24B8792823E6D71ull, 0xB24B8792823E6D71ull },
	{ 0x8694DAB2311DE9C1ull, 0x8694DAB2311DE9C1ull },
	{ 0xAA512889F4BB2A77ull, 0xAA512889F4BB2A77ull },
	{ 0x1AC08F38C57E5656ull, 0x1AC08F38C57E5656ull },
	{ 0x04FB2EA2CDB0E2CEull, 0x04FB2EA2CDB0E2CEull },
	{ 0xFB2AE6C2FB4C389Aull, 0xFB2AE6C2FB4C389Aull },
	{ 0xCEFB3CB79C65FB1Aull, 0xCEFB3CB79C65FB1Aull },
	{ 0x4ECCE7B90F1FEBF1ull, 0x4ECCE7B90F1FEBF1ull },
	{ 0xA79287D9812171E2ull, 0xA79287D9812171E2ull },
	{ 0x2AD4025CBD40C1E7ull, 0x2AD4025CBD40C1E7ull },
	{ 0x381E07E70CE1A013ull, 0x381E07E70CE1A013ull },
	{ 0x8ADB41D223AFC929ull, 0x8ADB41D223AFC929ull },
	{ 0x2C2CA76721FAF87Cull, 0x2C2CA76721FAF87Cull },
	{ 0x488E7BACA0DA8A08ull, 0x488E7BACA0DA8A08ull },
	{ 0x299F72C7D35EC6D1ull, 0x299F72C7D35EC6D1ull },
	{ 0x13E77FFFB4B5C57Cull, 0x13E77FFFB4B5C57Cull },
	{ 0xC893C9A145C9F2ABull, 0xC893C9A145C9F2ABull },
	{ 0xEB354913DF40371Dull, 0xEB354913DF40371Dull },
	{ 0x0B778DDC8E5BBC85ull, 0x0B778DDC8E5BBC85ull },
	{ 0x12ED2C5F836D54A9ull, 0x12ED2C5F836D54A9ull },
	{ 0xD6D7535E92D47F3Eull, 0xD6D7535E92D47F3Eull },
	{ 0x0DC60903C3380E83ull, 0x0DC60903C3380E83ull },
	{ 0x69969116EAFC6F1Cull, 0x69969116EAFC6F1Cull },
	{ 0x94DD794D621D8DB0ull, 0x94DD794D621D8DB0ull },
	{ 0xD4C1D052BEB25B45ull, 0xD4C1D052BEB25B45ull },
	{ 0x8F3701D9A57FF5DAull, 0x8F3701D9A57FF5DAull },
	{ 0xD11F4405A3889497ull, 0xD11F4405A3889497ull },
	{ 0x39A89AD18ED136A4ull, 0x39A89AD18ED136A4ull },
	{ 0xE6DB3E093BC006D4ull, 0xE6DB3E093BC006D4ull },
	{ 0xC7AD8A80B9DF3E44ull, 0xC7AD8A80B9DF3E44ull },
	{ 0x7FB1FD29240C1C17ull, 0x7FB1FD29240C1C17ull },
	{ 0xC4EA161D2F067A66ull, 0xC4EA161D2F067A66ull },
	{ 0xE77766D87987FEF3ull, 0xE77766D87987FEF3ull },
	{ 0xA6B3719003DE6217ull, 0xA6B3719003DE6217ull },
	{ 0x39BA09BA24F3D27Aull, 0x39BA09BA24F3D27Aull },
	{ 0x03251D93D2674210ull, 0x03251D93D2674210ull },
	{ 0x88EDE7E578276556ull, 0x88EDE7E578276556ull },
	{ 0x6FBD098E5E26444Eull, 0x6FBD098E5E26444Eull },
	{ 0x3B6B0A6D9FDB70B8ull, 0x3B6B0A6D9FDB70B8ull },
	{ 0x121AC36FB3AADD91ull, 0x121AC36FB3AADD91ull },
	{ 0x442F3037812655D1ull, 0x442F3037812655D1ull },
	{ 0xE675F2560EA57C88ull, 0xE675F2560EA57C88ull },
	{ 0x4842AD78B93C0D83ull, 0x4842AD78B93C0D83ull },
	{ 0xBF09D63C4C424BA2ull, 0xBF09D63C4C424BA2ull },
	{ 0x91198D0F616C7B39ull, 0x91198D0F616C7B39ull },
	{ 0xF9CED3418B8B7CF2ull, 0xF9CED3418B8B7CF2ull },
	{ 0x4690DEBB65548BDCull, 0x4690DEBB65548BDCull },
	{ 0x2F36B06CA7DD2AC4ull, 0x2F36B06CA7DD2AC4ull },
	{ 0xA3395224322B4DEDull, 0xA3395224322B4DEDull },
	{ 0xB4D14D54621B1F19ull, 0xB4D14D54621B1F19ull },
	{ 0xEB8343149B0D43A4ull, 0xEB8343149B0D43A4ull },
	{ 0x7CECE0233D88EF7Full, 0x7CECE0233D88EF7Full },
	{ 0x9A5A09D296264F4Eull, 0x9A5A09D296264F4Eull },
	{ 0x62F9335ADC5A8D45ull, 0x62F9335ADC5A8D45ull },
	{ 0x395C5BC041CB77BFull, 0x395C5BC041CB77BFull },
	{ 0x3D0C338C11B0C59Full, 0x3D0C338C11B0C59Full },
	{ 0x288AE947DE68FEEBull, 0x288AE947DE68FEEBull },
	{ 0x530C2019C187FFA5ull, 0x530C2019C187FFA5ull },
	{ 0x161F8CB0ABC520ECull, 0x161F8CB0ABC520ECull },
	{ 0xD773BADFC1CEFBD4ull, 0xD773BADFC1CEFBD4ull },
	{ 0xF69E7B17CCAAD201ull, 0xF69E7B17CCAAD201ull },
	{ 0x5ED0781DB825940Aull, 0x5ED0781DB825940Aull },
	{ 0xFCC1740D22FC838Cull, 0xFCC1740D22FC838Cull },
	{ 0xD82AAE85FBC37D8Cull, 0xD82AAE85FBC37D8Cull },
	{ 0x119CF19432AEDE92ull, 0x119CF19432AEDE92ull },
	{ 0x0477C79075470782ull, 0x0477C79075470782ull },
	{ 0xCC5F5849006B86BCull, 0xCC5F5849006B86BCull },
	{ 0x6F166EF17DABDA77ull, 0x6F166EF17DABDA77ull },
	{ 0x6A6A2728F21BD9DAull, 0x6A6A2728F21BD9DAull },
	{ 0xE5B4C2F4BEE9B8DCull, 0xE5B4C2F4BEE9B8DCull },
	{ 0x47A527DFAE9BAF5Dull, 0x47A527DFAE9BAF5Dull },
	{ 0xF020847C14BB3E84ull, 0xF020847C14BB3E84ull },
	{ 0x55FB9E37FD1418DAull, 0x55FB9E37FD1418DAull },
	{ 0xC89146A26ACCA3A7ull, 0xC89146A26ACCA3A7ull },
	{ 0x15B378F0A6FE220Cull, 0x15B378F0A6FE220Cull },
	{ 0xD35C9F629ED27861ull, 0xD35C9F629ED27861ull },
	{ 0x0D4E8115550D91D1ull, 0x0D4E8115550D91D1ull },
	{ 0x970457025EC86732ull, 0x970457025EC86732ull },
	{ 0x41658A3B7303A5A9ull, 0x41658A3B7303A5A9ull },
	{ 0xB48E98410A17F4FEull, 0xB48E98410A17F4FEull },
	{ 0xAD2DA2230C29D93Cull, 0xAD2DA2230C29D93Cull },
	{ 0xDEE8206A90A73F8Dull, 0xDEE8206A90A73F8Dull },
	{ 0xB9B741BBACF49F54ull, 0xB9B741BBACF49F54ull },
	{ 0x53A06F6E538FC10Aull, 0x53A06F6E538FC10Aull },
	{ 0xC38164F9B2112503ull, 0xC38164F9B2112503ull },
	{ 0xEDBD4428FB3834C9ull, 0xEDBD4428FB3834C9ull },
	{ 0x8A7AB30C6E97135Aull, 0x8A7AB30C6E97135Aull },
	{ 0x5CABB5C26F188A76ull, 0x5CABB5C26F188A76ull },
	{ 0x88DEA70798408B5Bull, 0x88DEA70798408B5Bull },
	{ 0x6BE72FDCF19F0371ull, 0x6BE72FDCF19F0371ull },
	{ 0x05C1C62EF0FDC4B5ull, 0x05C1C62EF0FDC4B5ull },
	{ 0x384935B04E1798E6ull, 0x384935B04E1798E6ull },
	{ 0x19BDAE0BA6466A6Eull, 0x19BDAE0BA6466A6Eull },
	{ 0x9BC4DB3EEE369592ull, 0x9BC4DB3EEE369592ull },
	{ 0xC6328483B44D56DDull, 0xC6328483B44D56DDull },
	{ 0x45B134B736D056C9ull, 0x45B134B736D056C9ull },
	{ 0x8E4A6865433C592Full, 0x8E4A6865433C592Full },
	{ 0xD295C708EB590B71ull, 0xD295C708EB590B71ull },
	{ 0x8E0EE0C607F7F0E5ull, 0x8E0EE0C607F7F0E5ull },
	{ 0x6428F78E8BAC7383ull, 0x6428F78E8BAC7383ull },
	{ 0x92D04D56A52B1052ull, 0x92D04D56A52B1052ull },
	{ 0xB9707C4AB41DA30Full, 0xB9707C4AB41DA30Full },
	{ 0x335F560A0A5ADA9Full, 0x335F560A0A5ADA9Full },
	{ 0xC6C3CB797F0EBF36ull, 0xC6C3CB797F0EBF36ull },
	{ 0x67785B90D3426648ull, 0x67785B90D3426648ull },
	{ 0xCDA08BD5E350BDB0ull, 0xCDA08BD5E350BDB0ull },
	{ 0x2060E6E8AA367B1Eull, 0x2060E6E8AA367B1Eull },
	{ 0xF4C104F3148B28C6ull, 0xF4C104F3148B28C6ull },
	{ 0x607719FFA4515346ull, 0x607719FFA4515346ull },
	{ 0xE0448E89E3ABDA57ull, 0xE0448E89E3ABDA57ull },
	{ 0x53431E8BA6265A37ull, 0x53431E8BA6265A37ull },
	{ 0xC95A011A1E5EBF55ull, 0xC95A011A1E5EBF55ull },
	{ 0x65EA9775BF3DEEBDull, 0x65EA9775BF3DEEBDull },
	{ 0xBF1A512CA9D3C3ECull, 0xBF1A512CA9D3C3ECull },
	{ 0x0E1DBA4F8FE89488ull, 0x0E1DBA4F8FE89488ull },
	{ 0xF81535EACDE0A994ull, 0xF81535EACDE0A994ull },
	{ 0x353FB0AF64B6B7FDull, 0x353FB0AF64B6B7FDull },
	{ 0x46F67F972185231Bull, 0x46F67F972185231Bull },
	{ 0x13FBB97A87FA94A9ull, 0x13FBB97A87FA94A9ull },
	{ 0x4764F8BC450D3001ull, 0x4764F8BC450D3001ull },
	{ 0x351C04913A02699Bull, 0x351C04913A02699Bull },
	{ 0x8116E29C11417B48ull, 0x8116E29C11417B48ull },
	{ 0xCB80D04365067CAAull, 0xCB80D04365067CAAull },
	{ 0x2025A4D9782C1876ull, 0x2025A4D9782C1876ull },
	{ 0x6A6919DC7B21E06Dull, 0x6A6919DC7B21E06Dull },
	{ 0x2CEB7DA664854518ull, 0x2CEB7DA664854518ull },
	{ 0xBD354C7357B32E42ull, 0xBD354C7357B32E42ull },
	{ 0x43CB31B59A882ABBull, 0x43CB31B59A882ABBull },
	{ 0xBF93F1FE1F254CCAull, 0xBF93F1FE1F254CCAull },
	{ 0x4DB1BC30976F9B73ull, 0x4DB1BC30976F9B73ull },
	{ 0x757862EA1EE02A6Aull, 0x757862EA1EE02A6Aull },
	{ 0xB7C1FEB1705AD1D6ull, 0xB7C1FEB1705AD1D6ull },
	{ 0xA31B7657ADA180C9ull, 0xA31B7657ADA180C9ull },
	{ 0x2B2A7160B51A35FFull, 0x2B2A7160B51A35FFull },
	{ 0x84C6317A0ABD9356ull, 0x84C6317A0ABD9356ull },
	{ 0xC574D3374D64168Eull, 0xC574D3374D64168Eull },
	{ 0x8746785B3E87EE41ull, 0x8746785B3E87EE41ull },
	{ 0xD08FD8E459F16EA4ull, 0xD08FD8E459F16EA4ull },
	{ 0x7F8DBD23FAE429FFull, 0x7F8DBD23FAE429FFull },
	{ 0xFA52DE7714C2A2EAull, 0xFA52DE7714C2A2EAull },
	{ 0xE2A9B12779FEA6C9ull, 0xE2A9B12779FEA6C9ull },
	{ 0x0D4B95ECB14C400Aull, 0x0D4B95ECB14C400Aull },
	{ 0x7E26F3935F6D4F9Cull, 0x7E26F3935F6D4F9Cull },
	{ 0x27E5B8023B0BBFB2ull, 0x27E5B8023B0BBFB2ull },
	{ 0x2FADC26397385AB8ull, 0x2FADC26397385AB8ull },
	{ 0x74ECB3EA77044315ull, 0x74ECB3EA77044315ull },
	{ 0x1D18A9FA374B06A3ull, 0x1D18A9FA374B06A3ull },
	{ 0x63775F47C3A356ECull, 0x63775F47C3A356ECull },
	{ 0x6CB5355B5E0C5CA5ull, 0x6CB5355B5E0C5CA5ull },
	{ 0xDEDDA629F70DB0EFull, 0xDEDDA629F70DB0EFull },
	{ 0x7CDE9DCD131FF04Cull, 0x7CDE9DCD131FF04Cull },
	{ 0x86883B704213F1DFull, 0x86883B704213F1DFull },
	{ 0x1C327B5B268E9DE7ull, 0x1C327B5B268E9DE7ull },
	{ 0xE67A482F0D0F637Dull, 0xE67A482F0D0F637Dull },
	{ 0x41CF747F12574D0Dull, 0x41CF747F12574D0Dull },
	{ 0x2E951C19DA66B992ull, 0x2E951C19DA66B992ull },
	{ 0xC4BDEDEBD867AF95ull, 0xC4BDEDEBD867AF95ull },
	{ 0xEB35DD3124BAD3E8ull, 0xEB35DD3124BAD3E8ull },
	{ 0x9452BB3DE47D001Eull, 0x9452BB3DE47D001Eull },
	{ 0x3CFD3854403B78CDull, 0x3CFD3854403B78CDull },
	{ 0x708BCE2EEB2CC387ull, 0x708BCE2EEB2CC387ull },
	{ 0x109110B739A41B95ull, 0x109110B739A41B95ull },
	{ 0x2106E76DC99589F5ull, 0x2106E76DC99589F5ull },
	{ 0xD7382FDCB7609816ull, 0xD7382FDCB7609816ull },
	{ 0x2E1245DA6677AB52ull, 0x2E1245DA6677AB52ull },
	{ 0xBE0FF5F3D7D9F4ABull, 0xBE0FF5F3D7D9F4ABull },
	{ 0x9837568591360BCAull, 0x9837568591360BCAull },
	{ 0x66AC09BA136B2134ull, 0x66AC09BA136B2134ull },
	{ 0x0FFF65F4E27ED7CEull, 0x0FFF65F4E27ED7CEull },
	{ 0x9EE008411AFDB813ull, 0x9EE008411AFDB813ull },
	{ 0xAE524E0A9B1C4793ull, 0xAE524E0A9B1C4793ull },
	{ 0x18D33578B84EBDAEull, 0x18D33578B84EBDAEull },
	{ 0xAC7F2811866D5B12ull, 0xAC7F2811866D5B12ull },
	{ 0xA2BE2B5CAE9B5FF2ull, 0xA2BE2B5CAE9B5FF2ull },
	{ 0x9BDEFB2CD7069E88ull, 0x9BDEFB2CD7069E88ull },
	{ 0xDF128A8699F5B6CCull, 0xDF128A8699F5B6CCull },
	{ 0x9E116D6C0A7AE775ull, 0x9E116D6C0A7AE775ull },
	{ 0x7CDCAA43CEC184B0ull, 0x7CDCAA43CEC184B0ull },
	{ 0x1D337342C50C131Full, 0x1D337342C50C131Full },
	{ 0xEEFF482F3B98F614ull, 0xEEFF482F3B98F614ull },
	{ 0x834FFF84B603CC0Eull, 0x834FFF84B603CC0Eull },
	{ 0xA4B9388F47196174ull, 0xA4B9388F47196174ull },
	{ 0x72DAB3C574B44590ull, 0x72DAB3C574B44590ull },
	{ 0xC1C515D7B15805CDull, 0xC1C515D7B15805CDull },
	{ 0xFE748FDC6160FCB4ull, 0xFE748FDC6160FCB4ull },
	{ 0xD5E2F16F733ECDA4ull, 0xD5E2F16F733ECDA4ull },
	{ 0x612AA20C858F7851ull, 0x612AA20C858F7851ull },
	{ 0x151C02DA9ED10E7Full, 0x151C02DA9ED10E7Full },
	{ 0x360EB79073C009FFull, 0x360EB79073C009FFull },
	{ 0x0FA058683F1844E9ull, 0x0FA058683F1844E9ull },
	{ 0xB09A0B8A278E8C1Aull, 0xB09A0B8A278E8C1Aull },
	{ 0xEA3E86A437F8D970ull, 0xEA3E86A437F8D970ull },
	{ 0x5479F816E57B2544ull, 0x5479F816E57B2544ull },
	{ 0x158558CD266EDEA8ull, 0x158558CD266EDEA8ull },
	{ 0xADA6A5871A5B0361ull, 0xADA6A5871A5B0361ull },
	{ 0x06BF808E4072D127ull, 0x06BF808E4072D127ull },
	{ 0x0833BADA3D711F9Bull, 0x0833BADA3D711F9Bull },
	{ 0x6EF9461D34C65608ull, 0x6EF9461D34C65608ull },
	{ 0x28BB0D7EADEEFEDEull, 0x28BB0D7EADEEFEDEull },
	{ 0x73E037FC0BA8FE6Bull, 0x73E037FC0BA8FE6Bull },
	{ 0x83B15135941D6C5Bull, 0x83B15135941D6C5Bull },
	{ 0xE75CDB7F06350472ull, 0xE75CDB7F06350472ull },
	{ 0xF634C89AC1AD3F87ull, 0xF634C89AC1AD3F87ull },
	{ 0x7F565C367DE40B44ull, 0x7F565C367DE40B44ull },
	{ 0x7D55F47090F01A9Cull, 0x7D55F47090F01A9Cull },
	{ 0x0E203C5CC63BE18Bull, 0x0E203C5CC63BE18Bull },
	{ 0x6E9A444FC5792814ull, 0x6E9A444FC5792814ull },
	{ 0x3A5204A80AA340F8ull, 0x3A5204A80AA340F8ull },
	{ 0x2F98C63B6BE5DDB3ull, 0x2F98C63B6BE5DDB3ull },
	{ 0x36C8F420309E893Bull, 0x36C8F420309E893Bull },
	{ 0x6E50D638B90EB9FCull, 0x6E50D638B90EB9FCull },
	{ 0x7A73B0D8B5B4880Eull, 0x7A73B0D8B5B4880Eull },
	{ 0x58EACF6AEC600638ull, 0x58EACF6AEC600638ull },
	{ 0x78B7F66E913B33A9ull, 0x78B7F66E913B33A9ull },
	{ 0xD5D2BB732C9A95F5ull, 0xD5D2BB732C9A95F5ull },
	{ 0xD91247CA128C6BCDull, 0xD91247CA128C6BCDull },
	{ 0xBBBD01F46A7C8602ull, 0xBBBD01F46A7C8602ull },
	{ 0x2F5F9E44C3A2BABCull, 0x2F5F9E44C3A2BABCull },
	{ 0x0985197B3F11FDF6ull, 0x0985197B3F11FDF6ull },
	{ 0x0888986A2B24A9A6ull, 0x0888986A2B24A9A6ull },
	{ 0xB730E6861FED1403ull, 0xB730E6861FED1403ull },
	{ 0x59EBDE98CF96D671ull, 0x59EBDE98CF96D671ull },
	{ 0x2891961154E9AA05ull, 0x2891961154E9AA05ull },
	{ 0xA35612ED68101397ull, 0xA35612ED68101397ull },
	{ 0x82DDA6DB3BA43F8Full, 0x82DDA6DB3BA43F8Full },
	{ 0x1D459281A556FFEEull, 0x1D459281A556FFEEull },
	{ 0x7F1C72299B4EEA1Full, 0x7F1C72299B4EEA1Full },
	{ 0xF77DEF73FCFD97FEull, 0xF77DEF73FCFD97FEull },
	{ 0x599D3E36AFC90128ull, 0x599D3E36AFC90128ull },
	{ 0x30D1DFAC0946FC85ull, 0x30D1DFAC0946FC85ull },
	{ 0xA9F22D6F8A68DD94ull, 0xA9F22D6F8A68DD94ull },
	{ 0x6FAF2DF2EA1530F5ull, 0x6FAF2DF2EA1530F5ull },
	{ 0xBB2D2EC6710D9BBCull, 0xBB2D2EC6710D9BBCull },
	{ 0x594CA97FA41ED8F3ull, 0x594CA97FA41ED8F3ull },
	{ 0x2CF82894378C4997ull, 0x2CF82894378C4997ull },
	{ 0x8AA0F624D5235A67ull, 0x8AA0F624D5235A67ull },
	{ 0x18D84AC13CC87039ull, 0x18D84AC13CC87039ull },
	{ 0xFD8D68425FEA8498ull, 0xFD8D68425FEA8498ull },
	{ 0xEB903696EAF8E003ull, 0xEB903696EAF8E003ull },
	{ 0x87D3CE4685882BF8ull, 0x87D3CE4685882BF8ull },
	{ 0xCA0BF700FDB2AD44ull, 0xCA0BF700FDB2AD44ull },
	{ 0x3F669741A99BF570ull, 0x3F669741A99BF570ull },
	{ 0xA1982B009C21AE70ull, 0xA1982B009C21AE70ull },
	{ 0x815AC474A5E7890Dull, 0x815AC474A5E7890Dull },
	{ 0xC52712192A0C4C12ull, 0xC52712192A0C4C12ull },
	{ 0x3C16EF7AC07600B6ull, 0x3C16EF7AC07600B6ull },
	{ 0x8498FBDDA84341CAull, 0x8498FBDDA84341CAull },
	{ 0x8734BA210C8FA8A3ull, 0x8734BA210C8FA8A3ull },
	{ 0xC546B8B214D95C49ull, 0xC546B8B214D95C49ull },
	{ 0x3BB43F91CAB5BEDFull, 0x3BB43F91CAB5BEDFull },
	{ 0x99CE0980CAA33AD5ull, 0x99CE0980CAA33AD5ull },
	{ 0x0BB18293C826324Cull, 0x0BB18293C826324Cull },
	{ 0x59B00FA3E0FF0DC9ull, 0x59B00FA3E0FF0DC9ull },
	{ 0x1A8BDEBDBF67661Eull, 0x1A8BDEBDBF67661Eull },
	{ 0xBA31BD228D245814ull, 0xBA31BD228D245814ull },
	{ 0x94EF783DB75BEDDCull, 0x94EF783DB75BEDDCull },
	{ 0x659E442280FBAF37ull, 0x659E442280FBAF37ull },
	{ 0x48B0ABB4651E4F64ull, 0x48B0ABB4651E4F64ull },
	{ 0x934B878D4B3E79C9ull, 0x934B878D4B3E79C9ull },
	{ 0x3D297E248964C265ull, 0x3D297E248964C265ull },
	{ 0x20108AC9B79EB0F5ull, 0x20108AC9B79EB0F5ull },
	{ 0xCA790D5652ACF0E5ull, 0xCA790D5652ACF0E5ull },
	{ 0x0353951722AD1246ull, 0x0353951722AD1246ull },
	{ 0x3B72910E5E653767ull, 0x3B72910E5E653767ull },
	{ 0xF72D93C5C8043B12ull, 0xF72D93C5C8043B12ull },
	{ 0x40C01D09EF757BC8ull, 0x40C01D09EF757BC8ull },
	{ 0x0919664844938C44ull, 0x0919664844938C44ull },
	{ 0xE895A1FE86D9288Full, 0xE895A1FE86D9288Full },
	{ 0xCDD8025AF58F48C1ull, 0xCDD8025AF58F48C1ull },
	{ 0xF7D0118A85C54111ull, 0xF7D0118A85C54111ull },
	{ 0xC0367CA0857B18C4ull, 0xC0367CA0857B18C4ull },
	{ 0x41800961ECE9533Cull, 0x41800961ECE9533Cull },
	{ 0x34756A126B8AAC78ull, 0x34756A126B8AAC78ull },
	{ 0x0057D854C231FA79ull, 0x0057D854C231FA79ull },
	{ 0xC015F9A96CEA2781ull, 0xC015F9A96CEA2781ull },
	{ 0xFA8C89E6A07E546Full, 0xFA8C89E6A07E546Full },
	{ 0x4FC64F7492E8113Aull, 0x4FC64F7492E8113Aull },
	{ 0xD966D6291801C00Aull, 0xD966D6291801C00Aull },
	{ 0xFF73736D8C3B10B0ull, 0xFF73736D8C3B10B0ull },
	{ 0x0176116A0E2D9FF4ull, 0x0176116A0E2D9FF4ull },
	{ 0x727D42810C0FB1A3ull, 0x727D42810C0FB1A3ull },
	{ 0xEA951CD61CE31658ull, 0xEA951CD61CE31658ull },
	{ 0x2B2C5DEB2B9E200Aull, 0x2B2C5DEB2B9E200Aull },
	{ 0x77D155CEF32CC8D6ull, 0x77D155CEF32CC8D6ull },
	{ 0xE34A1B27C807EA0Dull, 0xE34A1B27C807EA0Dull },
	{ 0x4FA38655B4C09F80ull, 0x4FA38655B4C09F80ull },
	{ 0x63BA74D6E43DC10Bull, 0x63BA74D6E43DC10Bull },
	{ 0x337AD4E95B8983DFull, 0x337AD4E95B8983DFull },
	{ 0x5CC7F95507AD3614ull, 0x5CC7F95507AD3614ull },
	{ 0x5849B4791579206Eull, 0x5849B4791579206Eull },
	{ 0xBDE1BC079EB67EEEull, 0xBDE1BC079EB67EEEull },
	{ 0xBE04D48AA4E1A31Bull, 0xBE04D48AA4E1A31Bull },
	{ 0x0DCB44BD5B2A3092ull, 0x0DCB44BD5B2A3092ull },
	{ 0x08141114C2577BCBull, 0x08141114C2577BCBull },
	{ 0xC26E7F82938F052Dull, 0xC26E7F82938F052Dull },
	{ 0x5525628D7B1B1B55ull, 0x5525628D7B1B1B55ull },
	{ 0x18EFAD29A3AF95A2ull, 0x18EFAD29A3AF95A2ull },
	{ 0x16E1CE0C910AB504ull, 0x16E1CE0C910AB504ull },
	{ 0x6DD353074BF01F05ull, 0x6DD353074BF01F05ull },
	{ 0xEA845501DB9BFF18ull, 0xEA845501DB9BFF18ull },
	{ 0x94E62AC3122C4020ull, 0x94E62AC3122C4020ull },
	{ 0x7052E57FA65DF043ull, 0x7052E57FA65DF043ull },
	{ 0x30F712BBA05CB970ull, 0x30F712BBA05CB970ull },
	{ 0xA3E949B912C2822Eull, 0xA3E949B912C2822Eull },
	{ 0x0C52FD7F26C52C27ull, 0x0C52FD7F26C52C27ull },
	{ 0x341618D476F070F3ull, 0x341618D476F070F3ull },
	{ 0xF072E237B9A9DE29ull, 0xF072E237B9A9DE29ull },
	{ 0xDAF9A651BD9CF823ull, 0xDAF9A651BD9CF823ull },
	{ 0xFE0BACABB28D7510ull, 0xFE0BACABB28D7510ull },
	{ 0xB5D6F244C72D9940ull, 0xB5D6F244C72D9940ull },
	{ 0x04F02D5210B6B92Bull, 0x04F02D5210B6B92Bull },
	{ 0x1D469903C861D4C1ull, 0x1D469903C861D4C1ull },
	{ 0xDC9508BAD6BE55F9ull, 0xDC9508BAD6BE55F9ull },
	{ 0xFD3BC389ABC4EE13ull, 0xFD3BC389ABC4EE13ull },
	{ 0x3B36A82B6DA496B0ull, 0x3B36A82B6DA496B0ull },
	{ 0x2CBB178C7067AB15ull, 0x2CBB178C7067AB15ull },
	{ 0x8EB1294F036F2ABAull, 0x8EB1294F036F2ABAull },
	{ 0x4603A6A78A221319ull, 0x4603A6A78A221319ull },
	{ 0xD3EA4E7AE9CEE1B5ull, 0xD3EA4E7AE9CEE1B5ull },
	{ 0x7EC9DFBA425D2F68ull, 0x7EC9DFBA425D2F68ull },
	{ 0xEF1D6E46F94D6EF1ull, 0xEF1D6E46F94D6EF1ull },
	{ 0x7FE47D7A16A4CABCull, 0x7FE47D7A16A4CABCull },
	{ 0x03B96419EE0325F3ull, 0x03B96419EE0325F3ull },
	{ 0xBD5B77791A8133C6ull, 0xBD5B77791A8133C6ull },
	{ 0x4C707F89C994BD76ull, 0x4C707F89C994BD76ull },
	{ 0xCE78F1A055D8F928ull, 0xCE78F1A055D8F928ull },
	{ 0x178B8B1AEAF423E7ull, 0x178B8B1AEAF423E7ull },
	{ 0x99DA528BADD05663ull, 0x99DA528BADD05663ull },
	{ 0x42F20299167E0F44ull, 0x42F20299167E0F44ull },
	{ 0x6160317F0B255EDCull, 0x6160317F0B255EDCull },
	{ 0xEEC593FFFB2FA3A1ull, 0xEEC593FFFB2FA3A1ull },
	{ 0x1143F33D1C2E1305ull, 0x1143F33D1C2E1305ull },
	{ 0x815EC9DEBA1FDC97ull, 0x815EC9DEBA1FDC97ull },
	{ 0x13B4098B961DF35Bull, 0x13B4098B961DF35Bull },
	{ 0xA70A893E55526FE5ull, 0xA70A893E55526FE5ull },
	{ 0x00000000008BC9C1ull, 0x00000000008BC9C1ull },
	{ 0x8000000012A4E606ull, 0x8000000012A4E606ull },
	{ 0x800041E7C126162Cull, 0x800041E7C126162Cull },
	{ 0x00000000DAA72F2Eull, 0x00000000DAA72F2Eull },
	{ 0x800000288D0A020Full, 0x800000288D0A020Full },
	{ 0x80007CB93BC06F41ull, 0x80007CB93BC06F41ull },
	{ 0x00000003B87BB5EDull, 0x00000003B87BB5EDull },
	{ 0x00001DCF0BB85178ull, 0x00001DCF0BB85178ull },
	{ 0x80005200295E2F97ull, 0x80005200295E2F97ull },
	{ 0x80000000026ECB08ull, 0x80000000026ECB08ull },
	{ 0x0000000D9B62D250ull, 0x0000000D9B62D250ull },
	{ 0x000182B458B184E4ull, 0x000182B458B184E4ull },
	{ 0x0000000000230548ull, 0x0000000000230548ull },
	{ 0x00000A3DAD02FE57ull, 0x00000A3DAD02FE57ull },
	{ 0x0000000000059D25ull, 0x0000000000059D25ull },
	{ 0x0000189FF915805Bull, 0x0000189FF915805Bull },
	{ 0x000001845D1E4851ull, 0x000001845D1E4851ull },
	{ 0x80000295238520DBull, 0x80000295238520DBull },
	{ 0x80000001E6DE7C55ull, 0x80000001E6DE7C55ull },
	{ 0x800000000620983Eull, 0x800000000620983Eull },
	{ 0x8000000AFACDC7F0ull, 0x8000000AFACDC7F0ull },
	{ 0x00000E51A4AC9CDFull, 0x00000E51A4AC9CDFull },
	{ 0x0000660462CE3BE4ull, 0x0000660462CE3BE4ull },
	{ 0x8000954B988ECC7Bull, 0x8000954B988ECC7Bull },
	{ 0x800004ADD96265FDull, 0x800004ADD96265FDull },
	{ 0x800047EE392A7501ull, 0x800047EE392A7501ull },
	{ 0x800277951AFE4287ull, 0x800277951AFE4287ull },
	{ 0x8000000A4B278EF3ull, 0x8000000A4B278EF3ull },
	{ 0x800001D28FCEAC39ull, 0x800001D28FCEAC39ull },
	{ 0x8000004E3CD969F0ull, 0x8000004E3CD969F0ull },
	{ 0x8000000000B614CEull, 0x8000000000B614CEull },
	{ 0x800000000001808Eull, 0x800000000001808Eull },
	{ 0x0000000D2F6AC6F2ull, 0x0000000D2F6AC6F2ull },
	{ 0x800000006E95846Full, 0x800000006E95846Full },
	{ 0x8000009BF61B7B74ull, 0x8000009BF61B7B74ull },
	{ 0x000000001605698Bull, 0x000000001605698Bull },
	{ 0x0001220E500B6BD8ull, 0x0001220E500B6BD8ull },
	{ 0x0000062A1F6F031Bull, 0x0000062A1F6F031Bull },
	{ 0x8000000015B3534Aull, 0x8000000015B3534Aull },
	{ 0x0000002AA3A43CF1ull, 0x0000002AA3A43CF1ull },
	{ 0x000000EDE0E77DC7ull, 0x000000EDE0E77DC7ull },
	{ 0x00000025636DE7AAull, 0x00000025636DE7AAull },
	{ 0x00000000015C9385ull, 0x00000000015C9385ull },
	{ 0x800000F08E47B543ull, 0x800000F08E47B543ull },
	{ 0x8000000000003D10ull, 0x8000000000003D10ull },
	{ 0x80000029DCBBF336ull, 0x80000029DCBBF336ull },
	{ 0x8000000001DB3FB2ull, 0x8000000001DB3FB2ull },
	{ 0x00000D824578363Dull, 0x00000D824578363Dull },
	{ 0x0000456051B82312ull, 0x0000456051B82312ull },
	{ 0x000000000000B6F8ull, 0x000000000000B6F8ull },
	{ 0x80087CEE4F1C337Bull, 0x80087CEE4F1C337Bull },
	{ 0x0000036BDA86950Bull, 0x0000036BDA86950Bull },
	{ 0x800000026F180EC5ull, 0x800000026F180EC5ull },
	{ 0x0000003DE54D8F7Aull, 0x0000003DE54D8F7Aull },
	{ 0x8000000000002FF8ull, 0x8000000000002FF8ull },
	{ 0x800000000004C019ull, 0x800000000004C019ull },
	{ 0x80000000057BEB3Full, 0x80000000057BEB3Full },
	{ 0x00035CEC4A9F6E5Full, 0x00035CEC4A9F6E5Full },
	{ 0x00006D82C02FC095ull, 0x00006D82C02FC095ull },
	{ 0x80000000000181F4ull, 0x80000000000181F4ull },
	{ 0x80002E5E4F5D5796ull, 0x80002E5E4F5D5796ull },
	{ 0x00000001725685DAull, 0x00000001725685DAull },
	{ 0x8001CA3263D5CC27ull, 0x8001CA3263D5CC27ull },
	{ 0x80000CC577340DACull, 0x80000CC577340DACull },
	{ 0x800000410CEFAB2Dull, 0x800000410CEFAB2Dull },
	{ 0x8000000000000033ull, 0x8000000000000033ull },
	{ 0x800000000C747818ull, 0x800000000C747818ull },
	{ 0x000000131D445E36ull, 0x000000131D445E36ull },
	{ 0x000012872C4C1D9Cull, 0x000012872C4C1D9Cull },
	{ 0x000000000000002Full, 0x000000000000002Full },
	{ 0x0007F73C2AC93428ull, 0x0007F73C2AC93428ull },
	{ 0x800000000667BEE3ull, 0x800000000667BEE3ull },
	{ 0x000000002F16B4D9ull, 0x000000002F16B4D9ull },
	{ 0x8000000007787479ull, 0x8000000007787479ull },
	{ 0x000092F970A2E86Dull, 0x000092F970A2E86Dull },
	{ 0x0000CCBA046F8654ull, 0x0000CCBA046F8654ull },
	{ 0x8000126AB7307FCCull, 0x8000126AB7307FCCull },
	{ 0x8000000000000013ull, 0x8000000000000013ull },
	{ 0x0000000000000050ull, 0x0000000000000050ull },
	{ 0x80000000024DCFC2ull, 0x80000000024DCFC2ull },
	{ 0x0000014F4CF96731ull, 0x0000014F4CF96731ull },
	{ 0x800CFA3FC85897D5ull, 0x800CFA3FC85897D5ull },
	{ 0x000014B865BF25DDull, 0x000014B865BF25DDull },
	{ 0x0000171039D4832Eull, 0x0000171039D4832Eull },
	{ 0x800271D0BEDB004Aull, 0x800271D0BEDB004Aull },
	{ 0x8000000164C1C0C7ull, 0x8000000164C1C0C7ull },
	{ 0x800000F56253AF8Full, 0x800000F56253AF8Full },
	{ 0x0000001E7725893Dull, 0x0000001E7725893Dull },
	{ 0x00000001575E5FD3ull, 0x00000001575E5FD3ull },
	{ 0x00000000000003B0ull, 0x00000000000003B0ull },
	{ 0x800000C090131A17ull, 0x800000C090131A17ull },
	{ 0x8001E0D68E7522AEull, 0x8001E0D68E7522AEull },
	{ 0x0000DE326D6D53FAull, 0x0000DE326D6D53FAull },
	{ 0x8000000000001A9Full, 0x8000000000001A9Full },
	{ 0x00000000002C05EDull, 0x00000000002C05EDull },
	{ 0x00000B0B57EE0B55ull, 0x00000B0B57EE0B55ull },
	{ 0x00000C150B08FB06ull, 0x00000C150B08FB06ull },
	{ 0x80001C669F8B04ACull, 0x80001C669F8B04ACull },
	{ 0x800000000003B053ull, 0x800000000003B053ull },
	{ 0x000000007A774AF4ull, 0x000000007A774AF4ull },
	{ 0x00007BF880127533ull, 0x00007BF880127533ull },
	{ 0x000035C8F105D296ull, 0x000035C8F105D296ull },
	{ 0x00000000DBCB4E29ull, 0x00000000DBCB4E29ull },
	{ 0x00000000000054A2ull, 0x00000000000054A2ull },
	{ 0x00000079006A8F56ull, 0x00000079006A8F56ull },
	{ 0x8000006A94EB41C5ull, 0x8000006A94EB41C5ull },
	{ 0x8000000002F89639ull, 0x8000000002F89639ull },
	{ 0x800000075F163DCAull, 0x800000075F163DCAull },
	{ 0x0000000000057087ull, 0x0000000000057087ull },
	{ 0x000001F5372C2A92ull, 0x000001F5372C2A92ull },
	{ 0x000000F8CCB8CA2Full, 0x000000F8CCB8CA2Full },
	{ 0x8001C79E52060BA9ull, 0x8001C79E52060BA9ull },
	{ 0x8001ADBEBC1B2A85ull, 0x8001ADBEBC1B2A85ull },
	{ 0x00001BD202394514ull, 0x00001BD202394514ull },
	{ 0x8000303C4103E7C4ull, 0x8000303C4103E7C4ull },
	{ 0x80000000002AE877ull, 0x80000000002AE877ull },
	{ 0x0000000000000818ull, 0x0000000000000818ull },
	{ 0x000098166A2AA7C6ull, 0x000098166A2AA7C6ull },
	{ 0x000000000735DF7Aull, 0x000000000735DF7Aull },
	{ 0x0000000001593AA5ull, 0x0000000001593AA5ull },
	{ 0x80000000006F8622ull, 0x80000000006F8622ull },
	{ 0x80000000000150BDull, 0x80000000000150BDull },
	{ 0x8000000000000F52ull, 0x8000000000000F52ull },
	{ 0x00000201FD20223Eull, 0x00000201FD20223Eull },
	{ 0x80000000B92B4BE0ull, 0x80000000B92B4BE0ull },
	{ 0x0000000000015E40ull, 0x0000000000015E40ull },
	{ 0x0000000001DF1E8Dull, 0x0000000001DF1E8Dull },
	{ 0x8000000423381C71ull, 0x8000000423381C71ull },
	{ 0x80000000000EA3F3ull, 0x80000000000EA3F3ull },
	{ 0x00000002AC9BC537ull, 0x00000002AC9BC537ull },
	{ 0x800000000000009Full, 0x800000000000009Full },
	{ 0x80015952A2D6990Bull, 0x80015952A2D6990Bull },
	{ 0x800000BB7E6A331Cull, 0x800000BB7E6A331Cull },
	{ 0x800000000FE164B8ull, 0x800000000FE164B8ull },
	{ 0x80007B271F706357ull, 0x80007B271F706357ull },
	{ 0x000000000191B079ull, 0x000000000191B079ull },
	{ 0x800051A1ED6B8531ull, 0x800051A1ED6B8531ull },
	{ 0x00005C765BDB027Eull, 0x00005C765BDB027Eull },
	{ 0x0000AFCC76785780ull, 0x0000AFCC76785780ull },
	{ 0x000011656C3F1304ull, 0x000011656C3F1304ull },
	{ 0x0000001DFD36F4D8ull, 0x0000001DFD36F4D8ull },
	{ 0x8000017F93CA600Bull, 0x8000017F93CA600Bull },
	{ 0x0000001AA1DE886Full, 0x0000001AA1DE886Full },
	{ 0x8000000000001345ull, 0x8000000000001345ull },
	{ 0x8000002D53AAEA76ull, 0x8000002D53AAEA76ull },
	{ 0x800000000000056Eull, 0x800000000000056Eull },
	{ 0x00000000005F1625ull, 0x00000000005F1625ull },
	{ 0x00005209221FEA61ull, 0x00005209221FEA61ull },
	{ 0x0000000000012CF2ull, 0x0000000000012CF2ull },
	{ 0x00000019CDA344FDull, 0x00000019CDA344FDull },
	{ 0x80000000082935B3ull, 0x80000000082935B3ull },
	{ 0x000000000166ACBDull, 0x000000000166ACBDull },
	{ 0x00002DC5318E10E8ull, 0x00002DC5318E10E8ull },
	{ 0x80000000716CAC8Aull, 0x80000000716CAC8Aull },
	{ 0x0000023838EDB68Aull, 0x0000023838EDB68Aull },
	{ 0x8000001003C77D76ull, 0x8000001003C77D76ull },
	{ 0x0000002372346967ull, 0x0000002372346967ull },
	{ 0x0000000000002889ull, 0x0000000000002889ull },
	{ 0x80000000000BEF01ull, 0x80000000000BEF01ull },
	{ 0x0000000176908142ull, 0x0000000176908142ull },
	{ 0x8000B14093D66B72ull, 0x8000B14093D66B72ull },
	{ 0x000000000FFF2359ull, 0x000000000FFF2359ull },
	{ 0x8000000007244F59ull, 0x8000000007244F59ull },
	{ 0x80000002CA19F07Full, 0x80000002CA19F07Full },
	{ 0x8003882E064E4E56ull, 0x8003882E064E4E56ull },
	{ 0x80000004293CB475ull, 0x80000004293CB475ull },
	{ 0x80000000001CA0F4ull, 0x80000000001CA0F4ull },
	{ 0x8000000033CFE95Eull, 0x8000000033CFE95Eull },
	{ 0x8001A484FD5EA5FFull, 0x8001A484FD5EA5FFull },
	{ 0x800000036E98D532ull, 0x800000036E98D532ull },
	{ 0x8000FECA65C309FDull, 0x8000FECA65C309FDull },
	{ 0x80000000002B3346ull, 0x80000000002B3346ull },
	{ 0x000000001AC546F6ull, 0x000000001AC546F6ull },
	{ 0x00000409D8B058AFull, 0x00000409D8B058AFull },
	{ 0x8000289D10C49298ull, 0x8000289D10C49298ull },
	{ 0x800000001BE120E5ull, 0x800000001BE120E5ull },
	{ 0x0000000000000559ull, 0x0000000000000559ull },
	{ 0x000000001162040Bull, 0x000000001162040Bull },
	{ 0x8000000000006F8Bull, 0x8000000000006F8Bull },
	{ 0x80001A7661210155ull, 0x80001A7661210155ull },
	{ 0x800000023005E08Eull, 0x800000023005E08Eull },
	{ 0x000B38BFBF269960ull, 0x000B38BFBF269960ull },
	{ 0x00000000DFE5C20Full, 0x00000000DFE5C20Full },
	{ 0x8000000000073D0Eull, 0x8000000000073D0Eull },
	{ 0x800001D51B523C4Aull, 0x800001D51B523C4Aull },
	{ 0x000016C57C33EA9Dull, 0x000016C57C33EA9Dull },
	{ 0x800000000224DE88ull, 0x800000000224DE88ull },
	{ 0x80000000006AD9C3ull, 0x80000000006AD9C3ull },
	{ 0x00000001F8146898ull, 0x00000001F8146898ull },
	{ 0x80019AA0765E56FBull, 0x80019AA0765E56FBull },
	{ 0x8000000001535F66ull, 0x8000000001535F66ull },
	{ 0x000001C4AD5E4115ull, 0x000001C4AD5E4115ull },
	{ 0x800001C048CC60C2ull, 0x800001C048CC60C2ull },
	{ 0x0000A7B83A3A74AEull, 0x0000A7B83A3A74AEull },
	{ 0x000000000003814Eull, 0x000000000003814Eull },
	{ 0x800028B17E9A15D5ull, 0x800028B17E9A15D5ull },
	{ 0x00000002A93F7AEEull, 0x00000002A93F7AEEull },
	{ 0x0000000000001A76ull, 0x0000000000001A76ull },
	{ 0x00000013D46C972Eull, 0x00000013D46C972Eull },
	{ 0x000000002E314945ull, 0x000000002E314945ull },
	{ 0x00091432D9FB9A15ull, 0x00091432D9FB9A15ull },
	{ 0x800000000029C7C5ull, 0x800000000029C7C5ull },
	{ 0x8000000000000052ull, 0x8000000000000052ull },
	{ 0x0000001F71F0BCC5ull, 0x0000001F71F0BCC5ull },
	{ 0x800038B50B97D88Eull, 0x800038B50B97D88Eull },
	{ 0x0000000000000024ull, 0x0000000000000024ull },
	{ 0x8000000006CC60CAull, 0x8000000006CC60CAull },
	{ 0x800000002DC3B4ACull, 0x800000002DC3B4ACull },
	{ 0x000000002F27928Eull, 0x000000002F27928Eull },
	{ 0x8004BD5BB5A197B2ull, 0x8004BD5BB5A197B2ull },
	{ 0x800000000547E8E6ull, 0x800000000547E8E6ull },
	{ 0x80000000013B2BF1ull, 0x80000000013B2BF1ull },
	{ 0x800130945C8DCF57ull, 0x800130945C8DCF57ull },
	{ 0x80000002E7CF64D0ull, 0x80000002E7CF64D0ull },
	{ 0x000000C440D83F8Bull, 0x000000C440D83F8Bull },
	{ 0x800000000001EF85ull, 0x800000000001EF85ull },
	{ 0x800000084E2875C6ull, 0x800000084E2875C6ull },
	{ 0x800000000511EEF7ull, 0x800000000511EEF7ull },
	{ 0x8000001B5EC3F08Aull, 0x8000001B5EC3F08Aull },
	{ 0x8000000000000020ull, 0x8000000000000020ull },
	{ 0x8000000000000047ull, 0x8000000000000047ull },
	{ 0x80000000000EDFC5ull, 0x80000000000EDFC5ull },
	{ 0x800D7B11AA89C192ull, 0x800D7B11AA89C192ull },
	{ 0x800007F2E85D8E8Dull, 0x800007F2E85D8E8Dull },
	{ 0x00000000000001D1ull, 0x00000000000001D1ull },
	{ 0x8000000000000331ull, 0x8000000000000331ull },
	{ 0x800000000BD74ED5ull, 0x800000000BD74ED5ull },
	{ 0x0000000000000009ull, 0x0000000000000009ull },
	{ 0x000000000000578Bull, 0x000000000000578Bull },
	{ 0x0000000E1C5B1AC8ull, 0x0000000E1C5B1AC8ull },
	{ 0x000000000000353Cull, 0x000000000000353Cull },
	{ 0x80000002587C237Cull, 0x80000002587C237Cull },
	{ 0x00000047B0F3B805ull, 0x00000047B0F3B805ull },
	{ 0x80000000000B8F33ull, 0x80000000000B8F33ull },
	{ 0x00093B50C9258DDFull, 0x00093B50C9258DDFull },
	{ 0x8000000000015765ull, 0x8000000000015765ull },
	{ 0x80000000076927E1ull, 0x80000000076927E1ull },
	{ 0x800015EFA862A1E1ull, 0x800015EFA862A1E1ull },
	{ 0x00000000003B2FD6ull, 0x00000000003B2FD6ull },
	{ 0x0000002B5ACB1579ull, 0x0000002B5ACB1579ull },
	{ 0x000000123B67EA52ull, 0x000000123B67EA52ull },
	{ 0x0000000000593EE6ull, 0x0000000000593EE6ull },
	{ 0x8000004A938DC891ull, 0x8000004A938DC891ull },
	{ 0x80031C0D3A2144EBull, 0x80031C0D3A2144EBull },
	{ 0x8000000002C4D75Dull, 0x8000000002C4D75Dull },
	{ 0x8000000040B1E8F7ull, 0x8000000040B1E8F7ull },
	{ 0x0000007C48BD5B6Dull, 0x0000007C48BD5B6Dull },
	{ 0x80005802EE68A273ull, 0x80005802EE68A273ull },
	{ 0x00000000004D5549ull, 0x00000000004D5549ull },
	{ 0x80007666158C72E6ull, 0x80007666158C72E6ull },
	{ 0x0000045C52C0A91Bull, 0x0000045C52C0A91Bull },
	{ 0x00001397A48DF9E8ull, 0x00001397A48DF9E8ull },
	{ 0x80002C0C657C47C8ull, 0x80002C0C657C47C8ull },
	{ 0x00046B61A4039B51ull, 0x00046B61A4039B51ull },
	{ 0x000000003CB89656ull, 0x000000003CB89656ull },
	{ 0x000000186DB109BAull, 0x000000186DB109BAull },
	{ 0x000000011761146Cull, 0x000000011761146Cull },
	{ 0x80000095F1B569CCull, 0x80000095F1B569CCull },
	{ 0x00000001A54E5CBAull, 0x00000001A54E5CBAull },
	{ 0x00000008EC73F6A2ull, 0x00000008EC73F6A2ull },
	{ 0x000009EB115C5ECDull, 0x000009EB115C5ECDull },
	{ 0x0000016CD18FAF6Eull, 0x0000016CD18FAF6Eull },
	{ 0x80000E0F696B41EFull, 0x80000E0F696B41EFull },
	{ 0x8001D9DA137CBA55ull, 0x8001D9DA137CBA55ull },
	{ 0x000213D9AD188340ull, 0x000213D9AD188340ull },
	{ 0x800734DE44EAA7E0ull, 0x800734DE44EAA7E0ull },
	{ 0x000000000484D474ull, 0x000000000484D474ull },
	{ 0x800000000001833Cull, 0x800000000001833Cull },
	{ 0x000000000352BA38ull, 0x000000000352BA38ull },
	{ 0x8000051734E94D07ull, 0x8000051734E94D07ull },
	{ 0x000000403652193Full, 0x000000403652193Full },
	{ 0x8000000293470113ull, 0x8000000293470113ull },
	{ 0x80000000733CDBA4ull, 0x80000000733CDBA4ull },
	{ 0x00001FB73B6D3D71ull, 0x00001FB73B6D3D71ull },
	{ 0x000000000000006Eull, 0x000000000000006Eull },
	{ 0x000199FB6567B67Full, 0x000199FB6567B67Full },
	{ 0x80000000CDA8FB5Cull, 0x80000000CDA8FB5Cull },
	{ 0x0000047DED1C8E86ull, 0x0000047DED1C8E86ull },
	{ 0x80000000000D7CF2ull, 0x80000000000D7CF2ull },
	{ 0x8000000AFBA2D308ull, 0x8000000AFBA2D308ull },
	{ 0x000006CEF79B3E8Dull, 0x000006CEF79B3E8Dull },
	{ 0x000001F959D92D02ull, 0x000001F959D92D02ull },
	{ 0x0000000F43976783ull, 0x0000000F43976783ull },
	{ 0x0000000001C3607Eull, 0x0000000001C3607Eull },
	{ 0x000000000330B8F7ull, 0x000000000330B8F7ull },
	{ 0x0000004F21358E16ull, 0x0000004F21358E16ull },
	{ 0x00000011DEBB3948ull, 0x00000011DEBB3948ull },
	{ 0x000045B557BAFA2Aull, 0x000045B557BAFA2Aull },
	{ 0x8000000000000D71ull, 0x8000000000000D71ull },
	{ 0x800015FACA56DEA7ull, 0x800015FACA56DEA7ull },
	{ 0x800000000077A27Full, 0x800000000077A27Full },
	{ 0x8000002CD9DC2D87ull, 0x8000002CD9DC2D87ull },
	{ 0x000002FD27CBF9A2ull, 0x000002FD27CBF9A2ull },
	{ 0x80000002D8897878ull, 0x80000002D8897878ull },
	{ 0x800000011289380Aull, 0x800000011289380Aull },
	{ 0x0000000000520137ull, 0x0000000000520137ull },
	{ 0x8000001011164537ull, 0x8000001011164537ull },
	{ 0x0002842459D4D1D3ull, 0x0002842459D4D1D3ull },
	{ 0x00000001C1CB6B26ull, 0x00000001C1CB6B26ull },
	{ 0x000000000000003Aull, 0x000000000000003Aull },
	{ 0x00000000000005D1ull, 0x00000000000005D1ull },
	{ 0x800007B96B3951DAull, 0x800007B96B3951DAull },
	{ 0x00000000000003D1ull, 0x00000000000003D1ull },
	{ 0x00000B528CB139F0ull, 0x00000B528CB139F0ull },
	{ 0x00000000000182C1ull, 0x00000000000182C1ull },
	{ 0x000000121F1E4C1Full, 0x000000121F1E4C1Full },
	{ 0x8000027A8AE2221Aull, 0x8000027A8AE2221Aull },
	{ 0x800004058AE56161ull, 0x800004058AE56161ull },
	{ 0x80000E0CABAB86DEull, 0x80000E0CABAB86DEull },
	{ 0x00000000000005D0ull, 0x00000000000005D0ull },
	{ 0x000000000138FEB9ull, 0x000000000138FEB9ull },
	{ 0x00000000000011B6ull, 0x00000000000011B6ull },
	{ 0x00001A7523D733EDull, 0x00001A7523D733EDull },
	{ 0x800002A9CE7EB63Aull, 0x800002A9CE7EB63Aull },
	{ 0x8000000057068F1Dull, 0x8000000057068F1Dull },
	{ 0x800000003591CE17ull, 0x800000003591CE17ull },
	{ 0x8000FDE474BD148Cull, 0x8000FDE474BD148Cull },
	{ 0x00002C8ED1EE8EF9ull, 0x00002C8ED1EE8EF9ull },
	{ 0x8000000F3F031417ull, 0x8000000F3F031417ull },
	{ 0x800000000001A67Cull, 0x800000000001A67Cull },
	{ 0x800000000000E9E0ull, 0x800000000000E9E0ull },
	{ 0x00000021A5AB13A4ull, 0x00000021A5AB13A4ull },
	{ 0x80007D815DA05D43ull, 0x80007D815DA05D43ull },
	{ 0x8000D492BDB70528ull, 0x8000D492BDB70528ull },
	{ 0x8000167E82162AE0ull, 0x8000167E82162AE0ull },
	{ 0x800000036B54FA9Full, 0x800000036B54FA9Full },
	{ 0x8006A36B39427E73ull, 0x8006A36B39427E73ull },
	{ 0x0000028FC382EF82ull, 0x0000028FC382EF82ull },
	{ 0x0000026808FA50A1ull, 0x0000026808FA50A1ull },
	{ 0x000000A61DB2C3CBull, 0x000000A61DB2C3CBull },
	{ 0x80015DF7A05E0AFEull, 0x80015DF7A05E0AFEull },
	{ 0x0000000BAF67843Cull, 0x0000000BAF67843Cull },
	{ 0x800000000000D722ull, 0x800000000000D722ull },
	{ 0x8000000048449770ull, 0x8000000048449770ull },
	{ 0x80000E5FBCF71514ull, 0x80000E5FBCF71514ull },
	{ 0x8000001E5C13A272ull, 0x8000001E5C13A272ull },
	{ 0x8000076A14245E9Dull, 0x8000076A14245E9Dull },
	{ 0x0000000019EC7DA1ull, 0x0000000019EC7DA1ull },
	{ 0x80007CCB3ED30F0Eull, 0x80007CCB3ED30F0Eull },
	{ 0x8000A0CB13D09728ull, 0x8000A0CB13D09728ull },
	{ 0x800000002002D24Eull, 0x800000002002D24Eull },
	{ 0x80000003BC3DD00Full, 0x80000003BC3DD00Full },
	{ 0x0000913507A1FA49ull, 0x0000913507A1FA49ull },
	{ 0x800000000001D494ull, 0x800000000001D494ull },
	{ 0x800000000001411Full, 0x800000000001411Full },
	{ 0x0000000001ADC95Dull, 0x0000000001ADC95Dull },
	{ 0x0000000094AEABC1ull, 0x0000000094AEABC1ull },
	{ 0x0013F96EADCC4CE1ull, 0x0013F96EADCC4CE1ull },
	{ 0x0000B1ACFE7274C7ull, 0x0000B1ACFE7274C7ull },
	{ 0x80014797A69BB70Bull, 0x80014797A69BB70Bull },
	{ 0x0000000000749223ull, 0x0000000000749223ull },
	{ 0x0000001570E3ADD5ull, 0x0000001570E3ADD5ull },
	{ 0x00000005DDBA939Bull, 0x00000005DDBA939Bull },
	{ 0x000005785F65F8A1ull, 0x000005785F65F8A1ull },
	{ 0x000032A1A1729F05ull, 0x000032A1A1729F05ull },
	{ 0x8000039F5F134DEBull, 0x8000039F5F134DEBull },
	{ 0x0000000692123A0Eull, 0x0000000692123A0Eull },
	{ 0x0000000000000028ull, 0x0000000000000028ull },
	{ 0x00001C24665F87D8ull, 0x00001C24665F87D8ull },
	{ 0x8000033A44FAC8EDull, 0x8000033A44FAC8EDull },
	{ 0x000000045430520Eull, 0x000000045430520Eull },
	{ 0x0000000004332C93ull, 0x0000000004332C93ull },
	{ 0x00000005654521EDull, 0x00000005654521EDull },
	{ 0x00000000095C6D53ull, 0x00000000095C6D53ull },
	{ 0x800000091B6354ABull, 0x800000091B6354ABull },
	{ 0x800000589CD9BC65ull, 0x800000589CD9BC65ull },
	{ 0x8000000000C39185ull, 0x8000000000C39185ull },
	{ 0x8000011B7273D2B4ull, 0x8000011B7273D2B4ull },
	{ 0x800000036D35DCC4ull, 0x800000036D35DCC4ull },
	{ 0x00022746F457A9FEull, 0x00022746F457A9FEull },
	{ 0x000000000000349Full, 0x000000000000349Full },
	{ 0x80000185D1D359FCull, 0x80000185D1D359FCull },
	{ 0x8000061CAE4254B2ull, 0x8000061CAE4254B2ull },
	{ 0x000003EB09677CD9ull, 0x000003EB09677CD9ull },
	{ 0x0000000001F043F3ull, 0x0000000001F043F3ull },
	{ 0x8000003D6604BF76ull, 0x8000003D6604BF76ull },
	{ 0x800035A88CDA13C5ull, 0x800035A88CDA13C5ull },
	{ 0x0000071ECDC2AB8Dull, 0x0000071ECDC2AB8Dull },
	{ 0x0000000000003EC3ull, 0x0000000000003EC3ull },
	{ 0x80000000000001F4ull, 0x80000000000001F4ull },
	{ 0x800003E2A87A5FD2ull, 0x800003E2A87A5FD2ull },
	{ 0x8000000000000004ull, 0x8000000000000004ull },
	{ 0x00000002FA67E52Bull, 0x00000002FA67E52Bull },
	{ 0x80000000061B17A3ull, 0x80000000061B17A3ull },
	{ 0x00000003EC436E84ull, 0x00000003EC436E84ull },
	{ 0x800000000041DD55ull, 0x800000000041DD55ull },
	{ 0x000009E6D8E978FBull, 0x000009E6D8E978FBull },
	{ 0x0000FE8A93D6029Dull, 0x0000FE8A93D6029Dull },
	{ 0x00004ACF46D359DFull, 0x00004ACF46D359DFull },
	{ 0x80000000000083D2ull, 0x80000000000083D2ull },
	{ 0x800029FE26361835ull, 0x800029FE26361835ull },
	{ 0x00000000003C7330ull, 0x00000000003C7330ull },
	{ 0x0000000051C41DD7ull, 0x0000000051C41DD7ull },
	{ 0x00000000000003F7ull, 0x00000000000003F7ull },
	{ 0x00000014CFEE9D92ull, 0x00000014CFEE9D92ull },
	{ 0x800000000B6FAD2Bull, 0x800000000B6FAD2Bull },
	{ 0x0000000C0CF617D7ull, 0x0000000C0CF617D7ull },
	{ 0x0000000003CC2746ull, 0x0000000003CC2746ull },
	{ 0x8000003415A59557ull, 0x8000003415A59557ull },
	{ 0x80000000002DD183ull, 0x80000000002DD183ull },
	{ 0x00000000CA0F704Aull, 0x00000000CA0F704Aull },
	{ 0x000004CB2A1269DEull, 0x000004CB2A1269DEull },
	{ 0x000000000005EC94ull, 0x000000000005EC94ull },
	{ 0x00000000000EF346ull, 0x00000000000EF346ull },
	{ 0x00000000006B2718ull, 0x00000000006B2718ull },
	{ 0x000000003E66942Full, 0x000000003E66942Full },
	{ 0x000003B2C4368B15ull, 0x000003B2C4368B15ull },
	{ 0x80001DC516914785ull, 0x80001DC516914785ull },
	{ 0x800F82A5358BFEE4ull, 0x800F82A5358BFEE4ull },
	{ 0x800000080E4A6552ull, 0x800000080E4A6552ull },
	{ 0x800000009F9BDF7Cull, 0x800000009F9BDF7Cull },
	{ 0x00000000001CF48Aull, 0x00000000001CF48Aull },
	{ 0x0000023DDE7DCD3Aull, 0x0000023DDE7DCD3Aull },
	{ 0x80002E92131C2A73ull, 0x80002E92131C2A73ull },
	{ 0x800000CD12338004ull, 0x800000CD12338004ull },
	{ 0x80000004EFBF901Full, 0x80000004EFBF901Full },
	{ 0x800007128678D1A5ull, 0x800007128678D1A5ull },
	{ 0x0000000000000438ull, 0x0000000000000438ull },
	{ 0x80000A04AB5D6B4Aull, 0x80000A04AB5D6B4Aull },
	{ 0x00000000004A5FAAull, 0x00000000004A5FAAull },
	{ 0x8000072E9E9656E1ull, 0x8000072E9E9656E1ull },
	{ 0x000001B6764E4365ull, 0x000001B6764E4365ull },
	{ 0x800048A3B246EE84ull, 0x800048A3B246EE84ull },
	{ 0x000000005739AD2Dull, 0x000000005739AD2Dull },
	{ 0x8000302877ACD45Dull, 0x8000302877ACD45Dull },
	{ 0x80000002462BD7C7ull, 0x80000002462BD7C7ull },
	{ 0x000430096881A6A4ull, 0x000430096881A6A4ull },
	{ 0x8000000CE54A5A4Cull, 0x8000000CE54A5A4Cull },
	{ 0x80000000CC8F7107ull, 0x80000000CC8F7107ull },
	{ 0x000001D37911108Full, 0x000001D37911108Full },
	{ 0x00000F509787C7F3ull, 0x00000F509787C7F3ull },
	{ 0x00007AD01FBF4EC1ull, 0x00007AD01FBF4EC1ull },
	{ 0x000000C5CF77A963ull, 0x000000C5CF77A963ull },
	{ 0x8000000000002B65ull, 0x8000000000002B65ull },
	{ 0x0001152ED5FB1228ull, 0x0001152ED5FB1228ull },
	{ 0x80017F87A7A99294ull, 0x80017F87A7A99294ull },
	{ 0x80000000000002D8ull, 0x80000000000002D8ull },
	{ 0x000000005FE29F79ull, 0x000000005FE29F79ull },
	{ 0x8000000190BF2E54ull, 0x8000000190BF2E54ull },
	{ 0x00018E94FD13356Cull, 0x00018E94FD13356Cull },
	{ 0x000000000D868A8Aull, 0x000000000D868A8Aull },
	{ 0x0000000002AB98B4ull, 0x0000000002AB98B4ull },
	{ 0x0005624C1CAAD803ull, 0x0005624C1CAAD803ull },
	{ 0x8000306529392B19ull, 0x8000306529392B19ull },
	{ 0x00000004512E69F3ull, 0x00000004512E69F3ull },
	{ 0x000000000000044Dull, 0x000000000000044Dull },
	{ 0x800000AE99794F49ull, 0x800000AE99794F49ull },
	{ 0x00000000000001A1ull, 0x00000000000001A1ull },
	{ 0x0000F5BEEC5137E0ull, 0x0000F5BEEC5137E0ull },
	{ 0x800001E489725387ull, 0x800001E489725387ull },
	{ 0x8000CA57509632FDull, 0x8000CA57509632FDull },
	{ 0x000000000A0B5F13ull, 0x000000000A0B5F13ull },
	{ 0x00000CFDD165C764ull, 0x00000CFDD165C764ull },
	{ 0x00058978FD4B78B0ull, 0x00058978FD4B78B0ull },
	{ 0x0000000028F49F9Bull, 0x0000000028F49F9Bull },
	{ 0x00062FA9CF9EC3E7ull, 0x00062FA9CF9EC3E7ull },
	{ 0x80000021BA6B2B28ull, 0x80000021BA6B2B28ull },
	{ 0x0000000408392D6Full, 0x0000000408392D6Full },
	{ 0x8003A0932DA715C8ull, 0x8003A0932DA715C8ull },
	{ 0x80000BF50A066FA9ull, 0x80000BF50A066FA9ull },
	{ 0x0000000078D4EBD1ull, 0x0000000078D4EBD1ull },
	{ 0x800016077CB0238Cull, 0x800016077CB0238Cull },
	{ 0x00009117181441CAull, 0x00009117181441CAull },
	{ 0x0000006C7407DEEDull, 0x0000006C7407DEEDull },
	{ 0x00000044C577B28Cull, 0x00000044C577B28Cull },
	{ 0x80000001E005038Cull, 0x80000001E005038Cull },
	{ 0x8000000003F6B7A6ull, 0x8000000003F6B7A6ull },
	{ 0x80000042E5210901ull, 0x80000042E5210901ull },
	{ 0x8000000000000CD0ull, 0x8000000000000CD0ull },
	{ 0x00052E654A1AA7F6ull, 0x00052E654A1AA7F6ull },
	{ 0x800034DC06124D6Eull, 0x800034DC06124D6Eull },
	{ 0x8000000000079753ull, 0x8000000000079753ull },
	{ 0x800001FE109DFC5Dull, 0x800001FE109DFC5Dull },
	{ 0x0000C0EBBF245677ull, 0x0000C0EBBF245677ull },
	{ 0x8000028B1FE8B4F2ull, 0x8000028B1FE8B4F2ull },
	{ 0x8000000028E29F4Eull, 0x8000000028E29F4Eull },
	{ 0x8000000000000012ull, 0x8000000000000012ull },
	{ 0x000000000000138Full, 0x000000000000138Full },
	{ 0x800000000CD0FFE5ull, 0x800000000CD0FFE5ull },
	{ 0x000000A559DEE409ull, 0x000000A559DEE409ull },
	{ 0x800052026B501A96ull, 0x800052026B501A96ull },
	{ 0x000212E11EFD09FBull, 0x000212E11EFD09FBull },
	{ 0x00000000000000D4ull, 0x00000000000000D4ull },
	{ 0x0007F8140C50BDC2ull, 0x0007F8140C50BDC2ull },
	{ 0x800000002C7686F9ull, 0x800000002C7686F9ull },
	{ 0x00000A40DB211064ull, 0x00000A40DB211064ull },
	{ 0x800000000000CC7Full, 0x800000000000CC7Full },
	{ 0x8000000000003EB7ull, 0x8000000000003EB7ull },
	{ 0x8000000000003926ull, 0x8000000000003926ull },
	{ 0x80000000380E27A5ull, 0x80000000380E27A5ull },
	{ 0x80000000005F00E8ull, 0x80000000005F00E8ull },
	{ 0x0000122462B2A248ull, 0x0000122462B2A248ull },
	{ 0x80008396357DC049ull, 0x80008396357DC049ull },
	{ 0x00000EA1DE84AEA4ull, 0x00000EA1DE84AEA4ull },
	{ 0x000000000B8E7461ull, 0x000000000B8E7461ull },
	{ 0x00010F970D189A3Dull, 0x00010F970D189A3Dull },
	{ 0x80001DB73421EFCEull, 0x80001DB73421EFCEull },
	{ 0x80000000004E50D6ull, 0x80000000004E50D6ull },
	{ 0x00000051118B667Bull, 0x00000051118B667Bull },
	{ 0x000000236D395BE2ull, 0x000000236D395BE2ull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull },
	{ 0xFFED115267D418BAull, 0xFFED115267D418BAull }
};

