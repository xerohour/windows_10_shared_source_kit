//////////////////////////////////////////////////////////////////////
// File:  CD3D11ShadedCubeRenderingTechnique.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for a Direct3D 11 rendering technique that renders a
// wrap-shaded cube.
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

////////////////////////////////////////////////////////////
//
// Class interfaces
//
////////////////////////////////////////////////////////////

class CD3D11SceneRenderer;


///////////////////////////////////////////////////////
// Class:  CD3D11ShadedCubeRenderingTechnique
//
// Purpose:
// Encapsulates a technique for rendering a wrap-shaded
// cube using Direct3D 11.
///////////////////////////////////////////////////////
class CD3D11ShadedCubeRenderingTechnique : public CD3D11RenderingTechnique
{
	//
	// Construction/destruction
	//

	public:
		// Constructors.
		CD3D11ShadedCubeRenderingTechnique
		(
			_In_ CD3D11SceneRenderer * pParentRenderer
		);


		// Destructor.
		virtual ~CD3D11ShadedCubeRenderingTechnique();


	//
	// CRenderingTechnique methods
	//

	public:
		HRESULT CreateResources
		(
			SWAPCHAIN_SLOT CurrentSwapChain
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
		// Shaded-cube rendering state
		//

		ID3D11Buffer * m_pShadedCubeVertexBuffer;

		ID3D11VertexShader * m_pShadedObjectVertexShader;
		ID3D11PixelShader * m_pShadedObjectPixelShader;
		ID3D11InputLayout * m_pShadedObjectInputLayout;
};
