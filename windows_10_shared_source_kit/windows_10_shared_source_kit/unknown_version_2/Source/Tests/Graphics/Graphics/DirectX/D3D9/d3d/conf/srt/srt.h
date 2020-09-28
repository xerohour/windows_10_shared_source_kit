//
// SRT.H - Header file for the SetRenderTarget test
//

#ifndef _SRT_H_
#define _SRT_H_

#include "CD3DTest.h"
#include "CTextures.h"
#include <vector>

#define SRT_SIZE 256 
#define SRT_FVFTLVERTEX (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX1)

typedef struct _srt_tlvertex
{
    float x, y, z, rhw;
    DWORD dwDiffuse;
    float u, v;
} SRT_TLVERTEX, *PSRT_TLVERTEX;

class CSRT: public CD3DTest
{
    // Data
    private:
    bool bValidate;
    VIEWPORT ViewportRT;
    CnTexture *pTextureRT;
    CnSurface *pOffScreenRT;
    CnSurface *pTempRT, *pTempZ;
    CnSurface *pOriginalRT, *pOriginalZ; 
    int nOriginalZFormat, nTexFormat, nZFormat;

    // Data
    public:
    bool bOffscreen, bInvalid, bNull;
    int nWidthRT, nHeightRT;

	public:
    CSRT();
    ~CSRT();

	// Framework functions
    virtual void CommandLineHelp(void);
    virtual UINT TestInitialize(void);
    virtual bool ClearFrame(void);
    virtual bool ExecuteTest(UINT uiTest);
    virtual bool ProcessFrame(void);
    virtual bool TestTerminate(void);

	// Helper functions
    bool CreateBuffers(bool bCreateRT, bool bCreateZ);
    void DescribeTest(TCHAR *, int, int);
    std::vector< std::pair<DWORD, DWORD> > m_vSupportedFormats;
};

// Texture Class definitions
class CSRTTexture: public CSRT
{
	public:
	CSRTTexture();
};

// No ZBuffer Texture Class definitions
class CSRTNoZTexture: public CSRT
{
	public:
	CSRTNoZTexture();
};

// Offscreen Class definitions
class CSRTOffscreen: public CSRT
{
	public:
	CSRTOffscreen();
};

// No ZBuffer Offscreen Class definitions
class CSRTNoZOffscreen: public CSRT
{
	public:
	CSRTNoZOffscreen();
};

// Null Class definitions
class CSRTNull: public CSRT
{
	public:
	CSRTNull();
};

// No ZBuffer Null Class definitions
class CSRTNoZNull: public CSRT
{
	public:
	CSRTNoZNull();
};

// Invalid Class definitions
class CSRTInvalid: public CSRT
{
	public:
	CSRTInvalid();
};

#endif //_SRT_H_
