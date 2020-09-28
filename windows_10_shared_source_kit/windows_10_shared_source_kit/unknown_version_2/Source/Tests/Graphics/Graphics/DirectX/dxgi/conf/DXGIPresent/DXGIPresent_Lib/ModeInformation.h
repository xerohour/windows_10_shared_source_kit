
#pragma once
#include <windows.h>
#include <vector>
#include <dxgi.h>


using namespace std;
typedef std::basic_string<TCHAR> tstring;

class ModeInformation
{
public:
	DXGI_MODE_DESC ModeDesc;
	UINT uiDXGIAdapterOrdinal;
	UINT uiDXGIOutputOrdinal;
	UINT uiMultisampleCount;
    UINT uiMultisampleQuality;
    HMONITOR hmonitor;
    DEVMODEW devmode;
    BOOL isInitialized;
};


bool operator == (const ModeInformation& lhs, const ModeInformation& rhs);
bool __stdcall FromString(ModeInformation *pFullscreenTestMode, const tstring &FullscreenTestModeString);
tstring __stdcall ToString(const ModeInformation &fullscreenTestMode);

bool operator > (const DEVMODEW& lmode, const DEVMODEW& rmode);
bool operator < (const DEVMODEW& lmode, const DEVMODEW& rmode);
bool operator >= (const DEVMODEW& lmode, const DEVMODEW& rmode);
bool operator <= (const DEVMODEW& lmode, const DEVMODEW& rmode);

bool operator == (const DEVMODEW& lmode, const DEVMODEW& rmode);
bool __stdcall FromString( DEVMODEW *pMode, const tstring &szMode);
tstring __stdcall ToString(const DEVMODEW &mode);
