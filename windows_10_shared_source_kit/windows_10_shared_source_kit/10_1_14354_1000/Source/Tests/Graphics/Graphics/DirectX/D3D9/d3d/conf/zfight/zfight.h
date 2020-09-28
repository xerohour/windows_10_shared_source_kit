// FILE:        zfight.h
// DESC:        zfight class header for z fighting conformance test
// AUTHOR:      Todd M. Frost

#include <d3d9.h>
#include "CD3DTest.h"
#include "CD3D.h"
#include "CFlags.h"

#define GLE(x) (m_pD3D->HResultToString(x))

typedef struct FlexVert
{
    D3DVALUE fPx;
    D3DVALUE fPy;
    D3DVALUE fPz;
    D3DVALUE fRHW;
    D3DCOLOR dcDiffuse;
} FLEXVERT, *PFLEXVERT;

const DWORD FLEXFMT = (D3DFVF_XYZRHW | D3DFVF_DIFFUSE);

const DWORD CF_STATIC    = 0x00000001; // static vertex orientation
const DWORD CF_WIREFRAME = 0x00000002; // wireframe mode
const DWORD CF_TRIFAN    = 0x00000010; // static primitive type (triangle fan)
const DWORD CF_TRILIST   = 0x00000020; // static primitive type (triangle list)
const DWORD CF_TRISTRIP  = 0x00000040; // static primitive type (triangle strip)
const DWORD CF_TRITYPES  = 0x00000070; // primitive type mask

#ifdef FAST_TEST
const DWORD PDEF = 2; // default number of passes
#else
const DWORD PDEF = 4; // default number of passes
#endif
const DWORD PMIN = 1; // min     number of passes
const DWORD PMAX = 4; // max     number of passes

const UINT MAXBUFFER = 128;

const UINT NSTATUS = 2;

class CZFight: public CD3DTest
{
    private:
        CHAR m_szBuffer[MAXBUFFER];             // text buffer
        CHAR m_szStatus[NSTATUS][2][MAXBUFFER]; // text buffer (status)
        D3DVALUE m_fDistance;                   // z distance
        DWORD m_dwPasses;                       // passes
        UINT m_nAPI;                            // potential API changes
        CFlags m_Flags;                         // flags class
        
    public:
        CZFight();
        ~CZFight();
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
