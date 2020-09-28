// Copyright(C) Microsoft.All rights reserved.

#include "common.h"


/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_CAPS
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CAPS_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CAPS_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CAPS_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize, pParams->cbSize, 0, 0);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CAPS_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, Small_Buffer);

}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CAPS_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, Large_Buffer);
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_CHANNELGROUPS
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CHANNELGROUPS_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CHANNELGROUPS_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CHANNELGROUPS_3(PGAUDITPARAMS pParams)
// Invalid Channel Group = 0xFFFFFFFE
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;
	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
										  pParams->cbSize, sizeof(ULONG), sizeof(ULONG));
		*(PULONG)gnt.GetData() = (ULONG)0xFFFFFFFE;
		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CHANNELGROUPS_4(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CHANNELGROUPS_5(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CHANNELGROUPS_6(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, Large_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_CHANNELGROUPS_7(PGAUDITPARAMS pParams)
// Invalid Channel Group = 0x80000001
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;
	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, pParams->cbSize,
		                                  pParams->cbSize, sizeof(ULONG), sizeof(ULONG));
		*(PULONG)gnt.GetData() = (ULONG)0x80000001;
		gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}
	return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_PORTPARAMETERS
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
 	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);

	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);


	// Invalid node id ULONG_MAX
	pParams->NodeProp.NodeId = ULONG_MAX;

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);

	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_AUDIOCHANNELS | SYNTH_PORTPARAMS_CHANNELGROUPS;
	pSynthPortParams->ChannelGroups = 0;
	pSynthPortParams->AudioChannels = 2;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                    sizeof(SYNTH_PORTPARAMS), sizeof(SYNTH_PORTPARAMS));

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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);

	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);


	// Invalid node id 0x80000001
	pParams->NodeProp.NodeId = 0x80000001;
	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);

	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_AUDIOCHANNELS | SYNTH_PORTPARAMS_CHANNELGROUPS;
	pSynthPortParams->ChannelGroups = 0;
	pSynthPortParams->AudioChannels = 2;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                    sizeof(SYNTH_PORTPARAMS), sizeof(SYNTH_PORTPARAMS));

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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);


	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_AUDIOCHANNELS | SYNTH_PORTPARAMS_CHANNELGROUPS;
	pSynthPortParams->ChannelGroups = 0;
	pSynthPortParams->AudioChannels = 2;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize, 0, 0);

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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);

	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}
	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);


	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_AUDIOCHANNELS | SYNTH_PORTPARAMS_CHANNELGROUPS;
	pSynthPortParams->ChannelGroups = 0;
	pSynthPortParams->AudioChannels = 2;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                        sizeof(SYNTH_PORTPARAMS)-1, sizeof(SYNTH_PORTPARAMS)-1);

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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);


	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_AUDIOCHANNELS | SYNTH_PORTPARAMS_CHANNELGROUPS;
	pSynthPortParams->ChannelGroups = 0;
	pSynthPortParams->AudioChannels = 2;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                        sizeof(SYNTH_PORTPARAMS)+1, sizeof(SYNTH_PORTPARAMS)+1);

           ULONG uExpectedBytes = 0;
           if (pPropertyGood->Property.Flags & KSPROPERTY_TYPE_GET) 
                uExpectedBytes = sizeof(SYNTH_PORTPARAMS);      // Since in case of Get request we are expecting bytes to be writtne where as in case of SET request, it should be zero
	    gnt.Test(FALSE, uExpectedBytes);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_6(PGAUDITPARAMS pParams)
// Invalid Channel Groups - ULONG_MAX
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);


	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_CHANNELGROUPS |SYNTH_PORTPARAMS_AUDIOCHANNELS;
	pSynthPortParams->ChannelGroups = ULONG_MAX;
	pSynthPortParams->AudioChannels = 2;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                        sizeof(SYNTH_PORTPARAMS), sizeof(SYNTH_PORTPARAMS));

	    gnt.Test(FALSE, sizeof(SYNTH_PORTPARAMS));  // As per Doc, Property request will always successed eventhogu paramert are incorrect
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
		else
		{
			pSynthPortParams = (PSYNTH_PORTPARAMS)gnt.GetData();
			if(pSynthPortParams->ChannelGroups == ULONG_MAX)
			{
				SLOG(eError, "ERROR: Property accepts an invalid param - ChannelGroups: %x",
								pSynthPortParams->ChannelGroups);
				delete [] pPropertyGood;
				return FNS_FAIL;
			}
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_7(PGAUDITPARAMS pParams)
// Invalid Channel Groups - 0x80000001
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);


	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_CHANNELGROUPS |SYNTH_PORTPARAMS_AUDIOCHANNELS;
	pSynthPortParams->ChannelGroups = 0x80000001;
	pSynthPortParams->AudioChannels = 2;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                        sizeof(SYNTH_PORTPARAMS), sizeof(SYNTH_PORTPARAMS));

	    gnt.Test(FALSE, sizeof(SYNTH_PORTPARAMS));
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
		else
		{
			pSynthPortParams = (PSYNTH_PORTPARAMS)gnt.GetData();
			if(pSynthPortParams->ChannelGroups == 0x80000001)
			{
				SLOG(eError, "ERROR: Property accepts an invalid param - ChannelGroups: %x",
								pSynthPortParams->ChannelGroups);
				delete [] pPropertyGood;
				return FNS_FAIL;
			}
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_8(PGAUDITPARAMS pParams)
// Invalid Channel ID - ULONG_MAX
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_CHANNELGROUPS |SYNTH_PORTPARAMS_AUDIOCHANNELS;
	pSynthPortParams->ChannelGroups = 0;
	pSynthPortParams->AudioChannels = ULONG_MAX;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                        sizeof(SYNTH_PORTPARAMS), sizeof(SYNTH_PORTPARAMS));

	    gnt.Test(FALSE, sizeof(SYNTH_PORTPARAMS));
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
		else
		{
			pSynthPortParams = (PSYNTH_PORTPARAMS)gnt.GetData();
			if(pSynthPortParams->AudioChannels == ULONG_MAX)
			{
				SLOG(eError, "ERROR: Property accepts an invalid param - AudioChannels: %x",
								pSynthPortParams->AudioChannels);
				delete [] pPropertyGood;
				return FNS_FAIL;
			}
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_9(PGAUDITPARAMS pParams)
// Invalid Channel ID - 0x80000001
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_CHANNELGROUPS |SYNTH_PORTPARAMS_AUDIOCHANNELS;
	pSynthPortParams->ChannelGroups = 0;
	pSynthPortParams->AudioChannels = 0x80000001;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                        sizeof(SYNTH_PORTPARAMS), sizeof(SYNTH_PORTPARAMS));

	    gnt.Test(FALSE, sizeof(SYNTH_PORTPARAMS));
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
		else
		{
			pSynthPortParams = (PSYNTH_PORTPARAMS)gnt.GetData();
			if(pSynthPortParams->AudioChannels == 0x80000001)
			{
				SLOG(eError, "ERROR: Property accepts an invalid param - AudioChannels: %x",
								pSynthPortParams->AudioChannels);
				delete [] pPropertyGood;
				return FNS_FAIL;
			}
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_10(PGAUDITPARAMS pParams)
// Invalid Sample Rate - ULONG_MAX
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);


	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_SAMPLERATE;
	pSynthPortParams->SampleRate = ULONG_MAX;

	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                        sizeof(SYNTH_PORTPARAMS), sizeof(SYNTH_PORTPARAMS));

	    gnt.Test(FALSE, sizeof(SYNTH_PORTPARAMS));
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
		else
		{
			pSynthPortParams = (PSYNTH_PORTPARAMS)gnt.GetData();
			if(pSynthPortParams->SampleRate  == ULONG_MAX)
			{
				SLOG(eError, "ERROR: Property accepts an invalid param - SampleRate : %x",
								pSynthPortParams->SampleRate);
				delete [] pPropertyGood;
				return FNS_FAIL;
			}
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_11(PGAUDITPARAMS pParams)
// Invalid Sample Rate - 0x80000001
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_SAMPLERATE;
	pSynthPortParams->SampleRate = 0x80000001;

	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                        sizeof(SYNTH_PORTPARAMS), sizeof(SYNTH_PORTPARAMS));

	    gnt.Test(FALSE, sizeof(SYNTH_PORTPARAMS));
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
		else
		{
			pSynthPortParams = (PSYNTH_PORTPARAMS)gnt.GetData();
			if(pSynthPortParams->SampleRate  == 0x80000001)
			{
				SLOG(eError, "ERROR: Property accepts an invalid param - SampleRate : %x",
								pSynthPortParams->SampleRate);
				delete [] pPropertyGood;
				return FNS_FAIL;
			}
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_12(PGAUDITPARAMS pParams)
// Invalid Effects Flags - 0xFFFFFFFF
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams = SYNTH_PORTPARAMS_EFFECTS;
	pSynthPortParams->EffectsFlags = 0xFFFFFFFF;

	{
		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                        sizeof(SYNTH_PORTPARAMS), sizeof(SYNTH_PORTPARAMS));

	    gnt.Test(FALSE, sizeof(SYNTH_PORTPARAMS));
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
		else
		{
			pSynthPortParams = (PSYNTH_PORTPARAMS)gnt.GetData();
			if(pSynthPortParams->EffectsFlags  == 0xFFFFFFFF)
			{
				SLOG(eError, "ERROR: Property accepts an invalid param - EffectsFlags : %x",
								pSynthPortParams->EffectsFlags );
				delete [] pPropertyGood;
				return FNS_FAIL;
			}
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_13(PGAUDITPARAMS pParams)
// Invalid Voices - ULONG_MAX
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);


	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams 	= SYNTH_PORTPARAMS_VOICES;
	pSynthPortParams->Voices		= ULONG_MAX;

	{
		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                        sizeof(SYNTH_PORTPARAMS), sizeof(SYNTH_PORTPARAMS));

	    gnt.Test(FALSE, sizeof(SYNTH_PORTPARAMS));
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
		else
		{
			pSynthPortParams = (PSYNTH_PORTPARAMS)gnt.GetData();
			if(pSynthPortParams->Voices	== ULONG_MAX)
			{
				SLOG(eError, "ERROR: Property accepts an invalid param - Voices: %x",
								pSynthPortParams->Voices);
				delete [] pPropertyGood;
				return FNS_FAIL;
			}
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_PORTPARAMETERS_14(PGAUDITPARAMS pParams)
// Invalid Voices - 0x80000001
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTH_PORTPARAMS);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY)new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	//
	// Offset the pointer by KSNODEPROPERTY  size to get the SYNTH_PORTPARAMS structure
	//
	PSYNTH_PORTPARAMS pSynthPortParams = (PSYNTH_PORTPARAMS)(pPropertyGood + 1);


	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
	pSynthPortParams->ValidParams 	= SYNTH_PORTPARAMS_VOICES;
	pSynthPortParams->Voices		= 0x80000001;

	{
		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                        sizeof(SYNTH_PORTPARAMS), sizeof(SYNTH_PORTPARAMS));

	    gnt.Test(FALSE, sizeof(SYNTH_PORTPARAMS));
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
		else
		{
			pSynthPortParams = (PSYNTH_PORTPARAMS)gnt.GetData();
			if(pSynthPortParams->Voices	== 0x80000001)
			{
				SLOG(eError, "ERROR: Property accepts an invalid param - Voices: %x",
								pSynthPortParams->Voices);
				delete [] pPropertyGood;
				return FNS_FAIL;
			}
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_LATENCYCLOCK
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_LATENCYCLOCK_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_LATENCYCLOCK_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, NodeID_0x80000001);
}


DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_LATENCYCLOCK_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_LATENCYCLOCK_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_LATENCYCLOCK_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, Large_Buffer);
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_MASTERCLOCK
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_MASTERCLOCK_1(PGAUDITPARAMS pParams)
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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_MASTERCLOCK_2(PGAUDITPARAMS pParams)
// Out Buffer Small
{

	if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		return FNS_PASS;
	{

		CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
		                                                   sizeof(ULONGLONG) - 1, sizeof(ULONGLONG) - 1);
	    gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}

	return FNS_PASS;
}


DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_MASTERCLOCK_3(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		return FNS_PASS;
	{

		CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
		                                                   sizeof(ULONGLONG) + 1, sizeof(ULONGLONG) + 1);
	    gnt.Test(FALSE, sizeof(ULONGLONG));
		if (gnt.GetFault() != FAULT_NO_FAULT)
			return FNS_FAIL;
	}

	return FNS_PASS;
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_RUNNINGSTATS
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_RUNNINGSTATS_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_RUNNINGSTATS_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, NodeID_0x80000001);
}


DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_RUNNINGSTATS_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
		return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_RUNNINGSTATS_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_RUNNINGSTATS_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;

	return Generic_Property_KSSYNTHNODEPROPERTY(pParams, Large_Buffer);
}


/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_VOICEPRIORITY
   ______________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOICEPRIORITY_1(PGAUDITPARAMS pParams)
// Invalid Node ID= ULONG_MAX
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTHVOICEPRIORITY_INSTANCE);


	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTHVOICEPRIORITY_INSTANCE pVoicePriority = (PSYNTHVOICEPRIORITY_INSTANCE)(pPropertyGood+ 1);

	// set the Invalid node ID = ULONG_MAX
	pParams->NodeProp.NodeId = ULONG_MAX;

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
    pVoicePriority->ChannelGroup = 0;
    pVoicePriority->Channel = 0;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                                           sizeof(DWORD), sizeof(DWORD));
	     *(LPDWORD)gnt.GetData() = DAUD_STANDARD_VOICE_PRIORITY;

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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOICEPRIORITY_2(PGAUDITPARAMS pParams)
// Invalid Node ID= 0x80000001
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTHVOICEPRIORITY_INSTANCE);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTHVOICEPRIORITY_INSTANCE pVoicePriority = (PSYNTHVOICEPRIORITY_INSTANCE)(pPropertyGood+ 1);
	// set the Invalid node ID = 0x80000001
	pParams->NodeProp.NodeId = 0x80000001;

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
    pVoicePriority->ChannelGroup = 0;
    pVoicePriority->Channel = 0;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                                           sizeof(DWORD), sizeof(DWORD));
	     *(LPDWORD)gnt.GetData() = DAUD_STANDARD_VOICE_PRIORITY;

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


DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOICEPRIORITY_3(PGAUDITPARAMS pParams)
//Invalid Channel ID - 0xFFFFFFFE
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTHVOICEPRIORITY_INSTANCE);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTHVOICEPRIORITY_INSTANCE pVoicePriority = (PSYNTHVOICEPRIORITY_INSTANCE)(pPropertyGood+ 1);

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
    pVoicePriority->ChannelGroup = 0;
    pVoicePriority->Channel = 0xFFFFFFFE;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                                           sizeof(DWORD), sizeof(DWORD));
	     *(LPDWORD)gnt.GetData() = DAUD_STANDARD_VOICE_PRIORITY;

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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOICEPRIORITY_4(PGAUDITPARAMS pParams)
// Invalid Channel ID - 0x80000001
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTHVOICEPRIORITY_INSTANCE);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTHVOICEPRIORITY_INSTANCE pVoicePriority = (PSYNTHVOICEPRIORITY_INSTANCE)(pPropertyGood+ 1);
	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
    pVoicePriority->ChannelGroup = 0;
    pVoicePriority->Channel = 0x80000001;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                                           sizeof(DWORD), sizeof(DWORD));
	     *(LPDWORD)gnt.GetData() = DAUD_STANDARD_VOICE_PRIORITY;

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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOICEPRIORITY_5(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTHVOICEPRIORITY_INSTANCE);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTHVOICEPRIORITY_INSTANCE pVoicePriority = (PSYNTHVOICEPRIORITY_INSTANCE)(pPropertyGood+ 1);

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
    pVoicePriority->ChannelGroup = 0;
    pVoicePriority->Channel = 0;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                                           0, 0);
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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOICEPRIORITY_6(PGAUDITPARAMS pParams)
// Out Buffer Small
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTHVOICEPRIORITY_INSTANCE);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTHVOICEPRIORITY_INSTANCE pVoicePriority = (PSYNTHVOICEPRIORITY_INSTANCE)(pPropertyGood+ 1);

	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
    pVoicePriority->ChannelGroup = 0;
    pVoicePriority->Channel = 0;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                                           sizeof(DWORD)-1, sizeof(DWORD)-1);
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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOICEPRIORITY_7(PGAUDITPARAMS pParams)
// Out Buffer Large
{

   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTHVOICEPRIORITY_INSTANCE);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTHVOICEPRIORITY_INSTANCE pVoicePriority = (PSYNTHVOICEPRIORITY_INSTANCE)(pPropertyGood+ 1);
	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
    pVoicePriority->ChannelGroup = 0;
    pVoicePriority->Channel = 0;
	{
		// We should see if the buffer overflow occurs while SET operation
		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                                           sizeof(DWORD)+1, sizeof(DWORD)+1);

           ULONG uExpectedBytes = 0;
           if (pPropertyGood->Property.Flags & KSPROPERTY_TYPE_GET) 
                uExpectedBytes = sizeof(DWORD);      // Since in case of Get request we are expecting bytes to be writtne where as in case of SET request, it should be zero
	    gnt.Test(FALSE, uExpectedBytes);
		if (gnt.GetFault() != FAULT_NO_FAULT)
		{
			delete [] pPropertyGood;
			return FNS_FAIL;
		}
	}
	delete [] pPropertyGood;
    return FNS_PASS;
}



DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOICEPRIORITY_8(PGAUDITPARAMS pParams)
//Invalid Voice Priority - 0xFFFFFFFF
{

	if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
		return FNS_PASS;

   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTHVOICEPRIORITY_INSTANCE);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTHVOICEPRIORITY_INSTANCE pVoicePriority = (PSYNTHVOICEPRIORITY_INSTANCE)(pPropertyGood+ 1);
	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
    pVoicePriority->ChannelGroup = 0;
    pVoicePriority->Channel = 0;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                                           sizeof(DWORD), sizeof(DWORD));
	     *(LPDWORD)gnt.GetData() = 0xFFFFFFFF;

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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOICEPRIORITY_9(PGAUDITPARAMS pParams)
// Invalid Channel Group - 0xFFFFFFFE
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTHVOICEPRIORITY_INSTANCE);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTHVOICEPRIORITY_INSTANCE pVoicePriority = (PSYNTHVOICEPRIORITY_INSTANCE)(pPropertyGood+ 1);
	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
    pVoicePriority->ChannelGroup = 0xFFFFFFFE;
    pVoicePriority->Channel = 0;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                                           sizeof(DWORD), sizeof(DWORD));
	     *(LPDWORD)gnt.GetData() = DAUD_STANDARD_VOICE_PRIORITY;

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

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOICEPRIORITY_10(PGAUDITPARAMS pParams)
// Invalid Channel Group - 0x80000001
{
   	ULONG cbGoodSize = pParams->cbSize + sizeof(SYNTHVOICEPRIORITY_INSTANCE);
	PKSNODEPROPERTY  pPropertyGood  = (PKSNODEPROPERTY) new BYTE[cbGoodSize];
	if (!pPropertyGood)
	{
		SLOG(eError, "ERROR: Internal memory allocation failure");
		return FNS_FAIL;
	}

	PSYNTHVOICEPRIORITY_INSTANCE pVoicePriority = (PSYNTHVOICEPRIORITY_INSTANCE)(pPropertyGood+ 1);
	memcpy(pPropertyGood, &pParams->NodeProp, pParams->cbSize);
    pVoicePriority->ChannelGroup = 0x80000001;
    pVoicePriority->Channel = 0;
	{

		CGenericIoControl gnt(pParams->hFile, pPropertyGood, cbGoodSize, cbGoodSize,
		                                                           sizeof(DWORD), sizeof(DWORD));
	     *(LPDWORD)gnt.GetData() = DAUD_STANDARD_VOICE_PRIORITY;

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

    @TEST KSPROPERTY_SYNTH_VOLUME
   ______________________________________________________________________ */


DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOLUME_1(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOLUME_2(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOLUME_3(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, Large_Buffer);
}

/* _____________________________________________________________________

    @TEST KSPROPERTY_SYNTH_VOLUMEBOOST
   ______________________________________________________________________ */


DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOLUMEBOOST_1(PGAUDITPARAMS pParams)
// Invalid NodeID - ULONGMAX
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOLUMEBOOST_2(PGAUDITPARAMS pParams)
// Invalid NodeID - 0x80000001
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOLUMEBOOST_3(PGAUDITPARAMS pParams)
// Out Buffer NULL
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, NULL_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOLUMEBOOST_4(PGAUDITPARAMS pParams)
// Out Buffer Small
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, Small_Buffer);
}

DWORD WINAPI Generic_Property_KSPROPERTY_SYNTH_VOLUMEBOOST_5(PGAUDITPARAMS pParams)
// Out Buffer Large
{
	return Generic_Property_KSNODEPROPERTY_ULONG(pParams, Large_Buffer);
}
