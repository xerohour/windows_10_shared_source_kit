/*==========================================================================;
 *
 *  Copyright (C) Microsoft Corporation.  All Rights Reserved.
 *
 *  Content: Graphics device - Resource format helper
 *
 ***************************************************************************/
#pragma once

#include <d3d10_1.h>
#include <cassert>

// ----------------------------------------------------------------------------
// Some enumerations used in the D3D10_FORMAT_DETAIL structure
// ----------------------------------------------------------------------------
typedef enum D3D10_REQUIREMENTS
{
    D3D10R_UND    = 0, // undecided
    D3D10R_NA     = -1, // disallowed or not applicable
    D3D10R_REQ    = 1, // required
    D3D10R_OPT    = -2, // optional
    // Note, 2 bits allocated for this in FORMAT_DETAIL below. If you add fields here, add bits...
    // NOTE SIGNED VALUES ARE USED SINCE COMPILER MAKES ENUMS SIGNED, AND BITFIELDS ARE SIGN EXTENDED ON READ
} D3D10_REQUIREMENTS;

typedef enum D3D10_FORMAT_LAYOUT
{
    D3D10FL_STANDARD = 0, // standard layout
    D3D10FL_CUSTOM   = -1  // custom layout
    // Note, 1 bit allocated for this in FORMAT_DETAIL below. If you add fields here, add bits...
    // NOTE SIGNED VALUES ARE USED SINCE COMPILER MAKES ENUMS SIGNED, AND BITFIELDS ARE SIGN EXTENDED ON READ
} D3D10_FORMAT_LAYOUT;

typedef enum D3D10_FORMAT_TYPE_LEVEL
{
    D3D10FTL_NO_TYPE      = 0,
    D3D10FTL_PARTIAL_TYPE = -2,
    D3D10FTL_FULL_TYPE    = -1,
    // Note, 2 bits allocated for this in FORMAT_DETAIL below. If you add fields here, add bits...
    // NOTE SIGNED VALUES ARE USED SINCE COMPILER MAKES ENUMS SIGNED, AND BITFIELDS ARE SIGN EXTENDED ON READ
} D3D10_FORMAT_TYPE_LEVEL;

typedef enum D3D10_FORMAT_COMPONENT_NAME
{
    D3D10FCN_R     = -4,
    D3D10FCN_G     = -3,
    D3D10FCN_B     = -2,
    D3D10FCN_A     = -1,
    D3D10FCN_D     = 0,
    D3D10FCN_S     = 1,
    D3D10FCN_X     = 2,
    // Note, 3 bits allocated for this in FORMAT_DETAIL below. If you add fields here, add bits...
    // NOTE SIGNED VALUES ARE USED SINCE COMPILER MAKES ENUMS SIGNED, AND BITFIELDS ARE SIGN EXTENDED ON READ
} D3D10_FORMAT_COMPONENT_NAME;

typedef enum D3D10_FORMAT_COMPONENT_INTERPRETATION
{
    D3D10FCI_TYPELESS           = 0,
    D3D10FCI_FLOAT              = -4,
    D3D10FCI_SNORM              = -3,
    D3D10FCI_UNORM              = -2,
    D3D10FCI_SINT               = -1,
    D3D10FCI_UINT               = 1,
    D3D10FCI_UNORM_SRGB         = 2,
    D3D10FCI_BIASED_FIXED_2_8   = 3,
    // Note, 3 bits allocated for this in FORMAT_DETAIL below. If you add fields here, add bits...
    // NOTE SIGNED VALUES ARE USED SINCE COMPILER MAKES ENUMS SIGNED, AND BITFIELDS ARE SIGN EXTENDED ON READ
} D3D10_FORMAT_COMPONENT_INTERPRETATION;

// ----------------------------------------------------------------------------
//
// CD3D10FormatHelper_Test
// Despite the class name, this class represents D3D10.1 level functionality.
//
// ----------------------------------------------------------------------------
class CD3D10FormatHelper_Test
{
private:
    // ----------------------------------------------------------------------------
    // Information describing everything about a D3D10 Resource Format
    // ----------------------------------------------------------------------------
    typedef struct FORMAT_DETAIL
    {
        DXGI_FORMAT                 DXGIFormat;
        UINT8                       BitsPerComponent[4]; // only used for D3D10FTL_PARTIAL_TYPE or FULL_TYPE
        UINT8                       BitsPerElement;
        BOOL                        SRGBFormat : 1;
        UINT                        WidthAlignment : 4;      // number of texels to align to in a mip level.
        UINT                        HeightAlignment : 3;     // Top level dimensions must be a multiple of these
        UINT                        DepthAlignment : 1;      // values.
        D3D10_FORMAT_LAYOUT         Layout : 1;
        D3D10_FORMAT_TYPE_LEVEL     TypeLevel : 2;
        D3D10_FORMAT_COMPONENT_NAME ComponentName0 : 3; // RED    ... only used for D3D10FTL_PARTIAL_TYPE or FULL_TYPE
        D3D10_FORMAT_COMPONENT_NAME ComponentName1 : 3; // GREEN  ... only used for D3D10FTL_PARTIAL_TYPE or FULL_TYPE
        D3D10_FORMAT_COMPONENT_NAME ComponentName2 : 3; // BLUE   ... only used for D3D10FTL_PARTIAL_TYPE or FULL_TYPE
        D3D10_FORMAT_COMPONENT_NAME ComponentName3 : 3; // ALPHA  ... only used for D3D10FTL_PARTIAL_TYPE or FULL_TYPE
        D3D10_FORMAT_COMPONENT_INTERPRETATION ComponentInterpretation0 : 3; // only used for D3D10FTL_FULL_TYPE
        D3D10_FORMAT_COMPONENT_INTERPRETATION ComponentInterpretation1 : 3; // only used for D3D10FTL_FULL_TYPE
        D3D10_FORMAT_COMPONENT_INTERPRETATION ComponentInterpretation2 : 3; // only used for D3D10FTL_FULL_TYPE
        D3D10_FORMAT_COMPONENT_INTERPRETATION ComponentInterpretation3 : 3; // only used for D3D10FTL_FULL_TYPE
        bool                        bDX9VertexOrIndexFormat : 1;
        bool                        bDX9TextureFormat : 1;
		bool                        bFloatNormFormat : 1;
        bool                        bPlanar : 1;
        bool                        bYUV : 1;
    } FORMAT_DETAIL;

    typedef struct FORMAT_REQUIREMENTS
    {
        DXGI_FORMAT                 DXGIFormat;
        DXGI_FORMAT                 ParentFormat;
        DXGI_FORMAT*                pFormatCastSet;
        BOOL                        APIFormatExists : 1;
        D3D10_REQUIREMENTS          Buffer : 2;
        D3D10_REQUIREMENTS          InputAssemblerVertexBuffer : 2;
        D3D10_REQUIREMENTS          InputAssemblerIndexBuffer : 2;
        D3D10_REQUIREMENTS          StreamOutputBuffer : 2;
        D3D10_REQUIREMENTS          Texture1D : 2;
        D3D10_REQUIREMENTS          Texture2D : 2;
        D3D10_REQUIREMENTS          Texture3D : 2;
        D3D10_REQUIREMENTS          TextureCube : 2;
        D3D10_REQUIREMENTS          ld : 2;
        D3D10_REQUIREMENTS          sample : 2;
        D3D10_REQUIREMENTS          sample_c : 2;
        D3D10_REQUIREMENTS          sample_mono : 2;
        D3D10_REQUIREMENTS          gather4 : 2;
        D3D10_REQUIREMENTS          MipMap : 2;
        D3D10_REQUIREMENTS          MipMapAutoGen : 2;
        D3D10_REQUIREMENTS          RenderTarget : 2;
        D3D10_REQUIREMENTS          BlendableRenderTarget : 2;
        D3D10_REQUIREMENTS          DepthStencilTarget : 2;
        D3D10_REQUIREMENTS          CPULockable : 2;
        D3D10_REQUIREMENTS          MultisampleRenderTarget : 2;
        D3D10_REQUIREMENTS          MultisampleLoad : 2;
        D3D10_REQUIREMENTS          MultisampleResolve : 2;
        D3D10_REQUIREMENTS          DisplayScanOut : 2;
        D3D10_REQUIREMENTS          CastWithinBitLayout : 2;
    } FORMAT_REQUIREMENTS;
    static const FORMAT_DETAIL      s_FormatDetail[];
    static const FORMAT_REQUIREMENTS      s_D3D10_FormatRequirements[];
    static const FORMAT_REQUIREMENTS      s_D3D10_BGR_FormatRequirements[];
    static const FORMAT_REQUIREMENTS      s_D3D10_Win7_FormatRequirements[];
    static const FORMAT_REQUIREMENTS      s_D3D10_1_FormatRequirements[];
    static const FORMAT_REQUIREMENTS      s_D3D10_1_Win7_FormatRequirements[];
    static LPCSTR             s_FormatNames[]; // separate from above structure so it can be compiled out of runtime.
    static const UINT               s_NumFormats;

public:
    typedef enum { e_None, e_BGR, e_AllFormatFeatures } eExtendedFormatFeatures;
    static bool CanCastBackBuffers(eExtendedFormatFeatures);
    static DXGI_FORMAT GetFormat(SIZE_T Index, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static bool FormatExists(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static bool APIFormatExists(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static LPCSTR GetName(DXGI_FORMAT Format);
    static bool IsSRGBFormat(DXGI_FORMAT Format);
	static bool Is16BitBGRFormat(DXGI_FORMAT Format);
    static UINT GetBitsPerStencil(DXGI_FORMAT  Format);
    static void GetFormatReturnTypes(
                    DXGI_FORMAT                            Format,
                    D3D10_FORMAT_COMPONENT_INTERPRETATION* pInterpretations ); // return array of 4 components
    static UINT  GetNumComponentsInFormat( DXGI_FORMAT  Format );
    // Converts the sequential component index (range from 0 to GetNumComponentsInFormat()) to
    // the absolute component index (range 0 to 3).
    static UINT  Sequential2AbsoluteComponentIndex(DXGI_FORMAT  Format, UINT SequentialComponentIndex);
    static bool CheckFormatCastability( DXGI_FORMAT a, DXGI_FORMAT b, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features ); // check for casting from a to b or vice versa
    static DXGI_FORMAT                          GetParentFormat(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static DXGI_FORMAT*                         GetFormatCastSet(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static bool                                 CanBeCastEvenFullyTyped(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_FORMAT_LAYOUT                  GetLayout(DXGI_FORMAT Format);
    static D3D10_FORMAT_TYPE_LEVEL              GetTypeLevel(DXGI_FORMAT Format);
    static UINT                                 GetBitsPerElementThrow(DXGI_FORMAT Format);
    static UINT                                 GetBitsPerElement(DXGI_FORMAT Format);
    static UINT                               GetWidthAlignment(DXGI_FORMAT Format);
    static UINT                               GetHeightAlignment(DXGI_FORMAT Format);
    static UINT                               GetDepthAlignment(DXGI_FORMAT Format);
    static D3D10_FORMAT_COMPONENT_NAME          GetComponentName(DXGI_FORMAT Format, UINT AbsoluteComponentIndex);
    static UINT                                 GetBitsPerComponent(DXGI_FORMAT Format, UINT AbsoluteComponentIndex);
    static D3D10_FORMAT_COMPONENT_INTERPRETATION    GetFormatComponentInterpretation(DXGI_FORMAT Format, UINT AbsoluteComponentIndex);
    static D3D10_REQUIREMENTS                   BufferSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   IAVertexBufferSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   IAIndexBufferSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   SOBufferSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   Texture1DSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   Texture2DSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   Texture3DSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   TextureCubeSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   ldSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   sampleSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   sample_cSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   sample_monoSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   gather4Support(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   MipMapSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   MipMapAutogenSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   RenderTargetSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   MultisampleRenderTargetSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   MultisampleLoadSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   BlendableRenderTargetSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   DepthStencilTargetSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   CPULockable(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   MultisampleResolveSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   DisplayScanOutSupport(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static D3D10_REQUIREMENTS                   CastWithinBitLayout(DXGI_FORMAT Format, D3D10_FEATURE_LEVEL1 fl, eExtendedFormatFeatures Features);
    static BOOL                                 DX9VertexOrIndexFormat(DXGI_FORMAT Format);
    static BOOL                                 DX9TextureFormat(DXGI_FORMAT Format);
	static BOOL									FloatNormTextureFormat(DXGI_FORMAT Format);
	static BOOL									Planar(DXGI_FORMAT Format);
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

//=================================================================================================================================
// float11
//
class float11
{
public:
    // Conversion from 32-bit float
    //
    static float11 make(const float* pfvalue) // taking float* rather than copying so that SNaN doesn't get quashed to QNaN in a stack copy.
    {
        UINT u = *(UINT*)pfvalue;
        UINT16 v;
        UINT Sign = (u & 0x80000000);
        UINT MagU = u & 0x7FFFFFFF;     // Absolute value
        if( (u & (0xff<<23)) == (0xff<<23) )
        {
            // INF or NaN
            v = (UINT16)(((1 << m_cExpBits) - 1)<<m_cFracBits);
            if( (u & (~0xff800000)) != 0 )
            {
                // NaN - smash together the fraction bits to
                //       keep the random 1's (in a way that keeps float16->float->float16
                //       conversion invertible down to the bit level, even with NaN).
                v = (UINT16)(v| (((u>>17)|(u>>11)|(u>>6)|(u))&0x0000003f));
            }
            else if( Sign ) // - INF
            {
                v = 0;
            }
        }
        else if( Sign )
        {
            v = 0; // negative (float11 has no sign)
        }
        else if (MagU > m_wMaxNormal)
        {
            // Not representable by 11 bit float -> use flt11_max (due to round to zero)
            v = (UINT16)(((((1 << m_cExpBits) - 2))<<m_cFracBits) | m_FracMask);
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
            v = (UINT16)(MagU>>m_cFracBitsDiff);
        }
        else
        {
            // Normalize value with Round to zero
            v = (UINT16)((MagU + m_BiasDiffo)>>m_cFracBitsDiff);
        }
        float11 val;
        val.v = v;
        return val;
    }

    UINT16 GetBits() {return v;}

    // Conversion to 32-bit float
    //
    // Note: The infinity value (e=31) is converted the same way as any other
    // normalized value
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
                tmp = ((exp + 127) << 23) |     // Exponent
                      (frac << m_cFracBitsDiff);  // Fraction
            }
            else
            {
                // Zero
                tmp = 0;
            }
        }
        else if( exp == m_ExpMask )
        {
            tmp =   0x7f800000 |                                       // Exponent
                  ((v & m_FracMask) << m_cFracBitsDiff);               // Fraction
        }
        else
        {
            tmp = ((((v >> m_cFracBits) & ((1 << m_cExpBits) - 1)) -
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

    static const UINT m_cFracBits = 6;            // Number of fraction bits
    static const UINT m_cExpBits = 5;             // Number of exponent bits
    static const UINT m_FracMask = (1 << m_cFracBits) - 1;         // Fraction mask
    static const UINT m_ExpMask = ((1<< m_cExpBits) - 1) << m_cFracBits; // Exponent mask
    static const INT32  m_cExpBias = (1 << (m_cExpBits - 1)) - 1;     // Exponent bias
    static const UINT m_eMax =  m_cExpBias;         // Max exponent
    static const INT32  m_eMin = -m_cExpBias+1;       // Min exponent
    static const UINT m_wMaxNormal = ((m_eMax+127) << 23) | 0x7E0000;
    static const UINT m_wMinNormal = (m_eMin+127) << 23;
    static const UINT m_BiasDiffo = (m_cExpBias-127) << 23;
    static const UINT m_cFracBitsDiff = 23 - m_cFracBits;

//protected:
    UINT16 v;
};

//=================================================================================================================================
// float10
//
class float10
{
public:
    // Conversion from 32-bit float
    //
    static float10 make(const float* pfvalue) // taking float* rather than copying so that SNaN doesn't get quashed to QNaN in a stack copy.
    {
        UINT u = *(UINT*)pfvalue;
        UINT16 v;
        UINT Sign = (u & 0x80000000);
        UINT MagU = u & 0x7FFFFFFF;     // Absolute value
        if( (u & (0xff<<23)) == (0xff<<23) )
        {
            // INF or NaN
            v = (UINT16)(((1 << m_cExpBits) - 1)<<m_cFracBits);
            if( (u & (~0xff800000)) != 0 )
            {
                // NaN - smash together the fraction bits to
                //       keep the random 1's (in a way that keeps float16->float->float16
                //       conversion invertible down to the bit level, even with NaN).
                v = (UINT16)(v| (((u>>18)|(u>>13)|(u>>8)|(u>>3)|(u))&0x0000001f));
            }
            else if( Sign ) // - INF
            {
                v = 0;
            }
        }
        else if( Sign )
        {
            v = 0; // negative (float10 has no sign)
        }
        else if (MagU > m_wMaxNormal)
        {
            // Not representable by 10 bit float -> use flt10_max (due to round to zero)
            v = (UINT16)(((((1 << m_cExpBits) - 2))<<m_cFracBits) | m_FracMask);
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
            v = (UINT16)(MagU>>m_cFracBitsDiff);
        }
        else
        {
            // Normalize value with Round to zero
            v = (UINT16)((MagU + m_BiasDiffo)>>m_cFracBitsDiff);
        }
        float10 val;
        val.v = v;
        return val;
    }

    UINT16 GetBits() {return v;}

    // Conversion to 32-bit float
    //
    // Note: The infinity value (e=31) is converted the same way as any other
    // normalized value
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
                tmp = ((exp + 127) << 23) |     // Exponent
                      (frac << m_cFracBitsDiff);  // Fraction
            }
            else
            {
                // Zero
                tmp = 0;
            }
        }
        else if( exp == m_ExpMask )
        {
            tmp =   0x7f800000 |                                       // Exponent
                  ((v & m_FracMask) << m_cFracBitsDiff);               // Fraction
        }
        else
        {
            tmp = ((((v >> m_cFracBits) & ((1 << m_cExpBits) - 1)) -
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

    static const UINT m_cFracBits = 5;            // Number of fraction bits
    static const UINT m_cExpBits = 5;             // Number of exponent bits
    static const UINT m_FracMask = (1 << m_cFracBits) - 1;         // Fraction mask
    static const UINT m_ExpMask = ((1<< m_cExpBits) - 1) << m_cFracBits; // Exponent mask
    static const INT32  m_cExpBias = (1 << (m_cExpBits - 1)) - 1;     // Exponent bias
    static const UINT m_eMax =  m_cExpBias;         // Max exponent
    static const INT32  m_eMin = -m_cExpBias+1;       // Min exponent
    static const UINT m_wMaxNormal = ((m_eMax+127) << 23) | 0x7C0000;
    static const UINT m_wMinNormal = (m_eMin+127) << 23;
    static const UINT m_BiasDiffo = (m_cExpBias-127) << 23;
    static const UINT m_cFracBitsDiff = 23 - m_cFracBits;

//protected:
    UINT16 v;
};

inline INT32
ClampSINT( INT32 x, UINT8 uiBits )
{
    assert( uiBits > 1 && uiBits <= 16 );

    const UINT uiMask = UINT( ( UINT64( 0x1 ) << ( uiBits - 1 ) ) - 1 );

    return ( x >= 0 ? min( x, INT32( uiMask ) ) : max( x, INT32( ~uiMask ) ) );
}

inline UINT32
ClampUINT( UINT32 x, UINT8 uiBits )
{
    assert( uiBits > 0 && uiBits < 32 );

    const UINT32 uiMask = UINT32( ( UINT64( 0x1 ) << uiBits ) - 1 );

    return min( x, uiMask );
}

#pragma pack( push, 1 ) // Defining memory layout here:
struct R32G32B32A32_FLOAT
{
    void Pack( float32 rr, float32 gg, float32 bb, float32 aa )
    { r = rr; g = gg; b = bb; a = aa; }
    float32 R() const
    { return r; }
    float32 G() const
    { return g; }
    float32 B() const
    { return b; }
    float32 A() const
    { return a; }

private:
    float32 r, g, b, a;
};

struct R32G32B32A32_SINT
{
    void Pack( INT32 rr, INT32 gg, INT32 bb, INT32 aa )
    { r = rr; g = gg; b = bb; a = aa; }
    INT32 R() const
    { return r; }
    INT32 G() const
    { return g; }
    INT32 B() const
    { return b; }
    INT32 A() const
    { return a; }

private:
    INT32 r, g, b, a;
};

struct R32G32B32A32_UINT
{
    void Pack( UINT rr, UINT gg, UINT bb, UINT aa )
    { r = rr; g = gg; b = bb; a = aa; }
    UINT R() const
    { return r; }
    UINT G() const
    { return g; }
    UINT B() const
    { return b; }
    UINT A() const
    { return a; }

private:
    UINT r, g, b, a;
};

struct R32G32B32_FLOAT
{
    void Pack( float32 rr, float32 gg, float32 bb )
    { r = rr; g = gg; b = bb; }
    float32 R() const
    { return r; }
    float32 G() const
    { return g; }
    float32 B() const
    { return b; }

private:
    float32 r, g, b;
};

struct R32G32B32_SINT
{
    void Pack( INT32 rr, INT32 gg, INT32 bb )
    { r = rr; g = gg; b = bb; }
    INT32 R() const
    { return r; }
    INT32 G() const
    { return g; }
    INT32 B() const
    { return b; }

private:
    INT32 r, g, b;
};

struct R32G32B32_UINT
{
    void Pack( UINT rr, UINT gg, UINT bb )
    { r = rr; g = gg; b = bb; }
    UINT R() const
    { return r; }
    UINT G() const
    { return g; }
    UINT B() const
    { return b; }

private:
    UINT r, g, b;
};

struct R16G16B16A16_FLOAT
{
    void Pack( float16 rr, float16 gg, float16 bb, float16 aa )
    {
        r = *reinterpret_cast< UINT16* >( &rr );
        g = *reinterpret_cast< UINT16* >( &gg );
        b = *reinterpret_cast< UINT16* >( &bb );
        a = *reinterpret_cast< UINT16* >( &aa );
    }
    float16 R() const
    { return *reinterpret_cast< const float16* >( &r ); }
    float16 G() const
    { return *reinterpret_cast< const float16* >( &g ); }
    float16 B() const
    { return *reinterpret_cast< const float16* >( &b ); }
    float16 A() const
    { return *reinterpret_cast< const float16* >( &a ); }

private:
    UINT16 r, g, b, a;
};

struct R16G16B16A16_SINT
{
    void Pack( INT32 rr, INT32 gg, INT32 bb, INT32 aa )
    {
        r = INT16( ClampSINT( rr, 16 ) ); g = INT16( ClampSINT( gg, 16 ) );
        b = INT16( ClampSINT( bb, 16 ) ); a = INT16( ClampSINT( aa, 16 ) );
    }
    INT32 R() const
    { return r; }
    INT32 G() const
    { return g; }
    INT32 B() const
    { return b; }
    INT32 A() const
    { return a; }

private:
    INT16 r, g, b, a;
};

struct R16G16B16A16_UINT
{
    void Pack( UINT rr, UINT gg, UINT bb, UINT aa )
    {
        r = UINT16( ClampUINT( rr, 16 ) ); g = UINT16( ClampUINT( gg, 16 ) );
        b = UINT16( ClampUINT( bb, 16 ) ); a = UINT16( ClampUINT( aa, 16 ) );
    }
    UINT R() const
    { return r; }
    UINT G() const
    { return g; }
    UINT B() const
    { return b; }
    UINT A() const
    { return a; }

private:
    UINT16 r, g, b, a;
};

struct R32G32_FLOAT
{
    void Pack( float32 rr, float32 gg )
    { r = rr; g = gg; }
    float32 R() const
    { return r; }
    float32 G() const
    { return g; }

private:
    float32 r, g;
};

struct R32G32_SINT
{
    void Pack( INT32 rr, INT32 gg )
    { r = rr; g = gg; }
    INT32 R() const
    { return r; }
    INT32 G() const
    { return g; }

private:
    INT32 r, g;
};

struct R32G32_UINT
{
    void Pack( UINT rr, UINT gg )
    { r = rr; g = gg; }
    UINT R() const
    { return r; }
    UINT G() const
    { return g; }

private:
    UINT r, g;
};

struct R32X8X24_FLOAT
{
	void Pack( float32 rr )
	{ r = rr; }
    float32 R() const
    { return r; }

private:
    float32 r;
    UINT8 x8;
    UINT8 X1X;
    UINT16 X2X;
};

struct R32X8X24_UINT
{
	void Pack( UINT32 rr )
	{ r = rr; }
	UINT32 R() const
	{ return r; }

private:
	UINT32 r;
	UINT8 x8;
	UINT8 X1X;
	UINT16 X2X;
};

struct X32G8X24_UINT
{
	void Pack( UINT8 gg )
	{ g = gg; }
    UINT G() const
    { return g; }

private:
    UINT x32;
    UINT8 g;
    UINT8 X1X;
    UINT16 X2X;
};

struct D32_FLOAT_S8X24_UINT
{
    void Pack( float32 dd, UINT ss )
    { d = dd; s = UINT8( ss ); }
    float32 R() const
    { return d; }
    UINT G() const
    { return s; }

private:
    float32 d;
    UINT8 s;
    UINT8 X1X;
    UINT16 X2X;
};

struct R16G16B16_FLOAT
{
    void Pack( float16 rr, float16 gg, float16 bb )
    {
        r = *reinterpret_cast< UINT16* >( &rr );
        g = *reinterpret_cast< UINT16* >( &gg );
        b = *reinterpret_cast< UINT16* >( &bb );
    }
    float16 R() const
    { return *reinterpret_cast< const float16* >( &r ); }
    float16 G() const
    { return *reinterpret_cast< const float16* >( &g ); }
    float16 B() const
    { return *reinterpret_cast< const float16* >( &b ); }

private:
    UINT16 r, g, b;
};

struct R16G16B16_SINT
{
    void Pack( INT32 rr, INT32 gg, INT32 bb )
    {
        r = INT16( ClampSINT( rr, 16 ) ); g = INT16( ClampSINT( gg, 16 ) );
        b = INT16( ClampSINT( bb, 16 ) );
    }
    INT32 R() const
    { return r; }
    INT32 G() const
    { return g; }
    INT32 B() const
    { return b; }

private:
    INT16 r, g, b;
};

struct R16G16B16_UINT
{
    void Pack( UINT rr, UINT gg, UINT bb )
    {
        r = UINT16( ClampUINT( rr, 16 ) ); g = UINT16( ClampUINT( gg, 16 ) );
        b = UINT16( ClampUINT( bb, 16 ) );
    }
    UINT R() const
    { return r; }
    UINT G() const
    { return g; }
    UINT B() const
    { return b; }

private:
    UINT16 r, g, b;
};

struct R8G8B8A8_SINT
{
    void Pack( INT32 rr, INT32 gg, INT32 bb, INT32 aa )
    {
        r = INT8( ClampSINT( rr, 8 ) ); g = INT8( ClampSINT( gg, 8 ) );
        b = INT8( ClampSINT( bb, 8 ) ); a = INT8( ClampSINT( aa, 8 ) );
    }
    INT32 R() const
    { return r; }
    INT32 G() const
    { return g; }
    INT32 B() const
    { return b; }
    INT32 A() const
    { return a; }

private:
    INT8 r, g, b, a;
};

struct R8G8B8A8_UINT
{
    void Pack( UINT rr, UINT gg, UINT bb, UINT aa )
    {
        r = UINT8( ClampUINT( rr, 8 ) ); g = UINT8( ClampUINT( gg, 8 ) );
        b = UINT8( ClampUINT( bb, 8 ) ); a = UINT8( ClampUINT( aa, 8 ) );
    }
    UINT R() const
    { return r; }
    UINT G() const
    { return g; }
    UINT B() const
    { return b; }
    UINT A() const
    { return a; }

private:
    UINT8 r, g, b, a;
};

struct B8G8R8A8_UINT
{
    void Pack( UINT rr, UINT gg, UINT bb, UINT aa )
    {
        r = UINT8( ClampUINT( rr, 8 ) ); g = UINT8( ClampUINT( gg, 8 ) );
        b = UINT8( ClampUINT( bb, 8 ) ); a = UINT8( ClampUINT( aa, 8 ) );
    }
    UINT R() const
    { return r; }
    UINT G() const
    { return g; }
    UINT B() const
    { return b; }
    UINT A() const
    { return a; }

private:
    UINT8 b, g, r, a;
};

struct B5G6R5_UINT
{
    void Pack( UINT rr, UINT gg, UINT bb )
    {
        v = UINT16( ClampUINT( rr, 5 ) << 11 ) |
            UINT16( ClampUINT( gg, 6 ) <<  5 ) |
            UINT16( ClampUINT( bb, 5 ) <<  0 );
    }
    UINT R() const
    { return ( v >> 11 ) & 0x1F; }
    UINT G() const
    { return ( v >>  5 ) & 0x3F; }
    UINT B() const
    { return ( v >>  0 ) & 0x1F; }

private:
    UINT16 v;
};

struct B5G5R5A1_UINT
{
    void Pack( UINT rr, UINT gg, UINT bb, UINT aa )
    {
        v = ( ClampUINT( rr, 5 ) << 10 ) |
            ( ClampUINT( gg, 5 ) <<  5 ) |
            ( ClampUINT( bb, 5 ) <<  0 ) |
            ( ClampUINT( aa, 1 ) << 15 );
    }
    UINT R() const
    { return ( v >> 10 ) & 0x1F; }
    UINT G() const
    { return ( v >>  5 ) & 0x1F; }
    UINT B() const
    { return ( v >>  0 ) & 0x1F; }
    UINT A() const
    { return ( v >> 15 ) & 0x1;  }

private:
    UINT16 v;
};

struct B4G4R4A4_UINT
{
    void Pack( UINT rr, UINT gg, UINT bb, UINT aa )
    {
        v = ( ClampUINT( rr, 4 ) <<  8 ) |
            ( ClampUINT( gg, 4 ) <<  4 ) |
            ( ClampUINT( bb, 4 ) <<  0 ) |
            ( ClampUINT( aa, 4 ) << 12 );
    }
    UINT R() const
    { return ( v >>  8 ) & 0xF; }
    UINT G() const
    { return ( v >>  4 ) & 0xF; }
    UINT B() const
    { return ( v >>  0 ) & 0xF; }
    UINT A() const
    { return ( v >> 12 ) & 0xF; }

private:
    UINT16 v;
};

struct R16G16_FLOAT
{
    void Pack( float16 rr, float16 gg )
    {
        r = *reinterpret_cast< UINT16* >( &rr );
        g = *reinterpret_cast< UINT16* >( &gg );
    }
    float16 R() const
    { return *reinterpret_cast< const float16* >( &r ); }
    float16 G() const
    { return *reinterpret_cast< const float16* >( &g ); }

private:
    UINT16 r, g;
};

struct R16G16_SINT
{
    void Pack( INT32 rr, INT32 gg )
    { r = INT16( ClampSINT( rr, 16 ) ); g = INT16( ClampSINT( gg, 16 ) ); }
    INT32 R() const
    { return r; }
    INT32 G() const
    { return g; }

private:
    INT16 r, g;
};

struct R16G16_UINT
{
    void Pack( UINT rr, UINT gg )
    { r = UINT16( ClampUINT( rr, 16 ) ); g = UINT16( ClampUINT( gg, 16 ) ); }
    UINT R() const
    { return r; }
    UINT G() const
    { return g; }

private:
    UINT16 r, g;
};

struct R8G8_SINT
{
    void Pack( INT32 rr, INT32 gg )
    { r = INT8( ClampSINT( rr, 8 ) ); g = INT8( ClampSINT( gg, 8 ) ); }
    INT32 R() const
    { return r; }
    INT32 G() const
    { return g; }

private:
    INT8 r, g;
};

struct R8G8_UINT
{
    void Pack( UINT rr, UINT gg )
    { r = UINT8( ClampUINT( rr, 8 ) ); g = UINT8( ClampUINT( gg, 8 ) ); }
    UINT R() const
    { return r; }
    UINT G() const
    { return g; }

private:
    UINT8 r, g;
};

struct R10G10B10A2_UINT
{
    void Pack( UINT rr, UINT gg, UINT bb, UINT aa )
    {
        v = ( ClampUINT( aa,  2 ) << 30 ) |
            ( ClampUINT( bb, 10 ) << 20 ) |
            ( ClampUINT( gg, 10 ) << 10 ) |
            ( ClampUINT( rr, 10 ) <<  0 );
    }
    UINT R() const
    { return ( v >>  0 ) & 0x3FF; }
    UINT G() const
    { return ( v >> 10 ) & 0x3FF; }
    UINT B() const
    { return ( v >> 20 ) & 0x3FF; }
    UINT A() const
    { return ( v >> 30 ) & 0x3; }

private:
    UINT v;
};

struct R10G10B10_SINT_A2_UINT
{
    void Pack( INT32 rr, INT32 gg, INT32 bb, UINT aa )
    {
        v = ( ClampUINT( aa, 2 ) << 30 ) |
            ( UINT( ClampSINT( bb, 10 ) & 0x3FF ) << 20 ) |
            ( UINT( ClampSINT( gg, 10 ) & 0x3FF ) << 10 ) |
            ( UINT( ClampSINT( rr, 10 ) & 0x3FF ) <<  0 );
    }
    INT32 R() const
    { return INT16( v << 6 ) >> (  6 +  0 ); }
    INT32 G() const
    { return INT16( v >> 4 ) >> ( -4 + 10 ); }
    INT32 B() const
    { return INT32( v << 2 ) >> (  2 + 20 ); }
    UINT A() const
    { return ( v >> 30 ) & 0x3; }

private:
    UINT v;
};

struct R11G11B10_FLOAT
{
    void Pack( float11 rr, float11 gg, float10 bb )
    {
        v = ( ( bb.GetBits() & 0x3FF ) << 22 ) |
            ( ( gg.GetBits() & 0x7FF ) << 11 ) |
            ( ( rr.GetBits() & 0x7FF ) <<  0 );
    }
    float11 R() const
    { UINT16 rr = (( v >>  0 ) & 0x7FF); return *reinterpret_cast< const float11* >(&rr); }
    float11 G() const
    { UINT16 gg = (( v >>  11 ) & 0x7FF); return *reinterpret_cast< const float11* >(&gg); }
    float10 B() const
    { UINT16 bb = (( v >>  22 ) & 0x3FF); return *reinterpret_cast< const float10* >(&bb); }

private:
    UINT v;
};

struct R9G9B9E5_UINT
{
    void Pack( UINT rr, UINT gg, UINT bb, UINT ee )
    {
        v = ( ClampUINT( ee, 5 ) << 27 ) |
            ( ClampUINT( bb, 9 ) << 18 ) |
            ( ClampUINT( gg, 9 ) <<  9 ) |
            ( ClampUINT( rr, 9 ) <<  0 );
    }
    UINT R() const
    { return ( v >>  0 ) & 0x1FF; }
    UINT G() const
    { return ( v >>  9 ) & 0x1FF; }
    UINT B() const
    { return ( v >> 18 ) & 0x1FF; }
    UINT E() const
    { return ( v >> 27 ) &  0x1F; }

private:
    UINT v;
};

struct R32_FLOAT
{
    void Pack( float32 rr )
    { r = rr; }
    float32 R() const
    { return r; }

private:
    float32 r;
};

struct R32_SINT
{
    void Pack( INT32 rr )
    { r = rr; }
    INT32 R() const
    { return r; }

private:
    INT32 r;
};

struct R32_UINT
{
    void Pack( UINT rr )
    { r = rr; }
    UINT R() const
    { return r; }

private:
    UINT r;
};

struct R24X8_UINT
{
	void Pack( UINT r24 )
	{ v = ClampUINT(r24, 24); }
    UINT R() const
    { return ( v >>  0 ) & 0xFFFFFF; }

private:
    UINT v;
};

struct X24G8_UINT
{
	void Pack( UINT8 gg )
	{ v = ClampUINT(gg, 8) << 24; }
    UINT G() const
    { return ( v >> 24 ) &     0xFF; }

private:
    UINT v;
};

struct D24S8_UINT
{
    void Pack( UINT dd, UINT ss )
    {
        v = ( ( dd & 0xFFFFFF ) << 0 ) |
            ( ( ss & 0xFF ) << 24 );
    }
    UINT R() const
    { return ( v >>  0 ) & 0xFFFFFF; }
    UINT G() const
    { return ( v >> 24 ) &     0xFF; }

private:
    UINT v;
};

struct R16_FLOAT
{
    void Pack( float16 rr )
    { r = *reinterpret_cast< UINT16* >( &rr ); }
    float16 R() const
    { return *reinterpret_cast< const float16* >( &r ); }
private:
    UINT16 r;
};

struct R16_SINT
{
    void Pack( INT32 rr )
    { r = INT16( ClampSINT( rr, 16 ) ); }
    INT32 R() const
    { return r; }

private:
    INT16 r;
};

struct R16_UINT
{
    void Pack( UINT rr )
    { r = UINT16( ClampUINT( rr, 16 ) ); }
    UINT R() const
    { return r; }

private:
    UINT16 r;
};

struct D16_UINT
{
    void Pack( UINT dd )
    { d = UINT16( dd ); }
    UINT R() const
    { return d; }

private:
    UINT16 d;
};

struct R8_SINT
{
    void Pack( INT32 rr )
    { r = INT8( ClampSINT( rr, 8 ) ); }
    INT32 R() const
    { return r; }

private:
    INT8 r;
};

struct R8_UINT
{
    void Pack( UINT rr )
    { r = UINT8( ClampUINT( rr, 8 ) );  }
    UINT R() const
    { return r; }

private:
    UINT8 r;
};

struct A8_UINT
{
    void Pack( UINT aa )
    { a = UINT8( ClampUINT( aa, 8 ) );  }
    UINT A() const
    { return a; }

private:
    UINT8 a;
};


////////////////////////////////////////////////////////////////////////////////
//
// Typeless
//
////////////////////////////////////////////////////////////////////////////////
union R32G32B32A32_TYPELESS
{
    R32G32B32A32_FLOAT FLOAT;
    R32G32B32A32_SINT SINT;
    R32G32B32A32_UINT UINT;
};

union R32G32B32_TYPELESS
{
    R32G32B32_FLOAT FLOAT;
    R32G32B32_SINT SINT;
    R32G32B32_UINT UINT;
};

union R16G16B16A16_TYPELESS
{
    R16G16B16A16_FLOAT FLOAT;
    R16G16B16A16_SINT SINT;
    R16G16B16A16_UINT UINT;
};

union R32G32_TYPELESS
{
    R32G32_FLOAT FLOAT;
    R32G32_SINT SINT;
    R32G32_UINT UINT;
};

union R32G8X24_TYPELESS
{
    R32X8X24_FLOAT R32_FLOAT;
    X32G8X24_UINT G8_UINT;
    D32_FLOAT_S8X24_UINT D32_FLOAT_S8_UINT;
};

union R16G16B16_TYPELESS
{
    R16G16B16_FLOAT FLOAT;
    R16G16B16_SINT SINT;
    R16G16B16_UINT UINT;
};

union R10G10B10A2_TYPELESS
{
    R10G10B10A2_UINT UINT;
    R10G10B10_SINT_A2_UINT SINT_A2_UINT;
};

union R8G8B8A8_TYPELESS
{
    R8G8B8A8_SINT SINT;
    R8G8B8A8_UINT UINT;
};

union R16G16_TYPELESS
{
    R16G16_FLOAT FLOAT;
    R16G16_SINT SINT;
    R16G16_UINT UINT;
};

union R32_TYPELESS
{
    R32_FLOAT FLOAT;
    R32_SINT SINT;
    R32_UINT UINT;
};

union R24G8_TYPELESS
{
    R24X8_UINT R24_UINT;
    X24G8_UINT G8_UINT;
    D24S8_UINT D24S8_UINT;
};

union R8G8_TYPELESS
{
    R8G8_SINT SINT;
    R8G8_UINT UINT;
};

union R16_TYPELESS
{
    R16_FLOAT FLOAT;
    R16_SINT SINT;
    R16_UINT UINT;
    D16_UINT D16_UINT;
};

union R8_TYPELESS
{
    R8_SINT SINT;
    R8_UINT UINT;
};

union A8_TYPELESS
{
    A8_UINT UINT;
};

#pragma pack( pop )

// End of file
