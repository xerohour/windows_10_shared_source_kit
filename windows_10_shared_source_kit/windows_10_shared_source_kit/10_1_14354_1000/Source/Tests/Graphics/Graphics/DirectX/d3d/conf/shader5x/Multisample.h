//////////////////////////////////////////////////////////////////////////
//  Move.h
//  created:	2005/03/10
//
//  purpose:
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"
#include <D3DSampleLocation.h>
#include "new_off.h"
#include "d3dx10math.h"
#include "new_on.h"

class CMSLoadTest : public CShader5xTest
{
public:
	CMSLoadTest();
	void		InitTestParameters();

	virtual TEST_RESULT Setup();
	TEST_RESULT	SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void		CleanupTestCase();
	void		Cleanup();
	virtual bool UseResidencyFeedback() const
	{
		return false;
	}

private:
	void SetupShaders();
	void SetupTextures();
	void FillBuffer(std::vector<D3DXVECTOR4> &vData);
	void GetResults(std::vector<D3DXVECTOR4> &vData, ID3D11Resource *pRes);
	void SetSlice(UINT uSlice);

	D3D11_TEXTURE2D_DESC			m_Tex2DDesc;
	D3D11_RENDER_TARGET_VIEW_DESC	m_RTVDesc;
	UINT							m_uArraySlice;
	UINT							m_uSrcAddress[4];
	UINT							m_uSampleIndex;
	bool							m_bDeclCount;
	bool							m_bLiteralIndex;

	// textures
	ID3D11Texture2D				*m_pMSRT;
	ID3D11Texture2D				*m_pRT;
	ID3D11Texture2D				*m_pRTStaging;
	ID3D11Texture2D				*m_pTex2DIndex;
	ID3D11RenderTargetView		*m_pMSRTV;
	ID3D11RenderTargetView		*m_pRTV;
	ID3D11ShaderResourceView	*m_pSRV;
	ID3D11ShaderResourceView	*m_pSRVIndex;

	ID3D11PixelShader		*m_pPSPass;
	ID3D11GeometryShader	*m_pGS;
	ID3D11Buffer			*m_pVBSamples;
	ID3D11Buffer			*m_pVBQuad;
	ID3D11InputLayout		*m_pLayout;
};

class CMSLoadTestResidencyFeedback : public CMSLoadTest
{
	TEST_RESULT Setup();
	bool UseResidencyFeedback() const
	{
		return true;
	}
};

//////////////////////////////////////////////////////////////////////////
// Multisample Instructions (10.1)
//////////////////////////////////////////////////////////////////////////

class CMultisampleTest : public CShader5xTest
{
public:
	CMultisampleTest();
	void		InitTestParameters();
protected:
	virtual TEST_RESULT Setup();
	virtual	TEST_RESULT	SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase() = 0;
	virtual void CleanupTestCase();
	virtual void Cleanup();
	void SetupShaders();
	virtual void SetupShaderStrings(tstring&, tstring&, tstring&) = 0;

	D3D11_TEXTURE2D_DESC			m_Tex2DDesc;
	D3D11_RENDER_TARGET_VIEW_DESC	m_RTVDesc;
	UINT						m_uArraySlice;

	ID3D11Texture2D				*m_pTex2DBackup;
	ID3D11RenderTargetView		*m_pRTVBackup;
	ID3D11Buffer					*m_pVBQuad;
	ID3D11InputLayout			*m_pLayout;
	ID3D11ShaderResourceView		*m_pSRView;

};

class CSampleInfoTest : public CMultisampleTest
{
public:
	CSampleInfoTest();
	void		InitTestParameters();
private:
	TEST_RESULT ExecuteTestCase();
	void SetupShaderStrings(tstring &szVS, tstring &szGS, tstring &szPS);

	// sampleinfo_uint
	bool m_bInfoInt;
	bool m_bRasterizer;
	bool m_bBindResource;
};

class CSamplePosTest : public CMultisampleTest
{
public:
	CSamplePosTest();
	void		InitTestParameters();
private:
	TEST_RESULT ExecuteTestCase();
	void SetupShaderStrings(tstring &szVS, tstring &szGS, tstring &szPS);

	std::vector<POINT> m_vSamplePos;
	UINT m_uSampleIndex;
	bool m_bRasterizer;
	bool m_bSRVBound;
};

class CSampleIndexTest : public CMultisampleTest
{
public:
	CSampleIndexTest();
	void		InitTestParameters();
private:
	TEST_RESULT ExecuteTestCase();
	void SetupShaderStrings(tstring &szVS, tstring &szGS, tstring &szPS);

};