#include <WGFTestCore.h>

#include "Precision.h"
#include "Arithmetic.h"
#include "Input.h"
#include "Mov.h"
#include "Swap.h"
#include "Convert.h"
#include "Resource.h"

CPrecisionTestApp g_TestApp;

//---------------------------------------------------------------

/******************************
* CMapHelper methods
******************************/

CMapHelper::CMapHelper( ID3D11DeviceContext* pContext ) 
	: m_pContext(pContext), m_pResource(NULL), m_Subresource(0) 
{
	if( pContext )
	{
		pContext->AddRef();
	}
}

HRESULT CMapHelper::Map( ID3D11Resource* pResource, UINT Subresource, D3D11_MAP MapType, UINT MapFlags, D3D11_MAPPED_SUBRESOURCE* pMappedResource )
{
	// Perform the map if we are not currently holding onto a map

	if( m_pContext == NULL || m_pResource )
	{
		return E_FAIL;
	}
	HRESULT hr = m_pContext->Map( pResource, Subresource, MapType, MapFlags, pMappedResource );
	if( SUCCEEDED( hr ) )
	{
		m_pResource = pResource;
		m_Subresource = Subresource;
	}
	return hr;
}

void CMapHelper::Unmap()
{
	if( m_pContext && m_pResource )
	{
		m_pContext->Unmap( m_pResource, m_Subresource );
		m_pResource = NULL;
		m_Subresource = 0;
	}
}

CMapHelper::~CMapHelper()
{
	Unmap();
	SAFE_RELEASE( m_pContext );
}


//---------------------------------------------------------------

/******************************
* CMinPrecValue methods
******************************/

const UINT g_cFracBits16 = 10;									// Number of fraction bits
const UINT g_cFracBits32 = 23;									// Number of fraction bits
const UINT g_cExpBits16 = 5;									// Number of exponent bits
const UINT g_cExpBits32 = 8;									// Number of exponent bits
const UINT g_cSignBit16 = g_cFracBits16 + g_cExpBits16;			// Index of the sign bit
const UINT g_cSignBit32 = g_cFracBits32 + g_cExpBits32;			// Index of the sign bit
const UINT g_FracMask16 = (1 << g_cFracBits16) - 1;				// Fraction mask
const UINT g_FracMask32 = (1 << g_cFracBits32) - 1;				// Fraction mask
const INT32 g_cExpBias16 = (1 << (g_cExpBits16 - 1)) - 1;		// Exponent bias
const INT32 g_cExpBias32 = (1 << (g_cExpBits32 - 1)) - 1;		// Exponent bias
const UINT g_cFracBitsDiff16 = g_cFracBits32 - g_cFracBits16;
const UINT g_cLastFracBit16 = 1 << g_cFracBitsDiff16;
const UINT g_cMissingFracBits16 = g_cLastFracBit16 - 1;
const UINT g_SignMask32 = 1 << g_cSignBit32;					// Sign mask
const UINT g_ExpMask32 = ( g_SignMask32 - 1 ) & ~g_FracMask32;	// Exponent mask

// These are used as a sanity check to ensure the values defined above are correct
C_ASSERT( g_FracMask32 == 0x7fffff );
C_ASSERT( g_FracMask16 == 0x3ff );
C_ASSERT( g_cSignBit32 == 31 );
C_ASSERT( g_cExpBias32 == 127 );
C_ASSERT( g_cFracBitsDiff16 == 13 );
C_ASSERT( g_cFracBits32 == 23 );
C_ASSERT( g_cLastFracBit16 == 0x00002000 );
C_ASSERT( g_cMissingFracBits16 == 0x1fff );
C_ASSERT( ~g_cMissingFracBits16 == 0xffffe000 );
C_ASSERT( g_ExpMask32 == 0x7f800000 );
C_ASSERT( g_SignMask32 == 0x80000000 );

#define UINT_F16( sign, exp, mantissa ) ( (((sign) & 0x1) << g_cSignBit32) | (((exp)+g_cExpBias32) << g_cFracBits32) | ((mantissa & g_FracMask16) << g_cFracBitsDiff16) );
#define UINT_F32( sign, exp, mantissa ) ( (((sign) & 0x1) << g_cSignBit32) | (((exp)+g_cExpBias32) << g_cFracBits32) | (mantissa & g_FracMask32) );

// Special values
const UINT g_uMaxFloat16 = UINT_F16(0,15,0x3ff);
const float g_fMaxFloat16 = *(float*)(&g_uMaxFloat16);
const float g_fMinFloat16 = -g_fMaxFloat16;
const UINT g_uSmallFloat16 = UINT_F16(0,-14,0);
const float g_fSmallFloat16 = *(float*)(&g_uSmallFloat16);
const UINT g_uDenormFloat16 = UINT_F16(0,-15,0x100);   // 0x100 was random - anything between 0x1 and 0x3ff would work
const float g_fDenormFloat16 = *(float*)(&g_uDenormFloat16);
const UINT g_uINFp32 = UINT_F32(0,128,0);
const float g_fINFp32 = *(float*)(&g_uINFp32);
const float g_fINFn32 = -g_fINFp32;
const UINT g_uQNAN = UINT_F32(0,128,0x30000);
const float g_fQNAN = *(float*)(&g_uQNAN);
const float g_fMinFloat10 = -2.0f;
const float g_fMaxFloat10_1 = 2.0f;					 // Max 10-bit float if the fractional part is divided by 255
const float g_fMaxFloat10_2 = 1.0f + 255.0f/256.0f;  // Max 10-bit float if the fractional part is divided by 256
const float g_fMinFloat8 = 0.0f;
const float g_fMaxFloat8 = 1.0f;

bool IsNAN( float value )
{
	// A value is NAN if the exponent bits are all set and the fraction is non-zero
	// One could also use "value != value" but some compilers (or compiler options) are not IEEE compliant
	UINT uVal = *reinterpret_cast<UINT*>( &value );
	return ( uVal & g_ExpMask32 ) == g_ExpMask32 && ( uVal & g_FracMask32 ) > 0;
}

bool IsINFp( float value )
{
	// A value is +INF if the exponent bits are all set, the fraction is zero, and the sign bit is zero
	UINT uVal = *reinterpret_cast<UINT*>( &value );
	return uVal == g_fINFp32;
}

bool IsINFn( float value )
{
	// A value is -INF if the exponent bits are all set, the fraction is zero, and the sign bit is one
	UINT uVal = *reinterpret_cast<UINT*>( &value );
	return uVal == g_fINFn32;
}

template< class T >
void ConvertToPrecision( T value, EPrecisionLevel level, vector<T>& newValues, D3D_FEATURE_LEVEL fl, bool bPerformingClamp = false );

// Convert value from a 32-bit value to a set of values at the associated precision level
template<>
void ConvertToPrecision<float>( float value, EPrecisionLevel level, vector<float>& newValues, D3D_FEATURE_LEVEL fl, bool bPerformingClamp )
{
	switch( level )
	{
	case EPL_32:
		newValues.push_back( value );
		return;
	case EPL_16:
		if( value > g_fMaxFloat16 )
		{
			// Clamp max
			newValues.push_back( g_fMaxFloat16 );
			newValues.push_back( g_fINFp32 );
			if( fl < D3D_FEATURE_LEVEL_10_0 )
			{
				// INF float16 cases are undefined on FL9.x.
				newValues.push_back( g_fQNAN );
			}
		}
		else if( value < g_fMinFloat16 )
		{
			// Clamp min
			newValues.push_back( g_fMinFloat16 );
			newValues.push_back( g_fINFn32 );
			if( fl < D3D_FEATURE_LEVEL_10_0 )
			{
				// -INF float16 cases are undefined on FL9.x.
				newValues.push_back( g_fQNAN );
			}
		}
		else if( !bPerformingClamp )
		{
			// Within Float16 range - remove precision
			// This is not needed when performing a clamp only

			// Drop final 13 bits
			UINT truncatedValue = *(UINT*)&value & ~g_cMissingFracBits16;
			float fTruncated = *(float*)&truncatedValue;
			newValues.push_back( fTruncated );

#if 0 // Round-to-nearest-even was removed from low precision spec.  It may be added later if IHVs do its
			// If we dropped some bits but the final bit is 1, let's round to nearest even.
			if( ( truncatedValue & g_cLastFracBit16 ) && ( *(UINT*)&value & g_cMissingFracBits16 ) )
			{
				UINT smallerTruncatedValue = truncatedValue & ~g_cLastFracBit16;
				float fSmallerTruncated = *(float*)&smallerTruncatedValue; // artificially rounded too far towards zero
				float diff = fTruncated - fSmallerTruncated;
				float fNearestEven = fTruncated + diff; // rounded away from zero
				newValues.push_back( fNearestEven );
			}
#endif

			// flush denorm to 0
			if( abs(value) < g_fSmallFloat16 )
			{
				newValues.push_back( 0 );
			}
		}
		return;
	case EPL_10:
		if( value > g_fMaxFloat10_2 )
		{
			// Clamp max
			newValues.push_back( g_fMaxFloat10_1 );
			newValues.push_back( g_fMaxFloat10_2 );

			// Out-of-range values are undefined
			newValues.push_back( g_fQNAN );
		}
		else if( value < g_fMinFloat10 )
		{
			// Clamp min
			newValues.push_back( g_fMinFloat10 );

			// Out-of-range values are undefined
			newValues.push_back( g_fQNAN );
		}
		else if( !bPerformingClamp )
		{
			// Within Float10 range - remove precision
			// This is not needed when performing a clamp only

			// Remove fractional bits
			float exp = floor( value + g_fMaxFloat10_1 ) - g_fMaxFloat10_1;
			float fractional = value - exp;

			// Add the value back, but truncate the fractional bits to either 1/255 or 1/256 quantum
			float quantized = fractional * 255;
			newValues.push_back( exp + floor(quantized+0.5f) / 255 );	// round to nearest 1/255
			newValues.push_back( exp + floor(quantized) / 255 );		// truncate to 1/255
			if( (UINT)floor(quantized) % 2 == 1 )
			{
				newValues.push_back( exp + ceil(quantized) / 255 );		// round to nearest even 1/255
			}
			quantized = fractional * 256;
			newValues.push_back( exp + floor(quantized+0.5f) / 256 );	// round to nearest 1/256
			newValues.push_back( exp + floor(quantized) / 256 );		// truncate to 1/256
			if( (UINT)floor(quantized) % 2 == 1 )
			{
				newValues.push_back( exp + ceil(quantized) / 256 );		// round to nearest even 1/256
			}
		}
		return;
	case EPL_8:
		if( value > g_fMaxFloat8 )
		{
			// Clamp max
			newValues.push_back( g_fMaxFloat8 );

			// Out-of-range values are undefined
			newValues.push_back( g_fQNAN );
		}
		else if( value < g_fMinFloat8 )
		{
			// Clamp min
			newValues.push_back( g_fMinFloat8 );

			// Out-of-range values are undefined
			newValues.push_back( g_fQNAN );
		}
		else if( !bPerformingClamp )
		{
			// This is not needed when performing a clamp only

			// Round to nearest or truncate to 1/255 or quantum
			float quantized = value * 255;
			newValues.push_back( floor(quantized+0.5f) / 255 );  // round to nearest 1/255
			newValues.push_back( floor(quantized) / 255 );  // truncate to 1/255
		}
		return;
	}
}

template< class T > T MaxValue();
template< class T > T MinValue();
template<> UINT MaxValue<UINT>() { return USHRT_MAX; }
template<> UINT MinValue<UINT>() { return 0; }
template<> int MaxValue<int>() { return SHRT_MAX; }
template<> int MinValue<int>() { return SHRT_MIN; }

template<>
void ConvertToPrecision<double>( double value, EPrecisionLevel level, vector<double>& newValues, D3D_FEATURE_LEVEL fl, bool bPerformingClamp )
{
	// Doubles don't support min precision, so keep the value.
	newValues.push_back( value );
}

template< class T >
void ConvertToPrecision<T>( T value, EPrecisionLevel level, vector<T>& newValues, D3D_FEATURE_LEVEL fl, bool bPerformingClamp )
{
	switch( level )
	{
	case EPL_32:
		newValues.push_back( value );
		return;
	case EPL_16:
		if( value > MaxValue<T>() )
		{
			newValues.push_back( MaxValue<T>() );					// clamp
			newValues.push_back( ( value & 0xffff ) << 16 >> 16 );	// wrap and sign extend
		}
		else if( value <= MinValue<T>() )
		{
			newValues.push_back( MinValue<T>() );					// clamp
			newValues.push_back( ( value & 0xffff ) << 16 >> 16 );	// wrap and sign extend
		}
		else
		{
			// Within range
			newValues.push_back( value );
		}
		return;
	}
}

template<>
EPrecisionLevel CMinPrecValue<float>::GetMinPrecisionLevel()
{
	return EPL_8;
}
template<>
EPrecisionLevel CMinPrecValue<UINT>::GetMinPrecisionLevel()
{
	return EPL_16;
}
template<>
EPrecisionLevel CMinPrecValue<int>::GetMinPrecisionLevel()
{
	return EPL_16;
}
template<>
EPrecisionLevel CMinPrecValue<double>::GetMinPrecisionLevel()
{
	return EPL_32;
}

template< class T >
EPrecisionLevel CMinPrecValue<T>::ConvertToPrecisionLevel( D3D_MIN_PRECISION level )
{
	switch( level )
	{
	case D3D_MIN_PRECISION_DEFAULT: return EPL_32;
	case D3D_MIN_PRECISION_ANY_16: return EPL_16;
	case D3D_MIN_PRECISION_ANY_10: return EPL_10;
	}
	assert( false );
	return EPL_32;
}

template< class T >
D3D_MIN_PRECISION CMinPrecValue<T>::ConvertToPrecisionLevel( EPrecisionLevel level )
{
	switch( level )
	{
	case EPL_32: return D3D_MIN_PRECISION_DEFAULT;
	case EPL_16: return D3D_MIN_PRECISION_ANY_16;
	case EPL_10: return D3D_MIN_PRECISION_ANY_10;
	}
	assert( false );
	return D3D_MIN_PRECISION_DEFAULT;
}

template< class T >
void CMinPrecValue<T>::Uniquify()
{
	sort( m_Values.begin(), m_Values.end() );
	vector<T>::iterator newEnd = unique( m_Values.begin(), m_Values.end() );

	// Remove values after the unique set
	m_Values.resize( newEnd - m_Values.begin() );
}

// For each of the current values, convert it to the specified min precision level
template< class T >
void CMinPrecValue<T>::EnsureMinPrecisionLevel( EPrecisionLevel minLevel, D3D_FEATURE_LEVEL fl )
{
	UINT size = m_Values.size();

	// Make sure we don't add any levels with lower precision than what the format supports
	minLevel = ( minLevel < GetMinPrecisionLevel() ) ? GetMinPrecisionLevel() : minLevel;

	// Add all precision levels that are at least a precise as minLevel
	for( EPrecisionLevel level=EPL_32; level >= minLevel; level = (EPrecisionLevel)(level >> 1) )
	{
		if( m_Levels & level )
		{
			continue;
		}
		for( UINT i=0; i < size; i++ )
		{
			vector<T> newValues;
			ConvertToPrecision<T>( m_Values[i], level, newValues, fl );
			for( UINT j=0; j < newValues.size(); j++ )
			{
				if( newValues[j] == m_Values[i] )
				{
					// don't add it if the conversion didn't change the value
					continue;
				}
				m_Values.push_back( newValues[j] );
			}
		}
		m_Levels = m_Levels | level;
	}

	// Remove any precision levels that are less precise than minLevel.
	// Note that we don't have to remove any actual numeric values because every numeric value can be represented 
	// in 32-bits, which cannot be removed.
	for( EPrecisionLevel level=EPL_8; level < minLevel; level = (EPrecisionLevel)(level << 1) )
	{
		m_Levels = m_Levels & ~level;
	}

	Uniquify();
}

// For each of the current values, apply the range clamp for each min precision level this value represents
template< class T >
void CMinPrecValue<T>::ApplyRangeClamp( D3D_FEATURE_LEVEL fl )
{
	UINT size = m_Values.size();

	// Apply clamps at all precision levels represented
	for( EPrecisionLevel level=EPL_32; level >= EPL_8; level = (EPrecisionLevel)(level >> 1) )
	{
		if( ~m_Levels & level )
		{
			continue;
		}
		for( UINT i=0; i < size; i++ )
		{
			vector<T> newValues;
			ConvertToPrecision<T>( m_Values[i], level, newValues, fl, true );
			for( UINT j=0; j < newValues.size(); j++ )
			{
				if( newValues[j] == m_Values[i] )
				{
					// don't add it if the conversion didn't change the value
					continue;
				}
				m_Values.push_back( newValues[j] );
			}
		}
	}

	Uniquify();
}

// For each of the current values, print them to a string so they can easily be logged
template< class T >
tstring CMinPrecValue<T>::PrintValues() const
{
	tstring values = "";
	const UINT numValues = m_Values.size();

	for( UINT i=0; i < numValues; i++ )
	{
		values += ToStringSafe( m_Values[i] );

		// Determine if we should add a ", "
		if( (numValues > 1) && (i < (numValues - 1)) )
		{
			values += ", ";
		}
	}

	return values;
}

template< class T >
void CMinPrecValue<T>::_neg()
{
	for( UINT i=0; i < m_Values.size(); i++ )
	{
#pragma warning( suppress:4146 ) // this allows unary minus on UINT on the following line
		m_Values[i] = -m_Values[i];
	}
	ApplyRangeClamp( D3D_FEATURE_LEVEL_10_0 ); // feature level doesn't matter when clamping after changing sign
}

template< class T >
void CMinPrecValue<T>::_abs()
{
	for( UINT i=0; i < m_Values.size(); i++ )
	{
		m_Values[i] = abs(m_Values[i]);
	}
	ApplyRangeClamp( D3D_FEATURE_LEVEL_10_0 ); // feature level doesn't matter when clamping after changing sign
}
template<>
void CMinPrecValue<UINT>::_abs()
{
	// UINT specialization is empty because all UINTs are positive
}

template< class T >
void CMinPrecValue<T>::_absneg()
{
	// perform the two halves separately, as the clamp may be applied in the middle
	for( UINT i=0; i < m_Values.size(); i++ )
	{
		m_Values[i] = abs(m_Values[i]);
	}
	ApplyRangeClamp( D3D_FEATURE_LEVEL_10_0 ); // feature level doesn't matter when clamping after changing sign
	for( UINT i=0; i < m_Values.size(); i++ )
	{
		m_Values[i] = -m_Values[i];
	}
	ApplyRangeClamp( D3D_FEATURE_LEVEL_10_0 ); // feature level doesn't matter when clamping after changing sign
}
template<>
void CMinPrecValue<UINT>::_absneg()
{
	// UINTs are already positive, so absneg = neg
	_neg();
}

template< class T >
void CMinPrecValue<T>::Saturate()
{
	// 0 and 1 are describable in all formats, so no need to be careful here.
	for( UINT i=0; i < m_Values.size(); i++ )
	{
		m_Values[i] = min( max( m_Values[i], 0 ), 1 );
	}
	Uniquify();
}
template<>
void CMinPrecValue<float>::Saturate()
{
	// 0 and 1 are describable in all formats, so no need to be careful here.
	for( UINT i=0; i < m_Values.size(); i++ )
	{
		m_Values[i] = IsNAN( m_Values[i] ) ? m_Values[i] : min( max( m_Values[i], 0 ), 1 );
	}
	Uniquify();
}


// Perform f on each of the values in A,B,C
template< class T >
CMinPrecValue<T> CMinPrecValue<T>::op( const CMinPrecValue<T>& A, const CMinPrecValue<T>& B, const CMinPrecValue<T>& C, PTOP3 f )
{
	CMinPrecValue< T > R;
	for( UINT i=0; i < A.m_Values.size(); i++ )
	{
		for( UINT j=0; j < B.m_Values.size(); j++ )
		{
			for( UINT k=0; k < C.m_Values.size(); k++ )
			{
				T v = f( A.m_Values[i], B.m_Values[j], C.m_Values[k] );
				R.m_Values.push_back(v);
			}
		}
	}
	R.m_Levels = EPL_32;

	R.Uniquify();

	return R;
}

// Perform f on each of the values in A,B
template< class T >
CMinPrecValue<T> CMinPrecValue<T>::op( const CMinPrecValue<T>& A, const CMinPrecValue<T>& B, PTOP2 f )
{
	CMinPrecValue< T > R;
	for( UINT i=0; i < A.m_Values.size(); i++ )
	{
		for( UINT j=0; j < B.m_Values.size(); j++ )
		{
			T v = f( A.m_Values[i], B.m_Values[j] );
			R.m_Values.push_back(v);
		}
	}
	R.m_Levels = EPL_32;

	R.Uniquify();

	return R;
}

// Perform f on each of the values in A
template< class T >
CMinPrecValue<T> CMinPrecValue<T>::op( const CMinPrecValue<T>& A, PTOP1 f )
{
	CMinPrecValue< T > R;
	for( UINT i=0; i < A.m_Values.size(); i++ )
	{
		T v = f( A.m_Values[i] );
		R.m_Values.push_back(v);
		if( A.m_Values[i] == 0 )
		{
			// Deal with float -0.  For example, 1/-0 = -INF while 1/0 = INF
#pragma warning( suppress:4146 ) // this allows unary minus on UINT on the following line
			T v2 = f( -A.m_Values[i] );
			if( v2 != v )
			{
				R.m_Values.push_back(v2);
			}
		}
	}
	R.m_Levels = EPL_32;

	R.Uniquify();

	return R;
}

// Returns whether or not value is within the ranges of values specified by this object
template<>
bool CMinPrecValue<float>::Matches( float value, EPrecisionLevel level, D3D_FEATURE_LEVEL fl, bool bForceExact, float ULPMultiplier )
{
	// 9.x shaders have 16-bit render targets, so don't expect more precision
	level = fl < D3D_FEATURE_LEVEL_10_0 ? min( level, EPL_16 ) : level;

	if( !bForceExact )
	{
		const bool bHalfULP = fl >= D3D_FEATURE_LEVEL_10_0;
		// Accept numbers that are within one ULP of expected result
		for( UINT i=0; i < m_Values.size(); i++ )
		{
			// If there was an undefined calculation, accept anything
			if( IsNAN( m_Values[i] ) )
			{
				return true;
			}

			if( value == m_Values[i] )
			{
				return true;
			}

			switch( level )
			{
			case EPL_32:
				{
					// Check within ULPMultiplier (default 1) 32-ULP of m_Values[i]

					// Calculate one ULP by finding the value of the final bit
					const UINT UintValue = *(UINT*)&m_Values[i];
					const UINT UintOffByOne = UintValue ^ 0x1;
					const float fOffByOne = *(float*)&UintOffByOne;
					const float diff = abs( m_Values[i] - fOffByOne ) * ULPMultiplier;

					// Calculate the acceptible range
					const float top = ( bHalfULP ? m_Values[i] : max( m_Values[i], fOffByOne ) ) + diff;
					const float bottom = ( bHalfULP ? m_Values[i] : min( m_Values[i], fOffByOne ) ) - diff;

					if( value <= top && value >= bottom )
					{
						return true;
					}
				}
				break;
			case EPL_16:
				{
					// Any denorm value is acceptible on 9.x
					if( fl < D3D10_FEATURE_LEVEL_10_0 && abs(value) < abs(g_fSmallFloat16) && abs(m_Values[i]) < abs(g_fSmallFloat16) && m_Values[i] != 0 )
					{
						return true;
					}

					// Check within 1 16-ULP of m_Values[i]

					// Calculate one ULP by finding the value of the final bit
					const UINT truncatedValue = *(UINT*)&m_Values[i] & 0xffffe000;
					const UINT truncatedOffByOne = truncatedValue ^ 0x00002000;
					const float fTruncated = *(float*)&truncatedValue;
					const float fTruncatedOffByOne = *(float*)&truncatedOffByOne;
					const float diff = abs( fTruncated - fTruncatedOffByOne ) * ULPMultiplier;

					// Calculate the acceptible range
					const float top = ( bHalfULP ? m_Values[i] : max( fTruncated, fTruncatedOffByOne ) ) + diff;
					const float bottom = ( bHalfULP ? m_Values[i] : min( fTruncated, fTruncatedOffByOne ) ) - diff;

					if( value <= top && value >= bottom )
					{
						return true;
					}
				}
				break;
			case EPL_10:
			case EPL_8:
				{
					// Check within 1/255 of m_Values[i]

					const float fSmall = min( floor( m_Values[i] * 255.0f ) / 255.0f, floor( m_Values[i] * 256.0f ) / 256.0f );
					const float fBig = max( floor( m_Values[i] * 255.0f ) / 255.0f, floor( m_Values[i] * 256.0f ) / 256.0f );
					const float diff = 1.0f / 255.0f * ULPMultiplier;

					const float top = ( bHalfULP ? m_Values[i] : fBig ) + diff;
					const float bottom = ( bHalfULP ? m_Values[i] : fSmall ) - diff;

					if( value <= top && value >= bottom )
					{
						return true;
					}
				}
				break;
			}
		}
	}
	else
	{
		// Accept only exact numbers
		for( UINT i=0; i < m_Values.size(); i++ )
		{
			// We still check for equality of NANs.  Note that "NAN == NAN" returns false so we use IsNAN instead
			if( IsNAN( m_Values[i] ) )
			{
				// There is no way to represent NAN in 10- or 8-bit, so accept anything in these cases
				if( IsNAN( value ) || level <= EPL_10 )
				{
					return true;
				}
			}

			if( value == m_Values[i] )
			{
				return true;
			}
		}
	}

	return false;
}

// Returns whether or not value is within the ranges of values specified by this object
template< class T >
bool CMinPrecValue<T>::Matches( T value, EPrecisionLevel level, D3D_FEATURE_LEVEL fl, bool bForceExact, float ULPMultiplier )
{
	// Accept only exact numbers
	for( UINT i=0; i < m_Values.size(); i++ )
	{
		if( value == m_Values[i] )
		{
			return true;
		}
	}

	return false;
}

// Force template instanciation so templated functions are available in other compilation units
template class CMinPrecValue<float>;
template class CMinPrecValue<UINT>;
template class CMinPrecValue<int>;
template class CMinPrecValue<double>;


//---------------------------------------------------------------

/******************************
* CPrecisionTest methods
******************************/

BEGIN_NAMED_VALUES( SRC_MOD_TYPE )
	NAMED_VALUE( _T( "NONE" ), SMOD_NONE )
	NAMED_VALUE( _T( "NEG" ), SMOD_NEG )
	NAMED_VALUE( _T( "ABS" ), SMOD_ABS )
	NAMED_VALUE( _T( "ABSNEG" ), SMOD_ABSNEG )
END_NAMED_VALUES( SRC_MOD_TYPE )

BEGIN_NAMED_VALUES( INST_MOD_TYPE )
	NAMED_VALUE( _T( "NONE" ), IMOD_NONE )
	NAMED_VALUE( _T( "SAT" ), IMOD_SAT )
END_NAMED_VALUES( INST_MOD_TYPE )

BEGIN_NAMED_VALUES( OPERAND_TYPE )
	NAMED_VALUE( _T( "Temp" ), OT_TEMP )
	NAMED_VALUE( _T( "ConstantBuffer" ), OT_CONSTANT_BUFFER )
	NAMED_VALUE( _T( "TempAndConstantBuffer" ), OT_TEMP_AND_CONSTANT_BUFFER )
	NAMED_VALUE( _T( "ConstantBufferAndImmediate" ), OT_CONSTANT_BUFFER_AND_IMMEDIATE )
	NAMED_VALUE( _T( "TempAndImmediate" ), OT_TEMP_AND_IMMEDIATE )
END_NAMED_VALUES( OPERAND_TYPE )

BEGIN_NAMED_VALUES( MIN_PRECISION_LOCATION )
	NAMED_VALUE( _T( "Src" ), MPL_SRC )
	NAMED_VALUE( _T( "Dest" ), MPL_DEST )
	NAMED_VALUE( _T( "Both" ), MPL_BOTH )
END_NAMED_VALUES( MIN_PRECISION_LOCATION )


TEST_RESULT CPrecisionTest::Setup()
{
	TEST_RESULT tr = CD3D11Test::Setup( );
	if( tr != RESULT_PASS )
		return tr;

	if( !WriteDeviceMinPrecisionSupportToTheLog() )
	{
		WriteToLog( _T("Cannot retrieve device min precision support.") );
		// Don't need to fail. A warning message will suffice.
	}

	return RESULT_PASS;
}

void CPrecisionTest::Cleanup()
{
	CD3D11Test::Cleanup();
}

char* CPrecisionTest::MinPrecisionPrefix( D3D_MIN_PRECISION level )
{
	char* pMinPrecDecl = NULL;
	switch( level )
	{
	case D3D_MIN_PRECISION_ANY_10:
		pMinPrecDecl = "min10";
		break;
	case D3D_MIN_PRECISION_ANY_16:
		pMinPrecDecl = "min16";
		break;
	case D3D_MIN_PRECISION_DEFAULT:
		pMinPrecDecl = "";
		break;
	}
	return pMinPrecDecl;
}

testfactor::RFactor CPrecisionTest::InitializeMinPrecisionFactor( testfactor::RFactor* pIntFactor )
{
	// Min precision
	CTestCaseParameter<D3D_MIN_PRECISION> *pMinPrecisionParam = AddParameter( _T( "MinPrecision" ), &m_MinPrecision );
	CUserValueSet<D3D_MIN_PRECISION> *pPrecisionValues = new CUserValueSet<D3D_MIN_PRECISION>( );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_DEFAULT );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_16 );
	testfactor::RFactor rMinPrecisionInt = AddParameterValueSet( pMinPrecisionParam, pPrecisionValues );
	pPrecisionValues->AddValue( D3D_MIN_PRECISION_ANY_10 );
	testfactor::RFactor rMinPrecisionFloat = AddParameterValueSet( pMinPrecisionParam, pPrecisionValues );
	SetParameterDefaultValue<D3D_MIN_PRECISION>( pMinPrecisionParam, D3D_MIN_PRECISION_DEFAULT );

	if( pIntFactor )
	{
		*pIntFactor = rMinPrecisionInt;
	}
	return rMinPrecisionFloat;
}


testfactor::RFactor CPrecisionTest::InitializeSrcModifierFactor()
{
	// Source Modifier
	CTestCaseParameter<SRC_MOD_TYPE> *pParam = AddParameter( _T( "SrcMod" ), &m_SrcModifier );
	CUserValueSet<SRC_MOD_TYPE> *pValues = new CUserValueSet<SRC_MOD_TYPE>( );
	pValues->AddValue( SMOD_NONE );
	pValues->AddValue( SMOD_NEG );
	pValues->AddValue( SMOD_ABS );
	pValues->AddValue( SMOD_ABSNEG );
	testfactor::RFactor rFactor = AddParameterValueSet( pParam, pValues );
	SetParameterDefaultValue<SRC_MOD_TYPE>( pParam, SMOD_NONE );

	return rFactor;
}

testfactor::RFactor CPrecisionTest::InitializeInstructionModifierFactor()
{
	// Instruction Modifier
	CTestCaseParameter<INST_MOD_TYPE> *pParam = AddParameter( _T( "InstMod" ), &m_InstructionModifier );
	CUserValueSet<INST_MOD_TYPE> *pValues = new CUserValueSet<INST_MOD_TYPE>( );
	pValues->AddValue( IMOD_NONE );
	pValues->AddValue( IMOD_SAT );
	testfactor::RFactor rFactor = AddParameterValueSet( pParam, pValues );
	SetParameterDefaultValue<INST_MOD_TYPE>( pParam, IMOD_NONE );

	return rFactor;
}

testfactor::RFactor CPrecisionTest::InitializeShaderStageModifierFactor()
{
	D3D_FEATURE_LEVEL FeatureLevel = GetFramework()->m_D3DOptions.FeatureLevel;

	// Shader stage
	CTestCaseParameter<D3D_SHADER_VARIABLE_TYPE> *pShaderStageParam = AddParameter( _T("ShaderStage"), &m_ShaderStage );
	CUserValueSet<D3D_SHADER_VARIABLE_TYPE> *pShaderStageValues = new CUserValueSet<D3D_SHADER_VARIABLE_TYPE>();
	pShaderStageValues->AddValue(D3D_SVT_VERTEXSHADER);
	pShaderStageValues->AddValue(D3D_SVT_PIXELSHADER);
	if( FeatureLevel >= D3D_FEATURE_LEVEL_10_0 )
	{
		pShaderStageValues->AddValue(D3D_SVT_GEOMETRYSHADER);
		if( FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
		{
			pShaderStageValues->AddValue(D3D_SVT_HULLSHADER);
			pShaderStageValues->AddValue(D3D_SVT_DOMAINSHADER);
			pShaderStageValues->AddValue(D3D_SVT_COMPUTESHADER);
		}
		else
		{
			// Check that the current adapter supports CS4_X
			D3D11_FEATURE_DATA_D3D10_X_HARDWARE_OPTIONS hwOptions = {0};
			HRESULT hr = S_OK;
			if( FAILED( hr = GetDevice()->CheckFeatureSupport(D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS, &hwOptions, sizeof(hwOptions) ) ) )
			{
				LogError( __FILEW__, __LINE__, L"CheckFeatureSupport() failed for D3D11_FEATURE_D3D10_X_HARDWARE_OPTIONS.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCheckFeatureSupport );
			}

			if( hwOptions.ComputeShaders_Plus_RawAndStructuredBuffers_Via_Shader_4_x )
			{
				pShaderStageValues->AddValue(D3D_SVT_COMPUTESHADER);
			}
		}
	}
	testfactor::RFactor rfShaderStage = AddParameterValueSet( pShaderStageParam, pShaderStageValues );
	SetParameterDefaultValue<D3D_SHADER_VARIABLE_TYPE>( pShaderStageParam, D3D_SVT_PIXELSHADER );

	return rfShaderStage;
}

testfactor::RFactor CPrecisionTest::InitializeOperandTypeFactor()
{
	// Operand Type
	CTestCaseParameter<OPERAND_TYPE> *pParam = AddParameter( _T( "OperandType" ), &m_OperandType );
	CUserValueSet<OPERAND_TYPE> *pValues = new CUserValueSet<OPERAND_TYPE>( );
	pValues->AddValue( OT_TEMP );
	pValues->AddValue( OT_CONSTANT_BUFFER );
	pValues->AddValue( OT_TEMP_AND_CONSTANT_BUFFER );
	pValues->AddValue( OT_CONSTANT_BUFFER_AND_IMMEDIATE );
	pValues->AddValue( OT_TEMP_AND_IMMEDIATE );
	testfactor::RFactor rFactor = AddParameterValueSet( pParam, pValues );
	SetParameterDefaultValue<OPERAND_TYPE>( pParam, OT_TEMP );

	return rFactor;
}

testfactor::RFactor CPrecisionTest::InitializeLocationFactor( testfactor::RFactor* pDefaultFactor )
{
	// Modifier Location
	CTestCaseParameter<MIN_PRECISION_LOCATION> *pParam = AddParameter( _T( "Location" ), &m_ModifierLocation );
	CUserValueSet<MIN_PRECISION_LOCATION> *pValues = new CUserValueSet<MIN_PRECISION_LOCATION>( );
	pValues->AddValue( MPL_BOTH );
	testfactor::RFactor rDefaultFactor = AddParameterValueSet( pParam, pValues );
	pValues->AddValue( MPL_SRC );
	pValues->AddValue( MPL_DEST );
	testfactor::RFactor rFactor = AddParameterValueSet( pParam, pValues );
	SetParameterDefaultValue<MIN_PRECISION_LOCATION>( pParam, MPL_SRC );

	if( pDefaultFactor )
	{
		*pDefaultFactor = rDefaultFactor;
	}
	return rFactor;
}

template< class T >
void CPrecisionTest::WriteActualResultAndExpectedResultsToTheLog(const UINT &resultRegister, const UINT &component, const T &actualResult, const CMinPrecValue<T> &expectedResults) const
{
	WriteToLog( _T("Result %u, component %u failed."), resultRegister, component );
	WriteToLog( _T("Valid result(s): %s"), expectedResults.PrintValues().c_str() );
	WriteToLog( _T("Actual result: %s (0x%.8x)"), ToStringSafe( actualResult ).c_str(), *(UINT*)&actualResult );
}

template<>
void CPrecisionTest::WriteActualResultAndExpectedResultsToTheLog<double>(const UINT &resultRegister, const UINT &component, const double &actualResult, const CMinPrecValue<double> &expectedResults) const
{
	WriteToLog( _T("Result %u, component %u failed."), resultRegister, component );
	WriteToLog( _T("Valid result(s): %s"), expectedResults.PrintValues().c_str() );
	WriteToLog( _T("Actual result: %s (0x%.16x)"), ToStringSafe( actualResult ).c_str(), *(UINT64*)&actualResult );
}

template void CPrecisionTest::WriteActualResultAndExpectedResultsToTheLog<float>(const UINT &resultRegister, const UINT &component, const float &actualResult, const CMinPrecValue<float> &expectedResults) const;
template void CPrecisionTest::WriteActualResultAndExpectedResultsToTheLog<INT>(const UINT &resultRegister, const UINT &component, const INT &actualResult, const CMinPrecValue<INT> &expectedResults) const;
template void CPrecisionTest::WriteActualResultAndExpectedResultsToTheLog<UINT>(const UINT &resultRegister, const UINT &component, const UINT &actualResult, const CMinPrecValue<UINT> &expectedResults) const;

bool CPrecisionTest::WriteDeviceMinPrecisionSupportToTheLog() const
{
	// Ensure the device is valid
	ID3D11Device *pDevice = GetFramework()->GetDevice();
	if( NULL == pDevice )
	{
		assert( false );
		WriteToLog( _T("Framework device is NULL. Cannot retrieve device min precision support.") );
		return false;
	}

	{
		HRESULT hr = E_FAIL;
		D3D11_FEATURE_DATA_SHADER_MIN_PRECISION_SUPPORT minPrecisionData;
		ZeroMemory( &minPrecisionData, sizeof(minPrecisionData) );
		if( FAILED( hr = pDevice->CheckFeatureSupport( D3D11_FEATURE_SHADER_MIN_PRECISION_SUPPORT, &minPrecisionData, sizeof(minPrecisionData) ) ) )
		{
			LogError( __FILEW__, __LINE__, L"CheckFeatureSupport() failed.", hr, ERRORCODE_TYPE_HRESULT, gFCDeviceCheckFeatureSupport );
			return false;
		}

		tstring pixelShaderSupportStr = "DEFAULT";
		tstring otherShaderSupportStr = "DEFAULT";
		std::vector<D3D11_SHADER_MIN_PRECISION_SUPPORT> minPrecisionLevelsToCheck;

		minPrecisionLevelsToCheck.clear();
		minPrecisionLevelsToCheck.push_back( D3D11_SHADER_MIN_PRECISION_16_BIT );
		minPrecisionLevelsToCheck.push_back( D3D11_SHADER_MIN_PRECISION_10_BIT );
		for( UINT i = 0; i < minPrecisionLevelsToCheck.size(); i++ )
		{
			const tstring minPrecisionStr = ToString(minPrecisionLevelsToCheck[i]);

			if( minPrecisionData.PixelShaderMinPrecision & minPrecisionLevelsToCheck[i] )
			{
				pixelShaderSupportStr += ", ";
				pixelShaderSupportStr += minPrecisionStr;
			}

			if( minPrecisionData.AllOtherShaderStagesMinPrecision & minPrecisionLevelsToCheck[i] )
			{
				otherShaderSupportStr += ", ";
				otherShaderSupportStr += minPrecisionStr;
			}
		}

		WriteToLog( _T("Device reported min precision support: Pixel Shader = %s; All Other Shader Stages = %s."), pixelShaderSupportStr.c_str(), otherShaderSupportStr.c_str() );
	}

	return true;
}

//---------------------------------------------------------------

/******************************
* CPrecisionTestApp methods
******************************/

void CPrecisionTestApp::InitOptionVariables( )
{
	CD3D11TestFramework::InitOptionVariables();
	int n = AddOptionCategory( _T( "Debug" ) );

	UnRegisterOptionVariable("srconly");
	m_D3DOptions.SrcOnly = true;
    m_D3DOptions.wMinFeatureLevel = D3D_FEATURE_LEVEL_9_2;
}

//---------------------------------------------------------------

bool CPrecisionTestApp::InitTestGroups( )
{
	if( !ADD_TEST( _T( "LowPrecision\\PixelShader\\Input" ), CPSInputPrecisionTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\VertexShader\\Input" ), CVSInputPrecisionTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\ShaderInstructions\\Arithmetic\\Uint" ), CArithmeticUintTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\ShaderInstructions\\Arithmetic\\Int" ), CArithmeticIntTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\ShaderInstructions\\Arithmetic\\Float" ), CArithmeticFloatTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\ShaderInstructions\\Mov\\Uint" ), CMovUintTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\ShaderInstructions\\Mov\\Int" ), CMovIntTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\ShaderInstructions\\Mov\\Float" ), CMovFloatTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\ShaderInstructions\\Swap\\Uint" ), CSwapUintTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\ShaderInstructions\\Swap\\Int" ), CSwapIntTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\ShaderInstructions\\Swap\\Float" ), CSwapFloatTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\ShaderInstructions\\TypeConversion" ), CConvertPrecisionTest ) ) return false;
	if( !ADD_TEST( _T( "LowPrecision\\ShaderInstructions\\ResourceAccess" ), CResourcePrecisionTest ) ) return false;

	return true;
}

