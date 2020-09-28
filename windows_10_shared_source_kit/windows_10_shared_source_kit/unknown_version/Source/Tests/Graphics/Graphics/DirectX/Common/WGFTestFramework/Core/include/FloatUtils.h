//////////////////////////////////////////////////////////////////////////
//  FloatUtils.h
//  created:	2005/04/26
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
//////////////////////////////////////////////////////////////////////////
// Compare Functions
//
// The arguments are passed in as const float references in order to
// prevent the FPU from changing SNaN values to QNaN values.
//////////////////////////////////////////////////////////////////////////

// Standard epsilon compare.
extern bool WGFTESTAPI CompareFloatEpsilon( const float &fsrc, const float &fref, float epsilon );

// Compare using ULP (unit last place) tolerance
extern bool WGFTESTAPI CompareFloatULP( const float &fsrc, const float &fref, int nULPTolerance );
extern bool WGFTESTAPI CompareDoubleULP( const double &dsrc, const double &dref, UINT nULPTolerance );

// Compare using relative epsilon error (relative diff < 2^nRelativeExp ).
extern bool WGFTESTAPI CompareFloatRelativeEpsilon( const float &fsrc, const float &fref, int nRelativeExp );


//////////////////////////////////////////////////////////////////////////
// FloatMaker
//////////////////////////////////////////////////////////////////////////

// Define wrapper macros for fundamental types to guarantee
// 4-byte size on all architectures.

#define FLOAT32 float

#if FLT_MANT_DIG == 24
	
class FloatMaker
{
	union ULFloat
	{
		UINT32	ulfI;
		FLOAT32	ulfF;

		inline void operator = ( UINT32 A ) { ulfI = A; }
		inline void operator = ( FLOAT32 A ) { ulfF = A; }
	}m_f;
      
public:
    FloatMaker( UINT32 dwVal ) 
	{
		assert( 4 == sizeof(FLOAT32) && 4 == sizeof(UINT32) );
		m_f = dwVal; 
	}
	FloatMaker( FLOAT32 fVal ) 
	{
		assert( 4 == sizeof(FLOAT32) && 4 == sizeof(UINT32) );
		m_f = fVal; 
	}
	FloatMaker( int nExponent, DWORD dwMantissa )
	{
		assert( ( dwMantissa & 0xff800000 ) == 0 );
		assert( nExponent >= -126 );
		assert( nExponent <= 127 );
		
		UINT32 dwVal = ( ( nExponent + 127 ) << 23 ) | dwMantissa;
		m_f = dwVal;
	}
	FloatMaker( ) 
	{
		assert( 4 == sizeof(FLOAT32) && 4 == sizeof(UINT32) );
		m_f = (UINT32)0; 
	}
	
	//Creates a positive SNAN with only the LSB of the mantissa set to 1
	static FloatMaker SNANp() { return FloatMaker( (UINT32) 0x7F800001 ); }
	//Creates a negative SNAN with only the LSB of the mantissa set to 1
	static FloatMaker SNANn() { return FloatMaker( (UINT32) 0xFF800001 ); }
	//Creates a positive QNAN with only the MSB set to 1
	static FloatMaker QNANp() { return FloatMaker( (UINT32) 0x7FC00000 ); }
	//Creates a negative QNAN with only the MSB and LSB set to 1
	static FloatMaker QNANn() { return FloatMaker( (UINT32) 0xFFC00001 ); }
	//Creates an IND
	static FloatMaker IND() { return FloatMaker( (UINT32) 0xFFC00000 ); }
	//Creates a positive INF
	static FloatMaker INFp() { return FloatMaker( (UINT32) 0x7F800000 ); }
	//Creates a negative INF
	static FloatMaker INFn() { return FloatMaker( (UINT32) 0xFF800000 ); }
	//Creates a negative Zero
	static FloatMaker ZEROn() { return FloatMaker( (UINT32) 0x80000000 ); }
	//Creates a positive Zero
	static FloatMaker ZEROp() { return FloatMaker( (UINT32) 0x00000000 ); }
	//Creates a negative Denorm
	static FloatMaker DENORMn() { return FloatMaker( (UINT32) 0x807fffff ); }
	//Creates a positive Denorm
	static FloatMaker DENORMp() { return FloatMaker( (UINT32) 0x007fffff ); }
	//Creates a positive FLOAT_MAX
	static FloatMaker FLOATMAXp() { return FloatMaker( (UINT32) 0x7f7fffff ); }
	//Creates a negative FLOAT_MAX
	static FloatMaker FLOATMAXn() { return FloatMaker( (UINT32) 0xff7fffff ); }
	
	//Creates a float with all exponent bits set to 1, QNAN, SNAN, etc.
	static FloatMaker MakeSpecialFloat( UINT32 ulMantissa, bool bSignBitOn = false )
	{
		UINT32 ulFLOAT32 = (UINT32) 0x7F800000;

		if ( bSignBitOn )
			ulFLOAT32 |= (UINT32) 0x80000000;

		ulFLOAT32 |= ( (UINT32) 0x007FFFFF & ulMantissa );

		return FloatMaker( ulFLOAT32 );
	}
	
	operator const FLOAT32& () const { return m_f.ulfF;	}
	operator FLOAT32& () { return m_f.ulfF;	}
	operator const UINT32&() const { return m_f.ulfI; }
	operator UINT32&() { return m_f.ulfI; }
	operator const DWORD&() const { return (const DWORD&) ( m_f.ulfI ); }
	operator DWORD&() { return (DWORD&) ( m_f.ulfI ); }
};

class Float16Maker
{
protected:
	UINT16 m_f;

	// same f16->f32 conversion runtime uses
	float ConvertToFloat32() const
	{
		UINT tmp;
        UINT exp = m_f & m_ExpMask;
        if( exp == 0 )
        {
            if ((m_f & m_FracMask) != 0)
            {
                // Normalizing the denormalized value
                exp = m_eMin;
                UINT frac = m_f & m_FracMask;
                while ((frac & (m_FracMask + 1)) == 0)
                {
                    exp--;
                    frac <<= 1;
                }
                frac &= ~(m_FracMask + 1); // Remove hidden bit
                tmp = ((m_f & m_SignMask) << 16) | // Sigh bit
                      ((exp + 127) << 23) |     // Exponent
                      (frac << m_cFracBitsDiff);  // Fraction
            }
            else
            {
                // Zero - only sign bit is used
                tmp = (m_f & m_SignMask) << 16;
            }
        }
        else if( exp == m_ExpMask )
        {
            tmp = ((m_f & m_SignMask) << 16) |                           // Sign bit
                    0x7f800000 |                                       // Exponent
                  ((m_f & m_FracMask) << m_cFracBitsDiff);               // Fraction
        }
        else
        {
            tmp = ((m_f & m_SignMask) << 16) |                         // Sign bit
                  ((((m_f >> m_cFracBits) & ((1 << m_cExpBits) - 1)) -
                    m_cExpBias + 127) << 23) |                        // Exponent
                  ((m_f & m_FracMask) << m_cFracBitsDiff);               // Fraction
        }
        return *((float*)&tmp);
	}

	// same f32->f16 conversion runtime uses
	void ConvertToFloat16(const float *pFloat32)
	{
		UINT u = *(UINT*)pFloat32;
        UINT16 v;
        UINT Sign = (u & 0x80000000) >> 16;
        UINT MagU = u & 0x7FFFFFFF;     // Absolute value
        if( (u & (0xff<<23)) == (0xff<<23) )
        {
            // INF or NaN
            v = (UINT16)(Sign | (((1 << m_cExpBits) - 1))<<m_cFracBits);
            if( (u & (~0xff800000)) != 0 )
            {
                // NaN - smash together the fraction bits to
                //       keep the random 1's (in a way that keeps float16->float->float16
                //       conversion invertible down to the bit level, even with NaN).
                v = (UINT16)(v| (((u>>13)|(u>>3)|(u))&0x000003ff));
            }
        }
        else if (MagU > m_wMaxNormal)
        {
            // Not representable by 16 bit float -> use flt16_max (due to round to zero)
            v = (UINT16)(Sign | ((((1 << m_cExpBits) - 2))<<m_cFracBits) | m_FracMask);
        }
        else if (MagU < m_wMinNormal)
        {
            // Denormalized value

            // Make implicit 1 explicit
            UINT Frac = (MagU & ((1<<23)-1)) | (1<<23);
            int nshift = (m_eMin + 127 - (MagU >> 23));

            if (nshift < 24)
            {
                MagU = Frac >> nshift;
            }
            else
            {
                MagU = 0;
            }

            // Round to zero
            v = (UINT16)(Sign | (MagU>>m_cFracBitsDiff));
        }
        else
        {
            // Normalize value with Round to zero
            v = (UINT16)(Sign | ((MagU + m_BiasDiffo)>>m_cFracBitsDiff));
        }

		m_f = v;
	}
	
	static const UINT m_cFracBits = 10;           // Number of fraction bits
    static const UINT m_cExpBits = 5;             // Number of exponent bits
    static const UINT m_cSignBit = 15;            // Index of the sign bit
    static const UINT m_SignMask = (1 << m_cSignBit);
    static const UINT m_FracMask = (1 << m_cFracBits) - 1;         // Fraction mask
    static const UINT m_ExpMask = ((1<< m_cExpBits) - 1) << m_cFracBits; // Exponent mask
    static const INT32  m_cExpBias = (1 << (m_cExpBits - 1)) - 1;     // Exponent bias
    static const UINT m_eMax =  m_cExpBias;         // Max exponent
    static const INT32  m_eMin = -m_cExpBias+1;       // Min exponent
    static const UINT m_wMaxNormal = ((m_eMax+127) << 23) | 0x7FE000;
    static const UINT m_wMinNormal = (m_eMin+127) << 23;
    static const UINT m_BiasDiffo = (m_cExpBias-127) << 23;
    static const UINT m_cFracBitsDiff = 23 - m_cFracBits;


public:
    Float16Maker( UINT16 wVal ) 
	{
		assert( 2 == sizeof(UINT16) );
		m_f = wVal; 
	}
	Float16Maker( FLOAT32 fVal ) 
	{
		assert( 4 == sizeof(FLOAT32) && 2 == sizeof(UINT16) );
		ConvertToFloat16(&fVal);
	}
	Float16Maker( int nExponent, UINT16 wMantissa )
	{
		assert( ( wMantissa & 0xfc00 ) == 0 );
		assert( nExponent >= -14 );
		assert( nExponent <= 15 );
		
		UINT16 wVal = ( ( nExponent + 15 ) << 10 ) | wMantissa;
		m_f = wVal;
	}
	Float16Maker( ) 
	{
		assert( 2 == sizeof(UINT16) );
		m_f = 0; 
	}
	
	//Creates a positive SNAN with only the LSB of the mantissa set to 1
	static Float16Maker SNANp() { return Float16Maker( (UINT16) 0x7C01 ); }
	//Creates a negative SNAN with only the LSB of the mantissa set to 1
	static Float16Maker SNANn() { return Float16Maker( (UINT16) 0xFC01 ); }
	//Creates a positive QNAN with only the MSB set to 1
	static Float16Maker QNANp() { return Float16Maker( (UINT16) 0x7E00 ); }
	//Creates a negative QNAN with only the MSB and LSB set to 1
	static Float16Maker QNANn() { return Float16Maker( (UINT16) 0xFE01 ); }
	//Creates an IND
	static Float16Maker IND() { return Float16Maker( (UINT16) 0xFE00 ); }
	//Creates a positive INF
	static Float16Maker INFp() { return Float16Maker( (UINT16) 0x7C00 ); }
	//Creates a negative INF
	static Float16Maker INFn() { return Float16Maker( (UINT16) 0xFC00 ); }
	//Creates a negative Zero
	static Float16Maker ZEROn() { return Float16Maker( (UINT16) 0x8000); }
	//Creates a positive Zero
	static Float16Maker ZEROp() { return Float16Maker( (UINT16) 0x0000); }
	//Creates a negative Denorm
	static Float16Maker DENORMn() { return Float16Maker( (UINT16) 0x83FF ); }
	//Creates a positive Denorm
	static Float16Maker DENORMp() { return Float16Maker( (UINT16) 0x03FF ); }
	//Creates a positive FLOAT_MAX
	static Float16Maker FLOATMAXp() { return Float16Maker( (UINT16) 0x73FF ); }
	//Creates a negative FLOAT_MAX
	static Float16Maker FLOATMAXn() { return Float16Maker( (UINT16) 0xF3FF ); }
	
	//Creates a float with all exponent bits set to 1, QNAN, SNAN, etc.
	static Float16Maker MakeSpecialFloat( UINT16 ulMantissa, bool bSignBitOn = false )
	{
		UINT16 ulFLOAT16 = (UINT16) 0x7c00;

		if ( bSignBitOn )
			ulFLOAT16 |= (UINT16) 0x8000;

		ulFLOAT16 |= ( (UINT16) 0x03FF & ulMantissa );

		return Float16Maker( ulFLOAT16 );
	}	

	operator const FLOAT32 () const { return ConvertToFloat32();	}
	operator FLOAT32 () { return ConvertToFloat32();	}
	operator const UINT16&() const { return m_f; }
	operator UINT16&() { return m_f; }
	operator const UINT32() const { return (UINT32) m_f; }
	operator UINT32() { return (UINT32) m_f; }
};
#endif
