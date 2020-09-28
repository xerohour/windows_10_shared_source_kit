#pragma once

#define IS_WDI_MESSAGE_ID_IHV_RESERVED ( MESSAGEID ) (0x400 &( MESSAGEID ))
#define IS_WDI_MESSAGE_ID_OS_RESERVED  ( MESSAGEID ) (0x800 &( MESSAGEID ))
#define WDI_MESSAGE_ID_IHV_BASE (0x400)
#define WDI_MESSAGE_ID_OS_BASE  (0x800)

#define WDI_INTERNAL_RAW_OID_REQUEST    0x801

enum WFC_MESSAGE_TYPE {
    WfcMessageCommand = 1,
    WfcMessageResponse = 3,
    WfcMessageIndication = 4
};

// Prefix for command/response/indication data used within the UE. For commands, this is created initially 
// & removed before calling IHV. For response and indications this is called when IHV calls us (completes
// OID or sends indication)
typedef struct _WFC_MESSAGE_METADATA {
public:
    // WFC Internal only fields
    ULONG                   MessageId;      // WABI Message ID
    UINT32                  MessageLength;  // Length of actual message to IHV

    // These flags dont apply to all message types, but being here works better than 
    // being inside the Command/Response structure since they are now always
    // on the same location for different message types.
    UINT32                  UseDirectOidCall:1;
    UINT32                  RetryOnSmallBuffer:1;
    UINT32                  NonWdiOid:1;
    // When adding new flags, try to make the default for a flags as 0

    // Stuff above this can be copied between different message types
    WFC_MESSAGE_TYPE        MessageType;    // M1, M3, M4

private:
    // Define these structures so they can be used below in the union and returned as
    // reference to consumers
    struct _WFC_COMMAND_METADATA{
        // Command specific private fields                 
        NDIS_OID            MappedWdiOid;
        UINT32              MinimumOutputLength;
        PNDIS_OID_REQUEST   pOriginalOidRequest;    // When available
    };

    struct _WFC_RESPONSE_METADATA{
        // Response specific private fields
        NDIS_OID            MappedWdiOid;
        NDIS_STATUS         OidCompletionStatus;              
        UINT32              OidBytesNeeded;
        UINT32              OidBytesWritten;
    };
    
    struct _WFC_INDICATION_METADATA{
        // Indicate specific private fields
        NDIS_STATUS         MappedWdiIndicationCode;
    };

    union {
        struct _WFC_COMMAND_METADATA _Command;
        struct _WFC_RESPONSE_METADATA _Response;
        struct _WFC_INDICATION_METADATA _Indication;

    };

public:    
    // Public Fields. Nothing can be added after here
    WDI_MESSAGE_HEADER     Message;
    // The message TLVs sit here

    // Defining a constructor to get clean initialization
    _WFC_MESSAGE_METADATA()
    {
        Initialize(WfcMessageCommand);
    }
    
    void Initialize(WFC_MESSAGE_TYPE messageType)
    {
        // Clear flags
        UseDirectOidCall = FALSE;
        RetryOnSmallBuffer = FALSE;
        NonWdiOid = FALSE;

        MessageType = messageType;

        _Command.pOriginalOidRequest = NULL;
    }

    struct _WFC_COMMAND_METADATA * Command()
    {
        NT_ASSERT(MessageType == WfcMessageCommand);
        return &_Command;
    }

    struct _WFC_RESPONSE_METADATA * Response()
    {
        NT_ASSERT(MessageType == WfcMessageResponse);
        return &_Response;
    }

    struct _WFC_INDICATION_METADATA * Indication()
    {
        NT_ASSERT(MessageType == WfcMessageIndication);
        return &_Indication;
    }
    
} WFC_MESSAGE_METADATA, *PWFC_MESSAGE_METADATA;
