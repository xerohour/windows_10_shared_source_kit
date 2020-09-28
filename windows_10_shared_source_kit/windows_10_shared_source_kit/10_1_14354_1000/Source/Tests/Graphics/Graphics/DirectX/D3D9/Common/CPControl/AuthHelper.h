#pragma once

#define XRML_TAG_MODULUS                ("Modulus")
#define XRML_TAG_EXPONENT               ("Exponent")
#define XRML_TAG_RSAKEYVALUE            ("RSAKeyValue")

#define RSA_MODULUS_BYTE_LENGTH  512

#include <CryptHlp.h>
#include "CertHelper.h"
#include <d3d9.h>

class CPAuthHelper
{
public:

    //  construction
    CPAuthHelper();
    ~CPAuthHelper();

    HRESULT     Initialize(const BYTE*  pbGHCert, 
                           const DWORD cbGHCert,
                           BOOL bIsX509=FALSE );

    //  functionality
    HRESULT     PrepareSignature(BYTE*      pbSignature, 
                                const DWORD cbSignature, 
                                const BYTE pgRandomNumber[16],
                                const DWORD dwStatusSeqIdx, 
                                const DWORD dwCmdSeqIdx,
                                BOOL bOAEPEnc);

    HRESULT     Sign(const BYTE*    pbData, 
                    const DWORD     cbData, 
                    BYTE*           pbSignature, 
                    const DWORD     cbSignature);

    HRESULT     VerifySignature(const BYTE* pbData, 
                                const DWORD cbData, 
                                const BYTE* pbSignature, 
                                const DWORD cbSignature);

    //  tools
    HRESULT     GenerateRandomNumber
    (
        BYTE*       pbNumber, 
        const DWORD cbSize
    )    
    {   
        return Crescent::CCryptHlp::GenerateRandom(pbNumber, cbSize);
    }

    HRESULT  PrepareAuthenticatedChannelSignature(BYTE* pbSignature, const DWORD cbSignature, GUID  KeyExchangeType = D3DKEYEXCHANGE_RSAES_OAEP);
    HRESULT  PrepareIntelSpecificSignature(BYTE* pbSignature, const DWORD cbSignature, GUID  KeyExchangeType);

    Crescent::CAESHelper* GetAESHelper()
    {
        return &m_AESHelper;
    }

private:
    Crescent::CAESHelper     m_AESHelper;
    Crescent::CRSAHelper     m_RSAHelper;

    CPCertHelper      m_CertHelper;

    BOOL              m_IsTestDriver;

};  //  class CPAuthHelper

void    PrintByteArray(const BYTE*  pbArray, 
                       const DWORD  cbArray);
//
//  function    :   SafeSecureMemDeleteArray
//  description :   
//  parameters  :   
//  returns     :   
//  notes       :   
//
template <class T>
inline
void
SafeSecureMemDeleteArray
(
    T*          pT,
    const DWORD cbSize
)
{
    if (0 == cbSize)
        return;

    if (0 != pT)
    {
        SecureZeroMemory(pT, cbSize);
        delete pT;
        pT = 0;
    }
}
