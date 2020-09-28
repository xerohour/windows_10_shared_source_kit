// Copyright(C) Microsoft.All rights reserved.

#include "common.h"


/* _____________________________________________________________________

    @TEST KSPROPERTY_AUDIOGFX_CAPTURETARGETDEVICEID
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIOGFX_CAPTURETARGETDEVICEID_1(PGAUDITPARAMS pParams)
//send NULL string
{

    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
           	return FNS_FAIL;
    }
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIOGFX_CAPTURETARGETDEVICEID_2(PGAUDITPARAMS pParams)
//One character string.
{

    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 1, 1);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
           	return FNS_FAIL;
    }
    return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_AUDIOGFX_RENDERTARGETDEVICEID
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIOGFX_RENDERTARGETDEVICEID_1(PGAUDITPARAMS pParams)
//send NULL string
{

    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
           	return FNS_FAIL;
    }
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIOGFX_RENDERTARGETDEVICEID_2(PGAUDITPARAMS pParams)
//One character string.
{

    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 1, 1);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
           	return FNS_FAIL;
    }
    return FNS_PASS;
}
