#include "DXGIWndClass.h"
#include <StrSafe.h>

#undef MSG_LEN
#define MSG_LEN     1024

const char * const  clsBasicWindowClass::m_szFocusWindowClassName = "FocusWindowClass";
HINSTANCE           clsBasicWindowClass::m_hProgInst = NULL;
bool                clsBasicWindowClass::m_bWindowClassRegistered = false;

clsBasicWindowClass::clsBasicWindowClass()
:
m_hMSCTF( NULL ),
m_bDisableLanguageBar( false ),
m_dwLangBarState( 0 )
{
}

clsBasicWindowClass::~clsBasicWindowClass()
{
}

LRESULT CALLBACK clsBasicWindowClass::WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    /*
    switch (message)
    {
    case WM_DESTROY:
    break;
    default:
    return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
    */

    return DefWindowProc(hwnd, message, wParam, lParam);
}

bool clsBasicWindowClass::InitializeWindowClass()
{
    char            szMsg[MSG_LEN] = {0};    

    if ( m_bDisableLanguageBar )
    {
        HideLanguageBar();
    }


    if ( !m_hProgInst 
        && !( m_hProgInst = GetModuleHandle(NULL) ) )
    {
        StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  FAILED: GetModuleHandle failed in InitializeWindowClass\n", __FILE__, __LINE__ );
        DebugOut.Write( szMsg );
        GetFramework()->WriteToLog( szMsg );
        return false;
    }

    if ( !m_bWindowClassRegistered )
    {
        WNDCLASS  wndclass = {0};

        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = (WNDPROC)(&WndProc);
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = m_hProgInst;
        wndclass.hIcon = LoadIcon( NULL, IDI_APPLICATION );
        wndclass.hCursor = LoadCursor( NULL, IDC_ARROW );
        wndclass.hbrBackground = (HBRUSH)GetStockObject( NULL_BRUSH );
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = m_szFocusWindowClassName;

        if ( !RegisterClass( &wndclass ) )
        {
            StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  FAILED: RegisterClass failed in InitializeWindowClass\n", __FILE__, __LINE__ );
            DebugOut.Write( szMsg );
            GetFramework()->WriteToLog( szMsg );
            return false;
        }

        m_bWindowClassRegistered = true;
    }

    return true;
}

void clsBasicWindowClass::CleanupWindowClass()
{
    if ( m_hProgInst )
    {
        if ( m_bWindowClassRegistered )
        {
            UnregisterClass( m_szFocusWindowClassName, m_hProgInst );
            m_bWindowClassRegistered = false;
        }

        m_hProgInst = NULL;
    }
    RestoreLanguageBar();
}

bool clsBasicWindowClass::HideLanguageBar()
{
    bool bRet = false;
    assert( m_hMSCTF == NULL );

    if
    (
        GetSystemMetrics( SM_IMMENABLED ) != 0
    )
    {
        char szMsg[MSG_LEN] = {0};    
        m_hMSCTF = LoadLibraryExW(L"msctf.dll", NULL, 0);

        if(m_hMSCTF == NULL)
        {
            StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  FAILED: Unable to load msctf.dll\n", __FILE__, __LINE__ );
            GetFramework()->WriteToLog( szMsg );
            return false;
        }
        PTF_CREATELANGBARMGR pfnCreateLangBarMgr;

        pfnCreateLangBarMgr = (PTF_CREATELANGBARMGR)GetProcAddress(m_hMSCTF, "TF_CreateLangBarMgr");

        if(pfnCreateLangBarMgr)
        {
            HRESULT hr;
            ITfLangBarMgr *pLangBarMgr;

            hr = (*pfnCreateLangBarMgr)(&pLangBarMgr);
            if(SUCCEEDED(hr))
            {
                // pLangBarMgr is a pointer of ITfLangBarMgr interface which is already obtained
                DWORD dwShow = TF_SFT_MINIMIZED;
                if ( pLangBarMgr->GetShowFloatingStatus(&m_dwLangBarState) != S_OK ) // get setting
                {
                    StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  FAILED: Unable to query Language Bar Status\n", __FILE__, __LINE__ );
                    m_dwLangBarState = (DWORD) 0 ;
                }
                // Still try to minimize even if GetShowFloatingStatus fails
                if ( pLangBarMgr->ShowFloating(dwShow) == S_OK ) // apply setting
                {	
                    StringCchPrintf( szMsg, sizeof(szMsg), "clsBasicWindowClass::HideLanguageBar() :  Language Bar minimized\n" );
                    bRet = true;
                }
                else
                {
                    StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  FAILED: Failed to minimize Language Bar\n", __FILE__, __LINE__ );
                }
                pLangBarMgr->Release();
            } 
            else
            {
                StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  FAILED: TF_CreateLangBarMgr\n", __FILE__, __LINE__ );
            }
        }
        GetFramework()->WriteToLog( szMsg );

    }

    return bRet;

}

bool clsBasicWindowClass::RestoreLanguageBar()
{
    bool bRet = true;

    if(m_hMSCTF != NULL)
    {
        if(m_dwLangBarState != 0x0)
        {
            PTF_CREATELANGBARMGR pfnCreateLangBarMgr;

            pfnCreateLangBarMgr = (PTF_CREATELANGBARMGR)GetProcAddress(m_hMSCTF, "TF_CreateLangBarMgr");

            if(pfnCreateLangBarMgr)
            {
                char            szMsg[MSG_LEN] = {0};    

                HRESULT hr;
                ITfLangBarMgr *pLangBarMgr;

                hr = (*pfnCreateLangBarMgr)(&pLangBarMgr);
                if(SUCCEEDED(hr))
                {
                    if ( pLangBarMgr->ShowFloating(m_dwLangBarState) == S_OK )
                    {
                        StringCchPrintf( szMsg, sizeof(szMsg), " clsBasicWindowClass::RestoreLanguageBar : Language Bar Restored\n" );
                    }
                    else
                    {
                        StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  Unable to Restore Language Bar\n", __FILE__, __LINE__ );
                        bRet = false;
                    }

                }
                else
                {
                    StringCchPrintf( szMsg, sizeof(szMsg), "%s at line %d.  Unable to Restore Language Bar\n", __FILE__, __LINE__ );
                    bRet = false;
                }					
                pLangBarMgr->Release();
                GetFramework()->WriteToLog( szMsg );

            }
        }
        FreeLibrary(m_hMSCTF);
        m_hMSCTF = NULL;
    }
    return bRet;
}