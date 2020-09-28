//---------------------------------------------------------------------------
//
//  Module:   validate.h
//
//  Description:
//
//











//
//  THIS CODE AND INFORMATION IS PROVIDED "AS IS" WITHOUT WARRANTY OF ANY
//  KIND, EITHER EXPRESSED OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE
//  IMPLIED WARRANTIES OF MERCHANTABILITY AND/OR FITNESS FOR A PARTICULAR
//  PURPOSE.
//
//  Copyright (c) Microsoft.  All rights reserved.
//
//---------------------------------------------------------------------------

#ifndef _VALIDATE_H_
#define _VALIDATE_H_

//---------------------------------------------------------------------------
// Validation Routines (for use outside validate.cpp)

NTSTATUS
ValidateDataFormat(
    PKSDATAFORMAT pDataFormat
);

void
AddValidation(
    const GUID&         DesiredSet,
    PKSPROPERTY_ITEM    pItem
);

//---------------------------------------------------------------------------
// Local prototypes (implemented in validate.cpp)
//---------------------------------------------------------------------------

NTSTATUS
ValidateAudioDataFormats(
    PKSDATAFORMAT pDataFormat
);

NTSTATUS 
PcValidateConnectionState(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    IN OUT PKSSTATE pState
);

NTSTATUS
PcValidateAudioQuality(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    IN PLONG pQuality
);

NTSTATUS
PcValidateAudioMixLevelCaps(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    IN OUT PVOID pVoid
);

NTSTATUS
PcValidateDataFormat(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    PKSDATAFORMAT pDataFormat
);

NTSTATUS 
PcValidateDataIntersection(
    IN PIRP pIrp,
    IN PKSP_PIN pPin,
    PVOID pData
);


#endif

