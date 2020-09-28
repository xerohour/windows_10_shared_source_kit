#include "ViewPorts.h"

#pragma once

/** Creates viewports in same-sized pairs.  One will have a scissor that covers
the whole thing, and the other will have a small scissor.
Draws a triangle that passes through the small scissor and makes sure that the edge
of the triangle is clipped correctly. */

enum ScissorOffset
{
	MIDDLE = 0,
	FIVE = 5,
	SEVENTEEN = 17,
	THIRTEEN = 13
};

enum TestColorOrOverdraw
{
	TEST_COLOR = 0,
	TEST_OVERDRAW = 1
};

DECLARE_NAMED_VALUES( ScissorOffset )

class SubScissor : public ViewportsTest
{
protected:
	ScissorOffset m_scissorPosition;
	TestColorOrOverdraw m_ColorOrOverDraw;

public:
	void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
};
