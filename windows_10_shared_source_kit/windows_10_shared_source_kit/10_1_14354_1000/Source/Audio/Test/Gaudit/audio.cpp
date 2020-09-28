//+-------------------------------------------------------------------------
//
//  Microsoft Windows
//
// Copyright(C) Microsoft.All rights reserved.
//
//  File:       audio.cpp
//
//
//--------------------------------------------------------------------------

#include "common.h"


/* ___________________________________________________________________________

    @TEST KSPROPERTY_AUDIO_MIX_LEVEL_CAPS
   ___________________________________________________________________________ */


DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_CAPS_1(PGAUDITPARAMS pParams)
// setting zero InputChannels and zero OutputChannels - it must fail
{
    PKSAUDIO_MIXCAP_TABLE pmxtbl = NULL;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)	{   
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp,       // as we are sending Node Property
                              pParams->cbSize, pParams->cbSize, 
                              sizeof(KSAUDIO_MIXCAP_TABLE), sizeof(KSAUDIO_MIXCAP_TABLE));

        pmxtbl = (PKSAUDIO_MIXCAP_TABLE)gnt.GetData();
        pmxtbl->InputChannels = pmxtbl->OutputChannels = 0;
        DWORD dwResult = gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_CAPS_2(PGAUDITPARAMS pParams)
//Testing with NODEID equal to ULONG_MAX for get request- it must fail    
// for set request setting input and putput channel to ULONG_MAX
{
    PKSAUDIO_MIXCAP_TABLE pmxtbl = NULL;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)      
        pParams->NodeProp.NodeId = ULONG_MAX;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, 
                              pParams->cbSize, pParams->cbSize, 
                              sizeof(KSAUDIO_MIXCAP_TABLE), sizeof(KSAUDIO_MIXCAP_TABLE));

        pmxtbl = (PKSAUDIO_MIXCAP_TABLE)gnt.GetData();
        pmxtbl->InputChannels = pmxtbl->OutputChannels = ULONG_MAX;
        DWORD dwResult = gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_CAPS_3(PGAUDITPARAMS pParams)
//Testing with NODEID equal to 0x80000001 duing get request and for set request, setting input channel to 0x80000001- it must fail    
{
    PKSAUDIO_MIXCAP_TABLE pmxtbl = NULL;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)      
        pParams->NodeProp.NodeId = 0x80000001;
    {
        
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, 
                              pParams->cbSize, pParams->cbSize, 
                              sizeof(KSAUDIO_MIXCAP_TABLE) + sizeof(KSAUDIO_MIX_CAPS), 
                              sizeof(KSAUDIO_MIXCAP_TABLE) + sizeof(KSAUDIO_MIX_CAPS));

		pmxtbl = (PKSAUDIO_MIXCAP_TABLE)gnt.GetData();
        pmxtbl->InputChannels = 0x80000001;
		pmxtbl->OutputChannels = 2;
        DWORD dwResult = gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_CAPS_4(PGAUDITPARAMS pParams)
//  test with Small input buffer - it must fail   
{
    PKSAUDIO_MIXCAP_TABLE pmxtbl = NULL;

    {
        ULONG OutputBufferSize = sizeof(KSAUDIO_MIXCAP_TABLE) -1;
        if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)              
            OutputBufferSize = 7; // Since for get request 8 byte long buffer will be successed. 
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, 
                              pParams->cbSize, pParams->cbSize, 
                              OutputBufferSize, OutputBufferSize);
        if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET){              
            pmxtbl = (PKSAUDIO_MIXCAP_TABLE)gnt.GetData();
            pmxtbl->InputChannels = pmxtbl->OutputChannels = 1;
        }
        DWORD dwResult = gnt.Test(TRUE, 0);
	 if (gnt.GetFault() != FAULT_NO_FAULT) 
		return FNS_FAIL;
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_CAPS_5(PGAUDITPARAMS pParams)
//  test with invalid Node Id  -0x10000000
{
    PKSAUDIO_MIXCAP_TABLE pmxtbl = NULL;

    {
        pParams->NodeProp.NodeId = 0x10000000;
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp, 
                              pParams->cbSize, pParams->cbSize, 
                              (2*sizeof(ULONG)),(2*sizeof(ULONG)));     //in case of valid node it this will return input and output channel value. 

        pmxtbl = (PKSAUDIO_MIXCAP_TABLE)gnt.GetData();
        DWORD dwResult = gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_CAPS(PGAUDITPARAMS pParams)
//  test with valid test data.
{
    // since we are testing with valid test data, we will run this test case for KSPROPERTY_TYPE_GET only and not for set.
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
    PKSAUDIO_MIXCAP_TABLE pmxtbl = NULL;

    CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp,
                                        pParams->cbSize, pParams->cbSize, 
                                        (2*sizeof(ULONG)),(2*sizeof(ULONG)));     //sending output buffer enough to have information about input and output channels. 

    pmxtbl = (PKSAUDIO_MIXCAP_TABLE)gnt.GetData();
    
    DWORD dwResult = gnt.Test(FALSE, (2*sizeof(ULONG)));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    if (!((dwResult ==  0xEA) || (dwResult == 0x7A )))  // as in this case we will not get good result form the node /driver
        return FNS_PASS;
    ULONG inputChannels = pmxtbl->InputChannels;
    ULONG outputChannels = pmxtbl->OutputChannels;
    ULONG  sizeofMixcap;
    sizeofMixcap = (2 * sizeof (ULONG)) + ((inputChannels*outputChannels) * sizeof(KSAUDIO_MIX_CAPS));
    SLOG(eInfo1, "we are now expecting %d no of bytes", sizeofMixcap);    
    CGenericIoControl gnt1(pParams->hFile, &pParams->NodeProp,
                                            pParams->cbSize, pParams->cbSize, 
                                            sizeofMixcap,sizeofMixcap);     //sending output buffer enough to have information about input and output channels. 

    pmxtbl = (PKSAUDIO_MIXCAP_TABLE)gnt1.GetData();
    dwResult = gnt1.Test(FALSE, sizeofMixcap);                                        

    if (gnt1.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    else
	return FNS_PASS;
}

//=========================
// KSNODEPROPERTY and GUID
DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_3D_INTERFACE_1(PGAUDITPARAMS pParams)
//sending invalid node id = 0xFFFFFFFF in get as well as set request
{
       GUID tempData = DS3DALG_DEFAULT;
	return Generic_Property_KSNODEPROPERTY(pParams, NodeID_ULONGMAX, (LPVOID)&tempData, sizeof (GUID));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_3D_INTERFACE_2(PGAUDITPARAMS pParams)
//sending invalid node id = 0x80000001 in get as well as set request
{
    GUID tempData = DS3DALG_DEFAULT;
    return Generic_Property_KSNODEPROPERTY(pParams, NodeID_0x80000001, (LPVOID)&tempData, sizeof (GUID));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_3D_INTERFACE_3(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
    GUID tempData = DS3DALG_DEFAULT;
    return Generic_Property_KSNODEPROPERTY(pParams, NULL_Buffer, (LPVOID)&tempData, sizeof (GUID));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_3D_INTERFACE_4(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
    GUID tempData = DS3DALG_DEFAULT;
    return Generic_Property_KSNODEPROPERTY(pParams, Small_Buffer, (LPVOID)&tempData, sizeof (GUID));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_3D_INTERFACE_5(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but Large buffer in get as well as set request
{
    GUID tempData = DS3DALG_DEFAULT;
    return Generic_Property_KSNODEPROPERTY(pParams, Large_Buffer, (LPVOID)&tempData, sizeof (GUID));
}
    
DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_3D_INTERFACE(PGAUDITPARAMS pParams)
//sending valid Test Data
{
    GUID tempData = DS3DALG_DEFAULT;
    return Generic_Property_KSNODEPROPERTY(pParams, Valid_Test, (LPVOID)&tempData, sizeof (GUID));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_ALGORITHM_INSTANCE_1(PGAUDITPARAMS pParams)
//sending invalid node id = 0xFFFFFFFF in get as well as set request
{
    GUID tempData;
    CKsNode* pCurrentNode =  NODE();
    GUID NodeGuid = pCurrentNode->m_guidType;
    if (IsEqualGUID(NodeGuid,KSNODETYPE_ACOUSTIC_ECHO_CANCEL))
        tempData = KSALGORITHMINSTANCE_SYSTEM_ACOUSTIC_ECHO_CANCEL;
    else if (IsEqualGUID(NodeGuid,KSNODETYPE_NOISE_SUPPRESS))
        tempData = KSALGORITHMINSTANCE_SYSTEM_NOISE_SUPPRESS;
    else if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;    // as we are not sure waht will be the valid value for the node 
    return Generic_Property_KSNODEPROPERTY(pParams, NodeID_ULONGMAX, (LPVOID)&tempData, sizeof (GUID));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_ALGORITHM_INSTANCE_2(PGAUDITPARAMS pParams)
//sending invalid node id = 0x80000001 in get as well as set request
{
    GUID tempData;
    CKsNode* pCurrentNode =  NODE();
    GUID NodeGuid = pCurrentNode->m_guidType;
    if (IsEqualGUID(NodeGuid,KSNODETYPE_ACOUSTIC_ECHO_CANCEL))
        tempData = KSALGORITHMINSTANCE_SYSTEM_ACOUSTIC_ECHO_CANCEL;
    else if (IsEqualGUID(NodeGuid,KSNODETYPE_NOISE_SUPPRESS))
        tempData = KSALGORITHMINSTANCE_SYSTEM_NOISE_SUPPRESS;
    else if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;    // as we are not sure waht will be the valid value for the node 
    return Generic_Property_KSNODEPROPERTY(pParams, NodeID_0x80000001,  (LPVOID)&tempData, sizeof (GUID));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_ALGORITHM_INSTANCE_3(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
    GUID tempData;
    CKsNode* pCurrentNode =  NODE();
    GUID NodeGuid = pCurrentNode->m_guidType;
    if (IsEqualGUID(NodeGuid,KSNODETYPE_ACOUSTIC_ECHO_CANCEL))
        tempData = KSALGORITHMINSTANCE_SYSTEM_ACOUSTIC_ECHO_CANCEL;
    else if (IsEqualGUID(NodeGuid,KSNODETYPE_NOISE_SUPPRESS))
        tempData = KSALGORITHMINSTANCE_SYSTEM_NOISE_SUPPRESS;
    else if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;    // as we are not sure waht will be the valid value for the node 
    return Generic_Property_KSNODEPROPERTY(pParams, NULL_Buffer,  (LPVOID)&tempData, sizeof (GUID));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_ALGORITHM_INSTANCE_4(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
    GUID tempData;
    CKsNode* pCurrentNode =  NODE();
    GUID NodeGuid = pCurrentNode->m_guidType;
    if (IsEqualGUID(NodeGuid,KSNODETYPE_ACOUSTIC_ECHO_CANCEL))
        tempData = KSALGORITHMINSTANCE_SYSTEM_ACOUSTIC_ECHO_CANCEL;
    else if (IsEqualGUID(NodeGuid,KSNODETYPE_NOISE_SUPPRESS))
        tempData = KSALGORITHMINSTANCE_SYSTEM_NOISE_SUPPRESS;
    else if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;    // as we are not sure waht will be the valid value for the node 
    return Generic_Property_KSNODEPROPERTY(pParams, Small_Buffer,  (LPVOID)&tempData, sizeof (GUID));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_ALGORITHM_INSTANCE_5(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but Large buffer in get as well as set request
{
    GUID tempData;
    CKsNode* pCurrentNode =  NODE();
    GUID NodeGuid = pCurrentNode->m_guidType;
    if (IsEqualGUID(NodeGuid,KSNODETYPE_ACOUSTIC_ECHO_CANCEL))
        tempData = KSALGORITHMINSTANCE_SYSTEM_ACOUSTIC_ECHO_CANCEL;
    else if (IsEqualGUID(NodeGuid,KSNODETYPE_NOISE_SUPPRESS))
        tempData = KSALGORITHMINSTANCE_SYSTEM_NOISE_SUPPRESS;
    else if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;    // as we are not sure waht will be the valid value for the node 
    return Generic_Property_KSNODEPROPERTY(pParams, Large_Buffer,  (LPVOID)&tempData, sizeof (GUID));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_ALGORITHM_INSTANCE(PGAUDITPARAMS pParams)
//sending valid data for valid testing
{
    GUID tempData;
    CKsNode* pCurrentNode =  NODE();
    GUID NodeGuid = pCurrentNode->m_guidType;
    if (IsEqualGUID(NodeGuid,KSNODETYPE_ACOUSTIC_ECHO_CANCEL))
        tempData = KSALGORITHMINSTANCE_SYSTEM_ACOUSTIC_ECHO_CANCEL;
    else if (IsEqualGUID(NodeGuid,KSNODETYPE_NOISE_SUPPRESS))
        tempData = KSALGORITHMINSTANCE_SYSTEM_NOISE_SUPPRESS;
    else if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;    // as we are not sure waht will be the valid value for the node 
    return Generic_Property_KSNODEPROPERTY(pParams, Valid_Test,  (LPVOID)&tempData, sizeof (GUID));
}    
//=========================
// KSNODEPROPERTY and KSAUDIO_CHANNEL_CONFIG
DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_CHANNEL_CONFIG_1(PGAUDITPARAMS pParams)
//sending invalid node id = 0xFFFFFFFF in get as well as set request
{
    KSAUDIO_CHANNEL_CONFIG AudConfig;
    AudConfig.ActiveSpeakerPositions= KSAUDIO_SPEAKER_DIRECTOUT;
    return Generic_Property_KSNODEPROPERTY(pParams, NodeID_ULONGMAX, (LPVOID)&AudConfig, sizeof (KSAUDIO_CHANNEL_CONFIG));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_CHANNEL_CONFIG_2(PGAUDITPARAMS pParams)
//sending invalid node id = 0x80000001 in get as well as set request
{
    KSAUDIO_CHANNEL_CONFIG AudConfig;
    AudConfig.ActiveSpeakerPositions= KSAUDIO_SPEAKER_DIRECTOUT;
    return Generic_Property_KSNODEPROPERTY(pParams, NodeID_0x80000001, (LPVOID)&AudConfig, sizeof (KSAUDIO_CHANNEL_CONFIG));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_CHANNEL_CONFIG_3(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
    KSAUDIO_CHANNEL_CONFIG AudConfig;
    AudConfig.ActiveSpeakerPositions= KSAUDIO_SPEAKER_DIRECTOUT;
    return Generic_Property_KSNODEPROPERTY(pParams, NULL_Buffer, (LPVOID)&AudConfig, sizeof (KSAUDIO_CHANNEL_CONFIG));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_CHANNEL_CONFIG_4(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
    KSAUDIO_CHANNEL_CONFIG AudConfig;
    AudConfig.ActiveSpeakerPositions= KSAUDIO_SPEAKER_DIRECTOUT;
    return Generic_Property_KSNODEPROPERTY(pParams, Small_Buffer, (LPVOID)&AudConfig, sizeof (KSAUDIO_CHANNEL_CONFIG));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_CHANNEL_CONFIG_5(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but Large buffer in get as well as set request
{
    KSAUDIO_CHANNEL_CONFIG AudConfig;
    AudConfig.ActiveSpeakerPositions= KSAUDIO_SPEAKER_DIRECTOUT;
    return Generic_Property_KSNODEPROPERTY(pParams, Large_Buffer, (LPVOID)&AudConfig, sizeof (KSAUDIO_CHANNEL_CONFIG));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_CHANNEL_CONFIG_6(PGAUDITPARAMS pParams)
// sending Invalid channel config - active speaker positions are invalid.
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
        return FNS_PASS;
	{
		CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp,pParams->cbSize, pParams->cbSize,
		                     sizeof(KSAUDIO_CHANNEL_CONFIG), sizeof(KSAUDIO_CHANNEL_CONFIG));

		((PKSAUDIO_CHANNEL_CONFIG)gnt.GetData())->ActiveSpeakerPositions = ULONG_MAX;

		gnt.Test(TRUE, 0);
	if (gnt.GetFault() != FAULT_NO_FAULT)
		  return FNS_FAIL;
	}

    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_CHANNEL_CONFIG(PGAUDITPARAMS pParams)
//sending valid data for valid testing
{
    KSAUDIO_CHANNEL_CONFIG AudConfig;
    AudConfig.ActiveSpeakerPositions= KSAUDIO_SPEAKER_DIRECTOUT;
    return Generic_Property_KSNODEPROPERTY(pParams, Valid_Test, (LPVOID)&AudConfig, sizeof (KSAUDIO_CHANNEL_CONFIG));
}
    
//=========================
// KSNODEPROPERTY and KSAUDIO_DYNAMIC_RANGE
DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DYNAMIC_RANGE_1(PGAUDITPARAMS pParams)
//sending invalid node id = 0xFFFFFFFF in get as well as set request
{
    KSAUDIO_DYNAMIC_RANGE tDynRange;
    tDynRange.LoudCompression = 1;
    tDynRange.QuietCompression = 1;
    return Generic_Property_KSNODEPROPERTY(pParams, NodeID_ULONGMAX,(LPVOID)&tDynRange, sizeof (KSAUDIO_DYNAMIC_RANGE));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DYNAMIC_RANGE_2(PGAUDITPARAMS pParams)
//sending invalid node id = 0x80000001 in get as well as set request
{
    KSAUDIO_DYNAMIC_RANGE tDynRange;
    tDynRange.LoudCompression = 1;
    tDynRange.QuietCompression = 1;
    return Generic_Property_KSNODEPROPERTY(pParams, NodeID_0x80000001,(LPVOID)&tDynRange, sizeof (KSAUDIO_DYNAMIC_RANGE));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DYNAMIC_RANGE_3(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
    KSAUDIO_DYNAMIC_RANGE tDynRange;
    tDynRange.LoudCompression = 1;
    tDynRange.QuietCompression = 1;
    return Generic_Property_KSNODEPROPERTY(pParams, NULL_Buffer,(LPVOID)&tDynRange, sizeof (KSAUDIO_DYNAMIC_RANGE));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DYNAMIC_RANGE_4(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
    KSAUDIO_DYNAMIC_RANGE tDynRange;
    tDynRange.LoudCompression = 1;
    tDynRange.QuietCompression = 1;
    return Generic_Property_KSNODEPROPERTY(pParams, Small_Buffer,(LPVOID)&tDynRange, sizeof (KSAUDIO_DYNAMIC_RANGE));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DYNAMIC_RANGE_5(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but Large buffer in get as well as set request
{
    KSAUDIO_DYNAMIC_RANGE tDynRange;
    tDynRange.LoudCompression = 1;
    tDynRange.QuietCompression = 1;
    return Generic_Property_KSNODEPROPERTY(pParams, Large_Buffer,(LPVOID)&tDynRange, sizeof (KSAUDIO_DYNAMIC_RANGE));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DYNAMIC_RANGE(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but Large buffer in get as well as set request
{
    KSAUDIO_DYNAMIC_RANGE tDynRange;
    tDynRange.LoudCompression = 1;
    tDynRange.QuietCompression = 1;
    return Generic_Property_KSNODEPROPERTY(pParams, Valid_Test,(LPVOID)&tDynRange, sizeof (KSAUDIO_DYNAMIC_RANGE));
}

DWORD WINAPI Generic_Property_KSNODEPROPERTYULONG_SETINVALIDVALUE(PGAUDITPARAMS pParams)
// Set property value to invalid value
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)	{   
        CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp,       // as we are sending Node Property
                              pParams->cbSize, pParams->cbSize, 
                              sizeof(ULONG), sizeof(ULONG));

        *(ULONG*)gnt.GetData() = 0xDEADBEEF;
        DWORD dwResult = gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT) 
			return FNS_FAIL;
    }

	return FNS_PASS;
}
    
//===============================
// KSNODEPROPERTY_AUDIO_CHANNEL and KSTIME
DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DELAY_1(PGAUDITPARAMS pParams)
//sending invalid node id - 0xFFFFFFFF in get as well as set request
{
    KSTIME tTime;
    tTime.Time= 1;
    tTime.Numerator= 1;
    tTime.Denominator= 1;
    return Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, NodeID_ULONGMAX, (LPVOID)&tTime, sizeof (KSTIME));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DELAY_2(PGAUDITPARAMS pParams)
//sending invalid node id - 0x80000001in get as well as set request
{
    KSTIME tTime;
    tTime.Time= 1;
    tTime.Numerator= 1;
    tTime.Denominator= 1;
    return Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, NodeID_0x80000001,(LPVOID)&tTime, sizeof (KSTIME));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DELAY_3(PGAUDITPARAMS pParams)
//sending valid node id  but invalid channel number - 0xFFFFFFFE in get as well as set request
{
    KSTIME tTime;
    tTime.Time= 1;
    tTime.Numerator= 1;
    tTime.Denominator= 1;
    return Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, ChannelID_ULONGMAX_Sub1,(LPVOID)&tTime, sizeof (KSTIME));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DELAY_4(PGAUDITPARAMS pParams)
//sending valid node id  but invalid channel number - 0x80000001 in get as well as set request
{
    KSTIME tTime;
    tTime.Time= 1;
    tTime.Numerator= 1;
    tTime.Denominator= 1;
    return Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, ChannelID_0x80000001, (LPVOID)&tTime, sizeof (KSTIME));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DELAY_5(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but NULL Buffer in get as well as set request
{
    KSTIME tTime;
    tTime.Time= 1;
    tTime.Numerator= 1;
    tTime.Denominator= 1;
    return Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, NULL_Buffer, (LPVOID)&tTime, sizeof (KSTIME));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DELAY_6(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but small buffer in get as well as set request
{
    KSTIME tTime;
    tTime.Time= 1;
    tTime.Numerator= 1;
    tTime.Denominator= 1;
    return Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, Small_Buffer, (LPVOID)&tTime, sizeof (KSTIME));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DELAY_7(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but Large buffer in get as well as set request
{
    KSTIME tTime;
    tTime.Time= 1;
    tTime.Numerator= 1;
    tTime.Denominator= 1;
    return Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, Large_Buffer, (LPVOID)&tTime, sizeof (KSTIME));
}
    
    DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DELAY_8(PGAUDITPARAMS pParams)
    //sending valid node id, valid channel number but negative time value in get as well as set request
    {
// in case of GET request, it will successed as we are passing valid input data
        if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) 
            return FNS_PASS;

        ULONG cbGoodSize = sizeof(KSNODEPROPERTY_AUDIO_CHANNEL);
        PKSNODEPROPERTY_AUDIO_CHANNEL  pPropertyGood = (PKSNODEPROPERTY_AUDIO_CHANNEL)new BYTE[cbGoodSize];
    
        {
            memcpy(&pPropertyGood->NodeProperty, &pParams->NodeProp, pParams->cbSize);
            pPropertyGood->Channel = -1;
    
            CGenericIoControl gnt(pParams->hFile, pPropertyGood , cbGoodSize, cbGoodSize,
                                                                     sizeof(KSTIME), sizeof(KSTIME));
    
            ((PKSTIME)gnt.GetData())->Time = -1;     //Negative time value
            ((PKSTIME)gnt.GetData())->Numerator = 1;
            ((PKSTIME)gnt.GetData())->Denominator = 1;
    
            gnt.Test(TRUE, 0);
    
            if (gnt.GetFault() != FAULT_NO_FAULT)
            {
                  delete[] pPropertyGood;
                  return FNS_FAIL;
            }
        }
    
        delete[] pPropertyGood;
    
        return FNS_PASS;
    }
    
    DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DELAY_9(PGAUDITPARAMS pParams)
    //sending valid node id, valid channel number but zero numerator in get as well as set request
    {
        // in case of GET request, it will successed as we are passing valid input data
                if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) 
                    return FNS_PASS;

        ULONG cbGoodSize = sizeof(KSNODEPROPERTY_AUDIO_CHANNEL);
        PKSNODEPROPERTY_AUDIO_CHANNEL  pPropertyGood = (PKSNODEPROPERTY_AUDIO_CHANNEL)new BYTE[cbGoodSize];
    
        {
            memcpy(&pPropertyGood->NodeProperty, &pParams->NodeProp, pParams->cbSize);
            pPropertyGood->Channel = -1;
    
            CGenericIoControl gnt(pParams->hFile, pPropertyGood , cbGoodSize, cbGoodSize,
                                                                     sizeof(KSTIME), sizeof(KSTIME));
    
            ((PKSTIME)gnt.GetData())->Time = 1;
            ((PKSTIME)gnt.GetData())->Numerator = 0; //zero numerator
            ((PKSTIME)gnt.GetData())->Denominator = 1;
    
            gnt.Test(TRUE, 0);
    
            if (gnt.GetFault() != FAULT_NO_FAULT)
            {
                  delete[] pPropertyGood;
                  return FNS_FAIL;
            }
        }
    
        delete[] pPropertyGood;
    
        return FNS_PASS;
    }
    
    DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DELAY_10(PGAUDITPARAMS pParams)
    //sending valid node id, valid channel number but Large buffer in get as well as set request
    {
        // in case of GET request, it will successed as we are passing valid input data
                if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET) 
                    return FNS_PASS;
        ULONG cbGoodSize = sizeof(KSNODEPROPERTY_AUDIO_CHANNEL);
        PKSNODEPROPERTY_AUDIO_CHANNEL  pPropertyGood = (PKSNODEPROPERTY_AUDIO_CHANNEL)new BYTE[cbGoodSize];
    
        {
            memcpy(&pPropertyGood->NodeProperty, &pParams->NodeProp, pParams->cbSize);
            pPropertyGood->Channel = -1;
    
            CGenericIoControl gnt(pParams->hFile, pPropertyGood , cbGoodSize, cbGoodSize,
                                                                     sizeof(KSTIME), sizeof(KSTIME));
    
            ((PKSTIME)gnt.GetData())->Time = 1;
            ((PKSTIME)gnt.GetData())->Numerator = 1;
            ((PKSTIME)gnt.GetData())->Denominator = 0; //zero denominator
    
            gnt.Test(TRUE, 0);
    
            if (gnt.GetFault() != FAULT_NO_FAULT)
            {
                  delete[] pPropertyGood;
                  return FNS_FAIL;
            }
        }
    
        delete[] pPropertyGood;
    
        return FNS_PASS;
    }

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_DELAY(PGAUDITPARAMS pParams)
//sending valid data for valid testing
{
    KSTIME tTime;
    tTime.Time= 1;
    tTime.Numerator= 1;
    tTime.Denominator= 1;
    return Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, Valid_Test, (LPVOID)&tTime, sizeof (KSTIME));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_LATENCY_1(PGAUDITPARAMS pParams)
// Out buffer NULL
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);

    	gnt.Test(TRUE, (ULONG)-1);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_LATENCY_2(PGAUDITPARAMS pParams)
// Out buffer Small
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                                      sizeof(KSTIME)-1, sizeof(KSTIME)-1);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          return FNS_FAIL;
    }
return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_LATENCY_3(PGAUDITPARAMS pParams)
// Out buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                                      sizeof(KSTIME)+1, sizeof(KSTIME)+1);

        gnt.Test(FALSE, sizeof(KSTIME));
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_LATENCY(PGAUDITPARAMS pParams)
// Valid test data
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                                      sizeof(KSTIME), sizeof(KSTIME));

        gnt.Test(FALSE, sizeof(KSTIME));
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

/*
//=========================
// KSNODEPROPERTY and Array of KSAUDIO_MIXLEVEL structures
// Incomplete functions :
DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_TABLE_1(PGAUDITPARAMS pParams)
//sending invalid node id = 0xFFFFFFFF in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY(pParams, NodeID_ULONGMAX);
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_TABLE_2(PGAUDITPARAMS pParams)
//sending invalid node id = 0x80000001 in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY(pParams, NodeID_0x80000001);
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_TABLE_3(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
	return Generic_Property_KSNODEPROPERTY(pParams, NULL_Buffer);
}

//=============================== 
*/

/* ___________________________________________________________________________

    @TEST KSPROPERTY_AUDIO_EQ_BANDS
   ___________________________________________________________________________ */
DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_BANDS_1(PGAUDITPARAMS pParams)
// Invalid Node ID ULONG_MAX
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_BANDS;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, NodeID_ULONGMAX, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
    
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_BANDS_2(PGAUDITPARAMS pParams)
// Invalid Node ID 0x80000001
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_BANDS;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, NodeID_0x80000001, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_BANDS_3(PGAUDITPARAMS pParams)
// Invalid channel ID - 0xFFFFFFFE
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_BANDS;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, ChannelID_ULONGMAX_Sub1, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_BANDS_4(PGAUDITPARAMS pParams)
// Invalid channel ID - 0x80000001
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_BANDS;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, ChannelID_0x80000001, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_BANDS_5(PGAUDITPARAMS pParams)
// Out buffer NULL
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_BANDS;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, NULL_Buffer, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_BANDS_6(PGAUDITPARAMS pParams)
// Out buffer Small
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_BANDS;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, Small_Buffer, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_BANDS_7(PGAUDITPARAMS pParams)
// Out buffer Large
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_BANDS;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, Large_Buffer, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_BANDS(PGAUDITPARAMS pParams)
// Out buffer Large
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
    // first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_BANDS;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, Valid_Test, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}


/* ___________________________________________________________________________

    @TEST KSPROPERTY_AUDIO_EQ_LEVEL
   ___________________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_LEVEL_1(PGAUDITPARAMS pParams)
// Invalid Node ID ULONG_MAX
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_LEVEL;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, NodeID_ULONGMAX, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
    
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_LEVEL_2(PGAUDITPARAMS pParams)
// Invalid Node ID 0x80000001
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_LEVEL;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, NodeID_0x80000001, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_LEVEL_3(PGAUDITPARAMS pParams)
// Invalid channel ID - 0xFFFFFFFE
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_LEVEL;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, ChannelID_ULONGMAX_Sub1, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_LEVEL_4(PGAUDITPARAMS pParams)
// Invalid channel ID - 0x80000001
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_LEVEL;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, ChannelID_0x80000001, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_LEVEL_5(PGAUDITPARAMS pParams)
// Out buffer NULL
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_LEVEL;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, NULL_Buffer, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_LEVEL_6(PGAUDITPARAMS pParams)
// Out buffer Small
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_LEVEL;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, Small_Buffer, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_LEVEL_7(PGAUDITPARAMS pParams)
// Out buffer Large
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
	// first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_LEVEL;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, Large_Buffer, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_EQ_LEVEL(PGAUDITPARAMS pParams)
// Out buffer Large
{
    DWORD dwResult = FNS_PASS;
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET) 
        return FNS_PASS;
    // first get the value of no of bands
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)    {
        LONG nBands = 0;
        pParams->Prop.Id = KSPROPERTY_AUDIO_NUM_EQ_BANDS;
        dwResult = Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG(pParams, Valid_Test, &nBands);
        if ((dwResult == FNS_PASS)  &&  (nBands != 0))  { // we have vailid values for bands
            SLOG ( eInfo2 , "No of bands equal to %d", nBands);
            ULONG dataSize = nBands * sizeof(ULONG);
            pParams->Prop.Id = KSPROPERTY_AUDIO_EQ_LEVEL;    
            PULONG  pCenterFreqVal=  (PULONG) new BYTE [dataSize];
            dwResult = Generic_Property_KSAUDIOCHANNELNODEPROPERTY(pParams, Valid_Test, (LPVOID)pCenterFreqVal, dataSize);
            if (pCenterFreqVal)
                delete[] pCenterFreqVal;
        }
    }
    return dwResult;
}


/* ___________________________________________________________________________

    @TEST KSPROPERTY_AUDIO_FILTER_STATE
   ___________________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_FILTER_STATE_1(PGAUDITPARAMS pParams)
// Out buffer NULL
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);

    	gnt.Test(TRUE, (ULONG)-1);
       if (gnt.GetFault() != FAULT_NO_FAULT)
            return FNS_FAIL;
    }
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_FILTER_STATE_2(PGAUDITPARAMS pParams)
// Out buffer Small
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                                      sizeof(GUID)-1, sizeof(GUID)-1);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
          return FNS_FAIL;
    }
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_FILTER_STATE_3(PGAUDITPARAMS pParams)
// Out buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;
    // first retrive the buffer size -Size of array of GUID
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                                      0, 0);

        gnt.Test(TRUE, 0);
        if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
        ULONG cbData = gnt.GetWritten();
//        GUID* aguidPropertySets = NULL;
//        aguidPropertySets = (GUID*)new BYTE[cbData];
 
        CGenericIoControl gnt1(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                                cbData +1, cbData+1 );
        gnt1.Test(FALSE, cbData);
        if (gnt1.GetFault() != FAULT_NO_FAULT)
            return FNS_FAIL;

    }
    return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_FILTER_STATE(PGAUDITPARAMS pParams)
// Valid Testing
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
    	return FNS_PASS;
    // first retrive the buffer size -Size of array of GUID
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                                      0, 0);

        gnt.Test(TRUE, 0);
        if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
        ULONG cbData = gnt.GetWritten();
//        GUID* aguidPropertySets = NULL;
//        aguidPropertySets = (GUID*)new BYTE[cbData];
 
        CGenericIoControl gnt1(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                                cbData , cbData );
        gnt1.Test(FALSE, cbData);
        if (gnt1.GetFault() != FAULT_NO_FAULT)
            return FNS_FAIL;

    }
    return FNS_PASS;
}

/* ___________________________________________________________________________

    @TEST KSPROPERTY_AUDIO_MIX_LEVEL_TABLE
   ___________________________________________________________________________ */


DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_TABLE_1(PGAUDITPARAMS pParams)
//sending invalid node id = 0xFFFFFFFF in get as well as set request
{
    KSAUDIO_MIXLEVEL tMixCaps;
    tMixCaps.Mute = FALSE;
    tMixCaps.Level= 0;
    return Generic_Property_KSNODEPROPERTY(pParams, NodeID_ULONGMAX, (LPVOID)&tMixCaps, sizeof (KSAUDIO_MIXLEVEL));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_TABLE_2(PGAUDITPARAMS pParams)
//sending invalid node id = 0x80000001 in get as well as set request
{
    KSAUDIO_MIXLEVEL tMixCaps;
    tMixCaps.Mute = FALSE;
    tMixCaps.Level= 0;
    return Generic_Property_KSNODEPROPERTY(pParams, NodeID_0x80000001, (LPVOID)&tMixCaps, sizeof (KSAUDIO_MIXLEVEL));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_TABLE_3(PGAUDITPARAMS pParams)
//sending valid node id but NULL buffer in get as well as set request
{
    KSAUDIO_MIXLEVEL tMixCaps;
    tMixCaps.Mute = FALSE;
    tMixCaps.Level= 0;
    return Generic_Property_KSNODEPROPERTY(pParams, NULL_Buffer, (LPVOID)&tMixCaps, sizeof (KSAUDIO_MIXLEVEL));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_TABLE_4(PGAUDITPARAMS pParams)
//sending valid node id but small buffer than required in get as well as set request
{
    KSAUDIO_MIXLEVEL tMixCaps;
    tMixCaps.Mute = FALSE;
    tMixCaps.Level= 0;
    return Generic_Property_KSNODEPROPERTY(pParams, Small_Buffer, (LPVOID)&tMixCaps, sizeof (KSAUDIO_MIXLEVEL));
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_TABLE_5(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number but Large buffer in get request
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;
    // we need to find array size first using KSPROPERTY_AUDIO_MIX_LEVEL_CAPS 
    PKSAUDIO_MIXCAP_TABLE pmxtbl = NULL;
    SLOG (eInfo2, "send KSPROPERTY_AUDIO_MIX_LEVEL_CAPS request to know input and output channels");
    pParams->Prop.Id = KSPROPERTY_AUDIO_MIX_LEVEL_CAPS ;    
    CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp,
                                        pParams->cbSize, pParams->cbSize, 
                                        (2*sizeof(ULONG)),(2*sizeof(ULONG)));     //sending output buffer enough to have information about input and output channels. 

    pmxtbl = (PKSAUDIO_MIXCAP_TABLE)gnt.GetData();
    DWORD dwResult = gnt.Test(FALSE, (2*sizeof(ULONG)));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    ULONG ulInputChannels = pmxtbl->InputChannels;
    ULONG ulOtputChannels = pmxtbl->OutputChannels;
    ULONG BufferSize = ulInputChannels * ulOtputChannels * sizeof(KSAUDIO_MIXLEVEL);
    SLOG (eInfo2, "size required to get information about MixLevel is %d",BufferSize);
    PKSAUDIO_MIXLEVEL pMixCaps = (PKSAUDIO_MIXLEVEL) new BYTE[BufferSize];
    // check for pMixCaps
    SLOG (eInfo2, "send KSPROPERTY_AUDIO_MIX_LEVEL_TABLE "); 
    pParams->Prop.Id = KSPROPERTY_AUDIO_MIX_LEVEL_TABLE;
    dwResult = Generic_Property_KSNODEPROPERTY(pParams, Large_Buffer, (LPVOID)&pMixCaps, BufferSize);    
    return dwResult;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_MIX_LEVEL_TABLE(PGAUDITPARAMS pParams)
//sending valid node id, valid channel number - valid testing
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;
    // we need to find array size first using KSPROPERTY_AUDIO_MIX_LEVEL_CAPS 
    PKSAUDIO_MIXCAP_TABLE pmxtbl = NULL;
    pParams->Prop.Id = KSPROPERTY_AUDIO_MIX_LEVEL_CAPS ;    
    SLOG (eInfo2, "send KSPROPERTY_AUDIO_MIX_LEVEL_CAPS request to know input and output channels");
    CGenericIoControl gnt(pParams->hFile, &pParams->NodeProp,
                                        pParams->cbSize, pParams->cbSize, 
                                        (2*sizeof(ULONG)),(2*sizeof(ULONG)));     //sending output buffer enough to have information about input and output channels. 

    pmxtbl = (PKSAUDIO_MIXCAP_TABLE)gnt.GetData();
    DWORD dwResult = gnt.Test(FALSE, (2*sizeof(ULONG)));
    if (gnt.GetFault() != FAULT_NO_FAULT) 
        return FNS_FAIL;
    ULONG ulInputChannels = pmxtbl->InputChannels;
    ULONG ulOtputChannels = pmxtbl->OutputChannels;
    ULONG BufferSize = ulInputChannels * ulOtputChannels * sizeof(KSAUDIO_MIXLEVEL);
    PKSAUDIO_MIXLEVEL pMixCaps = (PKSAUDIO_MIXLEVEL) new BYTE[BufferSize];
    SLOG (eInfo2, "size required to get information about MixLevel is %d",BufferSize);
    // check for pMixCaps
    pParams->Prop.Id = KSPROPERTY_AUDIO_MIX_LEVEL_TABLE;
    dwResult = Generic_Property_KSNODEPROPERTY(pParams, Valid_Test, (LPVOID)&pMixCaps, BufferSize);    
    return dwResult;
}

/* ___________________________________________________________________________

    @TEST KSPROPERTY_AUDIO_POSITION
   ___________________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_POSITION_1(PGAUDITPARAMS pParams)
// Out buffer NULL
{
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize, 0, 0);

        gnt.Test(TRUE, (ULONG)-1);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_POSITION_2(PGAUDITPARAMS pParams)
// Out buffer Small
{
    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                                      sizeof(KSAUDIO_POSITION)-1, sizeof(KSAUDIO_POSITION)-1);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_POSITION_3(PGAUDITPARAMS pParams)
// Out buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_SET)
        return FNS_PASS;
// TODO: needs to find out that can we just inquire get audio_position without submitting any buffer or setting pin into correct state. Once we Find out then we can have proper test case for it.
/*    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                                      sizeof(KSAUDIO_POSITION)+1, sizeof(KSAUDIO_POSITION)+1);
		KSAUDIO_POSITION tKsAudioPos;
		tKsAudioPos = *(PKSAUDIO_POSITION)gnt.GetData();
		tKsAudioPos.PlayOffset = 0;
		tKsAudioPos.WriteOffset = 0;
        gnt.Test(FALSE, sizeof(KSAUDIO_POSITION));
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }*/
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_POSITION_4(PGAUDITPARAMS pParams)
// Write Offset < Play Offset
{

    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;
// TODO: needs to find out that can we just inquire get audio_position without submitting any buffer or setting pin into correct state. Once we Find out then we can have proper test case for it.

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                                        sizeof(KSAUDIO_POSITION), sizeof(KSAUDIO_POSITION));

		KSAUDIO_POSITION tKsAudioPos;
		tKsAudioPos = *(PKSAUDIO_POSITION)gnt.GetData();
		tKsAudioPos.PlayOffset = 100;
		tKsAudioPos.WriteOffset = 0;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}


// preferred-status stuff is XP-only
#if (NTDDI_VERSION < NTDDI_VISTA)

/* ___________________________________________________________________________

    @TEST KSPROPERTY_AUDIO_PREFERRED_STATUS
   ___________________________________________________________________________ */

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_PREFERRED_STATUS_1(PGAUDITPARAMS pParams)
// Out buffer NULL
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

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_PREFERRED_STATUS_2(PGAUDITPARAMS pParams)
// Out buffer Small
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                          sizeof(KSAUDIO_PREFERRED_STATUS)-1, sizeof(KSAUDIO_PREFERRED_STATUS)-1);

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_PREFERRED_STATUS_3(PGAUDITPARAMS pParams)
// Out buffer Large
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                           sizeof(KSAUDIO_PREFERRED_STATUS)+1, sizeof(KSAUDIO_PREFERRED_STATUS)+1);

        ((PKSAUDIO_PREFERRED_STATUS)gnt.GetData())->Enable = TRUE;
        ((PKSAUDIO_PREFERRED_STATUS)gnt.GetData())->DeviceType = KSPROPERTY_SYSAUDIO_NORMAL_DEFAULT;

        gnt.Test(FALSE, 0); // since we are testing for set request we will get zero bytes written
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_PREFERRED_STATUS_4(PGAUDITPARAMS pParams)
// Invalid Device Type - ULONG_MAX
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                          sizeof(KSAUDIO_PREFERRED_STATUS), sizeof(KSAUDIO_PREFERRED_STATUS));

        ((PKSAUDIO_PREFERRED_STATUS)gnt.GetData())->Enable = TRUE;
        ((PKSAUDIO_PREFERRED_STATUS)gnt.GetData())->DeviceType = (KSPROPERTY_SYSAUDIO_DEFAULT_TYPE)ULONG_MAX;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

DWORD WINAPI Generic_Property_KSPROPERTY_AUDIO_PREFERRED_STATUS_5(PGAUDITPARAMS pParams)
// Invalid Device Type - 0x80000001
{
    if (pParams->Prop.Flags & KSPROPERTY_TYPE_GET)
    	return FNS_PASS;

    {
        CGenericIoControl gnt(pParams->hFile, &pParams->Prop, pParams->cbSize, pParams->cbSize,
                                           sizeof(KSAUDIO_PREFERRED_STATUS), sizeof(KSAUDIO_PREFERRED_STATUS));

        ((PKSAUDIO_PREFERRED_STATUS)gnt.GetData())->Enable = TRUE;
        ((PKSAUDIO_PREFERRED_STATUS)gnt.GetData())->DeviceType = (KSPROPERTY_SYSAUDIO_DEFAULT_TYPE)0x80000001;

        gnt.Test(TRUE, 0);
		if (gnt.GetFault() != FAULT_NO_FAULT)
        	return FNS_FAIL;
    }
	return FNS_PASS;
}

#endif // NTDDI_VERSION < NTDDI_VISTA
