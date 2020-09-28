// Copyright(C) Microsoft.All rights reserved.

#pragma once
#include "common.h"
#include <DevTrans.h>
#include <atlstr.h>
#include <strsafe.h>
#include <StressSafeFormattedString.h>
#include <math.h>
#include <kshelper.h>
#include <basicloghelper.h>
#include <MicArrayGeometryValidator.h>

#define ROUNDUP_8(x) (((x) + 7) & ~7)
#define GUID_FORMAT "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}"
#define GUID_ARGS(g) \
    (g).Data1, (g).Data2, (g).Data3, \
    (g).Data4[0], (g).Data4[1], (g).Data4[2], (g).Data4[3], \
    (g).Data4[4], (g).Data4[5], (g).Data4[6], (g).Data4[7]

inline double PI() {
    return 4.0 * atan(1.0); // the tangent of pi/4 is 1 so the arctangent of 1 is pi/4
}

// mic array angles are 10000 * radians
const SHORT MicArray_180_Degrees = 31416; // 10000 * pi
const SHORT MicArray_90_Degrees = 15708; // 10000 * pi / 2

// orientation is defined by spherical coordinates (theta, psi)
// locations on the surface of the Earth are identified in the same way:
// psi is the latitude and theta is the longitude
// for example, Redmond, WA is at:
//     psi = +47 degrees (N is positive) and
//     theta = -122 degrees (W is negative)
//
// theta, the horizontal angle, can range from -180 degrees to +180 degrees
// -180 = +180 = about the international date line
const SHORT MicArray_HorizontalAngle_Min = -MicArray_180_Degrees;
const SHORT MicArray_HorizontalAngle_Max = MicArray_180_Degrees;

// psi, the vertical angle, can range from -90 degrees to +90 degrees
// -90 = the South pole
// +90 = the North pole
//
// note that if psi = -90 or +90 then theta is meaningless
const SHORT MicArray_VerticalAngle_Min = -MicArray_90_Degrees;
const SHORT MicArray_VerticalAngle_Max = MicArray_90_Degrees;

inline double RadiansFromMicArrayAngle(SHORT micarray)
{
    return micarray / 10000.0;
}

inline double DegreesFromRadians(double radians)
{
    // 1 circle = 360 degrees = 2 pi radians
    // 1 = (360 degrees) / (2 pi radians) = (180 degrees) / (pi radians)
    // r radians * 1 = r radians * (180 degrees) / (pi radians) = (180 * r / pi) degrees
    return 180.0 * radians / PI();
}

inline double DegreesFromMicArrayAngle(SHORT micarray)
{
    return DegreesFromRadians(RadiansFromMicArrayAngle(micarray));
}

inline AngleBetweenInclusive(SHORT a, SHORT b, SHORT c)
{
    return a <= b && b <= c;
}

inline AngleBetweenExclusive(SHORT a, SHORT b, SHORT c)
{
    return a < b && b < c;
}

inline double FixedPoint16_16ToDouble(long lVal)
{
    return ((double)(lVal) / (65536.0));
}

inline void CPinReq::LogPinMsg(UINT level, LPCSTR message)
{
    XLOG(XMSG, level, "  Pin[%02d] %s: %s", 
            m_iPin, m_pPinFactory->GetFriendlyName(), message);
}

inline void CPinReq::LogPinErr(LPCSTR type, LPCSTR message)
{
    XLOG(XFAIL, eError, "  %s: Pin[%02d] %s: %s", 
            type, m_iPin, m_pPinFactory->GetFriendlyName(), message);
}

inline void CPinReq::LogPinErr(LPCSTR type, LPCSTR message, UINT uValue)
{
    XLOG(XFAIL, eError, "  %s: Pin[%02d] %s: %s: %u", 
            type, m_iPin, m_pPinFactory->GetFriendlyName(), message, uValue);
}

inline void CPinReq::LogPinSkip(LPCSTR message)
{
    XLOG(XSKIP, eError, "  Pin[%02d] %s: %s", 
            m_iPin, m_pPinFactory->GetFriendlyName(), message);
}

enum Sign
{
    Negative = -1,
    Zero = 0,
    Positive = 1
};

inline Sign SignOf(SHORT x)
{
    return (x > 0) ? Positive : ((x == 0) ? Zero : Negative);
}

// divides 3D space into 27 sections
//
// each section is defined by replacing the three ?s below with either <, >, or =
// x ? 0; y ? 0; z ? 0
//
// some example sections:
// x > 0; y < 0; z = 0
// x < 0; y = 0; z < 0
//
// initially all of space is empty
// ThreeDSpace threeD;
//
// we add a bunch of points into space and keep track of which sections are occupied
// threeD.Add(-5, 0, 7);
//
// once we're done we can make certain queries about which sections are occupied
// if (threeD.AllX(Zero)) { ... } // true if only sections with x = 0 are occupied
// if (threeD.AnyZ(Negative)) { ... } // true if there are any sections with z < 0 occupied
class ThreeDSpace
{
public:
    ThreeDSpace()
    {
        for (SHORT x = -1; x <= 1; x++)
        {
            for (SHORT y = -1; y <= 1; y++)
            {
                for (SHORT z = -1; z <= 1; z++)
                {
                    At(SignOf(x), SignOf(y), SignOf(z)) = false;
                }
            }
        }
    }
    
    void Add(SHORT x, SHORT y, SHORT z)
    {
        At(SignOf(x), SignOf(y), SignOf(z)) = true;
    }
    
    bool AnyX(Sign sought)
    {
        for (SHORT y = -1; y <= 1; y++)
        {
            for (SHORT z = -1; z <= 1; z++)
            {
                if (At(sought, SignOf(y), SignOf(z))) { return true; }
            }
        }
        
        return false;
    }
    
    bool AllX(Sign sought)
    {
        for (SHORT other = -1; other <= 1; other++)
        {
            // is there anything that doesn't match sought?
            if (SignOf(other) != sought && AnyX(SignOf(other))) { return false; }
        }
        
        return true;
    }
    
    bool AnyY(Sign sought)
    {
        for (SHORT x = -1; x <= 1; x++)
        {
            for (SHORT z = -1; z <= 1; z++)
            {
                if (At(SignOf(x), sought, SignOf(z))) { return true; }
            }
        }
        
        return false;
    }
    
    bool AllY(Sign sought)
    {
        for (SHORT other = -1; other <= 1; other++)
        {
            // is there anything that doesn't match sought?
            if (SignOf(other) != sought && AnyY(SignOf(other))) { return false; }
        }
        
        return true;
    }

    bool AnyZ(Sign sought)
    {
        for (SHORT x = -1; x <= 1; x++)
        {
            for (SHORT y = -1; y <= 1; y++)
            {
                if (At(SignOf(x), SignOf(y), sought)) { return true; }
            }
        }
        
        return false;
    }
    
    bool AllZ(Sign sought)
    {
        for (SHORT other = -1; other <= 1; other++)
        {
            // is there anything that doesn't match sought?
            if (SignOf(other) != sought && AnyZ(SignOf(other))) { return false; }
        }
        
        return true;
    }

private:
    bool m_region[3][3][3];
    bool &At(Sign s_x, Sign s_y, Sign s_z)
    {
        // the +1 converts from a "sign" value (-1, 0, 1) to an array offset (0, 1, 2)
        return m_region[(SHORT)s_x + 1][(SHORT)s_y + 1][(SHORT)s_z + 1];
    }
};

bool 
CPinReq::PinIoCtl_Get
(
    GUID PropertySetId,
    ULONG ulProperty, 
    void *pInData, 
    UINT uInSize,
    void **ppOutData, 
    UINT &uOutSize
)
{
    bool        fRet = true;
    KSP_PIN    *KsPinProp = NULL;
    void       *pInBuffer = NULL;
    ULONG       cbReturned = 0;
    UINT        uError = 0;
    
    // Initialize output data
    *ppOutData = NULL;
    uOutSize = 0;
    
    // Create the input data buffer to send to the DeviceIoControl
    // by appending the pInData to a KSP_PIN data structure and 
    // copying the whole thing in pInBuffer
    pInBuffer = new BYTE[sizeof(KSP_PIN) + uInSize];
    if(NULL == pInBuffer)
    {
        XLOG(XFAIL, eError, "ERROR: Unable to allocate memory");
        goto ExitFalse;
    }
    KsPinProp = (KSP_PIN*)pInBuffer;
    KsPinProp->Property.Set = PropertySetId;
    KsPinProp->Property.Id = ulProperty;
    KsPinProp->Property.Flags = KSPROPERTY_TYPE_GET;
    KsPinProp->PinId = m_iPin;
    KsPinProp->Reserved = 0;
    if((0 != uInSize) && (NULL != pInData))
    {   
        memcpy( (void*)((KSP_PIN*)(pInBuffer)+1), pInData, uInSize);
    }

    // Determine the needed size
    if(!DeviceIoControl(m_pFilter->GetHandle(), IOCTL_KS_PROPERTY, pInBuffer,
                sizeof(KSP_PIN)+uInSize, NULL, 0, &cbReturned, NULL))
    {
        uError = GetLastError();
        if(ERROR_MORE_DATA != uError)
        {
            LogPinErr("FAIL", FormatStr("DeviceIoControl failed to return ERROR_MORE_DATA when "
                              "passed in an output buffer of size 0. It returned %u instead.", uError));
            goto ExitFalse;
        }
    }

    // Allocate the needed size
    if( (uOutSize = cbReturned) == 0)
    {
        LogPinErr("FAIL", "DeviceIoControl returned output buffer size of 0. "
                          "Expected a value greater than 0.");
        goto ExitFalse;
    }
    *ppOutData = new BYTE[uOutSize];
    if(*ppOutData == NULL)
    {
        XLOG(XFAIL, eError, "ERROR: Unable to allocate memory");
        goto ExitFalse;
    }

    // Query the property
    if(!DeviceIoControl(m_pFilter->GetHandle(), IOCTL_KS_PROPERTY, pInBuffer,
                sizeof(KSP_PIN)+uInSize, *ppOutData, uOutSize, &cbReturned, NULL))
    {
        uError = GetLastError();
        LogPinErr("FAIL", FormatStr("DeviceIoControl failed to get pin property, returned error code %u.", uError));
        goto ExitFalse;
    }

    delete[] pInBuffer;
    pInBuffer = NULL;
    return fRet;

ExitFalse:
    fRet = false;
    uOutSize = 0;
    delete[] *ppOutData;
    *ppOutData = NULL;
    delete[] pInBuffer;
    pInBuffer = NULL;
    return fRet;
}

bool CPinReq::PinIoCtl_Support(GUID PropertySetId, ULONG ulProperty, DWORD &dwSupport)
{
    KSP_PIN KsPinProp = {};
    
    KsPinProp.Property.Set = PropertySetId;
    KsPinProp.Property.Id = ulProperty;
    KsPinProp.Property.Flags = KSPROPERTY_TYPE_BASICSUPPORT;
    KsPinProp.PinId = m_iPin;
    KsPinProp.Reserved = 0;
    
    ULONG cbReturned = 0;

    // Query basic support
    if(!DeviceIoControl(
        m_pFilter->GetHandle(),
        IOCTL_KS_PROPERTY,
        &KsPinProp, sizeof(KsPinProp),
        &dwSupport, sizeof(dwSupport),
        &cbReturned,
        NULL))
    {
        DWORD dwErr = GetLastError();
        switch (dwErr)
        {
            case ERROR_NOT_SUPPORTED:
            case ERROR_SET_NOT_FOUND:
            case ERROR_INVALID_FUNCTION:
            case ERROR_NOT_FOUND: 
            case ERROR_INVALID_PARAMETER:
                dwSupport = 0;
                return true;
                
            default:
                LogPinErr("FAIL",
                    FormatStr(
                        "DeviceIoControl failed to get basic support for pin property " GUID_FORMAT "/%u, returned error code %u.",
                        GUID_ARGS(PropertySetId),
                        ulProperty,
                        dwErr
                    )
                );
                return false;
        }
    }
    
    if (sizeof(dwSupport) != cbReturned)
    {
        LogPinErr("FAIL", FormatStr("Basic support request wrote %u bytes instead of %Iu", cbReturned, sizeof(dwSupport)));
        return false;
    }
    
    if (0 != dwSupport)
    {
        LogPinMsg(eInfo1, FormatStr("Basic support: 0x%x", dwSupport));
    }

    return true;
}

bool CPinReq::CheckPinKsDataRangeHelper(void *pArg)
{
    CPinReq            *pThis = static_cast<CPinReq*>(pArg);
    bool                fRet = true;
    KSDATARANGE        *Data = NULL;
    KSDATARANGE_AUDIO  *aData = NULL;
    UINT                iData = 0, cData = 0;
    char                buffer[MAX_BUFFER_SIZE]={0};

    cData = pThis->m_pPinFactory->GetCountDataRanges();
    bool thisIsAttributeList = false;
    bool nextIsAttributeList = false;
    for(iData = 0; iData < cData; ++iData)
    {
        thisIsAttributeList = nextIsAttributeList;
        nextIsAttributeList = false;

        // ignore attribute lists
        if(thisIsAttributeList)
        {
            continue;
        }
        
        Data = pThis->m_pPinFactory->GetDataRange(iData);
        
        if(Data->Flags & KSDATARANGE_ATTRIBUTES)  {
            nextIsAttributeList = true;
        }

        // If the specifier is KSDATAFORMAT_SPECIFIER_WAVEFORMATEX
        // then Data is actually a pointer to a KSDATARANGE_AUDIO struct
        if(Data->Specifier == KSDATAFORMAT_SPECIFIER_WAVEFORMATEX)
        {
            if(Data->FormatSize != sizeof(KSDATARANGE_AUDIO))
            {
                _snprintf_s(buffer, MAX_BUFFER_SIZE, MAX_BUFFER_SIZE-1, 
                    "Pin has a specifier "
                    "KSDATAFORMAT_SPECIFIER_WAVEFORMATEX for KSDATARANGE[%u] "
                    "but its FormatSize is not the size of KSDATARANGE_AUDIO.",
                    iData);
                buffer[MAX_BUFFER_SIZE-1] = 0;
                pThis->LogPinErr("FAIL", buffer);
                fRet = false;
                continue;
            }

            aData = (KSDATARANGE_AUDIO*)(Data);
            
            //Check that values of KSDATARANGE_AUDIO are logical
            if(aData->MinimumBitsPerSample > aData->MaximumBitsPerSample)
            {
                _snprintf_s(buffer, MAX_BUFFER_SIZE,  MAX_BUFFER_SIZE-1, "KSDATARANGE_AUDIO[%u] "
                    "MinimumBitsPerSample(%u) is greater than MaximumBitsPerSample(%u).",
                    iData, aData->MinimumBitsPerSample, aData->MaximumBitsPerSample);
                buffer[MAX_BUFFER_SIZE-1] = 0;
                pThis->LogPinErr("FAIL", buffer);
                fRet = false;
            }
            if(aData->MinimumBitsPerSample < MINIMUM_BITS_PER_SAMPLE)
            {
                _snprintf_s(buffer, MAX_BUFFER_SIZE,  MAX_BUFFER_SIZE-1, "KSDATARANGE_AUDIO[%u] "
                    "MinimumBitsPerSample(%u) is less than allowed limit of %u.",
                    iData, aData->MinimumBitsPerSample, MINIMUM_BITS_PER_SAMPLE);
                buffer[MAX_BUFFER_SIZE-1] = 0;
                pThis->LogPinErr("FAIL", buffer);
                fRet = false;
            }
            if(aData->MaximumBitsPerSample > MAXIMUM_BITS_PER_SAMPLE)
            {
                _snprintf_s(buffer, MAX_BUFFER_SIZE,  MAX_BUFFER_SIZE-1, "KSDATARANGE_AUDIO[%u] "
                    "MaximumBitsPerSample(%u) is greater than allowed limit of %u",
                    iData, aData->MaximumBitsPerSample, MAXIMUM_BITS_PER_SAMPLE);
                buffer[MAX_BUFFER_SIZE-1] = 0;
                pThis->LogPinErr("FAIL", buffer);
                fRet = false;
            }
            if(aData->MinimumSampleFrequency > aData->MaximumSampleFrequency)
            {
                _snprintf_s(buffer, MAX_BUFFER_SIZE,  MAX_BUFFER_SIZE-1, "KSDATARANGE_AUDIO[%u] "
                    "MinimumSampleFrequency(%u) is greater than MaximumSampleFrequency(%u).",
                    iData, aData->MinimumSampleFrequency, aData->MaximumSampleFrequency);
                buffer[MAX_BUFFER_SIZE-1] = 0;
                pThis->LogPinErr("FAIL", buffer);
                fRet = false;
            }
            if(aData->MinimumSampleFrequency < MINIMUM_SAMPLE_FREQUENCY)
            {
                _snprintf_s(buffer, MAX_BUFFER_SIZE,  MAX_BUFFER_SIZE-1, "KSDATARANGE_AUDIO[%u] "
                    "MinimumSampleFrequency(%u) is less than allowed limit of %u.",
                    iData, aData->MinimumSampleFrequency, MINIMUM_SAMPLE_FREQUENCY);
                buffer[MAX_BUFFER_SIZE-1] = 0;
                pThis->LogPinErr("FAIL", buffer);
                fRet = false;
            }
            if(aData->MaximumSampleFrequency > MAXIMUM_SAMPLE_FREQUENCY)
            {
                _snprintf_s(buffer, MAX_BUFFER_SIZE,  MAX_BUFFER_SIZE-1, "KSDATARANGE_AUDIO[%u] "
                    "MaximumSampleFrequency(%u) is greater than allowed limit of %u",
                    iData, aData->MaximumSampleFrequency, MAXIMUM_SAMPLE_FREQUENCY);
                buffer[MAX_BUFFER_SIZE-1] = 0;
                pThis->LogPinErr("FAIL", buffer);
                fRet = false;
            }
        }
    }// For Loop iData

    return fRet;
}


bool CPinReq::CheckPinKsDataFormatHelper(void *pArg)
{
    CPinReq            *pThis = static_cast<CPinReq*>(pArg);
    bool                fRet = true;
    KSMULTIPLE_ITEM    *KsDataRanges = NULL;
    KSDATARANGE        *KsDataRange = NULL;
    KSDATAFORMAT       *KsDataFormat = NULL;
    UINT                iKsDataRanges = 0, iKsDataFormat = 0;
    ULONG               ii = 0;

    // Only check communication pins
    if(   (pThis->m_pPinFactory->GetCommunication() == KSPIN_COMMUNICATION_NONE)
       || (pThis->m_pPinFactory->GetCommunication() == KSPIN_COMMUNICATION_BRIDGE))
    {
        goto Exit;
    }
    
    if(!pThis->PinIoCtl_Get(
                        KSPROPSETID_Pin, KSPROPERTY_PIN_DATARANGES, 
                        NULL, 0, (void**)&KsDataRanges, iKsDataRanges))
    {
        pThis->LogPinErr("FAIL", "Unable to get KSPROPERTY_PIN_DATARANGES "
                         "property of the pin.");
        fRet = false;
        goto Exit;
    }

    // If the MajorFormat is KSDATAFORMAT_TYPE_MUSIC then this device is
    // MIDI or DMusic and doesn't need to support DataIntersection
    if(KsDataRanges->Count > 0)
    {
        KsDataRange = (KSDATARANGE*)(KsDataRanges + 1);
    }
    else
    {
        fRet = true;
        goto Exit;
    }
    if( IsEqualGUID(KsDataRange->MajorFormat, KSDATAFORMAT_TYPE_MUSIC) )
    {
        fRet = true;
        goto Exit;
    }

    if(!pThis->PinIoCtl_Get(
                        KSPROPSETID_Pin, KSPROPERTY_PIN_DATAINTERSECTION, 
                        (void*)KsDataRanges, iKsDataRanges, 
                        (void**)&KsDataFormat, iKsDataFormat))
    {
        pThis->LogPinErr("FAIL", "Unable to get KSPROPERTY_PIN_DATAINTERSECTION "
                         "property of the pin.");
        fRet = false;
        goto Exit;
    }
    
    //Check that KSDATAFORMAT structure returned is valid
    if(KsDataFormat->FormatSize < sizeof(KSDATAFORMAT))
    {
        pThis->LogPinErr("FAIL", "KSDATAFORMAT.FormatSize is less "
                                 "than size of KSDATAFORMAT");
        fRet = false;
        goto Exit;
    }
    if(KsDataFormat->Reserved != 0)
    {
        pThis->LogPinErr("FAIL", "KSDATAFORMAT.Reserved is not zero");
        fRet = false;
        goto Exit;
    }
    
    // Check that KSDATAFORMAT returned matches one of the KSDATARANGES sent
    bool thisIsAttributeList = false;
    bool nextIsAttributeList = false;
    
    KsDataRange = (KSDATARANGE*)(KsDataRanges + 1);
    for(ii=0; ii < KsDataRanges->Count; ++ii)
    {
        thisIsAttributeList = nextIsAttributeList;
        nextIsAttributeList = false;
        
        // ignore attribute lists
        if(thisIsAttributeList)
        {
            goto next_range;
        }
        
        if(KsDataRange->Flags & KSDATARANGE_ATTRIBUTES)
        {
            nextIsAttributeList = true;
        }

        if(  (KsDataFormat->MajorFormat == KsDataRange->MajorFormat)
           &&(KsDataFormat->SubFormat == KsDataRange->SubFormat)
           &&(KsDataFormat->Specifier == KsDataRange->Specifier))
        {
            break;
        }
        
next_range:
        ULONG size = (thisIsAttributeList ? ((PKSMULTIPLE_ITEM)KsDataRange)->Size : KsDataRange->FormatSize);
        KsDataRange = (KSDATARANGE*)((char*)(KsDataRange) + ROUNDUP_8(size));
    }
    if(ii == KsDataRanges->Count)
    {
        pThis->LogPinErr("FAIL", "The MajorFormat, SubFormat and Specifier of the "
                         "KSDATAFORMAT structure returned don't those of the "
                         "KSDATARANGES sent to the driver in a data intersection "
                         "call.");
        fRet = false;
        goto Exit;
    }

Exit:
    if(KsDataRanges != NULL)
    {
        delete[] KsDataRanges;
    }
    return fRet;
}

bool CPinReq::IsPinKsDataFormat(void *pArg, GUID format)
{
    CPinReq            *pThis = static_cast<CPinReq*>(pArg);
    bool                fRet = false;
    KSMULTIPLE_ITEM    *KsDataRanges = NULL;
    KSDATARANGE        *KsDataRange = NULL;
    UINT                iKsDataRanges = 0;

    // Only check communication pins
    if ((pThis->m_pPinFactory->GetCommunication() != KSPIN_COMMUNICATION_SINK) &&
        (pThis->m_pPinFactory->GetCommunication() != KSPIN_COMMUNICATION_BOTH))
    {
        goto Exit;
    }

    if (!pThis->PinIoCtl_Get(
                   KSPROPSETID_Pin,
                   KSPROPERTY_PIN_DATARANGES,
                   NULL,
                   0,
                   (void**)&KsDataRanges,
                   iKsDataRanges))
    {
        pThis->LogPinErr("FAIL", "Unable to get KSPROPERTY_PIN_DATARANGES "
                         "property of the pin.");
        fRet = false;
        goto Exit;
    }

    if (KsDataRanges->Count > 0)
    {
        KsDataRange = (KSDATARANGE*)(KsDataRanges + 1);
    }
    else
    {
        fRet = false;
        goto Exit;
    }

    if (IsEqualGUID(KsDataRange->MajorFormat, format))
    {
        fRet = true;
    }

Exit:
    if (KsDataRanges != NULL)
    {
        delete[] KsDataRanges;
    }
    return fRet;
}

bool CPinReq::CheckPinDataFlowHelper(void *pArg)
{
    bool            fRet = true;
    CPinReq        *pThis = static_cast<CPinReq*>(pArg);
    UINT           *piIsConnected = NULL,
                    iNumberOfPins = pThis->m_pFilter->GetCountPins();
    // Get the pins' internal connections
    piIsConnected = new UINT[iNumberOfPins];
    SecureZeroMemory(piIsConnected, sizeof(UINT)*iNumberOfPins);
    if(!GetPinInternalConnections(pThis->m_pFilter, piIsConnected, iNumberOfPins))
    {
        fRet = false;
        XLOG(XFAIL, eError, "ERROR: Size of piIsConnected is too small");
        goto Exit;
    }

    // Check for orphaned pins (not connected internally)
    for(pThis->m_iPin = 0; pThis->m_iPin < iNumberOfPins; ++pThis->m_iPin)
    {
        pThis->m_pPinFactory = pThis->m_pFilter->GetPinFactory(pThis->m_iPin);
        if(NULL == pThis->m_pPinFactory)
        {
            XLOG(XFAIL, eError, "  ERROR: Unable to obtain reference to Pin[%d]", 
                            pThis->m_iPin);
            fRet = false;
            goto Exit;
        }
        switch(piIsConnected[pThis->m_iPin])
        {
            case DATA_INTERNAL_OUT:
                if(pThis->m_pPinFactory->GetDataFlow() == KSPIN_DATAFLOW_OUT)
                {
                    pThis->LogPinErr("FAIL", "Dataflow on pin should be KSPIN_DATAFLOW_OUT");
                    fRet = false;
                }
                break;
            case DATA_INTERNAL_IN:
                if(pThis->m_pPinFactory->GetDataFlow() == KSPIN_DATAFLOW_IN)
                {
                    pThis->LogPinErr("FAIL", "Dataflow on pin should be KSPIN_DATAFLOW_IN");
                    fRet = false;
                }
                break;
            default:
                break;
        }
    }

Exit:
    if(NULL != piIsConnected)
    {
        delete[] piIsConnected;
        piIsConnected = NULL;
    }
    return fRet;
}


bool CPinReq::CheckOrphanedPinsHelper(void *pArg)
{
    bool            fRet = true;
    CPinReq        *pThis = static_cast<CPinReq*>(pArg);
    UINT           *piIsConnected = NULL,
                    iNumberOfPins = pThis->m_pFilter->GetCountPins();

    // Get the pins' internal connections
    piIsConnected = new UINT[iNumberOfPins];
    SecureZeroMemory(piIsConnected, sizeof(UINT)*iNumberOfPins);
    if(!GetPinInternalConnections(pThis->m_pFilter, piIsConnected, iNumberOfPins))
    {
        fRet = false;
        XLOG(XFAIL, eError, "ERROR: Size of piIsConnected is too small");
        goto Exit;
    }

    // Check for orphaned pins (not connected internally)
    for(pThis->m_iPin = 0; pThis->m_iPin < iNumberOfPins; ++pThis->m_iPin)
    {
        if(piIsConnected[pThis->m_iPin] == DATA_NONE)
        {
            pThis->m_pPinFactory = pThis->m_pFilter->GetPinFactory(pThis->m_iPin);
            if(NULL == pThis->m_pPinFactory)
            {
                XLOG(XFAIL, eError, "  ERROR: Unable to obtain reference to Pin[%d]", 
                            pThis->m_iPin);
                fRet = false;
                goto Exit;
            }
            pThis->LogPinErr("FAIL", "Pin is orphaned");
            fRet = false;
        }
    }

Exit:
    if(NULL != piIsConnected)
    {
        delete[] piIsConnected;
        piIsConnected = NULL;
    }
    return fRet;
}

bool CPinReq::GetPinInternalConnections
                (CFilter * pFilter, UINT piIsConnected[], UINT iSize)
{
    bool            fRet = true;
    CConnection    *pConnection;
    UINT            iCount = 0, cCount = 0;

    if(!pFilter || !piIsConnected || (iSize < pFilter->GetCountPins()))
    {
        return false;
    }

    cCount = pFilter->GetCountConnections();
    for(iCount = 0; iCount < cCount; ++iCount)
    {
        pConnection = pFilter->GetConnection(iCount);
        if(NULL == pConnection)
        {
            XLOG(XFAIL, eError, "  ERROR: Unable to obtain reference to Connection[%d]", iCount);
            fRet = false;
            break;
        }

        if(pConnection->FromNode() == -1)
        {
            piIsConnected[pConnection->FromNodePin()] |= DATA_INTERNAL_OUT;
        }
        if(pConnection->ToNode() == -1)
        {
            piIsConnected[pConnection->ToNodePin()] |= DATA_INTERNAL_IN;
        }
    }

    return fRet;
}


bool CPinReq::IsSoftwareStreamingPin()
{
    CComPtr<IMMDevice>  pmmDeviceInterface = NULL,
                        pmmDevice = NULL;

    // return false if the pin is connected to another filter
    // because then it's not the endpoint
    if (0 != strlen(m_pPinFactory->GetPhysicalConnectionLink()))
    {
        return false;
    }


    bool IsSoftwareStreamingPin = (KSPIN_COMMUNICATION_SINK == m_pPinFactory->GetCommunication()) ||
        (KSPIN_COMMUNICATION_BOTH == m_pPinFactory->GetCommunication());

    
    
    return (IsSoftwareStreamingPin);

        
}

bool CPinReq::CheckProcessingModesHelper(void *pArg)
{
    CPinReq *pThis = static_cast<CPinReq*>(pArg);
    return pThis->CheckProcessingModes();
}

bool CPinReq::CheckProposeDataformat2Helper(void *pArg)
{
    CPinReq *pThis = static_cast<CPinReq*>(pArg);
    return pThis->CheckProposeDataformat2();
}

bool CPinReq::QueryForProcessingModes(PKSMULTIPLE_ITEM  *ppKsMultiple)
{    
    UINT                    uOutSize = 0;
    bool                    fRet = false;
    DWORD                   support = 0;   
    PKSMULTIPLE_ITEM        pKsMultipleItem = NULL;
    
    fRet =  PinIoCtl_Support(KSPROPSETID_AudioSignalProcessing, KSPROPERTY_AUDIOSIGNALPROCESSING_MODES , support) &&
        (support & KSPROPERTY_TYPE_GET);
    if (!fRet)
    {        
        return false; // nothing to test
    }
       
    if(!PinIoCtl_Get(KSPROPSETID_AudioSignalProcessing, KSPROPERTY_AUDIOSIGNALPROCESSING_MODES,
        NULL, 0, (void**)&pKsMultipleItem, uOutSize))
    {
        LogPinErr("FAIL", "Failed to query KSPROPERTY_AUDIOSIGNALPROCESSING_MODES.");
        fRet = false;
        goto Exit;
    }

    if(uOutSize < sizeof(KSMULTIPLE_ITEM))
    {
        LogPinErr("FAIL", "KSPROPERTY_AUDIOSIGNALPROCESSING_MODES response is too small", uOutSize);
        fRet = false;
        goto Exit;
    }
    

     if((pKsMultipleItem->Size != uOutSize )||(pKsMultipleItem->Size != sizeof(KSMULTIPLE_ITEM) + pKsMultipleItem->Count * sizeof(GUID)))
    {
        LogPinErr("FAIL", "KSPROPERTY_AUDIOSIGNALPROCESSING_MODES response size does not add up", sizeof(KSMULTIPLE_ITEM) + pKsMultipleItem->Count * sizeof(KSPROPERTY_AUDIOSIGNALPROCESSING_MODES));
        fRet = false;
        goto Exit;
    }    

Exit:  
     if (fRet)
     {
         *ppKsMultiple = pKsMultipleItem;
     }
     else
     {
         delete [] pKsMultipleItem;
     }
    return fRet;
}

bool CPinReq::CheckProcessingModes()
{    
    PKSMULTIPLE_ITEM        pKsMultiple = NULL;
    UINT                    uOutSize = 0;
    bool fRet = true;	
    GUID category ;
    GUID* pProcessModes = NULL;
    bool supported = false;
    bool required = false;
    DWORD support = 0;

    MODE_INFO SignalProcessingModeInfo[] =
    {
        { AUDIO_SIGNALPROCESSINGMODE_DEFAULT,           L"AUDIO_SIGNALPROCESSINGMODE_DEFAULT",           0 },
        { AUDIO_SIGNALPROCESSINGMODE_RAW,               L"AUDIO_SIGNALPROCESSINGMODE_RAW",               0 },
        { AUDIO_SIGNALPROCESSINGMODE_COMMUNICATIONS,    L"AUDIO_SIGNALPROCESSINGMODE_COMMUNICATIONS",    0 },
        { AUDIO_SIGNALPROCESSINGMODE_SPEECH,            L"AUDIO_SIGNALPROCESSINGMODE_SPEECH",            0 },
        { AUDIO_SIGNALPROCESSINGMODE_MOVIE,             L"AUDIO_SIGNALPROCESSINGMODE_MOVIE",             0 },
        { AUDIO_SIGNALPROCESSINGMODE_MEDIA,             L"AUDIO_SIGNALPROCESSINGMODE_MEDIA",             0 },
        { AUDIO_SIGNALPROCESSINGMODE_NOTIFICATION,      L"AUDIO_SIGNALPROCESSINGMODE_NOTIFICATION",      0 }
    };

    category = *(m_pPinFactory->GetCategory());

    // Condition for the test:
    // 1. Should be a software streaming pin.
    // 2. Should not be a loopback pin (Loopback pin does not support processing modes).
    // 3. Should support KSDATAFORMAT_TYPE_AUDIO data format.

    required = (IsSoftwareStreamingPin() && 
                category != KSNODETYPE_AUDIO_LOOPBACK && 
                IsPinKsDataFormat(static_cast<void*>(this), KSDATAFORMAT_TYPE_AUDIO));

    supported = PinIoCtl_Support(KSPROPSETID_AudioSignalProcessing, KSPROPERTY_AUDIOSIGNALPROCESSING_MODES, support) &&
        (support & KSPROPERTY_TYPE_GET);
    
    if (!supported)
    {
        if (required)
        {
            LogPinErr("ERROR", "KSPROPERTY_AUDIOSIGNALPROCESSING_MODES is required but not supported");
            fRet = false;
            goto Exit;
        }
        return true; // nothing to test
    }

    // First call returns the value of uOutSize
    if (!PinIoCtl_Get(KSPROPSETID_AudioSignalProcessing, KSPROPERTY_AUDIOSIGNALPROCESSING_MODES,
                     NULL, 0, (void**)&pKsMultiple, uOutSize))
    {
        LogPinErr("FAIL", "Failed to query KSPROPERTY_AUDIOSIGNALPROCESSING_MODES.");
        fRet = false;
        goto Exit;
    }
       
    if (uOutSize < sizeof(KSMULTIPLE_ITEM))
    {
        LogPinErr("FAIL", "KSPROPERTY_AUDIOSIGNALPROCESSING_MODES response is too small", uOutSize);
        fRet = false;
        goto Exit;
    }

    if (0 == pKsMultiple->Count)
    {
        if (category != KSNODETYPE_AUDIO_LOOPBACK)
        {
            LogPinErr("FAIL", "KSPROPERTY_AUDIOSIGNALPROCESSING_MODES response returned a KSMULTIPLE_ITEM.Count = 0");
            fRet = false;
            goto Exit;
        }
    }
    
    SLOG(3,
        "KSPROPERTY_AUDIOSIGNALPROCESSING_MODES response: KSMULTIPLE_ITEM.Size = %d, KSMULTIPLE_ITEM.Count = %d, buffer size = %d",
        pKsMultiple->Size, pKsMultiple->Count, uOutSize
    );

    if ((pKsMultiple->Size != uOutSize )||(pKsMultiple->Size != sizeof(KSMULTIPLE_ITEM) + pKsMultiple->Count * sizeof(GUID)))
    {
        LogPinErr("FAIL", "KSPROPERTY_AUDIOSIGNALPROCESSING_MODES response size does not add up", sizeof(KSMULTIPLE_ITEM) + pKsMultiple->Count * sizeof(KSPROPERTY_AUDIOSIGNALPROCESSING_MODES));
        fRet = false;
        goto Exit;
    }

   pProcessModes = reinterpret_cast<GUID*>(pKsMultiple+1);

   int cCount = pKsMultiple->Count;
   int rawGuidCount = 0;
   int defaultGuidCount = 0;
   int j = 0;

   for (j = 0; j < _countof(SignalProcessingModeInfo); j++)
   {
        SignalProcessingModeInfo[j].iCount = 0;
   }

   for (int i = 0; i < cCount; i++)
   {
       for (j = 0; j < _countof(SignalProcessingModeInfo); j++)
       {
           if (IsEqualGUID(pProcessModes[i], SignalProcessingModeInfo[j].guidType))
           {
               SLOG(3, "%ls is mode %u", SignalProcessingModeInfo[j].wszFriendlyName, i);
               SignalProcessingModeInfo[j].iCount++;
               break;
           }
       }

       if (j >= _countof(SignalProcessingModeInfo))
       {
           LogPinErr("FAIL", FormatStr("Mode %u unknown: " GUID_FORMAT, i, GUID_ARGS(pProcessModes[i])));
       }
   }

   for (j = 0; j < _countof(SignalProcessingModeInfo); j++)
   {
       if (IsEqualGUID(SignalProcessingModeInfo[j].guidType, AUDIO_SIGNALPROCESSINGMODE_DEFAULT))
       {
           defaultGuidCount = SignalProcessingModeInfo[j].iCount;
       }
       else if (IsEqualGUID(SignalProcessingModeInfo[j].guidType, AUDIO_SIGNALPROCESSINGMODE_RAW))
       {
           rawGuidCount = SignalProcessingModeInfo[j].iCount;
       }
   }

   if (rawGuidCount == 0 && defaultGuidCount == 0 && category != KSNODETYPE_AUDIO_LOOPBACK && !(m_pPinFactory->IsAEOffloadPinFactory()))
   {
        LogPinErr("FAIL", "All pins (other than offload and loopback) must support RAW or DEFAULT mode"); 
   }

   for (j = 0; j < _countof(SignalProcessingModeInfo); j++)
   {
       if (SignalProcessingModeInfo[j].iCount > 1)
       {
           LogPinErr("FAIL", "The mode guids should not appear more than once each per pin factory");
       }
       else if (SignalProcessingModeInfo[j].iCount > 0)
       {
           fRet = CreatePin(SignalProcessingModeInfo[j].guidType);
           if (fRet == false)
           {
               LogPinErr("FAIL", FormatStr("Pin creation failed for %ls", SignalProcessingModeInfo[j].wszFriendlyName));
           }
           else
           {
               SLOG(3, "Pin creation successful for %ls ", SignalProcessingModeInfo[j].wszFriendlyName);
           }
       }
   }

Exit:   
   if (pKsMultiple != NULL)
       delete [] pKsMultiple;
   return fRet;
}

//
// Test for KSPROPERTY_PIN_PROPOSEDATAFORMAT2
//
// Following is true:
//  - The support for this property is optional
//
//  If supported:
//      - Must only be supported on KSPIN_COMMUNICATION_SINK pins (pin factories)
//
//  Structure of a call:
//      - Target: Filter Handle
//      - Input Buffer
//          - KSP_PIN followed by a mode GUID
//          - Valid Mode GUIDs
//              AUDIO_SIGNALPROCESSINGMODE_DEFAULT
//              AUDIO_SIGNALPROCESSINGMODE_RAW
//      - Output Buffer
//          - WAVEFORMATEX
//          - KSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE with SignalProcessingMode equal to one provided on input
//
//  Tests:
//  1) Does this pin support this property? (Basic Support)
//  2) Is only GET supported?
//  3) If supported, is this a sink pin?
//  4) Are modes supported on this pin?
//  5a) Is this a capture sink pin? Does GET work for supported modes?
//  5b) Is this is a render sink pin? Are the formats supported the same per mode?
//  6) TO-DO: Does pin instantion work for each format returned for
//     each supported mode?
//  7) TO-DO: Does GET fail for defined but unsupported modes?
//  8) TO-DO: Does GET fail for undefined modes?
//
bool CPinReq::CheckProposeDataformat2()
{
    bool success = true;
    DWORD dwBasicSupportFlag = 0;
    PKSMULTIPLE_ITEM pKsMultiModes = NULL;

    //
    //  Test #1: Does this pin factory support this property? (Basic Support)
    //

    if( !PinIoCtl_Support( KSPROPSETID_Pin,
                          KSPROPERTY_PIN_PROPOSEDATAFORMAT2,
                          dwBasicSupportFlag ) )
    {
        // SLOG( 3, "An error occurred while querying for Basic Support of KSPROPERTY_PIN_PROPOSEDATAFORMAT2" );
        return false;
    }

    if( dwBasicSupportFlag & KSPROPERTY_TYPE_BASICSUPPORT )
    {
        SLOG( 3, "Basic Support for KSPROPERTY_PIN_PROPOSEDATAFORMAT2 is exposed on this pin factory." );
    }
    else if( dwBasicSupportFlag == 0 )
    {
        //SLOG( 3, "KSPROPERTY_PIN_PROPOSEDATAFORMAT2 is not supported on this pin factory.  Skipping Test." );
        return true;
    }
    else
    {
        LogPinErr( "FAIL", FormatStr("Basic Support query of KSPROPERTY_PIN_PROPOSEDATAFORMAT2 returned an unexpected value.  Flags: 0x%08x", dwBasicSupportFlag));
    }

    //
    //  Test #2: Is only GET supported?
    //

    if( dwBasicSupportFlag & KSPROPERTY_TYPE_SET )
    {
        LogPinErr( "FAIL", "KSPROPERTY_TYPE_SET should not be supported for KSPROPERTY_PIN_PROPOSEDATAFORMAT2. Please remove 'SET' support for the property on this pin factory.");  
    }
    else
    {
        SLOG( 3, "KSPROPERTY_TYPE_SET is not supported for KSPROPERTY_PIN_PROPOSEDATAFORMAT2 on this pin factory." );
    }
    
    if( dwBasicSupportFlag & KSPROPERTY_TYPE_GET )
    {
        SLOG( 3, "KSPROPERTY_TYPE_GET is supported for KSPROPERTY_PIN_PROPOSEDATAFORMAT2 on this pin factory." );
    }
    else
    {
        // I can't think of a good reason to require KSPROPERTY_TYPE_GET.
        // It should be fine to advertise that the only verb supported for a given property is "basic support".
        //
        // since KSPROPERTY_TYPE_GET is not supported, there is nothing further to test.
        return true;
    }

    //
    // Test #3: If supported, is this a sink pin?
    //

    if( IsSoftwareStreamingPin() )
    {
        SLOG( 3, "This pin factory is a sink pin." );
    }
    else
    {
        LogPinErr( "FAIL", "This property must only be supported on sink pins. Please remove support for the property for this pin factory." );
        return false;
    }

    //
    // Test #4: Are modes supported on this pin?
    //

    // Validation of the supported modes is not completed in this test case.  This test will assume
    // that the modes supported on this pin are correct.  However, a pin that supports
    // KSPROPERTY_PIN_PROPOSEDATAFORMAT2 *MUST* support modes.  This test case will fail if this is not true.
    if( !QueryForProcessingModes( &pKsMultiModes ) )
    {
        LogPinErr( "FAIL", "Failed to query the supported audio signal processing modes on this pin factory." );
        return false;
    }

    if( pKsMultiModes->Count == 0 )
    {
        LogPinErr( "FAIL", "This pin factory supports KSPROPERTY_PIN_PROPOSEDATAFORMAT2 but does not support KSPROPERTY_AUDIOSIGNALPROCESSING_MODES.  KSPROPERTY_AUDIOSIGNALPROCESSING_MODES must be supported on a pin factory that supports KSPROPERTY_PIN_PROPOSEDATAFORMAT2" );
        success = false;

        goto Exit;
    }

    GUID* pgSignalProcessingMode = (GUID *)(pKsMultiModes + 1);

    //
    // Test 5a: Is this a capture sink pin? Does GET work for supported modes?
    //

    // KSPIN_DATAFLOW_OUT means the audio data is going out of the pin -- so, a capture device
    if( m_pPinFactory->GetDataFlow() == KSPIN_DATAFLOW_OUT )
    {
        SLOG( 3, "This pin factory is a capture sink pin." );

        // for each supported mode, get the propose dataformat
        // the helper function will do the evaluation of the returned "out" buffer
        
        for( ULONG iMode = 0; iMode < pKsMultiModes->Count; iMode++ )
        {
            if( !EvaluateProposeDataformat2ForMode( pgSignalProcessingMode[iMode] ) )
            {
                success = false;

                goto Exit;
            }
        }
    }
    //
    // Test 5b: Is this is a render sink pin? Are the formats supported the same per mode?
    //

    // KSPIN_DATAFLOW_IN means the audio data is going in to the pin -- so, a render device
    else if( m_pPinFactory->GetDataFlow() == KSPIN_DATAFLOW_IN )
    {
        if( !EvaluateProposeDataformat2ForRender( pgSignalProcessingMode, pKsMultiModes->Count ) )
        {
            success = false;

            goto Exit;
        }
    }

Exit:   

    if( pKsMultiModes != NULL )
    {
        delete [] pKsMultiModes;
    }

    return success;
}

typedef struct
{
    KSMULTIPLE_ITEM                         Attributes;
    KSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE  SignalProcessingModeAttribute;
} ProposeDataformat2Input;

bool CPinReq::EvaluateProposeDataformat2ForMode(const GUID& gProcessingMode)
{
    bool success = true;
    UINT uOutbufferSize = 0;
    PKSDATAFORMAT pKsDataformat = NULL;
    PKSDATAFORMAT_WAVEFORMATEX pKsDataformatWaveformatex = NULL;
    ProposeDataformat2Input input = {0};
    PKSMULTIPLE_ITEM pKsMultipleItem_Attribute = NULL;
    PKSATTRIBUTE pKsAttribute = NULL;
    PKSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE pKsAttributeMode = NULL;

    input.Attributes.Count = 1;
    input.Attributes.Size = sizeof(ProposeDataformat2Input);

    input.SignalProcessingModeAttribute.AttributeHeader.Attribute = KSATTRIBUTEID_AUDIOSIGNALPROCESSING_MODE;
    input.SignalProcessingModeAttribute.AttributeHeader.Flags = 0;
    input.SignalProcessingModeAttribute.AttributeHeader.Size = sizeof(KSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE);
    input.SignalProcessingModeAttribute.SignalProcessingMode = gProcessingMode;

    if( !PinIoCtl_Get( KSPROPSETID_Pin,
                       KSPROPERTY_PIN_PROPOSEDATAFORMAT2,
                       &input,
                       sizeof(ProposeDataformat2Input),
                       (void **)&pKsDataformat,
                       uOutbufferSize ) )
    {
        LogPinErr( "FAIL", "Could not GET the KSPROPERTY_PIN_PROPOSEDATAFORMAT2" );

        return false;
    }

    if( uOutbufferSize < sizeof(KSDATAFORMAT_WAVEFORMATEX) )
    {
        LogPinErr( "FAIL", FormatStr("The output buffer size returned from KSPROPERTY_PIN_PROPOSEDATAFORMAT2 is less than the size "
                                     "of a KSDATAFORMAT_WAVEFORMATEX.  Expected at least: %Iu, Actual: %u.",
                                     sizeof(KSDATAFORMAT_WAVEFORMATEX),
                                     uOutbufferSize));
        success = false;

        goto Exit;
    }

    pKsDataformatWaveformatex = (PKSDATAFORMAT_WAVEFORMATEX)(pKsDataformat);

    if( uOutbufferSize < ROUNDUP_8(sizeof(KSDATAFORMAT) + sizeof(WAVEFORMATEX) + pKsDataformatWaveformatex->WaveFormatEx.cbSize) +
                         sizeof(ProposeDataformat2Input) )
    {
        LogPinErr( "FAIL", FormatStr("The output buffer size returned from KSPROPERTY_PIN_PROPOSEDATAFORMAT2 is less than the size of a KSDATAFORMAT, "
                                     "WAVEFORMATEX, KSMULTIPLE_ITEM and KSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE.  Expected at least: %Iu, Actual: %u.",
                                     sizeof(KSDATAFORMAT) +
                                     sizeof(WAVEFORMATEX) + pKsDataformatWaveformatex->WaveFormatEx.cbSize +
                                     sizeof(ProposeDataformat2Input),
                                     uOutbufferSize));
        success = false;

        goto Exit;
    }

    if( pKsDataformatWaveformatex->DataFormat.FormatSize < sizeof(KSDATAFORMAT) )
    {
        LogPinErr( "FAIL", "The KSDATAFORMAT returned from KSPROPERTY_PIN_PROPOSEDATAFORMAT2 is less than sizeof(KSDATAFORMAT)." );
        success = false;

        goto Exit;
    }

    if( (pKsDataformatWaveformatex->DataFormat.Flags & KSDATAFORMAT_ATTRIBUTES) == 0 )
    {
        LogPinErr( "FAIL", "The KSDATAFORMAT Flags field does not assert KSDATAFORMAT_ATTRIBUTES." );
        success = false;

        goto Exit;
    }

    pKsMultipleItem_Attribute = (PKSMULTIPLE_ITEM) (((PBYTE)pKsDataformat) + ROUNDUP_8(pKsDataformat->FormatSize));

    if( pKsMultipleItem_Attribute->Count != 1 )
    {
        LogPinErr( "FAIL", "The KSMULTIPLE_ITEM following the KSDATAFORMAT should have a count of 1." );
        success = false;

        goto Exit;
    }

    if( pKsMultipleItem_Attribute->Size < sizeof(ProposeDataformat2Input) )
    {
        LogPinErr( "FAIL", "The KSMULTIPLE_ITEM following the KSDATAFORMAT should have a size larger or equal to sizeof(KSMULTIPLE_ITEM) and sizeof(KSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE)." );
        success = false;

        goto Exit;
    }

    pKsAttribute = (PKSATTRIBUTE)(pKsMultipleItem_Attribute + 1);

    if( pKsAttribute->Attribute != KSATTRIBUTEID_AUDIOSIGNALPROCESSING_MODE )
    {
        LogPinErr( "FAIL", "Unknown GUID set as KSATTRIBUTE.Attribute." );
        success = false;

        goto Exit;
    }

    if( pKsAttribute->Size != sizeof(KSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE) )
    {
        LogPinErr( "FAIL", FormatStr("Expected the size of the KSATTRIBUTE to be %Iu.  Actual: %u.",
                                 sizeof(KSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE),
                                 pKsAttribute->Size));
        success = false;

        goto Exit;
    }

    pKsAttributeMode = (PKSATTRIBUTE_AUDIOSIGNALPROCESSING_MODE)pKsAttribute;
    if( !IsEqualGUID( pKsAttributeMode->SignalProcessingMode, gProcessingMode ) )
    {
        LogPinErr( "FAIL", "The Processing Mode GUID following the KSDATAFORMAT returned from KSPROPERTY_PIN_PROPOSEDATAFORMAT2 does not match the GUID provided for the input buffer." );
        success = false;

        goto Exit;
    }

Exit:   

    if( pKsDataformat != NULL )
    {
        delete [] pKsDataformat;
    }

    return success;
}

bool CPinReq::EvaluateProposeDataformat2ForRender(const GUID* garrayProcessingMode, UINT cModes)
{
    bool success = true;

    // first make sure the formats make sense
    for( ULONG iMode = 0; iMode < cModes; iMode++ )
    {
        if( !EvaluateProposeDataformat2ForMode( garrayProcessingMode[iMode] ) )
        {
            success = false;
        }
    }

    if( cModes > 1 )
    {
        // To-Do: compare the formats returned for all the supported modes and make sure they are the same
        LogPinErr( "FAIL", "This Render pin factory supports more than one mode." );
    }

    return success;
}

bool CPinReq::CreatePin(GUID mode)
{
   HRESULT hr = S_OK;
   CPinInstance *pCPinInstance = NULL;
   bool fRet = false;

#ifndef BUILD_TAEF
    CComPtr<IBasicLog> spBasicLog;
    hr = g_IShell->GetBasicLog( &spBasicLog );
    if (FAILED(hr)) {
        LogPinErr("ERROR", "Log Failed to be created");        
    }
#else
    CComPtr<IBasicLog> spBasicLog;
    hr = CreateWexBasicLog(&spBasicLog);
    if (FAILED(hr)) {
        LogPinErr("ERROR", "Log Failed to be created");        
    }
#endif

   pCPinInstance = new CPinInstance( m_pPinFactory, spBasicLog );
   if( NULL != pCPinInstance )
   {       
       pCPinInstance->SetSignalProcessingMode(mode);
       hr = pCPinInstance->CreatePin();
       if (hr !=S_OK)
       {
           SLOG(3, "pCPinInstance->CreatePin() failed with 0x%08x.", hr );
           fRet =  false;
       }
       else
       {
           SLOG(3, "pCPinInstance->CreatePin() succeeded");
           fRet = true;
       }
    }   
   delete pCPinInstance;
   return fRet;
}

bool CPinReq::IsHardwareBridgePin()
{
    // return false if the pin is connected to another filter
    // because then it's not the endpoint
    if (0 != strlen(m_pPinFactory->GetPhysicalConnectionLink()))
    {
        return false;
    }
    
    // audio paths have two endpoints; the streaming pin and the bridge pin
    // the endpoints can be told apart by their KSPROPERTY_PIN_COMMUNICATION value:
    // -- streaming pins --
    // KSPIN_COMMUNICATION_SINK (render pin)
    // KSPIN_COMMUNICATION_SOURCE (capture pin)
    // KSPIN_COMMUNICATION_BOTH
    //
    // -- bridge pins --
    // KSPIN_COMMUNICATION_NONE
    // KSPIN_COMMUNICATION_BRIDGE
    return
        (KSPIN_COMMUNICATION_NONE   == m_pPinFactory->GetCommunication()) ||
        (KSPIN_COMMUNICATION_BRIDGE == m_pPinFactory->GetCommunication());
}

//
// Check that each pin that:
//  1. Has a KSPIN_COMMUNICATION_NONE or KSPIN_COMMUNICATION_BRIDGE attribute
//  2. Corresponds to an audio endpoint
//  3. The audio endpoint state is not DEVICE_STATE_NOTPRESENT 
//  supports the property KSPROPERTY_JACK_DESCRIPTION
//
bool CPinReq::CheckJackDescHelper(void *pArg)
{
    CPinReq *pThis = static_cast<CPinReq*>(pArg);
    return pThis->CheckJackPropHelper(JACKTEST_JACKDESC);
}

//
// Check that each pin that:
//  1. Has a KSPIN_COMMUNICATION_NONE or KSPIN_COMMUNICATION_BRIDGE attribute
//  2. Corresponds to an audio endpoint
//  3. The audio endpoint state is not DEVICE_STATE_NOTPRESENT 
//  supports the property KSPROPERTY_JACK_DESCRIPTION2
//
bool CPinReq::CheckJackDesc2Helper(void *pArg)
{
    CPinReq *pThis = static_cast<CPinReq*>(pArg);
    return pThis->CheckJackPropHelper(JACKTEST_JACKDESC2);
}

//
// Check that each pin that:
//  1. Has a KSPIN_COMMUNICATION_NONE or KSPIN_COMMUNICATION_BRIDGE attribute
//  2. Corresponds to an audio endpoint
//  3. The audio endpoint state is not DEVICE_STATE_NOTPRESENT 
//  4. Is an HDMI device
//  supports the property KSJACK_SINK_INFORMATION
//
bool CPinReq::CheckJackSinkInfoHelper(void *pArg)
{
    CPinReq *pThis = static_cast<CPinReq*>(pArg);
    return pThis->CheckJackPropHelper(JACKTEST_SINKINFO);
}


//
// Check that each pin that:
//  1. Has a KSPIN_COMMUNICATION_NONE or KSPIN_COMMUNICATION_BRIDGE attribute
//  2. Corresponds to an audio endpoint
//  3. The audio endpoint state is not DEVICE_STATE_NOTPRESENT 
//  supports the property KSPROPERTY_JACK_DESCRIPTION or KSPROPERTY_JACK_DESCRIPTION2
//  4. Is an HDMI device
//  supports the property KSJACK_SINK_INFORMATION
//
bool CPinReq::CheckJackPropHelper(UINT JackPropType)
{
    DWORD               dwState = 0;
    CComPtr<IMMDevice>  pmmDeviceInterface = NULL,
                        pmmDevice = NULL;

    // Test can only have arg values less than JACKTEST_NUMTESTS
    if(JackPropType >= JACKTEST_NUMTESTS)  {
        LogPinErr("ERROR", "Out of range JackPropType", JackPropType);
        return false;
    }

    // 1. Check that this is a hardware bridge pin
    if (!IsHardwareBridgePin()) { return true; } // nothing to test

    // 2. Check that pin corresponds to an audio endpoint
    HRESULT hr = DevTrans::DevicePathToInterfaceA(m_pFilter->GetDevicePath(), &pmmDeviceInterface);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "ERROR: Unable to get Interface from DevicePath %s", 
                            m_pFilter->GetDevicePath());
        return false;
    }

    hr = DevTrans::KSEndpointPinToEndpoint(pmmDeviceInterface, m_iPin, &pmmDevice);
    if (E_NOTFOUND == hr)
    {
        // nothing to test
        return true;
    }
    else if (FAILED(hr))
    {
        XLOG(XFAIL, eError, "KSEndpointPinToEndpoint returned unexpected value 0x%X", hr);
        return false;
    }

    // 3. The audio endpoint state is not DEVICE_STATE_NOTPRESENT
    hr = pmmDevice->GetState(&dwState);
    if(FAILED(hr))
    {
        XLOG(XFAIL, eError, "ERROR: Unable to get state for device.");
        return false;
    }
 
    if(DEVICE_STATE_NOTPRESENT == dwState)
    {
        // nothing to test
        return true;
    }
    
    EndpointFormFactor formFactor;
    EDataFlow flow;
    hr = GetEndpointFormFactorAndFlow(pmmDevice, &formFactor, &flow);
    if(FAILED(hr))
    {
        LogPinErr("ERROR", "Unable to get form factor and flow for pin");
        return false;
    }

    // Ks Pin maps to an audio endpoint
    // Pin must support KSPROPERTY_JACK_DESCRIPTION
    // and KSPROPERTY_JACK_DESCRIPTION2
    switch(JackPropType)
    {
        case JACKTEST_JACKDESC:
            return IsJackDescSupported(formFactor, flow);
        case JACKTEST_JACKDESC2:
            return IsJackDesc2Supported();
        case JACKTEST_SINKINFO:
            GUID category = *m_pPinFactory->GetCategory();
            
            if(
                DigitalAudioDisplayDevice == formFactor ||
                KSNODETYPE_HDMI_INTERFACE == category ||
                KSNODETYPE_DISPLAYPORT_INTERFACE == category
            )
            {
                return IsJackSinkInfoSupported();
            }
            else
            {
                return true;
            }
    }
    
    return true;
}

bool CPinReq::CheckMicArrayGeometryHelper(void *pArg)
{
    CPinReq *pThis = static_cast<CPinReq*>(pArg);
    return pThis->CheckMicArrayGeometryOnPin();
}

#define CASE_RETURN(x) case x: return L ## #x

LPCWSTR StringFromMicrophoneType(USHORT type)
{
    switch (type)
    {
        CASE_RETURN(KSMICARRAY_MICTYPE_OMNIDIRECTIONAL);
        CASE_RETURN(KSMICARRAY_MICTYPE_SUBCARDIOID);
        CASE_RETURN(KSMICARRAY_MICTYPE_CARDIOID);
        CASE_RETURN(KSMICARRAY_MICTYPE_SUPERCARDIOID);
        CASE_RETURN(KSMICARRAY_MICTYPE_HYPERCARDIOID);
        CASE_RETURN(KSMICARRAY_MICTYPE_8SHAPED);
        
        default: return L"Unrecognized";
    }
}

LPCWSTR StringFromMicrophoneArrayType(USHORT type)
{
    switch (type)
    {
        CASE_RETURN(KSMICARRAY_MICARRAYTYPE_LINEAR);
        CASE_RETURN(KSMICARRAY_MICARRAYTYPE_PLANAR);
        CASE_RETURN(KSMICARRAY_MICARRAYTYPE_3D);
        
        default: return L"Unrecognized";
    }
}

bool CPinReq::CheckMicArrayGeometryOnPin()
{
    GUID category = *m_pPinFactory->GetCategory();
    ThreeDSpace threeD;
    UINT32 geometryErrorFlags = 0;
    UINT32 numElementsWithErrors = 0;
    MicArrayElementErrorDetails* pElementErrors = nullptr;

    bool required =
        IsHardwareBridgePin() &&
        (
            KSNODETYPE_MICROPHONE_ARRAY == category ||
            KSNODETYPE_PROCESSING_MICROPHONE_ARRAY == category
        );

    DWORD support = 0;
    
    bool supported =
        PinIoCtl_Support(KSPROPSETID_Audio, KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY, support) &&
        (support & KSPROPERTY_TYPE_GET);
    
    if (!supported)
    {
        if (required)
        {
            LogPinErr("ERROR", "KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY required but not supported");
            return false;
        }
        
        return true; // nothing to test
    }

    KSAUDIO_MIC_ARRAY_GEOMETRY *geometry = nullptr;
    UINT size = 0;
    
    if(!PinIoCtl_Get(
        KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY,
        NULL, 0,
        (void**)&geometry, size))
    {
        LogPinErr("ERROR", "KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY get request failed");
        return false;
    }
    
    LogPinMsg(eInfo1, FormatStr("KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY size is %u", size));

    bool pass = true;

    // validate the output size against KSAUDIO_MIC_ARRAY_GEOMETRY.usNumberOfMicrophones
    if (size < sizeof(*geometry))
    {
        LogPinErr("ERROR", "KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY is too small");
        pass = false;
        goto Exit; // can't continue
    }
    
    IsValidMicArrayGeometry(geometry, nullptr, nullptr, true, &geometryErrorFlags, &numElementsWithErrors, &pElementErrors);

    // Checked separately from the other errors so that the pointer math below is safe
    if (geometryErrorFlags & MIC_ARRAY_GEOMETRY_ERROR_FLAG_ZERO_MICS)
    {
        LogPinErr("ERROR", "KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY has no microphones");
        pass = false;
    }
    else if (size != sizeof(*geometry) + (geometry->usNumberOfMicrophones - 1) * sizeof(geometry->KsMicCoord[0]))
    {
        LogPinErr("ERROR", FormatStr("KSPROPERTY_AUDIO_MIC_ARRAY_GEOMETRY size (%u) does not match usNumberOfMicrophones (%u)", size, geometry->usNumberOfMicrophones));
        pass = false;
        goto Exit; // can't continue
    }

    // log the value of the mic array geometry
    LogPinMsg(eInfo1, FormatStr(
        "-- Microphone array geometry --\n"
        "        version: %u.%u\n"
        "        type: %hu (%ls)\n"
        "        vertical angle:\n"
        "            %hd to %hd in mic array angle units\n"
        "            %g to %g in radians\n"
        "            %g to %g in degrees\n"
        "        horizontal angle:\n"
        "            %hd to %hd in mic array angle units\n"
        "            %g to %g in radians\n"
        "            %g to %g in degrees\n"
        "        frequency band: %hu to %hu Hz\n"
        "        microphones: %hu"
        ,
        geometry->usVersion / 256, geometry->usVersion % 256,
        geometry->usMicArrayType, StringFromMicrophoneArrayType(geometry->usMicArrayType),
        geometry->wVerticalAngleBegin, geometry->wVerticalAngleEnd,
            RadiansFromMicArrayAngle(geometry->wVerticalAngleBegin), RadiansFromMicArrayAngle(geometry->wVerticalAngleEnd),
            DegreesFromMicArrayAngle(geometry->wVerticalAngleBegin), DegreesFromMicArrayAngle(geometry->wVerticalAngleEnd),
        geometry->wHorizontalAngleBegin, geometry->wHorizontalAngleEnd,
            RadiansFromMicArrayAngle(geometry->wHorizontalAngleBegin), RadiansFromMicArrayAngle(geometry->wHorizontalAngleEnd),
            DegreesFromMicArrayAngle(geometry->wHorizontalAngleBegin), DegreesFromMicArrayAngle(geometry->wHorizontalAngleEnd),
        geometry->usFrequencyBandLo, geometry->usFrequencyBandHi,
        geometry->usNumberOfMicrophones
    ));

    for (USHORT i = 0; i < geometry->usNumberOfMicrophones; i++)
    {
        KSAUDIO_MICROPHONE_COORDINATES *c = &geometry->KsMicCoord[i];
        
        XLOG(XMSG, eInfo1,
            "        :: microphone %hu of %hu ::\n"
            "            type: %hu (%ls)\n" 
            "            (x, y, z) = (%hd, %hd, %hd) millimeters\n"
            "            vertical angle:\n"
            "                %hd in mic array angle units\n"
            "                %g in radians\n"
            "                %g in degrees\n"
            "            horizontal angle:\n"
            "                %hd in mic array angle units\n"
            "                %g in radians\n"
            "                %g in degrees"
            ,
            i + 1, geometry->usNumberOfMicrophones,
            c->usType, StringFromMicrophoneType(c->usType),
            c->wXCoord, c->wYCoord, c->wZCoord,
            c->wVerticalAngle, 
                RadiansFromMicArrayAngle(c->wVerticalAngle),
                DegreesFromMicArrayAngle(c->wVerticalAngle),
            c->wHorizontalAngle, 
                RadiansFromMicArrayAngle(c->wHorizontalAngle),
                DegreesFromMicArrayAngle(c->wHorizontalAngle)
        );
    }
    
    // We already handled zero mics, and HLK doesn't care about inconsistent, > max,
    // or unsupported vendor-defined elements errors, so remove those from consideration
    UINT32 errorFlagsToIgnore = (
        MIC_ARRAY_GEOMETRY_ERROR_FLAG_ZERO_MICS |
        MIC_ARRAY_GEOMETRY_ERROR_FLAG_INCONSISTENT_NUM_MIC |
        MIC_ARRAY_GEOMETRY_ERROR_FLAG_EXCEEDS_MAX_NUM_MIC |
        MIC_ARRAY_GEOMETRY_ERROR_FLAG_MICTYPE_VENDORDEFINED_UNSUPPORTED);
    geometryErrorFlags = geometryErrorFlags & ~errorFlagsToIgnore;

    // Iterate through the error flags and print out appropriate messages
    LPCSTR errorMessage;
    for (UINT32 currentError = 0x01; currentError < MIC_ARRAY_GEOMETRY_ERROR_FLAG_MAX_VALUE; currentError <<= 1)
    {
        if(currentError & geometryErrorFlags)
        {
            GetHumanReadableArrayError((MicArrayGeometryErrorFlagMask)currentError, &errorMessage);
            LogPinErr("ERROR", errorMessage);
            pass = false;
        }
    }
 
    // print specific 
    for (USHORT i = 0; i < numElementsWithErrors; i++)
    {
        MicArrayElementErrorDetails elementError = pElementErrors[i];
        for (UINT32 currentError = 0x01; currentError < MIC_ARRAY_GEOMETRY_ELEMENT_ERROR_FLAG_MAX_VALUE; currentError <<= 1)
        {
            if(currentError & elementError.uElementErrorFlags)
            {
                if(MIC_ARRAY_GEOMETRY_ELEMENT_ERROR_FLAG_DUPLICATE_COORDINATE != currentError)
                {
                    GetHumanReadableElementError((MicArrayElementErrorFlagMask)currentError, &errorMessage);
                    LogPinErr("ERROR", FormatStr("Error on element %u: %s", elementError.uElementIndex, errorMessage));
                }
                else
                {
                    LogPinErr("ERROR", FormatStr("Error on element %u: duplicate of element %u", elementError.uElementIndex, elementError.uErrorParameter));
                }
                pass = false;
            }
        }
    }
    
Exit:
    delete [] geometry;
    free(pElementErrors);
    return pass;
}

bool CPinReq::CheckMicrophoneSensitivityHelper(void *pArg)
{
    CPinReq *pThis = static_cast<CPinReq*>(pArg);
    return pThis->CheckMicrophoneSensitivity();
}

bool CPinReq::CheckMicrophoneSensitivity()
{
    GUID category;
    DWORD support = 0;
    bool pass = true;

    category = *(m_pPinFactory->GetCategory());

    // Conditions for the test:
    // 1. It is an optional property valid only on capture pin.
    // 2. If implemented, must be within the range -130 dBFS < Senstitivity < 0 dBFS

    bool supported =
        PinIoCtl_Support(KSPROPSETID_Audio, KSPROPERTY_AUDIO_MIC_SENSITIVITY, support) &&
        (support & KSPROPERTY_TYPE_GET);

    if (!supported)
    {
        pass = true;
        goto Exit;     // nothing to test
    }

    if (m_pPinFactory->GetDataFlow() == KSPIN_DATAFLOW_OUT || category == KSNODETYPE_AUDIO_LOOPBACK)
    {
        LogPinErr("ERROR", "KSPROPERTY_AUDIO_MIC_SENSITIVITY is only valid on capture sink pin.");
        pass = false;
        goto Exit;
    }

    long* plVal = 0;
    UINT size = 0;
    double dblMicSensitivity = 0;

    if (!PinIoCtl_Get(
        KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_MIC_SENSITIVITY,
        NULL, 0,
        (void**)&plVal, size))
    {
        LogPinErr("ERROR", "KSPROPERTY_AUDIO_MIC_SENSITIVITY get request failed.");
        pass = false;
        goto Exit;
    }

    // validate the output size
    if (size < sizeof(*plVal))
    {
        LogPinErr("ERROR", FormatStr("KSPROPERTY_AUDIO_MIC_SENSITIVITY data size is too small. Expected = %u, Actual = %u", sizeof(*plVal), size));
        pass = false;
        goto Exit;
    }

    dblMicSensitivity = FixedPoint16_16ToDouble(*plVal);
    LogPinMsg(eInfo1, FormatStr("Microphone Sensitivity = %f", dblMicSensitivity));

    if (dblMicSensitivity <= -130 || dblMicSensitivity >= 0)
    {
        LogPinErr("ERROR", FormatStr("Microphone Sensitivity = %f is not within the range -130 dBFS < MicrophoneSenstitivity < 0 dBFS.", dblMicSensitivity));
        pass = false;
        goto Exit;
    }

Exit:
    return pass;
}

bool CPinReq::CheckMicrophoneSNRHelper(void *pArg)
{
    CPinReq *pThis = static_cast<CPinReq*>(pArg);
    return pThis->CheckMicrophoneSNR();
}

bool CPinReq::CheckMicrophoneSNR()
{
    GUID category;
    DWORD support = 0;
    bool pass = true;

    category = *(m_pPinFactory->GetCategory());

    // Conditions for the test:
    // 1. It is an optional property valid only on capture pin.
    // 2. If implemented, must be within the range 0 dB < SNR < 130 dB

    bool supported =
        PinIoCtl_Support(KSPROPSETID_Audio, KSPROPERTY_AUDIO_MIC_SNR, support) &&
        (support & KSPROPERTY_TYPE_GET);

    if (!supported)
    {
        pass = true;
        goto Exit;     // nothing to test
    }

    if (m_pPinFactory->GetDataFlow() == KSPIN_DATAFLOW_OUT || category == KSNODETYPE_AUDIO_LOOPBACK)
    {
        LogPinErr("ERROR", "KSPROPERTY_AUDIO_MIC_SNR is only valid on capture sink pin.");
        pass = false;
        goto Exit;
    }

    long* plVal = 0;
    UINT size = 0;
    double dblMicSNR = 0;

    if (!PinIoCtl_Get(
        KSPROPSETID_Audio,
        KSPROPERTY_AUDIO_MIC_SNR,
        NULL, 0,
        (void**)&plVal, size))
    {
        LogPinErr("ERROR", "KSPROPERTY_AUDIO_MIC_SNR get request failed.");
        pass = false;
        goto Exit;
    }

    // validate the output size
    if (size < sizeof(*plVal))
    {
        LogPinErr("ERROR", FormatStr("KSPROPERTY_AUDIO_MIC_SNR data size is too small. Expected = %u, Actual = %u", sizeof(*plVal), size));
        pass = false;
        goto Exit;
    }

    dblMicSNR = FixedPoint16_16ToDouble(*plVal);
    LogPinMsg(eInfo1, FormatStr("Microphone SNR = %f", dblMicSNR));

    if (dblMicSNR <= 0 || dblMicSNR >= 130 )
    {
        LogPinErr("ERROR", FormatStr("Microphone SNR = %f is not within the range 0 dB < MicrophoneSNR < 130 dB.", dblMicSNR));
        pass = false;
        goto Exit;
    }

Exit:
    return pass;
}

HRESULT CPinReq::GetEndpointFormFactorAndFlow(
    _In_ IMMDevice *pMMDevice,
    _Out_ EndpointFormFactor *pFormFactor,
    _Out_ EDataFlow *pFlow
)
{
    HRESULT                 hr = S_OK;
    CComQIPtr<IMMEndpoint>  pMMEndpoint;
    CComPtr<IPropertyStore> pPropStore = NULL;
    PROPVARIANT             PV_prop;

    // Open PropertyStore
    hr = pMMDevice->OpenPropertyStore(STGM_READ, &pPropStore);
    if (FAILED(hr)) {
        LogPinErr("ERROR", "Unable to open property store", hr);
        return hr;
    }

    // Get form factor
    PropVariantInit(&PV_prop);
    pPropStore->GetValue(PKEY_AudioEndpoint_FormFactor, &PV_prop);
    if(VT_UI4 != PV_prop.vt) 
    { 
        LogPinErr("ERROR", "Unexpected variant type for FormFactor property", PV_prop.vt);
        PropVariantClear(&PV_prop);
        return E_NOTFOUND; 
    }
    *pFormFactor = (EndpointFormFactor)PV_prop.uintVal;
    PropVariantClear(&PV_prop);
    
    // Get flow
    pMMEndpoint = pMMDevice;
    if (nullptr == pMMEndpoint) {
        LogPinErr("ERROR", "Could not QI IMMDevice to IMMEndpoint");
        return E_NOINTERFACE;
    }
    hr = pMMEndpoint->GetDataFlow(pFlow);
    if (FAILED(hr)) {
        LogPinErr("ERROR", "Unable to get data flow", hr);
        return hr;
    }
    
    return S_OK;
}

// KSJACK_DESCRIPTION.Color uses a different packing
// than does a GDI color
#define JACKDESC_RGB(jack_r, jack_g, jack_b) RGB(jack_b, jack_g, jack_r)
#define JackDesc_GetRValue(rgb) GetBValue(rgb)
#define JackDesc_GetGValue(rgb) GetGValue(rgb)
#define JackDesc_GetBValue(rgb) GetRValue(rgb)

COLORREF JackDescriptionColor_From_GdiColor(COLORREF gdi)
{
    return
        JACKDESC_RGB(
            GetRValue(gdi),
            GetGValue(gdi),
            GetBValue(gdi)
        );
}

COLORREF GdiColor_From_JackDescriptionColor(COLORREF jack)
{
    return
        RGB(
            JackDesc_GetRValue(jack),
            JackDesc_GetGValue(jack),
            JackDesc_GetBValue(jack)
        );
}

// consider two colors j1 and j2 a match
// if |j1.red - j2.red| <= 5
// and |j1.green - j2.green| <= 5
// and |j1.blue - j2.blue| <= 5
bool JackDescriptionColor_Match(COLORREF jack1, COLORREF jack2)
{
    BYTE r1 = JackDesc_GetRValue(jack1);
    BYTE r2 = JackDesc_GetRValue(jack2);
    
    if (r1 > r2 && r1 - r2 > 5) { return false; }
    if (r2 > r1 && r2 - r1 > 5) { return false; }

    BYTE g1 = JackDesc_GetGValue(jack1);
    BYTE g2 = JackDesc_GetGValue(jack2);
    
    if (g1 > g2 && g1 - g2 > 5) { return false; }
    if (g2 > g1 && g2 - g1 > 5) { return false; }
    
    BYTE b1 = JackDesc_GetBValue(jack1);
    BYTE b2 = JackDesc_GetBValue(jack2);
    
    if (b1 > b2 && b1 - b2 > 5) { return false; }
    if (b2 > b1 && b2 - b1 > 5) { return false; }

    return true;
}

struct WellKnownColor
{
    LPCWSTR name;
    EDataFlow flow;
    EndpointFormFactor formFactor;
    COLORREF colorGDI;
    COLORREF colorJackDescription;
    
    WellKnownColor(
        LPCWSTR n,
        EDataFlow f,
        EndpointFormFactor ff,
        COLORREF gdi
    )
    : name(n)
    , flow(f)
    , formFactor(ff)
    , colorGDI(gdi)
    , colorJackDescription(JackDescriptionColor_From_GdiColor(gdi))
    {}
};

WellKnownColor g_WellKnownColors[] =
{
    // PC '99 specification section 3.18.3
    // http://www.microsoft.com/whdc/archive/pcguides.mspx
    //
    // Pantone colors converted to RGB using
    // http://goffgrafix.com/pantone-rgb-100.php
    //
    // I also include naive approximations of these colors
    // in particular:
    //
    // Recommended Color Codes for Connectors
    //
    // Connector               Recommended color        Pantone
    // --------------------------------------------------------
    // Audio line in           Light blue               284C
    // Audio line out          Lime                     577C
    // Microphone              Pink                     701C
    // Speaker out/subwoofer   Orange                   157C
    // Right-to-left speaker   Brown                    4645C

    WellKnownColor(L"Pantone 284C",  eCapture, LineLevel,  RGB(117, 170, 219)),
    
    WellKnownColor(L"Pantone 577C",  eRender,  LineLevel,  RGB(181, 204, 142)),
    WellKnownColor(L"Pantone 577C",  eRender,  Speakers,   RGB(181, 204, 142)),
    WellKnownColor(L"Pantone 577C",  eRender,  Headphones, RGB(181, 204, 142)),
    WellKnownColor(L"Pantone 577C",  eRender,  Headset,    RGB(181, 204, 142)),
    WellKnownColor(L"Pantone 577C",  eCapture, Headset,    RGB(181, 204, 142)), // uses the same jack
    
    WellKnownColor(L"Pantone 701C",  eCapture, Microphone, RGB(232, 135, 142)),
    
    WellKnownColor(L"Pantone 157C",  eRender,  Speakers,   RGB(237, 160,  79)),

    WellKnownColor(L"Pantone 4645C", eRender,  Speakers,   RGB(178, 130,  96)),
    
    // David Roach's book High Definition Audio for the Digital Home
    // has a table 2.1 in chapter 2 "Intel HD Audio Jack Color Coding"
    // which contains RGB values for the Pantone colors:
    // Function         Pantone color       RGB
    // ------------------------------------------
    // Line In          284C                (122, 171, 222)
    // Front Out        577C                (179, 201, 140)
    // Microphone In    701C                (232, 140, 153)
    // Side surround    420C                (209, 204, 196)
    // Rear surround    Black               ( 43,  41,  38)
    // Center + LFE     157C                (232, 158,  71)    
    WellKnownColor(L"Pantone 284C",  eCapture, LineLevel,  RGB(122, 171, 222)),
    
    WellKnownColor(L"Pantone 577C",  eRender,  LineLevel,  RGB(179, 201, 140)),
    WellKnownColor(L"Pantone 577C",  eRender,  Speakers,   RGB(179, 201, 140)),
    WellKnownColor(L"Pantone 577C",  eRender,  Headphones, RGB(179, 201, 140)),
    WellKnownColor(L"Pantone 577C",  eRender,  Headset,    RGB(179, 201, 140)),
    WellKnownColor(L"Pantone 577C",  eCapture, Headset,    RGB(179, 201, 140)), // uses the same jack
    
    WellKnownColor(L"Pantone 701C",  eCapture, Microphone, RGB(232, 140, 153)),
    
    WellKnownColor(L"Pantone 420C",  eRender,  Speakers,   RGB(209, 204, 196)),

    // intentionally omitting black
    
    WellKnownColor(L"Pantone 157C",  eRender,  Speakers,   RGB(232, 158,  71)),

    // hdaudio.sys' naive approximations using round RGB values
    //
    // Line In    Blue  RGB(  0,   0, 255)
    // Line Out   Green RGB(  0, 255,   0)
    // Microphone Pink  RGB(255, 128, 192)
    WellKnownColor(L"Blue",          eCapture, LineLevel,  RGB(  0,   0, 255)),
    
    WellKnownColor(L"Green",         eRender,  LineLevel,  RGB(  0, 255,   0)),
    WellKnownColor(L"Green",         eRender,  Speakers,   RGB(  0, 255,   0)),
    WellKnownColor(L"Green",         eRender,  Headphones, RGB(  0, 255,   0)),
    WellKnownColor(L"Green",         eRender,  Headset,    RGB(  0, 255,   0)),
    WellKnownColor(L"Green",         eCapture, Headset,    RGB(  0, 255,   0)), // uses the same jack

    WellKnownColor(L"Pink",          eCapture, Microphone, RGB(255, 128, 192)),
};

bool CPinReq::IsJackDescSupported(EndpointFormFactor formFactor, EDataFlow flow)
{
    bool                fRet = true;
    KSMULTIPLE_ITEM    *pksmiPropVal = NULL;
    UINT                uOutSize = 0;
    KSJACK_DESCRIPTION *pKsJackDesc = NULL;
    ULONG               cKsJackDesc = 0;

    // Test does not apply to USB Audio 1.0 devices
    bool fIsUSBAudio10Device = false;
    HRESULT hr = IsUSBAudio10Device(fIsUSBAudio10Device);
    if (FAILED(hr))
    {
        LogPinErr("FAIL", "Failed to query for whether device is USB Audio 1.0 device");
        fRet = false;
        goto Exit;
    }
    
    if(!PinIoCtl_Get(KSPROPSETID_Jack, KSPROPERTY_JACK_DESCRIPTION,
                     NULL, 0, (void**)&pksmiPropVal, uOutSize))
    {
        if (fIsUSBAudio10Device)
        {
            LogPinSkip("KSPROPERTY_JACK_DESCRIPTION is not required to be supported on USB Audio 1.0 Devices");
            fRet = true;
            goto Exit;
        }
        else
        {
            LogPinErr("FAIL", "Failed to query KSPROPERTY_JACK_DESCRIPTION.");
            fRet = false;
            goto Exit;
        }
    }

    // KSPROPERTY_JACK_DESCRIPTION is a KSMULTIPLE_ITEM plus a bunch of KSJACK_DESCRIPTIONs
    // the output size is reported both as the size of the buffer and as KSMULTIPLE_ITEM.Size
    // this should be sizeof(KSMULTIPLE_ITEM) + KSMULTIPLE_ITEM.Count * sizeof(KSJACK_DESCRIPTION)
    //
    // +--------------------- KSMULTIPLE_ITEM ---------------------+
    // | Size = sizeof(KSMULTIPLE_ITEM) +                          |
    // |        KSMULTIPLE_ITEM.Count * sizeof(KSJACK_DESCRIPTION) |
    // | Count = N                                                 |
    // +----------------- first KSJACK_DESCRIPTION ----------------+
    // | ChannelMapping = ...                                      |
    // | Color = ...                                               |
    // | ...                                                       |
    // +----------------- second KSJACK_DESCRIPTION ---------------+
    // | ChannelMapping = ...                                      |
    // | Color = ...                                               |
    // | ...                                                       |
    // +---------------------------- ... --------------------------+
    // |                             ...                           |
    // +------------------- Nth KSJACK_DESCRIPTION ----------------+
    // | ChannelMapping = ...                                      |
    // | Color = ...                                               |
    // | ...                                                       |
    // +-----------------------------------------------------------+
    //
    // first make sure we actually got a full KSMULTIPLE_ITEM
    // this is necessary before we can read pksmiPropVal->Count and pksmiPropVal->Size
    if (uOutSize < sizeof(*pksmiPropVal))
    {
        LogPinErr("FAIL", "KSPROPERTY_JACK_DESCRIPTION returned less than sizeof(KSMULTIPLE_ITEM)", uOutSize);
        fRet = false;
        goto Exit;
    }
    
    if (
        uOutSize != sizeof(*pksmiPropVal) + pksmiPropVal->Count * sizeof(KSJACK_DESCRIPTION) ||
        uOutSize != pksmiPropVal->Size
    )
    {
        XLOG(XMSG, eInfo1,
            "  KSMULTIPLEITEM.Count is %u; KSMULTIPLEITEM.Size is %u; output size is %u",
            pksmiPropVal->Count, pksmiPropVal->Size, uOutSize
        );
        LogPinErr("FAIL", "KSPROPERTY_JACK_DESCRIPTION KSMULTIPLE_ITEM.Count is not consistent with returned property size");
        fRet = false;
        goto Exit;
    }
    
    cKsJackDesc = pksmiPropVal->Count;
    // the first element of the jack description array is just beyond the KSMULTIPLE_ITEM
    pKsJackDesc = (KSJACK_DESCRIPTION*)(pksmiPropVal + 1);

    // there should be at least one jack
    if (0 == cKsJackDesc)
    {
        LogPinErr("FAIL", "KSPROPERTY_JACK_DESCRIPTION has zero elements");
        fRet = false;
        goto Exit;
    }
    
    // test channel mapping
    DWORD dwOverallChannelMask = 0;
    for(UINT ii=0; ii < cKsJackDesc; ++ii)
    {
        XLOG(XMSG, eInfo1, "Channel mask on jack %u is 0x%x", ii, pKsJackDesc[ii].ChannelMapping);
        dwOverallChannelMask |= pKsJackDesc[ii].ChannelMapping;
    }
    
    // the union of the channel mapping should be a well-known format
    // e.g. all-zeros, mono, stereo, quad, 5.1 or 7.1
    switch(dwOverallChannelMask)
    {
        // well-known channel masks
        case KSAUDIO_SPEAKER_DIRECTOUT: // all zeros
        case KSAUDIO_SPEAKER_MONO:
        case KSAUDIO_SPEAKER_STEREO:
        case KSAUDIO_SPEAKER_QUAD: // four corners
        case KSAUDIO_SPEAKER_SURROUND: // three in the front, one in the back
        case KSAUDIO_SPEAKER_5POINT1_SURROUND:
        case KSAUDIO_SPEAKER_7POINT1_SURROUND:
            break;

        // obsolete
        case KSAUDIO_SPEAKER_5POINT1_BACK: // AKA KSAUDIO_SPEAKER_5POINT1
        case KSAUDIO_SPEAKER_7POINT1_WIDE: // AKA KSAUDIO_SPEAKER_7POINT1
            LogPinErr("FAIL", "Incorrect 5.1 or 7.1 channel mask: use side speakers");
            break;

        // unrecognized
        default:
            LogPinErr("FAIL", "Union of channel masks is unrecognized");
            break;
    }

    if(dwOverallChannelMask && cKsJackDesc >= 2)
    {
        // validate that each audio channel is assigned to only one jack
        //
        // for example, the following are valid:
        //
        // HDMI
        //     1. KSAUDIO_SPEAKER_5POINT1_SURROUND (all channels on a single jack)
        //
        // 5.1 analog output using stereo jacks
        //     1. SPEAKER_FRONT_LEFT | SPEAKER_FRONT_RIGHT
        //     2. SPEAKER_BACK_LEFT | SPEAKER_BACK_RIGHT
        //     3. SPEAKER_FRONT_CENTER | SPEAKER_LOW_FREQUENCY
        //
        // 5.1 analog output using mono jacks
        //     1. SPEAKER_FRONT_LEFT
        //     2. SPEAKER_FRONT_RIGHT
        //     3. SPEAKER_BACK_LEFT
        //     4. SPEAKER_BACK_RIGHT
        //     5. SPEAKER_FRONT_CENTER
        //     6. SPEAKER_LOW_FREQUENCY
        //
        // but it is invalid to associate a particular channel with multiple jacks
        // and it is invalid to have a jack with no channels mapped to it*
        // * unless the overall channel mask for the endpoint is 0
        for(UINT ii=0; ii < cKsJackDesc; ++ii)
        {
            for(UINT jj=ii + 1; jj < cKsJackDesc; ++jj)
            {
                DWORD dwIntersect = (pKsJackDesc[ii].ChannelMapping & pKsJackDesc[jj].ChannelMapping);
                if(0 != dwIntersect)
                {
                    XLOG(XMSG, eInfo1, "Intersection of jack %u and jack %u is 0x%x", ii, jj, dwIntersect);
#ifdef BUILD_FOR_MOBILE_CORE
                    LogPinErr("FAIL", "Jacks have intersecting channel mappings");
#endif
                }
            }
            
            if(0 == pKsJackDesc[ii].ChannelMapping)
            {
                LogPinErr("FAIL", "Jack has no channels assigned to it but overall channel mask is nonzero", ii);
            }
        }
    }

    // test that color has no invalid bits set
    for(UINT ii=0; ii < cKsJackDesc; ++ii)
    {
        XLOG(XMSG, eInfo1, "Color on jack %u is (JackDescription: 0x00rrggbb = 0x%08x)", ii, pKsJackDesc[ii].Color);
        
        if (0xff000000 & pKsJackDesc[ii].Color)
        {
            LogPinErr("FAIL", "Invalid bits set in 0x00rrggbb color", pKsJackDesc[ii].Color);
        }
    }

    // make sure the bits are in opposite-of-COLORREF-order
    // COLORREF is 0x00bbggrr
    // but KSJACK_DESCRIPTION.Color is 0x00rrggbb
    //
    // Can we know whether a color is reversed?
    // It turns out there are some well-known colors
    // For example, "Line Out" is usually a certain pale green
    // and "Mic In" is usually a certain pink
    // if an endpoint has a Microphone category
    // and has the red/blue swap of the well-known pink
    // then that is almost certainly a bug
    for (UINT ii=0; ii < ARRAYSIZE(g_WellKnownColors); ++ii)
    {
        if (
            flow != g_WellKnownColors[ii].flow ||
            formFactor != g_WellKnownColors[ii].formFactor
        )
        {
            continue;
        }
        
        for(UINT jj=0; jj < cKsJackDesc; ++jj)
        {
            if (
                // if the color is in the wrong format, the first clause will return true
                JackDescriptionColor_Match(pKsJackDesc[jj].Color, g_WellKnownColors[ii].colorGDI) &&
                
                // but some colors look the same in both formats, so exclude those
                !JackDescriptionColor_Match(pKsJackDesc[jj].Color, g_WellKnownColors[ii].colorJackDescription)
            )
            {
                XLOG(XMSG, eInfo1,
                    "A well-known color for this kind of audio device is: "
                    "\"%ls\"(GDI: 0x00bbggrr = 0x%08x) = (JackDescription: 0x00rrggbb = 0x%08x) "
                    "but jack %u has the reverse "
                    "(GDI: 0x00bbggrr = 0x%08x) = (JackDescription: 0x00rrggbb = 0x%08x); "
                    "make sure you are packing the color in 0x00rrggbb format instead of 0x00bbggrr", 
                    g_WellKnownColors[ii].name,
                    g_WellKnownColors[ii].colorGDI, g_WellKnownColors[ii].colorJackDescription,
                    jj,
                    GdiColor_From_JackDescriptionColor(pKsJackDesc[jj].Color), pKsJackDesc[jj].Color
                );
                LogPinErr("FAIL",
                    "Jack description has a well-known color for this type of audio device "
                    "but with red and blue swapped (in GDI format instead of Jack Description format)"
                );
            }
        }
    }
    
    for(UINT ii=0; ii < cKsJackDesc; ++ii)
    {
        // Validate ConnectionType
        switch(pKsJackDesc[ii].ConnectionType)
        {
            case eConnTypeUnknown:
            case eConnType3Point5mm:
            case eConnTypeQuarter:
            case eConnTypeAtapiInternal:
            case eConnTypeRCA:
            case eConnTypeOptical:
            case eConnTypeOtherDigital:
            case eConnTypeOtherAnalog:
            case eConnTypeMultichannelAnalogDIN:
            case eConnTypeXlrProfessional:
            case eConnTypeRJ11Modem:
            case eConnTypeCombination:
                break;
            default:
                LogPinErr("FAIL", 
                          "KSJACK_DESCRIPTION.ConnectionType assigned invalid value", 
                          pKsJackDesc[ii].ConnectionType);
                break;
        }
        
        // Validate GeoLocation
        switch(pKsJackDesc[ii].GeoLocation)
        {
            case eGeoLocRear:
            case eGeoLocFront:
            case eGeoLocLeft:
            case eGeoLocRight:
            case eGeoLocTop:
            case eGeoLocBottom:
            case eGeoLocRearPanel:
            case eGeoLocRiser:
            case eGeoLocInsideMobileLid:
            case eGeoLocDrivebay:
            case eGeoLocHDMI:
            case eGeoLocOutsideMobileLid:
            case eGeoLocATAPI:
            case eGeoLocNotApplicable:
                break;
            case eGeoLocReserved6:
            default:
                LogPinErr("FAIL", 
                          "KSJACK_DESCRIPTION.GeoLocation assigned invalid value", 
                          pKsJackDesc[ii].GeoLocation);
                break;
        }
        
        // Validate GenLocation
        switch(pKsJackDesc[ii].GenLocation)
        {
            case eGenLocPrimaryBox:
            case eGenLocInternal:
            case eGenLocSeparate:
            case eGenLocOther:
                break;
            default:
                LogPinErr("FAIL", 
                          "KSJACK_DESCRIPTION.GenLocation assigned invalid value", 
                          pKsJackDesc[ii].GenLocation);
                break;
        }
        
        // Validate PortConnection
        switch(pKsJackDesc[ii].PortConnection)
        {
            case ePortConnJack:
            case ePortConnIntegratedDevice:
            case ePortConnBothIntegratedAndJack:
            case ePortConnUnknown:
                break;
            default:
                LogPinErr("FAIL", 
                          "KSJACK_DESCRIPTION.PortConnection assigned invalid value", 
                          pKsJackDesc[ii].PortConnection);
                break;
        }
    }
    XLOG(XMSG, eInfo1, "KSPROPERTY_JACK_DESCRIPTION supported");

Exit:
    delete[] pksmiPropVal;
    return fRet;
}

bool CPinReq::IsJackDesc2Supported()
{
    bool                    fRet = true;
    PKSMULTIPLE_ITEM        pKsMultiple = NULL;
    PKSMULTIPLE_ITEM        pKsMultiple2 = NULL;
    UINT                    uOutSize = 0;
    UINT                    uOutSize2 = 0;
    PKSJACK_DESCRIPTION2    pKsJackDescription2;
    DWORD                   dwJackCapabilitiesMask =
                                JACKDESC2_PRESENCE_DETECT_CAPABILITY |
                                JACKDESC2_DYNAMIC_FORMAT_CHANGE_CAPABILITY;

    // Test does not apply to USB Audio 1.0 devices
    bool fIsUSBAudio10Device = false;
    HRESULT hr = IsUSBAudio10Device(fIsUSBAudio10Device);
    if (FAILED(hr))
    {
        LogPinErr("FAIL", "Failed to query for whether device is USB Audio 1.0 device");
        fRet = false;
        goto Exit;
    }

    if(!PinIoCtl_Get(KSPROPSETID_Jack, KSPROPERTY_JACK_DESCRIPTION2,
                     NULL, 0, (void**)&pKsMultiple2, uOutSize2))
    {
        if (fIsUSBAudio10Device)
        {
            LogPinSkip("KSPROPERTY_JACK_DESCRIPTION2 is not required to be supported on USB Audio 1.0 Devices");
            fRet = true;
            goto Exit;
        }
        else
        {
            LogPinErr("FAIL", "Failed to query KSPROPERTY_JACK_DESCRIPTION2.");
            fRet = false;
            goto Exit;
        }
    }

    SLOG(3, "Bytes in KSPROPERTY_JACK_DESCRIPTION2 response: %d", uOutSize2);

    if(uOutSize2 < sizeof(KSMULTIPLE_ITEM))
    {
        LogPinErr("FAIL", "KSPROPERTY_JACK_DESCRIPTION2 response is too small", uOutSize2);
        fRet = false;
        goto Exit;
    }

    SLOG(3,
        "KSPROPERTY_JACK_DESCRIPTION2 response: KSMULTIPLE_ITEM.Size = %d, KSMULTIPLE_ITEM.Count = %d, buffer size = %d",
        pKsMultiple2->Size, pKsMultiple2->Count, uOutSize2
    );

    if(
        sizeof(KSMULTIPLE_ITEM) + pKsMultiple2->Count * sizeof(KSJACK_DESCRIPTION2) != pKsMultiple2->Size ||
        pKsMultiple2->Size != uOutSize2
    )
    {
        LogPinErr("FAIL", "KSPROPERTY_JACK_DESCRIPTION2 response size does not add up", uOutSize2);
        fRet = false;
        goto Exit;
    }

    if(0 == pKsMultiple2->Count)
    {
        LogPinErr("FAIL", "KSPROPERTY_JACK_DESCRIPTION2 response returned a KSMULTIPLE_ITEM.Count = 0");
        fRet = false;
        goto Exit;
    }

    // check consistency with KSPROPERTY_JACK_DESCRIPTION
    if(!PinIoCtl_Get(KSPROPSETID_Jack, KSPROPERTY_JACK_DESCRIPTION,
                     NULL, 0, (void**)&pKsMultiple, uOutSize))
    {
        LogPinErr("FAIL", "Failed to query KSPROPERTY_JACK_DESCRIPTION.");
        fRet = false;
        // don't goto Exit - test KSJACK_DESCRIPTION2 further
    }
    else if(pKsMultiple->Count != pKsMultiple2->Count)
    {
        XLOG(XFAIL, 1,
            "FAIL: Jack counts differ between KSPROPERTY_JACK_DESCRIPTION (%d) and KSPROPERTY_JACK_DESCRIPTION2 (%d)",
            pKsMultiple->Count, pKsMultiple2->Count
        );
        fRet = false;
        // don't goto Exit - test KSJACK_DESCRIPTION2 further
    }
    
    pKsJackDescription2 = (PKSJACK_DESCRIPTION2)(pKsMultiple2 + 1);
    for(UINT i = 0; i < pKsMultiple2->Count; i++)
    {
        SLOG(3, "KSJACK_DESCRIPTION2 #%u: DeviceStateInfo (Reserved) = 0x%08x, JackCapabilities = 0x%08x",
            i + 1,
            pKsJackDescription2[i].DeviceStateInfo,
            pKsJackDescription2[i].JackCapabilities);

        if(0 != pKsJackDescription2[i].DeviceStateInfo)
        {
            LogPinErr("FAIL", "KSPROPERTY_JACK_DESCRIPTION2.DeviceStateInfo is reserved and must be 0",
                pKsJackDescription2[i].DeviceStateInfo);
            fRet = false;
            // don't goto Exit... test the rest
        }

        if(0 != (pKsJackDescription2[i].JackCapabilities & ~dwJackCapabilitiesMask))
        {
            LogPinErr("FAIL", "Invalid value for KSJACK_DESCRIPTION2.JackCapabilities",
                pKsJackDescription2[i].JackCapabilities);
            fRet = false;
            // don't goto Exit... test the rest
        }
    } // for each jack

Exit:
    delete[] pKsMultiple;
    delete[] pKsMultiple2;
    return fRet;
}

bool CPinReq::IsJackSinkInfoSupported()
{
    bool                        fRet = true;
    KSJACK_SINK_INFORMATION    *pKsSinkInfo = NULL;
    UINT                        uOutSize = 0;
    bool                        fIsNullTerminated = false;
    char                        szSinkDesc[MAX_SINK_DESCRIPTION_NAME_LENGTH] = {0};

    // Test does not apply to USB Audio 1.0 devices
    bool fIsUSBAudio10Device = false;
    HRESULT hr = IsUSBAudio10Device(fIsUSBAudio10Device);
    if (FAILED(hr))
    {
        LogPinErr("FAIL", "Failed to query for whether device is USB Audio 1.0 device");
        fRet = false;
        goto Exit;
    }
    
    if(!PinIoCtl_Get(KSPROPSETID_Jack, KSPROPERTY_JACK_SINK_INFO,
                     NULL, 0, (void**)&pKsSinkInfo, uOutSize))
    {
        if (fIsUSBAudio10Device)
        {
            LogPinSkip("KSPROPERTY_JACK_SINK_INFO is not required to be supported on USB Audio 1.0 Devices");
            fRet = true;
            goto Exit;
        }
        else
        {
            LogPinErr("FAIL", "Failed to query KSPROPERTY_JACK_SINK_INFO.");
            fRet = false;
            goto Exit;
        }
    }

    // Validate KSPROPERTY_JACK_SINK_INFO.SinkDescription is NULL terminated
    for(UINT i=0; i < MAX_SINK_DESCRIPTION_NAME_LENGTH; ++i)
    {
        if(pKsSinkInfo->SinkDescription[i] == L'\0')
        {
            fIsNullTerminated = true;
            // Length of string does not include terminating NULL
            if(pKsSinkInfo->SinkDescriptionLength != i)
            {
                LogPinErr("FAIL", "Length of KSPROPERTY_JACK_SINK_INFO.SinkDescription"
                                  " is not equal to value in "
                                  "KSPROPERTY_JACK_SINK_INFO.SinkDescriptionLength");
                fRet = false;
            }
            break;
        }
    }
    if(!fIsNullTerminated)
    {
        LogPinErr("FAIL", "KSPROPERTY_JACK_SINK_INFO.SinkDescription is not NULL terminated");
        fRet = false;
        goto Exit;
    }

    // Convert SinkDescription from WCHAR to CHAR
    wcstombs(szSinkDesc, pKsSinkInfo->SinkDescription, pKsSinkInfo->SinkDescriptionLength);

    // Log KSPROPERTY_JACK_SINK_INFO
    LogPinMsg(3, "KSPROPERTY_JACK_SINK_INFO supported");
    SLOG(5, "     KSJACK_SINK_INFORMATION.ConnType  = 0x%X\n"
            "     KSJACK_SINK_INFORMATION.ManufacturerId  = 0x%X\n"
            "     KSJACK_SINK_INFORMATION.ProductId = 0x%X\n"
            "     KSJACK_SINK_INFORMATION.AudioLatency = 0x%X\n"
            "     KSJACK_SINK_INFORMATION.HDCPCapable = %s\n"
            "     KSJACK_SINK_INFORMATION.AICapable = %s\n"
            "     KSJACK_SINK_INFORMATION.SinkDescription = %s\n"
            "     KSJACK_SINK_INFORMATION.SinkDescriptionLength = 0x%X\n"
            "     KSJACK_SINK_INFORMATION.PortId = {%.8lX-%.8lX}",
            pKsSinkInfo->ConnType, pKsSinkInfo->ManufacturerId, 
            pKsSinkInfo->ProductId, pKsSinkInfo->AudioLatency, 
            ((pKsSinkInfo->HDCPCapable) ? "true" : "false"),
            ((pKsSinkInfo->AICapable) ? "true" : "false"),
            szSinkDesc, pKsSinkInfo->SinkDescriptionLength,
            pKsSinkInfo->PortId.HighPart, pKsSinkInfo->PortId.LowPart);

Exit:
    delete[] pKsSinkInfo;
    return fRet;
}

bool CPinReq::CheckPinInstanceCountHelper(void *pArg)
{
    bool fRet = true;
    CPinReq *pThis = static_cast<CPinReq*>(pArg);
    CPinFactory* pPinFactory = pThis->m_pPinFactory;
    ULONG pinSupportFlags = pPinFactory->GetFlags();
    PKSPIN_CINSTANCES pPinInstances = NULL;

    if ( pPinFactory->GetCommunication() != KSPIN_COMMUNICATION_SINK )
    {
        return true;
    }

    if ( pinSupportFlags & CPIN_FLAG_GLOBALCINSTANCES )
    {
        pThis->LogPinMsg( 2, "Pin supports GLOBALINSTANCES." );
        pPinInstances = pPinFactory->GetGlobalCInstances();
    }
    else if ( pinSupportFlags & CPIN_FLAG_CINSTANCES )
    {
        pThis->LogPinMsg( 2, "Pin supports CINSTANCES." );
        pPinInstances = pPinFactory->GetCInstances();
    }
    else
    {
        pThis->LogPinErr( "FAIL", "Pin does not support KSPROPERTY_PIN_GLOBALCINSTANCES or KSPROPERTY_PIN_CINSTANCES" );

        return false;
    }
    
    if (pPinFactory->IsAEHostPinFactory())
    {
        PKSMULTIPLE_ITEM pKsMultipleItem = NULL;

        fRet = pThis->QueryForProcessingModes(&pKsMultipleItem);
        if (fRet)
        {
            if (pKsMultipleItem->Count > 0)
            {
                if (pPinInstances->PossibleCount < 1)
                {
                    fRet = false;
                    SLOG(1, "Pin instance possible count = %d, number of modes = %d", pPinInstances->PossibleCount, pKsMultipleItem->Count);
                    pThis->LogPinErr("FAIL", "Host Pin is required to support at least one instance.");
                }
            }
            else
            {
                fRet = false;
                SLOG(1, "Pin instance possible count = %d, number of modes = %d", pPinInstances->PossibleCount, pKsMultipleItem->Count);
                pThis->LogPinErr("FAIL", "Host Pin is required to support a number of instances ! = 0");
            }
            delete [] pKsMultipleItem;
        }
        else
        {
            if (pPinInstances->PossibleCount != 1)
            {
                fRet = false;
                pThis->LogPinErr("FAIL", "Host Pin is required to support a single instance.  Pin's possible count", pPinInstances->PossibleCount);
            }
            else
            {
                fRet = true;
            }

        }

        if (fRet)
        {
            pThis->LogPinMsg(2, "Host Pin supports correct number of pin instances.");
        }

    }
    else if ( pPinFactory->IsAEOffloadPinFactory() )
    {
        if ( pPinInstances->PossibleCount < 2 )
        {
            fRet = false;
            pThis->LogPinErr( "FAIL", "Offload Pin is required to support at least two instances. Pin's possible count", pPinInstances->PossibleCount );
        }
        else
        {
            pThis->LogPinMsg( 2, "Offload Pin supports at least two instances." );
        }
    }
    else if ( pPinFactory->IsAELoopbackPinFactory() ||  (*(pPinFactory->GetCategory()) == KSNODETYPE_AUDIO_LOOPBACK) )
    {
        if ( pPinInstances->PossibleCount < 1 )
        {
            fRet = false;
            pThis->LogPinErr( "FAIL", "Loopback Pin is required to support at least a single instance.  Pin's possible count", pPinInstances->PossibleCount );
        }
        else
        {
            pThis->LogPinMsg( 2, "Loopback Pin supports at least a single instance." );
        }
    }
    else
    {
        PKSMULTIPLE_ITEM pKsMultipleItem = NULL;	

        fRet = pThis->QueryForProcessingModes(&pKsMultipleItem);
        if ( fRet )
        {
            if ( pKsMultipleItem->Count > 0 )
            {
                if ( pPinInstances->PossibleCount < 1 )
                {
                    fRet = false;
                    SLOG(1,"Pin instance possible count = %d, number of modes = %d",pPinInstances->PossibleCount, pKsMultipleItem->Count);
                    pThis->LogPinErr( "FAIL", "Pin is required to support at least one instance.");

                }
            }
            else
            {
                fRet = false;
                SLOG(1,"Pin instance possible count = %d, number of modes = %d",pPinInstances->PossibleCount, pKsMultipleItem->Count);
                pThis->LogPinErr( "FAIL", "Host Pin is required to support a number of instances ! = 0");       
            }
            delete [] pKsMultipleItem;
        }
        else
        {
            if ( pPinInstances->PossibleCount < 1 )
            {        
                fRet = false;                
                pThis->LogPinErr( "FAIL", "Pin is required to support atleast a single instance. ");
            }
            else
            {
                fRet = true;
            }

        }

        if (fRet)
        {
            pThis->LogPinMsg( 2, "Pin supports correct number of pin instances." );
        }

    }

    return fRet;
}