// Copyright(C) Microsoft.All rights reserved.

#include "common.h"


/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DBUFFER_ALL
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Large_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_6(PGAUDITPARAMS pParams)
// Invalid Mode - ULONG_MAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_BUFFER_ALL), sizeof(KSDS3D_BUFFER_ALL));
        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->Mode = ULONG_MAX;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_7(PGAUDITPARAMS pParams)
// Invalid Mode - 0x80000001
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)// This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_BUFFER_ALL), sizeof(KSDS3D_BUFFER_ALL));
        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->Mode = 0x80000001;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_8(PGAUDITPARAMS pParams)
//Invalid Inside Cone angles - MIN-1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_BUFFER_ALL), sizeof(KSDS3D_BUFFER_ALL));

        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->InsideConeAngle = (ULONG)DS3D_MINCONEANGLE - 1;
        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->OutsideConeAngle = DS3D_DEFAULTCONEANGLE;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_9(PGAUDITPARAMS pParams)
// Invalid Inside Cone angles - MAX+1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_BUFFER_ALL), sizeof(KSDS3D_BUFFER_ALL));

        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->InsideConeAngle = DS3D_MAXCONEANGLE + 1;
        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->OutsideConeAngle = DS3D_DEFAULTCONEANGLE;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_10(PGAUDITPARAMS pParams)
//Invalid Outside Cone angles - MIN-1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_BUFFER_ALL), sizeof(KSDS3D_BUFFER_ALL));

        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->OutsideConeAngle = (ULONG)DS3D_MINCONEANGLE-1;
        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->InsideConeAngle = DS3D_DEFAULTCONEANGLE;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_11(PGAUDITPARAMS pParams)
// Invalid Outside Cone angles - MAX+1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_BUFFER_ALL), sizeof(KSDS3D_BUFFER_ALL));

        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->OutsideConeAngle = DS3D_MAXCONEANGLE+1;
        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->InsideConeAngle = DS3D_DEFAULTCONEANGLE;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_12(PGAUDITPARAMS pParams)
// Invalid Min Distance - (-1)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_BUFFER_ALL), sizeof(KSDS3D_BUFFER_ALL));

        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->MinDistance = -1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_ALL_13(PGAUDITPARAMS pParams)
// Invalid Max Distance - (-1)
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(KSDS3D_BUFFER_ALL), sizeof(KSDS3D_BUFFER_ALL));

        ((PKSDS3D_BUFFER_ALL)gnt.GetData())->MaxDistance = -1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Large_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES_6(PGAUDITPARAMS pParams)
//Invalid Inside Cone angles - MIN-1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                               sizeof(KSDS3D_BUFFER_CONE_ANGLES), sizeof(KSDS3D_BUFFER_CONE_ANGLES));

		((PKSDS3D_BUFFER_CONE_ANGLES)gnt.GetData())->InsideConeAngle = (ULONG)DS3D_MINCONEANGLE - 1;
        ((PKSDS3D_BUFFER_CONE_ANGLES)gnt.GetData())->OutsideConeAngle = DS3D_DEFAULTCONEANGLE;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES_7(PGAUDITPARAMS pParams)
// Invalid Inside Cone angles - MAX+1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                               sizeof(KSDS3D_BUFFER_CONE_ANGLES), sizeof(KSDS3D_BUFFER_CONE_ANGLES));

        ((PKSDS3D_BUFFER_CONE_ANGLES)gnt.GetData())->InsideConeAngle = DS3D_MAXCONEANGLE + 1;
        ((PKSDS3D_BUFFER_CONE_ANGLES)gnt.GetData())->OutsideConeAngle = DS3D_DEFAULTCONEANGLE;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES_8(PGAUDITPARAMS pParams)
//Invalid Outside Cone angles - MIN-1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                               sizeof(KSDS3D_BUFFER_CONE_ANGLES), sizeof(KSDS3D_BUFFER_CONE_ANGLES));

        ((PKSDS3D_BUFFER_CONE_ANGLES)gnt.GetData())->OutsideConeAngle = (ULONG)DS3D_MINCONEANGLE - 1;
        ((PKSDS3D_BUFFER_CONE_ANGLES)gnt.GetData())->InsideConeAngle = DS3D_DEFAULTCONEANGLE;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES_9(PGAUDITPARAMS pParams)
// Invalid Outside Cone angles - MAX+1
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) // This test is only for SET
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                               sizeof(KSDS3D_BUFFER_CONE_ANGLES), sizeof(KSDS3D_BUFFER_CONE_ANGLES));

        ((PKSDS3D_BUFFER_CONE_ANGLES)gnt.GetData())->OutsideConeAngle = DS3D_MAXCONEANGLE + 1;
        ((PKSDS3D_BUFFER_CONE_ANGLES)gnt.GetData())->InsideConeAngle = DS3D_DEFAULTCONEANGLE;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DBUFFER_CONEORIENTATION
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEORIENTATION_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEORIENTATION_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEORIENTATION_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEORIENTATION_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Small_Buffer);

}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_CONEORIENTATION_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Large_Buffer);
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DBUFFER_MAXDISTANCE
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MAXDISTANCE_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MAXDISTANCE_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MAXDISTANCE_3(PGAUDITPARAMS pParams)
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

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MAXDISTANCE_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MAXDISTANCE_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Large_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MAXDISTANCE_6(PGAUDITPARAMS pParams)
// Invalid Max Distance - (-1)
{
	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
												  sizeof(FLOAT), sizeof(FLOAT));
		*((PFLOAT)gnt.GetData()) = -1;

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
	return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DBUFFER_MINDISTANCE
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MINDISTANCE_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MINDISTANCE_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MINDISTANCE_3(PGAUDITPARAMS pParams)
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

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MINDISTANCE_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MINDISTANCE_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Large_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_MINDISTANCE_6(PGAUDITPARAMS pParams)
// Invalid Min Distance - (-1)
{
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize,
                                                  sizeof(FLOAT), sizeof(FLOAT));
        *((PFLOAT)gnt.GetData()) = -1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DBUFFER_POSITION
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_POSITION_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_POSITION_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_POSITION_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_POSITION_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Small_Buffer);

}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_POSITION_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Large_Buffer);
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_DIRECTSOUND3DBUFFER_VELOCITY
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_VELOCITY_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_VELOCITY_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_VELOCITY_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_VELOCITY_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Small_Buffer);

}

DWORD WINAPI Generic_Property_KSPROPERTY_DIRECTSOUND3DBUFFER_VELOCITY_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KS3DBUFFERNODEPROPERTY(pParams, Large_Buffer);
}