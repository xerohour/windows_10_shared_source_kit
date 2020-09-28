#pragma once

#include <vector>
#include <list>
#include <string>

class CD3DTest;

class CDevice;
class CPixelShader;
class CRasterizerState;
class CRenderTargetView;
class CShaderResourceView;
class CSamplerState;
class CTexture2D;
class CVertexShader;

class CPresentHelper
{
public:

	CPresentHelper(); //Initialize shaders
	~CPresentHelper();


	bool Setup(CD3DTest *pTest);
	void PresentTexture2D(CTexture2D *pTexture2D, CShaderResourceView *pTextureView = NULL);

protected:
	CD3DTest			*m_pTest;

	CDevice				*m_pDevice;

	CVertexShader		*m_pVS;
	CPixelShader		*m_pPS;
	CSamplerState		*m_pSampler;
	CRasterizerState	*m_pRasterizer;
	CRenderTargetView	*m_pRTView;

};
