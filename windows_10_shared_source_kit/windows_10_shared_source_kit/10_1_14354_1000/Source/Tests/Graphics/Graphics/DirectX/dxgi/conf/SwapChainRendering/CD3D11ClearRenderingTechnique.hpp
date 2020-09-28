//////////////////////////////////////////////////////////////////////
// File:  CD3D11ClearRenderingTechnique.hpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Interfaces for the world's most trivial rendering technique:  It
// clears the render target to a solid color.
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
// Class:  CD3D11ClearRenderingTechnique
//
// Purpose:
// Encapsulates a rendering technique that merely
// clears the render target to a solid color.
///////////////////////////////////////////////////////
class CD3D11ClearRenderingTechnique : public CD3D11RenderingTechnique
{
	//
	// Construction/destruction
	//

	public:
		// Constructors.
		CD3D11ClearRenderingTechnique
		(
			_In_ CD3D11SceneRenderer * pParentRenderer
		);


		// Destructor.
		virtual ~CD3D11ClearRenderingTechnique();


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
};
