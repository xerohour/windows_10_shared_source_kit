/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

//
// UpdateDriverForPlugAndPlayDevices
//
typedef BOOL (WINAPI *UpdateDriverForPlugAndPlayDevicesProto)(HWND hwndParent,
                                                         LPCTSTR HardwareId,
                                                         LPCTSTR FullInfPath,
                                                         DWORD InstallFlags,
                                                         PBOOL bRebootRequired OPTIONAL
                                                         );
typedef BOOL (WINAPI *SetupSetNonInteractiveModeProto)(IN BOOL NonInteractiveFlag
                                                      );

#ifdef _UNICODE
#define UPDATEDRIVERFORPLUGANDPLAYDEVICES "UpdateDriverForPlugAndPlayDevicesW"
#else
#define UPDATEDRIVERFORPLUGANDPLAYDEVICES "UpdateDriverForPlugAndPlayDevicesA"
#endif
#define SETUPSETNONINTERACTIVEMODE "SetupSetNonInteractiveMode"

typedef VOID (*CallbackFunc)(HDEVINFO Devs, PSP_DEVINFO_DATA DevInfo, DWORD Index, LPVOID Context);

BOOL EnumerateDevices (DWORD Flags, LPCGUID Class, LPCTSTR Enumerator,
                            CallbackFunc Callback, LPVOID Context);
BOOL UpdateDevice (LPCTSTR InfFile, LPCTSTR PnpID, BOOL *Reboot);

LPCTSTR * GetDevMultiSz(HDEVINFO Devs,PSP_DEVINFO_DATA DevInfo,DWORD Prop);
LPCTSTR * GetMultiSzIndexArray(LPCTSTR MultiSz);
void DelMultiSz(LPCTSTR *MultiSz);