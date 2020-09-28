/*
* Copyright (c) Microsoft Corporation. All rights reserved.
*/


#ifndef _AzCodec_h_Included_
#error stringify.h requires AzCodec.h
#endif

namespace Stringify {

    LPCTSTR ConnectionType(ULONG ct);
    LPCTSTR ConnectionType(ePcxConnType ct);
    
    LPCTSTR VRefEn(UCHAR v);
    LPCTSTR EPT(UCHAR e);
    
    LPCTSTR DefaultDevice(ULONG dv);
    LPCTSTR DefaultDevice(ePcxDefDev dv);

    LPCTSTR Color(ULONG c);
    LPCTSTR Color(ePcxColor c);
    
    LPCTSTR GrossLocation(ULONG loc);
    LPCTSTR GrossLocation(ePcxGenLoc loc);

    LPCTSTR GeometricLocation(ULONG loc);
    LPCTSTR GeometricLocation(ePcxGeoLoc loc);

    LPCTSTR PortConnectivity(ULONG con);
    LPCTSTR PortConnectivity(ePcxConnType con);

    LPCTSTR AudioWidgetType(ULONG t);
    LPCTSTR AudioWidgetType(AzAudNodeType t);

}; // namespace

void LogAudioWidgetCaps(ELogLevel eLevel, AzAudioWidgetCaps awc);
void LogPinCaps(ELogLevel eLevel, AzPinCaps pc);
void LogConfigDefault(ELogLevel eLevel, AzPinWidgetConfigDefault cd);
void LogAmplifierProperties(ELogLevel eLevel, AzAmpProperties amp);
void LogBytes(ELogLevel eLevel, LPCTSTR header, __in_ecount(nBytes) BYTE bytes[], UINT nBytes);