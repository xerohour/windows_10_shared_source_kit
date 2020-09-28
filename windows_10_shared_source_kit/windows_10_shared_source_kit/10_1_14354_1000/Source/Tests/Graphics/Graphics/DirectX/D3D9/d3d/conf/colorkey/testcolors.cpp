#define D3D_OVERLOADS
#include <d3d.h>
#include "ColorKey.h"

//************************************************************************
// Default Test functions

CDefaultTest::CDefaultTest()
{
	m_szTestName = "Texture ColorKey Default";
	m_szCommandKey = "Default";

	// Set the colorkey value to Red
	dwColorKey = RGBA_MAKE(255,0,0,255);

	// Tell the parent that we are the default case
	bDefault = true;
}

CDefaultTest::~CDefaultTest()
{
}

//************************************************************************
// White Test functions

CWhiteTest::CWhiteTest()
{
	m_szTestName = "Texture ColorKey White";
	m_szCommandKey = "White";

	// Set the colorkey value to White
	dwColorKey = RGBA_MAKE(255,255,255,255);
}

CWhiteTest::~CWhiteTest()
{
}


//************************************************************************
// Red Test functions

CRedTest::CRedTest()
{
	m_szTestName = "Texture ColorKey Red";
	m_szCommandKey = "Red";

	// Set the colorkey value to Red
	dwColorKey = RGBA_MAKE(255,0,0,255);
}

CRedTest::~CRedTest()
{
}

//************************************************************************
// Green Test functions

CGreenTest::CGreenTest()
{
	m_szTestName = "Texture ColorKey Green";
	m_szCommandKey = "Green";

	// Set the colorkey value to Green
	dwColorKey = RGBA_MAKE(0,255,0,255);
}

CGreenTest::~CGreenTest()
{
}

//************************************************************************
// Blue Test functions

CBlueTest::CBlueTest()
{
	m_szTestName = "Texture ColorKey Blue";
	m_szCommandKey = "Blue";

	// Set the colorkey value to Blue
	dwColorKey = RGBA_MAKE(0,0,255,255);
}

CBlueTest::~CBlueTest()
{
}

//************************************************************************
// Yellow Test functions

CYellowTest::CYellowTest()
{
	m_szTestName = "Texture ColorKey Yellow";
	m_szCommandKey = "Yellow";

	// Set the colorkey value to Yellow
	dwColorKey = RGBA_MAKE(255,255,0,255);
}

CYellowTest::~CYellowTest()
{
}

//************************************************************************
// Magenta Test functions

CMagentaTest::CMagentaTest()
{
	m_szTestName = "Texture ColorKey Magenta";
	m_szCommandKey = "Magenta";

	// Set the colorkey value to Magenta
	dwColorKey = RGBA_MAKE(255,0,255,255);
}

CMagentaTest::~CMagentaTest()
{
}

//************************************************************************
// Cyan Test functions

CCyanTest::CCyanTest()
{
	m_szTestName = "Texture ColorKey Cyan";
	m_szCommandKey = "Cyan";

	// Set the colorkey value to Cyan
	dwColorKey = RGBA_MAKE(0,255,255,255);
}

CCyanTest::~CCyanTest()
{
}

//************************************************************************
// Grey Test functions

CGreyTest::CGreyTest()
{
	m_szTestName = "Texture ColorKey Grey";
	m_szCommandKey = "Grey";

	// Set the colorkey value to Grey
	dwColorKey = RGBA_MAKE(128,128,128,255);
}

CGreyTest::~CGreyTest()
{
}

//************************************************************************
// Black Test functions

CBlackTest::CBlackTest()
{
	m_szTestName = "Texture ColorKey Black";
	m_szCommandKey = "Black";

	// Set the colorkey value to Black
	dwColorKey = RGBA_MAKE(0,0,0,255);
}

CBlackTest::~CBlackTest()
{
}

UINT CBlackTest::TestInitialize(void)
{
	UINT Result;

	// Call the parent TestInitialize
	Result = CColorKeyTest::TestInitialize();

	if (Result != D3DTESTINIT_RUN)
		return Result;

    // Setup the colors for the Stripes array
	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,0,255);		// Black
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

	return Result;
}

bool CBlackTest::ClearFrame(void)
{
	return Clear(0, NULL, FMWK_DEFAULT, RGBA_MAKE(0,0,255,255), 1.0f, 0);
}
