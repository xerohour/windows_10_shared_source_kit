#define D3D_OVERLOADS
#include <d3d.h>
#include "MipFilter.h"

//************************************************************************
// Point_FullSquareUT Test functions

CPoint_FullSquareUTTest::CPoint_FullSquareUTTest()
{
	m_szTestName = "MipFilter Point_FullSquare (UpdateTex)";
	m_szCommandKey = "Point_FullSquareUT";

    dwLoadOption = TEXLOAD_UPDATETEXTURE;

    // Inidicate that we are a Full test
	bFull = true;
}

//************************************************************************
// Point_FullSquareCR Test functions

CPoint_FullSquareCRTest::CPoint_FullSquareCRTest()
{
	m_szTestName = "MipFilter Point_FullSquare (CopyRects)";
	m_szCommandKey = "Point_FullSquareCR";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

    dwLoadOption = TEXLOAD_COPYRECTS;

    // Inidicate that we are a Full test
	bFull = true;
}

//************************************************************************
// Point_FullSquareLC Test functions

CPoint_FullSquareLCTest::CPoint_FullSquareLCTest()
{
	m_szTestName = "MipFilter Point_FullSquare (LockCopy)";
	m_szCommandKey = "Point_FullSquareLC";

    dwLoadOption = TEXLOAD_LOCKCOPY;

    // Inidicate that we are a Full test
	bFull = true;
}

//************************************************************************
// Point_FullSquareUS Test functions

CPoint_FullSquareUSTest::CPoint_FullSquareUSTest()
{
	m_szTestName = "MipFilter Point_FullSquare (UpdateSurf)";
	m_szCommandKey = "Point_FullSquareUS";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

    dwLoadOption = TEXLOAD_UPDATESURFACE;

    // Inidicate that we are a Full test
	bFull = true;
}

//************************************************************************
// Point_FullNonSquareWUT Test functions

CPoint_FullNonSquareWUTTest::CPoint_FullNonSquareWUTTest()
{
	m_szTestName = "MipFilter Point_FullNonSquareWidth (UpdateTex)";
	m_szCommandKey = "Point_FullNonSquareWidthUT";

    dwLoadOption = TEXLOAD_UPDATETEXTURE;

    // Inidicate that we are a Full & NonSquareW test
	bFull = true;
	bNonSquareW = true;
}

//************************************************************************
// Point_FullNonSquareWCR Test functions

CPoint_FullNonSquareWCRTest::CPoint_FullNonSquareWCRTest()
{
	m_szTestName = "MipFilter Point_FullNonSquareWidth (CopyRects)";
	m_szCommandKey = "Point_FullNonSquareWidthCR";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

    dwLoadOption = TEXLOAD_COPYRECTS;

    // Inidicate that we are a Full & NonSquareW test
	bFull = true;
	bNonSquareW = true;
}

//************************************************************************
// Point_FullNonSquareWLC Test functions

CPoint_FullNonSquareWLCTest::CPoint_FullNonSquareWLCTest()
{
	m_szTestName = "MipFilter Point_FullNonSquareWidth (LockCopy)";
	m_szCommandKey = "Point_FullNonSquareWidthLC";

    dwLoadOption = TEXLOAD_LOCKCOPY;

    // Inidicate that we are a Full & NonSquareW test
	bFull = true;
	bNonSquareW = true;
}

//************************************************************************
// Point_FullNonSquareWUS Test functions

CPoint_FullNonSquareWUSTest::CPoint_FullNonSquareWUSTest()
{
	m_szTestName = "MipFilter Point_FullNonSquareWidth (UpdateSurf)";
	m_szCommandKey = "Point_FullNonSquareWidthUS";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

    dwLoadOption = TEXLOAD_UPDATESURFACE;

    // Inidicate that we are a Full & NonSquareW test
	bFull = true;
	bNonSquareW = true;
}

//************************************************************************
// Point_FullNonSquareHUT Test functions

CPoint_FullNonSquareHUTTest::CPoint_FullNonSquareHUTTest()
{
	m_szTestName = "MipFilter Point_FullNonSquareHeight (UpdateTex)";
	m_szCommandKey = "Point_FullNonSquareHeightUT";

    dwLoadOption = TEXLOAD_UPDATETEXTURE;

    // Inidicate that we are a Full & NonSquareH test
	bFull = true;
	bNonSquareH = true;
}

//************************************************************************
// Point_FullNonSquareHCR Test functions

CPoint_FullNonSquareHCRTest::CPoint_FullNonSquareHCRTest()
{
	m_szTestName = "MipFilter Point_FullNonSquareHeight (CopyRects)";
	m_szCommandKey = "Point_FullNonSquareHeightCR";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

    dwLoadOption = TEXLOAD_COPYRECTS;

    // Inidicate that we are a Full & NonSquareH test
	bFull = true;
	bNonSquareH = true;
}

//************************************************************************
// Point_FullNonSquareHLC Test functions

CPoint_FullNonSquareHLCTest::CPoint_FullNonSquareHLCTest()
{
	m_szTestName = "MipFilter Point_FullNonSquareHeight (LockCopy)";
	m_szCommandKey = "Point_FullNonSquareHeightLC";

    dwLoadOption = TEXLOAD_LOCKCOPY;

    // Inidicate that we are a Full & NonSquareH test
	bFull = true;
	bNonSquareH = true;
}

//************************************************************************
// Point_FullNonSquareHUS Test functions

CPoint_FullNonSquareHUSTest::CPoint_FullNonSquareHUSTest()
{
	m_szTestName = "MipFilter Point_FullNonSquareHeight (UpdateSurf)";
	m_szCommandKey = "Point_FullNonSquareHeightUS";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

    dwLoadOption = TEXLOAD_UPDATESURFACE;

    // Inidicate that we are a Full & NonSquareH test
	bFull = true;
	bNonSquareH = true;
}

//************************************************************************
// Linear_FullSquareUT Test functions

CLinear_FullSquareUTTest::CLinear_FullSquareUTTest()
{
	m_szTestName = "MipFilter Linear_FullSquare (UpdateTex)";
	m_szCommandKey = "Linear_FullSquareUT";

    dwLoadOption = TEXLOAD_UPDATETEXTURE;

    // Inidicate that we are a MipLinear & Full test
	bFull = true;
	bMipLinear = true;
}

//************************************************************************
// Linear_FullSquareCR Test functions

CLinear_FullSquareCRTest::CLinear_FullSquareCRTest()
{
	m_szTestName = "MipFilter Linear_FullSquare (CopyRects)";
	m_szCommandKey = "Linear_FullSquareCR";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

    dwLoadOption = TEXLOAD_COPYRECTS;

    // Inidicate that we are a MipLinear & Full test
	bFull = true;
	bMipLinear = true;
}

//************************************************************************
// Linear_FullSquareLC Test functions

CLinear_FullSquareLCTest::CLinear_FullSquareLCTest()
{
	m_szTestName = "MipFilter Linear_FullSquare (LockCopy)";
	m_szCommandKey = "Linear_FullSquareLC";

    dwLoadOption = TEXLOAD_LOCKCOPY;

    // Inidicate that we are a MipLinear & Full test
	bFull = true;
	bMipLinear = true;
}

//************************************************************************
// Linear_FullSquareUS Test functions

CLinear_FullSquareUSTest::CLinear_FullSquareUSTest()
{
	m_szTestName = "MipFilter Linear_FullSquare (UpdateSurf)";
	m_szCommandKey = "Linear_FullSquareUS";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

    dwLoadOption = TEXLOAD_UPDATESURFACE;

    // Inidicate that we are a Full test
	bFull = true;
	bMipLinear = true;
}

//************************************************************************
// Linear_FullNonSquareWUT Test functions

CLinear_FullNonSquareWUTTest::CLinear_FullNonSquareWUTTest()
{
	m_szTestName = "MipFilter Linear_FullNonSquareWidth (UpdateTex)";
	m_szCommandKey = "Linear_FullNonSquareWidthUT";

    dwLoadOption = TEXLOAD_UPDATETEXTURE;

    // Inidicate that we are a MipLinear, Full, & NonSquareW test
	bFull = true;
	bMipLinear = true;
	bNonSquareW = true;
}

//************************************************************************
// Linear_FullNonSquareWCR Test functions

CLinear_FullNonSquareWCRTest::CLinear_FullNonSquareWCRTest()
{
	m_szTestName = "MipFilter Linear_FullNonSquareWidth (CopyRects)";
	m_szCommandKey = "Linear_FullNonSquareWidthCR";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

    dwLoadOption = TEXLOAD_COPYRECTS;

    // Inidicate that we are a MipLinear, Full, & NonSquareW test
	bFull = true;
	bMipLinear = true;
	bNonSquareW = true;
}

//************************************************************************
// Linear_FullNonSquareWLC Test functions

CLinear_FullNonSquareWLCTest::CLinear_FullNonSquareWLCTest()
{
	m_szTestName = "MipFilter Linear_FullNonSquareWidth (LockCopy)";
	m_szCommandKey = "Linear_FullNonSquareWidthLC";

    dwLoadOption = TEXLOAD_LOCKCOPY;

    // Inidicate that we are a MipLinear, Full, & NonSquareW test
	bFull = true;
	bMipLinear = true;
	bNonSquareW = true;
}

//************************************************************************
// Linear_FullNonSquareWUS Test functions

CLinear_FullNonSquareWUSTest::CLinear_FullNonSquareWUSTest()
{
	m_szTestName = "MipFilter Linear_FullNonSquareWidth (UpdateSurf)";
	m_szCommandKey = "Linear_FullNonSquareWidthUS";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

    dwLoadOption = TEXLOAD_UPDATESURFACE;

    // Inidicate that we are a Full & NonSquareW test
	bFull = true;
	bMipLinear = true;
	bNonSquareW = true;
}

//************************************************************************
// Linear_FullNonSquareHUT Test functions

CLinear_FullNonSquareHUTTest::CLinear_FullNonSquareHUTTest()
{
	m_szTestName = "MipFilter Linear_FullNonSquareHeight (UpdateTex)";
	m_szCommandKey = "Linear_FullNonSquareHeightUT";

    dwLoadOption = TEXLOAD_UPDATETEXTURE;

    // Inidicate that we are a MipLinear, Full, & NonSquareH test
	bFull = true;
	bMipLinear = true;
	bNonSquareH = true;
}

//************************************************************************
// Linear_FullNonSquareHCR Test functions

CLinear_FullNonSquareHCRTest::CLinear_FullNonSquareHCRTest()
{
	m_szTestName = "MipFilter Linear_FullNonSquareHeight (CopyRects)";
	m_szCommandKey = "Linear_FullNonSquareHeightCR";

    // Init Framework options
	m_Options.D3DOptions.fMaxDXVersion = 8.1f;

    dwLoadOption = TEXLOAD_COPYRECTS;

    // Inidicate that we are a MipLinear, Full, & NonSquareH test
	bFull = true;
	bMipLinear = true;
	bNonSquareH = true;
}

//************************************************************************
// Linear_FullNonSquareHLC Test functions

CLinear_FullNonSquareHLCTest::CLinear_FullNonSquareHLCTest()
{
	m_szTestName = "MipFilter Linear_FullNonSquareHeight (LockCopy)";
	m_szCommandKey = "Linear_FullNonSquareHeightLC";

    dwLoadOption = TEXLOAD_LOCKCOPY;

    // Inidicate that we are a MipLinear, Full, & NonSquareH test
	bFull = true;
	bMipLinear = true;
	bNonSquareH = true;
}

//************************************************************************
// Linear_FullNonSquareHUS Test functions

CLinear_FullNonSquareHUSTest::CLinear_FullNonSquareHUSTest()
{
	m_szTestName = "MipFilter Linear_FullNonSquareHeight (UpdateSurf)";
	m_szCommandKey = "Linear_FullNonSquareHeightUS";

    // Init Framework options
	m_Options.D3DOptions.fMinDXVersion = 9.0f;

    dwLoadOption = TEXLOAD_UPDATESURFACE;

    // Inidicate that we are a MipLinear, Full, & NonSquareH test
	bFull = true;
	bMipLinear = true;
	bNonSquareH = true;
}

//************************************************************************
// None_FullSquare Test functions

CNone_FullSquareTest::CNone_FullSquareTest()
{
	m_szTestName = "MipFilter None_FullSquare";
	m_szCommandKey = "None_FullSquare";

    // Inidicate that we are a MipNone & Full test
	bFull = true;
	bMipNone = true;
}
