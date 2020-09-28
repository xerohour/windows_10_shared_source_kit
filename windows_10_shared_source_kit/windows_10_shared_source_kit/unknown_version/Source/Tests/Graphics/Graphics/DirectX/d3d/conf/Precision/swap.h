#pragma once

#include "Arithmetic.h"


class CSwapPrecisionTest : public CArithmeticPrecisionTest
{
public:
	virtual TEST_RESULT Setup();
	virtual testfactor::RFactor InitializeShaderStageModifierFactor();
};

class CSwapFloatTest : public CSwapPrecisionTest
{
public:
	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor();
};

class CSwapUintTest : public CSwapPrecisionTest
{
public:
	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor();
};

class CSwapIntTest : public CSwapPrecisionTest
{
public:
	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor();
};

