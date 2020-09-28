//////////////////////////////////////////////////////////////////////
// File:  CD3D12RenderingTechnique.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a base class for Direct3D 12 rendering
// techniques.
//
// History:
// 24 Aug 2015  SEdmison  Factored out base class object.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CD3D12SceneRenderer.hpp"


/////////////////////////////////////////////////////////////////
//
// CD3D12RenderingTechnique implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D12RenderingTechnique
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D12RenderingTechnique::CD3D12RenderingTechnique
(
	_In_ CD3D12SceneRenderer * pParentRenderer
)
:
	CRenderingTechnique( pParentRenderer->m_pLogger ),
	m_pParentRenderer( pParentRenderer )
{
};


////////////////////////////////////////////////////////////
//
// Helper methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  InitializeSwapChainBuffers
//
// Purpose:
// Initializes swap chain buffer texture array slices.
///////////////////////////////////////////////////////
HRESULT CD3D12RenderingTechnique::InitializeSwapChainBuffers
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot,
	DXGI_FORMAT SwapChainBufferFormat,
	DXGI_ALPHA_MODE AlphaMode
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	IDXGIResource * pSwapChainBufferBufferArray_IDXGIResource = nullptr;

	GUID VideoDecoderProfileID;
	ZeroMemory( &VideoDecoderProfileID, sizeof( VideoDecoderProfileID ) );

	BOOL bFoundSupportedVideoDecoderProfile = FALSE;

#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
	ID3D11VideoDecoderOutputView * pVideoDecoderOutputView = nullptr;
#endif


	//
	// Validate members and state.
	//

	// This work is only needed for decode swap chains.
	if ( m_pParentRenderer->m_SwapChainTypes[ CurrentSwapChainSlot ] != SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE )
	{
		hOverallResult = S_FALSE;
		goto Cleanup;
	};


#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
	//
	// Get the video decoder profile count.
	//

	assert( m_pDevice_ID3D11VideoDevice != nullptr );
	const UINT VideoDecoderProfileCount = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->GetVideoDecoderProfileCount();


	//
	// Find a decoder profile to use.
	//

	// If the count is 0, and we call GetVideoDecoderProfile, the
	// runtime will crash, so we'll fail instead.
	if ( 0 == VideoDecoderProfileCount )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"GetVideoDecoderProfileCount returned 0."
		);

		goto Cleanup;
	};

	for
	(
		UINT iCurrentVideoDecoderProfile = 0;
		iCurrentVideoDecoderProfile < VideoDecoderProfileCount &&
			bFoundSupportedVideoDecoderProfile != TRUE;
		++iCurrentVideoDecoderProfile
	)
	{
		HRESULT hGetVideoDecoderProfileResult = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->GetVideoDecoderProfile
		(
			iCurrentVideoDecoderProfile,
			&VideoDecoderProfileID
		);

		if ( FAILED( hGetVideoDecoderProfileResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hGetVideoDecoderProfileResult,
				L"GetVideoDecoderProfile failed."
			);

			goto Cleanup;
		};

		HRESULT hCheckVideoDecoderFormatResult = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->CheckVideoDecoderFormat
		(
			&VideoDecoderProfileID,
			SwapChainBufferFormat,
			&bFoundSupportedVideoDecoderProfile
		);

		if ( FAILED( hCheckVideoDecoderFormatResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCheckVideoDecoderFormatResult,
				L"CheckVideoDecoderFormat failed."
			);

			goto Cleanup;
		};
	};


	// If we got here and we still haven't found a supported
	// video decoder profile, something is very wrong, because
	// CheckFormatSupport indicated DECODER_OUTPUT support.
	if ( bFoundSupportedVideoDecoderProfile != TRUE )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"CheckFormatSupport indicated decoder output support for this format, but no supported video decoder profile was found."
		);

		goto Cleanup;
	};


	//
	// Log the decoder profile that we selected.
	//
	// N.B. [SEdmison]:  These IDs are defined in dxva2api.h.
	//

	WCHAR GUIDFormattingBuffer[ 64 ] = L"";
	int nGUIDFormattingBuffer = StringFromGUID2
	(
		VideoDecoderProfileID,
		GUIDFormattingBuffer,
		ARRAYSIZE( GUIDFormattingBuffer )
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"- Using decoder profile %s.",
			GUIDFormattingBuffer
		)
	);


	//
	// Create video decoder views on the swap chain decode buffers.
	//

	for
	(
		UINT iCurrentSwapChainBuffer = 0;
		iCurrentSwapChainBuffer < m_pParentRenderer->m_cSwapChainBufferTextureArraySlices[ CurrentSwapChainSlot ];
		++iCurrentSwapChainBuffer
	)
	{
		//
		// Convert normalized RGBA buffer colors to a colors that
		// can be passed to ClearView.
		//

		float BackgroundClearViewColor[ 4 ] = { 0.f, 0.f, 0.f, 1.f };
		HRESULT hInitializeBackgroundClearViewColorResult = InitializeClearViewColor
		(
			SwapChainBufferFormat,
			AlphaMode,
			CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ],
			BackgroundClearViewColor
		);

		if ( FAILED( hInitializeBackgroundClearViewColorResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hInitializeBackgroundClearViewColorResult,
				L"InitializeClearViewColor with background color failed."
			);

			goto Cleanup;
		};


		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			WEX::Common::String().Format
			(
				L"  - Converted background color from normalized RGB color (%f, %f, %f, %f) to clear color (%f, %f, %f, %f).",
				CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].r,
				CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].g,
				CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].b,
				CD3D12SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].a,
				BackgroundClearViewColor[ 0 ],
				BackgroundClearViewColor[ 1 ],
				BackgroundClearViewColor[ 2 ],
				BackgroundClearViewColor[ 3 ]
			)
		);


		//
		// Create a video output view for this array slice.
		//

		D3D11_VIDEO_DECODER_OUTPUT_VIEW_DESC VideoOutputViewDescriptor;
		ZeroMemory( &VideoOutputViewDescriptor, sizeof( VideoOutputViewDescriptor ) );
		VideoOutputViewDescriptor.DecodeProfile = VideoDecoderProfileID;
		VideoOutputViewDescriptor.ViewDimension = D3D11_VDOV_DIMENSION_TEXTURE2D;
		VideoOutputViewDescriptor.Texture2D.ArraySlice = iCurrentSwapChainBuffer;


		assert( m_pParentRenderer->m_pDevice_ID3D11VideoDevice != nullptr );
		assert( pVideoDecoderOutputView == nullptr );
		HRESULT hCreateVideoDecoderOutputViewResult = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->CreateVideoDecoderOutputView
		(
			m_ppSwapChainBufferTextureArrays[ CurrentSwapChainSlot ],
			&VideoOutputViewDescriptor,
			&pVideoDecoderOutputView
		);

		if ( FAILED( hCreateVideoDecoderOutputViewResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateVideoDecoderOutputViewResult,
				L"CreateVideoDecoderOutputView failed."
			);

			goto Cleanup;
		};

		if ( pVideoDecoderOutputView == nullptr )
		{
			LOG_ERROR
			(
				ERROR_TYPE_BOOL,
				false,
				L"CreateVideoDecoderOutputView succeeded but returned NULL video decoder output view pointer."
			);

			goto Cleanup;
		};


		//
		// Clear the entire buffer to the background color.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"  - Clearing buffer %u to (%f, %f, %f, %f).",
				iCurrentSwapChainBuffer,
				BackgroundClearViewColor[ 0 ],
				BackgroundClearViewColor[ 1 ],
				BackgroundClearViewColor[ 2 ],
				BackgroundClearViewColor[ 3 ]
			)
		);

		assert( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 != nullptr );
		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->ClearView
		(
			pVideoDecoderOutputView,
			BackgroundClearViewColor,
			nullptr,
			0
		);


		SAFE_RELEASE( pVideoDecoderOutputView );
	};


	hOverallResult = S_OK;
#endif


Cleanup:
#if TEST_DECODE_SWAP_CHAINS_UNDER_D3D12
	SAFE_RELEASE( pVideoDecoderOutputView );
#endif

	return hOverallResult;
};
