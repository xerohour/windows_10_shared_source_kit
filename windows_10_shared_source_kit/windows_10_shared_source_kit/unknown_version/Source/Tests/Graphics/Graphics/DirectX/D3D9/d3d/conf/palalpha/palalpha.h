#ifndef __PalAlpha_h__
#define __PalAlpha_h__

#include "CD3DTest.h"
#include "CTextures.h"

enum TEXTURE_TYPE {
	UNDEFINED_TEXTURE,
	PAL8ALPHA,
	PAL8,
	PAL4ALPHA,
	PAL4
};

class CPalTest: public CD3DTest
{
public:
    CPalTest();
	~CPalTest();

    virtual UINT		TestInitialize(void);
	virtual bool		ClearFrame(void);
    virtual bool		ExecuteTest(UINT);
    virtual void		SceneRefresh(void);
    virtual bool        TestTerminate(void);

	virtual bool		SetDefaultRenderStates(void);

private:
	CnTexture*				m_pTexture;
    CnTexture*				m_pDestTexture;
	
	LPD3DTLVERTEX			pVertices;
	LPDIRECTDRAWPALETTE		pPalette;
    DWORD					dwVertexCount;
	LPWORD					pIndices;
	DWORD					dwIndexCount;
	DWORD					dwNumFail;
	bool					bFrame;

	// Palette type
	bool					bTest4BitPalette;
	bool					bTest8BitPalette;
	bool					bTestNonAlphaPalette;
	bool					bTestAlphaPalette;
	DWORD					dwTextureTypesNumber;
	LPTSTR					lpszTextureType;
	DWORD					dwCurrentTextureType;
	bool					bCreateNewTexture;

	// Test type
	bool					bTestTextureCreate;
	bool					bTestTextureLoad;
	bool					bTestPaletteCreate;
	bool					bTestSetEntries;
	bool					bTestWrapMode;
	bool					bTestMirrorMode;
	bool					bTestClampMode;
	bool					bTestBorderMode;
	bool					bTestColorKey;
	bool					bTestColorKeyBlend;
	DWORD					dwTestTypesNumber;
	LPTSTR					lpszTestType;
	DWORD					dwCurrentTestType;

	// Test number
	DWORD					dwSameFramesNumber;
	DWORD					dwTotalTestsNumber;

	// Current test information
	DWORD					dwCurrentTestNumber;
	unsigned char			dwPaletteType;
	bool					bAlphaPalette;
	bool					bCreateTexture;
	bool					bLoadTexture;
	bool					bCreatePalette;
	bool					bSetPaletteEntries;
	DWORD					dwAddressMode;
	bool					bUseColorKey;
	bool					bUseColorKeyBlend;
	DWORD					dwFrameParam;
	bool					bLastTestFailed;
	DWORD					dwCurrentTexturePalette;

	static D3DTLVERTEX		vertices1[4];
	static D3DTLVERTEX		vertices2[4];
	static WORD				indices[6];

	void					ResolveTestNumber(int iTestNum);
	bool					ValidateTest(int iTestNum);

//    CnTexture*	            CreateTexture(DWORD dwWidth, DWORD dwHeight, DWORD dwPaletteType, bool bFillTexture, bool bColorKeyFill);
//	bool					CreatePalettedTexture7(CSurface*, DWORD dwWidth, DWORD dwHeight, DWORD dwFlags, DWORD dwPaletteType, bool bFillTexture, bool bColorKeyFill);
	bool					CreateTexturePalette(CnTexture*, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill, DWORD dwFlags = CD3D_ALL);
	bool					ChangeTexturePalette(CnTexture*, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill, DWORD dwFlags = CD3D_ALL);

    CnTexture*              CreateTexture(DWORD dwWidth, DWORD dwHeight, DWORD dwPaletteType, bool bFillTexture, bool bColorKeyFill);
    bool                    SetPaletteEntries(DWORD dwPalette, bool bAlphaPalette, DWORD dwParam);
    bool                    SetCurrentTexturePalette(DWORD dwPalette);

	void					FillPaletteEntries(PALETTEENTRY *pal, DWORD dwPaletteType, bool bAlphaPalette, DWORD dwParam, bool bColorKeyFill);
	bool					ColorSurface(CSurface*);
};

#endif // __PalAlpha_h__