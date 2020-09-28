#include "testcomponentspch.h"
#include "RandomMultiMon.h"
#include <time.h>

/*
 * Implimentation of the RandomMultiMon Class as described in RandomMultiMon.h
 */

CRandomMultiMon::CRandomMultiMon()
{
    SetDescription(L"Randomly attaches and detaches available monitors from the desktop.");
    
	AddParam<int>(L"Counter", L"Counter Integer for calling component, not used internally.");
    AddParam<bool>(L"SeedFromTime", L"When true the random generator is seeded from the system time. Otherwise seed is a static value.");
    AddParam<int>(L"Delay", L"Number of Milliseconds to wait after attach/detach finishes before the test completes.");

    SetParamTestDomainRange<int>(L"Counter", 1, 10, 1);
    SetParamTestDomainSingleValue<bool>(L"SeedFromTime", true);
    SetParamTestDomainSingleValue<int>(L"Delay", 3000);

    m_Initialized = false;
    m_NumOfDisplays = 0;
    m_DisplaysAvailableToAttach = 0;
    m_StoredModes.clear();
}

void CRandomMultiMon::PreWalk(CLogWriter* pLog)
{
    if(!m_Initialized)
        InitializeDataStructures();

    DWORD DisplaysAttached = GetNumOfDisplaysAttached();

    if(DisplaysAttached < 2)
        AttachRandomDisplay(pLog);
    else if(DisplaysAttached >= m_DisplaysAvailableToAttach)
        DetachRandomDisplay(pLog);
    else
    {
        if(rand() % 2)
            AttachRandomDisplay(pLog);
        else
            DetachRandomDisplay(pLog);
    }

    int Delay = GetParam<int>(L"Delay");
    Sleep(Delay);
}

void CRandomMultiMon::InitializeDataStructures()
{
    bool SeedFromTime = GetParam<bool>(L"SeedFromTime");
    
    if(SeedFromTime)
        srand((unsigned)time(NULL));
    else
        srand(10);

    m_NumOfDisplays = GetNumOfDisplays();
    m_DisplaysAvailableToAttach = GetDisplaysAvailableToAttach();

    for(DWORD i = 0; i < m_NumOfDisplays; ++i)
    {
        DisplayMode NewDisplayMode;
        NewDisplayMode.DisplayNumber = i;
        NewDisplayMode.ModeIndex = 0;
        NewDisplayMode.Width = 800;
        NewDisplayMode.Height = 600;
        NewDisplayMode.ColorDepth = 32;
        NewDisplayMode.Frequency = 60;

        m_StoredModes.push_back(NewDisplayMode);
    }

    m_Initialized = true;
}


void CRandomMultiMon::AttachRandomDisplay(CLogWriter* pLog)
{
    DWORD SelectedDisplay = SelectDisplayForAttach();

    if(SelectedDisplay >= m_NumOfDisplays)
        throw std::exception ("Unable to determine appropriate display for detach.");

    DISPLAY_DEVICE dd;
    dd.cb = sizeof(DISPLAY_DEVICE);
    
    if(0 == EnumDisplayDevices(NULL, SelectedDisplay, &dd, 0))
        throw std::exception ("Unable to enumerate Selected Display.");

    DEVMODE dm;
    dm.dmSize = sizeof(DEVMODE);
    dm.dmDriverExtra = 0;
    
    if(0 == EnumDisplaySettings(dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm))
    {
        //this is allowed to fail, because the device can be disabled at any moment
        return;
    }

    dm.dmPelsWidth = m_StoredModes[SelectedDisplay].Width;
    dm.dmPelsHeight = m_StoredModes[SelectedDisplay].Height;
    dm.dmBitsPerPel = m_StoredModes[SelectedDisplay].ColorDepth;
    dm.dmDisplayFrequency = m_StoredModes[SelectedDisplay].Frequency;

    POINTL NewPosition;
    NewPosition.x = 0 - dm.dmPelsWidth;
    NewPosition.y = 0;

    dm.dmPosition = NewPosition;
    dm.dmFields = DM_POSITION | DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;



    LONG Result = ChangeDisplaySettingsEx(dd.DeviceName, &dm, NULL, 0, NULL);

    switch(Result)
	{
	case DISP_CHANGE_SUCCESSFUL:
		return;
	case DISP_CHANGE_BADMODE:
        throw std::exception ("Unable to attach Display: DISP_CHANGE_BADMODE");
	case DISP_CHANGE_FAILED:
        throw std::exception ("Unable to attach Display: DISP_CHANGE_FAILED");
    case DISP_CHANGE_NOTUPDATED:
        throw std::exception ("Unable to attach Display: DISP_CHANGE_NOTUPDATED");
    case DISP_CHANGE_RESTART:
        throw std::exception ("Unable to attach Display: DISP_CHANGE_RESTART");
    }
}
void CRandomMultiMon::DetachRandomDisplay(CLogWriter* pLog)
{
    DWORD SelectedDisplay = SelectDisplayForDetach();
    
    if(SelectedDisplay >= m_NumOfDisplays)
        throw std::exception ("Unable to determine appropriate display for detach.");

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


    DisplayMode NewDisplayMode;
    NewDisplayMode.DisplayNumber = SelectedDisplay;
    NewDisplayMode.ModeIndex = 0;
    NewDisplayMode.Width = dm.dmPelsWidth;
    NewDisplayMode.Height = dm.dmPelsHeight;
    NewDisplayMode.ColorDepth = dm.dmBitsPerPel;
    NewDisplayMode.Frequency = dm.dmDisplayFrequency;

    m_StoredModes[SelectedDisplay] = NewDisplayMode;
    dm.dmPelsWidth = 0;
    dm.dmPelsHeight = 0;
    dm.dmFields = DM_POSITION | DM_PELSWIDTH | DM_PELSHEIGHT;

    LONG Result = ChangeDisplaySettingsEx(dd.DeviceName, &dm, NULL, 0, NULL);
        

    switch(Result)
	{
	case DISP_CHANGE_SUCCESSFUL:
		return;
	case DISP_CHANGE_BADMODE:
        throw std::exception ("Unable to detach Display: DISP_CHANGE_BADMODE");
	case DISP_CHANGE_FAILED:
        throw std::exception ("Unable to detach Display: DISP_CHANGE_FAILED");
    case DISP_CHANGE_NOTUPDATED:
        throw std::exception ("Unable to detach Display: DISP_CHANGE_NOTUPDATED");
    case DISP_CHANGE_RESTART:
        throw std::exception ("Unable to detach Display: DISP_CHANGE_RESTART");
    }
}
DWORD CRandomMultiMon::SelectDisplayForDetach()
{
    DWORD SelectedDisplay = 0;
    DWORD Loop = 20;

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

    if(Loop == 0)
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
DWORD CRandomMultiMon::SelectDisplayForAttach()
{
    DWORD SelectedDisplay = 0;
    DWORD Loop = 20;

    while(Loop > 0)
    {
        SelectedDisplay = rand() % m_NumOfDisplays;

        DISPLAY_DEVICE dd;
        dd.cb = sizeof(DISPLAY_DEVICE);

        // Verify chosen adapter is  not connected to the desktop, and not a mirroring driver.
        if(EnumDisplayDevices(NULL, SelectedDisplay, &dd, 0))
        {
            if(!(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
                !(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
            {
                break;
            }
        }

        --Loop;
    }
    if(Loop == 0)
    {
        // Random selection didn't find an adapter not connected to the desktop.
        // Just go through all the adapters and select the first one that is not attached to the desktop.
        SelectedDisplay = m_NumOfDisplays;

        DISPLAY_DEVICE dd;
        dd.cb = sizeof(DISPLAY_DEVICE);

        for(DWORD DeviceNumber = 0; EnumDisplayDevices(NULL, DeviceNumber, &dd, 0); ++DeviceNumber)
        {
            if(!(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP) &&
                !(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
            {
                SelectedDisplay = DeviceNumber;
                break;
            }
        }
    }
    return SelectedDisplay;
}
DWORD CRandomMultiMon::GetNumOfDisplays()
{
    DWORD DeviceNumber;

	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

    for(DeviceNumber = 0; EnumDisplayDevices(NULL, DeviceNumber, &dd, 0); ++DeviceNumber);
        
	return DeviceNumber;
}
DWORD CRandomMultiMon::GetDisplaysAvailableToAttach()
{
    DWORD DisplaysAvailableToAttach = 0;

    DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

    for(DWORD DeviceNumber = 0; EnumDisplayDevices(NULL, DeviceNumber, &dd, 0); ++DeviceNumber)
    {
        if(!(dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER))
            ++DisplaysAvailableToAttach;
    }

    return DisplaysAvailableToAttach;
}
DWORD CRandomMultiMon::GetNumOfDisplaysAttached()
{
    DWORD DisplaysAttached = 0;

    DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

    for(DWORD DeviceNumber = 0; EnumDisplayDevices(NULL, DeviceNumber, &dd, 0); ++DeviceNumber)
    {
        if(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP)
            ++DisplaysAttached;
    }

    return DisplaysAttached;
}

