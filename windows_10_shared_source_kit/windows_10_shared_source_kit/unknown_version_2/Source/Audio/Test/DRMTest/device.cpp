// Copyright (C) Microsoft. All rights reserved.
#include "common.h"

#include <mmsystem.h>
#include <dsoundp.h>
#include <dsprv.h>

//--------------------------------------------------------------------------;
//
//  CAudioDevice::CAudioDevice
//
//  Description:
//      Constructor
//
//  Arguments:
//      (void)
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
CAudioDevice::CAudioDevice(void)
: CDeviceDescriptor(),
  m_pSADFilter(NULL),
  m_pDeviceProp(NULL),
  m_fIsDrmImplemented(FALSE),
  m_pFilter(NULL),
  m_ulRenderInstances(0),
  m_ulAllowedDRMLevel((ULONG)-1)
{
    m_DefaultWFX.wFormatTag      = WAVE_FORMAT_PCM;
    m_DefaultWFX.nChannels       = 2;
    m_DefaultWFX.nSamplesPerSec  = 48000;
    m_DefaultWFX.nAvgBytesPerSec = 192000;
    m_DefaultWFX.nBlockAlign     = 4;
    m_DefaultWFX.wBitsPerSample  = 16; 
    m_DefaultWFX.cbSize          = 0;
    //Initialize CDeviceDescriptor
    ZeroMemory(m_szPnPId, sizeof(TCHAR) * 256);
} // CAudioDevice

CAudioDevice::~CAudioDevice()
{
} // ~CAudioDevice

//--------------------------------------------------------------------------;
//
//  CheckDeviceInterface
//
//  Description:
//      Compare the given device interface to the CAudioDevice::m_pDeviceProp
//  interface.
//
//  Arguments:
//      PCKsFilter pKsFilter :
//
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL 
CAudioDevice::CheckDeviceInterface
(
    PCKsFilter pKsFilter
)
{
    NtVersion os(g_OSVersionInfo.dwMajorVersion, g_OSVersionInfo.dwMinorVersion);

    if (os < NtVersion(6, 0))
    {
        //KM Audio Stack is XP and before only

        //
        // Skip the first part of the interface name.
        // Filter names start with \\.\
        //
        if (m_pDeviceProp && pKsFilter)
        {
            LPTSTR   str1;
            LPTSTR   str2;

            str1 = m_pDeviceProp->m_szFilterName + 4;
            str2 = pKsFilter->m_szFilterName + 4;

            if (0 == _tcscmp(str1, str2))
            {
                return TRUE;
            }
        }

        return FALSE;
    }
    else
    {         
        return TRUE;
    }
} // CheckDeviceInterface

//--------------------------------------------------------------------------;
//
//  CheckDevNode
//
//  Description:
//      Finds the given devnode in the list.
//
//  Arguments:
//      TTList <CKsFilter> *pList :
//      DWORD dwDevNode :
//
//  Return:
//      (PCKsFilter)
//
//--------------------------------------------------------------------------;
PCKsFilter 
CAudioDevice::CheckDevNode
(
    TTList <CKsFilter>* pList, 
    DWORD               dwDevNode
)
{
    TTNode <CKsFilter>* pnFilter;
    PCKsFilter          pFilter;

    pnFilter = pList->GetHead();
    while (pnFilter)
    {
        pFilter = pnFilter->pData;
        pnFilter = pList->GetNext(pnFilter);

        if (pFilter->m_dwDevInst == dwDevNode)
        {
            return pFilter;
        }
    }

    return NULL;
} // CheckDevNodepFilter


//--------------------------------------------------------------------------;
//
//  CheckFilterName
//
//  Description:
//      Finds the given filtername in the list.
//
//  Arguments:
//      TTList <CKsFilter> *pList :
//      LPSTR strFilterName :
//
//  Return:
//      (PCKsFilter)
//
//--------------------------------------------------------------------------;
PCKsFilter 
CAudioDevice::CheckFilterName
(
    TTList <CKsFilter>* pList, 
    LPTSTR              strFilterName
)
{
    TTNode <CKsFilter>* pnFilter;
    PCKsFilter          pFilter;

    pnFilter = pList->GetHead();
    while (pnFilter)
    {
        pFilter = pnFilter->pData;
        pnFilter = pList->GetNext(pnFilter);

        if (_tcsicmp(pFilter->m_szFilterName, strFilterName) == 0)
        {
            return pFilter;
        }
    }

    return NULL;
}

//--------------------------------------------------------------------------;
//
//  CAudioDevice::DumpProperties
//
//  Description:
//      Print the properties of the device.
//
//  Arguments:
//      (void)
//
//  Return:
//      (void)
//
//--------------------------------------------------------------------------;
void
CAudioDevice::DumpProperties()
{
    XLOG(XMSG, eInfo1, _T("XXX Render Instances      : %d"), m_ulRenderInstances);
    XLOG(XMSG, eInfo1, _T("XXX DRM Compliant         : %s"), IsDRMCompliant() ? _T("Yes") : _T("No"));
    XLOG(XMSG, eInfo1, _T(""));
} // DumpProperties

//--------------------------------------------------------------------------;
//
//  CAudioDevice::FindDRMPin
//
//  Description:
//      Finds and returns the DRM pin.
//
//  Arguments:
//
//  Return:
//      (PCKsPin)
//
//--------------------------------------------------------------------------;
PCKsPin
CAudioDevice::FindDRMPin()
{
    PCKsPin                     pKsPin = NULL;
    
    if (IsValidHandle(m_pSADFilter->m_handle))
    {
        if (!m_pSADFilter->m_listRenderSinkPins.IsEmpty())
        {
            // Assume that COMMUNICATION and MEDIUM and INTERFACES are OK.
            if (::FindDRMPin( 
                    m_pSADFilter,
                    &m_pSADFilter->m_listRenderSinkPins,
                    &pKsPin))
            {
                return pKsPin;
            }
        }
    }

    return pKsPin;
} // FindDRMPin

//--------------------------------------------------------------------------;
//
//  CAudioDevice::FindPCMPin
//
//  Description:
//      Finds and returns the PCM pin.
//
//  Arguments:
//
//  Return:
//      (PCKsPin)
//
//--------------------------------------------------------------------------;
PCKsPin
CAudioDevice::FindPCMPin()
{
    PCKsPin                     pKsPin = NULL;
    
    if (IsValidHandle(m_pSADFilter->m_handle))
    {
        if (!m_pSADFilter->m_listRenderSinkPins.IsEmpty())
        {
            // Assume that COMMUNICATION and MEDIUM and INTERFACES are OK.
            if (::FindPCMPin( 
                    m_pSADFilter,
                    &m_pSADFilter->m_listRenderSinkPins,
                    &pKsPin))
            {
                return pKsPin;
            }
        }
    }

    return pKsPin;
} // FindPCMPin

//--------------------------------------------------------------------------;
//
//  CAudioDevice::Initialize
//
//  Description:
//      Initializes CAudio device.
//
//  Arguments:
//      CKsFilter   *pKSFilter
//      The Filter that this device is to be initialized with.
//
//  Return:
//      TRUE :  Current device is initialized successfully and ready to be added to the test list
//      FALSE:  If there is no render pin in the filter, initialization failed.
//
//--------------------------------------------------------------------------;
BOOL 
CAudioDevice::Initialize
(
    CKsFilter           *pKSFilter
)
{    

    ASSERT(pKSFilter->m_eType == ePCMAudio);

    m_pFilter = (CPCMAudioFilter*)pKSFilter;

    _tcsncpy(m_szRep, m_pFilter->m_szFriendlyName, sizeof(m_szRep)/sizeof(m_szRep[0]));
    _tcsncpy(m_szPnPId, m_pFilter->m_szInstanceID, sizeof(m_szPnPId)/sizeof(m_szPnPId[0]));

    QueryFilter();

    // Make sure current filter contains any render pin
    if ((NULL == m_pFilter) || m_pFilter->m_listRenderSinkPins.IsEmpty())
    {
        return FALSE;
    }

    m_fSelected = TRUE;

    // this test only enumerates render, so this must be render
    m_lpType |= DEVICE_RENDER;

    m_fIsDrmImplemented = SUCCEEDED(IsDrmInterfaceImplemented(this));

    return TRUE;
} // Initialize

//--------------------------------------------------------------------------;
//
//  CAudioDevice::IsDRMCompliant
//
//  Description:
//      Calls utils.cpp
//
//  Arguments:
//      (void)
//
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL
CAudioDevice::IsDRMCompliant
(
    PDRMRIGHTS                  pRights, /* = NULL */
    DWORD                       dwDrmLevel /* = NULL */
)
{
#ifdef WHQL
    return m_fIsDrmImplemented;
#else
    return 
        g_pDRMTest->IsInboxDRMCompliantDriver
        (
            m_pFilter->m_szBinary,  // was m_pDeviceProp->m_szFriendlyName (10/24/2001),
            pRights,
            dwDrmLevel
        );
#endif
} // IsDRMCompliant

//--------------------------------------------------------------------------;
//
//  CAudioDevice::IsPlayback
//
//  Description:
//      
//
//  Arguments:
//      (void)
//
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL
CAudioDevice::IsPlayback()
{
    if (m_lpType & DEVICE_RENDER)
    {
        return TRUE;
    }

    return FALSE;
} // IsPlayback

//--------------------------------------------------------------------------;
//
//  CAudioDevice::IsUsb
//
//  Description:
//      
//
//  Arguments:
//      (void)
//
//  Return:
//      (BOOL)
//
//--------------------------------------------------------------------------;
BOOL
CAudioDevice::IsUsb()
{
    if(NULL != m_pDeviceProp)
    {
        return ::IsUsb(m_pDeviceProp->m_szFilterName);
    }
    else
    {
        return ::IsUsb(m_pFilter->m_szHardwareID);
    }
} // IsUsb

BOOL    
CAudioDevice::QueryFilter()
{
    BOOL    fResult = FALSE;
    
    if (    m_pFilter 
         && m_pFilter->Instantiate()
         && m_pFilter->EnumeratePins()
        )
    {
        TTNode <CKsPin>*    pnPin;
        CPCMAudioPin*       pPin;
        KSPIN_CINSTANCES    Instances;
        DWORD               dwWvFmtMaskAggregate = 0;

        m_ulRenderInstances = 0;

        //
        // loop through all PCM render pins
        // 
        pnPin = m_pFilter->m_listRenderSinkPins.GetHead();
        while (pnPin)
        {
            pPin = (CPCMAudioPin*)pnPin->pData;
            pnPin = m_pFilter->m_listRenderSinkPins.GetNext(pnPin);

            // aggregate the supported formats
            dwWvFmtMaskAggregate |= pPin->m_dwWaveFormatMask;

            // adjust the default wfx to one that this device can deal with...
            // Older ISA cards can't do 48Khz.  Newer AC97s can't do 44.1Khz
            // every card should be capable of stereo, 16-bit
            fResult = 
                m_pFilter->GetPinPropertySimple(
                    pPin->m_nId,
                    KSPROPSETID_Pin,
                    KSPROPERTY_PIN_GLOBALCINSTANCES,
                    (LPVOID) &Instances,
                    sizeof(Instances));

            if (!fResult)
                break;

            m_ulRenderInstances = max(m_ulRenderInstances, Instances.PossibleCount);
        }

        m_DefaultWFX.nSamplesPerSec  = (dwWvFmtMaskAggregate & FMT_48000HZ) ? 48000 : 44100;
        m_DefaultWFX.nAvgBytesPerSec = m_DefaultWFX.nSamplesPerSec * m_DefaultWFX.nBlockAlign;
    }

    return fResult;
} // QueryFilter

