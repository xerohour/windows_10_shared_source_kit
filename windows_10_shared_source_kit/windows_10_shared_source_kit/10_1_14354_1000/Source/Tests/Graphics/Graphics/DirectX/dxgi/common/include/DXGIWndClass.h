#ifndef DXGI_COMMON_WNDCLASS_INCLUDED
#define DXGI_COMMON_WNDCLASS_INCLUDED

#include "WGFTestCore.h"
#include "WGFTestUI.h"
#include <Ctfutb.h>

typedef HRESULT (WINAPI *PTF_CREATELANGBARMGR)(ITfLangBarMgr**);

class clsBasicWindowClass
{
	public:
		clsBasicWindowClass();
		virtual ~clsBasicWindowClass();
		void SetDisableLanguageBar(bool bDisable){ m_bDisableLanguageBar = bDisable;}

	protected:
		static const char * const   m_szFocusWindowClassName;
		static HINSTANCE            m_hProgInst;
		static bool                 m_bWindowClassRegistered;
		HMODULE						m_hMSCTF;
		bool						m_bDisableLanguageBar;
		DWORD						m_dwLangBarState;

		static LRESULT CALLBACK  WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);

		bool InitializeWindowClass();
		void CleanupWindowClass();		
		
		bool HideLanguageBar();
		bool RestoreLanguageBar();
};

#endif
