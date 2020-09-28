#include <VidmmRequires.h>

#include "Image.h"


#ifndef _WIN64
typedef BOOL (*IsWow64ProcessFunc)(HANDLE hProcess, PBOOL Wow64Process);
#endif


bool Is32BitAddressSpace()
{

#ifndef _WIN64
    BOOL bIsWow = FALSE;
    IsWow64ProcessFunc IsWow64ProcFunc = NULL;
    HMODULE hKernel32 = GetModuleHandle(_T("kernel32"));
    if(NULL != hKernel32)
    {
        IsWow64ProcFunc = (IsWow64ProcessFunc)GetProcAddress(hKernel32, "IsWow64Process");
    }
    if (NULL != IsWow64ProcFunc)
    {
        if ((IsWow64ProcFunc)(GetCurrentProcess(), &bIsWow) == 0)
        {
            // error
            return false;
        }
        if (bIsWow)
        {
            //
            //	We are 32-bit app running on 64-bit OS
            //
            return false;
        }
    }
    //
    //	We are 32-bit app running on 32-bit OS
    //
    return true;
#else
    //
    //	We are 64-bit application, so we are running on 64-bit OS
    //
    return false;
#endif	
}


CImage::~CImage()
{
    if(mVertices)
    {
        delete [] mVertices;
        mVertices = NULL;
    }

    if(m_pTexture)
    {
        delete [] m_pTexture;
        m_pTexture = NULL;
    }

    if(m_nBytesPerPixel)
    {
        delete [] m_nBytesPerPixel;
        m_nBytesPerPixel = NULL;
    }
}

CImage::CImage(UINT nTriangles,CLogWriter* pLog)
: m_nTriangles(nTriangles),
  m_nTexture(0),
  m_pLog(pLog)
{
    m_AvailableVidMem = GetVidMemSize();
    m_nRequiredTextures = CalculateNumRequiredTexture();
    mVertices = new Vertex[m_nTriangles * 3];
    m_pTexture = new CI3DTexturePtr[m_nRequiredTextures];
    m_nBytesPerPixel = new int[m_nRequiredTextures];

    if(!mVertices || !m_pTexture || !m_nBytesPerPixel)
        throw std::exception("Failed to allocate memory");

    ZeroMemory(mVertices, sizeof(Vertex) * m_nTriangles * 3);
    ZeroMemory(m_pTexture, sizeof(CI3DTexturePtr) * m_nRequiredTextures);
    ZeroMemory(m_nBytesPerPixel, sizeof(int) * m_nRequiredTextures);

    InitializeImage();
}

CImage::CImage(CLogWriter* pLog)
: m_nTexture(0),
  m_pLog(pLog)
{	
    m_AvailableVidMem = GetVidMemSize();
    m_nRequiredTextures = CalculateNumRequiredTexture();
    m_nTriangles = m_nRequiredTextures / 2;
    mVertices = new Vertex[m_nTriangles * 3];
    m_pTexture = new CI3DTexturePtr[m_nRequiredTextures];
    m_nBytesPerPixel = new int[m_nRequiredTextures];

    if(!mVertices || !m_pTexture || !m_nBytesPerPixel)
        throw std::exception("Failed to allocate memory");

    ZeroMemory(mVertices, sizeof(Vertex) * m_nTriangles * 3);
    ZeroMemory(m_pTexture, sizeof(CI3DTexturePtr) * m_nRequiredTextures);
    ZeroMemory(m_nBytesPerPixel, sizeof(int) * m_nRequiredTextures);

    InitializeImage();
}



LONGLONG CImage::GetVidMemSize()
{
    // Determine the size of total video memory
    CSystemInfo sysInfo;
    LONGLONG VidMemSize = 0;

    if(FAILED(sysInfo.GetAvailableVideoMemory(NULL, (DWORDLONG*)&VidMemSize, NULL, NULL, NULL)))
    {
        VidMemSize = 512 * 1024 * 1024;
    }

    if(Is32BitAddressSpace())
    {
        VidMemSize = min(VidMemSize, (LONGLONG)1.5 * 1024 * 1024 * 1024);
    }

    return VidMemSize;
}


UINT CImage::CalculateNumRequiredTexture()
{
    // This routine calculates number of textures needed to fill all of video memory
    LONGLONG VidMemSize = GetVidMemSize();
    
    // We will calculate number of required textures assuming 2BPP (since we don't know which format will be chosen)
    // For purpose of these tests it's ok if we end up allocating more then the total of all video memory
    const UINT TEX_SIZE = TEX_DIMENTIONS * TEX_DIMENTIONS * 2;
    UINT nRequiredTextures = (UINT)(VidMemSize / TEX_SIZE);

    // We use textures in pairs, so we want even number
    if(nRequiredTextures % 2)
    {
        nRequiredTextures++;
    }
    
    std::wstring str(L"VidMemSize = ");
    str += ToString<LONGLONG>(VidMemSize);
    str += L" RequiredTextures = ";
    str += ToString<UINT>(nRequiredTextures);
    
    if(m_pLog)
    {
        m_pLog->Message(str.c_str());
    }
        
    return nRequiredTextures;
}

/*
    Initialize an image
*/
void CImage::InitializeImage()
{
    UINT i,j;

    const float MaxWH = 480.0f;
    UINT Factor = 0;

    for(UINT i = 0;; i = i + 2)
    {
        UINT Temp = (UINT)pow(2.0f, (int)i);

        if(Temp >= m_nTriangles)
        {
            Factor = i/2;
            break;
        }
    }

    Factor = (UINT)pow(2.0f, (int)Factor);

    /*
       The vertices of a triangle
    */
    float yMax= MaxWH / Factor, yMin= 0.0f, xMin=0.0f, xMax= MaxWH / Factor;

    /*
       Initializing all the vertices of the triangle.
    */
    for(i=0,j=0;i<m_nTriangles;i++,j+=3)
    {
        assert(xMin <= xMax && xMax <= MaxWH);
        assert(yMin <= yMax && yMax <= MaxWH);

        mVertices[j].Pos[0]= xMin;
        mVertices[j].Pos[1]= yMax;
        mVertices[j].Pos[2]= 0.5f;
        mVertices[j].Pos[3]= 1.0f;
        mVertices[j].Color = 0xffff0000;
        mVertices[j].tu1 = 0.0f;
        mVertices[j].tv1 = 0.0f;
        mVertices[j].tu2 = 0.0f;
        mVertices[j].tv2 = 0.0f; // x, y, z, rhw, color, texture x, texture y;

        mVertices[j+1].Pos[0]= xMin + (MaxWH / (2*Factor));
        mVertices[j+1].Pos[1]= yMin;
        mVertices[j+1].Pos[2]= 0.5f;
        mVertices[j+1].Pos[3]= 1.0f;
        mVertices[j+1].Color = 0xff00ff00;
        mVertices[j+1].tu1 = 0.0f;
        mVertices[j+1].tv1 = 1.0f;
        mVertices[j+1].tu2 = 0.0f;
        mVertices[j+1].tv2 = 1.0f;

        mVertices[j+2].Pos[0]= xMax;
        mVertices[j+2].Pos[1]= yMax;
        mVertices[j+2].Pos[2]= 0.5f;
        mVertices[j+2].Pos[3]= 1.0f;
        mVertices[j+2].Color = 0xff00ffff;
        mVertices[j+2].tu1 = 1.0f;
        mVertices[j+2].tv1 = 0.0f;
        mVertices[j+2].tu2 = 1.0f;
        mVertices[j+2].tv2 = 0.0f;

        if(xMax < MaxWH)
        {
            xMin = xMax;
            xMax = xMax + (MaxWH / Factor);
        }
        else
        {
            xMin = 0.0f;
            xMax = MaxWH / Factor;

            if(yMax < MaxWH)
            {
                yMin = yMax;
                yMax = yMax + (MaxWH / Factor);
            }
        }

    };

    return;
}


/*
    Creates an Image.
 */
HRESULT CImage::CreateImage(CI3DDevice* pDevice, bool bFlush)
{   
    HRESULT hr = S_OK;

    /*
        Create needed textures
    */
    UINT nTriangles = 0;

    while(SUCCEEDED(hr) && (m_AvailableVidMem > 0) && (nTriangles < m_nTriangles))
    {
        hr = CreateTriangleTexture(pDevice, m_nTexture, bFlush);

        if(SUCCEEDED(hr))
        {
            m_nBytesPerPixel[m_nTexture] = CPixel::BytesPerPixel((m_pTexture[m_nTexture])->GetFormat());

            m_nBytesPerPixel[m_nTexture + 1] = CPixel::BytesPerPixel((m_pTexture[m_nTexture+1])->GetFormat());

            /*
               After each pair of textures are created determine amount of memory left.. 
               If there is any then create another pair..
            */
            
            m_AvailableVidMem -= 2048*2048*(m_nBytesPerPixel[m_nTexture]+m_nBytesPerPixel[m_nTexture + 1]);

            m_nTexture+=2;

            nTriangles++;
        }

    }

    m_nTriangles = nTriangles;
	
	if (m_AvailableVidMem <= 0)
    {
        hr = E_OUTOFMEMORY;
    }

    return hr;
}

/*
    Renders an Image.
 */
CI3DSurface2Ptr CImage::RenderImage(CI3DDevice* pDevice, bool bFlush)
{
    CI3DSurface2Ptr pBackBuffer;
    CI3DSwapChainPtr pSwapChain;
    CI3DShaderPtr pShader;

    H_CHECK( pDevice->SetStreamSourceUM(0, sizeof(Vertex), mVertices) );


    H_CHECK(I3DXSetDefaultVertexPipe(pDevice, FVF));
    H_CHECK(I3DXSet2DTexModDiffusePixelShader(pDevice));

    /*
    Create a pixel shader.
    */
    H_CHECK(I3DXCreateHLSLShader(PSCONST_SHADER,
                                "main",
                                PS_BEST_AVAILABLE,
                                pDevice,
                                &pShader.p));


    /*
    Set the pixel shader.
    */
    H_CHECK(pDevice->SetPixelShader(pShader));

   /*
      Clear the back buffer
    */
    H_CHECK( pDevice->Clear(0,
                            D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_COMPUTERECTS,
                            0.5f,
                            0,
                            0,
                            NULL) );


    H_CHECK( pDevice->GetSwapChain(&pSwapChain.p));

    /*
       Draw the triangles
     */
    for(UINT i=0; i<m_nTriangles; i++)
    {
        DrawTriangle(pDevice,i*2);

        if(true == bFlush)
            pDevice->Flush();
    }

    /*
        Get this backbuffer and return it.
     */
    H_CHECK(pSwapChain->GetCurrentBackBuffer(&pBackBuffer.p));

    return pBackBuffer;
}


/*
    Draw a triangle.
*/
void CImage::DrawTriangle(CI3DDevice* pDevice,int nTexture)
{
    assert(nTexture + 2 <= (int) m_nTexture);

    /*
        Set the two textures for a triangle.
     */
    H_CHECK( pDevice->SetTexture(0, m_pTexture[m_nTexture - (nTexture+1)]));

    H_CHECK( pDevice->SetTexture(1, m_pTexture[m_nTexture - (nTexture+2)]));

    /*
        Draw the triangle.
    */

    //call BeginScene() and EndScene() if necessary
    CI3DXSceneManager BeginEndScene(pDevice);
    H_CHECK( pDevice->DrawPrimitive(D3DPT_TRIANGLELIST,
                                   (nTexture/2)*3,
                                    1) );

#if DBG
    WCHAR str[1024] = L"";
    wsprintf(str, L"nTexture = %d, pTexture1 = 0x%X, pTexture2 = 0x%X, v1 = (%d, %d), v2 = (%d, %d), v3 = (%d, %d)\n",
             nTexture, 
             m_pTexture[m_nTexture - (nTexture+1)], 
             m_pTexture[m_nTexture - (nTexture+2)],
             (int)mVertices[(nTexture/2)*3].Pos[0], (int)mVertices[(nTexture/2)*3].Pos[1],
             (int)mVertices[1+(nTexture/2)*3].Pos[0], (int)mVertices[1+(nTexture/2)*3].Pos[1],
             (int)mVertices[2+(nTexture/2)*3].Pos[0], (int)mVertices[2+(nTexture/2)*3].Pos[1]);

    OutputDebugStringW(str);
#endif
                                        
    return;
}

/*
    Each triangle has two texture. Create them.
*/
HRESULT CImage::CreateTriangleTexture(CI3DDevice* pDevice, int nTexture, bool bFlush)
{
    HRESULT hr = S_OK;

    for(UINT i = 0; i < 2; i++)
    {
        hr = CreateTheTexture(pDevice,nTexture+i);

        if(FAILED(hr)) break;

        if(bFlush)
            pDevice->Flush();
    }
    
    return hr;
}

HRESULT CImage::CreateTheTexture(CI3DDevice* pDevice, int nTexture)
{
    HRESULT hr = S_OK;

    m_pTexture[nTexture].Release();

    D3DDDIFORMAT Fmt;
    HR_ERROR(I3DXGetValidResourceFormat(pDevice,
                                        I3DFORMAT_ARGB,
                                        I3DSURF_NOTLOCKABLE | I3DSURF_TEXTURE,
                                        false,
                                        &Fmt));

    hr = pDevice->CreateTexture(&m_pTexture[nTexture].p, 
                                TEX_DIMENTIONS,
                                TEX_DIMENTIONS,
                                1,
                                I3DSURF_NOTLOCKABLE,
                                Fmt,
                                D3DDDIPOOL_VIDEOMEMORY,
                                NULL);
 
    if(SUCCEEDED(hr))
    {
        SI3DXGradientInfo Info;

        Info.cUpperLeft = SColor(1.0f, 1.0f, 0.0f, 0.0f);//red
        Info.cUpperRight = SColor(1.0f, 0.0f, 1.0f, 0.0f);//green
        Info.cLowerLeft = SColor(1.0f, 0.0f, 0.0f, 1.0f);//blue
        Info.cLowerRight = SColor(1.0f, 1.0f, 0.0f, 1.0f);//purple


        hr = I3DXFillTexture(pDevice,
                             m_pTexture[nTexture],
                             I3DXFillGradient,
                             &Info);
    }

    return hr;
}


const char* PSCONST_SHADER ="                    \
sampler TextureOne : register(s0);               \
sampler TextureTwo : register(s1);               \
                                                 \
                                                 \
struct VS_OUTPUT                                 \
{                                                \
    float4   vDiffuse : COLOR0;                  \
    float2   vTex0 : TEXCOORD0;                  \
    float2   vTex1 : TEXCOORD1;                  \
};                                               \
                                                 \
float4 main(VS_OUTPUT v) : COLOR                 \
{                                                \
    float4 tex1 = tex2D(TextureOne, v.vTex0);    \
    float4 tex2 = tex2D(TextureTwo, v.vTex1);    \
    return tex1 + tex2;                          \
}";
