//--------------------------------------------------------------------------
//
//  File: SetupApiHelper.h
//
// Copyright (C) Microsoft. All rights reserved.
//
//  History:
//      02/14/2012    CaWerner       Initial implementation.
//
//--------------------------------------------------------------------------
#pragma once

struct SetupApiContext
{
    SetupApiContext() : DeviceInformationSet(INVALID_HANDLE_VALUE)
    {
        DeviceInfoData.cbSize = 0;
        DeviceInfoData.ClassGuid = GUID_NULL;
        DeviceInfoData.DevInst = 0;
        DeviceInfoData.Reserved = 0;
    }

    ~SetupApiContext()
    {
        if( DeviceInformationSet != INVALID_HANDLE_VALUE )
        {
            DevObjDestroyDeviceInfoList( DeviceInformationSet );
            DeviceInformationSet = INVALID_HANDLE_VALUE;
        }
    }

    HDEVINFO DeviceInformationSet;
    DO_DEVINFO_DATA DeviceInfoData;
};

namespace SetupApiHelper
{
    HRESULT EnumerateAudioDeviceInstanceIds( IBasicLog* pBasicLog, CComHeapPtr<WCHAR>** pArrayDeviceInstanceIds,
        UINT* numDeviceInstanceIds );
    HRESULT GetSetupApiContext( IBasicLog* pBasicLog, LPCWSTR strDeviceInstanceId, SetupApiContext* pSetupApiContext );
    HRESULT GetDeviceProperty( IBasicLog* pBasicLog, SetupApiContext* pSetupApiContext, const DEVPROPKEY* pPropertyKey,
        PBYTE* pValueBuffer, UINT* pcbBuffer );
    HRESULT EnumerateDeviceInterfacePaths( IBasicLog* pBasicLog, LPCWSTR strDeviceInstanceId,
        CComHeapPtr<WCHAR>** pArrayDeviceInterfacePaths, UINT* numDeviceInterfacePaths );
    HRESULT DisableDevnode( IBasicLog* pBasicLog, SetupApiContext* pSetupApiContext );
    HRESULT EnableDevnode( IBasicLog* pBasicLog, SetupApiContext* pSetupApiContext );
    HRESULT GetConservationIdleTime( IBasicLog* pBasicLog, SetupApiContext* pSetupApiContext, ULONG* secsConservationIdleTime );
    HRESULT GetDevnodeStatus( IBasicLog* pBasicLog, SetupApiContext* pSetupApiContext, ULONG* pStatus, ULONG* pProblem );
}
