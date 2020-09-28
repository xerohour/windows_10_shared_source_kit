//---------------------------------------------------------------------------
//
//  Module:   validate.cpp
//
//  Description:
//
//











//
//  Copyright (c) Microsoft.  All rights reserved.
//
//---------------------------------------------------------------------------

#include "private.h"

//  Implemented in property.cpp- should pick up the definition from a header

NTSTATUS
PropertyItemPropertyHandler
(
    IN      PIRP            pIrp,
    IN      PKSIDENTIFIER   pKsIdentifier,
    IN OUT  PVOID           pvData      OPTIONAL
);

NTSTATUS
PcValidateDS3DBufferAll
(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    KSDS3D_BUFFER_ALL& BufferSpec
);

NTSTATUS
PcValidateDS3DBufferMode
(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    ULONG& Mode
);

NTSTATUS
PcValidateAudioStereoSpeakerGeometry
(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    LONG& Setting
);

NTSTATUS
PcValidateAudioChannelConfig
(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    KSAUDIO_CHANNEL_CONFIG& Config
);

NTSTATUS
PcValidateKsTime
(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    KSTIME& Time
);

NTSTATUS
PcValidateSynthChannelGroups
(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    ULONG& GroupCount
);

NTSTATUS
PcValidateKernelMode
(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    PVOID pRights
);

NTSTATUS
PcValidateSetStreamDegradation
(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    KSMULTIPLE_ITEM& ItemsSpec
);

NTSTATUS
PcValidateKsRate
(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    KSRATE& ksRate
);

//  steal some KS macros so I can shrink the amount of data used.
#define DEFINE_PCVAPROPERTY_TABLE(tablename)\
    const PCVAPROPERTY_ITEM tablename[] =

#define DEFINE_PCVAPROPERTY_ITEM(PropertyId, GetHandler,\
                               MinProperty,\
                               MinData,\
                               SetHandler)\
{\
    PropertyId, (PFNKSHANDLER)GetHandler, MinProperty, MinData,\
    (PFNKSHANDLER)SetHandler,\
}

typedef struct {
    ULONG                   PropertyId;
    union {
        PFNKSHANDLER            GetPropertyHandler;
        BOOLEAN                 GetSupported;
    };
    ULONG                   MinProperty;
    ULONG                   MinData;
    union {
        PFNKSHANDLER            SetPropertyHandler;
        BOOLEAN                 SetSupported;
    };
} PCVAPROPERTY_ITEM, *PPCVAPROPERTY_ITEM;

#define DEFINE_PCVAPROPERTY_SET(Set,\
                              PropertiesCount,\
                              PropertyItem)\
{\
    Set,\
    PropertiesCount,\
    PropertyItem\
}

#define DEFINE_PCVAPROPERTY_SET_TABLE(tablename)\
    const PCVAPROPERTY_SET tablename[] =

typedef struct {
    const GUID*                 Set;
    ULONG                       PropertiesCount;
    const PCVAPROPERTY_ITEM*    PropertyItem;
} PCVAPROPERTY_SET, *PPCVAPROPERTY_SET;


//  All DDK-defined KSPropSetID_Audio + one picked up from SysAudio

DEFINE_PCVAPROPERTY_TABLE(AudioPropertyValidationHandlers) 
{
    //  NOTE:  There is a list of defined GUIDs- but I assumed others are 
    //  possible, so no validation is done
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_3D_INTERFACE,                  // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(GUID),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Best not to validate bools against TRUE/FALSE
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_AGC,                           // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  NOTE:  There is a list of defined GUIDs- but I assumed others are 
    //  possible, so no validation is done
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_ALGORITHM_INSTANCE,            // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(GUID),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  All values are valid on set, no validation needed
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_BASS,                          // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_BASS_BOOST,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Some validation possible
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_CHANNEL_CONFIG,                // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(KSAUDIO_CHANNEL_CONFIG),                 // cbMinGetDataInput
        PcValidateAudioChannelConfig                    // pfnSetHandler
    ),
    //  All values are valid
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_CHORUS_LEVEL,                  // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(LONG),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Structure is two BOOLs- no validation seems reasonable
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_COPY_PROTECTION,               // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSAUDIO_COPY_PROTECTION),                // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Can't be set, so no further validation needed
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_CPU_RESOURCES,                 // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Uses stock KSTIME validation
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_DELAY,                         // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(KSTIME),                                 // cbMinGetDataInput
        PcValidateKsTime                                // pfnSetHandler
    ),
    //  Parameter is a Pin ID- validation deferred to the driver.
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_DEMUX_DEST,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  All values are valid
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_DYNAMIC_RANGE,                 // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSAUDIO_DYNAMIC_RANGE),                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_DYNAMIC_SAMPLING_RATE,         // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Note- this must be further validated by driver, because we don't know
    //  the number of bands in this layer
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_EQ_BANDS,                      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Note- this must be further validated by driver, because we don't know
    //  the number of bands in this layer
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_EQ_LEVEL,                      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Use KSTime validation
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_LATENCY,                       // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSTIME),                                 // cbMinGetDataInput
        PcValidateKsTime                                // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_LOUDNESS,                      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  All values valid
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_MID,                           // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(LONG),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_MIX_LEVEL_CAPS,                // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        2 * sizeof(ULONG),                              // cbMinGetDataInput
        PcValidateAudioMixLevelCaps                     // pfnSetHandler
    ),
    //  Cannot further validate size without calling KSPROPERTY_AUDIO_MIXELEVEL_CAPS
    //  Ain't gonna happen- let the driver handle it.
    //  Individual values are always valid
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_MIX_LEVEL_TABLE,               // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(KSAUDIO_MIXLEVEL),                       // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_MUTE,                          // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Can't validate value without knowing topology
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_MUX_SOURCE,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Can't be set, can't be further validated
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_NUM_EQ_BANDS,                  // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read-only property, no need to further validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_PEAKMETER,                     // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY_AUDIO_PEAKMETER),             // cbMinGetPropertyInput
        sizeof(LONG),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read-only property, no need to further validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_PEAKMETER2,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY_AUDIO_PEAKMETER),             // cbMinGetPropertyInput
        sizeof(LONG),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Any further validation needed already provided by the ports
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_POSITION,                      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSAUDIO_POSITION),                       // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_PREFERRED_STATUS,              // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_QUALITY,                       // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        PcValidateAudioQuality                         // pfnSetHandler
    ),
    //  All values are valid
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_REVERB_LEVEL,                  // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  I suppose we could look for outlandish values, but what driver is going 
    //  to accept harmful data in this property?  Validation lacks definition,
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_SAMPLING_RATE,                 // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_STEREO_SPEAKER_GEOMETRY,       // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(LONG),                                   // cbMinGetDataInput
        PcValidateAudioStereoSpeakerGeometry            // pfnSetHandler
    ),
    //  Unlikely this is used, but just in case
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_SURROUND_ENCODE,               // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  All values are valid
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_TREBLE,                        // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  All values are valid
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_VOLUMELEVEL,                   // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY_AUDIO_CHANNEL),           // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  All values are valid
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_WIDENESS,                      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

//  KSPropSetID_Connection properties
//  The remaining properties in this set are not listed because I can't imagine
//  why any miniport would want to expose them or use them.

DEFINE_PCVAPROPERTY_TABLE(PinConnectionValidationHandlers) 
{
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_CONNECTION_STATE,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        PcValidateConnectionState                       // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_CONNECTION_DATAFORMAT,               // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSDATAFORMAT_WAVEFORMATEX),              // cbMinGetDataInput
        PcValidateDataFormat                            // pfnSetHandler
    )
};

//  All defined KSPropSetID_Topology- all are get-only, so only size validation is ever needed

DEFINE_PCVAPROPERTY_TABLE(TopologyValidationHandlers) 
{
    //  Presumably at least one GUID- driver must validate further
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_TOPOLOGY_CATEGORIES,                 // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSMULTIPLE_ITEM) + sizeof(GUID),         // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Presumably at least one connection- driver must validate further
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_TOPOLOGY_CONNECTIONS,                        // idProperty
        NULL,                                                   // pfnGetHandler
        sizeof(KSPROPERTY),                                     // cbMinGetPropertyInput
        sizeof(KSMULTIPLE_ITEM) + sizeof(KSTOPOLOGY_CONNECTION),// cbMinGetDataInput
        NULL                                                    // pfnSetHandler
    ),
    //  Even null has one character- driver must validate further
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_TOPOLOGY_NAME,                       // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_NODE),                               // cbMinGetPropertyInput
        sizeof(WCHAR),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Presumably at least one connection- driver must validate further
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_TOPOLOGY_NODES,                      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSMULTIPLE_ITEM) + sizeof(GUID),         // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

//  All defined KSPropSetID_TopologyNode

DEFINE_PCVAPROPERTY_TABLE(TopologyNodeValidationHandlers) 
{
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_TOPOLOGYNODE_ENABLE,                 // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_TOPOLOGYNODE_RESET,                  // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

//  All defined KSPropSetID_Pin

DEFINE_PCVAPROPERTY_TABLE(PinValidationHandlers) 
{
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_CATEGORY,                        // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(GUID),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_CINSTANCES,                     // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(KSPIN_CINSTANCES),                       // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_COMMUNICATION,                   // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(KSPIN_COMMUNICATION),                    // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only- driver must further validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_CONSTRAINEDDATARANGES,           // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(KSMULTIPLE_ITEM) + sizeof(KSDATARANGE),  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_CTYPES,                          // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_DATAFLOW,                        // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(KSPIN_DATAFLOW),                         // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_DATAINTERSECTION,                // idProperty
        PcValidateDataIntersection,                     // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(KSMULTIPLE_ITEM) + sizeof(KSDATARANGE),  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_DATARANGES,                      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(KSMULTIPLE_ITEM) + sizeof(KSDATARANGE),  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_GLOBALCINSTANCES,                // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(KSPIN_CINSTANCES),                       // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_INTERFACES,                          // idProperty
        NULL,                                               // pfnGetHandler
        sizeof(KSP_PIN),                                    // cbMinGetPropertyInput
        sizeof(KSMULTIPLE_ITEM) + sizeof(KSPIN_INTERFACE),  // cbMinGetDataInput
        NULL                                                // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_MEDIUMS,                         // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(KSMULTIPLE_ITEM) + sizeof(KSPIN_MEDIUM), // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_NAME,                            // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(WCHAR),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_NECESSARYINSTANCES,              // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_PHYSICALCONNECTION,              // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(KSMULTIPLE_ITEM) + sizeof(KSDATARANGE),  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  We can actually validate this!
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_PIN_PROPOSEDATAFORMAT,               // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        0,                                              // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  All values are valid
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_AUDIO_VOLUMELIMIT_ENGAGED,           // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

//  KSPROPSETID_Stream handlers- I'm assuming these can be called from user
//  mode (if incorrect, we can enforce this).

DEFINE_PCVAPROPERTY_TABLE(StreamValidationHandlers)
{
    //  Underlying driver must validate values- it defines them
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_STREAM_ALLOCATOR,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(HANDLE),                                 // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  I assume the handle cannot be validated (private to driver)
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_STREAM_QUALITY,                      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSQUALITY_MANAGER),                      // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Set can definitely be validated
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_STREAM_DEGRADATION,                  // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSMULTIPLE_ITEM) + sizeof(KSDEGRADE),    // cbMinGetDataInput
        PcValidateSetStreamDegradation                  // pfnSetHandler
    ),
    //  Assumption is handle is private and cannot be validated here.
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_STREAM_MASTERCLOCK,                  // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(HANDLE),                                 // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  No validation needed
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_STREAM_TIMEFORMAT,                   // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(GUID),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Set times can be partially validated
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_STREAM_PRESENTATIONTIME,             // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSTIME),                                 // cbMinGetDataInput
        PcValidateKsTime                                // pfnSetHandler
    ),
    //  Assumed read-only- cannot be validated, and probably not used.
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_STREAM_PRESENTATIONEXTENT,           // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(LONGLONG),                               // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read-only, nothing more to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_STREAM_FRAMETIME,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSFRAMETIME),                            // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Read-only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_STREAM_RATECAPABILITY,               // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSRATE_CAPABILITY),                      // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Highly unlikely this gets used, but the flags can be validated
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_STREAM_RATE,                         // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSRATE),                                 // cbMinGetDataInput
        PcValidateKsRate                                // pfnSetHandler
    )
};

//  All defined KSPropSetID_DirectSound3DBuffer properties
//  This provides at least NULL / small buffer protection on all defined 
//  properties in this set.  Noted properties have additional validation
//  Validation of floating point values is not performed (no bullet-proof
//  means of doing so was found).

DEFINE_PCVAPROPERTY_TABLE(DS3DBufferValidationHandlers) 
{
    //  Some parameter validation has been added
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DBUFFER_ALL,             // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(KSDS3D_BUFFER_ALL),                      // cbMinGetDataInput
        PcValidateDS3DBufferAll                         // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DBUFFER_CONEANGLES,      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(KSDS3D_BUFFER_CONE_ANGLES),              // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DBUFFER_CONEORIENTATION, // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(DS3DVECTOR),                             // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DBUFFER_CONEOUTSIDEVOLUME,   // idProperty
        NULL,                                               // pfnGetHandler
        sizeof(KSNODEPROPERTY),                             // cbMinGetPropertyInput
        sizeof(LONG),                                       // cbMinGetDataInput
        NULL                                                // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DBUFFER_MAXDISTANCE,     // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(FLOAT),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DBUFFER_MINDISTANCE,     // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(FLOAT),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Enumerated, so we can validate sets
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DBUFFER_MODE,            // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        PcValidateDS3DBufferMode                        // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DBUFFER_POSITION,        // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(DS3DVECTOR),                             // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DBUFFER_VELOCITY,        // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(DS3DVECTOR),                             // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

//  All defined KSPropSetID_DirectSound3DListener properties
//  This provides NULL / small buffer protection on all defined properties in 
//  this set

DEFINE_PCVAPROPERTY_TABLE(DS3DListenerValidationHandlers) 
{
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DLISTENER_ALL,           // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(KSDS3D_LISTENER_ALL),                    // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DLISTENER_ALLOCATION,    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DLISTENER_BATCH,         // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(BOOL),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DLISTENER_DISTANCEFACTOR,// idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(FLOAT),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DLISTENER_DOPPLERFACTOR, // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(FLOAT),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DLISTENER_ORIENTATION,   // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(KSDS3D_LISTENER_ORIENTATION),            // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DLISTENER_POSITION,      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(DS3DVECTOR),                             // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DLISTENER_ROLLOFFFACTOR, // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(FLOAT),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DIRECTSOUND3DLISTENER_VELOCITY,      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(DS3DVECTOR),                             // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

//  KSPropSetID_Synth properties

DEFINE_PCVAPROPERTY_TABLE(SynthValidationHandlers) 
{
    //  No set, hence no validation
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_CAPS,                          // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(SYNTHCAPS),                              // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Presumably 0 is invalid- no upper limit is mentioned
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_CHANNELGROUPS,                 // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSNODEPROPERTY),                         // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        PcValidateSynthChannelGroups                    // pfnSetHandler
    ),
    //  No set, hence no validation
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_LATENCYCLOCK,                  // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(ULONGLONG),                              // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  No set, hence no validation
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_PORTPARAMETERS,                // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(SYNTH_PORTPARAMS),                       // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  No set, hence no validation
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_RUNNINGSTATS,                  // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(SYNTH_STATS),                            // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Not validated beyond sizes- all values appear to be potentially valid
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_VOICEPRIORITY,                 // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(SYNTHVOICEPRIORITY_INSTANCE),            // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  All values are valid- This is assumed to be a filter property
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_VOLUME,                        // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(LONG),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  All values are valid
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_VOLUMEBOOST,                   // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(LONG),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

//  PropSetID_Synth_DLS handlers

DEFINE_PCVAPROPERTY_TABLE(SynthDLSValidationHandlers)
{
    //  Get only- no additional validation
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_DLS_APPEND,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(ULONG),                                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  No data, so no additional validation needed
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_DLS_COMPACT,                   // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        0,                                              // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  The driver HAS TO validate the input here, so we won't
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_DLS_DOWNLOAD,                  // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(SYNTH_BUFFER),                           // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  The driver must validate this, as it defines valid input itself
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_DLS_UNLOAD,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(HANDLE),                                 // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

//  KSPropSetID_SynthClock

DEFINE_PCVAPROPERTY_TABLE(SynthClockValidationHandlers)
{
    //  Get only, so trivial
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_SYNTH_MASTERCLOCK,                   // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(ULONGLONG),                              // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

//  KSPropSetID_DrmAudioStream

DEFINE_PCVAPROPERTY_TABLE(DrmAudioStreamValidationHandlers)
{
    //  Validate Kernel mode caller 
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_DRMAUDIOSTREAM_CONTENTID,            // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_DRMAUDIOSTREAM_CONTENTID),           // cbMinGetPropertyInput
        sizeof(ULONG) + sizeof(DRMRIGHTS),              // cbMinGetDataInput
        PcValidateKernelMode                            // pfnSetHandler
    )
};

//  KSPropSetID_General

DEFINE_PCVAPROPERTY_TABLE(GeneralValidationHandlers)
{
    //  Not settable, so no input to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_GENERAL_COMPONENTID,                 // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSCOMPONENTID),                          // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

//  KSPropSetID_Clock

DEFINE_PCVAPROPERTY_TABLE(ClockValidationHandlers)
{
    //  Not settable, so no input to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_CLOCK_CORRELATEDPHYSICALTIME,        // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSCORRELATED_TIME),                      // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Not settable, so no input to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_CLOCK_CORRELATEDTIME,                // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSCORRELATED_TIME),                      // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Validate for kernel mode only
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_CLOCK_FUNCTIONTABLE,                 // idProperty
        PcValidateKernelMode,                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSCLOCK_FUNCTIONTABLE),                  // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Not settable, so no input to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_CLOCK_PHYSICALTIME,                  // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(LONGLONG),                               // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Not settable, so no input to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_CLOCK_RESOLUTION,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(KSRESOLUTION),                           // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Not settable, so no input to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_CLOCK_TIME,                          // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(LONGLONG),                               // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

//  KSPROPSETID_RtAudio

DEFINE_PCVAPROPERTY_TABLE(RtAudioValidationHandlers)
{
    //  Not settable, so no input to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_RTAUDIO_GETPOSITIONFUNCTION,         // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSPROPERTY),                             // cbMinGetPropertyInput
        sizeof(PRTAUDIOGETPOSITION),                    // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

// KSPROPSETID_Jack
DEFINE_PCVAPROPERTY_TABLE(JackValidationHandlers)
{
    //  Not settable, so no input to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_JACK_DESCRIPTION,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        0,                                              // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Not settable, so no input to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_JACK_DESCRIPTION2,                   // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        0,                                              // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Not settable, so no input to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_JACK_SINK_INFO,                      // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        0,                                              // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    ),
    //  Not settable, so no input to validate
    DEFINE_PCVAPROPERTY_ITEM(
        KSPROPERTY_JACK_CONTAINERID,                    // idProperty
        NULL,                                           // pfnGetHandler
        sizeof(KSP_PIN),                                // cbMinGetPropertyInput
        sizeof(GUID),                                   // cbMinGetDataInput
        NULL                                            // pfnSetHandler
    )
};

DEFINE_PCVAPROPERTY_SET_TABLE(ValidationPropertySet)
{
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_Audio,                              // Set
       SIZEOF_ARRAY(AudioPropertyValidationHandlers),   // PropertiesCount
       AudioPropertyValidationHandlers                  // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_Connection,                         // Set
       SIZEOF_ARRAY(PinConnectionValidationHandlers),   // PropertiesCount
       PinConnectionValidationHandlers                  // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_Synth,                              // Set
       SIZEOF_ARRAY(SynthValidationHandlers),           // PropertiesCount
       SynthValidationHandlers                          // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_TopologyNode,                       // Set
       SIZEOF_ARRAY(TopologyNodeValidationHandlers),    // PropertiesCount
       TopologyNodeValidationHandlers                   // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_DirectSound3DBuffer,                // Set
       SIZEOF_ARRAY(DS3DBufferValidationHandlers),      // PropertiesCount
       DS3DBufferValidationHandlers                     // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_DirectSound3DListener,              // Set
       SIZEOF_ARRAY(DS3DListenerValidationHandlers),    // PropertiesCount
       DS3DListenerValidationHandlers                   // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_RtAudio,                            // Set
       SIZEOF_ARRAY(RtAudioValidationHandlers),         // PropertiesCount
       RtAudioValidationHandlers                        // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_Synth_Dls,                          // Set
       SIZEOF_ARRAY(SynthDLSValidationHandlers),        // PropertiesCount
       SynthDLSValidationHandlers                       // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_SynthClock,                         // Set
       SIZEOF_ARRAY(SynthClockValidationHandlers),      // PropertiesCount
       SynthClockValidationHandlers                     // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_DrmAudioStream,                     // Set
       SIZEOF_ARRAY(DrmAudioStreamValidationHandlers),  // PropertiesCount
       DrmAudioStreamValidationHandlers                 // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_Clock,                              // Set
       SIZEOF_ARRAY(ClockValidationHandlers),           // PropertiesCount
       ClockValidationHandlers                          // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_Topology,                           // Set
       SIZEOF_ARRAY(TopologyValidationHandlers),        // PropertiesCount
       TopologyValidationHandlers                       // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_Pin,                                // Set 
       SIZEOF_ARRAY(PinValidationHandlers),             // PropertiesCount
       PinValidationHandlers                            // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_Stream,                             // Set 
       SIZEOF_ARRAY(StreamValidationHandlers),          // PropertiesCount
       StreamValidationHandlers                         // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_General,                            // Set
       SIZEOF_ARRAY(GeneralValidationHandlers),         // PropertiesCount
       GeneralValidationHandlers                        // PropertyItem
    ),
    DEFINE_PCVAPROPERTY_SET(
       &KSPROPSETID_Jack,                               // Set
       SIZEOF_ARRAY(JackValidationHandlers),            // PropertiesCount
       JackValidationHandlers                           // PropertyItem
    )
};

//===========================================================================
//
// Returns the WAVEFORMATEX structure appended to KSDATAFORMAT.
// Assumptions:
//     - pDataFormat is totally trusted. It has been probed and buffered
//     properly.
//     - This function should only be called if MajorFormat is AUDIO.
//

PWAVEFORMATEX 
GetWaveFormatExFromKsDataFormat(
    PKSDATAFORMAT pDataFormat,
    PULONG pcbFormat
)
{
    ASSERT(pDataFormat);

    if (NULL == pDataFormat)
        return NULL;

    PWAVEFORMATEX pWaveFormat = NULL;

    if(IsEqualGUID(pDataFormat->Specifier, KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)) {
        pWaveFormat = 
          &PKSDATAFORMAT_WAVEFORMATEX(pDataFormat)->WaveFormatEx;

        if (pcbFormat) {
            *pcbFormat = sizeof(KSDATAFORMAT_WAVEFORMATEX);
        }
    }
    else if(IsEqualGUID(pDataFormat->Specifier, KSDATAFORMAT_SPECIFIER_DSOUND)) {
        pWaveFormat = 
          &PKSDATAFORMAT_DSOUND(pDataFormat)->BufferDesc.WaveFormatEx;

        if (pcbFormat) {
            *pcbFormat = sizeof(KSDATAFORMAT_DSOUND);
        }
    }

    return pWaveFormat;
} // GetWaveFormatExFromKsDataFormat

//===========================================================================
//
// Validates the integrity of KSDATAFORMAT structure for AUDIO.
// Assumptions:
//     - pDataFormat is totally trusted. It has been probed and buffered
//     properly.
//     - This function should only be called if MajorFormat is AUDIO.
//
NTSTATUS
ValidateAudioDataFormats(
    PKSDATAFORMAT pDataFormat
)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;
    ULONG cbAudioFormat;
    PWAVEFORMATEX pWaveFormatEx;

    if (NULL == pDataFormat)
        return STATUS_INVALID_PARAMETER;

    ASSERT(pDataFormat);
    ASSERT(IsEqualGUID(pDataFormat->MajorFormat, KSDATAFORMAT_TYPE_AUDIO));

    //
    // We only support two specifiers in audio land. All the rest will be 
    // accepted without further checks, because we don't know how to validate
    // them.
    //
    pWaveFormatEx = GetWaveFormatExFromKsDataFormat(pDataFormat, &cbAudioFormat);
    if (NULL == pWaveFormatEx) {
        ntStatus = STATUS_SUCCESS;
        goto exit;
    }

    //
    // Make sure that we have enough space for the actual format packet.
    // Note that this will make sure we can at least touch the WAVEFORMATEX 
    // part.
    //
    if (pDataFormat->FormatSize < cbAudioFormat)
    {
        ntStatus = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    //
    // Check to see if WAVEFORMATEXTENSIBLE size is specified correctly.
    //
    if ((WAVE_FORMAT_EXTENSIBLE == pWaveFormatEx->wFormatTag) &&
        (pWaveFormatEx->cbSize < sizeof(WAVEFORMATEXTENSIBLE) - sizeof(WAVEFORMATEX))) 
    {
        ntStatus = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    //
    // Now that WaveFormatEx is guaranteed to be safe, check if we have extended
    // information in WAVEFORMATEX.
    // cbSize specifies the size of the extension structure. 
    // Validate that FormatSize accomodates cbSize
    // Validate that cbSize does not cause an overflow.
    //
    if (pDataFormat->FormatSize < cbAudioFormat + pWaveFormatEx->cbSize ||
        cbAudioFormat + pWaveFormatEx->cbSize < cbAudioFormat)
    {
        ntStatus = STATUS_INVALID_PARAMETER;
        goto exit;
    }

    //
    // Now we validated that the data buffer passed to us actually matches
    // with its specifier.
    //
    
exit:        
    return ntStatus;
} // ValidateAudioDataFormats

//===========================================================================
//
// Validates the integrity of KSDATAFORMAT structure. 
// Calls ValidateAudioDataFormat if MajorFormat is audio.
// Or checks the buffers size if Specifier is NONE.
// Assumptions:
//     - pDataFormat is totally trusted. It has been probed and buffered
//     properly.
//
NTSTATUS
ValidateDataFormat(
    PKSDATAFORMAT pDataFormat
)
{
    NTSTATUS ntStatus = STATUS_SUCCESS;

    ASSERT(pDataFormat);

    if (NULL == pDataFormat)
        return STATUS_INVALID_PARAMETER;

    if (IsEqualGUID(pDataFormat->MajorFormat, KSDATAFORMAT_TYPE_AUDIO))
    {
        ntStatus = ValidateAudioDataFormats(pDataFormat);
    }

    return ntStatus;
} // ValidateDataFormat

//===========================================================================
// 
// AddValidation
//
// This will modify a property item built by PcAddToPropertyTable so that it
// has the correct property and value minimum sizes, and so that it will pass
// through any common validation code we can define here before being 
// forwarded to PropertyItemPropertyHandler.
//

void 
AddValidation(
    const GUID&         DesiredSet,
    PKSPROPERTY_ITEM    pItem
)
{
    //  First see if we have any validation for any properties in the set
    for (unsigned Set = 0; Set < SIZEOF_ARRAY(ValidationPropertySet); Set++)
    {
        if  (IsEqualGUIDAligned(*ValidationPropertySet[Set].Set, DesiredSet))
        {
            const PCVAPROPERTY_SET& TargetSet = ValidationPropertySet[Set];

            //  OK, now see if this property is one we can validate
            for (unsigned Property = 0; Property < TargetSet.PropertiesCount; Property++)
            {
                if  (TargetSet.PropertyItem[Property].PropertyId == pItem->PropertyId)
                {
                    //  OK, this is the one!
                    const PCVAPROPERTY_ITEM& TargetItem = TargetSet.PropertyItem[Property];

                    pItem->MinData = TargetItem.MinData;
                    pItem->MinProperty = TargetItem.MinProperty;
                    //  Add any validation we have- typically only sets have input data
                    //  But we can screen some gets (eg for kernel mode only requestors)
                    if  (TargetItem.GetPropertyHandler) 
                        pItem->GetPropertyHandler = TargetItem.GetPropertyHandler;
                    if  (TargetItem.SetPropertyHandler) 
                        pItem->SetPropertyHandler = TargetItem.SetPropertyHandler;
                    return;
                }
            }
        }
    }
}   //  AddValidation

//  Property validation routines- these will all forward to 
//  PropertyItemPropertyHandler if the parameter validation succeeds

//===========================================================================
//
// PcValidateConnectionState
// Check that the KSSTATE is valid.
//
NTSTATUS 
PcValidateConnectionState(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    IN PKSSTATE pState
)
{
    ASSERT(pState);
    
    if (KSSTATE_STOP <= *pState &&
        KSSTATE_RUN >= *pState)
    {
        return PropertyItemPropertyHandler(pIrp, pProperty, pState);
    }

    return STATUS_INVALID_PARAMETER;
} // PcValidateConnectionState

//===========================================================================
//
// PcValidateDataFormat
// Checks whether the given format is valid.
//
NTSTATUS
PcValidateDataFormat(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    PKSDATAFORMAT pDataFormat
)
{
    ASSERT(pDataFormat);

    if (NULL == pDataFormat)
        return STATUS_INVALID_PARAMETER;

    NTSTATUS    Status = ValidateDataFormat(pDataFormat);

    if  (NT_SUCCESS(Status))
        Status = PropertyItemPropertyHandler(pIrp, pProperty, pDataFormat);

    return  Status;
} // PcValidateDataFormat

//===========================================================================
//
// PcValidateAudioQuality
// Checks if the quality is valid.
//
NTSTATUS
PcValidateAudioQuality(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    IN PLONG pQuality
)
{
    ASSERT(pQuality);

    if (KSAUDIO_QUALITY_WORST <= *pQuality &&
        KSAUDIO_QUALITY_ADVANCED >= *pQuality)
    {
        return PropertyItemPropertyHandler(pIrp, pProperty, pQuality);
    }

    return STATUS_INVALID_PARAMETER;
} // PcValidateAudioQuality

//===========================================================================
//
// PcValidateAudioMixLevelCaps
// Checks if the structure is valid.
//
NTSTATUS
PcValidateAudioMixLevelCaps(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    IN OUT PVOID pVoid
)
{
    ASSERT(pVoid);
    ASSERT(pIrp->AssociatedIrp.SystemBuffer);

    PKSAUDIO_MIXCAP_TABLE pMixTable;
    PIO_STACK_LOCATION pIrpStack;
    ULONG ulTotalChannels;
    ULONG cbRequiredSize;
    
    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    pMixTable = (PKSAUDIO_MIXCAP_TABLE) pVoid;

    if (pMixTable->InputChannels > 10000 ||
        pMixTable->OutputChannels > 10000)
    {
        return STATUS_INVALID_PARAMETER;
    }

    ulTotalChannels = pMixTable->InputChannels + pMixTable->OutputChannels;
    if (ulTotalChannels)
    {
        cbRequiredSize = 
            sizeof(KSAUDIO_MIXCAP_TABLE) + ulTotalChannels * sizeof(KSAUDIO_MIX_CAPS);
        if (cbRequiredSize < 
            pIrpStack->Parameters.DeviceIoControl.InputBufferLength)
        {
            return STATUS_BUFFER_TOO_SMALL;
        }
    }
    
    return PropertyItemPropertyHandler(pIrp, pProperty, pVoid);
} // PcValidateAudioMixLevelCaps

//===========================================================================
//
// PcValidateDataIntersection
// Checks the integrity of dataranges following pPin.
//
NTSTATUS 
PcValidateDataIntersection(
    IN PIRP pIrp,
    IN PKSP_PIN pPin,
    PVOID pData
)
{
    PIO_STACK_LOCATION pIrpStack;
    PKSMULTIPLE_ITEM pKsMultipleItem;
    PKSDATARANGE pKsDataRange;
    ULONG cbTotal;

    ASSERT(pIrp);
    ASSERT(pPin);

    pIrpStack = IoGetCurrentIrpStackLocation(pIrp);
    pKsMultipleItem = (PKSMULTIPLE_ITEM) (pPin + 1);
    pKsDataRange = (PKSDATARANGE) (pKsMultipleItem + 1);
    cbTotal = pKsMultipleItem->Size - sizeof(KSMULTIPLE_ITEM);

    //
    // Make sure that the Irp Input Size is valid. Basically 
    // InputBufferLength must be greater or equal to 
    // KSP_PIN + MULTIPLE_ITEM.Size
    //
    if (pIrpStack->Parameters.DeviceIoControl.InputBufferLength - sizeof(KSP_PIN) <
        pKsMultipleItem->Size)
    {
        return STATUS_INVALID_BUFFER_SIZE;
    }

    //
    // Make sure that the MULTIPLE_ITEM contains at least one DATARANGE.
    //
    if (cbTotal < sizeof(KSDATARANGE))
    {
        return STATUS_INVALID_BUFFER_SIZE;
    }

    for (ULONG ii = 0; ii < pKsMultipleItem->Count; ii++)
    {
        //
        // Check if we can touch the FormatSize field.
        // Check if we the next data-range is fully available.
        //
        if (cbTotal < sizeof(ULONG) ||
            cbTotal < pKsDataRange->FormatSize || 
            pKsDataRange->FormatSize < sizeof(KSDATARANGE))
        {
            return STATUS_INVALID_BUFFER_SIZE;
        }

        //
        // Check if the MajorFormat and size are consistent.
        // 
        if (IsEqualGUID(pKsDataRange->MajorFormat, KSDATAFORMAT_TYPE_AUDIO))
        {
            if (pKsDataRange->FormatSize < sizeof(KSDATARANGE_AUDIO)) 
            {
                return STATUS_INVALID_BUFFER_SIZE;
            }
        }

        //
        // Set next data range.
        //
        cbTotal -= pKsDataRange->FormatSize;
        pKsDataRange = (PKSDATARANGE) ( ((PBYTE) pKsDataRange) + pKsDataRange->FormatSize );
    }

    //
    // SECURITY NOTE:
    // We are not checking the output buffer integrity. The underlying drivers
    // are responsible for checking the size of the output buffer, based on
    // the result of the intersection.
    //

    return PropertyItemPropertyHandler(pIrp, (PKSIDENTIFIER) pPin, pData);
} // PcValidateDataIntersection





#if !defined(DS3DMODE_DISABLE)
#define DS3DMODE_DISABLE    2
#endif

NTSTATUS
PcValidateDS3DBufferAll(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    KSDS3D_BUFFER_ALL& BufferSpec
)
{
    if  (BufferSpec.Mode > DS3DMODE_DISABLE)
        return  STATUS_INVALID_PARAMETER;

    return  PropertyItemPropertyHandler(pIrp, pProperty, &BufferSpec);
}

NTSTATUS
PcValidateDS3DBufferMode(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    ULONG& Mode
)
{
    if  (Mode > DS3DMODE_DISABLE)
        return  STATUS_INVALID_PARAMETER;

    return  PropertyItemPropertyHandler(pIrp, pProperty, &Mode);
}

NTSTATUS
PcValidateAudioChannelConfig(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    KSAUDIO_CHANNEL_CONFIG& Config
)
{
    if  (Config.ActiveSpeakerPositions & SPEAKER_RESERVED)
        return  STATUS_INVALID_PARAMETER;

    return  PropertyItemPropertyHandler(pIrp, pProperty, &Config);
}

NTSTATUS
PcValidateAudioStereoSpeakerGeometry(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    LONG& Setting
)
{
    
    switch  (Setting)
    {
    case KSAUDIO_STEREO_SPEAKER_GEOMETRY_HEADPHONE:
    case KSAUDIO_STEREO_SPEAKER_GEOMETRY_MIN:
    case KSAUDIO_STEREO_SPEAKER_GEOMETRY_MAX:
    case KSAUDIO_STEREO_SPEAKER_GEOMETRY_NARROW:
    case KSAUDIO_STEREO_SPEAKER_GEOMETRY_WIDE:
        break;;
    default:
        return  STATUS_INVALID_PARAMETER;
    }

    return  PropertyItemPropertyHandler(pIrp, pProperty, &Setting);
}

NTSTATUS
PcValidateKsTime(
    PIRP pIrp,
    PKSPROPERTY pProperty,
    KSTIME& Time
)
{
    if  (!Time.Denominator || !Time.Numerator)
        return  STATUS_INVALID_PARAMETER;

    return  PropertyItemPropertyHandler(pIrp, pProperty, &Time);
}

NTSTATUS
PcValidateSynthChannelGroups(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    ULONG& GroupCount
)
{
    if  (0 == GroupCount)   //  I suppose we could pick some huge upper limit- none defined
        return  STATUS_INVALID_PARAMETER;

    return PropertyItemPropertyHandler(pIrp, pProperty, &GroupCount);
}

NTSTATUS
PcValidateKernelMode(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    PVOID pRights
)
{
    if  (pIrp->RequestorMode != KernelMode)
        return  STATUS_INVALID_DEVICE_REQUEST;

    return PropertyItemPropertyHandler(pIrp, pProperty, pRights);
}

NTSTATUS
PcValidateSetStreamDegradation(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    KSMULTIPLE_ITEM& ItemsSpec
)
{
    //  Ths list has to be at list as big as the items it holds
    if  (ItemsSpec.Size < ItemsSpec.Count * sizeof(KSDEGRADE) + sizeof ItemsSpec)
        return  STATUS_INVALID_PARAMETER;

    //  Now validate that size against the listed input size (we only validate 
    //  one item's worth, initially, as it is the definite minimum)

    IO_STACK_LOCATION&  Current = *IoGetCurrentIrpStackLocation(pIrp);

    if  (Current.Parameters.DeviceIoControl.OutputBufferLength < ItemsSpec.Size)
        return  STATUS_INVALID_BUFFER_SIZE;

    //  The items themselves are identifiers known only to the driver (and QM),
    //  so the rest is up to the driver

    return PropertyItemPropertyHandler(pIrp, pProperty, &ItemsSpec);
}


NTSTATUS
PcValidateKsRate(
    IN PIRP pIrp,
    IN PKSPROPERTY pProperty,
    KSRATE& ksRate
)
{
    //  Just check for invalid bits in the flags parameter
    if  (ksRate.Flags & ~(KSRATE_NOPRESENTATIONSTART | KSRATE_NOPRESENTATIONDURATION)) 
        return  STATUS_INVALID_PARAMETER;

    return PropertyItemPropertyHandler(pIrp, pProperty, &ksRate);
}

