15. + fRoundOffset);
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
                        *((INT16*)pSurfaceBits+1) = ((