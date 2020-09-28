#pragma once

#include <vector>
#include <list>
#include <string>
#include <ISimpleLog.h>

#include "D3D11Test.h"

class CPresentHelper
{
public:

    CPresentHelper(); //Initialize shaders
    ~CPresentHelper();


    bool Setup(ID3D11RenderTargetView *pView, IDXGISwapChain *pSwapChain, ISimpleLog *pLogger);
    bool PresentTexture2D(ID3D11Texture2D *pTexture2D, ID3D11ShaderResourceView *pTextureView = NULL);
    bool PresentBuffer(ID3D11Buffer* pBuffer);
    HRESULT Create2DTexture(ID3D11Texture2D** ppTex);

protected:
    ID3D11Device				*m_pDevice;
    ID3D11DeviceContext			*m_pDeviceContext;
    IDXGISwapChain              *m_pSwapChain;
    
    ID3D11Buffer            *m_pVB;
    ID3D11VertexShader      *m_pVS;
    ID3D11PixelShader       *m_pPS;
    ID3D11SamplerState      *m_pSampler;
    ID3D11RasterizerState   *m_pRasterizer;
    ID3D11RenderTargetView  *m_pRTView;
    ID3D11InputLayout       *m_pIL;
    ISimpleLog              *m_pLogger;
    ULONG                   *m_pTextureData;

};
