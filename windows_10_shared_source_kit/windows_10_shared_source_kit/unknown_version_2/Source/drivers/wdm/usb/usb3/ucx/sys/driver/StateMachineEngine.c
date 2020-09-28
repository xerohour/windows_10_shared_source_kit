add sat neg -r1 (t) (r0 = sat(t0 + -r1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.0,	1.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0
                        tex     t0                        

		        mov     r1,     c1
                        add_sat	r0,	t0,	-r1      // r0 = ( 1, 1, 0, 1)
		};
	}
}


//-----------------------------------------------------------------------------
// Operate on R2, R3 register
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add (r3 = r0 + c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
			
		        add	r3,	r0,	c1      // r3 = ( 1, 1, 1, 1)

			mov	r0,	r3
		};
	}
}

Technique
<
	String	Name = "Inst: add: neg (r3 = r0 + c0)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0

			mov	r0,	c1

	                add     r3,	r0,	c0      // r3 = (0,1,1,0)
                        mov     r3.a,   c0.a            // r3 = (0,1,1,1)

			mov	r0,	r3
		};
	}
}

Technique
<
	String	Name = "Inst: add sat (r3 = sat(r0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r3,	r0,	c1      // r3 = ( 1, 1, 1, 1)

			mov	r0,	r3
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg (r3 = sat(r0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r3,	r0,	c1      // r3 = ( 1, 1, 1, 1)

			mov	r0,	r3
		};
	}
}

Technique
<
	String	Name = "Inst: add -r3 (r3 = r0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
                        mov	r1,	c1
			
		        add	r3,	r0,	-r1      // r3 = ( 0, 1, 0, 1)

			mov	r0,	r3
		};
	}
}

Technique
<
	String	Name = "Inst: add neg -r2 (r2 = r0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0

			mov	r0,	c0
                        mov     r2,     c1
			
		        add	r2,	r0,	-r2      // r2 = ( 0, 1, 1, 1)

			mov	r0,	r2
		};
	}
}

Technique
<
	String	Name = "Inst: add sat -r2 (r1 = sat(r0 + -r2))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     1.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
                        mov     r2,     c1
			
		        add_sat	r2,	r0,	-r2      // r2 = ( 1, 0, 0, 1)

			mov	r0,	r2
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg -r3 (r1 = sat(r0 + -r3))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0

			mov	r0,	c0
                        mov	r3,	c1
			
		        add_sat	r3,	r0,	-r3      // r3 = ( 1, 1, 0, 1)

			mov	r0,	r3
		};
	}
}

//////////////////////////////////////////////////////////////////////
// File:  CD3D11ColorSwatchPatternRenderingTechnique.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a technique to render a pattern of colored
// swatches.
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

#include "CD3D11ColorSwatchPatternRenderingTechnique.hpp"
#include "CD3D11SceneRenderer.hpp"


/////////////////////////////////////////////////////////////////
//
// CD3D11ColorSwatchPatternRenderingTechnique implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Static initializers
//
////////////////////////////////////////////////////////////

COLOR_SWATCH CD3D11ColorSwatchPatternRenderingTechnique::s_BackgroundSwapChainColorSwatches[] =
{
	// White swatch in the upper-left 10% rown and column of the render target
	{
		{ 0.f, 0.f, .1f, .1f },
		{ 1.f, 1.f, 1.f, 1.f },
	},

	// Light gray swatch in the upper row, second column
	{
		{ .1f, 0.f, .2f, .1f },
		{ .75f, .75f, .75f, 1.f },
	},

	// Medium gray swatch in the upper row, third column
	{
		{ .2f, 0.f, .3f, .1f },
		{ .5f, .5f, .5f, 1.f },
	},

	// Dark gray swatch in the upper row, fourth column
	{
		{ .3f, 0.f, .4f, .1f },
		{ .25f, .25f, .25f, 1.f },
	},

	// Medium red swatch in the third row, first column
	{
		{ 0.f, .2f, .1f, .3f },
		{ .5f, 0.f, 0.f, 1.f },
	},

	// Medium green swatch in the third row, second column
	{
		{ .1f, .2f, .2f, .3f },
		{ 0.f, .5f, 0.f, 1.f },
	},

	// Medium blue swatch in the third row, fourth column
	{
		{ .2f, .2f, .3f, .3f },
		{ 0.f, 0.f, 0.5f, 1.f },
	},
};


COLOR_SWATCH CD3D11ColorSwatchPatternRenderingTechnique::s_ForegroundSwapChainColorSwatches[] =
{
	// White swatch in the second row, first column
	{
		{ 0.f, .1f, .1f, .2f },
		{ 1.f, 1.f, 1.f, 1.f },
	},

	// Light gray swatch in the second row, second column
	{
		{ .1f, .1f, .2f, .2f },
		{ .75f, .75f, .75f, 1.f },
	},

	// Medium gray swatch in the second row, third column
	{
		{ .2f, .1f, .3f, .2f },
		{ .5f, .5f, .5f, 1.f },
	},

	// Dark gray swatch in the second row, fourth column
	{
		{ .3f, .1f, .4f, .2f },
		{ .25f, .25f, .25f, 1.f },
	},

	// Medium red swatch in the fourth row, first column
	{
		{ 0.f, .3f, .1f, .4f },
		{ .5f, 0.f, 0.f, 1.f },
	},

	// Medium green swatch in the fourth row, second column
	{
		{ .1f, .3f, .2f, .4f },
		{ 0.f, .5f, 0.f, 1.f },
	},

	// Medium blue swatch in the fourth row, third column
	{
		{ .2f, .3f, .3f, .4f },
		{ 0.f, 0.f, 0.5f, 1.f },
	},

	// Medium red, half-opaque swatch in the firth row, first column
	{
		{ 0.f, .4f, .1f, .5f },
		{ .5f, 0.f, 0.f, .5f },
	},

	// Medium green, half-opaque swatch in the fifth row, second column
	{
		{ .1f, .4f, .2f, .5f },
		{ 0.f, .5f, 0.f, .5f },
	},

	// Medium blue, half-opaque swatch in the firth row, third column
	{
		{ .2f, .4f, .3f, .5f },
		{ 0.f, 0.f, 0.5f, .5f },
	},
};


////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CD3D11ColorSwatchPatternRenderingTechnique
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CD3D11ColorSwatchPatternRenderingTechnique::CD3D11ColorSwatchPatternRenderingTechnique
(
	_In_ CD3D11SceneRenderer * pParentRenderer
)
:
	CD3D11RenderingTechnique( pParentRenderer )
{
};


///////////////////////////////////////////////////////
// Function:  ~CD3D11ColorSwatchPatternRenderingTechnique
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CD3D11ColorSwatchPatternRenderingTechnique::~CD3D11ColorSwatchPatternRenderingTechnique()
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
// Creates and initializes resources for the color
// swatch pattern scene, including initializing the
// swap chain buffers with the color swatches.
///////////////////////////////////////////////////////
HRESULT CD3D11ColorSwatchPatternRenderingTechnique::CreateResources
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
void CD3D11ColorSwatchPatternRenderingTechnique::DestroyResources()
{
};


///////////////////////////////////////////////////////
// Function:  RenderScene
//
// Purpose:
// Renders color swatches onto the swap chain's buffer
// by calling ClearView on a render target view of the
// buffer with various rectangles and colors.
///////////////////////////////////////////////////////
HRESULT CD3D11ColorSwatchPatternRenderingTechnique::RenderScene
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
	// Validate members and state.
	//

	assert( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 != nullptr );
	if ( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"m_pImmediateContext_ID3D11DeviceContext1 is NULL."
		);

		goto Cleanup;
	};


	switch ( m_pParentRenderer->m_SwapChainTypes[ CurrentSwapChainSlot ] )
	{
		case SWAPCHAIN_TYPE_NONE:
		{
			hOverallResult = S_FALSE;
			goto Cleanup;
		} break;


		case SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
		{
			// For a decode swap chain, the decode buffers
			// are owned and maintained separately from the
			// swap chain, so the expectation is that they
			// have already been initialized, and will just
			// be passed directly to PresentBuffer when the
			// time comes to present the scene.  Thus, this
			// step is a no-op.

			hOverallResult = S_FALSE;
			goto Cleanup;
		} break;


		case SWAPCHAIN_TYPE_CREATESWAPCHAIN:
		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW:
		case SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND:
		{
			//
			// Validate members and state.
			//

			assert( m_pParentRenderer->m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] != nullptr );
			if ( m_pParentRenderer->m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ] == nullptr )
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					L"Swap chain pointer is NULL."
				);

				hOverallResult = E_INVALIDARG;
				goto Cleanup;
			};
		} break;


		default:
		{
			assert( false && "Unhandled swap chain type." );
		} break;
	};


	//
	// Create a render target view of the swap chain's current buffer.
	//

	HRESULT hGetBufferResult = m_pParentRenderer->m_ppSwapChains_IDXGISwapChain1[ CurrentSwapChainSlot ]->GetBuffer
	(
		0,
		IID_ID3D11Texture2D,
		(void **) &pBackBuffer
	);

	if ( FAILED( hGetBufferResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hGetBufferResult,
			L"GetBuffer on swap chain buffer 0 failed."
		);

		hOverallResult = hGetBufferResult;
		goto Cleanup;
	};

	if ( pBackBuffer == nullptr )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"GetBuffer on swap chain buffer 0 succeeded but returned NULL buffer pointer."
		);

		hOverallResult = E_FAIL;
		goto Cleanup;
	};


	//
	// Create a render target view over the buffer.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D11ColorSwatchPatternRenderingTechnique::RenderScene:  Creating render target view."
	);

	assert( pRenderTargetView == nullptr );
	HRESULT hCreateRenderTargetViewResult = m_pParentRenderer->m_pDevice_ID3D11Device->CreateRenderTargetView
	(
		pBackBuffer,
		nullptr,
		&pRenderTargetView
	);

	if ( FAILED( hCreateRenderTargetViewResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hCreateRenderTargetViewResult,
			L"CreateRenderTargetView failed."
		);

		hOverallResult = hCreateRenderTargetViewResult;
		goto Cleanup;
	};

	assert( pRenderTargetView != nullptr );


	//
	// Convert normalized RGBA buffer colors to a colors that
	// can be passed to ClearView.
	//

	float BackgroundClearViewColor[ 4 ] = { 0.f, 0.f, 0.f, 1.f };
	HRESULT hInitializeBackgroundClearViewColorResult = InitializeClearViewColor
	(
		m_pParentRenderer->m_SwapChainBufferFormats[ CurrentSwapChainSlot ],
		( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? DXGI_ALPHA_MODE_PREMULTIPLIED : DXGI_ALPHA_MODE_STRAIGHT,
		CD3D11SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ],
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
			CD3D11SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].r,
			CD3D11SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].g,
			CD3D11SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].b,
			CD3D11SceneRenderer::s_SwapChainClearColors[ CurrentSwapChainSlot ].a,
			BackgroundClearViewColor[ 0 ],
			BackgroundClearViewColor[ 1 ],
			BackgroundClearViewColor[ 2 ],
			BackgroundClearViewColor[ 3 ]
		)
	);


	//
	// Clear the entire render target to the background color.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"  - Clearing render target to (%f, %f, %f, %f).",
			BackgroundClearViewColor[ 0 ],
			BackgroundClearViewColor[ 1 ],
			BackgroundClearViewColor[ 2 ],
			BackgroundClearViewColor[ 3 ]
		)
	);


	assert( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 != nullptr );
	m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->ClearView
	(
		pRenderTargetView,
		BackgroundClearViewColor,
		nullptr,
		0
	);


	//
	// Fill color swatches in the buffer.
	//

	UINT SwatchCount = 0;
	if ( CurrentSwapChainSlot == SWAPCHAIN_SLOT_BACKGROUND )
	{
		SwatchCount = ARRAYSIZE( s_BackgroundSwapChainColorSwatches );
	}
	else
	{
		SwatchCount = ARRAYSIZE( s_ForegroundSwapChainColorSwatches );
	};

	for
	(
		UINT iCurrentSwatch = 0;
		iCurrentSwatch < SwatchCount;
		++iCurrentSwatch
	)
	{
		//
		// Convert the color if needed.
		//

		float SwatchClearViewColor[ 4 ] = { 0.f, 0.f, 0.f, 1.f };

		assert
		(
			CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND && iCurrentSwatch < ARRAYSIZE( s_ForegroundSwapChainColorSwatches ) ||
			CurrentSwapChainSlot == SWAPCHAIN_SLOT_BACKGROUND && iCurrentSwatch < ARRAYSIZE( s_BackgroundSwapChainColorSwatches )
		);

#pragma warning( suppress:  26014 )
		HRESULT hInitializeSwatchClearViewColorResult = InitializeClearViewColor
		(
			m_pParentRenderer->m_SwapChainBufferFormats[ CurrentSwapChainSlot ],
			( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? DXGI_ALPHA_MODE_PREMULTIPLIED : DXGI_ALPHA_MODE_STRAIGHT,
			( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].Color : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color,
			SwatchClearViewColor
		);

		if ( FAILED( hInitializeSwatchClearViewColorResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hInitializeSwatchClearViewColorResult,
				L"InitializeClearViewColor on swatch color failed."
			);

			goto Cleanup;
		};


#pragma warning( suppress:  26014 )
		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			WEX::Common::String().Format
			(
				L"  - Converted swatch %u color for %s swap chain from normalized RGB color (%f, %f, %f, %f) to clear color (%f, %f, %f, %f).",
				iCurrentSwatch,
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? L"foreground" : L"background",
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].Color.r : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.r,
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].Color.g : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.g,
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].Color.b : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.b,
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].Color.a : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.a,
				SwatchClearViewColor[ 0 ],
				SwatchClearViewColor[ 1 ],
				SwatchClearViewColor[ 2 ],
				SwatchClearViewColor[ 3 ]
			)
		);


		//
		// Clear a rectangle in the buffer to the swatch color.
		//

		RECT ClearViewRectangle;
		ZeroMemory( &ClearViewRectangle, sizeof( ClearViewRectangle ) );

#pragma warning( suppress:  26014 )
		ClearViewRectangle.left = (LONG) ( ( ( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.left : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.left ) * m_pParentRenderer->m_SceneWidth );

#pragma warning( suppress:  26014 )
		ClearViewRectangle.top = (LONG) ( ( ( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.top : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.top ) * m_pParentRenderer->m_SceneHeight );

#pragma warning( suppress:  26014 )
		ClearViewRectangle.right = (LONG) ( ( ( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.right : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.right ) * m_pParentRenderer->m_SceneWidth );

#pragma warning( suppress:  26014 )
		ClearViewRectangle.bottom = (LONG) ( ( ( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? s_ForegroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.bottom : s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.bottom ) * m_pParentRenderer->m_SceneHeight );

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			WEX::Common::String().Format
			(
				L"  - Clearing swatch (%li, %li) to (%li, %li) within %s render target with color (%f, %f, %f, %f).",
				ClearViewRectangle.left,
				ClearViewRectangle.top,
				ClearViewRectangle.right,
				ClearViewRectangle.bottom,
				( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? L"foreground" : L"background",
				SwatchClearViewColor[ 0 ],
				SwatchClearViewColor[ 1 ],
				SwatchClearViewColor[ 2 ],
				SwatchClearViewColor[ 3 ]
			)
		);


		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->ClearView
		(
			pRenderTargetView,
			SwatchClearViewColor,
			&ClearViewRectangle,
			1
		);
	};


	// If we made it this far, then we're good to go.
	hOverallResult = S_OK;


Cleanup:
	SAFE_RELEASE( pBackBuffer );

	SAFE_RELEASE( pRenderTargetView );


	return hOverallResult;
};


////////////////////////////////////////////////////////////
//
// Helper method implementation
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  InitializeSwapChainBuffers
//
// Purpose:
// Initializes swap chain buffer texture array slices.
///////////////////////////////////////////////////////
HRESULT CD3D11ColorSwatchPatternRenderingTechnique::InitializeSwapChainBuffers
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

	ID3D11VideoDecoderOutputView * pVideoDecoderOutputView = nullptr;


	//
	// Validate members and state.
	//

	// This work is only needed for decode swap chains.
	if ( m_pParentRenderer->m_SwapChainTypes[ CurrentSwapChainSlot ] != SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE )
	{
		hOverallResult = S_FALSE;
		goto Cleanup;
	};


	//
	// Get the video decoder profile count.
	//

	asse