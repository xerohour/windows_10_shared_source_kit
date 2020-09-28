//////////////////////////////////////////////////////////////////////
// File:  CD3D11ClearRenderingTechnique.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Purpose:
// Implementation of a Direct3D 11 rendering technique that clears the
// render target to a solid color.
//
// History:
// 19 Aug 2015  SEdmison  Factored out of CD3D11SceneRenderer.cpp.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "CD3D11ClearRenderingTechnique.hpp"
#include "CD3D11SceneRenderer.hpp"


/////////////////////////////////////////////////////////////////
//
// CD3D11SceneRenderer implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D11ClearRenderingTechnique
//
// Purpose:
// CD3D11ClearRenderingTechnique.
///////////////////////////////////////////////////////
CD3D11ClearRenderingTechnique::CD3D11ClearRenderingTechnique
(
	_In_ CD3D11SceneRenderer * pParentRenderer
)
:
	CD3D11RenderingTechnique( pParentRenderer )
{
};


///////////////////////////////////////////////////////
// Function:  ~CD3D11ClearRenderingTechnique
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CD3D11ClearRenderingTechnique::~CD3D11ClearRenderingTechnique()
{
};


////////////////////////////////////////////////////////////
//
// Public method implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CreateResources
//
// Purpose:
// Creates and initializes resources for the rendering
// technique.
///////////////////////////////////////////////////////
HRESULT CD3D11ClearRenderingTechnique::CreateResources
(
	SWAPCHAIN_SLOT /* CurrentSwapChainSlot */
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Initialize swap chain buffers.
	//

	for
	(
		SWAPCHAIN_SLOT CurrentSwapChainSlot = SWAPCHAIN_SLOT_FIRST;
		CurrentSwapChainSlot < SWAPCHAIN_SLOT_COUNT;
		CurrentSwapChainSlot = (SWAPCHAIN_SLOT) ( CurrentSwapChainSlot + 1 )
	)
	{
		// Skip empty swap chain slots.
		if ( m_pParentRenderer->m_SwapChainTypes[ CurrentSwapChainSlot] == SWAPCHAIN_TYPE_NONE )
		{
			continue;
		};


		// We need to already have determined the swap chain buffer format.
		assert( m_pParentRenderer->m_SwapChainBufferFormats[ CurrentSwapChainSlot ] != DXGI_FORMAT_UNKNOWN );


		HRESULT hInitializeSwapChainBuffersResult = InitializeSwapChainBuffers
		(
			CurrentSwapChainSlot,
			m_pParentRenderer->m_SwapChainBufferFormats[ CurrentSwapChainSlot ],
			( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? DXGI_ALPHA_MODE_PREMULTIPLIED : DXGI_ALPHA_MODE_STRAIGHT
		);

		if ( FAILED( hInitializeSwapChainBuffersResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hInitializeSwapChainBuffersResult,
				L"InitializeSwapChainBuffers failed."
			);

			hOverallResult = hInitializeSwapChainBuffersResult;
			goto Cleanup;
		};
	};


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  DestroyResources
//
// Purpose:
///////////////////////////////////////////////////////
void CD3D11ClearRenderingTechnique::DestroyResources()
{
};


///////////////////////////////////////////////////////
// Function:  RenderScene
//
// Purpose:
// Clears the render target to a solid color.
///////////////////////////////////////////////////////
HRESULT CD3D11ClearRenderingTechnique::RenderScene
(
	SWAPCHAIN_SLOT CurrentSwapChainSlot
)
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	RECT WindowClientRectangle = { 0, 0, 0, 0 };

	ID3D11Texture2D * pBackBuffer = nullptr;

	ID3D11RenderTargetView * pRenderTargetView = nullptr;


	//
	// Clear the render target for this swap chain.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11ClearRenderingTechnique::RenderScene:  Clearing render target."
	);

	HRESULT hClearRenderTargetResult = m_pParentRenderer->ClearRenderTarget
	(
		CurrentSwapChainSlot
	);

	if ( FAILED( hClearRenderTargetResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hClearRenderTargetResult,
			L"ClearRenderTarget failed."
		);

		hOverallResult = hClearRenderTargetResult;
		goto Cleanup;
	};


	// If we made it this far, then we're good to go.
	hOverallResult = S_OK;


Cleanup:
	SAFE_RELEASE( pBackBuffer );

	SAFE_RELEASE( pRenderTargetView );


	return hOverallResult;
};
