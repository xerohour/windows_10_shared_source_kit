#include "BaseVid.h"
#include "pixel.h"
#include "Image.h"
#include "BackBufferCompare.h"
#include "TestPreemption.h"
#include <Winbase.h>
#include <stdlib.h>



using namespace std;
/*
  This is where we define the component
*/
CTestPreemption::CTestPreemption()
{
    /*
      First, we describe the purpose of this component for the spec
    */
    SetDescription(L"This component tests Preemption.")  ;
}

/*
  This method is called each time we are to run a test case
*/
void CTestPreemption::RunTest()
{   
    CBackBufferCompare* pBackBufferCompare;
    CI3DSurface2Ptr pBackBuffer;
    HRESULT hr = S_OK;

    /*
        Here we create a triangle and apply a texture to it.
    */
    CImage image(4,m_pLog);

    hr = image.CreateImage(m_pDevice);

    if(S_OK == hr)
    {
        pBackBuffer = image.RenderImage(m_pDevice);

        /*
            This has the reference image.
        */
        pBackBufferCompare = GetSemantic<CBackBufferCompare*>(L"BackBuffer");

        if(pBackBufferCompare->CompareBufferToReference(pBackBuffer) != 0)
            m_pLog->Fail(L"The preemption is not correct");

        m_pDevice->Present();
    }
    else if(E_OUTOFMEMORY == hr)
    {
        m_pLog->Fail(L"Could not create an image with the required number of textures");
    }
    else
    {
        m_pLog->Skip(L"Device does not support pixel shader.. Test has not been executed");
    }

}

void CTestPreemption::EndTest()
{
    
}


