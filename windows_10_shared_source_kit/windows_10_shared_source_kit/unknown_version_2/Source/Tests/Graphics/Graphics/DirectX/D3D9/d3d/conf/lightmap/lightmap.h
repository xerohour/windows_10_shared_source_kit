// FILE:        lightmap.h
// DESC:        lightmap class header for D3DTOP_DOTPRODUCT3 conformance test
// AUTHOR:      Todd M. Frost

#include <d3d9.h>
#include "CD3DTest.h"
#include "CD3D.h"
#include "CFlags.h"
#include "CImage.h"
#include "CTextures.h"

#define GLE(x) (m_pD3D->HResultToString(x))

typedef enum _CoordIndex
{
    CIX = 0,
    CIY = 1,
    CIZ = 2,
    FORCE_DWORD = 0xffffffff
} COORDINDEX;

typedef struct FlexVert
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

const DWORD CF_VALIDATE       = 0x00000001; // validate src/ref devices
const DWORD CF_MIX            = 0x00000002; // mix sysmem/vidmem textures
const DWORD CF_MODULATE       = 0x00000004; // D3DTOP_MODULATE vs D3DTOP_DOTPRODUCT3
const DWORD CF_SYSTEM_ALLOWED = 0x00000010; // system memory textures allowed
const DWORD CF_SYSTEM_NORTH   = 0x00000020; // system memory texture created (north)
const DWORD CF_SYSTEM_SOUTH   = 0x00000040; // system memory texture created (south)
const DWORD CF_ALPHA          = 0x00000100; // use alpha blending
const DWORD CF_ALPHARS        = 0x00000200; // use alpha blending render state
const DWORD CF_FACTOR         = 0x00000400; // use texture factor

// signed (R,G,B) = (x,y,z)

const DWORD RGB_ZEROS     = RGB_MAKE(128, 128, 128); // (x,y,z) = ( 0, 0, 0)
const DWORD RGB_POSITIVEZ = RGB_MAKE(128, 128, 255); // (x,y,z) = ( 0, 0, 1)
const DWORD RGB_NEGATIVEZ = RGB_MAKE(128, 128,   0); // (x,y,z) = ( 0, 0,-1)

const DWORD NOFORMAT = 0xFFFFFFFF; // no format index

const DWORD TEXWIDTH  = 256;
const DWORD TEXHEIGHT = 256;

const FLOAT LEMNISCATE_SCALE = 0.4f; // lemniscate scale

const FLOAT ZERO = 1.0e-8f; // floating point zero

const FLOAT RADIUS         = 0.5f;  // sphere radius
const FLOAT RADIUS_SQUARED = 0.25f; // sphere radius squared

const UINT MAXBUFFER = 128;

const UINT MAXFORMAT = 16;

const UINT NSTAGES = 2; // number of stages

// intervals for lemniscate traversal

const UINT DEFAULT_INTERVALS = 8;   // default intervals
const UINT MIN_INTERVALS     = 0;   // min     intervals
const UINT MAX_INTERVALS     = 256; // min     intervals

// magnitude for lemniscate formula (r = sin(magnitude*theta))

const UINT DEFAULT_MAGNITUDE = 2; // default magnitude
const UINT MIN_MAGNITUDE     = 1; // min     magnitude
const UINT MAX_MAGNITUDE     = 8; // max     magnitude

const UINT NSTATUS = 2;

class CLightmap: public CD3DTest
{
    private:
        CHAR m_szBuffer[MAXBUFFER];             // text buffer
        CHAR m_szFormat[MAXFORMAT];             // texture format
        CHAR m_szStatus[NSTATUS][2][MAXBUFFER]; // texture format
        DOUBLE m_fFactor;                       // lemniscate factor (r = sin(factor*theta))
        DWORD m_dwFormat;                       // current RGB texture format index
        DWORD m_dwIntervals;                    // intervals for lemniscate traversal 
        DWORD m_dwOp;                           // color op (D3DTOP_DOTPRODUCT3 or D3DTOP_MODULATE)
        DWORD m_dwStages;                       // max texture stages (clamped to 2)
        CFlags m_Flags;                         // flags class
        CImage *m_pNorthernImage;               // image class
        CImage *m_pSouthernImage;               // image class
        CnTexture *m_pNorthernTexture;          // texture class
        CnTexture *m_pSouthernTexture;          // texture class
        
    public:
        CLightmap();
        ~CLightmap();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual bool Setup(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        virtual VOID Cleanup(VOID);
        virtual VOID UpdateStatus(VOID);
        VOID ClearStatus(VOID);
        VOID ProcessArgs(VOID);
};
