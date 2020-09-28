#pragma once

#define XRML_TAG_MODULUS                ("Modulus")
#define XRML_TAG_EXPONENT               ("Exponent")
#define XRML_TAG_RSAKEYVALUE            ("RSAKeyValue")

#define RSA_MODULUS_BYTE_LENGTH  512


class CPCertHelper
{
public:
     //  construction
    CPCertHelper();
    ~CPCertHelper();

    static const int MAX_RSA_PUB_KEY_LEN = 4096;
    static const int MAX_RSA_PUB_KEY_BYT_LEN   = (MAX_RSA_PUB_KEY_LEN / 8 );
	static const int MAX_BSAFE_PUB_KEY_MODULUS_BYT_LEN  = (MAX_RSA_PUB_KEY_BYT_LEN + sizeof(DWORD) * 4);


	typedef struct
	{
	    DWORD       magic;                  /* Should always be RSA1 */
	    DWORD       keylen;                 // size of modulus buffer
	    DWORD       bitlen;                 // # of bits in modulus
	    DWORD       datalen;                // max number of bytes to be encoded
	    DWORD       pubexp;                 //public exponent
	}
	BSAFE_PUB_KEY, FAR *LPBSAFE_PUB_KEY;

	typedef struct _BSAFE_PUB_KEY_CONTENT
	{
	    BSAFE_PUB_KEY   Header;
	    BYTE            rgbModulus[MAX_BSAFE_PUB_KEY_MODULUS_BYT_LEN];
	}
	BSAFE_PUB_KEY_CONTENT, *PBSAFE_PUB_KEY_CONTENT;

    //  methods
    HRESULT     SetCertificate(const BYTE*  pbCert,
                               const DWORD cbCertLen,
                               BOOL  bIsX509Cert = FALSE
                                );


    HRESULT     ExtractPublicKey(BYTE**     ppbKey,
                                DWORD*      pcbKeyLen,
                                DWORD*      pdwKeyExponent);

    HRESULT     ExtractPublicKeyForX509(BYTE**     ppbKey,
                                DWORD*      pcbKeyLen,
                                DWORD*      pdwKeyExponent);

protected:

    HRESULT     FindElement(CHAR**          ppstrElement,
                            DWORD*          pcbElementLen,
                            const CHAR*     pstrTag);

    HRESULT     ResetCertificate();

private:
    BYTE*       m_pbCertificate;
    DWORD       m_cbCertificateLength;
    BOOL        m_bIsX509Cert;
};  //  class CPCertHelper

//  other helper functions
HRESULT DecodeBase64RSAKey(const CHAR*  szBase64Modulus, 
                           const DWORD  cchBase64ModulusLen, 
                           const DWORD  cbModulusLen,
                           const CHAR*  szBase64Exponent, 
                           const DWORD  cchBase64ExponentLen, 
                           BYTE**       ppbBinaryRSAKey, 
                           DWORD*       pcbBinaryRSAKeyLen, 
                           DWORD*       pcbRSAExponent);

HRESULT DecodeBase64String(const CHAR*  pszBase64String,
                           const DWORD  cchBase64StringLen,
                           BYTE**       ppbBinaryNumber,
                           DWORD*       pcbBinaryNumberLen);
