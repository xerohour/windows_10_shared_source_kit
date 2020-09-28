//////////////////////////////////////////////////////////////////////////
//  Interpolator.h
//  created:	2006/09/20
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "rasterizer.h"
	
enum D3D_INTERPOLATOR {
	CONSTANT = 0,
	LINEAR, 
	LINEAR_CENTROID,
	LINEAR_NOPERSPECTIVE,
	LINEAR_NOPERSPECTIVE_CENTROID,
	LINEAR_SAMPLE,
	LINEAR_NOPERSPECTIVE_SAMPLE,
};

enum MODE_TYPE {
	MODE_FLOAT = 0,
	MODE_UINT,
	MODE_INT
};

class CInterpolator : public CRasterizerTest
{
public:
	CInterpolator();
	void InitTestParameters();
	
	TEST_RESULT Setup() { return RESULT_PASS; }
	void Cleanup() {}

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();
	void CreateShaders();
	
	void VerifyConstant(const std::vector<UINT> &);
	void VerifyLinear(const std::vector<UINT> &);

	float GetFloat(const std::vector<UINT> &, UINT index);
	UINT  GetUint(const std::vector<UINT> &, UINT index);
	int   GetInt(const std::vector<UINT> &, UINT index);

	UINT GetLeadingVertex(UINT x, UINT y);
	void WriteVBToLog();

	D3D_INTERPOLATOR m_Mode;
	MODE_TYPE		 m_Type;
	UINT	         m_uSize;
	float			 m_pData[4];
};

class CSampleFreq : public CD3D11Test
{
public:
	CSampleFreq();

	virtual void InitTestParameters();

	virtual TEST_RESULT Setup();
	virtual void Cleanup();

	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
	virtual void CleanupTestCase();

private:
	D3D_INTERPOLATOR                          m_Mode;
	UINT                                      m_uRTSampleCount;
	ID3D11Texture2D*                          m_pRenderTarget;
	ID3D11RenderTargetView*                   m_pRTV;
	ID3D11Buffer*                             m_pCB;
	ID3D11VertexShader*                       m_pVS;
	ID3D11PixelShader*                        m_pPS;
	ID3D11InputLayout*                        m_pIL;
	ID3D11Buffer*                             m_pVB;
	ID3D11RasterizerState*                    m_pRastState;
};