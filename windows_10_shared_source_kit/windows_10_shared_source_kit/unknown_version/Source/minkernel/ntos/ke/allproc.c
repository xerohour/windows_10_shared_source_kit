/*++

Copyright (c) 2000  Microsoft Corporation

Module Name:

    allproc.c

Abstract:

    This module allocates and initializes kernel resources required to
    start a new processor, and passes a complete process state structure
    to the hal to obtain a new processor.


Environment:

    Kernel mode only.

--*/

#include "ki.h"
#include "pool.h"

//
// Define prototypes for forward referenced functions.
//

VOID
KiInitializeProcessorContext (
    __out PKPROCESSOR_STATE ProcessorState,
    __in ULONG LoaderBlock,
    __in ULONG TopOfStack,
    __in ULONG Startup
    );

//
// Define processor node query function address.
//
// N.B. These function addresses are filled in during kernel initialization
//      if the host system is a multinode system.
//

extern PHALNUMAQUERYNODECAPACITY KiQueryNodeCapacity;
extern PHALNUMAQUERYNODEDISTANCE KiQueryNodeDistance;

//
// Dummy NUMA node structures used during system initialization.
//

#pragma bss_seg("PAGEDATAZ")

KNODE KiNodeInit[MAXIMUM_NUMA_NODES];

#pragma bss_seg()

#pragma alloc_text(INIT, KiAdjustProcessorAffinity)
#pragma alloc_text(PAGE, KiInitializeProcessorContext)
#pragma alloc_text(PAGE, KiInitializeProcessorState)
#pragma alloc_text(PAGELK, KiComputeProcessorDataSize)

ULONG
KiComputeProcessorDataSize (
    __in ULONG MaxProcCount
    )

/*++

Routine Description:

    This routine computes the size of per processor data structure.

Arguments:

    MaxProcCount - Supplies the maximum processors the system can have.

Return Value:

    Size of per processor data structure in bytes.

--*/

{

    ULONG PrcbSize;
    ULONG ProcessorDataSize;

    //
    // Calculate the size of the per processor data structures.
    //
    // This includes:
    //
    //   PCR (including the PRCB)
    //   Shared Ready Queue
    //   Idle Thread Object
    //   Double Fault Stack
    //   Multinode structure
    //
    // A DPC and Idle stack are also allocated, but they are done separately.
    //

    PrcbSize = sizeof(KPCR) + sizeof(REQUEST_MAILBOX) * (MaxProcCount - 1);
    ProcessorDataSize = ROUNDUP128(PrcbSize) +
                        ROUNDUP128(DOUBLE_FAULT_STACK_SIZE) +
                        ROUNDUP128(sizeof(ETHREAD)) +
                        ROUNDUP128(sizeof(ENODE));

    return ProcessorDataSize;
}

VOID
KiInitializeProcessorContext (
    __out PKPROCESSOR_STATE ProcessorState,
    __in ULONG LoaderBlock,
    __in ULONG TopOfStack,
    __in ULONG Startup
    )

/*++

Routine Description:

    This routine will initialize the processor context which will be used to
    start non-boot processors.

Arguments:

    ProcessorState - Supplies a pointer to the processor state.

    LoaderBlock - Supplies a pointer to the loader parameter block.

    TopOfStack - Supplies a pointer to the top of kernel stack for new
        processor.

    Startup - Supplies a pointer to the kernel routine where the new processor
        will start executing.

Return Value:

    None.

--*/

{

    PAGED_CODE();

    //
    // Each processor starts in the system initialization code
    // with address of the loader parameter block as an argument
    // and using the new kernel stack.
    //

    RtlZeroMemory(&ProcessorState->ContextFrame,
                  sizeof(ProcessorState->ContextFrame));

    ProcessorState->ContextFrame.ContextFlags = CONTEXT_INTEGER;
    ProcessorState->ContextFrame.R0 = LoaderBlock;
    ProcessorState->ContextFrame.Sp = TopOfStack;
    ProcessorState->ContextFrame.Pc = Startup;
    ProcessorState->ContextFrame.Cpsr = INITIAL_KERNEL_CPSR;
    return;
}

PKPRCB
KiInitializeProcessorState (
    __out PKPROCESSOR_STATE State,
    __out PLOADER_PARAMETER_BLOCK LoaderBlock,
    __out_bcount(AllocationSize) PUCHAR PerProcessorAllocation,
    __in LOGICAL DynamicProcessor,
    __in ULONG AllocationSize,
    __in ULONG NodeNumber,
    __in ULONG ProcessorIndex,
    __in ULONG MaxProcCount,
    __out PVOID Stack,
    __out PVOID DpcStack,
    __out PUCHAR IsrStack
    )

/*++

Routine Description:

    This routine initializes the processor state used for starting non-boot
    processors.

Arguments:

    State - Supplies a pointer to the processor state that will be initialized.

    LoaderBlock - Supplies a pointer to the loader parameter block.

    PerProcessorAllocation - Supplies a pointer to the per-processor allocation.

    DynamicProcessor - Supplies a boolean indicating whether this is a boot
        processor or one which is being hot-added.

    AllocationSize - Supplies size in bytes of the per-processor allocation.

    NodeNumber - Supplies the processor node number.

    ProcessorIndex - Supplies the NT processor number.

    MaxProcCount - Supplies the maximum processors the system can have.

    Stack - Supplies a pointer to the kernel stack for this processor.

    DpcStack - Supplies a pointer to the DPC stack for this processor.

Return Value:

    A pointer to the processor control block is returned.

--*/

{

    PUCHAR Base;
    PKPCR CurrentPcr;
    PKNODE Node;
    PKPCR PcrBase;
    PETHREAD Thread;

    PAGED_CODE();

    //
    // Build up a processor state for new processor.
    //

    CurrentPcr = KeGetPcr();
    RtlZeroMemory(PerProcessorAllocation, AllocationSize);
    Base = PerProcessorAllocation;
    RtlZeroMemory (State, sizeof(KPROCESSOR_STATE));

    //
    // Set the PCR base address for the next processor, set the processor
    // number, and set the processor speed.
    //
    // N.B. The PCR address is passed to the next processor by computing
    //      the containing address with respect to the PRCB.
    //

    PcrBase = (PKPCR)Base;
    PcrBase->Prcb.Number = ProcessorIndex;
    PcrBase->Prcb.MHz = KeGetCurrentPrcb()->MHz;
    PcrBase->Prcb.CycleCounterFrequency = KeGetCurrentPrcb()->CycleCounterFrequency;
    Base += ROUNDUP128(sizeof(KPCR) +
                       sizeof(REQUEST_MAILBOX) * (MaxProcCount - 1));

    //
    // Initialize the panic stack address. We offset by 8 because
    // the top 8 bytes are used to save the previous PC/SPSR before
    // we generated the fault that put us on the panic stack.
    //

    Base += DOUBLE_FAULT_STACK_SIZE;
    PcrBase->Prcb.PanicStackBase = (ULONG)Base;

    //
    // Initialize the ISR stack address.
    //

    PcrBase->Prcb.IsrStack = IsrStack;

    //
    // Idle Thread thread object.
    //

    Thread = (PETHREAD)Base;
    Base += ROUNDUP128(sizeof(ETHREAD));

    //
    // If this is the first processor on this node, then use the space
    // already allocated for the node. Otherwise, the space allocated
    // is not used.
    //

    Node = KeNodeBlock[NodeNumber];
    if (Node == &KiNodeInit[NodeNumber]) {
        Node = (PKNODE)Base;
        *Node = KiNodeInit[NodeNumber];
        KeNodeBlock[NodeNumber] = Node;
    }

    if (Node->Flags.GroupCommitted == 0) {
        KiCommitNodeAssignment(Node);
    }

    Base += ROUNDUP128(sizeof(ENODE));

    //
    // Set the parent node address.
    //

    PcrBase->Prcb.ParentNode = Node;
    KiAddProcessorToGroupDatabase(&PcrBase->Prcb, DynamicProcessor);

    //
    // Initialize processor context.
    //

    KiInitializeProcessorContext(State,
                                 (ULONG)LoaderBlock,
                                 (ULONG)Stack,
                                 (ULONG)KiSystemStartup);

    //
    // Set other special registers in the processor state.
    //
    // N.B. IRQL is raised to ensure that there is no partial save of control
    //      state. While at this stage this cannot occur even at passive level,
    //      IRQL is raised nevetheless to satisfy asserts while saving the
    //      state.
    //

    NT_ASSERT(KeGetCurrentIrql() == PASSIVE_LEVEL);

    KfRaiseIrql(DISPATCH_LEVEL);
    KiSaveProcessorControlState(State);
    KeLowerIrql(PASSIVE_LEVEL);

    State->SpecialRegisters.Cp15_Cr13_UsrRW = 0;
    State->SpecialRegisters.Cp15_Cr13_UsrRO = 0;
    State->SpecialRegisters.Cp15_Cr13_SvcRW = (ULONG) PcrBase;

    //
    // Adjust the loader block so it has the next processor state.
    //

    KiInitializeProcessorLoaderBlock(LoaderBlock,
                                     DpcStack,
                                     Thread,
                                     &PcrBase->Prcb);

    //
    // Return pointer to the PRCB for the new processor.
    //

    return &PcrBase->Prcb;
}

