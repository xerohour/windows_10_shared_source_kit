/*++

Copyright (c) Microsoft.  All rights reserved.

Module Name:

    verifier.h

Abstract:

    KS Driver Verifier Extension.

--*/

#include "xdvexport.h"
#include "xdvcheck\xdvcheck.h"

#if !defined( _VERIFIER_ )
#define _VERIFIER_

extern ULONG PcVerifierLevel;
extern ULONG PcXdvExtLevel;

#define XDV_EXT_ON (PcVerifierLevel & 0x10000)
#define PC_DEFAULT_ON (PcXdvExtLevel & 0x40)
#define PC_SPECIAL_ON (PcXdvExtLevel & 0x80)
#define STATELESS_ON PC_DEFAULT_ON
#define PC_VERIFIER_ON (PC_DEFAULT_ON || PC_SPECIAL_ON)

#define PC_XDV_PAGE_SECTION_NAME "PAGEPcVf"
#define PC_XDV_NXPOOL_TAG 'AvdX'

#if defined(__cplusplus)
extern "C" {
#endif // defined(__cplusplus)


//
// Function pointer type for callback interceptions
//

typedef VOID VFFUNPTR (VOID);
typedef VFFUNPTR *PVFFUNPTR;


typedef struct _PC_SLIC_STATE_EX {
    
    SLIC_PC_STATE PcState;

    //
    // Device descriptor attached to KsDevice
    //

    const KSDEVICE_DESCRIPTOR* DevicePristineDescriptor;
    KSDEVICE_DESCRIPTOR DeviceVerifierDescriptor;
    KSDEVICE_DISPATCH DeviceVerifierWrapper;

} PC_SLIC_STATE_EX, *PPC_SLIC_STATE_EX;


extern PXDV_EXPORT_TABLE PcVerifierUtilTable;

#define XDV_TYPE_CAST(T,V) ((T)V)

#define XDV_ALLOCATE_CONTEXT(DeviceObject, DriverContext, VerifierContext) \
    (PcVerifierUtilTable->CreateState(PcGroup, sizeof(PC_SLIC_STATE_EX), DeviceObject, DriverContext, VerifierContext))

#define XDV_FREE_CONTEXT(VerifierContext) \
    (PcVerifierUtilTable->FreeState(PcGroup, VerifierContext))

#define XDV_STATEFUL_ABORT(ErrorString,ErrorCode,ContextObject,TrackedObject,TrackedObjectHead,StateSize) \
    (PcVerifierUtilTable->BugcheckStateful(ErrorString,ErrorCode,ContextObject,TrackedObject,TrackedObjectHead,StateSize))

#define XDV_STATELESS_ABORT(ErrorString,ErrorCode) \
    (PcVerifierUtilTable->BugcheckStateless(ErrorString,ErrorCode))

#define XDV_START_CRITICAL_SECTION(State,VerifierContext,Group) \
    (PcVerifierUtilTable->StartCriticalSection(VerifierContext))

#define XDV_END_CRITICAL_SECTION(State,VerifierContext,Group) \
    (PcVerifierUtilTable->EndCriticalSection(VerifierContext))

#define LoadObjectState(Size,VerifierContext,Group) \
    (PcVerifierUtilTable->LoadState(VerifierContext))

#define XDV_START_TIMED_EXTENSION(Callback,GetTimedLimitCallback) \
    (PcVerifierUtilTable->StartTimedExtension(PcGroup,10,Callback,GetTimedLimitCallback,DISPATCH_LEVEL))

#define XDV_STOP_TIMED_EXTENSION() \
    (PcVerifierUtilTable->StopTimedExtension(PcGroup))

#define XDV_CURRENT_TIME \
    (PcVerifierUtilTable->GetTimeTick(PcGroup))

#define XDV_CLEAN_STATE(TYPE,Head,WatchedState) \
    (PcVerifierUtilTable->FreeWatchedState( \
        (PXDV_WATCH_HEAD *)(WatchedState), \
        sizeof(TYPE), \
        (PXDV_WATCH_HEAD *)(Head)))

#define XDV_START_WATCH(TYPE, Guard, Key, WatchedState, WatchOnPointer) \
    (PcVerifierUtilTable->FindCreateWatchedState ( \
        (PXDV_WATCH_HEAD*)(&(Guard)), \
        sizeof(*(Guard)), \
        (PVOID)(Key), \
        (WatchOnPointer), \
        (WatchedState) \
        ))

#define XDV_START_TRACK(TYPE, Guard, Key, WatchedState, WatchOnPointer) \
    (PcVerifierUtilTable->CreateWatchedState ( \
        (PXDV_WATCH_HEAD*)(&(Guard)), \
        sizeof(*(Guard)), \
        (PVOID)(Key), \
        (WatchOnPointer), \
        (WatchedState) \
        ))

#define XDV_GET_WATCHED_STATE(TYPE, Guard, Key, WatchedState, WatchOnPointer) \
    (PcVerifierUtilTable->FindWatchedState ( \
        (PXDV_WATCH_HEAD)(Guard), \
        sizeof(*(Guard)), \
        (PVOID)(Key), \
        TRUE, \
        (WatchOnPointer), \
        (WatchedState) \
        ))

#define XDV_GET_NEXT_WATCHED_STATE(TYPE, Head, InWatchedState, OutWatchedState) \
    (PcVerifierUtilTable->GetNextWatchedState(\
        (PXDV_WATCH_HEAD)(Head), \
        (PXDV_WATCH_HEAD)(InWatchedState), \
        sizeof(TYPE), \
        (OutWatchedState) \
        ))

#define XDV_GET_FIRST_TRACKED_STATE(TYPE, Guard, WatchedState) \
    (PcVerifierUtilTable->GetFirstWatchedState ( \
        (PXDV_WATCH_HEAD)(Guard), \
        sizeof(*(Guard)), \
        (WatchedState) \
        ))

#define XDV_ALLOCATE_POOL(Size) \
    (PcVerifierUtilTable->AllocatePool ( \
        Size, \
        FALSE \
        ))

#define XDV_FREE_POOL(P,Size) \
    (PcVerifierUtilTable->FreePool ( \
        P, \
        Size, \
        FALSE \
        ))

#define XDV_READ_REGISTRY_KEY_DWORD(Name, Value) \
    (PcVerifierUtilTable->ReadRegistryKeyDword ( \
        (PUNICODE_STRING) (Name), \
        (PULONG) (Value) \
        ))

#define XDV_READ_EXTENSION_CONFIGURATION() \
    (PcVerifierUtilTable->ReadExtensionConfiguration(PcGroup))

#define XDV_CURRENT_IRQL (KeGetCurrentIrql())


FORCEINLINE
BOOLEAN 
IsVerifierEnabled(
    PVOID VerifierContext
    )
{
    return (PC_VERIFIER_ON && VerifierContext != NULL);
}


BOOLEAN
PcVerifierInitialize(
    VOID
    );


BOOLEAN
PcVerifierUninitialize (
    VOID
    );

#define PcVerifierGetContextFromDeviceObject PcVerifierGetContext

PVOID
PcVerifierGetContext(
    __in PDEVICE_OBJECT  DeviceObject
    );

#ifdef __cplusplus
}
#endif


#include "stdunk.h"

//
// Interception for COM interfaces
//


/*****************************************************************************
 * CUnknown
 *****************************************************************************
 * Base INonDelegatingUnknown implementation.
 */
class CXdvUnknown : public CUnknown
{
protected:
    PUNKNOWN m_pPristineUnknown;
    REFIID m_rPristineIID;
    volatile PVOID m_pInterface;

public:

    /*************************************************************************
     * CXdvUnknown methods.
     */
    CXdvUnknown(PUNKNOWN pUnknownOuter, REFIID rIID) :
        CUnknown(NULL), m_rPristineIID(rIID)
    {
        NT_ASSERT(pUnknownOuter != NULL);

        CUnknown::NonDelegatingAddRef();
        m_pPristineUnknown = pUnknownOuter;
        m_pInterface = NULL;
    }
    
    virtual ~CXdvUnknown(void) { }

    /*************************************************************************
     * INonDelegatingUnknown methods.
     */
    STDMETHODIMP_(ULONG) NonDelegatingAddRef
    (
        void
    );
    
    STDMETHODIMP_(ULONG) NonDelegatingRelease
    (
        void
    );

    STDMETHODIMP_(NTSTATUS) NonDelegatingQueryInterface
    (
        _In_            REFIID      rIID,
        _COM_Outptr_    PVOID *     ppVoid
    );
    
private:
    /*************************************************************************
     * The assignment operator is not supported.
     */
    CXdvUnknown &operator=(CXdvUnknown &);
};


#define DECLARE_XDV_CLASS_BEGIN(Name)                                         \
    class CXdv##Name : public C##Name                                         \
    {                                                                         \
    public:                                                                   \
        PVOID m_VerifierContext;                                                \
                                                                              \
    public:                                                                   \
        CXdv##Name(PUNKNOWN pUnknownOuter) : C##Name(pUnknownOuter)           \
        {                                                                     \
            m_VerifierContext = NULL;                                           \
        }                                                                     \
        virtual ~CXdv##Name() { }                                             \


#define DECLARE_XDV_CLASS_END(Name)                                           \
    };


#define DECLARE_XDV_PORT_BEGIN(Name)                                          \
        DECLARE_XDV_CLASS_BEGIN(Name)                                         \
        IMP_IPort;


#define DECLARE_XDV_PORT_END(Name)                                            \
    };


#define DECLARE_XDV_MINIPORT_BEGIN(Name,Interface)                            \
    class CXdv##Name : public Interface, public CXdvUnknown                   \
    {                                                                         \
    public:                                                                   \
        PVOID m_VerifierContext;                                              \
                                                                              \
    public:                                                                   \
        CXdv##Name(PUNKNOWN pUnknownOuter, PVOID VerifierContext)             \
        : CXdvUnknown(pUnknownOuter, IID_##Interface)                         \
        {                                                                     \
            this->m_VerifierContext = VerifierContext;                        \
        }                                                                     \
        virtual ~CXdv##Name() { }                                             \
        DECLARE_STD_UNKNOWN();                                                \
                                                                              \
    private:                                                                  \
        CXdv##Name &operator=(CXdv##Name &);



#define DECLARE_XDV_MINIPORT_END(Name)                                        \
    };


//
// Declare COM interface interceptions
//

DECLARE_XDV_MINIPORT_BEGIN(MiniportWaveRTStreamNotification,IMiniportWaveRTStreamNotification)
IMP_IMiniportWaveRTStream;
IMP_IMiniportWaveRTStreamNotification;
DECLARE_XDV_MINIPORT_END(MiniportWaveRTStreamNotification)


#define XdvPcPnpRemoveDevice(DeviceObject) XdvEventPcPnpRemoveDevice_wrapper(DeviceObject)
// #define XdvPcPnpRemoveDevice(DeviceObject) ((PVOID)0)


#endif // _VERIFIER_


