//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       sysaudio.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"

// Sysaudio stuff is XP only
#if (NTDDI_VERSION < NTDDI_VISTA)

/* ________________________________________________________________________________________

     Sysaudio functional handlers

   ________________________________________________________________________________________*/


ULONG GetSysaudioDevices(HANDLE hFile)
{
    KSPROPERTY prop = { 0 };

    prop.Set = KSPROPSETID_Sysaudio;
    prop.Id  = KSPROPERTY_SYSAUDIO_DEVICE_COUNT;
    prop.Flags = KSPROPERTY_TYPE_GET;

    CGenericIoControl gnt(hFile, &prop, sizeof(prop), sizeof(prop), sizeof(ULONG), sizeof(ULONG));

	*(LPDWORD)gnt.GetData() = 0;
    gnt.Test(FALSE, sizeof(ULONG));

    return *(LPDWORD)gnt.GetData();
}

BOOL SetSysaudioDeviceInfo(HANDLE hFile, ULONG nDevice)
{
    KSPROPERTY prop = { 0 };

    prop.Set   = KSPROPSETID_Sysaudio;
    prop.Id    = KSPROPERTY_SYSAUDIO_DEVICE_INSTANCE;
    prop.Flags = KSPROPERTY_TYPE_SET;

    CGenericIoControl gnt(hFile, &prop, sizeof(prop), sizeof(prop), sizeof(ULONG), sizeof(ULONG));

    *(LPDWORD)gnt.GetData() = nDevice;

    DWORD dwerr = gnt.Test(FALSE, 0);

    return (dwerr == 0);
}

BOOL SetSysaudioDeviceUncombined(HANDLE hFile, ULONG nDevice)
{
    SYSAUDIO_INSTANCE_INFO prop = { 0 };

    prop.Property.Set   = KSPROPSETID_Sysaudio;
    prop.Property.Id    = KSPROPERTY_SYSAUDIO_INSTANCE_INFO;
    prop.Property.Flags = KSPROPERTY_TYPE_SET;
    prop.DeviceNumber   = nDevice;
    prop.Flags = SYSAUDIO_FLAGS_DONT_COMBINE_PINS;

    CGenericIoControl gnt(hFile, (PKSPROPERTY)&prop, sizeof(prop), sizeof(prop), NULL, NULL);

    DWORD dwerr = gnt.Test(FALSE, 0);

    return (dwerr == 0);
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_SYSAUDIO_DEVICE_INTERFACE_NAME
   ____________________________________________________________________ */


/* ____________________________________________________________________

    @TEST KSPROPERTY_SYSAUDIO_DEVICE_FRIENDLY_NAME
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_DEVICE_INTERFACE_NAME_1(PGAUDITPARAMS pParams)
{
    ULONG cbGoodSize = pParams->cbSize + sizeof(ULONG);
    PKSPROPERTY pPropertyGood = (PKSPROPERTY)new BYTE[cbGoodSize];
    if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
        return FNS_FAIL;
	}


    memcpy(pPropertyGood, &pParams->Prop, pParams->cbSize);
    *(LPDWORD)(pPropertyGood + 1) = 0;

	if (pParams->Prop.Flags & KSPROPERTY_TYPE_TOPOLOGY)
	{
        CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize, PAGE / 2 - 1, PAGE / 2 - 1);
        DWORD dwResult = gnt.Test(TRUE, 0);
		if (dwResult == S_OK) // we don't care about actual result but just succeed or not
		{
			SLOG(eError, "ERROR: DeviceIoControl call should have failed");
			delete[] pPropertyGood;
			return FNS_FAIL;
		}

	}

	delete[] pPropertyGood;
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_DEVICE_INTERFACE_NAME_2(PGAUDITPARAMS pParams)
// sending small ouput buffer     
{
    ULONG cbGoodSize = pParams->cbSize + sizeof(ULONG);
    PKSPROPERTY pPropertyGood = (PKSPROPERTY)new BYTE[cbGoodSize];
    if (!pPropertyGood)	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
        return FNS_FAIL;
    }
    memcpy(pPropertyGood, &pParams->Prop, pParams->cbSize);
    *(LPDWORD)(pPropertyGood + 1) = 0;
    DWORD dwResult =FNS_FAIL;
    ULONG BufferSize = 0; 
    if ((pParams->Prop.Flags & KSPROPERTY_TYPE_GET) && !(pParams->Prop.Flags & KSPROPERTY_TYPE_TOPOLOGY))   {
        CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize, sizeof(ULONG), sizeof(ULONG));
        
        dwResult = gnt.Test(FALSE, sizeof(ULONG));
        if (gnt.GetFault() != FAULT_NO_FAULT)   {
		delete[] pPropertyGood;
		return FNS_FAIL;
        }
        BufferSize = gnt.GetWritten();
    }
    // first pass null buffer to get the required size of buffer  
    if ((dwResult == ERROR_INSUFFICIENT_BUFFER) || (dwResult== ERROR_MORE_DATA))    {
        CGenericIoControl gnt1(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, BufferSize-1, BufferSize-1);
        dwResult = gnt1.Test(TRUE, 0);
        if (gnt1.GetFault() != FAULT_NO_FAULT)   {
            delete[] pPropertyGood;
    	    return FNS_FAIL;
        }
    }
    delete[] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_DEVICE_INTERFACE_NAME_3(PGAUDITPARAMS pParams)
//sending null string    
{
    // Setup good parameter
    ULONG cbGoodSize = pParams->cbSize + sizeof(ULONG);
    PKSPROPERTY pPropertyGood = (PKSPROPERTY)new BYTE[cbGoodSize];
    if (!pPropertyGood)
    {
        SLOG(eError, "ERROR: Internal memory allocation failure");
        return FNS_FAIL;
    }
    memcpy(pPropertyGood, &pParams->Prop, pParams->cbSize);
    *(LPDWORD)(pPropertyGood + 1) = 0;


    {
        CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize, 0, 0);

        DWORD dwResult = gnt.Test(TRUE, (ULONG)-1);
	if (gnt.GetFault() != FAULT_NO_FAULT)
	{       
   	    delete[] pPropertyGood;
	    return FNS_FAIL;
        }

    }

    delete[] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_DEVICE_INTERFACE_NAME_4(PGAUDITPARAMS pParams)
{
    // size too small
    ULONG cbGoodSize = pParams->cbSize + sizeof(ULONG);
    PKSPROPERTY pPropertyGood = (PKSPROPERTY)new BYTE[cbGoodSize];
    if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
        return FNS_FAIL;
	}
    memcpy(pPropertyGood, &pParams->Prop, pParams->cbSize);
    *(LPDWORD)(pPropertyGood + 1) = 0;

	{
        CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize, 2, ULONG_MAX);
        DWORD dwResult = gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			delete[] pPropertyGood;
			return FNS_FAIL;
		}

	}

	delete[] pPropertyGood;
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_DEVICE_INTERFACE_NAME_5(PGAUDITPARAMS pParams)
{
    // one char string should fail for all flags
    ULONG cbGoodSize = pParams->cbSize + sizeof(ULONG);
    PKSPROPERTY pPropertyGood = (PKSPROPERTY)new BYTE[cbGoodSize];
    if (!pPropertyGood) {
        SLOG(eError, "ERROR: Internal memory allocation failure");
        return FNS_FAIL;
    }
    memcpy(pPropertyGood, &pParams->Prop, pParams->cbSize);
    *(LPDWORD)(pPropertyGood + 1) = 0;

	{
        CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize, 1, 1);
        DWORD dwResult = gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			delete[] pPropertyGood;
			return FNS_FAIL;
		}

	}

	delete[] pPropertyGood;
	return FNS_PASS;
}


DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_DEVICE_INTERFACE_NAME_6(PGAUDITPARAMS pParams)
{
    // invalid device should fail always
    ULONG cbGoodSize = pParams->cbSize + sizeof(ULONG);
    PKSPROPERTY pPropertyGood = (PKSPROPERTY)new BYTE[cbGoodSize];
    if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
        return FNS_FAIL;
	}
    memcpy(pPropertyGood, &pParams->Prop, pParams->cbSize);
    *(LPDWORD)(pPropertyGood + 1) = ULONG_MAX - 1;

	{
        CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize, 0, 0);
        DWORD dwResult = gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			delete[] pPropertyGood;
			return FNS_FAIL;
		}

	}

	delete[] pPropertyGood;
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_DEVICE_INTERFACE_NAME_7(PGAUDITPARAMS pParams)
{
    // get length
    ULONG cbGoodSize = pParams->cbSize + sizeof(ULONG);
    PKSPROPERTY pPropertyGood = (PKSPROPERTY)new BYTE[cbGoodSize];
    if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
        return FNS_FAIL;
	}
    memcpy(pPropertyGood, &pParams->Prop, pParams->cbSize);
    *(LPDWORD)(pPropertyGood + 1) = 0;

    if ((pParams->Prop.Flags & KSPROPERTY_TYPE_GET) && !(pParams->Prop.Flags & KSPROPERTY_TYPE_TOPOLOGY))
	{
        CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize, sizeof(ULONG), sizeof(ULONG));
        DWORD dwResult;
        dwResult = gnt.Test(FALSE, sizeof(ULONG));

		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			delete[] pPropertyGood;
			return FNS_FAIL;
		}

	}

	delete[] pPropertyGood;
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_DEVICE_INTERFACE_NAME_8(PGAUDITPARAMS pParams)
{
    // get all string but unaligned buffer
    ULONG cbGoodSize = pParams->cbSize + sizeof(ULONG);
    PKSPROPERTY pPropertyGood = (PKSPROPERTY)new BYTE[cbGoodSize];
    if (!pPropertyGood)
    {
        SLOG(eError, "ERROR: Internal memory allocation failure");
        return FNS_FAIL;
    }
    memcpy(pPropertyGood, &pParams->Prop, pParams->cbSize);
    *(LPDWORD)(pPropertyGood + 1) = 0;

    if ((pParams->Prop.Flags & KSPROPERTY_TYPE_GET) && !(pParams->Prop.Flags & KSPROPERTY_TYPE_TOPOLOGY))
    {
        CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize, 
                             PAGE/4, PAGE/4, 
                             ALLOCATOR_STATIC_UNALIGNED);
        DWORD dwResult;
        dwResult = gnt.Test(FALSE, (ULONG)-1);

	if (gnt.GetFault() != FAULT_NO_FAULT) 
	{
   	    delete[] pPropertyGood;
	    return FNS_FAIL;
        }

    }
    delete[] pPropertyGood;
    return FNS_PASS;
}


/* ____________________________________________________________________

    @TEST KSPROPERTY_SYSAUDIO_RENDER_PIN_INSTANCES
   ____________________________________________________________________ */
DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_RENDER_PIN_INSTANCES_1(PGAUDITPARAMS pParams)
{
    // Rejects buffer too small 
    {
        CGenericIoControl gnt(pParams->hFile,&pParams->Prop, pParams->cbSize, pParams->cbSize, 4, 4);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_RENDER_PIN_INSTANCES_2(PGAUDITPARAMS pParams)
{
    // Rejects buffer too small 
    {
        CGenericIoControl gnt(pParams->hFile,&pParams->Prop, sizeof(KSPIN_CINSTANCES), sizeof(KSPIN_CINSTANCES), 4, 4);

        gnt.Test(FALSE, sizeof(KSPIN_CINSTANCES));
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
    }

	return FNS_PASS;
}


// KSPROPERTY_SYSAUDIO_CREATE_VIRTUAL_SOURCE_ONLY
// KSPROPERTY_SYSAUDIO_ALWAYS_CREATE_VIRTUAL_SOURCE
/* ____________________________________________________________________

    @TEST KSPROPERTY_SYSAUDIO_CREATE_VIRTUAL_SOURCE
   ____________________________________________________________________ */
DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_CREATE_VIRTUAL_SOURCE_1(PGAUDITPARAMS pParams)
{
	if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)	
		return FNS_FAIL;

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_CREATE_VIRTUAL_SOURCE_2(PGAUDITPARAMS pParams)
{
    // Rejects buffer too small 
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, sizeof(ULONG), sizeof(ULONG));

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_CREATE_VIRTUAL_SOURCE_3(PGAUDITPARAMS pParams)
{
    // Build a proper buffer for this property
    ULONG cbGoodSize = sizeof(SYSAUDIO_CREATE_VIRTUAL_SOURCE);
    PSYSAUDIO_CREATE_VIRTUAL_SOURCE pPropertyGood = (PSYSAUDIO_CREATE_VIRTUAL_SOURCE)LocalAlloc(0, sizeof(SYSAUDIO_CREATE_VIRTUAL_SOURCE));
    if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
        return FNS_FAIL; 
	}
    memcpy(pPropertyGood, &pParams->Prop, pParams->cbSize);
    if (S_OK != CoCreateGuid(&pPropertyGood->PinCategory))
	{
		SLOG(eInfo1, "WARNING: Could not generate a new guid");
	    LocalFree(pPropertyGood);
		return FNS_PASS;
	}
    if (S_OK != CoCreateGuid(&pPropertyGood->PinName))
	{
		SLOG(eInfo1, "WARNING: Could not generate a new guid");
	    LocalFree(pPropertyGood);
		return FNS_PASS;
	}

	if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)	
    {
        CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize, sizeof(ULONG), sizeof(ULONG));

        gnt.Test(FALSE, sizeof(ULONG));
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
		    LocalFree(pPropertyGood);
			return FNS_FAIL;
		}
    }

    LocalFree(pPropertyGood);
	return FNS_PASS;
}


/* ____________________________________________________________________

    @TEST KSPROPERTY_SYSAUDIO_PREFERRED_DEVICE
   ____________________________________________________________________ */
DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_PREFERRED_DEVICE_1(PGAUDITPARAMS pParams)
{
	REFER_FORMAL_PARAM(pParams);
	return FNS_FAIL;
}


/* ____________________________________________________________________

    @TEST KSPROPERTY_SYSAUDIO_SELECT_GRAPH
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_SELECT_GRAPH_3(PGAUDITPARAMS pParams)
{
	
    // Run valid select graph multiple times for all nodes and/or pins
    {
   
        SYSAUDIO_SELECT_GRAPH GoodProperty = { 0 };
        ULONG cbGoodSize  = sizeof(SYSAUDIO_SELECT_GRAPH);
        memcpy(&GoodProperty, &pParams->Prop, pParams->cbSize);
    
        TTNode<CKsPin>* ppinNode = FILTER()->m_listPins.GetHead();
        while (ppinNode)
        {
            TTNode<CKsNode>* pnodeNode = FILTER()->m_listNodes.GetHead();
            while (pnodeNode)
            {
                GoodProperty.PinId = ppinNode->pData->m_nId;
                GoodProperty.NodeId = pnodeNode->pData->m_nId;

                CGenericIoControl gnt(pParams->hFile, (PKSPROPERTY)&GoodProperty, cbGoodSize, cbGoodSize, 
                                      0, 0);

                gnt.Test((BOOL)-1, 0);
				if (gnt.GetFault() != FAULT_NO_FAULT && gnt.GetFault() != FAULT_DIFFERENT_RESULT) 
				{
					return FNS_FAIL;
				}

                // select same node second time
                CGenericIoControl gnt_second(pParams->hFile, (PKSPROPERTY)&GoodProperty, cbGoodSize, cbGoodSize, 
                                      0, 0);

                gnt_second.Test((BOOL)-1, 0);
				if (gnt_second.GetFault() != FAULT_NO_FAULT && gnt_second.GetFault() != FAULT_DIFFERENT_RESULT) 
				{
					return FNS_FAIL;
				}

                pnodeNode = FILTER()->m_listNodes.GetNext(pnodeNode);
            }    
            ppinNode = FILTER()->m_listPins.GetNext(ppinNode);
        }
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_SELECT_GRAPH_1(PGAUDITPARAMS pParams)
{

    // Invalid node select graph
    {
        SYSAUDIO_SELECT_GRAPH GoodProperty = { 0 };
        ULONG cbGoodSize  = sizeof(SYSAUDIO_SELECT_GRAPH);
        memcpy(&GoodProperty, &pParams->Prop, pParams->cbSize);
        GoodProperty.PinId  = 0;
        GoodProperty.NodeId = ULONG_MAX;

        CGenericIoControl gnt(pParams->hFile, (PKSPROPERTY)&GoodProperty, cbGoodSize, cbGoodSize, 
                              0, 0);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_SELECT_GRAPH_2(PGAUDITPARAMS pParams)
{

    // Invalid pin select graph
    {
        SYSAUDIO_SELECT_GRAPH GoodProperty = { 0 };
        ULONG cbGoodSize  = sizeof(SYSAUDIO_SELECT_GRAPH);
        memcpy(&GoodProperty, &pParams->Prop, pParams->cbSize);
        GoodProperty.PinId  = ULONG_MAX;
        GoodProperty.NodeId = 0;

        CGenericIoControl gnt(pParams->hFile, (PKSPROPERTY)&GoodProperty, cbGoodSize, cbGoodSize, 
                              0, 0);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
    }

	return FNS_PASS;
}

/* ____________________________________________________________________

    @TEST KSPROPERTY_SYSAUDIO_ADDREMOVE_GFX
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_ADDREMOVE_GFX_1(PGAUDITPARAMS pParams)
{
	
       // Bad pin handle
	{
		CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 
							sizeof(SYSAUDIO_GFX) + sizeof(WCHAR)*4, sizeof(SYSAUDIO_GFX) + sizeof(WCHAR)*4);

		PSYSAUDIO_GFX    pgfx = (PSYSAUDIO_GFX)gnt.GetData();
		memset(pgfx, 0, sizeof(SYSAUDIO_GFX));
		pgfx->ulType = GFX_DEVICETYPE_RENDER;
		pgfx->Enable = 1;
		pgfx->hGfx = CreateEvent(NULL, FALSE, TRUE, NULL);
              pgfx->ulDeviceNameOffset = pgfx->ulFriendlyNameOffset = sizeof(SYSAUDIO_GFX);  
              memcpy(pgfx + 1, L"\\\\\?\\", sizeof(WCHAR)*4);
              
		if (pgfx->hGfx)
		{
			gnt.Test(TRUE, 0);
			CloseHandle(pgfx->hGfx);
			if (gnt.GetFault() != FAULT_NO_FAULT) 
			{
				return FNS_FAIL;
			}
		}

	}

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_ADDREMOVE_GFX_2(PGAUDITPARAMS pParams)
{
	
	{
		CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 
								sizeof(SYSAUDIO_GFX) , sizeof(SYSAUDIO_GFX) );

		PSYSAUDIO_GFX    pgfx = (PSYSAUDIO_GFX)gnt.GetData();
		memset(pgfx, 0, sizeof(SYSAUDIO_GFX));
		pgfx->ulType = GFX_DEVICETYPE_RENDER;
              pgfx->Enable = 1;
              pgfx->hGfx = FILTER()->m_handle;
              pgfx->ulDeviceNameOffset = pgfx->ulFriendlyNameOffset = sizeof(SYSAUDIO_GFX);  

              // nothing is allocated after the structure
		if (pgfx->hGfx)
		{
			gnt.Test(TRUE, 0);
			if (gnt.GetFault() != FAULT_NO_FAULT) 
			{
				return FNS_FAIL;
			}
		}

	}

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_ADDREMOVE_GFX_3(PGAUDITPARAMS pParams)
{
	
	{
		CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 
								sizeof(SYSAUDIO_GFX), sizeof(SYSAUDIO_GFX));

		PSYSAUDIO_GFX    pgfx = (PSYSAUDIO_GFX)gnt.GetData();
		memset(pgfx, 0, sizeof(SYSAUDIO_GFX));
		pgfx->ulType = GFX_DEVICETYPE_RENDER;
              pgfx->Enable = 1;
              pgfx->hGfx = FILTER()->m_handle;

              // create likely to bugcheck offsets
              pgfx->ulDeviceNameOffset = pgfx->ulFriendlyNameOffset = (ULONG)(ULONG_MAX - (ULONG_PTR)pgfx); 

		if (pgfx->hGfx)
		{
			gnt.Test(TRUE, 0);
			if (gnt.GetFault() != FAULT_NO_FAULT) 
			{
				return FNS_FAIL;
			}
		}

	}

	return FNS_PASS;
}


/* ____________________________________________________________________

    @TEST KSPROPERTY-SYSAUDIO_ATTACH_VIRTUAL_SOURCE
   ____________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_ATTACH_VIRTUAL_SOURCE_1(PGAUDITPARAMS pParams)
{
	if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
		return FNS_PASS;

	{
		SYSAUDIO_ATTACH_VIRTUAL_SOURCE avs = { 0 };
 	
		memcpy(&avs, &pParams->Prop, pParams->cbSize);
        ULONG cbGoodSize = sizeof(SYSAUDIO_ATTACH_VIRTUAL_SOURCE);
        avs.MixerPinId = ULONG_MAX - 1;
        CGenericIoControl gnt(pParams->hFile, (PKSPROPERTY)&avs, cbGoodSize, cbGoodSize, 
                              sizeof(ULONG), sizeof(ULONG));
        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
		{
			return FNS_FAIL;
		}
    }
	
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYSAUDIO_ATTACH_VIRTUAL_SOURCE_2(PGAUDITPARAMS pParams)
{
	if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
		return FNS_PASS;

	{
		if (!PIN())
			return FNS_PASS;

        SYSAUDIO_CREATE_VIRTUAL_SOURCE cvs = { 0 };
        
        cvs.Property.Set = KSPROPSETID_Sysaudio;
        cvs.Property.Id =  KSPROPERTY_SYSAUDIO_CREATE_VIRTUAL_SOURCE;
        cvs.Property.Flags = KSPROPERTY_TYPE_GET;
        cvs.PinCategory = GUID_NULL;
        cvs.PinName = GUID_NULL;
   
        CGenericIoControl gnt(pParams->hFile, &cvs, sizeof(cvs), sizeof(cvs), 
                              sizeof(ULONG), sizeof(ULONG));

        DWORD dwerr = gnt.Test(FALSE, sizeof(ULONG));
        ULONG ulMixerId = *(ULONG*)gnt.GetData();
        
        if (!dwerr)
        {
            SYSAUDIO_ATTACH_VIRTUAL_SOURCE avs = { 0 };
            
            memcpy(&avs, &pParams->Prop, pParams->cbSize);
            ULONG cbGoodSize = sizeof(SYSAUDIO_ATTACH_VIRTUAL_SOURCE);
            avs.MixerPinId = ulMixerId;
            CGenericIoControl gnt1(pParams->hFile, (PKSPROPERTY)&avs, cbGoodSize, cbGoodSize, 
                                  0, 0);

            gnt1.Test(FALSE, 0);
			if (gnt1.GetFault() != FAULT_NO_FAULT) 
			{
				return FNS_FAIL;
			}
        }
    }
	
	return FNS_PASS;
}

#endif // NTDDI_VERSION < NTDDI_VISTA