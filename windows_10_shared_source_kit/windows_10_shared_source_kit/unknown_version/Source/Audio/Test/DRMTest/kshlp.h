// Copyright (C) Microsoft. All rights reserved.
#ifndef _KSHLP_H
#define _KSHLP_H

BOOL FindDRMPin
(
    PCKsFilter                  pFilter,
    TTList <CKsPin>             *pPinList,
    PCKsPin                    *ppKsDRMPin,
    PCKsPin                     pKsPStart = NULL
);

BOOL FindPCMPin
(
    PCKsFilter                  pFilter,
    TTList <CKsPin>             *pPinList,
    PCKsPin                    *ppKsPCMPin
);

BOOL
FindPin
(
    PCKsFilter                  pFilter,
    TTList <CKsPin>             *pPinList,
    PCKsPin                    *ppKsDRMPin,
    PCKsPin                     pKsPStart = NULL,
    LPCGUID                     guidSubFormat = NULL,
    LPCGUID                     guidSpecifier = NULL
);

#endif
