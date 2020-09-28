//////////////////////////////////////////////////////////////////////////
//  Coverage.h
//  created:	2005/07/18
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "multisample.h"

enum SAMPLE_MASK_PATTERN
{
	MASK_ALL_ON,
	MASK_ALL_OFF,
	MASK_LOWER_HALF_ON_HIGHER_HALF_OFF,
	MASK_LOWER_HALF_OFF_HIGHER_HALF_ON,
	MASK_PATTERN_OFF_OFF_OFF_ON,
	MASK_PATTERN_ON_ON_ON_OFF
};
DECLARE_NAMED_VALUES( SAMPLE_MASK_PATTERN );
	
// Test for alpha to coverage
// and SV_COVERAGE system interpreted value
class CCoverageTest : public CMultisampleTest
{
public:
	CCoverageTest();
	void InitTestParameters();

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	void ValidateA2C();
	void ValidateSVC();
	void CleanupTestCase();

	ID3D11Texture2D *m_pTexture;
	ID3D11ShaderResourceView *m_pSRV;
	ID3D11PixelShader *m_pPSMask;
	std::vector<UINT> m_vCoverage;

	float m_fAlpha;
	bool m_bSVCoverage;
	SHADER_FREQUENCY m_sf;
	UINT m_uMask;
	SAMPLE_MASK_PATTERN m_MaskPattern;
};

// Test for SV_COVERAGE system generated value
class CInputCoverageTest : public CMultisampleTest
{
public:
	CInputCoverageTest();
	void InitTestParameters();

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();
	void Validate();

	SHADER_FREQUENCY m_sf;
	UINT m_TargetPixelX;
	UINT m_TargetPixelY;
	UINT m_BlendStateSampleMask;

	ID3D11PixelShader* m_pCoveragePS;
};

// Test for SV_COVERAGE system generated value interacting with the depth test
class CInputCoverageWithDepthTest : public CMultisampleTest
{
public:
	CInputCoverageWithDepthTest();
	void InitTestParameters();

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();

	UINT ReadCoverageValue();
	void FillDepthBuffer();

	SHADER_FREQUENCY m_sf;
	UINT m_TargetPixelX;
	UINT m_TargetPixelY;
	UINT m_DepthMask;
	DXGI_FORMAT m_DepthBufferFormat;

	ID3D11PixelShader*       m_pCoveragePS;
	ID3D11Texture2D*         m_pDepthBuffer;
	ID3D11DepthStencilView*  m_pDSV;
	ID3D11DepthStencilState* m_pDepthStencilState;
};
	