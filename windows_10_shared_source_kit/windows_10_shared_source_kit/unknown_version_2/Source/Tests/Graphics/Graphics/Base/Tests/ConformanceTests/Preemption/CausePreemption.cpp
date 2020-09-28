#include "BaseVid.h"
#include "CausePreemption.h"
#include "image.h"

using namespace std;

/*
  This is where we define the component
*/
CCausePreemption::CCausePreemption()
{
    /*
      First, we describe the purpose of this component for the spec
    */
    SetDescription(L"This component tests Pre-emption.")  ;
}


/*
  This method is called each time we are to run a test case
*/
void CCausePreemption::RunTest()
{   
    /*
        Here we create a triangle and apply a texture to it.
    */
    CImage image(4,m_pLog);
    HRESULT hr = S_OK;

    hr = image.CreateImage(m_pDevice);

    if(S_OK == hr)
    {
        CI3DSurface2Ptr  pBackBuffer = image.RenderImage(m_pDevice);
    }
    else if(E_OUTOFMEMORY == hr)
    {
        m_pLog->Skip(L"Could not create an image with the required number of textures");
    }
    else
    {
        m_pLog->Skip(L"Device does not support pixel shader.. Test has not been executed");
    }

}

void CCausePreemption::EndTest()
{
}


