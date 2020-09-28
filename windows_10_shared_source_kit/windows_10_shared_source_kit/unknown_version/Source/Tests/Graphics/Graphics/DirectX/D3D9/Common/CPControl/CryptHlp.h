//***************************************************************************** 
// 
// Microsoft Windows Media 
// Copyright (C) Microsoft Corporation. All rights reserved. 
//
// FileName:            crypthlp.h
//
// Abstract:
//
//*****************************************************************************


#pragma once

#ifndef __CRYPTHLP_H_
#define __CRYPTHLP_H_

#ifndef QWORD
#define QWORD unsigned __int64
#endif

#define SAFE_ARRAYDELETE(p)         delete [] p; p = NULL;

#ifndef CHECKHR_GOTO
#define CHECKHR_GOTO( val, label ) hr = (val); if( FAILED( hr ) ) { goto label; }
#endif

#include <windows.h>
#include "aes.h"
#include "rsa.h"
#include <wincrypt.h>
//#include "wmsstd.h"

typedef enum AESHELPER_PROCESS_OPTION
{
    DECRYPT = 0,
    ENCRYPT = 1,
} AESHELPER_PROCESS_OPTION;

namespace Crescent
{
////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

extern HRESULT ProtectBlob( BYTE *pbBlob, DWORD cbBlob, BOOL fEncrypt );

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// Helper class for some math and bitwise operations
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
class CCryptHlp
{
public:

    static inline void XOR( BYTE *lpbLHS, BYTE *lpbRHS, DWORD cbSize = AES_BLOCKLEN )
    {
        for( DWORD i = 0; i < cbSize; i++ )
        {
            lpbLHS[i] ^= lpbRHS[i];
        }
    }

    static inline void LShift(  __in_ecount(AES_BLOCKLEN) BYTE lpbOpd[AES_BLOCKLEN], __out_ecount(AES_BLOCKLEN) BYTE lpbRes[AES_BLOCKLEN] )
    {
        for( DWORD i = 0; i < AES_BLOCKLEN; i++ )
        {
            lpbRes[i] = lpbOpd[i] << 1;
            if( i < AES_BLOCKLEN - 1 )
            {
                lpbRes[i] |= ( (unsigned char)lpbOpd[i+1] ) >> 7;
            }
        }
    }

    static inline void RShift( __in_ecount(AES_BLOCKLEN) BYTE lpbOpd[AES_BLOCKLEN], __out_ecount(AES_BLOCKLEN) BYTE lpbRes[AES_BLOCKLEN] )
    {
        for( DWORD i = 0; i < AES_BLOCKLEN; i++ )
        {
            lpbRes[i] = ( (unsigned char)lpbOpd[i] ) >> 1;
            if( i > 0 )
            {
                lpbRes[i] |= ( lpbOpd[i-1] ) << 7;
            }
        }
    }

    //
    // Random data generation utility
    //
        
    static HRESULT GenerateRandom( 
               BYTE *pbData, 
               DWORD cbData 
               );

#if DBG 
    static void PrintByteArray(
               BYTE * pb, 
               DWORD cb);
#endif
    
};


////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// Helper class for the Mask Generation Function MGF1. This implementation
// uses SHA 256 as the underlying hash function.
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

class CMGF1
{
public:

    //
    // GenerateMask: Generates the MGF1 mask (per PKCS#1 v2.1) using SHA
    //               256 as the underlying hash.
    //
    // Parameters:
    //
    // pbSeed - Pointer to the seed. Can be NULL if cbSeed = 0
    // 
    // cbSeed - Size of the seed in bytes
    //
    // cbMask - Desired size of the output mask in bytes. Cannot be 0.
    //
    // ppbMask - Pointer to receive the generated mask. The buffer is allocated
    //           by GenerateMask.
    //
    // Return Values:
    //
    // S_OK - Success. Caller must delete the output mask buffer
    //  
    // Other failure HRESULT. 
    //

    static HRESULT GenerateMask( IN const BYTE * pbSeed,
                                 IN const DWORD  cbSeed,
                                 IN const DWORD  cbMask,
                                 OUT BYTE **ppbMask );

};

////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
//
// Helper class to support RSA encryption, decryption and key creation.
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////

class CRSAHelper
{

public:

    CRSAHelper();
    virtual ~CRSAHelper();

    //
    // CreateKeys: Creates and stores a public-private key pair internally. 
    //
    // Parameters:
    //
    // dwKeyLenInBits - Number of bits in the key viz. RSA modulus    
    //
    // Return values:
    //
    // S_OK - Success.
    //
    // Other error HRESULT
    //


    HRESULT CreateKeys(
                IN DWORD dwKeyLenInBits );

    //
    // GetPublicKey: Get the public key
    //
    // Parameters:
    //
    // pbPubKey - Pointer to a buffer to receive the public key. Must be
    //            allocated by the caller. If NULL, the size of the buffer
    //            required is returned in pcbPubKey.
    // 
    // pcbPubKey - Pointer to a DWORD containing the size of pbPubKey buffer
    //             on input. On output, contains the actual size of the public
    //             key copied into pbPubKey.
    //
    // Return values:
    //
    // S_OK - Success
    //
    // HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY ) - pbPubKey is NULL or
    //             the supplied buffer is insufficient in size. pcbPubKey 
    //             contains the size of buffer that must be supplied.
    //
    // Other error HRESULT.
    //


    HRESULT GetPublicKey( 
                LPBYTE pbPubKey, 
                DWORD *pcbPubKey );


    //
    // GetPublicKey: Get the public key as exponent and modulus
    //
    // Parameters:
    //
    // pdwPubExp - Pointer to DWORD for receiving the public Key Exponent. 
    //             Cannot be NULL.
    //
    // pbPubMod - Pointer to a buffer to receive the public key Modulus. Must be
    //            allocated by the caller. If NULL, the size of the buffer
    //            required is returned in pcbPubMod. The modulus is returned in
    //            host byte order.
    //
    // pcbPubMod - Pointer to a DWORD containing the size of pbPubMod buffer
    //             on input. On output, contains the actual size of the public
    //             key Modulus copied into pbPubMod
    //
    // Return values:
    //
    // S_OK - Success
    //
    // HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY ) - pbPubMod is NULL or
    //             the supplied buffer is insufficient in size. pcbPubMod 
    //             contains the size of buffer that must be supplied.
    //
    // Other error hr.
    //

    HRESULT GetPublicKey( 
                DWORD   *pdwPubExp,
                LPBYTE   pbPubMod,
                DWORD   *pcbPubMod );
    
    //
    // SetPublicKey: Set the public key. An internal copy of the supplied buffer
    //               is made.
    //
    // Parameters:
    //
    // pbPubKey - Pointer to the public key. Cannot be NULL.
    //
    // cbPubKey - Size of the public key in bytes
    //
    // Return values:
    //
    // S_OK - Success
    //
    // Other error HRESULT.
    //

    HRESULT SetPublicKey( 
                const BYTE *pbPubKey, 
                DWORD cbPubKey );


    //
    // SetPublicKey: Set the public key specifying its exponent and modulus
    //
    // Parameters:
    //
    // dwPubKeyExp - RSA Public key exponent
    //
    // pbPubKeyMod - Pointer to the RSA Public Key modulus. The modulus must be
    //               in host byte order. Cannot be NULL.
    //
    // cbPubKeyMod - Size of the RSA public Key modulus, in bytes.
    //
    // Return values:
    //
    // S_OK - Success
    //
    // Other error HRESULT.
    //

    HRESULT SetPublicKey( 
                DWORD dwPubKeyExp,
                const BYTE *pbPubKeyMod,
                DWORD cbPubKeyMod
                );

    //
    // GetPrivateKey: Get the private key
    //
    // Parameters:
    //
    // pbPrvKey - Pointer to a buffer to receive the private key. Must be
    //            allocated by the caller. If NULL, the size of the buffer
    //            required is returned in pcbPrvKey.
    // 
    // pcbPrvKey - Pointer to a DWORD containing the size of pbPrvKey buffer
    //             on input. On output, contains the actual size of the private
    //             key copied into pbPrvKey.
    //
    // Return values:
    //
    // S_OK - Success
    //
    // HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY ) - pbPrvKey is NULL or
    //             the supplied buffer is insufficient in size. pcbPrvKey 
    //             contains the size of buffer that must be supplied.
    //
    // Other error HRESULT
    //

    HRESULT GetPrivateKey( 
                LPBYTE  pbPrvKey, 
                DWORD  *pcbPrvKey
                );

    //
    // SetPrivateKey: Set the Private key. An internal copy of the supplied buffer
    //               is made.
    //
    // Parameters:
    //
    // pbPrvKey - Pointer to the private key. Cannot be NULL.
    //
    // cbPrvKey - Size of the private key in bytes
    //
    // Return values:
    //
    // S_OK - Success
    //
    // Other error HRESULT.
    //

    HRESULT SetPrivateKey( 
                const BYTE *pbPrvKey, 
                DWORD   cbPrvKey 
                );

    //
    // RSAEncPublic: Encrypt using the public key. The input data is merely
    //               padded with zero's (if necessary). The public key must
    //               must already be set.
    //
    // Parameters:
    //
    // pbDataIn - Input data to be encrypted
    //
    // cbDataIn - Size of input data in bytes
    //
    // ppbDataOut - Pointer to output encrypted data buffer. This buffer is 
    //              allocated by RSAEncPublic. Cannot be NULL.
    //
    // pcbDataOut - Pointer to a DWORD to receive the size of the output
    //              encrypted data buffer. Cannot be NULL.
    //
    // Return Values:
    //
    // S_OK - Success. The caller must delete the output encrypted data buffer
    //
    // Other HRESULT indicates failure
    //

    HRESULT RSAEncPublic(
                IN const BYTE *pbDataIn,
                IN DWORD cbDataIn,
                OUT LPBYTE *ppbDataOut,
                OUT DWORD *pcbDataOut 
                );


    //
    // RSAOAEPEncPublic: Encrypt with the public key using the standard prescribed by
    //                   PKCS#1 v2.1. The public key must already be set.
    //
    // Parameters:
    //
    // pbDataIn - Input data to be encrypted
    //
    // cbDataIn - Size of input data in bytes
    //
    // ppbDataOut - Pointer to output encrypted data buffer. This buffer is 
    //              allocated by RSAOAEPEncPublic. Cannot be NULL.
    //
    // pcbDataOut - Pointer to a DWORD to receive the size of the output
    //              encrypted data buffer. Cannot be NULL
    //
    // Return Values:
    //
    // S_OK - Success. The caller must delete the output encrypted data buffer
    //
    // Other HRESULT indicates failure
    //

    HRESULT RSAOAEPEncPublic(
                IN const BYTE *pbDataIn,
                IN DWORD cbDataIn,
                OUT LPBYTE *ppbDataOut,
                OUT DWORD *pcbDataOut );

    //
    // RSADecPrivate: Decrypt the data using the private key. The private key
    //                must already be set.
    //
    // Parameters:
    //
    // pbDataIn - Pointer to the input encrypted data. Cannot be NULL.
    //
    // cbDataIn - Size of the input data in bytes.
    //
    // ppbDataOut - Pointer to receive the output decrypted data. This buffer is
    //              allocated by RSADecPrivate. Cannot be NULL
    //
    // pcbDataOut - Pointer to a DWORD to receive the size of the output
    //              decrypted data buffer. Cannot be NULL
    //
    // Return values:
    //
    // S_OK - Success. The caller must delete the output decrypted data buffer
    //
    // Other HRESULT indicates failure
    //

    HRESULT RSADecPrivate(
                IN const BYTE *pbDataIn,
                IN DWORD cbDataIn,
                OUT LPBYTE *ppbDataOut,
                OUT DWORD *pcbDataOut );

    //
    // RSAOAEPDecPrivate: Decrypt the data with the private key using the standard
    //                    prescribed by PKCS#1 v2.1. The private key must already be 
    //                    set.
    //
    // Parameters:
    //
    // pbDataIn - Pointer to the input encrypted data. Cannot be NULL.
    //
    // cbDataIn - Size of the input data in bytes.
    //
    // ppbDataOut - Pointer to receive the output decrypted data. This buffer is
    //              allocated by RSAOAEPDecPrivate. Cannot be NULL
    //
    // pcbDataOut - Pointer to a DWORD to receive the size of the output
    //              decrypted data buffer. Cannot be NULL
    //
    // Return values:
    //
    // S_OK - Success. The caller must delete the output decrypted data buffer
    //
    // Other HRESULT indicates failure
    //

    HRESULT RSAOAEPDecPrivate(
                IN const BYTE *pbDataIn,
                IN DWORD cbDataIn,
                OUT LPBYTE *ppbDataOut,
                OUT DWORD *pcbDataOut );

    //
    // RSASign: Sign the input data using the private key. This only performs
    //          a decryption operation on the input using the private key. The
    //          input is padded with zeros (if necessary). The private key must
    //          already be set.
    //
    // Parameters:
    //
    // pbDataIn - Pointer to the input data to be signed
    //
    // cbDataIn - Size of the input data in bytes
    //
    // ppbDataOut - Pointer to receive the output signature. This buffer is 
    //              allocated by RSASign. Cannot be NULL 
    //
    // pcbDataOut - Pointer to a DWORD to receive the size of the output signature.
    //              Cannot be NULL.
    //
    // Return values:
    //
    // S_OK - Success. The caller must delete the output decrypted data buffer
    //
    // Other error HRESULT
    //

    HRESULT RSASign(
                IN const BYTE *pbDataIn,
                IN DWORD cbDataIn,
                OUT LPBYTE *ppbDataOut,
                OUT DWORD *pcbDataOut );

    //
    // RSAPSSSign: Sign the input data using the private key based on the standard
    //             prescribed by PKCS#1 v2.1. The signing uses MGF1 as the mask 
    //             generation function and SHA 256 as the underlying Hash. The
    //             private key must already be set.
    //
    // Parameters:
    //
    // pbDataIn - Pointer to the input data to be signed
    //
    // cbDataIn - Size of the input data in bytes
    //
    // ppbDigest - Pointer to receive the digest of the message computed while
    //             signing. The buffer is allocated by RSAPSSSign. Can be NULL if 
    //             the digest is not required.
    //
    // pcbDigest - Pointer to a DWORD to receive the size of the output digest. 
    //             Cannot be NULL if ppbDigest is non-NULL.
    //
    // ppbDataOut - Pointer to receive the output signature. This buffer is 
    //              allocated by RSAPSSSign. Cannot be NULL 
    //
    // pcbDataOut - Pointer to a DWORD to receive the size of the output signature.
    //              Cannot be NULL.
    //
    // Return values:
    //
    // S_OK - Success. The caller must delete the output signature buffer and output
    //        digest buffer (if it was requested).
    //
    // Other error HRESULT
    //

    HRESULT RSAPSSSign(
                IN const BYTE *pbDataIn,
                IN DWORD cbDataIn,
                OUT LPBYTE *ppbDigest,
                OUT DWORD *pcbDigest,
                OUT LPBYTE *ppbSign,
                OUT DWORD *pcbSign );

    //
    // RSAVerify: Performs an Encryption operation on the input data using the public
    //            key. 
    //
    // Parameters:
    //
    // pbDataIn - Pointer to the input data
    //
    // cbDataIn - Size of the input data in bytes
    //
    // ppbDataOut - Pointer to receive the output. This buffer is allocated by RSASign. 
    //              Cannot be NULL 
    //
    // pcbDataOut - Pointer to a DWORD to receive the size of the output buffer.
    //              Cannot be NULL.
    //
    // Return values:
    //
    // S_OK - Success. The caller must delete the output buffer
    //
    // Other error HRESULT
    //

    HRESULT RSAVerify(
                IN const BYTE *pbDataIn,
                IN DWORD cbDataIn,
                OUT LPBYTE *ppbDataOut,
                OUT DWORD *pcbDataOut );

    //
    // RSAPSSVerify: Verifies the signature on the input data using the public 
    //               key, based on the standard prescribed by PKCS#1 v2.1. 
    //               This implementation uses MGF1 as the mask generation function 
    //               and SHA 256 as the underlying Hash. The public key must already 
    //               be set.
    //
    // Parameters:
    //
    // pbDataIn - Pointer to the input data that was signed
    //
    // cbDataIn - Size of the input data in bytes
    //
    // pbDigest - Pointer to the digest of the input data computed while
    //            signing. This can be NULL. RSAPSSVerify will match this digest with
    //            the digest that it computes on the input data during verification.
    //
    // cbDigest - Size of the supplied digest.
    //
    // pbSignature - Pointer to the signature on the input data that needs to be verified.
    //               Cannot be NULL.
    //
    // cbSignature - Size of the signature in bytes.
    //
    // Return values:
    //
    // S_OK - Success. The caller must delete the output signature buffer and output
    //        digest buffer (if it was requested).
    //
    // Other error HRESULT
    //

    HRESULT RSAPSSVerify(
                IN const BYTE *pbDataIn,
                IN DWORD cbDataIn,
                IN const BYTE *pbDigest,
                IN const DWORD cbDigest,
                IN const BYTE *pbSignature,
                IN const DWORD cbSignature );


    void Reset( )
    {
        _ResetPubKey();
        _ResetPrvKey();
    }

    inline BOOL IsPublicKeySet()
    {
        return( m_pbPubKey != NULL );
    }

    inline BOOL IsPrivateKeySet()
    {
        return( m_pbPrvKey != NULL );
    }
    
    BOOL IsSamePublicKey( CRSAHelper *pRSAHelper );

protected:

    _forceinline void _ProtectKey( BOOL fPub, BOOL fEnc )
    {
        DWORD cbAlloc = fPub? m_cbPubKey: m_cbPrvKey;
        if( cbAlloc % AES_BLOCKLEN > 0 )
        {
            cbAlloc += AES_BLOCKLEN - ( cbAlloc % AES_BLOCKLEN );
        }

        //
        // ProtectBlob only fails if size is not multiple of AES_BLOCKLEN.
        // We can safely ignore this error.
        //
        if( fPub )
        {
            (void)ProtectBlob( m_pbPubKey, cbAlloc, fEnc );
        }
        else
        {
            (void)ProtectBlob( m_pbPrvKey, cbAlloc, fEnc );
        }
        return;
    }

    //
    // _OAEPEncode takes an input message and computes the encoded message
    // (EM) per the PKCS#1 v2.1 standard. The EM is then RSA encrypted.
    //
    // pbMsg - Pointer to message to be encoded
    //
    // cbMsg - Size of message in bytes
    //
    // pbEncodedMsg - Buffer to receive EM. Must be of same length as the
    //                public key modulus
    //
    

    HRESULT _OAEPEncode(
                IN const BYTE *pbMsg,
                IN const DWORD cbMsg,
                OUT LPBYTE pbEncodedMsg );

    //
    // _OAEPDecode takes a decrypted, encoded message (EM) per the PKCS#1 v2.1
    // standard and decodes it to get the orignal message.
    //
    // pbEncodedMsg - Decrypted EM. Must be of same length as the private 
    //                key Modulus
    //
    // ppbDecodedMsg - Pointer to receive the decoded message buffer. The 
    //                 buffer is allocated by _OAEPDecode. Cannot be NULL.
    //
    // pcbDecodedMsg - Pointer to DWORD to receive the size of the decoded
    //                 message. Cannot be NULL
    //

    HRESULT _OAEPDecode(
                IN BYTE *pbEncodedMsg,
                OUT LPBYTE *ppbDecodedMsg,
                OUT DWORD *pcbDecodedMsg );

    //
    // _PSSEncode takes an input message and computes the Encoded Message (EM)
    // for signing the message, based on PKCS#1 2.1 standard. 
    //
    // pbMsg - Pointer to input message.
    //
    // cbMsg - Size of input message in bytes
    //
    // cbEMBits - Number of bits desired in EM. This is one less than the bit 
    //            size of the key modulus.
    // 
    // pbEncodedMsg - Pointer to receive the EM. Cannot be NULL.
    //
    // cbEncodedMsg - Size of the EM buffer in bytes. Must be size of key Modulus.
    //
    // ppbDigest - Optional pointer to a buffer containing the digest computed
    //             Can be NULL.
    //
    // pcbDigest - Pointer to DWORD to receive the size of the digest.
    //
    
    HRESULT _PSSEncode(
                IN const BYTE *pbMsg,
                IN const DWORD cbMsg,
                IN DWORD cbEMBits,
                OUT LPBYTE pbEncodedMsg,
                IN const DWORD cbEncodedMsg,
                OUT LPBYTE *ppbDigest,
                OUT DWORD *pcbDigest );

    //
    // _PSSVerify takes an input message, optional digest and decrypted EM computed
    // from the signature. The verification is based on PKCS#1 v2.1 standard.
    //
    // pbMsg - Pointer to input message
    //
    // cbMsg - Size of the input message in bytes.
    //
    // pbDigest - Optional pointer to a digest of the input message. _PSSVerify
    //            checks that this digest matches the digest of the input message
    //            that it computes during verification.
    //
    // cbDigest - Size of the digest in bytes
    //
    // pbEncodedMsg - Pointer to the EM (obtained by decrypting the signature)
    //
    // cbEMButs - Number of bits desired in EM.
    //

    HRESULT _PSSVerify(
                IN const BYTE *pbMsg,
                IN const DWORD cbMsg,
                IN const BYTE *pbDigest,
                IN const DWORD cbDigest,
                IN BYTE *pbEncodedMsg,
                IN DWORD cbEMBits );

    //
    // Internal helper: _RSAProcess processes the input data either by encrypting
    // it using the public key or decrypting it using the private key.
    //
    // pbDataIn - Pointer to input data.
    //
    // cbDataIn - Size of the input data
    //
    // fPublicKey - If set, requests encryption with public Key. Else, selects
    //              decryption with private key.
    //
    // ppbDataOut - Pointer to receive output buffer. This buffer is allocated by
    //              _RSAProcess.
    //
    // pcbDataOut - Pointer to DWORD to receive the size of the output buffer.
    //

    HRESULT _RSAProcess(
                IN const BYTE *pbDataIn,
                IN DWORD   cbDataIn,
                IN BOOL    fPublicKey, 
                OUT LPBYTE *ppbDataOut,
                OUT DWORD  *pcbDataOut );

    void _ResetPubKey( );
    void _ResetPrvKey( );
    
    _forceinline DWORD _PubModLen()
    {
        DWORD cb;
        if( m_pbPubKey == NULL )
        {
            return( 0 );
        }
        _ProtectKey( TRUE, FALSE );
        //round to 2048 bits for OPM and COPP
        cb = (((LPBSAFE_PUB_KEY)m_pbPubKey)->bitlen / (8*256))*256;
        _ProtectKey( TRUE, TRUE );

        return( cb );
    }

    _forceinline DWORD _PrvModLen()
    {
        DWORD cb;
        if( m_pbPrvKey == NULL )
        {
            return( 0 );
        }
        _ProtectKey( FALSE, FALSE );
        //round to 2048 bits for OPM and COPP
        cb = (((LPBSAFE_PRV_KEY)m_pbPrvKey)->bitlen / (8*256))*256;
        _ProtectKey( FALSE, TRUE );

        return( cb );
    }

protected:

    __field_ecount_opt(m_cbPubKey) BYTE   *m_pbPubKey;
    DWORD   m_cbPubKey;
    __field_ecount(m_cbPrvKey) BYTE   *m_pbPrvKey;
    DWORD   m_cbPrvKey;
};

////////////////////////////////////////////////////////////////////////////
//
// Helper class to support AES encryption, decryption, signing and 
// key creation.
//
////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////
class CAESHelper
{
public:

    CAESHelper();
    virtual ~CAESHelper();

    //
    // GenerateKey: Creates a random internal secret key for AES operations
    //
    // Parameters:
    //
    // cbKey - Desired size of the secret key in bytes. This must be one of the
    //         standard AES key sizes (128, 192 or256 bytes)
    //
    // Return values:
    //
    // S_OK - Success
    //
    // Other error HRESULT.
    //
    

    HRESULT GenerateKey( 
                DWORD cbKey 
                );
    
    //
    // SetKey: Sets a given secret key for AES operations. Makes an internal 
    //         copy of the secret key.
    //
    // Parameters:
    //
    // pbKey - Pointer to the key. Cannot be NULL.
    //
    // cbKey - Desired size of the secret key in bytes. This must be one of the
    //         standard AES key sizes (128, 192 or256 bytes)
    //
    // Return values:
    //
    // S_OK - Success
    //
    // Other error HRESULT.
    //

    HRESULT SetKey( 
                const BYTE *pbKey,
                DWORD cbKey 
                );

    //
    // GetKey: Get the internal secret key. 
    //
    // Parameters:
    //
    // pbKey - Pointer to a buffer to receive the key. If NULL, the function
    //         returns the buffer size required. If non-NULL the buffer must be 
    //         large enough to receive the key
    //
    // pcbKey - Pointer to a DWORD that on input contains the size of the key
    //          buffer. On output, it receives the size of the key copied into
    //          pbKey.
    //
    // Return values:
    //
    // S_OK - Success
    //
    // HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY ) - pbKey is NULL or
    //             the supplied buffer is insufficient in size. pcbKey 
    //             contains the size of buffer that must be supplied.
    //
    // Other error HRESULT.
    //

    HRESULT GetKey(
                LPBYTE pbKey,
                DWORD *pcbKey 
                );
    
    //
    // ReleaseKey: Reset the internal secret Key.
    //
    // Parameters:
    //
    // Return Value:
    //
    // S_OK - Success
    //
    // Other error HRESULT.
    //
    
    HRESULT ReleaseKey( );
    

    //
    // CBCSetInitVector: Configure the Initialization Vector for AES encryption 
    // and decryption in CBC mode
    //
    // Parameters:
    //
    // rgbInitVector: Buffer containing the initialization vector
    //
    // Return Values:
    //
    // S_OK - Success
    //
    // Other error HRESULT
    //
    
    HRESULT CBCSetInitVector( 
                const BYTE rgbInitVector[AES_BLOCKLEN]
                );


    //
    // CBCGetBufferLen: Gets the buffer length required for the output of CBC
    // mode operation
    //
    // Parameters:
    //
    // cbDataIn - Size of the input data to CBC operation, in bytes.
    //
    // Return values:
    //
    // Returns the size in bytes of the output buffer required for the CBC operation.
    //

    DWORD CBCGetBufferLen( 
                DWORD cbDataIn
                );

    //
    // Encrypt/Decrypt data using the internal secret key.
    //
    //      Op:         Specify DECRYPT or ENCRYPT
    //      fFinal:     Indicates if this is the last data block for the CBC 
    //                  mode of operation
    //      pbData:     Pointer to buffer containing the input data. The same
    //                  AES operations are in-place, so this buffer also 
    //                  receives the output data.
    //      pcbData:    Pointer to DWORD. During Input, this contains the size of
    //                  input data. During output, it contains the size of output
    //                  data.
    //      cbBuffer:   Indicates the total size of the data buffer pointed to
    //                  by pbData. The function fails if cbBuffer is not
    //                  sufficient to contain the output data. (Use CBCGetBufferLen
    //                  to calculate the size of output buffer that is required.)
    //

    HRESULT CBCProcessData( 
                AESHELPER_PROCESS_OPTION Op,
                BOOL fFinal,
                BYTE *pbData,
                DWORD *pcbData,
                DWORD cbBuffer
                );

    //
    // Process data using the internal secret key, in the Counter mode
    //
    //      pbData:     Pointer to buffer containing the input data. The same
    //                  AES operations are in-place, so this buffer also 
    //                  receives the output data.
    //      cbData:     Indicates the total size of the data pointed to
    //                  by pbData. The output produced is equal in size to the 
    //                  input
    //      qwDataId:   ID to use for the current data buffer in the counter mode.
    //

    HRESULT CounterProcessData( 
                BYTE *pbData,
                DWORD cbData,
                QWORD &qwDataId,
                QWORD &qwCount
                );

    //
    // Generate the AES OMAC signature using the internal secret key. We ONLY 
    // support signing with 128-bit keys. The specific algorithm used is OMAC-1.
    //
    //      pbData:     Ponter to data to be signed
    //      cbData:     Size of the input data
    //      rgbTag:     Buffer to receive the signature.
    //
    

    HRESULT SignData( 
                BYTE *pbData,
                DWORD cbData,
                BYTE rgbTag[AES_BLOCKLEN]
                );

    //
    // Process data using the internal secret key, in ECB mode
    //
    //      Op:         Specify DECRYPT or ENCRYPT
    //      pbData:     Pointer to buffer containing the input data. The same
    //                  AES operations are in-place, so this buffer also 
    //                  receives the output data.
    //      cbData:     Indicates the total size of the data pointed to
    //                  by pbData. The output produced is equal in size to the 
    //                  input.  The input must be a multiple of the block size.
    //

    HRESULT ECBProcessData(
                AESHELPER_PROCESS_OPTION Op,
                BYTE *pbData,
                DWORD cbData
                );

protected:

    void _ResetKey( );

    __forceinline void _ProtectKey( BOOL fEnc )
    {
        DWORD cbAlloc = m_cbKey;
        if( cbAlloc % AES_BLOCKLEN > 0 )
        {
            cbAlloc += AES_BLOCKLEN - ( cbAlloc % AES_BLOCKLEN );
        }

        //
        // ProtectBlob only fails if size is not multiple of AES_BLOCKLEN.
        // We can safely ignore this error.
        //
        (void)ProtectBlob( m_pbKey, cbAlloc, fEnc );
        return;
    }

    //
    // Internal helper to compute the OMAC signature.
    //
    HRESULT _GenerateSignTag( 
                BYTE *pbData,
                DWORD cbData,
                BYTE rgbLU[AES_BLOCKLEN],
                BYTE rgbLU_1[AES_BLOCKLEN],
                BYTE rgbTag[AES_BLOCKLEN]
                );

    //
    // Internal helper to compute the OMAC signing constants
    //
    HRESULT _GenerateSignInfo( 
                BYTE rgbLU[AES_BLOCKLEN],
                BYTE rgbLU_1[AES_BLOCKLEN]
                );
    QWORD _HTONQWORD( QWORD &qwArg );

protected:

    __field_ecount(m_cbKey) BYTE      *m_pbKey;
    DWORD      m_cbKey;

    BYTE       m_rgbCBCInitVector[AES_BLOCKLEN];
    BYTE       m_rgbCBCLastBlock[AES_BLOCKLEN];
};


#endif // CRYPTHLP

};

