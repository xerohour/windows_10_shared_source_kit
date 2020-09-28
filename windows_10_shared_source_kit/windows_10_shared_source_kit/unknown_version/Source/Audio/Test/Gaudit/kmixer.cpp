//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       kmixer.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"

#define Bug_900074      // GAUDIT - Sysaudio (KMIXER) does not validate parameters send for property KSPROPERTY_HRTF3D_PARAMS
// when this bug gets fixed then undef it. I have kept test case here so that if it creates any carsh then we can find out.

void InitItdParams(PKSDS3D_ITD_PARAMS_MSG pItdParams)   {
    pItdParams->Enabled = TRUE;
    pItdParams->LeftParams.Channel = 0;
    pItdParams->LeftParams.Delay = 441;
    pItdParams->LeftParams.SmoothFrequency = 44000;
    pItdParams->LeftParams.TotalDryAttenuation = 0.09f;
    pItdParams->LeftParams.TotalWetAttenuation = 0.09f;
    pItdParams->LeftParams.VolSmoothScale = 1.f;
    pItdParams->RightParams.Channel = 1;
    pItdParams->RightParams.Delay = 0;
    pItdParams->RightParams.SmoothFrequency = 44000;
    pItdParams->RightParams.TotalDryAttenuation = 0.1f;
    pItdParams->RightParams.TotalWetAttenuation = 0.1f;
    pItdParams->RightParams.VolSmoothScale = 1.f;
    pItdParams->Reserved = 0;
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_ITD3D_PARAMS
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_ITD3D_PARAMS_1(PGAUDITPARAMS pParams)
//Sending invalid node id - 0x80000001
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        pParams->NodeProp.NodeId = 0x80000001;
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, 
							  pParams->cbSize, pParams->cbSize, 
							  sizeof(KSDS3D_ITD_PARAMS_MSG), sizeof(KSDS3D_ITD_PARAMS_MSG),
                              ALLOCATOR_GUARD_PAGE);
        PKSDS3D_ITD_PARAMS_MSG pItdParams;
        pItdParams = (PKSDS3D_ITD_PARAMS_MSG)gnt.GetData();
        InitItdParams(pItdParams);
        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_ITD3D_PARAMS_2(PGAUDITPARAMS pParams)
//Sending invalid node id - ULONG_MAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        pParams->NodeProp.NodeId = ULONG_MAX;
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, 
							  pParams->cbSize, pParams->cbSize, 
							  sizeof(KSDS3D_ITD_PARAMS_MSG), sizeof(KSDS3D_ITD_PARAMS_MSG),
                              ALLOCATOR_GUARD_PAGE);
        PKSDS3D_ITD_PARAMS_MSG pItdParams;
        pItdParams = (PKSDS3D_ITD_PARAMS_MSG)gnt.GetData();
        InitItdParams(pItdParams);
        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_ITD3D_PARAMS_3(PGAUDITPARAMS pParams)
//Passing NULL buffer
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, 
							  pParams->cbSize, pParams->cbSize, 
							  0, 0);
        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_ITD3D_PARAMS_4(PGAUDITPARAMS pParams)
//passing small buffer 
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, 
							  pParams->cbSize, pParams->cbSize, 
							  sizeof(KSDS3D_ITD_PARAMS_MSG)-1, sizeof(KSDS3D_ITD_PARAMS_MSG)-1,
                              ALLOCATOR_GUARD_PAGE);
        PKSDS3D_ITD_PARAMS_MSG pItdParams = (PKSDS3D_ITD_PARAMS_MSG) new BYTE [sizeof(KSDS3D_ITD_PARAMS_MSG)];
        InitItdParams(pItdParams);
        memcpy(gnt.GetData(), pItdParams, sizeof(KSDS3D_ITD_PARAMS_MSG)-1);
        gnt.Test(TRUE, 0);
        delete[] pItdParams;
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_ITD3D_PARAMS_5(PGAUDITPARAMS pParams)
//Passing large Buffer
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, 
							  pParams->cbSize, pParams->cbSize, 
							  sizeof(KSDS3D_ITD_PARAMS_MSG) +1, sizeof(KSDS3D_ITD_PARAMS_MSG) +1,
                              ALLOCATOR_GUARD_PAGE);
        PKSDS3D_ITD_PARAMS_MSG pItdParams = (PKSDS3D_ITD_PARAMS_MSG) new BYTE [sizeof(KSDS3D_ITD_PARAMS_MSG)];
        InitItdParams(pItdParams);
        memcpy(gnt.GetData(), pItdParams, sizeof(KSDS3D_ITD_PARAMS_MSG));
        gnt.Test(FALSE, 0);     //since it is a set request we do not expect any data written
        delete[] pItdParams;
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_ITD3D_PARAMS_6(PGAUDITPARAMS pParams)
//Passing Valid Data
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, 
							  pParams->cbSize, pParams->cbSize, 
							  sizeof(KSDS3D_ITD_PARAMS_MSG), sizeof(KSDS3D_ITD_PARAMS_MSG) ,
                              ALLOCATOR_GUARD_PAGE);
        PKSDS3D_ITD_PARAMS_MSG pItdParams = (PKSDS3D_ITD_PARAMS_MSG) new BYTE [sizeof(KSDS3D_ITD_PARAMS_MSG)];
        InitItdParams(pItdParams);
        memcpy(gnt.GetData(), pItdParams, sizeof(KSDS3D_ITD_PARAMS_MSG));
        gnt.Test(FALSE, 0);     //since it is a set request we do not expect any data written
        delete[] pItdParams;

		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_HRTF3D_PARAMS
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_HRTF3D_PARAMS_1(PGAUDITPARAMS pParams)
//   Invalid size
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, 
							  pParams->cbSize, pParams->cbSize, 
							  4, 4,
                              ALLOCATOR_GUARD_PAGE);
        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;

    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_HRTF3D_PARAMS_2(PGAUDITPARAMS pParams)
// Disable 3D    
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
							  sizeof(KSDS3D_HRTF_PARAMS_MSG), sizeof(KSDS3D_HRTF_PARAMS_MSG));

		PKSDS3D_HRTF_PARAMS_MSG msg = (PKSDS3D_HRTF_PARAMS_MSG)gnt.GetData(); 
		msg->CrossFadeOutput = 0;
		msg->SwapChannels = 1;
		msg->ZeroAzimuth = 0;
		msg->Size = sizeof(KSDS3D_HRTF_PARAMS_MSG);
		msg->FilterSize = 0;
		msg->Enabled = 0;

        gnt.Test(FALSE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
                            return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_HRTF3D_PARAMS_3(PGAUDITPARAMS pParams)
// Dynamic invalid struct size    
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
							  sizeof(KSDS3D_HRTF_PARAMS_MSG), sizeof(KSDS3D_HRTF_PARAMS_MSG));

		PKSDS3D_HRTF_PARAMS_MSG msg = (PKSDS3D_HRTF_PARAMS_MSG)gnt.GetData(); 
		msg->CrossFadeOutput = 0;
		msg->SwapChannels = 1;
		msg->ZeroAzimuth = 0;
		msg->Size = 1024;
		msg->FilterSize = 0;
		msg->Enabled = 1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
                return FNS_FAIL;
    }
	return FNS_PASS;
}


DWORD WINAPI Generic_Property_KSPROPERTY_HRTF3D_PARAMS_4(PGAUDITPARAMS pParams)
 //Dynamic invalid filter size
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
							  sizeof(KSDS3D_HRTF_PARAMS_MSG), sizeof(KSDS3D_HRTF_PARAMS_MSG));

		PKSDS3D_HRTF_PARAMS_MSG msg = (PKSDS3D_HRTF_PARAMS_MSG)gnt.GetData(); 
		msg->CrossFadeOutput = 0;
		msg->SwapChannels = 1;
		msg->ZeroAzimuth = 0;
		msg->Size = 0;
		msg->FilterSize = 1024;
		msg->Enabled = 1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
                            return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_HRTF3D_PARAMS_5(PGAUDITPARAMS pParams)
//Zero size struct    
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
							  sizeof(KSDS3D_HRTF_PARAMS_MSG), sizeof(KSDS3D_HRTF_PARAMS_MSG));

		PKSDS3D_HRTF_PARAMS_MSG msg = (PKSDS3D_HRTF_PARAMS_MSG)gnt.GetData(); 
		msg->CrossFadeOutput = 0;
		msg->SwapChannels = 1;
		msg->ZeroAzimuth = 0;
		msg->Size = 0;
		msg->FilterSize = 0;
		msg->Enabled = 1;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
                            return FNS_FAIL;
    }
	return FNS_PASS;
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_HRTF3D_INITIALIZE
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_HRTF3D_INITIALIZE_1(PGAUDITPARAMS pParams)
//Out buffer invalid size
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, 
							  pParams->cbSize, pParams->cbSize, 
							  4, 4,
                              ALLOCATOR_GUARD_PAGE);
        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_HRTF3D_FILTER_FORMAT
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_HRTF3D_FILTER_FORMAT_1(PGAUDITPARAMS pParams)
//Out buffer invalid size
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, 
							  pParams->cbSize, pParams->cbSize, 
							  4, 4,
                              ALLOCATOR_GUARD_PAGE);
        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}
