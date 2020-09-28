//////////////////////////////////////////////////////////////////////////
//  MinMaxRange.h
//  created:	2008/03/10
//
//  purpose: Provides classes and functions useful for comparing values of
//			 various floating-point precision within known acceptable
//           tolerances.
//////////////////////////////////////////////////////////////////////////
#ifndef MIN_MAX_RANGE_H
#define MIN_MAX_RANGE_H

#include <cmath>
#include "dxgivaluesets.h"

namespace MinMaxRange
{

void SetD3DFeatureLevel(UINT featureLevel);
float GetFixedFloatULPVariance();
float GetFloatULPVariance(int precisionBits);

union TEST_COLOR
{
	typedef double realType;
	struct
	{
		realType r;
		realType g;
		realType b;
		realType a;
	};
	realType f[4];

	bool operator==(const TEST_COLOR& rhs) const;
	void operator*=(TEST_COLOR::realType rhs);
	TEST_COLOR operator*(TEST_COLOR::realType rhs) const;
	TEST_COLOR operator*(const TEST_COLOR& rhs) const;
	TEST_COLOR operator+(const TEST_COLOR& rhs) const;
	TEST_COLOR operator-(const TEST_COLOR& rhs) const;
};
static const TEST_COLOR s_defaultColor = {0,0,0,0};

static DXGI_FORMAT GetFormatEquivalentBits(DXGI_FORMAT format)
{
	switch (format)
	{
	default:
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
		break;
	case DXGI_FORMAT_R1_UNORM:
		break;
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
		format = DXGI_FORMAT_R8G8B8A8_UNORM; // doesn't really have alpha channel
		break;
	case( DXGI_FORMAT_BC1_UNORM ):
	case( DXGI_FORMAT_BC2_UNORM ):
	case( DXGI_FORMAT_BC3_UNORM ):
	case( DXGI_FORMAT_BC7_UNORM ):
		format = DXGI_FORMAT_R8G8B8A8_UNORM; // really more like 5-6-5
		break;
	case( DXGI_FORMAT_BC1_UNORM_SRGB ):
	case( DXGI_FORMAT_BC2_UNORM_SRGB ):
	case( DXGI_FORMAT_BC3_UNORM_SRGB ):
	case( DXGI_FORMAT_BC7_UNORM_SRGB ):
		format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
		break;
	case( DXGI_FORMAT_BC4_UNORM ):
		format = DXGI_FORMAT_R16_UNORM;
		break;
	case( DXGI_FORMAT_BC4_SNORM ):
		format = DXGI_FORMAT_R16_SNORM;
		break;
	case( DXGI_FORMAT_BC5_UNORM ):
		format = DXGI_FORMAT_R16G16_UNORM;
		break;
	case( DXGI_FORMAT_BC5_SNORM ):
		format = DXGI_FORMAT_R16G16_SNORM;
		break;
	case( DXGI_FORMAT_BC6H_UF16 ):
	case( DXGI_FORMAT_BC6H_SF16 ):
		format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		break;
	}
	return format;
};

static bool IsSpecialFormat(DXGI_FORMAT format)
{
	switch(format)
	{
	default:
		break;
	case DXGI_FORMAT_R9G9B9E5_SHAREDEXP:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_BC1_TYPELESS:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC2_TYPELESS:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_TYPELESS:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC4_TYPELESS:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC4_SNORM:
	case DXGI_FORMAT_BC5_TYPELESS:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_BC5_SNORM:
	case DXGI_FORMAT_BC6H_UF16:
	case DXGI_FORMAT_BC6H_SF16:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
		return true;
	}
	return false;
}

// realType should be either float or double
template <typename realType>
static bool
TestFloatsSame(realType c1,
			   realType c2,
			   const DXGIFormatInfo* pTexFormatInfo,
			   int nComponent,
			   float maxULP,
			   float* pUlpDiff)
{
	bool retVal = true;
	if (pUlpDiff)
		*pUlpDiff = 0;
	int realbits = pTexFormatInfo->pBitsPerComponent[nComponent];
	int bitsSum = 0;
	for (int n = 0; n < 4; ++n)
		bitsSum += pTexFormatInfo->pBitsPerComponent[n];
	if (IsSpecialFormat(pTexFormatInfo->Format)
		|| bitsSum == 0)
	{
		if (pTexFormatInfo->Format != DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
		{
			assert(!"Unsupported format, use GetFormatEquivalentBits()");
		}
	}
	if (pTexFormatInfo->dwFlags&(FF_FLOAT|FF_SHAREDEXP))
	{
		switch (pTexFormatInfo->pBitsPerComponent[nComponent])
		{
		default:
			if (!((   pTexFormatInfo->Format == DXGI_FORMAT_R24_UNORM_X8_TYPELESS
				|| pTexFormatInfo->Format == DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS)
				&& nComponent > 0))
			{
				assert(!"Unrecognized float format");
			}
			// fall-through to 0 bits
		case 0: realbits = 0; break;
		case 32: realbits = 23; break;
		case 16: realbits = 10; break;
		case 11: realbits = 6; break;
		case 10: realbits = 5; break;
		}
		if (pTexFormatInfo->Format == DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
			realbits = (nComponent < 3) ? 9 : 0;
		if (realbits == 0)
			goto colors_same_end;
		float f1 = (float)c1;
		float f2 = (float)c2;
		if (f1 == f2)
			goto colors_same_end;
		const DWORD dwVal1 = *((DWORD*)&f1);
		const DWORD dwVal2 = *((DWORD*)&f2);
		const int nDiff = dwVal1 - dwVal2;
		const DWORD uDiff = (nDiff < 0) ? -nDiff : nDiff;
		const int ulpBit = (23 - realbits);
		const float ulpDiff = uDiff / (float)(1 << ulpBit);
		if (pUlpDiff)
			*pUlpDiff = ulpDiff;
		if (maxULP < ulpDiff)
			retVal = false;
	}
	else
	{
		if (pTexFormatInfo->dwFlags&FF_SNORM)
			realbits -= 1;
		else if (pTexFormatInfo->dwFlags&FF_UNORM)
			realbits -= 0;
		else
		{
			assert(!"Unexpected format");
		}
		if ((   pTexFormatInfo->Format == DXGI_FORMAT_R24_UNORM_X8_TYPELESS 
			|| pTexFormatInfo->Format == DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS)
			&& nComponent > 0)
			realbits = 0;
		realbits = max(0,realbits);
		if (realbits == 0)
			goto colors_same_end;
		float f1 = (float)c1;
		float f2 = (float)c2;
		if (f1 == f2)
			goto colors_same_end;
		if (pTexFormatInfo->dwFlags & (FF_SNORM|FF_UNORM))
		{
			const float ulpVal = maxULP;
			const float fDiff = (1<<realbits) * (float)fabs(f1 - f2);
			if (pUlpDiff)
				*pUlpDiff = fDiff;
			if (ulpVal < fDiff)
				retVal = false;
		}
		else
		{
			assert(!"Unexpected format");
		}
	}
colors_same_end:
	return retVal;
}

static bool
TestColorsSame(const TEST_COLOR& c1, 
			   const TEST_COLOR& c2, 
			   DXGI_FORMAT formatConstraint, 
			   float maxULP,
			   TEST_COLOR& ulpDiffs)
{
	bool retVal = true;
	DXGI_FORMAT equivFormat = GetFormatEquivalentBits(formatConstraint);
	const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(equivFormat);

	for (int n = 0; n < 4; ++n)
	{
		ulpDiffs.f[n] = 0;
		switch(formatConstraint)
		{
		default:
			break;
		case DXGI_FORMAT_BC4_TYPELESS:
		case DXGI_FORMAT_BC4_UNORM:
		case DXGI_FORMAT_BC4_SNORM:
			if (n == 1)
				continue; // no green channel
			// intentional fall-through
		case DXGI_FORMAT_BC5_TYPELESS:
		case DXGI_FORMAT_BC5_UNORM:
		case DXGI_FORMAT_BC5_SNORM:
			if (n == 2)
				continue; // no blue channel
			// intentional fall-through
		case DXGI_FORMAT_BC1_TYPELESS:
		case DXGI_FORMAT_BC1_UNORM:
		case DXGI_FORMAT_BC1_UNORM_SRGB:
		case DXGI_FORMAT_R8G8_B8G8_UNORM:
		case DXGI_FORMAT_G8R8_G8B8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_TYPELESS:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
			if (n == 3)
				continue; // no alpha channel
			break;
		}
		float ulpDiff = 0;
		if (!TestFloatsSame(c1.f[n], c2.f[n], pTexFormatInfo, n, maxULP, &ulpDiff))
			retVal = false;
		ulpDiffs.f[n] = ulpDiff;
	}
	return retVal;
}

template <typename T>
struct MinMaxRangeTemplate
{
private:
	bool operator!=(const T & rhs) { return false};
public:
	T lo, hi;
	explicit MinMaxRangeTemplate(const T& ref)
		: lo(ref)
		, hi(ref)
	{
	}
	MinMaxRangeTemplate(const T& inLo, const T& inHi)
		: lo (inLo)
		, hi (inLo)
	{
		expand(inHi);
	}
	void operator=(const T & val)
	{
		lo = val;
		hi = val;
	}
	bool contains(const T & rhs) const
	{
		if (rhs < lo || rhs > hi)
			return false;
		return true;
	}
	void expand(const T & val)
	{
		if (val < lo)
			lo = val;
		if (val > hi)
			hi = val;
	}
	void expand(const MinMaxRangeTemplate<T> & val)
	{
		if (val.lo < lo)
			lo = val.lo;
		if (val.hi > hi)
			hi = val.hi;
	}
	bool isSingular() const
	{
		return (lo == hi);
	}
	bool operator==(const T& rhs)
	{
		return (isSingular() && rhs == lo);
	}
	void operator+=(const T & rhs)
	{
		lo += rhs;
		hi += rhs;
	}
	void operator+=(const MinMaxRangeTemplate<T> & rhs)
	{
		lo += rhs.lo;
		hi += rhs.hi;
	}
	MinMaxRangeTemplate<T> operator+(const T & rhs)
	{
		MinMaxRangeTemplate<T> newVal(lo, hi);
		newVal += rhs;
		return newVal;
	}
	MinMaxRangeTemplate<T> operator+(const MinMaxRangeTemplate<T> & rhs)
	{
		MinMaxRangeTemplate<T> newVal;
		newVal.lo = lo + rhs.lo;
		newVal.hi = hi + rhs.hi;
		return newVal;
	}
	void operator*=(const T & rhs)
	{
		lo *= rhs;
		hi *= rhs;
	}
	MinMaxRangeTemplate<T> operator*(const T & rhs)
	{
		MinMaxRangeTemplate<T> newVal(lo, hi);
		newVal *= rhs;
		return newVal;
	}
	void operator-=(const T & rhs)
	{
		lo -= rhs;
		hi -= rhs;
	}
	MinMaxRangeTemplate<T> operator-(const T & rhs)
	{
		MinMaxRangeTemplate<T> newVal(lo,hi);
		newVal -= rhs;
		return newVal;
	}
	MinMaxRangeTemplate<T> operator/(const T & rhs)
	{
		MinMaxRangeTemplate<T> newVal;
		newVal.lo = lo / rhs;
		newVal.hi = hi / rhs;
		return newVal;
	}
};
typedef MinMaxRangeTemplate<float> MinMaxFloat;
typedef MinMaxRangeTemplate<int> MinMaxInt;

struct MinMaxBool : MinMaxRangeTemplate<bool>
{
public:
	MinMaxBool()
		: MinMaxRangeTemplate(false)
	{
	}
	explicit MinMaxBool(const bool& ref)
		: MinMaxRangeTemplate(ref)
	{
	}
	MinMaxBool(const bool& inLo, const bool& inHi)
		: MinMaxRangeTemplate(inLo, inHi)
	{
	}
	bool contains(const bool & rhs) const
	{
		if ((int)rhs < (int)lo || (int)rhs > (int)hi)
			return false;
		return true;
	}
	void expand(const bool & val)
	{
		if ((int)val < (int)lo)
			lo = val;
		if ((int)val > (int)hi)
			hi = val;
	}
};

struct MinMaxDbl : MinMaxRangeTemplate<double>
{
public:
	static MinMaxDbl floatULPDrift(double in, float ulpVariance, int precisionBits = 23, bool bUnormPrecision = false)
	{
		assert(precisionBits < 52);
		assert(ulpVariance >= 0);
		if (bUnormPrecision)
		{
			//assert(0 <= in && in <= 1);
			UINT64 precisionMask = (1i64<<precisionBits);
			double bigIn = in * precisionMask;
			double outLo = (bigIn - ulpVariance)/precisionMask;
			double outHi = (bigIn + ulpVariance)/precisionMask;
			return MinMaxDbl(outLo, outHi);
		}
		else
		{
			// create a mask of bits that are extra in a double 
			UINT64 ulpDrift = (UINT64)((1i64<<(52-precisionBits))*ulpVariance);
			// if denorm, return initial value
			if (fabs(in) < *(double*)(&ulpDrift))
				return MinMaxDbl(in);
			// +/- potential ULP deviation
			UINT64 loVal = *(UINT64*)(&in) - ulpDrift;
			UINT64 hiVal = *(UINT64*)(&in) + ulpDrift;
			return MinMaxDbl(*(double*)(&loVal), *(double*)(&hiVal));
		}
	}
	static MinMaxDbl floatULPDrift(double in, int precisionBits = 23, bool bUnormPrecision = false)
	{
		float ulpVariance;
		if (bUnormPrecision)
			ulpVariance = 0.5;
		else
			ulpVariance = GetFloatULPVariance(precisionBits);
		return floatULPDrift(in, ulpVariance, precisionBits, bUnormPrecision);
	}


	MinMaxDbl()
		: MinMaxRangeTemplate(false)
	{
	}
	explicit MinMaxDbl(const double& ref)
		: MinMaxRangeTemplate(ref)
	{
	}
	MinMaxDbl(const MinMaxRangeTemplate<double>& ref)
		: MinMaxRangeTemplate(ref)
	{
	}
	MinMaxDbl(const double& inLo, const double& inHi)
		: MinMaxRangeTemplate(inLo, inHi)
	{
	}
	void operator*=(const double & rhs)
	{
		lo = MinMaxDbl::floatULPDrift(lo * rhs).lo;
		hi = MinMaxDbl::floatULPDrift(hi * rhs).hi;
	}
	MinMaxDbl operator*(const double & rhs)
	{
		MinMaxDbl newVal(lo, hi);
		newVal *= rhs;
		return newVal;
	}
};

struct MinMaxColorClass : MinMaxRangeTemplate<TEST_COLOR>
{
	static int precisionBits;
	static bool unormPrecision;
public:
	static void GetPrecision(int * bits, bool * unorm)
	{
		*bits = precisionBits;
		*unorm = unormPrecision;
	}
	static void SetUNormPrecision(int bits = 32)
	{
		assert(bits > 0 && bits < 100);
		precisionBits = bits;
		unormPrecision = true;
	}
	static void SetSNormPrecision(int bits = 32)
	{
		assert(bits > 1 && bits < 100);
		precisionBits = bits - 1;
		unormPrecision = true;
	}
	static void SetFloatPrecision(int bits = 32)
	{
		switch (bits)
		{
		default:
			assert(!"Unrecognized float format");
			// fall-through to 0 bits
		case 0: precisionBits = 0; break;
		case 32: precisionBits = 23; break;
		case 16: precisionBits = 10; break;
		case 11: precisionBits = 6; break;
		case 10: precisionBits = 5; break;
		}
		unormPrecision = false;
	}
	static void SetPrecisionExplicit(int bits, bool unorm)
	{
		precisionBits = bits;
		unormPrecision = unorm;
	}
	static void SetPrecisionFormat(DXGI_FORMAT format)
	{
		format = GetFormatEquivalentBits(format);
		const DXGIFormatInfo* pTexFormatInfo = CFormatInfoMap::Find(format);
		if (pTexFormatInfo->dwFlags&FF_FLOAT)
			SetFloatPrecision(pTexFormatInfo->pBitsPerComponent[0]);
		else if (pTexFormatInfo->dwFlags&FF_SNORM)
			SetSNormPrecision(pTexFormatInfo->pBitsPerComponent[0]);
		else if (pTexFormatInfo->dwFlags&FF_UNORM)
		{
			if (format == DXGI_FORMAT_A8_UNORM)
				SetUNormPrecision(8);
			else
				SetUNormPrecision(pTexFormatInfo->pBitsPerComponent[0]);
		}
		else if (format == DXGI_FORMAT_R9G9B9E5_SHAREDEXP)
		{
			SetUNormPrecision(9);
		}
		else
		{
			assert(!"Invalid MinMaxColor format");
		}
	}
	MinMaxColorClass()
		: MinMaxRangeTemplate(s_defaultColor)
	{
	}
	explicit MinMaxColorClass(const TEST_COLOR& ref)
		: MinMaxRangeTemplate(ref)
	{
	}
	explicit MinMaxColorClass(const MinMaxRangeTemplate<TEST_COLOR>& ref)
		: MinMaxRangeTemplate(ref.lo)
	{
		expand(ref.hi);
	}
	MinMaxColorClass(const TEST_COLOR& inLo, const TEST_COLOR& inHi)
		: MinMaxRangeTemplate(inLo)
	{
		expand(inHi);
	}
	bool contains(const TEST_COLOR & rhs) const
	{
		for (int n = 0; n < 4; ++n)
		{
			assert(lo.f[n] <= hi.f[n]);
			if (rhs.f[n] < lo.f[n] || rhs.f[n] > hi.f[n])
				return false;
		}
		return true;
	}
	bool containsULP(const TEST_COLOR & rhs, const DXGIFormatInfo* pTexFormatInfo, float maxULP, TEST_COLOR & ulpDiffs) const
	{
		if (isSingular())
			return TestColorsSame(lo, rhs, pTexFormatInfo->Format, maxULP, ulpDiffs);
		bool retVal = true;

		int numComponents = 4;
		if (pTexFormatInfo->Format == DXGI_FORMAT_B8G8R8X8_UNORM || 
			pTexFormatInfo->Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB ||
			pTexFormatInfo->Format == DXGI_FORMAT_B8G8R8X8_TYPELESS)
			numComponents = 3;

		for (int n = 0; n < numComponents; ++n)
		{
			float ulpDiff = 0;
			assert(lo.f[n] <= hi.f[n]);
			if (rhs.f[n] < lo.f[n] || rhs.f[n] > hi.f[n])
			{
				float ulpDiffLo = 0;
				float ulpDiffHi = 0;
				if (   !TestFloatsSame(rhs.f[n], lo.f[n], pTexFormatInfo, n, maxULP, &ulpDiffLo)
					&& !TestFloatsSame(rhs.f[n], hi.f[n], pTexFormatInfo, n, maxULP, &ulpDiffHi))
				{
					retVal = false;
					ulpDiff = min(ulpDiffLo, ulpDiffHi);
				}
			}
			ulpDiffs.f[n] = ulpDiff;
		}
		return retVal;
	}
	void expand(const TEST_COLOR & val)
	{
		for (int n = 0; n < 4; ++n)
		{
			if (val.f[n] < lo.f[n])
				lo.f[n] = val.f[n];
			if (val.f[n] > hi.f[n])
				hi.f[n] = val.f[n];
			assert(lo.f[n] <= hi.f[n]);
		}
	}
	void expand(const MinMaxColorClass & val)
	{
		for (int n = 0; n < 4; ++n)
		{
			if (val.lo.f[n] < lo.f[n])
				lo.f[n] = val.lo.f[n];
			if (val.hi.f[n] > hi.f[n])
				hi.f[n] = val.hi.f[n];
			assert(lo.f[n] <= hi.f[n]);
		}
	}
    void minimum( const MinMaxColorClass & val )
    {
		for (int n = 0; n < 4; ++n)
		{
			if (val.lo.f[n] < lo.f[n])
				lo.f[n] = val.lo.f[n];
			if (val.hi.f[n] < hi.f[n])
				hi.f[n] = val.hi.f[n];
			assert(lo.f[n] <= hi.f[n]);
		}
    }
    void maximum( const MinMaxColorClass & val )
    {
		for (int n = 0; n < 4; ++n)
		{
			if (val.lo.f[n] > lo.f[n])
				lo.f[n] = val.lo.f[n];
			if (val.hi.f[n] > hi.f[n])
				hi.f[n] = val.hi.f[n];
			assert(lo.f[n] <= hi.f[n]);
		}
    }
	void operator+=(const MinMaxColorClass & rhs)
	{
		for (int n = 0; n < 4; ++n)
		{
			lo.f[n] = MinMaxDbl::floatULPDrift(lo.f[n] + rhs.lo.f[n], precisionBits, unormPrecision).lo;
			hi.f[n] = MinMaxDbl::floatULPDrift(hi.f[n] + rhs.hi.f[n], precisionBits, unormPrecision).hi;
			assert(lo.f[n] <= hi.f[n]);
		}
	}
	template <typename T>
	MinMaxColorClass operator+(const T & rhs) const
	{
		MinMaxColorClass newVal(*this);
		newVal += rhs;
		return newVal;
	}
	void operator-=(const MinMaxColorClass & rhs)
	{
		for (int n = 0; n < 4; ++n)
		{
			lo.f[n] = MinMaxDbl::floatULPDrift(lo.f[n] - rhs.hi.f[n], precisionBits, unormPrecision).lo;
			hi.f[n] = MinMaxDbl::floatULPDrift(hi.f[n] - rhs.lo.f[n], precisionBits, unormPrecision).hi;
			assert(lo.f[n] <= hi.f[n]);
		}
	}
	template <typename T>
	MinMaxColorClass operator-(const T & rhs) const
	{
		MinMaxColorClass newVal(*this);
		newVal -= rhs;
		return newVal;
	}
	void operator*=(const MinMaxFloat & rhs)
	{
		if (rhs.isSingular())
		{
			*this = (*this) * rhs.lo;
			return;
		}
		for (int n = 0; n < 4; ++n)
		{
			MinMaxDbl lolo = MinMaxDbl::floatULPDrift(lo.f[n] * rhs.lo, precisionBits, unormPrecision);
			MinMaxDbl lohi = MinMaxDbl::floatULPDrift(lo.f[n] * rhs.hi, precisionBits, unormPrecision);
			MinMaxDbl hilo = MinMaxDbl::floatULPDrift(hi.f[n] * rhs.lo, precisionBits, unormPrecision);
			MinMaxDbl hihi = MinMaxDbl::floatULPDrift(hi.f[n] * rhs.hi, precisionBits, unormPrecision);
			lo.f[n] = min(min(lolo.lo, lohi.lo), min(hilo.lo, hihi.lo));
			hi.f[n] = max(max(lolo.hi, lohi.hi), max(hilo.hi, hihi.hi));
			assert(lo.f[n] <= hi.f[n]);
		}
	}
	void operator*=(const TEST_COLOR & rhs)
	{
		for (int n = 0; n < 4; ++n)
		{
			MinMaxDbl loVal = MinMaxDbl::floatULPDrift(lo.f[n] * rhs.f[n], precisionBits, unormPrecision);
			MinMaxDbl hiVal = MinMaxDbl::floatULPDrift(hi.f[n] * rhs.f[n], precisionBits, unormPrecision);
			lo.f[n] = min(loVal.lo, hiVal.lo);
			hi.f[n] = max(loVal.hi, hiVal.hi);
			assert(lo.f[n] <= hi.f[n]);
		}
	}
	template <typename T>
	void operator*=(const T & rhs)
	{
		for (int n = 0; n < 4; ++n)
		{
			MinMaxDbl loVal = MinMaxDbl::floatULPDrift(lo.f[n] * rhs, precisionBits, unormPrecision);
			MinMaxDbl hiVal = MinMaxDbl::floatULPDrift(hi.f[n] * rhs, precisionBits, unormPrecision);
			lo.f[n] = min(loVal.lo, hiVal.lo);
			hi.f[n] = max(loVal.hi, hiVal.hi);
			assert(lo.f[n] <= hi.f[n]);
		}
	}
	template <typename T>
	MinMaxColorClass operator*(const T & rhs) const
	{
		MinMaxColorClass newVal(*this);
		newVal *= rhs;
		return newVal;
	}
};

} // namespace MinMaxRange

#endif // MIN_MAX_RANGE_H
