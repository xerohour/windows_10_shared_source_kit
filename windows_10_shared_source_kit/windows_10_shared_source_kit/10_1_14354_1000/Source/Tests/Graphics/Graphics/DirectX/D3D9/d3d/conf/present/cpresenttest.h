//
//  CPresentTest.h
//
//  Class specification for CPresentTest.
//
//  ---------------------------------------------------------------------------
//
//  Copyright (C) 2000 Microsoft Corporation.
//

#ifndef __CPresentTest_H__
#define __CPresentTest_H__


#define TESTDXVER 0x0800

#define STRICT
#include <d3d8.h>
#include <d3dx8.h>
#include <tchar.h>
#include <windows.h>
#include <types.h>
#include <CD3DTest.h>
#include <TestUtils.h>

#include <CShapes.h>


//
// fun macros
//


#define COUNTOF(a) (sizeof(a)/sizeof((a)[0]))


typedef struct __RECT_SCALE
{
    FLOAT left;
    FLOAT top;
    FLOAT right;
    FLOAT bottom;
}
RECT_SCALE;

typedef struct __D3DTEST_RECT
{
    LPTSTR szName;
    BOOL bScale;
    RECT_SCALE *pRectScale;
    RECT *pRect;
}
D3DTEST_RECT;

typedef struct __D3DTEST_PRESENT
{
    LPTSTR szName;
    BOOL bScaleSrc;
    RECT_SCALE *pSrcRectScale;
    RECT *pSrcRect;
    BOOL bScaleDest;
    RECT_SCALE *pDestRectScale;
    RECT *pDestRect;
    BOOL bDestWndOverride;
    HWND hDestWindowOverride;
    RGNDATA *pDirtyRegion;
}
D3DTEST_PRESENT;


typedef union __MYCOLOR
{
    struct
    {
        BYTE blue;
        BYTE green;
        BYTE red;
        BYTE alpha;
    };
    DWORD color;
}
MYCOLOR;


//
//  The CDevice8Test specification
//


class CPresentTest : public CD3DTest
{
private:
    LPTEXTURE m_pTex;
    LPVERTEXBUFFER m_pVB;

    HWND m_hWnd;
	HWND m_hWndPresent;
    HWND m_hWndOverride;
    COLORREF m_FBColor;

	bool m_bDontHideCursor;

	float m_fDPIFactor;

    UINT m_ScreenWidth;
    UINT m_ScreenHeight;

    UINT m_SrcWidth;
    UINT m_SrcHeight;
    UINT m_DestWidth;
    UINT m_DestHeight;

    RECT *m_pSrcRect;
    RECT *m_pDestRect;
    RECT m_rcDestVis;
    HWND m_hDestWnd;
    HWND m_hDestParent;
    RGNDATA *m_pDirtyRegion;

public:

    CPresentTest();

    UINT TestInitialize();
    bool TestTerminate();
    bool ExecuteTest(UINT nTest);
    bool DisplayFrame(void);
    bool ProcessFrame(void);
    BOOL ValidateRects( const RECT *pSrc, const RECT *pDest );
    bool CompareRangePixel( MYCOLOR SrcMin, MYCOLOR SrcMax, MYCOLOR Dest, FORMAT Format, MYCOLOR *Diff );
    bool CompareSurfaces( LOCKED_RECT *pSrcLocked, LOCKED_RECT *pDestLocked, FORMAT Format );
};



#endif

