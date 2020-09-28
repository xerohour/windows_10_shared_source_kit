/*
 *  file        :   CPControl.h
 *  description :   control types declarations
 *  history     :	12/12/2007 created
 *  Author      :   mhusain
 */
#pragma once

#include <windows.h>
#include <wincrypt.h>

#include <d3d9.h>
#include <d3d11.h>
#include <dxva9typ.h>
#include <ddraw.h>
//#include <opmapi.h>
#include "AuthHelper.h"



/*
 *  class       :   CPControl
 *  description :   Content Protection helper based on DX9 APIs
 *  notes       :   abstract base
 */


class CPControl
{
public:
    CPControl(IDirect3DDevice9Ex*, D3DAUTHENTICATEDCHANNELTYPE , HRESULT*);
    ~CPControl();
    
    //General
    HRESULT QueryChannelType(D3DAUTHENTICATEDCHANNELTYPE*  pType);
    HRESULT QueryDeviceHandle(HANDLE*);
    HRESULT QueryProtectionType(BOOL*, BOOL*);
    HRESULT QueryCryptoSession(HANDLE, HANDLE*, HANDLE*);
    
    //Shared resources
    HRESULT QueryUnresricttedProtectedsharedResourceCount(UINT*);
    HRESULT QueryRestrictedSharedResourceProcess(UINT, UINT*, HANDLE*);
    HRESULT QueryRestrictedSharedResourceProcessCount(UINT*);
    
    // Eviction
    HRESULT QueryEvictionEncryptionGuidCount(UINT*);
    HRESULT QueryEvictionEncryptionGuid(UINT, GUID*);
    HRESULT QueryCurrentEvictionEncryption(GUID*);
    
    //Output
    HRESULT QueryBusData(D3DBUSTYPE*, BOOL*, BOOL*);
    HRESULT QueryOutputIdCount(HANDLE, HANDLE, UINT*);
    HRESULT QueryOutputId(HANDLE, HANDLE, UINT, UINT64*);


    HRESULT ConfigureInitialize();
    HRESULT ConfigureProtection(BOOL, BOOL);
    HRESULT ConfigureCryptoSession(HANDLE, HANDLE, HANDLE);
    HRESULT ConfigureSharedResource(HANDLE, BOOL, BOOL);
    HRESULT ConfigureEvictionCryption(GUID);


    HRESULT Initialize(BOOL bTryWithRSAEnc = FALSE, BOOL bRevoked = FALSE);

    D3DAUTHENTICATEDCHANNELTYPE GetChannelType(){return m_dact;};
    BYTE* GetCertificate(){return m_pbGHCert;};
    ULONG GetCertificateSize(){return m_cbGHCertLen;};
    HANDLE GetChannelHandle(){return m_hChannel;};
    HANDLE GetDevicelHandle(){return m_DeviceHandle;};
    HANDLE GetCryptoSessionHandle(){return m_CryptoSessionHandle;};
    CPAuthHelper GetAuthHelper(){return m_AuthHelper;};
    CPAuthHelper GetCryptoHelper(){return m_CryptoHelper;};
    IDirect3DAuthenticatedChannel9* GetAuthenticatedChannel(){return m_pChannel;};
    
    //CryptoSession
    HRESULT InitCryptoSession(GUID* pDecodeGuid);

    // Caps
    HRESULT InitializeContentProtectionCaps(GUID* pCryptoType = NULL, GUID* pDecodeProfile = NULL);
    D3DCONTENTPROTECTIONCAPS GetContentProtectionCaps() {return m_ContentProtectionCaps;};
    BOOL SoftwareProtectionSupported();
    BOOL HardwareProtectionSupported();
    BOOL HardwareProtectionAlwaysOn();
    BOOL DriverProtectionSupported(); 

protected:

    D3DAUTHENTICATEDCHANNELTYPE m_dact;//note that this is only used for creation

    //  context
    IDirect3DDevice9Ex* m_pParent;

    IDirect3DDevice9Video* m_pVideo;  
    IDirect3DCryptoSession9* m_pCryptoSession;
    IDirect3DAuthenticatedChannel9* m_pChannel;

    HANDLE m_hChannel;
    HANDLE m_CryptoSessionHandle;
    HANDLE m_DeviceHandle;
    
    //  OPM parameters
    UINT            m_SequenceQuery;
    UINT            m_SequenceConfigure;

    //  helper
    CPAuthHelper  m_AuthHelper;
    CPAuthHelper  m_CryptoHelper;
    Crescent::CAESHelper* m_pAESHelper;

    //certificate
    BYTE*          m_pbGHCert;
    UINT           m_cbGHCertLen;

    // Caps
    D3DCONTENTPROTECTIONCAPS m_ContentProtectionCaps;
    D3DCONTENTPROTECTIONCAPS m_CryptoCaps;

    //  methods
    HRESULT Query
    (
        D3DAUTHENTICATEDCHANNEL_QUERY_INPUT* pInput,
        UINT InputSize,
        D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT* pOutput,
        UINT OutputSize
    );

    HRESULT Configure
    (
        D3DAUTHENTICATEDCHANNEL_CONFIGURE_INPUT* pInput, 
        UINT InputSize
    );

};  //  class   CPControl


/*
 *  class       :   CPControl11
 *  description :   Content Protection helper based on DX11 APIs
 *  notes       :  
 */

class CPControl11
{
public:
    CPControl11(ID3D11Device*, D3D11_AUTHENTICATED_CHANNEL_TYPE , HRESULT*);
    ~CPControl11();
    
    //General
    HRESULT QueryChannelType(D3D11_AUTHENTICATED_CHANNEL_TYPE*  pType);

    HRESULT QueryDeviceHandle(HANDLE*);
    HRESULT QueryProtection(BOOL*, BOOL*);
    HRESULT QueryCryptoSession(HANDLE, HANDLE*, HANDLE*);
    
    //Shared resources
    HRESULT QueryUnresricttedProtectedsharedResourceCount(UINT*);
    HRESULT QueryRestrictedSharedResourceProcess(UINT, UINT*, HANDLE*);
    HRESULT QueryRestrictedSharedResourceProcessCount(UINT*);
    
    // Eviction
    HRESULT QueryEncryptionWhenAccessibleGuidCount(UINT*);
    HRESULT QueryEncryptionWhenAccessibleGuid(UINT, GUID*);
    HRESULT QueryCurrentEncryptionWhenAccessible(GUID*);
    
    //Output
    HRESULT QueryAccessibilityAttributes(D3D11_BUS_TYPE*, BOOL*, BOOL*);
    HRESULT QueryOutputIdCount(HANDLE, HANDLE, UINT*);
    HRESULT QueryOutputId(HANDLE, HANDLE, UINT, UINT64*);


    HRESULT ConfigureInitialize();
    HRESULT ConfigureProtection(BOOL, BOOL);
    HRESULT ConfigureCryptoSession(HANDLE, HANDLE, HANDLE);
    HRESULT ConfigureSharedResource(HANDLE, BOOL, BOOL);
    HRESULT ConfigureEncryptionWhenAccessible(GUID);


    HRESULT Initialize(BOOL bTryWithRSAEnc = FALSE, BOOL bRevoked = FALSE);

    D3D11_AUTHENTICATED_CHANNEL_TYPE GetChannelType(){return m_dact11;};
    BYTE* GetCertificate(){return m_pbGHCert;};
    ULONG GetCertificateSize(){return m_cbGHCertLen;};
    HANDLE GetChannelHandle(){return m_hChannel11;};
    HANDLE GetDeviceHandle(){return m_DeviceHandle;};
    HANDLE GetCryptoSessionHandle(){return m_CryptoSessionHandle11;};
    CPAuthHelper GetAuthHelper(){return m_AuthHelper;};
    CPAuthHelper GetCryptoHelper(){return m_CryptoHelper;};
    ID3D11AuthenticatedChannel* GetAuthenticatedChannel(){return m_pChannel11;};
    ID3D11VideoContext* GetVideoContext(){return m_pVideoContext11;};

    //CryptoSession
    HRESULT InitCryptoSession(GUID* pDecodeGuid);

    // Caps
    HRESULT InitializeContentProtectionCaps(GUID* pCryptoType = NULL, GUID* pDecodeProfile = NULL);
    D3D11_VIDEO_CONTENT_PROTECTION_CAPS GetContentProtectionCaps() {return m_ContentProtectionCaps;};
    BOOL SoftwareProtectionSupported();
    BOOL HardwareProtectionSupported();
    BOOL HardwareProtectionAlwaysOn();
    BOOL DriverProtectionSupported(); 

protected:
    D3DAUTHENTICATEDCHANNELTYPE m_dact;//note that this is only used for creation

    // D3D11 interfaces
    ID3D11Device* m_pParent11;
    ID3D11DeviceContext* m_pDeviceContext11;
    ID3D11VideoDevice* m_pVideo11;
    ID3D11VideoContext* m_pVideoContext11;
    ID3D11AuthenticatedChannel* m_pChannel11;
    ID3D11CryptoSession* m_pCryptoSession11;
    HANDLE m_hChannel11;
    HANDLE m_CryptoSessionHandle11;
    D3D11_AUTHENTICATED_CHANNEL_TYPE m_dact11;//note that this is only used for creation

    HANDLE m_DeviceHandle;
    
    //  OPM parameters
    UINT            m_SequenceQuery;
    UINT            m_SequenceConfigure;

    //  helper
    CPAuthHelper  m_AuthHelper;
    CPAuthHelper  m_CryptoHelper;
    Crescent::CAESHelper* m_pAESHelper;

    //certificate
    BYTE*          m_pbGHCert;
    UINT           m_cbGHCertLen;

    // Caps
    D3D11_VIDEO_CONTENT_PROTECTION_CAPS m_ContentProtectionCaps;
    D3D11_VIDEO_CONTENT_PROTECTION_CAPS m_CryptoCaps;


    HRESULT Query
    (
        D3D11_AUTHENTICATED_QUERY_INPUT* pInput,
        UINT InputSize,
        D3D11_AUTHENTICATED_QUERY_OUTPUT* pOutput,
        UINT OutputSize
    );

    HRESULT Configure
    (
        D3D11_AUTHENTICATED_CONFIGURE_INPUT* pInput, 
        UINT InputSize
    );

};  //  class   CPControl11
