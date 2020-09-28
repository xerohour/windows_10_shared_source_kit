#include "BaseVid.h"
#include "Reference.h"
#include "Image.h"
#include "BackBufferCompare.h"


using namespace std;

/*
  This is where we define the component
*/
CReference::CReference()
{
    /*
      First, we describe the purpose of this component for the spec
    */
    SetDescription(L"This component creates the reference image.")  ;

    m_pBackBufferCompare = NULL;
}

/*
  This method is called each time we are to run a test case
*/
void CReference::RunTest()
{   
 
    /*
        Here we create a triangle and apply a texture to it.
    */
    CImage image(4,m_pLog);
    CI3DSurface2Ptr  pBackBuffer;
    HRESULT hr = S_OK;

    hr = image.CreateImage(m_pDevice);

    if(S_OK == hr)
    {
        pBackBuffer = image.RenderImage(m_pDevice);

        m_pBackBufferCompare = new CBackBufferCompare(m_pLog);

        if(NULL == m_pBackBufferCompare)
            m_pLog->Fail(L"Could not create m_pBackBufferCompare.. no memory");

        /*
            Set this back buffer as refernce for comparison
        */
        m_pBackBufferCompare->SetReferenceBuffer(pBackBuffer);

        H_CHECK( m_pDevice->Present());

        /*
            This will be later used by TestReference to compare the back buffers.
        */
        AddSemantic<CBackBufferCompare*>(L"BackBuffer", m_pBackBufferCompare);  
    }
    else if(E_OUTOFMEMORY == hr)
    {
        m_pLog->Fail(L"Could not create an image with the required number of textures");
    }
    else
    {
        m_pLog->Skip(L"Device may not support pixel shader.. Test has not been executed");
    }
}

void CReference::EndTest()
{
    delete m_pBackBufferCompare;
}

