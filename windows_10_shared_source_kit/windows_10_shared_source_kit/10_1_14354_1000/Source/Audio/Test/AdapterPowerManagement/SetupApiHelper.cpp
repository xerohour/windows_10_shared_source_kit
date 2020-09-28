//--------------------------------------------------------------------------;
//
//  File: SetupApiHelper.cpp
//
// Copyright (C) Microsoft. All rights reserved.
//
//  History:
//      02/14/2012    CaWerner       Initial implementation.
//
//--------------------------------------------------------------------------;
#include "common.h"

#include <ks.h>
#include <ksmedia.h>
#include <devpkey.h>
#include <cfgmgr32.h>
#include <basiclog.h>
#include <basicprintf.h>
#include <basicloghelper.h>

#include "SetupApiHelper.h"

const LONG PORTCLS_CONVERSATION_IDLE_TIME = 30;
HRESULT SetupApiHelper::EnumerateAudioDeviceInstanceIds( IBasicLog* pBasicLog, CComHeapPtr<WCHAR>** pArrayDeviceInstanceIds,
    UINT* numDeviceInstanceIds )
{
    HRESULT hr = S_OK;
    CComHeapPtr<CComHeapPtr<WCHAR>> spArrayDeviceInstanceIds;
    UINT numIds = 0;
    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
    DO_DEVICE_INTERFACE_DATA devInterfaceData = {};
    DWORD memberIndex = 0;
    DWORD statusEnumDeviceInterfaces = ERROR_SUCCESS;

    *pArrayDeviceInstanceIds = nullptr;
    *numDeviceInstanceIds = 0;

    hDevInfo = DevObjCreateDeviceInfoList(NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL);

    if( INVALID_HANDLE_VALUE == hDevInfo )
    {
        DWORD error = GetLastError();
        LOG( pBasicLog, L"ERROR: EnumerateAudioDeviceInstanceIds: DevObjCreateDeviceInfoList Failed.  (error=%u)",
            error );

        return HRESULT_FROM_WIN32( error );
    }

    // Step 1. Enumerate the Device Information Set for Audio Devnodes
    if (!DevObjGetClassDevs( hDevInfo, 
                                &KSCATEGORY_AUDIO,
                                NULL,
                                DOGCF_PRESENT | DOGCF_DEVICEINTERFACE,
                                NULL,
                                NULL))
    {
        DWORD error = GetLastError();
        LOG( pBasicLog, L"ERROR: EnumerateAudioDeviceInstanceIds: DevObjGetClassDevs Failed to Enumerate a Device Information Set for KS Category Audio Devices.  (error=%u)",
            error );

        return HRESULT_FROM_WIN32( error );
    }

    OnExit destroyDeviceInfoList( [&]()
    {
        DevObjDestroyDeviceInfoList( hDevInfo );
        hDevInfo = INVALID_HANDLE_VALUE;
    } );

    // Step 2. Enumerate the Device Interfaces contained in the Device Information Set
    devInterfaceData.cbSize = sizeof( DO_DEVICE_INTERFACE_DATA );
    while( FALSE != DevObjEnumDeviceInterfaces( hDevInfo,
                                                 NULL,
                                                 &KSCATEGORY_AUDIO,
                                                 memberIndex,
                                                 &devInterfaceData ) )
    {
        DO_DEVINFO_DATA devInfoData = { 0, GUID_NULL, 0, 0 };
        CComHeapPtr<DO_DEVICE_INTERFACE_DETAIL_DATA> spDeviceInterfaceDetailData;
        DWORD cbDeviceInterfaceDetailData = 0;
        CComHeapPtr<WCHAR> spStrDeviceInstanceId;
        DWORD cchDeviceInstanceIdStr = 0;
        bool isNewId = true;

        // Step 3. Get detail about each Device Interface
        if( FALSE != DevObjGetDeviceInterfaceDetail( hDevInfo,
                                                      &devInterfaceData,
                                                      NULL,
                                                      0,
                                                      &cbDeviceInterfaceDetailData,
                                                      NULL ) )
        {
            DWORD error = GetLastError();
            LOG( pBasicLog, L"ERROR: EnumerateAudioDeviceInstanceIds: Expected DevObjGetDeviceInterfaceDetail to return FALSE when retreiving the size of the Device Interface Detail Data object.  (error=%u)",
                error );

            return E_UNEXPECTED;
        }

        if( !spDeviceInterfaceDetailData.AllocateBytes( cbDeviceInterfaceDetailData ) )
        {
            LOG( pBasicLog, L"ERROR: EnumerateAudioDeviceInstanceIds: Unable to allocate memory for DO_DEVICE_INTERFACE_DETAIL_DATA object." );

            return E_OUTOFMEMORY;
        }

        spDeviceInterfaceDetailData->cbSize = sizeof(DO_DEVICE_INTERFACE_DETAIL_DATA);
        devInfoData.cbSize = sizeof(DO_DEVINFO_DATA);
        if( FALSE == DevObjGetDeviceInterfaceDetail( hDevInfo,
                                                      &devInterfaceData,
                                                      spDeviceInterfaceDetailData,
                                                      cbDeviceInterfaceDetailData,
                                                      NULL,
                                                      &devInfoData ) )
        {
            DWORD error = GetLastError();
            LOG( pBasicLog, L"ERROR: EnumerateAudioDeviceInstanceIds: DevObjGetDeviceInterfaceDetail Failed retreiving interface details.  (error=%u)",
                error );

            return HRESULT_FROM_WIN32( error );
        }

        // Step 4. Get the Device Instance Id
        if( FALSE != DevObjGetDeviceInstanceId( hDevInfo,
                                                 &devInfoData,
                                                 NULL,
                                                 0,
                                                 &cchDeviceInstanceIdStr ) )
        {
            LOG( pBasicLog, L"ERROR: EnumerateAudioDeviceInstanceIds: DevObjGetDeviceInstanceId did NOT Return FALSE when Querying Required Size." );

            return E_UNEXPECTED;
        }

        if( !spStrDeviceInstanceId.Allocate( cchDeviceInstanceIdStr ) )
        {
            LOG( pBasicLog, L"ERROR: EnumerateAudioDeviceInstanceIds: Failed to allocate memory for the Device Instance Id." );

            return E_OUTOFMEMORY;
        }

        if( FALSE == DevObjGetDeviceInstanceId( hDevInfo,
                                                 &devInfoData,
                                                 spStrDeviceInstanceId,
                                                 cchDeviceInstanceIdStr,
                                                 NULL ) )
        {
            DWORD error = GetLastError();
            LOG( pBasicLog, L"ERROR: EnumerateAudioDeviceInstanceIds: DevObjGetDeviceInstanceId Failed.  (error=%u)",
                error );

            return HRESULT_FROM_WIN32( error );
        }

        for( UINT i = 0; i < numIds; i++ )
        {
            PWSTR strCurrId = spArrayDeviceInstanceIds.m_pData[i];

            if( 0 == _wcsicmp( strCurrId, spStrDeviceInstanceId ) )
            {
                isNewId = false;
                break;
            }
        }

        if( isNewId )
        {
            // add the device instance id string to the array
            if( !spArrayDeviceInstanceIds.Reallocate( numIds + 1 ) )
            {
                LOG( pBasicLog, L"ERROR: EnumerateAudioDeviceInstanceIds: Failed to allocate the updated device instance id array." );

                return E_OUTOFMEMORY;
            }

            spArrayDeviceInstanceIds.m_pData[numIds++].m_pData = spStrDeviceInstanceId.Detach();
        }

        memberIndex++;
    }

    statusEnumDeviceInterfaces = GetLastError();
    if( ERROR_NO_MORE_ITEMS != statusEnumDeviceInterfaces )
    {
        LOG( pBasicLog, L"ERROR: EnumerateAudioDeviceInstanceIds: DevObjEnumDeviceInterfaces Failed.  (error=%u)",
                statusEnumDeviceInterfaces );

        return HRESULT_FROM_WIN32( statusEnumDeviceInterfaces );
    }

    *pArrayDeviceInstanceIds = spArrayDeviceInstanceIds.Detach();
    *numDeviceInstanceIds = numIds;

    return hr;
}

HRESULT SetupApiHelper::GetSetupApiContext( IBasicLog* pBasicLog, LPCWSTR strDeviceInstanceId, SetupApiContext* pSetupApiContext )
{
    pSetupApiContext->DeviceInformationSet = INVALID_HANDLE_VALUE;
    pSetupApiContext->DeviceInfoData.cbSize = 0;
    pSetupApiContext->DeviceInfoData.ClassGuid = GUID_NULL;
    pSetupApiContext->DeviceInfoData.DevInst = 0;
    pSetupApiContext->DeviceInfoData.Reserved = 0;

    pSetupApiContext->DeviceInformationSet = DevObjCreateDeviceInfoList( NULL, NULL, NULL, NULL, NULL );
    if( INVALID_HANDLE_VALUE == pSetupApiContext->DeviceInformationSet )
    {
        DWORD error = GetLastError();
        LOG( pBasicLog, L"ERROR: GetSetupApiContext: DevObjCreateDeviceInfoList Failed with Error: %u", error );

        return HRESULT_FROM_WIN32( error );
    }

    // add the device information element to the device information set
    pSetupApiContext->DeviceInfoData.cbSize = sizeof(DO_DEVINFO_DATA);
    if( FALSE == DevObjOpenDeviceInfo( pSetupApiContext->DeviceInformationSet,
                                        strDeviceInstanceId,
                                        0,
                                        NULL,
                                        &pSetupApiContext->DeviceInfoData ) )
    {
        DWORD error = GetLastError();
        LOG( pBasicLog, L"ERROR: GetSetupApiContext: DevObjOpenDeviceInfo Failed with Error: %u", error );

        (void) DevObjDestroyDeviceInfoList( pSetupApiContext->DeviceInformationSet );
        pSetupApiContext->DeviceInformationSet = INVALID_HANDLE_VALUE;

        return HRESULT_FROM_WIN32( error );
    }

    return S_OK;
}

HRESULT SetupApiHelper::GetDeviceProperty( IBasicLog* pBasicLog, SetupApiContext* pSetupApiContext, const DEVPROPKEY* pPropertyKey,
    PBYTE* pValueBuffer, UINT* pcbBuffer )
{
    CComHeapPtr<BYTE> spByteBuffer;
    DEVPROPTYPE devicePropertyType;
    DWORD cbByteBuffer = 0;

    *pValueBuffer = nullptr;
    *pcbBuffer = 0;

    if( FALSE != DevObjGetDeviceProperty( pSetupApiContext->DeviceInformationSet,
                                           &(pSetupApiContext->DeviceInfoData),
                                           pPropertyKey,
                                           &devicePropertyType,
                                           NULL,
                                           0,
                                           &cbByteBuffer,
                                           0 ) )
    {
        LOG( pBasicLog, L"ERROR: GetDeviceProperty: DevObjGetDeviceProperty did not return FALSE when querying output buffer size." );

        return E_UNEXPECTED;
    }

    if( !spByteBuffer.AllocateBytes( cbByteBuffer ) )
    {
        LOG( pBasicLog, L"ERROR: GetDeviceProperty: Failed to allocate the driver description string." );

        return E_OUTOFMEMORY;
    }

    if( FALSE == DevObjGetDeviceProperty( pSetupApiContext->DeviceInformationSet,
                                           &(pSetupApiContext->DeviceInfoData),
                                           pPropertyKey,
                                           &devicePropertyType,
                                           (PBYTE)spByteBuffer.m_pData,
                                           cbByteBuffer,
                                           NULL,
                                           0 ) )
    {
        DWORD error = GetLastError();
        LOG( pBasicLog, L"ERROR: GetDeviceProperty: DevObjGetDeviceProperty Failed with Error: %u", error );

        return HRESULT_FROM_WIN32( error );
    }

    *pValueBuffer = spByteBuffer.Detach();
    *pcbBuffer = cbByteBuffer;

    return S_OK;
}

HRESULT SetupApiHelper::EnumerateDeviceInterfacePaths( IBasicLog* pBasicLog, LPCWSTR strDeviceInstanceId,
    CComHeapPtr<WCHAR>** pArrayDeviceInterfacePaths, UINT* numDeviceInterfacePaths )
{
    CComHeapPtr<CComHeapPtr<WCHAR>> spArrayDeviceInterfacePaths;
    UINT numPaths = 0;
    HDEVINFO hDevInfo = INVALID_HANDLE_VALUE;
    DO_DEVICE_INTERFACE_DATA devInterfaceData = {};
    DWORD memberIndex = 0;
    DWORD statusEnumDeviceInterfaces = ERROR_SUCCESS;

    *pArrayDeviceInterfacePaths = nullptr;
    *numDeviceInterfacePaths = 0;

    hDevInfo = DevObjCreateDeviceInfoList(NULL,
                                        NULL,
                                        NULL,
                                        NULL,
                                        NULL);

    if( INVALID_HANDLE_VALUE == hDevInfo )
    {
        DWORD error = GetLastError();
        LOG( pBasicLog, L"ERROR: EnumerateDeviceInterfacePaths: DevObjCreateDeviceInfoList Failed.  (error=%u)",
            error );

        return HRESULT_FROM_WIN32( error );
    }

    // Step 1. Enumerate the Device Information Set for Audio Devnodes
    if (!DevObjGetClassDevs( hDevInfo, 
                                &KSCATEGORY_AUDIO,
                                NULL,
                                DOGCF_PRESENT | DOGCF_DEVICEINTERFACE,
                                NULL,
                                NULL))
    {
        DWORD error = GetLastError();
        LOG( pBasicLog, L"ERROR: EnumerateDeviceInterfacePaths: DevObjGetClassDevs Failed to Enumerate a Device Information Set for KS Category Audio Devices.  (error=%u)",
            error );

        return HRESULT_FROM_WIN32( error );
    }

    OnExit destroyDeviceInfoList( [&]()
    {
        DevObjDestroyDeviceInfoList( hDevInfo );
        hDevInfo = INVALID_HANDLE_VALUE;
    } );

    // Step 2. Enumerate the Device Interfaces contained in the Device Information Set
    devInterfaceData.cbSize = sizeof( DO_DEVICE_INTERFACE_DATA );
    while( FALSE != DevObjEnumDeviceInterfaces( hDevInfo,
                                                 NULL,
                                                 &KSCATEGORY_AUDIO,
                                                 memberIndex,
                                                 &devInterfaceData ) )
    {
        HRESULT hr = S_OK;
        CComHeapPtr<WCHAR> spDeviceInterfacePath;
        CComHeapPtr<DO_DEVICE_INTERFACE_DETAIL_DATA> spDeviceInterfaceDetailData;
        DWORD cbDeviceInterfaceDetailData = 0;
        size_t cchDeviceInterfacePath = 0;

        // Step 3. Get detail about each Device Interface
        if( FALSE != DevObjGetDeviceInterfaceDetail( hDevInfo,
                                                      &devInterfaceData,
                                                      NULL,
                                                      0,
                                                      &cbDeviceInterfaceDetailData,
                                                      NULL ) )
        {
            DWORD error = GetLastError();
            LOG( pBasicLog, L"ERROR: EnumerateDeviceInterfacePaths: Expected DevObjGetDeviceInterfaceDetail to return FALSE when retreiving the size of the Device Interface Detail Data object.  (error=%u)",
                error );

            return HRESULT_FROM_WIN32( error );
        }

        if( !spDeviceInterfaceDetailData.AllocateBytes( cbDeviceInterfaceDetailData ) )
        {
            LOG( pBasicLog, L"ERROR: EnumerateDeviceInterfacePaths: Unable to allocate memory for DO_DEVICE_INTERFACE_DETAIL_DATA object." );

            return E_OUTOFMEMORY;
        }

        spDeviceInterfaceDetailData->cbSize = sizeof(DO_DEVICE_INTERFACE_DETAIL_DATA);
        if( FALSE == DevObjGetDeviceInterfaceDetail( hDevInfo,
                                                      &devInterfaceData,
                                                      spDeviceInterfaceDetailData,
                                                      cbDeviceInterfaceDetailData,
                                                      NULL,
                                                      NULL ) )
        {
            DWORD error = GetLastError();
            LOG( pBasicLog, L"ERROR: EnumerateDeviceInterfacePaths: DevObjGetDeviceInterfaceDetail Failed retreiving interface details.  (error=%u)",
                error );

            return HRESULT_FROM_WIN32( error );
        }

        // Step 4. Copy the Device Interface Path
        hr = StringCchLength( spDeviceInterfaceDetailData->DevicePath, STRSAFE_MAX_CCH, &cchDeviceInterfacePath );
        if( S_OK != hr )
        {
            LOG( pBasicLog, L"ERROR: EnumerateDeviceInterfacePaths: StringCchLength failed.  (HRESULT=0x%08x)", hr );

            return hr;
        }

        if( !spDeviceInterfacePath.Allocate( cchDeviceInterfacePath + 1 ) )
        {
            LOG( pBasicLog, L"ERROR: EnumerateDeviceInterfacePaths: Failed to allocate the memory for the device interface path." );

            return E_OUTOFMEMORY;
        }

        hr = StringCchCopy( spDeviceInterfacePath.m_pData, cchDeviceInterfacePath + 1, spDeviceInterfaceDetailData->DevicePath );
        if( S_OK != hr )
        {
            LOG( pBasicLog, L"ERROR: EnumerateDeviceInterfacePaths: StringCchCopy failed.  (HRESULT=0x%08x)", hr );

            return hr;
        }

        // Step 5. Add the Device Interface Path string to the array
        if( !spArrayDeviceInterfacePaths.Reallocate( numPaths + 1 ) )
        {
            LOG( pBasicLog, L"ERROR: EnumerateDeviceInterfacePaths: Failed to allocate the updated device iterface path array." );
            
            return E_OUTOFMEMORY;
        }

        spArrayDeviceInterfacePaths.m_pData[numPaths++].m_pData = spDeviceInterfacePath.Detach();

        memberIndex++;
    }

    statusEnumDeviceInterfaces = GetLastError();
    if( ERROR_NO_MORE_ITEMS != statusEnumDeviceInterfaces )
    {
        LOG( pBasicLog, L"ERROR: EnumerateDeviceInterfacePaths: DevObjEnumDeviceInterfaces Failed.  (error=%u)",
                statusEnumDeviceInterfaces );

        return HRESULT_FROM_WIN32( statusEnumDeviceInterfaces );
    }

    *pArrayDeviceInterfacePaths = spArrayDeviceInterfacePaths.Detach();
    *numDeviceInterfacePaths = numPaths;

    return S_OK;
}

HRESULT SetupApiHelper::DisableDevnode( IBasicLog* pBasicLog, SetupApiContext* pSetupApiContext )
{
    
    DO_CHANGESTATE_PARAMS propChangeParams;

    propChangeParams.StateChange = DevObjChangeStateDisable;
    propChangeParams.Scope = DOCS_FLAG_GLOBAL;
    propChangeParams.HwProfile = 0;

    if( FALSE == DevObjChangeState( pSetupApiContext->DeviceInformationSet,
                                               &pSetupApiContext->DeviceInfoData,
                                               &propChangeParams,
                                               0) )
    {
        DWORD error = GetLastError();
        LOG( pBasicLog, L"ERROR: DisableDevnode: DevObjChangeState Failed with Error: %u", error );

        return HRESULT_FROM_WIN32( error );
    }

    return S_OK;
}

HRESULT SetupApiHelper::EnableDevnode( IBasicLog* pBasicLog, SetupApiContext* pSetupApiContext )
{
    DO_CHANGESTATE_PARAMS propChangeParams;

    propChangeParams.StateChange = DevObjChangeStateEnable;
    propChangeParams.Scope = DOCS_FLAG_GLOBAL;
    propChangeParams.HwProfile = 0;

    if( FALSE == DevObjChangeState( pSetupApiContext->DeviceInformationSet,
                                               &pSetupApiContext->DeviceInfoData,
                                               &propChangeParams,
                                               0) )
    {
        DWORD error = GetLastError();
        LOG( pBasicLog, L"ERROR: EnableDevnode: DevObjChangeState Failed with Error: %u", error );

        return HRESULT_FROM_WIN32( error );
    }

    return S_OK;
}

HRESULT SetupApiHelper::GetConservationIdleTime( IBasicLog* pBasicLog, SetupApiContext* pSetupApiContext,
    ULONG* secsConservationIdleTime )
{
    HRESULT hr = E_FAIL;
    LONG error = ERROR_SUCCESS;
    CComHeapPtr<WCHAR> spDeviceDriverKeyPath;
    CComHeapPtr<WCHAR> spDeviceDriverKey;
    UINT cbDeviceDriverKey = 0;
    LPCWSTR strRegistryPath = L"SYSTEM\\CurrentControlSet\\Control\\Class\\";
    size_t cbRegistryPath = 0;
    HKEY hKeyDriverKey;
    HKEY hKeyPowerSettings;
    DWORD cbRead = sizeof(*secsConservationIdleTime);

    if (NULL == secsConservationIdleTime)
    {
        return E_POINTER;
    }

    *secsConservationIdleTime = 0;

    // Open the Device's Registry Property Store
    hr = GetDeviceProperty( pBasicLog,
                            pSetupApiContext,
                            &DEVPKEY_Device_Driver,
                            (PBYTE *)&spDeviceDriverKey.m_pData,
                            &cbDeviceDriverKey );
    if( S_OK != hr )
    {
        LOG( pBasicLog, L"ERROR: GetConservationIdleTime: Failed to Get DEVPKEY_Device_Driver with HRESULT: 0x%08x", hr );

        return hr;
    }

    hr = StringCbLength( strRegistryPath, STRSAFE_MAX_CCH * sizeof(WCHAR), &cbRegistryPath );
    if( S_OK != hr )
    {
        LOG( pBasicLog, L"ERROR: GetConservationIdleTime: Failed to Get Length of Registry Path: 0x%08x", hr );

        return hr;
    }

    if( !spDeviceDriverKeyPath.AllocateBytes( cbRegistryPath + cbDeviceDriverKey ) )
    {
        LOG( pBasicLog, L"ERROR: GetConservationIdleTime: Failed to Allocate Memory for Device Driver Registry Path: 0x%08x", hr );

        return hr;
    }

    hr = StringCbCopy( spDeviceDriverKeyPath.m_pData, cbRegistryPath + cbDeviceDriverKey, strRegistryPath );
    if( S_OK != hr )
    {
        LOG( pBasicLog, L"ERROR: GetConservationIdleTime: Failed to Copy Registry Path: 0x%08x", hr );

        return hr;
    }

    hr = StringCbCopy( spDeviceDriverKeyPath.m_pData + (cbRegistryPath / sizeof(WCHAR)), cbDeviceDriverKey, spDeviceDriverKey.m_pData );
    if( S_OK != hr )
    {
        LOG( pBasicLog, L"ERROR: GetConservationIdleTime: Failed to Append Device Driver Key with HRESULT: 0x%08x", hr );

        return hr;
    }

    // open the Key
    error = RegOpenKeyEx( HKEY_LOCAL_MACHINE,
                          spDeviceDriverKeyPath.m_pData,
                          0,
                          KEY_ALL_ACCESS,
                          &hKeyDriverKey );
    if( ERROR_SUCCESS != error )
    {
        LOG( pBasicLog, L"ERROR: GetConservationIdleTime: Failed to Open Device Driver Registry Store with error: %u", error );

        return HRESULT_FROM_WIN32(error);
    }

    OnExit closeDriverKey( [&]()
    {
        RegCloseKey( hKeyDriverKey );
    } );

    // open the power settings
    error = RegOpenKeyEx( hKeyDriverKey,
                          L"PowerSettings",
                          0,
                          KEY_ALL_ACCESS,
                          &hKeyPowerSettings );
    if (ERROR_FILE_NOT_FOUND == error)
    {
        //this key doesnt have to be there, use the default portcls value: 30s
        *secsConservationIdleTime = PORTCLS_CONVERSATION_IDLE_TIME;
        return S_OK;
    }
    else if( ERROR_SUCCESS != error )
    {
        LOG( pBasicLog, L"ERROR: GetConservationIdleTime: Failed to Open the Power Settings key with error: %u", error );

        return HRESULT_FROM_WIN32(error);
    }

    OnExit closePowerSettingsKey( [&]()
    {
        RegCloseKey( hKeyPowerSettings );
    } );

    // get the conservation idle time value
    error = RegQueryValueEx( hKeyPowerSettings,
                             L"ConservationIdleTime",
                             NULL,
                             NULL,
                             (LPBYTE)secsConservationIdleTime,
                             &cbRead );
    if( ERROR_SUCCESS != error )
    {
        LOG( pBasicLog, L"ERROR: GetConservationIdleTime: Failed to Query Conservation Idle Time with error: %u", error );

        return HRESULT_FROM_WIN32(error);
    }

    return S_OK;
}

HRESULT SetupApiHelper::GetDevnodeStatus( IBasicLog* pBasicLog, SetupApiContext* pSetupApiContext, ULONG* pStatus, ULONG* pProblem )
{
    CONFIGRET cmRet = CR_SUCCESS;
    ULONG status = 0;
    ULONG problem = 0;

    *pStatus = 0;
    *pProblem = 0;

    cmRet = CM_Get_DevNode_Status( &status,
                                   &problem,
                                   pSetupApiContext->DeviceInfoData.DevInst,
                                   0 );
    if( CR_SUCCESS != cmRet )
    {
        LOG( pBasicLog, L"ERROR: GetDevnodeStatus: Failed to Get Devnode Status with cr_error: %u", cmRet );

        return E_FAIL;
    }

    *pStatus = status;
    *pProblem = problem;

    return S_OK;
}
