#include "BaseVid.h"
#include "pixel.h"
#include "BackBufferCompare.h"

CBackBufferCompare::CBackBufferCompare(CLogWriter* pLog)
{
    CLogWriter* m_pLog = pLog;

    m_pReferenceBufferLockedRect = NULL;
}

CBackBufferCompare::~CBackBufferCompare()
{
    free(m_pReferenceBufferLockedRect);
}

/*
    Set a reference back buffer against which other back buffers can be compared.
*/
void CBackBufferCompare::SetReferenceBuffer(CI3DSurface2Ptr pRefenceBackBuffer)
{
    D3DLOCKED_RECT lockedRect;
    UINT nSize;
    
    /*
        Get the back buffers width and height
    */
    H_CHECK(pRefenceBackBuffer->GetSize(&m_nReferenceBufferWidth, &m_nReferenceBufferHeight));

    /*
        Lock it so that the CPU can access it.
    */
    H_CHECK(pRefenceBackBuffer->Lock(I3DLOCK_READONLY,NULL,&lockedRect));

    /*
        Get the stride of the back buffer.
    */
    m_nReferenceBufferStride = lockedRect.Pitch;

    /*
        Allocate enough memory and copy the contents of the back buffer into it.
    */

    m_nReferenceBytesPerPixel = CPixel::BytesPerPixel(pRefenceBackBuffer->GetFormat());

    nSize = m_nReferenceBufferHeight*m_nReferenceBufferStride;

    m_pReferenceBufferLockedRect = malloc(nSize);

    if( NULL == m_pReferenceBufferLockedRect)
        m_pLog->Fail(L"CBackBufferCompare::SetReferenceBuffer malloc failed");

    memcpy(m_pReferenceBufferLockedRect,lockedRect.pBits,nSize);

    /*
        Unlock the back buffer.
    */
    H_CHECK(pRefenceBackBuffer->Unlock());

    return;
}

/*
    Compare a back buffer to the reference back buffer.
*/
UINT CBackBufferCompare::CompareBufferToReference( CI3DSurface2Ptr pBackBuffer)
{
    
    UINT nWidth, nHeight,nSimilar,nStride,nBytesPerPixel;
    D3DLOCKED_RECT lockedRect;

    /*
        Get the back buffers width and height
    */
    H_CHECK(pBackBuffer->GetSize(&nWidth, &nHeight));

    /*
        Lock it so that the CPU can access it.
    */
    H_CHECK(pBackBuffer->Lock(I3DLOCK_READONLY,NULL,&lockedRect));
        
    /*
        Get the stride of the back buffer.
    */
    nStride = lockedRect.Pitch;

    nBytesPerPixel = CPixel::BytesPerPixel(pBackBuffer->GetFormat());

    /*
       Compare the sizes of the surfaces
    */
    if(nWidth==m_nReferenceBufferWidth && nHeight==m_nReferenceBufferHeight && nStride==m_nReferenceBufferStride && nBytesPerPixel==m_nReferenceBytesPerPixel)
    {
        /*
            Compare the bytes in all the raster lines specified by nHeight, upto nWidth bytes.
         */
        for(UINT j=0; j<nHeight; j++)
            if((nSimilar = memcmp((char*)m_pReferenceBufferLockedRect+j*m_nReferenceBufferStride,(char*)lockedRect.pBits+j*lockedRect.Pitch,nWidth*nBytesPerPixel))!=0)
                break;
    }
    else
    {
        m_pLog->Fail(L"CBackBufferCompare::CompareBufferToReference - Not identical back buffers");
        nSimilar=-1;  
    }

    /*
        Unlock the back buffer.
    */
    H_CHECK(pBackBuffer->Unlock());

    return nSimilar;
}
