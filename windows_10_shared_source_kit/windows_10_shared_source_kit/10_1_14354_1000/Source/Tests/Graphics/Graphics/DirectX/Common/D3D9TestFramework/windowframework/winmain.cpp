#include "WindowFrameworkPCH.h"

#include "Framework.h"

// Declare the app interface
CD3DWindowFramework * g_pFramework = NULL;

void DeclareFramework(CD3DWindowFramework * pNew)
{
    g_pFramework = pNew;
}

// Main application startup code
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPTSTR szCmdline, int nShow)
{
    g_pFramework->m_hInstance = hInst;

    return g_pFramework->Execute(nShow);
}

