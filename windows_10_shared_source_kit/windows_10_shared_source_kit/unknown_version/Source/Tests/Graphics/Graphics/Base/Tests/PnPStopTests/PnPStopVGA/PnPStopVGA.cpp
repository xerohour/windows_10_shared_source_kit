#include "PnPStopVGA.h"
#include "GeneratePnPStop.h"
#include <wchar.h>

using namespace std;

/*
This is where we define the component
*/
CPnPStopVGA::CPnPStopVGA()
{
    /*
    First, we describe the purpose of this component for the spec
    */
    SetDescription(L"This application generates a PnPStop and checks if the system is in VGA mode.")  ;

    AddParam<bool>(L"UMPC", L"Is being run on a UMPC system.");
    SetParamTestDomainSingleValue<bool>(L"UMPC", false);
}


/*
This method is called each time we are to run a test case
*/
void CPnPStopVGA::PreWalk(CLogWriter * pLog)
{
    bool bUMPC = GetParam<bool>(L"UMPC");

    CPnPControlDevice controlAdapter;

    //Stop the display device.
    controlAdapter.PnPStop();

    //Give some time for GDI to enumerate modes after PNPStop.
    Sleep(1000);

    CheckVGA(pLog, bUMPC);

    //Start all devices which were stopped earlier.
    controlAdapter.PnPStart();

    return;
}

void CPnPStopVGA::PostWalk(CLogWriter * pLog)
{
}

void CPnPStopVGA::CheckVGA(CLogWriter * pLog, bool bUMPC)
{
    DWORD devNum = 0;
    DISPLAY_DEVICE dispDev;
    DWORD Data;

    LPBYTE InstalledDrivers = NULL;
    WCHAR* DeviceKey = NULL;
    HKEY hKey;      // handle to registry key
    DWORD InstalledDriversSize;
    wchar_t *Drivers = NULL;
    bool bVga = false;

    ZeroMemory(&dispDev, sizeof(DISPLAY_DEVICE));
    dispDev.cb = sizeof(DISPLAY_DEVICE);

    while(EnumDisplayDevices(NULL, devNum, &dispDev, 0))
    {
        //Check to see if this has a vga driver. If not bail out.
        if((dispDev.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) == 0)
        {
            //Skip "\\Registry\\Machine\\" from the start.
            DeviceKey = dispDev.DeviceKey + wcslen(L"\\Registry\\Machine\\");

            if( RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                DeviceKey,
                0,
                KEY_READ,
                &hKey) != ERROR_SUCCESS)
            {
                pLog->Fail(L"RegOpenKeyEx failed.");
                return;
            }

            if( RegQueryValueEx(hKey,
                L"InstalledDisplayDrivers",
                NULL,
                NULL,
                NULL,
                &InstalledDriversSize) != ERROR_SUCCESS)
            {
                pLog->Fail(L"RegQueryValueEx failed while getting InstalledDisplayDrivers size.");
                RegCloseKey(hKey);
                return;
            }

            InstalledDrivers = new BYTE[InstalledDriversSize];
            if(NULL == InstalledDriversSize)
            {
                pLog->Fail(L"Insuffecient memory.");
                RegCloseKey(hKey);
                return;
            }

            if( RegQueryValueEx(hKey,
                L"InstalledDisplayDrivers",
                NULL,
                NULL,
                InstalledDrivers,
                &InstalledDriversSize) != ERROR_SUCCESS)
            {
                pLog->Fail(L"RegQueryValueEx failed.");
                RegCloseKey(hKey);
                delete[] InstalledDrivers;
                return;
            }

            //check if vga is a part of the InstalledDisplayDrivers.
            Drivers = (wchar_t*)InstalledDrivers;
            int len = 0;
            while(wcscmp(Drivers,L"") != 0)
            {
                if(NULL != wcsstr(Drivers, L"vga"))
                {
                    bVga = true;
                    break;
                }

                len += (wcslen(Drivers) + 1) * sizeof(wchar_t);
                Drivers = (wchar_t*)(InstalledDrivers + len);

            }

            if(false == bVga)
            {
                pLog->Fail(L"Failed to install VGA driver.");
            }
            else
            {
                //Check for the card to support 10x7 for 32bpp and 24bpp.
                if(false == IsMinResolutionSupported(pLog, &dispDev, bUMPC))
                {
                    if(bUMPC)
                    {
                        pLog->Fail(L"Resolution of 8x6 for 32bpp or 24bpp not supported.");
                    }
                    else
                    {
                        pLog->Fail(L"Resolution of 10x7 for 32bpp or 24bpp not supported.");
                    }
                }
            }

            if( RegCloseKey(hKey) != ERROR_SUCCESS)
            {
                pLog->Skip(L"RegCloseKey failed.");
            }

            delete[] InstalledDrivers;


        }     

        devNum++;
    }

}

bool CPnPStopVGA::IsMinResolutionSupported(CLogWriter * pLog, DISPLAY_DEVICE* pDispDev, bool bUMPC)
{
    DWORD mode = 0;
    DEVMODE devMode = {0};
    bool bRes24bpp = false;
    bool bRes32bpp = false;

    DWORD dmPelsWidth = bUMPC ? 800 : 1024;
    DWORD dmPelsHeight = bUMPC ? 600 : 768;

    devMode.dmSize = sizeof(DEVMODE);

    while(!(bRes24bpp || bRes32bpp))
    {
        if(!EnumDisplaySettings(pDispDev->DeviceName, mode, &devMode))
        {
            // End of modes.  bail out.
            break;
        }
        else
        {
            if((devMode.dmBitsPerPel == 24) && (devMode.dmPelsWidth == dmPelsWidth) && (devMode.dmPelsHeight == dmPelsHeight))
            {
                bRes24bpp = true;
            }
            else
            {
                if((devMode.dmBitsPerPel == 32) && (devMode.dmPelsWidth == dmPelsWidth) && (devMode.dmPelsHeight == dmPelsHeight))
                {
                    bRes32bpp = true;
                }
            }
        }

        mode++;
    }

    return (bRes24bpp || bRes32bpp);
}