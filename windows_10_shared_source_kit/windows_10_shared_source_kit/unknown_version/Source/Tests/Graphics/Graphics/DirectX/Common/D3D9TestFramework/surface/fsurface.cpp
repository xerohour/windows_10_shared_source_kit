#include "SurfacePCH.h"
#include "FSurface.h"

// -------------------------------------------------------------------------------------
// FColor class
//
// Defines a float based color class that has one 32-bit float per channel.
// Conversions to and from this format are provided most D3DFORMATs.

const FLOAT FColor::fDelta = .00001f;

// -------------------------------------------------------------------------------------

void FColor::UndoGammaCorrection()
{
    R = UndoGammaForChannel( R );
    G = UndoGammaForChannel( G );
    B = UndoGammaForChannel( B );
}

// -------------------------------------------------------------------------------------

void FColor::ConvertFrom(FMT Type, const char* pSurfaceBits, BOOL bGammaCorrect,
                            ELEMENTINDEX ElementIndex)
{
    UINT16 u16BITS;
    UINT8 u8BITS;

    switch (Type)
    {
    case FMT_A8R8G8B8: *this = *((UINT32*)pSurfaceBits); break;
    case FMT_X8R8G8B8: *this = *((UINT32*)pSurfaceBits); A = 1.F; break;

    case FMT_R5G6B5:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>(6+5)) & 0x001F)/31.f;
        G = ((u16BITS>>   5) & 0x003F)/63.f;
        B = ((u16BITS      ) & 0x001F)/31.f;
        A = 1.F;
        break;

    case FMT_X1R5G5B5:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>(5+5)) & 0x001F)/31.f;
        G = ((u16BITS>>   5) & 0x001F)/31.f;
        B = ((u16BITS      ) & 0x001F)/31.f;
        A = 1.F;
        break;

    case FMT_A1R5G5B5:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>(5+5)) & 0x001F)/31.f;
        G = ((u16BITS>>   5) & 0x001F)/31.f;
        B = ((u16BITS      ) & 0x001F)/31.f;
        A = ( u16BITS & 0x8000 ) ? 1.f : 0.f;
        break;

    case FMT_A4R4G4B4:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>  (4+4)) & 0x000F)/15.f;
        G = ((u16BITS>>    (4)) & 0x000F)/15.f;
        B = ((u16BITS         ) & 0x000F)/15.f;
        A = ((u16BITS>>(4+4+4)) & 0x000F)/15.f;
        break;

    case FMT_X4R4G4B4:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>(4+4)) & 0x000F)/15.f;
        G = ((u16BITS>>  (4)) & 0x000F)/15.f;
        B = ((u16BITS       ) & 0x000F)/15.f;
        A = 1.f;
        break;

    case FMT_R8G8B8:
        R = *((UINT8*)pSurfaceBits+2)/255.f;
        G = *((UINT8*)pSurfaceBits+1)/255.f;
        B = *((UINT8*)pSurfaceBits+0)/255.f;
        A = 1.F;
        break;

    case FMT_L8:
        R = G = B = *((UINT8*)pSurfaceBits)/255.f;
        A = 1.F;
        break;

    case FMT_L16:
        R = G = B = *((UINT16*)pSurfaceBits)/65535.f;
        A = 1.F;
        break;

    case FMT_A8L8:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = G = B = (UINT8)(0xff & u16BITS)/255.f;
        A = (UINT8)(0xff & (u16BITS >> 8))/255.f;
        break;

    case FMT_A8:
        R = G = B = 0.f;
        A = *((UINT8*)pSurfaceBits)/255.f;
        break;

    case FMT_R3G3B2:
        u8BITS = *((UINT8*)pSurfaceBits);
        R = ((u8BITS>>(3+2)) & 0x07)/7.f;
        G = ((u8BITS>>   2) & 0x07)/7.f;
        B = ((u8BITS      ) & 0x03)/3.f;
        A = 1.F;
        break;

    case FMT_A4L4:
        u16BITS = *((UINT8*)pSurfaceBits);
        R = G = B = (u16BITS & 0x0f)/15.f;
        A =    ((u16BITS>>4) & 0x0f)/15.f;
        break;

    case FMT_A8R3G3B2:
        u16BITS = *((UINT16*)pSurfaceBits);
        R = ((u16BITS>>(3+2)) & 0x07)/7.f;
        G = ((u16BITS>>   2) & 0x07)/7.f;
        B = ((u16BITS      ) & 0x03)/3.f;
        A = (UINT8)(0xff & (u16BITS >> 8))/255.f;
        break;

    case FMT_V8U8:
        {
            INT8 iDU = *(( INT8*)pSurfaceBits+0);
            INT8 iDV = *(( INT8*)pSurfaceBits+1);
            R = (FLOAT)(iDU+127) / 255.f;     // fDU
            G = (FLOAT)(iDV+127) / 255.f;     // fDV

            B = 1.0F;                   // fL
            A = 1.F;
        }
        break;

    case FMT_CxV8U8:
    {
        INT8 iDU = *(( INT8*)pSurfaceBits+0);
        INT8 iDV = *(( INT8*)pSurfaceBits+1);
        R = CLAMP_SIGNED8(iDU);     // fDU
        G = CLAMP_SIGNED8(iDV);     // fDV
        float fUV = 1.0f - R * R - G * G;
        B = (fUV <= 0.0f) ? 0.0f : sqrtf(fUV);

        R = (FLOAT)(iDU+127) / 255.f; G = (FLOAT)(iDV+127) / 255.f;
        B = B * 0.5f + 0.5f;
        A = 1.F;
    }
    break;

    case FMT_V16U16:
        {
            INT16 iDU = *(( INT16*)pSurfaceBits+0);
            INT16 iDV = *(( INT16*)pSurfaceBits+1);
            R = (FLOAT)(iDU+32768) / 65535.f;     // fDU
            G = (FLOAT)(iDV+32768) / 65535.f;     // fDV
            B = 1.0f;   // 1.0 here is intentional
            A = 1.0f;
        }
        break;

    case FMT_L6V5U5:
        {
            u16BITS = *((UINT16*)pSurfaceBits);
            INT8 iU = (INT8)(u16BITS & 0x1f);
            INT8 iV = (INT8)((u16BITS>>5) & 0x1f);
            UINT8 uL = (UINT8)(u16BITS >> 10);
                        iU <<= 3; iU >>= 3;
                        iV <<= 3; iV >>= 3; 
                        iU += (-16 == iU);
                        iV += (-16 == iV);
                        R = (FLOAT)(iU + 15) / 30.0f;
                        G = (FLOAT)(iV + 15) / 30.0f;
            // the unsigned uL is normalized with 2^N - 1, since this is the
            // largest representable value
            B = 1.0f;
            A = (FLOAT)uL * (1.0F/63.0F);       // fL
        }
        break;

    case FMT_X8L8V8U8:
        {
            INT8 iDU = *(( INT8*)pSurfaceBits+0);
            INT8 iDV = *(( INT8*)pSurfaceBits+1);
            UINT8 uL  = *((UINT8*)pSurfaceBits+2);
            R = (FLOAT)(iDU+127) / 255.f;     // fDU
            G = (FLOAT)(iDV+127) / 255.f;     // fDV
            // the unsigned uL is normalized with 2^N - 1, since this is the
            // largest representable value
            A = (FLOAT)uL * (1.0F/255.0F);      // fL
            B = 1.0f;
        }
        break;
    case FMT_Q8W8V8U8:
        {
            INT8 iDU = *(( INT8*)pSurfaceBits+0);
            INT8 iDV = *(( INT8*)pSurfaceBits+1);
            INT8 iDW = *(( INT8*)pSurfaceBits+2);
            INT8 iDQ = *(( INT8*)pSurfaceBits+3);
            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iDU+127) / 255.f;     // fDU
            G = (FLOAT)(iDV+127) / 255.f;     // fDV
            B = (FLOAT)(iDW+127) / 255.f;     // fDU
            A = (FLOAT)(iDQ+127) / 255.f;     // fDV
        }
        break;
    case FMT_W11V11U10:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            INT16 iDU = (INT16)((u32BITS>>(0    )) & 0x3FF);
            INT16 iDV = (INT16)((u32BITS>>(10   )) & 0x7FF);
            INT16 iDW = (INT16)((u32BITS>>(10+11)) & 0x7FF);

            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iDU+512) / 1023.f;    // fDU
            G = (FLOAT)(iDV+1024) / 2047.f;    // fDV
            B = (FLOAT)(iDW+1024) / 2047.f;    // fDW
            A = 1.0f;
        }
        break;
    case FMT_A2B10G10R10:
	case FMT_A2B10G10R10_XR_BIAS:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            R = ((u32BITS>>(0    ))  & 0x3FF)/1023.f;
            G = ((u32BITS>>(10   ))  & 0x3FF)/1023.f;
            B = ((u32BITS>>(10+10))  & 0x3FF)/1023.f;
            A = ((u32BITS>>(10+10+10)) & 0x3)/3.f;
        }
    break;
    case FMT_A2R10G10B10:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            B = ((u32BITS>>(0    ))  & 0x3FF)/1023.f;
            G = ((u32BITS>>(10   ))  & 0x3FF)/1023.f;
            R = ((u32BITS>>(10+10))  & 0x3FF)/1023.f;
            A = ((u32BITS>>(10+10+10)) & 0x3)/3.f;
        }
    break;
    case FMT_A8B8G8R8:
        {
            R = *(( UINT8*)pSurfaceBits+0)/255.f;
            G = *(( UINT8*)pSurfaceBits+1)/255.f;
            B = *(( UINT8*)pSurfaceBits+2)/255.f;
            A = *(( UINT8*)pSurfaceBits+3)/255.f;
        }
    break;
    case FMT_X8B8G8R8:
        {
            R = *(( UINT8*)pSurfaceBits+0)/255.f;
            G = *(( UINT8*)pSurfaceBits+1)/255.f;
            B = *(( UINT8*)pSurfaceBits+2)/255.f;
            A = 1.f;
        }
    break;
    case FMT_G16R16:
        {
            R = *(( UINT16*)pSurfaceBits+0)/65535.f;
            G = *(( UINT16*)pSurfaceBits+1)/65535.f;
            B = 1.0f;   // 1.0 here is intentional
            A = 1.0f;
        }
    break;
    case FMT_W10V11U11:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            INT16 iDU = (INT16)((u32BITS>>(0    )) & 0x7FF);
            INT16 iDV = (INT16)((u32BITS>>(11   )) & 0x7FF);
            INT16 iDW = (INT16)((u32BITS>>(11+11)) & 0x3FF);

            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iDU+1024) / 2047.f;    // fDU
            G = (FLOAT)(iDV+1024) / 2047.f;    // fDV
            B = (FLOAT)(iDW+512) / 1023.f;    // fDW
            A = 1.0f;
        }
    break;
    case FMT_A2W10V10U10:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            INT16 iDU = (INT16)((u32BITS>>(0    )) & 0x3FF);
            INT16 iDV = (INT16)((u32BITS>>(10   )) & 0x3FF);
            INT16 iDW = (INT16)((u32BITS>>(10+10)) & 0x3FF);

            // Sign extension
            iDU <<= 6; iDU >>= 6;
            iDV <<= 6; iDV >>= 6;
            iDW <<= 6; iDW >>= 6;

            iDU += (-512 == iDU);
            iDV += (-512 == iDV);
            iDW += (-512 == iDW);

            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iDU+512) / 1023.f;    // fDU
            G = (FLOAT)(iDV+512) / 1023.f;    // fDV
            B = (FLOAT)(iDW+512) / 1023.f;    // fDW

            // Note: The A component is treated as an unsigned component
            A = ((u32BITS>>(10+10+10)) & 0x3)/3.f; 
        }
	    break;
#if 0  //Removed from DX9
    case FMT_A8X8V8U8:
        {
            INT8 iU = *(( INT8*)pSurfaceBits+0);
            INT8 iV = *(( INT8*)pSurfaceBits+1);
            
            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iU+127) / 255.f;     // fDU
            G = (FLOAT)(iV+127) / 255.f;     // fDV
            B = 1.0f;

            // Note: The A component is treated as unsigned 
            A = *(( UINT8*)pSurfaceBits+3)/255.f;
        }
		break;
    case FMT_L8X8V8U8:
        {
            INT8 iU = *(( INT8*)pSurfaceBits+0);
            INT8 iV = *(( INT8*)pSurfaceBits+1);
            UINT8 iL = *(( UINT8*)pSurfaceBits+3);
            
            // signed values are normalized with 2^(N-1), since -2^(N-1) can
            // be exactly expressed in N bits
            R = (FLOAT)(iU+127) / 255.f;     // fDU
            G = (FLOAT)(iV+127) / 255.f;     // fDV
                        B = 1.0f;
            A = (FLOAT)iL / 255.f;
            
        }
		break;
#endif
	// shadow map texture formats (read only, not needed for ConvertTo)
    case FMT_D16:
        {
            u16BITS = *((UINT16*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)u16BITS * (1.0F/(FLOAT)0xffff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_D24S8:
    case FMT_D24X8:
    case FMT_D24X4S4:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)(u32BITS>>8) * (1.0F/(FLOAT)0xffffff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_S8D24:
    case FMT_X8D24:
    case FMT_X4S4D24:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)(u32BITS&0x00ffffff) * (1.0F/(FLOAT)0xffffff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_D15S1:
        {
            u16BITS = *((UINT16*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)(u16BITS>>1) * (1.0F/(FLOAT)0x7fff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_S1D15:
        {
            u16BITS = *((UINT16*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)(u16BITS&0x7fff) * (1.0F/(FLOAT)0x7fff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_D32:
        {
            UINT32 u32BITS = *((UINT32*)pSurfaceBits);
            R = 0.0F;
            G = (FLOAT)u32BITS * (1.0F/(FLOAT)0xffffffff);
            B = 0.0F;
            A = 1.0f;
        }
        break;

    case FMT_MULTI2_ARGB8:       // TODO: Is this correct?
        {
            if( ELEMENTINDEX_DEFAULT == ElementIndex )
                ElementIndex = ELEMENTINDEX_0; // default to reading from field 0

            UINT8* pOffsetSurfaceBits = (UINT8*)pSurfaceBits + (UINT)ElementIndex * 4;
            R = *(pOffsetSurfaceBits+0)/255.f;
            G = *(pOffsetSurfaceBits+1)/255.f;
            B = *(pOffsetSurfaceBits+2)/255.f;
            A = *(pOffsetSurfaceBits+3)/255.f;
        }
        break;

    case FMT_A16B16G16R16:
        {
            R = *((UINT16*)pSurfaceBits+0)/65535.f;
            G = *((UINT16*)pSurfaceBits+1)/65535.f;
            B = *((UINT16*)pSurfaceBits+2)/65535.f;
            A = *((UINT16*)pSurfaceBits+3)/65535.f;
        }
        break;

    case FMT_Q16W16V16U16:
        {
            INT16 iDU = *(( INT16*)pSurfaceBits+0);
            INT16 iDV = *(( INT16*)pSurfaceBits+1);
            INT16 iDW = *(( INT16*)pSurfaceBits+2);
            INT16 iDQ = *(( INT16*)pSurfaceBits+3);
            R = (FLOAT)(iDU + 32768) / 65536.f;     // fDU
            G = (FLOAT)(iDV + 32768) / 65536.f;     // fDV
            B = (FLOAT)(iDW + 32768) / 65536.f;     // fDW
            A = (FLOAT)(iDQ + 32768) / 65536.f;     // fDQ
        }
        break;

    case FMT_R16F:
        {
            R = (( CFloat16*)pSurfaceBits)[0];
            G = 1.0f;
            B = 1.0f;
            A = 1.0f;
        }
        break;

    case FMT_G16R16F:
        {
            R = (( CFloat16*)pSurfaceBits)[0];
            G = (( CFloat16*)pSurfaceBits)[1];
            B = 1.0f;
            A = 1.0f;
        }
        break;

    case FMT_A16B16G16R16F:
        {
            R = (( CFloat16*)pSurfaceBits)[0];
            G = (( CFloat16*)pSurfaceBits)[1];
            B = (( CFloat16*)pSurfaceBits)[2];
            A = (( CFloat16*)pSurfaceBits)[3];
        }
        break;

    case FMT_R32F:
        {
            R = *(float*)pSurfaceBits;
            G = 1.0f;
            B = 1.0f;
            A = 1.0f;
        }
        break;

    case FMT_G32R32F:
        {
            R = (( float*)pSurfaceBits)[0];
            G = (( float*)pSurfaceBits)[1];
            B = 1.0f;
            A = 1.0f;
        }
        break;

    case FMT_A32B32G32R32F:
        {
            R = (( float*)pSurfaceBits)[0];
            G = (( float*)pSurfaceBits)[1];
            B = (( float*)pSurfaceBits)[2];
            A = (( float*)pSurfaceBits)[3];
        }
        break;

	case FMT_A1:
        R = G = B = A = *((bool *)pSurfaceBits);
        break;

        default:
        {
            DPF(2, _T("FColor::ConvertFrom() - Unsupported format type. FMT=%d.\n"), (UINT)Type);
        }
        break;
    }
        
    if( bGammaCorrect )
    {
        UndoGammaCorrection();
    }
}

// -------------------------------------------------------------------------------------

void FColor::ConvertTo( FMT Type, float fRoundOffset, char* pSurfaceBits, 
                        BOOL bGammaCorrect, ELEMENTINDEX ElementIndex) const
{
    int iR, iG, iB, iA;
    FLOAT Rg, Gg, Bg; // Gamma Corrected values
        
    if( bGammaCorrect )
    {
        Rg = GammaCorrectChannel( R );
        Gg = GammaCorrectChannel( G );
        Bg = GammaCorrectChannel( B );
    }
    else
    {
        Rg = R;
        Gg = G;
        Bg = B;
    }

    switch (Type)
    {
    case FMT_A8R8G8B8:
        *((UINT8*)pSurfaceBits+0) = (UINT8)((FLOAT)Bg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+1) = (UINT8)((FLOAT)Gg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+2) = (UINT8)((FLOAT)Rg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+3) = (UINT8)((FLOAT)A * 255. + fRoundOffset);
        break;

    case FMT_X8R8G8B8:
        *((UINT8*)pSurfaceBits+0) = (UINT8)((FLOAT)Bg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+1) = (UINT8)((FLOAT)Gg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+2) = (UINT8)((FLOAT)Rg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+3) = 0x00;
        break;

    case FMT_R8G8B8:
        *((UINT8*)pSurfaceBits+0) = (UINT8)((FLOAT)Bg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+1) = (UINT8)((FLOAT)Gg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+2) = (UINT8)((FLOAT)Rg * 255. + fRoundOffset);
        break;

    case FMT_A4R4G4B4:
        iA = (int)((FLOAT)A * 15. + fRoundOffset);
        iR = (int)((FLOAT)Rg * 15. + fRoundOffset);
        iG = (int)((FLOAT)Gg * 15. + fRoundOffset);
        iB = (int)((FLOAT)Bg * 15. + fRoundOffset);
        *((UINT16*)pSurfaceBits) = (iA<<12) | (iR<<8) | (iG<<4) | iB;
        break;

    case FMT_X4R4G4B4:
        iR = (int)((FLOAT)Rg * 15. + fRoundOffset);
        iG = (int)((FLOAT)Gg * 15. + fRoundOffset);
        iB = (int)((FLOAT)Bg * 15. + fRoundOffset);
        *((UINT16*)pSurfaceBits) = (0x00<<12) | (iR<<8) | (iG<<4) | iB;
        break;

    case FMT_R5G6B5:
        iR = (int)((FLOAT)Rg * 31. + fRoundOffset); // apply rounding bias then truncate
        iG = (int)((FLOAT)Gg * 63. + fRoundOffset);
        iB = (int)((FLOAT)Bg * 31. + fRoundOffset);
        *((UINT16*)pSurfaceBits) =            (iR<<11) | (iG<<5) | iB;
        break;

    case FMT_A1R5G5B5:
        iA = (int)((FLOAT)A *  1. + fRoundOffset);
        iR = (int)((FLOAT)Rg * 31. + fRoundOffset);
        iG = (int)((FLOAT)Gg * 31. + fRoundOffset);
        iB = (int)((FLOAT)Bg * 31. + fRoundOffset);
        *((UINT16*)pSurfaceBits) = (iA<<15) | (iR<<10) | (iG<<5) | iB;
        break;

    case FMT_X1R5G5B5:
        iR = (int)((FLOAT)Rg * 31. + fRoundOffset);
        iG = (int)((FLOAT)Gg * 31. + fRoundOffset);
        iB = (int)((FLOAT)Bg * 31. + fRoundOffset);
        *((UINT16*)pSurfaceBits) = (iR<<10) | (iG<<5) | iB;
        break;

    case FMT_R3G3B2:
        iR = (int)((FLOAT)Rg * 7. + fRoundOffset);
        iG = (int)((FLOAT)Gg * 7. + fRoundOffset);
        iB = (int)((FLOAT)Bg * 3. + fRoundOffset);
        *((UINT8*)pSurfaceBits) = (iR<<5) | (iG<<2) | iB;
        break;

    case FMT_A8R3G3B2:
        iA = (int)((FLOAT)A * 255. + fRoundOffset);
        iR = (int)((FLOAT)Rg * 7. + fRoundOffset);
        iG = (int)((FLOAT)Gg * 7. + fRoundOffset);
        iB = (int)((FLOAT)Bg * 3. + fRoundOffset);
        *((UINT16*)pSurfaceBits) = (iA<<8) | (iR<<5) | (iG<<2) | iB;
        break;

    case FMT_A2B10G10R10:
	case FMT_A2B10G10R10_XR_BIAS:
        iR = (int)((FLOAT)Rg * 1023.f + fRoundOffset);
        iG = (int)((FLOAT)Gg * 1023.f + fRoundOffset);
        iB = (int)((FLOAT)Bg * 1023.f + fRoundOffset);
        iA = (int)((FLOAT)A * 3.f + fRoundOffset);
        *((UINT32*)pSurfaceBits) = (iA<<(10+10+10)) | (iB<<(10+10)) | (iG<<10) | iR;
        break;

    case FMT_A2R10G10B10:
        iR = (int)((FLOAT)Rg * 1023.f + fRoundOffset);
        iG = (int)((FLOAT)Gg * 1023.f + fRoundOffset);
        iB = (int)((FLOAT)Bg * 1023.f + fRoundOffset);
        iA = (int)((FLOAT)A * 3.f + fRoundOffset);
        *((UINT32*)pSurfaceBits) = (iA<<(10+10+10)) | (iR<<(10+10)) | (iG<<10) | iB;
        break;

    case FMT_A8B8G8R8:          
        *((UINT8*)pSurfaceBits+0) = (UINT8)((FLOAT)Rg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+1) = (UINT8)((FLOAT)Gg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+2) = (UINT8)((FLOAT)Bg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+3) = (UINT8)((FLOAT)A * 255. + fRoundOffset);
        break;

    case FMT_X8B8G8R8:
        *((UINT8*)pSurfaceBits+0) = (UINT8)((FLOAT)Rg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+1) = (UINT8)((FLOAT)Gg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+2) = (UINT8)((FLOAT)Bg * 255. + fRoundOffset);
        *((UINT8*)pSurfaceBits+3) = 0x00;
        break;

    case FMT_MULTI2_ARGB8:
        {
            if( ELEMENTINDEX_DEFAULT == ElementIndex )
                ElementIndex = ELEMENTINDEX_0;

            UINT8* pOffsetSurfaceBits = (UINT8*)pSurfaceBits + (UINT)ElementIndex * 4;
            *(pOffsetSurfaceBits+0) = (UINT8)((FLOAT)Rg * 255. + fRoundOffset);
            *(pOffsetSurfaceBits+1) = (UINT8)((FLOAT)Gg * 255. + fRoundOffset);
            *(pOffsetSurfaceBits+2) = (UINT8)((FLOAT)Bg * 255. + fRoundOffset);
            *(pOffsetSurfaceBits+3) = (UINT8)((FLOAT)A * 255. + fRoundOffset);
        }
        break;

    case FMT_A16B16G16R16:
        *((UINT16*)pSurfaceBits+0) = (UINT16)((FLOAT)Rg * 65535. + fRoundOffset);
        *((UINT16*)pSurfaceBits+1) = (UINT16)((FLOAT)Gg * 65535. + fRoundOffset);
        *((UINT16*)pSurfaceBits+2) = (UINT16)((FLOAT)Bg * 65535. + fRoundOffset);
        *((UINT16*)pSurfaceBits+3) = (UINT16)((FLOAT)A * 65535. + fRoundOffset);
        break;

    case FMT_R16F:
        {
            *((CFloat16*)pSurfaceBits+0) = Rg;
        }
        break;

    case FMT_G16R16F:
        {
            *((CFloat16*)pSurfaceBits+0) = Rg;
            *((CFloat16*)pSurfaceBits+1) = Gg;
        }
        break;

    case FMT_A16B16G16R16F:
        {
            *((CFloat16*)pSurfaceBits+0) = Rg;
            *((CFloat16*)pSurfaceBits+1) = Gg;
            *((CFloat16*)pSurfaceBits+2) = Bg;
            *((CFloat16*)pSurfaceBits+3) = A;
        }
        break;

    case FMT_R32F:
        {
            *((float*)pSurfaceBits+0) = Rg;
        }
        break;

    case FMT_G32R32F:
        {
            *((float*)pSurfaceBits+0) = Rg;
            *((float*)pSurfaceBits+1) = Gg;
        }
        break;

    case FMT_A32B32G32R32F:
        {
            *((float*)pSurfaceBits+0) = Rg;
            *((float*)pSurfaceBits+1) = Gg;
            *((float*)pSurfaceBits+2) = Bg;
            *((float*)pSurfaceBits+3) = A;
        }
        break;

    case FMT_P8:
        {
            iR = (((UINT32)(Rg * 255.f*6 + fRoundOffset)) >> 8);
            iG = (((UINT32)(Gg * 255.f*6 + fRoundOffset)) >> 8);
            iB = (((UINT32)(Bg * 255.f*6 + fRoundOffset)) >> 8);
            *((PBYTE)pSurfaceBits) = (BYTE)(iR + iG*6 + iB*36);
        }
        break;
    case FMT_A8P8:
        {
            iR = (((UINT32)(Rg * 255.f + fRoundOffset) * 6) >> 8);
            iG = (((UINT32)(Gg * 255.f + fRoundOffset) * 6) >> 8);
            iB = (((UINT32)(Bg * 255.f + fRoundOffset) * 6) >> 8);
            iA = (UINT32)(A * 255.f + fRoundOffset);
            *((PBYTE)pSurfaceBits+0) = (BYTE)(iR + iG*6 + iB*36);
            *((PBYTE)pSurfaceBits+1) = (BYTE)iA;
        }
        break;
    case FMT_P4:
        {
            iR  = ((UINT32)(Rg * 255.f + fRoundOffset) >> 7);
            iG  = ((UINT32)(Gg * 255.f + fRoundOffset) >> 7);
            iB  = ((UINT32)(Bg * 255.f + fRoundOffset) >> 7);
            
            *((PBYTE)pSurfaceBits) = (BYTE)(iR + iG*2 + iB*4);
        }
        break;
        
    case FMT_L8:
        {
            iR = (UINT32)((Rg *  0.2125f + Gg * 0.7154f + Bg * 0.0721f) * 255.f + fRoundOffset);
            *((PBYTE)pSurfaceBits) = (BYTE)iR;
        }
        break;
        
    case FMT_A8L8:
        {
            iR = (UINT32)((Rg *  0.2125f + Gg * 0.7154f + Bg * 0.0721f) * 255.f + fRoundOffset);
            iA = (UINT32)(A * 255.f + fRoundOffset);
            *((PBYTE)pSurfaceBits+0) = (BYTE)iR;
            *((PBYTE)pSurfaceBits+1) = (BYTE)iA;
        }
        break;
        
    case FMT_A4L4:
        {
            iR = (UINT32)((Rg *  0.2125f + Gg * 0.7154f + Bg * 0.0721f) * 15.f + fRoundOffset);
            iA = (UINT32)(A * 15.f + fRoundOffset);
            *((PBYTE)pSurfaceBits) = (BYTE)((iA << 4) | iR);
        }
        break;
        
    case FMT_L16:
        {
            iR = (UINT32)((Rg *  0.2125f + Gg * 0.7154f + Bg * 0.0721f) * 65535.f + fRoundOffset);
            *((UINT16*)pSurfaceBits) = (UINT16)iR;
        }
        break;
    case FMT_A8:
        {
            iA = (UINT32)(A * 255.f + fRoundOffset);
            *((UINT8*)pSurfaceBits) = (UINT8)iA;
        }
        break;
    case FMT_G16R16:
        {
            *((UINT16*)pSurfaceBits+0) = (UINT16)((FLOAT)Rg * 65535. + fRoundOffset);
            *((UINT16*)pSurfaceBits+1) = (UINT16)((FLOAT)Gg * 65535. + fRoundOffset);
        }
        break;
    case FMT_V8U8:
        {
            *((INT8*)pSurfaceBits+0) = ((INT8)(127.f * (Rg*2 - 1.f) + fRoundOffset));
            *((INT8*)pSurfaceBits+1) = ((INT8)(127.f * (Gg*2 - 1.f) + fRoundOffset));
        }
        break;
    case FMT_CxV8U8:
		{
			float r = (Rg*2 - 1.f);
			float g = (Gg*2 - 1.f);
			float b = (Bg*2 - 1.f);
        
			float f = r*r+g*g+b*b;
			if(f > FLT_MIN) {
				float sqrtF = sqrtf(f);
				r /= sqrtF; g /= sqrtF; b /= sqrtF;
			}
			else {
				r = g = b = 0.0f;
			}
			*((INT8*)pSurfaceBits+0) = ((INT8)(127.f * r + fRoundOffset));
			*((INT8*)pSurfaceBits+1) = ((INT8)(127.f * g + fRoundOffset));
		}
		break;
    case FMT_V16U16:
        {
            *((INT16*)pSurfaceBits+0) = ((INT16)(32767.f * (Rg*2 - 1.f) + fRoundOffset));
                        *((INT16*)pSurfaceBits+1) = ((INT16)(32767.f * (Gg*2 - 1.f) + fRoundOffset));
        }
        break;

        case FMT_L6V5U5:
        {
            INT8 iR8 = (INT8)((Rg*2 - 1.f) * 15.f + fRoundOffset);
            INT8 iG8 = (INT8)((Gg*2 - 1.f) * 15.f + fRoundOffset);
            UINT8 uA8 = (UINT8)(A * 63.f + fRoundOffset);
            *((UINT16*)pSurfaceBits) = ((uA8 & 63) << 10) | ((iG8 & 31) << 5) | (iR8 & 31);
        }
        break;


        case FMT_X8L8V8U8:
        {
            *((INT8*)pSurfaceBits+0) = ((INT8)(127.f * (Rg*2 - 1.f) + fRoundOffset));
            *((INT8*)pSurfaceBits+1) = ((INT8)(127.f * (Gg*2 - 1.f) + fRoundOffset));
            *((UINT8*)pSurfaceBits+2) = ((UINT8)(255.f * A + fRoundOffset));
        }
        break;
    case FMT_Q8W8V8U8:
        {
                        *((INT8*)pSurfaceBits+0) = ((INT8)(127.f * (Rg*2 - 1.f) + fRoundOffset));
                        *((INT8*)pSurfaceBits+1) = ((INT8)(127.f * (Gg*2 - 1.f) + fRoundOffset));
                        *((INT8*)pSurfaceBits+2) = ((INT8)(127.f * (Bg*2 - 1.f) + fRoundOffset));
                        *((INT8*)pSurfaceBits+3) = ((INT8)(127.f * (A*2 - 1.f) + fRoundOffset));                        
        }
        break;
        case FMT_Q16W16V16U16:
        {
            *((INT16*)pSurfaceBits+0) = ((INT16)(32767.f * (Rg*2 - 1.f) + fRoundOffset));
                        *((INT16*)pSurfaceBits+1) = ((INT16)(32767.f * (Gg*2 - 1.f) + fRoundOffset));
                        *((INT16*)pSurfaceBits+2) = ((INT16)(32767.f * (Bg*2 - 1.f) + fRoundOffset));
                        *((INT16*)pSurfaceBits+3) = ((INT16)(32767.f * (A*2 - 1.f) + fRoundOffset));
        }
        break;
    case FMT_W11V11U10:
        {
                        INT16 iB16 = (INT16)((Bg*2 - 1.f) * 1023.f + fRoundOffset);
                        INT16 iG16 = (INT16)((Gg*2 - 1.f) * 1023.f + fRoundOffset);
                        INT16 iR16 = (INT16)((Rg*2 - 1.f) * 511.f + fRoundOffset);
                        
                        *((UINT32*)pSurfaceBits) = ((iB16<<21) & 0xFFE00000) | ((iG16<<10) & 0x1FFC00) | (iR16 & 0x3FF);
        }
        break;
        case FMT_W10V11U11:
        {
                        INT16 iB16 = (INT16)((Bg*2 - 1.f) * 511.f + fRoundOffset);
                        INT16 iG16 = (INT16)((Gg*2 - 1.f) * 1023.f + fRoundOffset);
                        INT16 iR16 = (INT16)((Rg*2 - 1.f) * 1023.f + fRoundOffset);
                        
                        *((UINT32*)pSurfaceBits) = ((iB16<<21) & 0xFFC00000) | ((iG16<<10) & 0x3FF800) | (iR16 & 0x7FF);
        }
        break;
    case FMT_A2W10V10U10:
        {
                        INT16 iR16 = (INT16)((Rg*2 - 1.f) * 510.f + fRoundOffset);
                        INT16 iG16 = (INT16)((Gg*2 - 1.f) * 510.f + fRoundOffset);
                        INT16 iB16 = (INT16)((Bg*2 - 1.f) * 510.f + fRoundOffset);
                        iA = (UINT32)(A * 3.f + fRoundOffset);
                        
                        *((UINT32*)pSurfaceBits) = (iA << 30) | ((iB16<<20) & 0x3FF00000) | ((iG16<<10) & 0xFFC00) | (iR16 & 0x3FF);
        }
        break;
	case FMT_A1:
        iA = (int)((FLOAT)A + fRoundOffset);
        *((unsigned char *)pSurfaceBits) = (char)iA;
		break;

#if 0 //Removed from DX9
    case FMT_A8X8V8U8:
        {
                        *((INT8*)pSurfaceBits+0) = ((INT8)(127.f * (Rg*2 - 1.f) + fRoundOffset));
                        *((INT8*)pSurfaceBits+1) = ((INT8)(127.f * (Gg*2 - 1.f) + fRoundOffset));
                        *((UINT8*)pSurfaceBits+3) = ((UINT8)(255.f * A + fRoundOffset));
        }
                break;
    case FMT_L8X8V8U8:
        {
                        *((INT8*)pSurfaceBits+0) = ((INT8)(127.f * (Rg*2 - 1.f) + fRoundOffset));
                        *((INT8*)pSurfaceBits+1) = ((INT8)(127.f * (Gg*2 - 1.f) + fRoundOffset));
                        *((UINT8*)pSurfaceBits+3) = ((UINT8)(255.f * A + fRoundOffset));
        }
        break;
#endif
	default:
        {
            DPF(2, _T("FColor::ConvertTo() - Unsupported format type. FMT=%d.\n"), (UINT)Type);
        }
        break;
    }
}

// --------------------------------------------------------------------------------------

FSurface::FSurface(CSurface *pSurface)
{
        m_pData = NULL;
        m_uWidth = 0;
        m_uHeight = 0;
        m_fromFmt = FMT_A8R8G8B8;
        ConvertFrom(pSurface);
}

// --------------------------------------------------------------------------------------

HRESULT FSurface::SetSize(UINT Width, UINT Height)
{
        HRESULT hr = S_OK;

        if(m_uWidth == Width && m_uHeight == Height)
                goto Exit;

        SAFEDELETEA(m_pData);

        m_pData = new FColor[Width * Height];
        if(NULL == m_pData)
        {
                DPF(1, _T("FSurface::SetSize() - Out of memory.\n"));
                hr = E_OUTOFMEMORY;
                goto Exit;
        }

        memset(m_pData, 0, sizeof(FColor) * Width * Height);

        m_uWidth = Width;
        m_uHeight = Height;

Exit:

        return hr;
}

// --------------------------------------------------------------------------------------

FSurface::~FSurface()
{
        m_uWidth = 0;
        m_uHeight = 0;
        m_fromFmt = FMT_A8R8G8B8;
        SAFEDELETEA(m_pData);
}

// --------------------------------------------------------------------------------------

HRESULT FSurface::ConvertFrom(CSurface *pSurf)
{
    return ConvertFrom(pSurf, FILTER_NONE);
}

// --------------------------------------------------------------------------------------

HRESULT FSurface::ConvertFrom(CSurface *pSurf, DWORD filter)
{
    UINT i, j;
    HRESULT hr = S_OK;
    SURFACEDESC  Desc;
    PBYTE pScanline = NULL, pPixel = NULL;
    LOCKED_RECT r;
    FColor *pFloatSurf = NULL;
    float fY0, fY1, fU, fV;
    PALETTEENTRY Palette[256], *pEntry = NULL;
    DWORD dwNumEntries;
    FColor *pColor1 = NULL, *pColor2 = NULL, colorTmp;

    if(NULL == pSurf) {
        DPF(1, _T("FSurface::ConvertFrom() - Invalid params.\n"));
        hr = E_INVALIDARG; goto Exit;
    }

    if(FAILED(hr = pSurf->GetDesc(&Desc))) {
        DPF(1, _T("FSurface::ConvertFrom() - GetDesc failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }
    m_fromFmt = (FMT)Desc.Format.d3dfFormat;

    if((FMT)Desc.Format.d3dfFormat == FMT_P8 || (FMT)Desc.Format.d3dfFormat == FMT_A8P8) {
        if (FAILED(hr = pSurf->GetPalette((LPPALETTEENTRY)Palette, dwNumEntries))) {
            DPF(1, _T("FSurface::ConvertFrom() - GetPalette failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
            goto Exit;
        }
    }

    pFloatSurf = new FColor[Desc.Width * Desc.Height];
    if(NULL == pFloatSurf) {
        DPF(1, _T("FSurface::ConvertFrom() - Out of memory.\n"));
        hr = E_OUTOFMEMORY; goto Exit;
    }
    memset(pFloatSurf, 0, sizeof(FColor) * Desc.Width * Desc.Height);

    if (FAILED(hr = pSurf->LockRect(&r, NULL, LOCK_READONLY | LOCK_NOSYSLOCK))) {
        DPF(1, _T("FSurface::ConvertFrom() - LockRect failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }

    pScanline = (PBYTE)r.pBits;
    for(i=0; i<Desc.Height; i++) {
        for(j=0; j<Desc.Width; j++) {
            switch((FMT)Desc.Format.d3dfFormat) {
                case FMT_P8:
                case FMT_A8P8:
                {
                    pPixel = pScanline + ((Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8) * j);
                    pEntry = Palette + (*((WORD*)pPixel) & 0xFF);
                    pColor1 = pFloatSurf + (i * Desc.Width + j);
                    pColor1->R = pEntry->peRed / 255.f; pColor1->G = pEntry->peGreen / 255.f; pColor1->B = pEntry->peBlue / 255.f;
                    if((FMT)Desc.Format.d3dfFormat == FMT_P8)
                        pColor1->A = pEntry->peFlags / 255.f;
                    else
                        pColor1->A = ((*((WORD*)pPixel) >> 8) & 0xFF) / 255.f;
                }
                break;
                case FMT_YUY2:
                case FMT_UYVY:
                {
                    pPixel = pScanline + (2 * j);
                    if ((FMT)Desc.Format.d3dfFormat == FMT_YUY2) {
                        UNPACKYUY2(*((DWORD*)pPixel), fY0, fY1, fU, fV);
                    }
                    else {
                        UNPACKUYVY(*((DWORD*)pPixel), fY0, fY1, fU, fV);
                    }

                    pColor1 = pFloatSurf + (i * Desc.dwWidth) + j;
                    if (j+1 == Desc.Width) {
                        pColor2 = &colorTmp;
                    }
                    else {
                        pColor2 = pFloatSurf + (i * Desc.dwWidth) + j + 1;
                    }
                    YUVTORGB(fY0, fY1, fU, fV, pColor1, pColor2);
                    j++;
                }
                break;
                case FMT_R8G8_B8G8:
                case FMT_G8R8_G8B8:
                {
                    pPixel = pScanline + (2 * j);
                    DWORD dwPixel = *((DWORD*)pPixel);
                    
                    pColor1 = pFloatSurf + (i * Desc.dwWidth) + j;
                    if (j+1 == Desc.Width) {
                        pColor2 = &colorTmp;
                    }
                    else {
                        pColor2 = pFloatSurf + (i * Desc.dwWidth) + j + 1;
                    }
                    pColor1->A = 1.0f; pColor2->A = 1.0f;
                    if((FMT)Desc.Format.d3dfFormat == FMT_R8G8_B8G8) {

                        pColor1->B = pColor2->B = ((dwPixel >> 24)&0xff)/255.f;
                        pColor2->G = ((dwPixel >> 16)&0xff)/255.f;
						pColor1->R = pColor2->R = ((dwPixel >> 8)&0xff)/255.f;
						pColor1->G = (dwPixel&0xff)/255.f;
                    }
                    else {
						
                        pColor2->G = ((dwPixel >> 24)&0xff)/255.f;
						pColor1->B = pColor2->B = ((dwPixel >> 16)&0xff)/255.f;
						pColor1->G = ((dwPixel >> 8)&0xff)/255.f;
						pColor1->R = pColor2->R = (dwPixel&0xff)/255.f;
                    }
                    j++;
                }
                break;
                default:
                {
                    pPixel = pScanline + ((Desc.Format.ddpfPixelFormat.dwRGBBitCount / 8) * j);
                    pFloatSurf[i * Desc.Width + j].ConvertFrom((FMT)Desc.Format.d3dfFormat, (const char*)pPixel, FALSE, ELEMENTINDEX_0);
                }
                break;
            }
        }
        pScanline += r.iPitch;
    }

    if (FAILED(hr = pSurf->UnlockRect())) {
        DPF(1, _T("FSurface::ConvertFrom() - UnlockRect failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }

  Exit:
    if(FAILED(hr)) {
        SAFEDELETEA(pFloatSurf);
    }
    else {
        // Release any existing data
        SAFEDELETEA(m_pData);

        // Update the surface with the new data
        m_uWidth        = Desc.Width;
        m_uHeight = Desc.Height;
        m_pData = pFloatSurf;
    }
    return hr;
}

// --------------------------------------------------------------------------------------

static const DWORD COLORRANGE_UUUU = 0x01;
static const DWORD COLORRANGE_UNNN = 0x02;
static const DWORD COLORRANGE_NNNN = 0x03;
static const DWORD COLORRANGE_FFFF = 0x04;

static const char *COLORRANGE_STRINGS[] = {
    "Invalid Color Range", "UUUU", "UNNN", "NNNN", "FFFF"
};

DWORD GetColorRangeType(FMT fmt) {
    DWORD dwColorRangeType = 0;
    
    switch (fmt) {
        case FMT_R8G8B8 :
        case FMT_A8R8G8B8 :
        case FMT_X8R8G8B8 :
        case FMT_R5G6B5 :
        case FMT_X1R5G5B5 :
        case FMT_A1R5G5B5 :
        case FMT_A4R4G4B4 :
        case FMT_R3G3B2 :
        case FMT_A8 :
		case FMT_A1 :
        case FMT_A8R3G3B2 :
        case FMT_X4R4G4B4 :
        case FMT_A2B10G10R10 :
        case FMT_A8B8G8R8 :
        case FMT_X8B8G8R8 :
        case FMT_G16R16 :
        case FMT_A2R10G10B10 :
		case FMT_A2B10G10R10_XR_BIAS :
        case FMT_A16B16G16R16 :
        case FMT_A8P8 :
        case FMT_P8 :
        case FMT_L8 :
        case FMT_A8L8 :
        case FMT_A4L4 :
        case FMT_L16 :
        case FMT_UYVY :
        case FMT_R8G8_B8G8 :
        case FMT_YUY2 :
        case FMT_G8R8_G8B8 :
        case FMT_DXT1 :
        case FMT_DXT2 :
        case FMT_DXT3 :
        case FMT_DXT4 :
        case FMT_DXT5 :
        case FMT_D16_LOCKABLE :
            dwColorRangeType = COLORRANGE_UUUU;
            break;
        case FMT_V8U8 :
        case FMT_L6V5U5 :
        case FMT_X8L8V8U8 :
        case FMT_V16U16 :
        case FMT_W11V11U10 :
        case FMT_W10V11U11 :
        case FMT_A2W10V10U10 :
#if 0
        case FMT_A8X8V8U8 :
        case FMT_L8X8V8U8 :
#endif
        case FMT_CxV8U8 :
            dwColorRangeType = COLORRANGE_UNNN;
            break;

        case FMT_Q8W8V8U8 :
        case FMT_Q16W16V16U16 :
            dwColorRangeType = COLORRANGE_NNNN;
            break;
        case FMT_R16F :
        case FMT_G16R16F :
        case FMT_A16B16G16R16F :
        case FMT_R32F :
        case FMT_G32R32F :
        case FMT_A32B32G32R32F :
        case FMT_D32F_LOCKABLE :
            dwColorRangeType = COLORRANGE_FFFF;
            break;
        default :
            char szFormat[32];
            FmtToString(FORMAT_TEXTURE, &FORMAT(fmt), szFormat);
            DPF(0, _T("FSurface::ConvertFrom() - No ColorRange known for format: %s.\n"), szFormat);
            dwColorRangeType = 0;
    }
    return dwColorRangeType;
}

// --------------------------------------------------------------------------------------

static void ColorRangesAdjustFFFFToUNNN(FColor colorIn, FColor *pColorOut) {
    pColorOut->R = colorIn.R*0.5f + 0.5f;
    pColorOut->G = colorIn.G*0.5f + 0.5f;
    pColorOut->B = colorIn.B*0.5f + 0.5f;
    pColorOut->A = colorIn.A;
}

// --------------------------------------------------------------------------------------

static void ColorRangesAdjustFFFFToNNNN(FColor colorIn, FColor *pColorOut) {
    pColorOut->R = colorIn.R*0.5f + 0.5f;
    pColorOut->G = colorIn.G*0.5f + 0.5f;
    pColorOut->B = colorIn.B*0.5f + 0.5f;
    pColorOut->A = colorIn.A*0.5f + 0.5f;
}

// --------------------------------------------------------------------------------------

static void ColorRangesAdjustUNNNToFFFF(FColor colorIn, FColor *pColorOut) {
    pColorOut->R = colorIn.R*2 - 1.f;
    pColorOut->G = colorIn.G*2 - 1.f;
    pColorOut->B = colorIn.B*2 - 1.f;
    pColorOut->A = colorIn.A;
}

// --------------------------------------------------------------------------------------

static void  ColorRangesAdjustNNNNToFFFF(FColor colorIn, FColor *pColorOut) {
    pColorOut->R = colorIn.R*2 - 1.f;
    pColorOut->G = colorIn.G*2 - 1.f;
    pColorOut->B = colorIn.B*2 - 1.f;
    pColorOut->A = colorIn.A*2 - 1.f;
}

// --------------------------------------------------------------------------------------

static BOOL CheckColorRanges(FMT to, FMT from, UINT_PTR **ppfn) {
    DWORD crtTo = GetColorRangeType(to);
    DWORD crtFrom = GetColorRangeType(from);
    BOOL bConversionNeeded = FALSE;
    
    switch(crtTo) {
        case COLORRANGE_FFFF :
            switch(crtFrom) {
                case COLORRANGE_UNNN:
                    *ppfn = (UINT_PTR *)ColorRangesAdjustUNNNToFFFF;
                    bConversionNeeded = TRUE;
                    break;
                    
                case COLORRANGE_NNNN:
                    *ppfn = (UINT_PTR *)ColorRangesAdjustNNNNToFFFF;
                    bConversionNeeded = TRUE;
                    break;
            }
            break;
        case COLORRANGE_UNNN :
            switch(crtFrom) {
                case COLORRANGE_FFFF:
                    *ppfn = (UINT_PTR *)ColorRangesAdjustFFFFToUNNN;
                    bConversionNeeded = TRUE;
                    break;
            }
            break;
        case COLORRANGE_NNNN :
            switch(crtFrom) {
                case COLORRANGE_FFFF:
                    *ppfn = (UINT_PTR *)ColorRangesAdjustFFFFToNNNN;
                    bConversionNeeded = TRUE;
                    break;
            }
            break;
    }
    if (bConversionNeeded) {
        DPF(1, "Conversion needed: %s -> %s\n", COLORRANGE_STRINGS[crtFrom], COLORRANGE_STRINGS[crtTo]);
    }
    else {
        DPF(2, "No conversion needed: %s -> %s\n", COLORRANGE_STRINGS[crtFrom], COLORRANGE_STRINGS[crtTo]);
    }
    return bConversionNeeded;
}

// --------------------------------------------------------------------------------------

HRESULT FSurface::ConvertTo(CSurface *pSurf)
{
    return ConvertTo(pSurf, FILTER_NONE);
}

// --------------------------------------------------------------------------------------

HRESULT FSurface::ConvertTo(CSurface *pSurf, DWORD filter)
{
    UINT i, j;
    HRESULT hr = S_OK;
    SURFACEDESC  Desc;
    PBYTE pScanline = NULL;
    LOCKED_RECT r;
    FColor Pal[256];
    DWORD dwNumEntries;
    BOOL bGammaCorrect = FALSE; // !!! need to set this appropriately
    BOOL bAdjustColorRange = FALSE;
    void (*pfnColorRangeAdjust)(FColor colorIn, FColor *pColorOut) = 0L;

    if(NULL == pSurf || NULL == m_pData) {
        DPF(1, _T("FSurface::ConvertTo() - Invalid params.\n"));
        hr = E_INVALIDARG; goto Exit;
    }

    if(FAILED(hr = pSurf->GetDesc(&Desc))) {
        DPF(1, _T("FSurface::ConvertTo() - GetDesc failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }

        // Sizes must be identical
    if(Desc.Width != m_uWidth || Desc.Height != m_uHeight) {
        DPF(1, _T("FSurface::ConvertTo() - Output surface dimensions need to be identical to FSurface dimensions.\n"));
        goto Exit;
    }

    if ((FMT)Desc.Format.d3dfFormat == FMT_P8 || (FMT)Desc.Format.d3dfFormat == FMT_A8P8) {
        PALETTEENTRY Palette[256];
        if (FAILED(hr = pSurf->GetPalette((LPPALETTEENTRY)Palette, dwNumEntries))) {
            DPF(1, _T("FSurface::ConvertTo() - GetPalette failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
            goto Exit;
        }
        for (UINT u = 0; u < dwNumEntries; u++) {
            Pal[u] = FColor(Palette[u].peRed, Palette[u].peGreen, Palette[u].peBlue, Palette[u].peFlags);
        }
    }

    if (FAILED(hr = pSurf->LockRect(&r, NULL, LOCK_READONLY | LOCK_NOSYSLOCK))) {
        DPF(1, _T("FSurface::ConvertTo() - LockRect failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }

    bAdjustColorRange = CheckColorRanges((FMT)Desc.Format.d3dfFormat, m_fromFmt, (UINT_PTR **)&pfnColorRangeAdjust);
    pScanline = (PBYTE)r.pBits;
    for (i=0; i<Desc.Height; i++) {
        for (j=0; j<Desc.Width; j++){
            FColor color2;
            FColor color = m_pData[i * Desc.Width + j];
            BYTE *pPixel = (BYTE *)(pScanline + ((Desc.Format.ddpfPixelFormat.dwRGBBitCount/8) * j));

            switch((FMT)Desc.Format.d3dfFormat) {
                case FMT_P8:
                case FMT_A8P8:
                {
                    // !!! gamma support needed in all of these FSurface::Convert*() methods that don't call FColor::Convert*().
                    if(bGammaCorrect )
                        color = FColor(GammaCorrectChannel(color.R),GammaCorrectChannel(color.G),GammaCorrectChannel(color.B),color.A);

                    UINT uMin = 0;
                    FLOAT fDistMin = FLT_MAX;
                    for (UINT u = 0; u < dwNumEntries; u++){
                        FColor cDiff = color - Pal[u];
                        FLOAT fDist = cDiff.A * cDiff.A + cDiff.R * cDiff.R + cDiff.G * cDiff.G + cDiff.B * cDiff.B;

                        if (fDist < fDistMin) {
                            uMin = u;
                            fDistMin = fDist;
                            if (fDistMin == 0.0f)
                                u = dwNumEntries;
                        }
                    }

                    if((FMT)Desc.Format.d3dfFormat == FMT_P8) {
                        *((UINT8 *)pPixel) = (UINT8) uMin;
                    }
                    else {
                        *((UINT16 *)pPixel) = (UINT16) (uMin | (INT(color.A*255.0f) << 8));
                    }
                    break;
                }
                case FMT_R8G8_B8G8:
                case FMT_G8R8_G8B8:
                {
                    DWORD *pdwPixel = (DWORD *)(pScanline + (2 * j));
                    color = m_pData[i * Desc.Width + j];
                    color2 = m_pData[i * Desc.Width + j + 1];

                    // alternative methods can look like something below.  Current method used
                    // since that's what gives results closest to what d3dx does.
                    // R = (color.R + color2.R)/2 or R = color2.R

                    if ((FMT)Desc.Format.d3dfFormat == FMT_R8G8_B8G8) {
                        PACKYUY2(*pdwPixel, 255.f*color.G+.5f, 255.f*color2.G+.5f, 255.f*color.R+.5f, 255.f*color.B+.5f);
                    }
                    else {
                        PACKUYVY(*pdwPixel, 255.f*color.G+.5f, 255.f*color2.G+.5f, 255.f*color.R+.5f, 255.f*color.B+.5f);
                    }
                    j++;
                    break;
                }
                case FMT_YUY2:
                case FMT_UYVY:
                {
                    float y1, y2, u, v, u1, v1, u2, v2;
                    DWORD *pdwPixel = (DWORD *)(pScanline + (2 * j));

                    color = m_pData[i * Desc.Width + j];
                    RGBTOYUV((UINT32)(255.f*color.R+.5f),(UINT32)(255.f*color.G+.5f),(UINT32)(255.f*color.B+.5f),y1,u1,v1);

                    color2 = m_pData[i * Desc.Width + j + 1];
                    RGBTOYUV((UINT32)(255.f*color2.R+.5f),(UINT32)(255.f*color2.G+.5f),(UINT32)(255.f*color2.B+.5f),y2,u2,v2);
                    u = u1; v = v1;
                    // alternative methods.  current method used since
                    // that's what gives results closest to what d3dx does.
                    //u = (u1 + u2)/2.f; v = (v1 + v2)/2.f;
                    //u = u1; v = v2;
                    //u = u2; v = v2;
                    if ((FMT)Desc.Format.d3dfFormat == FMT_YUY2) {
                        PACKYUY2(*pdwPixel, y1, y2, u, v);
                    }
                    else {
                        PACKUYVY(*pdwPixel, y1, y2, u, v);
                    }
                    j++;
                    break;
                }
                default:
                    if (bAdjustColorRange) {
                        (*pfnColorRangeAdjust)(color, &color2);
                        color2.ConvertTo((FMT)Desc.Format.d3dfFormat,.5f,(char *)pPixel,FALSE,ELEMENTINDEX_0);
                    }
                    else {
                        color.ConvertTo((FMT)Desc.Format.d3dfFormat,.5f,(char *)pPixel,FALSE,ELEMENTINDEX_0);
                    }
                    
                    break;
            }
        }
        pScanline += r.iPitch;
    }

    if (FAILED(hr = pSurf->UnlockRect())) {
        DPF(1, _T("FSurface::ConvertTo() - UnlockRect failed. hr=%s(%x).\n"),pSurf->GetCDevice()->GetD3D()->HResultToString(hr), hr);
        goto Exit;
    }

 Exit:
    return hr;
}
