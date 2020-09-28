/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/

#ifndef _PCI_HCT_PLUG_IN_H_
#define _PCI_HCT_PLUG_IN_H_

HRESULT TestRead (HANDLE hController, HDCNT_ReadWrite& HdCntRead);
HRESULT TestWrite (HANDLE hController, HDCNT_ReadWrite HdCntWrite);
HRESULT ResetController(HANDLE hController, double* pdEnterResetTime, double* pdExitResetTime);

namespace Hit {

    HRESULT HdCntRead (
        IN ULONG                  Offset,
        IN ULONG                  Length,
        __bcount(Length) IN PVOID Buffer,
        IN OUT PVOID              Context
        );

	HRESULT HdCntWrite (
        IN ULONG                  Offset,
        IN ULONG                  Length,
        __bcount(Length) IN PVOID Buffer,
        IN OUT PVOID              Context
        );
}

#endif

