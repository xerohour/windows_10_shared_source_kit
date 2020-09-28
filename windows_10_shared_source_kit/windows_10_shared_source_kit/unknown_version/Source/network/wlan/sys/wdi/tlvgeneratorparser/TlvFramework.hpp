#pragma once

#include "TlvFramework.tmh"

namespace WDI_TLV
{
    namespace ENUMS
    {
        enum TLV_ID : UINT16
        {
#define UNKNOWN_TLV 0xFFFF
#define DUMPER_TLV_ID_1( x ) x = 
#define DUMPER_TLV_ID_2( y )     y,
#include "TlvGeneratedEnums_.hpp"
#undef UNKNOWN_TLV
        };

        enum MESSAGE_ID : UINT16
        {
#define UNKNOWN_TLV 0xFFFF
#define DUMPER_MSG_ID_1( x ) x = 
#define DUMPER_MSG_ID_2( y )     y,
#include "TlvGeneratedEnums_.hpp"
#undef UNKNOWN_TLV
        };

        enum FIELD_NAME : DUMPER_FIELD_NAME_ENUM_TYPE
        {
#define DUMPER_FIELD_NAME( x ) x,
#include "TlvGeneratedEnums_.hpp"
        };

        enum ENCODED_TYPE_NAME : DUMPER_TYPE_NAME_ENUM_TYPE
        {
#define DUMPER_TYPE_NAME( x ) x,
#include "TlvGeneratedEnums_.hpp"
        };
    };

    typedef struct _MESSAGE_OR_TLV_ID
    {
        _MESSAGE_OR_TLV_ID( ENUMS::TLV_ID tlvId ) : fIsMessage( FALSE ), TlvId( tlvId ) {}
        _MESSAGE_OR_TLV_ID( ENUMS::MESSAGE_ID msgId ) : fIsMessage( TRUE ), MsgId( msgId ) {}

        BOOLEAN fIsMessage;
        union
        {
            ENUMS::MESSAGE_ID MsgId;
            ENUMS::TLV_ID     TlvId;
        };
    } MESSAGE_OR_TLV_ID;

    C_ASSERT( sizeof( MESSAGE_OR_TLV_ID ) == 4 ); // Cram it all into a single DWORD

    typedef struct _FIELD_INFO
    {
        UINT16                   unpackedOffset;      // Offset within C struct of the field(s)
        UINT16                   packedLen;           // size in bytes of the field(s) within the TLV stream
        ENUMS::FIELD_NAME        fieldName;
        ENUMS::ENCODED_TYPE_NAME typeName;
        UINT8                    count;
    } FIELD_INFO;

    typedef struct _CONTAINER_INFO
    {
        _Field_size_( cntFields )
            const FIELD_INFO        * rgFields;
        UINT16                        cntFields;
        UINT16                        cbPackedSize;   // Length in bytes of a single  container
        UINT16                        cbUnpackedSize; // Sizeof the C struct (if fIsCollection, the size of a single element)
        // A little bit packing so we can fit nicely
        // And we don't have more than 4K ArrayOfElement types
        UINT16                        fIsCollection : 1;
        UINT16                        fIsZeroAllowed : 1;
        UINT16                        fDumpAsSingle : 1;
        UINT16                        fPrivate : 1;
        UINT16                        ArrayOfElementsTypeIndex : 12;
    } CONTAINER_INFO;

    typedef struct _VERSION_INFO
    {
        typedef NDIS_STATUS( *pfnTlvReconstructRemovedFieldForGenerate )(
            _In_ const void * pInput,
            _In_ bool CalculateSize,
            _In_ PCTLV_CONTEXT Context,
            _Out_ void * pOutput);
        typedef NDIS_STATUS( *pfnTlvReconstructReplacedFieldForGenerate )(
            _In_ bool OptionalInputIsPresent,
            _In_ const void * pInput,
            _In_ bool CalculateSize,
            _In_ PCTLV_CONTEXT Context,
            _Out_ void * pField);
        typedef NDIS_STATUS( *pfnTlvConvertRemovedFieldFromParse )(
            _In_opt_ void * pField,
            _In_ PCTLV_CONTEXT Context,
            _Inout_ void * pParsedOutput);
        typedef NDIS_STATUS( *pfnTlvConvertReplacedFieldFromParse )(
            _In_opt_ void * pField,
            _In_ PCTLV_CONTEXT Context,
            _Inout_ void * pParsedOutput);

        pfnTlvReconstructRemovedFieldForGenerate ReconstructRemovedFieldForGenerate;
        pfnTlvReconstructReplacedFieldForGenerate ReconstructReplacedFieldForGenerate;
        pfnTlvConvertRemovedFieldFromParse ConvertRemovedFieldFromParse;
        pfnTlvConvertReplacedFieldFromParse ConvertReplacedFieldFromParse;
        ULONG versionAdded;  // First version where this containerRef was added (inclusive), 0 if never added
        ULONG versionRemoved;  // First version where this containerRef was removed (inclusive), 0xFFFFFFFF if never removed
        ULONG iFieldCurrentReplacement; // Index within parent aggregate container
    } VERSION_INFO;

    typedef struct _ARRAY_HELPERS
    {
        typedef NDIS_STATUS( *pfnAllocateElements )(
            _In_ VOID * pvArrayOfElemenetsThisPtr,
            _In_ UINT32 SourceElementCount,
            _In_ ULONG_PTR AllocationContext);

        typedef VOID( *pfnCleanup )(
            _In_ VOID * pvArrayOfElemenetsThisPtr);

        pfnAllocateElements AllocateElements;
        pfnCleanup Cleanup;
    } ARRAY_HELPERS;

    extern const ARRAY_HELPERS g_ArrayHelpers[];

    struct _CONTAINER_REF_INFO;

    typedef struct _AGGREGATE_INFO
    {
        _Field_size_( cntFields )
            const struct _CONTAINER_REF_INFO * rgFields;
        UINT16                                 cntFields;
        UINT16                                 cbUnpackedSize;
    } AGGREGATE_INFO;

    typedef struct _CONTAINER_REF_INFO
    {
        union
        {
            const CONTAINER_INFO * simpleInfo;
            const AGGREGATE_INFO * nestedInfo;
        };
        const VERSION_INFO *       versionInfo;
        ENUMS::TLV_ID              tlvType;
        UINT16                     cbUnpackedOffset;
        // A little bit packing so we can fit nicely
        // And we don't have more than 16K ArrayOfElement types
        UINT16                     fIsSimple : 1;
        UINT16                     fIsMulti : 1;
        UINT16                     ArrayOfElementsTypeIndex : 14;
        // More bit packing so we fit nicely
        // And we don't need more than 32K optional fields per cotnainer
        UINT16                     fIsOptional : 1;
        UINT16                     iOptionalBitOffset : 15;

        // Returns the size of the field within the C structure (not the element size if multi or collection)
        UINT16                     GetUnpackedFieldSize() const
        {
            if (fIsMulti || (fIsSimple && simpleInfo->fIsCollection))
            {
                return sizeof( ArrayOfElements<BYTE> );
            }
            if (fIsSimple)
            {
                return simpleInfo->cbUnpackedSize;
            }
            return nestedInfo->cbUnpackedSize;
        }
        // Returns the size of the field within the C structure (if Multi the size of a single element)
        UINT16                     GetUnpackedFieldOrElementSize() const
        {
            if (fIsSimple)
            {
                if (simpleInfo->fIsCollection)
                {
                    return sizeof( ArrayOfElements<BYTE> );
                }
                return simpleInfo->cbUnpackedSize;
            }
            return nestedInfo->cbUnpackedSize;
        }

    } CONTAINER_REF_INFO;

    namespace DUMPER
    {
        typedef struct _CONTAINER_REF
        {
            ULONG  OuterTlvOrMsgId;
            ULONG  VersionAdded;
            ULONG  VersionRemoved;

            USHORT fIsMessage : 1;      // Is the outer container a message (or an aggregate)?
            USHORT fToIhv : 1;          // If the outer container is a message, is it ToIhv (or FromIhv)?
        } CONTAINER_REF;

        typedef struct _CONTEXT
        {
            ULONG  OuterTlvOrMsgId;
            ULONG  TlvVersion;
            UINT_PTR DumpContext;
            TlvDumperCallback pCallback;
            UCHAR IndentLevel;
            bool fIsMessage;
            bool fToIhv;
            bool fKnownOuter;
        } CONTEXT;

        _Function_class_( WDI_TLV_DUMPER_ENUM_API )
        typedef LPCSTR( *pfnMapEnumToStringApi )(
            _In_ ULONG enumValue);

        struct INFO
        {
            size_t                          cRefs;
            _Field_size_part_opt_( 0, cRefs )
            CONTAINER_REF const *           rgRefs;
            CONTAINER_INFO const *          pInfo;
        };

        struct MAP_ENTRY
        {
            UINT16          TlvType;
            UINT16          cDumpers;
            _Field_size_part_opt_( 0, cDumpers )
                INFO const *    rgDumpers;
        };

        // Go through function pointers, so that if users of the library
        // do *NOT* have to pay the cost of these maps, when they only use
        // WPP which can acocmplish the same thing via .PDBs
        extern pfnMapEnumToStringApi s_pfnMapTlvIdToString;
        extern pfnMapEnumToStringApi s_pfnMapFieldNameToString;
        extern pfnMapEnumToStringApi s_pfnMapTypeNameToString;
        _Function_class_( WDI_TLV_DUMPER_ENUM_API ) LPCSTR MapTlvIdToString( _In_ ULONG enumValue );
        _Function_class_( WDI_TLV_DUMPER_ENUM_API ) LPCSTR MapFieldNameToString( _In_ ULONG enumValue );
        _Function_class_( WDI_TLV_DUMPER_ENUM_API ) LPCSTR MapTypeNameToString( _In_ ULONG enumValue );


        extern const MAP_ENTRY s_rgAllDumpers[];
        extern const size_t s_cAllDumpers;

        void DumpKey( _Inout_ CONTEXT * pctx, _In_z_ LPCSTR szKey );
        void DumpZeroNotAllowed( _Inout_ CONTEXT *pctx );
        void DumpMissingField( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field );
        void DumpMissingField( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index );
        void DumpKeyValueBool( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, BOOLEAN Value );
        void DumpKeyValueBool( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, BOOLEAN Value );
        void DumpKeyValueString( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, LPCSTR szValue );
        void DumpKeyValueString( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, LPCSTR szValue );
        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT8 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::UINT8 );
        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT16 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::UINT16 );
        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT32 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::UINT32 );
        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT64 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::UINT64 );
        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, INT8 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::INT8 );
        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, INT16 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::INT16 );
        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, INT32 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::INT32 );
        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, INT64 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::INT64 );

        void DumpKeyArrayValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, UINT8 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::UINT8 );
        void DumpKeyArrayValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, UINT16 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::UINT16 );
        void DumpKeyArrayValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, UINT32 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::UINT32 );
        void DumpKeyArrayValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, UINT64 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::UINT64 );
        void DumpKeyArrayValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, INT8 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::INT8 );
        void DumpKeyArrayValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, INT16 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::INT16 );
        void DumpKeyArrayValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, INT32 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::INT32 );
        void DumpKeyArrayValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, INT64 Value, ENUMS::ENCODED_TYPE_NAME type = ENUMS::INT64 );

        void DumpKeyValueWithString( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, LPCSTR szValue, UINT16 Value );
        void DumpKeyBlobAsString( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer );
        void DumpKeyBlob( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer );
        void DumpKeyBlob( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer );
        void DumpTypedKeyBlob( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, ENUMS::ENCODED_TYPE_NAME type, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer );
        void DumpTypedKeyBlob( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, ENUMS::ENCODED_TYPE_NAME type, UINT index, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer );
        void DumpPrivateKeyBlob( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer );

        template <class ContentType>
        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, const UNALIGNED ContentType * pValue )
        {
#pragma warning(suppress: 4127) // Conditional Express is constant - we want it to be constant!
            if (sizeof( ContentType ) == 1)
            {
                DumpKeyValue( pctx, field, *(const UNALIGNED UINT8*)pValue );
            }
#pragma warning(suppress: 4127) // Conditional Express is constant - we want it to be constant!
            else if (sizeof( ContentType ) == 2)
            {
                DumpKeyValue( pctx, field, *(const UNALIGNED UINT16*)pValue );
            }
#pragma warning(suppress: 4127) // Conditional Express is constant - we want it to be constant!
            else if (sizeof( ContentType ) == 4)
            {
                DumpKeyValue( pctx, field, *(const UNALIGNED UINT32*)pValue );
            }
#pragma warning(suppress: 4127) // Conditional Express is constant - we want it to be constant!
            else if (sizeof( ContentType ) == 8)
            {
                DumpKeyValue( pctx, field, *(const UNALIGNED UINT64*)pValue );
            }
            else
            {
                DumpKeyBlob( pctx, field, sizeof( ContentType ), (const UNALIGNED BYTE *)pValue );
            }
        }

        template <class ContentType>
        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, const UNALIGNED ContentType * pValue, ENUMS::ENCODED_TYPE_NAME type )
        {
#pragma warning(suppress: 4127) // Conditional Express is constant - we want it to be constant!
            if (sizeof( ContentType ) == 1)
            {
                DumpKeyValue( pctx, field, *(const UNALIGNED UINT8*)pValue, type );
            }
#pragma warning(suppress: 4127) // Conditional Express is constant - we want it to be constant!
            else if (sizeof( ContentType ) == 2)
            {
                DumpKeyValue( pctx, field, *(const UNALIGNED UINT16*)pValue, type );
            }
#pragma warning(suppress: 4127) // Conditional Express is constant - we want it to be constant!
            else if (sizeof( ContentType ) == 4)
            {
                DumpKeyValue( pctx, field, *(const UNALIGNED UINT32*)pValue, type );
            }
#pragma warning(suppress: 4127) // Conditional Express is constant - we want it to be constant!
            else if (sizeof( ContentType ) == 8)
            {
                DumpKeyValue( pctx, field, *(const UNALIGNED UINT64*)pValue, type );
            }
            else
            {
                DumpTypedKeyBlob( pctx, field, type, sizeof( ContentType ), (const UNALIGNED BYTE *)pValue );
            }
        }

        template <class FieldType>
        void DumpSimpleField(
            ENUMS::FIELD_NAME field,
            _Inout_ ULONG * pBufferLength,
            _Deref_pre_bytecount_( *pBufferLength ) _Deref_post_bytecount_( *pBufferLength ) UINT8 const ** ppTlvData,
            _Inout_ CONTEXT * Context )
        {
            if (*pBufferLength < sizeof( FieldType ))
            {
                DumpMissingField( Context, field );
                if (*pBufferLength > 0)
                {
                    DumpKeyBlob( Context, ENUMS::PartialFieldData, *pBufferLength, *ppTlvData );
                    *ppTlvData += *pBufferLength;
                    *pBufferLength = 0;
                }
            }
            else
            {
                DumpKeyValue( Context, field, *(const UNALIGNED FieldType*)(*ppTlvData) );
                *ppTlvData += sizeof( FieldType );
                *pBufferLength -= sizeof( FieldType );
            }
        }

        template <class FieldType, class EncodedType>
        void DumpOtherField(
            ENUMS::FIELD_NAME field,
            ENUMS::ENCODED_TYPE_NAME type,
            _Inout_ ULONG * pBufferLength,
            _Deref_pre_bytecount_( *pBufferLength ) _Deref_post_bytecount_( *pBufferLength ) UINT8 const ** ppTlvData,
            _Inout_ CONTEXT * Context )
        {
            C_ASSERT( sizeof( EncodedType ) == sizeof( FieldType ) );
            if (*pBufferLength < sizeof( EncodedType ))
            {
                DumpMissingField( Context, field );
                if (*pBufferLength > 0)
                {
                    DumpKeyBlob( Context, ENUMS::PartialFieldData, *pBufferLength, *ppTlvData );
                    *ppTlvData += *pBufferLength;
                    *pBufferLength = 0;
                }
            }
            else
            {
                DumpKeyValue( Context, field, (const UNALIGNED FieldType*)(*ppTlvData), type );
                *ppTlvData += sizeof( EncodedType );
                *pBufferLength -= sizeof( EncodedType );
            }
        }

        void DumpBoolField(
            ENUMS::FIELD_NAME field,
            _Inout_ ULONG * pBufferLength,
            _Deref_pre_bytecount_( *pBufferLength ) _Deref_post_bytecount_( *pBufferLength ) UINT8 const ** ppTlvData,
            _Inout_ CONTEXT * Context );

        void DumpBoolField(
            ENUMS::FIELD_NAME field,
            UINT index,
            _Inout_ ULONG * pBufferLength,
            _Deref_pre_bytecount_( *pBufferLength ) _Deref_post_bytecount_( *pBufferLength ) UINT8 const ** ppTlvData,
            _Inout_ CONTEXT * Context );

        void DumpByteArrayHelper(
            _In_ USHORT fZeroAllowed,
            _In_ USHORT fPrivate,
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _Inout_ CONTEXT * Context );

        void DumpStringHelper(
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _Inout_ CONTEXT * Context );

        void DumpStringHelperZeroAllowed(
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _Inout_ CONTEXT * Context );

        template<class ContentType>
        void DumpSimpleNamedType(
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _Inout_ CONTEXT * Context )
        {
            if (sizeof( ContentType ) > DataLength)
            {
                DumpMissingField( Context, ENUMS::UNKNOWN_FIELD );
            }
            else
            {
                DumpKeyValue<ContentType>( Context, ENUMS::Value, (const UNALIGNED ContentType*)pTlvData );
                DataLength -= sizeof( ContentType );
                pTlvData += sizeof( ContentType );
            }
            if (DataLength > 0)
            {
                DumpKeyBlob( Context, ENUMS::PartialFieldData, DataLength, pTlvData );
            }
        }

        void DumpTlvs(
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _Inout_ CONTEXT * pContext );
    }; // namespace DUMPER

    namespace PARSER
    {
        NDIS_STATUS ParseEmptyMessage(
            _In_ ULONG                                     BufferLength,
            _In_reads_bytes_( BufferLength ) UINT8 const * pBuffer,
            _In_ PCTLV_CONTEXT                             Context,
            _Out_ EmptyMessageStructureType              * pParsedOutput );

        NDIS_STATUS FindTlv(
            _In_ UINT16 TlvType,
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _Out_opt_ UINT16* pDesiredTlvLength,
            _Outptr_opt_result_buffer_maybenull_( *pDesiredTlvLength ) BYTE const ** pDesiredTlvData,
            _Out_opt_ ULONG *pNewDataLength,
            _Outptr_opt_result_buffer_maybenull_( *pNewDataLength ) BYTE const ** pNewTlvData
            );

        NDIS_STATUS ParseSimpleContainer(
            _In_ const ULONG                        cbBuffer,
            _In_reads_( cbBuffer ) const BYTE     * rgBuffer,
            _In_ ENUMS::TLV_ID                      tlvType,
            _In_ const CONTAINER_INFO             & cont,
            _In_ PCTLV_CONTEXT                      Context,
            _In_ size_t                             cbOutput,
            _Inout_updates_bytes_( cbOutput ) PVOID pOutput );

        NDIS_STATUS ParseMessage(
            _In_ const ULONG                        cbBuffer,
            _In_reads_( cbBuffer ) const BYTE     * rgBuffer,
            _In_ ENUMS::MESSAGE_ID                  msgId,
            _In_ const AGGREGATE_INFO             & cont,
            _In_ PCTLV_CONTEXT                      Context,
            _In_ size_t                             cbOutput,
            _Out_writes_bytes_( cbOutput )  PVOID   pOutput );


        NDIS_STATUS ParseAggregateContainer(
            _In_ const ULONG                        cbBuffer,
            _In_reads_( cbBuffer ) const BYTE     * rgBuffer,
            _In_ MESSAGE_OR_TLV_ID                  tlvType,
            _In_ const AGGREGATE_INFO             & cont,
            _In_ PCTLV_CONTEXT                      Context,
            _In_ size_t                             cbOutput,
            _Inout_updates_bytes_( cbOutput ) PVOID pOutput );

        NDIS_STATUS ParseAggregateField(
            _In_ const ULONG                        cbBuffer,
            _In_reads_( cbBuffer ) const BYTE     * rgBuffer,
            _In_ MESSAGE_OR_TLV_ID                  tlvType,
            _In_ const CONTAINER_REF_INFO         & field,
            _In_ PCTLV_CONTEXT                      Context,
            _In_ size_t                             cbOutput,
            _Inout_updates_bytes_( cbOutput ) PVOID pOutput );

        void CleanupAllocations(
            _In_ const UINT16                    cbOutput,
            _In_reads_( cbOutput ) BYTE        * pbOutput,
            _In_ const CONTAINER_REF_INFO      & field );

        NDIS_STATUS AllocateAndParseOldAggregateField(
            _In_ const ULONG                        cbBuffer,
            _In_reads_( cbBuffer ) const BYTE     * rgBuffer,
            _In_ MESSAGE_OR_TLV_ID                  tlvType,
            _In_ const CONTAINER_REF_INFO         & oldField,
            _In_opt_ const CONTAINER_REF_INFO     * pNewField,
            _In_ PCTLV_CONTEXT                      Context,
            _In_ size_t                             cbOutput,
            _Inout_updates_bytes_( cbOutput ) PVOID pOutput );

    }; // namespace PARSER

    namespace GENERATOR
    {
        NDIS_STATUS IsOptionalAggregateFieldPresent(
            _In_ UINT16                                       cbInputAggregate,
            _In_reads_bytes_( cbInputAggregate ) const VOID * pInputAggregate,
            _In_ const CONTAINER_REF_INFO                   & field );

        NDIS_STATUS GenerateSingleContainer(
            _In_ UINT16                              cbField,
            _In_reads_bytes_( cbField ) const VOID * pField,
            _In_ const CONTAINER_REF_INFO          & field,
            _In_ PCTLV_CONTEXT                       Context,
            _Inout_ UINT16                         * pcbSize,
            _Out_writes_opt_( *pcbSize ) BYTE      * pbTlv );

        NDIS_STATUS GenerateMultiContainer(
            _In_ UINT16                              cbField,
            _In_reads_bytes_( cbField ) const VOID * pField,
            _In_ const CONTAINER_REF_INFO          & field,
            _In_ PCTLV_CONTEXT                       Context,
            _Inout_ UINT32                         * pcbSize,
            _Out_writes_opt_( *pcbSize ) BYTE      * pbTlv );

        NDIS_STATUS AllocateAndGenerateOldContainer(
            _In_ UINT16                              cbInput,
            _In_reads_bytes_( cbInput ) const VOID * pInput,
            _In_ const CONTAINER_REF_INFO          & oldField,
            _In_ const CONTAINER_REF_INFO          * newField,
            _In_ PCTLV_CONTEXT                       Context,
            _Inout_ UINT32                         * pcbSize,
            _Out_writes_opt_( *pcbSize ) BYTE      * pbTlv );

        NDIS_STATUS GenerateAggregate(
            _In_ UINT16                              cbInput,
            _In_reads_bytes_( cbInput ) const VOID * pInput,
            _In_ const AGGREGATE_INFO              & cont,
            _In_ PCTLV_CONTEXT                       Context,
            _Inout_ UINT32                         * pcbSize,
            _Out_writes_opt_( *pcbSize ) BYTE      * pbTlv );

        NDIS_STATUS GenerateMessage(
            _In_ UINT16                                cbInput,
            _In_reads_bytes_( cbInput ) const VOID   * pInput,
            _In_ ENUMS::MESSAGE_ID                     msgId,
            _In_ const AGGREGATE_INFO                & cont,
            _In_ ULONG                                 cbReserve,
            _In_ PCTLV_CONTEXT                         Context,
            _Out_ ULONG                              * pcbSize,
            _Outptr_result_buffer_( *pcbSize ) BYTE ** ppbTlv );

        NDIS_STATUS GenerateEmptyMessage(
            _In_opt_ void const *                      pInput,
            _In_ ULONG                                 cbReserve,
            _In_ PCTLV_CONTEXT                         Context,
            _Out_ ULONG                              * pcbSize,
            _Outptr_result_buffer_( *pcbSize ) BYTE ** ppbTlv );
    }; // namespace GENERATOR
};
