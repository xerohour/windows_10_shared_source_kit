//
// MRT.H - Header file for the MultipleRenderTarget test
//

#ifndef _MRT_H_
#define _MRT_H_

#include "CD3DTest.h"
#include "CTextures.h"

#define MRT_FVFTLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_SPECULAR | D3DFVF_TEX1)

typedef struct _mrt_tlvertex
{
    float x, y, z, rhw;
    DWORD dwDiffuse;
	DWORD dwSpecular;
    float u, v;
} MRT_TLVERTEX, *PMRT_TLVERTEX;

class CMRT: public CD3DTest
{
    // Data
    private:
	UINT nRTFormats;
	FORMAT RTFormats[80];
    VIEWPORT ViewportRT;
	MRT_TLVERTEX Tri[6];
	MRT_TLVERTEX Quad[4][4];
	CnPixelShader *pPixelShader;
    CnTexture *pTextureRT[4];
    CnSurface *pTempRT[4], *pTempZ;
    CnSurface *pOriginalRT, *pOriginalZ; 
    int nWidthRT, nHeightRT, nNumElements;
    int nOriginalZFormat, nTexFormat, nZFormat;

    // Data
    public:
	int nNumRTs;
    bool bColorWrite, bMETex, bBlend, bInvalid, bFog;

	public:
    CMRT();
    ~CMRT();

	// Framework functions
    virtual void CommandLineHelp(void);
    virtual UINT TestInitialize(void);
    virtual bool ClearFrame(void);
    virtual bool ExecuteTest(UINT);
    virtual bool TestTerminate(void);

	// Helper functions
	void DrawGrid(int);
    void DescribeTest(TCHAR *, FORMAT **, FORMAT *);
};

// One Texture Class definitions
class CMRTOneTexture: public CMRT
{
	public:
	CMRTOneTexture();
};

// Two Texture Class definitions
class CMRTTwoTexture: public CMRT
{
	public:
	CMRTTwoTexture();
};

// Three Texture Class definitions
class CMRTThreeTexture: public CMRT
{
	public:
	CMRTThreeTexture();
};

// Four Texture Class definitions
class CMRTFourTexture: public CMRT
{
	public:
	CMRTFourTexture();
};

// One Blend Class definitions
class CMRTOneBlend: public CMRT
{
	public:
	CMRTOneBlend();
};

// Two Blend Class definitions
class CMRTTwoBlend: public CMRT
{
	public:
	CMRTTwoBlend();
};

// Three Blend Class definitions
class CMRTThreeBlend: public CMRT
{
	public:
	CMRTThreeBlend();
};

// Four Blend Class definitions
class CMRTFourBlend: public CMRT
{
	public:
	CMRTFourBlend();
};

// MRT Fog Class definitions
class CMRTFog: public CMRT
{
	public:
	CMRTFog();
};

// MRT ColorWrite Class definitions
class CMRTColorWrite: public CMRT
{
	public:
	CMRTColorWrite();
};

// Invalid Class definitions
class CMRTInvalid: public CMRT
{
	public:
	CMRTInvalid();
};

// Multiple Element Texture Class definitions
class CMETexture: public CMRT
{
	public:
	CMETexture();
};

// MET Blend Class definitions
class CMETBlend: public CMRT
{
	public:
	CMETBlend();
};

// MET Fog Class definitions
class CMETFog: public CMRT
{
	public:
	CMETFog();
};

// MET ColorWrite Class definitions
class CMETColorWrite: public CMRT
{
	public:
	CMETColorWrite();
};

#endif //_MRT_H_
