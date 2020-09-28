/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include <windows.h>
#include <setupapi.h>
#include <newdev.h>
#include <tchar.h>

#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>

#include "log.h"
#include "setupdi.h"

BOOL FindIdMatch (LPCTSTR * Array, LPCTSTR Enumerator)
/*++

Routine Description:

    Compares all strings in Array against Id
    Use WildCardMatch to do real compare

Arguments:

    Array - pointer returned by GetDevMultiSz
    MatchEntry - string to compare against

Return Value:

    TRUE if any match, otherwise FALSE

--*/
{
  if(Array)
  {
    while(Array[0])
    {
      if (_tcsnicmp (Array[0], Enumerator, lstrlen(Enumerator)) == 0)
        return TRUE;

      Array++;
    }
  }

  return FALSE;
}

LPCTSTR * GetMultiSzIndexArray(LPCTSTR MultiSz)
/*++

Routine Description:

    Get an index array pointing to the MultiSz passed in

Arguments:

    MultiSz - well formed multi-sz string

Return Value:

    array of n strings
    array[0] points to original MultiSz
    array[n] == NULL
    returns NULL on failure

--*/
{
    LPCTSTR scan;
    LPCTSTR * array;
    int elements;

    for(scan = MultiSz, elements = 0; scan[0] ;elements++) {
        scan += lstrlen(scan)+1;
    }
    array = new LPCTSTR[elements+1];
    if(!array) {
        return NULL;
    }

    if(elements) {
        for(scan = MultiSz, elements = 0; scan[0]; elements++) {
            array[elements] = scan;
            scan += lstrlen(scan)+1;
        }
    }
    array[elements] = NULL;
    return array;
}


LPCTSTR * GetDevMultiSz(HDEVINFO Devs,PSP_DEVINFO_DATA DevInfo,DWORD Prop)
/*++

Routine Description:

    Get a multi-sz device property
    and return as an array of strings

Arguments:

    Devs    - HDEVINFO containing DevInfo
    DevInfo - Specific device
    Prop    - SPDRP_HARDWAREID or SPDRP_COMPATIBLEIDS

Return Value:

    array of strings. last entry+1 of array contains NULL
    returns NULL on failure

--*/
{
    LPTSTR buffer;
    DWORD size;
    DWORD reqSize;
    DWORD dataType;
    LPCTSTR * array;
    DWORD szChars;

    size = 8192; // initial guess, nothing magic about this
    buffer = new TCHAR[(size/sizeof(TCHAR))+2];
    if(!buffer) {
        return NULL;
    }
    while(!SetupDiGetDeviceRegistryProperty(Devs,DevInfo,Prop,&dataType,(LPBYTE)buffer,size,&reqSize)) {
        if(GetLastError() != ERROR_INSUFFICIENT_BUFFER) {
            goto failed;
        }
        if(dataType != REG_MULTI_SZ) {
            goto failed;
        }
        size = reqSize;
        delete [] buffer;
        buffer = new TCHAR[(size/sizeof(TCHAR))+2];
        if(!buffer) {
            goto failed;
        }
    }
    szChars = reqSize/sizeof(TCHAR);
    buffer[szChars] = TEXT('\0');
    buffer[szChars+1] = TEXT('\0');
    array = GetMultiSzIndexArray(buffer);
    if(array) {
        return array;
    }

failed:
    if(buffer) {
        delete [] buffer;
    }
    return NULL;
}

void DelMultiSz(LPCTSTR *MultiSz)
{
    // first delete the original buffer
    if (MultiSz) { delete [] MultiSz[0]; }

    // then delete the array of pointers
    delete [] MultiSz;
}

//
// EnumerateDevices
//
// Flags: SetupDi flags
// Class: class to enumerate, can be NULL
// Enumerator: Full or partial PnP ID or device interface name
// Partial: Specifies if the Enumerator is partial or full ID
// Callback: Once something is found the callback is called
// Context: callback context
BOOL EnumerateDevices (DWORD Flags, LPCGUID Class, LPCTSTR Enumerator,
                            CallbackFunc Callback, LPVOID Context)
{
  HDEVINFO                    devs = INVALID_HANDLE_VALUE;
  SP_DEVINFO_LIST_DETAIL_DATA devInfoListDetail;
  SP_DEVINFO_DATA             devInfo;

  //
  // add all id's to list
  // if there's a class, filter on specified class
  //
  devs = SetupDiGetClassDevs (Class,
                              NULL,
                              NULL,
                              (Class ? 0 : DIGCF_ALLCLASSES) | Flags);

  if (devs == INVALID_HANDLE_VALUE)
    return FALSE;

  devInfoListDetail.cbSize = sizeof(devInfoListDetail);
  if(!SetupDiGetDeviceInfoListDetail (devs, &devInfoListDetail))
  {
    SetupDiDestroyDeviceInfoList (devs);
    return FALSE;
  }

  devInfo.cbSize = sizeof(devInfo);
  for(int devIndex = 0; SetupDiEnumDeviceInfo (devs, devIndex, &devInfo); devIndex++)
  {
    LPCTSTR *hwIds = NULL;
    LPCTSTR *compatIds = NULL;

    //
    // determine hardware ID's
    // and search for matches
    //
    hwIds = GetDevMultiSz (devs, &devInfo, SPDRP_HARDWAREID);
    compatIds = GetDevMultiSz (devs, &devInfo, SPDRP_COMPATIBLEIDS);

    if (FindIdMatch (hwIds, Enumerator) ||
        FindIdMatch (compatIds, Enumerator))
    {
      Callback (devs, &devInfo, devIndex, Context);
    }
    
    DelMultiSz (hwIds);
    DelMultiSz (compatIds);
  }

  SetupDiDestroyDeviceInfoList (devs);
  return TRUE;
}


BOOL UpdateDevice (LPCTSTR InfFile, LPCTSTR PnpID, BOOL *Reboot)
/*++

Routine Description:
    UPDATE
    update driver for existing device(s)

Arguments:

    InfFile  - INF file name with or without path
    PnpID    - PnP ID of device to be updated

Return Value:

    EXIT_xxxx

--*/
{
    UpdateDriverForPlugAndPlayDevicesProto UpdateFn;
    TCHAR   InfPath[MAX_PATH];
    DWORD   res, flags = INSTALLFLAG_FORCE;
    HMODULE newdevMod = NULL;

    //
    // Inf must be a full pathname
    //
    res = GetFullPathName (InfFile, MAX_PATH, InfPath, NULL);
    if ((res >= MAX_PATH) || (res == 0))
    {
        SLOG(eError, "GetFullPathName returned unexpected value %u", res);
        return FALSE;
    }

    //
    // inf doesn't exist?
    //
    if (GetFileAttributes (InfPath) == (DWORD)(-1))
    {
        SLOG(eError, "Could not find file %s", InfPath);
        return FALSE;
    }

    //
    // make use of UpdateDriverForPlugAndPlayDevices
    //
    newdevMod = LoadLibrary(TEXT("newdev.dll"));
    if(!newdevMod)
    {
        SLOG(eError, "LoadLibrary(newdev.dll) failed: GetLastError = %u", GetLastError());
        return FALSE;
    }

    UpdateFn = (UpdateDriverForPlugAndPlayDevicesProto)GetProcAddress (newdevMod, UPDATEDRIVERFORPLUGANDPLAYDEVICES);
    if(!UpdateFn)
    {
        SLOG(eError, "GetProcAddress() failed: GetLastError = %u", GetLastError());
        FreeLibrary(newdevMod);
        return FALSE;
    }

    if (!UpdateFn (NULL, PnpID, InfPath, flags, Reboot))
    {
        SLOG(eError, "UpdateDriverForPlugAndPlayDevices failed: GetLastError() = %u", GetLastError());
        FreeLibrary(newdevMod);
        return FALSE;
    }

    // allow the newly installed device time to initialize
    Sleep(10 * 1000);

    FreeLibrary(newdevMod);
    return TRUE;
}



