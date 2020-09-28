#include "BaseVid.h"
#include "DMAsplit.h"
#include "pixel.h"
#include "BackBufferCompare.h"
#include "Image.h"
#include <Winbase.h>
#include <stdlib.h>

/*
  This is where we define the component
*/
CTestDMASplit::CTestDMASplit()
{
    /*
      First, we describe the purpose of this component for the spec
    */
    SetDescription(L"This component tests DMA buffer splitting.")  ;
}

/*
  This method is called each time we are to run a test case
*/
void CTestDMASplit::RunTest()
{   
	/*
		Check WDDM version
	*/

	D3DKMT_DRIVERVERSION DriverVersion = CTestDMASplit::GetWDDMVersion();
	if (DriverVersion >= KMT_DRIVERVERSION_WDDM_2_0)
	{	
		/*
			Test to be performed only below WDDM 2.0 
		*/
		m_pLog->Skip(L"DMA Split Test is skipped on WDDM2.0 and above");
		return ;
	}

    CBackBufferCompare backBufferCompare(m_pLog);
    CI3DSurface2Ptr pBackBuffer;
    CI3DSurface2Ptr pRefBackBuffer;
    HRESULT hr = S_OK;

    /*
        Here we create a triangle and apply a texture to it.
    */
    CImage image(m_pLog);
    
    //
    //	We are creating enough textures to fill all of the video memory, so we might get E_OUTOFMEMORY.
    //	It is fine in this case.
    //
    hr = image.CreateImage(m_pDevice,true);
    
    if(S_OK == hr || E_OUTOFMEMORY == hr)
    {

        /*
           true is for flush to be executed so that no DMA buffer splitting takes place.
        */
        pRefBackBuffer = image.RenderImage(m_pDevice,true);

        /*
            Set this back buffer as refernce for comparison
        */
        backBufferCompare.SetReferenceBuffer(pRefBackBuffer);

        H_CHECK( m_pDevice->Present());

        /*
            Render an image again, with DMA buffer splitting i.e. without flushing.
        */
        pBackBuffer = image.RenderImage(m_pDevice);

        /*
            Compare the two images.
        */
        if(backBufferCompare.CompareBufferToReference(pBackBuffer) != 0)
            m_pLog->Fail(L"The DMA buffer splitting is not correct");

        m_pDevice->Present();

    }
    else
    {
        m_pLog->Skip(L"Device does not support pixel shader");
    }

}

void CTestDMASplit::EndTest()
{
}
