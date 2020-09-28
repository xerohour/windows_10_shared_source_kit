// Copyright(C) Microsoft.All rights reserved.

#include "common.h"
#include "drmstruct.h"
#include "drmaudio.h"

/* _____________________________________________________________________

    @TEST KSPROPERTY_DRMAUDIOSTREAM_CONTENTID
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_DRMAUDIOSTREAM_CONTENTID_1(PGAUDITPARAMS pParams)
// Invalid ContentID - ULONGMAX
{

   	ULONG cbGoodSize = sizeof(KSP_DRMAUDIOSTREAM_CONTENTID);
	PKSP_DRMAUDIOSTREAM_CONTENTID  pPropertyGood = (PKSP_DRMAUDIOSTREAM_CONTENTID)new BYTE[cbGoodSize];

	{
		memcpy(&(pPropertyGood->Property) ,&pParams->Prop, pParams->cbSize);
		pPropertyGood->Context 							= new BYTE[sizeof(DRMFORWARD)];
		pPropertyGood->DrmAddContentHandlers	 		= NULL;
		pPropertyGood->DrmCreateContentMixed			= NULL;
		pPropertyGood->DrmDestroyContent				= NULL;
		pPropertyGood->DrmForwardContentToDeviceObject	= NULL;
		pPropertyGood->DrmForwardContentToFileObject	= NULL;
		pPropertyGood->DrmForwardContentToInterface		= NULL;
		pPropertyGood->DrmGetContentRights				= NULL;

		CGenericIoControl gnt(pParams->hFile, pPropertyGood , cbGoodSize, cbGoodSize,
		                     sizeof(KSDRMAUDIOSTREAM_CONTENTID), sizeof(KSDRMAUDIOSTREAM_CONTENTID));

		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.CopyProtect = FALSE;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.Reserved  	= 0;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.DigitalOutputDisable = FALSE;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->ContentId = ULONG_MAX;

		//
		// The driver should return status INVALID_DEVICE_REQUEST, since the request was initiated
		// from user mode
		//
		gnt.Test(TRUE, 0);

		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
		  delete[] pPropertyGood->Context;
		  delete[] pPropertyGood;
		  return FNS_FAIL;
	  	}
	}

    delete[] pPropertyGood->Context;
    delete[] pPropertyGood;

    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DRMAUDIOSTREAM_CONTENTID_2(PGAUDITPARAMS pParams)
// Invalid ContentID - 0x80000001
{

   	ULONG cbGoodSize = sizeof(KSP_DRMAUDIOSTREAM_CONTENTID);
	PKSP_DRMAUDIOSTREAM_CONTENTID  pPropertyGood = (PKSP_DRMAUDIOSTREAM_CONTENTID)new BYTE[cbGoodSize];
	{
		memcpy(&(pPropertyGood->Property) ,&pParams->Prop, pParams->cbSize);
		pPropertyGood->Context 							= new BYTE[sizeof(DRMFORWARD)];
		pPropertyGood->DrmAddContentHandlers	 		= NULL;
		pPropertyGood->DrmCreateContentMixed			= NULL;
		pPropertyGood->DrmDestroyContent				= NULL;
		pPropertyGood->DrmForwardContentToDeviceObject	= NULL;
		pPropertyGood->DrmForwardContentToFileObject	= NULL;
		pPropertyGood->DrmForwardContentToInterface		= NULL;
		pPropertyGood->DrmGetContentRights				= NULL;

		CGenericIoControl gnt(pParams->hFile, pPropertyGood , cbGoodSize, cbGoodSize,
		                     sizeof(KSDRMAUDIOSTREAM_CONTENTID), sizeof(KSDRMAUDIOSTREAM_CONTENTID));

		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.CopyProtect = FALSE;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.Reserved  	= 0;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.DigitalOutputDisable = FALSE;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->ContentId  = 0x80000001;

		//
		// The driver should return status INVALID_DEVICE_REQUEST, since the request was initiated
		// from user mode
		//
		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete[] pPropertyGood->Context;
		    delete[] pPropertyGood;
		    return FNS_FAIL;
	  	}
	}

    delete[] pPropertyGood->Context;
    delete[] pPropertyGood;

    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DRMAUDIOSTREAM_CONTENTID_3(PGAUDITPARAMS pParams)
// Out buffer NULL
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

   	ULONG cbGoodSize = sizeof(KSP_DRMAUDIOSTREAM_CONTENTID);
	PKSP_DRMAUDIOSTREAM_CONTENTID  pPropertyGood = (PKSP_DRMAUDIOSTREAM_CONTENTID)new BYTE[cbGoodSize];

	{
		memcpy(&(pPropertyGood->Property) ,&pParams->Prop, pParams->cbSize);
		pPropertyGood->Context 							= new BYTE[sizeof(DRMFORWARD)];
		pPropertyGood->DrmAddContentHandlers	 		= NULL;
		pPropertyGood->DrmCreateContentMixed			= NULL;
		pPropertyGood->DrmDestroyContent				= NULL;
		pPropertyGood->DrmForwardContentToDeviceObject	= NULL;
		pPropertyGood->DrmForwardContentToFileObject	= NULL;
		pPropertyGood->DrmForwardContentToInterface		= NULL;
		pPropertyGood->DrmGetContentRights				= NULL;

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize, 0, 0);

		//
		// The driver should return status INVALID_DEVICE_REQUEST, since the request was initiated
		// from user mode
		//
		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
		  delete[] pPropertyGood->Context;
		  delete[] pPropertyGood;
		  return FNS_FAIL;
	  	}
	}

	delete[] pPropertyGood->Context;
	delete[] pPropertyGood;

    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DRMAUDIOSTREAM_CONTENTID_4(PGAUDITPARAMS pParams)
// Out buffer Small
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

   	ULONG cbGoodSize = sizeof(KSP_DRMAUDIOSTREAM_CONTENTID);
	PKSP_DRMAUDIOSTREAM_CONTENTID  pPropertyGood = (PKSP_DRMAUDIOSTREAM_CONTENTID)new BYTE[cbGoodSize];

	{
		memcpy(&(pPropertyGood->Property) ,&pParams->Prop, pParams->cbSize);
		pPropertyGood->Context 							= new BYTE[sizeof(DRMFORWARD)];
		pPropertyGood->DrmAddContentHandlers	 		= NULL;
		pPropertyGood->DrmCreateContentMixed			= NULL;
		pPropertyGood->DrmDestroyContent				= NULL;
		pPropertyGood->DrmForwardContentToDeviceObject	= NULL;
		pPropertyGood->DrmForwardContentToFileObject	= NULL;
		pPropertyGood->DrmForwardContentToInterface		= NULL;
		pPropertyGood->DrmGetContentRights				= NULL;

		CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
		                    sizeof(KSDRMAUDIOSTREAM_CONTENTID)-10, sizeof(KSDRMAUDIOSTREAM_CONTENTID)-10);

		//
		// The driver should return status INVALID_DEVICE_REQUEST, since the request was initiated
		// from user mode
		//
		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
		  delete[] pPropertyGood->Context;
		  delete[] pPropertyGood;
		  return FNS_FAIL;
	  	}
	}

	delete[] pPropertyGood->Context;
	delete[] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DRMAUDIOSTREAM_CONTENTID_5(PGAUDITPARAMS pParams)
// Out buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

   	ULONG cbGoodSize = sizeof(KSP_DRMAUDIOSTREAM_CONTENTID);
	PKSP_DRMAUDIOSTREAM_CONTENTID  pPropertyGood = (PKSP_DRMAUDIOSTREAM_CONTENTID)new BYTE[cbGoodSize];

	{
		memcpy(&(pPropertyGood->Property) ,&pParams->Prop, pParams->cbSize);
		pPropertyGood->Context 							= new BYTE[sizeof(DRMFORWARD)];
		pPropertyGood->DrmAddContentHandlers	 		= NULL;
		pPropertyGood->DrmCreateContentMixed			= NULL;
		pPropertyGood->DrmDestroyContent				= NULL;
		pPropertyGood->DrmForwardContentToDeviceObject	= NULL;
		pPropertyGood->DrmForwardContentToFileObject	= NULL;
		pPropertyGood->DrmForwardContentToInterface		= NULL;
		pPropertyGood->DrmGetContentRights				= NULL;

		CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
		                    sizeof(KSDRMAUDIOSTREAM_CONTENTID)+1, sizeof(KSDRMAUDIOSTREAM_CONTENTID)+1);

		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.CopyProtect = FALSE;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.Reserved  	= 0;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.DigitalOutputDisable = FALSE;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->ContentId  = 1;

		//
		// The driver should return status INVALID_DEVICE_REQUEST, since the request was initiated
		// from user mode
		//
		gnt.Test(TRUE, 0);

		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
		  delete[] pPropertyGood->Context;
		  delete[] pPropertyGood;
		  return FNS_FAIL;
	  	}
	}
	delete[] pPropertyGood->Context;
	delete[] pPropertyGood;

    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DRMAUDIOSTREAM_CONTENTID_6(PGAUDITPARAMS pParams)
// Context buffer NULL
{

   	ULONG cbGoodSize = sizeof(KSP_DRMAUDIOSTREAM_CONTENTID);
	PKSP_DRMAUDIOSTREAM_CONTENTID  pPropertyGood = (PKSP_DRMAUDIOSTREAM_CONTENTID)new BYTE[cbGoodSize];

	{
		memcpy(&(pPropertyGood->Property) ,&pParams->Prop, pParams->cbSize);
		//
		//context buffer NULL
		//
		pPropertyGood->Context 							= NULL;

		pPropertyGood->DrmAddContentHandlers	 		= NULL;
		pPropertyGood->DrmCreateContentMixed			= NULL;
		pPropertyGood->DrmDestroyContent				= NULL;
		pPropertyGood->DrmForwardContentToDeviceObject	= NULL;
		pPropertyGood->DrmForwardContentToFileObject	= NULL;
		pPropertyGood->DrmForwardContentToInterface		= NULL;
		pPropertyGood->DrmGetContentRights				= NULL;

		CGenericIoControl gnt(pParams->hFile, pPropertyGood , cbGoodSize, cbGoodSize,
		                     sizeof(KSDRMAUDIOSTREAM_CONTENTID), sizeof(KSDRMAUDIOSTREAM_CONTENTID));

		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.CopyProtect = FALSE;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.Reserved  	= 0;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.DigitalOutputDisable = FALSE;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->ContentId  = 1;

		//
		// The driver should return status INVALID_DEVICE_REQUEST, since the request was initiated
		// from user mode
		//
		gnt.Test(TRUE, 0);

		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
		  delete[] pPropertyGood->Context;
		  delete[] pPropertyGood;
		  return FNS_FAIL;
	  	}
	}

   	delete[] pPropertyGood->Context;
    delete[] pPropertyGood;

    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_DRMAUDIOSTREAM_CONTENTID_7(PGAUDITPARAMS pParams)
// Context buffer Small - One Byte
{

   	
   	ULONG cbGoodSize = sizeof(KSP_DRMAUDIOSTREAM_CONTENTID);
	PKSP_DRMAUDIOSTREAM_CONTENTID  pPropertyGood = (PKSP_DRMAUDIOSTREAM_CONTENTID)new BYTE[cbGoodSize];

	{
		memcpy(&(pPropertyGood->Property) ,&pParams->Prop, pParams->cbSize);
		//
		// Context buffer 1 byte
		//
		pPropertyGood->Context 							= new BYTE[1];
		pPropertyGood->DrmAddContentHandlers	 		= NULL;
		pPropertyGood->DrmCreateContentMixed			= NULL;
		pPropertyGood->DrmDestroyContent				= NULL;
		pPropertyGood->DrmForwardContentToDeviceObject	= NULL;
		pPropertyGood->DrmForwardContentToFileObject	= NULL;
		pPropertyGood->DrmForwardContentToInterface		= NULL;
		pPropertyGood->DrmGetContentRights				= NULL;

		CGenericIoControl gnt(pParams->hFile, pPropertyGood , cbGoodSize, cbGoodSize,
		                     sizeof(KSDRMAUDIOSTREAM_CONTENTID), sizeof(KSDRMAUDIOSTREAM_CONTENTID));

		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.CopyProtect = FALSE;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.Reserved  	= 0;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->DrmRights.DigitalOutputDisable = FALSE;
		((PKSDRMAUDIOSTREAM_CONTENTID)gnt.GetData())->ContentId  = 1;

		//
		// The driver should return status INVALID_DEVICE_REQUEST, since the request was initiated
		// from user mode
		//
		gnt.Test(TRUE, 0);

		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
		  delete[] pPropertyGood->Context;
		  delete[] pPropertyGood;
		  return FNS_FAIL;
	  	}
	}

	delete[] pPropertyGood->Context;
    delete[] pPropertyGood;

    return FNS_PASS;
}