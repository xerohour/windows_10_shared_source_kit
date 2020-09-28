#pragma once

#include "TlvVersioningHelpers.tmh"

template<USHORT TlvTypeId, typename TlvContainer, typename TlvField>
NDIS_STATUS TlvReconstructRemovedFieldForGenerate(
    _In_ const TlvContainer * pInput,
    _In_ bool CalculateSize,
    _In_ PCTLV_CONTEXT Context,
    _Out_ TlvField * pField )
/*
   Given pInput (always the latest version of the Message or AggregateContainer), construct
   a field (container) for an earlier TLV version.

   Returns NDIS_STATUS_INDICATION_REQUIRED if the field is optional and should be included in the older TLV
   byte stream.  All other success codes are ignored.  Failure codes abort the generate API

   If the default behavior (memset to 0) is not acceptable, use template specialization to 'override'.
   See below for a specialization using ArrayOfElements.

   Called twice (once with CalculateSize true and once false), but pField is *NOT* shared between the
   two invocations.
*/
{
    UNREFERENCED_PARAMETER( pInput );
    UNREFERENCED_PARAMETER( Context );
    UNREFERENCED_PARAMETER( CalculateSize );

    memset( pField, 0, sizeof(TlvField) );
    return NDIS_STATUS_NOT_ACCEPTED;
}

template<USHORT TlvTypeId, typename TlvNewField, typename TlvOldField>
NDIS_STATUS TlvReconstructReplacedFieldForGenerate(
    _In_ bool OptionalInputIsPresent,
    _In_ const TlvNewField * pInput,
    _In_ bool CalculateSize,
    _In_ PCTLV_CONTEXT Context,
    _Out_ TlvOldField * pField )
/*
    Given pInput (always the latest version of the container), construct
    a field (container) for an earlier TLV version.

    Returns NDIS_STATUS_INDICATION_REQUIRED if the field is optional and should be included in the older TLV
    byte stream.  All other success codes are ignored.  Failure codes abort the generate API

    If the default behavior (memcpy from old to new) is not acceptable, use template specialization to 'override'.
    See below for a specialization using ArrayOfElements.
*/
{
    NDIS_STATUS status;

    UNREFERENCED_PARAMETER( Context );
    UNREFERENCED_PARAMETER( CalculateSize );

    memset( pField, 0, sizeof(TlvOldField) );
    if (OptionalInputIsPresent)
    {
#pragma warning ( suppress : 4127 ) // Conditional expression is constant
        memcpy( pField, pInput, min( sizeof(TlvNewField), sizeof(TlvOldField) ) );
        // Need to clear out any InternallyAllocated fields
        WDI_TLV::PARSER::MarkArrayOfElementFieldsAsCopied( pField );
        status =  NDIS_STATUS_INDICATION_REQUIRED;
    }
    else
    {
        // A success code that says to skip this field if it is optional
        status = NDIS_STATUS_NOT_ACCEPTED;
    }

    return status;
}

template<USHORT TlvTypeId, typename TlvContainer, typename TlvArrayField>
NDIS_STATUS TlvReconstructRemovedFieldForGenerate(
    _In_ const TlvContainer * pInput,
    _In_ bool CalculateSize,
    _In_ PCTLV_CONTEXT Context,
    _Out_ ArrayOfElements<TlvArrayField> * pField )
/*
    Specialization for ArrayOfElements which allocates a single element (in case it is
    non-optional), but does NOT return NDIS_STATUS_INDICATION_REQUIRED, so that if it
    is optional, it will be skipped.
*/
{
    NDIS_STATUS status = NDIS_STATUS_SUCCESS;

    UNREFERENCED_PARAMETER( pInput );
    UNREFERENCED_PARAMETER( CalculateSize );

    status = pField->AllocateElements( 1, Context->AllocationContext );

    if (NULL != pField->pElements)
    {
        memset( pField->pElements, 0, sizeof(TlvArrayField) * pField->ElementCount );
    }

    return status;
}

template<USHORT TlvTypeId, typename TlvNewField, typename TlvOldField>
NDIS_STATUS TlvReconstructReplacedFieldForGenerate(
    _In_ bool OptionalInputIsPresent,
    _In_ const TlvNewField * pInput,
    _In_ bool CalculateSize,
    _In_ PCTLV_CONTEXT Context,
    _Out_ ArrayOfElements<TlvOldField> * pField )
/*
    Specialization for when old is ArrayOfElements which copies first element.
*/
{
    NDIS_STATUS status;

    UNREFERENCED_PARAMETER( CalculateSize );

    if (OptionalInputIsPresent)
    {
        status = pField->AllocateElements( 1, Context->AllocationContext );
        if (!NT_SUCCESS( status ))
        {
            goto exit;
        }
        // Copy as much as we can of only field
#pragma warning ( suppress : 4127 25042 ) // Conditional expression is constant
        if (sizeof(TlvOldField) > sizeof(TlvNewField))
        {
            memset( &(pField->pElements[0]), 0, sizeof(TlvOldField) );
        }
#pragma warning ( suppress : 4127 ) // Conditional expression is constant
        memcpy( &(pField->pElements[0]), pInput, min( sizeof(TlvNewField), sizeof(TlvOldField) ) );
        status = NDIS_STATUS_INDICATION_REQUIRED;
    }
    else
    {
        status = pField->AllocateElements( 1, Context->AllocationContext );

        if (NULL != pField->pElements)
        {
            memset( pField->pElements, 0, sizeof( TlvOldField ) * pField->ElementCount );
        }

        // A success code that says to skip this field if it is optional
        status = NDIS_STATUS_NOT_ACCEPTED;
    }
exit:
    return status;
}

template<USHORT TlvTypeId, typename TlvNewField, typename TlvOldField>
NDIS_STATUS TlvReconstructReplacedFieldForGenerate(
    _In_ bool OptionalInputIsPresent,
    _In_ const ArrayOfElements<TlvNewField> * pInput,
    _In_ bool CalculateSize,
    _In_ PCTLV_CONTEXT Context,
    _Out_ TlvOldField * pField )
/*
    Specialization for when new is ArrayOfElements which copies the first element
*/
{
    NDIS_STATUS status;

    UNREFERENCED_PARAMETER( Context );
    UNREFERENCED_PARAMETER( CalculateSize );

    if (OptionalInputIsPresent && (pInput->ElementCount > 0))
    {
        // Copy as much as we can of first field
        memset( pField, 0, sizeof(TlvOldField) );
#pragma warning ( suppress : 4127 ) // Conditional expression is constant
        memcpy( pField, &(pInput->pElements[0]), min( sizeof(TlvNewField), sizeof(TlvOldField) ) );
        status = NDIS_STATUS_INDICATION_REQUIRED;
    }
    else
    {
        memset( pField, 0, sizeof(TlvOldField) );
        // A success code that says to skip this field if it is optional
        status = NDIS_STATUS_NOT_ACCEPTED;
    }
    return status;
}

template<USHORT TlvTypeId, typename TlvNewField, typename TlvOldField>
NDIS_STATUS TlvReconstructReplacedFieldForGenerate(
    _In_ bool OptionalInputIsPresent,
    _In_ const ArrayOfElements<TlvNewField> * pInput,
    _In_ bool CalculateSize,
    _In_ PCTLV_CONTEXT Context,
    _Out_ ArrayOfElements<TlvOldField> * pField )
/*
    Specialization for when both old and new are ArrayOfElements which copies each element.
*/
{
    NDIS_STATUS status;

    UNREFERENCED_PARAMETER( CalculateSize );

    if (OptionalInputIsPresent && (pInput->ElementCount > 0))
    {
        status = pField->AllocateElements( pInput->ElementCount, Context->AllocationContext );
        if (!NT_SUCCESS( status ))
        {
            goto exit;
        }
        // Copy as much as we can of each field, using the template for a single field
        for (UINT32 i = 0; i < pInput->ElementCount; i++)
        {
            status = TlvReconstructReplacedFieldForGenerate<TlvTypeId>( true, &(pInput->pElements[i]), CalculateSize, Context, &(pField->pElements[i]) );
            if (!NT_SUCCESS( status ))
            {
                goto exit;
            }
        }
        status = NDIS_STATUS_INDICATION_REQUIRED;
    }
    else
    {
        status = pField->AllocateElements( 1, Context->AllocationContext );

        if (NULL != pField->pElements)
        {
            memset( pField->pElements, 0, sizeof( TlvOldField ) * pField->ElementCount );
        }

        // A success code that says to skip this field if it is optional
        status = NDIS_STATUS_NOT_ACCEPTED;
    }
exit:
    return status;
}

template<USHORT TlvTypeId, typename TlvContainer, typename TlvField>
NDIS_STATUS TlvConvertRemovedFieldFromParse(
    _In_opt_ TlvField * pField,
    _In_ PCTLV_CONTEXT Context,
    _Inout_ TlvContainer * pParsedOutput )
/*
    Given pField which was parsed from an older version TLV stream, and pParsedOutput which is the
    latest version of the TLV data structure, update pParsedOutput with any data from pField as needed.
    This is called after all of the non-removed fields have been parsed, and will be called once for
    each child TLV field that was in the old stream, but is now removed.

    If the field was optional, and not present, this will be called with pField==NULL.

    The default behavior is to do nothing.
*/
{
    UNREFERENCED_PARAMETER( pField );
    UNREFERENCED_PARAMETER( Context );
    UNREFERENCED_PARAMETER( pParsedOutput );

    return NDIS_STATUS_NOT_COPIED;
}

template<USHORT TlvTypeId, typename TlvNewField, typename TlvOldField>
NDIS_STATUS TlvConvertReplacedFieldFromParse(
    _In_opt_ TlvOldField * pField,
    _In_ PCTLV_CONTEXT Context,
    _Inout_ TlvNewField * pParsedOutput)
/*
    Given pField which was parsed from an older version TLV stream, and pParsedOutput which is the
    latest version of the TLV data structure, update pParsedOutput with any data from pField as needed.
    This is called after all of the non-removed fields have been parsed, and will be called once for
    each child TLV field that was in the old stream, but is now replaced by a new field.

    If the field was optional, and not present, this will be called with pField==NULL.

    The default behavior is to copy from pField to pParsedOutput.

    Use template specialization to 'override' the default behavior (see below for example for ArrayOfElements)
*/
{
    NDIS_STATUS status = NDIS_STATUS_NOT_COPIED;

    UNREFERENCED_PARAMETER( Context );

    if (pField != NULL)
    {
#pragma warning ( suppress : 4127 ) // Conditional expression is constant
        memcpy( pParsedOutput, pField, min( sizeof(TlvNewField), sizeof(TlvOldField) ) );
        // Need to clear out any InternallyAllocated fields
        WDI_TLV::PARSER::MarkArrayOfElementFieldsAsCopied( pField );
        status = NDIS_STATUS_SUCCESS;
    }

    return status;
}

template<USHORT TlvTypeId, typename TlvNewField, typename TlvOldField>
NDIS_STATUS TlvConvertReplacedFieldFromParse(
    _In_opt_ TlvOldField * pField,
    _In_ PCTLV_CONTEXT Context,
    _Inout_ ArrayOfElements<TlvNewField> * pParsedOutput )
/*
    Override for when old is a single field and new is ArrayOfElements,
*/
{
    NDIS_STATUS status = NDIS_STATUS_NOT_COPIED;

    UNREFERENCED_PARAMETER( Context );

    if (pField != NULL)
    {
        status = pParsedOutput->AllocateElements( 1, Context->AllocationContext );
        if (!NT_SUCCESS( status ))
        {
            goto exit;
        }
        // Copy as much as we can of the first field
#pragma warning ( suppress : 4127 25042 ) // Conditional expression is constant
        if (sizeof(TlvNewField) > sizeof(TlvOldField))
        {
            memset( &(pParsedOutput->pElements[0]), 0, sizeof(TlvNewField) );
        }
#pragma warning ( suppress : 4127 ) // Conditional expression is constant
        memcpy( &(pParsedOutput->pElements[0]), pField, min( sizeof(TlvNewField), sizeof(TlvOldField) ) );
        status = NDIS_STATUS_SUCCESS;
    }

exit:
    return status;
}

template<USHORT TlvTypeId, typename TlvNewField, typename TlvOldField>
NDIS_STATUS TlvConvertReplacedFieldFromParse(
    _In_opt_ ArrayOfElements<TlvOldField> * pField,
    _In_ PCTLV_CONTEXT Context,
    _Inout_ TlvNewField * pParsedOutput )
/*
    Override for when old is an ArrayOfElements and new is a single field
*/
{
    NDIS_STATUS status = NDIS_STATUS_NOT_COPIED;

    UNREFERENCED_PARAMETER( Context );

    if ((pField != NULL) && (pField->ElementCount > 0))
    {
        // Copy as much as we can of the first field
#pragma warning ( suppress : 4127 ) // Conditional expression is constant
        memcpy( pParsedOutput, &(pField->pElements[0]), min( sizeof(TlvNewField), sizeof(TlvOldField) ) );
        status = NDIS_STATUS_SUCCESS;
    }

    return status;
}

template<USHORT TlvTypeId, typename TlvNewField, typename TlvOldField>
NDIS_STATUS TlvConvertReplacedFieldFromParse(
    _In_opt_ ArrayOfElements<TlvOldField> * pField,
    _In_ PCTLV_CONTEXT Context,
    _Inout_ ArrayOfElements<TlvNewField> * pParsedOutput )
/*
    Override for when old and new are both ArrayOfElements
*/
{
    NDIS_STATUS status = NDIS_STATUS_NOT_COPIED;

    UNREFERENCED_PARAMETER( TlvTypeId );
    UNREFERENCED_PARAMETER( Context );

    if ((pField != NULL) && (pField->ElementCount > 0))
    {
        status = pParsedOutput->AllocateElements( pField->ElementCount, Context->AllocationContext );
        if (!NT_SUCCESS( status ))
        {
            goto exit;
        }
        // Copy as much as we can of each field, using the template
        for (UINT32 i = 0; i < pField->ElementCount; i++)
        {
            status = TlvConvertReplacedFieldFromParse<TlvTypeId>( &(pField->pElements[i]), Context, &(pParsedOutput->pElements[i]) );
            if (!NT_SUCCESS( status ))
            {
                goto exit;
            }
        }
        status = NDIS_STATUS_SUCCESS;
    }

exit:
    return status;
}

//
// We need specialized versions because the fields got re-ordered
//
#define CopyOverFields( _pDest, _pInput, _DEST_TYPE, _INPUT_TYPE, _ThroughField, _FromField)    \
{                                                                                               \
    DWORD   _dwBytesToCopy;                                                                     \
    memcpy( _pDest, _pInput, RTL_SIZEOF_THROUGH_FIELD( _DEST_TYPE, _ThroughField ) );           \
    _dwBytesToCopy = min( (sizeof( _DEST_TYPE ) - FIELD_OFFSET( _DEST_TYPE, _FromField )),      \
                          (sizeof( _INPUT_TYPE ) - FIELD_OFFSET( _INPUT_TYPE, _FromField )));   \
    memcpy( ((BYTE*)_pDest) + FIELD_OFFSET( _DEST_TYPE, _FromField ),                           \
            ((BYTE*)_pInput) + FIELD_OFFSET( _INPUT_TYPE, _FromField ),                         \
            _dwBytesToCopy);                                                                    \
}

