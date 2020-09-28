rt( m_pParentRenderer->m_pDevice_ID3D11VideoDevice != nullptr );
	const UINT VideoDecoderProfileCount = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->GetVideoDecoderProfileCount();


	//
	// Find a decoder profile to use.
	//

	// If the count is 0, and we call GetVideoDecoderProfile, the
	// runtime will crash, so we'll fail instead.
	if ( 0 == VideoDecoderProfileCount )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"GetVideoDecoderProfileCount returned 0."
		);

		goto Cleanup;
	};

	for
	(
		UINT iCurrentVideoDecoderProfile = 0;
		iCurrentVideoDecoderProfile < VideoDecoderProfileCount &&
			bFoundSupportedVideoDecoderProfile != TRUE;
		++iCurrentVideoDecoderProfile
	)
	{
		HRESULT hGetVideoDecoderProfileResult = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->GetVideoDecoderProfile
		(
			iCurrentVideoDecoderProfile,
			&VideoDecoderProfileID
		);

		if ( FAILED( hGetVideoDecoderProfileResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hGetVideoDecoderProfileResult,
				L"GetVideoDecoderProfile failed."
			);

			goto Cleanup;
		};

		HRESULT hCheckVideoDecoderFormatResult = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->CheckVideoDecoderFormat
		(
			&VideoDecoderProfileID,
			SwapChainBufferFormat,
			&bFoundSupportedVideoDecoderProfile
		);

		if ( FAILED( hCheckVideoDecoderFormatResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCheckVideoDecoderFormatResult,
				L"CheckVideoDecoderFormat failed."
			);

			goto Cleanup;
		};
	};


	// If we got here and we still haven't found a supported
	// video decoder profile, something is very wrong, because
	// CheckFormatSupport indicated DECODER_OUTPUT support.
	if ( bFoundSupportedVideoDecoderProfile != TRUE )
	{
		LOG_ERROR
		(
			ERROR_TYPE_BOOL,
			false,
			L"CheckFormatSupport indicated decoder output support for this format, but no supported video decoder profile was found."
		);

		goto Cleanup;
	};


	//
	// Log the decoder profile that we selected.
	//
	// N.B. [SEdmison]:  These IDs are defined in dxva2api.h.
	//

	WCHAR GUIDFormattingBuffer[ 64 ] = L"";
	int nGUIDFormattingBuffer = StringFromGUID2
	(
		VideoDecoderProfileID,
		GUIDFormattingBuffer,
		ARRAYSIZE( GUIDFormattingBuffer )
	);

	LOG_MESSAGE
	(
		MESSAGE_TYPE_INFORMATIONAL,
		WEX::Common::String().Format
		(
			L"- Using decoder profile %s.",
			GUIDFormattingBuffer
		)
	);


	//
	// Create video decoder views on the swap chain decode buffers.
	//

	for
	(
		UINT iCurrentSwapChainBuffer = 0;
		iCurrentSwapChainBuffer < m_pParentRenderer->m_cSwapChainBufferTextureArraySlices[ CurrentSwapChainSlot ];
		++iCurrentSwapChainBuffer
	)
	{
		//
		// Convert normalized RGBA buffer colors to a colors that
		// can be passed to ClearView.
		//

		float BackgroundClearViewColor[ 4 ] = { 0.f, 0.f, 0.f, 1.f };
		HRESULT hInitializeBackgroundClearViewColorResult = InitializeClearViewColor
		(
			SwapChainBufferFormat,
			AlphaMode,
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
		// Create a video output view for this array slice.
		//

		D3D11_VIDEO_DECODER_OUTPUT_VIEW_DESC VideoOutputViewDescriptor;
		ZeroMemory( &VideoOutputViewDescriptor, sizeof( VideoOutputViewDescriptor ) );
		VideoOutputViewDescriptor.DecodeProfile = VideoDecoderProfileID;
		VideoOutputViewDescriptor.ViewDimension = D3D11_VDOV_DIMENSION_TEXTURE2D;
		VideoOutputViewDescriptor.Texture2D.ArraySlice = iCurrentSwapChainBuffer;


		assert( m_pParentRenderer->m_pDevice_ID3D11VideoDevice != nullptr );
		assert( pVideoDecoderOutputView == nullptr );
		HRESULT hCreateVideoDecoderOutputViewResult = m_pParentRenderer->m_pDevice_ID3D11VideoDevice->CreateVideoDecoderOutputView
		(
			m_pParentRenderer->m_ppSwapChainBufferTextureArrays[ CurrentSwapChainSlot ],
			&VideoOutputViewDescriptor,
			&pVideoDecoderOutputView
		);

		if ( FAILED( hCreateVideoDecoderOutputViewResult ) )
		{
			LOG_ERROR
			(
				ERROR_TYPE_HRESULT,
				hCreateVideoDecoderOutputViewResult,
				L"CreateVideoDecoderOutputView failed."
			);

			goto Cleanup;
		};

		if ( pVideoDecoderOutputView == nullptr )
		{
			LOG_ERROR
			(
				ERROR_TYPE_BOOL,
				false,
				L"CreateVideoDecoderOutputView succeeded but returned NULL video decoder output view pointer."
			);

			goto Cleanup;
		};


		//
		// Clear the entire buffer to the background color.
		//

		LOG_MESSAGE
		(
			MESSAGE_TYPE_INFORMATIONAL,
			WEX::Common::String().Format
			(
				L"  - Clearing buffer %u to (%f, %f, %f, %f).",
				iCurrentSwapChainBuffer,
				BackgroundClearViewColor[ 0 ],
				BackgroundClearViewColor[ 1 ],
				BackgroundClearViewColor[ 2 ],
				BackgroundClearViewColor[ 3 ]
			)
		);

		assert( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 != nullptr );
		m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->ClearView
		(
			pVideoDecoderOutputView,
			BackgroundClearViewColor,
			nullptr,
			0
		);


		//
		// Fill color swatches in the buffer.
		//

		for
		(
			UINT iCurrentSwatch = 0;
			iCurrentSwatch < ARRAYSIZE( s_BackgroundSwapChainColorSwatches );
			++iCurrentSwatch
		)
		{
			//
			// Convert the color if needed.
			//

			float SwatchClearViewColor[ 4 ] = { 0.f, 0.f, 0.f, 1.f };
			HRESULT hInitializeSwatchClearViewColorResult = InitializeClearViewColor
			(
				SwapChainBufferFormat,
				AlphaMode,
				s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color,
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


			LOG_MESSAGE
			(
				MESSAGE_TYPE_DEBUG,
				WEX::Common::String().Format
				(
					L"  - Converted swatch %u color for %s swap chain buffer %u from normalized RGB color (%f, %f, %f, %f) to clear color (%f, %f, %f, %f).",
					iCurrentSwatch,
					( CurrentSwapChainSlot == SWAPCHAIN_SLOT_FOREGROUND ) ? L"foreground" : L"background",
					iCurrentSwapChainBuffer,
					s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.r,
					s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.g,
					s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.b,
					s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].Color.a,
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
			ClearViewRectangle.left = (LONG) ( s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.left * m_pParentRenderer->m_SceneWidth );
			ClearViewRectangle.top = (LONG) ( s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.top * m_pParentRenderer->m_SceneHeight );
			ClearViewRectangle.right = (LONG) ( s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.right * m_pParentRenderer->m_SceneWidth );
			ClearViewRectangle.bottom = (LONG) ( s_BackgroundSwapChainColorSwatches[ iCurrentSwatch ].RelativeRectangle.bottom * m_pParentRenderer->m_SceneHeight );

			LOG_MESSAGE
			(
				MESSAGE_TYPE_INFORMATIONAL,
				WEX::Common::String().Format
				(
					L"  - Clearing swatch (%li, %li) to (%li, %li) within swap chain buffer %u to (%f, %f, %f, %f).",
					ClearViewRectangle.left,
					ClearViewRectangle.top,
					ClearViewRectangle.right,
					ClearViewRectangle.bottom,
					iCurrentSwapChainBuffer,
					SwatchClearViewColor[ 0 ],
					SwatchClearViewColor[ 1 ],
					SwatchClearViewColor[ 2 ],
					SwatchClearViewColor[ 3 ]
				)
			);

			assert( m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1 != nullptr );
			m_pParentRenderer->m_pImmediateContext_ID3D11DeviceContext1->ClearView
			(
				pVideoDecoderOutputView,
				SwatchClearViewColor,
				&ClearViewRectangle,
				1
			);
		};


		SAFE_RELEASE( pVideoDecoderOutputView );
	};


	hOverallResult = S_OK;


Cleanup:
	SAFE_RELEASE( pVideoDecoderOutputView );


	return hOverallResult;
};
/*++
Copyright (c) Microsoft Corporation

Module Name:

StateMachineEngine.c

Abstract:

     This header file contains State Machines for UCX .

--*/



#include "pch.h"

#include "StateMachineEngine.tmh"

/*
FORCEINLINE
__drv_acquiresExclusiveResource(SMEngineLock)
VOID
StateMachineEngine_AcquireLock(
    __in
        PKSPIN_LOCK StateLock,
    __in __drv_savesIRQL
        PKIRQL      irql
)
{
    KeAcquireSpinLock(StateLock, irql);
}

FORCEINLINE
__drv_mustHold(SMEngineLock)
__drv_releasesExclusiveResource(SMEngineLock)
VOID
StateMachineEngine_ReleaseLock(
    __in
        PKSPIN_LOCK StateLock,
    __in __drv_restoresIRQL
        KIRQL       irql
)
{
    KeReleaseSpinLock(StateLock, irql);
}
*/

//__drv_mustHold(SMEngineLock)
SM_ENGINE_EVENT
StateMachineEngine_DequeueEvent(
    __in
        PSM_ENGINE_CONTEXT                      SmEngineContext,
    __in
        PSM_ENGINE_STATE_TABLE_ENTRY            CurrentState
    )
/*++

Routine Description:

    Retrieves the next event to handle and determines what the next state
    should be.
    Note:  The state machine lock must be held when this function is called


--*/

{

    SM_ENGINE_EVENT           event;

    TRY {

        WHILE (TRUE) {

            if (FALSE == IsQueueEmpty(&SmEngineContext->NormalPriQueue)) {

                event = DequeueEvent(&SmEngineContext->NormalPriQueue);

            } else if ((CurrentState->StateFlags & SmEngineStateFlagHandlesLowPriEvents)
                       && (FALSE == IsQueueEmpty(&SmEngineContext->LowPriQueue))) {

                event = DequeueEvent(&SmEngineContext->LowPriQueue);

            } else {

                event = SmEngineEventNull;
                break;
            }

            break;

        }


    } FINALLY {

    }

    return event;

} // StateMachineEngine_DequeueEvent

//__drv_mustHold(SMEngineLock)
__drv_requiresIRQL(DISPATCH_LEVEL)
PFN_SM_ENGINE_AUTO_EVENT_HANDLER
StateMachineEngine_GetAutoEventHandler(
    __in
        PSM_ENGINE_CONTEXT             SmEngineContext,
    __in
        PSM_ENGINE_STATE_TABLE_ENTRY   CurrentState,
    __in
        SM_ENGINE_EVENT                Event
    )
/*++

Routine Description:

    Determines if there is an auto event handler for this state.
    If there is one, that handler is returned.

    Note:  The state machine lock must be held when this function is called

Arguments:

    SmEngineContext - State Machine Engine Context

    CurrentState - The current state of the state machine.

    Event - Event that is being handled by State Machine.

Return:
    Returns the Auto Event Handler if any.

--*/
{

    ULONG                            index;
    PFN_SM_ENGINE_AUTO_EVENT_HANDLER autoEventHandler;

    TRY {

        //
        // Ok, so now we have an event, we need to figure out which state is
        // next.
        //

        autoEventHandler = NULL;
        for (index = 0;
              CurrentState->AutoEventTransitions[index].Event != SmEngineEventNull;
              index++) {

            if (CurrentState->AutoEventTransitions[index].Event == Event) {

                autoEventHandler = CurrentState->AutoEventTransitions[index].AutoEventHandler;

                NT_ASSERT(autoEventHandler != NULL);

                RecordState(SmEngineContext, CurrentState->State, Event);

                break;

            }

        }

    } FINALLY {


    }

    return autoEventHandler;

} // StateMachineEngine_GetAutoEventHandler


//__drv_mustHold(SMEngineLock)
__drv_requiresIRQL(DISPATCH_LEVEL) 
PSM_ENGINE_STATE_TABLE_ENTRY
StateMachineEngine_GetNextState(
    __in
        PSM_ENGINE_CONTEXT             SmEngineContext,
    __in
        PSM_ENGINE_STATE_TABLE_ENTRY   CurrentState,
    __in
        SM_ENGINE_EVENT                Event
    )
/*++

Routine Description:

    Retrieves the next event to handle and determines what the next state
    should be.
    Note:  The state machine lock must be held when this function is called

Arguments:

    SmEngineContext - State Machine Engine Context

    CurrentState - The current state of the state machine.

    Event - Event that is being handled by State Machine.

Return:
    Returns the next state.

--*/
{
    
    PSM_ENGINE_STATE_TABLE_ENTRY    nextStateEntry;    
    ULONG                           nextStateIndex;
    ULONG                           index;
    
    TRY {                                             
        
        //
        // Ok, so now we have an event, we need to figure out which state is
        // next.
        //

        for (index = 0;
              CurrentState->Transitions[index].Event != SmEngineEventNull;
              index++) {

            if (CurrentState->Transitions[index].Event == Event) {

                break;

            }

        }

        //
        // If the found event is SmEngineEventNull, then something is wrong.
        // We do not expect this event.
        //
        if (CurrentState->Transitions[index].Event == SmEngineEventNull) {

            Ucx_DbgBreak("Unexpected Event");

            nextStateEntry = NULL;
            RecordState(SmEngineContext, UcxEndpointStateNull, Event);

        } else {

            nextStateIndex = CurrentState->Transitions[index].TargetStateIndex;
            nextStateEntry = SmEngineContext->StateTable[nextStateIndex];
            RecordState(SmEngineContext, nextStateEntry->State, Event);

        }


    } FINALLY {


    }

    return nextStateEntry;

} // StateMachineEngine_GetNextState

UCX_FORWARD_PROGRESS_WORKITEM_CALLBACK StateMachineEngine_OnPassive;

VOID
StateMachineEngine_OnPassive(
    PDEVICE_OBJECT                    DeviceObject,
    PVOID                             SmEngineContextAsPVOID,
    PUCX_FORWARD_PROGRESS_WORKITEM    UcxForwardProgressWorkItem
)
/*++

Routine Description:

    OnPassive Callback that is called when State Machine Engine wants to invoke a StateEntryFunction
    on Passive Level.

Arguments:

    DeviceObject - (Unused) The Wdm Device Object

    SmEngineContext - State Machine Engine Context

    UcxForwardProgressWorkItem - (UnUsed) Pointer to a Ucx Forward Progress Workitem

--*/
{
    PSM_ENGINE_CONTEXT smEngineContext = (PSM_ENGINE_CONTEXT) SmEngineContextAsPVOID;

    UNREFERENCED_PARAMETER(DeviceObject);
    UNREFERENCED_PARAMETER(UcxForwardProgressWorkItem);

    StateMachineEngine_EventAdd(smEngineContext, SmEngineEventOnPassive);

}

VOID
StateMachineEngine_ReleaseResources(
    __in
        PSM_ENGINE_CONTEXT            SmEngineContext
)
/*++

Routine Description:

    OnPassive Callback that is called when State Machine Engine wants to free
    resources
Arguments:

    SmEngineContext - State Machine Engine Context

--*/
{

    TRY {
        Controller_ForwardProgressWorkItemDelete(SmEngineContext->UcxForwardProgressWorkItem);
    } FINALLY {
    }
}

BOOLEAN
StateMachineEngine_EventAdd(
    __in
        PSM_ENGINE_CONTEXT       SmEngineContext,
    __in
        SM_ENGINE_EVENT          Event
    )
/*++

Routine Description:

    This routine handles adding and processing new events

Arguments:

    SmEngineContext - State Machine Engine Context

    Event - Event that is to be added to the State Machine.

Return:

    BOOLEAN that returns true if the Event was accepted and was scheduled.
    FALSE otherwise.

Comments:
    Due to compatibility there are restrictions on when can we handle the Abort requests from
    So certain states in the Endpoint State Machine reject the UcxEndpointEventAbortUrbFromClient.
    This is done so that the Abort Request can be failed on the calling thread itself (condition
    needed to meet compat)

--*/

{
    PSM_ENGINE_STATE_TABLE_ENTRY      currentStateEntry;
    PSM_ENGINE_STATE_TABLE_ENTRY      nextStateEntry;
    PFN_SM_ENGINE_AUTO_EVENT_HANDLER  autoEventHandler;
    BOOLEAN                           acceptEvent;
    BOOLEAN                           releaseSpinLock;
    BOOLEAN                           releaseRef = FALSE;
    SM_ENGINE_EVENT                   nextEvent;
    KIRQL                             irql;

    TRY {

        //
        // First, grab the state machine lock
        //

        KeAcquireSpinLock(&SmEngineContext->StateLock, &irql);

        releaseSpinLock = TRUE;

        //
        // Log this event in the history
        //
        RecordEvent(SmEngineContext, Event);
        
        currentStateEntry = SmEngineContext->CurrentStateEntry;
        
        //
        // Special Check for The AbortUrb from client to maintain compatiblity.
        // This ensure that only certain states handle the Abort Urb, in other cases the
        // abort urb must be failed on the same thread.
        //
        if (SmEngineContext->SpecialHandlingForAbortUrb != 0) {
            if (Event == UcxEndpointEventAbortUrbFromClient) {
                if (FALSE == TEST_FLAG(currentStateEntry->StateFlags,
                                       SmEngineStateFlagHandlesAbortUrb)) {
                    acceptEvent = FALSE;
                    LEAVE;
                }
            }
        }

        //
        // Special check for ControllerResetEventHCDNeedsReset and
        // ControllerResetEventResetCompleteFromHCD events.
        // These events can come many times, and this check ensure that maximum 1 of each
        // event is stored in the event queue at a time. States must still handle repeated
        // events, we are just protecting the queue from overflowing with too many in a
        // short time.
        //
        if (SmEngineContext->SpecialHandlingForControllerResetEventsFromHCD != 0) {
            if (ControllerResetEventHCDNeedsReset == Event) {
                if (SmEngineContext->EventHCDNeedsResetIsQueued == 1) {
                    acceptEvent = FALSE;
                    LEAVE;
                } else {
                    SmEngineContext->EventHCDNeedsResetIsQueued = 1;
                }
            } else if (ControllerResetEventResetCompleteFromHCD == Event) {
                if (SmEngineContext->EventResetCompleteFromHCDIsQueued == 1) {
                    acceptEvent = FALSE;
                    LEAVE;
                } else {
                    SmEngineContext->EventResetCompleteFromHCDIsQueued = 1;
                }
            }
        }

        acceptEvent = TRUE;

        //
        // NOTE: UcxEndpointEventControllerResetComplete needs Special Handling since
        // it needs to be handled when the state machine is Paused.
        //
        if (Event == UcxEndpointEventControllerResetComplete) {
            if (SmEngineContext->StateMachinePaused == FALSE ||
                SmEngineContext->StateMachineRunning == FALSE) {
                SmEngineContext->UnexpectedEventCount++;
                //
                // On recieving UcxEndpointEventControllerResetComplete
                // State machine should be in a Paused State
                //
                Ucx_DbgBreak("On recieving UcxEndpointEventControllerResetComplete State machine should be in a Paused State");
                LEAVE;
            }
        }

        if (Event == SmEngineEventOnPassive) {
            if (SmEngineContext->StateMachinePausedForPassive == FALSE ||
                SmEngineContext->StateMachineRunning == FALSE) {
                SmEngineContext->UnexpectedEventCount++;
                //
                // On recieving UcxEndpointEventControllerResetComplete, State machine should be 
                // in a Paused For Passive State
                //
                Ucx_DbgBreak("On recieving UcxEndpointEventControllerResetComplete, State machine should be in a Paused For Passive State");
                LEAVE;
            }
        }

        if (Event != SmEngineEventOnPassive &&
            Event != UcxEndpointEventControllerResetComplete) {

            //
            // Ok, time to enqueue this event
            //
            switch (EVENT_PRIORITY(Event)) {
            case SmEngineEventPriorityNormal:
                EnqueueEvent(&SmEngineContext->NormalPriQueue,
                             Event);
                break;
            case SmEngineEventPriorityLow:
                EnqueueEvent(&SmEngineContext->LowPriQueue,
                             Event);
                break;
            default:
                NT_ASSERTMSG("Invalid Type of Event %x", Event);
                break;
            }

            if (SmEngineContext->StateMachineRunning == TRUE) {
                //
                // The state machine is already handling an event, so we need
                // to bail out
                //
                LEAVE;
            }

            SmEngineContext->StateMachineRunning = TRUE;

        }
        //
        // As the state machine runs, events get handled for our Owner Object.
        // One of the event could actually delete the Owner object.
        // Thus to p