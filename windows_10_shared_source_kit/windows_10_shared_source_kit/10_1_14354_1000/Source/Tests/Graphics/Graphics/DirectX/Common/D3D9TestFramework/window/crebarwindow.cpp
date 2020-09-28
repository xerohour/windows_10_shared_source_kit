#include "WindowPCH.h"

#include "Framework.h"

// --------------------------------------------------------------------------------------

CRebarWindow::CRebarWindow()
{
}

// --------------------------------------------------------------------------------------

CRebarWindow::~CRebarWindow()
{
}

// --------------------------------------------------------------------------------------

bool CRebarWindow::AddBand(TCHAR *pcsText, DWORD dwStyle, int nWidth, HWND hChild, int nLoc)
{
	RECT r;
	REBARBANDINFO rbBand;
	UINT nStrWidth = 0;

	rbBand.cbSize = sizeof(REBARBANDINFO);
	rbBand.fMask = 0;

	if(pcsText && nWidth == -1)
	{
		rbBand.fMask |= RBBIM_TEXT;
		
		// Calculate the string space in pixels & add to the width
		SIZE s;
		HDC hDC = GetDC(m_hWindow);
		if(hDC)
		{
			GetTextExtentPoint32(hDC, pcsText, _tcslen(pcsText), &s);
			ReleaseDC(m_hWindow, hDC);
			nStrWidth += s.cx;
		}
	}
        
	if(dwStyle)
		rbBand.fMask |= RBBIM_STYLE;

	rbBand.fMask  |= RBBIM_CHILD | RBBIM_CHILDSIZE | RBBIM_SIZE;


	GetWindowRect(hChild, &r);
	rbBand.fStyle = dwStyle;
	rbBand.lpText = pcsText;
	rbBand.hwndChild = hChild;
	rbBand.cxMinChild = 0;
	rbBand.cyMinChild = r.bottom - r.top;
	rbBand.cx = nWidth == -1 ? (r.right - r.left) + nStrWidth : nWidth;

	SendMessage(m_hWindow, RB_INSERTBAND, (WPARAM)nLoc, (LPARAM)&rbBand);
	
	return true;
}

// --------------------------------------------------------------------------------------

void CRebarWindow::Close(void)
{
	// Detach all bands
	for(int i=0; i<m_nChildren; i++)
		SendMessage(m_hWindow, RB_DELETEBAND, (WPARAM)i, 0);

	CWindow::Close();
}

// --------------------------------------------------------------------------------------

void CRebarWindow::MoveWindow(int nX, int nY, UINT nWidth, UINT nHeight, bool bRepaint)
{
	// The children will moved by the control
	::MoveWindow(m_hWindow, nX, nY, nWidth, nHeight, bRepaint);
}
