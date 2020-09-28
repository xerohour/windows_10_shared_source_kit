// Copyright (C) Microsoft. All rights reserved.
#ifndef _DEVICE_H
#define _DEVICE_H

///////////////////////////////////////////////////////////////////////////////
// Flags 
#define DEVICE_RENDER             0x0002

typedef enum { eMMwaveOut, eMMwaveIn, eMMmidiOut, eMMmidiIn, eMMmixer } EMMAPI;

///////////////////////////////////////////////////////////////////////////////
// CAudioDevice
class CAudioDevice : public CDeviceDescriptor
{
public:
    CSysAudioDeviceFilter*  m_pSADFilter;       // This is the full device.
    CPCMAudioFilter*        m_pFilter;          // This is the actual KSCATEGORY_AUDIO filter.

private:
    CSysAudioDeviceFilter*  m_pDeviceProp;      // This only contains the device name.

    ULONG                   m_ulRenderInstances;

    BOOL                    m_fIsDrmImplemented;
    ULONG                   m_ulAllowedDRMLevel;    // in Vista, DRMLevel is 1100 or 1200 based on the 
                                                    // use of DrmForwardContentToFileObject.  See case 2.

    WAVEFORMATEX            m_DefaultWFX;

public:
    CAudioDevice();
    ~CAudioDevice();

    void    DumpProperties();

    PCKsPin FindDRMPin();
    PCKsPin FindPCMPin();

    WAVEFORMATEX* GetDefaultWFX() { return &m_DefaultWFX; };
    DWORD       GetCaps() { return (DWORD)m_lpType; };
    LPTSTR      GetName() { if(m_pDeviceProp) { return m_pDeviceProp->m_szFriendlyName;}
                            else if (m_pFilter) {return m_pFilter->m_szFriendlyName;}
                            return NULL;
                           };
    ULONG       GetAllowedDRMLevel() {return m_ulAllowedDRMLevel; };
    void        SetAllowedDRMLevel(ULONG ul) {m_ulAllowedDRMLevel = ul; };

    BOOL        Initialize(CKsFilter *pKSFilter);

    BOOL        IsDigitalRecording(UINT unLineId);
    BOOL        IsDRMCompliant(PDRMRIGHTS pRights = NULL, DWORD dwDrmLevel = 0);
    BOOL        IsDRMImplemented() { return m_fIsDrmImplemented; };
    BOOL        IsHWMixing() { return (m_ulRenderInstances > 1); };
    BOOL        IsPlayback();
    BOOL        IsUsb();

    BOOL        QueryFilter();

private:
    BOOL        CheckDeviceInterface(PCKsFilter pKsFilter);
    PCKsFilter  CheckFilterName(TTList <CKsFilter> *pList, LPTSTR strFilterName);
    PCKsFilter  CheckDevNode(TTList <CKsFilter> *pList, DWORD dwDevNode);
    
};
typedef CAudioDevice *          PCAudioDevice;

#endif
