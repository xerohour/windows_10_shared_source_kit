#include "precomp.hpp"
#include "TLVGeneratorParser.hpp"
#include "TlvFramework.hpp"
#include "TLVGeneratorParser.tmh"

extern "C" {

_Ret_notnull_ _Post_writable_byte_size_( _Size ) void *          __cdecl _alloca( _In_ size_t _Size );

void __stdcall FreeGenerated( _In_opt_ UINT8* pBuffer )
{
    if (pBuffer)
    {
        delete[] pBuffer;
    }
}

void __stdcall TraceUnknownTlvByteStream(
    _In_ ULONG PeerVersion,
    _In_ ULONG BufferLength,
    _In_reads_bytes_( BufferLength ) UINT8 const * pBuffer )
{
    WDI_TLV::DUMPER::CONTEXT ctx = { 0 };
    ctx.TlvVersion = PeerVersion;

    WDI_TLV::DUMPER::DumpTlvs( BufferLength, pBuffer, &ctx );
}

void __stdcall TraceMessageTlvByteStream(
    _In_ ULONG MessageId,
    _In_ BOOLEAN fToIhv,
    _In_ ULONG PeerVersion,
    _In_ ULONG BufferLength,
    _In_reads_bytes_( BufferLength ) UINT8 const * pBuffer )
{
    WDI_TLV::DUMPER::CONTEXT ctx = { 0 };
    ctx.fKnownOuter = TRUE;
    ctx.fIsMessage = TRUE;
    ctx.fToIhv = fToIhv ? TRUE : FALSE;
    ctx.OuterTlvOrMsgId = MessageId;
    ctx.TlvVersion = PeerVersion;

    WDI_TLV::DUMPER::DumpTlvs( BufferLength, pBuffer, &ctx );
}

void __stdcall DumpUnknownTlvByteStream(
    _In_ ULONG PeerVersion,
    _In_ ULONG BufferLength,
    _In_reads_bytes_( BufferLength ) UINT8 const * pBuffer,
    _In_opt_ ULONG_PTR Context,
    _In_opt_ TlvDumperCallback pCallback )
{
    WDI_TLV::DUMPER::CONTEXT ctx = { 0 };
    ctx.TlvVersion = PeerVersion;
    ctx.DumpContext = Context;
    ctx.pCallback = pCallback;

    //
    // This pulls in about 16K of read-only strings & code so
    // that the dumper callback can get the TLV names, field names, etc.
    // 
    // If WPP tracing will suffice, then use TraceUnknownTlvByteStream instead.
    //
    WDI_TLV::DUMPER::s_pfnMapTlvIdToString = WDI_TLV::DUMPER::MapTlvIdToString;
    WDI_TLV::DUMPER::s_pfnMapFieldNameToString = WDI_TLV::DUMPER::MapFieldNameToString;
    WDI_TLV::DUMPER::s_pfnMapTypeNameToString = WDI_TLV::DUMPER::MapTypeNameToString;

    WDI_TLV::DUMPER::DumpTlvs( BufferLength, pBuffer, &ctx );
}

void __stdcall DumpMessageTlvByteStream(
    _In_ ULONG MessageId,
    _In_ BOOLEAN fToIhv,
    _In_ ULONG PeerVersion,
    _In_ ULONG BufferLength,
    _In_reads_bytes_( BufferLength ) UINT8 const * pBuffer,
    _In_opt_ ULONG_PTR Context,
    _In_opt_ TlvDumperCallback pCallback )
{
    WDI_TLV::DUMPER::CONTEXT ctx = { 0 };
    ctx.fKnownOuter = TRUE;
    ctx.fIsMessage = TRUE;
    ctx.fToIhv = fToIhv ? TRUE : FALSE;
    ctx.OuterTlvOrMsgId = MessageId;
    ctx.TlvVersion = PeerVersion;
    ctx.DumpContext = Context;
    ctx.pCallback = pCallback;

    //
    // This pulls in about 16K of read-only strings & code so
    // that the dumper callback can get the TLV names, field names, etc.
    // 
    // If WPP tracing will suffice, then use TraceMessageTlvByteStream instead.
    //
    WDI_TLV::DUMPER::s_pfnMapTlvIdToString = WDI_TLV::DUMPER::MapTlvIdToString;
    WDI_TLV::DUMPER::s_pfnMapFieldNameToString = WDI_TLV::DUMPER::MapFieldNameToString;
    WDI_TLV::DUMPER::s_pfnMapTypeNameToString = WDI_TLV::DUMPER::MapTypeNameToString;

    WDI_TLV::DUMPER::DumpTlvs( BufferLength, pBuffer, &ctx );
}


} // extern "C"

namespace WDI_TLV
{
    namespace DUMPER
    {
        pfnMapEnumToStringApi s_pfnMapTlvIdToString = NULL;
        pfnMapEnumToStringApi s_pfnMapFieldNameToString = NULL;
        pfnMapEnumToStringApi s_pfnMapTypeNameToString = NULL;

        extern const MAP_ENTRY s_rgAllDumpers[];
        extern const size_t s_cAllDumpers;
        // Only support 16 levels of indent
        static const char s_szIndent[] = "                        ";
        static LPCSTR GetIndent( _In_ const CONTEXT * pctx )
        {
            return s_szIndent + (sizeof( s_szIndent ) - (1 + min( sizeof( s_szIndent ), pctx->IndentLevel * 2 ))); // 2 chars per indent
        }

        void DumpZeroNotAllowed( _Inout_ CONTEXT *pctx )
        {
            WFCTrace( "    TLV requires at least 1 element" );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%sTLV requires at least 1 element", GetIndent( pctx ) );
            }
        }
        void DumpMissingField( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field )
        {
            WFCTrace( "    !MISSING or PARTIAL! %!FIELD_NAME!", field );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s!MISSING or PARTIAL! %s", GetIndent( pctx ), s_pfnMapFieldNameToString( field ) );
            }
        }

        void DumpMissingField( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index )
        {
            WFCTrace( "    !MISSING or PARTIAL! %!FIELD_NAME![%u]", field, index );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s!MISSING or PARTIAL! %s[%u]", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), index );
            }
        }

        void DumpKey( _Inout_ CONTEXT * pctx, _In_z_ LPCSTR szKey )
        {
            WFCTrace( "%s", szKey );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s", GetIndent( pctx ), szKey );
            }
        }

        void DumpKeyValueBool( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, BOOLEAN Value )
        {
            if (Value)
            {
                WFCTrace( "    %!FIELD_NAME!: TRUE (0x%02x)", field, Value );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%s: TRUE (0x%02x)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), Value );
                }
            }
            else
            {
                WFCTrace( "    %!FIELD_NAME!: FALSE (0)", field );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%s: FALSE (0)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ) );
                }
            }
        }

        void DumpKeyValueBool( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, BOOLEAN Value )
        {
            if (Value)
            {
                WFCTrace( "    %!FIELD_NAME![%u]: TRUE (0x%02x)", field, index, Value );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%s[%u]: TRUE (0x%02x)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), index, Value );
                }
            }
            else
            {
                WFCTrace( "    %!FIELD_NAME![%u]: FALSE (0)", field, index );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%s[%u]: FALSE (0)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), index );
                }
            }
        }

        void DumpKeyValueString( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, LPCSTR szValue )
        {
            WFCTrace( "    %!FIELD_NAME!: '%s'", field, szValue );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: '%s'", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), szValue );
            }

        }

        void DumpKeyValueWithString( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, LPCSTR szValue, UINT16 Value )
        {
            WFCTrace( "    %!FIELD_NAME!: %s (0x%04X)", field, szValue, Value );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: %s (0x%04X)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), szValue, Value );
            }
        }

        void DumpTlvType( _Inout_ CONTEXT * pctx, UINT16 tlvType )
        {
            WFCTrace( ">>(T): %!TLV_ID! (0x%04X)", (ENUMS::TLV_ID)tlvType, tlvType );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s(T): %s (0x%04X)", GetIndent( pctx ), s_pfnMapTlvIdToString( tlvType ), tlvType );
            }
        }
        void DumpEndTlvType( _Inout_ CONTEXT * pctx, UINT16 tlvType )
        {
            WFCTrace( "<<(T): %!TLV_ID! (0x%04X)", (ENUMS::TLV_ID)tlvType, tlvType );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s(T): %s (0x%04X)", GetIndent( pctx ), s_pfnMapTlvIdToString( tlvType ), tlvType );
            }
        }
        void DumpTlvLength( _Inout_ CONTEXT * pctx, UINT16 tlvLength )
        {
            WFCTrace( "  (L): 0x%04X", tlvLength );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s(L): 0x%04X", GetIndent( pctx ), tlvLength );
            }
        }

        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT8 Value, ENUMS::ENCODED_TYPE_NAME type )
        {
            WFCTrace( "    %!FIELD_NAME!: 0x%02X (%!ENCODED_TYPE_NAME!)", field, Value, type );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: 0x%02X (%s)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), Value, s_pfnMapTypeNameToString( type ) );
            }

        }

        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT16 Value, ENUMS::ENCODED_TYPE_NAME type )
        {
            WFCTrace( "    %!FIELD_NAME!: 0x%04X (%!ENCODED_TYPE_NAME!)", field, Value, type );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: 0x%04X (%s)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), Value, s_pfnMapTypeNameToString( type ) );
            }
        }

        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT32 Value, ENUMS::ENCODED_TYPE_NAME type )
        {
            WFCTrace( "    %!FIELD_NAME!: 0x%08X (%!ENCODED_TYPE_NAME!)", field, Value, type );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: 0x%08X (%s)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), Value, s_pfnMapTypeNameToString( type ) );
            }
        }

        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT64 Value, ENUMS::ENCODED_TYPE_NAME type )
        {
            WFCTrace( "    %!FIELD_NAME!: 0x%016I64X (%!ENCODED_TYPE_NAME!)", field, Value, type );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: 0x%016I64X (%s)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), Value, s_pfnMapTypeNameToString( type ) );
            }
        }

        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, INT8 Value, ENUMS::ENCODED_TYPE_NAME type )
        {
            WFCTrace( "    %!FIELD_NAME!: %d (%!ENCODED_TYPE_NAME!)", field, Value, type );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: 0x%d (%s)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), Value, s_pfnMapTypeNameToString( type ) );
            }
        }

        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, INT16 Value, ENUMS::ENCODED_TYPE_NAME type )
        {
            WFCTrace( "    %!FIELD_NAME!: %d (%!ENCODED_TYPE_NAME!)", field, Value, type );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: 0x%d (%s)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), Value, s_pfnMapTypeNameToString( type ) );
            }
        }

        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, INT32 Value, ENUMS::ENCODED_TYPE_NAME type )
        {
            WFCTrace( "    %!FIELD_NAME!: %d (%!ENCODED_TYPE_NAME!)", field, Value, type );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: 0x%d (%s)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), Value, s_pfnMapTypeNameToString( type ) );
            }
        }

        void DumpKeyValue( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, INT64 Value, ENUMS::ENCODED_TYPE_NAME type )
        {
            WFCTrace( "    %!FIELD_NAME!: %I64d (%!ENCODED_TYPE_NAME!)", field, Value, type );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: 0x%I64d (%s)", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), Value, s_pfnMapTypeNameToString( type ) );
            }
        }

        void SanitizeString( ULONG cbLen, _In_reads_bytes_( cbLen ) const BYTE * src, _Out_writes_z_( cbLen + 1 ) CHAR * dst )
        {
            for (ULONG i = 0; i < cbLen; i++)
            {
                if ((src[i] < 32) || (src[i] > 126))
                {
                    dst[i] = '.';
                }
                else
                {
                    dst[i] = src[i];
                }
            }
            dst[cbLen] = 0;
        }

        void DumpKeyBlobAsString( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer )
        {
            if (len == 0)
            {
                WFCTrace( "    %!FIELD_NAME!: <empty>", field );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%s: <empty>", GetIndent( pctx ), s_pfnMapFieldNameToString( field ) );
                }
                return;
            }
            ULONG offset = 0;
            WFCTrace( "    %!FIELD_NAME!: blob length=%u, bytes:", field, len );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: blob length=%u, bytes:", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), len );
            }
            CHAR cleanStr[9];

            pctx->IndentLevel++;

            while (len >= 8)
            {
                SanitizeString( 8, buffer, cleanStr );
                WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x %02x  %s",
                    offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7],
                    cleanStr );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x %02x  %s", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7],
                        cleanStr );
                }
                offset += 8;
                buffer += 8;
                len -= 8;
            }
            SanitizeString( len, buffer, cleanStr );
            switch (len)
            {
            case 7:
                WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x     %s",
                    offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6],
                    cleanStr );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x     %s", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6],
                        cleanStr );
                }
                break;
            case 6:
                WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x        %s",
                    offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], cleanStr );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x        %s", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], cleanStr );
                }
                break;
            case 5:
                WFCTrace( "    %04x: %02x %02x %02x %02x %02x           %s", offset, buffer[0], buffer[1],
                    buffer[2], buffer[3], buffer[4], cleanStr );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x           %s", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], cleanStr );
                }
                break;
            case 4:
                WFCTrace( "    %04x: %02x %02x %02x %02x              %s", offset, buffer[0], buffer[1],
                    buffer[2], buffer[3], cleanStr );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x              %s", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], cleanStr );
                }
                break;
            case 3:
                WFCTrace( "    %04x: %02x %02x %02x                 %s", offset, buffer[0], buffer[1], buffer[2],
                    cleanStr );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x                 %s", GetIndent( pctx ), offset,
                        buffer[0], buffer[1], buffer[2], cleanStr );
                }
                break;
            case 2:
                WFCTrace( "    %04x: %02x %02x                    %s", offset, buffer[0], buffer[1], cleanStr );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x                    %s", GetIndent( pctx ), offset,
                        buffer[0], buffer[1], cleanStr );
                }
                break;
            case 1:
                WFCTrace( "    %04x: %02x                       %s", offset, buffer[0], cleanStr );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x                       %s", GetIndent( pctx ),
                        offset, buffer[0], cleanStr );
                }
                break;
            }
            pctx->IndentLevel--;
        }

        static bool s_DumpPII = false;

        void DumpPrivateKeyBlob( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer )
        {
            if (s_DumpPII)
            {
                DumpKeyBlob( pctx, field, len, buffer );
            }
            else
            {
                if (len == 0)
                {
                    WFCTrace( "    %!FIELD_NAME!: <empty>", field );
                    if (pctx->pCallback)
                    {
                        pctx->pCallback( pctx->DumpContext, "%s%s: <empty>", GetIndent( pctx ), s_pfnMapFieldNameToString( field ) );
                    }
                    return;
                }
                WFCTrace( "    %!FIELD_NAME!: blob length=%u, bytes: PRIVATE", field, len );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%s: blob length=%u, bytes: PRIVATE", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), len );
                }
            }
        }

        void DumpKeyBlobBody( _Inout_ CONTEXT * pctx, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer )
        {
            ULONG offset = 0;
            if (pctx->pCallback)
            {
                pctx->IndentLevel++;
                // Can't use !HEXDUMP! with external logger, so use our own.
                while (len >= 16)
                {
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x",
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14], buffer[15] );
                    offset += 16;
                    buffer += 16;
                    len -= 16;
                }
                switch (len)
                {
                case 15:
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x",
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10], buffer[11], buffer[12], buffer[13], buffer[14] );
                    break;
                case 14:
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x",
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10], buffer[11], buffer[12], buffer[13] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10], buffer[11], buffer[12], buffer[13] );
                    break;
                case 13:
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x",
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10], buffer[11], buffer[12] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10], buffer[11], buffer[12] );
                    break;
                case 12:
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x",
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10], buffer[11] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10], buffer[11] );
                    break;
                case 11:
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x",
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9], buffer[10] );
                    break;
                case 10:
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x",
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8],
                        buffer[9] );
                    break;
                case 9:
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x",
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x %02x-%02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7], buffer[8] );
                    break;
                case 8:
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x %02x",
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6], buffer[7] );
                    break;
                case 7:
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x %02x",
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5], buffer[6] );
                    break;
                case 6:
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x %02x",
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4], buffer[5] );
                    break;
                case 5:
                    WFCTrace( "    %04x: %02x %02x %02x %02x %02x", offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3], buffer[4] );
                    break;
                case 4:
                    WFCTrace( "    %04x: %02x %02x %02x %02x", offset, buffer[0], buffer[1], buffer[2], buffer[3] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x %02x", GetIndent( pctx ),
                        offset, buffer[0], buffer[1], buffer[2], buffer[3] );
                    break;
                case 3:
                    WFCTrace( "    %04x: %02x %02x %02x", offset, buffer[0], buffer[1], buffer[2] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x %02x", GetIndent( pctx ), offset, buffer[0], buffer[1], buffer[2] );
                    break;
                case 2:
                    WFCTrace( "    %04x: %02x %02x", offset, buffer[0], buffer[1] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x %02x", GetIndent( pctx ), offset, buffer[0], buffer[1] );
                    break;
                case 1:
                    WFCTrace( "    %04x: %02x", offset, buffer[0] );
                    pctx->pCallback( pctx->DumpContext, "%s%04x: %02x", GetIndent( pctx ), offset, buffer[0] );
                    break;
                }
                pctx->IndentLevel--;
            }
            else
            {
                // No external dumper, just use !HEXDUMP!
                pctx->IndentLevel++;
                WFCTrace( "  %!HEXDUMP!", LOG_HEXDUMP( len, buffer ) );
                pctx->IndentLevel--;
            }
        }

        void DumpKeyBlob( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer )
        {
            if (len == 0)
            {
                WFCTrace( "    %!FIELD_NAME!: <empty>", field );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%s: <empty>", GetIndent( pctx ), s_pfnMapFieldNameToString( field ) );
                }
                return;
            }
            WFCTrace( "    %!FIELD_NAME!: blob length=%u, bytes:", field, len );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s: blob length=%u, bytes:", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), len );
            }
            DumpKeyBlobBody( pctx, len, buffer );
        }

        void DumpKeyBlob( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, UINT index, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer )
        {
            if (len == 0)
            {
                WFCTrace( "    %!FIELD_NAME![%u]: <empty>", field, index );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%s[%u]: <empty>", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), index );
                }
                return;
            }
            WFCTrace( "    %!FIELD_NAME![%u]: blob length=%u, bytes:", field, index, len );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s[%u]: blob length=%u, bytes:", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), index, len );
            }
            DumpKeyBlobBody( pctx, len, buffer );
        }

        void DumpTypedKeyBlob( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, ENUMS::ENCODED_TYPE_NAME type, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer )
        {
            if (len == 0)
            {
                WFCTrace( "    %!FIELD_NAME! (%!ENCODED_TYPE_NAME!): <empty>", field, type );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%s (%s): <empty>", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), s_pfnMapTypeNameToString( type ) );
                }
                return;
            }
            WFCTrace( "    %!FIELD_NAME! (%!ENCODED_TYPE_NAME!): blob length=%u, bytes:", field, type, len );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s (%s): blob length=%u, bytes:", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), s_pfnMapTypeNameToString( type ), len );
            }
            DumpKeyBlobBody( pctx, len, buffer );
        }

        void DumpTypedKeyBlob( _Inout_ CONTEXT * pctx, ENUMS::FIELD_NAME field, ENUMS::ENCODED_TYPE_NAME type, UINT index, ULONG len, _In_reads_bytes_( len ) const BYTE * buffer )
        {
            if (len == 0)
            {
                WFCTrace( "    %!FIELD_NAME! (%!ENCODED_TYPE_NAME!)[%u]: <empty>", field, type, index );
                if (pctx->pCallback)
                {
                    pctx->pCallback( pctx->DumpContext, "%s%s (%s)[%u]: <empty>", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), s_pfnMapTypeNameToString( type ), index );
                }
                return;
            }
            WFCTrace( "    %!FIELD_NAME! (%!ENCODED_TYPE_NAME!)[%u]: blob length=%u, bytes:", field, type, index, len );
            if (pctx->pCallback)
            {
                pctx->pCallback( pctx->DumpContext, "%s%s (%s)[%u]: blob length=%u, bytes:", GetIndent( pctx ), s_pfnMapFieldNameToString( field ), s_pfnMapTypeNameToString( type ), index, len );
            }
            DumpKeyBlobBody( pctx, len, buffer );
        }

        void DumpBoolField(
            ENUMS::FIELD_NAME field,
            _Inout_ ULONG * pBufferLength,
            _Deref_pre_bytecount_( *pBufferLength ) _Deref_post_bytecount_( *pBufferLength ) UINT8 const ** ppTlvData,
            _Inout_ WDI_TLV::DUMPER::CONTEXT * Context )
        {
            if (*pBufferLength < 1)
            {
                WDI_TLV::DUMPER::DumpMissingField( Context, field );
            }
            else
            {
                WDI_TLV::DUMPER::DumpKeyValueBool( Context, field, *(const UNALIGNED BOOLEAN*)(*ppTlvData) );
                *ppTlvData += 1;
                *pBufferLength -= 1;
            }
        }

        void DumpBoolField(
            ENUMS::FIELD_NAME field,
            UINT index,
            _Inout_ ULONG * pBufferLength,
            _Deref_pre_bytecount_( *pBufferLength ) _Deref_post_bytecount_( *pBufferLength ) UINT8 const ** ppTlvData,
            _Inout_ WDI_TLV::DUMPER::CONTEXT * Context )
        {
            if (*pBufferLength < 1)
            {
                WDI_TLV::DUMPER::DumpMissingField( Context, field, index );
            }
            else
            {
                WDI_TLV::DUMPER::DumpKeyValueBool( Context, field, index, *(const UNALIGNED BOOLEAN*)(*ppTlvData) );
                *ppTlvData += 1;
                *pBufferLength -= 1;
            }
        }

        void DumpByteArrayHelper(
            _In_ USHORT fZeroAllowed,
            _In_ USHORT fPrivate,
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _Inout_ CONTEXT * Context )
        {
            if (fPrivate)
            {
                DumpPrivateKeyBlob( Context, ENUMS::Value, DataLength, pTlvData );
            }
            else
            {
                DumpKeyBlob( Context, ENUMS::Value, DataLength, pTlvData );
            }
            if (!fZeroAllowed && (0 == DataLength))
            {
                DumpZeroNotAllowed( Context );
            }
        }

        void DumpStringHelper(
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _Inout_ CONTEXT * Context )
        {
            DumpKeyBlobAsString( Context, ENUMS::Value, DataLength, pTlvData );
            if (0 == DataLength)
            {
                DumpZeroNotAllowed( Context );
            }
        }

        void DumpStringHelperZeroAllowed(
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _Inout_ CONTEXT * Context )
        {
            DumpKeyBlobAsString( Context, ENUMS::Value, DataLength, pTlvData );
        }


        void DumpResursiveTlv(
            _In_ USHORT TlvType,
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _In_ CONTEXT * pContext )
        {
            CONTEXT saved = *pContext;
            pContext->OuterTlvOrMsgId = TlvType;
            pContext->fKnownOuter = TRUE;
            pContext->fIsMessage = FALSE;
            pContext->fToIhv = FALSE;
            DumpTlvs( DataLength, pTlvData, pContext );
            *pContext = saved;
        }

        void DumpOtherField(
            ENUMS::FIELD_NAME field,
            ENUMS::ENCODED_TYPE_NAME type,
            ULONG fieldSize,
            _Inout_ ULONG * pBufferLength,
            _Deref_pre_bytecount_( *pBufferLength ) _Deref_post_bytecount_( *pBufferLength ) UINT8 const ** ppTlvData,
            _Inout_ CONTEXT * Context )
        {
            if (*pBufferLength < fieldSize)
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
                const UINT8 * pValue = *ppTlvData;
                if (fieldSize == 1)
                {
                    DumpKeyValue( Context, field, *(const UNALIGNED UINT8*)pValue, type );
                }
                else if (fieldSize == 2)
                {
                    DumpKeyValue( Context, field, *(const UNALIGNED UINT16*)pValue, type );
                }
                else if (fieldSize == 4)
                {
                    DumpKeyValue( Context, field, *(const UNALIGNED UINT32*)pValue, type );
                }
                else if (fieldSize == 8)
                {
                    DumpKeyValue( Context, field, *(const UNALIGNED UINT64*)pValue, type );
                }
                else
                {
                    DumpTypedKeyBlob( Context, field, type, fieldSize, (const UNALIGNED BYTE *)pValue );
                }
                *ppTlvData += fieldSize;
                *pBufferLength -= fieldSize;
            }
        }

        void DumpSimpleContainer(
            _In_ CONTAINER_INFO const * pInfo,
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _In_ CONTEXT * pContext )
        {
            for (UINT16 iField = 0; iField < pInfo->cntFields; iField++)
            {
                const FIELD_INFO & field = pInfo->rgFields[iField];
                if (field.count > 1)
                {
                    if (field.typeName == ENUMS::UINT8)
                    {
                        NT_ASSERT( field.packedLen == field.count );
                        if (DataLength < field.count)
                        {
                            DumpMissingField( pContext, field.fieldName );
                            if (DataLength > 0)
                            {
                                DUMPER::DumpKeyBlob( pContext, ENUMS::PartialFieldData, DataLength, pTlvData );
                                pTlvData += DataLength;
                                DataLength = 0;
                            }
                        }
                        else
                        {
                            DumpKeyBlob( pContext, field.fieldName, field.count, pTlvData );
                            pTlvData += field.count;
                            DataLength -= field.count;
                        }
                    }
                    else
                    {
                        NT_ASSERTMSG_ASSUME( "NYI - checked/enforced in wabigen", FALSE );
                    }
                }
                else
                {
                    switch (field.typeName)
                    {
                    case ENUMS::BOOLEAN:
                        DumpBoolField( field.fieldName, &DataLength, &pTlvData, pContext );
                        break;
                    case ENUMS::UINT8:
                        DumpSimpleField<UINT8>( field.fieldName, &DataLength, &pTlvData, pContext );
                        break;
                    case ENUMS::INT8:
                        DumpSimpleField<INT8>( field.fieldName, &DataLength, &pTlvData, pContext );
                        break;
                    case ENUMS::UINT16:
                        DumpSimpleField<UINT16>( field.fieldName, &DataLength, &pTlvData, pContext );
                        break;
                    case ENUMS::INT16:
                        DumpSimpleField<INT16>( field.fieldName, &DataLength, &pTlvData, pContext );
                        break;
                    case ENUMS::UINT64:
                        DumpSimpleField<UINT64>( field.fieldName, &DataLength, &pTlvData, pContext );
                        break;
                    case ENUMS::INT64:
                        DumpSimpleField<INT64>( field.fieldName, &DataLength, &pTlvData, pContext );
                        break;
                    case ENUMS::UINT32:
                        DumpSimpleField<UINT32>( field.fieldName, &DataLength, &pTlvData, pContext );
                        break;
                    case ENUMS::INT32:
                        DumpSimpleField<INT32>( field.fieldName, &DataLength, &pTlvData, pContext );
                        break;
                    default:
                        DumpOtherField( field.fieldName, field.typeName, field.packedLen, &DataLength, &pTlvData, pContext );
                        break;
                    }
                }
            }
            if (DataLength > 0)
            {
                DUMPER::DumpKeyBlob( pContext, ENUMS::UnknownFieldData, DataLength, pTlvData );
            }
        }

        void DumpTlvs(
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _Inout_ CONTEXT * pContext )
        {
            USHORT tlvType, tlvLength;

            while (DataLength >= WDI_TLV_HEADER_SIZE)
            {
                tlvType = *((USHORT*)pTlvData);
                tlvLength = *((USHORT*)(pTlvData + sizeof( tlvType )));

                pTlvData += WDI_TLV_HEADER_SIZE;
                DataLength -= WDI_TLV_HEADER_SIZE;

                if (tlvLength > DataLength)
                {
                    WFCTrace( "TLV Length (0x%04X) longer than buffer (0x%08X), truncating", tlvLength, DataLength );
                    if (pContext->pCallback)
                    {
                        pContext->pCallback( pContext->DumpContext, "%sTLV Length (0x%04X) longer than buffer (0x%08X), truncating", GetIndent( pContext ), tlvLength, DataLength );
                    }
                    // Cast is safe because we already know DataLength is shorter
                    // than a USHORT due to the above if check
                    tlvLength = (USHORT)DataLength;
                }

                // Find which dumper to use
                bool fFound = false;
                CONTAINER_INFO const * containerInfo = NULL;
                for (size_t i = 0; i < s_cAllDumpers; i++)
                {
                    const MAP_ENTRY & entry = s_rgAllDumpers[i];
                    if (entry.TlvType == tlvType)
                    {
                        const INFO * bestMatch = &entry.rgDumpers[0];
                        int bestMatchLevel = 0;

                        for (UINT16 j = 0; j < entry.cDumpers; j++)
                        {
                            const INFO * info = &entry.rgDumpers[j];
                            for (size_t k = 0; k < info->cRefs; k++)
                            {
                                const CONTAINER_REF & ref = info->rgRefs[k];
                                int currentMatch = 0;
                                if (info->pInfo != NULL)
                                {
                                    if (info->pInfo->fIsCollection)
                                    {
                                        if (tlvLength == 0)
                                        {
                                            // zero size is too easy to match
                                            if (info->pInfo->fIsZeroAllowed)
                                            {
                                                currentMatch += 9;
                                            }
                                            else
                                            {
                                                currentMatch += 7;
                                            }
                                        }
                                        else if (0 == tlvLength % info->pInfo->cbPackedSize)
                                        {
                                            currentMatch += 10;
                                        }
                                    }
                                    else if (tlvLength == info->pInfo->cbPackedSize)
                                    {
                                        currentMatch += 10;
                                    }
                                }
                                else if (tlvLength > WDI_TLV_HEADER_SIZE)
                                {
                                    // Unknown size because of nesting, assume match with higher weight because
                                    // it's at least enough for a nested TLV header
                                    currentMatch += 5;
                                }
                                else
                                {
                                    // Unknown size because of nesting, assume match
                                    currentMatch += 4;
                                }
                                if (pContext->fKnownOuter)
                                {
                                    if ((ref.OuterTlvOrMsgId == pContext->OuterTlvOrMsgId) && (ref.fIsMessage == pContext->fIsMessage))
                                    {
                                        // Outer Container Match
                                        currentMatch += 10;
                                    }
                                    if (ref.fIsMessage && pContext->fIsMessage && (ref.fToIhv == pContext->fToIhv))
                                    {
                                        // Direction Match;
                                        currentMatch += 5;
                                    }
                                    else if (!ref.fIsMessage && !pContext->fIsMessage)
                                    {
                                        // Both aggregates
                                        currentMatch += 5;
                                    }
                                }
                                else if (!ref.fIsMessage)
                                {
                                    // Give slight preference to aggregates
                                    currentMatch += 1;
                                }
                                if (pContext->TlvVersion != 0)
                                {
                                    if (ref.VersionAdded <= pContext->TlvVersion)
                                    {
                                        // versionAdded match
                                        currentMatch += 6;
                                    }
                                    if (ref.VersionRemoved > pContext->TlvVersion)
                                    {
                                        // versionRemoved match
                                        currentMatch += 6;
                                    }
                                }
                                else
                                {
                                    // Slight preference for latest TLV
                                    if (ref.VersionRemoved == 0xFFFFFFFFul)
                                    {
                                        currentMatch += 3;
                                    }
                                }

                                if (currentMatch > bestMatchLevel)
                                {
                                    bestMatchLevel = currentMatch;
                                    bestMatch = info;
                                }
                            }
                        }
                        containerInfo = bestMatch->pInfo;
                        fFound = true;
                        break;
                    }
                }
                DumpTlvType( pContext, tlvType );
                DumpTlvLength( pContext, tlvLength );
                pContext->IndentLevel++;
                if (!fFound)
                {
                    DumpKeyBlob( pContext, ENUMS::UNKNOWN_FIELD, tlvLength, pTlvData );
                }
                else if (containerInfo == NULL)
                {
                    CONTEXT saved = *pContext;
                    pContext->OuterTlvOrMsgId = tlvType;
                    pContext->fKnownOuter = TRUE;
                    pContext->fIsMessage = FALSE;
                    pContext->fToIhv = FALSE;
                    DumpTlvs( tlvLength, pTlvData, pContext );
                    *pContext = saved;
                }
                else if (containerInfo->fIsCollection && !containerInfo->fDumpAsSingle)
                {

                    bool fSawAtLeastOne = false;
                    const UINT16 elemSize = containerInfo->cbPackedSize;
                    UINT16 tempLen = tlvLength;
                    const BYTE * pTempData = pTlvData;
                    DumpKeyValue( pContext, ENUMS::ElementCount, (USHORT)(tlvLength / elemSize) );
                    while (tempLen >= elemSize)
                    {
                        DumpSimpleContainer( containerInfo, elemSize, pTempData, pContext );
                        tempLen -= elemSize;
                        pTempData += elemSize;
                        fSawAtLeastOne = true;
                    }
                    if (tempLen != 0)
                    {
                        fSawAtLeastOne = true;
                        DumpKeyBlob( pContext, ENUMS::PartialFieldData, tempLen, pTempData );
                    }
                    if (!fSawAtLeastOne && !containerInfo->fIsZeroAllowed)
                    {
                        DumpZeroNotAllowed( pContext );
                    }
                }
                else
                {
                    bool fDumpSpecial = false;
                    if (containerInfo->fIsCollection)
                    {
                        if (containerInfo->cntFields == 1)
                        {
                            if ((containerInfo->rgFields[0].typeName == ENUMS::CHAR) ||
                                (containerInfo->rgFields[0].fieldName == ENUMS::SSID))
                            {
                                fDumpSpecial = true;
                                if (containerInfo->fIsZeroAllowed)
                                {
                                    DumpStringHelperZeroAllowed( tlvLength, pTlvData, pContext );
                                }
                                else
                                {
                                    DumpStringHelper( tlvLength, pTlvData, pContext );
                                }
                            }
                            else if (containerInfo->rgFields[0].typeName == ENUMS::UINT8)
                            {
                                fDumpSpecial = true;
                                DumpByteArrayHelper( containerInfo->fIsZeroAllowed, containerInfo->fPrivate, tlvLength, pTlvData, pContext );
                            }
                        }
                    }
                    if (!fDumpSpecial)
                    {
                        DumpSimpleContainer( containerInfo, tlvLength, pTlvData, pContext );
                    }
                }
                pContext->IndentLevel--;
                DumpEndTlvType( pContext, tlvType );

                DataLength -= tlvLength;
                pTlvData += tlvLength;
            }
        }

        _Function_class_( WDI_TLV_DUMPER_ENUM_API ) LPCSTR MapTlvIdToString( _In_ ULONG enumValue )
        {
            switch (enumValue)
            {
#define DUMPER_TLV_ID_1( x ) case ENUMS::x: return # x ;
#include "TlvGeneratedEnums_.hpp"
default:
    return "UNKNOWN_TLV";
            };
        }

        _Function_class_( WDI_TLV_DUMPER_ENUM_API ) LPCSTR MapFieldNameToString( _In_ ULONG enumValue )
        {
            static const LPCSTR rgszFieldNames[] = {
#define DUMPER_FIELD_NAME( x ) # x ,
#include "TlvGeneratedEnums_.hpp"
            };

            if (enumValue >= ARRAYSIZE( rgszFieldNames ))
            {
                return rgszFieldNames[ENUMS::UNKNOWN_FIELD];
            }
            return rgszFieldNames[enumValue];
        }

        _Function_class_( WDI_TLV_DUMPER_ENUM_API ) LPCSTR MapTypeNameToString( _In_ ULONG enumValue )
        {
            static const LPCSTR rgszTypeNames[] = {
#define DUMPER_TYPE_NAME( x ) # x ,
#include "TlvGeneratedEnums_.hpp"
            };

            if (enumValue >= ARRAYSIZE( rgszTypeNames ))
            {
                return rgszTypeNames[ENUMS::UNKNOWN_TYPE];
            }
            return rgszTypeNames[enumValue];
        }
    };

    namespace PARSER
    {
        EmptyMessageStructureType s_empty;

        NDIS_STATUS ParseEmptyMessage(
            _In_ ULONG BufferLength,
            _In_reads_bytes_( BufferLength ) UINT8 const * pBuffer,
            _In_ PCTLV_CONTEXT Context,
            _Out_ EmptyMessageStructureType * pParsedOutput )
        {
            NDIS_STATUS status = NDIS_STATUS_SUCCESS;

            UNREFERENCED_PARAMETER( Context );
            UNREFERENCED_PARAMETER( pBuffer );

            TraceEntry();

            if (pParsedOutput)
            {
                memset( pParsedOutput, 0, sizeof( *pParsedOutput ) );
            }

            if (Context == NULL)
            {
                WFCError( "Missing Context parameter!" );
                status = NDIS_STATUS_INVALID_PARAMETER;
            }
            else if (Context->PeerVersion < WDI_VERSION_MIN_SUPPORTED)
            {
                WFCError( "Unsupported PeerVersion (0x%08X) should be at least 0x%08X", Context->PeerVersion, WDI_VERSION_MIN_SUPPORTED );
                status = NDIS_STATUS_UNSUPPORTED_REVISION;
            }
            else if (BufferLength != 0)
            {
                WFCError( "Parsing empty message with non-empty (%u) buffer", BufferLength );
                status = NDIS_STATUS_INVALID_DATA;
            }

            TraceExit( status );
            return status;
        }

        NDIS_STATUS FindTlv(
            _In_ UINT16 TlvType,
            _In_ ULONG DataLength,
            _In_reads_bytes_( DataLength ) BYTE const * pTlvData,
            _Out_opt_ UINT16* pDesiredTlvLength,
            _Outptr_opt_result_buffer_maybenull_( *pDesiredTlvLength ) BYTE const ** pDesiredTlvData,
            _Out_opt_ ULONG *pNewDataLength,
            _Outptr_opt_result_buffer_maybenull_( *pNewDataLength ) BYTE const ** pNewTlvData )
        {
            NDIS_STATUS status = NDIS_STATUS_FILE_NOT_FOUND;
            USHORT tlvType, tlvLength;

            if (pDesiredTlvLength != NULL)
            {
                *pDesiredTlvLength = 0;
            }
            if (pDesiredTlvData != NULL)
            {
                *pDesiredTlvData = NULL;
            }
            if (pNewDataLength != NULL)
            {
                *pNewDataLength = 0;
            }
            if (pNewTlvData != NULL)
            {
                *pNewTlvData = NULL;
            }

            while (DataLength >= WDI_TLV_HEADER_SIZE)
            {
                tlvType = *((USHORT*)pTlvData);
                tlvLength = *((USHORT*)(pTlvData + sizeof( tlvType )));

                if (tlvLength > DataLength - WDI_TLV_HEADER_SIZE)
                {
                    status = NDIS_STATUS_BUFFER_OVERFLOW;
                    goto exit;
                }

                if (tlvType == TlvType)
                {
                    status = NDIS_STATUS_SUCCESS;

                    //
                    // Give raw pointer to data content of TLV and data length
                    //
                    if ((pDesiredTlvData != NULL) &&
                        (pDesiredTlvLength != NULL))
                    {
                        *pDesiredTlvData = pTlvData + WDI_TLV_HEADER_SIZE;
                        *pDesiredTlvLength = tlvLength;
                    }

                    if ((pNewDataLength != NULL) &&
                        (pNewTlvData != NULL))
                    {
                        //
                        // Update input pointer to point to next IE after found IE
                        //
                        *pNewDataLength = DataLength - WDI_TLV_HEADER_SIZE - tlvLength;
                        *pNewTlvData = pTlvData + WDI_TLV_HEADER_SIZE + tlvLength;
                    }

                    goto exit;
                }

                if (!NT_SUCCESS(
                    RtlULongSub(
                    DataLength,
                    WDI_TLV_HEADER_SIZE + tlvLength,
                    &DataLength ) ))
                {
                    status = NDIS_STATUS_INVALID_DATA;
                    goto exit;
                }

                pTlvData += WDI_TLV_HEADER_SIZE + tlvLength;
            }


exit:
            return status;
        }

        //
        // Parse a single Container/PropertyGroup/NamedType.
        // It may be marked as isCollection, in which case this will allocate appropriate ArrayOfElements to parse into.
        //
        // Returns:
        //  NDIS_STATUS_SUCCESS - parse succeeded.
        //  NDIS_STATUS_FILE_NOT_FOUND - zero elements (when not allowed)
        //  NDIS_STATUS_INVALID_DATA - byte buffer is wrong
        //  NDIS_STATUS_FAILURE - Passed in structure (cbOutput) is wrong
        //  NDIS_STATUS_RESOURCES - allocation failed
        //
        NDIS_STATUS ParseSimpleContainer(
            _In_ const ULONG                        cbBuffer,
            _In_reads_( cbBuffer ) const BYTE     * rgBuffer,
            _In_ ENUMS::TLV_ID                      tlvType,
            _In_ const CONTAINER_INFO             & cont,
            _In_ PCTLV_CONTEXT                      Context,
            _In_ size_t                             cbOutput,
            _Inout_updates_bytes_( cbOutput ) PVOID pOutput )
        {
            NDIS_STATUS status = NDIS_STATUS_SUCCESS;
            size_t iBuffer = 0;
            ULONG cntElems;
            BYTE * pbOutput;

            TraceEntry();

            WFCTrace( "Parsing %!TLV_ID!", tlvType );

            //
            // Range Checks for input and output buffers
            //   and allocation for arrays
            //
            if (cont.fIsCollection)
            {
                if (cbBuffer == 0)
                {
                    if (!cont.fIsZeroAllowed)
                    {
                        WFCError( "Zero elements in %!TLV_ID! (must have at least 1).", tlvType );
                        // Treat 0 elements the same as missing
                        status = NDIS_STATUS_FILE_NOT_FOUND;
                    }
                    WFCTrace( "Parsed 0 element LIST (allowed)" );
                    // If there are zero elements, then nothing to parse
                    goto EXIT;
                }
                cntElems = cbBuffer / cont.cbPackedSize;
                if ((cbBuffer % cont.cbPackedSize) != 0)
                {
                    WFCError( "Buffer for %!TLV_ID! not the right size: %u, expected a multiple of %u", tlvType, cbBuffer, cont.cbPackedSize );
                    status = NDIS_STATUS_INVALID_DATA;
                    goto EXIT;
                }
                if (cbOutput != sizeof( ArrayOfElements<BYTE> ))
                {
                    WFCError( "Structure for %!TLV_ID! not the right size: %Iu, expected %u", tlvType, cbOutput, sizeof( ArrayOfElements<BYTE> ) );
                    status = NDIS_STATUS_FAILURE;
                    goto EXIT;
                }
                WFCTrace( "Collection of %u elements", cntElems );
                status = g_ArrayHelpers[cont.ArrayOfElementsTypeIndex].AllocateElements( pOutput, cntElems, Context->AllocationContext );
                if (status != NDIS_STATUS_SUCCESS)
                {
                    WFCTrace( "Failed to allocate structure array for %!TLV_ID! status 0x%08X", tlvType, status );
                    goto EXIT;
                }

                pbOutput = *(BYTE**)((BYTE*)pOutput + FIELD_OFFSET( ArrayOfElements<BYTE>, pElements ));
                cbOutput = cntElems * cont.cbUnpackedSize;
                RtlZeroMemory( pbOutput, cbOutput );
            }
            else
            {
                if (cbBuffer != cont.cbPackedSize)
                {
                    WFCError( "Buffer for %!TLV_ID! not the right size: %u, expected %u", tlvType, cbBuffer, cont.cbPackedSize );
                    status = NDIS_STATUS_INVALID_DATA;
                    goto EXIT;
                }
                if (cbOutput != cont.cbUnpackedSize)
                {
                    WFCError( "Structure for %!TLV_ID! not the right size: %Iu, expected %u", tlvType, cbOutput, cont.cbUnpackedSize );
                    status = NDIS_STATUS_FAILURE;
                    goto EXIT;
                }
                cntElems = 1;
                pbOutput = (BYTE*)pOutput;
            }

            if (cont.cbPackedSize == cont.cbUnpackedSize)
            {
                NT_ASSERT_ASSUME( cbOutput == cbBuffer );
                WFCTrace( "Parsing out all field(s) at once (%u bytes)", cbBuffer );
                //
                // Shortcut to copy all elements at once
                //
                RtlCopyMemory( pbOutput, rgBuffer, cbBuffer );
            }
            else
            {
                for (size_t iElem = 0; iElem < cntElems; iElem++)
                {
                    //
                    // 'Parse' a single propertyGroup/namedType
                    //
                    _Writable_bytes_( cont.cbUnpackedSize ) BYTE * pbElem = &(pbOutput[iElem * cont.cbUnpackedSize]);
                    for (int iField = 0; iField < cont.cntFields; iField++)
                    {
                        WFCTrace( "Parsing out field %!FIELD_NAME!", cont.rgFields[iField].fieldName );

                        const UINT32 len = cont.rgFields[iField].packedLen;
                        const UINT16 offset = cont.rgFields[iField].unpackedOffset;

                        // Indicates a bug in WABIGEN!
                        NT_ASSERT_ASSUME( offset + len <= cont.cbUnpackedSize );

                        // Validated up above
                        NT_ASSERT_ASSUME( iBuffer + len <= cbBuffer );
                        RtlCopyMemory( pbElem + offset, rgBuffer + iBuffer, len );
                        iBuffer += len;
                    }
                }
            }
            status = NDIS_STATUS_SUCCESS;

EXIT:
            TraceExit( status );
            return status;
        }

        //
        // Parse a single Container within a message or aggregate container.
        // It may be marked as multi, in which case this will allocate appropriate ArrayOfElements to parse into.
        //
        // Returns:
        //  NDIS_STATUS_SUCCESS - parse succeeded.
        //  NDIS_STATUS_FILE_NOT_FOUND - zero elements (when not allowed)
        //  NDIS_STATUS_INVALID_DATA - byte buffer is wrong
        //  NDIS_STATUS_FAILURE - Passed in structure (cbOutput) is wrong
        //  NDIS_STATUS_RESOURCES - allocation failed
        //
        NDIS_STATUS ParseAggregateField(
            _In_ const ULONG                        cbBuffer,
            _In_reads_( cbBuffer ) const BYTE     * rgBuffer,
            _In_ MESSAGE_OR_TLV_ID                  tlvType,
            _In_ const CONTAINER_REF_INFO         & field,
            _In_ PCTLV_CONTEXT                      Context,
            _In_ size_t                             cbOutput,
            _Inout_updates_bytes_( cbOutput ) PVOID pOutput )
        {
            NDIS_STATUS    status = NDIS_STATUS_SUCCESS;
            const UINT16   offset = field.cbUnpackedOffset;
            UINT16         cMulti = (UINT16)(-1);
            const UINT16   elemSize = field.GetUnpackedFieldOrElementSize();
            BYTE         * pbOutput = ((BYTE*)pOutput) + offset;

            TraceEntry();
            if (tlvType.fIsMessage)
            {
                WFCTrace( "Parsing %!MESSAGE_ID!::%!TLV_ID!", tlvType.MsgId, field.tlvType );
            }
            else
            {
                WFCTrace( "Parsing %!TLV_ID!::%!TLV_ID!", tlvType.TlvId, field.tlvType );
            }

            if ((cbOutput < offset) ||
                ((cbOutput - offset) < field.GetUnpackedFieldSize()))
            {
                WFCError( "Output wrong size (%Iu), expected offset (%u) + size (%u)", cbOutput, offset, field.GetUnpackedFieldSize() );
                NT_ASSERTMSG( "Output size wrong", FALSE );
                status = NDIS_STATUS_BUFFER_TOO_SHORT;
                goto EXIT;
            }

            //
            // Count, validate & allocate
            //
            ;{
                ULONG        cbRemainingBuffer = cbBuffer;
                const BYTE * pbRemainingBuffer = rgBuffer;

                do
                {
                    cMulti++;
                    status = FindTlv( field.tlvType, cbRemainingBuffer, pbRemainingBuffer, NULL, NULL, &cbRemainingBuffer, &pbRemainingBuffer );
                } while (status == NDIS_STATUS_SUCCESS);

                if (status != NDIS_STATUS_FILE_NOT_FOUND)
                {
                    if (tlvType.fIsMessage)
                    {
                        WFCError( "Error finding %!TLV_ID! within %!MESSAGE_ID!: 0x%08X", field.tlvType, tlvType.MsgId, status );
                    }
                    else
                    {
                        WFCError( "Error finding %!TLV_ID! within %!TLV_ID!: 0x%08X", field.tlvType, tlvType.TlvId, status );
                    }
                    goto EXIT;
                }
                if (cMulti == 0)
                {
                    if (field.fIsOptional)
                    {
                        // Optional and not present, move on
                        status = NDIS_STATUS_SUCCESS;
                        goto EXIT;
                    }
                    if (tlvType.fIsMessage)
                    {
                        WFCError( "Required field %!TLV_ID! missing from %!MESSAGE_ID!", field.tlvType, tlvType.MsgId );
                    }
                    else
                    {
                        WFCError( "Required field %!TLV_ID! missing from %!TLV_ID!", field.tlvType, tlvType.TlvId );
                    }
                    goto EXIT;
                }
                if (field.fIsOptional)
                {
                    //
                    // Set the 'IsPresent' bit for this field
                    //
                    if (cbOutput <= field.iOptionalBitOffset / 8u)
                    {
                        WFCError( "Output wrong size (%Iu), expected optional bit-offset (%u)", cbOutput, field.iOptionalBitOffset );
                        NT_ASSERTMSG( "Output size wrong", FALSE );
                        status = NDIS_STATUS_BUFFER_TOO_SHORT;
                        goto EXIT;
                    }

                    ((BYTE*)pOutput)[field.iOptionalBitOffset / 8] |= (1 << (field.iOptionalBitOffset % 8));
                }
                if (field.fIsMulti)
                {
                    //
                    // Allocate the array
                    //
                    status = g_ArrayHelpers[field.ArrayOfElementsTypeIndex].AllocateElements( pbOutput, cMulti, Context->AllocationContext );
                    if (status != NDIS_STATUS_SUCCESS)
                    {
                        if (tlvType.fIsMessage)
                        {
                            WFCError( "Failed to allocate multi-array for %!MESSAGE_ID!::%!TLV_ID! status 0x%08X", tlvType.MsgId, field.tlvType, status );
                        }
                        else
                        {
                            WFCError( "Failed to allocate multi-array for %!TLV_ID!::%!TLV_ID! status 0x%08X", tlvType.TlvId, field.tlvType, status );
                        }
                        goto EXIT;
                    }

                    pbOutput = *(BYTE**)(pbOutput + FIELD_OFFSET(ArrayOfElements<BYTE>, pElements));
                    RtlZeroMemory( pbOutput, cMulti * elemSize );
                }
                else
                {
                    if (cMulti > 1)
                    {
                        if (tlvType.fIsMessage)
                        {
                            WFCError( "Single instance field %!TLV_ID! appears %u times in %!TLV_ID!", field.tlvType, cMulti, tlvType.MsgId );
                        }
                        else
                        {
                            WFCError( "Single instance field %!TLV_ID! appears %u times in %!TLV_ID!", field.tlvType, cMulti, tlvType.TlvId );
                        }
                        status = NDIS_STATUS_INVALID_DATA;
                        goto EXIT;
                    }
                }
            }

            //
            // Parse
            //
            {
                ULONG        cbRemainingBuffer = cbBuffer;
                const BYTE * pbRemainingBuffer = rgBuffer;

                for (UINT iMulti = 0; iMulti < cMulti; iMulti++)
                {
                    const BYTE * pbField = NULL;
                    UINT16 cbField = 0;

                    status = FindTlv( field.tlvType, cbRemainingBuffer, pbRemainingBuffer, &cbField, &pbField, &cbRemainingBuffer, &pbRemainingBuffer );
                    if (status != NDIS_STATUS_SUCCESS)
                    {
                        WFCError( "Internal parsing error (tlv %!TLV_ID! was here and now it's not) 0x%08X", field.tlvType, status );
                        goto EXIT;
                    }
                    if (field.fIsSimple)
                    {
                        status = ParseSimpleContainer( cbField, pbField, field.tlvType, *field.simpleInfo, Context, elemSize, pbOutput);
                    }
                    else
                    {
                        status = ParseAggregateContainer( cbField, pbField, MESSAGE_OR_TLV_ID( field.tlvType ), *field.nestedInfo, Context, elemSize, pbOutput );
                    }
                    if (status != NDIS_STATUS_SUCCESS)
                    {
                        WFCError( "Failed to parse nested tlv %!TLV_ID! 0x%08X", field.tlvType, status );
                        goto EXIT;
                    }
                    pbOutput += elemSize;
                }
            }

EXIT:
            TraceExit( status );
            return status;
        }

        void CleanupAllocations(
            _In_ const UINT16               cbOutput,
            _In_reads_( cbOutput ) BYTE   * pbOutput,
            _In_ const CONTAINER_REF_INFO & field )
        {
            const UINT16 fieldSize = field.GetUnpackedFieldOrElementSize();
            BYTE * pbField = pbOutput + field.cbUnpackedOffset;

            TraceEntry();
            WFCTrace( "Cleanup for field %!TLV_ID!", field.tlvType );

            if ((cbOutput < field.cbUnpackedOffset) ||
                ((cbOutput - field.cbUnpackedOffset) < field.GetUnpackedFieldSize()))
            {
                WFCError( "Output wrong size (%u), expected offset (%u) + size (%u)", cbOutput, field.cbUnpackedOffset, field.GetUnpackedFieldSize() );
                NT_ASSERTMSG( "Output size wrong", FALSE );
                goto EXIT;
            }

            if (field.fIsMulti)
            {
                g_ArrayHelpers[field.ArrayOfElementsTypeIndex].Cleanup( pbField );
            }
            else
            {
                //
                // Cleanup
                //
                if (field.fIsSimple)
                {
                    if (field.simpleInfo->fIsCollection)
                    {
                        g_ArrayHelpers[field.simpleInfo->ArrayOfElementsTypeIndex].Cleanup( pbField );
                    }
                }
                else
                {
                    //
                    // Recurse
                    //
                    for (UINT16 iField = 0; iField < field.nestedInfo->cntFields; iField++)
                    {
                        const CONTAINER_REF_INFO & nestedField = field.nestedInfo->rgFields[iField];
                        if ((nestedField.versionInfo == NULL) ||
                            (nestedField.versionInfo->versionRemoved == 0xFFFFFFFF))
                        {
                            CleanupAllocations( fieldSize, pbField, nestedField );
                        }
                    }
                }

            }

EXIT:
            TraceExit( NDIS_STATUS_SUCCESS );
        }

        NDIS_STATUS AllocateAndParseOldAggregateField(
            _In_ const ULONG                        cbBuffer,
            _In_reads_( cbBuffer ) const BYTE     * rgBuffer,
            _In_ MESSAGE_OR_TLV_ID                  tlvType,
            _In_ const CONTAINER_REF_INFO         & oldField,
            _In_opt_ const CONTAINER_REF_INFO     * pNewField,
            _In_ PCTLV_CONTEXT                      Context,
            _In_ size_t                             cbOutput,
            _Inout_updates_bytes_( cbOutput ) PVOID pOutput )
        {
            NDIS_STATUS    status = NDIS_STATUS_SUCCESS;
            const UINT16   oldFieldSize = oldField.GetUnpackedFieldSize();
            BYTE         * pbTempOutput = NULL;
            const UINT16   cbTempOutput = oldField.cbUnpackedOffset + oldFieldSize;

            TraceEntry();

            WFCTrace( "Converting old field %!TLV_ID! to new %u", oldField.tlvType, oldField.versionInfo->iFieldCurrentReplacement );

#pragma warning(suppress: 6255) // using _alloca. Size should be small
            pbTempOutput = (BYTE*)_alloca( cbTempOutput );
            RtlZeroMemory( pbTempOutput, cbTempOutput );
            status = ParseAggregateField( cbBuffer, rgBuffer, tlvType, oldField, Context, cbTempOutput, pbTempOutput );
            if (status != NDIS_STATUS_SUCCESS)
            {
                goto EXIT;
            }

            PVOID pParsedField = (!oldField.fIsOptional || (pbTempOutput[0] & 1)) ? pbTempOutput + oldField.cbUnpackedOffset : NULL;

            if (pNewField)
            {
                status = oldField.versionInfo->ConvertReplacedFieldFromParse( pParsedField, Context, ((BYTE*)pOutput) + pNewField->cbUnpackedOffset );
                if (pNewField->fIsOptional && (status == NDIS_STATUS_SUCCESS))
                {
                    if (cbOutput <= pNewField->iOptionalBitOffset / 8u)
                    {
                        WFCError( "Output wrong size (%Iu), expected optional bit-offset (%u)", cbOutput, pNewField->iOptionalBitOffset );
                        NT_ASSERTMSG( "Output size wrong", FALSE );
                        status = NDIS_STATUS_BUFFER_TOO_SHORT;
                        goto EXIT;
                    }

                    ((BYTE*)pOutput)[pNewField->iOptionalBitOffset / 8] |= (1 << (pNewField->iOptionalBitOffset % 8));
                }
            }
            else
            {
                status = oldField.versionInfo->ConvertRemovedFieldFromParse( pParsedField, Context, pOutput );
            }

            if (!NT_SUCCESS( status ))
            {
                WFCError( "Failed to convert %!TLV_ID!. Error = 0x%x", oldField.tlvType, status );
                NT_ASSERT( FALSE );
                goto EXIT;
            }
            status = NDIS_STATUS_SUCCESS;

EXIT:
            CleanupAllocations( cbTempOutput, pbTempOutput, oldField );
            TraceExit( status );
            return status;
        }

        NDIS_STATUS ParseMessage(
            _In_ const ULONG                        cbBuffer,
            _In_reads_( cbBuffer ) const BYTE     * rgBuffer,
            _In_ ENUMS::MESSAGE_ID                  msgId,
            _In_ const AGGREGATE_INFO             & cont,
            _In_ PCTLV_CONTEXT                      Context,
            _In_ size_t                             cbOutput,
            _Out_writes_bytes_( cbOutput ) PVOID    pOutput )
        {
            NDIS_STATUS status;
            TraceEntry();

            RtlZeroMemory( pOutput, cbOutput );

            if (Context == NULL)
            {
                WFCError( "Missing Context parameter!" );
                status = NDIS_STATUS_INVALID_PARAMETER;
            }
            else if (Context->PeerVersion < WDI_VERSION_MIN_SUPPORTED)
            {
                WFCError( "Unsupported PeerVersion (0x%08X) should be at least 0x%08X", Context->PeerVersion, WDI_VERSION_MIN_SUPPORTED );
                status = NDIS_STATUS_UNSUPPORTED_REVISION;
            }
            else
            {
                status = ParseAggregateContainer( cbBuffer, rgBuffer, msgId, cont, Context, cbOutput, pOutput );
            }

            TraceExit( status );
            return status;
        }

        NDIS_STATUS ParseAggregateContainer(
            _In_ const ULONG                        cbBuffer,
            _In_reads_( cbBuffer ) const BYTE     * rgBuffer,
            _In_ MESSAGE_OR_TLV_ID                  tlvType,
            _In_ const AGGREGATE_INFO             & cont,
            _In_ PCTLV_CONTEXT                      Context,
            _In_ size_t                             cbOutput,
            _Inout_updates_bytes_( cbOutput ) PVOID pOutput )
        {
            NDIS_STATUS status = NDIS_STATUS_SUCCESS;

            TraceEntry();

            if (tlvType.fIsMessage)
            {
                WFCTrace( "Parsing %!MESSAGE_ID!", tlvType.MsgId );
            }
            else
            {
                WFCTrace( "Parsing %!TLV_ID!", tlvType.TlvId );
            }

            if (cont.cbUnpackedSize != cbOutput)
            {
                if (tlvType.fIsMessage)
                {
                    WFCError( "Structure for %!MESSAGE_ID! not the right size: %Iu, expected %u", tlvType.MsgId, cbOutput, cont.cbUnpackedSize );
                }
                else
                {
                    WFCError( "Structure for %!TLV_ID! not the right size: %Iu, expected %u", tlvType.TlvId, cbOutput, cont.cbUnpackedSize );
                }
                status = NDIS_STATUS_FAILURE;
                goto EXIT;
            }

            for (int iField = 0; iField < cont.cntFields; iField++)
            {
                //
                // First check for version applicability
                // and process all _current_ (non-removed) fields
                //
                const VERSION_INFO * vi = cont.rgFields[iField].versionInfo;
                if (vi != NULL)
                {
                    if (Context->PeerVersion < vi->versionAdded)
                    {
                        // Peer is too old, nothing to parse
                        continue;
                    }
                    if (vi->versionRemoved != 0)
                    {
                        // Save all removed fields for later
                        continue;
                    }
                }

                status = ParseAggregateField( cbBuffer, rgBuffer, tlvType, cont.rgFields[iField], Context, cbOutput, pOutput );
                if (status != NDIS_STATUS_SUCCESS)
                {
                    goto EXIT;
                }
            }

            for (int iField = 0; iField < cont.cntFields; iField++)
            {
                //
                // Check for version applicability
                // and process all replaced fields
                //
                const VERSION_INFO * vi = cont.rgFields[iField].versionInfo;
                if (vi == NULL)
                {
                    continue;
                }
                if (Context->PeerVersion < vi->versionAdded)
                {
                    // Peer is too old, nothing to parse
                    continue;
                }
                if (Context->PeerVersion >= vi->versionRemoved)
                {
                    // Peer is too new (or field is never removed), nothing to parse
                    continue;
                }
                if (vi->iFieldCurrentReplacement == 0xFFFFFFFF)
                {
                    // removed, not just replaced
                    continue;
                }

                status = AllocateAndParseOldAggregateField( cbBuffer, rgBuffer, tlvType, cont.rgFields[iField],
                    &(cont.rgFields[vi->iFieldCurrentReplacement]), Context, cbOutput, pOutput );
                if (status != NDIS_STATUS_SUCCESS)
                {
                    goto EXIT;
                }
            }

            for (int iField = 0; iField < cont.cntFields; iField++)
            {
                //
                // Check for version applicability
                // and process all removed fields
                //
                const VERSION_INFO * vi = cont.rgFields[iField].versionInfo;
                if (vi == NULL)
                {
                    continue;
                }
                if (Context->PeerVersion < vi->versionAdded)
                {
                    // Peer is too old, nothing to parse
                    continue;
                }
                if (Context->PeerVersion >= vi->versionRemoved)
                {
                    // Peer is too new (or field is never removed), nothing to parse
                    continue;
                }
                if (vi->iFieldCurrentReplacement != 0xFFFFFFFF)
                {
                    // replaced, not removed
                    continue;
                }

                status = AllocateAndParseOldAggregateField( cbBuffer, rgBuffer, tlvType, cont.rgFields[iField],
                    NULL, Context, cbOutput, pOutput );
                if (status != NDIS_STATUS_SUCCESS)
                {
                    goto EXIT;
                }
            }

EXIT:
            TraceExit( status );
            return status;
        }
    };

    namespace GENERATOR
    {

        //
        // Given a pointer to an AggregateContainer structure, and a
        // CONTAINER_REF_INFO, determines if the container is optional and
        // present.
        //
        // Returns:
        //  NDIS_STATUS_SUCCESS if optional and present
        //  NDIS_STATUS_NOT_COPIED if optional and NOT present
        //  NDIS_STATUS_NOT_RECOGNIZED if not optional
        //
        NDIS_STATUS IsOptionalAggregateFieldPresent(
            _In_ UINT16                                       cbInputAggregate,
            _In_reads_bytes_( cbInputAggregate ) const VOID * pInputAggregate,
            _In_ const CONTAINER_REF_INFO                   & field )
        {
            if (field.fIsOptional)
            {
                if ((field.iOptionalBitOffset / 8u) >= cbInputAggregate)
                {
                    WFCError( "BitOffset %u greater than aggregate size %u", field.iOptionalBitOffset, cbInputAggregate );
                    NT_ASSERTMSG( "OptionalBitOffset wrong or buffer size wrong", FALSE );
                    return NDIS_STATUS_BUFFER_TOO_SHORT;
                }
                if (((BYTE*)pInputAggregate)[field.iOptionalBitOffset / 8] & (1 << (field.iOptionalBitOffset % 8)))
                {
                    WFCTrace( "%!TLV_ID! is present", field.tlvType );
                    return NDIS_STATUS_SUCCESS;
                }
                return NDIS_STATUS_NOT_COPIED;
            }
            return NDIS_STATUS_NOT_RECOGNIZED;
        }

        //
        // Given a pointer to a simple container field (and size) and the
        // corresponding CONTAINER_REF_INFO, determines the size required
        // in the resulting TLV byte array, including the TLV header.
        // If output buffer (pbTlv) is provided, copies into buffer.
        // Also handles 'isCollection', but *NOT* 'multi'
        //
        NDIS_STATUS GenerateSingleContainer(
            _In_ UINT16                              cbField,
            _In_reads_bytes_( cbField ) const VOID * pField,
            _In_ const CONTAINER_REF_INFO          & field,
            _In_ PCTLV_CONTEXT                       Context,
            _Inout_ UINT16                         * pcbSize,
            _Out_writes_opt_( *pcbSize ) BYTE      * pbTlv )
        {
            NDIS_STATUS status;
            UINT16 nestedSizeNoHeader;
            BYTE * nestedTlv;
            BYTE * pbData = (BYTE*)pField;

            TraceEntry();

            WFCTrace( "Generate %!TLV_ID!", field.tlvType );

            if (pbTlv == NULL)
            {
                *pcbSize = 0;
                nestedSizeNoHeader = 0;
                nestedTlv = NULL;
            }
            else
            {
                if (*pcbSize < WDI_TLV_HEADER_SIZE)
                {
                    WFCError( "Buffer for %!TLV_ID! too small for header 0x%X", field.tlvType, *pcbSize );
                    status = NDIS_STATUS_BUFFER_TOO_SHORT;
                    goto EXIT;
                }
                reinterpret_cast<UINT16*>(pbTlv)[0] = field.tlvType;
                nestedSizeNoHeader = *pcbSize - WDI_TLV_HEADER_SIZE;
                nestedTlv = pbTlv + WDI_TLV_HEADER_SIZE;
            }

            if (field.fIsSimple)
            {
                const UINT16 fieldSize = field.simpleInfo->cbPackedSize;
                size_t cntElems;
                if (field.simpleInfo->fIsCollection)
                {
                    const ArrayOfElements<BYTE> * pTempArray = reinterpret_cast<const ArrayOfElements<BYTE>*>(pField);
                    if (pTempArray->ElementCount)
                    {
                        UINT32 arraySize = 0;
                        status = RtlUInt32Mult( pTempArray->ElementCount, fieldSize, &arraySize );
                        if (!NT_SUCCESS( status ))
                        {
                            WFCError( "TLV %!TLV_ID! too many elements (0x%x * 0x%x), status=0x%08X", field.tlvType, pTempArray->ElementCount, fieldSize, status );
                            goto EXIT;
                        }
                        status = RtlUInt32ToUInt16( arraySize, &nestedSizeNoHeader );
                        if (!NT_SUCCESS( status ))
                        {
                            WFCError( "TLV %!TLV_ID! collection size too big (0x%x), status=0x%08X", field.tlvType, arraySize, status );
                            goto EXIT;
                        }
                        cntElems = pTempArray->ElementCount;
                        pbData = pTempArray->pElements;
                        NT_ASSERT_ASSUME( cntElems * fieldSize == nestedSizeNoHeader );
                    }
                    else if (!field.simpleInfo->fIsZeroAllowed)
                    {
                        WFCError( "Zero elements passed in for LIST<%!TLV_ID!>.", field.tlvType );
                        status = NDIS_STATUS_INVALID_DATA;
                        goto EXIT;
                    }
                    else
                    {
                        WFCTrace( "Zero elements passed in for LIST<%!TLV_ID!> (allowed).", field.tlvType );
                        nestedSizeNoHeader = 0;
                        cntElems = 0;
                    }
                }
                else
                {
                    nestedSizeNoHeader = fieldSize;
                    cntElems = 1;
                }

                if (pbTlv)
                {
                    // Guaranteed by first pass
                    NT_ASSERT_ASSUME( *pcbSize >= WDI_TLV_HEADER_SIZE + nestedSizeNoHeader );

                    const CONTAINER_INFO & cont = *field.simpleInfo;
                    if (cont.cbPackedSize == cont.cbUnpackedSize)
                    {
                        WFCTrace( "Generating in all field(s) at once (%u bytes)", nestedSizeNoHeader );
                        //
                        // Shortcut to copy everything all at once
                        //
                        RtlCopyMemory( nestedTlv, pbData, nestedSizeNoHeader );
                    }
                    else
                    {
                        UINT32 iBuffer = 0;
                        for (size_t iElem = 0; iElem < cntElems; iElem++)
                        {
                            //
                            // 'Generate' a single propertyGroup/namedType
                            //
                            _Writable_bytes_( field.cbUnpackedSize ) BYTE * pbElem = &(pbData[iElem * cont.cbUnpackedSize]);
                            for (int iField = 0; iField < cont.cntFields; iField++)
                            {
                                WFCTrace( "Generating in field %!FIELD_NAME!", cont.rgFields[iField].fieldName );

                                const UINT32 len = cont.rgFields[iField].packedLen;
                                const UINT16 offset = cont.rgFields[iField].unpackedOffset;

                                // Indicates a bug in WABIGEN!
                                NT_ASSERT_ASSUME( offset + len <= cont.cbUnpackedSize );

                                // Validated up above
                                NT_ASSERT_ASSUME( iBuffer + len <= nestedSizeNoHeader );
                                RtlCopyMemory( nestedTlv + iBuffer, pbElem + offset, len );
                                iBuffer += len;
                            }
                        }
                    }
                }
            }
            else
            {
                UINT32 rawNestedSizeNoHeader = nestedSizeNoHeader;
                WFCTrace( "Recurse on aggregate container %!TLV_ID!", field.tlvType );
                status = GenerateAggregate( cbField, pField, *field.nestedInfo, Context, &rawNestedSizeNoHeader, nestedTlv );
                if (status != NDIS_STATUS_SUCCESS)
                {
                    goto EXIT;
                }

                // Even though we have the rawNestedSizeNoHeader as UINT32 for the single container this is going
                // to be inside a TLV, so we need this to be MAXUINT16
                status = RtlUInt32ToUInt16( rawNestedSizeNoHeader, &nestedSizeNoHeader );
                if (!NT_SUCCESS( status ))
                {
                    WFCError( "TLV %!TLV_ID! collection size too big (0x%x), status=0x%08X", field.tlvType, rawNestedSizeNoHeader, status );
                    goto EXIT;
                }
            }

            // Here we are enforing that the nestedSizeNoHeader + WDI_TLV_HEADER_SIZE < MAXUINT16. This is slightly 
            // more restrictive (we could allow the nestedSizeNoHeader to go upto MAXUINT16), but that would mean we would
            // lose a lot of easy size validation capability (everwhere we will have to use UINT32)
            UINT16 nestedSize = 0;
            status = RtlUInt16Add( nestedSizeNoHeader, WDI_TLV_HEADER_SIZE, &nestedSize );
            if (!NT_SUCCESS( status ))
            {
                WFCError( "TLV %!TLV_ID! too big for header 0x%x, status=0x%08X", field.tlvType, (UINT32)nestedSizeNoHeader + WDI_TLV_HEADER_SIZE, status );
                goto EXIT;
            }

            // Ignore any warnings
            status = NDIS_STATUS_SUCCESS;
            if (pbTlv)
            {
                if (*pcbSize < nestedSize)
                {
                    WFCError( "First pass computed wrong size! %!TLV_ID! need 0x%08X, provided 0x%08X", field.tlvType, nestedSize, *pcbSize );
                    status = NDIS_STATUS_BUFFER_TOO_SHORT;
                    goto EXIT;
                }
                reinterpret_cast<UINT16*>(pbTlv)[1] = nestedSizeNoHeader;
            }
            *pcbSize = nestedSize;

EXIT:
            TraceExit( status );
            return status;
        }

        //
        // Given a pointer to a container field (and size) and the
        // corresponding CONTAINER_REF_INFO, determines the the size required
        // in the resulting TLV byte array, including the TLV header(s).
        //
        NDIS_STATUS GenerateMultiContainer(
            _In_ UINT16                              cbField,
            _In_reads_bytes_( cbField ) const VOID * pField,
            _In_ const CONTAINER_REF_INFO          & field,
            _In_ PCTLV_CONTEXT                       Context,
            _Inout_ UINT32                         * pcbSize,
            _Out_writes_opt_( *pcbSize ) BYTE      * pbTlv )
        {
            NDIS_STATUS status;
            UINT32 cbSize = 0;
            const BYTE * pbData = reinterpret_cast<const BYTE*>(pField);
            UINT16 cbData = cbField;


            TraceEntry();

            WFCTrace( "Generate %!TLV_ID!", field.tlvType );

            if (pbTlv == NULL)
            {
                *pcbSize = 0;
            }

            UINT32 elemCount = 1;
            if (field.fIsMulti)
            {
                UINT32 cbTemp;
                const ArrayOfElements<BYTE> * pTempArray = reinterpret_cast<const ArrayOfElements<BYTE>*>(pField);
                if (!pTempArray->ElementCount)
                {
                    WFCError( "Zero elements passed in for multi-%!TLV_ID!.", field.tlvType );
                    status = NDIS_STATUS_INVALID_DATA;
                    goto EXIT;
                }
                pbData = pTempArray->pElements;
                elemCount = pTempArray->ElementCount;
                status = RtlUInt32Mult( field.GetUnpackedFieldOrElementSize(), elemCount, &cbTemp );
                if (!NT_SUCCESS( status ))
                {
                    WFCError( "Too many elements (%u) * bytes (%u) 0x%08X", field.GetUnpackedFieldOrElementSize(), elemCount, status );
                    goto EXIT;
                }
                status = RtlUInt32ToUInt16( cbTemp, &cbData );
                if (!NT_SUCCESS( status ))
                {
                    WFCError( "Too many elements (%u) * bytes (%u) 0x%08X", field.GetUnpackedFieldOrElementSize(), elemCount, status );
                    goto EXIT;
                }
            }

            for (UINT32 i = 0; i < elemCount; i++, pbData = pbData + field.GetUnpackedFieldOrElementSize())
            {
                UINT16 nestedSize = 0;
                BYTE * nestedTlv = NULL;
                if (pbTlv)
                {
                    // The nestedSize is going to be set inside the TLV's Length. So it is limited to MAXUINT16 size.
                    // We are intentionally truncating from UINT32 to UINT16 here
                    nestedSize = static_cast<UINT16>(min((*pcbSize - cbSize), MAXUINT16));
                    nestedTlv = pbTlv + cbSize;
                }

                status = GenerateSingleContainer( cbData, pbData, field, Context, &nestedSize, nestedTlv );
                if (status != NDIS_STATUS_SUCCESS)
                {
                    goto EXIT;
                }
                
                // Allow 32 bit math here to figure out how much total length is used
                UINT32 newSize = 0;
                status = RtlUInt32Add( cbSize, (UINT32)nestedSize, &newSize );
                if (!NT_SUCCESS( status ))
                {
                    if (field.fIsMulti)
                    {
                        WFCError( "TLV too big for nested field %!TLV_ID![%u] 0x%x, status=0x%08X", field.tlvType, i, cbSize + nestedSize, status );
                    }
                    else
                    {
                        WFCError( "TLV too big for nested field %!TLV_ID! 0x%x, status=0x%08X", field.tlvType, cbSize + nestedSize, status );
                    }
                    goto EXIT;
                }
                cbSize = newSize;
            }

            // Ignore any warnings
            status = NDIS_STATUS_SUCCESS;
            *pcbSize = cbSize;

EXIT:
            TraceExit( status );
            return status;
        }

        NDIS_STATUS AllocateAndGenerateOldContainer(
            _In_ UINT16                              cbInput,
            _In_reads_bytes_( cbInput ) const VOID * pInput,
            _In_ const CONTAINER_REF_INFO          & oldField,
            _In_ const CONTAINER_REF_INFO          * newField,
            _In_ PCTLV_CONTEXT                       Context,
            _Inout_ UINT32                         * pcbSize,
            _Out_writes_opt_( *pcbSize ) BYTE      * pbTlv )
        {
            const UINT16   cbTempInput = oldField.GetUnpackedFieldSize() + oldField.cbUnpackedOffset;
            BYTE         * pbTempInput = NULL;
            NDIS_STATUS    status;

            TraceEntry();

            WFCTrace( "Converting old field %!TLV_ID! from new %u", oldField.tlvType, oldField.versionInfo->iFieldCurrentReplacement );

#pragma warning(suppress: 6255) // using _alloca. Size should be small
            pbTempInput = (BYTE*)_alloca( cbTempInput );
            RtlZeroMemory( pbTempInput, cbTempInput );

            if (oldField.versionInfo->iFieldCurrentReplacement == 0xFFFFFFFF)
            {
                status = oldField.versionInfo->ReconstructRemovedFieldForGenerate(
                    pInput, (pbTlv == NULL), Context, pbTempInput + oldField.cbUnpackedOffset );
            }
            else
            {
                status = IsOptionalAggregateFieldPresent( cbInput, pInput, *newField );
                if (!NT_SUCCESS( status ))
                {
                    goto EXIT;
                }
                if (cbInput < newField->cbUnpackedOffset + newField->GetUnpackedFieldSize())
                {
                    WFCError( "Bad TLV metadata offset (0x%X) + size (0x%X) should be <= cbInput (0x%X)", newField->cbUnpackedOffset, newField->GetUnpackedFieldSize(), cbInput );
                    NT_ASSERT( FALSE );
                    status = NDIS_STATUS_BUFFER_TOO_SHORT;
                    goto EXIT;
                }
                status = oldField.versionInfo->ReconstructReplacedFieldForGenerate(
                    (status != NDIS_STATUS_NOT_COPIED),
                    (const BYTE*)pInput + newField->cbUnpackedOffset,
                    (pbTlv == NULL),
                    Context,
                    pbTempInput + oldField.cbUnpackedOffset );
            }
            if (!NT_SUCCESS( status ))
            {
                WFCError( "Failed to reconstruct %!TLV_ID!, 0x%08X", oldField.tlvType, status );
                goto EXIT;
            }

            if (oldField.fIsOptional && (status != NDIS_STATUS_INDICATION_REQUIRED))
            {
                WFCTrace( "Reconstructed %!TLV_ID! is optional and not present, skipping", oldField.tlvType );
                status = NDIS_STATUS_NOT_COPIED;
                *pcbSize = 0;
                goto EXIT;
            }

            status = GenerateMultiContainer(
                cbTempInput - oldField.cbUnpackedOffset,
                pbTempInput + oldField.cbUnpackedOffset,
                oldField, Context, pcbSize, pbTlv );

EXIT:
            PARSER::CleanupAllocations( cbTempInput, pbTempInput, oldField );
            TraceExit( status );
            return status;
        }

        NDIS_STATUS GenerateAggregate(
            _In_ UINT16                              cbInput,
            _In_reads_bytes_( cbInput ) const VOID * pInput,
            _In_ const AGGREGATE_INFO              & cont,
            _In_ PCTLV_CONTEXT                       Context,
            _Inout_ UINT32                         * pcbSize,
            _Out_writes_opt_( *pcbSize ) BYTE      * pbTlv )
        {
            NDIS_STATUS status = NDIS_STATUS_SUCCESS;
            UINT32 cbSize = 0;

            TraceEntry();

            if (pbTlv == NULL)
            {
                *pcbSize = 0;
            }

            for (int iField = 0; iField < cont.cntFields; iField++)
            {
                const CONTAINER_REF_INFO & field = cont.rgFields[iField];
                UINT32 nestedSize = 0;
                BYTE * nestedTlv = NULL;
                if (pbTlv)
                {
                    // We maybe getting put inside a TLV, but we dont know that (we could also just be in a message).
                    // So we will work with UINT32 and let the caller deal with whether what we generated is OK size
                    // or not
                    nestedSize = *pcbSize - cbSize;
                    nestedTlv = pbTlv + cbSize;
                }

                //
                // First check for version applicability
                //
                const VERSION_INFO * vi = field.versionInfo;
                if (vi != NULL)
                {
                    if (Context->PeerVersion < vi->versionAdded)
                    {
                        // Peer is too old, nothing to generate
                        continue;
                    }
                    if (vi->versionRemoved != 0)
                    {
                        if (Context->PeerVersion >= vi->versionRemoved)
                        {
                            // Peer is too new, nothing to generate
                            continue;
                        }
                        //
                        // This is not the newest TLV
                        // we need to reconstruct an older field
                        // and generate based on it.
                        //
                        status = AllocateAndGenerateOldContainer( cbInput, pInput, field, vi->iFieldCurrentReplacement != 0xFFFFFFFF ? &(cont.rgFields[vi->iFieldCurrentReplacement]) : NULL, Context, &nestedSize, nestedTlv );
                        if (!NT_SUCCESS(status))
                        {
                            goto EXIT;
                        }
                        status = NDIS_STATUS_SUCCESS;
                        goto ACCUMULATE_FIELD_SIZE;
                    }
                }

                //
                // Next check for optional
                //
                status = IsOptionalAggregateFieldPresent( cbInput, pInput, field );
                if (!NT_SUCCESS( status ))
                {
                    goto EXIT;
                }
                if (status == NDIS_STATUS_NOT_COPIED)
                {
                    status = NDIS_STATUS_SUCCESS;
                    continue;
                }

                status = GenerateMultiContainer( field.GetUnpackedFieldSize(), ((const BYTE*)pInput) + field.cbUnpackedOffset, field, Context, &nestedSize, nestedTlv );
                if (status != NDIS_STATUS_SUCCESS)
                {
                    WFCError( "Failed field %!TLV_ID! status=0x%08X", field.tlvType, status );
                    goto EXIT;
                }
ACCUMULATE_FIELD_SIZE:
                // Allow 32 bit math here to figure out how much total length is used
                status = RtlUInt32Add( cbSize, nestedSize, &cbSize );
                if (!NT_SUCCESS( status ))
                {
                    WFCError( "TLV too big for nested field %!TLV_ID! 0x%x + 0x%x, status=0x%08X", field.tlvType, cbSize, nestedSize, status );
                    goto EXIT;
                }
            }

            *pcbSize = cbSize;

EXIT:
            TraceExit( status );
            return status;
        }

        NDIS_STATUS GenerateMessage(
            _In_ UINT16                                cbInput,
            _In_reads_bytes_( cbInput ) const VOID   * pInput,
            _In_ ENUMS::MESSAGE_ID                     msgId,
            _In_ const AGGREGATE_INFO                & cont,
            _In_ ULONG                                 cbReserve,
            _In_ PCTLV_CONTEXT                         Context,
            _Out_ ULONG                              * pcbSize,
            _Outptr_result_buffer_( *pcbSize ) BYTE ** ppbTlv )
        {
            NDIS_STATUS status = NDIS_STATUS_SUCCESS;
            ULONG  cbSize = 0;
            UINT32 cbMessage = 0;
            BYTE * pbMessage = NULL;

            TraceEntry();

            WFCTrace( "Generate %!MESSAGE_ID!", msgId );

            *pcbSize = 0;
            *ppbTlv = NULL;

            if (Context == NULL)
            {
                WFCError( "Missing Context parameter!" );
                status = NDIS_STATUS_INVALID_PARAMETER;
                goto EXIT;
            }
            if (Context->PeerVersion < WDI_VERSION_MIN_SUPPORTED)
            {
                WFCError( "Unsupported PeerVersion (0x%08X) should be at least 0x%08X", Context->PeerVersion, WDI_VERSION_MIN_SUPPORTED );
                status = NDIS_STATUS_UNSUPPORTED_REVISION;
                goto EXIT;
            }

            status = GenerateAggregate( cbInput, pInput, cont, Context, &cbMessage, NULL );
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCError( "Failed to compute length of TLV for Message %!MESSAGE_ID! status = 0x%08X", msgId, status );
                goto EXIT;
            }

            status = RtlULongAdd( cbMessage, cbReserve, &cbSize );
            if (!NT_SUCCESS( status ))
            {
                WFCError( "TLV length 0x%x + Reserve length 0x%x too large (0x%08X)", cbMessage, cbReserve, status );
                goto EXIT;
            }

            pbMessage = new (Context->AllocationContext) BYTE[cbSize];
            if (pbMessage == NULL)
            {
                status = NDIS_STATUS_RESOURCES;
                goto EXIT;
            }
            RtlZeroMemory( pbMessage, cbSize );

            status = GenerateAggregate( cbInput, pInput, cont, Context, &cbMessage, pbMessage + cbReserve );
            if (status != NDIS_STATUS_SUCCESS)
            {
                WFCError( "Failed to generate TLV for Message %!MESSAGE_ID! status = 0x%08X", msgId, status );
                goto EXIT;
            }

            *pcbSize = cbSize;
            *ppbTlv = pbMessage;
            pbMessage = NULL;

EXIT:
            if (pbMessage != NULL)
            {
                FreeGenerated( pbMessage );
            }

            TraceExit( status );
            return status;
        }

        NDIS_STATUS GenerateEmptyMessage(
            _In_opt_ void const *                      pInput,
            _In_ ULONG                                 cbReserve,
            _In_ PCTLV_CONTEXT                         Context,
            _Out_ ULONG                              * pcbSize,
            _Outptr_result_buffer_( *pcbSize ) BYTE ** ppbTlv )
        {
            NDIS_STATUS status = NDIS_STATUS_SUCCESS;

            TraceEntry();

            UNREFERENCED_PARAMETER( pInput );

            *pcbSize = 0;
            *ppbTlv = NULL;

            if (Context == NULL)
            {
                WFCError( "Missing Context parameter!" );
                status = NDIS_STATUS_INVALID_PARAMETER;
                goto EXIT;
            }
            if (Context->PeerVersion < WDI_VERSION_MIN_SUPPORTED)
            {
                WFCError( "Unsupported PeerVersion (0x%08X) should be at least 0x%08X", Context->PeerVersion, WDI_VERSION_MIN_SUPPORTED );
                status = NDIS_STATUS_UNSUPPORTED_REVISION;
                goto EXIT;
            }

            BYTE * pbMessage = new (Context->AllocationContext) BYTE[cbReserve];
            if (pbMessage == NULL)
            {
                status = NDIS_STATUS_RESOURCES;
            }
            else
            {
                RtlZeroMemory( pbMessage, cbReserve );
                *pcbSize = cbReserve;
                *ppbTlv = pbMessage;
            }

EXIT:

            TraceExit( status );
            return status;
        }
    };
};

