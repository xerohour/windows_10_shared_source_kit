#include "precomp.hpp"
#include "PropertyCache.hpp"
#include "PropertyCache.tmh"

_WFC_VARIABLE_BUFFER_PROPERTY::_WFC_VARIABLE_BUFFER_PROPERTY() :
    m_PropertyBuffer(NULL),
    m_CurrentBufferLength(0),
    m_MaxBufferLength(0),
    m_CurrentElementCount(0) { }

void _WFC_VARIABLE_BUFFER_PROPERTY::Cleanup()
{
    if ( m_PropertyBuffer != NULL )
    {
        delete [] m_PropertyBuffer;
        m_PropertyBuffer = NULL;
    }
    
    m_CurrentBufferLength = 0;
    m_MaxBufferLength = 0;
    m_CurrentElementCount = 0;
}

_WFC_VARIABLE_BUFFER_PROPERTY::~_WFC_VARIABLE_BUFFER_PROPERTY()
{
    Cleanup();
}

NDIS_STATUS _WFC_VARIABLE_BUFFER_PROPERTY::CopyBuffer(
    _In_ ULONG BufferLength,
    _In_ UINT16 ElementCount,
    _In_reads_bytes_(BufferLength) BYTE* Buffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    
    TraceEntry();

    if ( Buffer == NULL || BufferLength == 0 )
    {
        // Not deleting the buffer here
        m_CurrentBufferLength = 0;
        m_CurrentElementCount = 0;
    }
    else
    {
        //
        // First call to copy buffer, or the new buffer is too big to
        // fit into the existing cached buffer.
        //
        if ( m_PropertyBuffer == NULL ||
             BufferLength > m_MaxBufferLength )
        {
            BYTE* newBuffer = new BYTE[BufferLength];
            if ( newBuffer == NULL )
            {
                status = NDIS_STATUS_RESOURCES;
                goto exit;
            }

            m_MaxBufferLength = BufferLength;

            if ( m_PropertyBuffer != NULL )
            {
                delete [] m_PropertyBuffer;
            }

            m_PropertyBuffer = newBuffer;
        }

        RtlCopyMemory(
            m_PropertyBuffer,
            Buffer,
            BufferLength );

        m_CurrentBufferLength = BufferLength;
        m_CurrentElementCount = ElementCount;
    }    

exit:
    TraceExit(status);
    return status;
}

_WFC_PROPERTY_ENTRY::_WFC_PROPERTY_ENTRY() :
    Name(WfcPropertyNameInvalid),
    Type(WfcPropertyTypeInvalid),
    IsPopulated(false),
    ULongProperty(0),
    BufferProperty(),
    pDefaultEntry(NULL) { }

_WFC_PROPERTY_ENTRY::~_WFC_PROPERTY_ENTRY()
{
    CleanUp();
}

void _WFC_PROPERTY_ENTRY::CleanUp()
{
    CPropertyCache *PropertyCacheListCurrent, *PropertyCacheListNext;

    if (Type == WfcPropertyTypePropertyCacheList)
    {
        PropertyCacheListCurrent = PropertyCacheList;
        while (PropertyCacheListCurrent != NULL)
        {
            PropertyCacheListNext = PropertyCacheListCurrent->Next;
            delete PropertyCacheListCurrent;

            PropertyCacheListCurrent = PropertyCacheListNext;
        }        
    }
}


NDIS_STATUS _WFC_PROPERTY_ENTRY::InitializeFromDefault(
    _In_ _WFC_PROPERTY_ENTRY* pDefaultEntry )
{
    NT_ASSERT(pDefaultEntry != NULL);

    this->Name = pDefaultEntry->Name;
    this->Type = pDefaultEntry->Type;
    this->IsPopulated = false;
    this->pDefaultEntry = pDefaultEntry;

    return NDIS_STATUS_SUCCESS;
}

CPropertyCache::CPropertyCache()
    :    m_PropertyTable (NULL),
         Next(NULL)
{
}

CPropertyCache::~CPropertyCache()
{
    if (m_PropertyTable)
    {
        delete [] m_PropertyTable;
    }
}

NDIS_STATUS CPropertyCache::Initialize(
    _In_ UINT32 NumOfProperties,
    _In_opt_ CPropertyCache* pDefaultValues )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY *PropertyTable;

    PropertyTable = new WFC_PROPERTY_ENTRY[NumOfProperties];
    if (PropertyTable == NULL)
    {
        status = NDIS_STATUS_RESOURCES;
        WFCError("Out of memory");
        goto exit;
    }
    RtlZeroMemory (PropertyTable, NumOfProperties * sizeof(WFC_PROPERTY_ENTRY));

    if ( pDefaultValues != NULL )
    {
        if ( NumOfProperties != pDefaultValues->GetNumOfProperties() )
        {
            NT_ASSERT(FALSE);
            status = NDIS_STATUS_INVALID_PARAMETER;
            WFCError("Default property count doesn't match to expected value");
            goto exit;
        }

        for( UINT32 i = 0; i < NumOfProperties; ++i )
        {
            WFC_PROPERTY_ENTRY* pCurrentDefault = &pDefaultValues->m_PropertyTable[i];
            WFC_PROPERTY_ENTRY* pCurrentEntry = &PropertyTable[i];

            if ( !pCurrentDefault->IsPopulated )
            {
                WFCInfo("Default entry for %x was not populated, skipping default entry initialization.", i);
                continue;
            }

            status = pCurrentEntry->InitializeFromDefault(
                pCurrentDefault );

            if ( status != NDIS_STATUS_SUCCESS )
            {
                WFCError("Couldn't initialize entry from default values.");
                goto exit;
            }
        }
    }

    m_PropertyTable = PropertyTable;
    PropertyTable = NULL;

    m_PropertyNameMax = NumOfProperties;

exit:
    if (PropertyTable)
    {
        delete [] PropertyTable;
    }

    return status;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CPropertyCache::GetPropertyBoolean(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _Out_ BOOLEAN* pPropertyValue )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;

    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypeBoolean,
        true,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
        
    *pPropertyValue = pPropertyEntry->BooleanProperty;

exit:
    return status;
}

BOOLEAN CPropertyCache::GetPropertyBooleanOrDefault(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ BOOLEAN bDefaultValue)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    BOOLEAN CacheValue;

    status = GetPropertyBoolean(PropertyName,
        &CacheValue);
    if ( status != NDIS_STATUS_SUCCESS)
    {
        WFCInfo("Failed to get value for %x.  Error = 0x%08x. Using default value", PropertyName, status);    
        CacheValue = bDefaultValue;
    }

    return CacheValue;
}

NDIS_STATUS CPropertyCache::SetPropertyBoolean(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ BOOLEAN PropertyValue )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;

    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypeBoolean,
        false,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
    
    pPropertyEntry->Name = PropertyName;
    pPropertyEntry->Type = WfcPropertyTypeBoolean;       
    pPropertyEntry->BooleanProperty = PropertyValue;
    pPropertyEntry->IsPopulated = true;

exit:
    return status;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CPropertyCache::GetPropertyUchar(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _Out_ UCHAR* pPropertyValue )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;

    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypeUchar,
        true,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
        
    *pPropertyValue = pPropertyEntry->UcharProperty;

exit:
    return status;
}

NDIS_STATUS CPropertyCache::SetPropertyUchar(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ UCHAR PropertyValue )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;

    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypeUchar,
        false,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
        
    pPropertyEntry->Name = PropertyName;
    pPropertyEntry->Type = WfcPropertyTypeUchar;       
    pPropertyEntry->UcharProperty = PropertyValue;
    pPropertyEntry->IsPopulated = true;

exit:
    return status;
}

UCHAR CPropertyCache::GetPropertyUcharOrDefault(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ UCHAR bDefaultValue)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    UCHAR CacheValue;

    status = GetPropertyUchar(PropertyName,
        &CacheValue);
    if ( status != NDIS_STATUS_SUCCESS)
    {
        WFCInfo("Failed to get value for %x.  Error = 0x%08x. Using default value", PropertyName, status);    
        CacheValue = bDefaultValue;
    }

    return CacheValue;
}


__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CPropertyCache::GetPropertyULong(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _Out_ ULONG* pPropertyValue )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;

    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypeULong,
        true,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
        
    *pPropertyValue = pPropertyEntry->ULongProperty;

exit:
    return status;
}


NDIS_STATUS CPropertyCache::SetPropertyULong(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ ULONG PropertyValue )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;

    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypeULong,
        false,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
        
    pPropertyEntry->Name = PropertyName;
    pPropertyEntry->Type = WfcPropertyTypeULong;       
    pPropertyEntry->ULongProperty = PropertyValue;
    pPropertyEntry->IsPopulated = true;

exit:
    return status;
}

ULONG CPropertyCache::GetPropertyULongOrDefault(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ ULONG bDefaultValue)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    ULONG CacheValue;

    status = GetPropertyULong(PropertyName,
        &CacheValue);
    if ( status != NDIS_STATUS_SUCCESS)
    {
        WFCInfo("Failed to get value for %x.  Error = 0x%08x. Using default value", PropertyName, status);    
        CacheValue = bDefaultValue;
    }

    return CacheValue;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CPropertyCache::GetPropertyBuffer(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _Out_ ULONG* pBufferLength,
    _Outptr_result_bytebuffer_(*pBufferLength) BYTE** pBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;
        
    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypeVariableBuffer,
        true,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
        
    *pBufferLength = pPropertyEntry->BufferProperty.m_CurrentBufferLength;
    *pBuffer = pPropertyEntry->BufferProperty.m_PropertyBuffer;

exit:
    return status;
}

NDIS_STATUS CPropertyCache::SetPropertyBuffer(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ ULONG BufferLength,
    _In_reads_bytes_(BufferLength) BYTE* Buffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;

    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypeVariableBuffer,
        false,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
     
    status = pPropertyEntry->BufferProperty.CopyBuffer(
        BufferLength,
        (UINT16)BufferLength,
        Buffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    pPropertyEntry->Name = PropertyName;
    pPropertyEntry->Type = WfcPropertyTypeVariableBuffer;       
    pPropertyEntry->IsPopulated = true;

exit:
    return status;
}


__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CPropertyCache::GetPropertyList(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _Out_ ULONG* pBufferLength,
    _Out_ UINT16* pElementCount,
    _Outptr_result_bytebuffer_(*pBufferLength) BYTE** pBuffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;
        
    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypeList,
        true,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
        
    *pBufferLength = pPropertyEntry->BufferProperty.m_CurrentBufferLength;
    *pBuffer = pPropertyEntry->BufferProperty.m_PropertyBuffer;
    *pElementCount = pPropertyEntry->BufferProperty.m_CurrentElementCount;

exit:
    return status;
}

NDIS_STATUS CPropertyCache::SetPropertyList(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ ULONG BufferLength,
    _In_ UINT16 ElementCount,
    _In_reads_bytes_(BufferLength) BYTE* Buffer )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;

    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypeList,
        false,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
     
    status = pPropertyEntry->BufferProperty.CopyBuffer(
        BufferLength,
        ElementCount,
        Buffer );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }

    pPropertyEntry->Name = PropertyName;
    pPropertyEntry->Type = WfcPropertyTypeList;       
    pPropertyEntry->IsPopulated = true;

exit:
    return status;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CPropertyCache::GetPropertyEntryForPropertyName(
    _In_ WFC_PROPERTY_NAME     PropertyName,
    _In_ WFC_PROPERTY_TYPE     ExpectedPropertyType,
    _In_ bool                  RequirePopulatedEntry,
    _Out_ WFC_PROPERTY_ENTRY **ppPropertyEntry )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* toReturn = NULL;

    if ( ppPropertyEntry == NULL ||         
         (ULONG)PropertyName >= m_PropertyNameMax )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("Bad parameter %d to %!FUNC!", (ULONG)PropertyName);
        goto exit;
    }

    toReturn = &m_PropertyTable[PropertyName];

    if ( RequirePopulatedEntry &&
         !toReturn->IsPopulated )
    {
        toReturn = toReturn->pDefaultEntry;
        if ( toReturn == NULL )
        {
            WFCInfo("Property entry %d was not populated, and default was not available.", PropertyName);
            status = NDIS_STATUS_INVALID_STATE;
            goto exit;
        }
    }

    /* #if 0 below - temporarily disable expected property type checking until initialization of defaults is implemented. */
#if 0
    if ( toReturn->Type != ExpectedPropertyType )
    {
        NT_ASSERT(FALSE);
        status = NDIS_STATUS_INVALID_STATE;
        WFCError(
            "Was expecting property type %x, but instead got %x for property %x",
            ExpectedPropertyType,
            toReturn->Type,
            PropertyName );
        goto exit;
    }
#else
    UNREFERENCED_PARAMETER(ExpectedPropertyType);
#endif

    *ppPropertyEntry = toReturn;

exit:
    TraceExit(status);
    return status;
}


NDIS_STATUS CPropertyCache::LoadProperty(
    CPropertyCache *propertyCache,
    _In_ UINT32 bufferElementSize,
    _In_reads_bytes_(bufferElementSize) BYTE* pBuffer,    
    _In_ const WFC_PROPERTY_LOAD_INFO * pPropertiesPerElement, 
    _In_ UINT32 maxPropertiesPerElement)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    BYTE* sourcePtr;
    BYTE* variableBufferPtr;    
    BYTE* variableBufferElementsCountPtr;    
    INT32 variableBufferElementsCount;
    INT32 variableBufferSize;
    UINT32 availableBufferElementSize;

    if (pPropertiesPerElement->PropertyName >= maxPropertiesPerElement)
    {
        ndisStatus = NDIS_STATUS_INVALID_DATA;
        goto exit;
    } 
  
    if (pPropertiesPerElement->WfcPropertyType == WfcPropertyTypeVariableBufferPtr ||
        pPropertiesPerElement->WfcPropertyType == WfcPropertyTypeVariableBuffer)
    {    
        //
        // get the address to a field that has the value of the buffer elements count
        //
        if (pPropertiesPerElement->ElementCountSourceOffset + pPropertiesPerElement->ElementCountSourceSize > bufferElementSize ||
            pPropertiesPerElement->ElementCountSourceSize  > sizeof(UINT32))
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            goto exit;
        }   
        variableBufferElementsCountPtr = pBuffer + pPropertiesPerElement->ElementCountSourceOffset;           

        variableBufferElementsCount = 0;
        RtlCopyMemory (&variableBufferElementsCount, variableBufferElementsCountPtr, pPropertiesPerElement->ElementCountSourceSize);

        //
        // size in bytes of the variable length buffer
        //
        variableBufferSize = variableBufferElementsCount * pPropertiesPerElement->ElementSourceSize;

        //
        // get the buffer pointer
        //
        if (pPropertiesPerElement->WfcPropertyType == WfcPropertyTypeVariableBufferPtr)
        {
            //
            // get the address of the pointer to a variable size buffer
            //
            if (pPropertiesPerElement->ElementSourceOffset + sizeof(ULONG_PTR) > bufferElementSize)
            {
                ndisStatus = NDIS_STATUS_INVALID_LENGTH;
                goto exit;
            }
            sourcePtr = pBuffer + pPropertiesPerElement->ElementSourceOffset;

            //
            // copy pointer value to the variable buffer 
            //
            RtlCopyMemory (&variableBufferPtr, sourcePtr, sizeof(ULONG_PTR));
        }
        else
        {             
            //
            // get the address of the pointer to a variable size buffer inside in the structure
            //
            if (pPropertiesPerElement->ElementSourceOffset + variableBufferSize > bufferElementSize)
            {
                ndisStatus = NDIS_STATUS_INVALID_LENGTH;
                goto exit;
            }
            sourcePtr = pBuffer + pPropertiesPerElement->ElementSourceOffset;

            variableBufferPtr = sourcePtr;
        }
  
        //
        // set the property
        //
        ndisStatus = propertyCache->SetPropertyBuffer ((WFC_PROPERTY_NAME)pPropertiesPerElement->PropertyName,
                                                        variableBufferSize,
                                                        variableBufferPtr);
    }   
    else
    {
        if ( ! NT_SUCCESS(
                 RtlUInt32Add(
                     pPropertiesPerElement->ElementSourceOffset,
                     pPropertiesPerElement->ElementSourceSize,
                     &availableBufferElementSize ) ) )
        {
            ndisStatus = NDIS_STATUS_BUFFER_OVERFLOW;
            WFCError(
                "Size overflow");
            goto exit;
        }

        if (availableBufferElementSize > bufferElementSize)
        {
            ndisStatus = NDIS_STATUS_INVALID_LENGTH;
            goto exit;
        }

        sourcePtr = pBuffer + pPropertiesPerElement->ElementSourceOffset;

        switch(pPropertiesPerElement->WfcPropertyType)
        {
            case WfcPropertyTypeULong:
                if (pPropertiesPerElement->ElementSourceSize != sizeof(ULONG))
                {
                    ndisStatus = NDIS_STATUS_INVALID_DATA;
                    goto exit;
                }
                ndisStatus = propertyCache->SetPropertyULong ((WFC_PROPERTY_NAME)pPropertiesPerElement->PropertyName,
                                                            *((ULONG*)sourcePtr));
                break;

            case WfcPropertyTypeBoolean:
                if (pPropertiesPerElement->ElementSourceSize != sizeof(BOOLEAN))
                {
                    ndisStatus = NDIS_STATUS_INVALID_DATA;
                    goto exit;
                }
                ndisStatus = propertyCache->SetPropertyBoolean ((WFC_PROPERTY_NAME)pPropertiesPerElement->PropertyName,
                                                            *((BOOLEAN*)sourcePtr));
                break;

            case WfcPropertyTypeUchar:
                if (pPropertiesPerElement->ElementSourceSize != sizeof(UCHAR))
                {
                    ndisStatus = NDIS_STATUS_INVALID_DATA;
                    goto exit;
                }
                ndisStatus = propertyCache->SetPropertyUchar ((WFC_PROPERTY_NAME)pPropertiesPerElement->PropertyName,
                                                            *((UCHAR*)sourcePtr));
                break;

            case WfcPropertyTypePropertyCacheList:
            default:
                ndisStatus = NDIS_STATUS_INVALID_DATA;
                goto exit;
        }

        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("SetProperty failed");
            goto exit;
        } 
    }
        
exit:

    return ndisStatus;
}

NDIS_STATUS CPropertyCache::LoadProperties(
    CPropertyCache *propertyCache,
    _In_ UINT32 bufferElementSize,
    _In_reads_bytes_(bufferElementSize) BYTE* pBuffer,    
    _In_reads_(propertiesCountPerElement) const WFC_PROPERTY_LOAD_INFO * pPropertiesPerElement, 
    _In_range_(1, MAXULONG32) const ULONG propertiesCountPerElement,
    _In_ UINT32 maxPropertiesPerElement)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    for (ULONG i = 0; i < propertiesCountPerElement; ++i)
    {
        ndisStatus = LoadProperty(propertyCache,
                                bufferElementSize,
                                pBuffer,    
                                &pPropertiesPerElement[i],
                                maxPropertiesPerElement);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("SetProperty failed");
            goto exit;
        } 
    }
    
exit:

    return ndisStatus;
}

NDIS_STATUS CPropertyCache::LoadPropertyListProperty(
    _In_ WFC_PROPERTY_NAME enumarablePropertyName,
    _In_ UINT32 bufferElementsCount,
    _In_ UINT32 bufferElementSize,
    _In_reads_bytes_(bufferElementsCount * bufferElementSize) BYTE* pBuffer,    
    _In_reads_(propertiesCountPerElement) const WFC_PROPERTY_LOAD_INFO * pPropertiesPerElement, 
    _In_range_(1, MAXULONG32) const ULONG propertiesCountPerElement,
    _In_ UINT32 maxPropertiesPerElement)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;

    CPropertyCache *propertyCacheFirst = NULL;
    CPropertyCache *propertyCacheLast = NULL;
    CPropertyCache *propertyCacheCurrent;

    BYTE* pBufferCurrent;

    ndisStatus = GetPropertyEntryForPropertyName (enumarablePropertyName,
                                                WfcPropertyTypePropertyCacheList,
                                                false,
                                                &pPropertyEntry );
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("GetPropertyEntryForPropertyName failed");
        goto exit;
    }

    pBufferCurrent = pBuffer;

    for (ULONG i = 0; i < bufferElementsCount; ++i)
    {
        propertyCacheCurrent = new CPropertyCache();
        if (propertyCacheCurrent == NULL)
        {
            ndisStatus = NDIS_STATUS_RESOURCES;
            WFCError("Out of memory");
            goto exit;
        }

        if (propertyCacheFirst == NULL)
        {
            propertyCacheFirst = propertyCacheCurrent;
        }
        else
        {
            propertyCacheLast->Next = propertyCacheCurrent;
        }
        propertyCacheLast = propertyCacheCurrent;
        
        ndisStatus = propertyCacheCurrent->Initialize(maxPropertiesPerElement,
                                                    NULL);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("Initialize failed");
            goto exit;
        }

        ndisStatus = LoadProperties(propertyCacheCurrent,
                                bufferElementSize,
                                pBufferCurrent,    
                                pPropertiesPerElement, 
                                propertiesCountPerElement,
                                maxPropertiesPerElement);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("SetProperties failed");
            goto exit;
        }

        pBufferCurrent += bufferElementSize;
    }

 
    // clean up the old list if exists
    pPropertyEntry->CleanUp();

    //
    // TODO: this is needed because the default entry initialization is not implemented yet, and the value is WfcPropertyTypeInvalid
    //
    pPropertyEntry->Type = WfcPropertyTypePropertyCacheList;

    pPropertyEntry->PropertyCacheList = propertyCacheFirst;
    pPropertyEntry->IsPopulated = true;

exit:

    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        // just reuse the temproary pointers to free all allocated propertyCache objects
        propertyCacheCurrent = propertyCacheFirst;
        while (propertyCacheCurrent != NULL)
        {
            propertyCacheLast = propertyCacheCurrent->Next;
            delete propertyCacheCurrent;

            propertyCacheCurrent = propertyCacheLast;
        }   

    }

    return ndisStatus;
}



NDIS_STATUS CPropertyCache::SetPropertyPropertyCacheList(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _In_ CPropertyCache * pPropertyValue )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;

    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypePropertyCacheList,
        false,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
        
    pPropertyEntry->Name = PropertyName;
    pPropertyEntry->Type = WfcPropertyTypePropertyCacheList;
    pPropertyEntry->PropertyCacheList = pPropertyValue;
    pPropertyEntry->IsPopulated = true;

exit:
    return status;
}


__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CPropertyCache::GetPropertyPropertyCacheList(
    _In_ WFC_PROPERTY_NAME PropertyName,
    _Out_ CPropertyCache** ppPropertyValue )
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    WFC_PROPERTY_ENTRY* pPropertyEntry;

    status = GetPropertyEntryForPropertyName(
        PropertyName,
        WfcPropertyTypePropertyCacheList,
        true,
        &pPropertyEntry );

    if ( status != NDIS_STATUS_SUCCESS )
    {
        goto exit;
    }
        
    *ppPropertyValue = pPropertyEntry->PropertyCacheList;

exit:
    return status;
}

CAdapterPropertyCache::CAdapterPropertyCache()
{
}

NDIS_STATUS CAdapterPropertyCache::Initialize()    
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    ndisStatus = CPropertyCache::Initialize (WfcAdapterPropertyNameMax, NULL);
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't initialize property cache.");
        goto exit;
    }

    // Set initial values for various properties
    ndisStatus = SetPropertyULong(WfcAdapterPropertyMediaStreamingRefcount, 0);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong(WfcAdapterPropertyCurrentListenState, 0);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcAdapterPropertyScanModePassiveOnly, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcAdapterHardwareRadioOff, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcAdapterSoftwareRadioOff, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

exit:
    return ndisStatus;
}

NDIS_STATUS CAdapterPropertyCache::SetFirmwareCapabilitiesProperties(
    _In_ WDI_GET_ADAPTER_CAPABILITIES_PARAMETERS* adapterCapabilities)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    ULONG numOfBandInfos;
    PWFC_BAND_INFO pBandInfo = NULL;

    ULONG numOfPhyInfos;
    PWFC_PHY_INFO pPhyInfo = NULL;

    CPropertyCache *propertyCache;
    ULONG currentPhyID = 0;
    ULONG opModeCapability = 0;
    ULONG *pPhyIDMap = NULL;

    ndisStatus = SetPropertyULong( 
            (WFC_PROPERTY_NAME)WfcAdapterPropertyMaxCommandSize, 
            adapterCapabilities->CommunicationAttributes.CommunicationCapabilities.MaxCommandSize);
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't populate MaxCommandSize property");
        goto exit;
    }

    ndisStatus = SetPropertyULong( 
            (WFC_PROPERTY_NAME)WfcAdapterPropertyMaxMulticastListSize, 
            adapterCapabilities->InterfaceAttributes.InterfaceCapabilities.MaxMultiCastListSize);
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't populate MaxMulticastListSize property");
        goto exit;
    }

    ndisStatus = SetPropertyULong(
        (WFC_PROPERTY_NAME)WfcAdapterPropertyBackFillSize,
        adapterCapabilities->InterfaceAttributes.InterfaceCapabilities.BackFillSize);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate BackFillSize property");
        goto exit;
    }

    ndisStatus = SetPropertyBoolean(
            (WFC_PROPERTY_NAME)WfcAdapterPropertyActionFramesSupported, 
            adapterCapabilities->InterfaceAttributes.InterfaceCapabilities.ActionFramesSupported);
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't populate ActionFramesSupported property");
        goto exit;
    }

    ndisStatus = SetPropertyBoolean(
        (WFC_PROPERTY_NAME)WfcAdapterPropertyMACRandomizationSupported,
        adapterCapabilities->InterfaceAttributes.InterfaceCapabilities.MACAddressRandomization);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate MACRandomizationSupported property");
        goto exit;
    }

    ndisStatus = SetPropertyBoolean(
        (WFC_PROPERTY_NAME)WfcAdapterPropertyNonWdiOidsSupported,
        adapterCapabilities->InterfaceAttributes.InterfaceCapabilities.SupportsNonWdiOidRequests);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate NonWdiOidsSupported property");
        goto exit;
    }
    //
    // set Infra Supported Ucast Algo Pairs properties
    //
    opModeCapability = DOT11_OPERATION_MODE_EXTENSIBLE_STATION;
    if (adapterCapabilities->Optional.P2PAttributes_IsPresent)
    {
        opModeCapability |= DOT11_OPERATION_MODE_WFD_DEVICE;

        if (adapterCapabilities->P2PAttributes.P2PCapabilities.ConcurrentGOCount > 0)
        {
            opModeCapability |= DOT11_OPERATION_MODE_WFD_GROUP_OWNER;
        }
        if (adapterCapabilities->P2PAttributes.P2PCapabilities.ConcurrentClientCount > 0)
        {
            opModeCapability |= DOT11_OPERATION_MODE_WFD_CLIENT;
        }

        WFCInfo ("SetFirmwareCapabilitiesProperties: NumCGOs=<%d>, NumCCClients=<%d>, WPSVers=<%d>, bSvcDisc=<%x>, bClntDisc=<%x>, bInfraMgmt=<%x>, GOSize=<%d>\n",
            adapterCapabilities->P2PAttributes.P2PCapabilities.ConcurrentGOCount,
            adapterCapabilities->P2PAttributes.P2PCapabilities.ConcurrentClientCount,
            adapterCapabilities->P2PAttributes.P2PCapabilities.WPSVersionSupport,
            adapterCapabilities->P2PAttributes.P2PCapabilities.ServiceDiscoverySupported,
            adapterCapabilities->P2PAttributes.P2PCapabilities.ClientDiscoverabilitySupported,
            adapterCapabilities->P2PAttributes.P2PCapabilities.InfrastructureManagementSupported,
            adapterCapabilities->P2PAttributes.P2PCapabilities.MaxSecondaryAdapterTypeListSize);

        WFCInfo ("SetFirmwareCapabilitiesProperties: DeviceAddress=%!MACADDR!\n",
            adapterCapabilities->P2PAttributes.P2PCapabilities.DeviceAddress);

        WFCInfo ("SetFirmwareCapabilitiesProperties: uInterfaceAddressListCount=<%d>\n",
            adapterCapabilities->P2PAttributes.InterfaceAddressList.ElementCount);
        if (adapterCapabilities->P2PAttributes.InterfaceAddressList.ElementCount)
        {
            for (DWORD i=0; i<adapterCapabilities->P2PAttributes.InterfaceAddressList.ElementCount; i++)
            {
                PUCHAR  pBuf = (PUCHAR) &adapterCapabilities->P2PAttributes.InterfaceAddressList.pElements[i];
                WFCInfo ("\t[%d] DeviceAddress=%!MACADDR!\n", i, pBuf);
            }
        }
    }
    
    NT_ASSERTMSG( "Unsupported operation mode", 
        0 == (opModeCapability & (DOT11_OPERATION_MODE_AP | DOT11_OPERATION_MODE_EXTENSIBLE_AP | DOT11_OPERATION_MODE_NETWORK_MONITOR | DOT11_OPERATION_MODE_MANUFACTURING)) );

    ndisStatus = SetPropertyULong( 
            (WFC_PROPERTY_NAME)WfcAdapterPropertySupportedOpModes,
            opModeCapability);

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't populate Supported Op Modes property");
        goto exit;
    }

    //
    // Copy the Station Attributes
    //
    ndisStatus = LoadProperties(this,
        sizeof(WDI_STATION_CAPABILITIES_CONTAINER),
        (BYTE*)&adapterCapabilities->StationAttributes.StationCapabilities,
        (PWFC_PROPERTY_LOAD_INFO) g_WFC_ExtSta_Capabilities, 
        celems(g_WFC_ExtSta_Capabilities),
        WfcAdapterPropertyNameMax);

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't populate g_WFC_ExtSta_Capabilities properties. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    if (adapterCapabilities->StationAttributes.Optional.UnicastAlgorithms_IsPresent)
    {
        ndisStatus = LoadProperties(this,
                        sizeof(WDI_STATION_ATTRIBUTES_CONTAINER),
                        (BYTE*)&adapterCapabilities->StationAttributes,
                        (PWFC_PROPERTY_LOAD_INFO) g_WFC_InfraSupportedUcastAlgoPairs, 
                        celems(g_WFC_InfraSupportedUcastAlgoPairs),
                        WfcAdapterPropertyNameMax);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("Couldn't populate ExtSTAAttributes->pInfraSupportedUcastAlgoPairs properties");
            goto exit;
        }
    }
    
    if (adapterCapabilities->StationAttributes.Optional.MulticastDataAlgorithms_IsPresent != NULL)
    {
        ndisStatus = LoadProperties(this,
                        sizeof(WDI_STATION_ATTRIBUTES_CONTAINER),
                        (BYTE*)&adapterCapabilities->StationAttributes,
                        g_WFC_InfraSupportedMcastAlgoPairs, 
                        celems(g_WFC_InfraSupportedMcastAlgoPairs),
                        WfcAdapterPropertyNameMax);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("Couldn't populate ExtSTAAttributes->pInfraSupportedMcastAlgoPairs properties");
            goto exit;
        }
    }

    if (adapterCapabilities->Optional.PmCapabilities_IsPresent)
    {
        ndisStatus = LoadProperties(this,
                        sizeof(WDI_PM_CAPABILITIES_CONTAINER),
                        (BYTE*)&adapterCapabilities->PmCapabilities,
                        (PWFC_PROPERTY_LOAD_INFO) g_WFC_PM_Capabilities, 
                        celems(g_WFC_PM_Capabilities),
                        WfcAdapterPropertyNameMax);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("Couldn't populate g_WFC_PM_Capabilities properties. Error = 0x%08x", ndisStatus);
            goto exit;
        }
    }

    if (!adapterCapabilities->Optional.BandInfo_IsPresent)
    {
        pBandInfo = NULL;
        numOfBandInfos = 0;
    }
    else
    {
        numOfBandInfos = adapterCapabilities->BandInfo.ElementCount;
        pBandInfo = new WFC_BAND_INFO[numOfBandInfos];

        for (UINT32 i = 0; i < adapterCapabilities->BandInfo.ElementCount; i++)
        {
            pBandInfo[i].bandID = adapterCapabilities->BandInfo.pElements[i].BandCapabilities.BandID;
            pBandInfo[i].bandState = adapterCapabilities->BandInfo.pElements[i].BandCapabilities.BandState;

            pBandInfo[i].numOfPhyTypes = adapterCapabilities->BandInfo.pElements[i].ValidPhyTypes.ElementCount;
            pBandInfo[i].pPhyTypeList = new UINT32[pBandInfo[i].numOfPhyTypes];
            if (!pBandInfo[i].pPhyTypeList)
            {
                WFCError("Couldn't allocate %d Elements for PhyTypeList", pBandInfo[i].numOfPhyTypes);
                goto exit;
            }
            RtlCopyMemory(pBandInfo[i].pPhyTypeList, adapterCapabilities->BandInfo.pElements[i].ValidPhyTypes.pElements, sizeof(UINT32)* pBandInfo[i].numOfPhyTypes);

            pBandInfo[i].numOfChannelWidths = adapterCapabilities->BandInfo.pElements[i].ChannelWidthList.ElementCount;
            pBandInfo[i].pChannelWidthList = new UINT32[pBandInfo[i].numOfChannelWidths];
            if (!pBandInfo[i].pChannelWidthList)
            {
                WFCError("Couldn't allocate %d Elements for ChannelWidthList", pBandInfo[i].numOfChannelWidths);
                goto exit;
            }
            RtlCopyMemory(pBandInfo[i].pChannelWidthList, adapterCapabilities->BandInfo.pElements[i].ChannelWidthList.pElements, sizeof(UINT32)* pBandInfo[i].numOfChannelWidths);
  
            pBandInfo[i].numOfChannels = adapterCapabilities->BandInfo.pElements[i].ValidChannelTypes.ElementCount;
            pBandInfo[i].pChannelList = new WDI_CHANNEL_MAPPING_ENTRY[pBandInfo[i].numOfChannels];
            if (!pBandInfo[i].pChannelList)
            {
                WFCError("Couldn't allocate %d Elements for ChannelList", pBandInfo[i].numOfChannels);
                goto exit;
            }

            for (UINT32 j = 0; j < pBandInfo[i].numOfChannels; j++)
            {
                pBandInfo[i].pChannelList[j].ChannelNumber = adapterCapabilities->BandInfo.pElements[i].ValidChannelTypes.pElements[j].ChannelNumber;
                pBandInfo[i].pChannelList[j].ChannelCenterFrequency = adapterCapabilities->BandInfo.pElements[i].ValidChannelTypes.pElements[j].ChannelCenterFrequency;
            }
        }
    }

    if (pBandInfo == NULL || numOfBandInfos == 0)
    {
        WFCError("Failed to find Band Info in firmware capabilities. Error = 0x%08x", ndisStatus);
        goto exit;
    }

   ndisStatus = LoadPropertyListProperty((WFC_PROPERTY_NAME) WfcAdapterPropertyBandInfo,
                                        numOfBandInfos,
                                        sizeof(WFC_BAND_INFO),
                                        (BYTE*)pBandInfo,    
                                        (PWFC_PROPERTY_LOAD_INFO) g_WFC_BandInfoProperties, 
                                        celems(g_WFC_BandInfoProperties),
                                        WfcBandInfoNameMax);

    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Couldn't populate Band Info properties. Error = 0x%08x", ndisStatus);
        goto exit;
    }


    //
    // add phy infos
    //

    if (!adapterCapabilities->Optional.PhyInfo_IsPresent)
    {
        pPhyInfo = NULL;
        numOfPhyInfos = 0;
        goto exit;
    }

    numOfPhyInfos = adapterCapabilities->PhyInfo.ElementCount;
    pPhyInfo = new WFC_PHY_INFO[numOfPhyInfos];

    for (UINT32 i = 0; i < adapterCapabilities->PhyInfo.ElementCount; i++)
    {
        pPhyInfo[i].PhyType = adapterCapabilities->PhyInfo.pElements[i].PhyCapabilities.PhyType;
        pPhyInfo[i].CFPollable = adapterCapabilities->PhyInfo.pElements[i].PhyCapabilities.SupportsCFPoll;
        pPhyInfo[i].MPDUMaxLength = adapterCapabilities->PhyInfo.pElements[i].PhyCapabilities.MPDUMaxLength;
        pPhyInfo[i].TempType = (DOT11_TEMP_TYPE)adapterCapabilities->PhyInfo.pElements[i].PhyCapabilities.TemperatureClass;
        pPhyInfo[i].DiversitySupport = (DOT11_DIVERSITY_SUPPORT)adapterCapabilities->PhyInfo.pElements[i].PhyCapabilities.DiversitySupport;

        pPhyInfo[i].NumberSupportedPowerLevels = adapterCapabilities->PhyInfo.pElements[i].TxPowerLevelList.ElementCount;

        RtlCopyMemory(pPhyInfo[i].TxPowerLevels, adapterCapabilities->PhyInfo.pElements[i].TxPowerLevelList.pElements, sizeof(UINT32) * pPhyInfo[i].NumberSupportedPowerLevels); 
        
        pPhyInfo[i].NumberDataRateEntries = adapterCapabilities->PhyInfo.pElements[i].DataRateList.ElementCount;
        for (UINT32 j = 0; j < pPhyInfo[i].NumberDataRateEntries; j++)
        {
            pPhyInfo[i].DataRateList[j].DataRateFlag = adapterCapabilities->PhyInfo.pElements[i].DataRateList.pElements[j].DataRateFlag;
            pPhyInfo[i].DataRateList[j].DataRateValue = adapterCapabilities->PhyInfo.pElements[i].DataRateList.pElements[j].DataRateValue;
        }
    }


    if (pPhyInfo == NULL || numOfPhyInfos == 0)
    {
        WFCError("Failed to find Phy Info in firmware capabilities");
        goto exit;
    }

    ndisStatus = LoadPropertyListProperty((WFC_PROPERTY_NAME)WfcAdapterPropertyPhyInfo,
        numOfPhyInfos,
        sizeof(WFC_PHY_INFO),
        (BYTE*)pPhyInfo,
        (PWFC_PROPERTY_LOAD_INFO)g_WFC_AdapterPhyInfoProperties,
        celems(g_WFC_AdapterPhyInfoProperties),
        WfcAdapterPhyInfoNameMax);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate Phy Info properties. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    // Assign each PHY an ID. For efficient conversion, we have save a table
    pPhyIDMap = new ULONG[numOfPhyInfos];
    if (pPhyIDMap == NULL)
    {
        WFCError("Couldn't allocate Phy ID map");
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }

    //
    // get a table for phy info properties
    //
    ndisStatus = GetPropertyPropertyCacheList((WFC_PROPERTY_NAME)WfcAdapterPropertyPhyInfo,
        &propertyCache);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't get Phy info property. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    currentPhyID = 0;   // Start with ID 0
    while (propertyCache != NULL)
    {
        //
        // Set Phy ID property
        //
        ndisStatus = propertyCache->SetPropertyULong((WFC_PROPERTY_NAME)WfcAdapterDot11PhyID,
            currentPhyID
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("Couldn't assign Phy ID to a phy. Error = 0x%08x", ndisStatus);
            goto exit;
        }

        if (currentPhyID >= numOfPhyInfos)
        {
            WFCError("Invalid current Phy ID value %d more than allowed count %d", currentPhyID, numOfPhyInfos);
            ndisStatus = NDIS_STATUS_INVALID_DATA;
            goto exit;
        }

        // Also save PhyType in our map
        ndisStatus = propertyCache->GetPropertyULong((WFC_PROPERTY_NAME)WfcAdapterPhyType,
            &pPhyIDMap[currentPhyID]
            );
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("Couldn't retrieve Phy Type for a phy. Error = 0x%08x", ndisStatus);
            goto exit;
        }
        currentPhyID++;

        //
        // get the next property table
        //
        propertyCache = propertyCache->Next;

    }

    // Save the Phy ID MAP
    ndisStatus = this->SetPropertyList(
        WfcAdapterPhyTypeDot11PhyIDMap,
        sizeof(ULONG)* numOfPhyInfos,
        (UINT16)numOfPhyInfos,
        (BYTE *)pPhyIDMap
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't save Phy Type to Phy ID Map. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    if (adapterCapabilities->Optional.TcpOffloadCapabilities_IsPresent)
    {
        ndisStatus = PopulateTcpOffloadCapabilities(adapterCapabilities->TcpOffloadCapabilities);
        
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("Couldn't populate TCP offload Capabilities. Error = 0x%08x", ndisStatus);
            goto exit;
        }
    }

    if ((adapterCapabilities->Optional.SupportedGuids_IsPresent) &&
        (adapterCapabilities->SupportedGuids.ElementCount))
    {
        // Save the Phy ID MAP
        ndisStatus = this->SetPropertyList(
            WfcAdapterSupportedGuids,
            sizeof(NDIS_GUID) * adapterCapabilities->SupportedGuids.ElementCount,
            (UINT16) adapterCapabilities->SupportedGuids.ElementCount,
            (BYTE *) adapterCapabilities->SupportedGuids.pElements
            );
        
        if (ndisStatus != NDIS_STATUS_SUCCESS)
        {
            WFCError("Couldn't populate Supported GUIDs. Error = 0x%08x", ndisStatus);
            goto exit;
        }
    }

exit:

    if (pBandInfo)
    {
        for (UINT32 i = 0; i < adapterCapabilities->BandInfo.ElementCount; i++)
        {
            if (pBandInfo[i].pPhyTypeList)
            {
                delete[] pBandInfo[i].pPhyTypeList;
            }
            if (pBandInfo[i].pChannelWidthList)
            {
                delete[] pBandInfo[i].pChannelWidthList;
            }
            if (pBandInfo[i].pChannelList)
            {
                delete[] pBandInfo[i].pChannelList;
            }
        }
        delete[] pBandInfo;
    }

    if (pPhyInfo)
    {
        delete[] pPhyInfo;
    }

    if (pPhyIDMap)
    {
        delete[] pPhyIDMap;
    }
    return ndisStatus;

}

NDIS_STATUS CAdapterPropertyCache::GetTCPOffloadParametersFromPropertyCache(
    _Out_ PWDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER pOffloadParameters)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;
    
    status = GetPropertyT(WfcAdapterPropertyChecksumV4TxEncapsulation,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV4TxIpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.IpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 IpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV4TxTcpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV4TxTcpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV4TxUdpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV4TxIpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.IpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 IpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcAdapterPropertyChecksumV4RxEncapsulation,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV4RxIpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.IpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 IpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV4RxTcpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV4RxTcpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV4RxUdpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV4RxIpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.IpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 IpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcAdapterPropertyChecksumV6TxEncapsulation,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV6TxIpExtensionHeadersSupported,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.IpExtensionHeadersSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 IpExtensionHeadersSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV6TxTcpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV6TxTcpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV6TxUdpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcAdapterPropertyChecksumV6RxEncapsulation,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV6RxIpExtensionHeadersSupported,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.IpExtensionHeadersSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 IpExtensionHeadersSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV6RxTcpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV6RxTcpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyChecksumV6RxUdpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV1Encapsulation,
        &pOffloadParameters->LsoV1Capabilities.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV1 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV1MaxOffLoadSize,
        &pOffloadParameters->LsoV1Capabilities.MaxOffLoadSize);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV1 MaxOffLoadSize from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV1MinSegmentCount,
        &pOffloadParameters->LsoV1Capabilities.MinSegmentCount);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV1 MinSegmentCount from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV1TcpOptions,
        &pOffloadParameters->LsoV1Capabilities.TcpOptions);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV1 TcpOptions from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV1IpOptions,
        &pOffloadParameters->LsoV1Capabilities.IpOptions);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV1 IpOptions from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV2EncapsulationV4,
        &pOffloadParameters->LsoV2Capabilities.Ipv4LSO.EncapsulationV4);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv4 EncapsulationV4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV2MaxOffloadSizeV4,
        &pOffloadParameters->LsoV2Capabilities.Ipv4LSO.MaxOffloadSizeV4);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv4 MaxOffloadSizeV4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV2MinSegmentCountV4,
        &pOffloadParameters->LsoV2Capabilities.Ipv4LSO.MinSegmentCountV4);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv4 MinSegmentCountV4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV2EncapsulationV6,
        &pOffloadParameters->LsoV2Capabilities.Ipv6LSO.EncapsulationV6);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv6 EncapsulationV6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV2MaxOffloadSizeV6,
        &pOffloadParameters->LsoV2Capabilities.Ipv6LSO.MaxOffloadSizeV6);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv6 MaxOffloadSizeV6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV2MinSegmentCountV6,
        &pOffloadParameters->LsoV2Capabilities.Ipv6LSO.MinSegmentCountV6);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv6 MinSegmentCountV6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV2IpExtensionHeadersSupported,
        &pOffloadParameters->LsoV2Capabilities.Ipv6LSO.IpExtensionHeadersSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv6 IpExtensionHeadersSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyLargeSendOffloadV2TcpOptionsSupported,
        &pOffloadParameters->LsoV2Capabilities.Ipv6LSO.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv6 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcAdapterPropertyRSCIpv4Enabled,
        &pOffloadParameters->ReceiveOffloadCapabilities.Ipv4Enabled);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve RSC Ipv4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcAdapterPropertyRSCIpv6Enabled,
        &pOffloadParameters->ReceiveOffloadCapabilities.Ipv6Enabled);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve RSC Ipv6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

exit:
    return status;
}

NDIS_STATUS CAdapterPropertyCache::PopulateTcpOffloadCapabilities(
    WDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER TcpOffloadCapabilities)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ndisStatus = LoadProperties(this,
        sizeof(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER),
        (BYTE*)&TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit,
        g_WFC_ChecksumOffloadV4Tx_Capabilities,
        celems(g_WFC_ChecksumOffloadV4Tx_Capabilities),
        WfcAdapterPropertyNameMax);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate g_WFC_ChecksumOffloadV4Tx_Capabilities properties. Error = 0x%08x", ndisStatus);
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    ndisStatus = LoadProperties(this,
        sizeof(WDI_CHECKSUM_OFFLOAD_V4_PARAMETERS_CONTAINER),
        (BYTE*)&TcpOffloadCapabilities.ChecksumOffload.ipv4ChecksumOffload.IpV4Receive,
        g_WFC_ChecksumOffloadV4Rx_Capabilities,
        celems(g_WFC_ChecksumOffloadV4Rx_Capabilities),
        WfcAdapterPropertyNameMax);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate g_WFC_ChecksumOffloadV4Rx_Capabilities properties. Error = 0x%08x", ndisStatus);
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    ndisStatus = LoadProperties(this,
        sizeof(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER),
        (BYTE*)&TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit,
        g_WFC_ChecksumOffloadV6Tx_Capabilities,
        celems(g_WFC_ChecksumOffloadV6Tx_Capabilities),
        WfcAdapterPropertyNameMax);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate g_WFC_ChecksumOffloadV6Tx_Capabilities properties. Error = 0x%08x", ndisStatus);
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    ndisStatus = LoadProperties(this,
        sizeof(WDI_CHECKSUM_OFFLOAD_V6_PARAMETERS_CONTAINER),
        (BYTE*)&TcpOffloadCapabilities.ChecksumOffload.ipv6ChecksumOffload.IpV6Receive,
        g_WFC_ChecksumOffloadV6Rx_Capabilities,
        celems(g_WFC_ChecksumOffloadV6Rx_Capabilities),
        WfcAdapterPropertyNameMax);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate g_WFC_ChecksumOffloadV6Rx_Capabilities properties. Error = 0x%08x", ndisStatus);
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    ndisStatus = LoadProperties(this,
        sizeof(WDI_LSO_V1_CAPABILITIES_CONTAINER),
        (BYTE*)&TcpOffloadCapabilities.LsoV1Capabilities,
        g_WFC_largeSendOffloadV1_Capabilities,
        celems(g_WFC_largeSendOffloadV1_Capabilities),
        WfcAdapterPropertyNameMax);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate g_WFC_largeSendOffloadV1_Capabilities properties. Error = 0x%08x", ndisStatus);
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    ndisStatus = LoadProperties(this,
        sizeof(WDI_IPV4_LSO_V2_CONTAINER),
        (BYTE*)&TcpOffloadCapabilities.LsoV2Capabilities.Ipv4LSO,
        (PWFC_PROPERTY_LOAD_INFO)g_WFC_largeSendOffloadV2ipv4_Capabilities,
        celems(g_WFC_largeSendOffloadV2ipv4_Capabilities),
        WfcAdapterPropertyNameMax);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate g_WFC_largeSendOffloadV2ipv4_Capabilities properties. Error = 0x%08x", ndisStatus);
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    ndisStatus = LoadProperties(this,
        sizeof(WDI_IPV6_LSO_V2_CONTAINER),
        (BYTE*)&TcpOffloadCapabilities.LsoV2Capabilities.Ipv6LSO,
        (PWFC_PROPERTY_LOAD_INFO)g_WFC_largeSendOffloadV2ipv6_Capabilities,
        celems(g_WFC_largeSendOffloadV2ipv6_Capabilities),
        WfcAdapterPropertyNameMax);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate g_WFC_largeSendOffloadV2ipv6_Capabilities properties. Error = 0x%08x", ndisStatus);
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

    ndisStatus = LoadProperties(this,
        sizeof(WDI_RECEIVE_COALESCE_OFFLOAD_CAPABILITIES_CONTAINER),
        (BYTE*)&TcpOffloadCapabilities.ReceiveOffloadCapabilities,
        (PWFC_PROPERTY_LOAD_INFO)g_WFC_RSC_Capabilities,
        celems(g_WFC_RSC_Capabilities),
        WfcAdapterPropertyNameMax);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate g_WFC_RSC_Capabilities properties. Error = 0x%08x", ndisStatus);
        ndisStatus = NDIS_STATUS_FAILURE;
        goto exit;
    }

exit:
    return ndisStatus;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CAdapterPropertyCache::GetPhyTypeFromDot11PhyID(
    _In_ ULONG Dot11PhyID,
    _Out_ UINT32 *pPhyType
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_NOT_RECOGNIZED;
    UINT16 phyCount;
    ULONG bufferSize;
    ULONG *pPhyIDMap;
    
    ndisStatus = GetPropertyList(WfcAdapterPhyTypeDot11PhyIDMap, 
                    &bufferSize,
                    &phyCount,
                    (BYTE **)&pPhyIDMap);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to retrieve Phy Type to Phy ID Map. Error = 0x%08x", ndisStatus);   
        goto exit;
    }

    if (Dot11PhyID >= phyCount)
    {
        WFCError("Failed to convert Unknown Phy ID (%d) to Phy Type", Dot11PhyID);   
        ndisStatus = NDIS_STATUS_NOT_RECOGNIZED;
    }

    *pPhyType = pPhyIDMap[Dot11PhyID];

exit:
    return ndisStatus;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CAdapterPropertyCache::GetDot11PhyIDFromPhyType(
    _In_ UINT32 PhyType,
    _Out_ ULONG *pDot11PhyID
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_NOT_RECOGNIZED;
    UINT16 phyCount;
    ULONG bufferSize;
    ULONG *pPhyIDMap;
    
    ndisStatus = this->GetPropertyList(WfcAdapterPhyTypeDot11PhyIDMap, 
                    &bufferSize,
                    &phyCount,
                    (BYTE **)&pPhyIDMap);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to retrieve Phy Type to Phy ID Map. Error = 0x%08x", ndisStatus);   
        goto exit;
    }

    for (ULONG i = 0; i < phyCount; i++)
    {
        if (pPhyIDMap[i] == PhyType)
        {
            *pDot11PhyID = i;
            goto exit;
        }
    }
    
    WFCError("Failed to convert Unknown Phy Type (%d) to Phy ID", PhyType);   
    ndisStatus = NDIS_STATUS_NOT_RECOGNIZED;

exit:
    return ndisStatus;

}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CAdapterPropertyCache::CreateBandCacheHelperForBandID(
    _In_ ULONG BandID,
    _Out_ CBandCacheHelper **ppBandCacheHelper)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CPropertyCache* pBandInfoCache;

    *ppBandCacheHelper = NULL;
    ndisStatus = GetBandInfoCacheForBandID(
                    BandID,
                    &pBandInfoCache);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to get Band Info Cache for Band ID %d. Error = 0x%08x", BandID, ndisStatus);   
        goto exit;
    }

    *ppBandCacheHelper = new CBandCacheHelper(
        this,
        pBandInfoCache
        );
    if (*ppBandCacheHelper == NULL)
    {
        WFCError("Failed to allocate Band Cache helper for Band ID %d", BandID);   
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }
    
exit:
    return ndisStatus;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CAdapterPropertyCache::CreatePhyCacheHelperForDot11PhyID(
    _In_ ULONG Dot11PhyID,
    _Out_ CPhyCacheHelper **ppPhyCacheHelper)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    CPropertyCache* pPhyInfoCache;

    *ppPhyCacheHelper = NULL;
    ndisStatus = GetPhyInfoCacheForDot11PhyID(
                    Dot11PhyID,
                    &pPhyInfoCache);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to get Phy Info Cache for Phy ID %d. Error = 0x%08x", Dot11PhyID, ndisStatus);   
        goto exit;
    }

    *ppPhyCacheHelper = new CPhyCacheHelper(
        this,
        pPhyInfoCache
        );
    if (*ppPhyCacheHelper == NULL)
    {
        WFCError("Failed to allocate Phy Cache helper for Phy ID %d", Dot11PhyID);   
        ndisStatus = NDIS_STATUS_RESOURCES;
        goto exit;
    }
    
exit:
    return ndisStatus;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CAdapterPropertyCache::GetBandInfoCacheForBandID(
    _In_ ULONG BandID,
    _Out_ CPropertyCache **ppBandInfoCache)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    CPropertyCache *propertyCache;
    ULONG currentBandID;
    
    //
    // get a table for bandinfo properties
    //
    ndisStatus = GetPropertyPropertyCacheList((WFC_PROPERTY_NAME)WfcAdapterPropertyBandInfo,
                                        &propertyCache);
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Failed to get Band Info property. Error = 0x%08x", ndisStatus);   
        goto exit;
    } 

    while (propertyCache != NULL)
    {                    
        //
        // get BandInfo band ID property
        //
        ndisStatus = propertyCache->GetPropertyULong((WFC_PROPERTY_NAME)WfcBandInfoBandId, 
                                                        &currentBandID);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("Failed to get Band ID property. Error = 0x%08x", ndisStatus);   
            goto exit;
        } 

        if (currentBandID == BandID)
        {
            *ppBandInfoCache = propertyCache;
            goto exit;
        }
           
        //
        // get the next property table
        //
        propertyCache = propertyCache->Next;

    }
    ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
    
exit:

    return ndisStatus;
}


__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CAdapterPropertyCache::GetBandIDForStartingFrequency(
    _In_ UINT32 StartingFrequencyInKhz,
    _Out_ UINT32* pBandID )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    CPropertyCache *propertyCache;

    //
    // get a table for bandinfo properties
    //
    ndisStatus = GetPropertyPropertyCacheList(
        WfcAdapterPropertyBandInfo,
        &propertyCache );
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Failed to get Band Info property. Error = 0x%08x", ndisStatus);   
        goto exit;
    } 
    while (propertyCache != NULL)
    {                    
        ULONG numOfChannels;    
        PWDI_CHANNEL_MAPPING_ENTRY pChannelList;

        ndisStatus = propertyCache->GetPropertyBuffer(
            WfcBandInfoChannelList, 
            &numOfChannels,
            (BYTE**)&pChannelList );

        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("Failed to get Band Channel list property. Error = 0x%08x", ndisStatus);
            goto exit;
        }

        if ( numOfChannels == 0 )
        {
            propertyCache = propertyCache->Next;
            NT_ASSERT(FALSE);
            continue;
        }

        UINT32 baseFrequencyInMhz = pChannelList->ChannelCenterFrequency -
            (pChannelList->ChannelNumber * 5);

        if ( baseFrequencyInMhz * 1000 != StartingFrequencyInKhz )
        {
            propertyCache = propertyCache->Next;
            continue;
        }

        //
        // get BandInfo band ID property
        //
        ndisStatus = propertyCache->GetPropertyT(
            WfcBandInfoBandId, 
            pBandID );

        if ( ndisStatus == NDIS_STATUS_SUCCESS )
        {
            goto exit;
        }
           
        //
        // get the next property table
        //
        propertyCache = propertyCache->Next;

    }
    ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
    
exit:

    return ndisStatus;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CAdapterPropertyCache::GetPhyInfoCacheForDot11PhyID(
    _In_ ULONG Dot11PhyID,
    _Out_ CPropertyCache **ppPhyInfoCache)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    CPropertyCache *propertyCache;
    ULONG currentPhyID;
    
    //
    // get a table for bandinfo properties
    //
    ndisStatus = GetPropertyPropertyCacheList((WFC_PROPERTY_NAME)WfcAdapterPropertyPhyInfo,
                                        &propertyCache);
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Failed to get Phy Info property. Error = 0x%08x", ndisStatus);   
        goto exit;
    } 

    while (propertyCache != NULL)
    {                    
        //
        // get Phy Info phy ID property
        //
        ndisStatus = propertyCache->GetPropertyULong ((WFC_PROPERTY_NAME)WfcAdapterDot11PhyID,
                                                        &currentPhyID);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("Failed to get Phy ID property. Error = 0x%08x", ndisStatus);   
            goto exit;
        } 

        if (currentPhyID == Dot11PhyID)
        {
            *ppPhyInfoCache = propertyCache;
            goto exit;
        }
           
        //
        // get the next property table
        //
        propertyCache = propertyCache->Next;

    }
    ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
    
exit:

    return ndisStatus;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CAdapterPropertyCache::LoadBandCacheForDot11PhyID(
    _In_ ULONG Dot11PhyID,
    _In_ CPhyCacheHelper *pPhyCacheHelper
)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;

    CPropertyCache *propertyCache;

    ULONG numOfPhyTypes;
    PUINT32 pPhyTypeList;
       
    UINT32 PhyType;

    // Convert Phy ID to Phy Type
    ndisStatus = GetPhyTypeFromDot11PhyID(Dot11PhyID, &PhyType);
    if ( ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to convert Phy ID to Phy Type. Error = 0x%08x", ndisStatus);   
        goto exit;
    }
    
    //
    // get a table for bandinfo properties
    //
    ndisStatus = GetPropertyPropertyCacheList((WFC_PROPERTY_NAME)WfcAdapterPropertyBandInfo,
                                        &propertyCache);
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Failed to get Band Info property. Error = 0x%08x", ndisStatus);   
        goto exit;
    } 

    ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;    // Assume we wont find it
    
    while (propertyCache != NULL)
    {                    
        //
        // get BandInfo pPhyTypeList property
        //
        ndisStatus = propertyCache->GetPropertyBuffer ((WFC_PROPERTY_NAME)WfcBandInfoPhyTypeList, 
                                                        &numOfPhyTypes,
                                                        (BYTE**)&pPhyTypeList);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("Failed to get Band pPhyTypeList property. Error = 0x%08x", ndisStatus);   
            goto exit;
        } 
        if ( numOfPhyTypes == 0 || (numOfPhyTypes % sizeof(UINT32)) != 0)
        {
            ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
            WFCError("property buffer size is invalid");
            goto exit;
        }   

        //
        // numOfPhyTypes is a buffer length in bytes, convert it to index UINT32 array
        //
        numOfPhyTypes = numOfPhyTypes / sizeof(UINT32);

        for (ULONG phyIndex = 0; phyIndex < numOfPhyTypes; ++phyIndex)
        {
            //
            // if phy type matched
            //
            if (pPhyTypeList[phyIndex] == PhyType)
            {
                ndisStatus = pPhyCacheHelper->AddBandInfoCache(propertyCache);
                if (ndisStatus != NDIS_STATUS_SUCCESS)
                {
                    WFCError("Failed to save Band Info cache in Phy cache helper. Error = 0x%08x", ndisStatus);   
                    goto exit;
                }
            }
        }
           
        //
        // get the next property table
        //
        propertyCache = propertyCache->Next;

    }

exit:

    return ndisStatus;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CAdapterPropertyCache::GetMaximumSupportedAuthAlgorithm(
    _In_ WFC_ADAPTER_PROPERTY_NAME AlgoPairProperty,
    _Out_ DOT11_AUTH_ALGORITHM *pAuthAlgorithmIdOut)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;              
    DOT11_AUTH_ALGORITHM authAlgorithmId;
    BOOLEAN IsSupported;

    authAlgorithmId = DOT11_AUTH_ALGO_80211_OPEN;
    for (ULONG algoIndex = 0; algoIndex < AUTH_ALGO_PREFERENCE_LIST_SIZE; ++algoIndex)
    {
        ndisStatus = IsAuthAlgorithmSupported(AlgoPairProperty,
                                           AUTH_ALGO_PREFERENCE_LIST[algoIndex],
                                           &IsSupported);
        if ( ndisStatus != NDIS_STATUS_SUCCESS )
        {
            WFCError("IsAlgorithmeSupported failed");                
            goto exit;
        } 

        if (IsSupported)
        {
            authAlgorithmId = AUTH_ALGO_PREFERENCE_LIST[algoIndex];
            break;
        }
    }

    *pAuthAlgorithmIdOut = authAlgorithmId;    

exit:

    return ndisStatus;

}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CAdapterPropertyCache::GetAuthAlgorithmPairs(        
    _In_ WFC_ADAPTER_PROPERTY_NAME AlgoPairProperty,
    _Out_ ULONG* pNumOfAlgoPairs,
    _Outptr_result_buffer_(*pNumOfAlgoPairs) PDOT11_AUTH_CIPHER_PAIR* ppAlgoPairs)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG numOfAlgoPairs;
    BYTE *pAlgoPairsBuffer;

    //
    // get algo pair property
    //
    ndisStatus = GetPropertyBuffer ((WFC_PROPERTY_NAME)AlgoPairProperty, 
                                    &numOfAlgoPairs,
                                    (BYTE**)&pAlgoPairsBuffer);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't get supported algo pair property");                
        goto exit;
    } 

    //
    // report algorithm is not supported if there is no algoPairs
    //
    if (numOfAlgoPairs == 0)
    {
        *pNumOfAlgoPairs = 0;
        *ppAlgoPairs = NULL;
        goto exit;
    }

    //
    // the buffer length must be exact multiple of sizeof(DOT11_AUTH_CIPHER_PAIR)
    //
    if ((numOfAlgoPairs % sizeof(DOT11_AUTH_CIPHER_PAIR)) != 0)
    {
        ndisStatus = NDIS_STATUS_INVALID_PARAMETER;
        WFCError("property buffer size is invalid");
        goto exit;
    }   

    //
    // numOfAlgoPairs is a buffer length in bytes, convert it to index DOT11_AUTH_CIPHER_PAIR array
    //
    *pNumOfAlgoPairs = numOfAlgoPairs / sizeof(DOT11_AUTH_CIPHER_PAIR);
   
    *ppAlgoPairs = (PDOT11_AUTH_CIPHER_PAIR) pAlgoPairsBuffer;

exit:

    return ndisStatus;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CAdapterPropertyCache::IsAuthAlgorithmSupported(
    _In_ WFC_ADAPTER_PROPERTY_NAME AlgoPairProperty,
    _In_ DOT11_AUTH_ALGORITHM AlgoId,
    _Out_ BOOLEAN *IsSupportedOut)
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG numOfAlgoPairs;
    PDOT11_AUTH_CIPHER_PAIR pAlgoPairs;

    ndisStatus = GetAuthAlgorithmPairs(AlgoPairProperty,
                                    &numOfAlgoPairs,
                                    &pAlgoPairs);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't get supported algo pair property");
        goto exit;
    }

    *IsSupportedOut = false;
    for (ULONG algoIndex = 0; algoIndex < numOfAlgoPairs; ++algoIndex)
    {
        //
        // if algoId matches
        //
        if (pAlgoPairs[algoIndex].AuthAlgoId == AlgoId)
        {
            *IsSupportedOut = true;
            break;
        }
    }

exit:

    return ndisStatus;
}

__success(return == NDIS_STATUS_SUCCESS)
 NDIS_STATUS CAdapterPropertyCache::GetMaxMulticastListSize(
     _Out_ PULONG pMulticastListSizeOut)
{
    NDIS_STATUS ndisStatus;
    ULONG listSize = 0;

    ndisStatus = GetPropertyULong(
        (WFC_PROPERTY_NAME)WfcAdapterPropertyMaxMulticastListSize,
        &listSize);

    if(NDIS_STATUS_SUCCESS == ndisStatus)
    {
        *pMulticastListSizeOut = listSize;
    }
    else
    {
        WFCError("Couldn't get multicast list size property");
    }
    
    return ndisStatus;
}

__success(return == NDIS_STATUS_SUCCESS)
 NDIS_STATUS CAdapterPropertyCache::GetPermanentMacAddress(
     _Out_ PDOT11_MAC_ADDRESS  pHardwareAddress)
{
    NDIS_STATUS ndisStatus;
    ULONG bufferLength;
    BYTE* pAddressBuffer = NULL;
    
    ndisStatus = GetPropertyBuffer(
        (WFC_PROPERTY_NAME)WfcAdapterPropertyPermanentMacAddress,
        &bufferLength,
        &pAddressBuffer);

    if(NDIS_STATUS_SUCCESS == ndisStatus &&
        bufferLength >= sizeof(DOT11_MAC_ADDRESS))
    {
        RtlCopyMemory(pHardwareAddress, pAddressBuffer, sizeof(DOT11_MAC_ADDRESS));
    }
    else
    {
        WFCError("Couldn't get the permanent mac address property");
        RtlZeroMemory( pHardwareAddress, sizeof( DOT11_MAC_ADDRESS ) );
    }


    return ndisStatus;
}

CPortPropertyCache::CPortPropertyCache() :
    m_pAdapterCache(NULL)
{

}

NDIS_STATUS CPortPropertyCache::Initialize(
    _In_ WFC_PORT_TYPE PortType,
    _In_ CAdapterPropertyCache* pAdapterPropertyCache
    )   
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    WDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER OffloadParameters;

    ndisStatus = CPropertyCache::Initialize(WfcPortPropertyNameMax, NULL);
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Failed to initialize port property cache. Error = 0x%08x", ndisStatus);
        goto exit;
    }

    m_PortType = PortType;
    m_pAdapterCache = pAdapterPropertyCache;

    //
    // Defaults
    //
    
    ndisStatus = NdisReset();
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to load NDIS defaults for port property cache. Error = 0x%08x", ndisStatus);
        goto exit;    
    }

    ndisStatus = Dot11Reset (TRUE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to load Dot11 defaults for port property cache. Error = 0x%08x", ndisStatus);
        goto exit;    
    }

    //
    // Copy the adapter capabilities for Checksum offloads/LSO
    //
    ndisStatus = pAdapterPropertyCache->GetTCPOffloadParametersFromPropertyCache(
        &OffloadParameters
        );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to Get adapter's offload caps for populating in port property cache. Error = 0x%08x", ndisStatus);
        ndisStatus = NDIS_STATUS_SUCCESS;
        goto exit;
    }

    ndisStatus = SetTCPOffloadParametersPortPropertyCache(
        &OffloadParameters);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Failed to populate TCP offload parameters for port property cache. Error = 0x%08x", ndisStatus);
        goto exit;
    }

exit:
    return ndisStatus;
}

#define MAX_SUPPORTED_CIPHERS 10

VOID CPortPropertyCache::AddCiphersToSupportedList(
    _In_ WFC_ADAPTER_PROPERTY_NAME AlgoPropertyName,
    _Out_writes_to_( MaxCiphers, *pCurrentCipherCount ) DOT11_CIPHER_ALGORITHM  *pSupportedCiphersList,
    _Out_ UINT16 *pCurrentCipherCount,
    _In_ UINT16 MaxCiphers,
    _In_reads_opt_( MaxAllowedCiphers ) DOT11_CIPHER_ALGORITHM *pAllowedCipherAlgoList,
    _In_ UINT16 MaxAllowedCiphers
    )
{
    NDIS_STATUS ndisStatus;
    UINT16 i, j, CurrentCipherCount;
    ULONG numOfAlgoPairs;
    PDOT11_AUTH_CIPHER_PAIR pAlgoPairs;
    DOT11_CIPHER_ALGORITHM AllowedCipher;

    CurrentCipherCount = *pCurrentCipherCount;

    ndisStatus = m_pAdapterCache->GetAuthAlgorithmPairs(AlgoPropertyName,
        &numOfAlgoPairs,
        &pAlgoPairs);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("AddCiphersToSupportedList: Couldn't get supported algo pair property [%d], ndisStatus=%x", AlgoPropertyName, ndisStatus);
        return;
    }

    //
    // If an Allowed list was not provided, use the driver list itself as the allowed list
    //
    if (!pAllowedCipherAlgoList)
    {
        MaxAllowedCiphers = (UINT16) numOfAlgoPairs;
    }

    //
    // The Ciphers have to be ordered as per the Allowed list, so enumerate the allowed list first
    //
    for (i=0; i<MaxAllowedCiphers; i++)
    {
        if (pAllowedCipherAlgoList)
        {
            AllowedCipher = pAllowedCipherAlgoList[i];
        }
        else
        {
            AllowedCipher = pAlgoPairs[i].CipherAlgoId;
        }

        if (CurrentCipherCount >= MaxCiphers)
        {
            break;
        }

        //
        // Check if the allowed/driver-provided cipher is new, or is already in the Supported list
        //
        for (j=0; j<CurrentCipherCount; j++)
        {
            if (AllowedCipher == pSupportedCiphersList[j])
            {
                break;
            }
        }

        //
        // If a match was found, the loop would have broken out before reaching the end
        // Continue only if this is a new entry
        //
        if (j < CurrentCipherCount)
        {
            continue;
        }

        //
        // If the new Cipher came from the allowed list, see if this Cipher is also in the list provided by the driver
        // Otherwise, the new Cipher was from the driver list itself, so no need to check further
        //
        if (pAllowedCipherAlgoList)
        {
            for (j=0; j < numOfAlgoPairs; j++)
            {
                if (AllowedCipher == pAlgoPairs[j].CipherAlgoId)
                {
                    break;
                }
            }

            //
            // If we reached the end of the driver list, then it means the allowed entry is not in the driver list
            //
            if (j >= numOfAlgoPairs)
            {
                continue;
            }
        }

        pSupportedCiphersList[CurrentCipherCount] = AllowedCipher;
        CurrentCipherCount++;
    }

    WFCInfo ("AddCiphersToSupportedList: Property=<%d>, CipherCount=<%d->%d>, Max=<%d>",
        AlgoPropertyName, *pCurrentCipherCount, CurrentCipherCount, MaxCiphers);

    *pCurrentCipherCount = CurrentCipherCount;
}

NDIS_STATUS CPortPropertyCache::Dot11Reset(
    _In_ BOOLEAN bSetDefaultMIB)  
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG uBufferLength;
    BYTE* pBuffer = NULL;

    DOT11_AUTH_ALGORITHM authAlgoList[] = {DOT11_AUTH_ALGO_RSNA};
    DOT11_CIPHER_ALGORITHM  SupportedCiphersList [MAX_SUPPORTED_CIPHERS];
    UINT16 CipherCount;

    DOT11_SSID Ssid = {0};
    DOT11_HESSID_LIST HessidList = {0};
    DOT11_MAC_ADDRESS BroadcastAddress = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    ULONG defaultDesiredPhyID = DOT11_PHY_ID_ANY;

    DOT11_CIPHER_ALGORITHM allowedCipherAlgoList[] = {
                                                DOT11_CIPHER_ALGO_GCMP,
                                                DOT11_CIPHER_ALGO_CCMP,
                                                DOT11_CIPHER_ALGO_TKIP,
                                                DOT11_CIPHER_ALGO_WEP,
                                                DOT11_CIPHER_ALGO_NONE 
                                              };

    TraceEntry();

    ndisStatus = SetPropertyULong(WfcPortPropertyNameDefaultKeyId, 0);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;
        
    ndisStatus = SetPropertyList(WfcPortPropertySSID, 
                    sizeof(Ssid),
                    1,
                    (BYTE *)&Ssid);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBuffer(WfcPortPropertyHESSID,
                    sizeof(HessidList),
                    (BYTE *)&HessidList);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyList(WfcPortPropertyAuthAlgoList, 
                    sizeof(authAlgoList),
                    celems(authAlgoList),
                    (BYTE *)&authAlgoList);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    //
    // Fill in the Unicast and Multicast Ciphers list from the Algo pairs list since that comes
    // directly from the driver and is actually reflective of the supported ciphers
    //
    CipherCount = 0;
    RtlZeroMemory( SupportedCiphersList, MAX_SUPPORTED_CIPHERS * sizeof( DOT11_CIPHER_ALGORITHM ) );

    AddCiphersToSupportedList(WfcAdapterPropertyInfraSupportedUcastAlgoPairs,
        SupportedCiphersList,
        &CipherCount,
        MAX_SUPPORTED_CIPHERS,
        allowedCipherAlgoList,
        celems(allowedCipherAlgoList));

    ndisStatus = SetPropertyList(WfcPortPropertyUnicastCipherAlgoList, 
                    CipherCount * sizeof(DOT11_CIPHER_ALGORITHM),
                    CipherCount,
                    (BYTE *)&SupportedCiphersList);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate UnicastCipherAlgoList properties");
        goto exit;
    }

    CipherCount = 0;
    RtlZeroMemory( SupportedCiphersList, MAX_SUPPORTED_CIPHERS * sizeof( DOT11_CIPHER_ALGORITHM ) );

    AddCiphersToSupportedList(WfcAdapterPropertyInfraSupportedMcastAlgoPairs,
        SupportedCiphersList,
        &CipherCount,
        MAX_SUPPORTED_CIPHERS,
        allowedCipherAlgoList,
        celems(allowedCipherAlgoList));

    ndisStatus = SetPropertyList(WfcPortPropertyMulticastCipherAlgoList, 
                    CipherCount * sizeof(DOT11_CIPHER_ALGORITHM),
                    CipherCount,
                    (BYTE *)&SupportedCiphersList);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't populate MulticastCipherAlgoList properties");
        goto exit;
    }

    ndisStatus = SetPropertyList(WfcPortPropertyDesiredBSSIDList, 
                    sizeof(DOT11_MAC_ADDRESS),
                    1,
                    BroadcastAddress);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyAnyBSSIDAllowed, TRUE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;


    ndisStatus = SetPropertyList(WfcPortPropertyExcludedMacAddressList, 
                    0,
                    0,
                    NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyIgnoreAllMacAddresses, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;
        
    ndisStatus = SetPropertyList(WfcPortPropertyPMKIDList, 
                    0,
                    0,
                    NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyPMKIDListUpdatePending, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyAssociationParameterPMKIDUpdatePending, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyPMKIDSupported, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyList(WfcPortPropertyDesiredPhyList_Dummy, 
                    sizeof(ULONG),
                    1,
                    (BYTE *)&defaultDesiredPhyID);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;
        
    ndisStatus = SetPropertyBoolean(WfcPortPropertyExcludeUnencrypted, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyHiddenNetwork, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBuffer(WfcPortPropertyExtraAssociationRequestIEs, 
                    0,
                    NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyUnicastUseGroupEnabled_Dummy, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyRadioState_Temp, TRUE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong(WfcPortBssType, dot11_BSS_type_infrastructure);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyMediaStreamingEnabled, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong(WfcPortPropertyConnectionQuality, 0);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;
        
    ndisStatus = SetPropertyULong(WfcPortPropertyAutoConfigEnabled, 
        (DOT11_MAC_AUTO_CONFIG_ENABLED_FLAG | DOT11_MAC_AUTO_CONFIG_ENABLED_FLAG));
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertySafeModeEnabled, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyMFPAllowed, TRUE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyMultipleBSSIDNetwork, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong(WfcPortPropertyRTSThreshold_Dummy, WFC_CONSTANT::DefaultRTSThreshold);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong(WfcPortPropertyFragmentationThreshold_Dummy, WFC_CONSTANT::DefaultFragmentationThreshold);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong(WfcPortPropertyCurrentChannelNumber, WFC_CONSTANT::DefaultChannel);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong(WfcPortPropertyCurrentBandID, 0);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong(WfcPortPropertyUnreachableThreshold_Dummy, WFC_CONSTANT::DefaultUnreachableThreshold);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong(WfcPortPropertyBeaconPeriod, 100);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;
    
    ndisStatus = SetPropertyULong(WfcPortPropertyDTIMPeriod, 1);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;
   
    ndisStatus = SetPropertyBoolean(WfcPortProperty802_11bDataRateSupport, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    if (bSetDefaultMIB)
    {
        ndisStatus = SetPropertyULong(WfcPortOSConfiguredPhyID, 
                                    0);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            goto exit;

        ndisStatus = SetPropertyList(WfcPortPropertyNamePrivacyExemptionList, 
                                    0,
                                    0,
                                    NULL);
        if (ndisStatus != NDIS_STATUS_SUCCESS)
            goto exit;
    }

    ndisStatus = SetPropertyULong( WfcPortPropertyNameTxLinkSpeed, 0xFFFFFFFFul );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong( WfcPortPropertyNameRxLinkSpeed, 0xFFFFFFFFul );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;
    ndisStatus = SetPropertyUchar( WfcPortPropertyNameLinkQuality, 0xFFu );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBuffer(WfcPortPropertyDot11MulticastList, 
                    0,
                    NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBuffer(WfcPortPropertyP2PGroupStartParameters,
        0,
        NULL );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong(WfcPortPropertyMostDesiredPhyIndex, 0);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyIhvRoamRequested, FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyULong(
        WfcPortPropertyIhvRoamReason,
        0x0000FFFFul);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBuffer(WfcPortPropertyPMKR0Name,
        0, 
        NULL );
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBuffer(WfcPortPropertyR0KHID,
        0,
        NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBuffer(WfcPortPropertySNonce,
        0,
        NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    ndisStatus = SetPropertyBoolean(WfcPortPropertyCurrentConnectionFT,
        FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;
    
    ndisStatus = SetPropertyBoolean(WfcPortPropertyTaskOffloadsDisabledForFIPS, 
        FALSE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    // WfcPortPropertyIsConnected,
    // WfcPortPropertyConnectedBSSID,

    // Apply Adapter Capabilities
    ndisStatus = m_pAdapterCache->GetPropertyBuffer(
      WfcAdapterPropertyEnhancedCapabilities,
      &uBufferLength,
      &pBuffer
      );
    if (ndisStatus == NDIS_STATUS_SUCCESS)
    {
        PDOT11_ADAPTER_CAPABILITIES pAdapterCapabilities = (PDOT11_ADAPTER_CAPABILITIES)pBuffer;
        if (pAdapterCapabilities->Dot11w_ProtectedMgmtFramesSupported != dot11_tri_state_true)
        {
            // if MFP is not supported, clear allowed
            ndisStatus = SetPropertyBoolean(WfcPortPropertyMFPAllowed, FALSE);
            if (ndisStatus != NDIS_STATUS_SUCCESS)
                goto exit;
        }
    }


exit:
    TraceExit(ndisStatus);

    return ndisStatus;        
}


NDIS_STATUS CPortPropertyCache::NdisReset()  
{
    NDIS_STATUS ndisStatus;
    TraceEntry();

    // WfcPortPropertyNamePacketFilter

    // AddressingReset in MiniportResetEx is true, so reset those values    
    ndisStatus = SetPropertyBuffer(WfcPortProperty8023MulticastList, 
                    0,
                    NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

    // Auto-power save settings is not reset on Dot11 Reset    
    ndisStatus = SetPropertyBoolean(WfcPortPropertyAutoPsm, TRUE);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;


    // Empty NLO list
    ndisStatus = SetPropertyList(WfcPortPropertyResumeNLOList, 
                    0,
                    0,
                    (BYTE *)NULL);
    if (ndisStatus != NDIS_STATUS_SUCCESS)
        goto exit;

exit:    
    TraceExit(ndisStatus);

    return ndisStatus;
        
}

NDIS_STATUS CPortPropertyCache::GetTCPOffloadParametersFromPortPropertyCache(
    _Out_ PWDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER pOffloadParameters)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    status = GetPropertyT(WfcPortPropertyChecksumV4TxEncapsulation,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV4TxIpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.IpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 IpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV4TxTcpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV4TxTcpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV4TxUdpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV4TxIpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.IpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 IpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcPortPropertyChecksumV4RxEncapsulation,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV4RxIpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.IpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 IpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV4RxTcpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV4RxTcpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV4RxUdpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV4RxIpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.IpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV4 IpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcPortPropertyChecksumV6TxEncapsulation,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV6TxIpExtensionHeadersSupported,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.IpExtensionHeadersSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 IpExtensionHeadersSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV6TxTcpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV6TxTcpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV6TxUdpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcPortPropertyChecksumV6RxEncapsulation,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV6RxIpExtensionHeadersSupported,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.IpExtensionHeadersSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 IpExtensionHeadersSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV6RxTcpOptionsSupported,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV6RxTcpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyChecksumV6RxUdpChecksum,
        &pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve ChecksumV6 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV1Encapsulation,
        &pOffloadParameters->LsoV1Capabilities.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV1 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV1MaxOffLoadSize,
        &pOffloadParameters->LsoV1Capabilities.MaxOffLoadSize);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV1 MaxOffLoadSize from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV1MinSegmentCount,
        &pOffloadParameters->LsoV1Capabilities.MinSegmentCount);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV1 MinSegmentCount from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV1TcpOptions,
        &pOffloadParameters->LsoV1Capabilities.TcpOptions);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV1 TcpOptions from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV1IpOptions,
        &pOffloadParameters->LsoV1Capabilities.IpOptions);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV1 IpOptions from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV2EncapsulationV4,
        &pOffloadParameters->LsoV2Capabilities.Ipv4LSO.EncapsulationV4);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv4 EncapsulationV4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV2MaxOffloadSizeV4,
        &pOffloadParameters->LsoV2Capabilities.Ipv4LSO.MaxOffloadSizeV4);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv4 MaxOffloadSizeV4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV2MinSegmentCountV4,
        &pOffloadParameters->LsoV2Capabilities.Ipv4LSO.MinSegmentCountV4);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv4 MinSegmentCountV4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV2EncapsulationV6,
        &pOffloadParameters->LsoV2Capabilities.Ipv6LSO.EncapsulationV6);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv6 EncapsulationV6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV2MaxOffloadSizeV6,
        &pOffloadParameters->LsoV2Capabilities.Ipv6LSO.MaxOffloadSizeV6);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv6 MaxOffloadSizeV6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV2MinSegmentCountV6,
        &pOffloadParameters->LsoV2Capabilities.Ipv6LSO.MinSegmentCountV6);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv6 MinSegmentCountV6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV2IpExtensionHeadersSupported,
        &pOffloadParameters->LsoV2Capabilities.Ipv6LSO.IpExtensionHeadersSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv6 IpExtensionHeadersSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyLargeSendOffloadV2TcpOptionsSupported,
        &pOffloadParameters->LsoV2Capabilities.Ipv6LSO.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve LSOV2 Ipv6 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = GetPropertyT(WfcPortPropertyRSCIpv4Enabled,
        &pOffloadParameters->ReceiveOffloadCapabilities.Ipv4Enabled);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve RSC Ipv4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = GetPropertyT(WfcPortPropertyRSCIpv6Enabled,
        &pOffloadParameters->ReceiveOffloadCapabilities.Ipv6Enabled);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Retrieve RSC Ipv6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

exit:
    return status;
}

NDIS_STATUS CPortPropertyCache::SetTCPOffloadParametersPortPropertyCache(
    _In_ PWDI_TCP_OFFLOAD_CAPABILITIES_CONTAINER pOffloadParameters)
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    status = SetPropertyT(WfcPortPropertyChecksumV4TxEncapsulation,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV4TxIpOptionsSupported,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.IpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 IpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV4TxTcpOptionsSupported,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV4TxTcpChecksum,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV4TxUdpChecksum,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV4TxIpChecksum,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Transmit.IpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 IpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV4RxEncapsulation,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV4RxIpOptionsSupported,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.IpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 IpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV4RxTcpOptionsSupported,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV4RxTcpChecksum,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV4RxUdpChecksum,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV4RxIpChecksum,
        pOffloadParameters->ChecksumOffload.ipv4ChecksumOffload.IpV4Receive.IpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV4 IpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = SetPropertyT(WfcPortPropertyChecksumV6TxEncapsulation,
        pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV6 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV6TxIpExtensionHeadersSupported,
        pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.IpExtensionHeadersSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV6 IpExtensionHeadersSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV6TxTcpOptionsSupported,
        pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV6 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV6TxTcpChecksum,
        pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV6 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV6TxUdpChecksum,
        pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Transmit.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV6 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = SetPropertyT(WfcPortPropertyChecksumV6RxEncapsulation,
        pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV6 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV6RxIpExtensionHeadersSupported,
        pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.IpExtensionHeadersSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV6 IpExtensionHeadersSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV6RxTcpOptionsSupported,
        pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV6 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV6RxTcpChecksum,
        pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.TcpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV6 TcpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyChecksumV6RxUdpChecksum,
        pOffloadParameters->ChecksumOffload.ipv6ChecksumOffload.IpV6Receive.UdpChecksum);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set ChecksumV6 UdpChecksum from propertycache. Error = 0x%08x", status);
        goto exit;
    }


    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV1Encapsulation,
        pOffloadParameters->LsoV1Capabilities.Encapsulation);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV1 Encapsulation from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV1MaxOffLoadSize,
        pOffloadParameters->LsoV1Capabilities.MaxOffLoadSize);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV1 MaxOffLoadSize from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV1MinSegmentCount,
        pOffloadParameters->LsoV1Capabilities.MinSegmentCount);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV1 MinSegmentCount from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV1TcpOptions,
        pOffloadParameters->LsoV1Capabilities.TcpOptions);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV1 TcpOptions from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV1IpOptions,
        pOffloadParameters->LsoV1Capabilities.IpOptions);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV1 IpOptions from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV2EncapsulationV4,
        pOffloadParameters->LsoV2Capabilities.Ipv4LSO.EncapsulationV4);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV2 Ipv4 EncapsulationV4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV2MaxOffloadSizeV4,
        pOffloadParameters->LsoV2Capabilities.Ipv4LSO.MaxOffloadSizeV4);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV2 Ipv4 MaxOffloadSizeV4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV2MinSegmentCountV4,
        pOffloadParameters->LsoV2Capabilities.Ipv4LSO.MinSegmentCountV4);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV2 Ipv4 MinSegmentCountV4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV2EncapsulationV6,
        pOffloadParameters->LsoV2Capabilities.Ipv6LSO.EncapsulationV6);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV2 Ipv6 EncapsulationV6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV2MaxOffloadSizeV6,
        pOffloadParameters->LsoV2Capabilities.Ipv6LSO.MaxOffloadSizeV6);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV2 Ipv6 MaxOffloadSizeV6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV2MinSegmentCountV6,
        pOffloadParameters->LsoV2Capabilities.Ipv6LSO.MinSegmentCountV6);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV2 Ipv6 MinSegmentCountV6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV2IpExtensionHeadersSupported,
        pOffloadParameters->LsoV2Capabilities.Ipv6LSO.IpExtensionHeadersSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV2 Ipv6 IpExtensionHeadersSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyLargeSendOffloadV2TcpOptionsSupported,
        pOffloadParameters->LsoV2Capabilities.Ipv6LSO.TcpOptionsSupported);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set LSOV2 Ipv6 TcpOptionsSupported from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyRSCIpv4Enabled,
        pOffloadParameters->ReceiveOffloadCapabilities.Ipv4Enabled);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set RSC Ipv4 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

    status = SetPropertyT(WfcPortPropertyRSCIpv6Enabled,
        pOffloadParameters->ReceiveOffloadCapabilities.Ipv6Enabled);
    if (status != NDIS_STATUS_SUCCESS)
    {
        WFCError("Couldn't Set RSC Ipv6 from propertycache. Error = 0x%08x", status);
        goto exit;
    }

exit:
    return status;
}

CBandCacheHelper::CBandCacheHelper(
    CAdapterPropertyCache* pAdapterCache,
    CPropertyCache*     pBandInfoCache
    )
{
    m_pAdapterCache = pAdapterCache;
    m_pBandInfoCache = pBandInfoCache;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CBandCacheHelper::DetectPhyID(
    _In_ ULONG ChannelNumber,
    _In_bytecount_(BeaconProbeLength)PDOT11_BEACON_FRAME pBeaconProbeFrame,
    _In_ UINT16 BeaconProbeLength,
    _Out_ ULONG* pDot11PhyID
    )
{
    UNREFERENCED_PARAMETER(pBeaconProbeFrame);
    UNREFERENCED_PARAMETER(BeaconProbeLength);
    UNREFERENCED_PARAMETER(ChannelNumber);

    // We dont do fancy stuff since the OS overwrites us anyways
    // Phy ID 0 is always present
    *pDot11PhyID = 0;

    return NDIS_STATUS_SUCCESS;
}

__success(return == NDIS_STATUS_SUCCESS)
NDIS_STATUS CBandCacheHelper::ConvertChannelNumberToFrequency(
    _In_ ULONG ChannelNumber,
    _Out_ ULONG* pChannelCenterFrequency
    )
{
    NDIS_STATUS ndisStatus = NDIS_STATUS_SUCCESS;
    ULONG numOfChannels;
    
    PWDI_CHANNEL_MAPPING_ENTRY pChannelList;

    //
    // get BandInfo pPhyTypeList property
    //
    ndisStatus = m_pBandInfoCache->GetPropertyBuffer ((WFC_PROPERTY_NAME)WfcBandInfoChannelList, 
                                                    &numOfChannels,
                                                    (BYTE**)&pChannelList);
    if ( ndisStatus != NDIS_STATUS_SUCCESS )
    {
        WFCError("Failed to get Band Channel list property. Error = 0x%08x", ndisStatus);   
        goto exit;
    } 

    ndisStatus = NDIS_STATUS_FILE_NOT_FOUND;
    
    //
    // numOfChannels is a buffer length in bytes, convert it 
    //
    numOfChannels = numOfChannels / sizeof(WDI_CHANNEL_MAPPING_ENTRY);
    for (ULONG availableChannelsIndex = 0; availableChannelsIndex < numOfChannels; availableChannelsIndex++)
    {
        //
        // if channel number
        //
        if (pChannelList[availableChannelsIndex].ChannelNumber == ChannelNumber)
        {
            *pChannelCenterFrequency = pChannelList[availableChannelsIndex].ChannelCenterFrequency;
            ndisStatus = NDIS_STATUS_SUCCESS;
            goto exit;
        }
    }  

exit:
    return ndisStatus;
}    


CPhyCacheHelper::CPhyCacheHelper(
    CAdapterPropertyCache* pAdapterCache,
    CPropertyCache*     pPhyInfoCache
    )
{
    m_pAdapterCache = pAdapterCache;
    m_pPhyInfoCache = pPhyInfoCache;
}

CPhyCacheHelper::~CPhyCacheHelper()
{
}

NDIS_STATUS CPhyCacheHelper::AddBandInfoCache(
    CPropertyCache*        pBandInfoCache
    )
{
    // Not using this today, so not adding this in
    UNREFERENCED_PARAMETER(pBandInfoCache);
    return NDIS_STATUS_SUCCESS;
}

