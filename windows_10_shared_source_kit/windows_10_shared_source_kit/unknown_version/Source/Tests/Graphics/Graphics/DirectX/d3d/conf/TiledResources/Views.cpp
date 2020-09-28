////////////////////////////////////////////////////////////////////////////////
// Views.cpp
//
////////////////////////////////////////////////////////////////////////////////

#include "D3D11Test.h"
#include "FormatHelper.h"
#include "TiledResources.h"
#include "UpdateTileMappings.h"
#include "CopyTileMappings.h"
#include "CopyTiles.h"
#include "d3d11sdklayers.h"

#include "Views.h"

using namespace WindowsTest;

BEGIN_NAMED_VALUES(RW_MODE)
    NAMED_VALUE(_T("ReadFromTiledUAV"), RW_MODE_READ_FROM_TILED_RESOURCE)
    NAMED_VALUE(_T("WriteToTiledUAV"), RW_MODE_WRITE_TO_TILED_RESOURCE)
END_NAMED_VALUES(RW_MODE)


////////////////////////////////////////////////////////////////////////////////
//
// CTiledDepthStencilViewTest
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////
// CTiledDepthStencilViewTest::CreateResources()
////////////////////////////////////////
CTiledDepthStencilViewTest::CTiledDepthStencilViewTest() :
m_pSRTexture(NULL),	// Texture used to render both depth and color
m_pSRView(NULL),
m_pRenderDepthPS(NULL),
m_pRenderColorPS(NULL),
m_pRS(NULL),
m_pDSS(NULL)
{
}

////////////////////////////////////////////////////////////////////////////////
// CTiledDepthStencilViewTest::CreateResources()
////////////////////////////////////////
TEST_RESULT CTiledDepthStencilViewTest::CreateResources()
{
    return CTiledResourceViewTestBase<TRB_TEXTURE_2D>::CreateResources(m_parameters.pDepthFormat->GetValue(), D3D11_BIND_DEPTH_STENCIL);
}

////////////////////////////////////////////////////////////////////////////////
// CTiledDepthStencilViewTest::InitTestParameters()
////////////////////////////////////////
void CTiledDepthStencilViewTest::InitTestParameters()
{
    CreateParameter(_T("ResidencyPattern"), &CTiledResourceViewTestBase<TRB_TEXTURE_2D>::m_parameters.pResidencyPattern);
    CreateParameter(_T("DepthFormat"), &m_parameters.pDepthFormat);

    CUserValueSet<RESIDENCY_PATTERN_2D_3D> *pResidencyPatternValues = new CUserValueSet<RESIDENCY_PATTERN_2D_3D>();
    pResidencyPatternValues->AddValue(RP2D_3D_FULL);
    pResidencyPatternValues->AddValue(RP2D_3D_FRAGMENTED_MULTI_TILE);
    pResidencyPatternValues->AddValue(RP2D_3D_UNIFORM_BOX);

    testfactor::RFactor residencyPattern = AddParameterValueSet<RESIDENCY_PATTERN_2D_3D>(
        CTiledResourceViewTestBase<TRB_TEXTURE_2D>::m_parameters.pResidencyPattern,
        pResidencyPatternValues
        );

    CUserValueSet<DXGI_FORMAT> *pDepthFormatValues = new CUserValueSet<DXGI_FORMAT>();
    pDepthFormatValues->AddValue(DXGI_FORMAT_D32_FLOAT);
    pDepthFormatValues->AddValue(DXGI_FORMAT_D16_UNORM);

    testfactor::RFactor depthFormat = AddParameterValueSet<DXGI_FORMAT>(
        m_parameters.pDepthFormat,
        pDepthFormatValues
        );

    // Because depth stencil views apply only to Texture2D, we will test only tiers 1 and 2
    testfactor::RFactor tiledResourcesTier = InitTiledResourcesTierParam(false);

    SetRootTestFactor(residencyPattern * depthFormat * tiledResourcesTier);
}

TEST_RESULT CTiledDepthStencilViewTest::Setup()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    CHandle<ID3D11Texture2D> pSRTexture;	// Texture used to render both depth and color
    CHandle<ID3D11ShaderResourceView> pSRView;
    CHandle<ID3D11PixelShader> pRenderDepthPS;
    CHandle<ID3D11PixelShader> pRenderColorPS;
    CHandle<ID3D11RasterizerState1> pRS;
    CHandle<ID3D11DepthStencilState> pDSS;

    tr = CTiledResourceViewTestBase<TRB_TEXTURE_2D>::Setup();
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    //
    // Create textures
    //

    // Width and Height are both 4
    const UINT width = 4 * CTiledResourceViewTestBase<TRB_TEXTURE_2D>::widthPerTile;
    const UINT height = 4 * CTiledResourceViewTestBase<TRB_TEXTURE_2D>::heightPerTile;

    // Create standard tile pattern in staging texture
    CHandle<ID3D11Texture2D> pStagingSource;
    tr = CreateStagingTex2D(width, height, &pStagingSource, true);
    if (RESULT_PASS != tr)
    {
        return tr;
    }

    // Shader resource texture
    D3D11_TEXTURE2D_DESC tex2ddesc;
    tex2ddesc.ArraySize = 1;
    tex2ddesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
    tex2ddesc.CPUAccessFlags = 0;
    tex2ddesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    tex2ddesc.Width = width;
    tex2ddesc.Height = height;
    tex2ddesc.MipLevels = 1;
    tex2ddesc.MiscFlags = 0;
    tex2ddesc.SampleDesc.Count = 1;
    tex2ddesc.SampleDesc.Quality = 0;
    tex2ddesc.Usage = D3D11_USAGE_DEFAULT;

    hr = pDevice->CreateTexture2D(&tex2ddesc, NULL, &pSRTexture);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateTexture2D);
        return RESULT_FAIL;
    }

    // Copy the staging resource to the shader resource texture
    pContext->CopySubresourceRegion(pSRTexture, 0, 0, 0, 0, pStagingSource, 0, NULL);

    //
    // Create the shaders
    //
    const BYTE *pShaderByteCodeRenderDepth;
    const BYTE *pShaderByteCodeRenderColorUsingDepth;
    SIZE_T shaderLenRenderDepth;
    SIZE_T shaderLenRenderColorUsingDepth;
    CTiledResourcesTest::GetShaderByteCode(CTiledResourcesTest::LSBT_PIXEL_RENDER_DEPTH, &pShaderByteCodeRenderDepth, &shaderLenRenderDepth, false, false);
    CTiledResourcesTest::GetShaderByteCode(CTiledResourcesTest::LSBT_PIXEL_RENDER_COLOR_USING_DEPTH, &pShaderByteCodeRenderColorUsingDepth, &shaderLenRenderColorUsingDepth, false, false);

    // Depth rendering pass
    hr = pDevice->CreatePixelShader(pShaderByteCodeRenderDepth, shaderLenRenderDepth, NULL, &pRenderDepthPS);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreatePixelShader);
        return RESULT_FAIL;
    }

    // Color rendering pass
    hr = pDevice->CreatePixelShader(pShaderByteCodeRenderColorUsingDepth, shaderLenRenderColorUsingDepth, NULL, &pRenderColorPS);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreatePixelShader);
        return RESULT_FAIL;
    }

    //
    // Create views
    //

    // Shader resource view
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Texture2D.MipLevels = 1;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Format = tex2ddesc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    hr = pDevice->CreateShaderResourceView(pSRTexture, &srvDesc, &pSRView);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateShaderResourceView);
        return RESULT_FAIL;
    }

    //
    // Create the depth stencil state
    //
    D3D11_DEPTH_STENCIL_DESC dsDesc;
    dsDesc.DepthEnable = TRUE;
    dsDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
    dsDesc.DepthFunc = D3D11_COMPARISON_LESS;
    dsDesc.StencilEnable = FALSE;
    hr = pDevice->CreateDepthStencilState(&dsDesc, &pDSS);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateDepthStencilState);
        return RESULT_FAIL;
    }


    //
    // Create the rasterizer state
    //
    D3D11_RASTERIZER_DESC1 rsDesc;
    rsDesc.AntialiasedLineEnable = FALSE;
    rsDesc.CullMode = D3D11_CULL_BACK;
    rsDesc.DepthBias = 0;
    rsDesc.DepthBiasClamp = 0.0f;
    rsDesc.DepthClipEnable = TRUE;
    rsDesc.FillMode = D3D11_FILL_SOLID;
    rsDesc.ForcedSampleCount = 0;
    rsDesc.FrontCounterClockwise = TRUE;
    rsDesc.MultisampleEnable = FALSE;
    rsDesc.ScissorEnable = FALSE;
    rsDesc.SlopeScaledDepthBias = 0;
    hr = pDevice->CreateRasterizerState1(&rsDesc, &pRS);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRasterizerState1);
        return RESULT_FAIL;
    }

    //
    // Initialize the pipeline state
    //

    // Input assembler state
    pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
    // Clear input layout as no input is needed since the 
    // vertex shader knows how to draw a quad using vertex ID's
    pContext->IASetInputLayout(NULL);

    // Pixel shader state
    pContext->PSSetShaderResources(0, 1, &pSRView);

    // Vertex shader state
    if (NULL == m_pCopyTexVS)
    {
        const BYTE *pShaderByteCodeVertex;
        SIZE_T shaderLenVertex;
        CTiledResourcesTest::GetShaderByteCode(CTiledResourcesTest::LSBT_VERTEX_COPY_TEX, &pShaderByteCodeVertex, &shaderLenVertex, false, false);

        hr = pDevice->CreateVertexShader(pShaderByteCodeVertex, shaderLenVertex, NULL, &m_pCopyTexVS);
        if (FAILED(hr))
        {
            REPORT_HRESULT_FAILURE(hr, CreateVertexShader);
            return RESULT_FAIL;
        }
    }

    pContext->VSSetShader(m_pCopyTexVS, NULL, 0);

    // Rasterizer state
    pContext->RSSetState(pRS);

    // Output merger
    pContext->OMSetDepthStencilState(pDSS, 0);


    //
    // Capture the pointers and detach smart pointers
    //
    m_pSRTexture = pSRTexture;
    pSRTexture.Detach();

    m_pSRView = pSRView;
    pSRView.Detach();

    m_pRenderDepthPS = pRenderDepthPS;
    pRenderDepthPS.Detach();

    m_pRenderColorPS = pRenderColorPS;
    pRenderColorPS.Detach();

    m_pRS = pRS;
    pRS.Detach();

    m_pDSS = pDSS;
    pDSS.Detach();

    return tr;
}

void CTiledDepthStencilViewTest::Cleanup()
{
    SAFE_RELEASE(m_pSRTexture);
    SAFE_RELEASE(m_pSRView);
    SAFE_RELEASE(m_pRenderDepthPS);
    SAFE_RELEASE(m_pRenderColorPS);
    SAFE_RELEASE(m_pRS);
    SAFE_RELEASE(m_pDSS);

    CTiledResourceViewTestBase<TRB_TEXTURE_2D>::Cleanup();
}

////////////////////////////////////////////////////////////////////////////////
// CTiledDepthStencilViewTest::ExecuteTestCase()
////////////////////////////////////////
TEST_RESULT CTiledDepthStencilViewTest::ExecuteTestCase()
{
    HRESULT hr = S_OK;
    TEST_RESULT tr = RESULT_PASS;
    ID3D11Device2 *pDevice = GetDevice2();
    ID3D11DeviceContext2 *pContext = GetImmediateContext2();
    CHandle<ID3D11Texture2D> pTiledDepthTexture = m_pTiledResource;
    CHandle<ID3D11Texture2D> pNormalDepthTexture;
    CHandle<ID3D11DepthStencilView> pTiledDSV;
    CHandle<ID3D11DepthStencilView> pNormalDSV;
    CHandle<ID3D11Texture2D> pTiledPassRTTexture;
    CHandle<ID3D11Texture2D> pNormalPassRTTexture;
    CHandle<ID3D11RenderTargetView> pTiledPassRTV;
    CHandle<ID3D11RenderTargetView> pNormalPassRTV;
    CTiledResourceHelper::CTilePoolManager *pTilePoolManager = GetFramework()->GetTilePoolManager();
    UINT startTiles;
    UINT tileWidth = 0;
    UINT tileHeight = 0;

    GetTileSize(m_parameters.pDepthFormat->GetValue(), tileWidth, tileHeight);

    // Width and Height are both 4
    const UINT width = 4 * tileWidth;
    const UINT height = 4 * tileHeight;

    // Set the viewport
    D3D11_VIEWPORT viewport =
    {
        0.0f,						//	FLOAT TopLeftX;
        0.0f,						//	FLOAT TopLeftY;
        (float)width,				//	FLOAT Width;
        (float)height,				//	FLOAT Height;
        0.0f,						//	FLOAT MinDepth;
        1.0f,						//	FLOAT MaxDepth;
    };

    pContext->RSSetViewports(1, &viewport);

    //
    // Create the depth stencil textures and views
    //

    // Non-tiled texture
    D3D11_TEXTURE2D_DESC texDesc;
    pTiledDepthTexture->GetDesc(&texDesc);
    texDesc.MiscFlags = 0;
    hr = pDevice->CreateTexture2D(&texDesc, NULL, &pNormalDepthTexture);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateDepthStencilState);
        return RESULT_FAIL;
    }

    // Depth stencil views
    D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
    dsvDesc.Format = m_parameters.pDepthFormat->GetValue();
    dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
    dsvDesc.Flags = 0;
    dsvDesc.Texture2D.MipSlice = 0;

    hr = pDevice->CreateDepthStencilView(pTiledDepthTexture, &dsvDesc, &pTiledDSV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateDepthStencilView);
        return RESULT_FAIL;
    }

    hr = pDevice->CreateDepthStencilView(pNormalDepthTexture, &dsvDesc, &pNormalDSV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateDepthStencilView);
        return RESULT_FAIL;
    }

    //
    // Initialize the tile mapping on the tiled depth texture
    //
    startTiles = pTilePoolManager->FetchRange(pTiledDepthTexture, 16);
    if (RESULT_PASS != UpdateTileMappingsTex2D(
        pTiledDepthTexture,
        startTiles,
        pTilePoolManager->GetTilePool(),
        CTiledResourceViewTestBase<TRB_TEXTURE_2D>::m_parameters.pResidencyPattern->GetValue(),
        m_tileMapping
        ))
    {
        return RESULT_FAIL;
    }

    //
    // Clear the depth stencil views
    //
    pContext->TiledResourceBarrier(pTiledDSV, pTiledDSV);
    pContext->ClearDepthStencilView(pTiledDSV, D3D11_CLEAR_DEPTH, 1.0, 0);
    pContext->ClearDepthStencilView(pNormalDSV, D3D11_CLEAR_DEPTH, 1.0, 0);

    //
    // Render depth values to the tiled depth texture
    //
    pContext->PSSetShader(m_pRenderDepthPS, NULL, 0);
    pContext->OMSetRenderTargets(0, NULL, pTiledDSV); // Clear any render targets
    pContext->Draw(4, 0);

    // Before clearing the blank tiles see if null sentinel is corrupted
    if (RESULT_PASS != CheckNullSentinel())
    {
        tr = RESULT_FAIL;
    }

    // Clear NULL/blank logical tiles to restore metadata and/or cleanup junk in shared blank tile
    //	ClearBlankRects2D(m_tileMapping, pTiledDSV, m_parameters.pDepthFormat->GetValue());
    ClearBlankRegions(m_tileMapping, pTiledDSV, m_parameters.pDepthFormat->GetValue());

    ShowResource(L"Tiled depth buffer rendering pass", pTiledDepthTexture);

    //
    // Render the same depth values to the non-tiled depth texture
    //
    pContext->OMSetRenderTargets(0, NULL, pNormalDSV); // Clear any render targets
    pContext->Draw(4, 0);

    //	ClearBlankRects2D(m_tileMapping, pNormalDSV, m_parameters.pDepthFormat->GetValue());
    ClearBlankRegions(m_tileMapping, pNormalDSV, m_parameters.pDepthFormat->GetValue());

    ShowResource(L"Normal depth buffer rendering pass", pNormalDepthTexture);

    //
    // Compare the depth buffers
    //
    if (RESULT_PASS != CompareTiledResourceToReference(pTiledDepthTexture, pNormalDepthTexture, 0, 0, m_tileMapping))
    {
        tstring message(_T("Tiled depth buffer does not match non-tiled depth buffer or contains non-zero data in "));
        if (CTiledResourcesTest::m_parameters.pTiledResourcesTier->GetValue() == 1)
        {
            message = message + tstring(_T("shared 'blank' mapped tiles"));
        }
        else
        {
            message = message + tstring(_T("NULL mapped tiles"));
        }
        WriteToLog(message.c_str());

        CheckNullSentinel(); // Deliberately ignoring the return value since this test has already failed

        tr = RESULT_FAIL;
    }

    //
    // Render target textures
    //
    texDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
    texDesc.MiscFlags = 0;
    texDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    hr = pDevice->CreateTexture2D(&texDesc, NULL, &pTiledPassRTTexture);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateTexture2D);
        return RESULT_FAIL;
    }

    hr = pDevice->CreateTexture2D(&texDesc, NULL, &pNormalPassRTTexture);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateTexture2D);
        return RESULT_FAIL;
    }

    //
    // Render target views
    //
    D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
    rtvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
    rtvDesc.Texture2D.MipSlice = 0;
    hr = pDevice->CreateRenderTargetView(pTiledPassRTTexture, &rtvDesc, &pTiledPassRTV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }

    hr = pDevice->CreateRenderTargetView(pNormalPassRTTexture, &rtvDesc, &pNormalPassRTV);
    if (FAILED(hr))
    {
        REPORT_HRESULT_FAILURE(hr, CreateRenderTargetView);
        return RESULT_FAIL;
    }

    //
    // Using the previous tiled depth result render color pass
    //
    const float black[] = { 0.0f, 0.0f, 0.0f, 0.0f };
    pContext->ClearRenderTargetView(pTiledPassRTV, black);
    pContext->ClearRenderTargetView(pNormalPassRTV, black);
    pContext->PSSetShader(m_pRenderColorPS, NULL, 0);
    pContext->OMSetRenderTargets(1, &pTiledPassRTV, pTiledDSV);
    pContext->Draw(4, 0);

    //
    // Using the previous non-tiled depth result render color pass
    //
    pContext->OMSetRenderTargets(1, &pNormalPassRTV, pNormalDSV);
    pContext->Draw(4, 0);

    //	ClearBlankRects2D(m_tileMapping, pNormalPassRTV, m_parameters.pDepthFormat->GetValue());
    ClearBlankRegions(m_tileMapping, pNormalPassRTV, m_parameters.pDepthFormat->GetValue());

    ShowResource(L"Observed Color Pass Result", pTiledPassRTTexture);
    ShowResource(L"Expected Color Pass Result", pNormalPassRTTexture);

    //
    // Compare the rendered results
    //
    if (RESULT_PASS != CompareTiledResourceToReference(pTiledPassRTTexture, pNormalPassRTTexture, 0, 0, m_tileMapping, tileWidth, tileHeight))
    {
        WriteToLog(_T("Rendered image using tiled depth buffer doesn't match rendered image using non-tiled depth buffer"));
        tr = RESULT_FAIL;
    }

    if (RESULT_PASS != CheckNullSentinel())
    {
        tr = RESULT_FAIL;
    }

    return tr;
}
