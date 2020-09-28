/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Registry.hpp

Abstract:

    Header file for Registry access functions

Environment:

    Kernel mode

Revision History:

--*/
#pragma once

/** Structure used to read information from the registry */
typedef struct _WFC_REG_ENTRY
{
    NDIS_STRING                 RegName;                // variable name text
    UCHAR                       RegType;                // Type in the registry = NdisParameterInteger/NdisParameterHexInteger
    WFC_PROPERTY_TYPE           PropertyType;           // ULONG/BOOLEAN
    WFC_PROPERTY_NAME           PropertyName;           // 
    ULONG                       Default;                // default value to use
    ULONG                       Min;                    // minimum value allowed
    ULONG                       Max;                    // maximum value allowed
} WFC_REG_ENTRY, *PWFC_REG_ENTRY;

class CRegistryHelper
{
private:
    NDIS_HANDLE m_MiniportAdapterHandle;
    NDIS_HANDLE m_ConfigurationHandle;
    
private:
    NDIS_STATUS Open();

    VOID Close();
    
public:
    CRegistryHelper(
        _In_ NDIS_HANDLE MiniportAdapterHandle
        );

    ~CRegistryHelper();
    
    NDIS_STATUS LoadPropertiesFromRegistry(
        _In_ CPropertyCache* pPropertyCache,
        _In_ PWFC_REG_ENTRY RegKeyList,
        _In_ ULONG NumKeys
        );

    NDIS_STATUS SavePropertyToRegistry(
        _In_        WFC_PROPERTY_NAME       PropertyName,
        _In_        CPropertyCache*         pPropertyCache,
        _In_        PWFC_REG_ENTRY          RegKeyTable,
        _In_        ULONG                   NumKeyTableEntries
        );

    NDIS_STATUS WriteAdapterRegistryEntry(
        _In_ PWFC_REG_ENTRY RegKey );

    NDIS_STATUS ReadAdapterRegistryBinaries(
            _In_ PCWSTR KeyName,
            _Inout_ ULONG*  pCb,
            _Out_writes_to_( *pCb, *pCb ) BYTE * pByte );

    NDIS_STATUS WriteAdapterRegistryBinaries(
        _In_ PCWSTR KeyName,
        _In_ ULONG Cb,
        _In_reads_( Cb ) BYTE * pByte );

    NDIS_STATUS ReadAdapterRegistryDword(
        _In_    NDIS_STRING *pucKeyName,
        _Inout_ ULONG * pUlong );

};
