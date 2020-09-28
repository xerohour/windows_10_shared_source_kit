// FILE:        lightmap.h
// DESC:        lightmap class header for D3DTOP_DOTPRODUCT3 conformance test
// AUTHOR:      Todd M. Frost

#include <d3d9.h>
// Other includes
#include "CD3DTest.h"
#include "CD3D.h"
#include "CFlags.h"
#include "CImage.h"
#include "CTextures.h"
#include "CBuffers.h"


//
// CLHFonts_Base - base functionality shared by all the longhorn fonts tests
//

class CLHFonts_Base : public CD3DTest
{
public:
	typedef struct
	{
		// Rect info
		UINT uX1;
		UINT uY1;
		UINT uX2;
		UINT uY2;
			
		// UV Info
		float fU1;
		float fV1;
		float fU2;
		float fV2;
			
		// Font writing help
		UINT uWidth;
		UINT uHeight;
	} FONTINFO, *PFONTINFO;
		
	typedef struct
	{
		float x;
		float y;
		float z;
		float w;
		DWORD dwColor;
		float u;
		float v;
	} DVERTEX, *PDVERTEX;   // Display vertex

	// constructor/destructor
	CLHFonts_Base();
	~CLHFonts_Base();
	
	// virtual methods inherited from the base class
	virtual void CommandLineHelp();
	virtual bool CapsCheck();
	virtual bool Setup();
	virtual void Cleanup();
	virtual UINT TestInitialize();
	virtual bool TestTerminate();
	virtual bool ExecuteTest(UINT uTest );

	// utility methods
	bool CreateFontImage( UINT );
	void DisplayImage( UINT uColor, float fScale );
	void DisplayDeviceImage( UINT iDevice, UINT uColor, float fScale );
	void PrepareDestinationSurface( UINT uOp );
	void DrawText( int, int, LPTSTR, UINT uOP, int, int );
	
	// the main rendering method to be defined by each test
	virtual bool ExecuteTestOnDevice( UINT uTest ) = 0;

protected:
	LPVERTEXBUFFERS m_pSrcVB;
	LPVERTEXBUFFERS m_pDestVB;
	LPTEXTURES m_pTexture;
	LPSURFACES m_pTexSurface;
	LPTEXTURES m_pFontTexture;
	LPSURFACES m_pFontSurface;
	bool m_bUseManagedSurfaces;
	
	FONTINFO m_FontInfo[256];
	CImage *m_pFontImage;
	UINT m_uTextureSize;

	UINT m_uVBRectOffset;
	UINT m_srcVBLength;
	UINT m_destVBLength;
};


//
// CLHFonts_ComposeOp - cycles on D3DCOMPOSEOP with random text
//

class CLHFonts_ComposeOp : public CLHFonts_Base
{
public:
	CLHFonts_ComposeOp();
	virtual void CommandLineHelp();
	virtual UINT TestInitialize();
	virtual bool ExecuteTestOnDevice( UINT uTest );
};


//
// CLHFonts_ComposeRects - main ComposeRects test, cycles on all params except D3DCOMPOSEOP with random text
//

class CLHFonts_ComposeRects : public CLHFonts_Base
{
public:
	CLHFonts_ComposeRects();
	virtual void CommandLineHelp();
	virtual UINT TestInitialize();
	bool ExecuteTestOnDevice( UINT uTest );
};


//
// CLHFonts_SetConvolutionMonoKernel - main test for SetConvolutionMonoKernel API
//

class CLHFonts_SetConvolutionMonoKernel : public CLHFonts_Base
{
public:
	CLHFonts_SetConvolutionMonoKernel();
	virtual void CommandLineHelp();
	virtual UINT TestInitialize();
	bool ExecuteTestOnDevice( UINT uTest );
};


//
// CLHFonts_ConvolutionStateBlock - test that SetConvolutionMonoKernel parameters get saved in the state block
//

class CLHFonts_ConvolutionStateBlock : public CLHFonts_Base
{
public:
	CLHFonts_ConvolutionStateBlock();
	virtual void CommandLineHelp();
	virtual UINT TestInitialize();
	bool ExecuteTestOnDevice( UINT uTest );
};


//
// CLHFonts_API - API validation
//

class CLHFonts_API : public CLHFonts_Base
{
public:
	CLHFonts_API();
	virtual UINT TestInitialize();
	bool ExecuteTestOnDevice( UINT uTest );
};


/*
class CTentFilter: public CBaseLongFonts
{
    public:
    
        bool ExecuteTestOnDevice(UINT uDevice, UINT uTest);
        virtual UINT TestInitialize(void);
};

class CRowOnly: public CBaseLongFonts
{
    public:
    
        bool ExecuteTestOnDevice(UINT uDevice, UINT uTest);
        virtual UINT TestInitialize(void);
};

class CColOnly: public CBaseLongFonts
{
    public:
    
        bool ExecuteTestOnDevice(UINT uDevice, UINT uTest);
        virtual UINT TestInitialize(void);
};
*/