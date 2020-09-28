#pragma once

// Includes
#include "SystemCheck.h"

////////////////////////

class CMinimumFeatureLevelTest : public CD3D11Test
{
public:
	CMinimumFeatureLevelTest(void);
	virtual ~CMinimumFeatureLevelTest(void);

	void InitTestParameters();
	TEST_RESULT ExecuteTestCase();
};

