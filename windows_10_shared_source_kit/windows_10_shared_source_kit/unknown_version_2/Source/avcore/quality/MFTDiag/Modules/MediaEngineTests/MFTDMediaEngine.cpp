reak;


		case SCENE_TYPE_SWATCH_RANDOMCOLOR:
		{
			// Randomize the clear color.
			BackgroundColor.r = rand() / (float) RAND_MAX;
			BackgroundColor.g = rand() / (float) RAND_MAX;
			BackgroundColor.b = rand() / (float) RAND_MAX;
			BackgroundColor.a = 1.f;
		} break;


		default:
		{
			// Use the default clear color for the swap chain slot.
			BackgroundColor.r = s_SwapChainClearColors[ CurrentSwapChainSlot ].r;
			BackgroundColor.g = s_SwapChainClearColors[ CurrentSwapChainSlot ].g;
			BackgroundColor.b = s_SwapChainClearColors[ CurrentSwapChainSlot ].b;
			BackgroundColor.a = s_SwapChainClearColors[ CurrentSwapChainSlot ].a;
		} break;
	};


	//
	// Convert normalized RGBA buffer colors to a colors that
	// can be passed to ClearView.
	//

	float BackgroundClearViewColor[ 4 ] = { 0.f, 0.f, 0.f, 0.f };
	HRESULT hInitializeBackgroundClearViewColorResult = InitializeClearViewColor
	(
		m_SwapChainBufferFormats[ CurrentSwapChainSlot ],
		( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? DXGI_ALPHA_MODE_PREMULTIPLIED : DXGI_ALPHA_MODE_STRAIGHT,
		BackgroundColor,
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
			BackgroundColor.r,
			BackgroundColor.g,
			BackgroundColor.b,
			BackgroundColor.a,
			BackgroundClearViewColor[ 0 ],
			BackgroundClearViewColor[ 1 ],
			BackgroundClearViewColor[ 2 ],
			BackgroundClearViewColor[ 3 ]
		)
	);


	//
	// Clear the entire buffer to the background color.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"CD3D12SceneRenderer::ClearRenderTarget:  Clearing render target view to (%f, %f, %f, %f).",
			BackgroundClearViewColor[ 0 ],
			BackgroundClearViewColor[ 1 ],
			BackgroundClearViewColor[ 2 ],
			BackgroundClearViewColor[ 3 ]
		)
	);

	m_ppCommandLists[ CurrentSwapChainSlot ]->ClearRenderTargetView
	(
		m_RenderTargetViews[ CurrentSwapChainSlot ],
		BackgroundClearViewColor,
		0,
		nullptr
	);


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  WaitForGPU
//
// Purpose:
// Waits for the GPU to catch up to work that has been
// issued.
///////////////////////////////////////////////////////
HRESULT CD3D12SceneRenderer::WaitForGPU()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Create the fence-completion event.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::WaitForGPU:  Creating fence-completion event."
	);

	HANDLE hFenceCompletionEvent = CreateEventEx
	(
		nullptr,
		FALSE,
		FALSE,
		EVENT_ALL_ACCESS
	);

	if ( hFenceCompletionEvent == nullptr )
	{
		DWORD CreateEventError = GetLastError();
		LOG_ERROR
		(
			ERROR_TYPE_WIN32,
			CreateEventError,
			L"CreateEventEx failed."
		);

		hOverallResult = HRESULT_FROM_WIN32( CreateEventError );
		goto Cleanup;
	};


	//
	// Signal the fence with the current fence value.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"CD3D12SceneRenderer::WaitForGPU:  Signalling fence with value %I64u.",
			m_CurrentFrame
		)
	);

	// Signal the fence.
	const UINT64 CurrentFrameFenceValue = m_CurrentFrame;
	HRESULT hSignalFenceResult = m_pQueue_ID3D12CommandQueue->Signal
	(
		m_pCurrentFrameFence,
		CurrentFrameFenceValue
	);

	if ( FAILED( hSignalFenceResult ) )
	{
		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hSignalFenceResult,
			L"Signal on command queue failed."
		);

		hOverallResult = hSignalFenceResult;
		goto Cleanup;
	};


	//
	// Increment the current fence value.
	//

	++m_CurrentFrame;


	//
	// Wait until the GPU reaches the fence.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::WaitForGPU:  Waiting for fence."
	);

	if ( m_pCurrentFrameFence->GetCompletedValue() >= CurrentFrameFenceValue )
	{
		// The GPU passed the fence before we got here, so we don't
		// need to wait.
		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::WaitForGPU:  The GPU's completion value has already passed the fence."
		);
	}
	else
	{
		// Have the fence signal the fence-completion event upon
		// completion of the specified fence value.
		HRESULT hSetCompletionEventResult = m_pCurrentFrameFence->SetEventOnCompletion
		(
			CurrentFrameFenceValue,
			hFenceCompletionEvent
		);

		if ( FAILED( hSetCompletionEventResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hSetCompletionEventResult,
				L"SetEventOnCompletion failed."
			);

			hOverallResult = hSetCompletionEventResult;
			goto Cleanup;
		};


		// Wait for the fence-completion event to be signalled.
		DWORD WaitResult = WaitForSingleObject
		(
			hFenceCompletionEvent,
			IsDebuggerPresent() == TRUE ? INFINITE : 30 * 1000
		);

		switch ( WaitResult )
		{
			case WAIT_OBJECT_0:
			{
				// The event was signalled as expected.
				LOG_MESSAGE
				(
					MESSAGE_TYPE_DEBUG,
					L"CD3D12SceneRenderer::WaitForGPU:  Fence-completion event was signalled."
				);
			} break;

			case WAIT_TIMEOUT:
			{
				hOverallResult = E_FAIL;
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hOverallResult,
					L"A single frame has taken longer than 30 seconds to render.  Something is desperately wrong."
				);

				goto Cleanup;
			} break;

			case WAIT_ABANDONED:
			case WAIT_FAILED:
			{
				hOverallResult = E_FAIL;
				LOG_ERROR
				(
					ERROR_TYPE_HRESULT,
					hOverallResult,
					L"WaitForSingleObject returned an unexpected value."
				);

				goto Cleanup;
			} break;
		};
	};


	hOverallResult = S_OK;


Cleanup:
	if ( hFenceCompletionEvent != nullptr )
	{
		CloseHandle( hFenceCompletionEvent );
		hFenceCompletionEvent = nullptr;
	};


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  RenderAndPresentOnce
//
// Purpose:
// Renders a single frame.
///////////////////////////////////////////////////////
HRESULT CD3D12SceneRenderer::RenderAndPresentOnce()
{
	return CD3D12SceneRenderer::RenderAndPresentOnce
	(
		1,
		m_PresentFlags
	);
};


HRESULT CD3D12SceneRenderer::RenderAndPresentOnce
(
	UINT SyncInterval,
	UINT PresentFlags
)
{
	//
	// Build default presentation parameters.
	//

	DXGI_PRESENT_PARAMETERS PresentParameters;
	PresentParameters.DirtyRectsCount = 0;
	PresentParameters.pDirtyRects = nullptr;
	PresentParameters.pScrollRect = nullptr;
	PresentParameters.pScrollOffset = nullptr;

	return CD3D12SceneRenderer::RenderAndPresentOnce
	(
		SyncInterval,
		PresentFlags,
		&PresentParameters
	);
};


HRESULT CD3D12SceneRenderer::RenderAndPresentOnce
(
	UINT SyncInterval,
	UINT PresentFlags,
	_In_ DXGI_PRESENT_PARAMETERS * pPresentParameters
)
{
	//
	// Enter the rendering guard critical section so that the main
	// thread does not change the mode during rendering.
	//

	EnterCriticalSection( &m_RenderingGuard );


	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;

	ID3D12Resource * pCurrentSwapChainBuffer = nullptr;


	//
	// Animate the world matrix if needed.
	//

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::RenderAndPresentOnce:  Animating world matrix."
	);

	static UINT64 StartTime = 0;

	if ( m_bAnimating == true )
	{
		UINT64 CurrentTime = GetTickCount64();
		if ( StartTime == 0 )
		{
			StartTime = CurrentTime;
		};

		float ElapsedTime = ( CurrentTime - StartTime ) / 1000.0f;

		D3DXMatrixRotationY( &m_WorldMatrix, ElapsedTime );
		D3DXMatrixTranspose( &m_pVertexShaderMatrices->m_World, &m_WorldMatrix );
	};


	//
	// Render to all render targets and present to all swap chains.
	//

	for
	(
		SWAPCHAIN_SLOT CurrentSwapChainSlot = SWAPCHAIN_SLOT_FIRST;
		CurrentSwapChainSlot < SWAPCHAIN_SLOT_COUNT;
		CurrentSwapChainSlot = (SWAPCHAIN_SLOT) ( CurrentSwapChainSlot + 1 )
	)
	{
		//
		// Skip empty swap chain slots.
		//

		if ( m_SwapChainTypes[ CurrentSwapChainSlot ] == SWAPCHAIN_TYPE_NONE )
		{
			continue;
		};


		//
		// Get a pointer to the current swap chain buffer.
		//

		HRESULT hGetCurrentBufferResult = m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]->GetBuffer
		(
			m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]->GetCurrentBackBufferIndex(),
			IID_ID3D12Resource,
			(void **) &pCurrentSwapChainBuffer
		);

		if ( FAILED( hGetCurrentBufferResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hGetCurrentBufferResult,
				L"GetBuffer on current buffer failed."
			);

			hOverallResult = hGetCurrentBufferResult;
			goto Cleanup;
		};


		//
		// Render scene-dependent content.
		//

		assert( m_ppRenderingTechniques[ m_SceneTypes[ CurrentSwapChainSlot ] ] != nullptr );
		HRESULT hRenderSceneResult = m_ppRenderingTechniques[ m_SceneTypes[ CurrentSwapChainSlot ] ]->RenderScene
		(
			CurrentSwapChainSlot,
			pCurrentSwapChainBuffer
		);

		if ( FAILED( hRenderSceneResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hRenderSceneResult,
				L"RenderScene failed."
			);

			hOverallResult = hRenderSceneResult;
			goto Cleanup;
		};


		//
		// Indicate that the current swap chain buffer will now be used to present.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::RenderAndPresentOnce:  Marking back buffer for presentation."
		);

		D3D12_RESOURCE_BARRIER RenderTargetPresentationResourceBarrier;
		ZeroMemory( &RenderTargetPresentationResourceBarrier, sizeof( RenderTargetPresentationResourceBarrier ) );
		RenderTargetPresentationResourceBarrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
		RenderTargetPresentationResourceBarrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
		RenderTargetPresentationResourceBarrier.Transition.pResource = pCurrentSwapChainBuffer;
		RenderTargetPresentationResourceBarrier.Transition.Subresource = 0;
		RenderTargetPresentationResourceBarrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
		RenderTargetPresentationResourceBarrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;

		m_ppCommandLists[ CurrentSwapChainSlot ]->ResourceBarrier
		(
			1,
			&RenderTargetPresentationResourceBarrier
		);


		//
		// Close the command list.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::RenderAndPresentOnce:  Closing command list."
		);

		HRESULT hCloseShadedCubeCommandListResult = m_ppCommandLists[ CurrentSwapChainSlot ]->Close();

		if ( FAILED( hCloseShadedCubeCommandListResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCloseShadedCubeCommandListResult,
				L"Close on command list failed."
			);

			hOverallResult = hCloseShadedCubeCommandListResult;
			goto Cleanup;
		};


		//
		// Execute the command list.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::RenderAndPresentOnce:  Executing command list."
		);

		ID3D12CommandList * CommandListsToExecute[] =
		{
			m_ppCommandLists[ CurrentSwapChainSlot ]
		};

		m_pQueue_ID3D12CommandQueue->ExecuteCommandLists
		(
			ARRAYSIZE( CommandListsToExecute ),
			CommandListsToExecute
		);


		//
		// Present to the current swap chain.
		//

		switch ( m_SwapChainTypes[ CurrentSwapChainSlot ] )
		{
			case SWAPCHAIN_TYPE_CREATEDECODESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_DEBUG,
					L"CD3D12SceneRenderer::RenderAndPresentOnce:  Calling PresentBuffer."
				);

				// @TODO:  Determine the next buffer in the sequence.
				UINT BufferToPresent = 0; // iCurrentFrame % m_cSwapChainBufferTextureArraySlices[ CurrentSwapChainSlot ];

				// Present the buffer.
				assert( m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ] != nullptr );
				HRESULT hPresentResult = m_ppSwapChains_IDXGIDecodeSwapChain[ CurrentSwapChainSlot ]->PresentBuffer
				(
					BufferToPresent,
					1, // @REVIEW:  Sync interval
					0  // @REVIEW:  Flags
				);

				if ( FAILED( hPresentResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"PresentBuffer failed."
					);

					hOverallResult = hPresentResult;
					goto Cleanup;
				};

				if ( hPresentResult != S_OK )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"PresentBuffer returned unexpected HRESULT."
					);

					goto Cleanup;
				};
			} break;


			case SWAPCHAIN_TYPE_CREATESWAPCHAIN:
			case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOMPOSITIONSURFACEHANDLE:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_DEBUG,
					L"CD3D12SceneRenderer::RenderAndPresentOnce:  Calling Present."
				);

				// Present to the swap chain.
				assert( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] != nullptr );
				HRESULT hPresentResult = m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]->Present
				(
					1, // @REVIEW:  Sync interval
					0  // @REVIEW:  Flags
				);

				if ( FAILED( hPresentResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"Present failed."
					);

					hOverallResult = hPresentResult;
					goto Cleanup;
				};

				if ( hPresentResult != S_OK )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"Present returned unexpected HRESULT."
					);

					goto Cleanup;
				};
			} break;


			case SWAPCHAIN_TYPE_CREATESWAPCHAINFORCOREWINDOW:
			case SWAPCHAIN_TYPE_CREATESWAPCHAINFORHWND:
			{
				LOG_MESSAGE
				(
					MESSAGE_TYPE_DEBUG,
					L"CD3D12SceneRenderer::RenderAndPresentOnce:  Calling Present1."
				);

				// Present the frame.
				DXGI_PRESENT_PARAMETERS PresentParameters;
				ZeroMemory( &PresentParameters, sizeof( PresentParameters ) );

				assert( m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ] != nullptr );
				HRESULT hPresentResult = m_ppSwapChains_IDXGISwapChain3[ CurrentSwapChainSlot ]->Present1
				(
					1, // @REVIEW:  Sync interval
					0, // @REVIEW:  Flags
					&PresentParameters
				);

				if ( FAILED( hPresentResult ) )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"Present1 failed."
					);

					hOverallResult = hPresentResult;
					goto Cleanup;
				};

				if ( hPresentResult != S_OK )
				{
					LOG_ERROR
					(
						ERROR_TYPE_HRESULT,
						hPresentResult,
						L"Present1 returned unexpected HRESULT."
					);

					goto Cleanup;
				};
			} break;


			default:
			{
				LOG_ERROR
				(
					ERROR_TYPE_BOOL,
					false,
					L"CD3D12SceneRenderer::RenderAndPresentOnce:  Unhandled swap chain type."
				);

				assert( false && L"Unhandled swap chain type." );

				hOverallResult = E_NOTIMPL;
				goto Cleanup;
			} break;
		};


		//
		// Wait for the frame to have been presented.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::RenderAndPresentOnce:  Waiting for frame presentation."
		);

		HRESULT hWaitForGPUResult = WaitForGPU();

		if ( FAILED( hWaitForGPUResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hWaitForGPUResult,
				L"WaitForGPU failed."
			);

			hOverallResult = hWaitForGPUResult;
			goto Cleanup;
		};


		LOG_MESSAGE
		(
			MESSAGE_TYPE_DEBUG,
			L"CD3D12SceneRenderer::RenderAndPresentOnce:  Presentation complete."
		);


		//
		// Release the current swap chain buffer.
		//

		SAFE_RELEASE( pCurrentSwapChainBuffer );
	};


	hOverallResult = S_OK;


Cleanup:
	//
	// Release the swap chain buffer.
	//

	SAFE_RELEASE( pCurrentSwapChainBuffer );


	//
	// Leave the rendering guard critical section.
	//

	LeaveCriticalSection( &m_RenderingGuard );


	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  SetShaderConstants
//
// Purpose:
// Sets the world, projection, and view matrices as
// constants in a constant buffer so that they will be
// available to the vertex shader.
///////////////////////////////////////////////////////
HRESULT CD3D12SceneRenderer::SetShaderConstants()
{
	//
	// Initialize locals.
	//

	HRESULT hOverallResult = E_FAIL;


	//
	// Initialize our matrices.
	//

	// Initialize the world matrix.
	D3DXMatrixIdentity( &m_WorldMatrix );

	// Initialize the view matrix.
	D3DXVECTOR3 PointToLookAt( 0.0f, 0.0f, 0.0f );
	D3DXVECTOR3 UpVector( 0.0f, 1.0f, 0.0f );

	D3DXVECTOR3 EyePosition( 2.0f, 2.0f, -5.0f );
	D3DXMatrixLookAtLH
	(
		&m_ViewMatrix,
		&EyePosition,
		&PointToLookAt,
		&UpVector
	);

	// Initialize the projection matrix.
	D3DXMatrixPerspectiveFovLH
	(
		&m_ProjectionMatrix,
		(float) D3DX_PI * 0.5f,
		(float) m_SceneWidth / (float) m_SceneHeight,
		MIN_DEPTH,
		MAX_DEPTH
	);


	//
	// Validate members and state.
	//

	// Check that the constant buffer upload heap is mapped.
	assert( m_pVertexShaderMatrices != nullptr );
	if ( m_pVertexShaderMatrices == nullptr )
	{
		hOverallResult = E_FAIL;

		LOG_ERROR
		(
			ERROR_TYPE_HRESULT,
			hOverallResult,
			L"Constant-buffer upload heap is not mapped."
		);

		goto Cleanup;
	};


	//
	// Copy constants into the constant buffer.
	//

	D3DXMatrixTranspose( &m_pVertexShaderMatrices->m_World, &m_WorldMatrix );
	D3DXMatrixTranspose( &m_pVertexShaderMatrices->m_View, &m_ViewMatrix );
	D3DXMatrixTranspose( &m_pVertexShaderMatrices->m_Projection, &m_ProjectionMatrix );

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::SetShaderConstants:  Stored world matrix:"
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]",
			m_pVertexShaderMatrices->m_World._11,
			m_pVertexShaderMatrices->m_World._12,
			m_pVertexShaderMatrices->m_World._13,
			m_pVertexShaderMatrices->m_World._14,
			m_pVertexShaderMatrices->m_World._21,
			m_pVertexShaderMatrices->m_World._22,
			m_pVertexShaderMatrices->m_World._23,
			m_pVertexShaderMatrices->m_World._24,
			m_pVertexShaderMatrices->m_World._31,
			m_pVertexShaderMatrices->m_World._32,
			m_pVertexShaderMatrices->m_World._33,
			m_pVertexShaderMatrices->m_World._34,
			m_pVertexShaderMatrices->m_World._41,
			m_pVertexShaderMatrices->m_World._42,
			m_pVertexShaderMatrices->m_World._43,
			m_pVertexShaderMatrices->m_World._44
		)
	);


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::SetShaderConstants:  Stored view matrix:"
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]",
			m_pVertexShaderMatrices->m_View._11,
			m_pVertexShaderMatrices->m_View._12,
			m_pVertexShaderMatrices->m_View._13,
			m_pVertexShaderMatrices->m_View._14,
			m_pVertexShaderMatrices->m_View._21,
			m_pVertexShaderMatrices->m_View._22,
			m_pVertexShaderMatrices->m_View._23,
			m_pVertexShaderMatrices->m_View._24,
			m_pVertexShaderMatrices->m_View._31,
			m_pVertexShaderMatrices->m_View._32,
			m_pVertexShaderMatrices->m_View._33,
			m_pVertexShaderMatrices->m_View._34,
			m_pVertexShaderMatrices->m_View._41,
			m_pVertexShaderMatrices->m_View._42,
			m_pVertexShaderMatrices->m_View._43,
			m_pVertexShaderMatrices->m_View._44
		)
	);


	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		L"CD3D12SceneRenderer::SetShaderConstants:  Stored projection matrix:"
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_DEBUG,
		WEX::Common::String().Format
		(
			L"[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]\n[%4.4f, %4.4f, %4.4f, %4.4f]",
			m_pVertexShaderMatrices->m_Projection._11,
			m_pVertexShaderMatrices->m_Projection._12,
			m_pVertexShaderMatrices->m_Projection._13,
			m_pVertexShaderMatrices->m_Projection._14,
			m_pVertexShaderMatrices->m_Projection._21,
			m_pVertexShaderMatrices->m_Projection._22,
			m_pVertexShaderMatrices->m_Projection._23,
			m_pVertexShaderMatrices->m_Projection._24,
			m_pVertexShaderMatrices->m_Projection._31,
			m_pVertexShaderMatrices->m_Projection._32,
			m_pVertexShaderMatrices->m_Projection._33,
			m_pVertexShaderMatrices->m_Projection._34,
			m_pVertexShaderMatrices->m_Projection._41,
			m_pVertexShaderMatrices->m_Projection._42,
			m_pVertexShaderMatrices->m_Projection._43,
			m_pVertexShaderMatrices->m_Projection._44
		)
	);


	hOverallResult = S_OK;


Cleanup:
	return hOverallResult;
};


///////////////////////////////////////////////////////
// Function:  StartAnimation
//
// Purpose:
// Indicates that the world matrix should be rotated as
// a function of elapsed (wall clock) time.
///////////////////////////////////////////////////////
void CD3D12SceneRenderer::StartAnimation()
{
	m_bAnimating = true;
};


///////////////////////////////////////////////////////
// Function:  StopAnimation
//
// Purpose:
// Indicates that the world matrix should not be
// rotated as a function of elapsed (wall clock) time.
///////////////////////////////////////////////////////
void CD3D12SceneRenderer::StopAnimation()
{
	m_bAnimating = false;
};
#ifndef __FRAMEWORKTYPES_H__
#define __FRAMEWORKTYPES_H__

// --------------------------------------------------------------------------------------
// Framework types declaration
//
// Copyright Microsoft Corp. 2000
//
// --------------------------------------------------------------------------------------

#include <windows.h>
#include <tchar.h>
#include <LIMITS.H>
#include "CD3DDef.h"

// --------------------------------------------------------------------------------------
// Forward declarations

class	CDebugOut;


// --------------------------------------------------------------------------------------
// Defines / Types / Externs

const float pi = 3.141592654f;
const int	MAX_D3D_STRING = 1024;
enum GRAPHICSBUSTYPES { BADBUS, UNKNOWNBUS, PCIBUS, AGPBUS };
enum DRIVERMODEL { DRIVERMODEL_XPDM = 1, DRIVERMODEL_LDDM = 2};
extern CDebugOut	DebugOut; // instantiated in CDebugOut.cpp

#define E_UNSUPPORTED                        _HRESULT_TYPEDEF_(0x8000000BL)

 // Supported APIs list
typedef enum _RENDERPRMITIVEAPI
{
    RP_BE               = 0,    // (Begin/Vertex/End)
    RP_BIE              = 1,    // (BeginIndexed/Index/End)
    RP_DP               = 2,    // DrawPrimitiveUP (DrawPrimitive)
    RP_DIP              = 3,    // DrawIndexedPrimitiveUP (DrawIndexedPrimitive)
    RP_DPS              = 4,    // DrawPrimitive multiple streams (DrawPrimitiveStrided)
    RP_DIPS             = 5,    // DrawIndexedPrimitive multiple streams (DrawIndexedPrimitiveStrided)
    RP_DPVB             = 6,    // DrawPrimitive single stream (DrawPrimitiveVB)
    RP_DIPVB            = 7,    // DrawIndexedPrimitive single stream (DrawIndexedPrimitiveVB)
    RP_DPVB_PV          = 8,    // (DrawPrimitiveVB after ProcessVertices)
    RP_DIPVB_PV         = 9,    // (DrawIndexedPrimitiveVB after ProcessVertices)
    RP_DPVB_OPT         = 10,   // (DrawPrimitiveVB with optimized vertex buffer)
    RP_DIPVB_OPT        = 11,   // (DrawIndexedPrimitiveVB with optimized vertex buffer)
    RP_DPVB_OPT_PV      = 12,   // (DrawPrimitiveVB after ProcessVertices with optimized vertex buffer)
    RP_DIPVB_OPT_PV     = 13,   // (DrawIndexedPrimitiveVB after ProcessVertices with optimized vertex buffer)
    RP_DPVB_PVS         = 14,   // (DrawPrimitiveVB after ProcessVerticesStrided)
    RP_DIPVB_PVS        = 15,   // (DrawIndexedPrimitiveVB after ProcessVerticesStrided)
    RP_FORCE_DWORD      = 0x7fffffff
} RENDERPRIMITIVEAPI, *PRENDERPRIMITIVEAPI;


typedef struct _RPOPTIONS
{
	DWORD		Pool;
	DWORD		Usage;
	UINT		uIBBitCount;
	DWORD		Flags;
} RPOPTIONS, *LPRPOPTIONS;

//enum TESTTYPE
//{
//	TESTTYPE_API=2,
//    TESTTYPE_STRESS=3,
//	TESTTYPE_CONF=4,
//	TESTTYPE_FUNC=5,
//	TESTTYPE_PERF=6,
//	TESTTYPE_APP=7,
//	TESTTYPE_SAMP=8,
//	TESTTYPE_PSGP=9
//};

const DWORD		PSGPREGCHANGE_NONE		= 0x00;
const DWORD		PSGPREGCHANGE_ENABLE	= 0x01;
const DWORD		PSGPREGCHANGE_DISABLE	= 0x02;

const DWORD		VPEMUREGCHANGE_NONE		= 0x00;
const DWORD		VPEMUREGCHANGE_ENABLE	= 0x01;
const DWORD		VPEMUREGCHANGE_DISABLE	= 0x02;

const DWORD		PPEMUREGCHANGE_NONE		= 0x00;
const DWORD		PPEMUREGCHANGE_ENABLE	= 0x01;
const DWORD		PPEMUREGCHANGE_DISABLE	= 0x02;


const DWORD		TESTSCENARIO_RAST		= 0x0001;		// Hardware rasterization test
const DWORD		TESTSCENARIO_TNL		= 0x0002;		// Hardware TnL test
const DWORD		TESTSCENARIO_PURE		= 0x0002;		// Pure HAL test
const DWORD		TESTSCENARIO_PSGP		= 0x0008;		// Processor-specific pipeline
const DWORD		TESTSCENARIO_MSGP		= 0x0010;		// MS pipeline
const DWORD		TESTSCENARIO_REF		= 0x0100;		// Reference rasterization test
const DWORD		TESTSCENARIO_TNLREF		= 0x0200;		// Reference pipeline test
const DWORD		TESTSCENARIO_PUREREF	= 0x0400;		// Pure Reference


// --------------------------------------------------------------------------------------
// DISPLAYMODE struct - maintains info for each possible display mode

typedef struct
{
    DWORD   dwWidth;
    DWORD   dwHeight;
    DWORD   dwBPP;
    DWORD   dwRefreshRate;
    FORMAT  Format;
} DISPLAYMODE, *PDISPLAYMODE;

// --------------------------------------------------------------------------------------
// TESTMODE - modes generated by CD3DTest

typedef struct  _TESTMODE
{
	DISPLAYMODE *pDisplay;
	UINT		nBackBufferWidth, nBackBufferHeight;
	bool   		bWindowed;
	int     	nTextureFormat;
    int     	nCubeFormat;
    int     	nVolumeFormat;
	int     	nZBufferFormat;
	int			nMultisample;
	int			nMltsmpQuality;
	int			nBackBufferFormat;
	PVOID		pExtraData;
	
	_TESTMODE*	pNext;
	_TESTMODE*	pPrevious;
} TESTMODE, *PTESTMODE;

// --------------------------------------------------------------------------------------
// DEVICEMODE - modes generated by CD3DWindowFramework

typedef struct _DEVICEMODE
{
	DISPLAYMODE *pDispMode;
	int   nAdapter;
	float fVersion;
	DWORD dwDevType;
	DWORD dwBehaviors;
	DWORD dwPSGPRegChange;

	_DEVICEMODE *pNext;
}DEVICEMODE, *PDEVICEMODE;

// --------------------------------------------------------------------------------------
// Adapter struct - maintains info for each adapter on a system

typedef struct _ADAPTER
{
	TCHAR*		sAdapterName;					// Adapter string
	TCHAR*		sChipName;						// Graphics chip name
	TCHAR*		sDescription;					// Description string
	TCHAR*		sDevicePath;					// Path to device
	TCHAR*		sChipDAC;						
	TCHAR*		sDriver;						// Driver string
	TCHAR*		sVersion;						// Driver version
	TCHAR*		sPnPID;							// PnP ID
    DISPLAYMODE	DesktopMode;					// Desktop mode
	DWORD       dwChipID;						// Graphics chip ID
	DWORD       dwVendorID;						// Chip vendor ID
	UINT		nAdapterOrdinal;				// Display enum ordinal, since it may not match d3d
    
	GRAPHICSBUSTYPES    eBusType;				// AGP, PCI, etc.
	bool				bTextureAGP;			// Can it texture to AGP?
    bool				bHardwareVP;			// Does adapter support HW VP
	bool				bPure;					// Adapter supports pure
	RECT        		rMonitorRect;           // Desktop coordinates for this monitor
    PDISPLAYMODE 		pModes;       			// List of display modes
	int					nModes;					// Number of modes in list
	UINT				m_nDisplyModesAlloc;	// Number of display modes allocated
	 	
	// Legacy vars
    GUID        Guid;                           // Adapter GUID 
    HMONITOR    hMonitor;                       // Monitor handle

} ADAPTER, *PADAPTER;

// --------------------------------------------------------------------------------------
// LOCK_LIST - CnTexture, etc classes use this to persist lock info across Lock/Unlock

typedef struct _LOCK_LIST
{
	_LOCK_LIST*	pNext;
	UINT		uFirstFound;
	union {
	RECT*		pRects;
	UINT*		nSizes;			// deferenced as an array, doesn't look "right" as pSizes[i].cx
	};
	union {
	LOCKED_RECT* pLockedRects;
	BYTE**		 pLockedData;		// ""
	};
	union {
	BOX*		pBoxs;
	UINT*		nBoxSizes;			// deferenced as an array, doesn't look "right" as pSizes[i].cx
	};
	union {
	LOCKED_BOX*  pLockedBoxs;
	BYTE**		 pLockedBoxData;		// ""
	};
	BOOL*		bNullRect;			// ""
	BOOL*		bNullBox;			// ""
	BOOL*		bReadOnly;
} LOCK_LIST, *LPLOCK_LIST;

#ifndef MAKEFOURCC
    #define MAKEFOURCC(ch0, ch1, ch2, ch3)                              \
                ((DWORD)(BYTE)(ch0) | ((DWORD)(BYTE)(ch1) << 8) |       \
                ((DWORD)(BYTE)(ch2) << 16) | ((DWORD)(BYTE)(ch3) << 24 ))
#endif /* defined(MAKEFOURCC) */

#ifndef RGBTOYUV
#define RGBTOYUV(r,g,b,y,u,v) \
 (y) = (BYTE)(16.0 + \
              (0.257 * (double)(r)) + \
              (0.504 * (double)(g)) + \
              (0.098 * (double)(b))); \
 (u) = (BYTE)(128.0 - \
              (0.148 * (double)(r)) - \
              (0.291 * (double)(g)) + \
              (0.439 * (double)(b))); \
 (v) = (BYTE)(128.0 + \
              (0.439 * (double)(r)) - \
              (0.386 * (double)(g)) - \
              (0.071 * (double)(b)));

#define YUVTORGB(fY0,fY1,fU,fV,c1,c2) \
        c1->R = 0.00456621f * fY0                    + 0.00625893f * fV;\
        c1->G = 0.00456621f * fY0 - 0.00153632f * fU - 0.00318811f * fV;\
        c1->B = 0.00456621f * fY0 + 0.00791071f * fU;\
        c1->A = 1.0f;\
        c1->R = (c1->R < 0.0f) ? 0.0f : ((c1->R > 1.0f) ? 1.0f : c1->R);\
        c1->G = (c1->G < 0.0f) ? 0.0f : ((c1->G > 1.0f) ? 1.0f : c1->G);\
        c1->B = (c1->B < 0.0f) ? 0.0f : ((c1->B > 1.0f) ? 1.0f : c1->B);\
        c2->R = 0.00456621f * fY1                    + 0.00625893f * fV;\
        c2->G = 0.00456621f * fY1 - 0.00153632f * fU - 0.00318811f * fV;\
        c2->B = 0.00456621f * fY1 + 0.00791071f * fU;\
        c2->A = 1.0f;\
        c2->R = (c2->R < 0.0f) ? 0.0f : ((c2->R > 1.0f) ? 1.0f : c2->R);\
        c2->G = (c2->G < 0.0f) ? 0.0f : ((c2->G > 1.0f) ? 1.0f : c2->G);\
        c2->B = (c2->B < 0.0f) ? 0.0f : ((c2->B > 1.0f) ? 1.0f : c2->B);

    // The following conversion factors were used above to pack the YUV pixels.
    // They were shamelessly stolen from overfly.
    // fYOffsetConstant=16.0; fYRFactor=0.257; fYGFactor=0.504; fYBFactor=0.098;
    // fUOffsetConstant=128.0; fURFactor=0.148; fUGFactor=0.291; fUBFactor=0.439;
    // fVOffsetConstant=128.0; fVRFactor=0.439; fVGFactor=0.386; fVBFactor=0.071;

#define PACKYUY2(dw, y0, y1, u0, v0) \
 (dw) = (DWORD)( (((DWORD)(v0) << 24) & 0xff000000) | \
                 (((DWORD)(y1) << 16) & 0x00ff0000) | \
                 (((DWORD)(u0) << 8 ) & 0x0000ff00) | \
                 (((DWORD)(y0)      ) & 0x000000ff) )

#define UNPACKYUY2(dw, y0, y1, u0, v0) \
	v0 = (float)((dw & 0xff000000) >> 24) - 128.f;\
	y1 = (float)((dw & 0x00ff0000) >> 16) - 16.f;\
	u0 = (float)((dw & 0x0000ff00) >> 8) - 128.f;\
	y0 = (float)((dw & 0x000000ff) >> 0) - 16.f;

#define PACKUYVY(dw, y0, y1, u0, v0) \
 (dw) = (DWORD)( (((DWORD)(y1) << 24) & 0xff000000) | \
                 (((DWORD)(v0) << 16) & 0x00ff0000) | \
                 (((DWORD)(y0) << 8 ) & 0x0000ff00) | \
                 (((DWORD)(u0)      ) & 0x000000ff) )

#define UNPACKUYVY(dw, y0, y1, u0, v0) \
	y1 = (float)((dw & 0xff000000) >> 24) - 16.f;\
	v0 = (float)((dw & 0x00ff0000) >> 16) - 128.f;\
	y0 = (float)((dw & 0x0000ff00) >> 8) - 16.f;\
	u0 = (float)((dw & 0x000000ff) >> 0) - 128.f;

#endif

#endif

﻿<?xml version='1.0' encoding='utf-8' standalone='yes'?>
<Content xmlns="http://microsoft.com/schemas/Windows/Kits/Manifest">
  <ContentGroup
      BuildArchitecture="each"
      Destination="$(_BuildArch)"
      >
    <FeatureReference Name="Device.Graphics"/>
    <BinaryFile
        Destination="nttest\windowstest\graphics\d3d\conf"
        Name="GammaCorrect.exe"
        Source="$(OBJ_PATH)\$(O)"
        />
  </ContentGroup>
</Content>
//*****************************************************************************
// Copyright (C) Microsoft Corporation, 2005.
//
// File:    WMVTests.cpp
//
// History:
//  5/18/05      Created as part of the Codec Pack Automation BVT
//               suite.
//
//*****************************************************************************
//

#include "MFTDMediaEngine.h"
#include "resource.h"

EXTERN_GUID(D3D11_DECODER_PROFILE_HEVC_VLD_MAIN, 0x5b11d51b, 0x2f4c, 0x4452, 0xbc, 0xc3, 0x09, 0xf2, 0xa1, 0x16, 0x0c, 0xc0);
EXTERN_GUID(D3D11_DECODER_PROFILE_HEVC_VLD_MAIN10, 0x107af0e0, 0xef1a, 0x4d19, 0xab, 0xa8, 0x67, 0xa1, 0x63, 0x07, 0x3d, 0x13);

EXTERN_GUID(D3D11_DECODER_PROFILE_VP8_VLD, 0x463707f8, 0xa1d0, 0x4585, 0x87, 0x6d, 0x83, 0xaa, 0x6d, 0x60, 0xb8, 0x9e);
EXTERN_GUID(D3D11_DECODER_PROFILE_VP9_VLD_PROFILE0, 0x463707f8, 0xa1d0, 0x4585, 0x87, 0x6d, 0x83, 0xaa, 0x6d, 0x60, 0xb8, 0x9e);
EXTERN_GUID(D3D11_DECODER_PROFILE_VP9_VLD_10BIT_PROFILE2, 0xa4c749ef, 0x6ecf, 0x48aa, 0x84, 0x48, 0x50, 0xa7, 0xa1, 0x16, 0x5f, 0xf7);


using namespace WEX::Logging;
using namespace WEX::Common;
using namespace WEX::TestExecution;

#define SAFECOMFREE(x) \
    { \
		if((x) != NULL) \
        		{ \
			CoTaskMemFree(x); \
			(x) = NULL; \
        		} \
	}

HRESULT InitializePlaybackTestTable(CXmlRootConfig** ppXMLRootConfig, DWORD dwTestConfig);

//Entry point for the test.
//This function will instantiate the desired test object and call the methods for executing the test.


HRESULT VerifyHardwareDecoderPresent(GUID subType)
{
    HRESULT                 hr = S_OK;
    MFT_REGISTER_TYPE_INFO  mfrtiInputType = { GUID_NULL };
    IMFActivate**           ppMFTActivates = NULL;
    UINT32                  un32NumActivates = 0;

    do
    {
        mfrtiInputType.guidMajorType = MFMediaType_Video;
        mfrtiInputType.guidSubtype = subType;

        hr = MFTEnumEx(
            MFT_CATEGORY_VIDEO_DECODER,
            MFT_ENUM_FLAG_HARDWARE,
            &mfrtiInputType,
            NULL,
            &ppMFTActivates,
            &un32NumActivates
            );
        if (FAILED(hr))
        {
            break;
        }

        if (un32NumActivates == 0)
        {
            // No Hardware decoders are present
            hr = S_FALSE;
            break;
        }
    } while (false);

    for (UINT32 un32CurrActivate = 0; un32CurrActivate < un32NumActivates; un32CurrActivate++)
    {
        SAFERELEASE(ppMFTActivates[un32CurrActivate]);
    }

    SAFECOMFREE(ppMFTActivates);

    return hr;
}

GUID GetSubTypeFromString(CString stringSubType)
{
    GUID subtype = GUID_NULL;

    if (stringSubType == "MEDIASUBTYPE_MP4V")
    {
        subtype = MFVideoFormat_MP4V;
    }
    else if (stringSubType == "MFVideoFormat_MJPG" || stringSubType == "MEDIASUBTYPE_MJPG")
    {
        subtype = MFVideoFormat_MJPG;
    }
    else if (stringSubType == "MEDIASUBTYPE_H263")
    {
        subtype = MFVideoFormat_H263;
    }

    return subtype;
}


HRESULT CreateDXGIManager(
    UINT32*                 pun32ResetToken,
    IMFDXGIDeviceManager**  ppDXGIManager)
{
    HRESULT                 hr                      = S_OK;
    ID3D11Device*           pD3D11Device            = NULL;
    D3D_FEATURE_LEVEL       d3dflFeatureLevelUsed   = D3D_FEATURE_LEVEL_9_1;
    UINT32                  un32ResetToken          = 0;
    IMFDXGIDeviceManager*   pDXGIDeviceManager      = NULL;

    static const D3D_FEATURE_LEVEL levels [] = {
        D3D_FEATURE_LEVEL_11_1,

        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,

        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1
    };



    do
    {
        if( (pun32ResetToken == NULL)   ||
            (ppDXGIManager == NULL)     )
        {
            hr = E_POINTER;
            break;
        }

        (*pun32ResetToken)  = 0;
        (*ppDXGIManager)    = NULL;

        hr = D3D11CreateDevice(
            NULL,
            D3D_DRIVER_TYPE_HARDWARE,
            NULL,
            D3D11_CREATE_DEVICE_VIDEO_SUPPORT | D3D11_CREATE_DEVICE_BGRA_SUPPORT,
            levels,
            ARRAYSIZE(levels), 
            D3D11_SDK_VERSION,
            &pD3D11Device,
            &d3dflFeatureLevelUsed,
            NULL
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = MFCreateDXGIDeviceManager(
            &un32ResetToken,
            &pDXGIDeviceManager
            );
        if(FAILED(hr))
        {
            break;
        }

        hr = pDXGIDeviceManager->ResetDevice(
            pD3D11Device,
            un32ResetToken
            );
        if(FAILED(hr))
        {
            break;
        }

        (*pun32ResetToken)  = un32ResetToken;
        (*ppDXGIManager)    = pDXGIDeviceManager;
        (*ppDXGIManager)->AddRef();
    }while(false);

    SAFERELEASE(pD3D11Device);
    SAFERELEASE(pDXGIDeviceManager);

    return hr;
}


HRESULT VerifyDXVASupported(GUID guid)
{
    HRESULT                     hr = S_OK;
    BOOL                        bShutdownMF = FALSE;
    UINT32                      un32ResetToken = 0;
    UINT32                      un32NumDecoderProfiles = 0;
    IMFDXGIDeviceManager*       pDXGIDeviceManager = NULL;
    ID3D11VideoDevice*          pD3D11VideoDevice = NULL;
    HANDLE                      hDevice = NULL;
    GUID                        guidDecoder = GUID_NULL;
    BOOL                        bSupportsDXVA = FALSE;

    do
    {
        hr = CreateDXGIManager(
            &un32ResetToken,
            &pDXGIDeviceManager
            );
        if (FAILED(hr))
        {
            break;
        }

        hr = pDXGIDeviceManager->OpenDeviceHandle(
            &hDevice
            );
        if (FAILED(hr))
        {
            break;
        }

        hr = pDXGIDeviceManager->GetVideoService(
            hDevice,
            __uuidof(ID3D11VideoDevice),
            (void**)&pD3D11VideoDevice
            );

        if (FAILED(hr))
        {
            if (hr == E_NOINTERFACE)
            {
                /*If a ID3D11VideoDevice is specified and the D3D device created is using the reference rasterizer or WARP.Or it is a hardware device and you
                are using the Microsoft Basic Display Adapter.*/
                hr = S_FALSE;
            }
            break;
        }

        un32NumDecoderProfiles = pD3D11VideoDevice->GetVideoDecoderProfileCount();

        for (UINT32 un32CurrProfile = 0; un32CurrProfile < un32NumDecoderProfiles; un32CurrProfile++)
        {
            hr = pD3D11VideoDevice->GetVideoDecoderProfile(
                un32CurrProfile,
                &guidDecoder
                );
            if (FAILED(hr))
            {
                break;
            }
            // D3D11_DECODER_PROFILE_VP9_VLD_10BIT_PROFILE2
            // 8bit VP9 file can be decoded by 8bit decoder or 10bit hardware decoder
            if (guid == D3D11_DECODER_PROFILE_VP9_VLD_PROFILE0)
            {
                if ((guidDecoder == guid) || (guidDecoder == D3D11_DECODER_PROFILE_VP9_VLD_10BIT_PROFILE2))
                {
                    bSupportsDXVA = TRUE;
                    break;
                }
            }
            else if(guid == D3D11_DECODER_PROFILE_VP9_VLD_10BIT_PROFILE2 || guid == D3D11_DECODER_PROFILE_VP8_VLD)
            {
                 if (guid == guidDecoder)
                {
                    bSupportsDXVA = TRUE;
                    break;
                }
            } else if (guid == D3D11_DECODER_PROFILE_HEVC_VLD_MAIN) 
            {
                // D3D11_DECODER_PROFILE_HEVC_VLD_MAIN
                // 8bit HEVC file can be decoded by 8bit decoder or 10bit hardware decoder
                if ((guidDecoder == guid) || (guidDecoder == D3D11_DECODER_PROFILE_HEVC_VLD_MAIN10))
                {
                    bSupportsDXVA = TRUE;
                    break;
                }
            }
            else
            {
                // D3D11_DECODER_PROFILE_HEVC_VLD_MAIN10
                // 10bit hevc file has to be decoded by 10bit hevc hardware decoder
                if (guid == guidDecoder)
                {
                    bSupportsDXVA = TRUE;
                    break;
                }
            }
        }

        if (FAILED(hr))
        {
            break;
        }

        if (bSupportsDXVA == FALSE)
        {
            hr = S_FALSE;
            break;
        }
    } while (false);

    if (hDevice != NULL)
    {
        pDXGIDeviceManager->CloseDeviceHandle(hDevice);
        hDevice = NULL;
    }

    SAFERELEASE(pD3D11VideoDevice);
    SAFERELEASE(pDXGIDeviceManager);


    return hr;
}



#ifdef BUILD_MINWIN

void CMediaEngineTests::PlaybackTest110() {
    VERIFY_SUCCEEDED(ExecuteTest(110));
}
void CMediaEngineTests::PlaybackTest111() {
    VERIFY_SUCCEEDED(ExecuteTest(111));
}
void CMediaEngineTests::PlaybackTest118() {
    VERIFY_SUCCEEDED(ExecuteTest(118));
}
void CMediaEngineTests::PlaybackTest125() {
    VERIFY_SUCCEEDED(ExecuteTest(125));
}
void CMediaEngineTests::PlaybackTest129() {
    VERIFY_SUCCEEDED(ExecuteTest(129));
}
void CMediaEngineTests::PlaybackTest131() {
    VERIFY_SUCCEEDED(ExecuteTest(131));
}
void CMediaEngineTests::PlaybackTest150() {
    VERIFY_SUCCEEDED(ExecuteTest(150));
}
void CMediaEngineTests::PlaybackTest151() {
    VERIFY_SUCCEEDED(ExecuteTest(151));
}
void CMediaEngineTests::PlaybackTest152() {
    VERIFY_SUCCEEDED(ExecuteTest(152));
}
void CMediaEngineTests::PlaybackTest153() {
    VERIFY_SUCCEEDED(ExecuteTest(153));
}
void CMediaEngineTests::PlaybackTest154() {
    VERIFY_SUCCEEDED(ExecuteTest(154));
}
void CMediaEngineTests::PlaybackTest155() {
    VERIFY_SUCCEEDED(ExecuteTest(155));
}
void CMediaEngineTests::PlaybackTest156() {
    VERIFY_SUCCEEDED(ExecuteTest(156));
}
void CMediaEngineTests::PlaybackTest157() {
    VERIFY_SUCCEEDED(ExecuteTest(157));
}



//H263 tests
void CMediaEngineTests::PlaybackTest160() {
    VERIFY_SUCCEEDED(ExecuteTest(160));
}
void CMediaEngineTests::PlaybackTest161() {
    VERIFY_SUCCEEDED(ExecuteTest(161));
}
void CMediaEngineTests::PlaybackTest162() {
    VERIFY_SUCCEEDED(ExecuteTest(162));
}
void CMediaEngineTests::PlaybackTest163() {
    VERIFY_SUCCEEDED(ExecuteTest(163));
}


void CMediaEngineTests::PlaybackTest444() {
    VERIFY_SUCCEEDED(ExecuteTest(444));
}
void CMediaEngineTests::PlaybackTest450() {
    VERIFY_SUCCEEDED(ExecuteTest(450));
}

//Decoder  Tests
void CMediaEngineTests::H264PlaybackTest1()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(1));
}

void CMediaEngineTests::H264PlaybackTest2()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(2));
}

void CMediaEngineTests::H264PlaybackTest3()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(3));
}

void CMediaEngineTests::H264PlaybackTest4()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(4));
}

void CMediaEngineTests::H264PlaybackTest5()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(5));
}

void CMediaEngineTests::H264PlaybackTest6()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(6));
}

void CMediaEngineTests::H264PlaybackTest7()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(7));
}

void CMediaEngineTests::H264PlaybackTest8()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(8));
}

void CMediaEngineTests::H264PlaybackTest9()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(9));
}

void CMediaEngineTests::H264PlaybackTest11()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(11));
}

void CMediaEngineTests::H264PlaybackTest12()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(12));
}

void CMediaEngineTests::H264PlaybackTest13()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(13));
}

void CMediaEngineTests::H264PlaybackTest14()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(14));
}

void CMediaEngineTests::H264PlaybackTest15()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(15));
}

void CMediaEngineTests::H264PlaybackTest16()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(16));
}

void CMediaEngineTests::H264PlaybackTest17()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(17));
}

void CMediaEngineTests::H264PlaybackTest18()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(18));
}

void CMediaEngineTests::H264PlaybackTest19()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(19));
}


void CMediaEngineTests::WMVPlaybackTest32()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(32));
}
void CMediaEngineTests::WMVPlaybackTest33()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(33));
}
void CMediaEngineTests::WMVPlaybackTest34()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(34));
}
void CMediaEngineTests::WMVPlaybackTest35()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(35));
}
void CMediaEngineTests::WMVPlaybackTest36()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(36));
}
void CMediaEngineTests::WMVPlaybackTest37()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(37));
}
void CMediaEngineTests::WMVPlaybackTest38()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(38));
}
void CMediaEngineTests::WMVPlaybackTest39()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(39));
}
void CMediaEngineTests::WMVPlaybackTest40()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(40));
}
void CMediaEngineTests::WMVPlaybackTest41()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(41));
}
void CMediaEngineTests::WMVPlaybackTest42()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(42));
}
void CMediaEngineTests::WMVPlaybackTest43()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(43));
}

void CMediaEngineTests::H264FuzzTest100()
{
    ExecuteDXVATest(100);
}
void CMediaEngineTests::H264FuzzTest101()
{
    ExecuteDXVATest(101);
}
void CMediaEngineTests::H264FuzzTest102()
{
    ExecuteDXVATest(102);
}


void CMediaEngineTests::WMVFuzzTest114() {
    ExecuteDXVATest(114);
}
void CMediaEngineTests::WMVFuzzTest115() {
    ExecuteDXVATest(115);
}
void CMediaEngineTests::WMVFuzzTest116() {
    ExecuteDXVATest(116);
}


void CMediaEngineTests::H264QualityTest200()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(200));
}
void CMediaEngineTests::H264QualityTest201()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(201));
}
void CMediaEngineTests::H264QualityTest202()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(202));
}
void CMediaEngineTests::H264QualityTest203()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(203));
}
void CMediaEngineTests::H264QualityTest204()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(204));
}
void CMediaEngineTests::H264QualityTest205()
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(205));
}

void CMediaEngineTests::WMVQualityTest212() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(212));
}
void CMediaEngineTests::WMVQualityTest213() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(213));
}
void CMediaEngineTests::WMVQualityTest214() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(214));
}
void CMediaEngineTests::WMVQualityTest215() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(215));
}
void CMediaEngineTests::WMVQualityTest216() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(216));
}
void CMediaEngineTests::WMVQualityTest217() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(217));
}
void CMediaEngineTests::WMVQualityTest218() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(218));
}
void CMediaEngineTests::WMVQualityTest219() {
    VERIFY_SUCCEEDED(ExecuteDXVATest(219));
}
void CMediaEngineTests::WMVQualityTest220(void)
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(220));
}

void CMediaEngineTests::WMVQualityTest221(void)
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(221));
}

void CMediaEngineTests::H264QualityTest299(void)
{
    VERIFY_SUCCEEDED(ExecuteDXVATest(299));
}

void CMediaEngineTests::VisValPlaybackTest90() {
    VERIFY_SUCCEEDED(ExecuteVisValTest(90));
}
void CMediaEngineTests::VisValPlaybackTest91() {
    VERIFY_SUCCEEDED(ExecuteVisValTest(91));
}
void CMediaEngineTests::VisValPlaybackTest92() {
    VERIFY_SUCCEEDED(ExecuteVisValTest(92));
}

void CMediaEngineTests::HEVCPlaybackTest50(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(50));
}

void CMediaEngineTests::HEVCPlaybackTest51(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(51));
}

void CMediaEngineTests::HEVCPlaybackTest52(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(52));
}

void CMediaEngineTests::HEVCPlaybackTest53(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(53));
}

void CMediaEngineTests::HEVCPlaybackTest54(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(54));
}

void CMediaEngineTests::HEVCPlaybackTest55(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(55));
}

void CMediaEngineTests::HEVCPlaybackTest56(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(56));
}

void CMediaEngineTests::HEVCPlaybackTest57(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(57));
}

void CMediaEngineTests::HEVCPlaybackTest58(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(58));
}

void CMediaEngineTests::HEVCPlaybackTest59(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(59));
}

void CMediaEngineTests::HEVCPlaybackTest60(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(60));
}

void CMediaEngineTests::HEVCPlaybackTest61(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(61));
}

void CMediaEngineTests::HEVCPlaybackTest62(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(62));
}

void CMediaEngineTests::HEVCPlaybackTest63(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(63));
}

void CMediaEngineTests::HEVCPlaybackTest64(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(64));
}

void CMediaEngineTests::HEVCPlaybackTest65(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(65));
}

void CMediaEngineTests::HEVCFuzzTest120(void)
{
    ExecuteHEVCDXVATest(120);
}

void CMediaEngineTests::HEVCFuzzTest121(void)
{
    ExecuteHEVCDXVATest(121);
}

void CMediaEngineTests::HEVCFuzzTest122(void)
{
    ExecuteHEVCDXVATest(122);
}

void CMediaEngineTests::HEVCFuzzTest123(void)
{
    ExecuteHEVCDXVATest(123);
}

void CMediaEngineTests::HEVCFuzzTest124(void)
{
    ExecuteHEVCDXVATest(124);
}

void CMediaEngineTests::HEVCFuzzTest125(void)
{
    ExecuteHEVCDXVATest(125);
}

void CMediaEngineTests::HEVCQualityTest230(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(230));
}

void CMediaEngineTests::HEVCQualityTest231(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(231));
}

void CMediaEngineTests::HEVCQualityTest232(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(232));
}

void CMediaEngineTests::HEVCQualityTest233(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(233));
}

void CMediaEngineTests::HEVCQualityTest234(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(234));
}

void CMediaEngineTests::HEVCQualityTest235(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(235));
}

void CMediaEngineTests::HEVCQualityTest236(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(236));
}

void CMediaEngineTests::HEVCQualityTest237(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(237));
}

void CMediaEngineTests::HEVCQualityTest238(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(238));
}

void CMediaEngineTests::HEVCQualityTest239(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(239));
}

void CMediaEngineTests::HEVCQualityTest240(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(240));
}

void CMediaEngineTests::HEVCQualityTest241(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(241));
}

void CMediaEngineTests::HEVCQualityTest242(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(242));
}

void CMediaEngineTests::HEVCQualityTest243(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(243));
}

void CMediaEngineTests::HEVCQualityTest244(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(244));
}

void CMediaEngineTests::HEVCQualityTest245(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(245));
}

void CMediaEngineTests::HEVCQualityTest246(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(246));
}

void CMediaEngineTests::HEVCQualityTest247(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(247));
}

void CMediaEngineTests::HEVCQualityTest248(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(248));
}

void CMediaEngineTests::HEVCQualityTest249(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(249));
}

void CMediaEngineTests::HEVCQualityTest250(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(250));
}

void CMediaEngineTests::HEVCQualityTest251(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(251));
}

void CMediaEngineTests::HEVCQualityTest252(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(252));
}

void CMediaEngineTests::HEVCQualityTest253(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(253));
}

void CMediaEngineTests::HEVCQualityTest254(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(254));
}

void CMediaEngineTests::HEVCQualityTest255(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(255));
}

void CMediaEngineTests::HEVCQualityTest256(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(256));
}

void CMediaEngineTests::HEVCQualityTest257(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(257));
}

void CMediaEngineTests::HEVCQualityTest258(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(258));
}

//OneCore tests
void CMediaEngineTests::MJPEGQualityTest1()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(1));
}
void CMediaEngineTests::MSS1QualityTest2()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(2));
}
void CMediaEngineTests::DVQualityTest3()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(3));
}
void CMediaEngineTests::MP43QualityTest4()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(4));
}
void CMediaEngineTests::Mpeg4DecQualityTest5()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(5));
}
void CMediaEngineTests::MP4SQualityTest6()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(6));
}
void CMediaEngineTests::MP4ResChange()
{
    VERIFY_SUCCEEDED(ExecuteOneCoreTest(7));
}
#endif

void CMediaEngineTests::HEVCPlaybackTest66(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(66));
}
void CMediaEngineTests::HEVCQualityTest259(void)
{
    VERIFY_SUCCEEDED(ExecuteHEVCDXVATest(259));
}

static HRESULT ReadRegDWORD(
    __in LPCWSTR szKeyName,
    __in LPCWSTR szValueName,
    __out DWORD* pValue
    )
{
    DWORD cbValue = sizeof(*pValue);
    HRESULT hr = S_OK;

    const LONG ret = RegGetValue(HKEY_LOCAL_MACHINE,
        szKeyName,
        szValueName,
        RRF_RT_REG_DWORD,
        nullptr,
        pValue,
        &cbValue);
    if( ret != ERROR_SUCCESS )
    {
        hr = HRESULT_FROM_WIN32(ret);
    }
    else if (cbValue != sizeof(*pValue)) {
        hr = E_INVALIDARG;
    }
    return hr;
}

void CMediaEngineTests::MediaPowerProfileTest() {
    wchar_t szPowerKeys[] = L"SYSTEM\\ControlSet001\\Control\\Power\\PowerSettings\\54533251-82be-4824-96c1-47b60b740d00\\";
    wchar_t szPostfix[] = L"\\4569E601-272E-4869-BCAB-1C6C03D7966F\\381b4222-f694-41f0-9685-ff5bb260df2e";
    wchar_t szCPIncrease[] = L"2ddd5a84-5a71-437e-912a-db0b8c788732";

    wchar_t szFull[500];
    VERIFY_SUCCEEDED(StringCchPrintf(szFull, ARRAYSIZE(szFull), L"%s%s%s", szPowerKeys, szCPIncrease, szPostfix));

    DWORD dwValue = 0;
    VERIFY_SUCCEEDED(ReadRegDWORD(szFull, L"AcSettingIndex", &dwValue));

    if (dwValue != 2) {
        VERIFY_SUCCEEDED(E_INVALIDARG);
    }

    VERIFY_SUCCEEDED(ReadRegDWORD(szFull, L"DcSettingIndex", &dwValue));

    if (dwValue != 2) {
        VERIFY_SUCCEEDED(E_INVALIDARG);
    }
}

void CMediaEngineTests::H264DRMTest1() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(1));
}

void CMediaEngineTests::H264DRMTest2() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(2));
}

void CMediaEngineTests::H264DRMTest3() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(3));
}

void CMediaEngineTests::H264DRMTest4() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(4));
}

void CMediaEngineTests::H264DRMTest5() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(5));
}

void CMediaEngineTests::HEVCDRMTest6() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(6));
}

void CMediaEngineTests::HEVCDRMTest7() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(7));
}

void CMediaEngineTests::HEVCDRMTest8() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(8));
}

void CMediaEngineTests::HEVCDRMTest9() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(9));
}

void CMediaEngineTests::HEVCDRMTest10() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(10));
}

void CMediaEngineTests::H264MPOTest11() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(11));
}


void CMediaEngineTests::HEVCMPOTest12() {
    VERIFY_SUCCEEDED(ExecutePipelineTest(12));
}

void CMediaEngineTests::VP9PlaybackTest1(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(1));
}

void CMediaEngineTests::VP9PlaybackTest2(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(2));
}

void CMediaEngineTests::VP9PlaybackTest3(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(3));
}

void CMediaEngineTests::VP9PlaybackTest4(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(4));
}

void CMediaEngineTests::VP9PlaybackTest5(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(5));
}

void CMediaEngineTests::VP9PlaybackTest6(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(6));
}

void CMediaEngineTests::VP9PlaybackTest7(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(7));
}

void CMediaEngineTests::VP9PlaybackTest8(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(8));
}

void CMediaEngineTests::VP9PlaybackTest9(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(9));
}



void CMediaEngineTests::VP9QualityTest50(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(50));
}

void CMediaEngineTests::VP9QualityTest51(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(51));
}

void CMediaEngineTests::VP9QualityTest52(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(52));
}

void CMediaEngineTests::VP9QualityTest53(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(53));
}

void CMediaEngineTests::VP9QualityTest54(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(54));
}

void CMediaEngineTests::VP9QualityTest55(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDXVATest(55));
}

void CMediaEngineTests::VP9QualityTest56(void)
{
    VERIFY_SUCCEEDED(ExecuteVPxDX