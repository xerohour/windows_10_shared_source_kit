//////////////////////////////////////////////////////////////////////
// File:  CD3D11TexturedPlaneRenderingTechnique.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a Direct3D 11 rendering technique that renders a
// plane (quadrilateral) with a texture.
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
// Class:  CD3D11TexturedPlaneRenderingTechnique
//
// Purpose:
// Encapsulates a technique for rendering a textured
// plane with a texture.
///////////////////////////////////////////////////////
class CD3D11TexturedPlaneRenderingTechnique : public CD3D11RenderingTechnique
{
	//
	// Construction/destruction
	//

	public:
		// Constructors.
		CD3D11TexturedPlaneRenderingTechnique
		(
			_In_ CD3D11SceneRenderer * pParentRenderer
		);


		// Destructor.
		virtual ~CD3D11TexturedPlaneRenderingTechnique();


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


	//
	// Member data
	//

	private:
		//
		// Textured-plane rendering state
		//

		ID3D11Buffer * m_pTexturedPlaneVertexBuffer;

		ID3D11Texture2D * m_pTexture;
		ID3D11ShaderResourceView * m_pTextureResourceView;

		ID3D11VertexShader * m_pTexturedObjectVertexShader;
		ID3D11PixelShader * m_pTexturedObjectPixelShader;
		ID3D11InputLayout * m_pTexturedObjectInputLayout;

		ID3D11SamplerState * m_pTextureSamplerState;
};
