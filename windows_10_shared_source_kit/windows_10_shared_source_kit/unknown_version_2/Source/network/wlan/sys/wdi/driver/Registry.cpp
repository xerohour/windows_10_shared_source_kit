/*++

Copyright (c) Microsoft Corporation.  All rights reserved.


Module Name:

    Registry.cpp

Abstract:

    Implementation for registry access functionality

Environment:

    Kernel mode

Revision History:

--*/

#include "precomp.hpp"
#include "Registry.hpp"
#include "Registry.tmh"
    
CRegistryHelper::CRegistryHelper(
    _In_ NDIS_HANDLE miniportAdapterHandle
    )
{
    m_ConfigurationHandle = NULL;
    m_MiniportAdapterHandle = miniportAdapterHandle;
}

CRegistryHelper::~CRegistryHelper()
{
    Close();
}

NDIS_STATUS CRegistryHelper::Open()
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    NDIS_CONFIGURATION_OBJECT   configObject;

    configObject.Header.Type = NDIS_OBJECT_TYPE_CONFIGURATION_OBJECT;
    configObject.Header.Revision = NDIS_CONFIGURATION_OBJECT_REVISION_1;
    configObject.Header.Size = sizeof( NDIS_CONFIGURATION_OBJECT );
    configObject.NdisHandle = m_MiniportAdapterHandle;
    configObject.Flags = 0;

    ndisStatus = NdisOpenConfigurationEx(
                    &configObject,
                    &m_ConfigurationHandle
                    );

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to open NDIS configuration handle. Error = 0x%08x", ndisStatus);
    }

    return ndisStatus;
}

VOID CRegistryHelper::Close()
{
    if (m_ConfigurationHandle)
    {
        NdisCloseConfiguration(m_ConfigurationHandle);
        m_ConfigurationHandle = NULL;
    }
}

NDIS_STATUS
CRegistryHelper::LoadPropertiesFromRegistry(
    _In_        CPropertyCache*         pPropertyCache,
    _In_        PWFC_REG_ENTRY          RegKeyList,
    _In_        ULONG                   NumKeys
    )
{
    NDIS_STATUS                 ndisStatus = NDIS_STATUS_SUCCESS;
    PWFC_REG_ENTRY              regEntry;
    ULONG                       i, valueRead, numValuesRead = 0;
    PNDIS_CONFIGURATION_PARAMETER parameter = NULL;

    if (m_ConfigurationHandle == NULL)
    {
        ndisStatus = Open();
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("Failed to open registry handle. Will initialize properties to defaults. Error = 0x%08x", ndisStatus);
            ndisStatus = NDIS_STATUS_SUCCESS;
            // Continue below
        }
    }

    for (i = 0; i < NumKeys; i++)
    {
        regEntry= &RegKeyList[i];

        //
        // Read the registry value if the hConfigurationHandle is valid.
        //
        if (m_ConfigurationHandle != NULL)
        {
            NT_ASSERT((regEntry->RegType == NdisParameterInteger) ||
                      (regEntry->RegType == NdisParameterHexInteger));

            //
            // Get the configuration value for a specific parameter.  Under NT the
            // parameters are all read in as DWORDs.
            //
            NdisReadConfiguration(&ndisStatus,
                &parameter,
                m_ConfigurationHandle,
                &regEntry->RegName,
                (enum _NDIS_PARAMETER_TYPE)regEntry->RegType
                );

        }
        else
        {
            //
            // Use defaults
            //
            ndisStatus = NDIS_STATUS_FAILURE;
        }

        if ((ndisStatus == NDIS_STATUS_SUCCESS) && (parameter != NULL))
        {
            if ((regEntry->RegType == NdisParameterInteger) || (regEntry->RegType == NdisParameterHexInteger))
            {
                if(parameter->ParameterData.IntegerData < regEntry->Min || 
                    parameter->ParameterData.IntegerData > regEntry->Max)
                {
                    WFCError("A bad value %d read from registry for property %d. Reverting to default value %d",
                                                parameter->ParameterData.IntegerData,
                                                regEntry->PropertyName,
                                                regEntry->Default
                                                );
                    valueRead = regEntry->Default;
                }
                else
                {
                    valueRead = parameter->ParameterData.IntegerData;
                    numValuesRead++;
                }
            }
            else
            {
                // We dont handle any other reads except for ints
                NT_ASSERT(FALSE);
                continue;
            }
        }
        else
        {
            WFCTrace("Unable to read from registry for property %d. Error = 0x%08x. Reverting to default value: %d\n",
                                        regEntry->PropertyName,
                                        ndisStatus,
                                        regEntry->Default
                                        );
            ndisStatus = NDIS_STATUS_SUCCESS;
            
            if ((regEntry->RegType == NdisParameterInteger) || (regEntry->RegType == NdisParameterHexInteger))
            {
                valueRead = regEntry->Default;
            }
            else
            {
                // We dont handle defaults for any type other than ints
                NT_ASSERT(FALSE);
                continue;
            }
        }

        //
        // Moving the registry values in the property store
        //
        switch(regEntry->PropertyType)
        {
            case WfcPropertyTypeUchar:
                ndisStatus = pPropertyCache->SetPropertyUchar(
                    regEntry->PropertyName, 
                    (UCHAR) valueRead);
                break;
            case WfcPropertyTypeULong:
                ndisStatus = pPropertyCache->SetPropertyULong(
                    regEntry->PropertyName, 
                    (ULONG) valueRead);
                break;
            case WfcPropertyTypeBoolean:
                ndisStatus = pPropertyCache->SetPropertyBoolean(
                    regEntry->PropertyName, 
                    valueRead != 0 ? TRUE : FALSE);
                break;
            default:
                WFCError("Unsupported property type %d for property %d", regEntry->PropertyType, regEntry->PropertyName);
                ndisStatus = NDIS_STATUS_INVALID_DATA;
                break;
        }

        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("Failed to save property data from registry. Error = 0x%08x", ndisStatus);
            // This is the only fatal error in this function
            goto exit;
        }
    }

exit:

    return ndisStatus;
}

NDIS_STATUS
CRegistryHelper::SavePropertyToRegistry(
    _In_        WFC_PROPERTY_NAME       PropertyName,
    _In_        CPropertyCache*         pPropertyCache,
    _In_        PWFC_REG_ENTRY          RegKeyTable,
    _In_        ULONG                   NumKeyTableEntries
    )
{
    NDIS_STATUS status = NDIS_STATUS_FAILURE;
    WFC_REG_ENTRY regEntry;
    
    ULONG i;
    
    for ( i=0; i < NumKeyTableEntries; i++ ) 
    {
        if ( RegKeyTable[i].PropertyName == PropertyName )
        {
            break;
        }
    }

    if ( i < NumKeyTableEntries )
    {
        //
        // Found our entry
        //
        regEntry = RegKeyTable[i];    // make a copy so we can write
        status = pPropertyCache->GetPropertyULong(PropertyName, &regEntry.Default);
        if (status == NDIS_STATUS_SUCCESS)
        {
            status = WriteAdapterRegistryEntry( &regEntry );
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCError("Failed to write data into registry. Error = 0x%08x", status);
            }
        }
        else
        {
            WFCError("Failed to obtain value from property cache for writing to registry. Error = 0x%08x", status);
        }
    }
    return status;
}

NDIS_STATUS
CRegistryHelper::WriteAdapterRegistryEntry(
     _In_ PWFC_REG_ENTRY RegKey )
/*++

    Persist an Adapter key info to the registry.
    This call does not load the value to the property cache.
    Also, this call does not guard againt reentance. 
    If callers manage a key from serialized jobs, there will be no reentrance problem.

    NDIS_STRING  RegName;    // variable name text
    UCHAR            RegType;      // Type in the registry = NdisParameterInteger/NdisParameterHexInteger
    WFC_PROPERTY_TYPE        PropertyType;           // ULONG/BOOLEAN
    WFC_PROPERTY_NAME       PropertyName;           //  ununsed for Writing registry
    UINT                Default;       // value to write
    UINT                Min;            // minimum value allowed
    UINT                Max;           // maximum value allowed

--*/
{
    NDIS_STATUS status = NTSTATUS2NDISSTATUS (STATUS_INVALID_PARAMETER);

    //
    // check
    //
    if ( RegKey->RegType != NdisParameterInteger &&
        RegKey->RegType != NdisParameterHexInteger )
    {
        WFCError( "Unsupported type to write registry " );
        goto Exit;
    }

    if ( RegKey->Default > RegKey->Max ||
        RegKey->Default < RegKey->Min )
    {
        WFCError( "OutofRange value to write registry " );
        goto Exit;
    }


    if ( m_ConfigurationHandle == NULL )
    {
        status = Open();
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCError("Failed to open registry handle to write. Error = 0x%08x", status);
            goto Exit;
        }
    }

    NDIS_CONFIGURATION_PARAMETER parameter;

    parameter.ParameterType = NdisParameterInteger; // this is always a dword for waht we support
    parameter.ParameterData.IntegerData = RegKey->Default;
    
    NdisWriteConfiguration(
        &status,
        m_ConfigurationHandle,
        &RegKey->RegName,
        &parameter );

Exit:
    return status;
}


NDIS_STATUS
CRegistryHelper::ReadAdapterRegistryBinaries(
    _In_ PCWSTR KeyName,
    _Inout_ ULONG*  pCb,
    _Out_writes_to_(*pCb, *pCb) BYTE * pByte )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    UNICODE_STRING ucKeyName;

    if ( m_ConfigurationHandle == NULL )
    {
        status = Open();
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCError("Failed to open registry handle to write. Error = 0x%08x", status);
            goto exit;
        }
    }

    PNDIS_CONFIGURATION_PARAMETER pParameter = NULL;

    //parameter.ParameterType = NdisParameterBinary;
    //parameter.ParameterData.BinaryData.Length = *pCb;
    //parameter.ParameterData.BinaryData.Buffer = (PVOID)pByte;

    RtlInitUnicodeString( &ucKeyName, KeyName );

    NdisReadConfiguration(&status,
            &pParameter,
            m_ConfigurationHandle,
            &ucKeyName,
            NdisParameterBinary);

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError( "ReadAdapterRegistryBinaries key=%S Error=0x%x\n", KeyName, status);
        goto exit;
    }

    if ( *pCb > pParameter->ParameterData.BinaryData.Length )
    {
        *pCb = pParameter->ParameterData.BinaryData.Length;
    }

    RtlCopyMemory( pByte, pParameter->ParameterData.BinaryData.Buffer, *pCb );

exit:
    return status;
}



NDIS_STATUS
CRegistryHelper::WriteAdapterRegistryBinaries(
    _In_ PCWSTR KeyName,
    _In_ ULONG Cb,
    _In_reads_( Cb ) BYTE * pByte )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    UNICODE_STRING ucKeyName;

    if ( m_ConfigurationHandle == NULL )
    {
        status = Open();
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCError("Failed to open registry handle to write. Error = 0x%08x", status);
            goto exit;
        }
    }

    NDIS_CONFIGURATION_PARAMETER parameter;

    parameter.ParameterType = NdisParameterBinary;
    parameter.ParameterData.BinaryData.Length = (USHORT)Cb;
    parameter.ParameterData.BinaryData.Buffer = (PVOID)pByte;

    RtlInitUnicodeString( &ucKeyName, KeyName );
    NdisWriteConfiguration( &status,
                        m_ConfigurationHandle,
                        &ucKeyName,
                        &parameter );

exit:
    return status;
}


NDIS_STATUS
CRegistryHelper::ReadAdapterRegistryDword(
    _In_    NDIS_STRING* pucKeyName,
    _Inout_ ULONG * pUlong )
/*++

    When success, *pUlong has the value from registry.
    Otherwise, the origianl value is kept uncahnged.

--*/
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    if ( m_ConfigurationHandle == NULL )
    {
        status = Open();
        if (status != NDIS_STATUS_SUCCESS)
        {
            WFCError("Failed to open registry handle to read. Error = 0x%08x", status);
            goto exit;
        }
    }

    PNDIS_CONFIGURATION_PARAMETER pParameter = NULL;

    NdisReadConfiguration(&status,
            &pParameter,
            m_ConfigurationHandle,
            pucKeyName,
            NdisParameterHexInteger );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        WFCError( "ReadAdapterRegistryIntegerKey=%S Error=0x%x\n", pucKeyName->Buffer, status);
        goto exit;
    }
    else
    {
        *pUlong = pParameter->ParameterData.IntegerData;
    }

exit:
    return status;
}

