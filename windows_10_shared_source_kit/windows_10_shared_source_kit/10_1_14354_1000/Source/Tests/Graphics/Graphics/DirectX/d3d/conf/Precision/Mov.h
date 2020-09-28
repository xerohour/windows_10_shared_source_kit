#pragma once

#include "Arithmetic.h"

class CMovFloatTest : public CArithmeticFloatTest
{
public:
	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor();
};

class CMovUintTest : public CArithmeticUintTest
{
public:
	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor();
};

class CMovIntTest : public CArithmeticIntTest
{
public:
	virtual testfactor::RFactor InitializeOpAndValueAndMinPrecisionAndModFactor();
};

