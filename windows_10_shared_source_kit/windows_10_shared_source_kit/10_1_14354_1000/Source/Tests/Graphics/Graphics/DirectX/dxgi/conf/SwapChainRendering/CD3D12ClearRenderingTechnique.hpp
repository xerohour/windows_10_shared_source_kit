//////////////////////////////////////////////////////////////////////
// File:  CD3D12ClearRenderingTechnique.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for the world's most trivial rendering technique:  It
// clears the render target to a solid color.
//
// History:
// 12 Aug 2015  SEdmison  Factored out of CD3D12SceneRenderer.hpp.
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

////////////////////////////////////////////////////////////
//
// Class interfaces
//
////////////////////////////////////////////////////////////

class CD3D12SceneRenderer;


///////////////////////////////////////////////////////
// Class:  CD3D12ClearRenderingTechnique
//
// Purpose:
// Encapsulates a rendering technique that merely
// clears the render target to a solid color.
///////////////////////////////////////////////////////
class CD3D12ClearRenderingTechnique : public CD3D12RenderingTechnique
{
	//
	// Construction/destruction
	//

	public:
		// Constructors.
		CD3D12ClearRenderingTechnique
		(
			_In_ CD3D12SceneRenderer * pParentRenderer
		);


		// Destructor.
		virtual ~CD3D12ClearRenderingTechnique();


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


	//
	// Member data
	//

	private:
		//
		// Colored-swatch rendering state.
		//

		ID3D12PipelineState * m_pColorSwatchGraphicsPipelineState;
};
