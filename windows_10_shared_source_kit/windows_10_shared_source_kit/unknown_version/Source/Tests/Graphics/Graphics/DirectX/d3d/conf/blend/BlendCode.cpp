#include <algorithm>
#include <cmath>
#include "blend.h"
#include "FormatDesc.hpp"
#include "MinMaxRange.h"

using namespace MinMaxRange;
typedef MinMaxColorClass MinMaxColor;


bool
isNaN(float in)
{
	UINT32 dwInVal = *(UINT32*)(&in);
	if ((dwInVal & 0x7f800000) != 0x7f800000)
		return false;
	if ((dwInVal & 0x007fffff) == 0)
		return false; // +infinity
	return true;
}

INT32
floatToFixed(float in, int iBits, int fBits)
{
	// implemented to 0.5 ULP spec
	// 3.2.3.1 FLOAT -> Fixed Point Integer
	assert(iBits >= 2);
	assert(0 <= fBits && fBits <= 23);
	assert(iBits + fBits <= 24);
	if (isNaN(in))
		return 0;
	double fixedMin = -pow(2.0, iBits-1);
	double fixedMax = pow(2.0, iBits-1) - pow(2.0, -fBits);
	if (in >= fixedMax)
		return (1<<(iBits+fBits-1))-1;
	if (in <= fixedMin)
		return -((1<<(iBits-1))<<fBits);

	const INT32 intIn = (*(INT32*)(&in));

	const INT32 e = (intIn >> 23) & 0xff; // exponent
	const INT32 ue = e - 0x7f; // unbiased exponent
	if (ue < (-fBits - 1))
		return 0;

	const INT32 s = (intIn >> 31) & 0x1; // sign
	INT32 storage = 0;
	// full mantissa
	storage |= intIn & ((1<<23)-1);
	// hidden bit
	storage |= 1 << 23;
	// 2's complement
	if (s)
		storage = (~storage) + 1;

	const int extraBits = (23 - fBits) - ue;
	assert(extraBits >= 0);
	if (extraBits == 0)
		return storage;
	const int roundHalf = 1 << (extraBits-1);
	const int roundVal = storage & ((1<<extraBits)-1);

	int retVal = storage >> extraBits;
	if (roundVal > roundHalf)
		retVal += 1;
	else if (roundVal < roundHalf)
		retVal += 0;
	else
		retVal = (retVal + 1) & (~1); // nearest even
	// overflow on case like 3.5 -> 4.0 with iBits = 3
	assert( retVal < (1<<(iBits+fBits-1))-1 );

	return retVal;
}

float
fixedFraction(INT32 in, int fBits)
{
	INT32 fPart = in & ((1<<fBits)-1);
	return ((float)fPart)/(1<<fBits);
}

INT32
fixedInt(INT32 in, int iBits, int fBits)
{
	INT32 iPart = (in >> fBits);
	assert(-(1<<iBits) <= iPart && iPart < (1<<iBits));
	return iPart;
}

float
fixedToFloat(INT32 in, int iBits, int fBits)
{
	return fixedInt(in, iBits, fBits) + fixedFraction(in, fBits);
}

float
f_fix_quantize(float in, int iBits, int fBits)
{
	INT32 intVal = floatToFixed(in, iBits, fBits);
	float retVal = fixedToFloat(intVal, iBits, fBits);
	return retVal;
}


MinMaxColor
calcBlendCoeff(D3D11_BLEND blend,
			   //TEST_COLOR srcColor0,
			   //TEST_COLOR srcColor1,
			   //TEST_COLOR dstColor,
			   MinMaxColor srcColor0,
			   MinMaxColor srcColor1,
			   MinMaxColor dstColor,
			   MinMaxColor blendFactor)
{
	const TEST_COLOR cZero = {0,0,0,0};
	const TEST_COLOR cOne = {1,1,1,1};
	const MinMaxColor cZeroMinMax = MinMaxColor(cZero);
	const MinMaxColor cOneMinMax = MinMaxColor(cOne);
	MinMaxColor coeff;
	switch (blend)
	{
	default:
		assert(!"Invalid blend type"); break;
	case D3D11_BLEND_ZERO:
		coeff = cZeroMinMax; break;
	case D3D11_BLEND_ONE:
		coeff = cOneMinMax; break;
	case D3D11_BLEND_SRC_COLOR:
		coeff = MinMaxColor(srcColor0); break;
	case D3D11_BLEND_INV_SRC_COLOR:
		coeff = cOneMinMax + MinMaxColor(srcColor0 * -1.0); break;
	case D3D11_BLEND_SRC_ALPHA:
		coeff =      cOneMinMax * (srcColor0.lo.a);
		coeff.expand(cOneMinMax * (srcColor0.hi.a));
		break;
	case D3D11_BLEND_INV_SRC_ALPHA:
		coeff =      cOneMinMax * (1.0f - srcColor0.lo.a); 
		coeff.expand(cOneMinMax * (1.0f - srcColor0.hi.a));
		break;
	case D3D11_BLEND_DEST_ALPHA:
		coeff =      cOneMinMax * (dstColor.lo.a); 
		coeff.expand(cOneMinMax * (dstColor.hi.a));
		break;
	case D3D11_BLEND_INV_DEST_ALPHA:
		coeff =      cOneMinMax * (1.0f - dstColor.lo.a); 
		coeff.expand(cOneMinMax * (1.0f - dstColor.hi.a));
		break;
	case D3D11_BLEND_DEST_COLOR:
		coeff = MinMaxColor(dstColor); break;
	case D3D11_BLEND_INV_DEST_COLOR:
		coeff = cOneMinMax + MinMaxColor(dstColor * -1.0f); break;
	case D3D11_BLEND_SRC_ALPHA_SAT:
		{
			// (f,f,f,1), f = min(1 - RT(N).a, oN.a)
			coeff.lo.r = coeff.lo.g = coeff.lo.b = min(srcColor0.lo.a, 1.0f - dstColor.hi.a);
			coeff.lo.a = 1;
			coeff.hi.r = coeff.hi.g = coeff.hi.b = min(srcColor0.hi.a, 1.0f - dstColor.lo.a);
			coeff.hi.a = 1;
		}
		break;
	case D3D11_BLEND_BLEND_FACTOR:
		coeff = MinMaxColor(blendFactor); break;
	case D3D11_BLEND_INV_BLEND_FACTOR:
		coeff = cOneMinMax + MinMaxColor(blendFactor * -1.0f); break;
		break;
	case D3D11_BLEND_SRC1_COLOR:
		coeff = MinMaxColor(srcColor1); break;
	case D3D11_BLEND_INV_SRC1_COLOR:
		coeff = cOneMinMax + MinMaxColor(srcColor1 * -1.0f); break;
	case D3D11_BLEND_SRC1_ALPHA:
		coeff =      cOneMinMax * (srcColor1.lo.a); 
		coeff.expand(cOneMinMax * (srcColor1.hi.a));
		break;
	case D3D11_BLEND_INV_SRC1_ALPHA:
		coeff =      cOneMinMax * (1.0f - srcColor1.lo.a); 
		coeff.expand(cOneMinMax * (1.0f - srcColor1.hi.a));
		break;
	}
	return coeff;
}

MinMaxColor
doBlendCombine(D3D11_BLEND_OP blendOp,
			   MinMaxColor srcIn,
			   MinMaxColor dstIn)
{
	MinMaxColor retColor;
	switch (blendOp)
	{
	default:
		assert(!"Invalid blend operation"); break;
	case D3D11_BLEND_OP_ADD:
		retColor = srcIn + dstIn; break;
	case D3D11_BLEND_OP_SUBTRACT:
		retColor = srcIn - dstIn; break;
	case D3D11_BLEND_OP_REV_SUBTRACT:
		retColor = dstIn - srcIn; break;
	case D3D11_BLEND_OP_MIN:
		{
			retColor.lo.r = min(srcIn.lo.r, dstIn.lo.r);
			retColor.lo.g = min(srcIn.lo.g, dstIn.lo.g);
			retColor.lo.b = min(srcIn.lo.b, dstIn.lo.b);
			retColor.lo.a = min(srcIn.lo.a, dstIn.lo.a);
			retColor.hi.r = min(srcIn.hi.r, dstIn.hi.r);
			retColor.hi.g = min(srcIn.hi.g, dstIn.hi.g);
			retColor.hi.b = min(srcIn.hi.b, dstIn.hi.b);
			retColor.hi.a = min(srcIn.hi.a, dstIn.hi.a);
		}
		break;
	case D3D11_BLEND_OP_MAX:
		{
			retColor.lo.r = max(srcIn.lo.r, dstIn.lo.r);
			retColor.lo.g = max(srcIn.lo.g, dstIn.lo.g);
			retColor.lo.b = max(srcIn.lo.b, dstIn.lo.b);
			retColor.lo.a = max(srcIn.lo.a, dstIn.lo.a);
			retColor.hi.r = max(srcIn.hi.r, dstIn.hi.r);
			retColor.hi.g = max(srcIn.hi.g, dstIn.hi.g);
			retColor.hi.b = max(srcIn.hi.b, dstIn.hi.b);
			retColor.hi.a = max(srcIn.hi.a, dstIn.hi.a);
		}
		break;
	}
	return retColor;
}

MinMaxColor
doColorOrAlphaOp(D3D11_BLEND_OP blendOp,
				 D3D11_BLEND srcBlend,
				 D3D11_BLEND dstBlend,
				 //TEST_COLOR srcColor0,
				 //TEST_COLOR srcColor1,
				 //TEST_COLOR dstColor,
				 MinMaxColor srcColor0,
				 MinMaxColor srcColor1,
				 MinMaxColor dstColor,
				 MinMaxColor blendFactor)
{
	MinMaxColor retColor;
	if (blendOp != D3D11_BLEND_OP_MIN && blendOp != D3D11_BLEND_OP_MAX)
	{
		MinMaxColor srcBlendCoeff = calcBlendCoeff(srcBlend, srcColor0, srcColor1, dstColor, blendFactor);
		MinMaxColor dstBlendCoeff = calcBlendCoeff(dstBlend, srcColor0, srcColor1, dstColor, blendFactor);

		MinMaxColor srcBlendColor = srcBlendCoeff * srcColor0.lo;
		srcBlendColor.expand(srcBlendCoeff * srcColor0.hi);
		MinMaxColor dstBlendColor = dstBlendCoeff * dstColor.lo;
		dstBlendColor.expand(dstBlendCoeff * dstColor.hi);

		retColor = doBlendCombine(blendOp, srcBlendColor, dstBlendColor);
	}
	else
	{
		retColor = doBlendCombine(blendOp, MinMaxColor(srcColor0), MinMaxColor(dstColor));
	}

	return retColor;
}

MinMaxColor
doWriteMask(MinMaxColor input, 
			UINT writeMask, 
			//TEST_COLOR origColor)
			MinMaxColor origColor)
{
	MinMaxColor retColor(origColor);

	if (writeMask & D3D11_COLOR_WRITE_ENABLE_RED)
	{
		retColor.lo.r = input.lo.r;
		retColor.hi.r = input.hi.r;
	}

	if (writeMask & D3D11_COLOR_WRITE_ENABLE_GREEN)
	{
		retColor.lo.g = input.lo.g;
		retColor.hi.g = input.hi.g;
	}

	if (writeMask & D3D11_COLOR_WRITE_ENABLE_BLUE)
	{
		retColor.lo.b = input.lo.b;
		retColor.hi.b = input.hi.b;
	}

	if (writeMask & D3D11_COLOR_WRITE_ENABLE_ALPHA)
	{
		retColor.lo.a = input.lo.a;
		retColor.hi.a = input.hi.a;
	}

	return retColor;
}

MinMaxColor
doBlendOp(D3D11_BLEND_DESC blendSetup,
		  MinMaxColor srcColor0,
		  MinMaxColor srcColor1,
		  MinMaxColor dstColor,
		  MinMaxColor blendFactor,
		  int nRenderTarget)
{
	assert(nRenderTarget >= 0 && nRenderTarget < 8);
	MinMaxColor retColor = srcColor0;
	switch (g_App.m_D3DOptions.FeatureLevel)
	{
	default:
		assert(!"Unexpected D3D Version");
		break;	
	case D3D_FEATURE_LEVEL_10_0: // 10.0
		if (blendSetup.RenderTarget[nRenderTarget].BlendEnable)
		{

			MinMaxColor rgbPortion = doColorOrAlphaOp(blendSetup.RenderTarget[0].BlendOp, 
				blendSetup.RenderTarget[0].SrcBlend, 
				blendSetup.RenderTarget[0].DestBlend, 
				srcColor0, srcColor1, dstColor, blendFactor);
			MinMaxColor alphaPortion = doColorOrAlphaOp(blendSetup.RenderTarget[0].BlendOpAlpha, 
				blendSetup.RenderTarget[0].SrcBlendAlpha, 
				blendSetup.RenderTarget[0].DestBlendAlpha, 
				srcColor0, srcColor1, dstColor, blendFactor);
			MinMaxColor blendedColor = rgbPortion;
			blendedColor.lo.a = alphaPortion.lo.a;
			blendedColor.hi.a = alphaPortion.hi.a;
			retColor = blendedColor;
		}
		break;
	case D3D_FEATURE_LEVEL_11_1:
	case D3D_FEATURE_LEVEL_11_0: // 11.0
	case D3D_FEATURE_LEVEL_10_1: // 10.1
	case D3D_FEATURE_LEVEL_9_1:
	case D3D_FEATURE_LEVEL_9_2:
	case D3D_FEATURE_LEVEL_9_3:
		{
			int rtNumToUse = blendSetup.IndependentBlendEnable?nRenderTarget:0;
			if (blendSetup.RenderTarget[rtNumToUse].BlendEnable)
			{
				MinMaxColor rgbPortion = doColorOrAlphaOp(blendSetup.RenderTarget[rtNumToUse].BlendOp, 
					blendSetup.RenderTarget[rtNumToUse].SrcBlend, 
					blendSetup.RenderTarget[rtNumToUse].DestBlend, 
					srcColor0, srcColor1, dstColor, blendFactor);
				MinMaxColor alphaPortion = doColorOrAlphaOp(blendSetup.RenderTarget[rtNumToUse].BlendOpAlpha, 
					blendSetup.RenderTarget[rtNumToUse].SrcBlendAlpha, 
					blendSetup.RenderTarget[rtNumToUse].DestBlendAlpha, 
					srcColor0, srcColor1, dstColor, blendFactor);
				MinMaxColor blendedColor = rgbPortion;
				blendedColor.lo.a = alphaPortion.lo.a;
				blendedColor.hi.a = alphaPortion.hi.a;
				retColor = blendedColor;
			}
			else
			{
				retColor = doWriteMask(srcColor0, blendSetup.RenderTarget[rtNumToUse].RenderTargetWriteMask, dstColor);
				return retColor;
			}
		}
		break;
	}
	retColor = doWriteMask(retColor, blendSetup.RenderTarget[nRenderTarget].RenderTargetWriteMask, dstColor);
	return retColor;
}

template <typename ValTypeA, typename ValTypeB>
void
clamp(ValTypeA& inVal, ValTypeB loVal, ValTypeB hiVal)
{
	if (inVal > hiVal)
		inVal = hiVal;
	if (inVal < loVal)
		inVal = loVal;
}

template <typename ValType>
void
clamp_float(ValType& inVal, int floatBits)
{
	switch (floatBits)
	{
	default: assert(!"Unrecognized float format");
	case 32: break;
	case 16: clamp(inVal, -65504.0f, 65504.0f); break;
		// small float formats have no sign bit
	case 11: clamp(inVal, 0.0f, 65024.0f); break;
	case 10: clamp(inVal, 0.0f, 64512.0f); break;
	case 0: break;
	}
}

float
f_quantize(float in, int bits)
{
	if (bits >= 23)
		return in;
	INT64 iVal = (INT64)(in * (1<<bits) + 0.5f);
	double retVal = (((double)iVal)/(1<<bits));
	return (float)retVal;
}

float
f_quantize_special(float in, int bits)
{
	if (isNaN(in))
		return in;
	UINT resFloat = *(UINT*)(&in);
	switch (bits)
	{
	default:
		assert(!"Unrecognized float format");
		break;
	case 32:
	case 0:
		resFloat = *(UINT*)(&in);
		break;
	case 16:
		resFloat = float16::make(&in).toFloat32_AsUINT32Bits();
		break;
	case 11:
		resFloat = float11::make(&in).toFloat32_AsUINT32Bits();
		break;
	case 10:
		resFloat = float10::make(&in).toFloat32_AsUINT32Bits();
		break;
	}
	return *(float*)(&resFloat);
}

float
unorm_quantize(float in, int bits)
{
	if (bits >= 23)
		return in;
	INT64 iVal = (INT64)(in * ((1<<bits)-1) + 0.5f);
	double retVal = (((double)iVal)/((1<<bits)-1));
	return (float)retVal;
}

float
snorm_quantize(float in, int bits)
{
	if (bits >= 23)
		return in;
	bits -= 1;
	INT64 iVal = (INT64)(in * ((1<<bits)-1) + 0.5f);
	double retVal = (((double)iVal)/((1<<bits)-1));
	return (float)retVal;
}

TEST_COLOR
doFormatClamp(const TEST_COLOR& inColor,
			  const DXGIFormatInfo* pTexFormatInfo)
{
	TEST_COLOR outColor = inColor;
	if (pTexFormatInfo->dwFlags & FF_UNORM)
	{
		clamp(outColor.r, 0.0f, 1.0f);
		clamp(outColor.g, 0.0f, 1.0f);
		clamp(outColor.b, 0.0f, 1.0f);
		clamp(outColor.a, 0.0f, 1.0f);
	}
	else if (pTexFormatInfo->dwFlags & FF_SNORM)
	{
		clamp(outColor.r, -1.0f, 1.0f);
		clamp(outColor.g, -1.0f, 1.0f);
		clamp(outColor.b, -1.0f, 1.0f);
		clamp(outColor.a, -1.0f, 1.0f);
	}
	else if (pTexFormatInfo->dwFlags & FF_FLOAT)
	{
		clamp_float(outColor.r, pTexFormatInfo->pBitsPerComponent[0]);
		clamp_float(outColor.g, pTexFormatInfo->pBitsPerComponent[1]);
		clamp_float(outColor.b, pTexFormatInfo->pBitsPerComponent[2]);
		clamp_float(outColor.a, pTexFormatInfo->pBitsPerComponent[3]);
	}
	else
	{
		assert(!"Unexpected format");
	}
	return outColor;
}

TEST_COLOR
doFormatQuantize(const TEST_COLOR& inColor,
				 const DXGIFormatInfo* pTexFormatInfo)
{
	TEST_COLOR outColor = inColor;
	if (pTexFormatInfo->dwFlags & FF_UNORM)
	{
		outColor.r = unorm_quantize((float)outColor.r, pTexFormatInfo->pBitsPerComponent[0]);
		outColor.g = unorm_quantize((float)outColor.g, pTexFormatInfo->pBitsPerComponent[1]);
		outColor.b = unorm_quantize((float)outColor.b, pTexFormatInfo->pBitsPerComponent[2]);
		outColor.a = unorm_quantize((float)outColor.a, pTexFormatInfo->pBitsPerComponent[3]);
	}
	else if (pTexFormatInfo->dwFlags & FF_SNORM)
	{
		outColor.r = snorm_quantize((float)outColor.r, pTexFormatInfo->pBitsPerComponent[0]);
		outColor.g = snorm_quantize((float)outColor.g, pTexFormatInfo->pBitsPerComponent[1]);
		outColor.b = snorm_quantize((float)outColor.b, pTexFormatInfo->pBitsPerComponent[2]);
		outColor.a = snorm_quantize((float)outColor.a, pTexFormatInfo->pBitsPerComponent[3]);
	}
	else if (pTexFormatInfo->dwFlags & FF_FLOAT)
	{
		outColor.r = f_quantize_special((float)outColor.r, pTexFormatInfo->pBitsPerComponent[0]);
		outColor.g = f_quantize_special((float)outColor.g, pTexFormatInfo->pBitsPerComponent[1]);
		outColor.b = f_quantize_special((float)outColor.b, pTexFormatInfo->pBitsPerComponent[2]);
		outColor.a = f_quantize_special((float)outColor.a, pTexFormatInfo->pBitsPerComponent[3]);
	}
	else
	{
		assert(!"Unexpected format");
	}
	return outColor;
}

TEST_COLOR
doFormatDefault(const TEST_COLOR& inColor,
				const DXGIFormatInfo* pTexFormatInfo)
{
	TEST_COLOR outColor = inColor;
	if (0 == (pTexFormatInfo->dwFlags & FF_R) || 0 == pTexFormatInfo->pBitsPerComponent[0])
		outColor.r = 0;
	if (0 == (pTexFormatInfo->dwFlags & FF_G) || 0 == pTexFormatInfo->pBitsPerComponent[1])
		outColor.g = 0;
	if (0 == (pTexFormatInfo->dwFlags & FF_B) || 0 == pTexFormatInfo->pBitsPerComponent[2])
		outColor.b = 0;
	if (0 == (pTexFormatInfo->dwFlags & FF_A) || 0 == pTexFormatInfo->pBitsPerComponent[3])
		outColor.a = 1;
	return outColor;
}

MinMaxColor
doFormatClampDefaultQuantize(const MinMaxColor& inColor,
							 const DXGIFormatInfo* pTexFormatInfo)
{
	TEST_COLOR outColorLo = doFormatDefault(doFormatQuantize(doFormatClamp(inColor.lo, pTexFormatInfo), pTexFormatInfo), pTexFormatInfo);
	TEST_COLOR outColorHi = doFormatDefault(doFormatQuantize(doFormatClamp(inColor.hi, pTexFormatInfo), pTexFormatInfo), pTexFormatInfo);
	return MinMaxColor(outColorLo, outColorHi);
}

MinMaxColor
doFormatConversion(const MinMaxColor& inColor,
				   const DXGIFormatInfo* pTexFormatInfo)
{
	return doFormatClampDefaultQuantize(inColor, pTexFormatInfo);
}

// For floating point formats, the pre-blend clamp is optional
// This returns a MinMaxColor that represents both choices (with and without the pre-blend clamp)
// for non-floating point formats, this returns the clamped value only (because the pre-blend clamp is required in this case)
MinMaxColor
doFormatClampTightAndLoose(const TEST_COLOR& inColor,
						   const DXGIFormatInfo* pTexFormatInfo)
{
	TEST_COLOR clampedColor = doFormatClamp(inColor, pTexFormatInfo);

	if(pTexFormatInfo->dwFlags & FF_FLOAT)
	{
		//
		// The R11G11B10_FLOAT format is unique.  It does not have an alpha channel
		// and the hardware is allowed to clamp negative values in all 4 channels (including alpha) to 0.0 if it chooses.
		//
		if(DXGI_FORMAT_R11G11B10_FLOAT == pTexFormatInfo->Format)
		{
			clampedColor.a = max(0.0f, clampedColor.a); 
		}

		return MinMaxColor(inColor, clampedColor);
	}
	else
	{
		return MinMaxColor(clampedColor, clampedColor);
	}
}

const DXGIFormatInfo*
findFormatInfo(DXGI_FORMAT format)
{
	return CFormatInfoMap::Find(format);
}

static TEST_VEC4F interpolateColors(const TEST_VEC4F inputColors[4], float dx, float dy)
{
	TEST_VEC4F topVals = inputColors[0] * (1-dx) + inputColors[1] * dx;
	TEST_VEC4F botVals = inputColors[3] * (1-dx) + inputColors[2] * dx;
	TEST_VEC4F finalVal = topVals * (1-dy) + botVals * (dy);

	return finalVal;
}

TEST_RESULT
CBlendConfTest::TestDrawnPixels(const TEST_VEC4F dstColors[4], 
								const TEST_VEC4F srcColors[2][4], 
								const TEST_VEC4F& inBlendFactor)
{
	SetD3DFeatureLevel( GetDevice()->GetFeatureLevel() );

	// basic setup:
	// dst = horizontal gradient, 0 at left, 1 at right
	// src0 = vertical gradient, 0 at top, 1 at bottom
	// src1 = diagonal gradient, 0 at top-left, 1 at bottom-right
	bool bCoordReport = false;
	TEST_RESULT tRes = RESULT_PASS;
	TEST_VEC4F dstDelta[2][2], srcDelta[2][2][2];
	dstDelta[0][0] = dstColors[1] - dstColors[0];
	dstDelta[0][1] = dstColors[2] - dstColors[1];

	dstDelta[1][0] = dstColors[2] - dstColors[3];
	dstDelta[1][1] = dstColors[3] - dstColors[0];

	srcDelta[0][0][0] = srcColors[0][1] - srcColors[0][0];
	srcDelta[0][0][1] = srcColors[0][2] - srcColors[0][1];
	srcDelta[0][1][0] = srcColors[1][1] - srcColors[1][0];
	srcDelta[0][1][1] = srcColors[1][2] - srcColors[1][1];

	srcDelta[1][0][0] = srcColors[0][2] - srcColors[0][3];
	srcDelta[1][0][1] = srcColors[0][3] - srcColors[0][0];
	srcDelta[1][1][0] = srcColors[1][2] - srcColors[1][3];
	srcDelta[1][1][1] = srcColors[1][3] - srcColors[1][0];
	int numFailed = 0;
	int x,y,rtNum;
	for (rtNum = 0; rtNum < m_NumRTs; ++rtNum)
	{
		if (m_bFL9X)
		{
			// FL 9.3 only supports 4 render targets
			// FL 9.1 and 9.2 only support 1 render target
			const INT maxRTs = (GetDevice()->GetFeatureLevel() >= D3D_FEATURE_LEVEL_9_3) ? 4 : 1;
			if( rtNum >= maxRTs )
			{
				break;
			}
		}

		// clamp all inputs to render target format
		DXGI_FORMAT rtFormat = m_eRTFormatsEffectiveRTV[rtNum];
		MinMaxColor::SetPrecisionFormat(rtFormat);
		const DXGIFormatInfo* pTexFormatInfo = findFormatInfo(rtFormat);
		for (y = 0; y < m_nPrimitiveH; ++y)
		{
			for (x = 0; x < m_nPrimitiveW; ++x)
			{
				assert(m_nPrimitiveW == m_nPrimitiveH);
				int primNum = (x<y)?1:0;

				TEST_COLOR srcColor0, srcColor1, dstColor;
				ZeroMemory(&srcColor0, sizeof(srcColor0));
				ZeroMemory(&srcColor1, sizeof(srcColor1));
				ZeroMemory(&dstColor, sizeof(dstColor));

				const TEST_COLOR fixedColor1 = {1,1,1,1};
				float xPos = x+0.5f;
				float yPos = y+0.5f;
				const float dx = xPos / m_nPrimitiveW;
				const float dy = yPos / m_nPrimitiveH;
				dstColor = vec2color(interpolateColors(dstColors, dx, dy)) + fixedColor1*(rtNum*0.125);
				srcColor0 = vec2color(interpolateColors(srcColors[0], dx, dy)) + fixedColor1*(rtNum*0.125);
				srcColor1 = vec2color(interpolateColors(srcColors[1], dx, dy));

				TEST_COLOR blendFactor = vec2color(inBlendFactor);

				MinMaxColor srcColor0_MinMax = doFormatClampTightAndLoose(srcColor0, pTexFormatInfo);
				MinMaxColor srcColor1_MinMax = doFormatClampTightAndLoose(srcColor1, pTexFormatInfo);
				MinMaxColor blendFactor_MinMax = doFormatClampTightAndLoose(blendFactor, pTexFormatInfo);
				MinMaxColor dstColor_MinMax = MinMaxColor(dstColor);

				for (int n = 0; n < 4; ++n)
				{
					if (pTexFormatInfo->dwFlags & FF_UNORM || pTexFormatInfo->dwFlags & FF_SNORM)
					{
						int numBits = pTexFormatInfo->pBitsPerComponent[n];
						if (pTexFormatInfo->dwFlags & FF_SNORM)
							numBits -= 1;
						numBits = max(0, numBits);
						MinMaxDbl colorRange = MinMaxDbl::floatULPDrift(dstColor.f[n], GetFixedFloatULPVariance(), numBits, true);
						dstColor_MinMax.lo.f[n] = colorRange.lo;
						dstColor_MinMax.hi.f[n] = colorRange.hi;
					}
				}

				const int UlpDeviation = 4;
				for (int n = 0; n < UlpDeviation; ++n)
				{
					srcColor0_MinMax *= 1.0f;
					srcColor1_MinMax *= 1.0f;
					dstColor_MinMax  *= 1.0f;
				}

				dstColor_MinMax = doFormatConversion(dstColor_MinMax, pTexFormatInfo);

				//MinMaxColor expectColor = doBlendOp(m_BlendDesc, srcColor0_Loose, srcColor1_Loose, dstColor, blendFactor_Loose, rtNum );
				MinMaxColor expectColor = doBlendOp(m_BlendDesc_1, srcColor0_MinMax, srcColor1_MinMax, dstColor_MinMax, blendFactor_MinMax, rtNum );
				MinMaxColor expectColorClamped = doFormatConversion(expectColor, pTexFormatInfo);

				TEST_COLOR pixelColor;
				HRESULT hr = ReadBackBufferColor(x,y,rtNum,&pixelColor);
				TEST_COLOR ulpDiffs;
				//if (!TestColorsSame(expectColorClamped, pixelColor, m_eRTFormats[rtNum], 3.0f, ulpDiffs))
				if (!expectColorClamped.containsULP(pixelColor, pTexFormatInfo, 3.0, ulpDiffs))
				{
					// TODO: don't fail dual-source blend mismatch on multiple render-targets
					tRes = RESULT_FAIL;
					if (!bCoordReport)
					{
						bCoordReport = true;
						WriteToLog( _T("Vertex colors:\n") );

						WriteToLog( _T("Dest:\n") );
						WriteToLog( _T("0 = (%g,%g,%g,%g) 1 = (%g,%g,%g,%g)\n"), 
							dstColors[0].x, dstColors[0].y, dstColors[0].z, dstColors[0].w,
							dstColors[1].x, dstColors[1].y, dstColors[1].z, dstColors[1].w);
						WriteToLog( _T("3 = (%g,%g,%g,%g) 2 = (%g,%g,%g,%g)\n"), 
							dstColors[3].x, dstColors[3].y, dstColors[3].z, dstColors[3].w,
							dstColors[2].x, dstColors[2].y, dstColors[2].z, dstColors[2].w );

						WriteToLog( _T("Src0:\n") );
						WriteToLog( _T("0 = (%g,%g,%g,%g) 1 = (%g,%g,%g,%g)\n"), 
							srcColors[0][0].x, srcColors[0][0].y, srcColors[0][0].z, srcColors[0][0].w,
							srcColors[0][1].x, srcColors[0][1].y, srcColors[0][1].z, srcColors[0][1].w);
						WriteToLog( _T("3 = (%g,%g,%g,%g) 2 = (%g,%g,%g,%g)\n"), 
							srcColors[0][3].x, srcColors[0][3].y, srcColors[0][3].z, srcColors[0][3].w,
							srcColors[0][2].x, srcColors[0][2].y, srcColors[0][2].z, srcColors[0][2].w );

						WriteToLog( _T("Src1:\n") );
						WriteToLog( _T("0 = (%g,%g,%g,%g) 1 = (%g,%g,%g,%g)\n"), 
							srcColors[1][0].x, srcColors[1][0].y, srcColors[1][0].z, srcColors[1][0].w,
							srcColors[1][1].x, srcColors[1][1].y, srcColors[1][1].z, srcColors[1][1].w);
						WriteToLog( _T("3 = (%g,%g,%g,%g) 2 = (%g,%g,%g,%g)\n"), 
							srcColors[1][3].x, srcColors[1][3].y, srcColors[1][3].z, srcColors[1][3].w,
							srcColors[1][2].x, srcColors[1][2].y, srcColors[1][2].z, srcColors[1][2].w );

						WriteToLog( _T("Blend Factor:\n") );
						WriteToLog( _T("0 = (%g,%g,%g,%g)\n"), blendFactor.r, blendFactor.g, blendFactor.b, blendFactor.a );
					}
					bool bLogAllPixels = g_App.LogAllPixelFailures();
					if (bLogAllPixels || numFailed == 0)
					{
						TEST_COLOR exLo = expectColorClamped.lo;
						TEST_COLOR exHi = expectColorClamped.hi;
						TCHAR expectedBuffer[1024];
						if (expectColorClamped.isSingular())
						{
							_sntprintf( expectedBuffer, sizeof( expectedBuffer ) - 1, 
								_T( "(%.10g,%.10g,%.10g,%.10g)" ), 
								exLo.r, exLo.g, exLo.b, exLo.a
								);
						}
						else
						{
							_sntprintf( expectedBuffer, sizeof( expectedBuffer ) - 1, 
								_T( "(%.10g,%.10g,%.10g,%.10g)-(%.10g,%.10g,%.10g,%.10g)" ), 
								exLo.r, exLo.g, exLo.b, exLo.a, 
								exHi.r, exHi.g, exHi.b, exHi.a 
								);
						}
						expectedBuffer[1023] = 0;
						WriteToLog( _T("Color mismatch at RT#%d(x,y)=(%d,%d). Expected %s, found (%.10g,%.10g,%.10g,%.10g) - ULP Diff (%g,%g,%g,%g).\n"), 
							rtNum, x, y, 
							expectedBuffer, 
							pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a,
							ulpDiffs.r, ulpDiffs.g, ulpDiffs.b, ulpDiffs.a
							);
					}
					else if (numFailed == 2 && !bLogAllPixels)
					{
						WriteToLog( _T("More pixels failed. Use -AllPixels to log them\n") );
					}
					numFailed += 1;
				}

			}
		}
	}

	if (numFailed > 0)
		WriteToLog( _T("%s %d pixels failed, of %d total."), LOG_HEADER, numFailed, m_nPrimitiveH*m_nPrimitiveW );

	return tRes;
}