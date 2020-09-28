//
// Common1.cpp
//
// Common Methods #1 with standard #includes
//

#include <d3dx9.h>
#include <dxgi.h>
#include <StrSafe.h>
#include <psapi.h>
#include "WGFTestCore.h"
#include "Common.hpp"


//////////////////////////////////////////////////////////////////////////////
//
// Class Definitions
//
//////////////////////////////////////////////////////////////////////////////

//
BOOL  CCommon::GetGDIDevMode( DWORD dwDevNum, DISPLAY_DEVICE& currentDispDev, DEVMODE& currentDevMode )
{
    BOOL            bReturn = TRUE;
    TCHAR           szMsg[c_nMSG_LEN] = {0};    

    memset( &currentDispDev, 0, sizeof(currentDispDev) );
    currentDispDev.cb = sizeof(currentDispDev);
    memset( &currentDevMode, 0, sizeof(currentDevMode) );
    currentDevMode.dmSize = sizeof(currentDevMode);

    bReturn = EnumDisplayDevices( NULL, dwDevNum, &currentDispDev, 0 );

    if ( bReturn == FALSE && dwDevNum == 0 )
    {
        bReturn = FALSE;
        StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  FAILED: EnumDisplayDevices() on Primary returned FALSE\n", __FUNCTION__, __FILE__, __LINE__ );
        WriteMsgs( szMsg, sizeof(szMsg) );
    }

    if ( bReturn == TRUE 
        && !(currentDispDev.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) )
    {
        if ( currentDispDev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) 
        {
            if ( ( bReturn = EnumDisplaySettings( currentDispDev.DeviceName, ENUM_CURRENT_SETTINGS, &currentDevMode ) ) == FALSE )
            {
                bReturn = FALSE;
                StringCchPrintf( szMsg, sizeof(szMsg),
                                 "%s in %s at line %d.  FAILED: EnumDisplaySettings() for device %s returned FALSE\n",
                                 __FUNCTION__, __FILE__, __LINE__,
                                 currentDispDev.DeviceName
                               );
                WriteMsgs( szMsg, sizeof(szMsg) );
            }

            StringCchCopy( (STRSAFE_LPSTR)&currentDevMode.dmDeviceName[0], sizeof(currentDevMode.dmDeviceName), currentDispDev.DeviceName );
        }
        else
        {
            if ( ( bReturn = EnumDisplaySettings( currentDispDev.DeviceName, ENUM_REGISTRY_SETTINGS, &currentDevMode ) ) == FALSE )
            {
                bReturn = FALSE;
                StringCchPrintf( szMsg, sizeof(szMsg),
                                 "%s in %s at line %d.  FAILED: EnumDisplaySettings() for device %s returned FALSE\n",
                                 __FUNCTION__, __FILE__, __LINE__,
                                 currentDispDev.DeviceName
                               );
                WriteMsgs( szMsg, sizeof(szMsg) );
            }

            StringCchCopy( (STRSAFE_LPSTR)&currentDevMode.dmDeviceName[0], sizeof(currentDevMode.dmDeviceName), currentDispDev.DeviceName );
        }
    }
    else
    {
        bReturn = FALSE;
    }

    return bReturn;
}

//
BOOL
CCommon::GetGDIDevMode(
    __in_ecount(nDevNameLen)  __nullterminated const char*  pszDevName,
    __in    UINT            nDevNameLen,
    __inout DEVMODE&        currentDevMode  // by ref
   )
{
    BOOL            bReturn = FALSE;
    TCHAR           szMsg[c_nMSG_LEN] = {0};    
    size_t          nLen = 0;
          
    // Validate parameters
    //
    if ( pszDevName == NULL || *pszDevName == 0 || pszDevName[nDevNameLen - 1] != 0 )
    {
        return FALSE;
    }

    memset( &currentDevMode, 0, sizeof(currentDevMode) );
    currentDevMode.dmSize = sizeof(currentDevMode);

    if ( ( bReturn = EnumDisplaySettings( pszDevName, ENUM_CURRENT_SETTINGS, &currentDevMode ) ) == FALSE )
    {
        StringCchPrintf( szMsg, sizeof(szMsg),
                        "%s in %s at line %d.  FAILED: EnumDisplaySettings() for device %s returned FALSE\n",
                        __FUNCTION__, __FILE__, __LINE__,
                        pszDevName
                       );
        WriteMsgs( szMsg, sizeof(szMsg) );
    }

    return bReturn;
}

//
int  CCommon::SetGDIDevMode( DISPLAY_DEVICE& newDispDev, DEVMODE& newDevMode )
{
    TEST_RESULT     test_result = RESULT_PASS;   
    LONG            lReturn = 0;
    TCHAR           szMsg[c_nMSG_LEN] = {0};    


    newDevMode.dmFields = DM_BITSPERPEL | DM_PELSWIDTH | DM_PELSHEIGHT | DM_DISPLAYFLAGS | DM_DISPLAYFREQUENCY | DM_POSITION | DM_DISPLAYORIENTATION;

    if ( ( lReturn = ChangeDisplaySettingsEx( newDispDev.DeviceName, &newDevMode, NULL, CDS_RESET | CDS_UPDATEREGISTRY, NULL ) ) != DISP_CHANGE_SUCCESSFUL )
    {
        // If BADMODE is returned, the monitor is probably not plugged in and therefore is not attached to the Desktop.
        if ( lReturn == DISP_CHANGE_BADMODE
            && !(newDispDev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) )
            ;       // ignore
        else
        {
            test_result = RESULT_FAIL;
            StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  FAILED: ChangeDisplaySettingsEx() returned 0x%08x\n", __FUNCTION__, __FILE__, __LINE__, lReturn );
            WriteMsgs( szMsg, sizeof(szMsg) );
        }
    }

    return test_result;
}

//
void  CCommon::GetFocusForFullscreen( HWND hwnd )
{
    BOOL            bOK = FALSE;    // This is only defined to make debugging easier.
    DWORD           PID = ::GetCurrentProcessId();
    HWND            hwndForegroundWindow = ::GetForegroundWindow();
    HWND            hwndPrevForegroundWindow = NULL;
    int             nCount = 0;


    // Try to bring the window to the top.
    while ( hwnd != hwndForegroundWindow && nCount++ < 5 )
    {
        hwndPrevForegroundWindow = hwndForegroundWindow;
        ::BringWindowToTop( hwnd );    

        if ( ( bOK = ::AllowSetForegroundWindow( PID ) ) == TRUE )
        {
            ::SetForegroundWindow( hwnd );
        }
        else
        {   
            // Try moving the topmost window to the bottom of the Z-order.
            ::SetWindowPos( hwndForegroundWindow, HWND_BOTTOM, 0, 0, 0, 0, SWP_ASYNCWINDOWPOS | SWP_NOACTIVATE | SWP_NOMOVE | SWP_NOREDRAW | SWP_NOSIZE );
        }

        hwndForegroundWindow = ::GetForegroundWindow();

        // Try clicking on the title bar if no change has yet occurred.
        if ( hwndForegroundWindow == hwndPrevForegroundWindow )
        {
            RECT    rc = {0};
            POINT   pt = {0};
            INPUT   input = {0};

            // Find location of title bar in mouse coordinates.
            ::GetWindowRect( hwnd, &rc );
            pt.x = ( rc.left + 50 ) * 65535 / GetSystemMetrics( SM_CXSCREEN );
            pt.y = ( rc.top + 15 ) * 65535 / GetSystemMetrics( SM_CYSCREEN );

            input.type = INPUT_MOUSE;
            input.mi.dx = pt.x;
            input.mi.dy = pt.y;
            input.mi.mouseData = 0;
            input.mi.dwFlags = MOUSEEVENTF_ABSOLUTE | MOUSEEVENTF_MOVE;

            // Move the mouse to the title bar.
            ::SendInput( 1, &input, sizeof(INPUT) );

            // Process outstanding messages.
            PumpWindowMsgs( hwnd, FALSE, 0 );

            hwndForegroundWindow = ::GetForegroundWindow();
        }
    }

    return;
}

//
void
CCommon::WriteMsgs(
    __in_ecount(nMsgLen)  const PCHAR  pszMsg,
    __in    UINT            nMsgLen
   )
{
    DebugOut.Write( pszMsg );
    GetFramework()->WriteToLog( pszMsg );
}

//
BOOL
CCommon::IsFullscreen(
    __in    HWND hwndDeviceWindow,
    __in    D3DDISPLAYMODEEX& dispModeEx    // by ref
  )
{
    LONG        lCurrentStyle = 0;
    RECT        rc = {0};

    GetWindowRect( hwndDeviceWindow, &rc );
    lCurrentStyle = GetWindowLong( hwndDeviceWindow, GWL_STYLE );

    if ( (UINT)rc.right == dispModeEx.Width
        && (UINT)rc.bottom == dispModeEx.Height )
    {
        return TRUE;
    }

    if ( lCurrentStyle & WS_MINIMIZE )
    {
        return FALSE;
    }

    return FALSE;
}

//
void
CCommon::SendAltTab(
    __in    HWND            hwndDevWnd
    )
{
    SetFocus( hwndDevWnd );
    PumpWindowMsgs( hwndDevWnd, TRUE, 70 );
    
    // Send ALT-TAB keystrokes.
	keybd_event( VK_MENU, 0, 0, 0 );
	keybd_event( VK_TAB, 0, 0, 0 );
    PumpWindowMsgs( hwndDevWnd, TRUE, 70 );

	keybd_event( VK_TAB, 0, KEYEVENTF_KEYUP, 0 );
	keybd_event( VK_MENU, 0, KEYEVENTF_KEYUP, 0 );
    PumpWindowMsgs( hwndDevWnd, TRUE, 70 );
    Sleep(200); 

    return;
}

//
void
CCommon::SendAltEnter(
    __in    HWND            hwndDevWnd
    )
{
    
    ::SetForegroundWindow( hwndDevWnd );

    // Send ALT-ENTER keystrokes.
	keybd_event( VK_MENU, 0, 0, 0 );
	keybd_event( VK_RETURN, 0, 0, 0 );
	keybd_event( VK_RETURN, 0, KEYEVENTF_KEYUP, 0 );
	keybd_event( VK_MENU, 0, KEYEVENTF_KEYUP, 0 );

    PumpWindowMsgs( hwndDevWnd, TRUE, 70 );

    return;
}

//
void
CCommon::PumpWindowMsgs(
    __in    HWND            hwnd,
    __in    BOOL            fCallSleep,
    __in    UINT            nSleepTime  
   )
{
    MSG     msg = {0};

    while (PeekMessage( &msg, hwnd, 0, 0, PM_REMOVE ))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    
        if ( fCallSleep && (int)nSleepTime >= 0 )
        {
            Sleep( nSleepTime );
        }
    }

    return;
}

//
void
CCommon::LogTestAppInfo()
{
    DWORD               dwCount = 0;
    TCHAR               tszExeNameAndPath[MAX_PATH] = {0};
    FILEIDENTITYINFO    FileInfo = {0};
    TCHAR               szMsg[c_nMSG_LEN] = {0};    


    dwCount = GetModuleFileNameEx(
                                  GetCurrentProcess(),
                                  GetModuleHandle(NULL), 
                                  tszExeNameAndPath,
                                  sizeof(tszExeNameAndPath)/sizeof(tszExeNameAndPath[0])
                                 );

    if ( dwCount && tszExeNameAndPath[0] )
    {
        StringCchPrintf( szMsg, sizeof(szMsg), "TestApp: %s\n", tszExeNameAndPath );
        WriteMsgs( szMsg, sizeof(szMsg) );

        if ( GetFileInfo( tszExeNameAndPath, &FileInfo ) )
        {
            StringCchPrintf( szMsg, sizeof(szMsg), "%s\n", FileInfo.tszFileIdentityInfo );
            WriteMsgs( szMsg, sizeof(szMsg) );
        }
    }

    return;
}

//
void  CCommon::AttachScreensAndSetRotationsToZero()
{
    LONG            lReturn = 0;
    TCHAR           szMsg[c_nMSG_LEN] = {0};    
    DISPLAY_DEVICE  dispDev = {0};
    DEVMODE         devMode = {0};


    // Get the current mode and if necessary set the orientation to the default of 0.
    // Then get the current mode again and save it.
    for ( DWORD dwDevNum = 0; (int)dwDevNum < MONITORS_MAX; dwDevNum++ )
    { 
        if ( GetGDIDevMode( dwDevNum, dispDev/* by ref */, devMode/* by ref */ ) )
        {
            // If the monitor is not attached to the Desktop, attach it.
            if ( !(dispDev.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) )
            {
                // The positioning of the monitor does not have to be exact.  The OS
                // will adjust the virtual coordinates to be valid and also to prevent any gaps between
                // screens in virtual space.
                devMode.dmFields = DM_POSITION;
                devMode.dmPosition.x = 2048;
                devMode.dmPosition.y = 0;

                // CDS_UPDATEREGISTRY
                if ( ( lReturn = ChangeDisplaySettingsEx( dispDev.DeviceName, &devMode, NULL, CDS_NORESET | CDS_UPDATEREGISTRY, NULL ) ) != DISP_CHANGE_SUCCESSFUL )
                {
                    StringCchPrintf( szMsg, sizeof(szMsg),
                                     "%s in %s at line %d.  WARNING: ChangeDisplaySettingsEx() returned %d and could not attach %s to Desktop\n",
                                     __FUNCTION__, __FILE__, __LINE__,
                                     lReturn,
                                     dispDev.DeviceName
                                   );
                    WriteMsgs( szMsg, sizeof(szMsg) );
                    continue;
                }

                // CDS_RESET all monitors
                if ( ( lReturn = ChangeDisplaySettings( NULL, CDS_RESET ) ) != DISP_CHANGE_SUCCESSFUL )
                {
                    StringCchPrintf( szMsg, sizeof(szMsg),
                                     "%s in %s at line %d.  WARNING: ChangeDisplaySettingsEx() returned %d and could not attach %s to Desktop\n",
                                     __FUNCTION__, __FILE__, __LINE__,
                                     lReturn,
                                     dispDev.DeviceName
                                   );
                    WriteMsgs( szMsg, sizeof(szMsg) );
                    continue;
                }
            }
            
            devMode.dmFields = 0;

            if ( devMode.dmDisplayOrientation != 0 )
            {
                // If the orientation is 1 (90 deg.) or 3 (270 deg.)
                // the width/height values need to be swopped.
                if ( devMode.dmDisplayOrientation == 1 || devMode.dmDisplayOrientation == 3 )
                {  
                    DWORD  dwTemp = devMode.dmPelsWidth;
                    devMode.dmPelsWidth = devMode.dmPelsHeight;
                    devMode.dmPelsHeight = dwTemp;
                    devMode.dmFields |= DM_PELSWIDTH;
                    devMode.dmFields |= DM_PELSHEIGHT;
                }

                devMode.dmDisplayOrientation = 0;
                devMode.dmFields |= DM_DISPLAYORIENTATION;

                if ( ( lReturn = ChangeDisplaySettingsEx( dispDev.DeviceName, &devMode, NULL, CDS_RESET | CDS_UPDATEREGISTRY, NULL ) ) != DISP_CHANGE_SUCCESSFUL )
                {
                    StringCchPrintf( szMsg, sizeof(szMsg),
                                     "%s in %s at line %d.  WARNING: ChangeDisplaySettingsEx() returned %d and could not set orientation to 0 deg.\n",
                                     __FUNCTION__, __FILE__, __LINE__,
                                     lReturn );
                    WriteMsgs( szMsg, sizeof(szMsg) );
                }
            }
        }
    }

    return;
}


//
//  Make a new monitor into the Primary monitor.
//
BOOL
CCommon::SetPrimaryMonitor(
    __in_ecount(nDevNameLen)  LPCSTR  pszDevName,
    __in    UINT            nDevNameLen,
    __in    DEVMODE&        devMode
   )
{
    BOOL            fReturn = FALSE;
    LONG            lReturn = 0;
    char            szMsg[c_nMSG_LEN] = {0};    
    size_t          nLen = 0;


    // Validate parameters
    //
    if ( pszDevName == NULL || *pszDevName == 0 || pszDevName[c_nDEV_NAME_LEN - 1] != 0 )
    {
        return FALSE;
    }

    // The Primary is always at virtual position 0,0.  If this output is already the Primary,
    // there is no need to do anything.
    if ( devMode.dmPosition.x == 0 && devMode.dmPosition.y == 0 )
    {
        return TRUE;
    }
    else 
    {
        DEVMODE     oldPrimaryDevMode = {0};

        oldPrimaryDevMode.dmSize = sizeof(oldPrimaryDevMode);

        // The positioning of the old Primary does not have to be exact.  The OS
        // will adjust the virtual coordinates to be valid and also to prevent any gaps between
        // screens in virtual space.
        oldPrimaryDevMode.dmFields = DM_POSITION;
        oldPrimaryDevMode.dmPosition.x = devMode.dmPosition.x;
        oldPrimaryDevMode.dmPosition.y = devMode.dmPosition.y;

        devMode.dmFields = DM_POSITION;
        devMode.dmPosition.x = 0;
        devMode.dmPosition.y = 0;

        // CDS_UPDATE_REGISTRY
        if ( ( lReturn = ChangeDisplaySettingsEx( NULL, &oldPrimaryDevMode, NULL, CDS_NORESET | CDS_UPDATEREGISTRY, NULL ) ) != DISP_CHANGE_SUCCESSFUL )
        {
            StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  FAILED: ChangeDisplaySettingsEx() returned %d\n", __FUNCTION__, __FILE__, __LINE__, lReturn );
            WriteMsgs( szMsg, sizeof(szMsg) );
            goto Exit;
        }

        if ( ( lReturn = ChangeDisplaySettingsEx( pszDevName, &devMode, NULL, CDS_NORESET | CDS_UPDATEREGISTRY | CDS_SET_PRIMARY, NULL ) ) != DISP_CHANGE_SUCCESSFUL )
        {
            StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  FAILED: ChangeDisplaySettingsEx() returned %d\n", __FUNCTION__, __FILE__, __LINE__, lReturn );
            WriteMsgs( szMsg, sizeof(szMsg) );
            goto Exit;
        }

        // CDS_RESET all monitors
        if ( ( lReturn = ChangeDisplaySettings( NULL, CDS_RESET ) ) != DISP_CHANGE_SUCCESSFUL )
        {
            StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  FAILED: ChangeDisplaySettingsEx() returned %d\n", __FUNCTION__, __FILE__, __LINE__, lReturn );
            WriteMsgs( szMsg, sizeof(szMsg) );
            goto Exit;
        }
        
        // Check that the output is now the new Primary Display.
        if ( GetGDIDevMode( pszDevName, c_nDEV_NAME_LEN, devMode ) 
             && ( devMode.dmPosition.x != 0 || devMode.dmPosition.y != 0 ) )
        {
            StringCchPrintf( szMsg, sizeof(szMsg), "%s in %s at line %d.  FAILED: %s could not be made the Primary Display\n", __FUNCTION__, __FILE__, __LINE__,  pszDevName );
            WriteMsgs( szMsg, sizeof(szMsg) );
            goto Exit;
        }
    }

    fReturn = TRUE;

Exit:

    return fReturn;
}


//////////////////////////////////////////////////////////////////////////////
//
// Global Function Definitions
//
//////////////////////////////////////////////////////////////////////////////

int  __cdecl  Compare_DXGI_MODE_DESC( const void* pModeDesc1, const void* pModeDesc2 )
{

    if ( ((DXGI_MODE_DESC*)pModeDesc1)->Width < ((DXGI_MODE_DESC*)pModeDesc2)->Width )
        return -1;
    else
    if ( ((DXGI_MODE_DESC*)pModeDesc1)->Width > ((DXGI_MODE_DESC*)pModeDesc2)->Width )
        return 1;
    else    // Width is EQUAL
    {
        if ( ((DXGI_MODE_DESC*)pModeDesc1)->Height < ((DXGI_MODE_DESC*)pModeDesc2)->Height )
            return -1;
        else
        if ( ((DXGI_MODE_DESC*)pModeDesc1)->Height > ((DXGI_MODE_DESC*)pModeDesc2)->Height )
            return 1;
        else    // Height is EQUAL
        {
            if ( ((DXGI_MODE_DESC*)pModeDesc1)->RefreshRate.Numerator < ((DXGI_MODE_DESC*)pModeDesc2)->RefreshRate.Numerator )
                return -1;
            else
            if ( ((DXGI_MODE_DESC*)pModeDesc1)->RefreshRate.Numerator > ((DXGI_MODE_DESC*)pModeDesc2)->RefreshRate.Numerator )
                return 1;
            else    // RefreshRate.Numerator is EQUAL
            {
                if ( ((DXGI_MODE_DESC*)pModeDesc1)->RefreshRate.Denominator < ((DXGI_MODE_DESC*)pModeDesc2)->RefreshRate.Denominator )
                    return -1;
                else
                if ( ((DXGI_MODE_DESC*)pModeDesc1)->RefreshRate.Denominator > ((DXGI_MODE_DESC*)pModeDesc2)->RefreshRate.Denominator )
                    return 1;
                else    // RefreshRate.Denominator is EQUAL
                {
                    if ( ((DXGI_MODE_DESC*)pModeDesc1)->Format < ((DXGI_MODE_DESC*)pModeDesc2)->Format )
                        return -1;
                    else
                    if ( ((DXGI_MODE_DESC*)pModeDesc1)->Format > ((DXGI_MODE_DESC*)pModeDesc2)->Format )
                        return 1;
                    else    // Format is EQUAL
                    {
                        if ( ((DXGI_MODE_DESC*)pModeDesc1)->ScanlineOrdering < ((DXGI_MODE_DESC*)pModeDesc2)->ScanlineOrdering )
                            return -1;
                        else
                        if ( ((DXGI_MODE_DESC*)pModeDesc1)->ScanlineOrdering > ((DXGI_MODE_DESC*)pModeDesc2)->ScanlineOrdering )
                            return 1;
                        else    // ScanlineOrdering is EQUAL
                        {
                            if ( ((DXGI_MODE_DESC*)pModeDesc1)->Scaling < ((DXGI_MODE_DESC*)pModeDesc2)->Scaling )
                                return -1;
                            else
                            if ( ((DXGI_MODE_DESC*)pModeDesc1)->Scaling > ((DXGI_MODE_DESC*)pModeDesc2)->Scaling )
                                return 1;
                            else    // Scaling is EQUAL
                                return 0;
                        }
                    }
                }
            }
        }
    }

}
