// Copyright(C) Microsoft.All rights reserved.

#include "common.h"

// forward declare test helper routines
BOOL ValidDataOnGet( IN PGAUDITPARAMS pParams, IN BYTE* pBuffer );
BOOL GetNodePropertyMulti( IN HANDLE hFilter, IN  ULONG nNodeID, IN  REFGUID guidPropertySet, IN  ULONG nProperty, OUT PKSMULTIPLE_ITEM* ppKsMultipleItem );
DWORD TestHelperForSupportedDeviceFormats( IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase );

//
// Test Cases
//

DWORD WINAPI AudioEngine_Property_AUDIOENGINE_Generic_Valid( IN PGAUDITPARAMS pParams )
{
    DWORD dwErrorStatus = ERROR_SUCCESS;
    errno_t errNo = 0;
    KSNODEPROPERTY kspNode = {0}; // KSNODEPROPERTY == KSP_NODE
    DWORD sizeOfInOut = 0;
    BYTE* bInOutBuffer = NULL;

    if( NULL == pParams )
    {
        SLOG( eError, "pParams Passed to AudioEngine_Property_AUDIOENGINE_Generic_Valid is NULL" );

        return FNS_FAIL;
    }

    // setup and send the audioengine ksproperty GET call
    kspNode = pParams->NodeProp;
    kspNode.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;

    switch( pParams->Prop.Id )
    {
        case KSPROPERTY_AUDIOENGINE_DEVICEFORMAT:
        case KSPROPERTY_AUDIOENGINE_MIXFORMAT:
            if( FALSE != SyncDeviceIoControl( pParams->hFile,
                                              IOCTL_KS_PROPERTY,
                                              &kspNode,
                                              sizeof(kspNode),
                                              NULL,
                                              0,
                                              &sizeOfInOut ) )
            {
                SLOG( eError, "ERROR: Expected DeviceIoControl to return FALSE when querying outbuffer size of Audio Engine KS Property." );

                return FNS_FAIL;
            }
            break;

        default:
            sizeOfInOut = pParams->cbOutSize;
    }

    bInOutBuffer = new BYTE[sizeOfInOut];

    if( NULL == bInOutBuffer )
    {
        SLOG( eError, "Failed to Allocate Memory for In/Out Buffer." );

        return FNS_FAIL;
    }

    CGenericIoControl aeDeviceIo( pParams->hFile,
                                  &kspNode,
                                  sizeof(KSNODEPROPERTY),
                                  sizeof(KSNODEPROPERTY),
                                  sizeOfInOut,
                                  sizeOfInOut );

    dwErrorStatus = aeDeviceIo.Test( FALSE, sizeOfInOut, TRUE );
    if( ERROR_SUCCESS != dwErrorStatus )
    {
        SLOG( eError, "Basic GET of Audio Engine Property failed with error code %u.", dwErrorStatus );
        delete[] bInOutBuffer;

        return FNS_FAIL;
    }

    // check results of GET

    if( NULL != aeDeviceIo.GetData() )
    {
        errNo = memcpy_s( bInOutBuffer, sizeOfInOut, aeDeviceIo.GetData(), sizeOfInOut );
        if( 0 != errNo )
        {
            SLOG( eError, "Failed to copy %u bytes from DeviceIoControl Get Buffer with errno: %u", sizeOfInOut, errNo );
            delete[] bInOutBuffer;

            return FNS_FAIL;
        }
    }
    else
    {
        SLOG( eError, "Output Buffer for Audio Engine Property GET is NULL." );
        delete[] bInOutBuffer;

        return FNS_FAIL;
    }

    if( FALSE == ValidDataOnGet( pParams, bInOutBuffer ) )
    {
        // ValidDataOnGet does it's own logging
        delete[] bInOutBuffer;

        return FNS_FAIL;
    }

    if( 0 < ( KSPROPERTY_TYPE_GET & pParams->NodeProp.Property.Flags ) )
    {
        // if this was just GET test, we're done
        delete[] bInOutBuffer;

        return FNS_PASS;
    }
    else if( 0 == ( KSPROPERTY_TYPE_SET & pParams->NodeProp.Property.Flags ) )
    {
        // this is a test setup issue, flag should be GET or SET
        delete[] bInOutBuffer;

        return FNS_FAIL;
    }
    else if( 0 < ( KSPROPERTY_TYPE_SET & pParams->NodeProp.Property.Flags ) &&
                pParams->NodeProp.Property.Id != KSPROPERTY_AUDIOENGINE_LFXENABLE &&
                pParams->NodeProp.Property.Id != KSPROPERTY_AUDIOENGINE_GFXENABLE )
    {
        // the set algorithm below will only work with these properties
        SLOG( eError, "SET for this Audio Engine Property (ID: %u) is NOT allowed.", pParams->NodeProp.Property.Id );
        delete[] bInOutBuffer;

        return FNS_FAIL;
    }

    // Now, we will do some basic SET testing.  Each loop iteration does the following
    //  1. Set KSPROPERTY_AUDIOENGINE_(G|L)FXENABLE
    //  2. Verify DeviceIoControl for SET Succeeded
    //  3. Get KSPROPERTY_AUDIOENGINE_(G|L)FXENABLE
    //  4. Verify DeviceIoControl for GET Succeeded
    //  5. Validate the (G|L)FXENABLE state is what we set in Step 1.
    //
    // There are two loops.  The first loop does the alogirthm for setting
    // the LFXENABLE state to !isLfxEnabled.  The second loop restores the LFXENABLE
    // state to isLfxEnabled.

    for( int loop = 1; loop < 3; loop++ )
    {
        BOOL loopEffectEnabled = (BOOL)-1;
        BOOL isEffectEnabledNow = (BOOL)-1;

        if( 1 == loop )
        {
            // setting to !isLfxEnabled
            loopEffectEnabled = ( FALSE == *((BOOL *)bInOutBuffer) ? TRUE : FALSE );
        }
        else if( 2 == loop )
        {
            // restoring LFXENABLED back to isLfxEnabled
            loopEffectEnabled = *((BOOL *)bInOutBuffer);
        }

        // SET the Lfx Enable state

        *((BOOL *)aeDeviceIo.GetData()) = loopEffectEnabled;

        kspNode.Property.Flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;

        dwErrorStatus = aeDeviceIo.Test( FALSE, sizeof(BOOL), TRUE );
        if( ERROR_SUCCESS != dwErrorStatus )
        {
            SLOG( eError, "Basic SET of KSPROPERTY_AUDIOENGINE_LFXENABLE failed with error code %u.", dwErrorStatus );
            delete[] bInOutBuffer;

            return FNS_FAIL;
        }

        // GET the Lfx Enable state

        *((BOOL *)aeDeviceIo.GetData()) = (BOOL)-1;

        kspNode.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;

        dwErrorStatus = aeDeviceIo.Test( FALSE, sizeof(BOOL), TRUE );
        if( ERROR_SUCCESS != dwErrorStatus )
        {
            SLOG( eError, "Basic SET of KSPROPERTY_AUDIOENGINE_LFXENABLE failed with error code %u.", dwErrorStatus );
            delete[] bInOutBuffer;

            return FNS_FAIL;
        }

        // Validate SET worked

        isEffectEnabledNow = *((BOOL *)aeDeviceIo.GetData());
        if( TRUE != isEffectEnabledNow && FALSE != isEffectEnabledNow )
        {
            SLOG( eError, "GET for KSPROPERTY_AUDIOENGINE_LFXENABLE returned %u.  Expected TRUE (%u) or FALSE (%u).",
                            isEffectEnabledNow, TRUE, FALSE );
            delete[] bInOutBuffer;

            return FNS_FAIL;
        }

        if( isEffectEnabledNow != loopEffectEnabled )
        {
            SLOG( eError, "SET KSPROPERTY_AUDIOENGINE_LFXENABLE to %s, but subsequent GET returned %s.",
                    (loopEffectEnabled == FALSE ? "FALSE" : "TRUE"),
                    (isEffectEnabledNow == FALSE ? "FALSE" : "TRUE") );
            delete[] bInOutBuffer;

            return FNS_FAIL;
        }
    }

    // done with this buffer
    delete[] bInOutBuffer;

    return FNS_PASS;
}

DWORD WINAPI AudioEngine_Property_DEVICEFORMAT_Valid( IN PGAUDITPARAMS pParams )
{
    BOOL bSuccess = FALSE;
    DWORD retCode = FNS_PASS;
    DWORD dwError = ERROR_SUCCESS;
    PKSMULTIPLE_ITEM pKsMultipleItem = NULL;
    KSNODEPROPERTY kspNode = {0};

    if( NULL == pParams )
    {
        SLOG( eError, "pParams Passed to AudioEngine_Property_DEVICEFORMAT_Valid is NULL" );

        return FNS_FAIL;
    }

    if( 0 < ( KSPROPERTY_TYPE_GET & pParams->NodeProp.Property.Flags ) )
    {
        return AudioEngine_Property_AUDIOENGINE_Generic_Valid( pParams );
    }

    // Now, we will do some basic SET testing of the Audio Engine Device Format
    //  1. Get KSPROPERTY_AUDIOENGINE_DEVICEFORMAT
    //  2. Get all device formats from GET KSPROPERTY_AUDIOENGINE_SUPPORTEDDEVICEFORMATS
    //  3. for each format returned from step 2:
    //      a. Set KSPROPERTY_AUDIOENGINE_DEVICEFORMAT
    //      b. Get KSPROPERTY_AUDIOENGINE_DEVICEFORMAT
    //      c. Validate that DEVICEFORMAT is what we set in Step a.
    //  4. Set KSPROPERTY_AUDIOENGINE_DEVICEFORMAT back to one from Step 1.

    kspNode = pParams->NodeProp;
    kspNode.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;

    SLOG( eInfo2, "Getting the Current Device Format" );

    DWORD origDeviceFormatByteSize = 0;
    if( FALSE != SyncDeviceIoControl( pParams->hFile,
                                      IOCTL_KS_PROPERTY,
                                      &kspNode,
                                      sizeof(kspNode),
                                      NULL,
                                      0,
                                      &origDeviceFormatByteSize ) )
    {
        SLOG( eError, "ERROR: Expected DeviceIoControl to return FALSE when querying outbuffer size of KSPROPERTY_AUDIOENGINE_DEVICEFORMATS" );

        return FNS_FAIL;
    }

    if( origDeviceFormatByteSize < sizeof(KSDATAFORMAT) )
    {
        SLOG( eError, "ERROR: Expected bytes returned (%u) to be larger or equal to sizeof(KSDATAFORMAT) (%Iu) for KSPROPERTY_AUDIOENGINE_DEVICEFORMATS",
            origDeviceFormatByteSize, sizeof(KSDATAFORMAT) );

        return FNS_FAIL;
    }

    CGenericIoControl origDeviceFormatDeviceIo( pParams->hFile,
                                                &kspNode,
                                                sizeof(KSNODEPROPERTY),
                                                sizeof(KSNODEPROPERTY),
                                                origDeviceFormatByteSize,
                                                origDeviceFormatByteSize );

    dwError = origDeviceFormatDeviceIo.Test( FALSE, origDeviceFormatByteSize, TRUE );
    if( ERROR_SUCCESS != dwError )
    {
        SLOG( eError, "Basic GET of Audio Engine Property DEVICEFORMAT failed with error code %u.", dwError );

        return FNS_FAIL;
    }

    if( NULL == origDeviceFormatDeviceIo.GetData() )
    {
        SLOG( eError, "Data Buffer for Device IO Control class is NULL" );

        return FNS_FAIL;
    }

    if( FALSE == ValidDataOnGet( pParams, (PBYTE)origDeviceFormatDeviceIo.GetData() ) )
    {
        // ValidDataOnGet does it's own logging
        return FNS_FAIL;
    }

    // get all the supported formats for this pin

    bSuccess = GetNodePropertyMulti( pParams->hFile,
                                     pParams->NodeProp.NodeId,
                                     KSPROPSETID_AudioEngine,
                                     KSPROPERTY_AUDIOENGINE_SUPPORTEDDEVICEFORMATS,
                                     &pKsMultipleItem );
    if( FALSE == bSuccess )
    {
        SLOG( eError, "Failed to GET KSPROPERTY_AUDIOENGINE_SUPPORTEDDEVICEFORMATS." );

        return FNS_FAIL;
    }

    PBYTE pFormatOffset = (PBYTE)(pKsMultipleItem + 1);
    for( UINT i = 0; i < pKsMultipleItem->Count; i++ )
    {
        PKSDATAFORMAT_WAVEFORMATEX pKsDataFormatWaveFormatEx = (PKSDATAFORMAT_WAVEFORMATEX)pFormatOffset;
        kspNode.Property.Flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;
        CGenericIoControl aeSetFormatDeviceIo( pParams->hFile,
                                               &kspNode,
                                               sizeof(KSNODEPROPERTY),
                                               sizeof(KSNODEPROPERTY),
                                               pKsDataFormatWaveFormatEx->DataFormat.FormatSize,
                                               pKsDataFormatWaveFormatEx->DataFormat.FormatSize );

        CopyMemory( aeSetFormatDeviceIo.GetData(), pKsDataFormatWaveFormatEx, pKsDataFormatWaveFormatEx->DataFormat.FormatSize );

        SLOG( eInfo2, "[%u:%u] Setting the Device Format", i+1, pKsMultipleItem->Count );

        dwError = aeSetFormatDeviceIo.Test( FALSE, pKsDataFormatWaveFormatEx->DataFormat.FormatSize, TRUE );
        if( ERROR_SUCCESS != dwError )
        {
            SLOG( eError, "Basic SET of Audio Engine Property DEVICEFORMAT failed with error code %u.", dwError );

            retCode = FNS_FAIL;
            break;
        }

        pFormatOffset += pKsDataFormatWaveFormatEx->DataFormat.FormatSize;
    }

    // done with the data ranges
    LocalFree( pKsMultipleItem );
    pKsMultipleItem = NULL;

    // restore original device format
    kspNode.Property.Flags = KSPROPERTY_TYPE_SET | KSPROPERTY_TYPE_TOPOLOGY;

    SLOG( eInfo2, "Restoring Original Device Format" );

    dwError = origDeviceFormatDeviceIo.Test( FALSE, origDeviceFormatByteSize, TRUE );
    if( ERROR_SUCCESS != dwError )
    {
        SLOG( eError, "Basic SET of Audio Engine Property DEVICEFORMAT failed with error code %u.", dwError );

        return FNS_FAIL;
    }

    return retCode;
}

DWORD WINAPI AudioEngine_Property_SUPPORTEDDEVICEFORMATS_1( IN PGAUDITPARAMS pParams )
{
    return TestHelperForSupportedDeviceFormats( pParams, NodeID_0x80000001 );
}

DWORD WINAPI AudioEngine_Property_SUPPORTEDDEVICEFORMATS_2( IN PGAUDITPARAMS pParams )
{
    return TestHelperForSupportedDeviceFormats( pParams, NodeID_ULONGMAX );
}

DWORD WINAPI AudioEngine_Property_SUPPORTEDDEVICEFORMATS_3( IN PGAUDITPARAMS pParams )
{
    return TestHelperForSupportedDeviceFormats( pParams, NULL_Buffer );
}

DWORD WINAPI  AudioEngine_Property_SUPPORTEDDEVICEFORMATS_4( IN PGAUDITPARAMS pParams )
{
    return TestHelperForSupportedDeviceFormats( pParams, Small_Buffer );
}

DWORD WINAPI AudioEngine_Property_SUPPORTEDDEVICEFORMATS_5( IN PGAUDITPARAMS pParams )
{
    return TestHelperForSupportedDeviceFormats( pParams, Large_Buffer );
}

DWORD WINAPI AudioEngine_Property_SUPPORTEDDEVICEFORMATS_6( IN PGAUDITPARAMS pParams )
{
    return TestHelperForSupportedDeviceFormats( pParams, Valid_Test );
}

//
// Helper Functions
//

BOOL ValidDataOnGet( IN PGAUDITPARAMS pParams, IN BYTE* pBuffer )
{
    BOOL isValid = FALSE;

    if( NULL == pParams || NULL == pBuffer )
    {
        SLOG( eError, "One of the Arguments to ValidDataOnGet(%p,%p) is Invalid.", pParams, pBuffer );

        // well, that's not good
        return FALSE;
    }

    switch( pParams->NodeProp.Property.Id )
    {
        case KSPROPERTY_AUDIOENGINE_LFXENABLE:
        case KSPROPERTY_AUDIOENGINE_GFXENABLE:
        {
            BOOL isEffectEnabled = *((BOOL *)pBuffer);

            if( TRUE != isEffectEnabled && FALSE != isEffectEnabled )
            {
                SLOG( eError, "Expecting value to be TRUE(%u) or FALSE(%u).  Actual %u.",
                        TRUE, FALSE, isEffectEnabled );

                isValid = FALSE;
            }
            else
            {
                isValid = TRUE;
            }
            break;

        }
        case KSPROPERTY_AUDIOENGINE_MIXFORMAT:
        case KSPROPERTY_AUDIOENGINE_DEVICEFORMAT:
        {
            KSDATAFORMAT ksDataFormat = *((KSDATAFORMAT *)pBuffer);

            if( ksDataFormat.FormatSize < sizeof(KSDATAFORMAT) )
            {
                SLOG( eError, "KSDATAFORMAT.FormatSize must be at least the size of KSDATAFORMAT (%Iu bytes).  Actual %u.",
                        sizeof(KSDATAFORMAT), ksDataFormat.FormatSize );

                isValid = FALSE;
            }
            else if( ksDataFormat.Flags != 0 )
            {
                SLOG( eError, "KSDATAFORMAT.Flags must be 0.  Actual %u.", ksDataFormat.Flags );

                isValid = FALSE;
            }
            else if( ksDataFormat.Reserved != 0 )
            {
                SLOG( eError, "KSDATAFORMAT.Reserved must be 0.  Actual %u.", ksDataFormat.Reserved );

                isValid = FALSE;
            }
            else
            {
                isValid = TRUE;
            }
            break;

        }
        case KSPROPERTY_AUDIOENGINE_DESCRIPTOR:
        {
            KSAUDIOENGINE_DESCRIPTOR aeDescriptor = *((KSAUDIOENGINE_DESCRIPTOR *)pBuffer);

            if( aeDescriptor.nOffloadPinId == aeDescriptor.nHostPinId )
            {
                SLOG( eError, "The Offload Pin Id (%u) is the same as the Host Pin Id (%u).",
                        aeDescriptor.nOffloadPinId, aeDescriptor.nHostPinId );

                isValid = FALSE;
            }
            else if( aeDescriptor.nOffloadPinId == aeDescriptor.nLoopbackPinId )
            {
                SLOG( eError, "The Offload Pin Id (%u) is the same as the Loopback Pin Id (%u).",
                        aeDescriptor.nOffloadPinId, aeDescriptor.nLoopbackPinId );

                isValid = FALSE;
            }
            else if( aeDescriptor.nHostPinId == aeDescriptor.nLoopbackPinId )
            {
                SLOG( eError, "The Host Pin Id (%u) is the same as the Loopback Pin Id (%u).",
                        aeDescriptor.nHostPinId, aeDescriptor.nLoopbackPinId );

                isValid = FALSE;
            }
            else
            {
                isValid = TRUE;
            }
            break;

        }
        default:
             SLOG( eError, "Property Id %u is not valid for the Audio Engine Property Set.",
                    pParams->NodeProp.Property.Id );

            isValid = FALSE;
    }

    return isValid;
}

BOOL GetNodePropertyMulti
(
    IN  HANDLE              hFilter,
    IN  ULONG               nNodeID,
    IN  REFGUID             guidPropertySet,
    IN  ULONG               nProperty,
    OUT PKSMULTIPLE_ITEM*   ppKsMultipleItem
)
{
    BOOL fRes = FALSE;
    ULONG ulReturned = 0;
    ULONG cbMultipleItem = 0;
    KSP_NODE kspNode;

    if( NULL == ppKsMultipleItem )
    {
        SLOG( eError, "ppKsMultipleItem passed to GetNodePropertyMulti() is NULL" );

        return FALSE;
    }

    *ppKsMultipleItem = NULL;

    kspNode.Property.Set = guidPropertySet;
    kspNode.Property.Id = nProperty;
    kspNode.Property.Flags = KSPROPERTY_TYPE_GET | KSPROPERTY_TYPE_TOPOLOGY;
    kspNode.NodeId = nNodeID;
    kspNode.Reserved = 0;

    fRes = SyncIoctl( hFilter,
                      IOCTL_KS_PROPERTY,
                      &kspNode.Property,
                      sizeof(KSP_NODE),
                      NULL,
                      0,
                      &cbMultipleItem );

    if( FALSE != fRes )
    {
        *ppKsMultipleItem = (PKSMULTIPLE_ITEM)LocalAlloc( LPTR, cbMultipleItem );

        fRes = SyncIoctl( hFilter,
                          IOCTL_KS_PROPERTY,
                          &kspNode,
                          sizeof(KSP_PIN),
                          (PVOID)*ppKsMultipleItem,
                          cbMultipleItem,
                          &ulReturned );

        if( FALSE != fRes )
        {
            if( ulReturned != cbMultipleItem )
            {
                SLOG( eInfo2, "Unexpected Amount of Bytes Returned (%u) from SUPPORTEDDEVICEFORMATS DeviceIoControl.  Expected %u",
                        ulReturned, cbMultipleItem );

                fRes = FALSE;
            }
        }
        else
        {
            SLOG( eInfo2, "DeviceIoControl for SUPPORTEDDEVICEFORMATS Failed" );
        }
    }
    else
    {
         SLOG( eInfo2, "Failed the DeviceIoControl for SUPPORTEDDEVICEFORMATS with Empty Output Buffer." );
    }

    return fRes;
}

DWORD TestHelperForSupportedDeviceFormats( IN PGAUDITPARAMS pParams, IN eTestCaseType TestCase )
{
    if( NULL == pParams )
    {
        SLOG( eError, "pParams Passed to TestHelperForSupportedDeviceFormat is NULL" );

        return FNS_FAIL;
    }

    if( pParams->Prop.Flags & KSPROPERTY_TYPE_SET )
    {
        return FNS_PASS;
    }

    BOOL fRes = FALSE;
    DWORD dwError = ERROR_SUCCESS;
    BOOL bShouldTestFail = TRUE;    // as normally test case must fail as we are passing invalid parameters 
    ULONG numberOfBytesWrote = 0;   // as normally test case must fail as we are passing invalid parameters
    ULONG sizeOfBufferInBytes = 0;
    PKSMULTIPLE_ITEM pPinDataRanges = NULL;

    // first get the size of the header of KSMULTIPLE_ITEM and property data that follows (KSMULTILPE_ITEM.SIZE)
    fRes = GetNodePropertyMulti( pParams->hFile,
                                 pParams->NodeProp.NodeId,
                                 KSPROPSETID_AudioEngine,
                                 pParams->NodeProp.Property.Id,
                                 &pPinDataRanges );
    if( FALSE == fRes )
    {
        SLOG( eError, "Failed to retrieve KSPROPERTY_AUDIOENGINE_SUPPORTEDDEVICEFORMATS" );

        return FNS_FAIL;
    }

    sizeOfBufferInBytes = pPinDataRanges->Size;

    // free buffer
    LocalFree( pPinDataRanges );
    pPinDataRanges = NULL;

    // now do test as per test case
    switch( TestCase)
    {
        case NodeID_0x80000001:
            pParams->NodeProp.NodeId = 0x80000001;
            break;

        case NodeID_ULONGMAX:
            pParams->NodeProp.NodeId = ULONG_MAX;
            break;

        case NULL_Buffer:
            sizeOfBufferInBytes = 0;
            break;

        case Small_Buffer:
            sizeOfBufferInBytes = sizeOfBufferInBytes - 1;  //setting channel as invalid.
            break;

        case Large_Buffer:
            numberOfBytesWrote = sizeOfBufferInBytes;
            sizeOfBufferInBytes = sizeOfBufferInBytes + 1;  // sending larger buffer then required one
            bShouldTestFail = FALSE;        // in case of Large buffer - Get/set request will Pass. In case of set request check for overwrite.
            break;

        case Valid_Test:
            numberOfBytesWrote = sizeOfBufferInBytes;
            sizeOfBufferInBytes = sizeOfBufferInBytes;
            bShouldTestFail = FALSE;
            break;

        default:
            _ASSERT(FALSE);
    }

    CGenericIoControl aeDeviceIo( pParams->hFile,
                                  &pParams->Prop,
                                  pParams->cbSize,
                                  pParams->cbSize,
                                  sizeOfBufferInBytes,
                                  sizeOfBufferInBytes );

    dwError = aeDeviceIo.Test( bShouldTestFail, numberOfBytesWrote, TRUE );
    if( ERROR_SUCCESS != dwError && FALSE == bShouldTestFail )
    {
        SLOG( eError, "Failed to retrieve KSPROPERTY_AUDIOENGINE_SUPPORTEDDEVICEFORMATS with error code %u.", dwError );

        return FNS_FAIL;
    }

    if( FAULT_NO_FAULT != aeDeviceIo.GetFault() )
    {
        // aeDeviceIo.Test does logging
        return FNS_FAIL;
    }
    else
    {
        return FNS_PASS;
    }
}