/*
 *  file        :   CPControl.cpp
 *  description :   CP control implement
 *  history     :	12/12/2007 created
 *  Author      :   mhusain
 */

#include <windows.h>
#include <assert.h>

#ifdef _DEBUGFORKEY
#pragma warning(disable : 4309)
#include "testcertOPM.h"
#endif
#include <CPControl.h>

/*****************************************************
 *  class implementation:   CPControl
 *****************************************************/

CPControl::CPControl
(
    IDirect3DDevice9Ex*        pParent,
    D3DAUTHENTICATEDCHANNELTYPE dact,
    HRESULT*        phr
):  m_SequenceQuery(0),
    m_SequenceConfigure(0),
    m_pbGHCert(NULL),
    m_cbGHCertLen(0),
    m_pVideo(NULL),
    m_pChannel(NULL),
    m_pCryptoSession(NULL),
    m_hChannel(0),
    m_dact(dact)
{

    ZeroMemory(&m_ContentProtectionCaps, sizeof(m_ContentProtectionCaps));

    OutputDebugString(TEXT("CPControl::CPControl \n"));
    do
    {
        if ((0 == pParent) || (0 == phr))
            break;

        if (FAILED(*phr))
            break;

        *phr = pParent->QueryInterface(IID_IDirect3DDevice9Video, (void**)&m_pVideo);
        if (FAILED(*phr))
            break;
    
        *phr = m_pVideo->CreateAuthenticatedChannel(dact, &m_pChannel, &m_hChannel);
        if (FAILED(*phr))
            break;

        InitializeContentProtectionCaps();

    }while(FALSE);

}

CPControl::~CPControl
()
{
    OutputDebugString(TEXT("CPControl::~CPControl \n"));
    
    //release certificate memory
    if (0 != m_pbGHCert)
    {
        delete [] m_pbGHCert;
        m_pbGHCert = NULL;
    }

    //Release Video device
    if (NULL != m_pVideo)
    {
        m_pVideo->Release();
        m_pVideo = NULL;
    }

    //Release Channel device
    if (NULL != m_pChannel)
    {
        m_pChannel->Release();
        m_pChannel = NULL;
    }

    //Release Crypto device
    if (NULL != m_pCryptoSession)
    {
        m_pCryptoSession->Release();
        m_pCryptoSession = NULL;
    }

}   //  CPControl dtor

HRESULT
CPControl::Initialize
(BOOL bTryWithRSAEnc,BOOL bRevoked)
{
    HRESULT         hr = S_OK;
    
    assert(0 != m_pChannel);

    if (0 != m_pbGHCert)
    {
        delete [] m_pbGHCert;
        m_pbGHCert = NULL;
    }


    do
    {
        if (0 == m_pChannel)
        {
            hr = E_UNEXPECTED;
            break;
        }

        if(m_dact != D3DAUTHENTICATEDCHANNEL_D3D9)
        {
            //  key exchange

            if (FAILED(hr = m_pChannel->GetCertificateSize(&m_cbGHCertLen)))
                break;

            m_pbGHCert = new BYTE[m_cbGHCertLen];
            if (m_pbGHCert == NULL)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            if (FAILED(hr = m_pChannel->GetCertificate(m_cbGHCertLen, m_pbGHCert)))
                break;

            
#ifdef _DEBUGFORKEY
            //  initialize
            if (FAILED(hr = m_AuthHelper.Initialize(TestCertOPM, sizeof TestCertOPM, TRUE)))
                break;
#else
            //  initialize
            if (FAILED(hr = m_AuthHelper.Initialize(m_pbGHCert, m_cbGHCertLen, TRUE)))
                break;
#endif

            //fail if bRevoked is TRUE. This is to simulate the code path when certificate is revoked.
            //In reality check for revocation should be done here.
            if(bRevoked)
                return E_FAIL;

            //  start session
            BYTE Signature[256];
            if (FAILED(hr = m_AuthHelper.PrepareAuthenticatedChannelSignature(Signature, 256)))
                break;

            if (FAILED(hr = m_pChannel->NegotiateKeyExchange(sizeof(Signature), Signature)))
                break;

            if (FAILED(hr = m_AuthHelper.GenerateRandomNumber((BYTE*)&m_SequenceQuery, sizeof UINT)))
                break;

            if (FAILED(hr = m_AuthHelper.GenerateRandomNumber((BYTE*)&m_SequenceConfigure, sizeof UINT)))
                break;
            
            if(FAILED(hr = ConfigureInitialize()))
                break;
        }
        
        // Verify that the channel type matches what we created
        D3DAUTHENTICATEDCHANNELTYPE ChannelType;
        if(FAILED(hr = QueryChannelType(&ChannelType)))
            break;
        
        if (ChannelType != m_dact)
        {
            hr = E_FAIL;
            break;
        }

        // Get the driver's device handle
        if(FAILED(hr = QueryDeviceHandle(&m_DeviceHandle)))
            break;

    
    }   while (FALSE);

    return hr;
}   //  CPControl::Initialize

HRESULT
CPControl::InitCryptoSession(GUID* pDecodeGuid)
{
    HRESULT hr = S_OK; 
    BYTE Signature[256];
    BYTE* pCertificate = NULL;
    UINT  CertificateSize = 0;

    do
    {
        if(FAILED(hr = m_pVideo->GetContentProtectionCaps(&D3DCRYPTOTYPE_AES128_CTR, pDecodeGuid, &m_CryptoCaps)))
            break;        
        
        if( FAILED (hr = m_pVideo->CreateCryptoSession(&D3DCRYPTOTYPE_AES128_CTR, pDecodeGuid, &m_pCryptoSession, &m_CryptoSessionHandle)))
            break;
        
        if ( m_CryptoCaps.KeyExchangeType == D3DKEYEXCHANGE_RSAES_OAEP)
        {        
            if( FAILED (hr = m_pCryptoSession->GetCertificateSize(&CertificateSize)))
                break;

            pCertificate = new BYTE[CertificateSize];
            if (pCertificate == NULL)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            if( FAILED (hr = m_pCryptoSession->GetCertificate(CertificateSize, pCertificate)))
                break;
        }

        if( FAILED (hr = m_CryptoHelper.Initialize(pCertificate, CertificateSize, (m_CryptoCaps.KeyExchangeType==D3DKEYEXCHANGE_RSAES_OAEP))))
            break;
        
        if( FAILED (hr = m_CryptoHelper.PrepareAuthenticatedChannelSignature(Signature, 256, m_CryptoCaps.KeyExchangeType)))
            break;

        if( FAILED (hr = m_pCryptoSession->NegotiateKeyExchange(sizeof(Signature), Signature)))
            break;
        
        m_pAESHelper = m_CryptoHelper.GetAESHelper();

    }  while (FALSE);

        //release certificate memory
    if (0 != pCertificate)
    {
        delete [] pCertificate;
        pCertificate = NULL;
    }
    return hr;
}
/******************************************************************/


HRESULT
CPControl::QueryChannelType
(
    D3DAUTHENTICATEDCHANNELTYPE*  pType
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3DAUTHENTICATEDQUERY_CHANNELTYPE;
    
    D3DAUTHENTICATEDCHANNEL_QUERYCHANNELTYPE_OUTPUT QueryChannelTypeOutput;

    hr = Query(&QueryInput,
                sizeof(QueryInput),
                (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryChannelTypeOutput,
                sizeof(QueryChannelTypeOutput));
    if(hr == S_OK)
    {
        assert(NULL != pType);

        *pType = QueryChannelTypeOutput.ChannelType;
    }
    
    return hr;

}   //  CPControl::QueryChannelType


HRESULT
CPControl::QueryDeviceHandle
(   
    HANDLE*              phDevice 
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3DAUTHENTICATEDQUERY_DEVICEHANDLE;
    
    D3DAUTHENTICATEDCHANNEL_QUERYDEVICEHANDLE_OUTPUT QueryDeviceHandleOutput;

    hr = Query(&QueryInput,
                sizeof(QueryInput),
                (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryDeviceHandleOutput,
                sizeof(QueryDeviceHandleOutput));
    if(hr == S_OK)
    {
        assert(NULL != phDevice);

        *phDevice = QueryDeviceHandleOutput.DeviceHandle;
    }
    
    return hr;
}   //  CPControl::QueryDeviceHandle


HRESULT
CPControl::QueryProtectionType
(
    BOOL*              pProtEnabled, 
    BOOL*              pbRequireFlip
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3DAUTHENTICATEDQUERY_PROTECTION;

    D3DAUTHENTICATEDCHANNEL_QUERYPROTECTION_OUTPUT  QueryProtectionOutput;

    hr = Query(&QueryInput,
                sizeof(QueryInput),
                (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryProtectionOutput,
                sizeof(QueryProtectionOutput));
    if(hr == S_OK)
    {
        assert(NULL != pProtEnabled);
        assert(NULL != pbRequireFlip);

        *pProtEnabled = QueryProtectionOutput.ProtectionFlags.ProtectionEnabled;
        *pbRequireFlip = QueryProtectionOutput.ProtectionFlags.OverlayOrFullscreenRequired;
    }
    
    return hr;
}   //  CPControl::QueryProtectionType


HRESULT
CPControl::QueryCryptoSession
(
    HANDLE             DXVA2DecodeHandle,  
    HANDLE*            pCryptoHandle,
    HANDLE*            pDeviceHandle
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERYCRYPTOSESSION_INPUT QueryCryptoSessionInput;
    ZeroMemory(&QueryCryptoSessionInput, sizeof(QueryCryptoSessionInput));
    QueryCryptoSessionInput.Input.QueryType = D3DAUTHENTICATEDQUERY_CRYPTOSESSION;
    QueryCryptoSessionInput.DXVA2DecodeHandle = DXVA2DecodeHandle;

    D3DAUTHENTICATEDCHANNEL_QUERYCRYPTOSESSION_OUTPUT  QueryCryptoSessionOutput;

    hr = Query((D3DAUTHENTICATEDCHANNEL_QUERY_INPUT*)&QueryCryptoSessionInput,
               sizeof(QueryCryptoSessionInput),
               (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryCryptoSessionOutput,
               sizeof(QueryCryptoSessionOutput));
    if(hr == S_OK)
    {
        assert(NULL != pCryptoHandle);
        assert(NULL != pDeviceHandle);

        *pCryptoHandle = QueryCryptoSessionOutput.CryptoSessionHandle;
        *pDeviceHandle = QueryCryptoSessionOutput.DeviceHandle;
    }
    
    return hr;
}   //  CPControl::QueryCryptoSession

HRESULT
CPControl::QueryUnresricttedProtectedsharedResourceCount
(
    UINT*              pResourceCount
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3DAUTHENTICATEDQUERY_UNRESTRICTEDPROTECTEDSHAREDRESOURCECOUNT;

    D3DAUTHENTICATEDCHANNEL_QUERYUNRESTRICTEDPROTECTEDSHAREDRESOURCECOUNT_OUTPUT  QueryResourceCountOutput;

    hr = Query(&QueryInput,
               sizeof(QueryInput),
               (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryResourceCountOutput,
               sizeof(QueryResourceCountOutput));
    if(hr == S_OK)
    {
        assert(NULL != pResourceCount);

        *pResourceCount = QueryResourceCountOutput.NumUnrestrictedProtectedSharedResources;
    }
    
    return hr;
}   //  CPControl::QueryUnresricttedProtectedsharedResourceCount

HRESULT
CPControl::QueryRestrictedSharedResourceProcessCount
(
    UINT*              pProcessCount
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3DAUTHENTICATEDQUERY_RESTRICTEDSHAREDRESOURCEPROCESSCOUNT;

    D3DAUTHENTICATEDCHANNEL_QUERYRESTRICTEDSHAREDRESOURCEPROCESSCOUNT_OUTPUT  QueryProcessCountOutput;

    hr = Query(&QueryInput,
               sizeof(QueryInput),
               (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryProcessCountOutput,
               sizeof(QueryProcessCountOutput));
    if(hr == S_OK)
    {
        assert(NULL != pProcessCount);

        *pProcessCount = QueryProcessCountOutput.NumRestrictedSharedResourceProcesses;
    }
    
    return hr;
}   //  CPControl::QueryRestrictedSharedResourceProcessCount

HRESULT
CPControl::QueryRestrictedSharedResourceProcess
(
    UINT               ProcessIndex,
    UINT*              pProcessIdentifer,
    HANDLE*            pProcessHandle
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERYRESTRICTEDSHAREDRESOURCEPROCESS_INPUT QueryProcessInput;
    ZeroMemory(&QueryProcessInput, sizeof(QueryProcessInput));
    QueryProcessInput.Input.QueryType = D3DAUTHENTICATEDQUERY_RESTRICTEDSHAREDRESOURCEPROCESS;
    QueryProcessInput.ProcessIndex = ProcessIndex;

    D3DAUTHENTICATEDCHANNEL_QUERYRESTRICTEDSHAREDRESOURCEPROCESS_OUTPUT  QueryProcessOutput;

    hr = Query((D3DAUTHENTICATEDCHANNEL_QUERY_INPUT*)&QueryProcessInput,
               sizeof(QueryProcessInput),
               (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryProcessOutput,
               sizeof(QueryProcessOutput));
    if(hr == S_OK)
    {
        assert(NULL != pProcessIdentifer);
        assert(NULL != pProcessHandle);

        *pProcessHandle = QueryProcessOutput.ProcessHandle;
        *pProcessIdentifer = (UINT)QueryProcessOutput.ProcessIdentifer;
    }
    
    return hr;
}   //  CPControl::QueryRestrictedSharedResourceProcess

HRESULT
CPControl::QueryOutputIdCount
(
    HANDLE            DeviceHandle,
    HANDLE            CryptoSessionHandle,
    UINT*             pOutputIdCount
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERYOUTPUTIDCOUNT_INPUT QueryIdCountInput;
    ZeroMemory(&QueryIdCountInput, sizeof(QueryIdCountInput));
    QueryIdCountInput.Input.QueryType = D3DAUTHENTICATEDQUERY_OUTPUTIDCOUNT;
    QueryIdCountInput.DeviceHandle = DeviceHandle;
    QueryIdCountInput.CryptoSessionHandle = CryptoSessionHandle;

    D3DAUTHENTICATEDCHANNEL_QUERYOUTPUTIDCOUNT_OUTPUT  QueryIdCountOutput;

    hr = Query((D3DAUTHENTICATEDCHANNEL_QUERY_INPUT*)&QueryIdCountInput,
               sizeof(QueryIdCountInput),
               (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryIdCountOutput,
               sizeof(QueryIdCountOutput));
    if(hr == S_OK)
    {
        assert(NULL != pOutputIdCount);

        *pOutputIdCount = QueryIdCountOutput.NumOutputIDs;
    }
    
    return hr;
}   //  CPControl::QueryOutputIdCount


HRESULT
CPControl::QueryOutputId
(
    HANDLE            DeviceHandle,
    HANDLE            CryptoSessionHandle,
    UINT              OutputIDIndex,
    UINT64*           pOutputID  
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERYOUTPUTID_INPUT QueryOutputIdInput;
    ZeroMemory(&QueryOutputIdInput, sizeof(QueryOutputIdInput));
    QueryOutputIdInput.Input.QueryType = D3DAUTHENTICATEDQUERY_OUTPUTID;
    QueryOutputIdInput.DeviceHandle = DeviceHandle;
    QueryOutputIdInput.CryptoSessionHandle = CryptoSessionHandle;
    QueryOutputIdInput.OutputIDIndex = OutputIDIndex;

    D3DAUTHENTICATEDCHANNEL_QUERYOUTPUTID_OUTPUT  QueryOutputIdOutput;

    hr = Query((D3DAUTHENTICATEDCHANNEL_QUERY_INPUT*)&QueryOutputIdInput,
               sizeof(QueryOutputIdInput),
               (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryOutputIdOutput,
               sizeof(QueryOutputIdOutput));
    if(hr == S_OK)
    {
        assert(NULL != pOutputID);

        *pOutputID = QueryOutputIdOutput.OutputID;
    }
    
    return hr;
}   //  CPControl::QueryOutputId


HRESULT
CPControl::QueryBusData
(
    D3DBUSTYPE*    pBusType,
    BOOL*          pAccessNonContigBlocks,
    BOOL*          pAccessContigBlocks
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3DAUTHENTICATEDQUERY_ACCESSIBILITYATTRIBUTES;

    D3DAUTHENTICATEDCHANNEL_QUERYINFOBUSTYPE_OUTPUT  QueryBusTypeOutput;

    hr = Query(&QueryInput,
               sizeof(QueryInput),
               (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryBusTypeOutput,
               sizeof(QueryBusTypeOutput));
    if(hr == S_OK)
    {
        assert(NULL != pBusType);
        assert(NULL != pAccessNonContigBlocks);
        assert(NULL != pAccessContigBlocks);

        *pBusType = QueryBusTypeOutput.BusType;
        *pAccessContigBlocks = QueryBusTypeOutput.bAccessibleInContiguousBlocks;
        *pAccessNonContigBlocks = QueryBusTypeOutput.bAccessibleInNonContiguousBlocks;

    }
    
    return hr;
}   //  CPControl::QueryBusData


HRESULT
CPControl::QueryEvictionEncryptionGuidCount
(
    UINT*    pEncryptionGuidCount
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3DAUTHENTICATEDQUERY_ENCRYPTIONWHENACCESSIBLEGUIDCOUNT;

    D3DAUTHENTICATEDCHANNEL_QUERYEVICTIONENCRYPTIONGUIDCOUNT_OUTPUT  QueryEvicryptGuidCountOutput;

    hr = Query(&QueryInput,
               sizeof(QueryInput),
               (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryEvicryptGuidCountOutput,
               sizeof(QueryEvicryptGuidCountOutput));
    if(hr == S_OK)
    {
        assert(NULL != pEncryptionGuidCount);

        *pEncryptionGuidCount = QueryEvicryptGuidCountOutput.NumEncryptionGuids;
    }
    
    return hr;
}   //  CPControl::QueryEvictionEncryptionGuidCount


HRESULT
CPControl::QueryEvictionEncryptionGuid
(
    UINT         EncryptionGuidIndex,
    GUID*      pEncryptionGuid
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERYEVICTIONENCRYPTIONGUID_INPUT QueryEvicryptGuidInput;
    ZeroMemory(&QueryEvicryptGuidInput, sizeof(QueryEvicryptGuidInput));
    QueryEvicryptGuidInput.Input.QueryType = D3DAUTHENTICATEDQUERY_ENCRYPTIONWHENACCESSIBLEGUID;
    QueryEvicryptGuidInput.EncryptionGuidIndex = EncryptionGuidIndex ;

    D3DAUTHENTICATEDCHANNEL_QUERYEVICTIONENCRYPTIONGUID_OUTPUT  QueryEvicryptGuidOutput;

    hr = Query((D3DAUTHENTICATEDCHANNEL_QUERY_INPUT*)&QueryEvicryptGuidInput,
               sizeof(QueryEvicryptGuidInput),
               (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryEvicryptGuidOutput,
               sizeof(QueryEvicryptGuidOutput));
    if(hr == S_OK)
    {
        assert(NULL != pEncryptionGuid);

        *pEncryptionGuid = QueryEvicryptGuidOutput.EncryptionGuid;
    }
    
    return hr;
}   //  CPControl::QueryEvictionEncryptionGuid


HRESULT
CPControl::QueryCurrentEvictionEncryption
(
    GUID*    pEncryptionGuid
)
{
    HRESULT						hr = S_OK;
    D3DAUTHENTICATEDCHANNEL_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3DAUTHENTICATEDQUERY_CURRENTENCRYPTIONWHENACCESSIBLE;

    D3DAUTHENTICATEDCHANNEL_QUERYUNCOMPRESSEDENCRYPTIONLEVEL_OUTPUT  QueryCurrEvicryptOutput;

    hr = Query(&QueryInput,
               sizeof(QueryInput),
               (D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT*)&QueryCurrEvicryptOutput,
               sizeof(QueryCurrEvicryptOutput));
    if(hr == S_OK)
    {
        assert(NULL != pEncryptionGuid);

        *pEncryptionGuid = QueryCurrEvicryptOutput.EncryptionGuid;
    }
    
    return hr;
}   //  CPControl::QueryCurrentEvictionEncryption


/******************************************************************/

HRESULT
CPControl::ConfigureInitialize
()
{
    D3DAUTHENTICATEDCHANNEL_CONFIGUREINITIALIZE ConfigureInitialize;
    ZeroMemory(&ConfigureInitialize, sizeof(ConfigureInitialize));
    ConfigureInitialize.Parameters.ConfigureType = D3DAUTHENTICATEDCONFIGURE_INITIALIZE;
    ConfigureInitialize.StartSequenceQuery = m_SequenceQuery;
    ConfigureInitialize.StartSequenceConfigure = m_SequenceConfigure;
 
    return Configure((D3DAUTHENTICATEDCHANNEL_CONFIGURE_INPUT*)&ConfigureInitialize,
                      sizeof(ConfigureInitialize));
}   //  CPControl::ConfigureInitialize


HRESULT
CPControl::ConfigureProtection
(
    BOOL            bProtectionEnabled,
    BOOL            bRequireFlip
)
{
    D3DAUTHENTICATEDCHANNEL_CONFIGUREPROTECTION ConfigureProtection;
    ZeroMemory(&ConfigureProtection, sizeof(ConfigureProtection));
    ConfigureProtection.Parameters.ConfigureType = D3DAUTHENTICATEDCONFIGURE_PROTECTION;
    ConfigureProtection.Protections.ProtectionEnabled = bProtectionEnabled;
    ConfigureProtection.Protections.OverlayOrFullscreenRequired = bRequireFlip;
 
    return Configure((D3DAUTHENTICATEDCHANNEL_CONFIGURE_INPUT*)&ConfigureProtection,
                      sizeof(ConfigureProtection));
}   //  CPControl::ConfigureProtection


HRESULT
CPControl::ConfigureCryptoSession
(
    HANDLE            hDXVA2Decode,
    HANDLE            hCryptoSession,
    HANDLE            hDevice
)
{
    D3DAUTHENTICATEDCHANNEL_CONFIGURECRYPTOSESSION ConfigureCryptoSession;
    ZeroMemory(&ConfigureCryptoSession, sizeof(ConfigureCryptoSession));
    ConfigureCryptoSession.Parameters.ConfigureType = D3DAUTHENTICATEDCONFIGURE_CRYPTOSESSION;
    ConfigureCryptoSession.DXVA2DecodeHandle = hDXVA2Decode;
    ConfigureCryptoSession.CryptoSessionHandle = hCryptoSession;
    ConfigureCryptoSession.DeviceHandle = hDevice;
 
    return Configure((D3DAUTHENTICATEDCHANNEL_CONFIGURE_INPUT*)&ConfigureCryptoSession,
                      sizeof(ConfigureCryptoSession));
}   //  CPControl::ConfigureCryptoSession


HRESULT
CPControl::ConfigureSharedResource
(
    HANDLE          hProcess,
    BOOL            bDWMProc,
    BOOL            bAllowed
)
{
    D3DAUTHENTICATEDCHANNEL_CONFIGURESHAREDRESOURCE ConfigureSharedResource;
    ZeroMemory(&ConfigureSharedResource, sizeof(ConfigureSharedResource));
    ConfigureSharedResource.Parameters.ConfigureType = D3DAUTHENTICATEDCONFIGURE_SHAREDRESOURCE;
    
    if(bDWMProc)
        ConfigureSharedResource.ProcessIdentiferType = PROCESSIDTYPE_DWM;    
    else
        ConfigureSharedResource.ProcessIdentiferType = PROCESSIDTYPE_HANDLE;    

    ConfigureSharedResource.ProcessHandle = hProcess;
    ConfigureSharedResource.AllowAccess = bAllowed;
 
    return Configure((D3DAUTHENTICATEDCHANNEL_CONFIGURE_INPUT*)&ConfigureSharedResource,
                      sizeof(ConfigureSharedResource));
}   //  CPControl::ConfigureSharedResource


HRESULT
CPControl::ConfigureEvictionCryption
(
    GUID            EncryptionGuid
)
{
    D3DAUTHENTICATEDCHANNEL_CONFIGUREUNCOMPRESSEDENCRYPTION ConfigureEvictionCryption;
    ZeroMemory(&ConfigureEvictionCryption, sizeof(ConfigureEvictionCryption));
    ConfigureEvictionCryption.Parameters.ConfigureType = D3DAUTHENTICATEDCONFIGURE_ENCRYPTIONWHENACCESSIBLE;
    ConfigureEvictionCryption.EncryptionGuid = EncryptionGuid;
 
    return Configure((D3DAUTHENTICATEDCHANNEL_CONFIGURE_INPUT*)&ConfigureEvictionCryption,
                      sizeof(ConfigureEvictionCryption));
}   //  CPControl::ConfigureEvictionCryption


// Caps
HRESULT 
CPControl::InitializeContentProtectionCaps(GUID* pCryptoType, GUID* pDecodeProfile)
{
    HRESULT             hr = S_OK;

    if( NULL == m_pVideo )
        return E_FAIL;

    if( FAILED(hr = m_pVideo->GetContentProtectionCaps(pCryptoType, pDecodeProfile, &m_ContentProtectionCaps)))
        return hr ;

    if( FAILED(hr = m_pVideo->GetContentProtectionCaps(&D3DCRYPTOTYPE_AES128_CTR, pDecodeProfile, &m_CryptoCaps)))
        return hr ;

    return hr;
}

BOOL 
CPControl::SoftwareProtectionSupported()
{
    if ((m_ContentProtectionCaps.Caps & D3DCPCAPS_SOFTWARE) == 0)
    {
        return FALSE;
    }
    if (m_ContentProtectionCaps.ProtectedMemorySize == 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL 
CPControl::HardwareProtectionSupported()
{
    if ((m_ContentProtectionCaps.Caps & D3DCPCAPS_HARDWARE) == 0)
    {
        return FALSE;
    }
    if (m_ContentProtectionCaps.ProtectedMemorySize == 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL 
CPControl::HardwareProtectionAlwaysOn()
{
    if ((m_ContentProtectionCaps.Caps & D3DCPCAPS_HARDWARE) == 0)
    {
        return FALSE;
    }
    if ((m_ContentProtectionCaps.Caps & D3DCPCAPS_PROTECTIONALWAYSON) == 0)
    {
        return FALSE;
    }
    if (m_ContentProtectionCaps.ProtectedMemorySize == 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL 
CPControl::DriverProtectionSupported()
{
    if ((m_ContentProtectionCaps.Caps & (D3DCPCAPS_SOFTWARE | D3DCPCAPS_HARDWARE)) == 0)
    {
        return FALSE;
    }
    if (m_ContentProtectionCaps.ProtectedMemorySize == 0)
    {
        return FALSE;
    }
    return TRUE;
}

/******************************************************************/

HRESULT 
CPControl::Query
(
    D3DAUTHENTICATEDCHANNEL_QUERY_INPUT*  pInput,
    UINT                                  InputSize,
    D3DAUTHENTICATEDCHANNEL_QUERY_OUTPUT* pOutput,
    UINT                                  OutputSize
)
{
    HRESULT             hr = S_OK;

    //  params
    if ((0 == pInput) || (InputSize == 0)) 
        return E_INVALIDARG;

    assert(0 != m_pChannel);

    ZeroMemory(pOutput, OutputSize);
    pInput->hChannel = m_hChannel;
    pInput->SequenceNumber = m_SequenceQuery;

    //  Proper Querqy call
    hr = m_pChannel->Query(InputSize, pInput, OutputSize, pOutput);
    m_SequenceQuery++; //  update seq number
    if (FAILED(hr))
    {
        return hr;
    }

    if(m_dact != D3DAUTHENTICATEDCHANNEL_D3D9)
    {
        //  verify signature
        if (FAILED(hr = m_AuthHelper.VerifySignature((BYTE*)pOutput + sizeof(D3D_OMAC),
                                                     OutputSize - sizeof(D3D_OMAC),
                                                     (BYTE*)pOutput, 
                                                     sizeof(D3D_OMAC))))
        {
            return hr;
        }

        if(pOutput->SequenceNumber != pInput->SequenceNumber)
        {
            return E_FAIL;
        }
    }

    if ((pOutput->hChannel != pInput->hChannel) ||          
        (pOutput->QueryType != pInput->QueryType))
    {            
        return E_FAIL;
    }

    return pOutput->ReturnCode;
}   //  CPControl::Query


HRESULT 
CPControl::Configure
(
    D3DAUTHENTICATEDCHANNEL_CONFIGURE_INPUT*   pInput, 
    UINT                                       InputSize
)
{
    D3DAUTHENTICATEDCHANNEL_CONFIGURE_OUTPUT Output;
    HRESULT         hr = S_OK;
    assert(0 != m_pChannel);


    if ((0 == pInput) || (InputSize == 0))
    {
        return E_INVALIDARG;            
    }

    //  fill in data
    ZeroMemory(&Output, sizeof(Output));
    pInput->hChannel = m_hChannel;
    pInput->SequenceNumber = m_SequenceConfigure;
    
    if(m_dact != D3DAUTHENTICATEDCHANNEL_D3D9)
    {
        //  sign command struct
        if (FAILED(hr = m_AuthHelper.Sign((BYTE*)pInput + sizeof(D3D_OMAC),
                                          InputSize - sizeof(D3D_OMAC),
                                          (BYTE*)pInput,
                                          sizeof(D3D_OMAC))))
        {
            return hr;
        }
    }

    //  Proper Configure call
    hr = m_pChannel->Configure(InputSize, pInput, &Output);
    m_SequenceConfigure++; //  update seq number
    if (FAILED(hr))
    {
        return hr;
    }

    if(m_dact != D3DAUTHENTICATEDCHANNEL_D3D9)
    {
        //  verify signature
        if (FAILED(hr = m_AuthHelper.VerifySignature((BYTE*)&Output + sizeof(D3D_OMAC),
                                                     sizeof (Output) - sizeof(D3D_OMAC),
                                                     (BYTE*)&(Output.omac), 
                                                     sizeof(D3D_OMAC))))
        {
            return hr;
        }

        if(Output.SequenceNumber != pInput->SequenceNumber)
        {
            return E_FAIL;
        }
    }

    if ((Output.hChannel != pInput->hChannel) || 
        (Output.ConfigureType != pInput->ConfigureType))
    {
        return E_FAIL;            
    }       

    return Output.ReturnCode;
}   //  CPControl::Configure


/////////////////////////////////////// CPControl11 ///////////////////////////////////////
CPControl11::CPControl11
(
    ID3D11Device*        pParent,
    D3D11_AUTHENTICATED_CHANNEL_TYPE dact,
    HRESULT*        phr
):  m_SequenceQuery(0),
    m_SequenceConfigure(0),
    m_pbGHCert(NULL),
    m_cbGHCertLen(0),
    m_pParent11(pParent),
    m_pVideo11(NULL),
    m_pChannel11(NULL),
    m_pCryptoSession11(NULL),
    m_hChannel11(0),
    m_dact11(dact),
    m_pVideoContext11(NULL),
    m_pDeviceContext11(NULL),
    m_DeviceHandle(NULL)
{
    ZeroMemory(&m_ContentProtectionCaps, sizeof(m_ContentProtectionCaps));

    OutputDebugString(TEXT("CPControl11::CPControl11)\n"));
    do
    {
        if ((0 == pParent) || (0 == phr))
            break;

        if (FAILED(*phr))
            break;

        pParent->GetImmediateContext(&m_pDeviceContext11);

        if(NULL == m_pDeviceContext11)
        {
            *phr = E_FAIL;
            break;
        }

        *phr = pParent->QueryInterface(IID_ID3D11VideoDevice, (void**)&m_pVideo11);
        if (FAILED(*phr))
            break;

        *phr = m_pDeviceContext11->QueryInterface(IID_ID3D11VideoContext, (void**)&m_pVideoContext11);
        if(FAILED(*phr))
            break;
   
        *phr = m_pVideo11->CreateAuthenticatedChannel(dact, &m_pChannel11);
        if (FAILED(*phr))
            break;

        m_pChannel11->GetChannelHandle(&m_hChannel11);
        if(NULL == m_hChannel11)
        {
            *phr = E_FAIL;
            break;
        }

        InitializeContentProtectionCaps();

    }while(FALSE);

}

CPControl11::~CPControl11
()
{
    OutputDebugString(TEXT("CPControl11::~CPControl11 \n"));
    
    //release certificate memory
    if (0 != m_pbGHCert)
    {
        delete [] m_pbGHCert;
        m_pbGHCert = NULL;
    }

    //Release Video device
    if(NULL != m_pVideo11)
    {
        m_pVideo11->Release();
        m_pVideo11 = NULL;
    }

    //Release Channel device
    if (NULL != m_pChannel11)
    {
        m_pChannel11->Release();
        m_pChannel11 = NULL;
    }

    // release contexts
    if(NULL != m_pDeviceContext11)
    {
        m_pDeviceContext11->Release();
        m_pDeviceContext11 = NULL;
    }

    if(NULL != m_pVideoContext11)
    {
        m_pVideoContext11->Release();
        m_pVideoContext11 = NULL;
    }

    //Release Crypto device
    if (NULL != m_pCryptoSession11)
    {
        m_pCryptoSession11->Release();
        m_pCryptoSession11 = NULL;
    }


}   //  CPControl11 dtor


HRESULT
CPControl11::Initialize
(BOOL bTryWithRSAEnc,BOOL bRevoked)
{
    HRESULT         hr = S_OK;
    
    assert(0 != m_pChannel11);

    if (0 != m_pbGHCert)
    {
        delete [] m_pbGHCert;
        m_pbGHCert = NULL;
    }


    do
    {
        if (0 == m_pChannel11)
        {
            hr = E_UNEXPECTED;
            break;
        }

        if(m_dact11 != D3D11_AUTHENTICATED_CHANNEL_D3D11)
        {
            //  key exchange

            if (FAILED(hr = m_pChannel11->GetCertificateSize(&m_cbGHCertLen)))
                break;

            m_pbGHCert = new BYTE[m_cbGHCertLen];
            if (m_pbGHCert == NULL)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            if (FAILED(hr = m_pChannel11->GetCertificate(m_cbGHCertLen, m_pbGHCert)))
                break;
            
    #ifdef _DEBUGFORKEY
            //  initialize
            if (FAILED(hr = m_AuthHelper.Initialize(TestCertOPM, sizeof TestCertOPM, TRUE)))
                break;
    #else
            //  initialize
            if (FAILED(hr = m_AuthHelper.Initialize(m_pbGHCert, m_cbGHCertLen, TRUE)))
                break;
    #endif

            //fail if bRevoked is TRUE. This is to simulate the code path when certificate is revoked.
            //In reality check for revocation should be done here.
            if(bRevoked)
                return E_FAIL;

            //  start session
            BYTE Signature[256];
            if (FAILED(hr = m_AuthHelper.PrepareAuthenticatedChannelSignature(Signature, 256)))
                break;


            if(FAILED(hr = m_pVideoContext11->NegotiateAuthenticatedChannelKeyExchange(m_pChannel11, sizeof(Signature), Signature)))
                break;

            if (FAILED(hr = m_AuthHelper.GenerateRandomNumber((BYTE*)&m_SequenceQuery, sizeof UINT)))
                break;

            if (FAILED(hr = m_AuthHelper.GenerateRandomNumber((BYTE*)&m_SequenceConfigure, sizeof UINT)))
                break;
            
            if(FAILED(hr = ConfigureInitialize()))
                break;
        }

        

        // Verify that the channel type matches what we created
        D3D11_AUTHENTICATED_CHANNEL_TYPE ChannelType;
        if(FAILED(hr = QueryChannelType(&ChannelType)))
            break;
        
        if (ChannelType != m_dact11)
        {
            hr = E_FAIL;
            break;
        }

        // Get the driver's device handle
        if(FAILED(hr = QueryDeviceHandle(&m_DeviceHandle)))
            break;
    
    }   while (FALSE);

    return hr;
}   //  CPControl11::Initialize


HRESULT
CPControl11::InitCryptoSession(GUID* pDecodeGuid)
{
    HRESULT hr = S_OK; 
    BYTE Signature[256];
    BYTE* pCertificate = NULL;
    UINT  CertificateSize = 0;
    GUID KeyExchange;

    do
    {
        if(FAILED(hr = m_pVideo11->GetContentProtectionCaps(&D3D11_CRYPTO_TYPE_AES128_CTR, pDecodeGuid, &m_CryptoCaps)))
            break;      

        if (FAILED(hr = m_pVideo11->CheckCryptoKeyExchange(&D3D11_CRYPTO_TYPE_AES128_CTR, pDecodeGuid, 0, &KeyExchange)))
            break;

        if( FAILED (hr = m_pVideo11->CreateCryptoSession(&D3D11_CRYPTO_TYPE_AES128_CTR, pDecodeGuid, &KeyExchange, &m_pCryptoSession11)))
            break;

        m_pCryptoSession11->GetCryptoSessionHandle(&m_CryptoSessionHandle11);
        
        if ( KeyExchange == D3D11_KEY_EXCHANGE_RSAES_OAEP)
        {        
            if( FAILED (hr = m_pCryptoSession11->GetCertificateSize(&CertificateSize)))
                break;

            pCertificate = new BYTE[CertificateSize];
            if (pCertificate == NULL)
            {
                hr = E_OUTOFMEMORY;
                break;
            }

            if( FAILED (hr = m_pCryptoSession11->GetCertificate(CertificateSize, pCertificate)))
                break;
        }

        if( FAILED (hr = m_CryptoHelper.Initialize(pCertificate, CertificateSize, (KeyExchange==D3D11_KEY_EXCHANGE_RSAES_OAEP))))
            break;
        
        if( FAILED (hr = m_CryptoHelper.PrepareAuthenticatedChannelSignature(Signature, 256, KeyExchange)))
            break;

        if( FAILED (hr = m_pVideoContext11->NegotiateCryptoSessionKeyExchange(m_pCryptoSession11, sizeof(Signature), Signature)))
            break;
        
        m_pAESHelper = m_CryptoHelper.GetAESHelper();

    }  while (FALSE);

        //release certificate memory
    if (0 != pCertificate)
    {
        delete [] pCertificate;
        pCertificate = NULL;
    }
    return hr;
}



/**************************************************************/

HRESULT
CPControl11::QueryProtection
(
    BOOL*              pbProtEnabled, 
    BOOL*              pbOverlayOrFullscreenRequired
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3D11_AUTHENTICATED_QUERY_PROTECTION;

    D3D11_AUTHENTICATED_QUERY_PROTECTION_OUTPUT  QueryProtectionOutput;

    hr = Query(&QueryInput,
                sizeof(QueryInput),
                (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryProtectionOutput,
                sizeof(QueryProtectionOutput));
    if(hr == S_OK)
    {
        assert(NULL != pbProtEnabled);
        assert(NULL != pbOverlayOrFullscreenRequired);

        *pbProtEnabled = QueryProtectionOutput.ProtectionFlags.Flags.ProtectionEnabled;
        *pbOverlayOrFullscreenRequired = QueryProtectionOutput.ProtectionFlags.Flags.OverlayOrFullscreenRequired;
    }
    
    return hr;
}   //  CPControl11::QueryProtectionType



HRESULT
CPControl11::QueryChannelType
(
    D3D11_AUTHENTICATED_CHANNEL_TYPE*  pType
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3D11_AUTHENTICATED_QUERY_CHANNEL_TYPE;
    
    D3D11_AUTHENTICATED_QUERY_CHANNEL_TYPE_OUTPUT QueryChannelTypeOutput;

    hr = Query(&QueryInput,
                sizeof(QueryInput),
                (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryChannelTypeOutput,
                sizeof(QueryChannelTypeOutput));
    if(hr == S_OK)
    {
        assert(NULL != pType);

        *pType = QueryChannelTypeOutput.ChannelType;
    }
    
    return hr;

}   //  CPControl11::QueryChannelType


HRESULT
CPControl11::QueryDeviceHandle
(   
    HANDLE*              phDevice 
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3D11_AUTHENTICATED_QUERY_DEVICE_HANDLE;
    
    D3D11_AUTHENTICATED_QUERY_DEVICE_HANDLE_OUTPUT QueryDeviceHandleOutput;

    hr = Query(&QueryInput,
                sizeof(QueryInput),
                (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryDeviceHandleOutput,
                sizeof(QueryDeviceHandleOutput));
    if(hr == S_OK)
    {
        assert(NULL != phDevice);

        *phDevice = QueryDeviceHandleOutput.DeviceHandle;
    }
    
    return hr;
}   //  CPControl11::QueryDeviceHandle




HRESULT
CPControl11::QueryCryptoSession
(
    HANDLE             DXVA2DecodeHandle,  
    HANDLE*            pCryptoHandle,
    HANDLE*            pDeviceHandle
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_CRYPTO_SESSION_INPUT QueryCryptoSessionInput;
    ZeroMemory(&QueryCryptoSessionInput, sizeof(QueryCryptoSessionInput));
    QueryCryptoSessionInput.Input.QueryType = D3D11_AUTHENTICATED_QUERY_CRYPTO_SESSION;
    QueryCryptoSessionInput.DecoderHandle = DXVA2DecodeHandle;

    D3D11_AUTHENTICATED_QUERY_CRYPTO_SESSION_OUTPUT  QueryCryptoSessionOutput;
    ZeroMemory(&QueryCryptoSessionOutput, sizeof(QueryCryptoSessionOutput));

    hr = Query((D3D11_AUTHENTICATED_QUERY_INPUT*)&QueryCryptoSessionInput,
               sizeof(QueryCryptoSessionInput),
               (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryCryptoSessionOutput,
               sizeof(QueryCryptoSessionOutput));
    if(hr == S_OK)
    {
        assert(NULL != pCryptoHandle);
        assert(NULL != pDeviceHandle);

        *pCryptoHandle = QueryCryptoSessionOutput.CryptoSessionHandle;
        *pDeviceHandle = QueryCryptoSessionOutput.DeviceHandle;
    }
    
    return hr;
}   //  CPControl11::QueryCryptoSession


HRESULT
CPControl11::QueryRestrictedSharedResourceProcessCount
(
    UINT*              pProcessCount
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3D11_AUTHENTICATED_QUERY_RESTRICTED_SHARED_RESOURCE_PROCESS_COUNT;

    D3D11_AUTHENTICATED_QUERY_RESTRICTED_SHARED_RESOURCE_PROCESS_COUNT_OUTPUT  QueryProcessCountOutput;

    hr = Query(&QueryInput,
               sizeof(QueryInput),
               (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryProcessCountOutput,
               sizeof(QueryProcessCountOutput));
    if(hr == S_OK)
    {
        assert(NULL != pProcessCount);

        *pProcessCount = QueryProcessCountOutput.RestrictedSharedResourceProcessCount;
    }
    
    return hr;
}   //  CPControl::QueryRestrictedSharedResourceProcessCount


HRESULT
CPControl11::QueryRestrictedSharedResourceProcess
(
    UINT               ProcessIndex,
    UINT*              pProcessIdentifer,
    HANDLE*            pProcessHandle
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_RESTRICTED_SHARED_RESOURCE_PROCESS_INPUT QueryProcessInput;
    ZeroMemory(&QueryProcessInput, sizeof(QueryProcessInput));
    QueryProcessInput.Input.QueryType = D3D11_AUTHENTICATED_QUERY_RESTRICTED_SHARED_RESOURCE_PROCESS;
    QueryProcessInput.ProcessIndex = ProcessIndex;

    D3D11_AUTHENTICATED_QUERY_RESTRICTED_SHARED_RESOURCE_PROCESS_OUTPUT  QueryProcessOutput;

    hr = Query((D3D11_AUTHENTICATED_QUERY_INPUT*)&QueryProcessInput,
               sizeof(QueryProcessInput),
               (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryProcessOutput,
               sizeof(QueryProcessOutput));
    if(hr == S_OK)
    {
        assert(NULL != pProcessIdentifer);
        assert(NULL != pProcessHandle);

        *pProcessHandle = QueryProcessOutput.ProcessHandle;
        *pProcessIdentifer = (UINT)QueryProcessOutput.ProcessIdentifier;
    }
    
    return hr;
}   //  CPControl11::QueryRestrictedSharedResourceProcess


HRESULT
CPControl11::QueryUnresricttedProtectedsharedResourceCount
(
    UINT*              pResourceCount
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3D11_AUTHENTICATED_QUERY_UNRESTRICTED_PROTECTED_SHARED_RESOURCE_COUNT;

    D3D11_AUTHENTICATED_QUERY_UNRESTRICTED_PROTECTED_SHARED_RESOURCE_COUNT_OUTPUT  QueryResourceCountOutput;

    hr = Query(&QueryInput,
               sizeof(QueryInput),
               (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryResourceCountOutput,
               sizeof(QueryResourceCountOutput));
    if(hr == S_OK)
    {
        assert(NULL != pResourceCount);

        *pResourceCount = QueryResourceCountOutput.UnrestrictedProtectedSharedResourceCount;
    }
    
    return hr;
}   //  CPControl11::QueryUnresricttedProtectedsharedResourceCount



HRESULT
CPControl11::QueryOutputIdCount
(
    HANDLE            DeviceHandle,
    HANDLE            CryptoSessionHandle,
    UINT*             pOutputIdCount
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_OUTPUT_ID_COUNT_INPUT QueryIdCountInput;
    ZeroMemory(&QueryIdCountInput, sizeof(QueryIdCountInput));
    QueryIdCountInput.Input.QueryType = D3D11_AUTHENTICATED_QUERY_OUTPUT_ID_COUNT;
    QueryIdCountInput.DeviceHandle = DeviceHandle;
    QueryIdCountInput.CryptoSessionHandle = CryptoSessionHandle;

    D3D11_AUTHENTICATED_QUERY_OUTPUT_ID_COUNT_OUTPUT  QueryIdCountOutput;

    hr = Query((D3D11_AUTHENTICATED_QUERY_INPUT*)&QueryIdCountInput,
               sizeof(QueryIdCountInput),
               (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryIdCountOutput,
               sizeof(QueryIdCountOutput));
    if(hr == S_OK)
    {
        assert(NULL != pOutputIdCount);

        *pOutputIdCount = QueryIdCountOutput.OutputIDCount;
    }
    
    return hr;
}   //  CPControl11::QueryOutputIdCount


HRESULT
CPControl11::QueryOutputId
(
    HANDLE            DeviceHandle,
    HANDLE            CryptoSessionHandle,
    UINT              OutputIDIndex,
    UINT64*           pOutputID  
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_OUTPUT_ID_INPUT QueryOutputIdInput;
    ZeroMemory(&QueryOutputIdInput, sizeof(QueryOutputIdInput));
    QueryOutputIdInput.Input.QueryType = D3DAUTHENTICATEDQUERY_OUTPUTID;
    QueryOutputIdInput.DeviceHandle = DeviceHandle;
    QueryOutputIdInput.CryptoSessionHandle = CryptoSessionHandle;
    QueryOutputIdInput.OutputIDIndex = OutputIDIndex;

    D3D11_AUTHENTICATED_QUERY_OUTPUT_ID_OUTPUT  QueryOutputIdOutput;

    hr = Query((D3D11_AUTHENTICATED_QUERY_INPUT*)&QueryOutputIdInput,
               sizeof(QueryOutputIdInput),
               (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryOutputIdOutput,
               sizeof(QueryOutputIdOutput));
    if(hr == S_OK)
    {
        assert(NULL != pOutputID);

        *pOutputID = QueryOutputIdOutput.OutputID;
    }
    
    return hr;
}   //  CPControl11::QueryOutputId


HRESULT
CPControl11::QueryAccessibilityAttributes
(
    D3D11_BUS_TYPE*    pBusType,
    BOOL*              pAccessNonContigBlocks,
    BOOL*              pAccessContigBlocks
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3D11_AUTHENTICATED_QUERY_ACCESSIBILITY_ATTRIBUTES;

    D3D11_AUTHENTICATED_QUERY_ACCESSIBILITY_OUTPUT  QueryBusTypeOutput;

    hr = Query(&QueryInput,
               sizeof(QueryInput),
               (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryBusTypeOutput,
               sizeof(QueryBusTypeOutput));
    if(hr == S_OK)
    {
        assert(NULL != pBusType);
        assert(NULL != pAccessNonContigBlocks);
        assert(NULL != pAccessContigBlocks);

        *pBusType = QueryBusTypeOutput.BusType;
        *pAccessContigBlocks = QueryBusTypeOutput.AccessibleInContiguousBlocks;
        *pAccessNonContigBlocks = QueryBusTypeOutput.AccessibleInNonContiguousBlocks;

    }
    
    return hr;
}   //  CPControl11::QueryAccessibilityAttributes


HRESULT
CPControl11::QueryEncryptionWhenAccessibleGuidCount
(
    UINT*    pEncryptionGuidCount
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3D11_AUTHENTICATED_QUERY_ENCRYPTION_WHEN_ACCESSIBLE_GUID_COUNT;

    D3D11_AUTHENTICATED_QUERY_ACCESSIBILITY_ENCRYPTION_GUID_COUNT_OUTPUT  QueryEvicryptGuidCountOutput;

    hr = Query(&QueryInput,
               sizeof(QueryInput),
               (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryEvicryptGuidCountOutput,
               sizeof(QueryEvicryptGuidCountOutput));
    if(hr == S_OK)
    {
        assert(NULL != pEncryptionGuidCount);

        *pEncryptionGuidCount = QueryEvicryptGuidCountOutput.EncryptionGuidCount;
    }
    
    return hr;
}   //  CPControl11::QueryEncryptionWhenAccessibleGuidCount


HRESULT
CPControl11::QueryEncryptionWhenAccessibleGuid
(
    UINT         EncryptionGuidIndex,
    GUID*      pEncryptionGuid
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_ACCESSIBILITY_ENCRYPTION_GUID_INPUT QueryEvicryptGuidInput;
    ZeroMemory(&QueryEvicryptGuidInput, sizeof(QueryEvicryptGuidInput));
    QueryEvicryptGuidInput.Input.QueryType = D3D11_AUTHENTICATED_QUERY_ENCRYPTION_WHEN_ACCESSIBLE_GUID;
    QueryEvicryptGuidInput.EncryptionGuidIndex = EncryptionGuidIndex ;

    D3D11_AUTHENTICATED_QUERY_ACCESSIBILITY_ENCRYPTION_GUID_OUTPUT  QueryEvicryptGuidOutput;

    hr = Query((D3D11_AUTHENTICATED_QUERY_INPUT*)&QueryEvicryptGuidInput,
               sizeof(QueryEvicryptGuidInput),
               (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryEvicryptGuidOutput,
               sizeof(QueryEvicryptGuidOutput));
    if(hr == S_OK)
    {
        assert(NULL != pEncryptionGuid);

        *pEncryptionGuid = QueryEvicryptGuidOutput.EncryptionGuid;
    }
    
    return hr;
}   //  CPControl11::QueryEncryptionWhenAccessibleGuid


HRESULT
CPControl11::QueryCurrentEncryptionWhenAccessible
(
    GUID*    pEncryptionGuid
)
{
    HRESULT						hr = S_OK;
    D3D11_AUTHENTICATED_QUERY_INPUT QueryInput;
    ZeroMemory(&QueryInput, sizeof(QueryInput));
    QueryInput.QueryType = D3D11_AUTHENTICATED_QUERY_CURRENT_ENCRYPTION_WHEN_ACCESSIBLE;

    D3D11_AUTHENTICATED_QUERY_CURRENT_ACCESSIBILITY_ENCRYPTION_OUTPUT  QueryCurrEvicryptOutput;

    hr = Query(&QueryInput,
               sizeof(QueryInput),
               (D3D11_AUTHENTICATED_QUERY_OUTPUT*)&QueryCurrEvicryptOutput,
               sizeof(QueryCurrEvicryptOutput));
    if(hr == S_OK)
    {
        assert(NULL != pEncryptionGuid);

        *pEncryptionGuid = QueryCurrEvicryptOutput.EncryptionGuid;
    }
    
    return hr;
}   //  CPControl11::QueryCurrentEvictionEncryption


/******************************************************************/

HRESULT
CPControl11::ConfigureInitialize
()
{
    D3D11_AUTHENTICATED_CONFIGURE_INITIALIZE_INPUT ConfigureInitialize;
    ZeroMemory(&ConfigureInitialize, sizeof(ConfigureInitialize));
    ConfigureInitialize.Parameters.ConfigureType = D3D11_AUTHENTICATED_CONFIGURE_INITIALIZE;
    ConfigureInitialize.StartSequenceQuery = m_SequenceQuery;
    ConfigureInitialize.StartSequenceConfigure = m_SequenceConfigure;
 
    return Configure((D3D11_AUTHENTICATED_CONFIGURE_INPUT*)&ConfigureInitialize,
                      sizeof(ConfigureInitialize));
}   //  CPControl::ConfigureInitialize


HRESULT
CPControl11::ConfigureProtection
(
    BOOL            bProtectionEnabled,
    BOOL            bRequireFlip
)
{
    D3D11_AUTHENTICATED_CONFIGURE_PROTECTION_INPUT ConfigureProtection;
    ZeroMemory(&ConfigureProtection, sizeof(ConfigureProtection));
    ConfigureProtection.Parameters.ConfigureType = D3D11_AUTHENTICATED_CONFIGURE_PROTECTION;
    ConfigureProtection.Protections.Flags.ProtectionEnabled = bProtectionEnabled;
    ConfigureProtection.Protections.Flags.OverlayOrFullscreenRequired = bRequireFlip;
 
    return Configure((D3D11_AUTHENTICATED_CONFIGURE_INPUT*)&ConfigureProtection,
                      sizeof(ConfigureProtection));
}   //  CPControl11::ConfigureProtection


HRESULT
CPControl11::ConfigureCryptoSession
(
    HANDLE            hDXVA2Decode,
    HANDLE            hCryptoSession,
    HANDLE            hDevice
)
{
    D3D11_AUTHENTICATED_CONFIGURE_CRYPTO_SESSION_INPUT ConfigureCryptoSession;
    ZeroMemory(&ConfigureCryptoSession, sizeof(ConfigureCryptoSession));
    ConfigureCryptoSession.Parameters.ConfigureType = D3D11_AUTHENTICATED_CONFIGURE_CRYPTO_SESSION;
    ConfigureCryptoSession.DecoderHandle = hDXVA2Decode;
    ConfigureCryptoSession.CryptoSessionHandle = hCryptoSession;
    ConfigureCryptoSession.DeviceHandle = hDevice;
 
    return Configure((D3D11_AUTHENTICATED_CONFIGURE_INPUT*)&ConfigureCryptoSession,
                      sizeof(ConfigureCryptoSession));
}   //  CPControl11::ConfigureCryptoSession


HRESULT
CPControl11::ConfigureSharedResource
(
    HANDLE          hProcess,
    BOOL            bDWMProc,
    BOOL            bAllowed
)
{
    D3D11_AUTHENTICATED_CONFIGURE_SHARED_RESOURCE_INPUT ConfigureSharedResource;
    ZeroMemory(&ConfigureSharedResource, sizeof(ConfigureSharedResource));
    ConfigureSharedResource.Parameters.ConfigureType = D3D11_AUTHENTICATED_CONFIGURE_SHARED_RESOURCE;
    
    if(bDWMProc)
        ConfigureSharedResource.ProcessType = D3D11_PROCESSIDTYPE_DWM;    
    else
        ConfigureSharedResource.ProcessType = D3D11_PROCESSIDTYPE_HANDLE;    

    ConfigureSharedResource.ProcessHandle = hProcess;
    ConfigureSharedResource.AllowAccess = bAllowed;
 
    return Configure((D3D11_AUTHENTICATED_CONFIGURE_INPUT*)&ConfigureSharedResource,
                      sizeof(ConfigureSharedResource));
}   //  CPControl11::ConfigureSharedResource


HRESULT
CPControl11::ConfigureEncryptionWhenAccessible
(
    GUID            EncryptionGuid
)
{
    D3D11_AUTHENTICATED_CONFIGURE_ACCESSIBLE_ENCRYPTION_INPUT ConfigureEvictionCryption;
    ZeroMemory(&ConfigureEvictionCryption, sizeof(ConfigureEvictionCryption));
    ConfigureEvictionCryption.Parameters.ConfigureType = D3D11_AUTHENTICATED_CONFIGURE_ENCRYPTION_WHEN_ACCESSIBLE;
    ConfigureEvictionCryption.EncryptionGuid = EncryptionGuid;
 
    return Configure((D3D11_AUTHENTICATED_CONFIGURE_INPUT*)&ConfigureEvictionCryption,
                      sizeof(ConfigureEvictionCryption));
}   //  CPControl11::ConfigureEncryptionWhenAccessible

// Caps
HRESULT 
CPControl11::InitializeContentProtectionCaps(GUID* pCryptoType, GUID* pDecodeProfile)
{
    HRESULT             hr = S_OK;

    if( NULL == m_pVideo11 )
        return E_FAIL;

    if( FAILED(hr = m_pVideo11->GetContentProtectionCaps(pCryptoType, pDecodeProfile, &m_ContentProtectionCaps)))
        return hr ;

    if( FAILED(hr = m_pVideo11->GetContentProtectionCaps(&D3D11_CRYPTO_TYPE_AES128_CTR, pDecodeProfile, &m_CryptoCaps)))
        return hr ;

    return hr;
}

BOOL 
CPControl11::SoftwareProtectionSupported()
{
    if ((m_ContentProtectionCaps.Caps & D3D11_CONTENT_PROTECTION_CAPS_SOFTWARE) == 0)
    {
        return FALSE;
    }
    if (m_ContentProtectionCaps.ProtectedMemorySize == 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL 
CPControl11::HardwareProtectionSupported()
{
    if ((m_ContentProtectionCaps.Caps & D3D11_CONTENT_PROTECTION_CAPS_HARDWARE) == 0)
    {
        return FALSE;
    }
    if (m_ContentProtectionCaps.ProtectedMemorySize == 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL 
CPControl11::HardwareProtectionAlwaysOn()
{
    if ((m_ContentProtectionCaps.Caps & D3D11_CONTENT_PROTECTION_CAPS_HARDWARE) == 0)
    {
        return FALSE;
    }
    if ((m_ContentProtectionCaps.Caps & D3D11_CONTENT_PROTECTION_CAPS_PROTECTION_ALWAYS_ON) == 0)
    {
        return FALSE;
    }
    if (m_ContentProtectionCaps.ProtectedMemorySize == 0)
    {
        return FALSE;
    }
    return TRUE;
}

BOOL 
CPControl11::DriverProtectionSupported()
{
    if ((m_ContentProtectionCaps.Caps & (D3D11_CONTENT_PROTECTION_CAPS_SOFTWARE | D3D11_CONTENT_PROTECTION_CAPS_HARDWARE)) == 0)
    {
        return FALSE;
    }
    if (m_ContentProtectionCaps.ProtectedMemorySize == 0)
    {
        return FALSE;
    }
    return TRUE;
}




/*************************************************************/

HRESULT 
CPControl11::Query
(
    D3D11_AUTHENTICATED_QUERY_INPUT*  pInput,
    UINT                              InputSize,
    D3D11_AUTHENTICATED_QUERY_OUTPUT* pOutput,
    UINT                              OutputSize
)
{
    HRESULT             hr = S_OK;

    //  params
    if ((0 == pInput) || (InputSize == 0)) 
        return E_INVALIDARG;

    assert(0 != m_pChannel11);

    ZeroMemory(pOutput, OutputSize);
    pInput->hChannel = m_hChannel11;
    pInput->SequenceNumber = m_SequenceQuery;

    //  Proper Querqy call
    if (FAILED(hr = m_pVideoContext11->QueryAuthenticatedChannel(m_pChannel11, InputSize, pInput, OutputSize, pOutput)))
    {
        return hr;
    }

    if(m_dact11 != D3D11_AUTHENTICATED_CHANNEL_D3D11)
    {
        //  verify signature
        if (FAILED(hr = m_AuthHelper.VerifySignature((BYTE*)pOutput + sizeof(D3D11_OMAC),
                                                     OutputSize - sizeof(D3D11_OMAC),
                                                     (BYTE*)pOutput, 
                                                     sizeof(D3D11_OMAC))))
        {
            return hr;
        }

        // TODO: Uncomment when Win 8 Bug 133350 is fixed
        //if(pOutput->SequenceNumber != pInput->SequenceNumber)
        //{
        //    return E_FAIL;
        //}
    }

    // TODO: Uncomment when Win 8 Bug 133350 is fixed
    //if ((pOutput->hChannel != pInput->hChannel) ||          
    //    (pOutput->QueryType != pInput->QueryType))
    //{            
    //    return E_FAIL;
    //}

    //  update seq number
    m_SequenceQuery++;

    return pOutput->ReturnCode;
}   //  CPControl11::Query


HRESULT 
CPControl11::Configure
(
    D3D11_AUTHENTICATED_CONFIGURE_INPUT*   pInput, 
    UINT                                   InputSize
)
{
    D3D11_AUTHENTICATED_CONFIGURE_OUTPUT Output;
    HRESULT         hr = S_OK;
    assert(0 != m_pChannel11);


    if ((0 == pInput) || (InputSize == 0))
    {
        return E_INVALIDARG;            
    }

    //  fill in data
    ZeroMemory(&Output, sizeof(Output));
    pInput->hChannel = m_hChannel11;
    pInput->SequenceNumber = m_SequenceConfigure;
    
    if(m_dact11 != D3D11_AUTHENTICATED_CHANNEL_D3D11)
    {
        //  sign command struct
        if (FAILED(hr = m_AuthHelper.Sign((BYTE*)pInput + sizeof(D3D11_OMAC),
                                          InputSize - sizeof(D3D11_OMAC),
                                          (BYTE*)pInput,
                                          sizeof(D3D11_OMAC))))
        {
            return hr;
        }
    }

    //  proper call
    if (FAILED(hr = m_pVideoContext11->ConfigureAuthenticatedChannel(m_pChannel11, InputSize, pInput, &Output)))
    {
        return hr;
    }

    if(m_dact11 != D3D11_AUTHENTICATED_CHANNEL_D3D11)
    {
        //  verify signature
        if (FAILED(hr = m_AuthHelper.VerifySignature((BYTE*)&Output + sizeof(D3D11_OMAC),
                                                     sizeof (Output) - sizeof(D3D11_OMAC),
                                                     (BYTE*)&(Output.omac), 
                                                     sizeof(D3D11_OMAC))))
        {
            return hr;
        }

        if(Output.SequenceNumber != pInput->SequenceNumber)
        {
            return E_FAIL;
        }
    }

    if ((Output.hChannel != pInput->hChannel) || 
        (Output.ConfigureType != pInput->ConfigureType))
    {
        return E_FAIL;            
    }

    //  update seq number
    m_SequenceConfigure++;        

    return Output.ReturnCode;
}   //  CPControl11::Configure


