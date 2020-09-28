// Copyright(C) Microsoft.All rights reserved.

// channels.cpp

#include "common.h"
#include "eqbands.h"
#include "channels.h"

// Testing for in - KSNODEPROPERTY_AUDIO_CHANNEL and result LONG/ULONG and BOOL 
DWORD Generic_Property_KSNODEPROPERTY_AUDIO_CHANNEL_ULONG_Channel(ULONG ulChannel, IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase, OPTIONAL IN PLONG pResultData)
{
    ULONG OutBufferSize;    //Output bufferzise
    LPVOID pSetData; // pointer to hold data required to send for SET request
    DWORD dwResult;     //used for test result
    BOOL bExpectedTestResult = TRUE;    // as normally test case must fail as we are passing invalid parameters 
    ULONG cbExpectedWritten = 0;    //as normally test case must fail as we are passing invalid parameters 
    LONG tLONGData = 1;
    KSNODEPROPERTY npOriginalNodeProperty = pParams->NodeProp;
    LONG tOriginalData = 0;
    DWORD dwOriginalFlags;
    BOOL fRestoreOriginalData = FALSE;
    OutBufferSize = sizeof(ULONG);
    pSetData = &tLONGData;
    KSNODEPROPERTY_AUDIO_CHANNEL pNodeAudioChannelProperty;
    pNodeAudioChannelProperty.NodeProperty = npOriginalNodeProperty;
    pNodeAudioChannelProperty.Channel = ulChannel; // no more -1
    pNodeAudioChannelProperty.Reserved = 0;
    CKsNode* pCurrentNode =  NODE();
    GUID NodeGuid = pCurrentNode->m_guidType;
    DWORD dwOverallResult = FNS_PASS;

    switch (pParams->NodeProp.Property.Id)   {    
        case KSPROPERTY_AUDIO_BASS: 
            tLONGData = -196608; 
            if (IsEqualGUID(NodeGuid,KSNODETYPE_TONE))
                SLOG (eInfo1, "Test is correct");
            break;
        case KSPROPERTY_AUDIO_MID:             
            tLONGData = 1; 
            break;
        case KSPROPERTY_AUDIO_NUM_EQ_BANDS:      
            tLONGData = 1;
            break;
        case KSPROPERTY_AUDIO_PEAKMETER:
            break;
        case KSPROPERTY_AUDIO_TREBLE: 
            tLONGData = -196608; 
            break;            
        case KSPROPERTY_AUDIO_VOLUMELEVEL: 
            tLONGData = 0; 
            break;            
        case KSPROPERTY_AUDIO_AGC: 
            
            break;            
        case KSPROPERTY_AUDIO_BASS_BOOST: 
            
            break;            
        case KSPROPERTY_AUDIO_LOUDNESS:
            
            break;            
        case KSPROPERTY_AUDIO_MUTE:
            tLONGData = TRUE;
            if (IsEqualGUID(NodeGuid,KSNODETYPE_MUTE)) {
                SLOG (eInfo2,"This is mute type node and it should support this property");
            }
            if (pNodeAudioChannelProperty.NodeProperty.Property.Flags & KSPROPERTY_TYPE_SET) {
                SLOG(eInfo1, "Querying whether mute is originally set...");
                dwOriginalFlags = pNodeAudioChannelProperty.NodeProperty.Property.Flags;
                // change SET to GET
                pNodeAudioChannelProperty.NodeProperty.Property.Flags &= ~KSPROPERTY_TYPE_SET;
                pNodeAudioChannelProperty.NodeProperty.Property.Flags |= KSPROPERTY_TYPE_GET;
                CGenericIoControl gntOriginal(
                    pParams->hFile,
                    &pNodeAudioChannelProperty, 
                    sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                    sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                    sizeof(ULONG),
                    sizeof(ULONG)
                );
                // FALSE means we expect this to succeed
                // sizeof(ULONG) is how much data we expect returned to us
                dwResult = gntOriginal.Test(FALSE, sizeof(ULONG));
                if (sizeof(ULONG) == gntOriginal.GetWritten()) {
                    pSetData = (gntOriginal.GetData());
                    tOriginalData = *(ULONG*)pSetData;
                    SLOG(eInfo1, "Original mute setting: %u", tOriginalData);
                    tLONGData = !tOriginalData;
                    SLOG(eInfo1, "Will set mute to %u", tLONGData);
                    fRestoreOriginalData = TRUE;
                    pSetData = &tLONGData;
                } else {
                    // not necessarily a big deal - node might not support mute
                    SLOG(eInfo1, "Could not get original value of the mute node.");
                }
                pNodeAudioChannelProperty.NodeProperty.Property.Flags = dwOriginalFlags;
            }
            break;            
    }    
    switch (TestCase)   {
        case NodeID_ULONGMAX:
            pNodeAudioChannelProperty.NodeProperty.NodeId = ULONG_MAX;  //setting node id to invalid node id and passing all other parameters correct        
            break;
        case NodeID_0x80000001:
            pNodeAudioChannelProperty.NodeProperty.NodeId = 0x80000001;  //setting node id to invalid node id and passing all other parameters correct        
            break;    
        case ChannelID_ULONGMAX_Sub1:
            pNodeAudioChannelProperty.Channel = 0xFFFFFFFE;  //setting channel as invalid.   
            break;    
        case ChannelID_0x80000001:
            pNodeAudioChannelProperty.Channel = 0x80000001;  //setting channel as invalid.   
            break;    
        case NULL_Buffer:
            OutBufferSize = 0;  //sending null buffer.   
            break;    
        case Small_Buffer:
            OutBufferSize = OutBufferSize - 1;  //sending small buffer then required one
            break;
        case Large_Buffer:
            OutBufferSize = OutBufferSize + 1;  // lying about buffer size
            // ks.sys might step in, in which case DeviceIoControl should fail
            // if ks.sys doesn't step in, then the overwrite check is necessary
            bExpectedTestResult = -1;
            cbExpectedWritten = -1;
            break;
        case Valid_Test:
            cbExpectedWritten = OutBufferSize;
            bExpectedTestResult = FALSE; // should succeed
            break;

        default:
            _ASSERT(FALSE);
            return FNS_FAIL;
    }
    // TODO: we need to verify that in case of setting property, data returns by the driver should not be more then the buffer lenght supplied to it
    // since for set request we are not expecting any data to be writtne back. 
    //Ideally we should expect zero bytes as written, however there is a bug in some of drivers that they are returning number during set request. However this was take care by KS so that IO manager will not copy data back to user mode for Property Set request
    if (pNodeAudioChannelProperty.NodeProperty.Property.Flags & KSPROPERTY_TYPE_SET)
        cbExpectedWritten = (ULONG) -1;
        
    CGenericIoControl gnt(
        pParams->hFile,
        &pNodeAudioChannelProperty, 
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
        OutBufferSize,
        OutBufferSize
    );
    memcpy(gnt.GetData(), pSetData, OutBufferSize);
    dwResult = gnt.Test(bExpectedTestResult, cbExpectedWritten);
    if (sizeof(LONG) == gnt.GetWritten())   {
        pSetData = (gnt.GetData());
        tLONGData = *(LONG*)pSetData;
        if (pResultData)
            *pResultData = tLONGData;
        SLOG (eInfo1,"Value of data is 0x%x", tLONGData);
    }
    if (FAULT_NO_FAULT != gnt.GetFault()) {
        SLOG(eError, "FAIL: fault == %u", gnt.GetFault());
        dwOverallResult = FNS_FAIL;
    }

    if (fRestoreOriginalData) {
        SLOG(eError, "Restoring the original setting of %u", tOriginalData);
        // erase any distortions made for the purpose of testing driver data validation
        pNodeAudioChannelProperty.NodeProperty = npOriginalNodeProperty;
        pNodeAudioChannelProperty.Channel = ulChannel;
        CGenericIoControl gntRestore(
            pParams->hFile,
            &pNodeAudioChannelProperty,
            sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
            sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
            sizeof(ULONG),
            sizeof(ULONG)
        );
        memcpy(gntRestore.GetData(), &tOriginalData, sizeof(tOriginalData));
        // FALSE means we expect this to succeed
        // -1 means we don't care whether data is returned to us
        dwResult = gntRestore.Test(FALSE, -1);
        if (FAULT_NO_FAULT == gntRestore.GetFault()) {
            SLOG(eInfo1, "Write of original data was successful");
        } else {
            SLOG(eError, "FAIL: Could not write original value of %u back", tOriginalData);
            dwOverallResult = FNS_FAIL;
        }
    }
    
    return dwOverallResult;
}

// the property ulPropId will be used to find the number of channels
// this is done by means of a basic support request, and n + 1 get requests
DWORD GetChannelCount(
    HANDLE hFile,
    GUID guidPropSet,
    ULONG ulPropId,
    ULONG ulNodeId,
    OUT PULONG pulChannels,
    OUT PBOOL pbBasicSupport,
    OUT PBOOL pbBasicSupportMultichannel
) {

    ULONG OutBufferSize = 0;    //Output buffersize
    DWORD dwResult = FNS_PASS;     //used for test result
    ULONG AccessFlag = 0;
    ULONG cbExpectedWritten = 0;

    KSNODEPROPERTY_AUDIO_CHANNEL NodeAudioChannelProperty = {0};
    NodeAudioChannelProperty.NodeProperty.Property.Set = guidPropSet;
    NodeAudioChannelProperty.NodeProperty.Property.Id = ulPropId;
    NodeAudioChannelProperty.NodeProperty.Property.Flags =
        KSPROPERTY_TYPE_TOPOLOGY | KSPROPERTY_TYPE_BASICSUPPORT;
        // KSPROPERTY_TYPE_TOPOLOGY goes with KSNODEPROPERTY
    NodeAudioChannelProperty.NodeProperty.NodeId = ulNodeId;
    NodeAudioChannelProperty.NodeProperty.Reserved = 0;
    NodeAudioChannelProperty.Channel = 0; // use channel 0 for the basic support request
    NodeAudioChannelProperty.Reserved = 0;
    
    // send ULONG size buffer for access flags
    OutBufferSize = sizeof(ULONG);                
    CGenericIoControl gnt_ulong(  hFile, &NodeAudioChannelProperty, 
                            sizeof(KSNODEPROPERTY_AUDIO_CHANNEL), sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                            OutBufferSize, OutBufferSize);
    cbExpectedWritten = sizeof(ULONG);
    dwResult = gnt_ulong.Test(FALSE, cbExpectedWritten);

    if (gnt_ulong.GetFault() != FAULT_NO_FAULT) {
        SLOG (eError, "Passing buffer of ULONG size for the Basic support request failed");
        return FNS_FAIL;
    }

    // get the result of access flag
    AccessFlag = *(ULONG *)(gnt_ulong.GetData());

    // send one byte buffer
    OutBufferSize = 1;                
    CGenericIoControl gnt_byte(  hFile, &NodeAudioChannelProperty, 
                            sizeof(KSNODEPROPERTY_AUDIO_CHANNEL), sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                            OutBufferSize, OutBufferSize);
    cbExpectedWritten = 0;
    dwResult = gnt_byte.Test(TRUE, cbExpectedWritten);
    // Hmmm... is it reasonable to require a particular error code of all drivers?  I don't think so...
    if (FAULT_NO_FAULT != gnt_byte.GetFault()) {
        SLOG (eError,
            "ERROR: Passing buffer of 1 byte size for the Basic support request incorrectly succeeded with fault %u",
            gnt_byte.GetFault()
        );
        return FNS_FAIL;
    } else if (ERROR_INSUFFICIENT_BUFFER != dwResult) {
        SLOG (eError,
            "ERROR: Passing buffer of one byte resulted in error %u - expected %u (ERROR_INSUFFICIENT_BUFFER)",
            dwResult, ERROR_INSUFFICIENT_BUFFER
        );
        return FNS_FAIL;
    }
    dwResult = FNS_PASS;

    BOOL bBasicSupport = FALSE;
    BOOL bBasicSupportMultichannel = FALSE;
    ULONG ulChannelsInHeader = 0;

    if ( 0 == (KSPROPERTY_TYPE_GET & AccessFlag) ) {
        // not supported!
        SLOG (eError, "GET not supported; cannot iteratively check channels");
        return FNS_FAIL;
    }
    if ( 0 == (KSPROPERTY_TYPE_BASICSUPPORT & AccessFlag) ) {   // that means that driver provides further support for basic-support property requests
        SLOG (eInfo2, "No further information from basic support request");
        // will calculate number of channels manually after this massive else clause
    } else {

        bBasicSupport = TRUE;

        // send KSPROPERTY_DESCRIPTION-sized buffer
        OutBufferSize = sizeof(KSPROPERTY_DESCRIPTION);
        CGenericIoControl gnt_desc(  hFile, &NodeAudioChannelProperty, 
                                 sizeof(KSNODEPROPERTY_AUDIO_CHANNEL), sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                                 OutBufferSize, OutBufferSize);    
        cbExpectedWritten = sizeof(KSPROPERTY_DESCRIPTION);    
        dwResult = gnt_desc.Test(FALSE, cbExpectedWritten);
        if (gnt_desc.GetFault() != FAULT_NO_FAULT) {
            SLOG (eError, "Passing buffer of KSPROPERTY_DESCRIPTION size for the Basic support request failed");
            return FNS_FAIL;
        }
        KSPROPERTY_DESCRIPTION PropertyDes = *(KSPROPERTY_DESCRIPTION*)(gnt_desc.GetData());
        OutBufferSize = PropertyDes.DescriptionSize;

        //Check the DescriptionSize
        SLOG(eInfo1, "Validate DescriptionSize for Node Properties");
        //Validate that the description size is not less than sizeof(KSPROPERTY_DESCRIPTION) and
        //that it is not more than 262144 bytes (1/4 MB), this upper limit comes from wdmaud.sys
        //see definition for MEMORY_LIMIT_CHECK in robust.c in wdmaud.sys
        //This validation check stems from the check that wdmaud.sys does before allocating
        //memory for the driver (kmxlQueryPropertyRange in kmxltop.c in wdmaud.sys).
        if(OutBufferSize < sizeof(KSPROPERTY_DESCRIPTION) ||
            OutBufferSize > 262144 ) 
        {       
            SLOG(eError, "ERROR: Property Call returned DescriptionSize = %d", OutBufferSize);
            return FNS_FAIL;
        }

        // send a full-sized buffer
        CGenericIoControl gnt_full(  hFile, &NodeAudioChannelProperty, 
                                 sizeof(KSNODEPROPERTY_AUDIO_CHANNEL), sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                                 OutBufferSize, OutBufferSize);    
        cbExpectedWritten = OutBufferSize;    
        dwResult = gnt_full.Test(FALSE, cbExpectedWritten);
        if (gnt_full.GetFault() != FAULT_NO_FAULT) {
            SLOG (eError, "Passing buffer of size %d for the Basic support request failed", OutBufferSize);
            return FNS_FAIL;
        }

        // two possibilities:
        // either the KSPROPERTY_DESCRIPTION is it, and ->MembersListCount is 0 (channels == -1)
        // or there is a single KSPROPERTY_MEMBERSHEADER following, containing the channel count
        // anything else is unexpected.
        PropertyDes = *(KSPROPERTY_DESCRIPTION*)gnt_full.GetData();

        // two allowable response sizes
        if ( // a single description by itself is old-school behavior
            0 == PropertyDes.MembersListCount &&
            sizeof(KSPROPERTY_DESCRIPTION) == OutBufferSize
        ) {
            SLOG(eInfo1, "No members header... need to ascertain channel count iteratively");
        } else if ( // description + members header (+ optional further data) is new-school
            1 == PropertyDes.MembersListCount &&
            OutBufferSize >= sizeof(KSPROPERTY_DESCRIPTION) + sizeof(KSPROPERTY_MEMBERSHEADER)
        ) {
            SLOG(eInfo1, "One member list header... reading number of channels");

            // fine, per-channel response (allowed in XP, required in Vista)
            KSPROPERTY_MEMBERSHEADER MembersHead =
                *(KSPROPERTY_MEMBERSHEADER*)(
                    (KSPROPERTY_DESCRIPTION*)gnt_full.GetData() + 1
                );

            // if this driver supports multichannel, read off the channel count
            if (KSPROPERTY_MEMBER_FLAG_BASICSUPPORT_MULTICHANNEL & MembersHead.Flags) {
                bBasicSupportMultichannel = TRUE;
                ulChannelsInHeader = MembersHead.MembersCount;
            }
            
            // make sure rest of header is sane
            if (
                sizeof(KSPROPERTY_DESCRIPTION) +
                sizeof(KSPROPERTY_MEMBERSHEADER) +
                MembersHead.MembersCount * MembersHead.MembersSize != OutBufferSize
            ) {
                SLOG(
                    eError,
                    "Buffer size (%u) does not match sum of parts: %u != %u + %u + (%u * %u)",
                    OutBufferSize, OutBufferSize,
                    sizeof(KSPROPERTY_DESCRIPTION), sizeof(KSPROPERTY_MEMBERSHEADER),
                    MembersHead.MembersCount, MembersHead.MembersSize
                );
                return FNS_FAIL;
            }

            // TODO: check stepping ranges for sanity... there's a wide variety of types here
            // KSPROPERTY_AUDIO_DELAY returns a KSTIME
            // KSPROPERTY_AUDIO_EQ_LEVEL returns a LONG array
            // KSPROPERTY_AUDIO_EQ_BANDS returns a ULONG array
            // KSPROPERTY_AUDIO_BASS returns a LONG
            // KSPROPERTY_AUDIO_MID returns a LONG
            // KSPROPERTY_AUDIO_PEAKMETER returns a LONG
            // KSPROPERTY_AUDIO_TREBLE returns a LONG
            // KSPROPERTY_AUDIO_VOLUMELEVEL returns a LONG
            // KSPROPERTY_AUDIO_NUM_EQ_BANDS returns a ULONG
            // KSPROPERTY_AUDIO_AGC returns a BOOL
            // KSPROPERTY_AUDIO_BASS_BOOST returns a BOOL
            // KSPROPERTY_AUDIO_LOUDNESS returns a BOOL
            // KSPROPERTY_AUDIO_MUTE returns a BOOL

        } else { 
            SLOG(eError,
                "ERROR: Unexpected combination of member header count (%u) and buffer size (%u)",
                PropertyDes.MembersListCount,
                OutBufferSize
            );
            return FNS_FAIL;
        } // else (number of headers)
    } // else (no further BASIC_SUPPORT info)

    ULONG ulChannels = 0;
    UINT ulErroredAt = -1;
    const UINT uiMaxChannel = 200; // some ridiculously large number

    // find number of channels by iterating "get" requests on different channels until we get a failure
    ULONG ulEqBandsCheck;
    BOOL bEqBandsSet = FALSE;

    for (UINT i = 0; i < uiMaxChannel; i++) {
        ULONG ulEqBands;
        SLOG(eInfo1, "Trying channel %u (0-based)...", i);
        switch (ulPropId) {
            case KSPROPERTY_AUDIO_DELAY:
                OutBufferSize = sizeof(KSTIME);
                break;

            case KSPROPERTY_AUDIO_EQ_LEVEL:
            case KSPROPERTY_AUDIO_EQ_BANDS:
                dwResult = GetEqBandsCount(hFile, i, ulNodeId, &ulEqBands);
                if (FNS_PASS != dwResult) {
                    ulErroredAt = i;
                    SLOG(eInfo1, "GetEqBandsCount for channel %u (0-based) failed... possibly expected", i);
                    dwResult = FNS_PASS; // expected
                    break; // out of switch
                }
                if (!bEqBandsSet) {
                    ulEqBandsCheck = ulEqBands;
                    bEqBandsSet = TRUE;
                }
                // this is just a warning
                if (ulEqBands != ulEqBandsCheck) {
                    SLOG(eWarn1,
                        "Inconsistent number of equalization bands -- %u and %u -- between channels %u and %u (0-based)",
                        ulEqBandsCheck, ulEqBands,
                        i, i - 1
                    );
                }
                // LONG for _EQ_LEVEL, ULONG for _EQ_BANDS
                OutBufferSize = ulEqBands * sizeof(LONG);
                break;

            case KSPROPERTY_AUDIO_BASS:
            case KSPROPERTY_AUDIO_MID:
            case KSPROPERTY_AUDIO_PEAKMETER:
            case KSPROPERTY_AUDIO_TREBLE:
            case KSPROPERTY_AUDIO_VOLUMELEVEL:
                OutBufferSize = sizeof(LONG);
                break;

            case KSPROPERTY_AUDIO_NUM_EQ_BANDS:
                OutBufferSize = sizeof(ULONG);
                break;

            case KSPROPERTY_AUDIO_AGC:
            case KSPROPERTY_AUDIO_BASS_BOOST:
            case KSPROPERTY_AUDIO_LOUDNESS:
            case KSPROPERTY_AUDIO_MUTE:
                OutBufferSize = sizeof(BOOL);
                break;

            default:
                SLOG(eError, "Unexpected per-channel property ID: %u", ulNodeId);
                return FNS_FAIL;
        }

        if (-1 != ulErroredAt) {
            break; // out of for
        }

        // do a "get" request for channel #i
        NodeAudioChannelProperty.Channel = i;
        NodeAudioChannelProperty.NodeProperty.Property.Flags =
            KSPROPERTY_TYPE_TOPOLOGY | KSPROPERTY_TYPE_GET;
            // KSPROPERTY_TYPE_TOPOLOGY goes with KSNODEPROPERTY

        CGenericIoControl gnt_get_i(  hFile, &NodeAudioChannelProperty, 
                                 sizeof(KSNODEPROPERTY_AUDIO_CHANNEL), sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                                 OutBufferSize, OutBufferSize);
        cbExpectedWritten = OutBufferSize;    
        DWORD dwTempResult = gnt_get_i.Test(FALSE, cbExpectedWritten); // expect it to succeed
        if (gnt_get_i.GetFault() != FAULT_NO_FAULT) { // out of range
            ulErroredAt = i;
            SLOG(eInfo1, "Get request for channel %u (0-based) failed... possibly expected", i);
            break;
        }
    }

    if (-1 == ulErroredAt) {
        // "get" requests never failed! there can't be /that/ many channels...
        SLOG(eError,
            "FAIL: Channel-specific property request are not failing as they should. GET requests succeeded for channels 0 through %u (0-based) which is way too high.",
            uiMaxChannel - 1
        );
        return FNS_FAIL;
    }

    // + 1 to convert from 0-based to 1-based; - 1 to get back to last success
    ulChannels = (ulErroredAt + 1) - 1;

    // number of channels == 0 is ridiculous
    if (0 == ulChannels) {
        SLOG(eError, "FAIL: First GET request failed, therefore could not calculate number of channels");
        return FNS_FAIL;
    }

    // if there was an advertised number of channels,
    // verify that it jives with the iteratively calculated number of channels
    if (bBasicSupportMultichannel && (ulChannels != ulChannelsInHeader)) {
        SLOG(eError,
            "FAIL: Number of channels advertised in basic support request (%u) differs from number of channels passing get requests (%u)",
            ulChannelsInHeader,
            ulChannels
        );
        return FNS_FAIL;
    }

    if (NULL != pulChannels) {
        *pulChannels = ulChannels;
    }

    if (NULL != pbBasicSupport) {
        *pbBasicSupport = bBasicSupport;
    }

    if (NULL != pbBasicSupportMultichannel) {
        *pbBasicSupportMultichannel = bBasicSupportMultichannel;
    }
        
    return dwResult;
}

// for testing of KSNODEPROPERTY_AUDIO_CHANNEL - with invalid node id + valid channel number and with valid node id + invalid channel number 
// default - it will test for invalid node id. If NodeIdTest= False means it will test for invalid channel number.
DWORD Generic_Property_KSAUDIOCHANNELNODEPROPERTY_Channel(DWORD dwChannel, IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase, IN LPVOID pSetData, ULONG DataSize)
{
    ULONG OutBufferSize = DataSize;    //Output bufferzise
    DWORD dwResult;     //used for test result
    BOOL bExpectedTestResult = TRUE;    // as normally test case must fail as we are passing invalid parameters 
    ULONG cbExpectedWritten = 0;    //as normally test case must fail as we are passing invalid parameters 

    KSNODEPROPERTY_AUDIO_CHANNEL pNodeAudioChannelProperty;
    pNodeAudioChannelProperty.NodeProperty = pParams->NodeProp;
    pNodeAudioChannelProperty.Channel = dwChannel;
    pNodeAudioChannelProperty.Reserved = 0;
    switch (TestCase)   {
        case NodeID_ULONGMAX:
            pNodeAudioChannelProperty.NodeProperty.NodeId = ULONG_MAX;  //setting node id to invalid node id and passing all other parameters correct        
            break;
        case NodeID_0x80000001:
            pNodeAudioChannelProperty.NodeProperty.NodeId = 0x80000001;  //setting node id to invalid node id and passing all other parameters correct        
            break;    
        case ChannelID_ULONGMAX_Sub1:
            pNodeAudioChannelProperty.Channel = 0xFFFFFFFE;  //setting channel as invalid.   
            break;    
        case ChannelID_0x80000001:
            pNodeAudioChannelProperty.Channel = 0x80000001;  //setting channel as invalid.   
            break;    
        case NULL_Buffer:
            OutBufferSize = 0;  //sending null buffer.   
            break;    
        case Small_Buffer:
            OutBufferSize = OutBufferSize - 1;  //sending small buffer then required one
            break;
        case Large_Buffer:
            OutBufferSize = OutBufferSize + 1;  // lying about buffer size
            // ks.sys might step in, in which case DeviceIoControl should fail
            // if ks.sys doesn't step in, then the overwrite check is necessary
            bExpectedTestResult = -1;
            cbExpectedWritten = -1;
            break;
        case Valid_Test:
            cbExpectedWritten = OutBufferSize;
            bExpectedTestResult = FALSE; // should succeed
            break;

        default:
            _ASSERT(FALSE);
            return FNS_FAIL;
    }
    // since for set request we are not expecting any data to be writtne back. 
    //Ideally we should expect zero bytes as written, however there is a bug in some of drivers that they are returning number during set request. However this was take care by KS so that IO manager will not copy data back to user mode for Property Set request
    if (pParams->NodeProp.Property.Flags & KSPROPERTY_TYPE_SET)
        cbExpectedWritten = (ULONG) -1;     // this is temporary fix - we have many driver which are reporting that they have set 5 bytes eventhough it should return zero in case of set request.
        
    CGenericIoControl gnt(  pParams->hFile,  &pNodeAudioChannelProperty, 
                                        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL), sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),
                                        OutBufferSize, OutBufferSize);
    memcpy(gnt.GetData(), pSetData, OutBufferSize);
    dwResult = gnt.Test(bExpectedTestResult, cbExpectedWritten);
    if (gnt.GetFault() != FAULT_NO_FAULT) 
	return FNS_FAIL;
    else 
       return FNS_PASS; 
}

