
#include "Stdafx.h"
#include "common.h"

BOOL g_fOwner       = TRUE;
BOOL g_fWindowed    = TRUE;
IUserBase * g_pUser = NULL;


void ParseCmdLine(LPSTR cmdLine)
{
    char seps[]   = "/,= ";
    char *token;
    token = strtok(cmdLine,seps); 

    while(token != NULL)
    {
        if(strcmp(_strlwr(token), "user") == 0)
        {
            g_fOwner = FALSE;
        }
        else if(strcmp(_strlwr(token), "fs") == 0)
        {
            g_fWindowed = FALSE;
        }

        token = strtok(NULL, seps);
    }
}

//
// CodePathStep 2:
//
//  Here we're either going to create a COwner or CUser object depending on what the
// command line said (discovered in ParseCmdLine()). The first time through, it 
// should be COwner. In both cases, if the object is successfully created, we'll
// call the appropriate Initialize() routine and then Run(). 
//
// CodePathStep 8:
//
//  Second time through, we create the CUser object and call it's Initialize() and 
// Run() routines. 
int Go(HWND hWnd)
{
    if(g_fOwner == TRUE)
    {
        DBG_MSG(L"creating owner");
        g_pUser = new COwner(hWnd);
    }
    else
    {
        DBG_MSG(L"creating user");
        g_pUser = new CUser(hWnd);
    }

    if(g_pUser)
    {
        DBG_MSG(L"Calling Initialize()");
        if(SUCCEEDED(g_pUser->Initialize(g_fWindowed)))
        {
            DBG_MSG(L"Calling Run()");
            g_pUser->Run();
            DBG_MSG(L"back from Run()");
        }
    }
    return 0;
}

//
// CodePathStep 1: 
//
//  This is the entry point for CrossProc. Upon entry we look to see what our command
// line is. If it contains the string " user" we know CrossProc launched this instance.
// Note that ParseCmdLine() will change g_fOwner to false if " user" is found.
// The key work is done in Go().
//
// CodePathStep 7:
//
//  When the COwner object has processed to the point of running the test, it called
// CreateUserProcess() which brings us here a second time. But this time, g_fOwner
// is set false in the ParseCmdLine() routine. Thus, we're going to create the 
// CUser object.
int WINAPI WinMain(HINSTANCE hInstance,
            HINSTANCE hPrevInstance,
            LPSTR lpCmdLine,
            int nCmdShow)
{
    DBG_MSG(L"Enter");
    ParseCmdLine(lpCmdLine);

    //Window creation
    HWND hWnd = ::CreateWindow(L"Static",
                               g_fOwner?L"CrossProc Owner":L"CrossProc User",
                               WS_VISIBLE | WS_POPUP | WS_CAPTION,
                               g_fOwner?0:WIDTH,
                               0,
                               WIDTH,
                               HEIGHT,
                               NULL,
                               NULL,
                               hInstance,
                               NULL);

    if(NULL == hWnd)
    {
        DBG_MSG(L"Leave with error");
        return 0;
    }    
    
    try
    {
        DBG_MSG(L"calling Go()");
        Go(hWnd);   
        DBG_MSG(L"back from Go()");
    }
    catch(...)
    {
        cout << "Caught an unknown exception at the highest level\n";
        PATH_TRAP();
    }

    if(NULL != g_pUser)
    {
        g_pUser->Cleanup();
        delete g_pUser;
        g_pUser = NULL;
    }
    DestroyWindow(hWnd);
    DBG_MSG(L"Leave");
    return 1;
}