//////////////////////////////////////////////////////////////////////
// File:  CD3D12TexturedPlaneRenderingTechnique.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a Direct3D 12 rendering technique that renders a
// plane (quadrilateral) with a texture.
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
// Class:  CD3D12TexturedPlaneRenderingTechnique
//
// Purpose:
// Encapsulates a technique for rendering a textured
// plane with a texture.
///////////////////////////////////////////////////////
class CD3D12TexturedPlaneRenderingTechnique : public CD3D12RenderingTechnique
{
	//
	// Construction/destruction
	//

	public:
		// Constructors.
		CD3D12TexturedPlaneRenderingTechnique
		(
			_In_ CD3D12SceneRenderer * pParentRenderer
		);


		// Destructor.
		virtual ~CD3D12TexturedPlaneRenderingTechnique();


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
		// Textured-plane rendering state.
		//

		ID3D12Resource * m_pTexturedPlaneVertexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_TexturedPlaneVertexBufferView;

		ID3DBlob * m_pTexturedObjectVertexShaderBlob;
		ID3DBlob * m_pTexturedObjectPixelShaderBlob;

		ID3D12PipelineState * m_pTexturedPlaneGraphicsPipelineState;

		ID3D12Resource * m_pTexture2D;
		ID3D12Resource * m_pTexture2DUploadBuffer;
};
