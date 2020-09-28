#pragma once

#include "D3D11Test.h"

interface ID2D1RenderTarget;
interface ID2D1SolidColorBrush;

class CD2DTileableCopyTest : public CD3D11Test
{
public:
	CD2DTileableCopyTest();
	~CD2DTileableCopyTest();

	virtual void			InitTestParameters();
	virtual TEST_RESULT		Setup();
	virtual void			Cleanup();

	virtual TEST_RESULT		SetupTestCase();
	virtual TEST_RESULT		ExecuteTestCase();
	virtual void			CleanupTestCase();

private:
	HRESULT ReadBackOnePixel();
	HRESULT RenderWithLayers(float opacity);
	HRESULT RenderWithCompatibleRenderTarget(float opacity);
	void RenderOneLayer(ID2D1RenderTarget *pRenderTarget, UINT nestingLevel);
	HRESULT CompareResults();
	void FillBackground();

	D3DTEST_INTERFACE_LEVEL m_savedOrigInterfaceLevel;
	bool m_savedBgraSupport;

	bool m_bReadback;
	UINT m_uNestingDepth;

	ID2D1RenderTarget       *m_pD2DRenderTarget;
	ID2D1SolidColorBrush    *m_pBrush;
	ID3D11Texture2D         *m_pTargetTexture;
	ID3D11Texture2D         *m_pResults[2];
};