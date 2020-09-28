#include "PresentHelper.h"
#include "WGFTestCore.h"
#include "D3DTest.h"
#include "D3DWrapped.h"
#include "Strsafe.h"

#define RELEASE(a) if (!(a)){} else (a)->Release();

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
	"	uint vertexId : SV_VertexId;\n"
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
	"	float4 color : SV_Target;\n"
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
	m_pVS(NULL),
	m_pPS(NULL),
	m_pRasterizer(NULL),
	m_pRTView(NULL),
	m_pSampler(NULL)
{
}

CPresentHelper::~CPresentHelper()
{
	SAFE_RELEASE(m_pVS);
	SAFE_RELEASE(m_pPS);
	SAFE_RELEASE(m_pSampler);
	SAFE_RELEASE(m_pRasterizer);
	SAFE_RELEASE(m_pRTView);
	SAFE_RELEASE(m_pDevice);
}

bool CPresentHelper::Setup(CD3DTest *pTest)
{
	CBlob *pShader;
	CBlob *pError;
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
	// Get the Backbuffer RT View
	//
	m_pRTView = pTest->GetRTView();
	if (m_pRTView == NULL)
		return false;

	m_pRTView->AddRef();

	//
	// Create VS
	//
	hr = pTest->GetFramework()->CompileShaderFromMemory( szVS, strlen( szVS ), "main", "vs_4_0", 0, &pShader, &pError );

	if( pError )
	{
		pTest->WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
	}

	if( FAILED( hr ) || pError )
	{
		pTest->WriteToLog( _T( "CPresentHelper::Setup() CompileShaderFromMemory() unexpectedly failed for VS - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		goto FAIL;
	}

	if( FAILED( hr = m_pDevice->CreateVertexShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), &m_pVS ) ) )
	{
		pTest->WriteToLog( _T( "CPresentHelper::Setup() - CreateVertexShader failed : %s" ), D3DHResultToString( hr ).c_str() );
		goto FAIL;
	}

	SAFE_RELEASE( pShader );
	SAFE_RELEASE( pError );

	//
	// Create PS
	//
	hr = pTest->GetFramework()->CompileShaderFromMemory( szPS, strlen( szPS ), "main", "ps_4_0", 0, &pShader, &pError );

	if( pError )
	{
		pTest->WriteToLog( _T( "%s" ), pError->GetBufferPointer() );
	}

	if( FAILED( hr ) || pError )
	{
		pTest->WriteToLog( _T( "CPresentHelper::Setup CompileShaderFromMemory() unexpectedly failed for PS - result code '%s'" ), D3DHResultToString( hr ).c_str() );
		goto FAIL;
	}

	if( FAILED( hr = m_pDevice->CreatePixelShader( pShader->GetBufferPointer(), pShader->GetBufferSize(), &m_pPS ) ) )
	{
		pTest->WriteToLog( _T( "CPresentHelper::Setup - CreatePixelShader failed : %s" ), D3DHResultToString( hr ).c_str() );
		goto FAIL;
	}

	//
	// Create Sampler
	//
	D3D_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));
	sampDesc.AddressU = D3D_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressV = D3D_TEXTURE_ADDRESS_CLAMP;
	sampDesc.AddressW = D3D_TEXTURE_ADDRESS_CLAMP;
	sampDesc.ComparisonFunc = D3D_COMPARISON_ALWAYS;
	sampDesc.Filter = D3D_FILTER_MIN_MAG_MIP_POINT;
	sampDesc.MaxAnisotropy = 1;
	sampDesc.MaxLOD = 0;
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
	D3D_RASTERIZER_DESC rastDesc;
	rastDesc.FillMode = D3D_FILL_SOLID;
	rastDesc.CullMode = D3D_CULL_NONE;
	rastDesc.FrontCounterClockwise = TRUE;
	rastDesc.DepthBias = 0;
	rastDesc.DepthBiasClamp = 0;
	rastDesc.SlopeScaledDepthBias = 0;
	rastDesc.DepthClipEnable = FALSE;
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

void CPresentHelper::PresentTexture2D(CTexture2D *pTexture2D, CShaderResourceView *pTextureView)
{
	CTexture2D *pSRVTex2D = NULL;
	CShaderResourceView *pSRV = NULL;

	if (pTextureView != NULL)
	{
		pSRV = pTextureView;
		pTextureView->AddRef();
	}
	else
	{
		//
		//Make sure the texture can be used as a Shader Resource View
		//Otherwise create a new texture and copy from original
		//
		D3D_TEXTURE2D_DESC desc;
		pTexture2D->GetDesc(&desc);
		if (desc.BindFlags & D3D_BIND_SHADER_RESOURCE)
		{
			pSRVTex2D = pTexture2D;
			pTexture2D->AddRef();
		}
		else
		{
		}

		//
		//Create a Shader Resource View
		//
		D3D_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = desc.Format;
		srvDesc.ViewDimension = D3D_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Texture2D.MostDetailedMip = 0;
		srvDesc.Texture2D.MipLevels = 1;
		HRESULT hr = m_pDevice->CreateShaderResourceView(pSRVTex2D, &srvDesc, &pSRV);

		// BUGBUG: This is a bandaid to address a failure.  This function should be made more robust by handling this error and 
		// probably should return an HRESULT rather than 'void'.
		if( FAILED( hr ) )
			throw( _T( "m_pDevice->CreateShaderResourceView() failed in PresentHelper.cpp" ) );
	}
	

	//
	//Save previous objects
	//
	CVertexShader *pOriginalVS = NULL;
	CGeometryShader *pOriginalGS = NULL;
	CPixelShader *pOriginalPS = NULL;
	CSamplerState *pOriginalSampler = NULL;
	CRasterizerState *pOriginalRasterizer = NULL;
	CShaderResourceView *pOriginalSRV = NULL;
	CRenderTargetView *pOriginalRT = NULL;
	CDepthStencilView *pOriginalDS = NULL;
	CInputLayout *pOriginalInputLayout = NULL;
	D3D_PRIMITIVE_TOPOLOGY originalTopology = D3D_PRIMITIVE_TOPOLOGY_UNDEFINED;

	m_pDevice->VSGetShader(&pOriginalVS);
	m_pDevice->GSGetShader(&pOriginalGS);
	m_pDevice->PSGetShader(&pOriginalPS);
	m_pDevice->PSGetSamplers(0, 1, &pOriginalSampler);
	m_pDevice->PSGetShaderResources(0, 1, &pOriginalSRV);
	m_pDevice->OMGetRenderTargets(1, &pOriginalRT, &pOriginalDS);
	m_pDevice->RSGetState(&pOriginalRasterizer);
	m_pDevice->IAGetInputLayout(&pOriginalInputLayout);
	m_pDevice->IAGetPrimitiveTopology(&originalTopology);
	//D3D_VIEWPORT viewports[4];
	//UINT numViewports = 0;
	//m_pDevice->RSGetViewports(&numViewports, viewports);

	//
	//Unbind everything not needed
	//
	m_pDevice->IASetInputLayout( NULL );


	//
	//Draw a quad with the texture
	//
	m_pDevice->VSSetShader( m_pVS );
	m_pDevice->GSSetShader( NULL );
	m_pDevice->PSSetShader( m_pPS );
	m_pDevice->PSSetSamplers(0, 1, &m_pSampler);
	m_pDevice->OMSetRenderTargets(1, &m_pRTView, NULL);
	m_pDevice->PSSetShaderResources(0, 1, &pSRV);
	m_pDevice->RSSetState(m_pRasterizer);
	m_pDevice->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);


	m_pDevice->Draw(4, 0);

	m_pTest->GetFramework()->Present(NULL, NULL, NULL, NULL, 0, 0);

	//
	//Reset original objects
	//
	m_pDevice->VSSetShader(pOriginalVS);
	m_pDevice->GSSetShader(pOriginalGS);
	m_pDevice->PSSetShader(pOriginalPS);
	m_pDevice->PSSetSamplers(0, 1, &pOriginalSampler);
	m_pDevice->PSSetShaderResources(0, 1, &pOriginalSRV);
	m_pDevice->OMSetRenderTargets(1, &pOriginalRT, pOriginalDS);
	m_pDevice->RSSetState(pOriginalRasterizer);
	m_pDevice->IASetInputLayout( pOriginalInputLayout );
	m_pDevice->IASetPrimitiveTopology(originalTopology);

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


	SAFE_RELEASE( pSRVTex2D );
	SAFE_RELEASE( pSRV );
}
