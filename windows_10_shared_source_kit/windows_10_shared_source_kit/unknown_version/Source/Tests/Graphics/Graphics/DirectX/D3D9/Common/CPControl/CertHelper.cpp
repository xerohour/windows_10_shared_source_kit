
#define RTL_MAX_HASH_LEN 20
#define MAX_CERTS_CNT   256
#include <windows.h>
#include <strsafe.h>
#include <wincrypt.h>
#include "mincrypt.h"


#include "CertHelper.h"

/*****************************************************
 *  class implementation:   CPCertHelper
 *****************************************************/

CPCertHelper::CPCertHelper
(): m_pbCertificate(NULL),
    m_cbCertificateLength(0),
    m_bIsX509Cert(FALSE)
{}  //  CPCertHelper::CPCertHelper


CPCertHelper::~CPCertHelper
()
{
    delete [] m_pbCertificate;
    m_pbCertificate = 0;
    m_cbCertificateLength = 0;
}   //  CPCertHelper::~CPCertHelper

HRESULT
CPCertHelper::ExtractPublicKey
(
    BYTE**          ppbKey,
    DWORD*          pcbKeyLength,
    DWORD*          pdwExponent
)
{
    if(m_bIsX509Cert == TRUE)
        return ExtractPublicKeyForX509(ppbKey,pcbKeyLength,pdwExponent);

    HRESULT         hr = S_OK;
    DWORD           cbKey64Len = 0, cbExponentLen = 0, dwExponent = 0;
    BYTE*           pbExponent = 0;
    CHAR*           szKey64 = 0;
    CHAR*           szExponent64 = 0;
    DWORD           cbBinaryKeyLen = 0;
    BYTE*           pbBinaryKey = 0;
    DWORD           dwSkip = 0;
    DWORD           dwFlags = 0;
    const DWORD     RSA_MODULUS_LENGTH = 0x100;

    //  validation
    if ((0 == ppbKey) || (0 == pcbKeyLength) || (0 == pdwExponent))
        return E_INVALIDARG;

    //ASSERT(0 != m_pbCertificate);
    if ((0 == m_pbCertificate) || (0 == m_cbCertificateLength))
        return E_UNEXPECTED;

    //
    do
    {
        //  find key
        if (FAILED(hr = FindElement(&szKey64, &cbKey64Len, XRML_TAG_MODULUS)))
            break;

        //  find exponent
        if (FAILED(hr = FindElement(&szExponent64, &cbExponentLen, XRML_TAG_EXPONENT)))
            break;

        //  Base64 decode the modulus and the exponent
        if (FAILED(hr = DecodeBase64RSAKey(szKey64, cbKey64Len/(sizeof CHAR), RSA_MODULUS_LENGTH,
                                        szExponent64, cbExponentLen, &pbBinaryKey, &cbBinaryKeyLen, &dwExponent)))
            break;

        //  prepare out params
        *ppbKey = pbBinaryKey;
        *pcbKeyLength = cbBinaryKeyLen;
        *pdwExponent = dwExponent;

    }   while (FALSE);

    //  cleanup
    delete [] szKey64;
    delete [] szExponent64;
    if (FAILED(hr))
    {
        delete [] pbBinaryKey;
    }

    return hr;
}   //  CPCertHelper::ExtractPublicKey

//#if 0
HRESULT
CPCertHelper::ExtractPublicKeyForX509
(
    BYTE**          ppbKey,
    DWORD*          pcbKeyLength,
    DWORD*          pdwExponent
)
{
    BSAFE_PUB_KEY_CONTENT BSAFEPublicKey;
    HRESULT hr = S_OK;
    DWORD dwGRLVersion = 0;
    LONG lErr = 0;
    CRYPT_DER_BLOB rgrgCertBlob[MAX_CERTS_CNT][MINASN1_CERT_BLOB_CNT];
    DWORD cCert = MAX_CERTS_CNT;
    MINCRYPT_POLICY_INFO PolicyInfo = {0};
    ULONG i = 0;
    const ALG_ID aiHashId = CALG_SHA1;
    BYTE                   *pPubKeyContent = NULL;
    
    do{
        //
        // Parse cert chain
        //
        lErr = MinAsn1ExtractParsedCertificatesFromSignedData(
            m_pbCertificate,
            m_cbCertificateLength,
            &cCert,
            rgrgCertBlob
            );
        if( ( lErr < 0 ) || ( 0 == cCert ) )
        {
            hr = E_FAIL;
            break;
        }

        //
        // Verify cert chain and get MINCRYPT_POLICY_INFO with EKUs and public keys
        //
        lErr = MinCryptVerifyCertificateWithPolicy(
            rgrgCertBlob[0],
            cCert,
            rgrgCertBlob,
            NULL,       // no policy requirements
            &PolicyInfo
            );
        if( ( ERROR_SUCCESS != lErr ) || ( 0 == PolicyInfo.cbSize ) || ( NULL == PolicyInfo.pChainInfo ) )
        {
            hr = E_FAIL;
            break;
        }
        if ((PolicyInfo.ulPolicyBits & MINCRYPT_POLICY_SYSTEMROOT) == 0)
        {
            hr = E_FAIL;
            break;   
        }

        

        PCRYPT_DER_BLOB pBlob = MincryptGetLeafKeyFromPolicy( &PolicyInfo );

        lErr = MinCryptConvertASNPubKeyInfoToBSafePubKey( pBlob, (BYTE *)&BSAFEPublicKey );
        if( ERROR_SUCCESS != lErr )
        {
            hr = E_FAIL;
            break;
        }
        
        BYTE *pbBinaryKey = new BYTE[BSAFEPublicKey.Header.keylen / sizeof(BYTE)]; //(BYTE *)malloc(BSAFEPublicKey.Header.keylen);
        memcpy((BYTE *)pbBinaryKey, BSAFEPublicKey.rgbModulus, BSAFEPublicKey.Header.keylen);

        *ppbKey = pbBinaryKey;
        *pcbKeyLength = BSAFEPublicKey.Header.keylen;
        *pdwExponent = BSAFEPublicKey.Header.pubexp;

    }while(FALSE);

    MincryptFreePolicyInfo( &PolicyInfo );
    return hr;

}   //  CPCertHelper::ExtractPublicKey
//#endif


HRESULT
CPCertHelper::FindElement
(
    CHAR**          ppstrElement,
    DWORD*          pcbElementLen,
    const CHAR*     pstrTag
)
{
    HRESULT     hr = S_OK;
    CHAR*       pbIdx = 0;
    DWORD       cbIdx = 0;
    CHAR*       pstrElement = 0;
    DWORD       cbElementLen = 0;
    CHAR*       pbBuf = 0;
    DWORD       cbBufLen = 0;
    CHAR        szTag[MAX_PATH+1] = "";
    DWORD       chLen = 0;

    //ASSERT((0 != m_pbCertificate) && (0 != m_cbCertificateLength));

    if ((0 == ppstrElement) || (0 == pcbElementLen) || (0 == pstrTag))
        return E_INVALIDARG;

    do
    {
        //  prepare NULL terminated buffer
        cbBufLen = m_cbCertificateLength + sizeof(CHAR);
        pbBuf = new CHAR[cbBufLen / sizeof(CHAR)];
        if (0 == pbBuf)
        {
            hr = E_OUTOFMEMORY;
            break;
        }
        //  copy certificate; NULL-terminate
        CopyMemory(pbBuf, m_pbCertificate, m_cbCertificateLength);
        pbBuf[m_cbCertificateLength]='\0';

        //  start parsing - first look for the leading tag
        StringCchPrintfA(szTag, MAX_PATH, "<%s>", pstrTag);
        pbIdx = strstr(pbBuf, szTag);
        if (0 == pbIdx)
        {
            hr = E_FAIL;
            break;
        }
        StringCchLengthA(szTag, MAX_PATH, (size_t*)&chLen);
        cbIdx = pbIdx - pbBuf + chLen * sizeof (CHAR);
        pbIdx = pbBuf + cbIdx;

        //  look for the closing tag
        StringCchPrintfA(szTag, MAX_PATH, "</%s>", pstrTag);
        pbIdx = strstr(pbIdx, szTag);
        if (0 == pbIdx)
        {
            hr = E_FAIL;
            break;
        }
        chLen = (pbIdx - pbBuf - cbIdx)/sizeof CHAR;
        pbIdx = pbBuf + cbIdx;

        pstrElement = new CHAR[chLen + 1];
        if (0 == pstrElement)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        //  copy
        CopyMemory(pstrElement, pbIdx, chLen * sizeof CHAR);
        pstrElement[chLen] = '\0';

    }   while (FALSE);

    //  cleanup
    ZeroMemory(pbBuf, cbBufLen);
    delete [] pbBuf;
    pbBuf = 0;

    if (FAILED(hr))
    {
        if (0 != pstrElement)
        {
            ZeroMemory(pstrElement, chLen * sizeof CHAR);
            delete [] pstrElement;
            pstrElement = 0;
        }
        chLen = 0;
    }

    //  and return
    *ppstrElement = pstrElement;
    *pcbElementLen = chLen * sizeof CHAR;

    return hr;
}   //  CPCertHelper::FindElement


HRESULT
CPCertHelper::SetCertificate
(
    const BYTE*     pbCert,
    const DWORD     cbCertLen,
    BOOL  bIsX509Cert 
)
{
    HRESULT         hr = S_OK;

    //ASSERT((0 != pbCert) && (0 != cbCertLen));
    if ((0 == pbCert) || (0 == cbCertLen))
        return E_INVALIDARG;

    do
    {
        hr = ResetCertificate();

        m_pbCertificate = new BYTE[cbCertLen];
        if (0 == m_pbCertificate)
        {
            hr = E_OUTOFMEMORY;
            break;
        }
        
        m_bIsX509Cert = bIsX509Cert;

        CopyMemory(m_pbCertificate, pbCert, cbCertLen);
        m_cbCertificateLength = cbCertLen;

        hr = S_OK;
    }   while (FALSE);

    return hr;
}   //  CPCertHelper::SetCertificate


HRESULT
CPCertHelper::ResetCertificate
()
{
    HRESULT     hr = S_OK;

    if ((0 == m_pbCertificate) || (0 == m_cbCertificateLength))
        return E_UNEXPECTED;

    ZeroMemory(m_pbCertificate, m_cbCertificateLength);
    delete [] m_pbCertificate;
    m_pbCertificate = 0;
    m_cbCertificateLength = 0;

    return S_OK;
}   //  CPCertHelper::ResetCertificate

/*
 *  function    :   DecodeBase64String
 *  description :   allocates and decodes a base64 string to a binary array
 *  arguments   :
 *  returns     :   hresult
 *  notes       :   don't forget to deallocate the buffer when done
 */
HRESULT
DecodeBase64String
(
    IN const CHAR* pszBase64Key,
    IN const DWORD  cchBase64KeyLen,
    OUT BYTE**      ppbBinaryKey,
    OUT DWORD*      pcbBinaryKeyLen
)
{
    HRESULT         hr = S_OK;
    DWORD           cbLen = 0, cbWritten = 0;
    BYTE*           pbBuf = 0;
    DWORD           dwSkip = 0, dwFlags = 0;

    do
    {
        if ((0 == pszBase64Key) || (0 == cchBase64KeyLen))
        {
            hr = E_INVALIDARG;
            break;
        }

        if ((0 == ppbBinaryKey) || (0 == pcbBinaryKeyLen))
        {
            hr = E_POINTER;
            break;
        }

        if (!CryptStringToBinaryA(pszBase64Key, cchBase64KeyLen, CRYPT_STRING_BASE64, 0, &cbLen, &dwSkip, &dwFlags))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            break;
        }

        pbBuf = new BYTE [cbLen];
        if (0 == pbBuf)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        if (!CryptStringToBinaryA(pszBase64Key, cchBase64KeyLen, CRYPT_STRING_BASE64, pbBuf, &cbLen, &dwSkip, &dwFlags))
        {
            hr = HRESULT_FROM_WIN32(GetLastError());
            break;
        }

        *pcbBinaryKeyLen = cbLen;
        *ppbBinaryKey = pbBuf;

    }   while (FALSE);

    if (FAILED(hr))
    {
        delete [] pbBuf;
    }

    return hr;
}   //  DecodeBase64String


/*
 *  function    :   DecodeBase64RSAKey
 *  description :
 *  arguments   :
 *  returns     :
 *  notes       :
 */
HRESULT
DecodeBase64RSAKey
(
    IN const CHAR* pszBase64Modulus,
    IN const DWORD  cchBase64ModulusLen,
    IN const DWORD  cbModulusLen,
    IN const CHAR* pszBase64Exponent,
    IN const DWORD  cchBase64Exponent,
    OUT BYTE**      ppbModulus,
    OUT DWORD*      pcbModulus,
    OUT DWORD*      pdwExponent
)
{
    HRESULT         hr = S_OK;
    DWORD           dwExp = 0;
    BYTE*           pbMod = NULL;
    DWORD           cbMod = 0;
    BYTE*           pbExpNet = NULL;
    DWORD           cbExpNet = 0;
    BYTE*           pbModNet = NULL;
    DWORD           cbModNet = 0;
    DWORD           i = 0;

    do
    {
        //  validate params
        if ((0 == pszBase64Modulus) ||
            (0 == cchBase64ModulusLen) ||
            (0 == cbModulusLen) ||
            (0 == pszBase64Exponent) ||
            (0 == cchBase64Exponent))
        {
            hr = E_INVALIDARG;
            break;
        }

        if ((0 == ppbModulus) ||
            (0 == pcbModulus) ||
            (0 == pdwExponent))
        {
            hr = E_POINTER;
            break;
        }


        //  decode exponent
        hr = DecodeBase64String(pszBase64Exponent,
                                cchBase64Exponent,
                                &pbExpNet, &cbExpNet);
        if (FAILED(hr))
        {
            break;
        }
        if (cbExpNet > sizeof DWORD)
        {
            hr = E_FAIL;
            break;
        }

        // convert to host order
        for (i = 0; i < cbExpNet; i++)
        {
            dwExp <<= 8;
            dwExp |= pbExpNet[i];
        }

        //  decode modulus
        hr = DecodeBase64String(pszBase64Modulus,
                                cchBase64ModulusLen,
                                &pbModNet, &cbModNet);
        if (FAILED(hr))
        {
            break;
        }
        if (cbModNet > cbModulusLen)
        {
            hr = E_FAIL;
            break;
        }

        pbMod = new BYTE[cbModulusLen];
        if (0 == pbMod)
        {
            hr = E_OUTOFMEMORY;
            break;
        }

        // convert to host order by reversing
        for (i = 0; i < cbModNet; i++)
        {
            pbMod[i] = pbModNet[cbModNet - 1 - i];
        }

        // set leading zeroes
        for (; i < cbModulusLen - 1; i++)
        {
            pbMod[i] = 0;
        }

        *pdwExponent = dwExp;
        *ppbModulus = pbMod;
        *pcbModulus = cbModulusLen;

    }   while (FALSE);

    delete [] pbExpNet;
    delete [] pbModNet;
    if (FAILED(hr))
    {
        delete [] pbMod;
    }

    return(hr);
}   //  DecodeBase64RSAKey

/*
 *  function    :   PrintByteArray
 *  description :   helper, debug prints a byte array
 *  arguments   :   array, array size
 *  returns     :
 *  notes       :
 */
void
PrintByteArray
(
    const BYTE* pbArray,
    const DWORD cbArraySize
)
{
#ifdef  CRYPTO_DEBUG
    char    szText[MAX_PATH+1];
    char    sztmp[10];
    DWORD   i = 0, j = 0, k = cbArraySize/16;
    DWORD   rest = cbArraySize % 16;

    for (i = 0; i < k; i++)
    {
        StringCchPrintfA(szText, MAX_PATH, "%2.2X,%2.2X,%2.2X,%2.2X,%2.2X,%2.2X,%2.2X,%2.2X,%2.2X,%2.2X,%2.2X,%2.2X,%2.2X,%2.2X,%2.2X,%2.2X\n",
                        pbArray[i*16+0x0],
                        pbArray[i*16+0x1],
                        pbArray[i*16+0x2],
                        pbArray[i*16+0x3],
                        pbArray[i*16+0x4],
                        pbArray[i*16+0x5],
                        pbArray[i*16+0x6],
                        pbArray[i*16+0x7],
                        pbArray[i*16+0x8],
                        pbArray[i*16+0x9],
                        pbArray[i*16+0xa],
                        pbArray[i*16+0xb],
                        pbArray[i*16+0xc],
                        pbArray[i*16+0xd],
                        pbArray[i*16+0xe],
                        pbArray[i*16+0xf]);
        OutputDebugStringA(szText);
    }
    StringCchPrintfA(szText, MAX_PATH, "");
    for (i = 0; i < rest; i++)
    {
        StringCchPrintfA(sztmp, 10, "%2.2x,", pbArray[k*16 + i]);
        StringCchCatA(szText, MAX_PATH, sztmp);
    }
    OutputDebugStringA(szText);
    OutputDebugStringA("\n");
#endif  //  CRYPTO_DEBUG
}   //  PrintByteArray
