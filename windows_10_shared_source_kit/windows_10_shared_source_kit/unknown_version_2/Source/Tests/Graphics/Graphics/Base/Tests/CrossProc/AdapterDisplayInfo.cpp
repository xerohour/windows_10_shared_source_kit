#include "AdapterDisplayInfo.h"

AdapterDisplayInfo::AdapterDisplayInfo(LUID luid, std::wstring displayName)
{
    m_luid = luid;
    m_displayName = displayName;
}

LUID AdapterDisplayInfo::GetLuid()
{
    return m_luid;
}

std::wstring AdapterDisplayInfo::GetDisplayName()
{
    return m_displayName;
}