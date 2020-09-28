#ifndef _ADAPTERDISPLAYINFO_H_
#define _ADAPTERDISPLAYINFO_H_

#include "stdafx.h"

class AdapterDisplayInfo
{
    public:
        AdapterDisplayInfo(LUID luid, std::wstring displayName);
        LUID GetLuid();
        std::wstring GetDisplayName();
        
    private:
        LUID m_luid;
        std::wstring m_displayName;
};

#endif