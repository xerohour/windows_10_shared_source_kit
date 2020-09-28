#pragma once

#include "fill.h"
#include "PixelResult.h"
#include <vector>
#include <d3dmshelper.h>

struct LineSegment {
	// starting and ending point of the line segment in image space
	FLOAT startX;
	FLOAT startY;
	FLOAT intX;  // interior point
	FLOAT intY;  // interior point for line strip
	FLOAT endX;
	FLOAT endY;
};


class FillLine : public CMultisampleTest_Fill
{
public:
	FillLine();
	void InitTestParameters();
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();

private:
	// renders the line segment, called by ExecuteTestCase()
	void Draw();
	// validates the resulting pixels with the expected value, 
	// and returns true if they match, false otherwise.
	// called by ExecuteTestCase()
	TEST_RESULT Validate();

	// fill the member vector "pr" with actual expected results
	void CreatePixelResults();

	// the blend state
	ID3D11BlendState* blendState;

	LineSegment line;  // the line which stores the current parameter value
	vector<PixelResult> pr; // a vector of expected result for lines

	testfactor::RFactor rsx;
	testfactor::RFactor rsxLS;

	// determine whether a line segment is a line strip
	bool isLineStrip(const LineSegment &ls);


	// The following are not used in this test! 
	void Draw( float x, float y ) {}
	TEST_RESULT Validate(BOOL result) {return RESULT_FAIL;}
	void LogInfo() {}
};
