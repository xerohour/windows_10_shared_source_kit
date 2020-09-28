#include "MinMaxRange.h"
#include <d3dcommon.h>

namespace MinMaxRange
{

int MinMaxColorClass::precisionBits = 23;
bool MinMaxColorClass::unormPrecision = false;

D3D_FEATURE_LEVEL s_D3D_Version = D3D_FEATURE_LEVEL_10_0;

void SetD3DFeatureLevel(UINT featureLevel)
{
	s_D3D_Version = (D3D_FEATURE_LEVEL)featureLevel;
}

float GetFixedFloatULPVariance()
{
	float ulpVariance = 0.5;
	switch (s_D3D_Version)
	{
	case D3D_FEATURE_LEVEL_9_1:	// 9.1
	case D3D_FEATURE_LEVEL_9_2:	// 9.2
	case D3D_FEATURE_LEVEL_9_3:	// 9.3
		ulpVariance = 1.0f;
		break;
	case D3D_FEATURE_LEVEL_10_0:	// 10.0
	case D3D_FEATURE_LEVEL_10_1:	// 10.1
		ulpVariance = 0.6f;
		break;
	case D3D_FEATURE_LEVEL_11_0:	// 11.0
	case D3D_FEATURE_LEVEL_11_1:
		ulpVariance = 0.5f;
		break;
	default:
		assert(!"Unknown D3D version");
		break;
	}

	return ulpVariance;
}

float GetFloatULPVariance(int precisionBits)
{
	float ulpVariance = 0.5;
	switch (s_D3D_Version)
	{
	case D3D_FEATURE_LEVEL_9_1:	// 9.1
	case D3D_FEATURE_LEVEL_9_2:	// 9.2
	case D3D_FEATURE_LEVEL_9_3:	// 9.3
		ulpVariance = 1.0f;
		break;
	case D3D_FEATURE_LEVEL_10_0:	// 10.0
		if (precisionBits > 10)
			ulpVariance = 1.0f;
		else
			ulpVariance = 0.5f;
		break;
	case D3D_FEATURE_LEVEL_11_1:
	case D3D_FEATURE_LEVEL_11_0:
	case D3D_FEATURE_LEVEL_10_1:	// 10.1
		ulpVariance = 0.5f;
		break;
	default:
		assert(!"Unknown D3D version");
		break;
	}

	return ulpVariance;
}

void TEST_COLOR::operator*=(TEST_COLOR::realType rhs)
{
	TEST_COLOR& lhs = *this;
	lhs.r = (TEST_COLOR::realType)(lhs.r * rhs);
	lhs.g = (TEST_COLOR::realType)(lhs.g * rhs);
	lhs.b = (TEST_COLOR::realType)(lhs.b * rhs);
	lhs.a = (TEST_COLOR::realType)(lhs.a * rhs);
}
TEST_COLOR TEST_COLOR::operator*(TEST_COLOR::realType rhs) const
{
	TEST_COLOR retCol = *this;
	retCol *= rhs;
	return retCol;
}
TEST_COLOR TEST_COLOR::operator*(const TEST_COLOR& rhs) const
{
	TEST_COLOR retCol = *this;
	retCol.r *= rhs.r;
	retCol.g *= rhs.g;
	retCol.b *= rhs.b;
	retCol.a *= rhs.a;
	return retCol;
}
TEST_COLOR TEST_COLOR::operator+(const TEST_COLOR& rhs) const
{
	TEST_COLOR retCol = *this;
	retCol.r += rhs.r;
	retCol.g += rhs.g;
	retCol.b += rhs.b;
	retCol.a += rhs.a;
	return retCol;
}
TEST_COLOR TEST_COLOR::operator-(const TEST_COLOR& rhs) const
{
	TEST_COLOR retCol = *this;
	retCol.r -= rhs.r;
	retCol.g -= rhs.g;
	retCol.b -= rhs.b;
	retCol.a -= rhs.a;
	return retCol;
}
bool TEST_COLOR::operator==(const TEST_COLOR& rhs) const
{
	const TEST_COLOR& lhs = *this;
	if (lhs.r != rhs.r)
		return false;
	if (lhs.g != rhs.g)
		return false;
	if (lhs.b != rhs.b)
		return false;
	if (lhs.a != rhs.a)
		return false;
	return true;
}

} // namespace MinMaxRange
