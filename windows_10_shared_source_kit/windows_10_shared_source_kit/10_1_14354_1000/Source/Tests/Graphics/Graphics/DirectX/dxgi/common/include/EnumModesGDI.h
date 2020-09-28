#ifndef DXGI_TEST_ENUMMODESGDI_INCLUDED
#define DXGI_TEST_ENUMMODESGDI_INCLUDED

#include <windows.h>
#include <vector>

#include "WGFTestCore.h"


class clsEnumModesGDI
{
	public:
		clsEnumModesGDI();
		virtual ~clsEnumModesGDI();


	protected:
		static BOOL CALLBACK MonitorEnumProc(HMONITOR hMonitor, HDC hdcMonitor, LPRECT lprcMonitor, LPARAM dwData);


	public:
		typedef struct _DesktopModeRecord
		{
			UINT uiWidth;
			UINT uiHeight;
			UINT uiBPP;
		} DesktopModeRecord;

		typedef struct _GDI_MONITOR_MODE
		{
			HMONITOR hmonitor;
			DEVMODEW devmode;
		} GDI_MONITOR_MODE;

		static std::vector<GDI_MONITOR_MODE> MonitorModePairs;

		static void ClearPairs();
		static UINT EnumeratePairs(const DesktopModeRecord *pModeFilters, UINT uiNoOfModeFilters);
        static UINT FindClosestMatchRequireBpp(const DesktopModeRecord *pModeFilters, UINT uiNoOfModeFilters);
        static UINT GetAllMontitorModes();

};

#endif
