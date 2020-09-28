/*++

Copyright (C) Microsoft. All rights reserved.

Module Name:

    client.h

Abstract:

    This file contains declarations for interfaces exported by the class
    extension to GPIO client drivers.


Environment:

    Kernel mode

--*/

#pragma once

//
// Interfaces exported by the class extension to the client driver.
//

GPIO_CLX_REGISTER_CLIENT GpioClxRegisterClient;
GPIO_CLX_UNREGISTER_CLIENT GpioClxUnregisterClient;
GPIO_CLX_PROCESS_ADD_DEVICE_PRE_DEVICE_CREATE
    GpioClxProcessAddDevicePreDeviceCreate;

GPIO_CLX_PROCESS_ADD_DEVICE_POST_DEVICE_CREATE
    GpioClxProcessAddDevicePostDeviceCreate;

GPIO_CLX_ACQUIRE_INTERRUPT_LOCK GpioClxAcquireInterruptLock;
GPIO_CLX_RELEASE_INTERRUPT_LOCK GpioClxReleaseInterruptLock;

extern const PGPIO_CLX_EXPORTED_INTERFACES
    GpioClxExports[GPIO_CLX_TOTAL_EXPORTS];


