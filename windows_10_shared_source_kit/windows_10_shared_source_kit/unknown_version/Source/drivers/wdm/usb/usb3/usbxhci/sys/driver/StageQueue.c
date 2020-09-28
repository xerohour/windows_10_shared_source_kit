
/*++

Copyright (c) Microsoft Corporation

Module Name:

    StageQueue.c

Abstract:

    Manages a circular queue of Transfer stages.

--*/

#include "pch.h"

VOID
StageQueue_Initialize(
    __out
        PVOID   Context,
    __in
        UCHAR   StageSize,
    __in
        UCHAR   StageCount
    )
{
    PSTAGE_QUEUE stageQueue;

    stageQueue = (PSTAGE_QUEUE)Context;

    stageQueue->StageSize                = StageSize;
    stageQueue->StageCount               = StageCount;
    stageQueue->UsedStageCount           = 0;
    stageQueue->NextFreeStageIndex       = 0;
    stageQueue->OldestAcquiredStageIndex = 0;
    stageQueue->IteratorIndex            = 0;
    stageQueue->IterationCount           = 0;
}

UCHAR
StageQueue_GetUsedStageCount(
    __in
        PVOID   Context
    )
{
    PSTAGE_QUEUE stageQueue;

    stageQueue = (PSTAGE_QUEUE)Context;

    return stageQueue->UsedStageCount;
}

PVOID
StageQueue_Acquire(
    __inout
        PVOID   Context
    )
{
    PVOID           stage;
    PSTAGE_QUEUE    stageQueue;

    TRY {

        stageQueue = (PSTAGE_QUEUE)Context;

        if (stageQueue->UsedStageCount == stageQueue->StageCount) {
            stage = NULL;
            LEAVE;
        }

        stage = &stageQueue->StageArray[stageQueue->NextFreeStageIndex * stageQueue->StageSize];

        stageQueue->UsedStageCount += 1;

        stageQueue->NextFreeStageIndex =
            ((stageQueue->NextFreeStageIndex + 1) % stageQueue->StageCount);

    } FINALLY {

    }

    return stage;
}

VOID
StageQueue_Release(
    __inout
        PVOID   Context,
    __in
        PVOID   Stage
    )
/*++

Routine Description:

    Reclaims the Stage back to the StageQueue. The Stage supplied must be either
    the oldest or the newest stage.

--*/
{
    PSTAGE_QUEUE    stageQueue;
    UCHAR           lastEntryIndex;

    stageQueue = (PSTAGE_QUEUE)Context;

    NT_ASSERT(stageQueue->UsedStageCount != 0);

    stageQueue->UsedStageCount -= 1;

    if (Stage == &stageQueue->StageArray[stageQueue->OldestAcquiredStageIndex * stageQueue->StageSize]) {

        stageQueue->OldestAcquiredStageIndex =
            ((stageQueue->OldestAcquiredStageIndex + 1) % stageQueue->StageCount);

    } else {

        lastEntryIndex =
            ((stageQueue->NextFreeStageIndex + stageQueue->StageCount - 1) %
             stageQueue->StageCount);

        NT_ASSERTMSG("Only oldest or newest stage can be released",
                     (Stage == &stageQueue->StageArray[lastEntryIndex * stageQueue->StageSize]));

        stageQueue->NextFreeStageIndex = lastEntryIndex;
    }
}

VOID
StageQueue_ForwardScanStart(
    __inout
        PVOID   Context
    )
{
    PSTAGE_QUEUE stageQueue;

    stageQueue = (PSTAGE_QUEUE)Context;

    stageQueue->IteratorIndex  = stageQueue->OldestAcquiredStageIndex;
    stageQueue->IterationCount = stageQueue->UsedStageCount;
}

PVOID
StageQueue_ForwardScanGetNextStage(
    __inout
        PVOID   Context
    )
{
    PVOID           stage;
    PSTAGE_QUEUE    stageQueue;

    TRY {

        stageQueue = (PSTAGE_QUEUE)Context;

        if (stageQueue->IterationCount == 0) {
            stage = NULL;
            LEAVE;
        }

        stage = &stageQueue->StageArray[stageQueue->IteratorIndex * stageQueue->StageSize];

        stageQueue->IterationCount -= 1;

        stageQueue->IteratorIndex =
            ((stageQueue->IteratorIndex + 1) % stageQueue->StageCount);

    } FINALLY {

    }

    return stage;
}
