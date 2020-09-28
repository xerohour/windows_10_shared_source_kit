#include "testcomponentspch.h"
#include "RandomModeChange.h"
#include <time.h>

/*
 * Implimentation of the RandomModeChange Class as described in RandomModeChange.h
 */

CRandomModeChange::CRandomModeChange()
{
    SetDescription(L"Switches the display mode of one randomly choosen display to a randomly choosen mode. Stores and compares results to determine failures.");
    
	AddParam<int>(L"Counter", L"Counter Integer for calling component, not used internally.");
    AddParam<bool>(L"SeedFromTime", L"When true the random generator is seeded from the system time. Otherwise seed is a static value.");
    AddParam<int>(L"Delay", L"Number of Milliseconds to wait after mode change before test completes.");

    SetParamTestDomainRange<int>(L"Counter", 1, 10000, 1);
    SetParamTestDomainSingleValue<bool>(L"SeedFromTime", true);
    SetParamTestDomainSingleValue<int>(L"Delay", 3000);

    m_Initialized = false;
    m_NumOfDisplays = 0;
    m_DisplayModes.clear();
    m_ResultMap.clear();
}

void CRandomModeChange::PreWalk(CLogWriter* pLog)
{
    if(!m_Initialized)
        InitializeDataStructures();

    DWORD SelectedDisplay = SelectRandomDisplay();

    if(SelectedDisplay >= m_NumOfDisplays)
        throw std::exception ("Unable to determine appropriate display for mode change.");

    DWORD SelectedMode = rand() % m_DisplayModes[SelectedDisplay].size();

    FullDisplayMode NewFullDisplayMode; // This will be a log of the modes for all the adapters on the system.
    NewFullDisplayMode.isSupported = false;

    for(DWORD i = 0; i < m_NumOfDisplays; ++i)
    {
        DisplayMode NewDisplayMode;

        DISPLAY_DEVICE dd;
        dd.cb = sizeof(DISPLAY_DEVICE);

        if(0 == EnumDisplayDevices(NULL, i, &dd, 0))
            throw std::exception("Unable to enumerate selected display.");

        DEVMODE dm;
        dm.dmSize = sizeof(DEVMODE);
        dm.dmDriverExtra = 0;

        if(0 == EnumDisplaySettings( dd.DeviceName, ENUM_CURRENT_SETTINGS, &dm ))
            continue;

        if(SelectedDisplay == i)
        {
            NewDisplayMode.DisplayNumber  = i;
            NewDisplayMode.Width          = m_DisplayModes[SelectedDisplay][SelectedMode].Width;
            NewDisplayMode.Height         = m_DisplayModes[SelectedDisplay][SelectedMode].Height;
            NewDisplayMode.ColorDepth     = m_DisplayModes[SelectedDisplay][SelectedMode].ColorDepth;
            NewDisplayMode.Frequency      = m_DisplayModes[SelectedDisplay][SelectedMode].Frequency;
            NewDisplayMode.ModeIndex      = m_DisplayModes[SelectedDisplay][SelectedMode].ModeIndex;
        }
        else
        {
            NewDisplayMode.DisplayNumber  = i;
            NewDisplayMode.Width          = dm.dmPelsWidth;
            NewDisplayMode.Height         = dm.dmPelsHeight;
            NewDisplayMode.ColorDepth     = dm.dmBitsPerPel;
            NewDisplayMode.Frequency      = dm.dmDisplayFrequency;
            NewDisplayMode.ModeIndex      = FindModeNumber(i, NewDisplayMode.Width, NewDisplayMode.Height, NewDisplayMode.ColorDepth, NewDisplayMode.Frequency);
        }

        if(NewDisplayMode.ModeIndex >= m_DisplayModes[i].size())
            throw std::exception("Unable to match current display mode to any enumerated mode.");

        NewFullDisplayMode.DisplayModes.push_back(NewDisplayMode);
    }

    // Attempt Switch
    bool Result = ChangeDisplayModeByIndex(SelectedMode, SelectedDisplay);

    if(Result)
    {
        int Delay = GetParam<int>(L"Delay");
        Sleep(Delay);
    }
    
    // Compare result / log result
    std::set<FullDisplayMode>::iterator it;
    it = m_ResultMap.find(NewFullDisplayMode);

    if(it == m_ResultMap.end())
    {
        m_ResultMap.insert(NewFullDisplayMode);
    }
    else
    {
        if((*it).isSupported != NewFullDisplayMode.isSupported)
        {
            // Compare failed, build error string and log failure.
            std::wstring Failure = L"Mode switch ";

            if(NewFullDisplayMode.isSupported)
                Failure += L"succeeded where previously failed changing Display #";
            else
                Failure += L"failed where previously succeeded changing Display #";

            Failure += ToString<DWORD>(SelectedDisplay);
            Failure += L"; Attempted mode was: ";

            for(DWORD i = 0; i < NewFullDisplayMode.DisplayModes.size(); ++i)
            {
                Failure += L"Display #";
                Failure += ToString<DWORD>(NewFullDisplayMode.DisplayModes[i].DisplayNumber);
                Failure += L": ";
                Failure += ToString<DWORD>(NewFullDisplayMode.DisplayModes[i].Width);
                Failure += L"x";
                Failure += ToString<DWORD>(NewFullDisplayMode.DisplayModes[i].Height);
                Failure += L", ";
                Failure += ToString<DWORD>(NewFullDisplayMode.DisplayModes[i].ColorDepth);
                Failure += L"bpp, ";
                Failure += ToString<DWORD>(NewFullDisplayMode.DisplayModes[i].Frequency);
                Failure += L"hz ;";
            }
            pLog->Fail(Failure.c_str());
        }
    }

}

void CRandomModeChange::InitializeDataStructures()
{
    bool SeedFromTime = GetParam<bool>(L"SeedFromTime");
    
    if(SeedFromTime)
        srand((unsigned)time(NULL));
    else
        srand(10);

    m_NumOfDisplays = GetNumOfDisplays();

    m_DisplayModes.clear();
    for(DWORD i = 0; i < m_NumOfDisplays; ++i)
    {
        std::vector<DisplayMode> newVector;
        m_DisplayModes.push_back(newVector);

        DISPLAY_DEVICE dd;
        dd.cb = sizeof(DISPLAY_DEVICE);

        if(0 == EnumDisplayDevices(NULL, i, &dd, 0))
            throw std::exception ("Unable to enumerate previously enumerated display device.");

        DEVMODE dm;
        dm.dmSize = sizeof(DEVMODE);
        dm.dmDriverExtra = 0;

        for(DWORD ModeNumber = 0; EnumDisplaySettingsEx(dd.DeviceName, ModeNumber, &dm, EDS_ROTATEDMODE); ++ModeNumber)
        {
            DisplayMode NewDisplayMode;

            NewDisplayMode.DisplayNumber = i;
            NewDisplayMode.ModeIndex     = ModeNumber;
            NewDisplayMode.Width         = dm.dmPelsWidth;
            NewDisplayMode.Height        = dm.dmPelsHeight;
            NewDisplayMode.ColorDepth    = dm.dmBitsPerPel;
            NewDisplayMode.Frequency     = dm.dmDisplayFrequency;

            m_DisplayModes[i].push_back(NewDisplayMode);
        }
    }

    m_Initialized = true;
}

bool CRandomModeChange::ChangeDisplayModeByIndex(DWORD ModeIndex, DWORD DisplayNumber)
{
    DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

    if(0 == EnumDisplayDevices(NULL, DisplayNumber, &dd, 0))
        return false;

	if((dd.StateFlags & DISPLAY_DEVICE_MIRRORING_DRIVER) || 
        !(dd.StateFlags & DISPLAY_DEVICE_ATTACHED_TO_DESKTOP))
        return false;

    DEVMODE dm;
    dm.dmSize = sizeof(DEVMODE);
    dm.dmDriverExtra = 0;

    dm.dmFields = DM_PELSWIDTH | DM_PELSHEIGHT | DM_BITSPERPEL | DM_DISPLAYFREQUENCY;

    dm.dmPelsWidth = m_DisplayModes[DisplayNumber][ModeIndex].Width;
    dm.dmPelsHeight = m_DisplayModes[DisplayNumber][ModeIndex].Height;
    dm.dmBitsPerPel = m_DisplayModes[DisplayNumber][ModeIndex].ColorDepth;
    dm.dmDisplayFrequency = m_DisplayModes[DisplayNumber][ModeIndex].Frequency;


	LONG Result = ChangeDisplaySettingsEx(dd.DeviceName, &dm, NULL, 0, NULL);

   	switch(Result)
	{
	case DISP_CHANGE_SUCCESSFUL:
		break;
	case DISP_CHANGE_BADMODE:
		return false;
	case DISP_CHANGE_FAILED:
		return false;
	case DISP_CHANGE_NOTUPDATED:
		return false;
	case DISP_CHANGE_RESTART:
		return false;
	}

    return true;
}
DWORD CRandomModeChange::SelectRandomDisplay()
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

DWORD CRandomModeChange::GetNumOfDisplays()
{
    DWORD DeviceNumber;

	DISPLAY_DEVICE dd;
	dd.cb = sizeof(DISPLAY_DEVICE);

    for(DeviceNumber = 0; EnumDisplayDevices(NULL, DeviceNumber, &dd, 0); ++DeviceNumber);
        
	return DeviceNumber;
}


DWORD CRandomModeChange::FindModeNumber(DWORD Display, DWORD Width, DWORD Height, DWORD ColorDepth, DWORD Frequency)
{
    DWORD Mode = 0;
    while(Mode < m_DisplayModes[Display].size())
    {
        if( (m_DisplayModes[Display][Mode].Width == Width) &&
            (m_DisplayModes[Display][Mode].Height == Height) &&
            (m_DisplayModes[Display][Mode].ColorDepth == ColorDepth) &&
            (m_DisplayModes[Display][Mode].Frequency == Frequency))
        {
            return Mode;
        }
        ++Mode;
    }

    return Mode;
}
