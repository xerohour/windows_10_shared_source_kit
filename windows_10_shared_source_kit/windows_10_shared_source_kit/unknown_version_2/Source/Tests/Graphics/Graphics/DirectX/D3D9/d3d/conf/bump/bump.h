// FILE:        bump.h
// DESC:        bump class header for
//                  D3DTOP_BUMPENVMAP
//                  D3DTOP_BUMPENVMAPLUMINANCE
//              conformance tests
// AUTHOR:      Todd M. Frost
// COMMENTS:    du:dv:lu usage:
//                  v += du*M[0][0] + dv*M[0][1]
//                  u += du*M[1][0] + dv*M[1][1]
//                  l = lu*scale + offset
//                  lu defaults to 1 for Du8:Dv8 textures

#include <d3d9.h>
#include "CD3DTest.h"
#include "CD3D.h"
#include "CFlags.h"
#include "CImage.h"
#include "CTextures.h"

#define GLE(x) (m_pD3D->HResultToString(x))

typedef union _FLOATUINT32
{
    FLOAT f;
    DWORD dw;
    UINT32 u;
} FLOATUINT32;

typedef struct _FlexVert
{
    D3DVALUE fPx;
    D3DVALUE fPy;
    D3DVALUE fPz;
    D3DVALUE fRHW;
    DWORD dcDiffuse;
    D3DVALUE fSu;
    D3DVALUE fSv;
    D3DVALUE fTu;
    D3DVALUE fTv;
} FLEXVERT, *PFLEXVERT;

const DWORD FLEXFMT = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE | D3DFVF_TEX2);

const D3DTEXTUREADDRESS DEFAULT_MODE = D3DTADDRESS_WRAP;

const DWORD CF_INDEPENDENT      = 0x00000001; // independent uv texture address mode
const DWORD CF_SYSTEM_ALLOWED   = 0x00000002; // system memory textures allowed
const DWORD CF_SYSTEM_CREATED   = 0x00000004; // system memory textures created
const DWORD CF_VALIDATE         = 0x00000008; // validate src/ref devices
const DWORD CF_MODULATE         = 0x00000010; // blend:  modulate texture/current
const DWORD CF_MODULATE2X       = 0x00000020; // blend:  modulate texture/current 2x
const DWORD CF_MODULATE4X       = 0x00000040; // blend:  modulate texture/current 4x
const DWORD CF_BLEND            = 0x00000070; // blend mask

const DWORD NOFORMAT = 0xFFFFFFFF; // no format index

const DWORD TEXWIDTH = 256;
const DWORD TEXHEIGHT = 256;

const FLOAT DEFAULT_MAG    = 0.04f; // default bumpenvmap matrix magnitude
const FLOAT DEFAULT_OFFSET = 0.0f; // default bumpenvmap luminance offset
const FLOAT DEFAULT_SCALE  = 0.7f; // default bumpenvmap luminance scale

const UINT MAXBUFFER = 128;

const UINT MAXFORMAT = 16;

const UINT NBLENDS = 3; // number of blend modes (mod, mod2x, mod4x)

const UINT DEFAULT_BUMPS = 6;  // default number of bumps
const UINT MIN_BUMPS     = 1;  // min number of bumps
const UINT MAX_BUMPS     = 64; // max number of bumps

const UINT DEFAULT_VARIATIONS = 8;    // default number of variations
const UINT MIN_VARIATIONS     = 2;    // min number of variations
const UINT MAX_VARIATIONS     = 1024; // max number of variations

const UINT MATRIX_SCALE_DU   = 0;
const UINT MATRIX_SCALE_DV   = 1;
const UINT MATRIX_SCALE_DUDV = 2;
const UINT MATRIX_SKEW_DU    = 3;
const UINT MATRIX_SKEW_DV    = 4;
const UINT MATRIX_SKEW_DUDV  = 5;
const UINT NMATRICES         = 6;

const UINT MODE_BORDER = 0;
const UINT MODE_CLAMP  = 1;
const UINT MODE_MIRROR = 2;
const UINT MODE_WRAP   = 3;
const UINT NMODES      = 4;

const UINT NSTATUS = 8;

class CBump: public CD3DTest
{
    public:
        CHAR m_szBuffer[MAXBUFFER];             // text buffer
        CHAR m_szBaseFormat[MAXFORMAT];         // text buffer (format (base))
        CHAR m_szBumpFormat[MAXFORMAT];         // text buffer (format (bump))
        CHAR m_szStatus[NSTATUS][2][MAXBUFFER]; // text buffer (status)
        D3DTEXTUREADDRESS m_dwModes[2];         // uv texture address modes
        DWORD m_dwBaseFormat;                   // base texture format index
        DWORD m_dwBumpFormat;                   // bump texture format index
        DWORD m_dwCommonBumpFormats;            // common bump formats
        DWORD m_dwInclusion;                    // inclusion flags
        DWORD m_dwMatrixFormat;                 // bump map matrix format index
        DWORD m_dwTexAddressCaps;               // texture address caps
        DWORD m_dwTexOpCaps;                    // texture op caps
        DWORD m_dwModeCapsVals[NMODES][2];      // texture address mode caps/vals
        FLOATUINT32 m_fOffset;                  // bump map luminance offset
        FLOATUINT32 m_fScale;                   // bump map luminance scale
        FLOATUINT32 m_fMatrix[2][2];            // bump map transformation matrix
        UINT m_uVariations;                     // number of variations
        CFlags m_Flags;                         // flags class
        CImage *m_pBaseImage;                   // image class (base)
        CImage *m_pBumpImage;                   // image class (bump)
        CnTexture *m_pBaseTexture;              // texture class (base)
        CnTexture *m_pBumpTexture;              // texture class (bump)
        
    public:
        CBump();
        ~CBump();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual bool Setup(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        virtual VOID Cleanup(VOID);
        virtual VOID UpdateStatus(VOID);
        VOID ClearStatus(VOID);
        bool SetBumpTexture(DWORD dwFormat, DWORD dwWidth, DWORD dwHeight);
        VOID SetBumpMatrix(DWORD dwType, FLOAT fDu, FLOAT fDv);
        DWORD GetCountFromMask(DWORD dwMask);
        FLOAT Normalize(DWORD dwCurrent, DWORD dwMin, DWORD dwMax);
        VOID ProcessArgs(VOID);
};

class CBumpAddress : public CBump
{
    public:
        CBumpAddress();
        ~CBumpAddress();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CBumpBlend : public CBump
{
    public:
        CBumpBlend();
        ~CBumpBlend();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CBumpMatrix : public CBump
{
    public:
        CBumpMatrix();
        ~CBumpMatrix();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CBumpMix : public CBump
{
    public:
        CBumpMix();
        ~CBumpMix();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
		virtual bool ProcessFrame(VOID);
};

class CBumpOffset : public CBump
{
    public:
        CBumpOffset();
        ~CBumpOffset();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CBumpScale : public CBump
{
    public:
        CBumpScale();
        ~CBumpScale();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};
