// Copyright(C) Microsoft.All rights reserved.

#include "common.h"

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DLISTENER_ALL
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ALL_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ALL_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ALL_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ALL_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Small_Buffer);

}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ALL_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Large_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ALL_6(PGAUDITPARAMS pParams)
//Invalid RolloffFactor - MIN-1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_LISTENER_ALL), sizeof(KSDS3D_LISTENER_ALL));

        ((PKSDS3D_LISTENER_ALL)gnt.GetData())->RolloffFactor = DS3D_MINROLLOFFFACTOR - 1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ALL_7(PGAUDITPARAMS pParams)
// Invalid RolloffFactor - MAX+1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_LISTENER_ALL), sizeof(KSDS3D_LISTENER_ALL));

        ((PKSDS3D_LISTENER_ALL)gnt.GetData())->RolloffFactor = DS3D_MAXROLLOFFFACTOR + 1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ALL_8(PGAUDITPARAMS pParams)
// Invalid DopplerFactor - MIN-1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_LISTENER_ALL), sizeof(KSDS3D_LISTENER_ALL));

        ((PKSDS3D_LISTENER_ALL)gnt.GetData())->DopplerFactor = DS3D_MINDOPPLERFACTOR - 1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ALL_9(PGAUDITPARAMS pParams)
// Invalid DopplerFactor - MAX+1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_LISTENER_ALL), sizeof(KSDS3D_LISTENER_ALL));

        ((PKSDS3D_LISTENER_ALL)gnt.GetData())->DopplerFactor = DS3D_MAXDOPPLERFACTOR + 1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ALL_10(PGAUDITPARAMS pParams)
// Invalid Distance Factor - (-1)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_LISTENER_ALL), sizeof(KSDS3D_LISTENER_ALL));

        ((PKSDS3D_LISTENER_ALL)gnt.GetData())->DistanceFactor = -1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          	return FNS_FAIL;
    }
	return FNS_PASS;
}


/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DLISTENER_DISTANCEFACTOR
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DISTANCEFACTOR_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DISTANCEFACTOR_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DISTANCEFACTOR_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  0, 0);
        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DISTANCEFACTOR_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DISTANCEFACTOR_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Large_Buffer);
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  0, 0);
        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Large_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR_6(PGAUDITPARAMS pParams)
// Invalid Doppler factor - MIN-1
{
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(FLOAT), sizeof(FLOAT));

        *(PFLOAT)gnt.GetData() =(FLOAT)DS3D_MINDOPPLERFACTOR - 1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR_7(PGAUDITPARAMS pParams)
// IInvalid Doppler factor - MAX+1
{
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(FLOAT), sizeof(FLOAT));

        *(PFLOAT)gnt.GetData() =(FLOAT)DS3D_MAXDOPPLERFACTOR - 1;
        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          	return FNS_FAIL;
    }
	return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DLISTENER_ORIENTATION
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ORIENTATION_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ORIENTATION_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ORIENTATION_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ORIENTATION_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Small_Buffer);

}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ORIENTATION_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Large_Buffer);
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DLISTENER_POSITION
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_POSITION_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_POSITION_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_POSITION_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_POSITION_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Small_Buffer);

}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_POSITION_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Large_Buffer);
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  0, 0);
        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Large_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR_6(PGAUDITPARAMS pParams)
// Invalid rolloff factor - MIN-1
{
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(FLOAT), sizeof(FLOAT));

        *(PFLOAT)gnt.GetData() =(FLOAT)DS3D_MINROLLOFFFACTOR - 1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR_7(PGAUDITPARAMS pParams)
// Invalid rolloff factor - MAX+1
{
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(FLOAT), sizeof(FLOAT));

        *(PFLOAT)gnt.GetData() =(FLOAT)DS3D_MAXROLLOFFFACTOR + 1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          	return FNS_FAIL;
    }
	return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DLISTENER_VELOCITY
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_VELOCITY_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_VELOCITY_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_VELOCITY_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_VELOCITY_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DLISTENER_VELOCITY_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DLISTENERNODEPROPERTY(pParams, Large_Buffer);
}