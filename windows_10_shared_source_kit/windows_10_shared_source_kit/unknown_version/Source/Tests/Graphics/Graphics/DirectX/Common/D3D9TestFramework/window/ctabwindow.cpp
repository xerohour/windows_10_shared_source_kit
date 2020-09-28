#include "WindowPCH.h"

#include "Framework.h"

// --------------------------------------------------------------------------------------

CTabWindow::CTabWindow()
{
    m_nSelectedTab = 0;
}

// --------------------------------------------------------------------------------------

bool CTabWindow::AddChild(TCHAR *pcsName, CWindow *pChild)
{
	if ( pChild == NULL || m_nChildren >= MAX_CHILDREN )
	{
		return false;
	};


	TCITEM tie;
	memset(&tie, 0, sizeof(TCITEM));
	tie.mask = TCIF_TEXT; 
	tie.iImage = -1; 
	tie.pszText = ( pcsName == NULL ) ? _T("No Name Given") : pcsName;

	TabCtrl_InsertItem(m_hWindow, m_nChildren, &tie);
	
	return CWindow::AddChild(pcsName, pChild);
}

// --------------------------------------------------------------------------------------

void CTabWindow::SelectTab(UINT nIndex)
{
	if(m_nSelectedTab < m_nChildren)
		m_pChildren[m_nSelectedTab]->SetVisible(false);
	if(nIndex < m_nChildren)
	{
		m_pChildren[nIndex]->Update();
		m_pChildren[nIndex]->SetVisible(true);
		m_nSelectedTab = nIndex;
	}
}

// --------------------------------------------------------------------------------------

void CTabWindow::Move(UINT nX, UINT nY, UINT nWidth, UINT nHeight)
{
	RECT r1, r2;
	 
	CWindow::Move(nX, nY, nWidth, nHeight);

	// Calculate the client area
	GetClientRect(m_hWindow, &r1);
	GetClientRect(m_hWindow, &r2);
	// In/Out to find out the display area
	TabCtrl_AdjustRect(m_hWindow, FALSE, &r1);

	nX = (r1.left - r2.left);
	nY = (r1.top - r2.top);
    nWidth = r1.right - r1.left;
	nHeight = r1.bottom - r1.top;

	for(int i=0; i<m_nChildren; i++)
	{
		m_pChildren[i]->Move(nX, nY, nWidth, nHeight);
	}
}

// --------------------------------------------------------------------------------------

void CTabWindow::Move(UINT nX, UINT nY)
{
	RECT r1, r2;
	 
	CWindow::Move(nX, nY);

	// Calculate the client area
	GetClientRect(m_hWindow, &r1);
	GetClientRect(m_hWindow, &r2);
	// In/Out to find out the display area
	TabCtrl_AdjustRect(m_hWindow, FALSE, &r1);

	nX = (r1.left - r2.left);
	nY = (r1.top - r2.top);

	for(int i=0; i<m_nChildren; i++)
	{
		m_pChildren[i]->Move(nX, nY);
	}
}

// --------------------------------------------------------------------------------------

void CTabWindow::Resize(UINT nWidth, UINT nHeight)
{
	RECT r1, r2;
	    
	CWindow::Resize(nWidth, nHeight);

	// Calculate the client area
	GetClientRect(m_hWindow, &r1);
	GetClientRect(m_hWindow, &r2);
	// In/Out to find out the display area
	TabCtrl_AdjustRect(m_hWindow, FALSE, &r1);

	nWidth = r1.right - r1.left;
	nHeight = r1.bottom - r1.top;

	for(int i=0; i<m_nChildren; i++)
	{
		m_pChildren[i]->Resize(nWidth, nHeight);
	}
}

// --------------------------------------------------------------------------------------

void CTabWindow::OnNotify(WPARAM wParam, LPARAM lParam)
{
	NMHDR *nm = (NMHDR*)lParam;

	switch(nm->code)
	{
	case TCN_SELCHANGE:
		// Select the new tab
		SelectTab(TabCtrl_GetCurSel(m_hWindow));
		// Force a repaint
        SendMessage(m_pChildren[m_nSelectedTab]->m_hWindow, WM_NCPAINT, 1, 0);
        InvalidateRect(m_pChildren[m_nSelectedTab]->m_hWindow,NULL,FALSE);
        UpdateWindow(m_pChildren[m_nSelectedTab]->m_hWindow);
		break;
	default:
		break;
	}
}
