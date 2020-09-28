#include "BaseVid.h"
#include "eviction.h"
#include "pixel.h"
#include "BackBufferCompare.h"
#include "Image.h"
#include <Winbase.h>

/*
  This is where we define the component
*/
CTestEviction::CTestEviction()
{
    /*
      First, we describe the purpose of this component for the spec
    */
    SetDescription(L"This component tests eviction.")  ;
}

/*
  This method is called each time we are to run a test case
*/
void CTestEviction::RunTest()
{   
    CBackBufferCompare backBufferCompare(m_pLog);
    CI3DSurface2Ptr pBackBuffer;
    CI3DSurface2Ptr pRefBackBuffer;
    bool bSimilar=true;
    HRESULT hr = S_OK;

    /*
        Here we create a triangle and apply a texture to it.
    */
    CImage image(1, m_pLog);			// use this image to render before and after eviction
    CImage EvictionImage(m_pLog);       // use this image to generate enough textures to cause eviction of image's textures

    hr = image.CreateImage(m_pDevice);

    if(S_OK == hr)
    {
        pRefBackBuffer = image.RenderImage(m_pDevice);

        /*
            Set this back buffer as reference for comparison
        */
        backBufferCompare.SetReferenceBuffer(pRefBackBuffer);

        H_CHECK( m_pDevice->Present());
    }
    else if(E_OUTOFMEMORY == hr)
    {
        m_pLog->Fail(L"Could not create an image with the required number of textures");

        return;
    }
    else
    {
        m_pLog->Skip(L"Device does not support pixel shader.. Test has not been executed");

        return;
    }


    if(SUCCEEDED(hr))
    {
        //
        //	Generate new textures to cause evictions
        // 
        hr = EvictionImage.CreateImage(m_pDevice);

        if(hr == S_OK || hr == E_OUTOFMEMORY)
        {
            EvictionImage.RenderImage(m_pDevice, true);
            H_CHECK( m_pDevice->Present());
            hr = S_OK;
        }
        else
        {
            m_pLog->Fail(L"Failed to create EvictionImage");
        }
    }
    

    if(SUCCEEDED(hr))
    {
        /*
            Render an image again, this will bring evicted textures back into memory
        */
        pBackBuffer = image.RenderImage(m_pDevice);

        /*
            Compare the two images.
        */
        if(backBufferCompare.CompareBufferToReference(pBackBuffer) != 0)
            bSimilar = false;
        
        H_CHECK(m_pDevice->Present());

        if(bSimilar == false)
            m_pLog->Fail(L"The eviction is not correct");
    }
}


void CTestEviction::EndTest()
{
}

