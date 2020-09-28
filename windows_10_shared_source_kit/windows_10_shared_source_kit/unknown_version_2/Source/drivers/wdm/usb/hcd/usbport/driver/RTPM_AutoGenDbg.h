
/*++
Copyright (c) Microsoft Corporation

Module Name:

RTPM_AutoGenDbg.h

Abstract:

     This header file contains State Machines for USBPORT
     This has been generated automatically from a visio file.
     DO NOT MODIFY THIS FILE MANUALLY.

--*/



DECLSPEC_SELECTANY PCSTR RTPMOperationsPerState[][6] =
{
    { "AcknowledgePower" ,  "" },
    { "AcknowledgePower" ,  "" },
    { "ArmForWakeFromS0" ,  "" },
    { "CompleteHubWWIrp" ,  "" },
    { "EmptyFunction" ,  "" },
    { "DisarmForWakeFromS0" ,  "" },
    { "DisarmForWakeFromS0" ,  "" },
    { "EmptyFunction" ,  "" },
    { "CompleteHubWWIrp" , "EnableTimerWork" ,  "" },
    { "EmptyFunction" ,  "" },
    { "IndicateCriticalPowerFailure" ,  "" },
    { "EnableInterrupts" , "EnableTimerWork" ,  "" },
    { "EnableInterrupts" ,  "" },
    { "RequestD0" ,  "" },
    { "RequestD0" ,  "" },
    { "DisableTimerWork" , "RequestDx" ,  "" },
    { "EmptyFunction" ,  "" },
    { "EmptyFunction" ,  "" },
    { "EmptyFunction" ,  "" },
    { "EmptyFunction" ,  "" },
};



