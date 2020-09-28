//*****************************************************************************
//  CWGFTestUIDeviceWindow
//
//  The device window class implements the requirements for the render window
//  as well as the status information for the current device.
//
//  For core framework UI implementation notes, see Notes.TXT
//*****************************************************************************

#pragma once

#include "WGFTestUIWindow.h"

class CRenderWindow : public CWGFTestUIWindow
{
public:
	CRenderWindow() : m_hBitmap( NULL ) {}
	
	HBITMAP SetBitmap( HBITMAP hBitmap ) { HBITMAP hbmOld = m_hBitmap; m_hBitmap = hBitmap; return hbmOld; }
	
protected:
	LRESULT WindowProc( UINT uMsg, WPARAM wParam, LPARAM lParam );
	HBITMAP m_hBitmap;
};

class CWGFTestUIDeviceWindow : public CWGFTestUIWindow
{
	enum
	{
		MAX_TITLE =		512,
		MAX_DESC =		512,
	};
	
protected:

	TCHAR			m_szTitle[MAX_TITLE]; 
	TCHAR			m_szDescription[MAX_DESC];  
    HFONT           m_hTitleFont;
    HFONT           m_hDescFont;
    int             m_nTitleSizeY;
    int             m_nDescSizeY;
    HBRUSH          m_hBackgroundBrush;
    DWORD           m_dwFlags;
    CRenderWindow	m_RenderWindow;

public:

    CWGFTestUIDeviceWindow();
    ~CWGFTestUIDeviceWindow();

	DWORD    GetDeviceWindowFlags();
    bool     Create(UINT uWidth, UINT uHeight, CWGFTestUIWindow *pParent, DWORD dwFlags);
    
    HBITMAP SetBitmap( HBITMAP hBitmap ) { return m_RenderWindow.SetBitmap( hBitmap ); }

    
    //------------[ Base Class Overrides (CShellRenderWindow) ]------------

	virtual bool SetDeviceWindowFlags( DWORD dwFlags );
	virtual void SetDeviceTitle(LPCTSTR szTitle, LPCTSTR szDescription);
	virtual bool SetDeviceWindowSize(UINT uWidth, UINT uHeight);
	
    //------------------------[ Protected Methods ]------------------------        
    
protected:

    //------------------------[ Virtual Declarations ]---------------------

    virtual LRESULT WindowProc(UINT uMsg, WPARAM wParam, LPARAM lParam);
        
    friend class CWGFTestUIShell; 
};
