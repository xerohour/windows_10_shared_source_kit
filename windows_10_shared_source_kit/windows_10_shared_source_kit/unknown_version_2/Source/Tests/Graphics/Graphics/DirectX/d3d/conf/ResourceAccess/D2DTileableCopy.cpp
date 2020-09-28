//
// A test for TILEABLE_COPY that leverages D2D.  
// D2D uses TILEABLE_COPY in a very specific call-pattern that is optimized by some drivers.
// The purpose of this test is to ensure that drivers correct implement optimized and non-optimized paths.
// This ensures that all D2D usage of this feature will work correctly.
//

#include "D2DTileableCopy.h"
#include "d2d1.h"

CD2DTileableCopyTest::CD2DTileableCopyTest() 
	: m_savedOrigInterfaceLevel(D3DTEST_INTERFACE_LEVEL_11_1)
	, m_savedBgraSupport(false) 
	, m_bReadback(false)
	, m_uNestingDepth(0)
	, m_pD2DRenderTarget(NULL)
	, m_pBrush(NULL)
	, m_pTargetTexture(NULL)
{    
	ZeroMemory(m_pResults, sizeof(m_pResults));

	// D2D requires a specific D3D device configuration
	// Set these values to require a new D3D device to be created
	m_savedOrigInterfaceLevel = GetFramework()->m_D3DOptions.origInterfaceLevel;
	m_savedBgraSupport = GetFramework()->m_D3DOptions.bgraSupport;

	GetFramework()->m_D3DOptions.bgraSupport = true; // D2D requires BGRA support
	GetFramework()->m_D3DOptions.origInterfaceLevel = D3DTEST_INTERFACE_LEVEL_11_0; // D3D does not work with the D3D10.0 API
}

CD2DTileableCopyTest::~CD2DTileableCopyTest()
{
	// Restore settings saved in the constructor
	GetFramework()->m_D3DOptions.bgraSupport = m_savedBgraSupport;
	GetFramework()->m_D3DOptions.origInterfaceLevel = m_savedOrigInterfaceLevel;
}

void CD2DTileableCopyTest::InitTestParameters()
{
	CTestCaseParameter<bool> *pReadBack = AddParameter( _T( "Readback" ), &m_bReadback );
	testfactor::RFactor rfReabackFalse = AddParameterValue( pReadBack, false );
	testfactor::RFactor rfReabackTrue = AddParameterValue( pReadBack, true );
	testfactor::RFactor rfReadback = rfReabackFalse + rfReabackTrue;

	CTestCaseParameter<UINT> *pNestingDepth = AddParameter( _T("NestingDepth"), &m_uNestingDepth );
	CUserValueSet<UINT> *pNestingDepthValues = new CUserValueSet<UINT>();
	pNestingDepthValues->AddValue(1);
	pNestingDepthValues->AddValue(2);
	pNestingDepthValues->AddValue(3);
	testfactor::RFactor rfNestingDepth = AddParameterValueSet( pNestingDepth, pNestingDepthValues );

	SetRootTestFactor( rfNestingDepth * rfReadback );
}

TEST_RESULT	CD2DTileableCopyTest::Setup()
{
	return RESULT_PASS;
}

void CD2DTileableCopyTest::Cleanup()
{
}

TEST_RESULT CD2DTileableCopyTest::SetupTestCase()
{
	ID2D1Factory *pD2DFactory = NULL;
	IDXGISurface *pDxgiSurface = NULL;

	TEST_RESULT tr = CD3D11Test::SetupTestCase();
	HRESULT hr = S_OK;

	if (tr == RESULT_PASS)
	{
		HRESULT hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_MULTI_THREADED, &pD2DFactory);
		if (FAILED(hr))
		{
			WriteToLog( _T("D2D1CreateFactory failed hr = 0x%x" ), hr );
			tr = RESULT_FAIL;
		}
	}

	// Create render-target texture
	D3D11_TEXTURE2D_DESC texDesc = 
	{
		// large enough to cover multiple tiles on a tile-based rendered
		1024,
		1024,
		1,
		1,
		DXGI_FORMAT_B8G8R8A8_UNORM,
		{ 1, 0 },
		D3D11_USAGE_DEFAULT,
		D3D11_BIND_RENDER_TARGET,
		0,
		0
	};

	if (tr == RESULT_PASS)
	{
		hr = GetDevice()->CreateTexture2D(&texDesc, NULL,  &m_pTargetTexture);
		if (FAILED(hr))
		{
			WriteToLog( _T("CreateTexture2D (default_ failed hr = 0x%x" ), hr );
			tr = RESULT_FAIL;
		}
	}

	// Create staging textures (which will hold rendered result)
	texDesc.Usage = D3D11_USAGE_STAGING;
	texDesc.BindFlags = 0;
	texDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

	for (UINT i = 0; i < ARRAYSIZE(m_pResults); i++)
	{
		if (tr == RESULT_PASS)
		{
			hr = GetDevice()->CreateTexture2D(&texDesc, NULL, &m_pResults[i]);   

			if (FAILED(hr))
			{
				WriteToLog( _T("CreateTexture2D (staging) failed hr = 0x%x" ), hr );
				tr = RESULT_FAIL;
			}
		}
	}

	if (tr == RESULT_PASS)
	{
		hr = m_pTargetTexture->QueryInterface(&pDxgiSurface);

		if (FAILED(hr))
		{
			WriteToLog( _T("QueryInterface (IDXGISurface) failed hr = 0x%x" ), hr );
			tr = RESULT_FAIL;
		}
	}

	if (tr == RESULT_PASS)
	{
		D2D1_RENDER_TARGET_PROPERTIES renderTargetProperties = 
		{
			D2D1_RENDER_TARGET_TYPE_HARDWARE,
			{ DXGI_FORMAT_B8G8R8A8_UNORM, D2D1_ALPHA_MODE_PREMULTIPLIED },
			96.0f,
			96.0f,
			D2D1_RENDER_TARGET_USAGE_NONE,
			D2D1_FEATURE_LEVEL_DEFAULT
		};

		hr = pD2DFactory->CreateDxgiSurfaceRenderTarget(
			pDxgiSurface,
			&renderTargetProperties,
			&m_pD2DRenderTarget
			);

		if (FAILED(hr))
		{
			WriteToLog( _T("CreateDxgiSurfaceRenderTarget failed hr = 0x%x" ), hr );
			tr = RESULT_FAIL;
		}
	}

	if (tr == RESULT_PASS)
	{
		hr = m_pD2DRenderTarget->CreateSolidColorBrush(D2D1::ColorF(0), &m_pBrush);
		if (FAILED(hr))
		{
			WriteToLog( _T("CreateSolidColorBrush failed hr = 0x%x" ), hr );
			tr = RESULT_FAIL;
		}
	}

	SAFE_RELEASE(pD2DFactory);
	SAFE_RELEASE(pDxgiSurface);

	return tr;
}

void CD2DTileableCopyTest::CleanupTestCase()
{
	SAFE_RELEASE(m_pD2DRenderTarget);
	SAFE_RELEASE(m_pBrush);
	SAFE_RELEASE(m_pTargetTexture);

	for (UINT i = 0; i < ARRAYSIZE(m_pResults); i++)
	{
		SAFE_RELEASE(m_pResults[i]);
	}

	CD3D11Test::CleanupTestCase();
}

// Uses pRenderTarget to render to a single layer
// Each layer consists of 2 primitives to ensure that this test
// won't be broken by future D2D optimizations that optimize a layer that contains a single primitive
void CD2DTileableCopyTest::RenderOneLayer(ID2D1RenderTarget *pRenderTarget, UINT nestingLevel)
{
	const D2D1_COLOR_F colors[] = 
	{
		D2D1::ColorF(0x0000ff, 1.0f),
		D2D1::ColorF(0x00ff00, 1.0f),
		D2D1::ColorF(0xff0000, 1.0f),
	};

	D2D1_COLOR_F color = colors[nestingLevel % ARRAYSIZE(colors)];

	switch (nestingLevel % 3)
	{
	case 0:
		{
			D2D1_ELLIPSE ellipse = D2D1::Ellipse(D2D1::Point2F(100.0f, 200.0f), 30.0f, 80.0f);

			m_pBrush->SetColor(color);
			pRenderTarget->FillEllipse(ellipse,m_pBrush);

			m_pBrush->SetColor(D2D1::ColorF(0, 1.0f));
			pRenderTarget->DrawEllipse(ellipse,m_pBrush);
		}
		break;

	case 1:
		{
			D2D1_RECT_F rect = D2D1::RectF(30.0f, 200.0f, 800.0f, 400.0f);

			m_pBrush->SetColor(color);
			pRenderTarget->FillRectangle(rect, m_pBrush);

			m_pBrush->SetColor(D2D1::ColorF(0, 1.0f));
			pRenderTarget->DrawRectangle(rect, m_pBrush);
		}
		break;

	case 2:
		{
			D2D1_ROUNDED_RECT rect = D2D1::RoundedRect(D2D1::RectF(90.0f, 350.0f, 600.0f, 805.0f), 100.0f, 200.0f);

			m_pBrush->SetColor(color);
			pRenderTarget->FillRoundedRectangle(rect, m_pBrush);

			m_pBrush->SetColor(D2D1::ColorF(0, 1.0f));
			pRenderTarget->DrawRoundedRectangle(rect, m_pBrush);
		}
		break;
	}
}

// Force a mid-scene flush on a tile-based renderer
// by reading back 1 pixel from the render target
HRESULT CD2DTileableCopyTest::ReadBackOnePixel()
{
	HRESULT hr = S_OK;
	ID3D11Texture2D *pStaging = NULL;

	// First ensure all D2D operations have completed
	hr = m_pD2DRenderTarget->Flush();
	if (FAILED(hr))
	{
		WriteToLog( _T("ID2D1RenderTarget::Flush failed: hr = 0x%x" ), hr );
	}

	if (SUCCEEDED(hr))
	{
		D3D11_TEXTURE2D_DESC desc;
		m_pTargetTexture->GetDesc(&desc);

		desc.Usage = D3D11_USAGE_STAGING;
		desc.BindFlags = 0;
		desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;

		hr = GetDevice()->CreateTexture2D(&desc, NULL, &pStaging);
		if (FAILED(hr))
		{
			WriteToLog( _T("Failed to create staging texture: hr = 0x%x" ), hr );
		}
	}

	if (SUCCEEDED(hr))
	{
		GetImmediateContext()->CopyResource(pStaging, m_pTargetTexture);

		D3D11_MAPPED_SUBRESOURCE mappedSubResource;
		hr = GetImmediateContext()->Map(pStaging, 0, D3D11_MAP_READ, 0, &mappedSubResource);
		if (FAILED(hr))
		{
			WriteToLog( _T("Failed to map staging texture: hr = 0x%x" ), hr );
		}
	}

	if (SUCCEEDED(hr))
	{
		GetImmediateContext()->Unmap(pStaging, 0);
	}

	SAFE_RELEASE(pStaging);
	return hr;
}

// Clears m_pD2DRenderTarget and then fills in a grid
void CD2DTileableCopyTest::FillBackground()
{
	m_pD2DRenderTarget->Clear(D2D1::ColorF(0xffffffff));

	m_pBrush->SetColor(D2D1::ColorF(0, 1.0f));

	D2D1_SIZE_F rtSize = m_pD2DRenderTarget->GetSize();

	for (float y = 0; y < rtSize.height; y += 57.0f)
	{
		m_pD2DRenderTarget->DrawLine(
			D2D1::Point2F(0.0f, y),
			D2D1::Point2F(rtSize.width, y),
			m_pBrush
			);
	}
}

HRESULT CD2DTileableCopyTest::RenderWithLayers(float opacity)
{
	HRESULT hr = S_OK;

	m_pD2DRenderTarget->BeginDraw();

	FillBackground();

	for (UINT nestingLevel = 0; nestingLevel < m_uNestingDepth; nestingLevel++)
	{
		D2D1_LAYER_PARAMETERS layerParameters = D2D1::LayerParameters();
		layerParameters.opacity = opacity;

		m_pD2DRenderTarget->PushLayer(&layerParameters, NULL);

		if (m_bReadback)
		{
			// Read back 1 pixel with D3D
			if (SUCCEEDED(hr))
			{
				hr = ReadBackOnePixel();
			}
		}

		RenderOneLayer(m_pD2DRenderTarget, nestingLevel);
	}

	for (UINT nestingLevel = 0; nestingLevel < m_uNestingDepth; nestingLevel++)
	{
		m_pD2DRenderTarget->PopLayer();
	}

	// Don't overwrite failing hr with succeeded result from EndDraw()
	// But always do call EndDraw(), even if a failure was encountered earlier
	HRESULT hr2 = m_pD2DRenderTarget->EndDraw();
	if (FAILED(hr2))
	{
		WriteToLog( _T("ID2D1RenderTarget::EndDraw failed: hr = 0x%x" ), hr2 );
		hr = hr2;
	}

	return hr;
}

HRESULT CD2DTileableCopyTest::RenderWithCompatibleRenderTarget(float opacity)
{
	HRESULT hr = S_OK;

	// There is a hard-coded maximum nesting depth
	ID2D1BitmapRenderTarget *pBitmapRenderTargets[3];
	ZeroMemory(pBitmapRenderTargets, sizeof(ID2D1BitmapRenderTarget *) * ARRAYSIZE(pBitmapRenderTargets));

	if (m_uNestingDepth > ARRAYSIZE(pBitmapRenderTargets))
	{
		WriteToLog( _T("Nesting depth too deep") );
		return E_FAIL;
	}

	// Create all compatible render targets that will be needed
	for (UINT nestingLevel = 0; nestingLevel < m_uNestingDepth; nestingLevel++)
	{
		if (SUCCEEDED(hr))
		{
			hr = m_pD2DRenderTarget->CreateCompatibleRenderTarget(
				NULL,
				NULL,
				NULL,
				D2D1_COMPATIBLE_RENDER_TARGET_OPTIONS_NONE,
				&pBitmapRenderTargets[nestingLevel]
			);

			if (FAILED(hr))
			{
				WriteToLog( _T("ID2D1RenderTarget::CreateCompatibleRenderTarget failed: hr = 0x%x" ), hr );
			}
		}
	}

	// Render to each layer
	for (UINT nestingLevel = 0; nestingLevel < m_uNestingDepth; nestingLevel++)
	{
		pBitmapRenderTargets[nestingLevel]->BeginDraw();

		pBitmapRenderTargets[nestingLevel]->Clear(D2D1::ColorF(0, 0.0f));

		if (SUCCEEDED(hr))
		{
			RenderOneLayer(pBitmapRenderTargets[nestingLevel], nestingLevel);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBitmapRenderTargets[nestingLevel]->EndDraw();
			if (FAILED(hr))
			{
				WriteToLog( _T("ID2D1BitmapRenderTarget::EndDraw failed: hr = 0x%x" ), hr );
			}
		}
	}

	// Compose all layers together (last to first)
	for (UINT nestingLevel = 0; nestingLevel < m_uNestingDepth; nestingLevel++)
	{
		ID2D1Bitmap *pSrcBitmap = NULL;
		if (SUCCEEDED(hr))
		{
			UINT srcIndex = m_uNestingDepth - nestingLevel - 1;

			hr = pBitmapRenderTargets[srcIndex]->GetBitmap(&pSrcBitmap);
			if (FAILED(hr))
			{
				WriteToLog( _T("ID2D1BitmapRenderTarget::GetBitmap failed: hr = 0x%x" ), hr );
			}
		}

		ID2D1RenderTarget *pDstRenderTarget = NULL;
		if (nestingLevel == (m_uNestingDepth - 1))
		{
			pDstRenderTarget = m_pD2DRenderTarget;
		}
		else
		{
			pDstRenderTarget = pBitmapRenderTargets[m_uNestingDepth - nestingLevel - 2];
		}

		if (SUCCEEDED(hr))
		{
			pDstRenderTarget->BeginDraw();

			// If this is the last iteration (writing to the final render target
			// then clear to opaque white first
			if (pDstRenderTarget == m_pD2DRenderTarget)
			{
				FillBackground();
			}

			pDstRenderTarget->DrawBitmap(pSrcBitmap, NULL, opacity);

			hr = pDstRenderTarget->EndDraw();
			if (FAILED(hr))
			{
				WriteToLog( _T("ID2D1RenderTarget::EndDraw failed: hr = 0x%x" ), hr );
			}
		}

		SAFE_RELEASE(pSrcBitmap);
	}

	for (UINT nestingLevel = 0; nestingLevel < m_uNestingDepth; nestingLevel++)
	{
		SAFE_RELEASE(pBitmapRenderTargets[nestingLevel]);
	}

	return hr;
}

// Compares 2 pixels.  Some tolerance is allowed because
// D2D layers use slightly different math than the compatible render target path.
static bool PixelsAreDifferent(DWORD pixel0, DWORD pixel1)
{
	// for each channel
	const BYTE *pPixel0 = reinterpret_cast<const BYTE *>(&pixel0);
	const BYTE *pPixel1 = reinterpret_cast<const BYTE *>(&pixel1);

	for (UINT c = 0; c < 4; c++)
	{
		int channel0 = pPixel0[c];
		int channel1 = pPixel1[c];

		int diff = abs(channel0 - channel1);

		if (diff > 4)
		{
			return true;
		}
	}

	return false;
}

// Reads 1 pixel from a mapped subresource
static DWORD ReadPixel(D3D11_MAPPED_SUBRESOURCE mappedSubresource, UINT x, UINT y)
{
	const BYTE *pScanLine = static_cast<const BYTE *>(mappedSubresource.pData) + (mappedSubresource.RowPitch * y);

	return reinterpret_cast<const DWORD *>(pScanLine)[x];
}

// Compares the contents of m_pResults[0] and m_pResults[1]
HRESULT CD2DTileableCopyTest::CompareResults()
{
	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mappedSubResource[2];
	bool mapped[2] = { false, false };

	for (UINT i = 0; i < 2; i++)
	{
		if (SUCCEEDED(hr))
		{
			hr = GetImmediateContext()->Map(m_pResults[i], 0, D3D11_MAP_READ, 0, &mappedSubResource[i]);
			if (SUCCEEDED(hr))
			{
				mapped[i] = true;
			}
			else
			{
				WriteToLog( _T("Failed to map staging texture: hr = 0x%x" ), hr );
			}
		}
	}

	D3D11_TEXTURE2D_DESC desc;
	m_pTargetTexture->GetDesc(&desc);

	for (UINT y = 0; y < desc.Height; y++)
	{
		for (UINT x = 0; x < desc.Width; x++)
		{
			if (SUCCEEDED(hr))
			{
				DWORD pixels[2];

				for (UINT i = 0; i < 2; i++)
				{
					pixels[i] = ReadPixel(mappedSubResource[i], x, y);
				}

				if (PixelsAreDifferent(pixels[0], pixels[1]))
				{
					WriteToLog( _T("Wrong pixel values found.  (x,y) = (%u, %u)"), x, y );
					hr = E_FAIL;
				}
			}
		}
	}

	for (UINT i = 0; i < 2; i++)
	{
		if (mapped[i])
		{
			GetImmediateContext()->Unmap(m_pResults[i], 0);
		}
	}

	return hr;
}

TEST_RESULT	CD2DTileableCopyTest::ExecuteTestCase()
{
	HRESULT hr = S_OK;

	const float opacity = 0.75f;

	// Render using PushLayer/PopLayer to m_pResults[0]
	// This will use TILEABLE_COPY.
	// Some test variables will readback from the render target inbetween PushLayer and PopLayer
	// which will cause a mid-scene flush on tile-based renderers.
	// Various nesting depths are used to test drivers that optimize for nesting depth == 1.
	hr = RenderWithLayers(opacity);

	if (SUCCEEDED(hr))
	{
		GetImmediateContext()->CopyResource(m_pResults[0], m_pTargetTexture);
	}

	// Render using off-screen surfaces into m_pResults[1]
	// This will not use tileable copy, and should produce results that are equivalent to those produced above
	if (SUCCEEDED(hr))
	{
		hr = RenderWithCompatibleRenderTarget(opacity);
	}

	if (SUCCEEDED(hr))
	{
		GetImmediateContext()->CopyResource(m_pResults[1], m_pTargetTexture);
	}

	if (SUCCEEDED(hr))
	{
		hr = CompareResults();

		if (FAILED(hr) && !GetFramework()->IsMobile())
		{
			// save failing test cases to a file
			// D3DX10 is used to save images to a file
			// so a D3D10 device context state must be used

			D3D_FEATURE_LEVEL featureLevels[] = 
			{
				D3D_FEATURE_LEVEL_10_0,
				D3D_FEATURE_LEVEL_9_1,
			};

			ID3DDeviceContextState *pD3D10DeviceContextState = NULL;
			ID3DDeviceContextState *pOldDeviceContextState = NULL;

			if (SUCCEEDED(GetDevice1()->CreateDeviceContextState(
				0,
				featureLevels,
				ARRAYSIZE(featureLevels),
				D3D11_SDK_VERSION,
				__uuidof(ID3D10Device1),
				NULL,
				&pD3D10DeviceContextState
				)))
			{
				GetImmediateContext1()->SwapDeviceContextState(pD3D10DeviceContextState, &pOldDeviceContextState);

				WriteToLog( _T("Saving images to Layers.png (implemented with D2D layers) and OffscreenSurface.png (equivalent, not implemented with D2D layers)") );

				const wchar_t *pFileNames[] = { L"Layers.png", L"OffscreenSurface.png" };

				for (UINT i = 0; i < 2; i++)
				{
					ID3D10Texture2D *pSrcTexture = NULL;
					ID3D10Device *pD3D10Device = NULL;
					ID3D10Texture2D *pDstTexture = NULL;

					if (SUCCEEDED(m_pResults[i]->QueryInterface(&pSrcTexture)))
					{
						// Convert to RGBA (so that it can be saved as a png)
						D3D10_TEXTURE2D_DESC desc;
						pSrcTexture->GetDesc(&desc);
						desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

						pSrcTexture->GetDevice(&pD3D10Device);

						if (SUCCEEDED(pD3D10Device->CreateTexture2D(&desc, NULL, &pDstTexture)))
						{
							if (SUCCEEDED(D3DX10LoadTextureFromTexture(pSrcTexture, NULL, pDstTexture)))
							{                           
								D3DX10SaveTextureToFileW(pDstTexture, D3DX10_IFF_PNG, pFileNames[i]);
							}
						}
					}

					SAFE_RELEASE(pSrcTexture);
					SAFE_RELEASE(pD3D10Device);
					SAFE_RELEASE(pDstTexture);
				}

				GetImmediateContext1()->SwapDeviceContextState(pOldDeviceContextState, NULL);
			}

			SAFE_RELEASE(pOldDeviceContextState);
			SAFE_RELEASE(pD3D10DeviceContextState);
		}
	}

	return SUCCEEDED(hr) ? RESULT_PASS : RESULT_FAIL;
}