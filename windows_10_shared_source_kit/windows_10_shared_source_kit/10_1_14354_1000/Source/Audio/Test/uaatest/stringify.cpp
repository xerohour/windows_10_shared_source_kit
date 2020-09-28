/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/


#include <windows.h>
#include <tchar.h>
#define USERMODE
#include <azcodec.h>

#include <shell98.h>
#include <module98.h>
#include <dvmodule.h>

#include "aznode.h"
#include "power.h"
#include "codec.h"
#include "stringify.h"
#include "log.h"

#define CASE(x) case x: return _T(#x)
#define DEFAULT() default: return _T("Unknown")

namespace Stringify {

    LPCTSTR ConnectionType(ULONG ct) { return ConnectionType((ePcxConnType)ct); }
    LPCTSTR ConnectionType(ePcxConnType ct) {
        switch (ct) {
            CASE(ePcxConnTypeUnknown);
            CASE(ePcxConnTypeEighth);
            CASE(ePcxConnTypeQuarter);
            CASE(ePcxConnTypeAtapiInternal);
            CASE(ePcxConnTypeRCA);
            CASE(ePcxConnTypeOptical);
            CASE(ePcxConnTypeOtherDigital);
            CASE(ePcxConnTypeOtherAnalog);
            CASE(ePcxConnTypeMultichannelAnalogDIN);
            CASE(ePcxConnTypeXlrProfessional);
            CASE(ePcxConnTypeRJ11Modem);
            CASE(ePcxConnTypeCombination);
            CASE(ePcxConnTypeReserved0);
            CASE(ePcxConnTypeReserved1);
            CASE(ePcxConnTypeReserved2);
            CASE(ePcxConnTypeReserved3);
            DEFAULT();
        }
    }

    LPCTSTR VRefEn(UCHAR v) {
        switch(v) {
            case 0: return _T("Hi-Z");
            case 1: return _T("50%");
            case 2: return _T("Ground (0 V)");
            // 3 is reserved
            case 4: return _T("80%");
            case 5: return _T("100%");

            case 3: __fallthrough;
            case 6: __fallthrough;
            case 7: return _T("Reserved");

            DEFAULT();
        }
    }

    LPCTSTR EPT(UCHAR e)
    {
        switch (e)
        {
            case 0: return _T("Native");
            case 1: __fallthrough;
            case 2: return _T("Reserved");
            case 3: return _T("High Bit Rate");
            DEFAULT();
        }
    }

    LPCTSTR DefaultDevice(ULONG dv) { return DefaultDevice((ePcxDefDev)dv); }
    LPCTSTR DefaultDevice(ePcxDefDev dv) {
        switch (dv) {
            CASE(ePcxDefDevLineOut);
            CASE(ePcxDefDevSpeaker);
            CASE(ePcxDefDevHPOut);
            CASE(ePcxDefDevCD);
            CASE(ePcxDefDevSpdifOut);
            CASE(ePcxDefDevDigitalOtherOut);
            CASE(ePcxDefDevModemLineSide);
            CASE(ePcxDefDevModemHandsetSide);
            CASE(ePcxDefDevLineIn);
            CASE(ePcxDefDevAUX);
            CASE(ePcxDefDevMicIn);
            CASE(ePcxDefDevTelephony);
            CASE(ePcxDefDevSpdifIn);
            CASE(ePcxDefDevDigitalOtherIn);
            CASE(ePcxDefDevReserved0);
            CASE(ePcxDefDevOther);

            DEFAULT();
        }
    }

    LPCTSTR Color(ULONG c) { return Color((ePcxColor)c); }
    LPCTSTR Color(ePcxColor c) {
        switch (c) {
            CASE(ePcxColorUnknown);
            CASE(ePcxColorBlack);
            CASE(ePcxColorGrey);
            CASE(ePcxColorBlue);
            CASE(ePcxColorGreen);
            CASE(ePcxColorRed);
            CASE(ePcxColorOrange);
            CASE(ePcxColorYellow);
            CASE(ePcxColorPurple);
            CASE(ePcxColorPink);
            CASE(ePcxColorReserved0);
            CASE(ePcxColorReserved1);
            CASE(ePcxColorReserved2);
            CASE(ePcxColorReserved3);
            CASE(ePcxColorWhite);
            CASE(ePcxColorOther);
            
            DEFAULT();
        }
    }

    LPCTSTR GeometricLocation(ULONG loc) { return GeometricLocation((ePcxGeoLoc)loc); }
    LPCTSTR GeometricLocation(ePcxGeoLoc loc) {
        switch (loc) {
            CASE(ePcxGeoLocReserved0);
            CASE(ePcxGeoLocRear);
            CASE(ePcxGeoLocFront);
            CASE(ePcxGeoLocLeft);
            CASE(ePcxGeoLocRight);
            CASE(ePcxGeoLocTop);
            CASE(ePcxGeoLocBottom);
            CASE(ePcxGeoLocSpecial0);
            CASE(ePcxGeoLocSpecial1);
            CASE(ePcxGeoLocSpecial2);
            CASE(ePcxGeoLocReserved1);
            CASE(ePcxGeoLocReserved2);
            CASE(ePcxGeoLocReserved3);
            CASE(ePcxGeoLocReserved4);
            CASE(ePcxGeoLocReserved5);
            CASE(ePcxGeoLocReserved6);
            
            DEFAULT();
        }
    }
    
    LPCTSTR GrossLocation(ULONG loc) { return GrossLocation((ePcxGenLoc)loc); }
    LPCTSTR GrossLocation(ePcxGenLoc loc) {
        switch (loc) {
            CASE(ePcxGenLocPrimBox);
            CASE(ePcxGenLocInternal);
            CASE(ePcxGenLocSeperate);
            CASE(ePcxGenLocOther);
            
            DEFAULT();
        }
    }

    LPCTSTR PortConnectivity(ULONG con) { return PortConnectivity((ePcxConnType)con); }
    LPCTSTR PortConnectivity(ePcxConnType con) {
        switch (con) {
            CASE(ePcxPortConnJack);
            CASE(ePcxPortConnNoPhysConn);
            CASE(ePcxPortConnIntegratedDevice);
            CASE(ePcxPortConnBothIntegratedAndJack);
            
            DEFAULT();
        }
    }


    LPCTSTR AudioWidgetType(ULONG t) { return AudioWidgetType((AzAudNodeType)t); }
    LPCTSTR AudioWidgetType(AzAudNodeType t) {
        switch (t) {
            CASE(AzAudNodeOutput);
            CASE(AzAudNodeInput); 
            CASE(AzAudNodeMixer);
            CASE(AzAudNodeSelector);
            CASE(AzAudNodePinComplex);
            CASE(AzAudNodePower);
            CASE(AzAudVolumeKnob);
            CASE(AzAudBeepGen);
            CASE(AzAudNodeVendor);
            
            DEFAULT();
        }
    }

}; // namespace

void LogAudioWidgetCaps(ELogLevel eLevel, AzAudioWidgetCaps awc) {
    SLOG(eLevel,
        _T("-- Audio Widget Capabilities --\n")
        _T("Stereo/Channel Count least-significant bit: %u\n")
        _T("In Amplifier Present: %u\n")
        _T("Out Amplifier Present: %u\n")
        _T("Amplifier Parameter Override: %u\n")
        _T("Format Override: %u\n")
        _T("Stripe: %u\n")
        _T("Processing Controls Widget: %u\n")
        _T("Unsolicited Response Capable: %u\n")
        _T("Connection List: %u\n")
        _T("Digital: %u\n")
        _T("Power State Control: %u\n")
        _T("Left/Right Swap: %u\n")
        _T("Content Protection Capable: %u\n")
        _T("Extended Channel Count: %u\n")
        _T("Delay: %u\n")
        _T("Type: %u (%s)\n")
        _T("Reserved (24 through 31): 0x%x\n")
        _T("\n")
        _T("(Calculated channel count: %u)"),
        awc.Stereo,
        awc.InAmpPresent,
        awc.OutAmpPresent,
        awc.AmpParamOverride,
        awc.FormatOverride,
        awc.Stripe,
        awc.ProcNode,
        awc.UnSolCapable,
        awc.ConnList,
        awc.Digital,
        awc.PowerCntrl,
        awc.LRSwap,
        awc.CPCapable,
        awc.ChanCountExt,
        awc.Delay,
        awc.Type, Stringify::AudioWidgetType(awc.Type),
        awc.Reserved0,
        ((awc.ChanCountExt << 1) | awc.Stereo) + 1
    );
}

void LogPinCaps(ELogLevel eLevel, AzPinCaps pc) {
    SLOG(eLevel,
        _T("--- Pin Capabilities ---\n")
        _T("Impedance Sense: %u\n")
        _T("Trigger Required: %u\n")
        _T("Presence Detect Capable: %u\n")
        _T("Headphone Drive Capable: %u\n")
        _T("Output Capable: %u\n")
        _T("Input Capable: %u\n")
        _T("Balanced I/O Pins: %u\n")
        _T("HDMI: %u\n")
        _T("VRef - HiZ: %u\n")
        _T("VRef - 50%%: %u\n")
        _T("VRef - Ground: %u\n")
        _T("VRef - Reserved (11): %u\n")
        _T("VRef - 80%%: %u\n")
        _T("VRef - 100%%: %u\n")
        _T("VRef - Reserved (14, 15): 0x%x\n")
        _T("EAPD Capable: %u\n")
        _T("Reserved (17-23): 0x%x\n")
        _T("Display Port: %u\n")
        _T("Reserved (25, 26): 0x%x\n")
        _T("High Bit Rate: %u\n")
        _T("Reserved (28-31): 0x%x"),
        pc.ImpSense,
        pc.TriggerRequired,
        pc.PresenceDetect,
        pc.HpDriveCapable,
        pc.OutputCapable,
        pc.InputCapable,
        pc.BalancedIO,
        pc.HDMICapable,
        pc.HiZ,
        pc.Volt50,
        pc.Ground,
        pc.Reserved1,
        pc.Volt80,
        pc.Volt100,
        pc.Reserved2,
        pc.EAPD,
        pc.Reserved3,
        pc.DPCapable,
        pc.Reserved4,
        pc.HBR,
        pc.Reserved4
    );
}

void LogConfigDefault(ELogLevel eLevel, AzPinWidgetConfigDefault cd)
{
    SLOG(eLevel,
        _T("--- Configuration Default ---\n")
        _T("Sequence: %u\n")
        _T("Default Association: %u\n")
        _T("Miscellaneous - Jack Detect Override: %u\n")
        _T("Miscellaneous - Reserved (9-11): %u\n")
        _T("Color: %u (%s)\n")
        _T("Connection Type: %u (%s)\n")
        _T("Default Device: %u (%s)\n")
        _T("Geometric Location: %u (%s)\n")
        _T("Gross Location: %u (%s)\n")
        _T("Port Connectivity: %u (%s)"),
        cd.Sequence,
        cd.DefAssoc,
        cd.JackDetectOverride,
        cd.Reserved,
        cd.Color, Stringify::Color(cd.Color),
        cd.ConnType, Stringify::ConnectionType(cd.ConnType),
        cd.DefDev, Stringify::DefaultDevice(cd.DefDev),
        cd.GeoLoc, Stringify::GeometricLocation(cd.GeoLoc),
        cd.GenLoc, Stringify::GrossLocation(cd.GenLoc),
        cd.PortConn, Stringify::PortConnectivity(cd.PortConn)
    );
}

void LogAmplifierProperties(ELogLevel eLevel, AzAmpProperties amp)
{
    SLOG(eLevel,
        _T("--- Amplifier Properties ---\n")
        _T("Offset: %u\n")
        _T("Reserved (7): %u\n")
        _T("NumSteps: %u\n")
        _T("Reserved (15): %u\n")
        _T("StepSize: %u\n")
        _T("Reserved (23-30): 0x%x\n")
        _T("Mute: %u"),
        amp.Offset,
        amp.Reserved0,
        amp.NumSteps,
        amp.Reserved1,
        amp.StepSize,
        amp.Reserved2,
        amp.Mute
    );
}

void LogBytes(ELogLevel eLevel, LPCTSTR header, __in_ecount(nBytes) BYTE bytes[], UINT nBytes)
{
    switch (nBytes)
    {
        case 0:
            // nothing to do
            break;

        case 1:
            SLOG(eLevel,
                _T("%s")
                _T("%02x"),
                header,
                bytes[0]
            );
            break;

        case 2:
            SLOG(eLevel,
                _T("%s")
                _T("%02x %02x"),
                header,
                bytes[0], bytes[1]
            );
            break;

        case 3:
            SLOG(eLevel,
                _T("%s")
                _T("%02x %02x %02x"),
                header,
                bytes[0], bytes[1], bytes[2]
            );
            break;

        case 4:
            SLOG(eLevel,
                _T("%s")
                _T("%02x %02x %02x %02x"),
                header,
                bytes[0], bytes[1], bytes[2], bytes[3]
            );
            break;

        case 5:
            SLOG(eLevel,
                _T("%s")
                _T("%02x %02x %02x %02x %02x -- -- --"),
                header,
                bytes[0], bytes[1], bytes[2], bytes[3],
                bytes[4]
            );
            break;

        case 6:
            SLOG(eLevel,
                _T("%s")
                _T("%02x %02x %02x %02x %02x %02x -- --"),
                header,
                bytes[0], bytes[1], bytes[2], bytes[3],
                bytes[4], bytes[5]
            );
            break;

        case 7:
            SLOG(eLevel,
                _T("%s")
                _T("%02x %02x %02x %02x %02x %02x %02x --"),
                header,
                bytes[0], bytes[1], bytes[2], bytes[3],
                bytes[4], bytes[5], bytes[6]
            );
            break;

        case 8:
            SLOG(eLevel,
                _T("%s")
                _T("%02x %02x %02x %02x %02x %02x %02x %02x"),
                header,
                bytes[0], bytes[1], bytes[2], bytes[3],
                bytes[4], bytes[5], bytes[6], bytes[7]
            );
            break;
            
        // at least 9; log 8, then log the rest recursively
        default:
            SLOG(eLevel,
                _T("%s")
                _T("%02x %02x %02x %02x %02x %02x %02x %02x"),
                header,
                bytes[0], bytes[1], bytes[2], bytes[3],
                bytes[4], bytes[5], bytes[6], bytes[7]
            );
            LogBytes(eLevel, _T(""), bytes + 8, nBytes - 8);
    }
}
