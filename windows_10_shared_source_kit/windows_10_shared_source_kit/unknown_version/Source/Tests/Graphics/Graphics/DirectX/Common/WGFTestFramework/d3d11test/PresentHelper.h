#pragma once

#include <vector>
#include <list>
#include <string>

#include "D3D11Test.h"

class CPresentHelper
{
public:

    CPresentHelper(); //Initialize shaders
    ~CPresentHelper();


    bool Setup(CD3D11Test *pTest);
    bool PresentTexture2D(ID3D11Texture2D *pTexture2D, ID3D11ShaderResourceView *pTextureView = NULL);
    bool PresentBuffer(ID3D11Buffer* pBuffer);
    HRESULT Create2DTexture(ID3D11Texture2D** ppTex);

protected:
    CD3D11Test			*m_pTest;

    ID3D11Device				*m_pDevice;
    ID3D11DeviceContext			*m_pDeviceContext;

    ID3D11Buffer            *m_pVB;
    ID3D11VertexShader      *m_pVS;
    ID3D11PixelShader       *m_pPS;
    ID3D11SamplerState      *m_pSampler;
    ID3D11RasterizerState   *m_pRasterizer;
    ID3D11RenderTargetView  *m_pRTView;
    ID3D11InputLayout       *m_pIL;
    ULONG                   *m_pTextureData;

};
