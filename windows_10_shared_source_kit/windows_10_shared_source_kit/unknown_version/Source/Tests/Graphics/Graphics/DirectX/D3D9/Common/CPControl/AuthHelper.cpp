
#include <windows.h>
#include <strsafe.h>

#include "CryptHlp.h"
#include "CertHelper.h"
#include "AuthHelper.h"
#include "IntelKeyExchange.h"

//#define __FOR_INTEL__

/*
 *        class:    CPAuthHelper
 *  description:    helper class for authentification mechanism
 */

CPAuthHelper::CPAuthHelper
()
{
    m_IsTestDriver = FALSE;
}

CPAuthHelper::~CPAuthHelper
()
{
}

HRESULT
CPAuthHelper::Initialize
(
    const BYTE* pbCert,
    const DWORD dwCertLen,
    BOOL  bIsX509Cert
)
{
    HRESULT     hr = S_OK;
    BYTE*       pbPublicKey = 0;
    DWORD       cbKeyLen = 0;
    DWORD       dwExponent = 0;

    if(!bIsX509Cert)
    {
        GUID  guidKDI;
        Crescent::CCryptHlp::GenerateRandom((BYTE *)&guidKDI,sizeof(GUID));
        m_AESHelper.SetKey((BYTE *)&guidKDI,sizeof(GUID));
        return hr;
    }

    //  validation
    if ((0 == pbCert) || (0 == dwCertLen))
        return E_INVALIDARG;
    
    do
    {
        //  set this certificate
        if (FAILED(hr = m_CertHelper.SetCertificate(pbCert, dwCertLen, bIsX509Cert)))
            break;

        //  extract the public key from the certificate
        if (FAILED(hr = m_CertHelper.ExtractPublicKey(&pbPublicKey, &cbKeyLen, &dwExponent)))
        {
            // Check to see if the test driver is running or not

            HKEY D3DRegKey = NULL;  

            if (ERROR_SUCCESS == RegOpenKeyExA(HKEY_LOCAL_MACHINE,
                "Software\\Microsoft\\Direct3D", 0, KEY_QUERY_VALUE, &D3DRegKey))
            {
                DWORD Type = REG_DWORD;
                DWORD HookDriver = 0;
                DWORD Size = sizeof(HookDriver);
                if (ERROR_SUCCESS == RegQueryValueExA(D3DRegKey, 
                                                     "UseHookDriver", 
                                                     NULL,
                                                     &Type,
                                                     (LPBYTE) &HookDriver, 
                                                     &Size))
                {
                    m_IsTestDriver = HookDriver != 0;
                }

                RegCloseKey(D3DRegKey);
            }

            if (m_IsTestDriver)
            {
                // We can't get a public key from the cert, so make one up
                pbPublicKey = new BYTE[256];
                for (UINT i = 0; i < 256; i++)
                {
                    pbPublicKey[i] = i + 1;
                }
                hr = S_OK;
            }
            else
            {
                break;
            }
        }

        cbKeyLen = 256;

        //  initialize the crypto helper
        //  assert we're creating a new object

        
        m_RSAHelper.SetPublicKey(dwExponent, pbPublicKey, cbKeyLen );

#ifdef __FOR_INTEL__
        BYTE guidKDI[16];
        for (UINT i = 0; i < 16; i++)
        {
            guidKDI[i] = 1;
        }
#else
        GUID  guidKDI;
        Crescent::CCryptHlp::GenerateRandom((BYTE *)&guidKDI,sizeof(GUID));
#endif
        m_AESHelper.SetKey((BYTE *)&guidKDI,sizeof(GUID));

    }   while (FALSE);

    SafeSecureMemDeleteArray(pbPublicKey, cbKeyLen);

    return hr;
}   //  CPAuthHelper::Initialize


HRESULT
CPAuthHelper::PrepareSignature
(
    BYTE*       pbSignature,
    const DWORD cbSigSize,
    const BYTE pgRandomNumber[16],
    const DWORD dwStatusSeqStart,
    const DWORD dwCommandSeqStart,
    BOOL  bOAEPEnc
)
{
    HRESULT     hr = S_OK;
    DWORD       cbOffset = 0;
    BYTE*       pbEncData = 0;
    DWORD       cbEncData = 0;
    DWORD       cbKeyLen = 0;
    BYTE*       pbAESKey = NULL;
    BYTE*       pbBSafeEncData = 0;
    DWORD       cbBSafeEncData = 0;

    //  validation
    //ASSERT((0 != pbSignature) && (0 != cbSigSize));
    if ((0 == pbSignature) || (0 == cbSigSize))
        return E_INVALIDARG;

    do
    {
        //  assembly data
        //  first comes the random 128 bit nr
        CopyMemory(pbSignature + cbOffset, pgRandomNumber, 16 * sizeof BYTE);
        cbOffset += 16 * sizeof BYTE;

        cbKeyLen = sizeof(GUID);
        pbAESKey = new BYTE[cbKeyLen];
        if(pbAESKey==NULL)
            return E_OUTOFMEMORY;
        if (FAILED(hr = m_AESHelper.GetKey(pbAESKey, &cbKeyLen)))
            break;

        CopyMemory(pbSignature + cbOffset, pbAESKey, cbKeyLen);
        SafeSecureMemDeleteArray(pbAESKey, cbKeyLen);
        cbOffset += cbKeyLen;

        //  then the status seq number
        CopyMemory(pbSignature + cbOffset, &dwStatusSeqStart, sizeof DWORD);
        cbOffset += sizeof DWORD;

        //  and the cmd seq number
        CopyMemory(pbSignature + cbOffset, &dwCommandSeqStart, sizeof DWORD);
        cbOffset += sizeof DWORD;

        //  zero the rest
        //ASSERT(cbSigSize >= cbOffset);
        
        ZeroMemory(pbSignature + cbOffset, cbSigSize - cbOffset);
        
#if _NOOAEPPADDING
        if (FAILED(hr = m_RSAHelper.RSAEncPublic(pbSignature, cbSigSize, &pbEncData, &cbEncData)))
            break;
#else
        if(bOAEPEnc){
            if (FAILED(hr = m_RSAHelper.RSAOAEPEncPublic(pbSignature, cbOffset, &pbEncData, &cbEncData)))
                break;
        }
        else{
            if (FAILED(hr = m_RSAHelper.RSAEncPublic(pbSignature, cbSigSize, &pbEncData, &cbEncData)))
               break;
        }
#endif
        //  transfer result
        //ASSERT((0 != pbEncData) && (0 != cbEncData));
        ZeroMemory(pbSignature, cbSigSize);

        //don't want to copy more that allocated size of Signature
        if(cbEncData>=cbSigSize)
            cbEncData = cbSigSize;
        CopyMemory(pbSignature, pbEncData, cbEncData);
        
    }   while (FALSE);

    SafeSecureMemDeleteArray(pbEncData, cbKeyLen);

    return hr;
}   //  CPAuthHelper::PrepareSignature

HRESULT  
CPAuthHelper::PrepareAuthenticatedChannelSignature
(
     BYTE* pbSignature, 
     const DWORD cbSigSize,
     GUID  KeyExchangeType
)
{
    HRESULT     hr = S_OK;
    DWORD       cbOffset = 0;
    BYTE*       pbEncData = 0;
    DWORD       cbEncData = 0;
    DWORD       cbKeyLen = 0;
    BYTE*       pbAESKey = NULL;
    BYTE*       pbBSafeEncData = 0;
    DWORD       cbBSafeEncData = 0;

    //  validation
    //ASSERT((0 != pbSignature) && (0 != cbSigSize));
    if ((0 == pbSignature) || (0 == cbSigSize))
        return E_INVALIDARG;
    //KeyExchangeType  = D3DKEYEXCHANGE_CANTIGA;
    if( (KeyExchangeType == D3DKEYEXCHANGE_CANTIGA)  || 
        (KeyExchangeType == D3DKEYEXCHANGE_EAGLELAKE)|| 
        (KeyExchangeType == D3DKEYEXCHANGE_SAFEID)   ||
        (KeyExchangeType == D3DKEYEXCHANGE_LARABEE) )
        return PrepareIntelSpecificSignature(pbSignature, cbSigSize, KeyExchangeType);

    do
    {
#ifdef __FOR_INTEL__
        cbOffset += 16 * sizeof BYTE;
#endif
        cbKeyLen = sizeof(GUID);
        pbAESKey = new BYTE[cbKeyLen];
        if(pbAESKey==NULL)
            return E_OUTOFMEMORY;
        if (FAILED(hr = m_AESHelper.GetKey(pbAESKey, &cbKeyLen)))
            break;

        CopyMemory(pbSignature + cbOffset, pbAESKey, cbKeyLen);
        SafeSecureMemDeleteArray(pbAESKey, cbKeyLen);
        cbOffset += cbKeyLen;

        //  zero the rest
        //ASSERT(cbSigSize >= cbOffset);
        
        ZeroMemory(pbSignature + cbOffset, cbSigSize - cbOffset);
        
        if (!m_IsTestDriver)
        {
#ifdef __FOR_INTEL__
            if (FAILED(hr = m_RSAHelper.RSAOAEPEncPublic(pbSignature, 40, &pbEncData, &cbEncData)))
                break;
#else
            if (FAILED(hr = m_RSAHelper.RSAOAEPEncPublic(pbSignature, cbOffset, &pbEncData, &cbEncData)))
                break;
#endif

            ZeroMemory(pbSignature, cbSigSize);
        }


        //don't want to copy more that allocated size of Signature
        if(cbEncData>=cbSigSize)
            cbEncData = cbSigSize;
        CopyMemory(pbSignature, pbEncData, cbEncData);
        
    }   while (FALSE);

    SafeSecureMemDeleteArray(pbEncData, cbKeyLen);

    return hr;
}   //  OPMAuthHelper::PrepareAuthenticatedChannelSignature


HRESULT
CPAuthHelper::Sign
(
    const BYTE* pbData,
    const DWORD cbDataSize,
    BYTE*       pbSignature,
    const DWORD cbSigSize
)
{
    HRESULT     hr = S_OK;
    BYTE        rgbSignature[AES_BLOCKLEN] = {0};

    //  validation
    if ((0 == pbData) || (0 == cbDataSize) ||
        (0 == pbSignature) || (0 == cbSigSize))
        return E_INVALIDARG;

    hr = m_AESHelper.SignData((BYTE*)pbData, cbDataSize, rgbSignature);
    if (SUCCEEDED(hr))
    {
        CopyMemory(pbSignature, rgbSignature, min(cbSigSize, AES_BLOCKLEN));
    }

    return hr;
}   //  CPAuthHelper::Sign


HRESULT
CPAuthHelper::VerifySignature
(
    const BYTE* pbData,
    const DWORD cbDataSize,
    const BYTE* pbSignature,
    const DWORD cbSigSize
)
{
    HRESULT     hr = S_OK;
    BYTE        rgbSignature[AES_BLOCKLEN] = {0};

    //  validation
    if ((0 == pbData) || (0 == cbDataSize) ||
        (0 == pbSignature) || (0 == cbSigSize))
        return E_INVALIDARG;

    if (cbSigSize != AES_BLOCKLEN)
        return E_INVALIDARG;

    do
    {
        //calculate our own signature
        if(FAILED(hr = m_AESHelper.SignData((BYTE*)pbData, cbDataSize, rgbSignature)))
            break;

        if (memcmp(pbSignature, rgbSignature, min(cbSigSize, AES_BLOCKLEN)))
            hr = E_FAIL;

    }   while (FALSE);

    return hr;
}   //  CPAuthHelper::VerifySignature




HRESULT  
CPAuthHelper::PrepareIntelSpecificSignature
(
     BYTE* pbSignature, 
     const DWORD cbSigSize,
     GUID  KeyExchangeType
)
{
    HRESULT     hr = S_OK;
    BYTE*       pbEncData = 0;
    DWORD       cbEncData = 0;
    DWORD       cbKeyLen = 0;
    BYTE*       pbAESKey = NULL;
    BYTE*       pbFixedKey = NULL;
    DWORD       cbFixedKey = 0;
    BYTE*       pbEncFixedKey = NULL;
    DWORD       cbEncFixedKey = 0;

    if ((0 == pbSignature) || (0 == cbSigSize))
        return E_INVALIDARG;

    do
    {
        GPUCP_CRYPTOSESSION_PAVP_KEYEXCHANGE KeyExchangeData;		
        ZeroMemory(&KeyExchangeData , sizeof(GPUCP_CRYPTOSESSION_PAVP_KEYEXCHANGE));
        
        KeyExchangeData.PAVPKeyExchangeGUID = KeyExchangeType;
        if( (KeyExchangeType == D3DKEYEXCHANGE_CANTIGA)  || 
            (KeyExchangeType == D3DKEYEXCHANGE_LARABEE) )
        {
            KeyExchangeData.DataSize = sizeof(PAVP_FIXED_EXCHANGE_PARAMS);
            
            cbFixedKey = sizeof(s_dwFixedKeyCtgB2);
            pbFixedKey = new BYTE[cbFixedKey];
            if(pbFixedKey==NULL)
                return E_OUTOFMEMORY;
            CopyMemory(pbFixedKey, s_dwFixedKeyCtgB2, cbFixedKey);

            cbEncFixedKey = sizeof(s_dwEncryptedFixedKeyCtgB2);
            pbEncFixedKey = new BYTE[cbEncFixedKey];
            if(pbEncFixedKey==NULL)
                return E_OUTOFMEMORY;
            CopyMemory(pbEncFixedKey, s_dwEncryptedFixedKeyCtgB2, cbEncFixedKey);

        }
        else if((KeyExchangeType == D3DKEYEXCHANGE_EAGLELAKE))
        {
            KeyExchangeData.DataSize = sizeof(PAVP_FIXED_EXCHANGE_PARAMS);
            
            cbFixedKey = sizeof(s_dwFixedKeyEglA2);
            pbFixedKey = new BYTE[cbFixedKey];
            if(pbFixedKey==NULL)
                return E_OUTOFMEMORY;
            CopyMemory(pbFixedKey, s_dwFixedKeyEglA2, cbFixedKey);

            cbEncFixedKey = sizeof(s_dwEncryptedFixedKeyEglA2);
            pbEncFixedKey = new BYTE[cbEncFixedKey];
            if(pbEncFixedKey==NULL)
                return E_OUTOFMEMORY;
            CopyMemory(pbEncFixedKey, s_dwEncryptedFixedKeyEglA2, cbEncFixedKey);
        }
        else if( (KeyExchangeType == D3DKEYEXCHANGE_SAFEID) )
        {
            KeyExchangeData.DataSize = sizeof(PAVP_SAFEID_EXCHANGE_PARAMS);
        }
        KeyExchangeData.pKeyExchangeParams = pbSignature + sizeof(KeyExchangeData);

        // Encrypt the Session Key using RSAOAEP w/ Fixed Key 
        cbKeyLen = sizeof(GUID);
        pbAESKey = new BYTE[cbKeyLen];
        if(pbAESKey==NULL)
            return E_OUTOFMEMORY;
        if (FAILED(hr = m_AESHelper.GetKey(pbAESKey, &cbKeyLen)))
            break;

        pbEncData = new BYTE[cbKeyLen];
        if(pbEncData==NULL)
            return E_OUTOFMEMORY;
        CopyMemory(pbEncData, pbAESKey, cbKeyLen);
        
        if (!m_IsTestDriver)
        {			 
            // Set the AEShelper to use Intel Fixed_key for AES128 
            m_AESHelper.SetKey(pbFixedKey, cbFixedKey);

            // Using Intel Fixed_Key for Aes128_Encrypt the Sesion_Key
            if (FAILED(hr = m_AESHelper.ECBProcessData(ENCRYPT, pbEncData, cbKeyLen)))
                break;

            // Reset the AEShelper to use Orig Session Key 
            m_AESHelper.SetKey(pbAESKey, cbKeyLen);

            ZeroMemory(pbSignature, cbSigSize);
        }
        
        SafeSecureMemDeleteArray(pbAESKey, cbKeyLen);

        // Intel KeyExchangeData
        CopyMemory(pbSignature , &KeyExchangeData, sizeof(KeyExchangeData));

        // Intel Encrypted Fixed Key
        CopyMemory(pbSignature + sizeof(KeyExchangeData), pbEncFixedKey, cbEncFixedKey);

        // Session Key Encrypted with Fixed Key
        CopyMemory(pbSignature + sizeof(KeyExchangeData) + cbEncFixedKey, pbEncData, cbKeyLen);
        
    }   while (FALSE);

    SafeSecureMemDeleteArray(pbEncData, cbKeyLen);
    SafeSecureMemDeleteArray(pbFixedKey, cbFixedKey);
    SafeSecureMemDeleteArray(pbEncFixedKey, cbEncFixedKey);

    return hr;
}   //  OPMAuthHelper::PrepareIntelSpecificSignature

