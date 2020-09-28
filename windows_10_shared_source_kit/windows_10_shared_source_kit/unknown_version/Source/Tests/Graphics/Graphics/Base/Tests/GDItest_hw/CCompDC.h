//Classes for HDC and BITMAP with particular format, e.g. 32 bpp.
//Each instance of CCompatibleDC holds one CCompatiableBitmap member

#include <stdlib.h>
#include "stdafx.h"
#include "stdio.h"
#include "strsafe.h"
#include <stddef.h>
#include <math.h>
#include <dxgi.h>
#include <shlwapi.h>

#define BFT_BITMAP 0x4d42 // 'BM'

void RemoveMessage(HWND hWnd);

struct ERROR_INFO
{
	int	NumofColorError;
	int	MaxInteriorError;
	int	AveRedError;
	int	MaxRedError;
	int	AveGreenError;
	int	MaxGreenError;
	int	AveBlueError;
	int	MaxBlueError;
	int	AveTotalError;
	int	MaxPixelX;
	int	MaxPixelY;
	int RedValue;
	int GreenValue;
	int BlueValue;
	int NumofPixelwithTextCol;
	int	NumofMismatchedPixel;
	int NumofInteriorColorError;
};

class CCompatibleBitmap
{
public:
	CCompatibleBitmap(HDC hDC, UINT Width, UINT Height, BOOL bDeviceBitmap);
	~CCompatibleBitmap();
	BOOL IsValid()  const {return m_bValid;}
	HBITMAP GetBitmap() const {return m_hBitmap;}
	BYTE* GetBits() const {return m_pBitmapBits;}
	UINT GetPitch() const {return m_Width * 4;}
	UINT GetWidth() const {return m_Width;}
	UINT GetHeight() const {return m_Height;}
	BITMAPINFO GetBMI() const {return m_bmi;}
	BOOL SaveBitmap(TCHAR* FileName);
		
protected:
	BOOL    m_bValid;
	HBITMAP m_hBitmap;
	BYTE*   m_pBitmapBits;
	UINT    m_Width;
	UINT    m_Height;
	BITMAPINFO m_bmi;
};
//-----------------------------------------------------------------------------


//-----------------------------------------------------------------------------
class CCompatibleDC
{
public:
	CCompatibleDC::CCompatibleDC(HWND hWnd, HDC hDC, BOOL bDeviceBitmap);
	~CCompatibleDC();
	HDC GetScreenDC() {return m_hScreenDC;}
	HDC GetMemoryDC() {return m_hMemoryDC;}
	BOOL IsValid()  const {return m_bValid;}
	BOOL Initialize(HBRUSH hBrush);
	BOOL GetInteriorPixels(COLORREF TextCol);
	BOOL SyncBitmapDC(BOOL bUseDeviceBitmap);
	BOOL IsUsingDeviceBitmap(){ return m_bDeviceBitmap;}
	BOOL CompareDC();
	BOOL CompareDC(TCHAR* ScenarioName, BOOL bTolerance);
	BOOL CompareDC(TCHAR* ScenarioName, BOOL bTolerance, COLORREF TextCol, UINT iTolerance);
	HGDIOBJ Select(HGDIOBJ hObj);
	COLORREF SetTextCol(COLORREF crColor);
	COLORREF SetBKCol(COLORREF crColor);
	int SetBKMode(int mode);
	BOOL DoTextout();
	BOOL DoBitBlt(HDC hTempDC, UINT BitmapSizeX, UINT BitmapSizeY, DWORD Rop, float factor);
	BOOL DoColorFill(UINT BitmapSizeX, UINT BitmapSizeY, DWORD Rop, float factor);
	BOOL DoAlphaBlend(HDC hTempDC, UINT BitmapSizeX, UINT BitmapSizeY, BLENDFUNCTION Blend, BOOL bDoMath);
	BOOL DoTransparentBlt(HDC hTempDC, UINT Width, UINT Height, UINT ColorKey);
	BOOL DoStretchBlt(HDC hTempDC, UINT Width, UINT Height, float factor);
	BOOL DoLineTo(UINT nTheme);
	BOOL DoStrokePath(UINT nTheme);
	BOOL DoMaskBlt(HDC hSrcDC, UINT BitmapSizeX, UINT BitmapSizeY, HBITMAP hMskBmp, DWORD fRop, DWORD bRop);
	BOOL SelectRgn(HRGN hRgn, int fnMode);
	BOOL SelectRgn(HRGN hRgn);
	BOOL ExcludeRect(int nL, int nT, int nR, int nB);
	BOOL IntersectRect(int nL, int nT, int nR, int nB);
	void SetMaxErrorValue( int eValue );

	ERROR_INFO ErrorInfo;
protected:
	BOOL m_bValid;
	UINT m_Width;
	UINT m_Height;
	HWND m_hWnd;
	HDC m_hDC;
	HDC m_hScreenDC;
	HDC m_hMemoryDC;
	HDC m_hDeviceBitmapDC;
	BITMAPINFO m_bmi;
	CCompatibleBitmap* m_pScreenBitmap;
	CCompatibleBitmap* m_pMemoryBitmap;
	CCompatibleBitmap* m_pDiffBitmap;
	CCompatibleBitmap* m_pDeviceBitmap;
	BYTE* m_pTemplateBits;
	BOOL m_bDeviceBitmap;
	int m_iMaxError;
};
