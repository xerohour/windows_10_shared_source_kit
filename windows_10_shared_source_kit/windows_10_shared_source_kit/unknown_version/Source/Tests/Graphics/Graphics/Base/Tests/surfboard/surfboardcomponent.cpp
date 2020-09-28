#include <etwConsumersRequire.h>

#include <lddmcoreeventdefs.h>
#include <d3dkmthk.h>
#include <d3dkmthk_p.h>
#include <d3dukmdt.h>
#include <dmmdiag.h>
#include <dxetwevt.hxx>

#include <DxgKernelConsumer.h>
#include <DxgKernelLegConsumer.h>
#include <Controller.h>
#include <etwStatistics.h>
#include <DXGKernelStatistics.h>

#include <etwLibEvents.h>
#include <Provider.h>
#include "basevid.h"
#include <PreemptionCaps.h>

#include "surfboardcomponent.h"

#define fBOUNDARY_SLACK 0.80
#define fBOUNDARY_THRESHOLD 64.0 
#define FIRST_FRAME_MS 12 // 64-12=52/64=> 81% Thus, within our slack.

extern CController etwController;
extern CDMAStandardTime etwDMAStandardTimeConsumer;
extern CProvider etwProvider;
//------------------------------------------------------------------------------

// Only support EquilateralTriangle for proof of concept
// _PixelsPerTriangle = (1/2)b*b
CTriangle::CTriangle(TriangleType _Type,
                     UINT _iPixelsPerTriangle):
    m_Base(0.0),
    m_iBase(0),
    m_iHeight(0)
{
    int iPixels=0;
    m_Type = _Type;
    switch(m_Type)
    {
    case EquilateralTriangle:
        {
            // Determine the height and base
            m_Base=sqrt((double)(2*_iPixelsPerTriangle));
            m_iBase=(int)m_Base;
            m_iHeight=(int)m_Base;

            iPixels = (int)((m_iBase*m_iHeight)/2);
        }
        break;
    case RightTriangle:
    case IsoscelesTriangle:
    case ScaleneTriangle:
    case ObtuseTriangle:
    default:
        {
            return;
        }
        break;
    }

#ifdef DBG
    WCHAR wszBuf[128];
    StringCchPrintf(wszBuf,128,L"m_Base=%.2f, m_iBase=%d, m_iHeight=%d, effective pixels=%d\n\r",
                    m_Base,m_iBase,m_iHeight,iPixels);
    OutputDebugString(wszBuf);
#endif
}

CTriangle::~CTriangle()
{
}

int 
CTriangle::Base()
{
    return m_iBase;
}

int 
CTriangle::Height()
{
    return m_iHeight;
}

int
CTriangle::Pixels()
{
    return (int)((m_iBase*m_iHeight)/2);
}

//------------------------------------------------------------------------------

CSurfboardManager::CSurfboardManager()
{
    /*
      First, we describe the purpose of this component
      for the spec
    */
    SetDescription(L"This is our surfboard manager component. It does the book keeping of textures and VBs");
}

void CSurfboardManager::RunTest()
{
    CreateTheVB();

    AddSemantic<CI3DTexturePtr*>(L"TexturePtrs", m_pTexture);
    AddSemantic<CI3DVertexBuffer*>(L"VBPtr", m_pVertexBuffer.p);
}


void CSurfboardManager::EndTest()
{
    FreeResources();
}

void CSurfboardManager::FreeResources()
{
    for (int i=0; i<MAX_TEXTURE_COUNT; i++)
    {
        m_pTexture[i].Release();
    }
    m_pVertexBuffer.Release();
}

void CSurfboardManager::CreateTheVB()
{
    CI3DSurface2Ptr pBackBuffer;
    UINT nWidth, nHeight;
    Vertex* pLockedVertices = NULL;

    H_CHECK( m_pDevice->GetRenderTarget(0, &pBackBuffer.p) );

    H_CHECK( pBackBuffer->GetSize(&nWidth, &nHeight) );

    H_CHECK( m_pDevice->CreateVertexBuffer(&m_pVertexBuffer.p,
                                           sizeof(Vertex) * 4,
                                           0,
                                           FVF,
                                           D3DDDIPOOL_SYSTEMMEM,
                                           0) );

    H_CHECK( m_pVertexBuffer->Lock(0, 0, (void**)&pLockedVertices, 0) );

    pLockedVertices[0].Pos[0] = 0.0f+10;//x
    pLockedVertices[0].Pos[1] = 0.0f+10;//y
    pLockedVertices[0].Pos[2] = 0.5f;//z
    pLockedVertices[0].Pos[3] = 1.0f;//w
    pLockedVertices[0].Tex[0] = 0.0f;
    pLockedVertices[0].Tex[1] = 0.0f;

    pLockedVertices[1].Pos[0] = (float)nWidth-10;//x
    pLockedVertices[1].Pos[1] = 0.0f+10;//y
    pLockedVertices[1].Pos[2] = 0.5f;//z
    pLockedVertices[1].Pos[3] = 1.0f;//w
    pLockedVertices[1].Tex[0] = 1.0f;
    pLockedVertices[1].Tex[1] = 0.0f;

    pLockedVertices[2].Pos[0] = (float)nWidth-10;//x
    pLockedVertices[2].Pos[1] = (float)nHeight-10;//y
    pLockedVertices[2].Pos[2] = 0.5f;//z
    pLockedVertices[2].Pos[3] = 1.0f;//w
    pLockedVertices[2].Tex[0] = 1.0f;
    pLockedVertices[2].Tex[1] = 1.0f;

    pLockedVertices[3].Pos[0] = 0.0f+10;//x
    pLockedVertices[3].Pos[1] = (float)nHeight-10;//y
    pLockedVertices[3].Pos[2] = 0.5f;//z
    pLockedVertices[3].Pos[3] = 1.0f;//w
    pLockedVertices[3].Tex[0] = 0.0f;
    pLockedVertices[3].Tex[1] = 1.0f;

    H_CHECK( m_pVertexBuffer->Unlock() );
}

CSurfboardComponent::CSurfboardComponent()
{
    /*
      First, we describe the purpose of this component
      for the spec
    */
    SetDescription(L"This is our surfboard test component.  It uses the I3D device created by the device component to render");
    
    /*
      Next, we add a description of all of our parameters
    */
    AddParam<UINT>(L"OpCode", L"The special operation code we pass in");
    AddParam<UINT>(L"TextureCount", L"The number of textures per pass");
}

/*
  This method is called each time we are to run a test case
*/
void CSurfboardComponent::RunTest()
{   
    UINT Op = GetParam<UINT>(L"OpCode");
    UINT iTextureCount = GetParam<UINT>(L"TextureCount");

    for (int i=0; i<50; i++)
    {
        Op = Op + 1;
        UINT nCase = (Op/iTextureCount) % 8;
        UINT nFrame = Op%iTextureCount; 
    
        switch (nCase)
        {
            case 0:
                Render(nFrame, 2);
                break;
            case 1:
                Render(nFrame, 4);
                break;
            case 2:
                Render(nFrame, 32);
                break;
            case 3:
                Render(nFrame, 128);
                break;
            case 4:
                Render(nFrame, 256);
                break;
            case 5:
                Render(nFrame, 512);
                break;
            case 6:
                if (nFrame < iTextureCount/6)
                {
                    Render(nFrame, 2);
                }
                else if (nFrame < iTextureCount/3)
                {
                    Render(nFrame, 4);
                }
                else if (nFrame < iTextureCount/2)
                {
                    Render(nFrame, 16);
                }
                else if (nFrame < 2*iTextureCount/3)
                {
                    Render(nFrame, 128);
                }
                else if (nFrame < 5*iTextureCount/6)
                {
                    Render(nFrame, 256);
                }
                else
                {
                    Render(nFrame, 512);
                }
                break;
            case 7:
                if (nFrame%6 == 0)
                {
                    Render(nFrame, 2);
                }
                if (nFrame%6 == 1)
                {
                    Render(nFrame, 4);
                }
                if (nFrame%6 == 2)
                {
                    Render(nFrame, 16);
                }
                if (nFrame%6 == 3)
                {
                    Render(nFrame, 128);
                }
                if (nFrame%6 == 4)
                {
                    Render(nFrame, 256);
                }
                if (nFrame%6 == 5)
                {
                    Render(nFrame, 512);
                }
                break;
        default:
            H_CHECK(E_FAIL);
        }
    }
}



void CSurfboardComponent::EndTest()
{
}

void CSurfboardComponent::CreateTheTexture(CI3DTexturePtr &pTexture, UINT uSize)
{
    //Free the old one if necessary
    pTexture.Release();

    H_CHECK( I3DXCreateValidTexture2(m_pDevice,
                                     0,//full mip chain
                                     I3DFORMAT_ARGB,//the basic format type we want
                                     0,//no useage flags
                                     D3DDDIPOOL_VIDEOMEMORY,
                                     false,//not a vertex texture
                                     uSize,
                                     uSize,
                                     &pTexture.p) );

    SI3DXGradientInfo Info;
    static bool bToggle = false;
    if (bToggle)
    {
        Info.cUpperLeft = SColor(1.0f, 1.0f, 0.0f, 0.0f);//red
        Info.cUpperRight = SColor(1.0f, 0.0f, 1.0f, 0.0f);//green
        Info.cLowerLeft = SColor(1.0f, 0.0f, 0.0f, 1.0f);//blue
        Info.cLowerRight = SColor(1.0f, 1.0f, 1.0f, 0.0f);//yellow
        bToggle = false;
    }
    else
    {
        Info.cUpperLeft = SColor(1.0f, 0.0f, 1.0f, 0.0f);//green
        Info.cUpperRight = SColor(1.0f, 0.0f, 0.0f, 1.0f);//blue
        Info.cLowerLeft = SColor(1.0f, 1.0f, 1.0f, 0.0f);//yellow
        Info.cLowerRight = SColor(1.0f, 1.0f, 0.0f, 0.0f);//red
        bToggle = true;
    }

    H_CHECK( I3DXFillTexture(m_pDevice,
                             pTexture,
                             I3DXFillGradient,
                             &Info) );
}


void CSurfboardComponent::Render(UINT nFrame, UINT uSize)
{
    CI3DVertexBuffer* pVertexBuffer = GetSemantic<CI3DVertexBuffer*>(L"VBPtr");
    CI3DTexturePtr *pTexture = GetSemantic<CI3DTexturePtr*>(L"TexturePtrs");

    CreateTheTexture(pTexture[nFrame], uSize);

    H_CHECK( m_pDevice->Clear(D3DCOLOR_ARGB(255, 0, 0, 128),//color
                              I3DXGetClearFlags(m_pDevice) | D3DCLEAR_COMPUTERECTS,//have the driver clear the entire screen
                              1.0f,//z
                              0,//stencil
                              0,//rect count
                              NULL/*rects*/) );

    H_CHECK( m_pDevice->SetStreamSource(0, sizeof(Vertex), pVertexBuffer) );

    //blakep, setup vertex pipeline
    H_CHECK( I3DXSetDefaultVertexPipe(m_pDevice, FVF) );

    //blakep, setup pixel pipeline
    H_CHECK( I3DXSet2DTexModDiffusePixelShader(m_pDevice) );

    H_CHECK( m_pDevice->SetTexture(0, pTexture[nFrame]) );

    H_CHECK( m_pDevice->BeginScene() );

    H_CHECK( m_pDevice->DrawPrimitive(D3DPT_TRIANGLEFAN,
                                      0,
                                      2) );

    H_CHECK( m_pDevice->EndScene() );

    H_CHECK( m_pDevice->Present() );
}


#define LOGFUNCTION(_Enter_) { WCHAR wszBuf[MAX_PATH]; \
    StringCchPrintf(wszBuf,MAX_PATH,L"%s %S",(_Enter_?L"Entering":L"Leaving"),__FUNCTION__); \
    m_pLog->Message(wszBuf); \
    EventWriteLogMsg(wszBuf);}

#define ENTER() LOGFUNCTION(true)

#define LEAVE() LOGFUNCTION(false)


//---------------------------- CBasePreemption ---------------------------------
//
// This class is used by CPreemptionShader and Others. It manages resources and basic
// working functionality.
//
CBasePreemption::CBasePreemption():
    m_nTotalTime(0),
    m_nTriangles(0),
    m_nppPrimitive(0),
    m_nppFrame(0),
    m_nShaderInstructions(0),
    m_pVertexBuffer(NULL),
    m_pVertices(NULL),
    m_pShader(NULL),
    m_Level(D3DKMDT_GRAPHICS_PREEMPTION_NONE),
    m_ullLongestPreemptionRequestTime(0)
{
}

void CBasePreemption::SetupCommandLine(UINT _TotalTime,
                                       UINT _nTriangles,
                                       UINT _nppPrimitive,
                                       UINT _nppFrame,
                                       UINT _nShaderInstructions)
{
    AddParam<UINT>(L"TotalTime", L"The total number of seconds to run for");
    AddParam<UINT>(L"nTriangles", L"The number of triangles");
    AddParam<UINT>(L"nppPrimitive", L"The number of pixels per primitive");
    AddParam<UINT>(L"nppFrame", L"The number of primitives per frame");
    AddParam<UINT>(L"nShaderInstructions", L"The number of instructions for shader");
    AddParam<UINT>(L"nIterations", L"The number of times to loop");
    AddParam<bool>(L"bWantCSVFile", L"Write data to a csv file");
    AddParam<bool>(L"bDWM", L"Don't perform Preeption check");
    
    // Set the default for these command line arguments. 
    SetParamTestDomainSingleValue<UINT>(L"TotalTime", _TotalTime);
    SetParamTestDomainSingleValue<UINT>(L"nTriangles", _nTriangles);
    SetParamTestDomainSingleValue<UINT>(L"nppPrimitive", _nppPrimitive);
    SetParamTestDomainSingleValue<UINT>(L"nppFrame", _nppFrame);
    SetParamTestDomainSingleValue<UINT>(L"nShaderInstructions", _nShaderInstructions);
    SetParamTestDomainSingleValue<UINT>(L"nIterations", 0);
    SetParamTestDomainSingleValue<bool>(L"bWantCSVFile", false);
    SetParamTestDomainSingleValue<bool>(L"bDWM", false);
}


void CBasePreemption::FreeResources()
{
    for (int i=0; i<THE_TEXTURE_COUNT; i++)
    {
        m_pTexture[i].Release();
    }
    
    m_pVertexBuffer.Release();
}


void CBasePreemption::CreateTheVB(CTriangle* _pTriangle,
                                    int _iTriangles)
{
    const int nVertices = _iTriangles * 3;
    const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;

    if( m_pVertexBuffer )
    {
        m_pVertexBuffer.Release();
    }

    // Create a vextex buffer that will hold the number of triangles that we want.
    H_CHECK( m_pDevice->CreateVertexBuffer(&m_pVertexBuffer.p,
                                           sizeof(Vertex) * nVertices,
                                           0,
                                           FVF,
                                           D3DDDIPOOL_SYSTEMMEM,
                                           0) );

#ifdef DBG
    WCHAR wszBuf[128];
    StringCchPrintf(wszBuf,128,L"VertexBufferSize=%d\n\r",
                    sizeof(Vertex) * nVertices);
    OutputDebugString(wszBuf);
#endif

    //fill the VB
    H_CHECK(m_pVertexBuffer->Lock(0, 0, (void**) &(m_pVertices), 0));
    
    // This loop sets up the float that represents how much of the triangle to show.
    for(int i = 0; i < _iTriangles; i++)
    {
        Vertex* pStart = &m_pVertices[i * 3];
        pStart[0].Pos[0] = 0.0f;
        pStart[0].Pos[1] = 1.0f;
        pStart[0].Pos[2] = 0.5f;
        pStart[0].Pos[3] = 1.0f;

        pStart[1].Pos[0] = 1.0f;
        pStart[1].Pos[1] = 0.0f;
        pStart[1].Pos[2] = 0.5f;
        pStart[1].Pos[3] = 1.0f;

        pStart[2].Pos[0] = 1.0f;
        pStart[2].Pos[1] = 1.0f;
        pStart[2].Pos[2] = 0.5f;
        pStart[2].Pos[3] = 1.0f;
    }

    // This loop sets the size of the triangle and the z order.
    for(int i = 0; i < nVertices; i++)
    {
        m_pVertices[i].Pos[0] *= (float)_pTriangle->Base();   //.Width;  // Use TrianglePrim.Base() 
        m_pVertices[i].Pos[1] *= (float)_pTriangle->Height(); // ;m_RTDesc.Height; // Use TrianglePrim.Height()
        m_pVertices[i].Pos[2] = 1.0f - (i / (float)_iTriangles);//back to front
    }    

    H_CHECK(m_pVertexBuffer->Unlock());
}


void CBasePreemption::CreateTheTexture(CI3DTexturePtr &pTexture, UINT uSize)
{
    //Free the old one if necessary
    pTexture.Release();

    H_CHECK( I3DXCreateValidTexture2(m_pDevice,
                                     0,//full mip chain
                                     I3DFORMAT_ARGB,//the basic format type we want
                                     0,//no useage flags
                                     D3DDDIPOOL_VIDEOMEMORY,
                                     false,//not a vertex texture
                                     uSize,
                                     uSize,
                                     &pTexture.p) );

    SI3DXGradientInfo Info;
    static bool bToggle = false;
    if (bToggle)
    {
        Info.cUpperLeft = SColor(1.0f, 1.0f, 0.0f, 0.0f);//red
        Info.cUpperRight = SColor(1.0f, 0.0f, 1.0f, 0.0f);//green
        Info.cLowerLeft = SColor(1.0f, 0.0f, 0.0f, 1.0f);//blue
        Info.cLowerRight = SColor(1.0f, 1.0f, 1.0f, 0.0f);//yellow
        bToggle = false;
    }
    else
    {
        Info.cUpperLeft = SColor(1.0f, 0.0f, 1.0f, 0.0f);//green
        Info.cUpperRight = SColor(1.0f, 0.0f, 0.0f, 1.0f);//blue
        Info.cLowerLeft = SColor(1.0f, 1.0f, 1.0f, 0.0f);//yellow
        Info.cLowerRight = SColor(1.0f, 1.0f, 0.0f, 0.0f);//red
        bToggle = true;
    }

    H_CHECK( I3DXFillTexture(m_pDevice,
                             pTexture,
                             I3DXFillGradient,
                             &Info) );
}


const char* PSCONST_SHADER ="                    \
sampler TextureOne : register(s0);               \
sampler TextureTwo : register(s1);               \
sampler TextureThree : register(s2);             \
sampler TextureFour : register(s3);              \
                                                 \
                                                 \
struct VS_OUTPUT                                 \
{                                                \
    float4   vDiffuse : COLOR0;                  \
    float2   vTex0 : TEXCOORD0;                  \
    float2   vTex1 : TEXCOORD1;                  \
    float2   vTex2 : TEXCOORD2;                  \
    float2   vTex3 : TEXCOORD3;                  \
};                                               \
                                                 \
float4 main(VS_OUTPUT v) : COLOR                 \
{                                                \
    float4 tex1 = 0;                             \
    float4 tex2 = 0;                             \
    float4 tex3 = 0;                             \
    float4 tex4 = 0;                             \
    int i = 0;                                   \
    float2 offset = 0 ;                          \
    for ( i = 27 ; i < ( 27 + %d ); i++ )        \
    {                                            \
        offset = float(i * 17 % 13) / 0.023;     \
        tex1 += tex2D(TextureOne, (v.vTex0 + offset )); \
        tex2 += tex2D(TextureTwo, (v.vTex1 + offset));   \
        tex3 += tex2D(TextureThree, (v.vTex2 + offset)); \
        tex4 += tex2D(TextureFour, (v.vTex3 + offset));  \
    }                                             \
    return tex1 + tex2 + tex3 + tex4;             \
}";



DWORD CBasePreemption::Render(UINT nFrame, UINT uSize)
{
    DWORD dwDone=RENDER_OK;
    EventWriteLogMsg(L"Start Render");
    
    EventWriteLogMsg(L"Clear");
    H_CHECK( m_pDevice->Clear(D3DCOLOR_ARGB(255, 0, 0, 128),//color
                              I3DXGetClearFlags(m_pDevice) | D3DCLEAR_COMPUTERECTS,//have the driver clear the entire screen
                              1.0f,//z
                              0,//stencil
                              0,//rect count
                              NULL/*rects*/) );

    EventWriteLogMsg(L"SetStreamSource");
    H_CHECK( m_pDevice->SetStreamSource(0, sizeof(Vertex), m_pVertexBuffer) );

    // setup vertex pipeline
    EventWriteLogMsg(L"I3DXSetDefaultVertexPipe");
    H_CHECK( I3DXSetDefaultVertexPipe(m_pDevice, FVF) );

    //Set the pixel shader.
    EventWriteLogMsg(L"SetPixelShader");
    H_CHECK(m_pDevice->SetPixelShader(m_pShader));

    EventWriteLogMsg(L"SetTexture");
    H_CHECK( m_pDevice->SetTexture(0, m_pTexture[nFrame]) );

    DWORD dwStartRender = GetTickCount();

    EventWriteLogMsg(L"BeginScene");
    H_CHECK( m_pDevice->BeginScene() );

    // This loop determines the number of Primitives per DMA buffer. The number
    // will be based on m_nppFrame (primitives per frame). 
    EventWriteLogMsg(L"Starting Frame loop");
    for(UINT i = 0; i <m_nppFrame; i++)
    {
        EventWriteLogMsg(L"DrawPrimitive");
        H_CHECK( m_pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,
                                          0,
                                          m_nTriangles) );
    }
    EventWriteLogMsg(L"Frame loop Done");

    EventWriteLogMsg(L"EndScene");
    H_CHECK( m_pDevice->EndScene() );
    if( (GetTickCount() - dwStartRender) >= 1500) //250 )
    {
        // We spend 1/4 second in our render call. That is too long
        dwDone = RENDER_EXCEEDED_TIME;
    }

    EventWriteLogMsg(L"Calling Present");
    HRESULT hr = m_pDevice->Present();
    if( hr == E_OUTOFMEMORY ) // "Not enought storage is available to complete this operation" 0x8007000E
    {
        m_pLog->Message(L"Present returned E_OUTOFMEMORY, cannot get memory to grow things");
        dwDone = RENDER_PRESENT_ERROR;
    }
    else
    {
        H_CHECK( m_pDevice->Present() );
    }
    EventWriteLogMsg(L"End Render");
    return dwDone;
}

//
//  This method is called each time we are to run a test case
//
bool CBasePreemption::Work(bool _GrowTriangleList,
                           bool _GrowTriangleSize,
                           bool _GrowPrimitiveCount)
{   
    bool bDoneWithTest = false;
    DWORD dwRenderDone = RENDER_OK;
    WCHAR wszBuf[MAX_PATH];

    // Setup our Textures before performing our work.
    EventWriteLogMsg(L"Starting texture creation process");
    UINT ui;
    for(ui=0;ui<THE_TEXTURE_COUNT;ui++)
    {
        if( NULL == m_pTexture[ui] )
        {
            // Brown number for uSize.
            CreateTheTexture(m_pTexture[ui], 16);
        }
    }
    EventWriteLogMsg(L"Done with texture creation process");

    //Create a pixel shader. 
    size_t stLen=0;
    // Shader size plus room for number expansion.
    if( S_OK != StringCchLengthA(PSCONST_SHADER,2048,&stLen) )
    {
        DbgBreakPoint();
    }
    char* pszShader = new char[stLen+64];

    StringCchPrintfA(pszShader,stLen+64,PSCONST_SHADER,m_nShaderInstructions);

    EventWriteLogMsg(L"I3DXCreateHLSLShader");
    H_CHECK(I3DXCreateHLSLShader((const char *)pszShader,
                                "main",
                                PS_BEST_AVAILABLE,
                                m_pDevice,
                                &m_pShader.p));

    // We'll loop for m_nTotalTime seconds.
    DWORD dwStartRender;
    time_t StartTime = time(NULL);
    time_t CurrentTime = StartTime;

    UINT nFrame=0;
    EventWriteLogMsg(L"Start Work loop");
    do
    {
        if( nFrame >= THE_TEXTURE_COUNT )
        {
            nFrame=0;
        }

        // Now we want to time the actual render call. if our renders take too long,
        // the User mode driver will simply carve up our data into little DMA buffers
        // and we don't get what we expect - which is larger DMA buffers.
        dwRenderDone = Render(nFrame,0);
        if( RENDER_PRESENT_ERROR == dwRenderDone )
        {
            bDoneWithTest = true;
            m_pLog->Message(L"Terminating test due to present out of memory error.");
            break;
        }
        if( RENDER_EXCEEDED_TIME == dwRenderDone )
        {
            bDoneWithTest = true;
            // We spend 1/4 second in our render call. That is too long
            m_pLog->Message(L"Terminating test due to scene building excessive time.");
            break;
        }

        nFrame++;

        // update our time for the time check
        CurrentTime = time(NULL);
    }while(((UINT)(CurrentTime - StartTime)) <= m_nTotalTime);
    EventWriteLogMsg(L"End Work loop");

    // Each time through our work routine, we need to push the results for our final summary.
    float fAveragePremptionTime=0.0;
    float fAverageDMAProcessingTime=0.0;

    // Longest consecutive execution time
    ULONGLONG ullLongestConsecutiveTime = etwDMAStandardTimeConsumer.LongestConsecutiveTime();
    ullLongestConsecutiveTime = etwController.TimeStampToMilliseconds(ullLongestConsecutiveTime);

    // Longest DMA packet time.
    ULONGLONG ullLongestExecutionTime = etwDMAStandardTimeConsumer.LongestExecutionTime();
    if( ullLongestExecutionTime == -1 )
    {
        ullLongestExecutionTime=0;
    }
    else
    {
        ullLongestExecutionTime = etwController.TimeStampToMilliseconds(ullLongestExecutionTime);
    }
#ifdef DBG_DMAPACKET
#ifdef UNICODE
    StringCchPrintf(wszBuf,MAX_PATH,L"  LongestConsecutiveTimeMS: %I64d, LongestExecutionTime: %I64d\n\r",
                    ullLongestConsecutiveTime,
                    ullLongestExecutionTime);
    OutputDebugString(wszBuf);
    m_pLog->Message(wszBuf);
#endif
#endif

    // Need to terminate based upon longest packet during pass.
    if( ullLongestConsecutiveTime > 300 )
    {
        m_pLog->Message(L"Terminating test due to LongestConsecutiveTimeMS greater than 300 ms");
        OutputDebugString(L"Terminating test due to LongestConsecutiveTimeMS greater than 300 ms");
        bDoneWithTest = true;
    }

    //----------------------
    ULONGLONG ullPreemptionRequestValue = 0;
    ULONGLONG ullDMAValue = 0;
    m_ullLongestPreemptionRequestTime = 0;

    ULONG ulCount=0;
    ULONGLONG ullTimeMS = 0;
    ULONGLONG ullTime = 0;
    std::list<CDelayData*>* pList =  etwDMAStandardTimeConsumer.GetDelayList();

    std::list<CDelayData*>::iterator itList = pList->begin();
    while( itList != pList->end() )
    {
        CDelayData* pData = (*itList);

#ifdef DBG_DMAPACKET
#ifdef UNICODE
        WCHAR wszBuf[MAX_PATH];
        StringCchPrintf(wszBuf,MAX_PATH,L"  Work: DMAStart=%I64d, DMALen=%I64d, PreemptStart=%I64d, PreemptLen=%I64d\n\r",
                        pData->DMAStart(),pData->DMALength(),
                        pData->PreemptionRequestStart(),pData->PreemptionRequestLength());
        OutputDebugString(wszBuf);
#endif
#endif
        // The preemption algorithm is designed around the idea that when a preemption request is
        // issued, it's going to happen near the start of the DMA packet that we're tracking. yet,
        // based on the hardware capabilities and the driver actions, the data may come through
        // carved up.

        ullTime = pData->DMALength();

        // For DMA boundary checking, we only look at DMA packets that are 64ms in length


        ullTimeMS = etwController.TimeStampToMilliseconds(ullTime);
        if( ullTimeMS < fBOUNDARY_THRESHOLD )
        {
#ifdef DBG_DMAPACKET
#ifdef UNICODE
            StringCchPrintf(wszBuf,MAX_PATH,L"Work: Not long enough: ullTimeMS=%I64d\n\r",ullTimeMS);
            OutputDebugString(wszBuf);
            m_pLog->Message(wszBuf);
#endif
#endif
            // We can't use any packet that isn't long enough.
            itList++;
            continue;
        }

        // if it is long enough, let's validate that that the preemption request happened within
        // the required windows (FIRST_FRAME_MS)

        ullTime = (pData->PreemptionRequestStart()-pData->DMAStart());
        ullTimeMS = etwController.TimeStampToMilliseconds(ullTime);

        if( ullTimeMS > FIRST_FRAME_MS )
        {
#ifdef DBG_DMAPACKET
#ifdef UNICODE
            StringCchPrintf(wszBuf,MAX_PATH,L"Work: Started late: ullTimeMS=%I64d\n\r",ullTimeMS);
            OutputDebugString(wszBuf);
            m_pLog->Message(wszBuf);
#endif
#endif
            // we can't use something that starts late.
            itList++;
            continue;
        }

        // if we get here, we have a packet that fits the mold
        ulCount++;

        ullPreemptionRequestValue += pData->PreemptionRequestLength();
        ullDMAValue += pData->DMALength();

        if( m_ullLongestPreemptionRequestTime < pData->PreemptionRequestLength() )
        {
            m_ullLongestPreemptionRequestTime = pData->PreemptionRequestLength();
        }

#ifdef DBG_DMAPACKET
#ifdef UNICODE
        StringCchPrintf(wszBuf,MAX_PATH,L"Work: Fits: ulCount=%d, ullPreemptionRequestValue=%I64d, ullDMAValue=%I64d, ullTimeMS=%I64d\n\r",
                        ulCount,ullPreemptionRequestValue,ullDMAValue,ullTimeMS);
        OutputDebugString(wszBuf);
        m_pLog->Message(wszBuf);
#endif
#endif
        itList++;
    }

    ULONGLONG ullPreemptionRequestValuems = etwDMAStandardTimeConsumer.TimeStampToMilliseconds(ullPreemptionRequestValue);
    ULONGLONG ullDMAValuems = etwDMAStandardTimeConsumer.TimeStampToMilliseconds(ullDMAValue);
    m_ullLongestPreemptionRequestTime = etwDMAStandardTimeConsumer.TimeStampToMilliseconds(m_ullLongestPreemptionRequestTime);

    //----------------------

#ifdef DBG
    StringCchPrintf(wszBuf,MAX_PATH,L"DMA totals: Count=%d, Value=%I64d, ms=%I64d\n\r",
                    ulCount,ullDMAValue,ullDMAValuems);
    OutputDebugString(wszBuf);
    m_pLog->Message(wszBuf);
#endif

    float fAverage=0.0;
    if(ulCount)
    {
        fAverage = ((float)((float)ullPreemptionRequestValuems/(float)ulCount));
    }
    fAveragePremptionTime=fAverage;

    fAverage=0.0;
    if(ulCount)
    {
        fAverage = ((float)((float)ullDMAValuems/(float)ulCount));
    }
    fAverageDMAProcessingTime=fAverage;


    // We'll quit the test if our DMA buffers take more then 250 ms to complete
    if( fAverageDMAProcessingTime > 250.0 )
    {
        m_pLog->Message(L"Terminating test due to DMA processing time length greater than 250 ms");

        bDoneWithTest = true;
    }

    // Need to know how many packets where preempted, this is the actual count of DMA
    // packets that have the prempted property. This is different than the number of 
    // preemption requests (above) for many DMA packets could finish with the preemption
    // bit set for each preemption request.
    UINT uiCountOfPreemptions = etwDMAStandardTimeConsumer.GetPreemptionCount();
    if(0 == uiCountOfPreemptions)
    {
        m_pLog->Message(L"No preempted packets detected with this workload.");
    }
    else
    {
        WCHAR wszBuf[MAX_PATH];
        StringCchPrintf(wszBuf,MAX_PATH,L"Count of DMA packets Preempted=%d",uiCountOfPreemptions);
        m_pLog->Message(wszBuf);
    }

    ResultsData* pResults = new ResultsData(m_nppFrame,            // PrimitivesPerFrame
                                            m_nTriangles,          // TrianglesPerPrimitive
                                            m_nppPrimitive,        // PixelsPerTriangle
                                            m_nShaderInstructions, // ShaderInstructions
                                            ulCount,
                                            fAveragePremptionTime,
                                            m_ullLongestPreemptionRequestTime,
                                            ulCount,
                                            fAverageDMAProcessingTime,
                                            uiCountOfPreemptions,
                                            ullLongestConsecutiveTime,
                                            ullLongestExecutionTime);
    m_Results.push_back(pResults);

    return bDoneWithTest;
}

// NAME:        UnicodeToAnsi()
// DESC:        Unicode to ANSI conversion
// INPUT:       pszWide .address of null termianted string (wide character string)
//              ppszAnsi.address of null terminated string (ANSI character string)
// OUTPUT:      Number of characters copied, note including NULL.

UINT CBasePreemption::UnicodeToAnsi(wchar_t *pwszWide, LPSTR *ppszAnsi)
{
    ULONG nAnsi, nWide;

    if (NULL == pwszWide)
    {
        *ppszAnsi = NULL;
        return 0;
    }

    // determine number of bytes to be allocated for ANSI string
    // (ANSI strings can have at most 2 bytes per character (for Double Byte Character Strings))

    nWide = wcslen(pwszWide) + 1;
    nAnsi = nWide*2;
    *ppszAnsi = (LPSTR) malloc(nAnsi);

    if (NULL == *ppszAnsi)
    {
        *ppszAnsi = NULL;
        return 0;
    }

    // convert to ANSI

    if (0 == WideCharToMultiByte(CP_ACP, 0, pwszWide, nWide, *ppszAnsi, nAnsi, NULL, NULL))
    {
        free(*ppszAnsi);
        *ppszAnsi = NULL;
        return 0;
    }

    return nWide;
} 


#define WRITE_RESULTS_LEN MAX_PATH*2

void CBasePreemption::WriteResults(bool _GrowTriangleList,
                                   bool _GrowTriangleSize,
                                   bool _GrowPrimitiveCount)
{
    FILE* stream=NULL;
    WCHAR wszBuf[WRITE_RESULTS_LEN];

    // Let's look at our command line arguments.
    StringCchPrintf(wszBuf,WRITE_RESULTS_LEN,L"TotalTimeToRun=%d",m_nTotalTime);
    m_pLog->Message(wszBuf);
    StringCchPrintf(wszBuf,WRITE_RESULTS_LEN,L"Triangles=%d",m_nTriangles);
    m_pLog->Message(wszBuf);
    StringCchPrintf(wszBuf,WRITE_RESULTS_LEN,L"m_nppPrimitive=%d",m_nppPrimitive);
    m_pLog->Message(wszBuf);
    StringCchPrintf(wszBuf,WRITE_RESULTS_LEN,L"m_nppFrame=%d",m_nppFrame);
    m_pLog->Message(wszBuf);
    StringCchPrintf(wszBuf,WRITE_RESULTS_LEN,L"m_nShader=%d",m_nShaderInstructions);
    m_pLog->Message(wszBuf);
    StringCchPrintf(wszBuf,WRITE_RESULTS_LEN,L"m_bWantCSVFile=%s",(m_bWantCSVFile?L"true":L"false"));
    m_pLog->Message(wszBuf);
    StringCchPrintf(wszBuf,WRITE_RESULTS_LEN,L"m_bDWM=%s",(m_bDWM?L"true":L"false"));
    m_pLog->Message(wszBuf);

    if( m_bWantCSVFile )
    {
        TCHAR tBuf[MAX_PATH];
        DWORD dwRet = GetCurrentDirectory(MAX_PATH,tBuf);

        if( dwRet )
        {
            // Add our name to the path
            HRESULT hr = StringCchCat(tBuf,MAX_PATH,L"\\surfboard.csv");
            if( S_OK == hr )
            {
                char* pszFileName=NULL;
                UINT iLen = UnicodeToAnsi(tBuf, &pszFileName);
                if( iLen && pszFileName != NULL )
                {
                    // should have ansi string now.

                    // open the CSV file for writing
                    // TODO: What file name do we use?  GetCurrentDirectory? with a log appended?
                    fopen_s(&stream,pszFileName,"w");
                }
                else
                {
                    WCHAR wszMsg[MAX_PATH];
                    StringCchPrintf(wszMsg,MAX_PATH,L"Unable to open csv file: %s",tBuf);
                    m_pLog->Fail(wszMsg);
                }
                free(pszFileName);
            }
            else
            {
                WCHAR wszMsg[MAX_PATH];
                StringCchPrintf(wszMsg,MAX_PATH,L"Unable to create csv file name: %s",tBuf);
                m_pLog->Fail(wszMsg);
            }
        }
        else
        {
            WCHAR wszMsg[MAX_PATH];
            StringCchPrintf(wszMsg,MAX_PATH,L"Unable to get current directory for csv file: %s",tBuf);
            m_pLog->Fail(wszMsg);
        }
    }


    //
    // Here is where we want to dump out the summary of what we gathered. we want the data
    // to look something like this:
    //
    // output the header for the data that's coming.
    StringCchPrintf(wszBuf,WRITE_RESULTS_LEN,
                    L"%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s,%s", // 12 strings make up the header.
                    L"Primitives Per Frame",
                    L"Triangles Per Primitive",
                    L"Pixels Per Triangle",
                    L"Shader Instructions",
                    L"Preemption Request Count",
                    L"Average Premption Request Time",
                    L"Longest Preemption Request Wait",
                    L"DMA Packet Count",
                    L"Average DMA Processing Time",
                    L"Count Of Preempted Packets",
                    L"Longest Consecutive Time",
                    L"Longest DMA Packet");
    m_pLog->Message(wszBuf);
    if( m_bWantCSVFile && stream!=NULL)
    {
        char* pData=NULL;
        UINT iLen = UnicodeToAnsi(wszBuf, &pData);
        if( iLen && pData != NULL )
        {
            fwrite(pData,iLen,1,stream);
            fwrite("\n\r", 2, 1, stream);
            free(pData);
        }
        else
        {
            WCHAR wszMsg[MAX_PATH];
            StringCchPrintf(wszMsg,MAX_PATH,L"Unable to convert unicode to ansi: %s",wszBuf);
            m_pLog->Fail(wszMsg);
        }
    }


    UINT uiCountOfPreemptions=0;
    std::vector<ResultsData*>::iterator It;
    for(It = m_Results.begin();
        It != m_Results.end();
        It++)
    {
        // dump one line.
        ResultsData* pResults = *It;

        StringCchPrintf(wszBuf,WRITE_RESULTS_LEN, L"%d,%d,%d,%d,%d,%4.2f,%I64d,%d,%f,%d,%I64d,%I64d", // 12 data items
                        pResults->uiPrimitivesPerFrame,
                        pResults->uiTrianglesPerPrimitive,
                        pResults->uiPixelsPerTriangle,
                        pResults->uiShaderInstructions,
                        pResults->uiPreemptionRequestCount,

                        pResults->fAveragePremptionTime,
                        pResults->ullLongestPreemptionWait,
                        pResults->uiDMAPacketCount,
                        pResults->fAverageDMAProcessingTime,
                        pResults->uiCountOfPreemptedPackets,

                        pResults->ullLongestConsecutiveTime,
                        pResults->ullLongestDMAPacket
                        );

        m_pLog->Message(wszBuf);
        if( m_bWantCSVFile && stream!=NULL)
        {
            char* pData=NULL;
            UINT iLen = UnicodeToAnsi(wszBuf, &pData);
            if( iLen && pData != NULL )
            {
                fwrite(pData,iLen,1,stream);
                fwrite("\n\r", 2, 1, stream);
                free(pData);
            }
            else
            {
                WCHAR wszMsg[MAX_PATH];
                StringCchPrintf(wszMsg,MAX_PATH,L"Unable to convert unicode to ansi: %s",wszBuf);
                m_pLog->Fail(wszMsg);
            }
        }

        // Now, did the hardware act like we wanted it too?
        if( D3DKMDT_GRAPHICS_PREEMPTION_NONE == m_Level )
        {
            // older drivers can preempt if they want. There is nothing saying that they
            // can't preempt. Thus there is nothing that can fail them for here.
        }
        else if( D3DKMDT_GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY == m_Level )
        {
            if( pResults->uiDMAPacketCount != pResults->uiPreemptionRequestCount )
            {
                WCHAR wszMsg[MAX_PATH];
                StringCchPrintf(wszMsg,MAX_PATH,
                                L"Counts not equal, (averages will be off) skipping: uiDMAPacketCount=%d, uiPreemptionCount=%d",
                                pResults->uiDMAPacketCount,
                                pResults->uiPreemptionRequestCount);
                m_pLog->Message(wszMsg);
            }
            else
            {
                if( (pResults->fAverageDMAProcessingTime >= fBOUNDARY_THRESHOLD) )
                {
                    bool bFail=false;

                    WCHAR wszMsg[MAX_PATH];
                    StringCchPrintf(wszMsg,MAX_PATH,
                                    L"Validating on large workload, AveragePreemptionPacket=%f, AverageDMAPacket=%f",
                                    pResults->fAveragePremptionTime,
                                    pResults->fAverageDMAProcessingTime);
                    m_pLog->Message(wszMsg);
                    // we have a big enough packet to validate on.
                    //
                    // we're expecting:
                    //  (Average_DMA_Completion_Time*.9) <= Averqage_Request_Time <= Average_DMA_Completion_Time
                    //
                    // Note that 90% is just a guess, thus we'll use fBOUNDARY_SLACK
                    // BOUNDARY_THRESHOLD = 64ms
#ifdef WANT_FINER_GRANULARITY
                    if( pResults->fAveragePremptionTime < (pResults->fAverageDMAProcessingTime*fBOUNDARY_SLACK) )
                    {
                        // The average preemption time is less than what we're expecting for 
                        // boundary conditions

                        // here is where we fail.
                        StringCchPrintf(wszBuf,WRITE_RESULTS_LEN,
    L"Failed: Hardware Claims to support Boundary preemption, but preempts on a finer granularty. "
    L"AvePreemptionTimePerSec=%f, AveDMATimePerSec=%f",
                                        pResults->fAveragePremptionTime,
                                        pResults->fAverageDMAProcessingTime);
                        m_pLog->Fail(wszBuf);
                        bFail=true;
                    }
#endif
                    if( pResults->fAveragePremptionTime > pResults->fAverageDMAProcessingTime )
                    {
                        // The average preemption time is longer than the average DMA time. We
                        // expect a shorter time.
                        // here is where we fail.
                        StringCchPrintf(wszBuf,WRITE_RESULTS_LEN,
    L"Failed: Hardware Claims to support Boundary preemption, but preemption requests are longer then DMA packets. "
    L"AvePreemptionTimePerSec=%f, AveDMATimePerSec=%f",
                                        pResults->fAveragePremptionTime,
                                        pResults->fAverageDMAProcessingTime);
                        m_pLog->Fail(wszBuf);
                        bFail=true;
                    }
                    // This gives us passing numbers for every check we make.
                    if( !bFail )
                    {
                        m_pLog->StartSubTest();
                    }
                }
            }
        }
        else
        {
            // we always expect preemptions if the hardware is capable of preemptions. But we're
            // going to cut the hardware some slack and only check if they preempt if the 
            // workload on the hardware takes over 3 milliseconds.
            if(  pResults->fAverageDMAProcessingTime >= 3.0 )
            {
                // if we asked for a preemption, we expect to get one
                if( (0 == pResults->uiCountOfPreemptedPackets) && 
                    (0 != pResults->uiPreemptionRequestCount) )
                {
                    // here is where we fail.
                    StringCchPrintf(wszBuf,WRITE_RESULTS_LEN,
L"Failed: Hardware Claims to support preemption, but doesn't preempt larger workloads. AveDMATimePerSec=%f, PreemptionCount=%d",
                                    pResults->fAverageDMAProcessingTime,
                                    pResults->uiCountOfPreemptedPackets);
                    m_pLog->Fail(wszBuf);
                }
            }
        }
    }
    m_Results.clear();

    if( m_bWantCSVFile && stream!=NULL)
    {
        fclose(stream);
    }

}

// returns false on error (fail test), true otherwise.
bool CBasePreemption::GetPreemptionLevelSupported()
{
    bool bRet=true;
    WCHAR wszBuf[MAX_PATH]={0};

    CQueryPreemptionCaps VidHardwareCaps(m_pLog);
    NTSTATUS ntStatus = VidHardwareCaps.QueryForCapabilities(m_pDevice->GetDisplayName());
    if( NT_SUCCESS(ntStatus) )
    {
        //successfully got capabilities
        if( VidHardwareCaps.IsPreemptableGraphics() )
        {
            LogMessageF(m_pLog, TEXT("This adapter (%s) supports Graphics Preemption"),
                        m_pDevice->GetDisplayName());

            // Log the level
            m_Level = VidHardwareCaps.GetGraphicsPreemptionGranularity();
            LogMessageF(m_pLog, TEXT("Graphics Preemption level %d"),m_Level);
        }
        else
        {
            StringCchPrintf(wszBuf,MAX_PATH,
                            L"This adapter (%s) does not support Graphics Preemption",
                            m_pDevice->GetDisplayName());
            m_pLog->Skip(wszBuf);
            return false;
        }
    }
    else
    {
        PATH_TRAP();
        // failed to get capabilities
        StringCchPrintf(wszBuf,MAX_PATH,L"Failed in call to KmQueryAdapterInfo(Type = KMTQAITYPE_WDDM_1_2_CAPS) ntStatus=%d(0x%08x)",
                        ntStatus,ntStatus);
        m_pLog->Fail(wszBuf);
        bRet = false;
    }

    return bRet;
}


void CBasePreemption::StandardTest(bool _GrowTriangleList,
                                   bool _GrowTriangleSize,
                                   bool _GrowPrimitiveCount,
                                   bool _Loop)
{
    ENTER();

    WCHAR wszBuf[MAX_PATH]={0};
    time_t StartTime = time(NULL);

    m_nTotalTime = GetParam<UINT>(L"TotalTime");
    m_nTriangles = GetParam<UINT>(L"nTriangles");
    m_nppPrimitive = GetParam<UINT>(L"nppPrimitive");
    m_nppFrame = GetParam<UINT>(L"nppFrame");
    m_nShaderInstructions = GetParam<UINT>(L"nShaderInstructions");
    UINT uiIterations = GetParam<UINT>(L"nIterations");
    m_bWantCSVFile = GetParam<bool>(L"bWantCSVFile");
    m_bDWM = GetParam<bool>(L"bDWM");

#ifdef DBG
    // Let's look at our command line arguments.
    StringCchPrintf(wszBuf,MAX_PATH,L"TotalTimeToRun=%d\n\r",m_nTotalTime);
    OutputDebugString(wszBuf);
    StringCchPrintf(wszBuf,MAX_PATH,L"Triangles=%d\n\r",m_nTriangles);
    OutputDebugString(wszBuf);
    StringCchPrintf(wszBuf,MAX_PATH,L"m_nppPrimitive=%d\n\r",m_nppPrimitive);
    OutputDebugString(wszBuf);
    StringCchPrintf(wszBuf,MAX_PATH,L"m_nppFrame=%d\n\r",m_nppFrame);
    OutputDebugString(wszBuf);
    StringCchPrintf(wszBuf,MAX_PATH,L"m_nShader=%d\n\r",m_nShaderInstructions);
    OutputDebugString(wszBuf);
    StringCchPrintf(wszBuf,MAX_PATH,L"uiIterations=%d\n\r",uiIterations);
    OutputDebugString(wszBuf);
    StringCchPrintf(wszBuf,MAX_PATH,L"m_bWantCSVFile=%s\n\r",(m_bWantCSVFile?L"true":L"false"));
    OutputDebugString(wszBuf);
    StringCchPrintf(wszBuf,MAX_PATH,L"m_bDWM=%s\n\r",(m_bDWM?L"true":L"false"));
    OutputDebugString(wszBuf);
#endif

    // If DWM:true is not on the command line, run our preemption check
    if( !m_bDWM )
    {
        // In order to determine what the hardware capabilties are, we'll need to call and
        // find out.
        if( GetPreemptionLevelSupported() == false )
        {
            return;
        }
    }

    // 40 is a special number. if we ask for 40, we'll run for 15 minutes or until our
    // DMA packets get to be 1/4 second in length.
    if( 40 == uiIterations )
    {
        uiIterations = 1000;
    }

    // What level work should we do?
    //
    // NOTE: With all these levels, we should see preempted packets when we issue work on the GPU.
    // Even if they can't preempt in the middle of a DMA packet, they should preempt packets that 
    // haven't started running on the hardware.
    // Thus, if we don't see any preemption packets, we'll fail.
    //
    // Likewise, when tracking preemption of DMA packets, once a preemption request is issued
    // we only want to track the time it takes until the current running packet completes. 
    //
    // Use: CDMAStandardTime counter with the TrackNextCompletion flags.
    //
    etwDMAStandardTimeConsumer.TrackNextCompleted();
    etwDMAStandardTimeConsumer.TrackPacketsThatDelayPreemptions();
/*
    // This section is for notes only.
    if( D3DKMDT_GRAPHICS_PREEMPTION_NONE == m_Level )
    {
        // Here, the hardware does not perform any type of preemption. In this case, our preemption
        // packets should take as long as the number of packets in the queue take to complete. For instance,
        // if there are three packets on the hardware, and the first packet takes Xms to complete
        // and the second takes Yms to complete and the last takes Wms to complete, the preemption 
        // packet will take at least W+Y ms or up to W+Y+X ms. Thus, if we average 4ms per packet
        // and we're three deep, the premption packets should come in between 8 and 12 in length and 
        // average 10.
        //
        // Averqage_Request_Time > Average_DMA_Completion_Time
        //
        // Use: CDMAStandardTime counter with TrackActual but not TrackNextCompletion
    }
    else if(D3DKMDT_GRAPHICS_PREEMPTION_DMA_BUFFER_BOUNDARY == m_Level )
    {
        // Here, the hardware is known to complete the currently executing packet and then
        // it preempts all following packets. Since we're timing actual time on hardware
        // of the next completed packet, we expect to see preemption requests that take
        // no longer than the length of the next completed packet. 
        //
        // Averqage_Request_Time <= Average_DMA_Completion_Time
        //
        // And because we are running in an environment where lots of preemption requests
        // are being made (SchBillboard - 1ms granulartiy, DWM - 16 MS granularity), we
        // expect that as packets get larger the preemption request time will converge on
        // the DMA completion time.
        //
        //  (Average_DMA_Completion_Time*.9) <= Averqage_Request_Time <= Average_DMA_Completion_Time
        //
        // Note that 90% is just a guess, thus we'll use fBOUNDARY_SLACK
        // fBOUNDARY_THRESHOLD = 64.0ms
        //
        // Also, the preemption time will grow along with the execution time of the DMA packets.
        // Thus, as the DMA times grow, so do the preemption times.
        //
        // When dealing with slack, we want to make sure that our packets are at least
        // as long as four frames in length. Thus, 4*16=64ms where we could have started
        // the preemption request anywhere in the first packet. This would make our preemption
        // packet anywhere between 3*16=48ms and 64ms (average 56ms). If we where to figure
        // slack for the lower bound, it might be 56/64=0.875. 
    }
    else if( D3DKMDT_GRAPHICS_PREEMPTION_PRIMITIVE_BOUNDARY == m_Level )
    {
        // Here, and with the finer granularity preemption, we'll have to contrive a workload
        // of a specific type in order to force the hardware to respond like we're expecting.
        // With regards to primitive boundary support:
        //
        //
        // TODO: Outline that layout here.
        //
    }
    else if( D3DKMDT_GRAPHICS_PREEMPTION_TRIANGLE_BOUNDARY == m_Level )
    {
        // TODO:
    }
    else if( D3DKMDT_GRAPHICS_PREEMPTION_PIXEL_BOUNDARY == m_Level )
    {
        // TODO:
    }
    else if( D3DKMDT_GRAPHICS_PREEMPTION_SHADER_BOUNDARY == m_Level )
    {
        // TODO:
    }
    else 
    {
        PATH_TRAP();
        StringCchPrintf(wszBuf,MAX_PATH,L"Unknown Preemption Type");
        m_pLog->Fail(wszBuf);
        return;
    }
*/
    DWORD dwStartTime = GetTickCount();
    // here is where we'll loop growing our work load every time.
    bool bDone = false;
    UINT uiLoop=0;
    do
    {
        // Create our triangle
        CTriangle TrianglePrim(EquilateralTriangle,m_nppPrimitive);

        // Create our triangle list
        CreateTheVB(&TrianglePrim,m_nTriangles);

        // Output work parameters before actually working
        StringCchPrintf(wszBuf,MAX_PATH,L"%d: Work: m_nTriangles=%d, m_nppPrimitive=%d, m_nppFrame=%d",
                        uiLoop,m_nTriangles,m_nppPrimitive,m_nppFrame);
        m_pLog->Message(wszBuf);
        OutputDebugString(wszBuf);
        OutputDebugString(L"\n\r");

        // Now start recording data.
        etwDMAStandardTimeConsumer.Reset();
        etwDMAStandardTimeConsumer.Start();

        // put it to work.
        bDone = Work(_GrowTriangleList,_GrowTriangleSize,_GrowPrimitiveCount);

        // Now stop gathering data.
        etwDMAStandardTimeConsumer.Stop();

        if( !_Loop )
        {
            // Force a completion on the loop - we're only going through one time.
            bDone = true;
        }
        else
        {
            if( _GrowTriangleList )
            {
                // Grow the triangle list length
                m_nTriangles *=2;
            }
            if( _GrowTriangleSize ) 
            {
                // Grow the size of the individual triangle
                m_nppPrimitive *=2;
            }
            if( _GrowPrimitiveCount )
            {
                // Grow the number of primitives in each DMA packet.

                // Because we're dealing with all different types of hardware, some machines
                // will handle our workload in fractions of a second, others will take a much
                // longer time. So, if we're on faster hardware, we'll want to grow faster than
                // slower hardware. 
                
                if( m_ullLongestPreemptionRequestTime < 48 )
                {
                    // grow quickly
                    m_nppFrame += 10;
                }
                else
                {
                    m_nppFrame += 3;
                }
            }
        }

        uiLoop++;


        // never run for more than 15 minutes
        if( (GetTickCount() - dwStartTime ) > ((60*1000)*15) )
        {
            bDone = true;
            StringCchPrintf(wszBuf,MAX_PATH,L"Exceeded time limit of 10 minutes");
            m_pLog->Message(wszBuf);
        }

    }while(!bDone && uiLoop<uiIterations);

    WriteResults(_GrowTriangleList,_GrowTriangleSize,_GrowPrimitiveCount);

    time_t CurrentTime = time(NULL);
    StringCchPrintf(wszBuf,MAX_PATH,L"Test took %d seconds",(CurrentTime-StartTime));
    m_pLog->Message(wszBuf);

    LEAVE();
}


//---------------------------- CBasePreemption ---------------------------------



//---------------------------- CPreemptionShader -------------------------------
//
// A command line might look like:
//
//  Surfboard Preempt Time:5 ppFrame:10 Triangles:100 ppPrimitive:10000 ShaderInstructions:5 monitor:2 windowed:true listen PreemptionTime:true StandardTime:true
//
// Or, to generate an event file showing the work.
//
//  Surfboard Preempt Time:5 ppFrame:10 Triangles:100 ppPrimitive:10000 ShaderInstructions:5 monitor:2 windowed:true eventlog
//
CPreemptionShader::CPreemptionShader()
{
    //
    //  First, we describe the purpose of this component
    //  for the spec
    //
    SetDescription(L"Manual test pass");

    SetupCommandLine(5,   // TotalTime
                     1000, // nTriangles
                     1000, // nppPrimitive
                     1,    // nppFrame
                     1);   // nShaderInstructions
}

void CPreemptionShader::RunTest()
{
    StandardTest(false,  // Grow Triangle List
                 false,  // Grow Triangle Size
                 false,  // grow Primitive count in DMA buffer
                 false); // Loop
}


void CPreemptionShader::EndTest()
{
    ENTER();
    FreeResources();
    LEAVE();
}

//---------------------------- CPreemptionShader -------------------------------



//--------------------- Growing the Count of Triangles -------------------------
//
// Each time through the main loop we will double the number of triangles in the triangle
// list.
//
// A command line might look like:
//
//  Surfboard GrowTriangleList monitor:2 windowed:true listen PreemptionTime:true StandardTime:true
//
// Or, to generate an event file showing the work.
//
//  Surfboard GrowTriangleList Time:5 ppFrame:10 Triangles:100 ppPrimitive:10000 ShaderInstructions:1 monitor:2 windowed:true eventlog
//
CGrowTriangleList::CGrowTriangleList()
{
    SetDescription(L"This is our surfboard manager component. It does the book keeping of textures and VBs");

    SetupCommandLine(5,    // TotalTime
                     1,    // nTriangles - This will very every loop.
                     1000, // nppPrimitive
                     2,    // nppFrame
                     1);   // nShaderInstructions
}


void CGrowTriangleList::RunTest()
{
    StandardTest(true,   // Grow Triangle List
                 false,  // Grow Triangle Size
                 false,  // grow Primitive count in DMA buffer
                 true);  // Loop
}


void CGrowTriangleList::EndTest()
{
    ENTER();
    FreeResources();
    LEAVE();
}

//--------------------- Growing the Size of Triangles -------------------------
//
// Each time through the main loop we will double the size of each triangle in the triangle
// list.
//
// A command line might look like:
//
//  Surfboard GrowTriangleSize monitor:2 windowed:true listen PreemptionTime:true StandardTime:true
//
// Or, to generate an event file showing the work.
//
//  Surfboard GrowTriangleSize Time:5 ppFrame:10 Triangles:100 ppPrimitive:10000 ShaderInstructions:1 monitor:2 windowed:true eventlog
//
CGrowTriangleSize::CGrowTriangleSize()
{
    SetDescription(L"This is our surfboard manager component. It does the book keeping of textures and VBs");

    SetupCommandLine(5,    // TotalTime
                     10,   // nTriangles 1000
                     32,   // nppPrimitive - This will very every loop.
                     1,    // nppFrame
                     1);   // nShaderInstructions
}


void CGrowTriangleSize::RunTest()
{
    StandardTest(false,  // Grow Triangle List
                 true,   // Grow Triangle Size
                 false,  // grow Primitive count in DMA buffer
                 true);  // Loop
}


void CGrowTriangleSize::EndTest()
{
    ENTER();
    FreeResources();
    LEAVE();
}

//--------------------- Growing the Size of Triangles -------------------------
//
// Each time through the main loop we will double the count of primitives in the main
// DMA buffer.
//
// A command line might look like:
//
//  Surfboard GrowPrimitiveCount monitor:2 windowed:true listen PreemptionTime:true StandardTime:true
//
// Or, to generate an event file showing the work.
//
//  Surfboard GrowPrimitiveCount Time:5 ppFrame:10 Triangles:100 ppPrimitive:10000 ShaderInstructions:1 monitor:2 windowed:true eventlog Iterations:15
//
CGrowPrimitiveCount::CGrowPrimitiveCount()
{
    SetDescription(L"This is our surfboard manager component. It does the book keeping of textures and VBs");

    SetupCommandLine(5,    // TotalTime
                     100,  // nTriangles
                     1000, // nppPrimitive - This will very every loop.
                     1,    // nppFrame
                     1);   // nShaderInstructions
}


void CGrowPrimitiveCount::RunTest()
{
    StandardTest(false,  // Grow Triangle List
                 false,  // Grow Triangle Size
                 true,   // grow Primitive count in DMA buffer
                 true);  // Loop
}


void CGrowPrimitiveCount::EndTest()
{
    ENTER();
    FreeResources();
    LEAVE();
}



