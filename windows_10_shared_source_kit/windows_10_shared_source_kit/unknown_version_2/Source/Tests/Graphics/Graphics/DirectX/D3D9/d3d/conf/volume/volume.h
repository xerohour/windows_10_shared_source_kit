static BOOL									YUV(DXGI_FORMAT Format);
protected:
    static UINT GetNumFormats(D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static UINT GetDetailTableIndex(DXGI_FORMAT  Format);
    static UINT GetDetailTableIndexThrow(DXGI_FORMAT  Format);
    static UINT GetDetailTableIndexNoThrow(DXGI_FORMAT  Format);
    static const FORMAT_DETAIL* GetFormatDetail( DXGI_FORMAT  Format );
    static const FORMAT_REQUIREMENTS* GetRequirementsTable( D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features );
};


typedef float float32;

//=================================================================================================================================
// float16
//
class float16
{
public:
    // Conversion from 32-bit float
    //
    static float16 make(const float* pfvalue) // taking float* rather than copying so that SNaN doesn't get quashed to QNaN in a stack copy.
    {
        UINT u = *(UINT*)pfvalue;
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
        float16 val;
        val.v = v;
        return val;
    }

    // Conversion to 32-bit float
    //
    //
    UINT toFloat32_AsUINT32Bits() const
    {
        UINT tmp;
        UINT exp = v & m_ExpMask;
        if( exp == 0 )
        {
            if ((v & m_FracMask) != 0)
            {
                // Normalizing the denormalized value
                exp = m_eMin;
                UINT frac = v & m_FracMask;
                while ((frac & (m_FracMask + 1)) == 0)
                {
                    exp--;
                    frac <<= 1;
                }
                frac &= ~(m_FracMask + 1); // Remove hidden bit
                tmp = ((v & m_SignMask) << 16) | // Sigh bit
                      ((exp + 127) << 23) |     // Exponent
                      (frac << m_cFracBitsDiff);  // Fraction
            }
            else
            {
                // Zero - only sign bit is used
                tmp = (v & m_SignMask) << 16;
            }
        }
        else if( exp == m_ExpMask )
        {
            tmp = ((v & m_SignMask) << 16) |                           // Sign bit
                    0x7f800000 |                                       // Exponent
                  ((v & m_FracMask) << m_cFracBitsDiff);               // Fraction
        }
        else
        {
            tmp = ((v & m_SignMask) << 16) |                         // Sign bit
                  ((((v >> m_cFracBits) & ((1 << m_cExpBits) - 1)) -
                    m_cExpBias + 127) << 23) |                        // Exponent
                  ((v & m_FracMask) << m_cFracBitsDiff);               // Fraction
        }
        return tmp;
    }

    operator float32() const
    {
        UINT tmp = toFloat32_AsUINT32Bits();
        return *(float32*)&tmp;
    }

	UINT16 GetBits() {return v;}

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

//protected:
    UINT16 v;
};

//======================================================================================================