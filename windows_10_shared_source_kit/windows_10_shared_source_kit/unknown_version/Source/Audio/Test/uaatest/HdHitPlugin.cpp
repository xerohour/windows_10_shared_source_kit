/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
#include <windows.h>
#include <tchar.h>
#include <atlstr.h>

#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>
#include <cfgmgr32.h>
#include <setupapi.h>
#include <hitframework.h>
#define USERMODE // or azcodec.h includes wdm.h (kernel mode version of windows.h)
#include <azcodec.h>
#include <privioct.h>
#include <controller-interface.h>

#include "install.h"
#include "uaatest.h"
#include "HdRegTest.h"
#include "HdHitLog.h"
#include "HdHitPlugin.h"
#include "hdaudiomemoryspace.h"
#include "controller.h"
extern Hit::ILog *Log;

HRESULT ResetController(HANDLE hController, double* pdEnterResetTime, double* pdExitResetTime)
{

    DWORD BytesReturned;
    HRESULT hResult = STATUS_SUCCESS;

    double times[] = { 0.0, 0.0 };

    BOOL b = DeviceIoControl(hController, IOCTL_HDCNTTEST_RESET, NULL , 0, times, sizeof(times), &BytesReturned,0);//HdCntRead.pRWBuffer, HdCntRead.RWBytes, &BytesReturned, 0);

    if (!b) {
        hResult = GetLastError();
    } else {
        *pdEnterResetTime = times[0];
        *pdExitResetTime = times[1];
    }

    return hResult;
}

HRESULT TestRead(HANDLE hController, HDCNT_ReadWrite& HdCntRead)
{

    DWORD BytesReturned;
    HRESULT hResult = STATUS_SUCCESS;

    BOOL b = DeviceIoControl(hController, IOCTL_HDCNTTEST_READ, &HdCntRead, sizeof (HDCNT_ReadWrite), HdCntRead.RWBuffer, HdCntRead.RWBytes, &BytesReturned,0);//HdCntRead.pRWBuffer, HdCntRead.RWBytes, &BytesReturned, 0);

    if (!b) {
        hResult = GetLastError();
    }
    return hResult;
}

HRESULT TestWrite(HANDLE hController, HDCNT_ReadWrite HdCntWrite)
{

    DWORD BytesReturned;
    HRESULT hResult = STATUS_SUCCESS;  

    BOOL  b = DeviceIoControl(hController, IOCTL_HDCNTTEST_WRITE, &HdCntWrite, sizeof (HDCNT_ReadWrite), NULL, 0, &BytesReturned, 0);
    if (!b) {
        hResult = GetLastError();
    }

    return hResult;

}


namespace Hit {
	
	HRESULT HdCntRead (		
		IN ULONG                  Offset, 				
		IN ULONG                  Length,				
		__bcount(Length) IN PVOID Buffer,				
		IN OUT PVOID              Context			
		)
	{
	     HDCNT_ReadWrite HdRead;
            HRESULT hResult;
            HdRead.RWBytes = Length;
            HdRead.RWOffset = Offset;
//            HdRead.RWBuffer = Buffer;
            HANDLE hController =  *(HANDLE*)Context;
            hResult = TestRead(hController , HdRead);
            memcpy_s(Buffer, Length, (PVOID)HdRead.RWBuffer, HdRead.RWBytes);
            return hResult;
	}


	HRESULT HdCntWrite (		
		IN ULONG                  Offset, 				
		IN ULONG                  Length,				
		__bcount(Length) IN PVOID Buffer,				
		IN OUT PVOID              Context			
		)
	{
	        HDCNT_ReadWrite HdWrite;
            HdWrite.RWBytes = Length;
            HdWrite.RWOffset = Offset;
            memcpy_s( HdWrite.RWBuffer, sizeof(HdWrite.RWBuffer), Buffer, Length);
            HANDLE hController = *(HANDLE*)Context;

            return TestWrite(hController , HdWrite);
	}

}






