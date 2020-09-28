// FILE:        multisample.h
// DESC:        multisample antialiasing/masking conformance test header

#include <d3d9.h>
#include <d3dx9.h>
#include "CD3DTest.h"

#include "CFlags.h"

#define GLE(x) (m_pD3D->HResultToString(x))

#ifndef D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE
#define D3DPRASTERCAPS_STRETCHBLTMULTISAMPLE 0x00800000L
#endif

const D3DCOLOR RED		= 0x00FF0000;
const D3DCOLOR GREEN	= 0x0000FF00;
const D3DCOLOR BLUE		= 0x000000FF;
const D3DCOLOR YELLOW	= 0x00FFFF00;
const D3DCOLOR CYAN		= 0x0000FFFF;
const D3DCOLOR MAGENTA	= 0x00FF00FF;

const D3DCOLOR BLACK = 0x00000000;
const D3DCOLOR GRAY	 = 0x00A0A0A0;
const D3DCOLOR WHITE = 0x00FFFFFF;

const UINT MAXBUFFER = 512; // max generic buffer length

const float SUBPIX_FOURBIT =		0.0625f;
const float SUBPIX_EIGHTBIT =		0.00390625f;
/*
These levels of accuracy would make the test take a rediculous amount of time
In fact, even eightbit is just too long
const float SUBPIX_SIXTEENBIT =		0.0000152587890625f;
const float SUBPIX_TWENTYFOURBIT =	0.000000059604644775390625f;
const float SUBPIX_THIRTYTWOBIT =	0.00000000023283064365386962890625f;
*/

const DWORD FLEXFMT = ( D3DFVF_XYZRHW | D3DFVF_DIFFUSE ); // FVF format

const UINT TEST_POINT_COUNT = 13;

typedef struct _FlexVert
{
    float fPx;
    float fPy;
    float fPz;
    float fRHW;
    D3DCOLOR Diffuse;
} FLEXVERTEX, *PFLEXVERTEX;

class CMultisample: public CD3DTest
{
private:
	RECT MouseTrap;

protected:
	bool m_bGetSamplePoints;
	bool m_bVerbose;
	void HideMouse();
	void ShowMouse();
	HRESULT GetScene( LOCKED_RECT ** pRect, CSurface ** fbSurf );

public:
    CMultisample();
    ~CMultisample();
	virtual bool DisplayFrame();
    virtual UINT TestInitialize();
	bool ParseCmdLine();
};

class CSamples: public CMultisample
{
private:
	void Render( PFLEXVERTEX pfvSubPix );
	bool QuickTest();
	bool ExhaustiveTest();
	void WasSampled( BOOL * rgbResults );
	UINT SampleCount();
	
	//Diagnostic test
	void GetPointsTest();
	void FindPoints( CSurface *pSurface, UINT X, UINT Y );
	void SaveSurface( CSurface *pSurface );
	void DrawDividers( CSurface *pSurface );

	POINT TestPoint[TEST_POINT_COUNT];

	bool m_bForceSlow;
	float m_fSampleSize;

public:
	CSamples();
	~CSamples();
	void CommandLineHelp();

	UINT TestInitialize();
	bool ExecuteTest(UINT nTest);
	bool ProcessFrame() { return true; };
	bool ParseCmdLine();
};

class CMask: public CMultisample
{
private:
	D3DCOLOR GetColor( D3DCOLOR OldColor );
	bool VerifyDiff( D3DCOLOR * ColorDiff, UINT uiSamples, UINT uiMax );

public:
	CMask();
	~CMask();
	UINT TestInitialize();
	void CommandLineHelp();
	bool ExecuteTest( UINT nTest );
	bool ProcessFrame( void ) { return true; };
};

class CToggle: public CMultisample
{
private:
	void QuickSampleTest( UINT iYOffset );
	void MaskTest();
	void VerifyResults();

	bool m_bMask;

public:
	CToggle();
	~CToggle();
	UINT TestInitialize();
	bool ExecuteTest( UINT nTest );
	bool ProcessFrame( void ) { return true; };
};

class CCentroid: public CMultisample
{
private:
	bool SetupPixelShader( UINT nTest );
	bool SetupRender( UINT nTest );
	bool Render( UINT nTest );
	bool VerifyResults( UINT nTest );
	bool m_bWarning;

	CPShader		*pShader;
	CnTexture		*ppTexture;

public:
	CCentroid();
	~CCentroid();
	UINT TestInitialize();
	bool ExecuteTest( UINT nTest );
	bool ProcessFrame( void ) { return true; };
	bool Setup( void );
	void Cleanup();
};
