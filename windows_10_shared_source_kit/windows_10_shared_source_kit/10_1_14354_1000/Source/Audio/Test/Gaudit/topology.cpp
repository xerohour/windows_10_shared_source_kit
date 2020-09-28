//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       topology.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"


/* _____________________________________________________________________

    @TEST KSPROPERTY_TOPOLOGY_NAME
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGY_NAME_1(PGAUDITPARAMS pParams)
//sends NULL string buffer for result
{

    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;

    // Rejects NULL string call
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);

        gnt.Test(TRUE, 0);
	if (gnt.GetFault() != FAULT_NO_FAULT) 
           return FNS_FAIL;
    }
 
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGY_NAME_2(PGAUDITPARAMS pParams)
{

    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;

    // Rejects one char string call
    {
        // There are some slight chances for this to give false error if the return string is
	// an empty byte string
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 1, 1);

        gnt.Test(TRUE, 0);
	if (gnt.GetFault() != FAULT_NO_FAULT) 
          return FNS_FAIL;
    }
 
    return FNS_PASS;
}

/* _____________________________________________________________________

    @KSPROPERTY_TOPOLOGY_CATEGORIES
   ______________________________________________________________________ */


DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGY_CATEGORIES_1(PGAUDITPARAMS pParams)
// NULL Output Buffer
{

    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;
    {

        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          return FNS_FAIL;
    }

    return FNS_PASS;
}
DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGY_CATEGORIES_2(PGAUDITPARAMS pParams)
// Output Buffer Small
{



    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;
    {

        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSMULTIPLE_ITEM)-1, sizeof(KSMULTIPLE_ITEM)-1);
	    gnt.Test(TRUE, 0);

		if (gnt.GetFault() != FAULT_NO_FAULT)
          return FNS_FAIL;
    }

    return FNS_PASS;
}
DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGY_CATEGORIES_3(PGAUDITPARAMS pParams)
// Output Buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;
    {
        //
        //There are 12 possible supportes categories (GUIDs). We are passing a large buffer
        // sufficient for 14 GUIDs.
        //
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                              sizeof(KSMULTIPLE_ITEM) +(14 * sizeof(GUID)),
                              sizeof(KSMULTIPLE_ITEM) +(14 * sizeof(GUID)));

        //
        //The test case passes. but, we can't be sure of the number of bytes the call returns
        //
        gnt.Test(FALSE, (ULONG)-1);
		if (gnt.GetFault() != FAULT_NO_FAULT)
   			return FNS_FAIL;
   	}
    return FNS_PASS;
}

/* _____________________________________________________________________

    @KSPROPERTY_TOPOLOGY_CONNECTIONS
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGY_CONNECTIONS_1(PGAUDITPARAMS pParams)
// Output Buffer NULL
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          return FNS_FAIL;
    }

    return FNS_PASS;
}
DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGY_CONNECTIONS_2(PGAUDITPARAMS pParams)
// Output Buffer Small
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize,
                                   pParams->cbSize, sizeof(KSMULTIPLE_ITEM)-1,sizeof(KSMULTIPLE_ITEM)-1);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          return FNS_FAIL;
    }

    return FNS_PASS;
}
DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGY_CONNECTIONS_3(PGAUDITPARAMS pParams)
// Out Buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;

    {
    	//
    	// Assuming 100 connections as a large number
    	//
    	CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                   sizeof(KSMULTIPLE_ITEM)+(100 * sizeof(KSTOPOLOGY_CONNECTION)),
                                                   sizeof(KSMULTIPLE_ITEM)+(100 * sizeof(KSTOPOLOGY_CONNECTION)));

        gnt.Test(FALSE, (ULONG)-1);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }

    return FNS_PASS;
}

/* _____________________________________________________________________

    @KSPROPERTY_TOPOLOGY_NODES
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGY_NODES_1(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }

    return FNS_PASS;
}
DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGY_NODES_2(PGAUDITPARAMS pParams)
// Out Buffer small
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                     sizeof(KSMULTIPLE_ITEM)-1, sizeof(KSMULTIPLE_ITEM)-1);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          return FNS_FAIL;
    }

    return FNS_PASS;
}
DWORD WINAPI Generic_Property_KSPROPERTY_TOPOLOGY_NODES_3(PGAUDITPARAMS pParams)
// Out Buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                     sizeof(KSMULTIPLE_ITEM)+ (100 * sizeof(GUID)),
                                                     sizeof(KSMULTIPLE_ITEM)+ (100 * sizeof(GUID)));

        gnt.Test(FALSE, (ULONG)-1);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          return FNS_FAIL;
    }

    return FNS_PASS;
}    
