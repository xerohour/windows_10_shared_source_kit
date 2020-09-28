#include "EnumModesGDI.h"

std::vector<clsEnumModesGDI::GDI_MONITOR_MODE> clsEnumModesGDI::MonitorModePairs;

clsEnumModesGDI::clsEnumModesGDI()
{
}

clsEnumModesGDI::~clsEnumModesGDI()
{
}

BOOL CALLBACK clsEnumModesGDI::MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor,
											   LPRECT lprcMonitor, LPARAM dwData)
{
	((std::vector<HMONITOR> *)dwData) -> push_back(hMonitor);
	return TRUE;
}

void clsEnumModesGDI::ClearPairs()
{
	MonitorModePairs.clear();
}

UINT clsEnumModesGDI::EnumeratePairs(const DesktopModeRecord *pModeFilters, UINT uiNoOfModeFilters)
{
	UINT uiTotalModes = 0;
	std::vector<HMONITOR> hMonitors;
	std::vector<HMONITOR>::iterator iter;

	ClearPairs();
	hMonitors.clear();
	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)(&MonitorEnumProc), (LPARAM)(&hMonitors));

	iter = hMonitors.begin();

	while (iter != hMonitors.end())
	{
		MONITORINFOEXW monitorinfo;
		HMONITOR hmonitorEnumerated = *iter;

		ZeroMemory((LPVOID)&monitorinfo, sizeof(MONITORINFOEXW));
		monitorinfo.cbSize = sizeof(MONITORINFOEXW);

		if (GetMonitorInfoW(hmonitorEnumerated, &monitorinfo))
		{
			UINT uiMatchModeIndex;

			DebugOut.Write("Monitor string: %s\n", monitorinfo.szDevice);

			for (uiMatchModeIndex = 0; uiMatchModeIndex < uiNoOfModeFilters; ++uiMatchModeIndex)
			{
				DEVMODEW devmode;
				UINT uiModeIndex = 0;

				DebugOut.Write("Matching record %d\n", uiMatchModeIndex);

				ZeroMemory((PVOID)(&devmode), sizeof(DEVMODEW));
				devmode.dmSize = sizeof(DEVMODEW);

				if (EnumDisplaySettingsW(monitorinfo.szDevice, uiModeIndex, &devmode))
					do
					{
						DebugOut.Write("Handling mode %d\n", uiModeIndex);

						if
						(
							(devmode.dmPelsWidth == (pModeFilters[uiMatchModeIndex]).uiWidth) &&
							(devmode.dmPelsHeight == (pModeFilters[uiMatchModeIndex]).uiHeight) &&
							(devmode.dmBitsPerPel == (pModeFilters[uiMatchModeIndex]).uiBPP)
						)
						{
							GDI_MONITOR_MODE NewTestableMode;

							NewTestableMode.hmonitor = hmonitorEnumerated;
							NewTestableMode.devmode = devmode;

							DebugOut.Write("Pushing a mode.\n");

							MonitorModePairs.push_back(NewTestableMode);
							++uiTotalModes;
							break;
						}

						/*
						GetFramework()->WriteToLogF
						(
							"%d x %d x %dbpp @ %dHz\n",
							devmode.dmPelsWidth,
							devmode.dmPelsHeight,
							devmode.dmBitsPerPel,
							//devmode.dmDisplayFlags;
							devmode.dmDisplayFrequency
						);
						*/

						++uiModeIndex;
						ZeroMemory((PVOID)(&devmode), sizeof(DEVMODEW));
						devmode.dmSize = sizeof(DEVMODEW);
					}
					while (EnumDisplaySettingsW(monitorinfo.szDevice, uiModeIndex, &devmode));
			}
		}

		++iter;
	}

	DebugOut.Write("Returning %d modes.\n", uiTotalModes);

	return uiTotalModes;
}

//-----------------------------------------------------------------------------
//
//   Match bits per pixel first and then try to find a resolution greater than
//   or equal to the passed in value.
//
//----------------------------------------------------------------------------- 
UINT clsEnumModesGDI::FindClosestMatchRequireBpp(const DesktopModeRecord *pModeFilters, UINT uiNoOfModeFilters)
{
	UINT uiTotalModes = 0;
	std::vector<HMONITOR> hMonitors;
	std::vector<HMONITOR>::iterator iter;

	ClearPairs();
	hMonitors.clear();
	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)(&MonitorEnumProc), (LPARAM)(&hMonitors));
    
    iter = hMonitors.begin();

	while (iter != hMonitors.end())
	{
		MONITORINFOEXW monitorinfo;
		HMONITOR hmonitorEnumerated = *iter;

		ZeroMemory((LPVOID)&monitorinfo, sizeof(MONITORINFOEXW));
		monitorinfo.cbSize = sizeof(MONITORINFOEXW);

		if (GetMonitorInfoW(hmonitorEnumerated, &monitorinfo))
		{
			UINT uiMatchModeIndex;

			DebugOut.Write(L"Monitor string: %s\n", monitorinfo.szDevice);

			for (uiMatchModeIndex = 0; uiMatchModeIndex < uiNoOfModeFilters; ++uiMatchModeIndex)
			{
				BOOL foundExactMatch = false;
                DEVMODEW devmode;
				UINT uiModeIndex = 0;

				DebugOut.Write("Matching record %d\n", uiMatchModeIndex);

				ZeroMemory((PVOID)(&devmode), sizeof(DEVMODEW));
				devmode.dmSize = sizeof(DEVMODEW);


				if (EnumDisplaySettingsW(monitorinfo.szDevice, uiModeIndex, &devmode))
                {
                    DEVMODEW nearestHigherMode;
                    ZeroMemory((PVOID)(&nearestHigherMode), sizeof(DEVMODEW));
				    nearestHigherMode.dmSize = sizeof(DEVMODEW);

                    do
					{
                        DebugOut.Write("Handling mode %d\n", uiModeIndex);
					    //
                        // Require matching bits per pixel
                        //
                        if(devmode.dmBitsPerPel == (pModeFilters[uiMatchModeIndex]).uiBPP)
    					{
                            if
						    (
							    (devmode.dmPelsWidth == (pModeFilters[uiMatchModeIndex]).uiWidth) &&
							    (devmode.dmPelsHeight == (pModeFilters[uiMatchModeIndex]).uiHeight)
                            )
                            {
                            
                                GDI_MONITOR_MODE NewTestableMode;

							    NewTestableMode.hmonitor = hmonitorEnumerated;
							    NewTestableMode.devmode = devmode;

							    DebugOut.Write("Pushing a mode.\n");

							    MonitorModePairs.push_back(NewTestableMode);
							    ++uiTotalModes;
                                foundExactMatch = true;
							    break;
                            }
                            else if
					        (
						        (devmode.dmPelsWidth >= (pModeFilters[uiMatchModeIndex]).uiWidth) &&
						        (devmode.dmPelsHeight >= (pModeFilters[uiMatchModeIndex]).uiHeight) &&
                                (((devmode.dmPelsWidth < nearestHigherMode.dmPelsWidth) &&
						        (devmode.dmPelsHeight < nearestHigherMode.dmPelsHeight)) || 
                                ((nearestHigherMode.dmPelsWidth == 0) &&
							    (nearestHigherMode.dmPelsHeight == 0) &&
                                (nearestHigherMode.dmBitsPerPel == 0)))

                            )
                            {
                                 memcpy( &nearestHigherMode, &devmode, sizeof( DEVMODEW ) );                               
                            }
                        }
                        
                        /*
                        DebugOut.Write
						(
							"%d x %d x %dbpp @ %dHz dmDisplayFlags %d\n",
							devmode.dmPelsWidth,
							devmode.dmPelsHeight,
							devmode.dmBitsPerPel,
							devmode.dmDisplayFrequency,
                            devmode.dmDisplayFlags
						);
                        */

						++uiModeIndex;
						ZeroMemory((PVOID)(&devmode), sizeof(DEVMODEW));
						devmode.dmSize = sizeof(DEVMODEW);
					}
					while (EnumDisplaySettingsW(monitorinfo.szDevice, uiModeIndex, &devmode));

                    if(!foundExactMatch && 
                        nearestHigherMode.dmPelsHeight != 0 && 
                        nearestHigherMode.dmPelsWidth !=0 )
                    {
                        GDI_MONITOR_MODE NewTestableMode;
                        NewTestableMode.hmonitor = hmonitorEnumerated;
				        NewTestableMode.devmode = nearestHigherMode;
                        DebugOut.Write("Pushing a mode.\n");
                        MonitorModePairs.push_back(NewTestableMode);
                        ++uiTotalModes;                      
                    }
                }
			}
		}

		++iter;
	}

	DebugOut.Write("Returning %d modes.\n", uiTotalModes);

	return uiTotalModes;
}


//-----------------------------------------------------------------------------
//
// Gets the MONITORINFO for all the connected monitors
//
//----------------------------------------------------------------------------- 
UINT clsEnumModesGDI::GetAllMontitorModes()
{
	UINT uiTotalModes = 0;
	std::vector<HMONITOR> hMonitors;
	std::vector<HMONITOR>::iterator iter;

	EnumDisplayMonitors(NULL, NULL, (MONITORENUMPROC)(&MonitorEnumProc), (LPARAM)(&hMonitors));
    DebugOut.Write("EnumDisplayMonitors: %d\n", hMonitors.size());   

	iter = hMonitors.begin();

	while (iter != hMonitors.end())
    {
        MONITORINFOEXW monitorinfo;
        HMONITOR hmonitorEnumerated = *iter;

        ZeroMemory((LPVOID)&monitorinfo, sizeof(MONITORINFOEXW));
        monitorinfo.cbSize = sizeof(MONITORINFOEXW);

        if (GetMonitorInfoW(hmonitorEnumerated, &monitorinfo))
        {
            UINT uiMatchModeIndex;
            DebugOut.Write(L"Monitor string: %s\n", monitorinfo.szDevice);
            DEVMODEW devmode;

            ZeroMemory((PVOID)(&devmode), sizeof(DEVMODEW));
            devmode.dmSize = sizeof(DEVMODEW);
            if (EnumDisplaySettingsW(NULL, ENUM_CURRENT_SETTINGS, &devmode))
            {
                DebugOut.Write
                    (
                    "%d x %d x %dbpp @ %dHz dmDisplayFlags %d\n",
                    devmode.dmPelsWidth,
                    devmode.dmPelsHeight,
                    devmode.dmBitsPerPel,
                    devmode.dmDisplayFrequency,
                    devmode.dmDisplayFlags
                    );

                GDI_MONITOR_MODE newTestableMode;
                newTestableMode.hmonitor = hmonitorEnumerated;
                newTestableMode.devmode = devmode;
                MonitorModePairs.push_back(newTestableMode);
                ++uiTotalModes;
            }
        }
        ++iter;
    }

	DebugOut.Write("Returning %d modes.\n", uiTotalModes);

	return uiTotalModes;
}