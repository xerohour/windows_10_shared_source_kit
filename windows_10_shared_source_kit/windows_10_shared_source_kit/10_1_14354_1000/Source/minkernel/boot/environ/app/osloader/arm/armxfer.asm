;++
;
; Copyright (c) Microsoft Corporation
;
; Module Name:
;
;   armxfer.asm
;
; Abstract:
;
;   ASM routines for transitioning control between the OS Loader and the
;   OS Kernel.
;
; Environment:
;
;   Boot Environment
;
;--

#include <ksarm.h>


        IMPORT OslArchKernelStack
        EXPORT OslArmTransferToKernel

        TEXTAREA

;++
;
; VOID
; OslArmTransferToKernel (
;       IN PLOADER_PARAMETER_BLOCK  LoaderBlock,
;       IN PVOID                    KernelEntryPoint
;       );
;
; RoutineDescription -
;
;   This routine will transfer execution from the OS loader to the kernel.
;
; Arguments:
;
;   LoaderBlock (r0) - A pointer to the kernel's input parameter structure.
;
;   KernelEntryPoint (r1) - Linear address to the kernel's entry point.
;
; Return Value:
;
;   None.
;
;--
        LEAF_ENTRY OslArmTransferToKernel

        ldr     r2, =OslArchKernelStack                 ; Grab the kernel stack
        ldr     sp, [r2]                                ; switch stacks
        bx      r1                                          ; jump to the kernel

        LEAF_END OslArmTransferToKernel

        END

