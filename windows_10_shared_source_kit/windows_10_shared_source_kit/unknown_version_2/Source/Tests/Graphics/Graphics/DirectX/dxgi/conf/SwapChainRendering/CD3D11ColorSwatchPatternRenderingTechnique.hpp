//////////////////////////////////////////////////////////////////////
// File:  CD3D11ColorSwatchPatternRenderingTechnique.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for rendering technique that renders a pattern of
// colored swatches.
//
// History:
// 19 Aug 2015  SEdmison  Factored out of CD3D11SceneRenderer.hpp.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Pragmas
//
/////////////////////////////////////////////////////////////////

#pragma once


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "RenderingTypes.hpp"


/////////////////////////////////////////////////////////////////
//
// Interfaces
//
/////////////////////////////////////////////////////////////////

class CD3D11SceneRenderer;


////////////////////////////////////////////////////////////
//
// Class interfaces
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CD3D11ColorSwatchPatternRenderingTechnique
//
// Purpose:
// Encapsulates a technique for rendering a pattern of
// colored swatches.
///////////////////////////////////////////////////////
class CD3D11ColorSwatchPatternRenderingTechnique : public CD3D11RenderingTechnique
{
	//
	// Construction/destruction
	//

	public:
		// Constructors.
		CD3D11ColorSwatchPatternRenderingTechnique
		(
			_In_ CD3D11SceneRenderer * pParentRenderer
		);


		// Destructor.
		virtual ~CD3D11ColorSwatchPatternRenderingTechnique();


	//
	// CRenderingTechnique methods
	//

	public:
		HRESULT CreateResources
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot
		);

		void DestroyResources();


	//
	// CD3D11RenderingTechnique methods
	//

	public:
		HRESULT RenderScene
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot
		);


	protected:
		HRESULT InitializeSwapChainBuffers
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot,
			DXGI_FORMAT SwapChainBufferFormat,
			DXGI_ALPHA_MODE AlphaMode
		);


	//
	// Static data
	//

	private:
		static COLOR_SWATCH s_BackgroundSwapChainColorSwatches[];

		static COLOR_SWATCH s_ForegroundSwapChainColorSwatches[];
};
