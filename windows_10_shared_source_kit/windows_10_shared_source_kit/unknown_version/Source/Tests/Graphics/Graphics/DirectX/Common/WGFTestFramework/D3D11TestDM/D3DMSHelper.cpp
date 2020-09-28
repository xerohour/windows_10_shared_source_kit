//////////////////////////////////////////////////////////////////////////
//  D3DMSHelper.h
//  created:	2006/10/03
//
//  purpose: Helper class to read and write to MS buffers
//////////////////////////////////////////////////////////////////////////

#include "pch.h"

#include "D3DSampleLocation.h"
#include "D3DMSHelper.h"
#include "D3D11Test.h"
#include "FormatHelper.h"

// This may be increased if some test requires more single-sampled slices for read/write
#define SLICE_COUNT 512

char szVS[] = 
	"struct VSIn																\n"
	"{																			\n"
	"	float2 pos : pos;														\n"
	"};																			\n"
	"struct VSOut																\n"
	"{																			\n"
	"	float4 pos : SV_Position;												\n"
	"};																			\n"
	"VSOut main(VSIn input)														\n"
	"{																			\n"
	"	VSOut output;															\n"
	"	output.pos = float4( input.pos, 0, 1.0 );							    \n"
	"	return output;															\n"
	"}";

char szGS[] = 
	"struct GSIn																\n"
	"{																			\n"
	"	float4 pos : SV_Position;												\n"
	"};																			\n"
	"struct GSOut																\n"
	"{																			\n"
	"   float4 pos : SV_Position;												\n"
	"   uint RTIndex : SV_RenderTargetArrayIndex;								\n"
	"};																			\n"
	"cbuffer cb0 : register(b0)													\n"
	"{																			\n"
	"	uint RTIndex;															\n"
	"};																			\n"
	"[maxvertexcount (3)]														\n"
	"void main(inout TriangleStream<GSOut> Stream, triangle GSIn input[3])		\n"
	"{																			\n"
	"	GSOut output;															\n"
	"   output.RTIndex = RTIndex;												\n"
	"	output.pos = input[0].pos;												\n"
	"   Stream.Append(output);													\n"
	"	output.pos = input[1].pos;												\n"
	"   Stream.Append(output);													\n"
	"	output.pos = input[2].pos;												\n"
	"	Stream.Append(output);													\n"
	"	Stream.RestartStrip();													\n"
	"}";

char szMSWritePS[] =
	"Texture2DArray<%s> MyTex;													\n"
	"struct PSIn																\n"
	"{																			\n"
	"	float4 pos : SV_Position;												\n"
	"};																			\n"
	"struct PSOut																\n"
	"{																			\n"
	"	%s Color : SV_Target;													\n"
	"};																			\n"
	"PSOut main(PSIn input)														\n"
	"{																			\n"
	"	PSOut output;															\n"	
	"	output.Color = MyTex.Load(int4(input.pos.xy,0,0));		   				\n"
	"	return output;															\n"
	"}";

char szMSReadPS[] =
	"Texture2DMS%s<%s, %d> MyTex;													\n"
	"struct PSIn																\n"
	"{																			\n"
	"	float4 pos : SV_Position;												\n"
	"};																			\n"
	"struct PSOut																\n"
	"{																			\n"
	"	%s Color : SV_Target;													\n"
	"};																			\n"
	"cbuffer cb0 : register(b0)													\n"
	"{																			\n"
	"	uint RTIndex;															\n"
	"};																			\n"
	"PSOut main(PSIn input)														\n"
	"{																			\n"
	"	PSOut output;															\n"	
	"	output.Color = MyTex.Load(int3(input.pos.xy,RTIndex), %d);	   			\n"
	"	return output;															\n"
	"}";
//
// MSHelper
//

tstring GetType( DXGI_FORMAT format )
{
	tstring szType;
	UINT uFlags = GetFlags( format );
	if (uFlags & FF_UINT)
		szType = tstring("uint");
	else if (uFlags & FF_SINT)
		szType = tstring("int");
	else 
		szType = tstring("float");

	UINT uComps = 0;
	for (UINT i = 0; i < 4; ++i)
		if (GetBitsPerComponent(i, format)) 
			++uComps;

	if (uComps > 1)
		szType += ToString(uComps);

	return szType;
}

// Write
bool MSHelper::Write( CD3D11TestFramework *pFramework, ID3D11Texture2D* pTex2DMS, FLOAT* pData, DXGI_FORMAT ViewFormat )
{
	assert( pFramework );
	assert( pTex2DMS );
	
	ID3D11Device			*pDevice( pFramework->GetDevice() );
	ID3D11DeviceContext		*pDeviceContext( pFramework->GetImmediateContext() );
	ID3D11Texture2D			*pTex2DData(NULL);
	ID3D11ShaderResourceView *pSRVData[SLICE_COUNT];
	ID3D11RenderTargetView	*pRTVMS(NULL);
	ID3D11VertexShader		*pVS(NULL);
	ID3D11GeometryShader		*pGS(NULL);
	ID3D11PixelShader		*pPS(NULL);
	ID3D10Blob				*pBlob(NULL);
	ID3D10Blob				*pErrorBlob(NULL);
	ID3D11InputLayout		*pIL(NULL);
	ID3D11Buffer				*pVertexBuffer(NULL);
	ID3D10StateBlock	*pStateBlock(NULL);
	ID3D11DepthStencilState  *pDSState(NULL);

	D3D11_TEXTURE2D_DESC tex2DMSDesc;
	pTex2DMS->GetDesc(&tex2DMSDesc);
	assert(tex2DMSDesc.ArraySize*tex2DMSDesc.SampleDesc.Count < SLICE_COUNT);
	memset(&pSRVData[0], 0, SLICE_COUNT*sizeof(ID3D11ShaderResourceView*));

	HRESULT hr;
	try
	{
		//
		// Setup
		//

		// validate format
		D3D11_TEXTURE2D_DESC tex2DDesc;
		pTex2DMS->GetDesc(&tex2DMSDesc);
		UINT uFlags = GetFlags(tex2DMSDesc.Format);
		UINT uViewFlags = GetFlags(ViewFormat);
		if (uViewFlags & FF_TYPELESS)
			throw TRException("Specfied view format is typeless.");
		if ((uFlags & FF_TYPELESS) && (ViewFormat == DXGI_FORMAT_UNKNOWN))
			throw TRException("Texture is typeless and no render target view format is specified.");
		if (ViewFormat == DXGI_FORMAT_UNKNOWN)
			ViewFormat = tex2DMSDesc.Format;

		// convert float data to texture data
		const UINT uSampleCount = tex2DMSDesc.SampleDesc.Count;
		const UINT uElementSize = GetBitsPerElement(tex2DMSDesc.Format)/8;
		const UINT uSlicePitch = tex2DMSDesc.Width * tex2DMSDesc.Height * uElementSize;
		std::vector<BYTE> vData(uSlicePitch * uSampleCount * tex2DMSDesc.ArraySize);
		for (UINT i = 0; i < uSampleCount * tex2DMSDesc.ArraySize; ++i)
		{
			FLOAT_COLOR *pInput = (FLOAT_COLOR*)&pData[tex2DMSDesc.Width * tex2DMSDesc.Height * i * 4];
			void *pOutput = (void*)&vData[uSlicePitch * i];
			if (!CFormatHelper::ConvertPixels( CONV_TO_FORMAT, ViewFormat, pInput, tex2DMSDesc.Height, tex2DMSDesc.Width, pOutput ))
			{
				throw TRException("CFormatHelper::ConvertPixels() failed");
			}
		}
		
		// data texture initial data
		std::vector<D3D11_SUBRESOURCE_DATA> vSRTexData(tex2DMSDesc.SampleDesc.Count * tex2DMSDesc.ArraySize);
		for (UINT i = 0; i < vSRTexData.size(); ++i)
		{
			vSRTexData[i].pSysMem = &vData[uSlicePitch * i];
			vSRTexData[i].SysMemPitch = tex2DMSDesc.Width * uElementSize;
			vSRTexData[i].SysMemSlicePitch = uSlicePitch;
		}
		
		// create data texture
		memcpy((void*)&tex2DDesc, (void*)&tex2DMSDesc, sizeof(D3D11_TEXTURE2D_DESC));
		tex2DDesc.ArraySize = tex2DMSDesc.SampleDesc.Count * tex2DMSDesc.ArraySize;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.SampleDesc.Quality = 0;
		tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		if( FAILED( hr = pDevice->CreateTexture2D( &tex2DDesc, &vSRTexData[0], &pTex2DData ) ) )
			throw TRException("CreateTexture2D", hr);

		// create shader resoure view
		D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
		srvDesc.Format = ViewFormat;
		srvDesc.Texture2DArray.ArraySize = 1;
		srvDesc.Texture2DArray.MipLevels = 1;
		srvDesc.Texture2DArray.MostDetailedMip = 0;
		srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
		for (UINT i = 0; i < tex2DDesc.ArraySize; ++i)
		{
			// array pSRVData is of length SLICE_COUNT elements.
			if( i >= SLICE_COUNT )
				throw TRException("Overflow of array pSRVData", E_FAIL);
			
			srvDesc.Texture2DArray.FirstArraySlice = i;
			if( FAILED( hr = pDevice->CreateShaderResourceView( pTex2DData, &srvDesc, &pSRVData[i] ) ) )
				throw TRException("CreateShaderResourceView", hr);
		}

		// create render target view
		D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
		rtvDesc.Format = ViewFormat;
		if (tex2DMSDesc.ArraySize > 1)
		{
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMSARRAY;
			rtvDesc.Texture2DMSArray.ArraySize = tex2DMSDesc.ArraySize;
			rtvDesc.Texture2DMSArray.FirstArraySlice = 0;
		}
		else
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DMS;

		if( FAILED( hr = pDevice->CreateRenderTargetView( pTex2DMS, &rtvDesc, &pRTVMS ) ) )
			throw TRException("CreateRenderTargetView", hr);

		// create shaders
		
		// Compile and create vertex shaders
		if ( FAILED( hr = D3DX10CompileFromMemory( szVS, sizeof(szVS), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, NULL, &pBlob, &pErrorBlob, NULL )) )
		{
			pFramework->WriteToLogF("%s", pErrorBlob->GetBufferPointer());
			throw TRException("Failed to compile VS.", hr);
		}
		if ( FAILED( hr = pDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVS )) )
			throw TRException("Failed to create VS.", hr);

		// Define our vertex data layout
		const D3D11_INPUT_ELEMENT_DESC layout[] = {{ "pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }};
		if(	FAILED(	hr = pDevice->CreateInputLayout( layout, 1, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pIL ) ) )
			throw TRException("CreateInputLayout for m_pVL Failed.", hr);

		// release for reuse
		SAFE_RELEASE( pBlob );
		SAFE_RELEASE( pErrorBlob );

		// compile and create pixel shaders
		tstring szType = GetType( ViewFormat );
		char szPS[ARRAY_SIZE(szMSWritePS) + 100];
		_snprintf( szPS, ARRAY_SIZE(szMSWritePS) + 99, szMSWritePS, szType.c_str(), szType.c_str());
		szPS[ARRAY_SIZE(szMSWritePS) + 99] = 0;
		if ( FAILED( hr = D3DX10CompileFromMemory( szPS, strlen(szPS), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, NULL, &pBlob, &pErrorBlob, NULL )) )
		{
			pFramework->WriteToLogF("%s", pErrorBlob->GetBufferPointer());
			throw TRException("Failed to compile PS.", hr);
		}
		if ( FAILED( hr = pDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPS )) )
			throw TRException("Failed to create PS.", hr);



		// const buffer initial data
		float pVBData[] = {-1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f, 1.f};  // full screen quad 
		D3D11_SUBRESOURCE_DATA srVBData;
		srVBData.pSysMem = &pVBData[0];
		srVBData.SysMemPitch = sizeof(pVBData);
		srVBData.SysMemSlicePitch = sizeof(pVBData);

		// Create vertex buffer
		D3D11_BUFFER_DESC bufDesc;
		bufDesc.ByteWidth = 4 * 2 * sizeof(float);
		bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufDesc.Usage = D3D11_USAGE_DEFAULT;
		bufDesc.CPUAccessFlags = 0;
		bufDesc.MiscFlags = 0;
		if( FAILED( hr = pDevice->CreateBuffer( &bufDesc, &srVBData, &pVertexBuffer ) ) )
			throw TRException("CreateBuffer for pVB failed.", hr);

		// create depth stencil state
		D3D11_DEPTH_STENCIL_DESC dssDesc;
		dssDesc.DepthEnable = false;
		dssDesc.StencilEnable = false;
		dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
		if( FAILED( hr = pDevice->CreateDepthStencilState( &dssDesc, &pDSState ) ) )
			throw TRException("CreateDepthStencilState failed.", hr);
			
		//
		// State
		//

		// preserve state
		// BUGBUG - chrisza: State blocks not implemented in M1
		/*D3D10_STATE_BLOCK_MASK mask;
		if( FAILED( hr = D3D10StateBlockMaskEnableAll( &mask ) ) )
			throw TRException("D3D10StateBlockMaskEnableAll", hr);
		if( FAILED( hr = D3D10CreateStateBlock( pDevice, &mask, &pStateBlock ) ) )
			throw TRException("D3D10CreateStateBlock", hr);
		if( FAILED( hr = pStateBlock->Capture() ) )
			throw TRException("pStateBlock->Capture()", hr);*/

		// unbind 
		ID3D11RenderTargetView *ppRTNull[] = { NULL };
		pDeviceContext->OMSetRenderTargets( 1, ppRTNull, NULL );		
		ID3D11ShaderResourceView *ppSRVNull[] = { NULL };
		pDeviceContext->PSSetShaderResources( 0, 1, ppSRVNull );

		// IA
		UINT uStride = 2 * sizeof(float);
		UINT uOffset = 0;
		pDeviceContext->IASetInputLayout( pIL );
		pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
		pDeviceContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &uStride, &uOffset );
		// VS
		pDeviceContext->VSSetShader( pVS, NULL, 0 );
		// GS
		pDeviceContext->GSSetShader( NULL, NULL, 0 );
		// PS
		pDeviceContext->PSSetShader( pPS, NULL, 0 );
		// RS
		D3D11_VIEWPORT vp = { 0, 0, (FLOAT) tex2DMSDesc.Width, (FLOAT) tex2DMSDesc.Height, 0, 1.f };
		pDeviceContext->RSSetViewports( 1, &vp );
		pDeviceContext->RSSetState( NULL );
		// OM
		pDeviceContext->OMSetDepthStencilState( pDSState, 0 );
		pDeviceContext->OMSetRenderTargets( 1, &pRTVMS, NULL );

		//
		// loop
		//
		for (UINT uSlice = 0; uSlice < tex2DMSDesc.ArraySize; ++uSlice)
		{
			for (UINT i = 0; i < uSampleCount; ++i)
			{
				pDeviceContext->PSSetShaderResources( 0, 1, &pSRVData[i + uSlice*uSampleCount] );
				pDeviceContext->OMSetBlendState( NULL, 0, 1 << i );
				pDeviceContext->Draw(4, 0);
			}
		}

		// unbind 
		pDeviceContext->OMSetRenderTargets( 1, ppRTNull, NULL );		
		pDeviceContext->PSSetShaderResources( 0, 1, ppSRVNull );

		// restore state
		// BUGBUG - chrisza: State blocks not implemented in M1
		/*if( FAILED( hr = pStateBlock->Apply() ) )
			throw TRException("pStateBlock->Apply()", hr);*/

	}
	catch (TRException& exc)
	{
		hr = E_FAIL;
		exc.Prepend("[MSHelper::Write()] ");
		pFramework->WriteToLog( exc.GetError().c_str() );
	}

	SAFE_RELEASE( pTex2DData );
	for (UINT i = 0; i < SLICE_COUNT; ++i) SAFE_RELEASE( pSRVData[i] );
	SAFE_RELEASE( pRTVMS );
	SAFE_RELEASE( pStateBlock );
	SAFE_RELEASE( pBlob );
	SAFE_RELEASE( pErrorBlob );
	SAFE_RELEASE( pVS );
	SAFE_RELEASE( pPS );
	SAFE_RELEASE( pIL );
	SAFE_RELEASE( pVertexBuffer );
	SAFE_RELEASE( pDSState );

	return SUCCEEDED( hr );
}

bool MSHelper::Read( CD3D11TestFramework *pFramework, ID3D11Texture2D* pTex2DMS, FLOAT* pData, DXGI_FORMAT ViewFormat )
{
	return ReadInternal( pFramework, pTex2DMS, pData, ViewFormat, true);
}

bool MSHelper::ReadWithoutConversion( CD3D11TestFramework *pFramework, ID3D11Texture2D* pTex2DMS, VOID* pData, DXGI_FORMAT ViewFormat )
{
	return ReadInternal( pFramework, pTex2DMS, pData, ViewFormat, false);
}


// Read
bool MSHelper::ReadInternal( CD3D11TestFramework *pFramework, ID3D11Texture2D* pTex2DMS, VOID* pData, DXGI_FORMAT ViewFormat, bool Convert )
{
	assert( pFramework );
	assert( pTex2DMS );
	
	ID3D11Device			*pDevice( pFramework->GetDevice() );
	ID3D11DeviceContext		*pDeviceContext( pFramework->GetImmediateContext() );
	ID3D11Texture2D			*pTex2DData(NULL);
	ID3D11Texture2D			*pTex2DStaging(NULL);
	ID3D11ShaderResourceView *pSRVData(NULL);
	ID3D11RenderTargetView	*pRTV[SLICE_COUNT];
	ID3D11VertexShader		*pVS(NULL);
	ID3D11PixelShader		*pPS[SLICE_COUNT];
	ID3D10Blob				*pBlob(NULL);
	ID3D10Blob				*pErrorBlob(NULL);
	ID3D11InputLayout		*pIL(NULL);
	ID3D11Buffer				*pVertexBuffer(NULL);
	ID3D11Buffer				*pConstBuffer(NULL);
	ID3D10StateBlock	*pStateBlock(NULL);
	ID3D11DepthStencilState  *pDSState(NULL);

	D3D11_TEXTURE2D_DESC tex2DMSDesc;
	pTex2DMS->GetDesc(&tex2DMSDesc);
	UINT uSliceCount = tex2DMSDesc.SampleDesc.Count * tex2DMSDesc.ArraySize;
	memset(&pRTV[0], 0, SLICE_COUNT*sizeof(ID3D11RenderTargetView*));
	memset(&pPS[0], 0, SLICE_COUNT*sizeof(ID3D11PixelShader*));

	// Mask Tiled flag in case pTex2DMS is a tiled resource
	tex2DMSDesc.MiscFlags &= ~D3D11_RESOURCE_MISC_TILED;

	HRESULT hr;
	try
	{
		//
		// Setup
		//

		ID3D11RenderTargetView *ppRTNull[] = { NULL };
		ID3D11ShaderResourceView *ppSRVNull[] = { NULL };

		// validate format
		D3D11_TEXTURE2D_DESC tex2DDesc;
		UINT uFlags = GetFlags(tex2DMSDesc.Format);
		UINT uViewFlags = GetFlags(ViewFormat);
		if (uViewFlags & FF_TYPELESS)
			throw TRException("Specfied view format is typeless.");
		if ((uFlags & FF_TYPELESS) && (ViewFormat == DXGI_FORMAT_UNKNOWN))
			throw TRException("Texture is typeless and no render target view format is specified.");
		if (ViewFormat == DXGI_FORMAT_UNKNOWN)
			ViewFormat = tex2DMSDesc.Format;

		// create data texture
		memcpy((void*)&tex2DDesc, (void*)&tex2DMSDesc, sizeof(D3D11_TEXTURE2D_DESC));
		tex2DDesc.ArraySize = uSliceCount;
		tex2DDesc.SampleDesc.Count = 1;
		tex2DDesc.SampleDesc.Quality = 0;
		tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
		if( FAILED( hr = pDevice->CreateTexture2D( &tex2DDesc, NULL, &pTex2DData ) ) )
			throw TRException("CreateTexture2D for render target tex", hr);

		// create data texture copy
		tex2DDesc.BindFlags = 0;
		tex2DDesc.Usage = D3D11_USAGE_STAGING;
		tex2DDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		if( FAILED( hr = pDevice->CreateTexture2D( &tex2DDesc, NULL, &pTex2DStaging ) ) )
			throw TRException("CreateTexture2D for staging tex", hr);

		if (tex2DMSDesc.SampleDesc.Count > 1)
		{
			// create shader resoure view
			D3D11_SHADER_RESOURCE_VIEW_DESC srvDesc;
			srvDesc.Format = ViewFormat;
			if (tex2DMSDesc.ArraySize > 1)
			{
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMSARRAY;
				srvDesc.Texture2DMSArray.ArraySize = tex2DMSDesc.ArraySize;
				srvDesc.Texture2DMSArray.FirstArraySlice = 0;
			}
			else
				srvDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DMS;
			if( FAILED( hr = pDevice->CreateShaderResourceView( pTex2DMS, &srvDesc, &pSRVData ) ) )
				throw TRException("CreateShaderResourceView", hr);

			// create render target view
			D3D11_RENDER_TARGET_VIEW_DESC rtvDesc;
			rtvDesc.Format = ViewFormat;
			rtvDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
			rtvDesc.Texture2DArray.ArraySize = 1;
			rtvDesc.Texture2DArray.MipSlice = 0;
			for (UINT i = 0; i < uSliceCount; ++i)
			{
				// array pRTV is of length SLICE_COUNT elements.
				if( i >= SLICE_COUNT )
					throw TRException("Overflow of array pRTV", E_FAIL);
				rtvDesc.Texture2DArray.FirstArraySlice  = i;
				if( FAILED( hr = pDevice->CreateRenderTargetView( pTex2DData, &rtvDesc, &pRTV[i] ) ) )
					throw TRException("CreateRenderTargetView", hr);
			}
		
			// create shaders
			
			// Compile and create vertex shaders
			if ( FAILED( hr = D3DX10CompileFromMemory( szVS, sizeof(szVS), NULL, NULL, NULL, "main", "vs_4_0", 0, 0, NULL, &pBlob, &pErrorBlob, NULL )) )
			{
				pFramework->WriteToLogF("%s", pErrorBlob->GetBufferPointer());
				throw TRException("Failed to compile VS.", hr);
			}
			if ( FAILED( hr = pDevice->CreateVertexShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pVS )) )
				throw TRException("Failed to create VS.", hr);

			// Define our vertex data layout
			const D3D11_INPUT_ELEMENT_DESC layout[] = {{ "pos", 0, DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 }};
			if(	FAILED(	hr = pDevice->CreateInputLayout( layout, 1, pBlob->GetBufferPointer(), pBlob->GetBufferSize(), &pIL ) ) )
				throw TRException("CreateInputLayout for m_pVL Failed.", hr);

			// compile and create pixel shaders
			tstring szType = GetType( ViewFormat );
			tstring szArray = tex2DMSDesc.ArraySize > 1 ? tstring("Array") : tstring(" ");
			for (UINT i = 0; i < tex2DMSDesc.SampleDesc.Count; ++i)
			{
				// release for reuse
				SAFE_RELEASE( pBlob );
				SAFE_RELEASE( pErrorBlob );

				// rewrite shader based on sample to load
				char szPS[ARRAY_SIZE(szMSReadPS) + 100];
				_snprintf( szPS, ARRAY_SIZE(szMSReadPS) + 99, szMSReadPS, szArray.c_str(), szType.c_str(), tex2DMSDesc.SampleDesc.Count, szType.c_str(), i);
				szPS[ARRAY_SIZE(szMSReadPS) + 99] = 0;
				if ( FAILED( hr = D3DX10CompileFromMemory( szPS, strlen(szPS), NULL, NULL, NULL, "main", "ps_4_0", 0, 0, NULL, &pBlob, &pErrorBlob, NULL )) )
				{
					pFramework->WriteToLogF("%s", pErrorBlob->GetBufferPointer());
					throw TRException("Failed to compile PS.", hr);
				}
				if ( FAILED( hr = pDevice->CreatePixelShader( pBlob->GetBufferPointer(), pBlob->GetBufferSize(), NULL, &pPS[i] )) )
					throw TRException("Failed to create PS.", hr);
			}

			// vertex buffer initial data
			float pVBData[] = {-1.f, -1.f, -1.f, 1.f, 1.f, -1.f, 1.f, 1.f};  // full screen quad 
			D3D11_SUBRESOURCE_DATA srVBData;
			srVBData.pSysMem = &pVBData[0];
			srVBData.SysMemPitch = sizeof(pVBData);
			srVBData.SysMemSlicePitch = sizeof(pVBData);

			// Create vertex buffer
			D3D11_BUFFER_DESC bufDesc;
			bufDesc.ByteWidth = 4 * 2 * sizeof(float);
			bufDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
			bufDesc.Usage = D3D11_USAGE_DEFAULT;
			bufDesc.CPUAccessFlags = 0;
			bufDesc.MiscFlags = 0;
			if( FAILED( hr = pDevice->CreateBuffer( &bufDesc, &srVBData, &pVertexBuffer ) ) )
				throw TRException("CreateBuffer for pVertexBuffer failed.", hr);
		
			// creat const buffer
			bufDesc.ByteWidth = 16;
			bufDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
			bufDesc.Usage = D3D11_USAGE_DYNAMIC;
			bufDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
			bufDesc.MiscFlags = 0;
			if( FAILED( hr = pDevice->CreateBuffer( &bufDesc, NULL, &pConstBuffer ) ) )
				throw TRException("CreateBuffer for pConstBuffer failed.", hr);

			// create depth stencil state
			D3D11_DEPTH_STENCIL_DESC dssDesc;
			dssDesc.DepthEnable = false;
			dssDesc.StencilEnable = false;
			dssDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
			if( FAILED( hr = pDevice->CreateDepthStencilState( &dssDesc, &pDSState ) ) )
				throw TRException("CreateDepthStencilState failed.", hr);
				
			//
			// State
			//

			// preserve state
			// BUGBUG - chrisza: State blocks not implemented in M1
			/*D3D10_STATE_BLOCK_MASK mask;
			if( FAILED( hr = D3D10StateBlockMaskEnableAll( &mask ) ) )
				throw TRException("D3D10StateBlockMaskEnableAll", hr);
			if( FAILED( hr = D3D10CreateStateBlock( pDevice, &mask, &pStateBlock ) ) )
				throw TRException("D3D10CreateStateBlock", hr);
			if( FAILED( hr = pStateBlock->Capture() ) )
				throw TRException("pStateBlock->Capture()", hr);*/


			// unbind 
			pDeviceContext->OMSetRenderTargets( 1, ppRTNull, NULL );		
			pDeviceContext->PSSetShaderResources( 0, 1, ppSRVNull );

			// IA
			UINT uStride = 2 * sizeof(float);
			UINT uOffset = 0;
			pDeviceContext->IASetInputLayout( pIL );
			pDeviceContext->IASetPrimitiveTopology( D3D11_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP );
			pDeviceContext->IASetVertexBuffers( 0, 1, &pVertexBuffer, &uStride, &uOffset );
			// VS
			pDeviceContext->VSSetShader( pVS, NULL, 0 );
			// GS
			pDeviceContext->GSSetShader( NULL, NULL, 0 );
			// PS
			pDeviceContext->PSSetConstantBuffers( 0, 1, &pConstBuffer );
			pDeviceContext->PSSetShaderResources( 0, 1, &pSRVData );
			// RS
			D3D11_VIEWPORT vp = { 0, 0, (FLOAT) tex2DMSDesc.Width, (FLOAT) tex2DMSDesc.Height, 0, 1.f };
			pDeviceContext->RSSetViewports( 1, &vp );
			pDeviceContext->RSSetState( NULL );
			// OM
			pDeviceContext->OMSetDepthStencilState( pDSState, 0 );
			pDeviceContext->OMSetBlendState( NULL, 0, 0xffffffff );

			//
			// loop
			//
			for (UINT uSlice = 0; uSlice < tex2DMSDesc.ArraySize; ++uSlice)
			{
				D3D11_MAPPED_SUBRESOURCE mappedRes;
				if( FAILED( hr = pDeviceContext->Map( pConstBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedRes ) ) )
					throw TRException( "Map failed for pConstBuffer.", hr );
				memcpy( mappedRes.pData, &uSlice, sizeof(uSlice) );
				pDeviceContext->Unmap(pConstBuffer,0);
				for (UINT i = 0; i < tex2DMSDesc.SampleDesc.Count; ++i)
				{
					pDeviceContext->PSSetShader( pPS[i], NULL, 0 );
					pDeviceContext->OMSetRenderTargets( 1, &pRTV[i + uSlice*tex2DMSDesc.SampleDesc.Count], NULL );			
					pDeviceContext->Draw(4, 0);
				}
			}

			// unbind 
			pDeviceContext->OMSetRenderTargets( 1, ppRTNull, NULL );		
			pDeviceContext->PSSetShaderResources( 0, 1, ppSRVNull );

			// BUGBUG - chrisza: State blocks not implemented in M1
			/*if( FAILED( hr = pStateBlock->Apply() ) )
				throw TRException("pStateBlock->Apply()", hr);*/

			// convert from DXGI_FORMAT to float4
			pDeviceContext->CopyResource(pTex2DStaging, pTex2DData);
		}
		else
		{
			// convert from DXGI_FORMAT to float4
			pDeviceContext->CopyResource(pTex2DStaging, pTex2DMS);
		}

		// convert staging resource to float4s if requested
		UINT uDataRowPitch;

		if( Convert )
		{
			uDataRowPitch = tex2DDesc.Width*4*sizeof(FLOAT);
		}
		else
		{
			UINT uBpe = GetBitsPerElement( ViewFormat );
			uDataRowPitch = tex2DDesc.Width*uBpe/8;
		}

		UINT uDataSlicePitch = tex2DDesc.Height*uDataRowPitch;

		for (UINT uSlice = 0; uSlice < uSliceCount; ++uSlice)
		{
			D3D11_MAPPED_SUBRESOURCE texMap;
			if( FAILED( hr = pDeviceContext->Map( pTex2DStaging, uSlice, D3D11_MAP_READ, 0, &texMap ) ) )
				throw TRException("pDeviceContext->Map()", hr);
			for (UINT uRow = 0; uRow < tex2DDesc.Height; ++uRow)
			{
				void *pInput = (void*)((BYTE*)texMap.pData + uRow*texMap.RowPitch);
				VOID *pDst = ((BYTE*)pData + uRow*uDataRowPitch + uSlice*uDataSlicePitch);
				
				if( Convert )
				{
					FLOAT_COLOR *pOutput = (FLOAT_COLOR*)pDst;
					if ( !CFormatHelper::ConvertPixels( CONV_FROM_FORMAT, ViewFormat, pOutput, 1, tex2DMSDesc.Width, pInput ) )
					{
						throw TRException("CFormatHelper::ConvertPixels() failed");
					}
				}
				else
				{
					memcpy( pDst, pInput, uDataRowPitch );
				}
			}
			pDeviceContext->Unmap(pTex2DStaging, uSlice);
		}
	}
	catch (TRException& exc)
	{
		hr = E_FAIL;
		exc.Prepend("[MSHelper::Write()] ");
		pFramework->WriteToLog( exc.GetError().c_str() );
	}

	SAFE_RELEASE( pTex2DData );
	SAFE_RELEASE( pTex2DStaging );
	SAFE_RELEASE( pSRVData );
	for (UINT i = 0; i < uSliceCount; ++i) SAFE_RELEASE( pRTV[i] );
	SAFE_RELEASE( pStateBlock );
	SAFE_RELEASE( pBlob );
	SAFE_RELEASE( pErrorBlob );
	SAFE_RELEASE( pVS );
	for (UINT i = 0; i < tex2DMSDesc.SampleDesc.Count; ++i) SAFE_RELEASE( pPS[i] );
	SAFE_RELEASE( pIL );
	SAFE_RELEASE( pVertexBuffer );
	SAFE_RELEASE( pConstBuffer );
	SAFE_RELEASE( pDSState );

	return SUCCEEDED( hr );
}

