//////////////////////////////////////////////////////////////////////
// File:  CD3D12ColorSwatchPatternRenderingTechnique.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for rendering technique that renders a pattern of
// colored swatches.
//
// History:
// 12 Aug 2015  SEdmison  Factoured out of CD3D12SceneRenderer.hpp.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Pragmas
//
/////////////////////////////////////////////////////////////////

#pragma once


/////////////////////////////////////////////////////////////////
//
// Build flags
//
/////////////////////////////////////////////////////////////////

//#define COMPILE_SHADERS_AT_RUNTIME 1


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

class CD3D12SceneRenderer;


////////////////////////////////////////////////////////////
//
// Class interfaces
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Class:  CD3D12ColorSwatchPatternRenderingTechnique
//
// Purpose:
// Encapsulates a technique for rendering a pattern of
// colored swatches.
///////////////////////////////////////////////////////
class CD3D12ColorSwatchPatternRenderingTechnique : public CD3D12RenderingTechnique
{
	//
	// Construction/destruction
	//

	public:
		// Constructors.
		CD3D12ColorSwatchPatternRenderingTechnique
		(
			_In_ CD3D12SceneRenderer * pParentRenderer
		);


		// Destructor.
		virtual ~CD3D12ColorSwatchPatternRenderingTechnique();


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
	// CD3D12RenderingTechnique methods
	//

	public:
		HRESULT RenderScene
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot,
			_In_ ID3D12Resource * pCurrentSwapChainBuffer
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


	//
	// Member data
	//

	private:
		//
		// Colored-swatch rendering state.
		//

		ID3D12PipelineState * m_pColorSwatchGraphicsPipelineState;
};
