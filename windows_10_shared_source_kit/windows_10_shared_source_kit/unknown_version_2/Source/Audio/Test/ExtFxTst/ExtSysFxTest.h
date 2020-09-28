// Copyright (C) Microsoft. All rights reserved.
#pragma once

// ------------------------------------------------------------------------------
// Device definition

#define DT_APO          0x00000001
#define DT_GFX          0x00000002
#define DT_LFX          0x00000004
#define DT_LGFX         (DT_LFX | DT_GFX)
#define DT_SFX          0x00000008
#define DT_MFX          0x00000010
#define DT_EFX          0x00000020
#define DT_SMEFX        (DT_SFX | DT_MFX | DT_EFX)
#define DT_SYSFX        (DT_LGFX | DT_SMEFX)
#define DT_ALL          (DT_SYSFX | DT_APO)

class CAPODevice : public CDeviceDescriptor
{
    public:
        CAPODevice
        (
            IMMDevice          *pIEndpoint,
            LPCWSTR             pszClassId,
            LPCWSTR             pszEndpoint,
            IPropertyStore     *pIStoreDevice,
            IPropertyStore     *pIStoreFx,
            LPARAM              apoType,
            LPCWSTR             pszAttachedDevice,
            BOOL                bProxyAPO
        );
        CAPODevice
        (
            IAudioProcessingObject     *pIAPO,
            LPCWSTR                     pszClassId
        );
        ~CAPODevice(void);
        HRESULT InitializeAPO(IAudioProcessingObject *pIAPO);
        DWORD   GetDeviceType() { return (DWORD)(m_lpType); }
        BOOL    IsValid() { return (m_fValid); }
        void    GetClsID(GUID *pgClsID) { *pgClsID = m_gClsID; }
        BOOL    GetAPOInterfaces
        (
            IAudioProcessingObject                **pIAPO,
            IAudioProcessingObjectRT              **pIAPORT,
            IAudioProcessingObjectConfiguration   **pIAPOConfig
        );
        IUnknown *GetObject() { return ((IUnknown*)m_pIUnknown); }
        IMMDevice *GetEndpoint() { return m_pIEndpoint; }
        HRESULT GetMixFormat(IAudioMediaType **ppAMTFormat);
        BOOL NegotiateFormatsFromInput();
        IPropertyStore *GetEndpointStore() { return m_pIEPStore; }
        IPropertyStore *GetFxStore() { return m_pIFXStore; }
        IMMDeviceCollection *GetDeviceCollection() { return m_pIDevCollection; }
        UINT GetSoftwareIoDeviceInCollection() { return m_nSoftwareIoDeviceInCollection; }
        UINT GetSoftwareIoConnectorIndex() { return m_nSoftwareIoConnectorIndex; }
        PAPO_REG_PROPERTIES GetProperties() { return (m_pRegProps); }
        LPSTR GetAttachedDevice() { return (LPSTR)(&m_szAttachedDevice[0]); }
        BOOL IsProxyApo() { return m_bProxyAPO; }
        // @@BEGIN_NOTPUBLICCODE
        //BOOL GetRegistrationProperties(LPCWSTR pszClassId);
        // @@END_NOTPUBLICCODE

    private:
        BOOL                                            m_fValid;
        GUID                                            m_gClsID;
        CComPtr<IUnknown>                               m_pIUnknown;
        CComPtr<IAudioProcessingObject>                 m_pIAPO;
        PAPO_REG_PROPERTIES                             m_pRegProps;
        
        //  These are only used for SysFx
        CComPtr<IMMDevice>                              m_pIEndpoint;
        char                                            m_szAttachedDevice[MAX_PATH];
        CComPtr<IPropertyStore>                         m_pIEPStore;
        CComPtr<IPropertyStore>                         m_pIFXStore;
        CComPtr<IMMDeviceCollection>                    m_pIDevCollection;
        UINT         									m_nSoftwareIoDeviceInCollection;
        UINT        									m_nSoftwareIoConnectorIndex;
        BOOL                                            m_bProxyAPO;
};


// ----------------------------------------------------------
// Test Module Class declaration
class CAPODeviceTestModule: public CDeviceTestModule
{
    public:
    #ifndef BUILD_TAEF
        CAPODeviceTestModule(CTestShell* pShell, HINSTANCE hInstDLL);
    #else
        CAPODeviceTestModule(DV_TCINFO * ptcInfo);
    #endif
        virtual ~CAPODeviceTestModule(VOID);

        // CTestModule virtuals
        virtual DWORD   OnInitialUpdate(VOID);
        virtual VOID    SetTestDevice(CDeviceDescriptor* pDevice);
        virtual DWORD   EndTestRun(VOID) { return (FNS_PASS); }
        virtual DWORD   StartTestRun(VOID) { return (FNS_PASS); }
        virtual DWORD   RunTest(DWORD dwTestCase);
        CAPODevice     *GetDevice() const { return m_pDevice; }
        
    private:
        BOOL AddSysFxDevices();
        BOOL AddAPODevices();
        BOOL AddAPODevicesWithInternalList();
        BOOL AddAPODevice(LPCWSTR szAPOGuid, IAudioProcessingObject **ppIAPO);

        CAPODevice     *m_pDevice; // current device under test
};
