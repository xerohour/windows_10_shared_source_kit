// FILE:        luminance.h
// DESC:        luminance class header for AL8LU8 and LU8 texture formats
// AUTHOR:      Todd M. Frost

#include <ddraw.h>
#include <d3d9.h>
#include "CD3DTest.h"
#include "CD3D.h"
#include "CFlags.h"
#include "CImage.h"
#include "CTextures.h"

#define GLE(x) (m_pD3D->HResultToString(x))

typedef struct _FlexVert
{
    D3DVALUE fPx;
    D3DVALUE fPy;
    D3DVALUE fPz;
    D3DVALUE fRHW;
    D3DCOLOR dcDiffuse;
    D3DVALUE fSu;
    D3DVALUE fSv;
    D3DVALUE fTu;
    D3DVALUE fTv;
} FLEXVERT, *PFLEXVERT;

const DWORD FLEXFMT = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2);

const D3DTEXTUREADDRESS DEFAULT_MODE = D3DTADDRESS_WRAP; // default address mode

const DWORD CF_INDEPENDENT = 0x00000001; // independent uv address modes
const DWORD CF_VALIDATE    = 0x00000002; // validate src/ref devices

const DWORD NOFORMAT = 0xFFFFFFFF; // no format index

const DWORD TEXWIDTH = 256;
const DWORD TEXHEIGHT = 256;

const UINT MAXBUFFER = 128;

const UINT MAXFORMAT = 16;

const UINT NMODES = 4; // number of address modes

const UINT NSTAGES = 3; // number of stages

const UINT DEFAULT_BUMPS = 8;  // default number of bumps
const UINT MIN_BUMPS     = 1;  // min number of bumps
const UINT MAX_BUMPS     = 64; // max number of bumps

const UINT NSTATUS = 5;

class CLuminance: public CD3DTest
{
    public:
        CHAR m_szBuffer[MAXBUFFER];             // text buffer
        CHAR m_szBaseFormat[MAXFORMAT];         // texture format (base)
        CHAR m_szLuminanceFormat[MAXFORMAT];    // texture format (luminance)
        CHAR m_szStatus[NSTATUS][2][MAXBUFFER]; // texture format (luminance)
        D3DTEXTUREADDRESS m_dwModes[2];         // uv texture address modes
        DWORD m_dwCommonLuminanceFormats;       // common luminance formats
        DWORD m_dwBaseFormat;                   // base texture format index
        DWORD m_dwLuminanceFormat;              // luminance texture format index
        DWORD m_dwStages;                       // texture stages (used)
        DWORD m_dwStagesMax;                    // texture stages (max)
        DWORD m_dwTexAddressCaps;               // texture address caps
        DWORD m_dwTexOpCaps;                    // texture op caps
        DWORD m_dwModeCapsVals[NMODES][2];      // texture address mode caps/vals
        CFlags m_Flags;                         // flags class
        CImage *m_pBaseImage;                   // image class (base)
        CImage *m_pLuminanceImage;              // image class (luminance)
        CnTexture *m_pBaseTexture;              // texture class (base)
        CnTexture *m_pLuminanceTexture;         // texture class (luminance)
        
    public:
        CLuminance();
        ~CLuminance();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual bool Setup(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        virtual VOID Cleanup(VOID);
        virtual VOID UpdateStatus(VOID);
        VOID ClearStatus(VOID);
        bool Match(DWORD dwFormat, DDPIXELFORMAT ddpfPixelFormat);
        bool SetLuminanceTexture(DWORD dwFormat, DWORD dwWidth, DWORD dwHeight);
        DWORD GetCountFromMask(DWORD dwMask);
        VOID ProcessArgs(VOID);
};

class CLuminanceAddress : public CLuminance
{
    public:
        CLuminanceAddress();
        ~CLuminanceAddress();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};
