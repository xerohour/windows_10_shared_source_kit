//////////////////////////////////////////////////////////////////////
// File:  CD3D12ShadedCubeRenderingTechnique.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a Direct3D 12 rendering technique that renders a
// wrap-shaded cube.
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
// Class:  CD3D12ShadedCubeRenderingTechnique
//
// Purpose:
// Encapsulates a technique for rendering a wrap-shaded
// cube using Direct3D 12.
///////////////////////////////////////////////////////
class CD3D12ShadedCubeRenderingTechnique : public CD3D12RenderingTechnique
{
	//
	// Construction/destruction
	//

	public:
		// Constructors.
		CD3D12ShadedCubeRenderingTechnique
		(
			_In_ CD3D12SceneRenderer * pParentRenderer
		);


		// Destructor.
		virtual ~CD3D12ShadedCubeRenderingTechnique();


	//
	// CD3D12RenderingTechnique methods
	//

	public:
		HRESULT CreateResources
		(
			SWAPCHAIN_SLOT CurrentSwapChainSlot
		);

		void DestroyResources();


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
		// Shaded-cube rendering state.
		//

		ID3D12Resource * m_pShadedCubeVertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_ShadedCubeVertexBufferView;

		ID3DBlob * m_pShadedObjectVertexShaderBlob;
		ID3DBlob * m_pShadedObjectPixelShaderBlob;

		ID3D12PipelineState * m_pShadedCubeGraphicsPipelineState;
};
