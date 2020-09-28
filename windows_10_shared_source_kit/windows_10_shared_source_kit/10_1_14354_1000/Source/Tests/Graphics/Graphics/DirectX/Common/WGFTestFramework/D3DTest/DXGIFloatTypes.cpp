// DXGIFloatTypes.cpp
// Copyright (c) 2004 Microsoft Corporation. All rights reserved.
//

#include "pch.h"

#include "DXGIFloatTypes.h"

#define FLOAT32_SIGN_MASK 0x80000000
#define FLOAT32_EXP_MASK 0x7f800000
#define FLOAT32_FRAC_MASK ( ( 1 << 23 ) - 1 )

// CDXGIFloat16
CDXGIFloat16::CDXGIFloat16( float f )
{
	UINT u = *(UINT*)&f;
	UINT Sign = (u & 0x80000000) >> 16;
	UINT MagU = u & 0x7FFFFFFF;     // Absolute value
	if( (u & (0xff<<23)) == (0xff<<23) )
	{
		// INF or NaN
		m_uValue = (UINT16)(Sign | (((1 << EXP_BITS) - 1))<<FRAC_BITS);
		if( (u & (~0xff800000)) != 0 )
		{
			// NaN - smash together the fraction bits to 
			//       keep the random 1's (in a way that keeps float16->float->float16 
			//       conversion invertible down to the bit level, even with NaN).
			m_uValue = (UINT16)(m_uValue| (((u>>13)|(u>>3)|(u))&0x000003ff));
		}
	}
	else if (MagU > MAX_UNBIASED_NORM)       
	{
		// Not representable by 16 bit float -> use flt16_max (due to round to zero)
		m_uValue = (UINT16)(Sign | ((((1 << EXP_BITS) - 2))<<FRAC_BITS) | FRAC_MASK);
	}
	else if (MagU < MIN_UNBIASED_NORM) 
	{
		// Denormalized value

		// Make implicit 1 explicit
		UINT Frac = (MagU & ((1<<23)-1)) | (1<<23);   
		int nshift = (EXP_MIN + 127 - (MagU >> 23));

		if (nshift < 24)
		{
			MagU = Frac >> nshift;
		}
		else
		{
			MagU = 0;
		}

		// Round to zero
		m_uValue = (UINT16)(Sign | (MagU>>FRAC_BITS_DIFF));
	}
	else
	{
		// Normalize value with Round to zero
		m_uValue = (UINT16)(Sign | ((MagU + BIAS_DIFF)>>FRAC_BITS_DIFF));
	}
}

CDXGIFloat16::operator float() const
{
	UINT tmp;
	UINT exp = m_uValue & EXP_MASK;
	if( exp == 0 )
	{
		if ((m_uValue & FRAC_MASK) != 0)
		{
			// Normalizing the denormalized value
			exp = EXP_MIN;
			UINT frac = m_uValue & FRAC_MASK;
			while ((frac & (FRAC_MASK + 1)) == 0)
			{
				exp--;
				frac <<= 1;
			}
			frac &= ~(FRAC_MASK + 1);			// Remove hidden bit
			tmp = ((m_uValue & SIGN_BIT_MASK) << 16) |	// Sigh bit
				((exp + 127) << 23) |			// Exponent
				(frac << FRAC_BITS_DIFF);		// Fraction
		}
		else
		{
			// Zero - only sign bit is used
			tmp = (m_uValue & SIGN_BIT_MASK) << 16;
		}
	}
	else if( exp == EXP_MASK )
	{
		tmp = ((m_uValue & SIGN_BIT_MASK) << 16) |						// Sign bit
			0x7f800000 |										// Exponent
			((m_uValue & FRAC_MASK) << FRAC_BITS_DIFF);				// Fraction
	}
	else
	{
		tmp = ((m_uValue & SIGN_BIT_MASK) << 16) |						// Sign bit
			((((m_uValue >> FRAC_BITS) & ((1 << EXP_BITS) - 1)) -
			EXP_BIAS + 127) << 23) |							// Exponent
			((m_uValue & FRAC_MASK) << FRAC_BITS_DIFF);				// Fraction
	}
	return *(FLOAT*)&tmp;
}


// CDXGIFloat10
CDXGIFloat10::CDXGIFloat10( float f )
{
	const UINT32 dwMax = ( ( EXP_MAX_UNBIASED_NORM + 127 ) << 23 ) + 0x007f0000;
	const UINT32 dwMin = ( ( EXP_MIN_UNBIASED_NORM + 127 ) << 23 );
	const INT32 nExpDiff = ( EXP_BIAS - 127 ) << 23;

	m_dwValue = 0;	

	if( f > 0 )
	{
		UINT32 &dwF = *( (UINT32 *) &f );
		
		if( dwF > dwMax)
		{
			if( ( dwF & FLOAT32_EXP_MASK ) == FLOAT32_EXP_MASK && dwF != FLOAT32_EXP_MASK )
			{
				// Clamp NaN to zero
				m_dwValue = 0;
			}
			else
			{
				// Clamp to Max
				m_dwValue = MAX_VALUE;
			}
		}
		else
		if( dwF < dwMin )
		{
			// Denormal
			
			int nShift = ( ( dwMin - dwF ) >> 23 ) + ( 1 + 23 - FRAC_BITS );
			
			// Make implicit 1 explicit and round toward zero
			m_dwValue = ( ( dwF & FLOAT32_FRAC_MASK ) | ( 1 << 23 ) ) >> nShift;
		}
		else
		{
			// Round toward zero
			m_dwValue = ( ( ( dwF & FLOAT32_EXP_MASK ) + nExpDiff ) >> ( 23 - FRAC_BITS ) ) | ( dwF & FLOAT32_FRAC_MASK ) >> ( 23 - FRAC_BITS );
		}
	}
}

CDXGIFloat10::operator float() const
{
	const INT32 nExpDiff = ( 127 - EXP_BIAS ) << 23;
	UINT32 dwVal;
	
	if( m_dwValue == 0 )
		return 0.0f;
		
	int nShift = ( 23 - FRAC_BITS );
	UINT32 dwExp = ( ( m_dwValue << nShift ) & FLOAT32_EXP_MASK ) + nExpDiff;
	UINT32 dwFrac = ( m_dwValue & FRAC_MASK ) << nShift;
	
	if( dwExp == nExpDiff && dwFrac != 0 )
	{
		for( dwFrac <<= 1; ( dwFrac & ( 1 << 23 ) ) == 0; dwFrac <<= 1 )
		{
			dwExp -= ( 1 << 23 );
		}

		// Strip off extra bit
		dwFrac &= FLOAT32_FRAC_MASK;
	}
			
	dwVal = dwExp | dwFrac;

	return *( (float *) &dwVal );
}




// CDXGIFloat10
CDXGIFloat11::CDXGIFloat11( float f )
{
	const UINT32 dwMax = ( ( EXP_MAX_UNBIASED_NORM + 127 ) << 23 ) + 0x007f0000;
	const UINT32 dwMin = ( ( EXP_MIN_UNBIASED_NORM + 127 ) << 23 );
	const INT32 nExpDiff = ( EXP_BIAS - 127 ) << 23;

	m_dwValue = 0;	

	if( f > 0 )
	{
		UINT32 &dwF = *( (UINT32 *) &f );
		
		if( dwF > dwMax)
		{
			if( ( dwF & FLOAT32_EXP_MASK ) == FLOAT32_EXP_MASK && dwF != FLOAT32_EXP_MASK )
			{
				// Clamp NaN to zero
				m_dwValue = 0;
			}
			else
			{
				// Clamp to Max
				m_dwValue = MAX_VALUE;
			}
		}
		else
		if( dwF < dwMin )
		{
			// Denormal
			
			int nShift = ( ( dwMin - dwF ) >> 23 ) + ( 1 + 23 - FRAC_BITS );
			
			// Make implicit 1 explicit and round toward zero
			m_dwValue = ( ( dwF & FLOAT32_FRAC_MASK ) | ( 1 << 23 ) ) >> nShift;
		}
		else
		{
			// Round toward zero
			m_dwValue = ( ( ( dwF & FLOAT32_EXP_MASK ) + nExpDiff ) >> ( 23 - FRAC_BITS ) ) | ( dwF & FLOAT32_FRAC_MASK ) >> ( 23 - FRAC_BITS );
		}
	}
}

CDXGIFloat11::operator float() const
{
	const INT32 nExpDiff = ( 127 - EXP_BIAS ) << 23;
	UINT32 dwVal;
	
	if( m_dwValue == 0 )
		return 0.0f;
		
	int nShift = ( 23 - FRAC_BITS );
	UINT32 dwExp = ( ( m_dwValue << nShift ) & FLOAT32_EXP_MASK ) + nExpDiff;
	UINT32 dwFrac = ( m_dwValue & FRAC_MASK ) << nShift;
	
	if( dwExp == nExpDiff && dwFrac != 0 )
	{
		for( dwFrac <<= 1; ( dwFrac & ( 1 << 23 ) ) == 0; dwFrac <<= 1 )
		{
			dwExp -= ( 1 << 23 );
		}

		// Strip off extra bit
		dwFrac &= FLOAT32_FRAC_MASK;
	}
			
	dwVal = dwExp | dwFrac;

	return *( (float *) &dwVal );
}

