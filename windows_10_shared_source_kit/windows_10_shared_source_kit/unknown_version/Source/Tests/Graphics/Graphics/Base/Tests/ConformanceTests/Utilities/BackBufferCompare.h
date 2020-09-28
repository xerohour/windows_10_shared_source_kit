#ifndef __BACKBUFFERCOMPARE_H
#define __BACKBUFFERCOMPARE_H

/*
 Compares back buffers.
*/
class CBackBufferCompare 
{

    public:
    /*
      In the constructor, we describe this component, and each variable it contains
    */
    CBackBufferCompare(CLogWriter*);

    ~CBackBufferCompare();

    /*
      Set a reference back buffer against which other back buffers can be compared.
    */
    void SetReferenceBuffer(CI3DSurface2Ptr pRefenceBackBuffer);

    /*
      Compare a back buffer to the reference back buffer.
    */
    UINT CompareBufferToReference( CI3DSurface2Ptr pBackBuffer);

private:

    /*
        The width of the reference buffer.
    */
    UINT m_nReferenceBufferWidth;

    /*
        The height of the reference buffer.
    */
    UINT m_nReferenceBufferHeight;

    /*
        The stride of the reference buffer.
    */
    UINT m_nReferenceBufferStride;

    /*
        The contents of the reference buffer.
    */
    void* m_pReferenceBufferLockedRect;

    /*
        Bytes per pixel of the reference buffer.
    */
    int m_nReferenceBytesPerPixel;

    /*
       Used for logging.
    */
    CLogWriter* m_pLog;
};


#endif