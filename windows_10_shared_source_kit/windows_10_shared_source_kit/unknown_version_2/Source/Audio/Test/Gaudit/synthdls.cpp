// Copyright(C) Microsoft.All rights reserved.

#include "common.h"


/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_DLS_APPEND
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_APPEND_1(PGAUDITPARAMS pParams)
// Invalid NodeID - ULONGMAX
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_APPEND_2(PGAUDITPARAMS pParams)
// Invalid NodeID - 0x80000001
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_APPEND_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_APPEND_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_APPEND_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, Large_Buffer);
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_DLS_COMPACT
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_COMPACT_1(PGAUDITPARAMS pParams)
// Invalid NodeID - ULONGMAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
		                                  pParams->cbSize, 0, 0);

		pParams->NodeProp.NodeId = ULONG_MAX;

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_COMPACT_2(PGAUDITPARAMS pParams)
// Invalid NodeID - 0x80000001
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
		                                  pParams->cbSize, 0, 0);

		pParams->NodeProp.NodeId = 0x80000001;

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
    return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_DLS_DOWNLOAD
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_DOWNLOAD_1(PGAUDITPARAMS pParams)
// Invalid NodeID - ULONGMAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_BUFFER);

	PKSNODEPROPERTY  pPropertyGood = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTH_BUFFER pSynthBuffer = (PSYNTH_BUFFER)(pPropertyGood + 1);

	// Invalid Node id
	pParams->NodeProp.NodeId = ULONG_MAX;

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);

	// uninitialized temp buffer
	BYTE buffer[50];
    pSynthBuffer->BufferSize = 50;
    pSynthBuffer->BufferAddress = buffer;

	{
		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize,
		                                  cbGoodSize, sizeof(SYNTHDOWNLOAD), sizeof(SYNTHDOWNLOAD));

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;

}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_DOWNLOAD_2(PGAUDITPARAMS pParams)
// Invalid NodeID - 0x80000001
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_BUFFER);
	PKSNODEPROPERTY  pPropertyGood = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTH_BUFFER pSynthBuffer = (PSYNTH_BUFFER)(pPropertyGood + 1);

	// Invalid Node id
	pParams->NodeProp.NodeId = 0x80000001;

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);

	// uninitialized temp buffer
	BYTE buffer[50];
    pSynthBuffer->BufferSize = 50;
    pSynthBuffer->BufferAddress = buffer;

	{
		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize,
		                                  cbGoodSize, sizeof(SYNTHDOWNLOAD), sizeof(SYNTHDOWNLOAD));

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_DOWNLOAD_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_BUFFER);
	PKSNODEPROPERTY  pPropertyGood = (PKSNODEPROPERTY) new BYTE[cbGoodSize ];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTH_BUFFER pSynthBuffer = (PSYNTH_BUFFER)(pPropertyGood + 1);

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	// uninitialized temp buffer
	BYTE buffer[50];
    pSynthBuffer->BufferSize = 50;
    pSynthBuffer->BufferAddress = buffer;

	{
		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize,
		                                  cbGoodSize, 0, 0);

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_DOWNLOAD_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_BUFFER);
	PKSNODEPROPERTY  pPropertyGood = (PKSNODEPROPERTY) new BYTE[cbGoodSize ];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTH_BUFFER pSynthBuffer = (PSYNTH_BUFFER)(pPropertyGood + 1);

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);

	// uninitialized temp buffer
	BYTE buffer[50];
    pSynthBuffer->BufferSize = 50;
    pSynthBuffer->BufferAddress = buffer;

	{
		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize,
		                                  cbGoodSize, sizeof(SYNTHDOWNLOAD) -1 , sizeof(SYNTHDOWNLOAD) -1);

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_DOWNLOAD_5(PGAUDITPARAMS pParams)
// Out Buffer Large 
// TODO: needs to findout why are we expecting failures in this test case  - passing large buffer normally expects success.
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_BUFFER);
	PKSNODEPROPERTY  pPropertyGood = (PKSNODEPROPERTY) new BYTE[cbGoodSize ];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTH_BUFFER pSynthBuffer = (PSYNTH_BUFFER)(pPropertyGood + 1);

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	// uninitialized temp buffer
	BYTE buffer[50];
    pSynthBuffer->BufferSize = 50;
    pSynthBuffer->BufferAddress = buffer;

	{
		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize,
		                                  cbGoodSize, sizeof(SYNTHDOWNLOAD) +1 , sizeof(SYNTHDOWNLOAD) +1);

// TODO: needs to find out what will be the valid DLS data. Why are we expecting failure in this test case
		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_DOWNLOAD_6(PGAUDITPARAMS pParams)
// Synth Buffer NULL
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

   	ULONG cbGoodSize = pParams->cbSize + sizeof(PSYNTH_BUFFER); // Size of the pointer
	PKSNODEPROPERTY  pPropertyGood = (PKSNODEPROPERTY) new BYTE[cbGoodSize ];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTH_BUFFER pSynthBuffer = (PSYNTH_BUFFER)(pPropertyGood + 1);
	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);

    // synth Buffer Pointer NULL
    pSynthBuffer = NULL;

	{
		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize,
		                                  cbGoodSize, sizeof(SYNTHDOWNLOAD), sizeof(SYNTHDOWNLOAD));

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_DOWNLOAD_7(PGAUDITPARAMS pParams)
// Synth Buffer Small - One byte
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

   	ULONG cbGoodSize = pParams->cbSize + sizeof(char); // Synth buffer One byte
	PKSNODEPROPERTY  pPropertyGood = (PKSNODEPROPERTY) new BYTE[cbGoodSize ];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	{
		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize,
		                                  cbGoodSize, sizeof(SYNTHDOWNLOAD), sizeof(SYNTHDOWNLOAD));

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_DLS_UNLOAD
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_UNLOAD_1(PGAUDITPARAMS pParams)
// Invalid NodeID - ULONGMAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
		                                  pParams->cbSize, sizeof(HANDLE), sizeof(HANDLE));

		pParams->NodeProp.NodeId = ULONG_MAX;

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_UNLOAD_2(PGAUDITPARAMS pParams)
// Invalid NodeID - 0x80000001
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
		                                  pParams->cbSize, sizeof(HANDLE), sizeof(HANDLE));

		pParams->NodeProp.NodeId = 0x80000001;

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_UNLOAD_3(PGAUDITPARAMS pParams)
// Invalid Handle - NULL
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
		                                  pParams->cbSize, sizeof(HANDLE), sizeof(HANDLE));

		*(PHANDLE) gnt.GetData() = NULL;

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
    return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_DLS_WAVEFORMAT
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_WAVEFORMAT_1(PGAUDITPARAMS pParams)
// Invalid NodeID - ULONGMAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
		                                  pParams->cbSize, sizeof(WAVEFORMATEX), sizeof(WAVEFORMATEX));

		pParams->NodeProp.NodeId = ULONG_MAX;

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_WAVEFORMAT_2(PGAUDITPARAMS pParams)
// Invalid NodeID - 0x80000001
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
		                                  pParams->cbSize, sizeof(WAVEFORMATEX), sizeof(WAVEFORMATEX));

		pParams->NodeProp.NodeId = 0x80000001;

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_WAVEFORMAT_3(PGAUDITPARAMS pParams)
// Out buffer NULL
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
		                                  pParams->cbSize, 0, 0);

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_WAVEFORMAT_4(PGAUDITPARAMS pParams)
// Out buffer Small
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
		                                  pParams->cbSize, sizeof(WAVEFORMATEX)-1, sizeof(WAVEFORMATEX)-1);

		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_DLS_WAVEFORMAT_5(PGAUDITPARAMS pParams)
// Out buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
		                                  pParams->cbSize, sizeof(WAVEFORMATEX)+1, sizeof(WAVEFORMATEX)+1);

		gnt.Test(FALSE, sizeof(WAVEFORMATEX));
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
    return FNS_PASS;
}