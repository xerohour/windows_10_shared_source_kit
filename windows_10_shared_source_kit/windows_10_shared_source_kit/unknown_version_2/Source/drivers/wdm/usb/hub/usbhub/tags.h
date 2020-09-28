
/*++

Copyright (c) Microsoft Corporation. All rights reserved

Module Name:

    tags.h

Abstract:

    tag definitions we use for structures internal to the hub

Author:

    jdunn

Environment:

    Kernel mode

Revision History:

--*/

/***********************************/

#ifndef   __TAGS_H__
#define   __TAGS_H__

#define USBHUB_HEAP_TAG     0x42554855      //"UHUB"
#define USBHUB_FREE_TAG     0x62756875      //"uhub"

/* lock sigs */
#define FdoPnpLock_Sig_FDO1     '1ODF'
#define FdoPwrLock_Sig_FDO2     '2ODF'
#define PdoPnpLock_Sig_PDOx     'xODP'
#define HardResetLock_Sig_RSTx  'xTSR'


#define WakePoComplete_Action_Tag1          '1kaw'  // wak1
#define WakePoComplete_Action_Tag2          '2kaw'  // wak2
#define DxPoComplete_Action_Tag             'opxd'  // dxpo
#define D0PoComplete_Action_Tag1            '1p0d'  // d0p1
#define D0PoComplete_Action_Tag2            '2p0d'  // d0p2
#define S0IoComplete_Action_Tag1            '10si'  // is01
#define S0IoComplete_Action_Tag2            '20si'  // is02
#define SxIoComplete_Action_Tag1            '1xsi'  // isx1
#define SxIoComplete_Action_Tag2            '2xsi'  // isx2
#define SystemPowerState_Tag0               '0sys'  // sys0
#define SystemPowerState_Tag1               '1sys'  // sys1
#define SystemPowerState_Tag2               '2sys'  // sys2
#define SystemPowerState_Tag3               '3sys'  // sys3
#define DevicePowerState_Tag                'sved'  // devs
#define D0Worker_Action_Tag1                '1w0d'  // d0w1
#define D0Worker_Action_Tag2                '2w0d'  // d0w2

#define PdoSetD0_Finish_Tag                 'nifP'  // Pfin
#define PdoSetD0_Tag                        '0DsP'  // PsD0
#define PdoSystemPowerState_Tag             'sysP'  // Psys
#define PdoDevicePowerState_Tag             'vedP'  // Pdev
#define PdoDeviceWake_Tag                   'vedW'  // Wdev
#define PdoDeviceRseet_Tag                  'serD'  // Dres
#define PdoUserDescReq_Tag                  'qrdU'  // Udrq

#define PdoCheckPortTag                     'tpKC'  // CKpt

#define PdoSurpriseRemove_Tag               'meRS'  // SRem
#define PdoRemoveDevice_Tag                 'DmeR'  // RemD

#define FdoCheckIdleTag                     'diKC'  // CKid

/*
SSH (hub selective suspend) busy list tags
*/

#define UsbhBusyIsr_Tag                     '1Bss'  // ssB1
#define UsbhBusyResetIrqPipe_Tag            'RBss'  // ssBR
#define UsbhBusyPCH_Tag                     'CPss'  // ssPC
#define UsbhBusyBUS_Tag                     'UBss'  // ssBU
#define UsbhBusyEnum_Tag                    'UBne'  // enBU
#define UsbhBusyResume_Tag                  'UBsr'  // rsBU
#define UsbhBusyBusLock_Tag                 'UBkl'  // lkBU
#define UsbhIocBusyResume_Tag               'UBci'  // icBU
#define UsbhBusyBusPnPStop_Tag              'UBts'  // lkBU
#define UsbhBusyEnumFailed_Tag              'UBfl'  // lfBU
#define UsbhBusyPdoSetD0_Tag                'UBdp'  // pdBU
#define PdoSetD0ResumeRequested_Tag         'RRdp'  // RRBU
#define UsbhPcqResume_Tag                   'Rqcp'  // pcqR
#define UsbhSshDisabled_Tag                 'iDss'  // ssDi
#define UsbhSshEnterSx_Tag                  'xSss'  // ssSx
#define UsbhSshPortsBusy_Tag                'bPss'  // ssPb
#define UsbhWaitEvent_Tag                   'WTss'  // ssTW
#define UsbhBusySoftRemove_Tag              'RSss'  // ssSR
#define UsbhBusyHardReset_Tag               'UBrh'  // hrBU
#define UsbhSshResumeChildHub_Tag           'CRrh'  // hrRC
#define UsbhSshResumeUpstream_Tag           'PUrh'  // hrRC
#define UsbhBusyPortConnect_Tag             'CPss'  // ssPC
#define UsbhEnablePdo_Tag                   'PEss'  // ssEP
#define UsbhOverCurrentBusy_Tag             'COss'  // ssOC

// devh tags
#define Devh_Pdo_Tag                        'HDdp'  // pdDH
#define Devh_ReturnDH_Tag                   'HDtr'  // rtDH
#define Devh_QIF_Tag                        'HDfq'  // qfDH
#define Devh_PdoApi_gni_Tag                 'HDng'  // gnDH
#define Devh_PdoApi_ssc_Tag                 'HDcs'  // scDH
#define Devh_PdoApi_perf_Tag                'HDpf'  // pfDH
#define Devh_PdoApi_HubNode_Tag             'HDhn'  // hnDH
#define Devh_PdoApi_DeviceNode_Tag          'HDdn'  // dnDH
#define Devh_ResetOld_Tag                   'HDlo'  // olDH
#define Devh_ResetNew_Tag                   'HDwn'  // nwDH
#define Devh_Pdo_Flush_Tag                  'HDlf'  // flDH
//UsbhSyncSendCommandToDevice
#define Devh_Pdo_SyncSendCommand            'DNSs'  // sSND
//UsbhClearPdoIdleReady
#define Devh_ClearIdle_Tag                  'Irlc'  // clrI
//UsbhSetPdoIdleReady
#define Devh_SetIdle_Tag                    'Ites'  // setI
//UsbhPdoSetDeviceData
#define Devh_SetDevData_Tag                 'Dtes'  // setD
//UsbhPdoAbortAllDevicePipes
#define Devh_AbortAllDevicePipes_Tag        'Drba'  // abrD
//UsbhFdoHandleNotifyForwardProgress
#define Devh_ForwardProgress_Tag            'DFwP'  // PwFD


#define msosExtProp_Tag         '1osm'  // mso1
#define wmi_Tag                 '1imw'  // wmi1
#define initcallback_Tag        'OTni'  // inTO

/*
Reference Tags, used in reference list
*/
#define hFDO 'ODFh'                   // hub Fdo
#define hEXR 'RXEh'                   // exception record
#define PDOt 'tODP'                   // pdo timer
#define OutOfBwObject_OBWt 'tWBO'     // out-of-bandwidth timer
#define dfPW 'wPfd'                   // deferred power request
#define DMtm 'mtMD'                   // dmTimer init
#define dTMR 'RMTd'     // dmTimer running
#define OutOfBwTimer_obtm   'mtbo'     // out-of-bandwidth timer (kernel timer)
#define rTMR 'RMTr'     // reset timer running
#define hASY 'YSAh'     // USB2 async callback
#define syIO 'OIys'     // synch IO call
#define PwrI 'IrwP'     // synch IO call
#define TOSr 'rSOT'     // ref to the top of the USB device stack
#define IDLE_Tag 'ELDI'     // Idle ready
#define DevOverCurrentWorker_ocdW       'Wdco'  // device overcurrent reset worker
#define AutoOverCurrentWorker_ocaW      'Waco'  // auto overcurrent reset worker
#define Qtmo 'omtQ'     // PCQ reset timeout
/*
Pdo Latch tags
*/

//UsbhFdoResetPdoPort
#define PdoLatchResetPort_Tag                   'dTSR'
//UsbhCycleDevicePort
#define PdoLatchCyclePort_Tag                   'lCYC'
//UsbhSyncResumeDeviceInternal
#define PdoLatchResumePort_Tag                  'pSMR'
//UsbhIoctlGetNodeConnectionDriverKeyName
#define PdoLatchIo05_Tag                        'Il05'
//UsbhIoctlGetNodeConnectionName
#define PdoLatchIo03_Tag                        'Il03'
//UsbhHubSyncSuspendPortEvent
#define PdoLatchSyncSuspend_Tag                 'sPDO'
//UsbhIdleIrp_Event
#define PdoLatchIdleWorker_Tag                  'eldI'

//UsbhFdoSetD0Cold
#define PdoLatchD0_Cold_Tag                     'COLD'
#define PdoLatchD0_Cold2_Tag                    'CLD2'
//UsbhCompletePdoWakeIrps
#define PdoLatchD0_WakeIrpCpt_Tag               'pdoW'
//UsbhFdoSetPowerDx_Action
#define PdoLatch_FdoSetD3_Tag                   'FdD3'
#define PdoLatch_FdoSetD2                       'FdD2'
//Usbh_PCE_BusDisconnect_Action
#define PdoLatch_PCE_Disconnect_Tag             'rPDO'
//UsbhFdoSetD0WarmPortResumeFailure
#define PdoLatchD0_Warm_Tag                     'warm'
//UsbhInsertQueuePortChangeObject
#define PdoLatchInsertQueuePortChange           'IQPC'
#define PdoLatchD0_WarmReset_Tag                'WMRS'
#define PdoLatch_Pause_Tag                      'PauP'
#define PdoLatch_Resume_Tag                     'ResP'
#define PdoLatch_DNF_Timer_Tag                  'DNFT'

//UsbhBusUnlatchPdo
//UsbhLatchPdo(HubFdo, PortData->PortNumber, NULL, 'pPDO')
#define PdoBusLatch_Tag                         'pPDO'
//UsbhGlobalSuspendPort
#define PdoGlobalSuspend_Tag                    'Slbg'
// 
#define PdoGlobalResume_Tag                     'Rlbg'
#define PdoBusIfSuspendHub_Tag                  'HSiB'
#define PdoBusIfResumeHub_Tag                   'HSiB'

#define PdoBusSurpriseRemove_Tag                   'RuSB'

#define PdoDriverNotFoundSuspend_Tag             'SFND'

/*
IOCTL (Io) Tags
UsbhIncPdoIoCount, UsbhDecPdoIoCount
*/

#define IOCTL_INTERNAL_USB_GET_HUB_COUNT_Tag            'Io00'

#define IOCTL_INTERNAL_USB_GET_ROOTHUB_PDO_Tag          'Io01'

#define IOCTL_INTERNAL_USB_GET_CONTROLLER_NAME_Tag      'Io02'

#define IOCTL_INTERNAL_USB_GET_BUS_INFO_Tag             'Io03'

#define IOCTL_INTERNAL_USB_SUBMIT_URB_Tag               'Io04'

#define IOCTL_INTERNAL_USB_GET_PORT_STATUS_Tag          'Io05'

#define IOCTL_INTERNAL_USB_GET_TT_DEVICE_HANDLE_Tag     'Io06'

#define IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_Tag        'Io07'

#define IOCTL_INTERNAL_USB_GET_DEVICE_HANDLE_EX_Tag     'Io08'

#define IOCTL_INTERNAL_USB_RESET_PORT_Tag               'Io09'

#define IOCTL_INTERNAL_USB_ENABLE_PORT_Tag              'Io0a'

#define IOCTL_INTERNAL_USB_CYCLE_PORT_Tag               'Io0b'

#define IOCTL_INTERNAL_USB_GET_HUB_NAME_Tag             'Io0c'

#define IOCTL_INTERNAL_USB_GET_PARENT_HUB_INFO_Tag      'Io0d'

#define IOCTL_INTERNAL_USB_SUBMIT_IDLE_NOTIFICATION_Tag 'Io0e'

#define IOCTL_INTERNAL_USB_RECORD_FAILURE_Tag           'Io0f'

#define IOCTL_INTERNAL_USB_GET_DEVICE_CONFIG_INFO_Tag      'Io10'

#define IOCTL_INTERNAL_USB_GET_DUMP_DATA_Tag               'Io11'

#define IOCTL_INTERNAL_USB_FREE_DUMP_DATA_Tag              'Io12'

#define IOCTL_INTERNAL_USB_NOTIFY_FORWARD_PROGRESS_Tag     'Io13'

#endif

