/*++

Copyright (c) Microsoft Corporation

Module Name:

    ucxdbg.h

Abstract:

    
Environment:

    Kernel mode

Revision History:

--*/

#include "dbg.h"

#define WPP_CONTROL_GUIDS\
    WPP_DEFINE_CONTROL_GUID(UCX,(6fb6e467,9ed4,4b73,8c22,70b97e22c7d9),\
    WPP_DEFINE_BIT(FlagController)\
    WPP_DEFINE_BIT(FlagEndpoint)\
    WPP_DEFINE_BIT(FlagRootHub)\
    WPP_DEFINE_BIT(FlagHubInterface)\
    WPP_DEFINE_BIT(FlagUsbDevice)\
    WPP_DEFINE_BIT(FlagUrb)\
    WPP_DEFINE_BIT(FlagBusInterfaces)\
    WPP_DEFINE_BIT(FlagUcx)\
    WPP_DEFINE_BIT(FlagUsbdi)\
    WPP_DEFINE_BIT(FlagStreams)\
    WPP_DEFINE_BIT(FlagWmi)\
    )

