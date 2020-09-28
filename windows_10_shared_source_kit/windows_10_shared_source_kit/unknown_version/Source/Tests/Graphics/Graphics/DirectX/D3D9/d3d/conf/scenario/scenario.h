// FILE:        scenario.h
// DESC;        scenario class header for driver scenario conformance test

#define D3D_OVERLOADS
#include <ddraw.h>
#include <d3d9.h>
#include <d3dx9.h>
#include "CD3DTest.h"
#include "CD3D.h"
#include "CFlags.h"
#include "CShapes.h"
#include "CSurface.h"
#include "CImage.h"
#include "CTextures.h"
#include <vector>

// Dx8 specific stuff

#ifndef D3DFVF_VERTEX 
#define D3DFVF_VERTEX (D3DFVF_XYZ | D3DFVF_NORMAL | D3DFVF_TEX1)
#endif

#ifndef D3DPRASTERCAPS_COLORPERSPECTIVE
#define D3DPRASTERCAPS_COLORPERSPECTIVE 0x00400000L
#endif

#define ADD(_f, _v, _i) { _v.push_back(_v[_i]); _v.back().dwFlags |= _f; }

#define CHECK(_f, _r) (((_f) & (_r)) == (_f))

#define GLE(x) (m_pD3D->HResultToString(x))

#define BMP_LUMINANCE "luminance.bmp"
#define BMP_PLANE     "stripes.bmp"
#define BMP_SPHERE    "dxlogo.bmp"

typedef union
{
    DWORD dwValue;
    float fValue;
} DWORDFLOAT;

typedef struct _TestDef
{
    BYTE bFrame;
    DWORD dwFlags;

    _TestDef(DWORD dwState, BYTE bRot)
    {
        bFrame = bRot;
        dwFlags = dwState;
    }
    _TestDef()
    {
        bFrame = (BYTE) 0;
        dwFlags = (DWORD) 0;
    }
    _TestDef(const _TestDef &Ref)
    {
        bFrame = Ref.bFrame;
        dwFlags = Ref.dwFlags;
    }
} TESTDEF, *PTESTDEF;

typedef std::vector<TESTDEF> TESTVECTOR;

const DWORD CF_ALPHA     = 0x00000001;
const DWORD CF_CORRECT   = 0x00000002;
const DWORD CF_DITHER    = 0x00000004;
const DWORD CF_FOG       = 0x00000008;
const DWORD CF_MAGFILTER = 0x00000010;
const DWORD CF_MINFILTER = 0x00000020;
const DWORD CF_MIPFILTER = 0x00000040;
const DWORD CF_MODULATE  = 0x00000080;
const DWORD CF_NORENDER  = 0x00000100;
const DWORD CF_REFRESH   = 0x00000200;
const DWORD CF_SPECULAR  = 0x00000400;
const DWORD CF_TARGET    = 0x00000800;
const DWORD CF_TEXTURE   = 0x00001000;
const DWORD CF_WBUFFER   = 0x00002000;
const DWORD CF_ZBUFFER   = 0x00004000;

const DWORD CF_FAIL      = 0x40000000;
const DWORD CF_SKIP      = 0x80000000;

const UINT MAXBUFFER = 256;

const UINT MAXFORMAT = 16;

const UINT MAXSIZE = 256;

const UINT MINTARGET = 3; // 8888, 4444, 1555
const UINT MAXTARGET = 4; // 8888, 4444, 1555, X888

const UINT N8888 = 0;
const UINT N4444 = 1;
const UINT N1555 = 2;
const UINT NX888 = 3;

const UINT NCUBES  = 6;
const UINT NLEVELS = 9; // levels in mipmap from 256x256 to 1x1
const UINT NPLANES = 3;
const UINT NSTEPS  = 8;

const UINT NSTATUS = 15;

class C3DObject : public CShapes
{
    public:
        float m_fOrbitOffset; // radians
        float m_fOrbitPos;    // radians 

    public:
        C3DObject()
        {
            m_fOrbitOffset = 0.0f;
            m_fOrbitPos = 0.0f;
        };
        ~C3DObject()
        {
        };
};

class CScenario : public CD3DTest
{
    public:
        CHAR m_szBuffer[MAXBUFFER];             // text buffer
        CHAR m_szStatus[NSTATUS][2][MAXBUFFER]; // text buffer (status)
        D3DVALUE m_fGradientAlpha;              // gradient texture alpha override
        D3DVALUE m_fMipmapAlpha;                // mipmap texture alpha override
        DWORD m_dwMinLevel;                     // min mip level
        DWORD m_dwMaxLevel;                     // max mip level
        DWORD m_dwTest;                         // test number 
        INT m_pnTargetFormats[MAXTARGET];       // number of texture render target formats
        C3DObject m_pCubes[NCUBES];             // 3D object class array (cubes)
        CFlags m_Flags;                         // flags class
        CImage *m_pGradientImage;               // image class (gradient)
        CImage *m_pPlaneImage;                  // image class (plane)
        CImage *m_pSphereImage;                 // image class (sphere)
        CImage *m_pMipMapImages[NLEVELS];       // image class array (mipmap)
        CShapes m_Sphere;                       // shapes class (sphere)
        CShapes m_pPlanes[NPLANES];             // shapes class (plane)
        CnTexture *m_pGradientTexture;          // texture class (gradient)
        CnTexture *m_pMipMapTexture;            // texture class (mipmap)
        CnTexture *m_pPlaneTexture;             // texture class (plane)
        CnTexture *m_pSphereTexture;            // texture class (sphere)
        CnTexture *m_pTexTarget;                // texture class (texture render target)
        CnSurface *m_pTexZBuffer;               // surface class (texture render target z buffer)
        CnSurface *m_pDefTarget;                // surface class (default render target)
        CnSurface *m_pDefZBuffer;               // surface class (default render target z buffer)
        MATERIAL m_AlphaMaterial;               // alpha material
        MATERIAL m_DefaultMaterial;             // default material
        TESTVECTOR m_Tests;                     // test vector

    public:
        CScenario();
        ~CScenario();
        virtual VOID CommandLineHelp(VOID);
        virtual bool FilterResolution(PDISPLAYMODE pMode);
        virtual bool CapsCheck(VOID);
        virtual bool Setup(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ClearFrame(VOID);
        virtual bool ExecuteTest(UINT uiTest);
        virtual VOID SceneRefresh(VOID);
        virtual bool ProcessFrame(VOID);
        virtual VOID Cleanup(VOID);
        virtual VOID UpdateStatus(VOID);
        virtual VOID ClearStatus(VOID);
        virtual bool TestTerminate(VOID);
        bool SetupLights(VOID);
        bool SetupMaterials(VOID);
        bool SetupMatrices(VOID);
        bool SetupRenderStates(VOID);
        bool CreateImages(VOID);
        bool CreateShapes(VOID);
        bool CreateTextures(VOID);
        bool CreateTests(VOID);
        VOID DestroyTests(VOID);
        VOID DumpTests(VOID);
        VOID EliminateTests(DWORD dwFlags);
        VOID DescribeTest(DWORD dwTest, LPTSTR pszDesc);
        VOID WriteCurrentStatus(VOID);
        VOID InterpretTestResults(VOID);
        VOID ValidateTextureStageStates(VOID);
        bool ValidateTextureAsRenderTarget(bool *pbAbort, LPTSTR pszFormat);
        bool CheckRefreshRate(VOID);
        DWORD BitCount(DWORD dwMask);
        HRESULT ValidateDevice(VOID);
};
