// Copyright(C) Microsoft.All rights reserved.

#include "common.h"


/* _____________________________________________________________________

    @TEST KSPROPERTY_TOPOLOGYNODE_ENABLE
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGYNODE_ENABLE_1(PGAUDITPARAMS pParams)
{
	return Generic_Property_KSNODEPROPERTYULONG_NodeID_ULONGMAX(pParams);
}

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGYNODE_ENABLE_2(PGAUDITPARAMS pParams)
{
    return Generic_Property_KSNODEPROPERTYULONG_NodeID_0x80000001(pParams);
}

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGYNODE_ENABLE_3(PGAUDITPARAMS pParams)
{
    return Generic_Property_KSNODEPROPERTYULONG_NULL_Buffer(pParams);
}

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGYNODE_ENABLE_4(PGAUDITPARAMS pParams)
{
    return Generic_Property_KSNODEPROPERTYULONG_Small_Buffer(pParams);
}

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGYNODE_ENABLE_5(PGAUDITPARAMS pParams)
{
    return Generic_Property_KSNODEPROPERTYULONG_Large_Buffer(pParams);
}


/* _____________________________________________________________________

    @TEST KSPROPERTY_TOPOLOGYNODE_RESET
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGYNODE_RESET_1(PGAUDITPARAMS pParams)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    return Generic_Property_KSNODEPROPERTYULONG_NodeID_ULONGMAX(pParams);
}

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGYNODE_RESET_2(PGAUDITPARAMS pParams)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    return Generic_Property_KSNODEPROPERTYULONG_NodeID_0x80000001(pParams);
}

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGYNODE_RESET_3(PGAUDITPARAMS pParams)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    return Generic_Property_KSNODEPROPERTYULONG_NULL_Buffer(pParams);
}

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGYNODE_RESET_4(PGAUDITPARAMS pParams)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    return Generic_Property_KSNODEPROPERTYULONG_Small_Buffer(pParams);
}

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGYNODE_RESET_5(PGAUDITPARAMS pParams)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    return Generic_Property_KSNODEPROPERTYULONG_Large_Buffer(pParams);
}