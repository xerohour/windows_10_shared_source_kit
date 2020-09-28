#ifndef EXTERNAL
#if (NTDDI_VERSION < NTDDI_VISTA)

/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include <windows.h>
#include <setupapi.h>
#include <hdaudiopolicy.h>
#include "hdaudio.h"
#include "uaatest.h"
#include "log.h"

//==============================================================================
// GetHDAudioIoctlInterfaceHandle
//
// Description:
//  Helper function to get a handle to the specified device's 
//  GuidHDAudioIoctlInterface interface.
//
// Arguments:
//  PCHAR pcDevID   Device Instance ID
//
// Returns:
//  HANDLE - INVALID_HANDLE_VALUE on failure, any other value indicates success
//
// History:
//  2/26/04 - jwexler - created
//==============================================================================
HANDLE GetHDAudioIoctlInterfaceHandle(PCHAR pcDevID)
{
    HANDLE hInterface = NULL;
    BOOL fRes = FALSE;

    // get the set of devices that expose the private hdaudio ioctl interface
    HDEVINFO hHDAudioSet = SetupDiGetClassDevs(&GuidHDAudioIoctlInterface, 
                                               NULL, 
                                               NULL, 
                                               DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    BOOL fFound = FALSE;
    SP_DEVINFO_DATA devinfodata;
    for (ULONG ulMember = 0; !fFound; ulMember++)
    {
        devinfodata.cbSize = sizeof(SP_DEVINFO_DATA);
        if (!SetupDiEnumDeviceInfo(hHDAudioSet, ulMember, &devinfodata))
        {
            if (ERROR_NO_MORE_ITEMS == GetLastError())
                break;
            else
            {
                SLOG(eError, "Error: Failed SetupDiEnumDeviceInfo");
                fRes = FALSE;
                goto DONE;
            }
        }
        // get the device instance id
        DWORD dwBufSize = 0;
        fRes = SetupDiGetDeviceInstanceId(hHDAudioSet, 
                                          &devinfodata, 
                                          NULL, 
                                          0, 
                                          &dwBufSize);
        CBuffer<CHAR> pcDevID2(dwBufSize);
        fRes = SetupDiGetDeviceInstanceId(hHDAudioSet, 
                                          &devinfodata, 
                                          pcDevID2, 
                                          dwBufSize, 
                                          NULL);
        if (!fRes)
        {
            SLOG(eError, "Error: Could not get Device Instance ID");
            goto DONE;
        }                        

        if (!strcmp(pcDevID, pcDevID2))
            fFound = TRUE;
        
    }
    if (!fFound)
    {
        XLOG(XWARN, eWarn1, "Warning: Could not find a GuidHDAudioIoctlInterface interface for this Device Instance ID");
        fRes = FALSE;
        goto DONE;
    }

    // we found a match, get the device interface 
    SP_DEVICE_INTERFACE_DATA devintdata;
    devintdata.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
    fRes = SetupDiEnumDeviceInterfaces(hHDAudioSet, 
                                       &devinfodata, 
                                       &GuidHDAudioIoctlInterface, 
                                       0, 
                                       &devintdata);
    if (!fRes)
    {
        SLOG(eError, "Error: Could not get Device Interface");
        goto DONE;
    }

    // get the device interface details
    DWORD dwBufSize = 0;
    fRes = SetupDiGetDeviceInterfaceDetail(hHDAudioSet, 
                                           &devintdata, 
                                           NULL, 
                                           0, 
                                           &dwBufSize, 
                                           NULL);
    CBuffer<BYTE> pdevintdetaildatabuffer(dwBufSize);
    PSP_DEVICE_INTERFACE_DETAIL_DATA pdevintdetaildata = (PSP_DEVICE_INTERFACE_DETAIL_DATA)(PBYTE)pdevintdetaildatabuffer;
    pdevintdetaildata->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    fRes = SetupDiGetDeviceInterfaceDetail(hHDAudioSet, 
                                           &devintdata, 
                                           pdevintdetaildata, 
                                           dwBufSize, 
                                           NULL, 
                                           NULL);

    if (!fRes)
    {
        SLOG(eError, "Error: Could not get device interface details!");
        SLOG(eError, "  GetLastError == %u", GetLastError());
        goto DONE;
    }
    SLOG(eInfo1, "  Path: %s", pdevintdetaildata->DevicePath);
    // return a handle to the driver                    
    hInterface = CreateFile(pdevintdetaildata->DevicePath, 
                            GENERIC_READ | GENERIC_WRITE, 
                            0, 
                            NULL, 
                            OPEN_EXISTING, 
                            FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED, 
                            NULL);
DONE:
    
    if (!fRes)
        hInterface = INVALID_HANDLE_VALUE;

    return hInterface;
} // GetHDAudioIoctlInterfaceHandle

//==============================================================================
// PolicyIOCTLs
//
// Description:
//  Tests the private IOCTLs supported by hdaudio.sys through the 
//  GuidHDAudioIoctlInterface interface.  Based on the reported capabilities, 
//  the test tries various interesting policy settings and gets them back to 
//  ensure they were written.  Where appropriate, the test case should use 
//  additional means to verify that the policy changes actually took.  The 
//  test case restores the policy settings back to their original state before 
//  exiting.
//
// Arguments:
//  None
//
// Returns:
//  BOOL - TRUE if all private IOCTLs are functioning as expected, FALSE if not (or error)
//
// History:
//  2/26/04 - jwexler - created
//==============================================================================
DWORD PolicyIOCTLs()
{
    BOOL dwRes = FNS_FAIL;

    // get this AFD's ioctl interface
    HANDLE hInterface = GetHDAudioIoctlInterfaceHandle(((CAFD*)g_pUAATest->m_pCurDevice)->m_szDevInstID);
    if (INVALID_HANDLE_VALUE == hInterface)
    {
        SLOG(eError, "Error: Failed to get HDAudioIoctlInterface handle for this device");
        dwRes = FNS_FAIL;
        goto DONE;
    }

    // IOCTL_HDAUD_GET_CAPS
    HDAudioDriverCaps caps;
    DWORD dwSize = 0;
    if (!DeviceIoControl(hInterface, 
                         IOCTL_HDAUD_GET_CAPS, 
                         NULL, 
                         0, 
                         &caps, 
                         sizeof(HDAudioDriverCaps), 
                         &dwSize, 
                         NULL))
    {
        SLOG(eError, "Error: Failed IOCTL_HDAUD_GET_CAPS");
        dwRes = FNS_FAIL;
        goto DONE;
    }
    SLOG(eInfo1, "Capabilities:");
    SLOG(eInfo1, " Version == %u", caps.Version);
    SLOG(eInfo1, " RetaskSupported == %u", caps.RetaskSupported);
    for (ULONG i = 0; i < eHDAudioNumHpModes; i++)
        SLOG(eInfo1, " HpSupport[%u] == %u", i, caps.HpSupport[i]);
    SLOG(eInfo1, " SpdifOut == %u", caps.SpdifOut);
    for (i = 0; i < eHDAudioSpdifNumSampleRates; i++)
        SLOG(eInfo1, " SpdifInputSampleRateSupport[%u] == %u", i, caps.SpdifInputSampleRateSupport[i]);
    for (i = 0; i < eHDAudioSpdifNumBitDepths; i++)
        SLOG(eInfo1, " SpdifInputBitDepthSupport[%u] == %u", i, caps.SpdifInputBitDepthSupport[i]);
    for (i = 0; i < eHDAudioSpdifNumSampleRates; i++)
        SLOG(eInfo1, " SpdifOutputSampleRateSupport[%u] == %u", i, caps.SpdifOutputSampleRateSupport[i]);
    for (i = 0; i < eHDAudioSpdifNumBitDepths; i++)
        SLOG(eInfo1, " SpdifOutputBitDepthSupport[%u] == %u", i, caps.SpdifOutputBitDepthSupport[i]);
    SLOG(eInfo1, " CenterLfeSwap == %u", caps.CenterLfeSwap);

    // IOCTL_HDAUD_GET_POLICY and stash it away
    HDAudioGlobalPolicy policyOriginal;
    dwSize = 0;
    if (!DeviceIoControl(hInterface, 
                         IOCTL_HDAUD_GET_POLICY, 
                         NULL, 
                         0, 
                         &policyOriginal, 
                         sizeof(HDAudioGlobalPolicy), 
                         &dwSize, 
                         NULL))
    {
        SLOG(eError, "Error: Failed IOCTL_HDAUD_GET_POLICY");
        dwRes = FNS_FAIL;
        goto DONE;
    }
    SLOG(eInfo1, "Original Policy:");
    SLOG(eInfo1, " Version == %u", policyOriginal.Version);
    SLOG(eInfo1, " CreateRearMaxOutChannels == %u", policyOriginal.CreateRearMaxOutChannels);
    SLOG(eInfo1, " MaxRearSampleRate == %u", policyOriginal.MaxRearSampleRate);
    SLOG(eInfo1, " RetaskMode == %u", policyOriginal.RetaskMode);
    SLOG(eInfo1, " HpMode == %u", policyOriginal.HpMode);
    SLOG(eInfo1, " HpShutsOffRearSpeaker == %u", policyOriginal.HpShutsOffRearSpeaker);
    SLOG(eInfo1, " SpdifOutputEchosRearRenderWhenNoAc3 == %u", policyOriginal.SpdifOutputEchosRearRenderWhenNoAc3);
    SLOG(eInfo1, " SpdifInputSampleRate == %u", policyOriginal.SpdifInputSampleRate);
    SLOG(eInfo1, " SpdifInputBitDepth == %u", policyOriginal.SpdifInputBitDepth);

    for (i = 0; i < eHDAudioSpdifNumSampleRates; i++)
        SLOG(eInfo1, " ExternalSpdifReceiverSupportedSampleRates[%u] == %u", i, policyOriginal.ExternalSpdifReceiverSupportedSampleRates[i]);

    for (i = 0; i < eHDAudioSpdifNumBitDepths; i++)
        SLOG(eInfo1, " ExternalSpdifReceiverSupportedBitDepths[%u] == %u", i, policyOriginal.ExternalSpdifReceiverSupportedBitDepths[i]);

    SLOG(eInfo1, " CenterLfeSwap == %u", policyOriginal.CenterLfeSwap);
    SLOG(eInfo1, " PredefinedCodecsUseHardCodedTopo == %u", policyOriginal.PredefinedCodecsUseHardCodedTopo);
    SLOG(eInfo1, " DcOffsetWorkaround == %u", policyOriginal.DcOffsetWorkaround);
    
    // ***NOTE: This is an experiment in model-based testing
    // The goal is to get a taste of it and some of the tools involved.
    // I'm also curious if we gain additional code coverage or catch any bugs that otherwise would have been missed.
    
    // 1) Any action is possible from any state when the caps support it
    // 2) Outcome: Policy IOCTL reports the change - behavior is changed (which is more difficult to test)
    
    // entry point is whatever the current policy is set to

    // V1_0 - retask not supported

    // Loop on each interesting parameter combination
    //  IOCTL_HDAUD_SET_POLICY
    //  IOCTL_HDAUD_GET_POLICY and verify changes
    
    // IOCTL_HDAUD_SET_POLICY back to original
    dwRes = FNS_PASS;

DONE:    
    return dwRes;
} // PolicyIOCTLs
#endif // #if (NTDDI_VERSION < NTDDI_VISTA)
#endif // #ifndef EXTERNAL