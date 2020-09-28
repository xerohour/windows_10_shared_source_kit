#include "testcomponentspch.h"
#include "UnmanagedPrimarySurface.h"
#include <time.h>

/*
 * Implimentation of the CUnmanagedPrimarySurface Class as described in UnmanagedPrimarySurface.h
 */


CUnmanagedPrimarySurface::CUnmanagedPrimarySurface()
{
    SetDescription(L"Randomly selects an output head, and then attempts to create a full-screen device on that head.");
    
	AddParam<int>(L"Counter", L"Counter Integer for calling component, not used internally.");
    AddParam<bool>(L"SeedFromTime", L"When true the random generator is seeded from the system time. Otherwise seed is a static value.");
    AddParam<int>(L"Delay", L"Number of Milliseconds to wait after creating full screen device before the destruction and return process should begin.");
    
    SetParamTestDomainRange<int>(L"Counter", 1, 3, 1);
    SetParamTestDomainSingleValue<bool>(L"SeedFromTime", true);
    SetParamTestDomainSingleValue<int>(L"Delay", 10000);

    m_Initialized = false;
}

void CUnmanagedPrimarySurface::PreWalk(CLogWriter* pLog)
{
    if(!m_Initialized)
        InitializeDataStructures();
    
    DWORD SelectedDisplay = SelectRandomDisplay();

    if(SelectedDisplay >= m_Win32DisplayInfo.size())
        throw std::exception ("SelectedDisplay out of bounds of m_Win32DisplayInfo.");

    // Get current display mode so we can use the same resolution.
    DISPLAY_DEVICE dd;
    dd.cb = sizeof(DISPLAY_DEVICE);
    
    if(0 == EnumDisplayDevices(NULL, SelectedDisplay, &dd, 0))
        throw std::exception ("Unable to enumerate Selected Display.");

    DEVMODE dm;
    dm.dmSize = sizeof(DEVMODE);
    dm.dmDriverExtra = 0;
    
    if(0 == EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm))
    {
        //this can fail because we are conncurently enabling/disablingmonitors
        return;
    }

    // Now create the device
    CI3DPtr pI3D;
    CI3DDevicePtr pDevice;
    H_CHECK(::CreateI3D(&pI3D.p, m_Win32DisplayInfo[SelectedDisplay].sName.c_str()));
    
    DeviceCreateOptions Options;

    Options.bLegacyMode = (0 == (rand() % 2));//create legacy mode devices 1/2 of the time
                                              //this is important because it tests managed and unmanaged primary surfaces

    H_CHECK(pI3D->GetValidDeviceCreateOptions(false, NULL, dm.dmPelsWidth, dm.dmPelsHeight, 1, true, false, NULL, &Options));

    

    //this can fail because we are enabling/disabling displays
    if(FAILED(pI3D->CreateDevice(&Options, &pDevice.p)))
    {
        pLog->Skip(L"Failed to create a device");
    }
    else
    {
        pDevice->SetFatalErrorCB(&FatalErrorCallBack, pLog);
    }

    int Delay = GetParam<int>(L"Delay");
    Sleep(Delay);
}

void CUnmanagedPrimarySurface::InitializeDataStructures()
{
    bool SeedFromTime = GetParam<bool>(L"SeedFromTime");
    
    if(SeedFromTime)
        srand((unsigned)time(NULL));
    else
        srand(10);

    m_NumOfDisplays = GetNumOfDisplays();
    I3DGetDisplayInfo(m_Win32DisplayInfo);


    m_Initialized = true;
}

DWORD CUnmanagedPrimarySurface::SelectRandomDisplay()
{
    DWORD SelectedDisplay = 0;
    DWORD Loop = 20;        // Number of attempts at a random display

    while(Loop > 0)
    {
        SelectedDisplay = rand() % m_NumOfDisplays;
        DISPLAY_DEVICE dd;
        dd.cb = sizeof(DISPLAY_DEVICE);
        
        // Verify chosen adapter is connected to the desktop, and not a mirroring driver.
        if(EnumDisplayDevices(NULL, SelectedDisplay, &dd, 0))
        {
            if((dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
                !(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
            {
                break;
            }
        }

        --Loop;
    }

    if(Loop <= 0)
    {
        // Random selection didn't find an adapter connected to the desktop.
        // Just go through all the adapters and select the first one that is attached to the desktop.
        SelectedDisplay = m_NumOfDisplays;

        DISPLAY_DEVICE dd;
        dd.cb = sizeof(DISPLAY_DEVICE);

        for(DWORD DeviceNumber = 0; EnumDisplayDevices(NULL, DeviceNumber, &dd, 0); ++DeviceNumber)
        {
            if((dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) && 
                !(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
            {
                SelectedDisplay = DeviceNumber;
                break;
            }
        }
    }

    return SelectedDisplay;
}

DWORD CUnmanagedPrimarySurface::GetNumOfDisplays()
{
    DWORD DeviceNumber;

	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

    for(DeviceNumber = 0; EnumDisplayDevices(NULL, DeviceNumber, &dd, 0); ++DeviceNumber);
        
	return DeviceNumber;
}

void FatalErrorCallBack(I3D_FATAL_ERROR Error, const wchar_t* szExtraData, void* pUserData)
{
    //don't do anything that may cause an exception (because this is called from a destructor)
    OutputDebugString(L"Fatal error: ");

    const wchar_t *ErrorString = I3DFatalErrorToString(Error);

    OutputDebugString(ErrorString);

    OutputDebugString(L"\n");

}
