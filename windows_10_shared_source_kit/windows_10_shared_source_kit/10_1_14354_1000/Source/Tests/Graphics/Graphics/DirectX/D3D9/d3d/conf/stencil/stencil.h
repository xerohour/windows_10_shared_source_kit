// FILE:        stencil.h
// DESC:        stencil class header for stencil conformance tests
// AUTHOR:      Todd M. Frost

#include <ddraw.h>
#include <d3d9.h>
#include "CD3DTest.h"
#include "CD3D.h"
#include "CFlags.h"

// legacy render state values taken from d3dtypes.h

#define D3DRS_STIPPLEENABLE     (D3DRENDERSTATETYPE) 39
#define D3DRS_STIPPLEPATTERN00  (D3DRENDERSTATETYPE) 64
#define D3DRS_STIPPLEPATTERN(y) (D3DRS_STIPPLEPATTERN00 + (y))

#define GLE(x) (m_pD3D->HResultToString(x))

const DWORD FLOOD_CW   = 0x00000001;
const DWORD FLOOD_CCW  = 0x00000002;
const DWORD FLOOD_BOTH = 0x00000003;

typedef struct _Args
{
    D3DCOLOR dwClearColor; // clear color
    DWORD dwStencil;       // stencil clear
    DWORD dwReference[2];  // reference masks
} ARGS, *PARGS;

typedef struct _FlexVert
{
    D3DVALUE fPx;
    D3DVALUE fPy;
    D3DVALUE fPz;
    D3DVALUE fRHW;
    D3DCOLOR dcDiffuse;
} FLEXVERT, *PFLEXVERT;

const DWORD FLEXFMT = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

const DWORD CF_VERBOSE  = 0x00000001;
const DWORD CF_VALIDATE = 0x00000002;
const DWORD CF_REVERSE  = 0x00000004;

const UINT MAXBUFFER = 128;

const UINT NMASKS = 2;

class CStencil: public CD3DTest
{
    public:
        CHAR m_szBuffer[MAXBUFFER];        // text buffer
        D3DCOLOR m_dwFailColor;            // fail color
        D3DCOLOR m_dwPassColor;            // pass color
        DWORD m_dwStencilBitDepth;         // stencil bit depth
        DWORD m_dwStencilBitDepthReported; // stencil bit depth reported
        DWORD m_dwStencilBitMask;          // stencil bit mask
        DWORD m_dwStencilCaps;             // stencil caps
        DWORD m_dwZCmpCaps;                // z compare caps
        UINT m_uRange;                     // test range
        CFlags m_Flags;                    // flags class

	public:
    	CStencil();
    	~CStencil();
        virtual VOID CommandLineHelp(VOID);
    	virtual bool ClearFrame(VOID);
        virtual bool BeginScene(D3DVALUE dvZ, DWORD dwStencil, DWORD dwColor, DWORD dwFlags);
        virtual bool SetTestRange(UINT uStart, UINT uEnd);
    	virtual bool CapsCheck(VOID);
    	virtual bool Setup(VOID);
    	virtual UINT TestInitialize(VOID);
    	virtual bool ExecuteTest(UINT uTest);
    	virtual VOID Cleanup(VOID);
        virtual bool CompareImages(CSurface *pSrc, CSurface *pRef, CSurface *pResult);
        virtual bool ProcessFrame(VOID);
        bool Compare(VOID);
        bool SetDefaultStencilState(VOID);
        bool SupportedOperation(DWORD dwOp);
        VOID Flood(D3DVALUE dvDepth, D3DCOLOR dwColor, DWORD dwFlags = FLOOD_CW);
        VOID Stipple(bool bEnable);
        VOID ProcessArgs(VOID);
};

class CStencilCap : public CStencil
{
    public:
        CStencilCap();
        ~CStencilCap();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CStencilClear : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilClear();
        ~CStencilClear();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID Test(bool bStipple, DWORD dwClear, DWORD dwMask, UINT uTest);
};

class CStencilClearCCW : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilClearCCW();
        ~CStencilClearCCW();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID Test(DWORD dwClear, DWORD dwMask, UINT uTest);
};

class CStencilCullOne : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilCullOne();
        ~CStencilCullOne();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID Test(bool bStipple, DWORD dwClear, DWORD dwMask, UINT uTest);
};

class CStencilCullTwo : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilCullTwo();
        ~CStencilCullTwo();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID Test(DWORD dwClear, DWORD dwMask, UINT uTest);
};

class CStencilFunc : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilFunc();
        ~CStencilFunc();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID Test(PARGS pArgs);
        LPSTR GetFunc(DWORD dwFunc);
};

class CStencilFuncCCW : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilFuncCCW();
        ~CStencilFuncCCW();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID Test(PARGS pArgs);
        LPSTR GetFunc(DWORD dwFunc);
};

class CStencilIgnore : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilIgnore();
        ~CStencilIgnore();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID Test(bool bStipple, DWORD dwClear, DWORD dwMask, UINT uTest);
};

class CStencilIgnoreCCW : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilIgnoreCCW();
        ~CStencilIgnoreCCW();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID Test(DWORD dwClear, DWORD dwMask, UINT uTest);
};

class CStencilOp : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilOp();
        ~CStencilOp();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID Test(DWORD dwState, DWORD dwOp);
        LPSTR GetOp(DWORD dwOp);
        LPSTR GetState(DWORD dwState);
};

class CStencilOpCCW : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilOpCCW();
        ~CStencilOpCCW();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
        VOID Test(DWORD dwState, DWORD dwOp);
        LPSTR GetOp(DWORD dwOp);
        LPSTR GetState(DWORD dwState);
};

class CStencilSCorrupt : public CStencil 
{
    public:
        UINT m_nStipple;

    public:
        CStencilSCorrupt();
        ~CStencilSCorrupt();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CStencilSCorruptCCW : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilSCorruptCCW();
        ~CStencilSCorruptCCW();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CStencilZCorrupt : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilZCorrupt();
        ~CStencilZCorrupt();
        virtual VOID CommandLineHelp(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};

class CStencilZCorruptCCW : public CStencil
{
    public:
        UINT m_nStipple;

    public:
        CStencilZCorruptCCW();
        ~CStencilZCorruptCCW();
        virtual VOID CommandLineHelp(VOID);
        virtual bool CapsCheck(VOID);
        virtual UINT TestInitialize(VOID);
        virtual bool ExecuteTest(UINT uTest);
};
