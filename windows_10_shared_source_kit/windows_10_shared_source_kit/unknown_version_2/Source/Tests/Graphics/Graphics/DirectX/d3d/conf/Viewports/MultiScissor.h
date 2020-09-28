#include "ViewPorts.h"

#pragma once

class MultiScissor : public ViewportsTest
{
protected:
	UINT m_scissorSize;
	UINT m_scissorEnabled;
public:
	void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
};
