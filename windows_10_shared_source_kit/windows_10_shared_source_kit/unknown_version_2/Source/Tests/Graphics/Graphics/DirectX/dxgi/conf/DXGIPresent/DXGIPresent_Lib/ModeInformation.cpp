#include <modeinformation.h>
#include <TCHAR.h>
#include <sstream>
#include <DXGLogger.h>
#include <CDebugOut.h>
#include <DXGIValueSets.h>


typedef std::basic_string<TCHAR> tstring;
typedef basic_ostringstream<TCHAR> tostringstream;

bool operator == (const ModeInformation& lhs, const ModeInformation& rhs)
{
    if (lhs.ModeDesc.Width == rhs.ModeDesc.Width &&
        lhs.ModeDesc.Height == rhs.ModeDesc.Height &&
        lhs.ModeDesc.RefreshRate.Numerator == rhs.ModeDesc.RefreshRate.Numerator &&
        lhs.ModeDesc.RefreshRate.Denominator == rhs.ModeDesc.RefreshRate.Denominator &&
        lhs.ModeDesc.Format == rhs.ModeDesc.Format &&
        lhs.ModeDesc.ScanlineOrdering == rhs.ModeDesc.ScanlineOrdering &&
        lhs.ModeDesc.Scaling == rhs.ModeDesc.Scaling &&
        lhs.uiDXGIAdapterOrdinal == rhs.uiDXGIAdapterOrdinal &&
        lhs.uiDXGIOutputOrdinal == rhs.uiDXGIOutputOrdinal && 
        lhs.uiMultisampleCount == rhs.uiMultisampleCount &&
        lhs.uiMultisampleQuality == rhs.uiMultisampleQuality &&
        lhs.devmode.dmBitsPerPel == rhs.devmode.dmBitsPerPel &&
        lhs.devmode.dmPelsWidth == rhs.devmode.dmPelsWidth &&
        lhs.devmode.dmPelsHeight == rhs.devmode.dmPelsHeight &&
        lhs.devmode.dmDisplayFrequency == rhs.devmode.dmDisplayFrequency &&
        lhs.uiMultisampleCount == rhs.uiMultisampleCount&&
        lhs.uiMultisampleQuality == rhs.uiMultisampleQuality)
    {
        return TRUE;
    }

    return FALSE;

};

bool __stdcall FromString(ModeInformation *pFullscreenTestMode, const tstring &FullscreenTestModeString)
{
    return true;
};

tstring __stdcall ToString(const ModeInformation &fullscreenTestMode)
{
    tostringstream temp; 

    tstring outString = _T("ModeDesc.Width ");

    temp << fullscreenTestMode.ModeDesc.Width <<
        _T("X") << fullscreenTestMode.ModeDesc.Height <<
        _T(" @ ") << fullscreenTestMode.ModeDesc.RefreshRate.Numerator << _T("/") << fullscreenTestMode.ModeDesc.RefreshRate.Denominator << 
        _T("(")<< fullscreenTestMode.ModeDesc.RefreshRate.Numerator / fullscreenTestMode.ModeDesc.RefreshRate.Denominator<< _T(")") <<
        _T(" Format=")<< CFormatInfoMap::Find( fullscreenTestMode.ModeDesc.Format )->szDesc <<
        _T(" bpp=") <<fullscreenTestMode.devmode.dmBitsPerPel <<
        _T(" dmDisplayOrientation=")<< fullscreenTestMode.devmode.dmDisplayOrientation <<
        _T(" ScanlineOrdering=")<< fullscreenTestMode.ModeDesc.ScanlineOrdering <<
        _T(" Scaling=")<< fullscreenTestMode.ModeDesc.Scaling <<
        _T(" MultisampleCount=")<< fullscreenTestMode.uiMultisampleCount <<
        _T(" MultisampleQuality=")<< fullscreenTestMode.uiMultisampleQuality;

    return tstring(temp.str());
};

// The == operator for ModeInfomation does not include dmDisplayOrientation
bool operator == (const DEVMODEW& lmode, const DEVMODEW& rmode)
{
    if ( lmode.dmBitsPerPel == rmode.dmBitsPerPel &&
        lmode.dmPelsWidth == rmode.dmPelsWidth &&
        lmode.dmPelsHeight == rmode.dmPelsHeight &&
        lmode.dmDisplayFrequency == rmode.dmDisplayFrequency &&
        lmode.dmDisplayOrientation == rmode.dmDisplayOrientation )
    {
        return TRUE;
    }

    return FALSE;
};

bool operator > (const DEVMODEW& lmode, const DEVMODEW& rmode)
{
	// for bbpp comparison
	if ( lmode.dmBitsPerPel > rmode.dmBitsPerPel )
		return TRUE;
	return FALSE;
}

bool operator < (const DEVMODEW& lmode, const DEVMODEW& rmode)
{
	// for bbpp comparison
	if ( lmode.dmBitsPerPel < rmode.dmBitsPerPel )
		return TRUE;
	return FALSE;
}
bool operator >= (const DEVMODEW& lmode, const DEVMODEW& rmode)
{
	// for bbpp comparison
	if ( lmode.dmBitsPerPel >= rmode.dmBitsPerPel )
		return TRUE;
	return FALSE;
}

bool operator <= (const DEVMODEW& lmode, const DEVMODEW& rmode)
{
	// for bbpp comparison
	if ( lmode.dmBitsPerPel <= rmode.dmBitsPerPel )
		return TRUE;
	return FALSE;
}

bool __stdcall FromString(DEVMODEW *pMode, const tstring &szMode)
{
    if ( pMode )
        return true;
    else
        return false;
};

tstring __stdcall ToString(const DEVMODEW &mode)
{
    tostringstream temp; 

    tstring outString = _T("devmode dmBitsPerPel");

    temp << mode.dmBitsPerPel <<
        _T(" dmDisplayOrientation=")<< mode.dmDisplayOrientation ;

    return tstring(temp.str());
};