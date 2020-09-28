#if !defined(_NTSTATUS_)
typedef long NTSTATUS;
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)
#endif

#include <windows.h>
#include <cguid.h>
#include "monwmipub.h"
#include "MonitorEdidInfo.h"

#ifndef ARRAY_SIZE
#define ARRAY_SIZE(x) (sizeof(x)/sizeof(*x))
#endif

namespace DEVX { namespace GRFX {


    HRESULT GetInstanceId( TCHAR const* DevPath, TCHAR* szInstanceId, DWORD* pIdSize )
    {
        if( !pIdSize )
        {
            return E_INVALIDARG;
        }

        struct SafeReleaseTrick 
        { 
            HDEVINFO info; 
            SP_DEVICE_INTERFACE_DATA data; 
        };

        union 
        {
            struct
            {
                HDEVINFO hardwareDeviceInfo; 
                SP_DEVICE_INTERFACE_DATA deviceInterfaceData; 
            };
            SafeReleaseTrick packet;
        };

        memset( &deviceInterfaceData, 0, sizeof(deviceInterfaceData) );

        SP_DEVINFO_DATA           deviceInfoData = {0};
        DWORD error;

        struct DestroyDeviceInfo
        {
            void operator ()( void* h ) const
            {
                 SetupDiDestroyDeviceInfoList(h);
            }
        };

        struct DeleteDeviceInterface
        {
            void operator ()( void* h ) const
            {
                if( h )
                {
                    SafeReleaseTrick* packet = (SafeReleaseTrick*)h;
                    if( !SetupDiDeleteDeviceInterfaceData( packet->info, &packet->data ) )
                    {
                        HRESULT hr = HRESULT_FROM_WIN32(GetLastError());
                    }
                    memset( &packet->data, 0, sizeof(packet->data) );
                }
            }
        };


        if( DevPath == NULL )
        {
            return E_INVALIDARG;
        }
        
        GUID const* pGuid = &GUID_DEVINTERFACE_MONITOR;
        //GUID const* pGuid = &GUID_DEVINTERFACE_DISPLAY_ADAPTER;
        
        hardwareDeviceInfo = SetupDiGetClassDevs( pGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE );
        if (INVALID_HANDLE_VALUE == hardwareDeviceInfo)
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        std::unique_ptr<void,DestroyDeviceInfo> rDeviceInfo;
        rDeviceInfo.reset( (void*)hardwareDeviceInfo );

        deviceInterfaceData.cbSize = sizeof(deviceInterfaceData);
        if( !SetupDiOpenDeviceInterface( hardwareDeviceInfo, DevPath, 0, &deviceInterfaceData ))
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        // make sure the device interface is cleaned up.
        std::unique_ptr<void,DeleteDeviceInterface> rInterfaceData;
        rInterfaceData.reset( (void*)&packet );

        deviceInfoData.cbSize = sizeof(deviceInfoData);
        if( !SetupDiGetDeviceInterfaceDetail( hardwareDeviceInfo,  &deviceInterfaceData,  NULL, 0, NULL,  &deviceInfoData ) &&
              (error = GetLastError()) != ERROR_INSUFFICIENT_BUFFER)
        {
            return HRESULT_FROM_WIN32(error);
        }

        if( !SetupDiGetDeviceInstanceId( hardwareDeviceInfo, &deviceInfoData, szInstanceId, *pIdSize, pIdSize ) )
        {
            return HRESULT_FROM_WIN32(GetLastError());
        }

        return S_OK;
    }

    HRESULT GetWmiInstanceId( TCHAR const* DevPath, __out_ecount(ulWmiSize) TCHAR* szWmiInstID, ULONG ulWmiSize )
    {
        ULONG ulWmiInstSize = 0;
        TCHAR szInstanceId[512];
        DWORD size = ARRAY_SIZE(szInstanceId);

        HRESULT hr = GetInstanceId( DevPath, szInstanceId, &size );
        if(S_OK == hr)
        {
            ulWmiInstSize = WmiDevInstToInstanceName( szWmiInstID, ulWmiSize, szInstanceId, 0);
        }
        return hr;
    }

    HRESULT GetRawEdid( LoggerWrapper* m_pLogger, DISPLAY_DEVICE const* pMonitor, std::unique_ptr<CMonitorRawEDID>& rRawEdid )
    {
        std::unique_ptr<CWMIHelper> rWMIHelper( new CWMIHelper() );
        HRESULT hr = rWMIHelper->Init(nullptr);
        if( FAILED(hr) )
        {
            LOG_ERROR( hr, _T("Failed to init wmi helper.") );
            return hr;
        }

        IWbemServices* pServices = rWMIHelper->GetWMIServices();
        
        TCHAR wmiInstanceId[512];
        hr = GetWmiInstanceId( pMonitor->DeviceID, wmiInstanceId, sizeof(wmiInstanceId)/sizeof(*wmiInstanceId) );
        if( FAILED(hr) )
        {
            // report error
            LOG_ERROR( hr, _T("Failed to get wmi instance for %s"), pMonitor->DeviceID );
            return hr;
        }
                
        rRawEdid.reset( new CMonitorRawEDID() );
        hr = rRawEdid->Init( pServices, tstr2str(wmiInstanceId).c_str() );
        if( FAILED(hr) )
        {
            LOG_ERROR( hr, _T("Failed to get edid for wmi instance id %s"), wmiInstanceId );
            return hr;
        }

        return hr;
    }

    TestResult::Type GetMonitorEdidModeList( LoggerWrapper* m_pLogger, DISPLAY_DEVICE const* pMonitor, ResMapFreqSet* pEdidResSet )
    {
        std::unique_ptr<CMonitorRawEDID> rRawEdid;
        HRESULT hr = GetRawEdid( m_pLogger, pMonitor, rRawEdid );
        if( FAILED(hr) )
        {
            return TestResult::Fail;
        }

        MONITOR_SUPPORTED_VIDEO_MODES msvm;
        memset( &msvm, 0, sizeof(msvm) );
        hr = rRawEdid->GetSupportedVideoModeList( &msvm );
        if( FAILED(hr) )
        {
            return TestResult::Fail;
        }

        for( USHORT i = 0; i < msvm.NumListedVideoModes; ++i )
        {
            float refresh = (float)msvm.SupportedVideoModes[i].VerticalRefreshRateNumerator / (float)msvm.SupportedVideoModes[i].VerticalRefreshRateDenominator;

            // use both floor and ceil to account for inconsistent rounding in OS and driver
            UINT32 high = (UINT32)ceil( refresh );
            UINT32 low = (UINT32)floor( refresh );

            Resolution r = { msvm.SupportedVideoModes[i].VerticalActivePixels, msvm.SupportedVideoModes[i].HorizontalActivePixels };
            (*pEdidResSet)[r].m_VerticalRefreshSet.insert( high );
            (*pEdidResSet)[r].m_VerticalRefreshSet.insert( low );
        }

        return TestResult::Pass;
    }

    HRESULT EnumEdid( LoggerWrapper* m_pLogger, DISPLAY_DEVICE const* pMonitor, UCHAR edid[128], UCHAR block )
    {
        std::unique_ptr<CMonitorRawEDID> rRawEdid;
        HRESULT hr = GetRawEdid( m_pLogger, pMonitor, rRawEdid );
        if( FAILED(hr) )
        {
            return hr;
        }

        return rRawEdid->EnumEdid( edid, block );
    }

    HRESULT LogMonitorDetails( LoggerWrapper* m_pLogger, DISPLAY_DEVICE const* pMonitor )
    {
        std::unique_ptr<CMonitorRawEDID> rRawEdid;
        HRESULT hr = GetRawEdid( m_pLogger, pMonitor, rRawEdid );
        if( SUCCEEDED(hr) )
        {
            USHORT manufacturerName[16];
            USHORT serialNumber[16];
            USHORT* pFriendlyName = nullptr;

            hr = rRawEdid->GetManufacturerName( manufacturerName );
            if( FAILED(hr) )
            {
                LOG_ERROR( hr, _T("Failed to get monitor manufacturer name.") );
                return hr;
            }

            hr = rRawEdid->GetSerialNumberID( serialNumber );
            if( FAILED(hr) )
            {
                LOG_ERROR( hr, _T("Failed to get monitor serial number ID.") );
                return hr;
            }

            hr = rRawEdid->GetUserFriendlyName( &pFriendlyName );
            if( FAILED(hr) )
            {
                LOG_ERROR( hr, _T("Failed to get monitor user friendly name.") );
                return hr;
            }

            LOG_MESSAGE( L"Manufacturer: %*s\nSerial Number: %*s\nName: %s",
                16, manufacturerName,
                16, serialNumber,
                pFriendlyName );
        }
        return hr;
    }

	std::ostream& operator << ( std::ostream& s, Resolution const& r )
	{
		s << r.x << "x" << r.y;
		return s;
	}

    bool ResCmp::operator ()( Resolution const& l, Resolution const& r )
    {
        return l.value < r.value;
    }

    RefreshSupport::RefreshSupport()
        : m_SupportStatus(false)
    {
    }

} }
