
//+-------------------------------------------------------------------------
//
//  Microsoft NetShow
//  Copyright (C) Microsoft Corporation 1998.
//
//  File:       CryptHlp.cpp
//
//  Contents:   
//
//  History:    01/13/04            Created
//  
//--------------------------------------------------------------------------
//#include "stdafx.h"

#include <winsock2.h>

#include "CryptHlp.h"
#include <sha.h>
#include <sha2.h>
#include <stdio.h>
#include <assert.h>
#include <ntsecapi.h>

namespace Crescent
{

#define SETHR_GOTO( val, label ) hr = val; goto label;
#define SAFE_SECUREMEMARRAYDELETE(p,c) if( NULL != p ) { SecureZeroMemory( p, c ); delete [] p; p = NULL; }
#define MAKEQWORD(a, b)     MAKEULONGLONG((a),(b))
#define LODWORD(_qw)    ((unsigned long)(_qw))
#define HIDWORD(_qw)    ((unsigned long)((_qw) >> 32))
#define MAKEULONGLONG(a, b) ((ULONGLONG)(((DWORD)(a)) | ((ULONGLONG)((DWORD)(b))) << 32))

u_long WSAAPI
htonl (
    IN u_long Source
    )
{

    return ((Source)              << (8 * 3)) |
              ((Source & 0x0000FF00) << (8 * 1)) |
              ((Source & 0x00FF0000) >> (8 * 1)) |
              ((Source)              >> (8 * 3));

}

////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//
HRESULT CCryptHlp::GenerateRandom( 
    BYTE *pbData, 
    DWORD cbData )
{
    HRESULT hr = S_OK;

    if( pbData == NULL )
    {
        return( E_INVALIDARG );
    }
    
    if( RtlGenRandom( pbData, cbData ) == FALSE )
    {
        hr = E_FAIL;
    }

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//

#if DBG 

void CCryptHlp::PrintByteArray( BYTE * pb, DWORD cb )
{
    DWORD i;
    for( i = 0; i < cb; i++ )
    {
        printf( "0x%02x", pb[i] );
        if( i + 1 != cb )
        {
            printf( ", " );
        }
        
        if( ( i & 0x0f ) == 15 )
        {
            printf( "\r\n" );
        }
    }
}

#endif

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////
// Results:
//  S_OK
//  E_INVALIDARG
//  E_OUTOFMEMORY
//
HRESULT 
CMGF1::GenerateMask( 
    IN const BYTE * pbSeed,
    IN const DWORD  cbSeed,
    IN const DWORD  cbMask,
    OUT BYTE **ppbMask 
    )
{
    HRESULT hr = S_OK;
    DWORD nIterations = 0;
    DWORD cbHashInput = 0;
    DWORD cbTotalMask = 0;
    DWORD i = 0;
    DWORD dwNetCounter = 0;
    BYTE *pbHashInput = NULL;
    BYTE *pbMask = NULL;
    BYTE *pbHashOut = NULL;
    SHA512_CTX ShaContext;
        
    cbHashInput = cbSeed + sizeof( DWORD );

    if( ( pbSeed == NULL && cbSeed > 0 ) || 
        cbHashInput < cbSeed ||                 // overflow
        cbMask == 0 )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }

    nIterations = cbMask / SHA512_DIGEST_LEN;
    if( cbMask % SHA512_DIGEST_LEN > 0 )
    {
        nIterations++;
    }

    cbTotalMask = nIterations * SHA512_DIGEST_LEN;

    pbMask = new BYTE[ cbTotalMask ];
    if( pbMask == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    pbHashInput = new BYTE[ cbHashInput ];
    if( pbHashInput == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    CopyMemory( pbHashInput, pbSeed, cbSeed );

    pbHashOut = pbMask;
        
    for( i = 0; i < nIterations; i++ )
    {
        SHA512Init( &ShaContext );

        dwNetCounter = htonl( i );
        CopyMemory( pbHashInput + cbSeed, (BYTE *)&dwNetCounter, sizeof( DWORD ) );

        SHA512Update( &ShaContext, pbHashInput, cbHashInput  );
        SHA512Final( &ShaContext, pbHashOut );

        pbHashOut += SHA512_DIGEST_LEN;
    }

    //
    // Wipe off the mask bits that we don't need
    //
    ZeroMemory( pbMask + cbMask, cbTotalMask - cbMask );

    *ppbMask = pbMask;
    pbMask = NULL;

done:

    SAFE_ARRAYDELETE( pbMask );
    SAFE_ARRAYDELETE( pbHashInput );
    return( hr );
}

///////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////


CRSAHelper::CRSAHelper( ) :
    m_pbPubKey( NULL ),
    m_cbPubKey( 0 ),
    m_pbPrvKey( NULL ),
    m_cbPrvKey( 0 )
{
}

////////////////////////////////////////////////////////////////////////////
//
CRSAHelper::~CRSAHelper( )
{
    _ResetPubKey();
    _ResetPrvKey();
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::CreateKeys(
    IN DWORD dwKeyLenInBits
    )
{
    HRESULT hr = S_OK;
    DWORD dwLocalBits = dwKeyLenInBits;
    DWORD cbAllocPub = 0, cbAllocPrv = 0;
        
    _ResetPubKey();
    _ResetPrvKey();

    if( !BSafeComputeKeySizes( &m_cbPubKey, &m_cbPrvKey, &dwLocalBits ) )
    {
        return( E_FAIL );
    }
    if( m_cbPrvKey == 0 || m_cbPubKey == 0 )
    {
        return( E_UNEXPECTED );
    }

    cbAllocPub = m_cbPubKey;
    if( cbAllocPub % AES_BLOCKLEN > 0 )
    {
        cbAllocPub += AES_BLOCKLEN - ( cbAllocPub % AES_BLOCKLEN );
    }

    m_pbPubKey = new BYTE[cbAllocPub];
    if( m_pbPubKey == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    cbAllocPrv = m_cbPrvKey;
    if( cbAllocPrv % AES_BLOCKLEN > 0 )
    {
        cbAllocPrv += AES_BLOCKLEN - ( cbAllocPrv % AES_BLOCKLEN );
    }

    m_pbPrvKey = new BYTE[cbAllocPrv];
    if( m_pbPrvKey == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    if( !BSafeMakeKeyPair( ( LPBSAFE_PUB_KEY )m_pbPubKey,
                           ( LPBSAFE_PRV_KEY )m_pbPrvKey,
                           dwKeyLenInBits ) )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    _ProtectKey( TRUE, TRUE );
    _ProtectKey( FALSE, TRUE );

done:
    
    if( FAILED( hr ) )
    {
        if( m_pbPubKey )
        {
            delete[] m_pbPubKey;
        }
        if( m_pbPrvKey )
        {
            delete[] m_pbPrvKey;
        }
        m_cbPubKey = m_cbPrvKey = 0;
    }

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  ERROR_NOT_ENOUGH_MEMORY
//
HRESULT CRSAHelper::GetPublicKey( 
    LPBYTE pbPubKey, 
    DWORD *pcbPubKey )
{
    if( pcbPubKey == NULL )
    {
        return( E_INVALIDARG );
    }

    if( m_pbPubKey == NULL || m_cbPubKey == 0 )
    {
        return( E_FAIL );
    }

    if( pbPubKey == NULL || *pcbPubKey < m_cbPubKey )
    {
        *pcbPubKey = m_cbPubKey;
        return( HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY ) );
    }

    _ProtectKey( TRUE, FALSE );
    memcpy( pbPubKey, m_pbPubKey, m_cbPubKey );
    _ProtectKey( TRUE, TRUE );

    *pcbPubKey = m_cbPubKey;

    return( S_OK );    
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::SetPublicKey( 
    const BYTE * pbPubKey, 
    DWORD cbPubKey )
{
    HRESULT hr = S_OK;
    DWORD cbAlloc = cbPubKey;

    if( pbPubKey == NULL || cbPubKey == 0 )
    {
        return( E_INVALIDARG );
    }
    
    _ResetPubKey();
    
    if( cbAlloc % AES_BLOCKLEN > 0 )
    {
        cbAlloc += AES_BLOCKLEN - ( cbAlloc % AES_BLOCKLEN );
    }

    m_pbPubKey = new BYTE[cbAlloc];
    if( m_pbPubKey == NULL )
    {
        hr = E_OUTOFMEMORY;
        goto done;
    }
    
    CopyMemory( m_pbPubKey, pbPubKey, cbPubKey );
    m_cbPubKey = cbPubKey;
    _ProtectKey( TRUE, TRUE );
    
done:

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  ERROR_NOT_ENOUGH_MEMORY
//
HRESULT CRSAHelper::GetPublicKey( 
    DWORD   *pdwPubExp,
    LPBYTE   pbPubMod,
    DWORD   *pcbPubMod )
{
    HRESULT hr = S_OK;
    LPBSAFE_PUB_KEY pBSafePubKey = (LPBSAFE_PUB_KEY)m_pbPubKey;

    if( pcbPubMod == NULL || pdwPubExp == NULL )
    {
        return( E_INVALIDARG );
    }
    
    if( pBSafePubKey == NULL )
    {
        return( E_FAIL );
    }

    _ProtectKey( TRUE, FALSE );

    if( pbPubMod == NULL || 
        *pcbPubMod < ( pBSafePubKey->keylen - 8 ) )
    {
        *pcbPubMod = pBSafePubKey->keylen - 8;
        SETHR_GOTO( HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY ), done );
    }

    *pdwPubExp = pBSafePubKey->pubexp;

    CopyMemory( pbPubMod, 
                m_pbPubKey + sizeof( BSAFE_PUB_KEY ),
                pBSafePubKey->keylen - 8 );

    *pcbPubMod = pBSafePubKey->keylen - 8;

done:

    _ProtectKey( TRUE, TRUE );
    return( hr );
}


////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::SetPublicKey( 
    DWORD dwPubKeyExp,
    const BYTE * pbPubKeyMod,
    DWORD cbPubKeyMod
    )
{
    HRESULT hr = S_OK;
    DWORD cbPubKey = 0;
    DWORD cbAlloc = 0;
    LPBSAFE_PUB_KEY pBSafePubKey = NULL;

    if( pbPubKeyMod == NULL || cbPubKeyMod == 0 )
    {
        return( E_INVALIDARG );
    }

    _ResetPubKey();

    cbPubKey = sizeof( BSAFE_PUB_KEY ) + cbPubKeyMod + 8;
    
    //
    // Allocate multiple of AES_BLOCKLEN bytes since we keep the key 
    // encrypted in memory
    //
    cbAlloc = cbPubKey;
    if( cbAlloc % AES_BLOCKLEN > 0 )
    {
        cbAlloc += AES_BLOCKLEN - ( cbAlloc % AES_BLOCKLEN );
    }
   
    pBSafePubKey = (LPBSAFE_PUB_KEY)new BYTE[cbAlloc];
    if( pBSafePubKey == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    ZeroMemory( pBSafePubKey, cbPubKey );

    pBSafePubKey->magic = RSA1;
    pBSafePubKey->keylen = cbPubKeyMod + 8;
    pBSafePubKey->bitlen = cbPubKeyMod * 8;
    pBSafePubKey->datalen = cbPubKeyMod - 1;
    pBSafePubKey->pubexp = dwPubKeyExp;
    
    CopyMemory( (LPBYTE)pBSafePubKey + sizeof( BSAFE_PUB_KEY ),
                pbPubKeyMod,
                cbPubKeyMod );

    m_pbPubKey = (LPBYTE)pBSafePubKey;
    pBSafePubKey = NULL;
    m_cbPubKey = cbPubKey;
    _ProtectKey( TRUE, TRUE );


done:

    if( pBSafePubKey )
    {
        delete[] (BYTE *)pBSafePubKey;
    }

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  ERROR_NOT_ENOUGH_MEMORY
//
HRESULT CRSAHelper::GetPrivateKey( 
    LPBYTE  pbPrvKey, 
    DWORD  *pcbPrvKey )
{
    HRESULT hr = S_OK;
    
    if( pcbPrvKey == NULL )
    {
        return( E_INVALIDARG );
    }

    if( m_pbPrvKey == NULL || m_cbPrvKey == 0 )
    {
        return( E_FAIL );
    }

    if( pbPrvKey == NULL || *pcbPrvKey < m_cbPrvKey )
    {
        *pcbPrvKey = m_cbPrvKey;
        SETHR_GOTO( HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY ), done );
    }

    _ProtectKey( FALSE, FALSE );
    CopyMemory( pbPrvKey, m_pbPrvKey, m_cbPrvKey );
    _ProtectKey( FALSE, TRUE );
    *pcbPrvKey = m_cbPrvKey;

done:
    
    return( hr );    
}


////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::SetPrivateKey( 
    const BYTE *pbPrvKey, 
    DWORD   cbPrvKey 
    )
{
    HRESULT hr = S_OK;
    DWORD cbAlloc = cbPrvKey;
    
    if( pbPrvKey == NULL || cbPrvKey == 0 )
    {
        return( E_INVALIDARG );
    }

    _ResetPrvKey();

    if( cbAlloc % AES_BLOCKLEN > 0 )
    {
        cbAlloc += AES_BLOCKLEN - ( cbAlloc % AES_BLOCKLEN );
    }
    
    m_pbPrvKey = new BYTE[ cbAlloc ];
    if( m_pbPrvKey == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    CopyMemory( m_pbPrvKey, pbPrvKey, cbPrvKey );
    m_cbPrvKey = cbPrvKey;
    _ProtectKey( FALSE, TRUE );

done:
    
    return( hr );    
}

////////////////////////////////////////////////////////////////////////////
//
BOOL CRSAHelper::IsSamePublicKey(
    CRSAHelper *pRSAHelper 
    )
{
    BOOL fMatch = FALSE;

    if( pRSAHelper == NULL )
    {
        return( FALSE );
    }
    
    if( pRSAHelper->m_pbPubKey == NULL && m_pbPubKey == NULL )
    {
        return( TRUE );
    }
    
    if( pRSAHelper->m_pbPubKey == NULL || m_pbPubKey == NULL )
    {
        return( FALSE );
    }

    if( m_cbPubKey != pRSAHelper->m_cbPubKey )
    {
        return( FALSE );
    }

    _ProtectKey( TRUE, FALSE );
    pRSAHelper->_ProtectKey( TRUE, FALSE );
    if( 0 == memcmp( m_pbPubKey, pRSAHelper->m_pbPubKey, m_cbPubKey ) )
    {
        fMatch = TRUE;
    }
    _ProtectKey( TRUE, TRUE );
    pRSAHelper->_ProtectKey( TRUE, TRUE );

    return( fMatch );

}

////////////////////////////////////////////////////////////////////////////
//
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::RSAEncPublic(
    IN const BYTE *pbDataIn,
    IN DWORD cbDataIn,
    OUT LPBYTE *ppbDataOut,
    OUT DWORD *pcbDataOut )
{
    HRESULT hr = S_OK;
    const DWORD cbModulus = _PubModLen();
    
    if( pbDataIn == NULL || ppbDataOut == NULL || pcbDataOut == NULL )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }
    
    //
    // Input data must be smaller in size than the modulus: we do not 
    // support encrypting an arbitrary length data stream
    //
    if( cbModulus == 0 || cbDataIn > cbModulus )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    hr = _RSAProcess( pbDataIn,
                      cbDataIn,
                      TRUE,
                      ppbDataOut,
                      pcbDataOut );
	
    CHECKHR_GOTO( hr, done );
    
 
done:

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::RSAOAEPEncPublic(
    IN const BYTE *pbDataIn,
    IN DWORD cbDataIn,
    OUT LPBYTE *ppbDataOut,
    OUT DWORD *pcbDataOut )
{
    HRESULT hr = S_OK;
    const DWORD cbModulus = _PubModLen();
    BYTE *pbEncodedData = NULL;
    BYTE *pbEncryptedData = NULL;
    DWORD cbEncryptedData = 0;
    BYTE *pbDataOut = NULL;
    DWORD i = 0;
    
    if( pbDataIn == NULL || ppbDataOut == NULL || pcbDataOut == NULL )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }
    
    if( cbModulus == 0 )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Allocate buffer to accomodate EM and OS2IP(EM)
    //
    pbEncodedData = new BYTE[ 2 * cbModulus ];
    if( pbEncodedData == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    //
    // Compute EM
    //
    hr = _OAEPEncode( pbDataIn, 
                      cbDataIn, 
                      pbEncodedData + cbModulus );
    CHECKHR_GOTO( hr, done );

    
    //
    // Perform OS2IP(EM): Reverse EM
    //
    for( i = 0; i < cbModulus; i++ )
    {
        pbEncodedData[ i ] = pbEncodedData[ 2*cbModulus - 1 - i ];
    }

    //
    // Encrypt using the public key
    //
    hr = _RSAProcess( pbEncodedData,
                      cbModulus,
                      TRUE,
                      &pbEncryptedData,
                      &cbEncryptedData );
    CHECKHR_GOTO( hr, done );
    
    //
    // Compute I2OSP for encrypted message: reverse encrypted
    // message and hand out
    //
    pbDataOut = new BYTE[ cbModulus ];
    if( pbDataOut == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    for( i = 0; i < cbModulus; i++ )
    {
        pbDataOut[ i ] = pbEncryptedData[ cbModulus - 1 - i ];
    }

    *ppbDataOut = pbDataOut;
    pbDataOut = NULL;
    *pcbDataOut = cbModulus;

done:

    SAFE_SECUREMEMARRAYDELETE( pbEncodedData, 2 * cbModulus );
    SAFE_ARRAYDELETE( pbEncryptedData );
    SAFE_ARRAYDELETE( pbDataOut );

    return( hr );
}


////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::RSADecPrivate(
    IN const BYTE *pbDataIn,
    IN DWORD cbDataIn,
    OUT LPBYTE *ppbDataOut,
    OUT DWORD *pcbDataOut )
{
    HRESULT hr = S_OK;
    const DWORD cbModulus = _PrvModLen();
    
    if( pbDataIn == NULL || ppbDataOut == NULL || pcbDataOut == NULL )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }
    
    //
    // Input data for decryption must be equal in size to the modulus 
    //
    if( cbModulus == 0 || cbDataIn != cbModulus )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    hr = _RSAProcess( pbDataIn,
                      cbDataIn,
                      FALSE,
                      ppbDataOut,
                      pcbDataOut );
	
    CHECKHR_GOTO( hr, done );


done:

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::RSAOAEPDecPrivate(
    IN const BYTE *pbDataIn,
    IN DWORD cbDataIn,
    OUT LPBYTE *ppbDataOut,
    OUT DWORD *pcbDataOut )
{
    HRESULT hr = S_OK;
    const DWORD cbModulus = _PrvModLen();
    BYTE *pbDataBuff = NULL;
    BYTE *pbDecryptedData = NULL;
    DWORD cbDecryptedData = 0;
    BYTE *pbDecodedMsg = NULL;
    DWORD cbDecodedMsg = 0;
    DWORD i = 0;
    
    if( pbDataIn == NULL || ppbDataOut == NULL || pcbDataOut == NULL )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }
    
    //
    // Input data for OAEP decryption must be equal to the modulus length
    //
    if( cbModulus == 0 || cbDataIn != cbModulus )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Perform OS2IP on encrypted data: reverse input data
    //
    pbDataBuff = new BYTE[ cbModulus ];
    if( pbDataBuff == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }
    for( i = 0; i < cbModulus; i++ )
    {
        pbDataBuff[ i ] = pbDataIn[ cbModulus - 1 - i ];
    }

    //
    // Decrypt 
    //
    hr = _RSAProcess( pbDataBuff,
                      cbModulus,
                      FALSE,
                      &pbDecryptedData,
                      &cbDecryptedData );
    CHECKHR_GOTO( hr, done );

    //
    // Perform I2OSP on decrypted data: reverse the decrypted buffer
    // to get EM
    //
    for( i = 0; i < cbModulus; i++ )
    {
        pbDataBuff[ i ] = pbDecryptedData[ cbModulus - 1 - i ];
    }

    //
    // Decode EM and hand out
    //
    hr = _OAEPDecode( pbDataBuff, 
                      &pbDecodedMsg, 
                      &cbDecodedMsg );
    CHECKHR_GOTO( hr, done );
    
    *ppbDataOut = pbDecodedMsg;
    pbDecodedMsg = NULL;
    *pcbDataOut = cbDecodedMsg;

done:

    SAFE_SECUREMEMARRAYDELETE( pbDataBuff, cbModulus );
    SAFE_SECUREMEMARRAYDELETE( pbDecryptedData, cbDecryptedData );
    SAFE_SECUREMEMARRAYDELETE( pbDecodedMsg, cbDecodedMsg );

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::RSASign(
    IN const BYTE *pbDataIn,
    IN DWORD cbDataIn,
    OUT LPBYTE *ppbDataOut,
    OUT DWORD *pcbDataOut )
{
    HRESULT hr = S_OK;
    const DWORD cbModulus = _PrvModLen();
    
    if( pbDataIn == NULL || ppbDataOut == NULL || pcbDataOut == NULL )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }
    
    //
    // Input data for signing cannot be larger than the modulus
    //
    if( cbModulus == 0 || cbDataIn > cbModulus )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    hr = _RSAProcess( pbDataIn,
                      cbDataIn,
                      FALSE,
                      ppbDataOut,
                      pcbDataOut );
    CHECKHR_GOTO( hr, done );

    *pcbDataOut = cbModulus;

done:

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::RSAPSSSign(
    IN const BYTE *pbDataIn,
    IN DWORD cbDataIn,
    OUT LPBYTE *ppbDigest,
    OUT DWORD *pcbDigest,
    OUT LPBYTE *ppbSign,
    OUT DWORD *pcbSign )
{
    HRESULT hr = S_OK;
    const DWORD cbModulus = _PrvModLen();
    BYTE *pbEncodedData = NULL;
    BYTE *pbDigest = NULL;
    DWORD cbDigest = 0;
    BYTE *pbEncryptedData = NULL;
    DWORD cbEncryptedData = 0;
    BYTE *pbDataOut = NULL;
    DWORD i = 0;
    
    if( ( pbDataIn == NULL && cbDataIn > 0 ) ||
        ppbSign == NULL || 
        pcbSign == NULL ||
        ( ppbDigest != NULL && pcbDigest == NULL ) )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }
    
    if( cbModulus == 0 )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Allocate buffer to hold EM and OS2IP(EM)
    //
    pbEncodedData = new BYTE[ 2 * cbModulus ];
    if( pbEncodedData == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    //
    // Compute EM
    //
    hr = _PSSEncode( pbDataIn, 
                     cbDataIn, 
                     (cbModulus * 8) - 1, 
                     pbEncodedData + cbModulus, 
                     cbModulus,
                     (ppbDigest != NULL)? &pbDigest: NULL,
                     (ppbDigest != NULL)? &cbDigest: NULL
                     );
    CHECKHR_GOTO( hr, done );

    //
    //  Compute OS2IP(EM): reverse EM
    //
    for( i = 0; i < cbModulus; i++ )
    {
        pbEncodedData[ i ] = pbEncodedData[ 2*cbModulus - 1 - i ];
    }

    //
    // Decrypted OS2IP(EM) using the private key.
    //
    hr = _RSAProcess( pbEncodedData,
                      cbModulus,
                      FALSE,
                      &pbEncryptedData,
                      &cbEncryptedData );
    CHECKHR_GOTO( hr, done );

    //
    // Finally, perform I2OSP on the encrypted data: reverse and hand
    // out
    //
    pbDataOut = new BYTE[ cbModulus ];
    if( pbDataOut == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    for( i = 0; i < cbModulus; i++ )
    {
        pbDataOut[ i ] = pbEncryptedData[ cbModulus - 1 - i ];
    }

    *ppbSign = pbDataOut;
    pbDataOut = NULL;
    *pcbSign = cbModulus;
    
    if( ppbDigest != NULL )
    {
        *ppbDigest = pbDigest;
        pbDigest = NULL;
        *pcbDigest = cbDigest;
    }

done:

    SAFE_ARRAYDELETE( pbDigest );
    SAFE_ARRAYDELETE( pbDataOut );
    SAFE_ARRAYDELETE( pbEncryptedData );
    SAFE_SECUREMEMARRAYDELETE( pbEncodedData, 2 * cbModulus );

    return( hr );
}


////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::RSAVerify(
    IN const BYTE *pbDataIn,
    IN DWORD cbDataIn,
    OUT LPBYTE *ppbDataOut,
    OUT DWORD *pcbDataOut )
{
    HRESULT hr = S_OK;
    const DWORD cbModulus = _PubModLen();
    
    if( pbDataIn == NULL || ppbDataOut == NULL || pcbDataOut == NULL )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }
    
    //
    // Signed data cannot be larger than the modulus
    //
    if( cbModulus == 0 || cbDataIn != cbModulus )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    hr = _RSAProcess( pbDataIn,
                      cbDataIn,
                      TRUE,
                      ppbDataOut,
                      pcbDataOut );
    CHECKHR_GOTO( hr, done );
    
    *pcbDataOut = cbModulus;

done:

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::RSAPSSVerify(
    IN const BYTE *pbDataIn,
    IN DWORD cbDataIn,
    IN const BYTE *pbDigest,
    IN const DWORD cbDigest,
    IN const BYTE *pbSignature,
    IN const DWORD cbSignature
    )
{
    HRESULT hr = S_OK;
    const DWORD cbModulus = _PubModLen();
    BYTE *pbDataBuff = NULL;
    BYTE *pbDecryptedData = NULL;
    DWORD cbDecryptedData = 0;
    DWORD i = 0;
    
    if( ( pbDataIn == NULL && cbDataIn > 0 ) ||
        pbSignature == NULL )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }
    
    //
    // Signed data must be equal in length to the modulus
    //
    if( cbModulus == 0 || cbSignature != cbModulus )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Perform OS2IP on the signature: reverse the signature
    //
    pbDataBuff = new BYTE[ cbModulus ];
    if( pbDataBuff == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }
    for( i = 0; i < cbModulus; i++ )
    {
        pbDataBuff[ i ] = pbSignature[ cbModulus - 1 - i ];
    }

    //
    // Decrypt
    //
    hr = _RSAProcess( pbDataBuff,
                      cbModulus,
                      TRUE,
                      &pbDecryptedData,
                      &cbDecryptedData );
    CHECKHR_GOTO( hr, done );

    //
    // Perform I2OSP on the decrypted data to get EM: reverse 
    // decrypted Data
    //
    for( i = 0; i < cbModulus; i++ )
    {
        pbDataBuff[ i ] = pbDecryptedData[ cbModulus - 1 - i ];
    }

    //
    // Verify EM
    //
    hr = _PSSVerify( pbDataIn, 
                     cbDataIn, 
                     pbDigest,
                     cbDigest,
                     pbDataBuff, 
                     (cbModulus * 8) - 1 );
    CHECKHR_GOTO( hr, done );

done:

    SAFE_ARRAYDELETE( pbDecryptedData );
    SAFE_ARRAYDELETE( pbDataBuff );

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
//  Generation/Format of EM for OAEP Encode/Decode:
//      Length(EM) = ModulusLen
//      Length(DB) = ModulusLen - HashLen - 1
//
//
//                                HashLen                 1 Byte    MsgLen
//                           <---------------><----------><-----><---------->
//                           +----------------------------------------------+
//                       DB= |     LHash     |     PS    | 0x01 |    Msg    |
//                           +----------------------------------------------+
//                                                  |
//                                                  |
//                 HashLen                          |
//              <------------>                      |
//              +------------+                      |
//              |    Seed    |                      |
//              +------------+                      |
//                   |                              |
//                   |                              |
//                   |                              |
//                   |                             \|/
//                   +------------> (MGF1) -----> (XOR)
//                   |                              |
//                  \|/                             |
//                 (XOR) <--------- (MGF1) <--------+
//                   |                              |
//                   |                              |
//                  \|/                            \|/
//
//       1 byte   HashLen          ModulusLen - HashLen - 1 bytes
//      <------><-----------><--------------------------------------------->
//      +------------------------------------------------------------------+
// EM = | 0x00 | maskedSeed |                   maskedDB                   |
//      +------------------------------------------------------------------+
//
////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::_OAEPEncode(
    IN const BYTE *pbMsg,
    IN const DWORD cbMsg,
    OUT LPBYTE pbEncodedMsg )
{
    HRESULT hr = S_OK;
    DWORD cbMaxMsgLen = 0;
    DWORD cbEncodedMsg = 0;
    DWORD cbDataBlock = 0;
    BYTE *pbDataBlock = NULL;
    BYTE *pbCur = NULL;
    BYTE *pbSeed = NULL;
    BYTE *pbDBMask = NULL;
    BYTE *pbSeedMask = NULL;
    SHA512_CTX ShaCtx;
    DWORD i = 0;
   
    if( _PubModLen() < ( 2*SHA512_DIGEST_LEN + 2 ) )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    cbEncodedMsg = _PubModLen( );
    cbDataBlock = cbEncodedMsg - SHA512_DIGEST_LEN - 1;
    cbMaxMsgLen = cbDataBlock - SHA512_DIGEST_LEN - 1;

    if( cbMsg > cbMaxMsgLen )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }

    *pbEncodedMsg = 0x00;                     // Set first byte of EM to 0
    pbSeed = pbEncodedMsg + 1;                // Set up pointer to the seed/maskedSeed
    pbDataBlock = pbSeed + SHA512_DIGEST_LEN; // Set up pointer to DB/maskedDB
       
    //
    // Construct DB
    //
    pbCur = pbDataBlock;

    // Compute Hash of the empty Label
    SHA512Init( &ShaCtx );
    SHA512Update( &ShaCtx, NULL, 0 );
    SHA512Final( &ShaCtx, pbCur );
    pbCur += SHA512_DIGEST_LEN;

    // Construct PS
    ZeroMemory( pbCur, cbMaxMsgLen - cbMsg );
    pbCur += cbMaxMsgLen - cbMsg;

    // Write the 0x01 value byte to mark start of original message
    *pbCur = 0x01;
    pbCur++;

    // Copy the original message
    CopyMemory( pbCur, pbMsg, cbMsg );
    pbCur += cbMsg;

    //
    // Generate the seed in DB
    //
    CHECKHR_GOTO( CCryptHlp::GenerateRandom( pbSeed, SHA512_DIGEST_LEN ), done );

    //
    // Compute the DB Mask from the seed and XOR it with DB
    //
    hr = CMGF1::GenerateMask( pbSeed, 
                              SHA512_DIGEST_LEN, 
                              cbDataBlock, 
                              &pbDBMask );
    CHECKHR_GOTO( hr, done );

    CCryptHlp::XOR( pbDataBlock, pbDBMask, cbDataBlock );

    //
    // Next, compute the seed mask and XOR it with the seed
    //
    hr = CMGF1::GenerateMask( pbDataBlock, 
                              cbDataBlock, 
                              SHA512_DIGEST_LEN, 
                              &pbSeedMask );
    CHECKHR_GOTO( hr, done );
    
    CCryptHlp::XOR( pbSeed, pbSeedMask, SHA512_DIGEST_LEN );

done:

    SAFE_SECUREMEMARRAYDELETE( pbDBMask, cbDataBlock );
    SAFE_SECUREMEMARRAYDELETE( pbSeedMask, SHA512_DIGEST_LEN );

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::_OAEPDecode(
    IN BYTE *pbEncodedMsg,
    OUT LPBYTE *ppbDecodedMsg,
    OUT DWORD *pcbDecodedMsg )
{
    HRESULT hr = S_OK;
    const DWORD cbModulus = _PrvModLen();
    DWORD cbDataBlock = 0;
    BYTE *pbDataBlock = NULL;
    BYTE *pbCur = NULL;
    BYTE *pbSeed = NULL;
    BYTE *pbDBMask = NULL;
    BYTE *pbSeedMask = NULL;
    SHA512_CTX ShaCtx;
    BYTE rgbLHash[SHA512_DIGEST_LEN];
    DWORD i = 0;
   
    if( cbModulus < ( 2*SHA512_DIGEST_LEN + 2 ) )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    pbSeed = pbEncodedMsg + 1;                       // Set up pointer to seed/maskedSeed
    pbDataBlock = pbSeed + SHA512_DIGEST_LEN;        // Set up pointer to DB/maskedDB
    cbDataBlock = cbModulus - 1 - SHA512_DIGEST_LEN;

    //
    // First byte of the EM must be 0
    //
    if( *pbEncodedMsg != 0x00 )
    {
        SETHR_GOTO( E_FAIL, done );
    }
       
    //
    // Compute the seed mask and decode the seed
    //
    hr = CMGF1::GenerateMask( pbDataBlock, 
                              cbDataBlock, 
                              SHA512_DIGEST_LEN,
                              &pbSeedMask );
    CHECKHR_GOTO( hr, done );

    CCryptHlp::XOR( pbSeed, pbSeedMask, SHA512_DIGEST_LEN );

    //
    // Compute DB Mask from the seed and decode DB
    //
    hr = CMGF1::GenerateMask( pbSeed,
                              SHA512_DIGEST_LEN,
                              cbDataBlock,
                              &pbDBMask );
    CHECKHR_GOTO( hr, done );

    CCryptHlp::XOR( pbDataBlock, pbDBMask, cbDataBlock );

    //
    // Compute LHash for empty label and verify that DB starts with LHash
    //
    SHA512Init( &ShaCtx );
    SHA512Update( &ShaCtx, NULL, 0 );
    SHA512Final( &ShaCtx, rgbLHash );

    if( memcmp( rgbLHash, pbDataBlock, sizeof( rgbLHash ) ) != 0 )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Locate a 0x01 value byte to indicate start of original message
    //
    pbCur = pbDataBlock + SHA512_DIGEST_LEN;
    while( pbCur < pbDataBlock + cbDataBlock && *pbCur == 0x00 )
    {
        pbCur++;
    }

    if( pbCur == pbDataBlock + cbDataBlock || *pbCur != 0x01 )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Finally, output the decoded message
    //
    pbCur++;
    *pcbDecodedMsg = cbModulus - ( DWORD )( pbCur - pbEncodedMsg );
    if( *pcbDecodedMsg > 0 )
    {
        *ppbDecodedMsg = new BYTE[ *pcbDecodedMsg ];
        if( *ppbDecodedMsg == NULL )
        {
            SETHR_GOTO( E_OUTOFMEMORY, done );
        }
        CopyMemory( *ppbDecodedMsg, pbCur, *pcbDecodedMsg );
    }
    else
    {
        *ppbDecodedMsg = NULL;
    }
    
done:

    SAFE_SECUREMEMARRAYDELETE( pbDBMask, cbDataBlock );
    SAFE_SECUREMEMARRAYDELETE( pbSeedMask, SHA512_DIGEST_LEN );

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
// Generation/Format of EM for PSS Signing/Verification:
//      Length(EM) = ModulusLen
//      Length(DB) = ModulusLen - HashLen - 1
//
//                                            +---------------------------+
//                                            |             Msg           |
//                                            +---------------------------+
//                                                           |
//                                                           |
//                                                          \|/
//                                                         (Hash)
//                                                           |
//                                                           |
//                                                          \|/
//                                          8 Bytes       HashLen     SaltLen
//                                     <---------------><----------><----------->                      
//                                     +----------------------------------------+
//                               M' =  |  Zero Padding |  MsgHash  |    Salt    | 
//                                     +----------------------------------------+
//                                                           |
//                                                           |
//       Length(DB)-SaltLen-1    1 byte    SaltLen           |
//      <-----------------------><-----><-------------->     |
//      +----------------------------------------------+     |
// DB = |   PS (zero padding)   | 0x01 |     Salt      |   (Hash)
//      +----------------------------------------------+     |
//                              |                            |
//                              |                            |
//                              |                            |
//                              |                            |
//                            (XOR) <------- (MGF1) <--------+
//                              |                            |
//                             \|/                          \|/
//
//              ModulusLen - HashLen - 1 bytes            HashLen    1 Byte
//      <---------------------------------------------><------------><----->
//      +------------------------------------------------------------------+
// EM = |                    maskedDB                  |      H     | 0xBC |        
//      +------------------------------------------------------------------+
//
////////////////////////////////////////////////////////////////////////////


////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::_PSSEncode(
    IN const BYTE *pbMsg,
    IN const DWORD cbMsg,
    IN DWORD cbEMBits,
    OUT LPBYTE pbEncodedMsg,
    IN const DWORD cbEncodedMsg,
    OUT LPBYTE *ppbDigest,
    OUT DWORD *pcbDigest )
{
    HRESULT hr = S_OK;
    const DWORD cbSalt = 0;
    DWORD cbEM = 0;
    BYTE *pbDB = NULL;
    DWORD cbDB = 0;
    BYTE *pbMHash = NULL;
    BYTE *pbMPrime = NULL;
    DWORD cbMPrime = 0;
    BYTE *pbSalt = NULL;
    BYTE *pbMPrimeHash = NULL;
    BYTE *pbCur = NULL;
    BYTE *pbDBMask = NULL;
    SHA512_CTX ShaCtx;
    DWORD cbZeroBits = 0;
    DWORD i = 0;

    if( ppbDigest != NULL && pcbDigest == NULL )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }
    
    cbEM = (cbEMBits % 8 == 0)? (cbEMBits / 8): (cbEMBits / 8) + 1;
    cbZeroBits = (8 * cbEM) - cbEMBits;

    if( cbEM < SHA512_DIGEST_LEN + cbSalt + 2 || 
        cbEM != cbEncodedMsg )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    pbDB = pbEncodedMsg;                  // Set pointer to DB/maskedDB in EM
    cbDB = cbEM - SHA512_DIGEST_LEN - 1;  
    pbMPrimeHash = pbDB + cbDB;           // Set pointer to Hash(M') in EM

    //
    // Generate M' and compute its hash
    //

    cbMPrime = 8 + SHA512_DIGEST_LEN + cbSalt;
    pbMPrime = new BYTE[ cbMPrime ];
    if( pbMPrime == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    pbCur = pbMPrime;
    ZeroMemory( pbCur, 8 );                // 8 bytes of zero padding
    pbCur += 8;

    SHA512Init( &ShaCtx );                 // Original message hash
    SHA512Update( &ShaCtx, (BYTE *) pbMsg, cbMsg );
    SHA512Final( &ShaCtx, pbCur );
    pbMHash = pbCur;
    pbCur += SHA512_DIGEST_LEN;

    if( cbSalt > 0 )
    {
        pbSalt = pbCur;
        CHECKHR_GOTO( CCryptHlp::GenerateRandom( pbSalt, cbSalt ), done );
    }
    
    //
    // Calculate Hash(M')
    //
    SHA512Init( &ShaCtx );
    SHA512Update( &ShaCtx, pbMPrime, cbMPrime );
    SHA512Final( &ShaCtx, pbMPrimeHash );
    
    //
    // Compute DB, DBMask and XOR them. Then set the appropriate number of 
    // initial bits of maskedDB to 0
    //

    pbCur = pbDB;
    ZeroMemory( pbCur, cbDB - cbSalt - 1 ); // Zero padding
    pbCur += cbDB - cbSalt - 1;

    *pbCur = 0x01;                          // 0x01 byte separating padding and salt
    pbCur++;

    CopyMemory( pbCur, pbSalt, cbSalt );    // Write Salt

    hr = CMGF1::GenerateMask( pbMPrimeHash, 
                              SHA512_DIGEST_LEN, 
                              cbDB, 
                              &pbDBMask );
    CHECKHR_GOTO( hr, done );
    
    CCryptHlp::XOR( pbDB, pbDBMask, cbDB );

    pbDB[ 0 ] &= ( 0xFF >> cbZeroBits );

    //
    // Complete EM by appending byte 0xbc at the end
    //
    pbEncodedMsg[ cbEM - 1 ] = 0xBC;

    //
    // Output the digest if requested
    //
    if( ppbDigest != NULL )
    {
        *ppbDigest = new BYTE[ SHA512_DIGEST_LEN ];
        if( *ppbDigest == NULL )
        {
            SETHR_GOTO( E_OUTOFMEMORY, done );
        }
        CopyMemory( *ppbDigest, pbMHash, SHA512_DIGEST_LEN );
        *pcbDigest = SHA512_DIGEST_LEN;
    }

done:

    SAFE_SECUREMEMARRAYDELETE( pbMPrime, cbMPrime );
    SAFE_SECUREMEMARRAYDELETE( pbDBMask, cbDB );

    return( hr );
}


////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CRSAHelper::_PSSVerify(
    IN const BYTE *pbMsg,
    IN const DWORD cbMsg,
    IN const BYTE *pbDigest,
    IN const DWORD cbDigest,
    IN BYTE *pbEncodedMsg,
    IN DWORD cbEMBits )
{
    HRESULT hr = S_OK;
    const DWORD cbSalt = 0;
    DWORD cbEM = 0;
    BYTE *pbDB = NULL;
    DWORD cbDB = 0;
    BYTE *pbMPrime = NULL;
    DWORD cbMPrime = 0;
    BYTE *pbSalt = NULL;
    BYTE *pbMPrimeHash = NULL;
    BYTE *pbCur = NULL;
    BYTE *pbDBMask = NULL;
    SHA512_CTX ShaCtx;
    DWORD cbZeroBits = 0;
    DWORD i = 0;
    BYTE bMask = 0;
    BYTE rgbMPrimeHash[ SHA512_DIGEST_LEN ];

    cbEM = (cbEMBits % 8 == 0)? (cbEMBits / 8): (cbEMBits / 8) + 1;
    cbZeroBits = (8 * cbEM) - cbEMBits;

    if( cbEM < SHA512_DIGEST_LEN + cbSalt + 2 )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Verify last bit of EM
    //
    if( pbEncodedMsg[ cbEM - 1 ] != 0xBC )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Generate partial M'. We don't yet have the salt so leave it empty
    //

    cbMPrime = 8 + SHA512_DIGEST_LEN + cbSalt;
    pbMPrime = new BYTE[ cbMPrime ];
    if( pbMPrime == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }

    pbCur = pbMPrime;
    ZeroMemory( pbCur, 8 );                 // 8-bytes of zero padding
    pbCur += 8;

    SHA512Init( &ShaCtx );                  // Original message hash
    SHA512Update( &ShaCtx, (BYTE *)pbMsg, cbMsg );
    SHA512Final( &ShaCtx, pbCur );
    if( pbDigest != NULL )
    {
        if( cbDigest != SHA512_DIGEST_LEN ||
            memcmp( pbDigest, pbCur, SHA512_DIGEST_LEN ) != 0 )
        {
            SETHR_GOTO( E_FAIL, done );
        }
    }
    
    pbCur += SHA512_DIGEST_LEN;

    if( cbSalt > 0 )            // Remember pointer to Salt: fill this later
    {
        pbSalt = pbCur;
    }

    //
    // Set up DB and Hash(M') in EM
    //

    cbDB = cbEM - SHA512_DIGEST_LEN - 1;
    pbDB = pbEncodedMsg;
    pbMPrimeHash = pbDB + cbDB;
    
    //
    // Check that the requisite leftmost bits of DB are 0
    //
    bMask = 0xFF << (8 - cbZeroBits);
    if( ( pbDB[ 0 ] & bMask ) != 0 )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Compute DBMask and extract DB
    //
    hr = CMGF1::GenerateMask( pbMPrimeHash, 
                              SHA512_DIGEST_LEN, 
                              cbDB, 
                              &pbDBMask );
    CHECKHR_GOTO( hr, done );

    CCryptHlp::XOR( pbDB, pbDBMask, cbDB );

    pbDB[ 0 ] &= 0xFF >> cbZeroBits;

    //
    // Verify that bytes before the salt are all zero followed by 0x01
    //
    for( i = 0; i < cbDB - cbSalt - 1; i++ )
    {
        if( pbDB[ i ] != 0 )
        {
            SETHR_GOTO( E_FAIL, done );
        }
    }

    if( pbDB[ cbDB - cbSalt - 1 ] != 0x01 )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Fill the salt in M' now, compute Hash(M') and check against the hash in EM
    //
    CopyMemory( pbSalt, pbDB + cbDB - cbSalt, cbSalt );

    SHA512Init( &ShaCtx );
    SHA512Update( &ShaCtx, pbMPrime, cbMPrime );
    SHA512Final( &ShaCtx, rgbMPrimeHash );

    if( memcmp( pbMPrimeHash, rgbMPrimeHash, SHA512_DIGEST_LEN ) != 0 )
    {
        SETHR_GOTO( E_FAIL, done );
    }

done:

    SAFE_SECUREMEMARRAYDELETE( pbMPrime, cbMPrime );
    SAFE_SECUREMEMARRAYDELETE( pbDBMask, cbDB );

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_OUTOFMEMORY
//  E_FAIL
//
HRESULT CRSAHelper::_RSAProcess(
    IN const BYTE *pbDataIn,
    IN DWORD   cbDataIn,
    IN BOOL    fPublicKey, 
    OUT LPBYTE *ppbDataOut,
    OUT DWORD  *pcbDataOut )
{
    HRESULT hr = S_OK;
    BYTE *pbDataOut = NULL;
    DWORD cbDataOut = 0;
    BYTE *pbDataInForProcess = NULL;
    BOOL fSuccess = TRUE;

    _ProtectKey( fPublicKey, FALSE );
    if( fPublicKey )
    {
        cbDataOut = ( (LPBSAFE_PUB_KEY)m_pbPubKey )->keylen;
    }
    else
    {
        cbDataOut = ( (LPBSAFE_PRV_KEY)m_pbPrvKey )->keylen;
    }
    _ProtectKey( fPublicKey, TRUE );
    
    pbDataOut = new BYTE[ cbDataOut ];
    if( pbDataOut == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }
    ZeroMemory( pbDataOut, cbDataOut );

    pbDataInForProcess = new BYTE[cbDataOut];
    if( pbDataInForProcess == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }
    ZeroMemory( pbDataInForProcess, cbDataOut );

    CopyMemory( pbDataInForProcess, pbDataIn, cbDataIn );

    _ProtectKey( fPublicKey, FALSE );

    if( fPublicKey )
    {
        fSuccess = BSafeEncPublic( (LPBSAFE_PUB_KEY)m_pbPubKey,
                                   pbDataInForProcess,
                                   pbDataOut );
    }
    else
    {
        fSuccess = BSafeDecPrivate( (LPBSAFE_PRV_KEY)m_pbPrvKey,
                                    pbDataInForProcess,
                                    pbDataOut );
    }

    _ProtectKey( fPublicKey, TRUE );

    if( !fSuccess )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    *ppbDataOut = pbDataOut;
    *pcbDataOut = cbDataOut;
    pbDataOut = NULL;

done:

    SAFE_SECUREMEMARRAYDELETE( pbDataInForProcess, cbDataOut );
 
    return( hr );
}


////////////////////////////////////////////////////////////////////////////
//
void CRSAHelper::_ResetPubKey( )
{
    SAFE_SECUREMEMARRAYDELETE( m_pbPubKey, m_cbPubKey );
    m_cbPubKey = 0;
}

////////////////////////////////////////////////////////////////////////////
//
void CRSAHelper::_ResetPrvKey( )
{
    SAFE_SECUREMEMARRAYDELETE( m_pbPrvKey, m_cbPrvKey );
    m_cbPrvKey = 0;
}

////////////////////////////////////////////////////////////////////////////
//

CAESHelper::CAESHelper( ) :
    m_pbKey( NULL ),
    m_cbKey( 0 )
{
    ZeroMemory( m_rgbCBCLastBlock, sizeof( m_rgbCBCLastBlock ) );
    ZeroMemory( m_rgbCBCInitVector, sizeof( m_rgbCBCInitVector ) );
}

////////////////////////////////////////////////////////////////////////////
//

CAESHelper::~CAESHelper( )
{
    _ResetKey();
}

////////////////////////////////////////////////////////////////////////////
//
QWORD CAESHelper::_HTONQWORD( QWORD &qwArg )
{
    return( MAKEQWORD( htonl( HIDWORD( qwArg ) ), htonl( LODWORD( qwArg ) ) ) );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  E_OUTOFMEMORY
//
HRESULT CAESHelper::GenerateKey( 
    DWORD cbKey )
{
    HRESULT hr = S_OK;
    DWORD nRounds = 0;
    DWORD cbAlloc = 0;

    //
    // Support for standard AES key sizes 
    //

    if( cbKey != AES_KEYSIZE_128 && 
        cbKey != AES_KEYSIZE_192 && 
        cbKey != AES_KEYSIZE_256 )
    {
        return( E_INVALIDARG );
    }

    _ResetKey();

    //
    // Allocate a multiple of AES_BLOCKLEN bytes to keep the key encrypted
    //    
    cbAlloc = (cbKey == AES_KEYSIZE_192)? AES_KEYSIZE_256: cbKey;
    
    m_pbKey = new BYTE[ cbAlloc ];
    if( m_pbKey == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }
    m_cbKey = cbKey;
        
    CHECKHR_GOTO( CCryptHlp::GenerateRandom( m_pbKey, m_cbKey ), done );

    _ProtectKey( TRUE );
   
done:

    if( FAILED( hr ) && m_pbKey != NULL )
    {
        _ResetKey();
    }

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_OUTOFMEMORY
//
HRESULT CAESHelper::SetKey( 
    const BYTE *pbKey,
    DWORD cbKey )
{
    HRESULT hr = S_OK;
    DWORD cbAlloc = 0;

    if( pbKey == NULL )
    {
        return( E_INVALIDARG );
    }
    
    //
    // Support for standard AES key sizes 
    //

    if( cbKey != AES_KEYSIZE_128 && 
        cbKey != AES_KEYSIZE_192 && 
        cbKey != AES_KEYSIZE_256 )
    {
        return( E_INVALIDARG );
    }

    _ResetKey();

    //
    // Allocate a multiple of AES_BLOCKLEN bytes to keep the key encrypted
    //    
    cbAlloc = (cbKey == AES_KEYSIZE_192)? AES_KEYSIZE_256: cbKey;
    
    m_pbKey = new BYTE[ cbAlloc ];
    if( m_pbKey == NULL )
    {
        SETHR_GOTO( E_OUTOFMEMORY, done );
    }
    m_cbKey = cbKey;

    CopyMemory( m_pbKey, pbKey, cbKey );

    _ProtectKey( TRUE );

done:

    if( FAILED( hr ) && m_pbKey != NULL )
    {
        _ResetKey();
    }

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  ERROR_NOT_ENOUGH_MEMORY
//
HRESULT CAESHelper::GetKey(
    LPBYTE pbKey,
    DWORD *pcbKey )
{
    HRESULT hr = S_OK;
    
    if( pcbKey == 0 )
    {
        return( E_INVALIDARG );
    }

    if( m_pbKey == NULL )
    {
        return( E_FAIL );
    }

    if( pbKey == NULL || *pcbKey < m_cbKey )
    {
        *pcbKey = m_cbKey;
        return( HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY ) );
    }

    _ProtectKey( FALSE );
    memcpy( pbKey, m_pbKey, m_cbKey );
    _ProtectKey( TRUE );

    *pcbKey = m_cbKey;

    return( hr );    
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//
HRESULT CAESHelper::ReleaseKey( )
{
    _ResetKey();    
    return( S_OK);
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//
HRESULT CAESHelper::CBCSetInitVector( 
    const BYTE rgbInitVector[AES_BLOCKLEN]
    )
{
    HRESULT hr = S_OK;

    if( rgbInitVector == NULL )
    {
        return( E_INVALIDARG );
    }

    CopyMemory( m_rgbCBCInitVector, rgbInitVector, AES_BLOCKLEN );
    CopyMemory( m_rgbCBCLastBlock, rgbInitVector, AES_BLOCKLEN );

    return( S_OK );
}

////////////////////////////////////////////////////////////////////////////
//

DWORD CAESHelper::CBCGetBufferLen( 
    DWORD cbDataIn
    )
{
    if( cbDataIn % AES_BLOCKLEN == 0 )
    {
        return( cbDataIn );
    }

    return( cbDataIn + AES_BLOCKLEN - ( cbDataIn % AES_BLOCKLEN ) );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//  ERROR_NOT_ENOUGH_MEMORY
//
HRESULT CAESHelper::CBCProcessData( 
    AESHELPER_PROCESS_OPTION Op,
    BOOL fFinal,
    BYTE *pbData,
    DWORD *pcbData,
    DWORD cbBuffer
    )
{
    HRESULT hr = S_OK;
    
    DWORD cbDataLeft = 0;
    BYTE *pbDataInCur = NULL, *pbDataOutCur = NULL;
    DWORD nBlocks = 0;
    BYTE rgbDataBlock[AES_BLOCKLEN];
    DWORD nRounds = 0;
    AESTable KeyTable;

    if( pbData == NULL || 
        pcbData == NULL || 
        *pcbData == 0 ||
        ( Op != ENCRYPT && Op != DECRYPT ) )
    {
        return( E_INVALIDARG );
    }

    if( m_pbKey == 0 )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Calculate blocks of processed data
    //

    nBlocks = ( *pcbData + AES_BLOCKLEN - 1 ) / AES_BLOCKLEN;
    if( cbBuffer < nBlocks * AES_BLOCKLEN )
    {
        SETHR_GOTO( HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY ), done );
    }

    pbDataInCur = pbData;
    pbDataOutCur = pbData;
    cbDataLeft = *pcbData;

    nRounds = (m_cbKey == AES_KEYSIZE_128)? AES_ROUNDS_128: 
              (m_cbKey == AES_KEYSIZE_192)? AES_ROUNDS_192: AES_KEYSIZE_256;

    //
    // Retrieve the key, generate the AES table and hide the key again
    //
    _ProtectKey( FALSE );
    aeskey( &KeyTable, m_pbKey, nRounds );
    _ProtectKey( TRUE );
    
    while( cbDataLeft > 0 )
    {
        DWORD cbDataToUse = 0;
        
        if( cbDataLeft >= AES_BLOCKLEN )
        {
            cbDataToUse = AES_BLOCKLEN;
        }
        else
        {
            ZeroMemory( rgbDataBlock, AES_BLOCKLEN );
            cbDataToUse = cbDataLeft;
        }

        CopyMemory( rgbDataBlock, pbDataInCur, cbDataToUse );

        if( Op == ENCRYPT )
        {
            CCryptHlp::XOR( rgbDataBlock, m_rgbCBCLastBlock );
        }

        aes( pbDataOutCur, rgbDataBlock, &KeyTable, Op );
        
        if( Op == DECRYPT )
        {
            CCryptHlp::XOR( pbDataOutCur, m_rgbCBCLastBlock );
            CopyMemory( m_rgbCBCLastBlock, rgbDataBlock, AES_BLOCKLEN );
        }
        else if( Op == ENCRYPT )
        {
            CopyMemory( m_rgbCBCLastBlock, pbDataOutCur, AES_BLOCKLEN );
        }

        pbDataInCur += cbDataToUse;
        pbDataOutCur += AES_BLOCKLEN;
        cbDataLeft -= cbDataToUse;
                
    }

    *pcbData = nBlocks * AES_BLOCKLEN;

    if( fFinal )
    {
        CopyMemory( m_rgbCBCLastBlock, m_rgbCBCInitVector, AES_BLOCKLEN );
    }
        
done:

    SecureZeroMemory( &KeyTable, sizeof( KeyTable ) );
    return( hr );
}


////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_INVALIDARG
//  E_FAIL
//
HRESULT CAESHelper::CounterProcessData( 
    BYTE *pbData,
    DWORD cbData,
    QWORD &qwDataId,
    QWORD &qwCount
    )
{
    HRESULT hr = S_OK;
    
    DWORD cbDataLeft = 0;
    BYTE *pbDataOutCur = NULL;
    BYTE rgbDataIn[AES_BLOCKLEN];
    BYTE rgbDataOut[AES_BLOCKLEN];
    QWORD qwTemp = 0;
    QWORD qwBlockNumber = qwCount;
    DWORD nRounds = 0;
    AESTable KeyTable;

    if( pbData == NULL )
    {
        return( E_INVALIDARG );
    }

    if( m_pbKey == 0 )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    pbDataOutCur = pbData;
    cbDataLeft = cbData;

    qwTemp = _HTONQWORD( qwDataId );
    CopyMemory( rgbDataIn, (BYTE *)&qwTemp, sizeof( QWORD ) );

    nRounds = (m_cbKey == AES_KEYSIZE_128)? AES_ROUNDS_128: 
              (m_cbKey == AES_KEYSIZE_192)? AES_ROUNDS_192: AES_KEYSIZE_256;

    //
    // Retrieve the key, generate the AES table and hide the key again
    //
    _ProtectKey( FALSE );
    aeskey( &KeyTable, m_pbKey, nRounds );
    _ProtectKey( TRUE );

    while( cbDataLeft > 0 )
    {
        DWORD cbDataToUse = ( cbDataLeft < AES_BLOCKLEN )? cbDataLeft: AES_BLOCKLEN;
        
        qwTemp = _HTONQWORD( qwBlockNumber );
    
        CopyMemory( rgbDataIn + sizeof( QWORD ), 
                    (BYTE *)&qwTemp, 
                    sizeof( QWORD ) );

        aes( rgbDataOut, rgbDataIn, &KeyTable, ENCRYPT );
        
        CCryptHlp::XOR( pbDataOutCur, rgbDataOut, cbDataToUse );
        
        pbDataOutCur += cbDataToUse;
        cbDataLeft -= cbDataToUse;
        qwBlockNumber++;
    }

done:

    SecureZeroMemory( &KeyTable, sizeof( KeyTable ) );
    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//   S_OK
//   E_INVALIDARG
//   E_FAIL
//
HRESULT CAESHelper::SignData( 
    BYTE *pbData,
    DWORD cbData,
    BYTE rgbTag[AES_BLOCKLEN]
    )
{
    HRESULT hr = S_OK;
    BYTE rgbLU[AES_BLOCKLEN];
    BYTE rgbLU_1[AES_BLOCKLEN];

    if( pbData == NULL || rgbTag == NULL )
    {
        return( E_INVALIDARG );
    }

    if( m_pbKey == NULL )
    {
        hr = E_FAIL;
        goto done;
    }

    if( m_cbKey != AES_KEYSIZE_128 )
    {
        SETHR_GOTO( E_FAIL, done ); // we only sign with AES128
    }

    CHECKHR_GOTO( _GenerateSignInfo( rgbLU, rgbLU_1 ), done );

    CHECKHR_GOTO( _GenerateSignTag( pbData, cbData, rgbLU, rgbLU_1, rgbTag ), done );

done:

    return( hr );
}


////////////////////////////////////////////////////////////////////////////
//
// Results:
//   S_OK
//   E_INVALIDARG
//   E_FAIL
//   ERROR_NOT_ENOUGH_MEMORY
//
HRESULT CAESHelper::ECBProcessData( 
    AESHELPER_PROCESS_OPTION Op,
    BYTE *pbData,
    DWORD cbData
    )
{
    HRESULT hr = S_OK;
    
    BYTE *pbDataInCur = NULL;
    DWORD i;
    DWORD nBlocks = 0;
    BYTE rgbDataBlock[AES_BLOCKLEN];
    DWORD nRounds = 0;
    AESTable KeyTable;

    if( ( pbData == NULL ) ||
        ( Op != ENCRYPT && Op != DECRYPT ) )
    {
        SETHR_GOTO( E_INVALIDARG, done );
    }

    if( NULL == m_pbKey )
    {
        SETHR_GOTO( E_FAIL, done );
    }

    //
    // Calculate blocks of processed data
    //
    nBlocks = ( cbData + AES_BLOCKLEN - 1 ) / AES_BLOCKLEN;

    if( ( 0 == nBlocks ) || ( cbData != nBlocks * AES_BLOCKLEN ) )
    {
        SETHR_GOTO( HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY ), done );
    }

    nRounds = (m_cbKey == AES_KEYSIZE_128)? AES_ROUNDS_128: 
              (m_cbKey == AES_KEYSIZE_192)? AES_ROUNDS_192: AES_KEYSIZE_256;

    //
    // Retrieve the key, generate the AES table and hide the key again
    //
    _ProtectKey( FALSE );

    aeskey( &KeyTable, m_pbKey, nRounds );
   
    _ProtectKey( TRUE );

    //
    // Process all of the data
    //
    pbDataInCur = pbData;

    for( i = 0; i < nBlocks; i++ )
    {
        CopyMemory( rgbDataBlock, pbDataInCur, AES_BLOCKLEN );
        aes( pbDataInCur, rgbDataBlock, &KeyTable, Op );

        pbDataInCur += AES_BLOCKLEN;
    }
        
done:

    SecureZeroMemory( &KeyTable, sizeof( KeyTable ) );
    return( hr );
}


////////////////////////////////////////////////////////////////////////////
//

void CAESHelper::_ResetKey( )
{
    SAFE_SECUREMEMARRAYDELETE( m_pbKey, m_cbKey );
    m_cbKey = 0;

    return;
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//
HRESULT CAESHelper::_GenerateSignTag( 
    BYTE *pbData,
    DWORD cbData,
    BYTE rgbLU[AES_BLOCKLEN],
    BYTE rgbLU_1[AES_BLOCKLEN],
    BYTE rgbTag[AES_BLOCKLEN]
    )
{
    HRESULT hr = S_OK;
    
    BYTE *pbDataInCur = pbData;

    BYTE rgbDataBlock[AES_BLOCKLEN];
    BYTE rgbLastDataBlock[AES_BLOCKLEN];
    DWORD nRounds = 0;
    AESTable KeyTable;

    ZeroMemory( rgbLastDataBlock, sizeof( rgbLastDataBlock ) );

    nRounds = (m_cbKey == AES_KEYSIZE_128)? AES_ROUNDS_128: 
              (m_cbKey == AES_KEYSIZE_192)? AES_ROUNDS_192: AES_KEYSIZE_256;

    //
    // Retrieve the key, generate the AES table and hide the key again
    //
    _ProtectKey( FALSE );
    aeskey( &KeyTable, m_pbKey, nRounds );
    _ProtectKey( TRUE );

    do
    {
        if( cbData > AES_BLOCKLEN ) 
        {
            CopyMemory( rgbDataBlock, pbDataInCur, AES_BLOCKLEN );
            CCryptHlp::XOR( rgbDataBlock, rgbLastDataBlock );

            aes( rgbLastDataBlock, rgbDataBlock, &KeyTable, ENCRYPT );

            pbDataInCur += AES_BLOCKLEN;
            cbData -= AES_BLOCKLEN;
        }
        else 
        {   
            if( cbData == AES_BLOCKLEN )
            {
                CopyMemory( rgbDataBlock, pbDataInCur, AES_BLOCKLEN );
                CCryptHlp::XOR( rgbDataBlock, rgbLastDataBlock );
                CCryptHlp::XOR( rgbDataBlock, rgbLU );
            }
            else 
            {
                ZeroMemory( rgbDataBlock, AES_BLOCKLEN );
                CopyMemory( rgbDataBlock, pbDataInCur, cbData );
                rgbDataBlock[ cbData ] = 0x80;

                CCryptHlp::XOR( rgbDataBlock, rgbLastDataBlock );
                CCryptHlp::XOR( rgbDataBlock, rgbLU_1 );
            }

            aes( rgbTag, rgbDataBlock, &KeyTable, ENCRYPT );

            cbData = 0;
            
        }
            
    }while( cbData > 0 );

    SecureZeroMemory( &KeyTable, sizeof( KeyTable ) );
    return( hr );
}

////////////////////////////////////////////////////////////////////////////
//
// Results:
//  S_OK
//  E_FAIL
//
HRESULT CAESHelper::_GenerateSignInfo( 
    BYTE rgbLU[AES_BLOCKLEN],
    BYTE rgbLU_1[AES_BLOCKLEN]
    )
{
    HRESULT hr = S_OK;

    BYTE rBuffer[AES_BLOCKLEN];
    DWORD cbBuffer = sizeof( rBuffer );
    
    LPBYTE pbL = NULL;
    DWORD cbL = 0;

    BYTE bLU_ComputationConstant = 0x87;

    BYTE bLU_1_ComputationConstant1 = 0x80;
    BYTE bLU_1_ComputationConstant2 = 0x43;

    ZeroMemory( rBuffer, sizeof( rBuffer ) );

    
    CHECKHR_GOTO( CBCProcessData( ENCRYPT, 
                                  TRUE, 
                                  rBuffer, 
                                  &cbBuffer, 
                                  sizeof( rBuffer ) ), done );
    pbL = rBuffer;
    cbL = cbBuffer;

    //
    // Compute L.u from the OMAC algorithm
    //

    CCryptHlp::LShift( pbL, rgbLU );

    if( pbL[0] & 0x80 )
    {
        rgbLU[AES_BLOCKLEN - 1] ^= bLU_ComputationConstant;
    }

#if 0

    //
    // Compute L.u-1 from the OMAC algorithm (OMAC2 veriant)
    //

    CCryptHlp::RShift( pbL, rgbLU_1 );

    if( pbL[AES_BLOCKLEN - 1] & 0x01 )
    {
        rgbLU_1[0] ^= bLU_1_ComputationConstant1;
        rgbLU_1[AES_BLOCKLEN - 1] ^= bLU_1_ComputationConstant2;
    }

#endif

    //
    // Compute L.u2 from the OMAC algorithm (OMAC1 veriant)
    //

    CCryptHlp::LShift( rgbLU, rgbLU_1 );

    if( rgbLU[0] & 0x80 )
    {
        rgbLU_1[AES_BLOCKLEN - 1] ^= bLU_ComputationConstant;
    }

done:

    return( hr );
}

////////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////

#define PROTECTION_KEY_LENGTH   16


void GetProtectionKey ( BYTE rgbKey[ PROTECTION_KEY_LENGTH ] )
{
    BYTE        rgbProtectionKey[] = { 0xc3, 0x7b, 0x4e, 0x7c, 
                                       0x9b, 0xc9, 0x4b, 0x58, 
                                       0xa8, 0xc7, 0x52, 0x35, 
                                       0xf2, 0xd6, 0x6f, 0x91 };
    CopyMemory( rgbKey, rgbProtectionKey, PROTECTION_KEY_LENGTH );
    return;
}

extern
HRESULT ProtectBlob( BYTE *pbBlob, DWORD cbBlob, BOOL fEncrypt )
{
    HRESULT hr = S_OK;
    AESTable KeyTable;
    BYTE rgbPKey[ PROTECTION_KEY_LENGTH ];
    
    if( ( cbBlob % AES_BLOCKLEN ) != 0 )
    {
        hr = HRESULT_FROM_WIN32( ERROR_NOT_ENOUGH_MEMORY );
        goto done;
    }

    GetProtectionKey( rgbPKey );
    
    aeskey( &KeyTable, rgbPKey, AES_ROUNDS_128 );

    while( cbBlob > 0 )
    {
        aes128( pbBlob, pbBlob, &KeyTable, (fEncrypt? 1: 0 ) );
        pbBlob += AES_BLOCKLEN;
        cbBlob -= AES_BLOCKLEN;
    }

    SecureZeroMemory( rgbPKey, sizeof( rgbPKey ) );
    SecureZeroMemory( &KeyTable, sizeof( KeyTable ) );
    
done:

    return( hr );
}




};
