// Copyright (C) Microsoft. All rights reserved.
#include "common.h"
#include "resmgr.h"
#include "waveres.h"
#include "resource.h"
#include <math.h>

// Preferrable to pass in be HardwareId or PnpId
//   however, it can be FilterName
BOOL IsUsb(LPTSTR szPnpId)
{
    const TCHAR szSearch[] = _T("\\usb");
    const UINT  uLenSearch = sizeof(szSearch)/sizeof(szSearch[0]) - 1;
    
    UINT        uLenTotal  = _tcslen(szPnpId);
    UINT        uLenCheck = 
                 ((uLenTotal > uLenSearch) ? (uLenTotal - uLenSearch + 1) : 0);

    // If it is a HardwareId, then USB will show up in the beginning
    // and will not be preceded by backslash
    if (0 == _tcsnicmp(szPnpId, szSearch + 1, uLenSearch - 1))
    {
        return (TRUE);
    }

    for (UINT iOffset = 0; iOffset < uLenCheck; iOffset++)
    {
        if (0 == _tcsnicmp(&(szPnpId[iOffset]), szSearch, uLenSearch))
        {
            return (TRUE);
        }
    }
    
    //  Never found...
    return (FALSE);
}
