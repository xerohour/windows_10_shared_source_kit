#include <algorithm>
#include <vector>
#include "filter.h"

using namespace MinMaxRange;

const float c_EllipticalTransformDeviation = 0.375f;
const float c_CubeDerivativeDeviation = 1.5f;
const float c_LengthApproxTolerance = 11.0f/128;

typedef std::pair<int,float> texelWeight;
typedef MinMaxRangeTemplate<texelWeight> MinMaxTexelWeight;

typedef MinMaxColorClass MinMaxColor;


struct FilterHardwareSpecs
{
	bool FixedPt6ULP;
	bool RoundNearestEven;
	int SubTexelPrecision;
};

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

bool
isINF(float in)
{
	UINT32 dwInVal = *(UINT32*)(&in);
	if ((dwInVal & 0x7fffffff) != 0x7f800000)
		return false;
	return true;
}

int
f_sign(double in)
{
	if (in >= 0)
		return 1;
	return -1;
}

double
f_quantize(double in, int bits)
{
	if (isNaN(float(in)) || isINF(float(in)))
		return in;
	INT64 iVal = (INT64)((in - (int)in) * ((INT64)1 << bits) + 0.5*f_sign(in));
	return (int)in + ((double)iVal)/((INT64)1 << bits);
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

MinMaxInt
floatToFixedULP(float in, int iBits, int fBits, float ulpVariance)
{
	float floatVariance = ulpVariance * 0.5f/(1<<fBits);

	return MinMaxInt(
		floatToFixed(in - floatVariance, iBits, fBits),
		floatToFixed(in + floatVariance, iBits, fBits)
		);
}

MinMaxInt
floatToFixedULP(float in, int iBits, int fBits)
{
	return floatToFixedULP(in, iBits, fBits, GetFixedFloatULPVariance());
}

MinMaxFloat
fixedFractionULP(MinMaxInt in, int fBits, float ulpVariance)
{
	float floatVariance = ulpVariance * 0.5f/(1<<fBits);

	MinMaxFloat retVal(
		fixedFraction(in.lo, fBits) - floatVariance, 
		fixedFraction(in.lo, fBits) + floatVariance);

	if (!in.isSingular())
		retVal.expand(fixedFraction(in.hi, fBits) + floatVariance);

	return retVal;
}

MinMaxFloat
fixedFraction(MinMaxInt in, int fBits)
{
	float ulpVariance = GetFixedFloatULPVariance();
	return fixedFractionULP(in, fBits, ulpVariance);
}

MinMaxInt
fixedIntULP(MinMaxInt in, int iBits, int fBits, float ulpVariance)
{
	MinMaxInt retVal(fixedInt(in.lo, iBits, fBits));

	if (!in.isSingular())
		retVal.expand(fixedInt(in.hi,iBits,fBits));

	if (fBits < 23 && fixedFraction(in.lo, fBits) == 0)
		retVal.lo -= 1;

	return retVal;
}

MinMaxInt
fixedInt(MinMaxInt in, int iBits, int fBits)
{
	return fixedIntULP(in, iBits, fBits, GetFixedFloatULPVariance());
}

void
testFloatFixed()
{
#define testFloatFixVal(i,f,in,out) if (floatToFixed(in,i,f) == out) {} else DebugBreak()
	// rounding
	testFloatFixVal(3,2,  3.125,(3<<2)+0); // => 3.0
	testFloatFixVal(3,2,  3.250,(3<<2)+1); // => 3.25
	testFloatFixVal(3,2,  3.370f,(3<<2)+1); // => 3.25
	testFloatFixVal(3,2,  3.375,(3<<2)+2); // => 3.5
	testFloatFixVal(3,2,  3.5,	(3<<2)+2); // => 3.5
	testFloatFixVal(3,2,  3.625,(3<<2)+2); // => 3.5
	testFloatFixVal(3,2,  3.875,(3<<2)+3); // => 3.75
	// negatives
	testFloatFixVal(3,2,  -4.5,	(-4<<2)+0);
	testFloatFixVal(3,2,  -4.0,	(-4<<2)+0);
	testFloatFixVal(3,2,  -3.9f,(-4<<2)+0);
	testFloatFixVal(3,2,  -3.5,	(-4<<2)+2);
	testFloatFixVal(3,2,  -1.0,	(-1<<2)+0);
	testFloatFixVal(3,2,  -0.25,(-1<<2)+3);
	testFloatFixVal(3,2,  -0.125,0); // => 0
	testFloatFixVal(18,6, -0.0,	(0<<6)+0);
	// 24-bit fixed
	testFloatFixVal(18,6, 0.0,	(0<<6)+0);
	testFloatFixVal(18,6, 0.5,	(0<<6)+32);
	testFloatFixVal(18,6, 1.0,	(1<<6)+0);
	testFloatFixVal(18,6, 1.5,	(1<<6)+32);
	testFloatFixVal(18,6, 1.97f,(1<<6)+62);
	testFloatFixVal(18,6, 1.98f,(1<<6)+63);
	testFloatFixVal(18,6, 1.99f,(1<<6)+63);
	testFloatFixVal(18,6, 2.0,	(2<<6)+0);
	testFloatFixVal(18,6, 131071.5,	(131071<<6)+32);
	testFloatFixVal(18,6, 131071.97f,	(131071<<6)+62);
	testFloatFixVal(18,6, 131071.98f,	(131071<<6)+62);
	testFloatFixVal(18,6, 131071.981f,	(131071<<6)+63);
	testFloatFixVal(18,6, 131071.99f,	(131071<<6)+63);
	testFloatFixVal(18,6, 131072,	(131071<<6)+63);
#undef testFloatFixVal
#define testFixFloatVal(i,f,out,in) if (fixedToFloat(in,i,f) == out) {} else DebugBreak()
	// rounding
	testFixFloatVal(3,2,  3.0,	(3<<2)+0);
	testFixFloatVal(3,2,  3.25,	(3<<2)+1);
	testFixFloatVal(3,2,  3.5,	(3<<2)+2);
	testFixFloatVal(3,2,  3.75,	(3<<2)+3);
	// negatives
	testFixFloatVal(3,2,  -4.0,	(-4<<2)+0);
	testFixFloatVal(3,2,  -3.5,	(-4<<2)+2);
	testFixFloatVal(3,2,  -1.0,	(-1<<2)+0);
	testFixFloatVal(3,2,  -0.25,(-1<<2)+3);
	testFixFloatVal(18,6, -0.0,	(0<<6)+0);
	// 24-bit fixed
	testFixFloatVal(18,6, 0.0,	(0<<6)+0);
	testFixFloatVal(18,6, 0.5,	(0<<6)+32);
	testFixFloatVal(18,6, 1.0,	(1<<6)+0);
	testFixFloatVal(18,6, 1.5,	(1<<6)+32);
	testFixFloatVal(18,6, 1.96875f,(1<<6)+62);
	testFixFloatVal(18,6, 1.984375f,(1<<6)+63);
	testFixFloatVal(18,6, 2.0,	(2<<6)+0);
	testFixFloatVal(18,6, 131071.5,	(131071<<6)+32);
	testFixFloatVal(18,6, 131071.96875f,	(131071<<6)+62);
	testFixFloatVal(18,6, 131071.984375f,	(131071<<6)+63);
#undef testFixFloatVal
}

float
f_clamp(float in, float epsilon)
{
	if (fabsf(in) < epsilon)
		return 0;
	return in;
}

double round_z(double f){if (f > 0) return floor(f); else return ceil(f);}
double round_ni(double f){return floor(f);}
double signedFrac(double f){ return (f - round_z(f));} // round_z : "round towards zero"
double frac(double f){ return (f - round_ni(f));}      // round_ni : "round towards negative infinity"

double
doReduceRange(double in, D3D11_TEXTURE_ADDRESS_MODE addressMode)
{
	switch (addressMode)
	{
	default:
		assert(!"Invalid address mode");
		break;
	case D3D11_TEXTURE_ADDRESS_WRAP:
		// The reduced range is [0, 1)
		return frac(in);
	case D3D11_TEXTURE_ADDRESS_MIRROR:
		// The reduced range is (-2, 2)
		return signedFrac(in/2) * 2;
	case D3D11_TEXTURE_ADDRESS_MIRROR_ONCE:
	case D3D11_TEXTURE_ADDRESS_CLAMP:
	case D3D11_TEXTURE_ADDRESS_BORDER:
		// The reduced range is [-10, 10].
		// Each of these modes might use different tightnesses of reduced range,
		// but since there really is no benefit in that, a one-size-fits-all
		// approach is taken here.
		// Note that the range leaves room for immediate texel-space offsets
		// supported by sample instructions, [-8...7],
		// preventing these offsets from causing texcoords that clearly should
		// be out of range (i.e. in border/clamp region) from falling within
		// range after range reduction.  The point is that range reduction does
		// not have an affect on the texels that are supposed to be chosen.
		if(in <= -10)
			return -10;
		else if(in >= 10)
			return 10;
		else
			return in;
	}
	return 0;
}

INT32
doTexClamp(INT32 scaledU, INT32 numTexelsU, D3D11_TEXTURE_ADDRESS_MODE addressU, bool& bUseBorderColor)
{
	assert(numTexelsU > 0 && numTexelsU < (1<<18));
	if ((scaledU < 0) || (scaledU > numTexelsU-1))
	{
		switch (addressU)
		{
		case D3D11_TEXTURE_ADDRESS_WRAP:
			scaledU = scaledU % numTexelsU;
			if(scaledU < 0)
				scaledU += numTexelsU;
			break;
		case D3D11_TEXTURE_ADDRESS_MIRROR:
			{
				if(scaledU < 0)
					scaledU = -scaledU - 1;
				bool Flip = (scaledU/numTexelsU) & 1;
				scaledU %= numTexelsU;
				if( Flip ) // Odd tile
					scaledU = numTexelsU - scaledU - 1;
				break;
			}
		case D3D11_TEXTURE_ADDRESS_CLAMP:
			scaledU = max( 0, min( scaledU, numTexelsU - 1 ) );
			break;
		case D3D11_TEXTURE_ADDRESS_MIRROR_ONCE:
			if(scaledU < 0)
				scaledU = -scaledU - 1;
			scaledU = max( 0, min( scaledU, numTexelsU - 1 ) );
			break;
		case D3D11_TEXTURE_ADDRESS_BORDER:
			// Special case: Instead of fetching from the texture,
			// use the Border Color.
			bUseBorderColor = true;
			break;
		default:
			assert(!"Invalid address mode");
			scaledU = 0;
			break;
		}
	}
	assert (bUseBorderColor || !((scaledU < 0) || (scaledU > numTexelsU-1)));
	return scaledU;
}

MinMaxInt
doTexClamp(MinMaxInt scaledU, INT32 numTexelsU, D3D11_TEXTURE_ADDRESS_MODE addressU, MinMaxBool& bUseBorderColor)
{
	if (scaledU.isSingular())
	{
		MinMaxInt retVal(doTexClamp(scaledU.lo, numTexelsU, addressU, bUseBorderColor.lo));
		if (bUseBorderColor.lo)
			bUseBorderColor.hi = true;
		return retVal;
	}
	bool bBorderColor[2] = {false,false};
	MinMaxInt retVal(doTexClamp(scaledU.lo, numTexelsU, addressU, bBorderColor[0]));
	retVal.expand(doTexClamp(scaledU.hi, numTexelsU, addressU, bBorderColor[1]));
	if (bBorderColor[0] && bBorderColor[1])
		bUseBorderColor = MinMaxBool(true);
	else if (bBorderColor[0] || bBorderColor[1])
		bUseBorderColor.expand(true);

	return retVal;
}

enum eFilterMode_t
{
	eFilter_Invalid,
	eFilter_Point,
	eFilter_Linear,
	eFilter_Aniso,
};

int
doTexAddressPt(float in, 
			   INT32 texSize, 
			   D3D11_TEXTURE_ADDRESS_MODE addressMode, 
			   bool& bUseBorderColor, 
			   int immOffset, 
			   FilterHardwareSpecs* pHardwareSpecs)
{
	assert(pHardwareSpecs->SubTexelPrecision >= 6);
	assert(-8 <= immOffset && immOffset <= 7);
	/*
	Given a 1D texture coordinate in normalized space U, assumed to be any float32 value. 
	U is scaled by the Texture1D size. Call this scaledU 
	scaledU is converted to at least 18.6 Fixed Point. Call this fxpScaledU. 
	The integer part of fxpScaledU is the chosen texel. Call this t. Note that the conversion to Fixed Point basically accomplished: t = floor(scaledU). 
	If t is outside [0...numTexels-1] range, D3D10_SAMPLER_STATE's AddressU mode is applied. 
	*/
	double newIn = doReduceRange(in, addressMode);
	double scaledU = newIn * texSize;
	INT32 fxpScaledU = floatToFixed((float)scaledU, 16, pHardwareSpecs->SubTexelPrecision);
	INT32 texel = fixedInt(fxpScaledU, 16, pHardwareSpecs->SubTexelPrecision);
	float fraction = fixedFraction(fxpScaledU, pHardwareSpecs->SubTexelPrecision);

	texel += immOffset;
	return doTexClamp(texel, texSize, addressMode, bUseBorderColor);
}

texelWeight
doTexAddressLnr(float in, INT32 texSize, 
				D3D11_TEXTURE_ADDRESS_MODE addressMode, 
				int immOffset, 
				FilterHardwareSpecs* pHardwareSpecs)
{
	assert(pHardwareSpecs->SubTexelPrecision >= 6);
	assert(-8 <= immOffset && immOffset <= 7);
	/*
	Given a 1D texture coordinate in normalized space U, assumed to be any float32 value. 
	U is scaled by the Texture1D size, and 0.5f is subtracted. Call this scaledU. 
	scaledU is converted to at least 18.6 Fixed Point. Call this fxpScaledU. 
	The integer part of fxpScaledU is the chosen left texel. Call this tFloorU. Note that the conversion to Fixed Point basically accomplished: tFloorU = floor(scaledU). 
	The right texel, tCeilU is simply tFloorU + 1. 
	The weight value wCeilU is assigned the fractional part of fxpScaledU, converted to float. 
	The weight value wFloorU is 1.0f - wCeilU. 
	If tFloorU or tCeilU are out of range of the texture, D3D10_SAMPLER_STATE's AddressU mode is applied to each individually. 
	Since more than one texel is chosen, the single sample result is computed as:
	texelFetch(tFloorU) * wFloorU +
	texelFetch( tCeilU) *  wCeilU
	*/
	double newIn = doReduceRange(in, addressMode);
	double scaledU = newIn * texSize;
	scaledU -= 0.5;
	INT32 fxpScaledU = floatToFixed((float)scaledU, 16, pHardwareSpecs->SubTexelPrecision);
	INT32 texel = fixedInt(fxpScaledU, 16, pHardwareSpecs->SubTexelPrecision);
	float fraction = fixedFraction(fxpScaledU, pHardwareSpecs->SubTexelPrecision);

	return texelWeight(texel + immOffset, fraction);
}

MinMaxInt
doTexAddressPtMinMax(float in, 
					 INT32 texSize, 
					 D3D11_TEXTURE_ADDRESS_MODE addressMode, 
					 MinMaxBool& bUseBorderColor, 
					 int immOffset, 
					 FilterHardwareSpecs* pHardwareSpecs)
{
	assert(pHardwareSpecs->SubTexelPrecision >= 6);
	assert(-8 <= immOffset && immOffset <= 7);
	/*
	Given a 1D texture coordinate in normalized space U, assumed to be any float32 value. 
	U is scaled by the Texture1D size. Call this scaledU 
	scaledU is converted to at least 18.6 Fixed Point. Call this fxpScaledU. 
	The integer part of fxpScaledU is the chosen texel. Call this t. Note that the conversion to Fixed Point basically accomplished: t = floor(scaledU). 
	If t is outside [0...numTexels-1] range, D3D10_SAMPLER_STATE's AddressU mode is applied. 
	*/
	MinMaxDbl addressInRange = MinMaxDbl::floatULPDrift(in, GetFloatULPVariance(32));
	MinMaxDbl newIn = MinMaxDbl
		( doReduceRange(addressInRange.lo, addressMode)
		, doReduceRange(addressInRange.hi, addressMode));
	MinMaxDbl scaledU = newIn * texSize;

	MinMaxDbl texelRange = MinMaxDbl
		( MinMaxDbl::floatULPDrift(scaledU.lo, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true).lo
		, MinMaxDbl::floatULPDrift(scaledU.hi, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true).hi);
	MinMaxInt texel((int)round_ni(texelRange.lo), (int)round_ni(texelRange.hi));
	texel += immOffset;
	return doTexClamp(texel, texSize, addressMode, bUseBorderColor);
}

MinMaxTexelWeight
doTexAddressLnrMinMax(float in, INT32 texSize, 
					  D3D11_TEXTURE_ADDRESS_MODE addressMode, 
					  int immOffset, 
					  FilterHardwareSpecs* pHardwareSpecs)
{
	assert(pHardwareSpecs->SubTexelPrecision >= 6);
	assert(-8 <= immOffset && immOffset <= 7);
	/*
	Given a 1D texture coordinate in normalized space U, assumed to be any float32 value. 
	U is scaled by the Texture1D size, and 0.5f is subtracted. Call this scaledU. 
	scaledU is converted to at least 18.6 Fixed Point. Call this fxpScaledU. 
	The integer part of fxpScaledU is the chosen left texel. Call this tFloorU. Note that the conversion to Fixed Point basically accomplished: tFloorU = floor(scaledU). 
	The right texel, tCeilU is simply tFloorU + 1. 
	The weight value wCeilU is assigned the fractional part of fxpScaledU, converted to float. 
	The weight value wFloorU is 1.0f - wCeilU. 
	If tFloorU or tCeilU are out of range of the texture, D3D10_SAMPLER_STATE's AddressU mode is applied to each individually. 
	Since more than one texel is chosen, the single sample result is computed as:
	texelFetch(tFloorU) * wFloorU +
	texelFetch( tCeilU) *  wCeilU
	*/
	MinMaxDbl addressInRange = MinMaxDbl::floatULPDrift(in, GetFloatULPVariance(32));
	MinMaxDbl newIn = MinMaxDbl
		( doReduceRange(addressInRange.lo, addressMode)
		, doReduceRange(addressInRange.hi, addressMode));
	MinMaxDbl scaledU = newIn * texSize;
	scaledU -= 0.5;

	MinMaxDbl texelRange = MinMaxDbl
		( MinMaxDbl::floatULPDrift(scaledU.lo, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true).lo
		, MinMaxDbl::floatULPDrift(scaledU.hi, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true).hi);
	return MinMaxTexelWeight(
		texelWeight((int)round_ni(texelRange.lo) + immOffset, (float)frac(texelRange.lo)),
		texelWeight((int)round_ni(texelRange.hi) + immOffset, (float)frac(texelRange.hi))
		);
}

std::vector<int>
doCubeFaceSelect( TEST_VEC4F inTexCoord )
{
	/*
	For TextureCube Resources, the following occurs:
	Choose the largest magnitude component of the input vector. Call this magnitude of this value AxisMajor. In the case of a tie, the following precedence should occur: Z, Y, X. 
	Select and mirror the minor axes as defined by the TextureCube coordinate space. Call this new 2d coordinate Position. 
	Project the coordinate onto the cube by dividing the components Position by AxisMajor. 
	Transform to 2d Texture space as follows: Position = Position * 0.5f + 0.5f; 
	Convert the coordinate to fixed point as for a Texture2D.
	*/
	TEST_VEC4F coordMag;
	coordMag.x = fabs(inTexCoord.x);
	coordMag.y = fabs(inTexCoord.y);
	coordMag.z = fabs(inTexCoord.z);
	std::vector<int> retArr;
	{
		static const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(DXGI_FORMAT_R32_FLOAT);
		bool xyTie=false, xzTie=false, yzTie=false;
		if (   fabs(coordMag.x - coordMag.y) < 0.01 
			&& MinMaxRange::TestFloatsSame(coordMag.x, coordMag.y, pTexFormatInfo, 0, 2.0f, NULL))
		{	// tie for largest between X & Y
			xyTie = true;
		}
		if (   fabs(coordMag.x - coordMag.z) < 0.01 
			&& MinMaxRange::TestFloatsSame(coordMag.x, coordMag.z, pTexFormatInfo, 0, 2.0f, NULL))
		{	// tie for largest between X & Z
			xzTie = true;
		}
		if (   fabs(coordMag.y - coordMag.z) < 0.01 
			&& MinMaxRange::TestFloatsSame(coordMag.y, coordMag.z, pTexFormatInfo, 0, 2.0f, NULL))
		{	// tie for largest between Y & Z
			yzTie = true;
		}
		if (xyTie)
		{
			if (xzTie || yzTie) // 3-way tie
				retArr.push_back(2);
			else if (coordMag.z > coordMag.x)
				goto singleFace;
			retArr.push_back(1);
			retArr.push_back(0);
		}
		else if (xzTie)
		{
			if (coordMag.y > coordMag.x)
				goto singleFace;
			retArr.push_back(2);
			retArr.push_back(0);
		}
		else if (yzTie)
		{
			if (coordMag.x > coordMag.y)
				goto singleFace;
			retArr.push_back(2);
			retArr.push_back(1);
		}
		else
			goto singleFace;
		for (size_t n = 0; n < retArr.size(); ++n)
		{
			int nAxisMajor = retArr[n];
			retArr[n] = (inTexCoord.f[nAxisMajor]>=0)?nAxisMajor*2:nAxisMajor*2+1;
		}
		return retArr;
	}
singleFace:
	retArr.clear();
	int nAxisMajor;
	// find largest magnitude axis
	if (coordMag.x > coordMag.z) // Z wins tie
	{
		if (coordMag.x > coordMag.y) // Y wins tie
			nAxisMajor = 0;
		else
			nAxisMajor = 1;
	}
	else
	{
		if (coordMag.y > coordMag.z) // Z wins tie
			nAxisMajor = 1;
		else
			nAxisMajor = 2;
	}
	// pick a 2d face
	// X+/- = 0/1  Y+/- = 2/3  Z+/- = 4/5
	int faceNum = (inTexCoord.f[nAxisMajor]>=0)?nAxisMajor*2:nAxisMajor*2+1;
	retArr.push_back(faceNum);
	return retArr;
}

TEST_VEC4F
doCubeFaceMap( TEST_VEC4F inTexCoord, int faceNum )
{
	TEST_VEC4F position;
	switch (faceNum)
	{
	case 0: position.x = -inTexCoord.z; break;
	case 1: position.x = inTexCoord.z; break;
	case 2: // fall-through
	case 3: // fall-through
	case 4: position.x = inTexCoord.x; break;
	case 5: position.x = -inTexCoord.x; break;
	}
	switch (faceNum)
	{
	case 0: // fall-through
	case 1: // fall-through
	case 4: // fall-through
	case 5: position.y = -inTexCoord.y; break;
	case 2: position.y = inTexCoord.z; break;
	case 3: position.y = -inTexCoord.z; break;
	}
	position.z = 0; position.w = 0;
	return position;
}

TEST_VEC4F
doCubeFaceTransform( TEST_VEC4F inTexCoord, int faceNum )
{
	int nAxisMajor = faceNum / 2;
	float axisMajor = fabs(inTexCoord.f[nAxisMajor]);
	// select and mirror minor axes
	TEST_VEC4F position = doCubeFaceMap(inTexCoord, faceNum);
	// divide by major magnitude
	for (int n = 0; n < 2; ++n)
		position.f[n] /= axisMajor;
	// transform into 2d face space
	for (int n = 0; n < 2; ++n)
		position.f[n] = position.f[n] * 0.5f + 0.5f;

	return position;
}

void
doCubeFaceTransformDeriv(TEST_VEC4F inTexCoord,
						 TEST_VEC4F inTexDerivX,
						 TEST_VEC4F inTexDerivY,
						 int faceNum,
						 TEST_VEC4F& outTexDerivX,
						 TEST_VEC4F& outTexDerivY)
{
	/*
	Using TC, determine which component is of the largest magnitude, as when calculating the texel location.
		If any of the components are equivalent, precedence is as follows: Z, Y, X. 
		The absolute value of this will be referred to as AxisMajor. 
	select and mirror the minor axes of TC as defined by the TextureCube coordinate space to generate TC’.uv 
	select and mirror the minor axes of the partial derivative vectors as defined by the TextureCube coordinate space, 
		generating 2 new partial derivative vectors dX'.uv & dY'.uv. 
	Suppose DerivativeMajorX and DerivativeMajorY are the major axis component of the original partial derivative vectors. 
	Calculate 2 new dX and dY vectors for future calculations as follows:
	dX.uv = (AxisMajor*dX'.uv – TC’.uv*DerivativeMajorX)/(AxisMajor*AxisMajor)
	dY.uv = (AxisMajor*dY'.uv – TC’.uv*DerivativeMajorY)/(AxisMajor*AxisMajor)
	*/
	int nAxisMajor = faceNum / 2;
	float axisMajor = fabs(inTexCoord.f[nAxisMajor]);
	float fDerivMajorX = (inTexDerivX.f[nAxisMajor]);
	float fDerivMajorY = (inTexDerivY.f[nAxisMajor]);
	if (axisMajor <= 1.0e-30)
	{
		outTexDerivX = inTexDerivX;
		outTexDerivY = inTexDerivY;
	}
	else
	{
		// select and mirror minor axes
		// TC'
		TEST_VEC4F TCp = doCubeFaceMap(inTexCoord, faceNum);
		// dX'
		TEST_VEC4F dXp = doCubeFaceMap(inTexDerivX, faceNum);
		// dY'
		TEST_VEC4F dYp = doCubeFaceMap(inTexDerivY, faceNum);

		if ((faceNum % 2) == 1)
		{
			fDerivMajorX = -fDerivMajorX;
			fDerivMajorY = -fDerivMajorY;
		}

		outTexDerivX = (dXp*axisMajor - TCp*fDerivMajorX) * (1.0f/(axisMajor*axisMajor));
		outTexDerivY = (dYp*axisMajor - TCp*fDerivMajorY) * (1.0f/(axisMajor*axisMajor));
	}
}

MinMaxTexelWeight
doMipSelect(MinMaxFloat fLOD, 
			INT32 minMip, 
			INT32 maxMip, 
			eFilterMode_t filterMode, 
			FilterHardwareSpecs* pHardwareSpecs)
{
	/*
	If the Sampler defines a Filter for which MIP is set to POINT (otherwise known as 'nearest'): 
		the LOD is first converted to at least 8.6 fixed point (if not already in fixed point form), 
		0.5 is added, 
		and then the integer part of the LOD is taken as the mipmap level (clamped to available miplevels or any settings for clamping miplevels). 
		This selects the "nearest" miplevel. 
	If the Sampler defines a Filter for which MIP is set to LINEAR: 
		The two nearest mipmaps are selected as follows. 
		First, the LOD is converted to at least 8.6 fixed point (if not already in fixed point form). Call this fxpLOD. 
		The integer part of the fxpLOD is the first miplevel. Call this mipFloor. 
		The second miplevel, call it mipCeil, is miplFloor+1. 
		The selected miplevels are clamped to the range of mipmaps available, plus any other settings for clamping miplevels. 
		The weight for mipCeil, call it wMipCeil, is the fractional component of fxpLOD, converted to float. 
		The weight for mipFloor, call it wMipFloor, is 1.0f - wMipCeil. 
		[remark] In the past multiple IHVs have cheated here (weight selection) with tactics such as snapping LOD values loosely "around" a given mipmap level to that level in order to avoid performing fetches from multiple mipmap levels. Such practices were always in violation of spec, and will continue to be violations in D3D10.0.
		Finally, the texture filtering operation receives the pair of chosen miplevels and weights. The filter can perform some sampling operation at each miplevel combines them using the weights: sampleAt(mipCeil) * wMipCeil + sampleAt(mipFloor) * wMipFloor, where the particular sample operation performed depends on the filtering mode (and multiple such operations involving LINEAR mipmap selection could be involved in a complicated filtering process, e.g. in anisotropic filtering). 
	*/
	if (isNaN(fLOD.lo))
		fLOD.lo = 0.0;
	if (isNaN(fLOD.hi))
		fLOD.hi = 0.0;

	if (fLOD.lo < -256)
		fLOD.lo = -256;
	if (fLOD.hi < -256)
		fLOD.hi = -256;

	if (fLOD.lo > 255)
		fLOD.lo = 255;
	if (fLOD.hi > 255)
		fLOD.hi = 255;

	MinMaxDbl mipRange;
	{
		MinMaxDbl mipRangeLo = MinMaxDbl::floatULPDrift(fLOD.lo, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true);
		MinMaxDbl mipRangeHi = MinMaxDbl::floatULPDrift(fLOD.hi, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true);
		mipRange.lo = min(maxMip, max(0, mipRangeLo.lo) + minMip);
		mipRange.hi = min(maxMip, max(0, mipRangeHi.hi) + minMip);
	}
	switch (filterMode)
	{
	case eFilter_Point:
		{
			// do not add 0.5, it has already been done during LOD calculation before clamping
			return MinMaxTexelWeight(
				texelWeight((int)round_ni(mipRange.lo), 0.f),
				texelWeight((int)round_ni(mipRange.hi), 0.f)
				);
		}
	case eFilter_Linear:
		{
			return MinMaxTexelWeight(
				texelWeight((int)round_ni(mipRange.lo), (float)frac(mipRange.lo)),
				texelWeight((int)round_ni(mipRange.hi), (float)frac(mipRange.hi))
				);
		}
	default:
		assert(!"Invalid filter type");
		return MinMaxTexelWeight(texelWeight(0, 0.f));
	}
}

float
sgn(float fIn)
{
	if (fIn >= 0)
		return 1;
	return -1;
}

void
doEllipticalTransform(TEST_VEC4F& dX,
					  TEST_VEC4F& dY,
					  bool* pbTransformed)
{
	//If w is nonzero, as when calculating LOD for a volume map, an orthogonal transformation 
	// must be used to calculate a pair of 2 dimensional vectors with the same lengths and inner angle 
	// prior to computing the correct Jacobian matrix.
	// The following is the transformation implemented by the reference rasterizer:
	// orthovec = dX x (dX x dY)
	// dX' = (|dX|, 0, 0)
	// dY' = (dot(dY,dX) / |dX|, dot(dY,orthovec) / |orthovec|, 0)

	if (pbTransformed)
		*pbTransformed = false;

	if (dX.z != 0 || dY.z != 0)
	{
		dX.w = 0;
		dY.w = 0;
		TEST_VEC4F orthovec = cross3f(dX, cross3f(dX, dY));
		float dXMag = (float)sqrt(dot4f(dX,dX));
		float orthoMag = (float)sqrt(dot4f(orthovec,orthovec));
		TEST_VEC4F dXp = {dXMag, 0, 0, 0};
		TEST_VEC4F dYp = {dot4f(dY,dX)/dXMag, dot4f(dY,orthovec)/orthoMag, 0, 0};
		if (isNaN(dXp.x) || isNaN(dXp.y) || isNaN(dXp.z))
			return;
		if (isNaN(dYp.x) || isNaN(dYp.y) || isNaN(dYp.z))
			return;
		dX = dXp;
		dY = dYp;

		if (pbTransformed)
			*pbTransformed = true;
	}

	/*
	if either of dX or dY are of zero length, an implementation should skip these transformations. 
	if dX and dY are parallel, an implementation should skip these transformations. 
	if dX and dY are perpendicular, an implementation should skip these transformations. 
	if any component of dX or dY is inf or NaN, an implementation should skip these transformations. 
	if components of dX and dY are large or small enough to cause NaNs in these calculations, an implementation should skip these transformations.
	*/

	if (dot4f(dX,dX) == 0) //< 0.000001)
		return;
	if (dot4f(dY,dY) == 0) //< 0.000001)
		return;
	if (dot4f(dX,dY) == 0) //< 0.000001)
		return;
	if (1 - dot4f(dX,dY) == 0) //< 0.000001)
		return;
	if (isINF(dX.x) || isINF(dX.y) || isINF(dX.z))
		return;
	if (isINF(dY.x) || isINF(dY.y) || isINF(dY.z))
		return;
	if (isNaN(dX.x) || isNaN(dX.y) || isNaN(dX.z))
		return;
	if (isNaN(dY.x) || isNaN(dY.y) || isNaN(dY.z))
		return;

	//Implicit ellipse coefficients:

	TEST_VEC4F new_dX = {0,0,0,0};
	TEST_VEC4F new_dY = {0,0,0,0};

	const float A = dX.y*dX.y + dY.y*dY.y;
	const float B = -2 * (dX.x * dX.y + dY.x * dY.y);
	const float C = dX.x*dX.x + dY.x*dY.x;
	const float F = (float)pow((dX.x * dY.y) - (dY.x * dX.y), 2);
	//Defining the following variables:
	const float p = A - C;
	const float q = A + C;
	const float t = (float)sqrt(p*p + B*B);
	//The new vectors may be then calculated as: 
	new_dX.x = (float)sqrt(F * (t+p) / ( t * (q+t)));
	new_dX.y = (float)sqrt(F * (t-p) / ( t * (q+t)))*sgn(B*p);
	new_dY.x = (float)sqrt(F * (t-p) / ( t * (q-t)))*-sgn(B*p);
	new_dY.y = (float)sqrt(F * (t+p) / ( t * (q-t)));
	if (isNaN(new_dX.x) || isNaN(new_dX.y) || isNaN(new_dX.z))
		return;
	if (isNaN(new_dY.x) || isNaN(new_dY.y) || isNaN(new_dY.z))
		return;
	dX = new_dX;
	dY = new_dY;

	if (pbTransformed)
		*pbTransformed = true;
}

MinMaxFloat
computeLODFromIsoDerivs(TEST_VEC4F dX, TEST_VEC4F dY)
{
	float lengthX = sqrt(dX.x*dX.x + dX.y*dX.y + dX.z*dX.z);
	float lengthY = sqrt(dY.x*dY.x + dY.y*dY.y + dY.z*dY.z);
	// ~8.5% tolerance
	MinMaxFloat lengthXMinMax = MinMaxFloat(lengthX*(1-c_LengthApproxTolerance), lengthX*(1+c_LengthApproxTolerance));
	MinMaxFloat lengthYMinMax = MinMaxFloat(lengthY*(1-c_LengthApproxTolerance), lengthY*(1+c_LengthApproxTolerance));
	float LODHi = (float)(log(max(lengthXMinMax.hi,lengthYMinMax.hi))/log(2.0));
	float LODLo = (float)(log(max(lengthXMinMax.lo,lengthYMinMax.lo))/log(2.0));
	return MinMaxFloat(LODHi, LODLo);
}

MinMaxFloat
doLodCalc(TEST_VEC4F texCoord, 
		  TEST_VEC4F texDerivs[2],
		  int bestMipWHD[3], 
		  D3D11_SRV_DIMENSION texType,
		  bool bAnisotropic,
		  float* pAnisoLineDir,
		  float* pAnisoRatio,
		  int nMaxAniso)
{
	bool bTransformed = false;
	TEST_VEC4F dX = texDerivs[0];
	TEST_VEC4F dY = texDerivs[1];
	switch (texType)
	{
	default:
		assert(!"Invalid texture type");
		break;
	case D3D11_SRV_DIMENSION_TEXTURE1D:
		dX.y = 0;
		dY.y = 0;
	case D3D11_SRV_DIMENSION_TEXTURE2D:
		dX.z = 0;
		dY.z = 0;
	case D3D11_SRV_DIMENSION_TEXTURE3D:
		{
			TEST_VEC4F texScale = {(float)bestMipWHD[0], (float)bestMipWHD[1], (float)bestMipWHD[2], 0};
			dX = dX * texScale;
			dY = dY * texScale;
		}
		break;
	case D3D11_SRV_DIMENSION_TEXTURECUBE:
		{
			std::vector<int> faceNums = doCubeFaceSelect(texCoord);
			MinMaxFloat returnLOD(0);
			for (size_t n = 0; n < faceNums.size(); ++n)
			{
				// assume LOD is calculated the same regardless of which face is chosen across minimal difference
				int faceNum = faceNums[n];
				doCubeFaceTransformDeriv(texCoord, texDerivs[0], texDerivs[1], faceNum, dX, dY );
				// multiplying by 0.5f to adjust for TextureCube coordinate system
				TEST_VEC4F texScale = {bestMipWHD[0] * 0.5f, bestMipWHD[0] * 0.5f, 0, 0};
				dX = dX * texScale;
				dY = dY * texScale;

				MinMaxFloat LOD = computeLODFromIsoDerivs(dX, dY);
				if (n == 0)
					returnLOD = LOD;
				else
					returnLOD.expand(LOD);

				doEllipticalTransform(dX, dY, &bTransformed);
				MinMaxFloat LOD2 = computeLODFromIsoDerivs(dX, dY);
				returnLOD.expand(LOD2.hi + c_CubeDerivativeDeviation);
				returnLOD.expand(LOD2.lo - c_CubeDerivativeDeviation);
				if (bTransformed)
				{
					returnLOD.expand(LOD2.hi + c_EllipticalTransformDeviation);
					returnLOD.expand(LOD2.lo - c_EllipticalTransformDeviation);
				}
			}
			return returnLOD;
		}
		break;
	}

	if (texType == D3D11_SRV_DIMENSION_TEXTURE2D || texType == D3D11_SRV_DIMENSION_TEXTURE3D)
		doEllipticalTransform(dX, dY, &bTransformed);

	if (bAnisotropic)
	{
		assert(pAnisoLineDir != NULL);
		assert(pAnisoRatio != NULL);
		// Compute outputs:
		// (1) float ratioOfAnisotropy
		// (2) float anisoLineDirection
		// (3) float LOD

		// (For 1D Textures, dX.v and dY.v are 0, so all the
		// math below can be simplified)

		float squaredLengthX = dX.x*dX.x + dX.y*dX.y;
		float squaredLengthY = dY.x*dY.x + dY.y*dY.y;
		float determinant = abs(dX.x*dY.y - dX.y*dY.x);
		bool isMajorX = squaredLengthX > squaredLengthY;
		float squaredLengthMajor = isMajorX ? squaredLengthX : squaredLengthY;
		float lengthMajor = sqrt(squaredLengthMajor);
		float normMajor = 1.f/lengthMajor;

		pAnisoLineDir[0] = (isMajorX ? dX.x : dY.x) * normMajor;
		pAnisoLineDir[1] = (isMajorX ? dX.y : dY.y) * normMajor;

		*pAnisoRatio = squaredLengthMajor/determinant;

		// clamp ratio and compute LOD
		float lengthMinor;
		if ( (*pAnisoRatio) > nMaxAniso ) // maxAniso comes from a Sampler state.
		{
			// ratio is clamped - LOD is based on ratio (preserves area)
			*pAnisoRatio = (float)nMaxAniso;
			lengthMinor = lengthMajor/(*pAnisoRatio);
		}
		else
		{
			// ratio not clamped - LOD is based on area
			lengthMinor = determinant/lengthMajor;
		}

		// clamp to top LOD
		if (lengthMinor < 1.0f)
		{
			(*pAnisoRatio) = max( 1.0f, (*pAnisoRatio)*lengthMinor );

			// Clamping lengthMinor to 1.0 is not required by FL11.0
			if ( g_App.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0 )
				lengthMinor = 1.0f;
		}

		float LOD = (float)(log(lengthMinor)/log(2.0));
		// if clamping lengthMinor to 1.0 happens, the LOD will be 0
		float LODZero = 0.0f;
		if (bTransformed)
		{
			MinMaxFloat minMaxLOD(LOD - c_EllipticalTransformDeviation, LOD + c_EllipticalTransformDeviation);
			minMaxLOD.expand(LODZero - c_EllipticalTransformDeviation);
			minMaxLOD.expand(LODZero + c_EllipticalTransformDeviation);
			return minMaxLOD;
		}
		else
		{
			MinMaxFloat minMaxLOD(LOD);
			minMaxLOD.expand(LODZero);
			return minMaxLOD;
		}
	}
	else
	{
		MinMaxFloat LOD = computeLODFromIsoDerivs(dX, dY);
		if (bTransformed)
			return MinMaxFloat(LOD.lo - c_EllipticalTransformDeviation, LOD.hi + c_EllipticalTransformDeviation);
		else
			return MinMaxFloat(LOD);
	}
}

bool
doComparison(double a, double b, D3D11_COMPARISON_FUNC compareFunc)
{
	if (isNaN((float)a) || isNaN((float)b))
		return (compareFunc==D3D11_COMPARISON_NOT_EQUAL);
	switch(compareFunc)
	{
	default:
		assert(!"Invalid comparison function");
		return false;
	case D3D11_COMPARISON_NEVER:
		return false;
	case D3D11_COMPARISON_LESS:
		return a < b;
	case D3D11_COMPARISON_LESS_EQUAL:
		return a <= b;
	case D3D11_COMPARISON_EQUAL:
		return a == b;
	case D3D11_COMPARISON_NOT_EQUAL:
		return a != b;
	case D3D11_COMPARISON_GREATER:
		return a > b;
	case D3D11_COMPARISON_GREATER_EQUAL:
		return a >= b;
	case D3D11_COMPARISON_ALWAYS:
		return true;
	}
}

MinMaxBool
doComparison(MinMaxDbl a, MinMaxDbl b, D3D11_COMPARISON_FUNC compareFunc)
{
	if (isNaN((float)a.lo) || isNaN((float)b.lo))
		return MinMaxBool(compareFunc==D3D11_COMPARISON_NOT_EQUAL);
	bool retVal = false;
	switch(compareFunc)
	{
	default:
		assert(!"Invalid comparison function");
		retVal = (false);
		break;
	case D3D11_COMPARISON_NEVER:
		retVal = (false);
		break;
	case D3D11_COMPARISON_LESS:
		if (a.hi < b.lo)
			retVal = (true);
		else if (!(a.lo < b.hi))
			retVal = (false);
		else
			return MinMaxBool(true, false);
		break;
	case D3D11_COMPARISON_LESS_EQUAL:
		if (a.hi <= b.lo)
			retVal = (true);
		else if (!(a.lo <= b.hi))
			retVal = (false);
		else
			return MinMaxBool(true, false);
		break;
	case D3D11_COMPARISON_EQUAL:
		if (a.isSingular() && b.isSingular() && a.lo == b.lo)
			retVal = (true);
		else if (a.lo > b.hi || a.hi < b.lo)
			retVal = (false);
		else
			return MinMaxBool(true, false);
		break;
	case D3D11_COMPARISON_NOT_EQUAL:
		if (a.lo > b.hi || a.hi < b.lo)
			retVal = (true);
		else if (a.isSingular() && b.isSingular() && a.lo == b.lo)
			retVal = (false);
		else
			return MinMaxBool(true, false);
		break;
	case D3D11_COMPARISON_GREATER:
		if (a.lo > b.hi)
			retVal = (true);
		else if (!(a.hi > b.lo))
			retVal = (false);
		else
			return MinMaxBool(true, false);
		break;
	case D3D11_COMPARISON_GREATER_EQUAL:
		if (a.lo >= b.hi)
			retVal = (true);
		else if (!(a.hi >= b.lo))
			retVal = (false);
		else
			return MinMaxBool(true, false);
		break;
	case D3D11_COMPARISON_ALWAYS:
		retVal = (true);
		break;
	}
	return MinMaxBool(retVal);
}

namespace Texture_ALU
{
void 
WriteData(const TEST_VEC4F* pInputColor, 
		  DXGI_FORMAT DataType, 
		  BYTE* pOutput);
void
ReadDataWithDefaults(const void* pInput, 
					 UINT uOffset, 
					 DXGI_FORMAT DataType, 
					 TEST_VEC4F* pOutputColor);
void
readD3DColor(void* buffer, 
			 TEST_COLOR* color,
			 DXGI_FORMAT format, 
			 size_t index);
}

template <typename ValType>
void
clamp(ValType& inVal, ValType loVal, ValType hiVal)
{
	if (inVal > hiVal)
		inVal = hiVal;
	if (inVal < loVal)
		inVal = loVal;
}

template <typename ValType>
void
clamp_float(ValType& inVal, int floatBits, bool bTightClamp)
{
	if (!bTightClamp)
		return;
	switch (floatBits)
	{
	default: assert(!"Unrecognized float format");
	case 32: break;
	case 16: clamp(inVal, (TEST_COLOR::realType)-65504.0, (TEST_COLOR::realType)65504.0); break;
		// small float formats have no sign bit
	case 11: clamp(inVal, (TEST_COLOR::realType)0.0, (TEST_COLOR::realType)65024.0); break;
	case 10: clamp(inVal, (TEST_COLOR::realType)0.0, (TEST_COLOR::realType)64512.0); break;
	case 0: break;
	}
}

TEST_COLOR
doFormatClamp(const TEST_COLOR& inColor,
			  DXGI_FORMAT format,
			  bool bTightClamp)
{
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(format);
	TEST_COLOR outColor = inColor;
	if (pTexFormatInfo->dwFlags & FF_UNORM)
	{
		clamp(outColor.r, (TEST_COLOR::realType)0.0, (TEST_COLOR::realType)1.0);
		clamp(outColor.g, (TEST_COLOR::realType)0.0, (TEST_COLOR::realType)1.0);
		clamp(outColor.b, (TEST_COLOR::realType)0.0, (TEST_COLOR::realType)1.0);
		clamp(outColor.a, (TEST_COLOR::realType)0.0, (TEST_COLOR::realType)1.0);
	}
	else if (pTexFormatInfo->dwFlags & FF_SNORM)
	{
		clamp(outColor.r, (TEST_COLOR::realType)-1.0, (TEST_COLOR::realType)1.0);
		clamp(outColor.g, (TEST_COLOR::realType)-1.0, (TEST_COLOR::realType)1.0);
		clamp(outColor.b, (TEST_COLOR::realType)-1.0, (TEST_COLOR::realType)1.0);
		clamp(outColor.a, (TEST_COLOR::realType)-1.0, (TEST_COLOR::realType)1.0);
	}
	else if (pTexFormatInfo->dwFlags & (FF_FLOAT | FF_SHAREDEXP))
	{
		clamp_float(outColor.r, pTexFormatInfo->pBitsPerComponent[0], bTightClamp);
		clamp_float(outColor.g, pTexFormatInfo->pBitsPerComponent[1], bTightClamp);
		clamp_float(outColor.b, pTexFormatInfo->pBitsPerComponent[2], bTightClamp);
		clamp_float(outColor.a, pTexFormatInfo->pBitsPerComponent[3], bTightClamp);
	}
	else
	{
		assert(!"Unexpected format");
	}
	return outColor;
}

TEST_COLOR
doFormatQuantize(TEST_COLOR inColor,
				 DXGI_FORMAT format)
{
	DXGI_FORMAT equivFormat = MinMaxRange::GetFormatEquivalentBits(format);

	BYTE buffer[256];
	TEST_VEC4F color4f = {
		(float)inColor.r,
		(float)inColor.g,
		(float)inColor.b,
		(float)inColor.a
	};
	Texture_ALU::WriteData(&color4f, equivFormat, buffer);
	Texture_ALU::ReadDataWithDefaults(buffer, 0, equivFormat, &color4f);
	TEST_COLOR retColor;
	for (int n = 0; n < 4; ++n)
		retColor.f[n] = color4f.f[n];
	return retColor;
}

TEST_COLOR
doFormatDefaults(TEST_COLOR inColor,
				 DXGI_FORMAT format)
{
	TEST_COLOR retColor = inColor;
	DXGI_FORMAT equivFormat = MinMaxRange::GetFormatEquivalentBits(format);
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(equivFormat);
	UINT bitsPerComponent[4];
	memcpy(bitsPerComponent, pTexFormatInfo->pBitsPerComponent, sizeof(bitsPerComponent));
	if (format == DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
	{
		bitsPerComponent[0] = 9;
		bitsPerComponent[1] = 9;
		bitsPerComponent[2] = 9;
	}
	if (format == DXGI_FORMAT_G8R8_G8B8_UNORM || format == DXGI_FORMAT_R8G8_B8G8_UNORM)
	{
		bitsPerComponent[3] = 0;
	}
	if (0 == (pTexFormatInfo->dwFlags & FF_R) || bitsPerComponent[0] == 0)
		retColor.r = 0;
	if (0 == (pTexFormatInfo->dwFlags & FF_G) || bitsPerComponent[1] == 0)
		retColor.g = 0;
	if (0 == (pTexFormatInfo->dwFlags & FF_B) || bitsPerComponent[2] == 0)
		retColor.b = 0;
	if (0 == (pTexFormatInfo->dwFlags & FF_A) || bitsPerComponent[3] == 0)
		retColor.a = 1;
	return retColor;
}

MinMaxColor
doFormatDefaults(MinMaxColor inColor,
				 DXGI_FORMAT format)
{
	if (inColor.isSingular())
		return MinMaxColor(doFormatDefaults(inColor.lo, format));
	return MinMaxColor(
		doFormatDefaults(inColor.lo, format),
		doFormatDefaults(inColor.hi, format)
		);
}

MinMaxColor
getTexColor(int x, int y, int z,
			DXGI_FORMAT texFormat,
			int texWHD[3], 
			const TEST_COLOR* pTexture, 
			const TEST_COLOR* pBorderColor,
			bool bUseBorderColor,
			bool bComparison, D3D11_COMPARISON_FUNC compareFunc, float compareValue,
			int faceNum = 0)
{
	assert(pTexture != NULL);
	assert(pBorderColor != NULL);
	TEST_COLOR retColor;
	MinMaxColor retColorMinMax;
	if (bUseBorderColor)
	{
		TEST_COLOR clampedColor = doFormatDefaults(doFormatClamp(*pBorderColor, texFormat, false), texFormat);
		TEST_COLOR quantizedColor = doFormatQuantize(*pBorderColor, texFormat);
		retColorMinMax = MinMaxColor(clampedColor);
		retColorMinMax.expand(quantizedColor);
	}
	else
	{
		TEST_COLOR texColor;
		if ( x < 0 || x >= texWHD[0] || y < 0 || y >= texWHD[1] || z < 0 || z >= texWHD[2] )
		{
			//assert(!"Bad texture coordinate");
			texColor = *pBorderColor;
		}
		else
		{
			const size_t facePitch = texWHD[0]*texWHD[0];
			const TEST_COLOR* faceTexture = pTexture + (facePitch*faceNum);

			texColor = faceTexture[x + texWHD[0]*(y + texWHD[1]*z)];
		}
		retColorMinMax = MinMaxColor(doFormatQuantize(texColor, texFormat));
		float colorTolerance = CFormatHelper::GetBCFormatMaxTolerance(texFormat);
		for (int n = 0; n < 4; ++n)
		{
			retColorMinMax.lo.f[n] -= colorTolerance;
			retColorMinMax.hi.f[n] += colorTolerance;
		}
	}
	if (bComparison)
	{
		MinMaxDbl retColorRMinMax = MinMaxDbl::floatULPDrift(retColorMinMax.lo.r);
		retColorRMinMax.expand(MinMaxDbl::floatULPDrift(retColorMinMax.hi.r));
		MinMaxDbl compareValMinMax = MinMaxDbl::floatULPDrift(compareValue);
		MinMaxDbl retVal(0.0); 
		MinMaxBool compareResult = doComparison(compareValMinMax, retColorRMinMax, compareFunc);
		if (compareResult.contains(true))
			retVal = MinMaxDbl(1.0);
		if (compareResult.contains(false))
			retVal.expand(0.0);
		for (int n = 0; n < 4; ++n)
		{
			retColorMinMax.lo.f[n] = (TEST_COLOR::realType)(retVal.lo);
			retColorMinMax.hi.f[n] = (TEST_COLOR::realType)(retVal.hi);
		}
	}

	return retColorMinMax;
}

MinMaxColor
getTexColor(MinMaxInt x, MinMaxInt y, MinMaxInt z,
			DXGI_FORMAT texFormat,
			int texWHD[3], 
			const TEST_COLOR* pTexture, 
			const TEST_COLOR* pBorderColor,
			MinMaxBool bUseBorderColor,
			bool bComparison, D3D11_COMPARISON_FUNC compareFunc, float compareValue,
			int faceNum = 0)
{
	MinMaxColor aColor;
	MinMaxColor retColor(aColor);

	if (bUseBorderColor.contains(true))
	{
		aColor = getTexColor(0,0,0,texFormat,texWHD,pTexture,pBorderColor,true,bComparison,compareFunc,compareValue,faceNum);
		retColor = MinMaxColor(aColor);
	}
	else
	{
		aColor = getTexColor(x.lo, y.lo, z.lo, texFormat, texWHD, pTexture, 
			pBorderColor, false, bComparison, compareFunc, compareValue, faceNum);
		retColor = MinMaxColor(aColor);
	}

	if (bUseBorderColor.contains(false))
	{
		if (bUseBorderColor.contains(true))
		{
			aColor = getTexColor(x.lo, y.lo, z.lo, texFormat, texWHD, pTexture, 
				pBorderColor, false, bComparison, compareFunc, compareValue, faceNum);
			retColor.expand(aColor);
		}

		if (!z.isSingular())
		{
			aColor = getTexColor(x.lo, y.lo, z.hi, texFormat, texWHD, pTexture, 
				pBorderColor, false, bComparison, compareFunc, compareValue, faceNum);
			retColor.expand(aColor);
		}

		if (!y.isSingular())
		{
			aColor = getTexColor(x.lo, y.hi, z.lo, texFormat, texWHD, pTexture, 
				pBorderColor, false, bComparison, compareFunc, compareValue, faceNum);
			retColor.expand(aColor);

			if (!z.isSingular())
			{
				aColor = getTexColor(x.lo, y.hi, z.hi, texFormat, texWHD, pTexture, 
					pBorderColor, false, bComparison, compareFunc, compareValue, faceNum);
				retColor.expand(aColor);
			}
		}


		if (!x.isSingular())
		{
			aColor = getTexColor(x.hi, y.lo, z.lo, texFormat, texWHD, pTexture, 
				pBorderColor, false, bComparison, compareFunc, compareValue, faceNum);
			retColor.expand(aColor);

			if (!z.isSingular())
			{
				aColor = getTexColor(x.hi, y.lo, z.hi, texFormat, texWHD, pTexture, 
					pBorderColor, false, bComparison, compareFunc, compareValue, faceNum);
				retColor.expand(aColor);
			}

			if (!y.isSingular())
			{

				aColor = getTexColor(x.hi, y.hi, z.lo, texFormat, texWHD, pTexture, 
					pBorderColor, false, bComparison, compareFunc, compareValue, faceNum);
				retColor.expand(aColor);

				if (!z.isSingular())
				{
					aColor = getTexColor(x.hi, y.hi, z.hi, texFormat, texWHD, pTexture, 
						pBorderColor, false, bComparison, compareFunc, compareValue, faceNum);
					retColor.expand(aColor);
				}
			}
		}

	}

	return retColor;
}

bool
getCubeTexColor(int u, int v,
				DXGI_FORMAT texFormat, 
				int texWHD[3], 
				const TEST_COLOR* texture, 
				int faceNum,
				MinMaxColor& sampleOut,
				bool bComparison, D3D11_COMPARISON_FUNC compareFunc, float compareValue)
{
	assert(texWHD[0] == texWHD[1] && texWHD[2] == 1);
	if ( u < -1 || u > texWHD[0] || v < -1 || v > texWHD[1] )
	{
		assert(!"Bad texture coordinate");
		return false;
	}
	int edgeLen = texWHD[0];

	int uOff = (u < 0 || u >= edgeLen);
	int vOff = (v < 0 || v >= edgeLen);
	// corner outside isn't counted
	if (uOff && vOff)
		return false;

	// handle cube edge overhangs
	if (u < 0)
	{
		switch (faceNum)
		{
		case 0: faceNum = 4; u = edgeLen-1; break;
		case 1: faceNum = 5; u = edgeLen-1; break;
		case 4: faceNum = 1; u = edgeLen-1; break;
		case 5: faceNum = 0; u = edgeLen-1; break;
		case 2: faceNum = 1; u = v; v = 0; break;
		case 3: faceNum = 1; u = (edgeLen-1)-v; v = edgeLen-1; break;
		}
	}
	else if (u >= edgeLen)
	{
		switch (faceNum)
		{
		case 0: faceNum = 5; u = 0; break;
		case 1: faceNum = 4; u = 0; break;
		case 4: faceNum = 0; u = 0; break;
		case 5: faceNum = 1; u = 0; break;
		case 2: faceNum = 0; u = (edgeLen-1)-v; v = 0; break;
		case 3: faceNum = 0; u = v; v = edgeLen-1; break;
		}
	}
	else if (v < 0)
	{
		switch (faceNum)
		{
		case 0: faceNum = 2; v = (edgeLen-1)-u; u = edgeLen-1; break;
		case 1: faceNum = 2; v = u; u = 0; break;
		case 2: faceNum = 5; v = 0; u = (edgeLen-1)-u; break;
		case 3: faceNum = 4; v = edgeLen-1; break;
		case 4: faceNum = 2; v = edgeLen-1; break;
		case 5: faceNum = 2; v = 0; u = (edgeLen-1)-u; break;
		}
	}
	else if (v >= edgeLen)
	{
		switch (faceNum)
		{
		case 0: faceNum = 3; v = u; u = edgeLen-1; break;
		case 1: faceNum = 3; v = (edgeLen-1)-u; u = 0; break;
		case 2: faceNum = 4; v = 0; break;
		case 3: faceNum = 5; v = edgeLen-1; u = (edgeLen-1)-u; break;
		case 4: faceNum = 3; v = 0; break;
		case 5: faceNum = 3; v = edgeLen-1; u = (edgeLen-1)-u; break;
		}
	}

	sampleOut = getTexColor(u, v, 0, texFormat, texWHD, texture, texture, false, bComparison, compareFunc, compareValue, faceNum);

	return true;
}

MinMaxBool
getCubeTexColor(MinMaxInt u, MinMaxInt v,
				DXGI_FORMAT texFormat, 
				int texWHD[3], 
				const TEST_COLOR* pTexture, 
				int faceNum,
				MinMaxColor& sampleOut,
				bool bComparison, D3D11_COMPARISON_FUNC compareFunc, float compareValue)
{
	int numGoodFaces = 0;
	bool bGoodFace = false;
	MinMaxBool goodFaceRange(true);
	MinMaxColor aColor;
	MinMaxColor retColor;

	bGoodFace = getCubeTexColor(u.lo, v.lo, texFormat, texWHD, pTexture, 
		faceNum, aColor, bComparison, compareFunc, compareValue);
	goodFaceRange.expand(bGoodFace);
	if (numGoodFaces > 0)
		retColor.expand(aColor);
	else
		retColor = MinMaxColor(aColor);
	if (bGoodFace)
		++numGoodFaces;
	if (!v.isSingular())
	{
		bGoodFace = getCubeTexColor(u.lo, v.hi, texFormat, texWHD, pTexture, 
			faceNum, aColor, bComparison, compareFunc, compareValue);
		goodFaceRange.expand(bGoodFace);
		if (numGoodFaces > 0)
			retColor.expand(aColor);
		else
			retColor = MinMaxColor(aColor);
		if (bGoodFace)
			++numGoodFaces;
	}

	if (!u.isSingular())
	{
		bGoodFace = getCubeTexColor(u.lo, v.lo, texFormat, texWHD, pTexture, 
			faceNum, aColor, bComparison, compareFunc, compareValue);
		goodFaceRange.expand(bGoodFace);
		if (numGoodFaces > 0)
			retColor.expand(aColor);
		else
			retColor = MinMaxColor(aColor);
		if (bGoodFace)
			++numGoodFaces;
		if (!v.isSingular())
		{
			bGoodFace = getCubeTexColor(u.lo, v.hi, texFormat, texWHD, pTexture, 
				faceNum, aColor, bComparison, compareFunc, compareValue);
			goodFaceRange.expand(bGoodFace);
			if (numGoodFaces > 0)
				retColor.expand(aColor);
			else
				retColor = MinMaxColor(aColor);
			if (bGoodFace)
				++numGoodFaces;
		}
	}

	if (numGoodFaces > 0)
		sampleOut = retColor;
	else
		goodFaceRange = MinMaxBool(false);
	return goodFaceRange;
}

MinMaxColor
doTexSamplePt(TEST_VEC4F texCoord, 
			  const TEST_COLOR* texture, 
			  const TEST_COLOR* borderColor,
			  DXGI_FORMAT texFormat, 
			  int texWHD[3], 
			  D3D11_TEXTURE_ADDRESS_MODE addressMode[3],
			  D3D11_SRV_DIMENSION texType,
			  bool bComparison,
			  D3D11_COMPARISON_FUNC compareFunc, 
			  float compareValue,
			  int immOffsets[3],
			  FilterHardwareSpecs* pHardwareSpecs)
{
	MinMaxBool bUseBorderColor(false);
	MinMaxInt x(0),y(0),z(0);
	switch (texType)
	{
	default:
		assert(!"Invalid texture type");
		break;
		// intentional fall-through
	case D3D11_SRV_DIMENSION_TEXTURE3D:
		z = doTexAddressPtMinMax(texCoord.z, texWHD[2], addressMode[2], bUseBorderColor, immOffsets[2], pHardwareSpecs);
	case D3D11_SRV_DIMENSION_TEXTURE2D:
		y = doTexAddressPtMinMax(texCoord.y, texWHD[1], addressMode[1], bUseBorderColor, immOffsets[1], pHardwareSpecs);
	case D3D11_SRV_DIMENSION_TEXTURE1D:
		x = doTexAddressPtMinMax(texCoord.x, texWHD[0], addressMode[0], bUseBorderColor, immOffsets[0], pHardwareSpecs);
		break;
	case D3D11_SRV_DIMENSION_TEXTURECUBE:
		assert(texWHD[0] == texWHD[1] && texWHD[2] == 1);
		{
			std::vector<int> faceNums = doCubeFaceSelect(texCoord);
			MinMaxColor retColor;
			for (size_t n = 0; n < faceNums.size(); ++n)
			{
				const int faceNum = faceNums[n];
				TEST_VEC4F texCoord2d = doCubeFaceTransform(texCoord, faceNum);
				const size_t facePitch = texWHD[0]*texWHD[0];
				const TEST_COLOR* faceTexture = texture + (facePitch*faceNum);
				D3D11_TEXTURE_ADDRESS_MODE cubeAddressMode[3] = {
					D3D11_TEXTURE_ADDRESS_CLAMP,
					D3D11_TEXTURE_ADDRESS_CLAMP,
					D3D11_TEXTURE_ADDRESS_CLAMP
				};
				MinMaxColor sampleColor = doTexSamplePt(texCoord2d, faceTexture, borderColor, texFormat, texWHD, cubeAddressMode, D3D11_SRV_DIMENSION_TEXTURE2D, bComparison, compareFunc, compareValue, immOffsets, pHardwareSpecs);
				if (n == 0)
					retColor = sampleColor;
				else
					retColor.expand(sampleColor);
			}
			return retColor;
		}
	}

	return getTexColor(x,y,z, texFormat, texWHD, texture, borderColor, bUseBorderColor, bComparison, compareFunc, compareValue);
}

void mergeMinMaxFilterSamples(MINMAX_FILTERTYPE minMaxFilter, const MinMaxColor first, bool bUseFirst,
									 const MinMaxColor second, bool bUseSecond, MinMaxColor& outMerged, bool* pbOutUseResult)
{
	MinMaxColor ret;
	if( !bUseFirst && !bUseSecond )
	{
		if( pbOutUseResult ) *pbOutUseResult = false;
		return;
	}
	if( pbOutUseResult ) *pbOutUseResult = true;
	if( bUseFirst && bUseSecond )
	{
		outMerged = first;
		switch(minMaxFilter)
		{
		case MINIMUM_FILTER:
			outMerged.minimum(second);
			break;
		case MAXIMUM_FILTER:
			outMerged.maximum(second);
			break;
		case NOT_MINMAX_FILTER: // use when samples are a tolerance range and not meant to be combined
			outMerged.expand(second);
			break;
		default:
			assert(!"Expected minimum or maximum filter.");
		}
	}
	else if( bUseFirst )
	{
		outMerged = first;
	}
	else
	{
		outMerged = second;
	}
}

const float c_MaxQ8ZeroEquivalent = .5f / 256.f; // Sample weights are Q8 minimum

MinMaxColor
linearHelper(const MinMaxTexelWeight x, const MinMaxTexelWeight y, const MinMaxTexelWeight z, 
			 const TEST_COLOR* texture, 
			 const TEST_COLOR* borderColor, 
			 DXGI_FORMAT texFormat,
			 int texWHD[3], 
			 D3D11_TEXTURE_ADDRESS_MODE addressMode[3],
			 int nDimension,
			 bool bComparison, D3D11_COMPARISON_FUNC compareFunc, float compareValue,
			 MINMAX_FILTERTYPE minMaxFilterType, bool* pbOutUseMinMaxResult)
{
	assert(nDimension >= 0 && nDimension <= 3);
	if (nDimension < 0 || nDimension > 3)
		return MinMaxColor(*borderColor);
	if (nDimension == 0)
	{
		// recursion base, sample a texel
		MinMaxBool bUseBorderColor(false);
		const MinMaxTexelWeight coords[3] = {x,y,z};
		MinMaxInt clampedCoords[3] = {MinMaxInt(0,0),MinMaxInt(0,0),MinMaxInt(0,0)};
		for (int n = 0; n < 3; ++n)
		{
			//coords[0][n].lo.first = doTexClamp(coords[0][n].lo.first, texWHD[n], addressMode[n], bUseBorderColor);
			//coords[0][n].hi.first = doTexClamp(coords[0][n].hi.first, texWHD[n], addressMode[n], bUseBorderColor);
			MinMaxInt nCoords(coords[n].lo.first, coords[n].hi.first);
			clampedCoords[n] = doTexClamp(nCoords, texWHD[n], addressMode[n], bUseBorderColor);
		}
		if( pbOutUseMinMaxResult ) *pbOutUseMinMaxResult = true; // only used for minmax filters      

		//MinMaxInt xTexel(coords[0][0].lo.first, coords[0][0].hi.first);
		//MinMaxInt yTexel(coords[0][1].lo.first, coords[0][1].hi.first);
		//MinMaxInt zTexel(coords[0][2].lo.first, coords[0][2].hi.first);
		//return getTexColor(xTexel,yTexel,zTexel,texFormat,texWHD,texture,borderColor,
		//	bUseBorderColor, bComparison, compareFunc, compareValue);
		return getTexColor(clampedCoords[0],clampedCoords[1],clampedCoords[2],texFormat,texWHD,texture,borderColor,
			bUseBorderColor, bComparison, compareFunc, compareValue);
	}
	else
	{
		// recursion into 2 samples of lower dimension
		if (1)
		{
			int precisionBits;
			bool unormPrecision;
			MinMaxColor::GetPrecision(&precisionBits, &unormPrecision);
			float weights[3][2] = {
				{x.lo.second,x.hi.second}, 
				{y.lo.second,y.hi.second}, 
				{z.lo.second,z.hi.second}
			};
			MinMaxTexelWeight xyzLo[3] = {x,y,z}, xyzHi[3] = {x,y,z};
			xyzLo[nDimension-1].hi = xyzLo[nDimension-1].lo;
			xyzHi[nDimension-1].lo = xyzHi[nDimension-1].hi;
			MinMaxColor samplesLoHi[2][2];
			bool bUseMinMaxResult[2][2];
			UINT8 uMergeMinMaxResultMask = 0;
			// left/top/front sample lo & hi
			samplesLoHi[0][0] = linearHelper(xyzLo[0],xyzLo[1],xyzLo[2], texture, borderColor, texFormat, texWHD, addressMode, nDimension-1, bComparison, compareFunc, compareValue, minMaxFilterType,&bUseMinMaxResult[0][0]);
			samplesLoHi[0][1] = linearHelper(xyzHi[0],xyzHi[1],xyzHi[2], texture, borderColor, texFormat, texWHD, addressMode, nDimension-1, bComparison, compareFunc, compareValue, minMaxFilterType,&bUseMinMaxResult[0][1]);
			MinMaxColor::SetUNormPrecision(6);
			switch(minMaxFilterType)
			{
			case MINIMUM_FILTER:
			case MAXIMUM_FILTER:
				// Note this technique only works for 1D textures.
				// The recursive design of linearHelper creates virtual texel samples with weights larger than the 0 tolerance
				if( ((1 - weights[nDimension-1][0]) == 0.0f) )
				{
					bUseMinMaxResult[0][0] = false;
				}
				else if( (1 - weights[nDimension-1][0]) <= c_MaxQ8ZeroEquivalent) // Sample weights are Q8 min
				{
					bUseMinMaxResult[0][0] = false;
					uMergeMinMaxResultMask |= 0x1;
				}
				if( ((1 - weights[nDimension-1][1]) == 0.0f) )
				{
					bUseMinMaxResult[0][1] = false;
				}
				else if( (1 - weights[nDimension-1][1]) <= c_MaxQ8ZeroEquivalent) // Sample weights are Q8 min
				{
					bUseMinMaxResult[0][1] = false;
					uMergeMinMaxResultMask |= 0x2;
				}
				break;
			case NOT_MINMAX_FILTER:
				samplesLoHi[0][0] *= (1 - weights[nDimension-1][0]);
				samplesLoHi[0][1] *= (1 - weights[nDimension-1][1]);
				break;
			default:
				assert(!"Invalid filter.");
			}
			MinMaxColor::SetPrecisionExplicit(precisionBits, unormPrecision);
			// right/bottom/back sample lo & hi
			xyzLo[nDimension-1].lo.first += 1;
			xyzLo[nDimension-1].hi.first += 1;
			xyzHi[nDimension-1].lo.first += 1;
			xyzHi[nDimension-1].hi.first += 1;
			samplesLoHi[1][0] = linearHelper(xyzLo[0],xyzLo[1],xyzLo[2], texture, borderColor, texFormat, texWHD, addressMode, nDimension-1, bComparison, compareFunc, compareValue, minMaxFilterType,&bUseMinMaxResult[1][0]);
			samplesLoHi[1][1] = linearHelper(xyzHi[0],xyzHi[1],xyzHi[2], texture, borderColor, texFormat, texWHD, addressMode, nDimension-1, bComparison, compareFunc, compareValue, minMaxFilterType,&bUseMinMaxResult[1][1]);
			MinMaxColor::SetUNormPrecision(6);
			switch(minMaxFilterType)
			{
			case MINIMUM_FILTER:
			case MAXIMUM_FILTER:
				if( (weights[nDimension-1][0] == 0.0f) )
				{
					bUseMinMaxResult[1][0] = false;
				}
				else if( weights[nDimension-1][0] <= c_MaxQ8ZeroEquivalent) // Sample weights are Q8 min
				{
					bUseMinMaxResult[1][0] = false;
					uMergeMinMaxResultMask |= 0x4;
				}
				if( (weights[nDimension-1][1] == 0.0f) )
				{
					bUseMinMaxResult[1][1] = false;
				}
				else if( weights[nDimension-1][1] <= c_MaxQ8ZeroEquivalent) // Sample weights are Q8 min
				{
					bUseMinMaxResult[1][1] = false;
					uMergeMinMaxResultMask |= 0x8;
				}
				break;
			case NOT_MINMAX_FILTER:
				samplesLoHi[1][0] *= weights[nDimension-1][0];
				samplesLoHi[1][1] *= weights[nDimension-1][1];
				break;
			}

			MinMaxColor::SetPrecisionExplicit(precisionBits, unormPrecision);
			// combine left & right samples, then merge hi/lo color results

			MinMaxColor retCol;
			switch(minMaxFilterType)
			{
			case MINIMUM_FILTER:
			case MAXIMUM_FILTER:
				{
					MinMaxColor expanded[2];
					bool bUseResult[2];
					mergeMinMaxFilterSamples(NOT_MINMAX_FILTER, // expand, don't min/max
											 samplesLoHi[0][0],bUseMinMaxResult[0][0],
											 samplesLoHi[0][1],bUseMinMaxResult[0][1],
											 expanded[0],&bUseResult[0]);
					mergeMinMaxFilterSamples(NOT_MINMAX_FILTER, // expand, don't min/max
											 samplesLoHi[1][0],bUseMinMaxResult[1][0],
											 samplesLoHi[1][1],bUseMinMaxResult[1][1],
											 expanded[1],&bUseResult[1]);
					mergeMinMaxFilterSamples(minMaxFilterType, // apply min/max
											 expanded[0],bUseResult[0],
											 expanded[1],bUseResult[1],
											 retCol,pbOutUseMinMaxResult);
					for (UINT8 uMergeSampleMask = 0x1; uMergeSampleMask < (uMergeMinMaxResultMask ? 0x10 : 0); ++uMergeSampleMask)
					{
						if (uMergeSampleMask & ~uMergeMinMaxResultMask) continue;
                        
						MinMaxColor expanded2;
						bool bUseResult2;
						mergeMinMaxFilterSamples(NOT_MINMAX_FILTER, // expand, don't min/max
												 samplesLoHi[0][0],bUseMinMaxResult[0][0] || (uMergeSampleMask & 0x1) != 0,
												 samplesLoHi[0][1],bUseMinMaxResult[0][1] || (uMergeSampleMask & 0x2) != 0,
												 expanded[0],&bUseResult[0]);
						mergeMinMaxFilterSamples(NOT_MINMAX_FILTER, // expand, don't min/max
												 samplesLoHi[1][0],bUseMinMaxResult[1][0] || (uMergeSampleMask & 0x4) != 0,
												 samplesLoHi[1][1],bUseMinMaxResult[1][1] || (uMergeSampleMask & 0x8) != 0,
												 expanded[1],&bUseResult[1]);
						mergeMinMaxFilterSamples(minMaxFilterType, // apply min/max
												 expanded[0],bUseResult[0],
												 expanded[1],bUseResult[1],
												 expanded2,&bUseResult2);
						mergeMinMaxFilterSamples(NOT_MINMAX_FILTER, // expand, don't min/max
												 expanded2,bUseResult2,
												 retCol,*pbOutUseMinMaxResult,
												 retCol,pbOutUseMinMaxResult);
					}
				}
				break;
			case NOT_MINMAX_FILTER:
				retCol = samplesLoHi[0][0] + samplesLoHi[1][0];
				retCol.expand(samplesLoHi[0][1] + samplesLoHi[1][1]);
				if( pbOutUseMinMaxResult ) *pbOutUseMinMaxResult = false; // unused parameter
				break;
			}
			return retCol;
		}
	}
}

MinMaxColor
doTexSampleLnr(TEST_VEC4F texCoord, 
			   const TEST_COLOR* texture, 
			   const TEST_COLOR* borderColor, 
			   DXGI_FORMAT texFormat,
			   int texWHD[3], 
			   D3D11_TEXTURE_ADDRESS_MODE addressMode[3],
			   D3D11_SRV_DIMENSION texType,
			   bool bComparison,
			   D3D11_COMPARISON_FUNC compareFunc, 
			   float compareValue,
			   MINMAX_FILTERTYPE minMaxFilter,
			   int immOffsets[3],
			   FilterHardwareSpecs* pHardwareSpecs,
			   bool* pbCornerCube = NULL,
			   int cubeFace = -1)
{
	MinMaxColor retColor(*borderColor);
	bool bRetFilledByMinMaxFilterSample = false;
	MinMaxTexelWeight x(texelWeight(0, 0.f)), y(texelWeight(0, 0.f)), z(texelWeight(0, 0.f));
	texelWeight xSingle, ySingle, zSingle;
	bool useSingleVals = false;
	switch (texType)
	{
	default:
		assert(!"Invalid texture type");
		return MinMaxColor(*borderColor);
		// intentional fall-through
	case D3D11_SRV_DIMENSION_TEXTURE3D:
		z = doTexAddressLnrMinMax(texCoord.z, texWHD[2], addressMode[2], immOffsets[2], pHardwareSpecs);
		if (z.lo.first + 1 == z.hi.first)
		{	// place a sample exactly on a texel for maximum weighting
			zSingle = texelWeight(z.hi.first, 0.f);
			useSingleVals = true;
		}
		else
			zSingle = doTexAddressLnr(texCoord.z, texWHD[2], addressMode[2], immOffsets[2], pHardwareSpecs);
	case D3D11_SRV_DIMENSION_TEXTURE2D:
		y = doTexAddressLnrMinMax(texCoord.y, texWHD[1], addressMode[1], immOffsets[1], pHardwareSpecs);
		if (y.lo.first + 1 == y.hi.first)
		{	// place a sample exactly on a texel for maximum weighting
			ySingle = texelWeight(y.hi.first, 0.f);
			useSingleVals = true;
		}
		else
			ySingle = doTexAddressLnr(texCoord.y, texWHD[1], addressMode[1], immOffsets[1], pHardwareSpecs);
	case D3D11_SRV_DIMENSION_TEXTURE1D:
		x = doTexAddressLnrMinMax(texCoord.x, texWHD[0], addressMode[0], immOffsets[0], pHardwareSpecs);
		if (x.lo.first + 1 == x.hi.first)
		{	// place a sample exactly on a texel for maximum weighting
			xSingle = texelWeight(x.hi.first, 0.f);
			useSingleVals = true;
		}
		else
			xSingle = doTexAddressLnr(texCoord.x, texWHD[0], addressMode[0], immOffsets[0], pHardwareSpecs);
		break;
	case D3D11_SRV_DIMENSION_TEXTURECUBE:
		break;
	}

	switch (texType)
	{
	case D3D11_SRV_DIMENSION_TEXTURE3D:
		switch(minMaxFilter)
		{
		case MINIMUM_FILTER:
		case MAXIMUM_FILTER:
			{
				bool bUseResult = false;
				retColor = linearHelper(x,y,z,texture,borderColor,texFormat,texWHD,addressMode,3,bComparison,compareFunc,compareValue,minMaxFilter,&bUseResult);
				assert(bUseResult); // At least one sample must have a nonzero weight
			}
			break;
		case NOT_MINMAX_FILTER:
			retColor = linearHelper(x,y,z,texture,borderColor,texFormat,texWHD,addressMode,3,bComparison,compareFunc,compareValue,minMaxFilter,NULL);
			if (useSingleVals)
			{
				retColor.expand(linearHelper(MinMaxTexelWeight(xSingle),MinMaxTexelWeight(ySingle),MinMaxTexelWeight(zSingle),
					texture,borderColor,texFormat,texWHD,addressMode,3,bComparison,compareFunc,compareValue,minMaxFilter,NULL));
			}
			break;
		default:
			assert(!"Invalid filter.");
		}
		break;
	case D3D11_SRV_DIMENSION_TEXTURE2D:
		switch(minMaxFilter)
		{
		case MINIMUM_FILTER:
		case MAXIMUM_FILTER:
			{
				bool bUseResult = false;
				retColor = linearHelper(x,y,z,texture,borderColor,texFormat,texWHD,addressMode,2,bComparison,compareFunc,compareValue,minMaxFilter,&bUseResult);
				assert(bUseResult); // At least one sample must have a nonzero weight
			}
			break;
		case NOT_MINMAX_FILTER:
			retColor = linearHelper(x,y,z,texture,borderColor,texFormat,texWHD,addressMode,2,bComparison,compareFunc,compareValue,minMaxFilter,NULL);
			if (useSingleVals)
			{
				retColor.expand(linearHelper(MinMaxTexelWeight(xSingle),MinMaxTexelWeight(ySingle),MinMaxTexelWeight(zSingle),
					texture,borderColor,texFormat,texWHD,addressMode,2,bComparison,compareFunc,compareValue,minMaxFilter,NULL));
			}
			break;
		default:
			assert(!"Invalid filter.");
		}
		break;
	case D3D11_SRV_DIMENSION_TEXTURE1D:
		switch(minMaxFilter)
		{
		case MINIMUM_FILTER:
		case MAXIMUM_FILTER:
			{
				bool bUseResult = false;
				retColor = linearHelper(x,y,z,texture,borderColor,texFormat,texWHD,addressMode,1,bComparison,compareFunc,compareValue,minMaxFilter,&bUseResult);
				assert(bUseResult); // At least one sample must have a nonzero weight
			}
			break;
		case NOT_MINMAX_FILTER:
			retColor = linearHelper(x,y,z,texture,borderColor,texFormat,texWHD,addressMode,1,bComparison,compareFunc,compareValue,minMaxFilter,NULL);
			if (useSingleVals)
			{
				retColor.expand(linearHelper(MinMaxTexelWeight(xSingle),MinMaxTexelWeight(ySingle),MinMaxTexelWeight(zSingle),
					texture,borderColor,texFormat,texWHD,addressMode,1,bComparison,compareFunc,compareValue,minMaxFilter,NULL));
			}
			break;
		default:
			assert(!"Invalid filter.");
		}
		break;
	case D3D11_SRV_DIMENSION_TEXTURECUBE:
		assert(texWHD[0] == texWHD[1] && texWHD[2] == 1);
		{
			int precisionBits;
			bool unormPrecision;
			MinMaxColor::GetPrecision(&precisionBits, &unormPrecision);

			std::vector<int> faceNums = doCubeFaceSelect(texCoord);
			for (size_t n = 0; n < faceNums.size(); ++n)
			{
				const int faceNum = faceNums[n];
				TEST_VEC4F texCoord2d = doCubeFaceTransform(texCoord, faceNum);
				const size_t facePitch = texWHD[0]*texWHD[0];
				const TEST_COLOR* faceTexture = texture + (facePitch*faceNum);

				x = doTexAddressLnrMinMax(texCoord2d.x, texWHD[0], D3D11_TEXTURE_ADDRESS_BORDER, 0, pHardwareSpecs);
				y = doTexAddressLnrMinMax(texCoord2d.y, texWHD[1], D3D11_TEXTURE_ADDRESS_BORDER, 0, pHardwareSpecs);

				for (int xLoHi = 0; xLoHi < 3; ++xLoHi)
				{
					if (xLoHi > 0 && x.isSingular())
						continue;
					if (xLoHi == 2)
					{
						// only execute 3rd step when sample is across texel border
						if (x.lo.first == x.hi.first)
							continue;
					}
					for (int yLoHi = 0; yLoHi < 3; ++yLoHi)
					{
						if (yLoHi > 0 && y.isSingular())
							continue;
						if (yLoHi == 2)
						{
							// only execute 3rd step when sample is across texel border
							if (y.lo.first == y.hi.first)
								continue;
						}
						texelWeight xCoord;
						texelWeight yCoord;
						switch (xLoHi)
						{
						case 0: xCoord = x.lo; break;
						case 1: xCoord = x.hi; break;
						case 2:
							xCoord = doTexAddressLnr(texCoord2d.x, texWHD[0], D3D11_TEXTURE_ADDRESS_BORDER, 0, pHardwareSpecs);
							break;
						default:
							assert(!"Unexpected loop value");
							break;
						}
						switch (yLoHi)
						{
						case 0: yCoord = y.lo; break;
						case 1: yCoord = y.hi; break;
						case 2: 
							yCoord = doTexAddressLnr(texCoord2d.y, texWHD[0], D3D11_TEXTURE_ADDRESS_BORDER, 0, pHardwareSpecs);
							break;
						default:
							assert(!"Unexpected loop value");
							break;
						}

						MinMaxColor cubeSamples[2][2];
						bool samplesGood[2][2];
						int badOne = -1;
						for (int iY = 0; iY < 2; ++iY)
						{
							for (int iX = 0; iX < 2; ++iX)
							{
								samplesGood[iY][iX] = getCubeTexColor(xCoord.first+iX, yCoord.first+iY,texFormat,texWHD,texture,faceNum,cubeSamples[iY][iX],bComparison,compareFunc,compareValue);
								if (!samplesGood[iY][iX])
								{
									badOne = iY*2+iX;
									if( pbCornerCube )
									{
										*pbCornerCube = true;
									}
								}
							}
						}
						// ugly hack?!
						float sampleWeights[2][2] = {
							{(1-xCoord.second)*(1-yCoord.second), xCoord.second*(1-yCoord.second)},
							{(1-xCoord.second)*yCoord.second,     xCoord.second*yCoord.second}
						};
						MinMaxFloat sampleWeightRanges[2][2] = {{MinMaxFloat(0),MinMaxFloat(0)},{MinMaxFloat(0),MinMaxFloat(0)}};
						for (int iY = 0; iY < 2; ++iY)
							for (int iX = 0; iX < 2; ++iX)
								sampleWeightRanges[iY][iX] = MinMaxFloat(sampleWeights[iY][iX]);
						if (badOne >= 0) // handle a corner wrap, distribute the "dead" weight among the others
						{
							float badWeight = sampleWeights[badOne/2][badOne%2];
							const MinMaxDbl badWeightThird = MinMaxDbl::floatULPDrift(badWeight / 3, 6, true);
							const MinMaxDbl badWeightHalf = MinMaxDbl::floatULPDrift(badWeight / 2, 6, true);
							const MinMaxFloat thirdWeight = MinMaxFloat((float)badWeightThird.lo, (float)badWeightThird.hi);
							for (int iY = 0; iY < 2; ++iY)
								for (int iX = 0; iX < 2; ++iX)
								{
									MinMaxFloat halfWeight = MinMaxFloat((float)badWeightHalf.lo, (float)badWeightHalf.hi);
									float currWeight = sampleWeights[iY][iX];
									if (iX == 1-(badOne%2) && iY == 1-(badOne/2))
										halfWeight = MinMaxFloat(0);
									MinMaxFloat currPlusThird(currWeight);
									MinMaxFloat currPlusHalf(currWeight);
									currPlusThird += thirdWeight;
									currPlusHalf += halfWeight;
									sampleWeightRanges[iY][iX] = currPlusThird;
									sampleWeightRanges[iY][iX].expand(currPlusHalf);
								}
								sampleWeights[badOne/2][badOne%2] = 0;
								sampleWeightRanges[badOne/2][badOne%2] = MinMaxFloat(0);
						}
						MinMaxColor totalColor;

						switch(minMaxFilter)
						{
						case MINIMUM_FILTER:
						case MAXIMUM_FILTER:
                            {
								bool bMinContributorGotSample = false, bMaxContributorGotSample = false;
								MinMaxColor MinFilterContributors(*borderColor), MaxFilterContributors(*borderColor);
								for (int iY = 0; iY < 2; ++iY)
								{
									for (int iX = 0; iX < 2; ++iX)
									{
										if (sampleWeightRanges[iY][iX].lo == 0.f)
										{
											// When sample weights are 0, the texel value does not contribute to the filter result.
										}
										else
										{
											MinMaxColor::SetUNormPrecision(6);
											MinMaxColor::SetPrecisionExplicit(precisionBits, unormPrecision);
											if (sampleWeightRanges[iY][iX].lo <= c_MaxQ8ZeroEquivalent) // Sample weights are Q8 min
											{
												// When a sample weight is ~= 0, the end minmaxvalue must account for the texel value
												// potentially affecting the filter operation.
												mergeMinMaxFilterSamples(minMaxFilter,
													cubeSamples[iY][iX], true,
													MinFilterContributors, bMinContributorGotSample,
													MinFilterContributors, &bMinContributorGotSample);
											}
											else
											{
												// When sample weight is significantly > 0,
												// the filter performs min/max on the texel values
												mergeMinMaxFilterSamples(minMaxFilter,
													cubeSamples[iY][iX], true,
													MaxFilterContributors, bMaxContributorGotSample,
													MaxFilterContributors, &bMaxContributorGotSample);
											}
										}
									}
								}
								if (bMinContributorGotSample)
								{
									// Aggregate the filter on both the collection of samples that obviously contribute,
									// and those that may not.
									mergeMinMaxFilterSamples(minMaxFilter,
										MinFilterContributors,true,
										MaxFilterContributors,bMaxContributorGotSample,
										MinFilterContributors,&bMinContributorGotSample);

									// Expand the mixmaxvalue by the array with min contributors and max contributors.
									mergeMinMaxFilterSamples(NOT_MINMAX_FILTER,
										MaxFilterContributors,bMaxContributorGotSample,
										MinFilterContributors,true,
										MaxFilterContributors,&bMaxContributorGotSample); // expand, don't min/max
								}
								mergeMinMaxFilterSamples(NOT_MINMAX_FILTER,
									MaxFilterContributors,bMaxContributorGotSample,
									retColor,bRetFilledByMinMaxFilterSample,
									retColor,&bRetFilledByMinMaxFilterSample); // expand, don't min/max
                            }
							break;
						case NOT_MINMAX_FILTER:
							for (int iY = 0; iY < 2; ++iY)
							{
								for (int iX = 0; iX < 2; ++iX)
								{
									if (sampleWeights[iY][iX] > 0)
									{
										MinMaxColor::SetUNormPrecision(6);
										MinMaxColor loColor = cubeSamples[iY][iX] * sampleWeightRanges[iY][iX].lo;
										MinMaxColor hiColor = cubeSamples[iY][iX] * sampleWeightRanges[iY][iX].hi;
										MinMaxColor::SetPrecisionExplicit(precisionBits, unormPrecision);
										MinMaxColor sampleColor(loColor);
										sampleColor.expand(hiColor);
										totalColor += sampleColor;
									}
								}
							}
							if (n == 0 && xLoHi == 0 && yLoHi == 0)
								retColor = totalColor;
							else
								retColor.expand(totalColor);
							break;
						default:
							assert(!"Invalid filter.");
						}
					}
				}
			}
		}
		break;
	}

	// no longer meaningful since border color can be quantized or clamped
	//assert(retColor.hi.r - retColor.lo.r < 0.5 || bComparison);
	return retColor;
}

MinMaxColor
doTexSample(TEST_VEC4F texCoord, 
			TEST_VEC4F texDerivs[2],
			TEST_COLOR* const* texture, 
			const TEST_COLOR* borderColor, 
			DXGI_FORMAT texFormat,
			int texWHD[3], 
			int minMip, int maxMip,
			float minLOD, float maxLOD,
			int maxAniso,
			float mipLODBias, float sample_bias,
			D3D11_FILTER d3dFilterMode, 
			D3D11_TEXTURE_ADDRESS_MODE addressMode[3],
			D3D11_SRV_DIMENSION texType,
			bool bLODOverride, float texLODOverride,
			D3D11_COMPARISON_FUNC compareFunc, 
			float compareValue,
			int immOffsets[3],
			float resourceMinLOD,
			FilterHardwareSpecs* pHardwareSpecs,
			bool* pbCornerCube = NULL)
{
	assert(0 <= minMip && minMip < 256);
	assert(0 <= maxMip && maxMip < 256);
	int nDimension = 1;
	switch (texType)
	{
	default:
		assert(!"Unexpected texture type");
	case D3D11_SRV_DIMENSION_TEXTURE1D:
	case D3D11_SRV_DIMENSION_TEXTURE1DARRAY:
		nDimension = 1;
		break;
	case D3D11_SRV_DIMENSION_TEXTURE2D:
	case D3D11_SRV_DIMENSION_TEXTURE2DARRAY:
		nDimension = 2;
		break;
	case D3D11_SRV_DIMENSION_TEXTURE3D:
		nDimension = 3;
		break;
	case D3D11_SRV_DIMENSION_TEXTURECUBE:
		nDimension = 4;
		break;
	}
	float x=0,y=0,z=0;
	float fAnisoLine[2], fAnisoRatio;
	int bestMipWHD[3];
	CFilterConfTest::CalcMipSize(texWHD, minMip, bestMipWHD, nDimension);
	MinMaxFloat fLOD = doLodCalc(texCoord, texDerivs, bestMipWHD, texType,
		(d3dFilterMode == D3D11_FILTER_ANISOTROPIC), fAnisoLine, &fAnisoRatio, maxAniso);
	if (bLODOverride)
		fLOD = MinMaxFloat(texLODOverride);
	MinMaxFloat biasedLOD = fLOD + (mipLODBias + sample_bias);
	eFilterMode_t mipMode = eFilter_Invalid;



	switch(d3dFilterMode)
	{
	default:
		assert(!"Invalid texture type");
		break;
	case D3D11_FILTER_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
		mipMode = eFilter_Point;
		break;
	case D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
		mipMode = eFilter_Linear;
		break;
	case D3D11_FILTER_ANISOTROPIC:
	case D3D11_FILTER_COMPARISON_ANISOTROPIC:
	case D3D11_FILTER_MINIMUM_ANISOTROPIC:
	case D3D11_FILTER_MAXIMUM_ANISOTROPIC:
		mipMode = eFilter_Linear;
		break;
	}
	MinMaxBool bMagnifyMinMax((biasedLOD.lo <= 0.0));

	// DX11 forces post-clamp min-mag selection
	if (g_App.m_D3DOptions.FeatureLevel < D3D_FEATURE_LEVEL_11_0)
	{
		assert(0.0f == resourceMinLOD);

		// in dx10, bMagnify is pre-clamp, like dx9. see dx bug 102805
		MinMaxDbl biasedLODRangeLo = MinMaxDbl::floatULPDrift(biasedLOD.lo, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true);
		MinMaxDbl biasedLODRangeHi = MinMaxDbl::floatULPDrift(biasedLOD.hi, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true);
		bMagnifyMinMax.expand(biasedLODRangeLo.lo <= 0);
		bMagnifyMinMax.expand(biasedLODRangeHi.hi <= 0);
	}
	// point-mip rounding happens before LOD clamping/min-mag selection on REF, while after clamping/min-mag selection on HW
	MinMaxFloat clampedLOD = MinMaxFloat( max(minLOD,(min(maxLOD, biasedLOD.lo))), max(minLOD,(min(maxLOD, biasedLOD.hi))) );
	if (mipMode == eFilter_Point)
		biasedLOD += 0.5;
	MinMaxFloat clampedLODREF = MinMaxFloat( max(minLOD,(min(maxLOD, biasedLOD.lo))), max(minLOD,(min(maxLOD, biasedLOD.hi))) );
	

	// apply per-resource LOD clamp
	if (resourceMinLOD > (float)minMip)
	{
		assert(clampedLOD.hi >= clampedLOD.lo);
		clampedLOD = MinMaxFloat( max(clampedLOD.lo, (resourceMinLOD - minMip)), max(clampedLOD.hi, (resourceMinLOD - minMip)) );
		clampedLODREF = MinMaxFloat( max(clampedLODREF.lo, (resourceMinLOD - minMip)), max(clampedLODREF.hi, (resourceMinLOD - minMip)) );
		assert(clampedLOD.hi >= clampedLOD.lo);
		assert(clampedLODREF.hi >= clampedLODREF.lo);
	}

	
	if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
	{
		// in dx11, bMagnify is expressly post-clamp due to interaction with resource mip clamping
		bMagnifyMinMax = MinMaxBool((clampedLOD.lo <= 0.0));
	}
	// post-clamp magnify is also allowable for dx10, with point-mip rounding included for REF; 
	// but we need also allow HW to do min-mag selection without point-mip rounding (windows 8 bugs: 5341 )
	{
		MinMaxDbl clampedLODRangeLoREF = MinMaxDbl::floatULPDrift(clampedLODREF.lo, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true);
		MinMaxDbl clampedLODRangeHiREF = MinMaxDbl::floatULPDrift(clampedLODREF.hi, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true);
		MinMaxDbl clampedLODRangeLo = MinMaxDbl::floatULPDrift(clampedLOD.lo, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true);
		MinMaxDbl clampedLODRangeHi = MinMaxDbl::floatULPDrift(clampedLOD.hi, GetFixedFloatULPVariance(), pHardwareSpecs->SubTexelPrecision, true);

		bMagnifyMinMax.expand(clampedLODRangeLoREF.lo <= 0);
		bMagnifyMinMax.expand(clampedLODRangeHiREF.hi <= 0);
		bMagnifyMinMax.expand(clampedLODRangeLo.lo <= 0);
		bMagnifyMinMax.expand(clampedLODRangeHi.hi <= 0);
	}

	if (mipMode == eFilter_Point)
	{
		// HW does the point-mip rounding after clamp/min-map selection.
		clampedLOD += 0.5;
		// result will accomodate both REF and HW implementation regarding the order of point-mip rounding and clamping.
		clampedLOD.expand(clampedLODREF);
	}


	MINMAX_FILTERTYPE minMaxType = NOT_MINMAX_FILTER;
	switch(d3dFilterMode)
	{
	case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_ANISOTROPIC:
		minMaxType = MINIMUM_FILTER;
		break;
	case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_ANISOTROPIC:
		minMaxType = MAXIMUM_FILTER;
		break;        
	}

	eFilterMode_t filterMode[2] = {eFilter_Invalid, eFilter_Invalid};
	bool bComparison = false;
	switch(d3dFilterMode)
	{
	default:
		assert(!"Invalid texture type");
		break;
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
		bComparison = true; // intentional fall-through
	case D3D11_FILTER_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
		filterMode[0] = eFilter_Point;
		break;
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
		bComparison = true; // intentional fall-through
	case D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
		filterMode[0] = bMagnifyMinMax.lo?eFilter_Linear:eFilter_Point;
		filterMode[1] = bMagnifyMinMax.hi?eFilter_Linear:eFilter_Point;
		break;
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		bComparison = true; // intentional fall-through
	case D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
		filterMode[0] = bMagnifyMinMax.lo?eFilter_Point:eFilter_Linear;
		filterMode[1] = bMagnifyMinMax.hi?eFilter_Point:eFilter_Linear;
		break;
	case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
		bComparison = true; // intentional fall-through
	case D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
		filterMode[0] = eFilter_Linear;
		break;
	case D3D11_FILTER_COMPARISON_ANISOTROPIC:
		bComparison = true; // intentional fall-through
	case D3D11_FILTER_ANISOTROPIC:
	case D3D11_FILTER_MINIMUM_ANISOTROPIC:
	case D3D11_FILTER_MAXIMUM_ANISOTROPIC:
		assert(texType == D3D11_SRV_DIMENSION_TEXTURE2D || texType == D3D11_SRV_DIMENSION_TEXTURE2DARRAY);
		if (maxAniso <= 1)
			filterMode[0] = eFilter_Linear;
		else
			filterMode[0] = eFilter_Aniso;
		break;
	}
	MinMaxTexelWeight mipLevel = doMipSelect(clampedLOD, minMip, maxMip, mipMode, pHardwareSpecs );
	MinMaxFloat mipWeights[2] = 
	{
		MinMaxFloat(1.0f-mipLevel.hi.second,1.0f-mipLevel.lo.second), 
		MinMaxFloat(mipLevel.lo.second,mipLevel.hi.second), 
	};
	MinMaxColor mipSamples[2];
	ZeroMemory(mipSamples, sizeof(mipSamples));
	int numMips = ((mipMode == eFilter_Point) || (mipWeights[0] == 1))?1:2;
	for (int mip = 0; mip < numMips; ++mip)
	{
		bool bValueRecorded = false;
		int mipWHD[3];
		//for (int hiLoVal = 0; hiLoVal < (mipLevel.isSingular()?1:2); ++hiLoVal)
		for (int hiLoVal = mipLevel.lo.first; hiLoVal <= mipLevel.hi.first; ++hiLoVal)
		{
			//int mipClampSingular = mip + ((hiLoVal==0) ? mipLevel.lo.first : mipLevel.hi.first);
			int mipClampSingular = mip + hiLoVal;
			mipClampSingular = max(minMip, min(maxMip, mipClampSingular));
			CFilterConfTest::CalcMipSize(texWHD, mipClampSingular, mipWHD, nDimension);
			const TEST_COLOR* pTexData = texture[mipClampSingular];
			for (int nFilterMode = 0; nFilterMode < 2; ++nFilterMode)
			{
				if (nFilterMode > 0)
				{
					if (filterMode[nFilterMode] == eFilter_Invalid)
						continue;
					if (filterMode[nFilterMode] == filterMode[0])
						continue;
				}
				switch (filterMode[nFilterMode])
				{
				default:
					assert(!"Invalid filter mode");
					break;
				case eFilter_Point:
					{
						MinMaxColor mipSampleSingular = doTexSamplePt( texCoord, pTexData, borderColor, texFormat, mipWHD, addressMode, texType, bComparison, compareFunc, compareValue, immOffsets, pHardwareSpecs );
						if (!bValueRecorded)
							mipSamples[mip] = mipSampleSingular;
						else
							mipSamples[mip].expand(mipSampleSingular);
					}
					break;
				case eFilter_Linear:
					{
						MinMaxColor mipSampleSingular = doTexSampleLnr( texCoord, pTexData, borderColor, texFormat, mipWHD, addressMode, texType, bComparison, compareFunc, compareValue, minMaxType, immOffsets, pHardwareSpecs, pbCornerCube );
						if (!bValueRecorded)
							mipSamples[mip] = mipSampleSingular;
						else
							mipSamples[mip].expand(mipSampleSingular);
					}
					break;
				}
				bValueRecorded = true;
			}
		}
	}
	MinMaxColor retVal;
	switch(minMaxType)
	{
	case NOT_MINMAX_FILTER:
		{
			MinMaxColor retVal_A = mipSamples[0] * mipWeights[0].lo;
			if (!mipWeights[0].isSingular())
				retVal_A.expand(mipSamples[0] * mipWeights[0].hi);
			MinMaxColor retVal_B = mipSamples[1] * mipWeights[1].lo;
			if (!mipWeights[1].isSingular())
				retVal_B.expand(mipSamples[1] * mipWeights[1].hi);
			retVal = retVal_A + retVal_B;
		}
		break;
	case MINIMUM_FILTER:
	case MAXIMUM_FILTER:
		{
			MinMaxColor retVal_A;
			bool bUseA = false;
			// Note this doesn't work when creating a virtual sample and weight for each mip.
			// Sample weights smaller than a particular tolerance for fixed-point math must be treated special,
			// and a virtual weight for each mip prevents the weight from ever being under this tolerance.
			if( mipWeights[0].lo == 0 )
			{
				// include result for only one mip used
				assert(mipWeights[1].hi == 1);
				mergeMinMaxFilterSamples(NOT_MINMAX_FILTER,retVal_A,bUseA,mipSamples[1],true,retVal_A,&bUseA); // expand, don't min/max
			}
			if( mipWeights[1].lo == 0 )
			{
				// include result for only one mip used
				assert(mipWeights[0].hi == 1);
				mergeMinMaxFilterSamples(NOT_MINMAX_FILTER,retVal_A,bUseA,mipSamples[0],true,retVal_A,&bUseA); // expand, don't min/max
			}
			// get result for applying both mips
			MinMaxColor retVal_B, retVal_C, retVal_D;
			bool bUseB = false;
			if( (mipWeights[0].lo != 0) || (!mipWeights[0].isSingular()) )
			{
				retVal_B = mipSamples[0];
				bUseB = true;
			}
			bool bUseC = false;
			if( (mipWeights[1].lo != 0) || (!mipWeights[1].isSingular()) )
			{
				retVal_C = mipSamples[1];
				bUseC = true;
			}
			bool bUseD = false;
			mergeMinMaxFilterSamples(minMaxType,retVal_B,bUseB,retVal_C,bUseC,retVal_D,&bUseD); // do min/max filter 
			// combine all result options
			mergeMinMaxFilterSamples(NOT_MINMAX_FILTER,retVal_A,bUseA,retVal_D,bUseD,retVal,NULL); // expand, don't min/max
		}
		break;
	default:
		assert(!"Invalid filter.");
	}

	// Expect out-of-bounds behavior when resourceMinLOD does not intersect the set of mips
	// defined in the SRV
	if (resourceMinLOD > (float)maxMip)
	{
		assert(g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0);
		TEST_COLOR outOfBoundsColor = doFormatDefaults(s_defaultColor, texFormat);
		retVal = MinMaxColor(outOfBoundsColor);
	}

	return retVal;
}

void
vec3_sub(float out[3], float head[3], float tail[3])
{
	out[0] = head[0] - tail[0];
	out[1] = head[1] - tail[1];
	out[2] = head[2] - tail[2];
}

float
vec3_dot(float v1[3], float v2[3])
{
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] * v2[2];
}

void
vec3_cross(float out[3], float v1[3], float v2[3])
{
	float temp[3];
	temp[0] = v1[1]*v2[2] - v1[2]*v2[1];
	temp[1] = v1[2]*v2[0] - v1[0]*v2[2];
	temp[2] = v1[0]*v2[1] - v1[1]*v2[0];
	out[0] = temp[0];
	out[1] = temp[1];
	out[2] = temp[2];
}

void
vec3_normalize(float v[3])
{
	float len = sqrtf(vec3_dot(v, v));
	float invlen = 1.0f/len;
	v[0] *= invlen;
	v[1] *= invlen;
	v[2] *= invlen;
}

void
make_plane(float plane[4], float v0[3], float v1[3], float v2[3])
{
	float d1[3], d2[3];
	vec3_sub(d1, v1, v0);
	vec3_sub(d2, v2, v0);
	vec3_cross(plane, d1, d2);
	vec3_normalize(plane);
	plane[3] = -vec3_dot(plane, v0);
}

float
plane_intersect(float plane[4], float planept[3], float p0[3], float p1[3])
{
	float d1[3], d2[3];
	vec3_sub(d1, planept, p0);
	vec3_sub(d2, p1, p0);
	return vec3_dot(plane, d1) / vec3_dot(plane, d2);
}

float
test_point(float plane[4], float planePt[3], float x, float y)
{
	float testPt[2][3] = {
		{x, y, 0},
		{x, y, 1}
	};
	return plane_intersect(plane, planePt, testPt[0], testPt[1]);
}

static TEST_VEC4F interpolateCoords(const TEST_VEC4F inputCoords[4], float xFrac, float yFrac)
{
	TEST_VEC4F topVals = inputCoords[0] * (1-xFrac) + inputCoords[1] * xFrac;
	TEST_VEC4F botVals = inputCoords[3] * (1-xFrac) + inputCoords[2] * xFrac;
	TEST_VEC4F finalVal = topVals * (1-yFrac) + botVals * (yFrac);

	return finalVal;
}

static float interpolateTexBias(float xFrac, float yFrac)
{
	float biasVal = (xFrac + yFrac) / 2;

	return (biasVal * 31.99f) - 16.0f;
}

static float interpolateComparisonValue(float xFrac, float yFrac)
{
	return (xFrac + yFrac) / 2;
}

static float interpolateTexLevel(float xFrac, float yFrac)
{
	float levelVal = (xFrac + yFrac) / 2;
	return (levelVal * 7) - 1;
}

TEST_RESULT
CFilterConfTest::TestDrawnPixels(const TEST_VEC4F inTexCoords[4], float fThresholdExtra)
{
	MinMaxColor::SetPrecisionFormat(m_eFormat);

	SetD3DFeatureLevel( m_pDevice->GetFeatureLevel() );
	D3D11_SAMPLER_DESC samplerDesc = m_SamplerDesc;
	if (m_bDefaultSampler)
	{
		samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
		samplerDesc.MipLODBias = 0;
		samplerDesc.MaxAnisotropy = 1;
		samplerDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
		for (int n = 0; n < 4; ++n)
			samplerDesc.BorderColor[n] = 1.0f;
		samplerDesc.MinLOD = -FLT_MAX;
		samplerDesc.MaxLOD = FLT_MAX;
	}
	FilterHardwareSpecs hardwareSpecs;
	hardwareSpecs.SubTexelPrecision = 6; // start at n.6 as per spec, allow higher precision
	if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
		hardwareSpecs.SubTexelPrecision = 8;
	assert(0 <= fThresholdExtra && fThresholdExtra < 1);
	bool bComparisonFilter = false;
	bool bMinMaxFilter = false;
	switch (samplerDesc.Filter)
	{
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_ANISOTROPIC:
		bComparisonFilter = true;
		break;
	case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_MINIMUM_ANISOTROPIC:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_MAXIMUM_ANISOTROPIC:
		bMinMaxFilter = true;
	default:
		break;
	}
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_eFormat);
	const DXGIFormatInfo* pEquivTexFormatInfo = CFormatInfoMap::Find(MinMaxRange::GetFormatEquivalentBits(m_eFormat));
	bool bCoordReport = false;
	TEST_RESULT tRes = RESULT_PASS;
	D3D11_SRV_DIMENSION texType = 
		(m_nDimension==1)
			?D3D11_SRV_DIMENSION_TEXTURE1D
			:(m_nDimension==2)
				?D3D11_SRV_DIMENSION_TEXTURE2D
				:(m_nDimension==3)
					?D3D11_SRV_DIMENSION_TEXTURE3D
					:(m_nDimension==4)
						?D3D11_SRV_DIMENSION_TEXTURECUBE
						:D3D11_SRV_DIMENSION_BUFFER;
	TEST_COLOR borderColor = {
		samplerDesc.BorderColor[0],
		samplerDesc.BorderColor[1],
		samplerDesc.BorderColor[2],
		samplerDesc.BorderColor[3],
	};
	int numFailed = 0;
	int x,y;
	for (y = 0; y < m_nPrimitiveH; y += 2)
	{
		for (x = 0; x < m_nPrimitiveW; x += 2)
		{
			int pixelNum;
			float primWeights[4][2];
			TEST_VEC4F texCoords[4];
			TEST_VEC4F texDerivs[4][2];
			float texBiases[4];
			float texCompareVals[4];
			float texLOD[4];
			for (pixelNum = 0; pixelNum < 4; ++pixelNum)
			{
				int numFirst=0, numSecond=0;
				for (int n = 0; n < m_nMultiSamples; ++n)
				{
					double xPos = x + (pixelNum%2);
					double yPos = y + (pixelNum/2);
					xPos += 1.0/(m_nMultiSamples*2);
					yPos += 1.0/(m_nMultiSamples*2);
					xPos += n*1.0/m_nMultiSamples;
					yPos += ((n+m_nMultiSamples/2)%m_nMultiSamples)*1.0/m_nMultiSamples;

					if (xPos >= yPos)
						numFirst += 1;
					else
						numSecond += 1;
				}
				primWeights[pixelNum][0] = ((float)numFirst)/m_nMultiSamples;
				primWeights[pixelNum][1] = ((float)numSecond)/m_nMultiSamples;
				float xPos = x + (pixelNum%2) + 0.5f;
				float yPos = y + (pixelNum/2) + 0.5f;

				const float xFrac = xPos / m_nPrimitiveW;
				const float yFrac = yPos / m_nPrimitiveH;
				TEST_VEC4F coord = interpolateCoords(inTexCoords, xFrac, yFrac);
				
				float lod = (m_SampleFunction == SampleLevel) ? interpolateTexLevel(xFrac, yFrac) : 0;
				float bias = (m_SampleFunction == SampleBias) ? interpolateTexBias(xFrac, yFrac) : 0;
				float compareVal = bComparisonFilter ? interpolateComparisonValue(xFrac, yFrac) : 0;

				texLOD[pixelNum] = (m_SampleFunction == SampleCmpLvlZero)?0:lod;
				texBiases[pixelNum] = bias;
				texCompareVals[pixelNum] = compareVal;
				texCoords[pixelNum] = coord;
			}
			for (pixelNum = 0; pixelNum < 4; ++pixelNum)
			{
				//int xOff = pixelNum%2;
				//int yOff = pixelNum/2;

				texDerivs[pixelNum][0] 
					= texCoords[pixelNum|1]
					- texCoords[pixelNum&(~1)];
				texDerivs[pixelNum][1]
					= texCoords[pixelNum|2]
					- texCoords[pixelNum&(~2)];
			}
			for (pixelNum = 0; pixelNum < 4; ++pixelNum)
			{
				int xPos = x + (pixelNum%2);
				int yPos = y + (pixelNum/2);
				if (xPos >= m_nPrimitiveW)
					continue;
				if (yPos >= m_nPrimitiveH)
					continue;

				int immOffsets[3] = {0,0,0};
				if (m_bSampleAOffImmI)
				{
					if (xPos < m_nPrimitiveW/2)
					{
						if (yPos < m_nPrimitiveH/2)
						{
							immOffsets[0] = -1;
							immOffsets[1] =  0;
							immOffsets[2] = -1;
						}
						else
						{
							immOffsets[0] =  0;
							immOffsets[1] = -1;
							immOffsets[2] =  0;
						}
					}
					else
					{
						if (yPos < m_nPrimitiveH/2)
						{
							immOffsets[0] = 1;
							immOffsets[1] = 1;
							immOffsets[2] = 2;
						}
						else
						{
							immOffsets[0] = 0;
							immOffsets[1] = 2;
							immOffsets[2] = 1;
						}
					}
				}

				TEST_VEC4F texCoordUsed, *texDerivUsed = NULL;
				MinMaxColor expectColorMinMax;
				ZeroMemory(&expectColorMinMax, sizeof(expectColorMinMax));
				bool bCornerCube = false;
				for (int primNum = 0; primNum < 2; ++primNum)
				{
					if (primWeights[pixelNum][primNum] > 0)
					{
//						assert (primWeights[pixelNum][1-primNum] == 0);
						D3D11_TEXTURE_ADDRESS_MODE texAddressModes[3] = {
							samplerDesc.AddressU,
							samplerDesc.AddressV,
							samplerDesc.AddressW
						};
						TEST_COLOR compareValue = { texCompareVals[pixelNum], 0, 0, 0 };
						compareValue = doFormatQuantize(compareValue, m_eFormat);
						texCoordUsed = texCoords[pixelNum];
						texDerivUsed = texDerivs[pixelNum];
						MinMaxColor sampleColor = doTexSample(texCoords[pixelNum], texDerivs[pixelNum],
							m_pTexData, &borderColor, m_eFormat,
							m_nTextureSize,
							m_nMinMip, m_nNumMips-1,
							samplerDesc.MinLOD, samplerDesc.MaxLOD,
							samplerDesc.MaxAnisotropy, samplerDesc.MipLODBias, texBiases[pixelNum],
							samplerDesc.Filter, texAddressModes, texType, 
							(m_SampleFunction == SampleLevel || m_SampleFunction == SampleCmpLvlZero), texLOD[pixelNum],
							samplerDesc.ComparisonFunc, (float)compareValue.r,
							immOffsets,
							m_fResourceMinLOD,
							&hardwareSpecs,
							&bCornerCube);
						expectColorMinMax = expectColorMinMax + sampleColor * primWeights[pixelNum][primNum];
					}
				}
				if (!bComparisonFilter)
				{
					expectColorMinMax = doFormatDefaults(expectColorMinMax, m_eFormat);
				}

				if( bCornerCube && bMinMaxFilter )
				{
                    // Skip cases which hit cubemap corners because thse are finicky to test with min/max filtering.
					return RESULT_SKIP;
				}

				TEST_COLOR pixelColor;
				HRESULT hr = ReadBackBufferColor(xPos,yPos,&pixelColor);
				double delta = 1.0/(1<<6) + fThresholdExtra;
				float ulpConstraint = 1.5f;
				
				D3D11_FILTER filter10 = samplerDesc.Filter;
				if (D3D11_FILTER_TYPE_LINEAR == filter10)
					ulpConstraint += 1.0f;
				if (D3D11_FILTER_TYPE_LINEAR == filter10)
					ulpConstraint += 1.0f;
				if (D3D11_FILTER_TYPE_LINEAR == filter10)
					ulpConstraint += 1.0f;
				TEST_COLOR ulpDiffs = {0,0,0,0};
				if (!expectColorMinMax.containsULP(pixelColor, pEquivTexFormatInfo, ulpConstraint, ulpDiffs))
				{
					tRes = RESULT_FAIL;
					const bool bLogAllPixels = g_App.LogAllPixelFailures();
					if (GetFramework()->GetTestMode() == WGFTEST_MODE_WHQL)
					{
						if (bLogAllPixels || numFailed == 0)
						{
							WriteToLog( _T("Color@(%d,%d) = (%.10g,%.10g,%.10g,%.10g)\n"), 
								xPos, yPos, 
								pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a
								);
						}
					}
					else
					{
						if (!bCoordReport)
						{
							bCoordReport = true;
							WriteToLog( _T("Texture Coords:\n") );
							WriteToLog( _T("0 = (%g,%g,%g)\n"), 
								inTexCoords[0].x, inTexCoords[0].y, inTexCoords[0].z );
							WriteToLog( _T("1 = (%g,%g,%g)\n"), 
								inTexCoords[1].x, inTexCoords[1].y, inTexCoords[1].z );
							WriteToLog( _T("2 = (%g,%g,%g)\n"), 
								inTexCoords[2].x, inTexCoords[2].y, inTexCoords[2].z );
							WriteToLog( _T("3 = (%g,%g,%g)\n"), 
								inTexCoords[3].x, inTexCoords[3].y, inTexCoords[3].z );
						}
						if (bLogAllPixels || numFailed == 0)
						{
							TEST_COLOR exLo = expectColorMinMax.lo;
							TEST_COLOR exHi = expectColorMinMax.hi;
							TCHAR expectedBuffer[1024];
							expectedBuffer[0] = 0;
							if (expectColorMinMax.isSingular())
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
							TCHAR aoffBuffer[1024];
							aoffBuffer[0] = 0;
							if (m_bSampleAOffImmI)
							{
								_sntprintf( aoffBuffer, sizeof( aoffBuffer ) - 1, 
									_T( "AoffImmI=(%d,%d,%d)" ), 
									immOffsets[0], immOffsets[1], immOffsets[2]
								);
							}
							aoffBuffer[1023] = 0;
							WriteToLog( _T("Color mismatch at RT(x,y)=(%d,%d). Expected %s, found (%.10g,%.10g,%.10g,%.10g) - ULP Diff (%g,%g,%g,%g). texcoord(u,v,w)=(%.10g,%.10g,%.10g), texderiv(X(u,v,w),Y(u,v,w)=((%g,%g,%g),(%g,%g,%g)) %s.\n"), 
								xPos, yPos, 
								expectedBuffer, 
								pixelColor.r, pixelColor.g, pixelColor.b, pixelColor.a,
								ulpDiffs.r, ulpDiffs.g, ulpDiffs.b, ulpDiffs.a, 
								texCoordUsed.x, texCoordUsed.y, texCoordUsed.z, 
								texDerivUsed[0].x, texDerivUsed[0].y, texDerivUsed[0].z, 
								texDerivUsed[1].x, texDerivUsed[1].y, texDerivUsed[1].z,
								aoffBuffer
								);
						}
						else if (numFailed == 1 && !bLogAllPixels)
						{
							WriteToLog( _T("More pixels were wrong. Use -AllPixels to log them\n") );
						}
					}
					numFailed += 1;
				}
			}
		}
	}

	if (numFailed > 0)
		WriteToLog( _T("%d pixels wrong in total."), numFailed );

	return tRes;
}

static void PrintBadMipGenResult(CTest* pTest, MinMaxColor expectColorMinMax, TEST_COLOR foundColor, int x, int y, int z, int arrayIdx, int mipNum)
{
	TEST_COLOR exLo = expectColorMinMax.lo;
	TEST_COLOR exHi = expectColorMinMax.hi;
	TCHAR expectedBuffer[1024];
	if (expectColorMinMax.isSingular())
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
	pTest->WriteToLog( _T("Color mismatch at (array,mipNum)=(%d,%d), scaled texcoord(u,v,w)=(%d,%d,%d). Expected %s, found (%.10g,%.10g,%.10g,%.10g)\n"), 
		arrayIdx, mipNum, x, y, z,
		expectedBuffer, 
		foundColor.r, foundColor.g, foundColor.b, foundColor.a );
}

TEST_RESULT 
CMipGenTest::TestMipGenResult(ID3D11Resource* pTexRsc)
{
	SetD3DFeatureLevel( m_pDevice->GetFeatureLevel() );
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = E_FAIL;
	int numFailed = 0;
	bool mapFailed = false;

	const float ulpConstraint = 3.0f;
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(m_eFormat);
	const DXGIFormatInfo* pEquivTexFormatInfo = CFormatInfoMap::Find(MinMaxRange::GetFormatEquivalentBits(m_eFormat));

	MinMaxColor** mipColorRange = NULL;
	try
	{
		mipColorRange = new MinMaxColor*[m_nNumMips];
		mipColorRange[0] = NULL;
		// generate mips
		FilterHardwareSpecs hardwareSpecs;
		hardwareSpecs.SubTexelPrecision = 6;
		if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
			hardwareSpecs.SubTexelPrecision = 8;
		hardwareSpecs.FixedPt6ULP = false;
		hardwareSpecs.RoundNearestEven = true;
		D3D11_SRV_DIMENSION texType;
		switch(m_nDimension)
		{
		default: 
			assert(!"Invalid texture dimension"); 
			return RESULT_FAIL;
		case 1: texType = D3D11_SRV_DIMENSION_TEXTURE1D; break;
		case 2: texType = D3D11_SRV_DIMENSION_TEXTURE2D; break;
		case 3: texType = D3D11_SRV_DIMENSION_TEXTURE3D; break;
		case 4: texType = D3D11_SRV_DIMENSION_TEXTURE2D; break; // intentional, cube face is 2d
		}

		for (int mipNum = 1; mipNum < m_nNumMips; ++mipNum)
		{
			const bool bIsCube = m_nDimension==4;
			int mipWHD[3];
			CalcMipSize(m_nTextureSize, mipNum, mipWHD, m_nDimension);
			const int prevMip = mipNum - 1;
			int prevMipWHD[3];
			CalcMipSize(m_nTextureSize, prevMip, prevMipWHD, m_nDimension);

			const size_t mipSize = mipWHD[0] * mipWHD[1] * mipWHD[2] * (bIsCube?6:1);
			mipColorRange[mipNum] = new MinMaxColor[mipSize];

			const int zLimit = bIsCube?6:mipWHD[2];
			for (int z = 0; z < zLimit; ++z)
			{
				for (int y = 0; y < mipWHD[1]; ++y)
				{
					for (int x = 0; x < mipWHD[0]; ++x)
					{
						int immOffsets[3] = {0,0,0};
						MinMaxColor sampleColor;
						// fetch color from previous mip level
						if (bIsCube)
						{
							TEST_VEC4F texCoord = {
								(x+0.5f)/mipWHD[0], 
								(y+0.5f)/mipWHD[1], 
								0, 
								1
							};
							const size_t faceTexels = prevMipWHD[0]*prevMipWHD[0];
							TEST_COLOR* texData = m_pTexData[prevMip] + z*faceTexels;
							D3D11_TEXTURE_ADDRESS_MODE addressModes[3] = {
								D3D11_TEXTURE_ADDRESS_CLAMP,
								D3D11_TEXTURE_ADDRESS_CLAMP,
								D3D11_TEXTURE_ADDRESS_CLAMP,
							};
							sampleColor = doTexSampleLnr(texCoord, texData, texData, m_eFormat, prevMipWHD, addressModes, texType, 
								false, D3D11_COMPARISON_ALWAYS, 0, NOT_MINMAX_FILTER, immOffsets, &hardwareSpecs, NULL, z );
						}
						else
						{
							TEST_VEC4F texCoord = {
								(x+0.5f)/mipWHD[0], 
								(y+0.5f)/mipWHD[1], 
								(z+0.5f)/mipWHD[2], 
								1
							};
							TEST_COLOR* texData = m_pTexData[prevMip];
							D3D11_TEXTURE_ADDRESS_MODE addressModes[3] = {
								D3D11_TEXTURE_ADDRESS_CLAMP,
								D3D11_TEXTURE_ADDRESS_CLAMP,
								D3D11_TEXTURE_ADDRESS_CLAMP,
							};
							sampleColor = doTexSampleLnr(texCoord, texData, texData, m_eFormat, prevMipWHD, addressModes, texType, 
								false, D3D11_COMPARISON_ALWAYS, 0, NOT_MINMAX_FILTER, immOffsets, &hardwareSpecs, NULL, -1 );
						}
						TEST_COLOR resColor;
						resColor = (sampleColor.lo + sampleColor.hi) * 0.5f;
						//resColor = sampleColor.hi;
						// now write the value into current mip level
						if (bIsCube)
						{
							const size_t faceTexels = mipWHD[0]*mipWHD[0];
							TEST_COLOR* texData = m_pTexData[mipNum];
							const size_t baseIndex = (x) + mipWHD[0]*y + z*faceTexels;
							texData[baseIndex] = resColor;
							mipColorRange[mipNum][baseIndex] = sampleColor;
						}
						else
						{
							TEST_COLOR* texData = m_pTexData[mipNum];
							size_t baseIndex = (x) + mipWHD[0]*(y + mipWHD[1]*z);
							texData[baseIndex] = resColor;
							mipColorRange[mipNum][baseIndex] = sampleColor;
						}
					}
				}
			}
		}
	}
	catch (std::bad_alloc e)
	{
		tRes = RESULT_FAIL;
		WriteToLog( _T("Memory allocation error.\n") );
		goto testMips_done;
	}

	switch(m_nDimension)
	{
	default:
		assert(!"Invalid dimension value");
		tRes = RESULT_FAIL;
		break;
	case 1:
		{
			ID3D11Texture1D* pTex1d = NULL;
			hr = pTexRsc->QueryInterface(__uuidof(ID3D11Texture1D), (void**)&pTex1d);
			assert(pTex1d != NULL);
			if (FAILED(hr))
			{
				tRes = RESULT_FAIL;
				break;
			}
			D3D11_TEXTURE1D_DESC desc1d;
			pTex1d->GetDesc(&desc1d);
			for (UINT arrayIdx = 0; arrayIdx < (desc1d.ArraySize==0?1:desc1d.ArraySize); ++arrayIdx)
			{
				for (UINT mipNum = 0; mipNum < desc1d.MipLevels; ++mipNum)
				{
					int texWHD[3] = {desc1d.Width, 1, 1}, mipWHD[3];
					CalcMipSize(texWHD, mipNum, mipWHD, m_nDimension);
					BYTE* pData;
					D3D11_MAPPED_SUBRESOURCE mappedRes;
					hr = GetImmediateContext()->Map(pTex1d, mipNum + arrayIdx*desc1d.MipLevels, D3D11_MAP_READ, 0, &mappedRes);
					if (hr == S_OK)
					{
						pData = (BYTE*) mappedRes.pData;
						for (int x = 0; x < mipWHD[0]; ++x)
						{
							TEST_COLOR texColor;
							Texture_ALU::readD3DColor(pData, &texColor, m_eFormat, x);
							TEST_COLOR ulpDiffs;
							//TEST_COLOR expectColor = m_pTexData[mipNum][x];
							//if (!TestColorsSame(expectColor, texColor, m_eFormat, 3.0f, ulpDiffs))
							MinMaxColor expectColorMinMax 
								= (mipColorRange[mipNum]==NULL)
								?MinMaxColor(m_pTexData[mipNum][x])
								:mipColorRange[mipNum][x];
							if (!expectColorMinMax.containsULP(texColor, pEquivTexFormatInfo, ulpConstraint, ulpDiffs))
							{
								tRes = RESULT_FAIL;
								bool bLogAllPixels = g_App.LogAllPixelFailures();
								if (bLogAllPixels || numFailed == 0)
								{
									PrintBadMipGenResult(this, expectColorMinMax, texColor, x, 0, 0, arrayIdx, mipNum);
								}
								else if (numFailed == 1 && !bLogAllPixels)
								{
									WriteToLog( _T("More pixels failed. Use -AllPixels to log them\n") );
								}
								numFailed++;
							}
						}
						GetImmediateContext()->Unmap(pTex1d, mipNum + arrayIdx*desc1d.MipLevels);
					}
					else
					{
						tRes = RESULT_FAIL;
						if (!mapFailed)
							WriteToLog( _T("%s Map() failed"), LOG_HEADER, D3DHResultToString(hr).c_str() );
						mapFailed = true;
					}
				}
			}
			SAFE_RELEASE(pTex1d);
		}
		break;
	case 2:
		{
			ID3D11Texture2D* pTex2d = NULL;
			hr = pTexRsc->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTex2d);
			assert(pTex2d != NULL);
			if (FAILED(hr))
			{
				tRes = RESULT_FAIL;
				break;
			}
			D3D11_TEXTURE2D_DESC desc2d;
			pTex2d->GetDesc(&desc2d);
			for (UINT arrayIdx = 0; arrayIdx < (desc2d.ArraySize==0?1:desc2d.ArraySize); ++arrayIdx)
			{
				for (UINT mipNum = 0; mipNum < desc2d.MipLevels; ++mipNum)
				{
					int texWHD[3] = {desc2d.Width, desc2d.Height, 1}, mipWHD[3];
					CalcMipSize(texWHD, mipNum, mipWHD, m_nDimension);
					D3D11_MAPPED_SUBRESOURCE map2d;
					hr = GetImmediateContext()->Map(pTex2d, mipNum + arrayIdx*desc2d.MipLevels, D3D11_MAP_READ, 0, &map2d);
					if (hr == S_OK)
					{
						for (int y = 0; y < mipWHD[1]; ++y)
						{
							for (int x = 0; x < mipWHD[0]; ++x)
							{
								TEST_COLOR texColor;
								Texture_ALU::readD3DColor((BYTE*)map2d.pData + (y*map2d.RowPitch), &texColor, m_eFormat, x);
								TEST_COLOR ulpDiffs;
								//TEST_COLOR expectColor = m_pTexData[mipNum][x + (y*mipWHD[0])];
								//if (!TestColorsSame(expectColor, texColor, m_eFormat, 3.0f, ulpDiffs))
								MinMaxColor expectColorMinMax 
									= (mipColorRange[mipNum]==NULL)
									?MinMaxColor(m_pTexData[mipNum][x + (y*mipWHD[0])])
									:mipColorRange[mipNum][x + (y*mipWHD[0])];
								if (!expectColorMinMax.containsULP(texColor, pEquivTexFormatInfo, ulpConstraint, ulpDiffs))
								{
									tRes = RESULT_FAIL;
									bool bLogAllPixels = g_App.LogAllPixelFailures();
									if (bLogAllPixels || numFailed == 0)
									{
										PrintBadMipGenResult(this, expectColorMinMax, texColor, x, y, 0, arrayIdx, mipNum);
									}
									else if (numFailed == 1 && !bLogAllPixels)
									{
										WriteToLog( _T("More pixels failed. Use -AllPixels to log them\n") );
									}
									numFailed++;
								}
							}
						}
						GetImmediateContext()->Unmap(pTex2d, mipNum + arrayIdx*desc2d.MipLevels);
					}
					else
					{
						tRes = RESULT_FAIL;
						if (!mapFailed)
							WriteToLog( _T("%s Map() failed"), LOG_HEADER, D3DHResultToString(hr).c_str() );
						mapFailed = true;
					}
				}
			}
			SAFE_RELEASE(pTex2d);
		}
		break;
	case 3:
		{
			ID3D11Texture3D* pTex3d = NULL;
			hr = pTexRsc->QueryInterface(__uuidof(ID3D11Texture3D), (void**)&pTex3d);
			assert(pTex3d != NULL);
			if (FAILED(hr))
			{
				tRes = RESULT_FAIL;
				break;
			}
			D3D11_TEXTURE3D_DESC desc3d;
			pTex3d->GetDesc(&desc3d);
			for (UINT arrayIdx = 0; arrayIdx < 1; ++arrayIdx) // no 3d arrays
			{
				for (UINT mipNum = 0; mipNum < desc3d.MipLevels; ++mipNum)
				{
					int texWHD[3] = {desc3d.Width, desc3d.Height, desc3d.Depth}, mipWHD[3];
					CalcMipSize(texWHD, mipNum, mipWHD, m_nDimension);
					D3D11_MAPPED_SUBRESOURCE map3d;
					hr = GetImmediateContext()->Map(pTex3d, mipNum + arrayIdx*desc3d.MipLevels, D3D11_MAP_READ, 0, &map3d);
					if (hr == S_OK)
					{
						for (int z = 0; z < mipWHD[2]; ++z)
						{
							for (int y = 0; y < mipWHD[1]; ++y)
							{
								for (int x = 0; x < mipWHD[0]; ++x)
								{
									TEST_COLOR texColor;
									Texture_ALU::readD3DColor((BYTE*)map3d.pData + (y*map3d.RowPitch) + (z*map3d.DepthPitch), &texColor, m_eFormat, x);
									TEST_COLOR ulpDiffs;
									//TEST_COLOR expectColor = m_pTexData[mipNum][x + ((y + (z*mipWHD[1]))*mipWHD[0])];
									//if (!TestColorsSame(expectColor, texColor, m_eFormat, 3.0f, ulpDiffs))
									MinMaxColor expectColorMinMax 
										= (mipColorRange[mipNum]==NULL)
										?MinMaxColor(m_pTexData[mipNum][x + ((y + (z*mipWHD[1]))*mipWHD[0])])
										:mipColorRange[mipNum][x + ((y + (z*mipWHD[1]))*mipWHD[0])];
									if (!expectColorMinMax.containsULP(texColor, pEquivTexFormatInfo, ulpConstraint, ulpDiffs))
									{
										tRes = RESULT_FAIL;
										bool bLogAllPixels = g_App.LogAllPixelFailures();
										if (bLogAllPixels || numFailed == 0)
										{
											PrintBadMipGenResult(this, expectColorMinMax, texColor, x, y, z, arrayIdx, mipNum);
										}
										else if (numFailed == 1 && !bLogAllPixels)
										{
											WriteToLog( _T("More pixels failed. Use -AllPixels to log them\n") );
										}
										numFailed++;
									}
								}
							}
						}
						GetImmediateContext()->Unmap(pTex3d, mipNum + arrayIdx*desc3d.MipLevels);
					}
					else
					{
						tRes = RESULT_FAIL;
						if (!mapFailed)
							WriteToLog( _T("%s Map() failed"), LOG_HEADER, D3DHResultToString(hr).c_str() );
						mapFailed = true;
					}
				}
			}
			SAFE_RELEASE(pTex3d);
		}
		break;
	case 4: // cube
		{
			ID3D11Texture2D* pTexCube = NULL;
			hr = pTexRsc->QueryInterface(__uuidof(ID3D11Texture2D), (void**)&pTexCube);
			assert(pTexCube != NULL);
			if (FAILED(hr))
			{
				tRes = RESULT_FAIL;
				break;
			}
			D3D11_TEXTURE2D_DESC descCube;
			pTexCube->GetDesc(&descCube);
			assert(descCube.Width == descCube.Height);
			for (UINT faceIdx = 0; faceIdx < 6; ++faceIdx) // fixed @ arraysize 6
			{
				for (UINT mipNum = 0; mipNum < descCube.MipLevels; ++mipNum)
				{
					int texWHD[3] = {descCube.Width, descCube.Height, 1}, mipWHD[3];
					CalcMipSize(texWHD, mipNum, mipWHD, m_nDimension);
					D3D11_MAPPED_SUBRESOURCE mapCube;
					hr = GetImmediateContext()->Map(pTexCube, mipNum + faceIdx*descCube.MipLevels, D3D11_MAP_READ, 0, &mapCube);
					if (hr == S_OK)
					{
						for (int y = 0; y < mipWHD[1]; ++y)
						{
							for (int x = 0; x < mipWHD[0]; ++x)
							{
								TEST_COLOR texColor;
								Texture_ALU::readD3DColor((BYTE*)mapCube.pData + (y*mapCube.RowPitch), &texColor, m_eFormat, x);
								TEST_COLOR ulpDiffs;
								//TEST_COLOR expectColor = m_pTexData[mipNum][x + ((y + (faceIdx*mipWHD[1]))*mipWHD[0])];
								//if (!TestColorsSame(expectColor, texColor, m_eFormat, 3.0f, ulpDiffs))
								MinMaxColor expectColorMinMax 
									= (mipColorRange[mipNum]==NULL)
									?MinMaxColor(m_pTexData[mipNum][x + ((y + (faceIdx*mipWHD[0]))*mipWHD[0])])
									:mipColorRange[mipNum][x + ((y + (faceIdx*mipWHD[0]))*mipWHD[0])];
								if (!expectColorMinMax.containsULP(texColor, pEquivTexFormatInfo, ulpConstraint, ulpDiffs))
								{
									tRes = RESULT_FAIL;
									bool bLogAllPixels = g_App.LogAllPixelFailures();
									if (bLogAllPixels || numFailed == 0)
									{
										PrintBadMipGenResult(this, expectColorMinMax, texColor, x, y, 0, faceIdx, mipNum);
									}
									else if (numFailed == 1 && !bLogAllPixels)
									{
										WriteToLog( _T("More pixels failed. Use -AllPixels to log them\n") );
									}
									numFailed++;
								}
							}
						}
						GetImmediateContext()->Unmap(pTexCube, mipNum + faceIdx*descCube.MipLevels);
					}
					else
					{
						tRes = RESULT_FAIL;
						if (!mapFailed)
							WriteToLog( _T("%s Map() failed."), LOG_HEADER, D3DHResultToString(hr).c_str() );
						mapFailed = true;
					}
				}
			}
			SAFE_RELEASE(pTexCube);
		}
		break;
	}

	if (numFailed > 0)
		WriteToLog( _T("%s %d pixels failed in total."), LOG_HEADER, numFailed );

testMips_done:
	for (int mipNum = 0; mipNum < m_nNumMips; ++mipNum)
	{
		delete[] mipColorRange[mipNum];
	}
	delete[] mipColorRange;

	return tRes;
}
