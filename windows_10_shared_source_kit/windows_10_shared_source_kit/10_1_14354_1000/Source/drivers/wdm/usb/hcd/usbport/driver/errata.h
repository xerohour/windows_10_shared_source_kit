/*

Copyright (c) 1999-2004  Microsoft Corporation

Module Name:

    errata.h

Abstract:

    Attempt to document usb device errata.

    Note that we currently only document production devices here, not
    prototypes.

    The information and definitions in this file can be used in the event that some type of
    expert system is developed for diagnostics in the operating system.

Environment:

    Kernel & user mode

Revision History:

    11-15-01 : created
    12-19-03 : revised for neohub project

Errata Entry Format - each device sjould have an entry that looks like the following


#define USB_ERRATA_ID####_

Device:
<name or description of the device>

Device Info:
<vid/pid and descriptor data -- this can be obtained by from !_errata addr>
<usbhub debug command>

Description:
<Description of the hadware problem>

Workaround:
<hoops we jump thru (if any) to make this device work properly>

Frequency:
<how likely this will happen -- this is really just a guess>

User Impact:
<Effect -- what the end user sees>

Flags:
Any 'special' registry flags that are relavent

*/

// Template
/*---------------------------------------------------------------------------------------------*/
/*
#
Device:
Device Info:
Description:
Workaround:
Frequency:
User Impact:
Flags:
*/

#ifndef   __ERRATA_H__
#define   __ERRATA_H__

/*---------------------------------------------------------------------------------------------*/
/*
#1

Device:
USB Intellimouse

Device Info:
VID_045E PID_009 revision unknown

Description:
This device has problems with multiple packets per frame, it will return a truncated config
descriptor and stall error randomly.  This is seen on OHCI and EHCI controllers because they
transmit >1 phase of a control transfer per frame.  It is not seen on UHCI controllers which
transmit only one packet (phase) of a control transfer.

Workaround:
Validation and retry logic for getting the configuration descriptors allows this device to function
most of the time.

Frequency:
Low in real life. Common in stress.

User Impact:
The device may fail to enumerate.  The device will also fail in stress tests.

Flags:
none.

*/

/*---------------------------------------------------------------------------------------------*/
/*
#2

Device:
Nokia Monitor Hub with hid controls

Device Info:
VID_045E PID_009 revision unknown

Description:
This device will not function downstream of a USB 2.0 hub (tested with NEC).  The device
requests time out probably due to a device response time (ACK) that is  not within spec.
Enumeration is erratic or it does enumerate at all.

Workaround:
none in software, device should not be used in line with a high speed hub.

Frequency:
Common.

User Impact:
The device may fail to enumerate.

Flags:
none.

*/

/*---------------------------------------------------------------------------------------------*/
/*
#3

Device:
Samsung (Compaq) composite keyboard. AKA 'prowler'.

Device Info:
Description:
Workaround:
Frequency:
User Impact:
Flags:


    VID_049F PID_000E Samsung (Compaq) composite keyboard

    This devices resume signalling is not within spec (according to
    Compaq/Intel).  This casues the device to 'disconnect' when signalling
    resume (AKA remote wake) on some Intel ICH4 chipsets.
*/

/*
    Vid_0738&Pid_4420 Andretti Racing Wheel

    This device has problems with enumeration speed -- ie how quickly the
    requests are sent following a reset

    This device will also fail the MS OS descriptor request in such a way
    as to require a reset -- which fails due to reasons above.

    NO WORKAROUND
*/

/*
    Vid Pid unknown Visioneer Strobe Pro USB

    The "Visioneer Strobe Pro USB" returns a bogus serial # If we return this
    bogus string to PnP we blue screen.

    WORKAROUND
    Code is added to the hub driver to validate the serial number string
    and remove bogus characters

*/


#endif
