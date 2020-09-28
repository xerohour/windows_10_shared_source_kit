
/*++

Copyright (c) Microsoft Corporation

Module Name:

    StageQueue.h

Abstract:

    Manages a circular queue of Transfer stages.

--*/

#pragma once

typedef struct _STAGE_QUEUE_HEADER {

    UCHAR   StageSize;
    UCHAR   StageCount;
    UCHAR   UsedStageCount;

    UCHAR   NextFreeStageIndex;
    UCHAR   OldestAcquiredStageIndex;

    UCHAR   IteratorIndex;
    UCHAR   IterationCount;

    UCHAR   Padding;

} STAGE_QUEUE_HEADER, *PSTAGE_QUEUE_HEADER;

//
// STAGE_QUEUE_HEADER is followed by a byte array in the _STAGE_QUEUE structure.
// That byte array may start on any byte aligned offset, which is not ok because
// the StageArray in _BULK_STAGE_DATA and _ISOCH_STAGE_DATA start on 8-byte
// alignment. So ensure that the STAGE_QUEUE_HEADER is 8-byte aligned so that
// the StageArray is aligned in the same way across all structures.
//
C_ASSERT((sizeof(STAGE_QUEUE_HEADER) % 8) == 0);

typedef struct _STAGE_QUEUE {

    STAGE_QUEUE_HEADER;

    DECLSPEC_ALIGN(MEMORY_ALLOCATION_ALIGNMENT)
    BYTE    StageArray[ANYSIZE_ARRAY];

} STAGE_QUEUE, *PSTAGE_QUEUE;

VOID
StageQueue_Initialize(
    __out
        PVOID   Context,
    __in
        UCHAR   StageSize,
    __in
        UCHAR   StageCount
    );

UCHAR
StageQueue_GetUsedStageCount(
    __in
        PVOID   Context
    );

PVOID
StageQueue_Acquire(
    __inout
        PVOID   Context
    );

VOID
StageQueue_Release(
    __inout
        PVOID   Context,
    __in
        PVOID   Stage
    );

VOID
StageQueue_ForwardScanStart(
    __inout
        PVOID   Context
    );

PVOID
StageQueue_ForwardScanGetNextStage(
    __inout
        PVOID   Context
    );
