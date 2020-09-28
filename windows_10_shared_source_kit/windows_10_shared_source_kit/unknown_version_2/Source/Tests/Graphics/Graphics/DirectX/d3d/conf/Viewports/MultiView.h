#pragma once
#include "ViewPorts.h"

class MultiView : public ViewportsTest
{
public:
	void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();
	virtual TEST_RESULT ExecuteTestCase();
};

class ArbitraryView : public MultiView
{
public:
	ArbitraryView() :
		m_pTextureRT_MS(NULL),
		m_pRTV_MS(NULL),
		m_pRast_MS(NULL)
	{
	}
	
	void InitTestParameters();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();
	TEST_RESULT VerifyMultisample();
	TEST_RESULT ExecuteTestCase();

private:
	UINT m_ActiveViewport;
	UINT m_NumSamples;

	ID3D11Texture2D *m_pTextureRT_MS;
	ID3D11RenderTargetView *m_pRTV_MS;
	ID3D11RasterizerState *m_pRast_MS;
};
