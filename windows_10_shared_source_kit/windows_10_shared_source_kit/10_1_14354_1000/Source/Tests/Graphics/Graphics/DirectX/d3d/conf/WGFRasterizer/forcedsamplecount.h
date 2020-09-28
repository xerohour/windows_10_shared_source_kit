//////////////////////////////////////////////////////////////////////////
//  forcedsamplecount.h
//  created:	2011/01/11
//
//  purpose: test TIR's forced sample count feature - part of D3D11.1
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "rasterizer.h"

// Enums
enum COVERED_SAMPLES_REPRESENTATION
{
	COVERED_SAMPLES_REPRESENTATION_FORCED_SAMPLE_COUNT_PATTERN,		// The covered samples representation is based on the forced sample count pattern
	COVERED_SAMPLES_REPRESENTATION_RENDER_TARGET_SAMPLE_PATTERN,	// The covered samples representation is based on the render targets multisample pattern
};

enum OUTPUT_TYPE
{
    OUTPUT_TYPE_RTV,
    OUTPUT_TYPE_UAV
};

// Named value sets
DECLARE_NAMED_VALUES( COVERED_SAMPLES_REPRESENTATION );
DECLARE_NAMED_VALUES( OUTPUT_TYPE );

class CForcedSampleCountTest : public CRasterizerTest
{
public:
	CForcedSampleCountTest();
	void InitTestParameters();

protected:
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	void Cleanup();

	// Helper functions
	bool CreateShaders();											// Create the shaders used in the test
	bool SetupVB();													// Fill the vertex buffer with verticies based on the current test case
	bool CoverSubpixel( const UINT &pixelX, const UINT &pixelY,		// Adds a triangle to the vertex buffer covering only the specified sample location in the pixel
		const float &sampleOffsetX, const float &sampleOffsetY );	

protected:
	struct VERTEX
	{
		float pos[2];
		float tex[2];
	};

private:
	// Test factors
    OUTPUT_TYPE m_OutputType;
	UINT m_uCoveredSamples;
	UINT m_uSampleMask;
	COVERED_SAMPLES_REPRESENTATION m_coveredSamplesRepresentation;
	D3D11_STANDARD_MULTISAMPLE_QUALITY_LEVELS m_qualityLevel;

	UINT m_uVertexCount;
	D3D11_VIEWPORT m_Viewport;
	ID3D11DepthStencilState *m_pDepthStateDisabled;
	ID3D11Query *m_pPipelineQuery;
	ID3D11Texture2D *m_pRTTexture2DStaging;
	ID3D11Texture2D *m_pRTTexture2DSS;
	ID3D11Texture2D *m_pRTTexture2DSSStaging;
	ID3D11ShaderResourceView *m_pSRView;
	ID3D11RenderTargetView *m_pSSRTView;
	ID3D11UnorderedAccessView* m_pUAV;

    ID3D11PixelShader		*m_pUAVPS;

	ID3D11PixelShader *m_pPSMSToSS;
	bool m_bRenderedGeometryAtThePixelCenter;
};