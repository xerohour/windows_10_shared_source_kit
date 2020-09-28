#ifndef _FSURFACE_H_
#define _FSURFACE_H_

#include <math.h>
#include "FrameworkTypes.h"


#define D3DRGBA(r, g, b, a) \
    (   (((long)((a) * 255)) << 24) | (((long)((r) * 255)) << 16) \
    |   (((long)((g) * 255)) << 8) | (long)((b) * 255) \
    )
#define MAX(a,b)  (((a) > (b)) ? (a) : (b))
#define MIN(a,b)  (((a) < (b)) ? (a) : (b))
#define ABS(a) (((a) < 0) ? (-(a)) : (a))

typedef enum _ELEMENTINDEX
{
    ELEMENTINDEX_0       = 0,
    ELEMENTINDEX_1       = 1,
    ELEMENTINDEX_2       = 2,
    ELEMENTINDEX_3       = 3,
    ELEMENTINDEX_DEFAULT = 0xffffffff // for don't care cases
} ELEMENTINDEX;

static const UINT cFracBits = 10;           // Number of fraction bits
static const UINT cExpBits = 5;             // Number of exponent bits
static const UINT cSignBit = 15;            // Index of the sign bit
static const UINT cSignMask = (1 << cSignBit);  
static const UINT cFracMask = (1 << cFracBits) - 1;         // Fraction mask
static const int  cExpBias = (1 << (cExpBits - 1)) - 1;     // Exponent bias 
static const UINT cRoundBit = 1 << (23 - cFracBits - 1);    // Bit to add for rounding 
static const UINT eMax =  cExpBias;         // Max exponent 
static const int  eMin = -cExpBias+1;       // Min exponent 
static const UINT wMaxNormal = ((eMax+127) << 23) | 0x7FEFFF;//  <-max nbr that doesnt round to infinity
static const UINT wMinNormal = (eMin+127) << 23;
static const UINT BiasDiffo = (cExpBias-127) << 23;
static const UINT cFracBitsDiff = 23 - cFracBits;

//-----------------------------------------------------------------------------
// CFLoat16 - Taken from REF.  Manages 16bit float values

class CFloat16
{
public:
    CFloat16()
    {
        v = 0;
    }
    // Conversion from 32-bit float
    //
    CFloat16(float fvalue)
    {
        UINT u = *(DWORD*)&fvalue;
        UINT Sign = (u & 0x80000000) >> 16;
    	UINT MagU = u & 0x7FFFFFFF;     // Absolute value
        if (MagU > wMaxNormal)       
        {
            // Not representable by 16 bit float
            v = (WORD)(Sign | 0x7fff);
        }
        else
	    if (MagU < wMinNormal) 
        {
            // Denormalized value

            // Make implicit 1 explicit
	        UINT Frac = (MagU & ((1<<23)-1)) | (1<<23);   
		    int nshift = (eMin + 127 - (MagU >> 23));
		    MagU = Frac >> nshift;
            // Round to nearest even
	        v = (WORD)(Sign | ((MagU + (cRoundBit-1) + 
                       ((MagU >> cFracBitsDiff) & 1)) >> cFracBitsDiff)); 
	    }
        else
        {
            // Normalized value with Round to nearest even
	        v = (WORD)(Sign | ((MagU + BiasDiffo + (cRoundBit-1) + 
                       ((MagU >> cFracBitsDiff) & 1)) >> cFracBitsDiff)); 
        }
    }

    // Conversion to 32-bit float
    //
    // Note: The infinity value (e=31) is converted the same way as any other
    // normalized value
    //
    operator float()
    {
        DWORD tmp;
        if ((v & ~(cSignMask | cFracMask)) == 0)
        {
            if ((v & cFracMask) != 0)
            {
                // Normalizing the denormalized value
                UINT exp = eMin;
                UINT frac = v & cFracMask;
                while ((frac & (cFracMask + 1)) == 0)
                {
                    exp--;
                    frac <<= 1;
                }
                frac &= ~(cFracMask + 1); // Remove hidden bit
                tmp = ((v & cSignMask) << 16) | // Sigh bit
                      ((exp + 127) << 23) |     // Exponent
                      (frac << cFracBitsDiff);  // Fraction
            }
            else
            {
                // Zero - only sign bit is used
                tmp = (v & cSignMask) << 16;
            }
        }
        else
        {
            tmp = ((v & cSignMask) << 16) |                         // Sigh bit
                  ((((v >> cFracBits) & ((1 << cExpBits) - 1)) -
                    cExpBias + 127) << 23) |                        // Exponent
                  ((v & cFracMask) << cFracBitsDiff);               // Fraction
        }
        return *(float*)&tmp;
    }

public: 

    // NOTE: No need for this to be protected. memcpy can defeat it. Direct assignment required
    // for 16 bit float texture reading. Public simplifies this.

    WORD  v;
};

//-----------------------------------------------------------------------------
// macros for converting n-bit signed integers to floats clamped to [-1.0, 1.0]
//
// e.g. For an 8 bit number, if it is -128, it gets clamped to -127.
//      Then the number is divided by 127.
//
//-----------------------------------------------------------------------------

inline FLOAT CLAMP_SIGNED16(INT16 i)
{
    return (-32768 == i ? -1.f : (FLOAT)i/32767.f);
}

inline FLOAT CLAMP_SIGNED11(INT16 i) //only looks at bottom 11 bits
{
    // sign extend to 16 bits
    i <<= 5; i >>= 5;
    return (-1024 == i ? -1.f : (FLOAT)i/1023.f);
}

inline FLOAT CLAMP_SIGNED10(INT16 i) //only looks at bottom 10 bits
{
    // sign extend to 16 bits
    i <<= 6; i >>= 6;
    return (-512 == i ? -1.f : (FLOAT)i/511.f);
}

inline FLOAT CLAMP_SIGNED8(INT8 i)
{
    return (-128 == i ? -1.f : (FLOAT)i/127.f);
}

inline FLOAT CLAMP_SIGNED6(INT8 i) //only looks at bottom 6 bits
{
    // sign extend to 8 bits
    i <<= 2; i >>= 2;
    return (-32 == i ? -1.f : (FLOAT)i/31.f);
}

inline FLOAT CLAMP_SIGNED5(INT8 i) //only looks at bottom 5 bits
{
    // sign extend to 8 bits
    i <<= 3; i >>= 3;
    return (-16 == i ? -1.f : (FLOAT)i/15.f);
}

inline FLOAT CLAMP_SIGNED4(INT8 i)  //only looks at bottom 4 bits
{
    // sign extend to 8 bits
    i <<= 4; i >>= 4;
    return (-8 == i ? -1.f : (FLOAT)i/7.f);
}

//-------------------------------------------------------------------------
// Gamma Correction
//-------------------------------------------------------------------------
inline FLOAT UndoGammaForChannel( FLOAT c )
{
    if( c <= 0.03928 )     
    {                      
        return (FLOAT)(c / 12.92);     
    }                      
    else                   
    {                      
        return (FLOAT)pow( (c + 0.055)/1.055, 2.4 );
    }                                            
}

inline FLOAT GammaCorrectChannel( FLOAT c )
{
    if( c <= 0.00304f )     
    {                      
        return (FLOAT)(c * 12.92f);     
    }                      
    else                   
    {                      
        return (FLOAT)(pow( c , 1.0f/2.4f )*1.055f - 0.055f);
    }                                            
}

//-----------------------------------------------------------------------------
// FColor - Single float-based color value
//

class FColor
{
public:
	union
	{
		FLOAT R;
		FLOAT L;
	};
	union
	{
		FLOAT G;
		FLOAT DV;
	};
	union
	{
		FLOAT B;
		FLOAT DU;
	};	
	union
	{
		FLOAT A;
	};	
	static const FLOAT fDelta;

    inline FColor( void ) { R = G = B = 0.0f; A = 1.0f; }
    // assignment constructors
    inline FColor( UINT32 uVal )
    {
        R = (FLOAT)RGBA_GETRED(   uVal )*(1.f/255.f);
        G = (FLOAT)RGBA_GETGREEN( uVal )*(1.f/255.f);
        B = (FLOAT)RGBA_GETBLUE(  uVal )*(1.f/255.f);
        A = (FLOAT)RGBA_GETALPHA( uVal )*(1.f/255.f);
    }

    inline FColor(BYTE r,  BYTE g,  BYTE b,  BYTE a)
    {
        R = FLOAT(r)/255.f;
        G = FLOAT(g)/255.f;
        B = FLOAT(b)/255.f;
        A = FLOAT(a)/255.f;
    }
    inline FColor( FLOAT fR, FLOAT fG, FLOAT fB, FLOAT fA )
    {
        R = fR; G = fG; B = fB; A = fA;
    }
    inline FColor( FLOAT* pC )
    {
        R = *(pC+0); G = *(pC+1); B= *(pC+2); A = *(pC+3);
    }

    // UINT32 copy operator
    inline void operator=(const UINT32 uVal)
    {
        R = (FLOAT)RGBA_GETRED(   uVal )*(1.f/255.f);
        G = (FLOAT)RGBA_GETGREEN( uVal )*(1.f/255.f);
        B = (FLOAT)RGBA_GETBLUE(  uVal )*(1.f/255.f);
        A = (FLOAT)RGBA_GETALPHA( uVal )*(1.f/255.f);
    }
    // FLOAT array copy operator
    inline void operator=(const FLOAT* pFVal)
    {
        R = *(pFVal+0);
        G = *(pFVal+1);
        B = *(pFVal+2);
        A = *(pFVal+3);
    }
	// Equality operator
    inline bool operator==(const FColor& FVal)
    {
		return (fabs(R - FVal.R) < fDelta &&
				fabs(G - FVal.G) < fDelta &&
				fabs(B - FVal.B) < fDelta &&
				fabs(A - FVal.A) < fDelta);
    }
    // casting operator
    inline operator UINT32() const
    {
        return D3DRGBA( R, G, B, A );
    }

    // set all channels
    inline void SetAllChannels( FLOAT fVal )
    {
        R = fVal; G = fVal; B = fVal; A = fVal;
    }

    // clamp to unity
    inline void Saturate( void )
    {
        R = MIN( 1.f, R );
        G = MIN( 1.f, G );
        B = MIN( 1.f, B );
        A = MIN( 1.f, A );
    }

    inline void Clamp( void )
    {
        R = MAX( 0.f, MIN( 1.f, R ) );
        G = MAX( 0.f, MIN( 1.f, G ) );
        B = MAX( 0.f, MIN( 1.f, B ) );
        A = MAX( 0.f, MIN( 1.f, A ) );
    }

    inline void Complement( void )
    {
        R = 1.0f - R;
        G = 1.0f - G;
        B = 1.0f - B;
        A = 1.0f - A;
    }

    // copy to array of FLOATs
    inline void CopyTo( FLOAT* pF )
    {
        *(pF+0) = R;
        *(pF+1) = G;
        *(pF+2) = B;
        *(pF+3) = A;
    }

    // Binary Subtraction
    inline FColor operator - (const FColor& c) const {
	return FColor(R - c.R, G - c.G, B - c.B, A - c.A);
    }

    //
    // conversions between surface format and FColor - these define the
    // correct way to map between resolutions
    //

    // The colors stored in FColor are assumed to be in Physically Linear space (Gamma = 1.0)
    // The conversion functions can optionally perform the gamma correction for Gamma = 2.2.

    // convert from surface type format to FColor
    void ConvertFrom(   FMT Type, 
                        const char* pSurfaceBits, 
                        BOOL bGammaCorrect, 
                        ELEMENTINDEX ElementIndex );

    // Convert surface type format to FColor
    void ConvertTo( FMT Type, 
                    float fRoundOffset, 
                    char* pSurfaceBits, 
                    BOOL bGammaCorrect, 
                    ELEMENTINDEX ElementIndex ) const;
    
    // Converts from Gamma2.2 space to Gamma1.0 space
    void UndoGammaCorrection();
};

//-----------------------------------------------------------------------------
// FSurface - Surface of float-based colors
//

class FSurface
{
	FColor *m_pData;
	UINT	m_uWidth, m_uHeight;
	FMT     m_fromFmt;

public:
	inline FSurface() { m_pData = NULL; m_uWidth = 0; m_uHeight = 0; m_fromFmt = FMT_A8R8G8B8;};
	FSurface(CSurface *);
	~FSurface();

	virtual HRESULT ConvertTo(CSurface *pSurface);
	virtual HRESULT ConvertTo(CSurface *pSurface, DWORD filter);
	virtual HRESULT ConvertFrom(CSurface *pSurface);
	virtual HRESULT ConvertFrom(CSurface *pSurface, DWORD filter);
	virtual HRESULT SetSize(UINT Width, UINT Height);

	inline	UINT GetWidth() { return m_uWidth; }
	inline	UINT GetHeight() { return m_uHeight; }
	inline  FColor* GetData() { return m_pData; }

};

#endif
