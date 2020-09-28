#include "PresentHelper.h"
#include "WGFTestCore.h"
#include "Strsafe.h"

#define RELEASE(a) if (!(a)){} else (a)->Release();
#define RESOURCE_PATTERN 0xacc0FAB0

//const char szVS[] = 
//	"struct VSIn\n"
//	"{\n"
//	"	float4 pos : POS;\n"
//	"};\n"
//	"struct VSOut\n"
//	"{\n"
//	"	float4 pos : SV_Position;\n"
//	"	float2 texCoord : texCoord;\n"
//	"};\n"
//	"VSOut main( VSIn input )\n"
//	"{\n"
//	"	return input; \n"
//	"}\n"
//	;

const char szVS[] = 
    "struct VSIn\n"
    "{\n"
    "	uint vertexId : VertexId;\n"
    "};\n"
    "struct VSOut\n"
    "{\n"
    "	float4 pos : SV_Position;\n"
    "	float2 texCoord : texCoord;\n"
    "};\n"
    "VSOut main( VSIn input )\n"
    "{\n"
    "	VSOut output; \n"
    "	output.pos = 0; \n"
    "	output.texCoord = 0; \n"
    "	switch (input.vertexId) \n"
    "	{\n"
    "		case 0:\n"
    "			output.pos = float4( -1.0, 1.0, 0.0, 1.0 );\n"
    "			output.texCoord = float2( 0.0, 0.0 );\n"
    "			break;\n"
    "		case 1:\n"
    "			output.pos = float4( 1.0, 1.0, 0.0, 1.0 );\n"
    "			output.texCoord = float2( 1.0, 0.0 );\n"
    "			break;\n"
    "		case 2:\n"
    "			output.pos = float4( -1.0, -1.0, 0.0, 1.0 );\n"
    "			output.texCoord = float2( 0.0, 1.0 );\n"
    "			break;\n"
    "		case 3:\n"
    "			output.pos = float4( 1.0, -1.0, 0.0, 1.0 );\n"
    "			output.texCoord = float2( 1.0, 1.0 );\n"
    "			break;\n"
    "	}\n"
    "	return output; \n"
    "}\n"
    ;

const char szPS[] = 
    "Texture2D myTex2D;\n"
    "sampler samp;\n"
    "\n"
    "struct PSIn\n"
    "{\n"
    "	float4 pos : SV_Position;\n"
    "	linear float2 texCoord : texCoord;\n"
    "};\n"
    "struct PSOut\n"
    "{\n"
    "	float4 color : SV_Target; \n"
    "};\n"
    "PSOut main( PSIn input )\n"
    "{\n"
    "	PSOut output; \n"
    "	output.color = myTex2D.Sample(samp, input.texCoord); \n"
    //"	output.color = float4(0.5,0.5,0.5,0.5); \n"
    "	return output; \n"
    "}\n"
    ;


CPresentHelper::CPresentHelper() :
    m_pDevice(NULL),
    m_pDeviceContext(NULL),
    m_pVS(NULL),
    m_pPS(NULL),
    m_pRasterizer(NULL),
    m_pRTView(NULL),
    m_pSampler(NULL),
    m_pVB(NULL),
    m_pIL(NULL),
    m_pTextureData(NULL)
{
}

CPresentHelper::~CPresentHelper()
{
    SAFE_RELEASE(m_pIL);
    SAFE_RELEASE(m_pVB);
    SAFE_RELEASE(m_pVS);
    SAFE_RELEASE(m_pPS);
    SAFE_RELEASE(m_pSampler);
    SAFE_RELEASE(m_pRasterizer);
    SAFE_RELEASE(m_pRTView);
    SAFE_RELEASE(m_pDevice);
    SAFE_RELEASE(m_pDeviceContext);
    SAFE_DELETE(m_pTextureData);
}

bool CPresentHelper::Setup(CD3D11Test *pTest)
{
    ID3D10Blob *pShader;
    ID3D10Blob *pError;
    bool bRes = false;
    HRESULT hr;

    m_pTest = pTest;
    if (pTest == NULL)
        return false;

    //
    // Get the device
    //
    m_pDevice = pTest->GetDevice();
    if (m_pDevice == NULL)
        return false;

    m_pDevice->AddRef();

    //
    // Get the device context
    //
    m_pDeviceContext = pTest->GetImmediateContext();
    if (m_pDeviceContext == NULL)
        return false;

    m_pDeviceContext->AddRef();

    //
    // Get the Backbuffer RT View
    //
    m_pRTView = pTest->GetRTView();
    if (m_pRTView == NULL)
        return false;

    m_pRTView->AddRef();

    D3D_FEATURE_LEVEL FL = m_pDevice->GetFeatureLevel();
    bool isFL9 = (FL < D3D_FEATURE_LEVEL_10_0) ? true : false;

    //
    // Create VS
    //
    hr = D3DX10CompileFromMemory( szVS, strlen( szVS ), NULL, NULL, NULL, "main", isFL9 ? "vs_4_0_level_9_1" : "vs_4_0", 0, 0, NULL, &pShader, &pError, NULL );

    if( pError )
    {
        pTest->WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
    }

    if( FAILED( hr ) || pError )
    {
        pTest->WriteToLog( _T( "CPresentHelper::Setup() D3DX10CompileFromMemory() unexpectedly failed for VS - result code '%s'" ), D3DHResultToString( hr ).c_str() );
        goto FAIL;
    }

    if( FAILED( hr = m_pDevice->CreateVertexShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &m_pVS ) ) )
    {
        pTest->WriteToLog( _T( "CPresentHelper::Setup() - CreateVertexShader failed : %s" ), D3DHResultToString( hr ).c_str() );
        goto FAIL;
    }


    //
    //	Create VertexBuffer
    //
    D3D11_BUFFER_DESC BufferDesc;
    ZeroMemory(&BufferDesc, sizeof(BufferDesc));
    UINT Vertices[] = {0,1,2,3};

    BufferDesc.ByteWidth = sizeof(Vertices);
    BufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
    
    D3D11_SUBRESOURCE_DATA BufferData;
    ZeroMemory(&BufferData, sizeof(BufferData));
    BufferData.pSysMem = Vertices;

    hr = m_pDevice->CreateBuffer(&BufferDesc, &BufferData, &m_pVB);
 
    if(FAILED(hr))
    {
        pTest->WriteToLog( _T( "CPresentHelper::Setup - CreateBuffer failed : %s" ), D3DHResultToString( hr ).c_str() );
        goto FAIL;
    }

    //
    // Input Layout
    //
    D3D11_INPUT_ELEMENT_DESC Desc;
    Desc.InputSlot = 0;
    Desc.AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
    Desc.Format = DXGI_FORMAT_R8G8B8A8_UINT;
    Desc.SemanticName = "VertexID";
    Desc.SemanticIndex = 0;
    Desc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
    Desc.InstanceDataStepRate = 0;

    hr = m_pDevice->CreateInputLayout( &Desc, 1, pShader->GetBufferPointer(), pShader->GetBufferSize(), &m_pIL );

    if(FAILED(hr))
    {
        pTest->WriteToLog( _T( "CPresentHelper::Setup - CreateInputLayout failed : %s" ), D3DHResultToString( hr ).c_str() );
        goto FAIL;
    }


    SAFE_RELEASE( pShader );
    SAFE_RELEASE( pError );

    //
    // Create PS
    //
    hr = D3DX10CompileFromMemory( szPS, strlen( szPS ), NULL, NULL, NULL, "main", isFL9 ? "ps_4_0_level_9_1" : "ps_4_0", 0, 0, NULL, &pShader, &pError, NULL );

    if( pError )
    {
        pTest->WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
    }

    if( FAILED( hr ) || pError )
    {
        pTest->WriteToLog( _T( "CPresentHelper::Setup D3DX10CompileFromMemory() unexpectedly failed for PS - result code '%s'" ), D3DHResultToString( hr ).c_str() );
        goto FAIL;
    }

    if( FAILED( hr = m_pDevice->CreatePixelShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), NULL, &m_pPS ) ) )
    {
        pTest->WriteToLog( _T( "CPresentHelper::Setup - CreatePixelShader failed : %s" ), D3DHResultToString( hr ).c_str() );
        goto FAIL;
    }


    //
    // Create Sampler
    //
    D3D11_SAMPLER_DESC sampDesc;
    ZeroMemory(&sampDesc, sizeof(sampDesc));
    sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
    sampDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
    sampDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_POINT;
    sampDesc.MaxAnisotropy = 1;
    sampDesc.MaxLOD = isFL9 ? FLT_MAX : 0;
    sampDesc.MinLOD = 0;
    sampDesc.MipLODBias = 0;

    hr = m_pDevice->CreateSamplerState(&sampDesc, &m_pSampler);

    if( FAILED( hr ) )
    {
        pTest->WriteToLog( _T( "CPresentHelper::Setup - CreateSamplerState() failed : '%s'" ), D3DHResultToString( hr ).c_str() );
        goto FAIL;
    }

    //
    //Create Rasterizer State
    //
    D3D11_RASTERIZER_DESC rastDesc;
    rastDesc.FillMode = D3D11_FILL_SOLID;
    rastDesc.CullMode = D3D11_CULL_NONE;
    rastDesc.FrontCounterClockwise = TRUE;
    rastDesc.DepthBias = 0;
    rastDesc.DepthBiasClamp = 0;
    rastDesc.SlopeScaledDepthBias = 0;
    rastDesc.DepthClipEnable = isFL9 ? TRUE : FALSE;
    rastDesc.ScissorEnable = FALSE;
    rastDesc.MultisampleEnable = FALSE;
    rastDesc.AntialiasedLineEnable = FALSE;

    hr = m_pDevice->CreateRasterizerState(&rastDesc, &m_pRasterizer);
    if (FAILED(hr))
    {
        pTest->WriteToLog( _T( "CPresentHelper::Setup - CreateRasterizerState() failed : '%s'" ), D3DHResultToString( hr ).c_str() );
        goto FAIL;
    }

    bRes = true;

FAIL:
    SAFE_RELEASE( pShader );
    SAFE_RELEASE( pError );

    return bRes;
}

bool CPresentHelper::PresentTexture2D(ID3D11Texture2D *pTexture2D, ID3D11ShaderResourceView *pTextureView)
{
    ID3D11Texture2D *pSRVTex2D = NULL;
    ID3D11ShaderResourceView *pSRV = NULL;
    bool bTexAddRef = false;
    bool bTexViewAddRef = false;

    if (pTextureView != NULL)
    {
        pSRV = pTextureView;
        pTextureView->AddRef();
        bTexViewAddRef = true;
    }
    else
    {
        //
        //Make sure the texture can be used as a Shader Resource View
        //Otherwise create a new texture and copy from original
        //
        D3D11_TEXTURE2D_DESC desc;
        pTexture2D->GetDesc(&desc);
        if (desc.BindFlags & D3D11_BIND_SHADER_RESOURCE)
        {
            pSRVTex2D = pTexture2D;
            pTexture2D->AddRef();
            bTexAddRef = true;
        }
        else
        {
        }

        //
        //Create a Shader Resource View
        //
        D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
        srvDesc.Format = desc.Format;
        srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
        srvDesc.Texture2D.MostDetailedMip = 0;
        srvDesc.Texture2D.MipLevels = 1;
        m_pDevice->CreateShaderResourceView(pSRVTex2D, &srvDesc, &pSRV);
    }
    

    //
    //Save previous objects
    //
    ID3D11VertexShader *pOriginalVS = NULL;
    ID3D11GeometryShader *pOriginalGS = NULL;
    ID3D11PixelShader *pOriginalPS = NULL;
    ID3D11SamplerState *pOriginalSampler = NULL;
    ID3D11RasterizerState *pOriginalRasterizer = NULL;
    ID3D11ShaderResourceView *pOriginalSRV = NULL;
    ID3D11RenderTargetView *pOriginalRT = NULL;
    ID3D11DepthStencilView *pOriginalDS = NULL;
    ID3D11InputLayout *pOriginalInputLayout = NULL;
    D3D11_PRIMITIVE_TOPOLOGY originalTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    ID3D11Buffer *pOriginalVB = NULL;
    UINT OriginalVBStrides = 0;
    UINT OriginalVBOffsets = 0;

    m_pDeviceContext->VSGetShader(&pOriginalVS, NULL, NULL);
    m_pDeviceContext->GSGetShader(&pOriginalGS, NULL, NULL);
    m_pDeviceContext->PSGetShader(&pOriginalPS, NULL, NULL);
    m_pDeviceContext->PSGetSamplers(0, 1, &pOriginalSampler);
    m_pDeviceContext->PSGetShaderResources(0, 1, &pOriginalSRV);
    m_pDeviceContext->OMGetRenderTargets(1, &pOriginalRT, &pOriginalDS);
    m_pDeviceContext->RSGetState(&pOriginalRasterizer);
    m_pDeviceContext->IAGetInputLayout(&pOriginalInputLayout);
    m_pDeviceContext->IAGetPrimitiveTopology(&originalTopology);
    m_pDeviceContext->IAGetVertexBuffers(0, 1, &pOriginalVB, &OriginalVBStrides, &OriginalVBOffsets);
    //D3D11_VIEWPORT viewports[4];
    //UINT numViewports = 0;
    //m_pDeviceContext->RSGetViewports(&numViewports, viewports);


    //
    //Draw a quad with the texture
    //
    m_pDeviceContext->VSSetShader( m_pVS , NULL, 0);
    m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
    m_pDeviceContext->PSSetShader( m_pPS, NULL, 0 );
    m_pDeviceContext->PSSetSamplers(0, 1, &m_pSampler);
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRTView, NULL);
    m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);
    m_pDeviceContext->RSSetState(m_pRasterizer);
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);

    UINT Stride = sizeof(UINT);
    UINT Offset = 0;
    m_pDeviceContext->IASetVertexBuffers(0, 1, &m_pVB, &Stride, &Offset);
    m_pDeviceContext->IASetInputLayout(m_pIL);


    m_pDeviceContext->Draw(4, 0);

    bool bRet = m_pTest->GetFramework()->Present(NULL, NULL, NULL, NULL, 0, 0);

    //
    //Reset original objects
    //
    m_pDeviceContext->VSSetShader(pOriginalVS, NULL, 0);
    m_pDeviceContext->GSSetShader(pOriginalGS, NULL, 0);
    m_pDeviceContext->PSSetShader(pOriginalPS, NULL, 0);
    m_pDeviceContext->PSSetSamplers(0, 1, &pOriginalSampler);
    m_pDeviceContext->PSSetShaderResources(0, 1, &pOriginalSRV);
    m_pDeviceContext->OMSetRenderTargets(1, &pOriginalRT, pOriginalDS);
    m_pDeviceContext->RSSetState(pOriginalRasterizer);
    m_pDeviceContext->IASetInputLayout( pOriginalInputLayout );
    m_pDeviceContext->IASetPrimitiveTopology(originalTopology);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &pOriginalVB, &OriginalVBStrides, &OriginalVBOffsets);

    //Release for all the get
    SAFE_RELEASE( pOriginalVS );
    SAFE_RELEASE( pOriginalGS );
    SAFE_RELEASE( pOriginalPS );
    SAFE_RELEASE( pOriginalSampler );
    SAFE_RELEASE( pOriginalSRV );
    SAFE_RELEASE( pOriginalRT );
    SAFE_RELEASE( pOriginalDS );
    SAFE_RELEASE( pOriginalRasterizer );
    SAFE_RELEASE( pOriginalInputLayout );
    SAFE_RELEASE( pOriginalVB);


    SAFE_RELEASE( pSRVTex2D );
    SAFE_RELEASE( pSRV );

    //if(bTexAddRef)
    //{
    //    pTexture2D->Release();
    //}
    //if(bTexViewAddRef)
    //{
    //    pTextureView->Release();
    //}

    return bRet;
}


bool CPresentHelper::PresentBuffer(ID3D11Buffer* pBuffer)
{
    ID3D11Texture2D *pSRVTex2D = NULL;
    ID3D11ShaderResourceView *pSRV = NULL;

    HRESULT hr = Create2DTexture(&pSRVTex2D);

    if(FAILED(hr))
        return false;


    D3D11_TEXTURE2D_DESC desc;
    pSRVTex2D->GetDesc(&desc);

    //
    //Create a Shader Resource View
    //
    D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
    srvDesc.Format = desc.Format;
    srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
    srvDesc.Texture2D.MostDetailedMip = 0;
    srvDesc.Texture2D.MipLevels = 1;
    m_pDevice->CreateShaderResourceView(pSRVTex2D, &srvDesc, &pSRV);
    

    //
    //Save previous objects
    //
    ID3D11VertexShader *pOriginalVS = NULL;
    ID3D11GeometryShader *pOriginalGS = NULL;
    ID3D11PixelShader *pOriginalPS = NULL;
    ID3D11SamplerState *pOriginalSampler = NULL;
    ID3D11RasterizerState *pOriginalRasterizer = NULL;
    ID3D11ShaderResourceView *pOriginalSRV = NULL;
    ID3D11RenderTargetView *pOriginalRT = NULL;
    ID3D11DepthStencilView *pOriginalDS = NULL;
    ID3D11InputLayout *pOriginalInputLayout = NULL;
    D3D11_PRIMITIVE_TOPOLOGY originalTopology = D3D11_PRIMITIVE_TOPOLOGY_UNDEFINED;
    ID3D11Buffer *pOriginalVB = NULL;
    UINT OriginalVBStrides = 0;
    UINT OriginalVBOffsets = 0;

    m_pDeviceContext->VSGetShader(&pOriginalVS, NULL, NULL);
    m_pDeviceContext->GSGetShader(&pOriginalGS, NULL, NULL);
    m_pDeviceContext->PSGetShader(&pOriginalPS, NULL, NULL);
    m_pDeviceContext->PSGetSamplers(0, 1, &pOriginalSampler);
    m_pDeviceContext->PSGetShaderResources(0, 1, &pOriginalSRV);
    m_pDeviceContext->OMGetRenderTargets(1, &pOriginalRT, &pOriginalDS);
    m_pDeviceContext->RSGetState(&pOriginalRasterizer);
    m_pDeviceContext->IAGetInputLayout(&pOriginalInputLayout);
    m_pDeviceContext->IAGetPrimitiveTopology(&originalTopology);
    m_pDeviceContext->IAGetVertexBuffers(0, 1, &pOriginalVB, &OriginalVBStrides, &OriginalVBOffsets);


    //
    //Draw a quad with the texture
    //
    m_pDeviceContext->VSSetShader( m_pVS , NULL, 0);
    m_pDeviceContext->GSSetShader( NULL, NULL, 0 );
    m_pDeviceContext->PSSetShader( m_pPS, NULL, 0 );
    m_pDeviceContext->PSSetSamplers(0, 1, &m_pSampler);
    m_pDeviceContext->OMSetRenderTargets(1, &m_pRTView, NULL);
    m_pDeviceContext->PSSetShaderResources(0, 1, &pSRV);
    m_pDeviceContext->RSSetState(m_pRasterizer);
    m_pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_POINTLIST);

    UINT Stride = sizeof(UINT);
    UINT Offset = 0;
    m_pDeviceContext->IASetVertexBuffers(0, 1, &pBuffer, &Stride, &Offset);
    m_pDeviceContext->IASetInputLayout(m_pIL);


    m_pDeviceContext->Draw(4, 0);

    bool bRet = m_pTest->GetFramework()->Present(NULL, NULL, NULL, NULL, 0, 0);

    //
    //Reset original objects
    //
    m_pDeviceContext->VSSetShader(pOriginalVS, NULL, 0);
    m_pDeviceContext->GSSetShader(pOriginalGS, NULL, 0);
    m_pDeviceContext->PSSetShader(pOriginalPS, NULL, 0);
    m_pDeviceContext->PSSetSamplers(0, 1, &pOriginalSampler);
    m_pDeviceContext->PSSetShaderResources(0, 1, &pOriginalSRV);
    m_pDeviceContext->OMSetRenderTargets(1, &pOriginalRT, pOriginalDS);
    m_pDeviceContext->RSSetState(pOriginalRasterizer);
    m_pDeviceContext->IASetInputLayout( pOriginalInputLayout );
    m_pDeviceContext->IASetPrimitiveTopology(originalTopology);
    m_pDeviceContext->IASetVertexBuffers(0, 1, &pOriginalVB, &OriginalVBStrides, &OriginalVBOffsets);

    //Release for all the get
    SAFE_RELEASE( pOriginalVS );
    SAFE_RELEASE( pOriginalGS );
    SAFE_RELEASE( pOriginalPS );
    SAFE_RELEASE( pOriginalSampler );
    SAFE_RELEASE( pOriginalSRV );
    SAFE_RELEASE( pOriginalRT );
    SAFE_RELEASE( pOriginalDS );
    SAFE_RELEASE( pOriginalRasterizer );
    SAFE_RELEASE( pOriginalInputLayout );
    SAFE_RELEASE( pOriginalVB);


    SAFE_RELEASE( pSRVTex2D );
    SAFE_RELEASE( pSRV );

    return bRet;
}

HRESULT CPresentHelper::Create2DTexture(ID3D11Texture2D** ppTex)
{
    D3D11_SUBRESOURCE_DATA	 subResData = {0};
    D3D11_SUBRESOURCE_DATA*  pSubResData = NULL;
    HRESULT hr = S_OK;

    UINT Width = 64;
    UINT Height = 64;
    DXGI_FORMAT DXGIFormat = DXGI_FORMAT_R8G8B8A8_UNORM;

    UINT bytesPerPixel = GetBitsPerElement(DXGIFormat) / 8;
    UINT SizeInBytes = Width * Height * bytesPerPixel;

    UINT DataSize = SizeInBytes/sizeof(ULONG);
    m_pTextureData = new ULONG[DataSize];

    //m_pTextureData is used to update texture data in resource
    for (UINT i = 0; i < DataSize; ++i)
    {
        m_pTextureData[i] = RESOURCE_PATTERN + i;
    }


    subResData.SysMemPitch = SizeInBytes / Height; // same as bytesPerPixel * m_Width

    D3D11_TEXTURE2D_DESC tex2DDesc = {0};
    tex2DDesc.ArraySize			= 1;
    tex2DDesc.Format			= DXGIFormat;
    tex2DDesc.Height			= Height;
    tex2DDesc.Width				= Width;
    tex2DDesc.MipLevels			= 1;
    tex2DDesc.SampleDesc.Count	= 1;
    tex2DDesc.SampleDesc.Quality= 0;
    tex2DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE | D3D11_BIND_RENDER_TARGET;
    tex2DDesc.CPUAccessFlags	= 0;
    tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
    tex2DDesc.MiscFlags         = D3D11_RESOURCE_MISC_GENERATE_MIPS;


    hr = m_pDevice->CreateTexture2D(&tex2DDesc, pSubResData, ppTex);

    if(m_pTextureData)
    {
        delete[] m_pTextureData;
        m_pTextureData = NULL;
    }

    return hr;
}