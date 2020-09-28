revent a bugcheck, hold an additional reference on
        // the Object, and its needed children.
        //
        WdfObjectReferenceWithTag(SmEngineContext->Owner, TAG(StateMachineEngine_EventAdd));

        releaseRef = TRUE;

        if (Event == UcxEndpointEventControllerResetComplete) {

            //
            // This is a special Event that needs to be handled when the state machine is
            // in a pused state.
            //
            NT_ASSERT(SmEngineContext->StateMachinePaused);

            nextEvent = Event;

        } else if (Event == SmEngineEventOnPassive) {

            //
            // This is a special Event that is used to indicate that the state machine
            // engine has transitioned to Passive Level.
            //
            RecordState(SmEngineContext, currentStateEntry->State, Event);
            
            SmEngineContext->StateMachinePausedForPassive = FALSE;

            KeReleaseSpinLock(&SmEngineContext->StateLock, irql);

            //
            // Now that we are on Passive call the state machine'e callback routine.
            //
            nextEvent = currentStateEntry->StateEntryFunction(
                SmEngineContext->StateEntryFunctionContext);

            KeAcquireSpinLock(&SmEngineContext->StateLock, &irql);

        } else {

            nextEvent = SmEngineEventNull;
        }

        //
        // Now start handling the events
        //
        WHILE (TRUE) {

            if (nextEvent == SmEngineEventNull) {

                nextEvent = StateMachineEngine_DequeueEvent(SmEngineContext, currentStateEntry);

            }

            if (SmEngineContext->SpecialHandlingForControllerResetEventsFromHCD != 0) {
                if (ControllerResetEventHCDNeedsReset == nextEvent) {
                    NT_ASSERT(SmEngineContext->EventHCDNeedsResetIsQueued == 1);
                    SmEngineContext->EventHCDNeedsResetIsQueued = 0;
                }
                if (ControllerResetEventResetCompleteFromHCD == nextEvent) {
                    NT_ASSERT(SmEngineContext->EventResetCompleteFromHCDIsQueued == 1);
                    SmEngineContext->EventResetCompleteFromHCDIsQueued = 0;
                }
            }

            if (nextEvent == SmEngineEventNull) {

                //
                // No More Events
                //

                SmEngineContext->StateMachineRunning = FALSE;

                LEAVE;

            }

            autoEventHandler = StateMachineEngine_GetAutoEventHandler(SmEngineContext,
                                                                      currentStateEntry,
                                                                      nextEvent);

            if (autoEventHandler) {

                KeReleaseSpinLock(&SmEngineContext->StateLock, irql);

                nextEvent = (*autoEventHandler)(SmEngineContext->StateEntryFunctionContext);

                if (nextEvent == SmEngineEventPaused) {
                    releaseSpinLock = FALSE;
                    //
                    // We need to bail out here.
                    // UcxEndpointEventControllerResetComplete would be the one
                    // responsible to resume the statemachine.
                    //
                    LEAVE;
                }
                KeAcquireSpinLock(&SmEngineContext->StateLock, &irql);
                continue;

            }

            nextStateEntry = StateMachineEngine_GetNextState(SmEngineContext, 
                                                             currentStateEntry, 
                                                             nextEvent);

            nextEvent = SmEngineEventNull;
            
            if (nextStateEntry == NULL) {
                //
                // Case of an unexpected Event. Try to make further progress by ignorning the event.
                //
                SmEngineContext->UnexpectedEventCount++;
                continue;
            }
            
            SmEngineContext->CurrentStateEntry = nextStateEntry;
            currentStateEntry = nextStateEntry;

            if (currentStateEntry->StateEntryFunction != NULL) {

                if (TEST_FLAG(currentStateEntry->StateFlags, SmEngineStateFlagRequiresPassive) &&
                    (irql != PASSIVE_LEVEL)) {

                    //
                    // The State Entry routine must be called at Passive Level, however we are
                    // not at passive level.
                    //
                    SmEngineContext->StateMachinePausedForPassive = TRUE;

                    RecordState(SmEngineContext,
                                SmEngineStateWaitingForPassive,
                                SmEngineEventNeedsPassive);

                    Controller_ForwardProgressWorkItemEnqueue(
                        SmEngineContext->UcxForwardProgressWorkItem,
                        StateMachineEngine_OnPassive,
                        SmEngineContext,
                        FALSE);

                    LEAVE;
                }

                KeReleaseSpinLock(&SmEngineContext->StateLock, irql);


                nextEvent = currentStateEntry->StateEntryFunction(
                    SmEngineContext->StateEntryFunctionContext);

                if (nextEvent == SmEngineEventPaused) {
                    releaseSpinLock = FALSE;
                    //
                    // We need to bail out here.
                    // UcxEndpointEventControllerResetComplete would be the one
                    // responsible to resume the statemachine.
                    //
                    LEAVE;
                }
                KeAcquireSpinLock(&SmEngineContext->StateLock, &irql);

            }

        }

    } FINALLY {

        if (releaseSpinLock) {
            KeReleaseSpinLock(&SmEngineContext->StateLock, irql);
        }

        if (releaseRef) {
            WdfObjectDereferenceWithTag(SmEngineContext->Owner, TAG(StateMachineEngine_EventAdd));
        }

    }

    return acceptEvent;

} // StateMachineEngine_EventAdd

_Must_inspect_result_
NTSTATUS
StateMachineEngine_Init(
    __in
        PUCXCONTROLLER_PRIVCONTEXT                UcxControllerContext,
    __out
        PSM_ENGINE_CONTEXT                        SmEngineContext,
    __in
        WDFOBJECT                                 Owner,
    __in
        PVOID                                     StateEntryFunctionContext,
    __in
        ULONG                                     InitialStateIndex,
    __in
        PSM_ENGINE_STATE_TABLE_ENTRY*             StateTable,
    __in
        PFN_SM_ENGINE_ETW_WRITE_STATE_EVENT       EtwWriteStateEvent,
    __in
        PFN_SM_ENGINE_ETW_WRITE_STATE_TRANSITION  EtwWriteStateTransition
)
/*++

Routine Description:

    This routine Initializes the StateMachine Engine Context

Arguments:

    UcxControllerContext - The Ucx Controller Context

    SmEngineContext - State Machine Engine Context, to be initialized

    Owner - WdfObject indicating the owner of the state machine

    StateEntryFunctionContext - Pointer to the context that is passed in each state entry function
    
    InitialStateIndex - Index of the Initial State in which the state machine engine will be in
    
    StateTable - Pointer to the state table.        

Return:

    STATUS_INSUFFICIENT_RESOURCES - insufficient resources
    NTSTATUS

Comments:

    For state entry function on Passive Level, we use UCX_FORWARD_PROGRESS_WORKITEM.
    For that we must allocate a Wdm WorkItem

    Caller must call StateMachineEngine_ReleaseResources to release any resources acquired.

--*/
{
    PDEVICE_OBJECT                  wdmDeviceObject;
    NTSTATUS                        status;
    PUCX_FORWARD_PROGRESS_WORKITEM  ucxWorkItem;

    TRY {

        RtlZeroMemory(SmEngineContext, sizeof(SM_ENGINE_CONTEXT));
        SmEngineContext->Owner = Owner;
        SmEngineContext->StateEntryFunctionContext = StateEntryFunctionContext;
        
        // The State table is generated by the Visio to C convertor. Since the tables are 
        // built statically in the code, not modified at run time, and not exposed to any component 
        // outside ucx, there is
        // little need to pass information about sizes of the table. However Prefast complains
        // about it. So suppressing that error
        #pragma prefast(suppress: 26007)
        SmEngineContext->CurrentStateEntry = StateTable[InitialStateIndex];
        SmEngineContext->StateTable = StateTable;
        KeInitializeSpinLock(&SmEngineContext->StateLock);

        wdmDeviceObject = WdfDeviceWdmGetDeviceObject(UcxControllerContext->ParentWdfDevice);

        ucxWorkItem = Controller_ForwardProgressWorkItemAllocate(UcxControllerContext,
                                                                 wdmDeviceObject,
                                                                 UCX_FORWARD_PROGRESS_WORKITEM_NO_FLAGS);

        if (ucxWorkItem == NULL) {
            status = STATUS_INSUFFICIENT_RESOURCES;
            LEAVE;
        }

        SmEngineContext->UcxForwardProgressWorkItem = ucxWorkItem;
        SmEngineContext->EtwWriteStateEvent = EtwWriteStateEvent;
        SmEngineContext->EtwWriteStateTransition = EtwWriteStateTransition;

        status = STATUS_SUCCESS;

        RecordEvent(SmEngineContext, SmEngineEventStart);
        RecordState(SmEngineContext, SmEngineContext->CurrentStateEntry->State, SmEngineEventStart);

        //
        // Initial state must not have a state entry function (because our code currently does
        // not support executing it)
        //
        NT_ASSERT(StateTable[InitialStateIndex]->StateEntryFunction == NULL);

    } FINALLY {
    }

    return status;
}

#include <d3d8.h>
#include <d3dx8.h>
#include "Volume.h"

const int NUM_LEVELS = 5;   // Mipmap levels to create
const int NUM_ANGLES = 8;

CVolFilterTest::CVolFilterTest()
{
	m_szTestName = "Volume Filter";
	m_szCommandKey = "Filter";

	// Indicate that we have MipMap tests
    bMipMap = true;

    // Initialize some variables
	pMagVolTexture = NULL;
	pMipVolTexture = NULL;

    // We only want one volume format
    if (m_Options.ModeOptions.VolumeFmt == FMT_ALL)
        m_Options.ModeOptions.VolumeFmt = FMT_ANY;
}

CVolFilterTest::~CVolFilterTest()
{
}

UINT CVolFilterTest::TestInitialize(void)
{
    // Setup the Test range
	SetTestRange(1, (int)(NUM_ANGLES * NUM_LEVELS * MAX_FILTER_TESTS));

 	// Make sure we are on at lease DX8
	if (m_dwVersion <= 0x0700)
	{
		WriteToLog("This test requires at least Dx8.\n");
		return D3DTESTINIT_SKIPALL;
	}

    // Check if Volumes are available	
	DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;

	if (!(dwTextureCaps & D3DPTEXTURECAPS_VOLUMEMAP))
	{
		if (m_uCommonVolumeFormats != 0) 
		{
  			WriteToLog("Device capability inconsistency: Found volume texture formats with Texture VolumeMap not set.");
			return D3DTESTINIT_ABORT;
		}

		WriteToLog("Device capability not supported: Texture VolumeMap.\n");
		return D3DTESTINIT_SKIPALL;
	}
 
    // Check if any volume formats were available
	if (m_uCommonVolumeFormats == 0) 
    {
		WriteToLog("No volume texture formats found.\n");
		return D3DTESTINIT_ABORT;
    }

    // Load sphere vertices
	LoadSphere();
	
	//Win8 [TestBug]276365
	// Set image compare variable - default m_fPassRequired is 0.85f
	// Set it to 0.80f to loosen passing criteria
	m_pFramework->GetImageCmp()->SetPassRequired(0.80f);
    
	// Setup the colors for the RedStripes array
	D3DCOLOR		RedStripes[4];

	RedStripes[0] = 0xffffffff; // White
	RedStripes[1] = 0xffff0000;	// Red
	RedStripes[2] = 0xffffffff; // White
	RedStripes[3] = 0xffff0000;	// Red

    // Setup the Image depth, width, and height for the min volume texture
    int i,j;
    UINT nImageDepth = 4;
    UINT nImageWidth = 32;
    UINT nImageHeight = 32;
	bool bResult = true;
    TCHAR szBuffer[80];
	szStatus[0] = '\0';

	// Log the current Volume format and load method.
    FmtToString(FORMAT_TEXTURE,&m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],szBuffer);
    WriteToLog("Tex: %s (UpdateTex).\n",szBuffer);
   	sprintf(szStatus, "%s$aUpdateTex",szStatus);

    // Create the stripe image data
    CImage ** ppImage = new CImage*[nImageDepth];
	if(ppImage == NULL)
	{
    	WriteToLog("TestInitialize() - Out of memory.\n");
		return D3DTESTINIT_ABORT;
	}

    for (i=0; i<nImageDepth; i++)
    {
        ppImage[i] = new CImage;
		if(ppImage[i] == NULL)
		{
			// Cleanup image data
			for (j=0; j<i; j++)
			{
			   SAFEDELETE(ppImage[j]); 
			}
			SAFEDELETEA(ppImage);

			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}

        ppImage[i]->LoadStripes(nImageWidth,nImageHeight,1,&RedStripes[i],false); 
    }

	// Create the volume texture
	bResult = CreateVolumeTexture(nImageWidth,nImageHeight,nImageDepth,1,0,m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],POOL_DEFAULT,&pVolTexture);

	// Tell log about CreateVolumeTexture failure
    if (!bResult || NULL == pVolTexture) 
	{
    	WriteToLog("CreateVolumeTexture(pVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Set the palette
	if (pVolTexture->IsPalettized())
        pVolTexture->SetPalette(NULL);

	// Load the images
	if (!pVolTexture->LoadImage(1, ppImage, TEXLOAD_UPDATETEXTURE))
	{
		WriteToLog("LoadImage(pVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

    // Cleanup image data
    for (i=0; i<nImageDepth; i++)
    {
       SAFEDELETE(ppImage[i]); 
    }
	SAFEDELETEA(ppImage);

	// Setup the colors for the GreenStripes array
	D3DCOLOR		GreenStripes[4];

	GreenStripes[0] = 0xffffffff;   // White
	GreenStripes[1] = 0xff00ff00;   // Green
	GreenStripes[2] = 0xffffffff;   // White
	GreenStripes[3] = 0xff00ff00;   // Green

    // Setup the Image depth, width, and height for the mag volume texture
    nImageDepth = 4;
    nImageWidth = 128;
    nImageHeight = 128;

    // Create the stripe image data
    ppImage = new CImage*[nImageDepth];
	if(ppImage == NULL)
	{
    	WriteToLog("TestInitialize() - Out of memory.\n");
		return D3DTESTINIT_ABORT;
	}

    for (i=0; i<nImageDepth; i++)
    {
        ppImage[i] = new CImage;
		if(ppImage[i] == NULL)
		{
			// Cleanup image data
			for (j=0; j<i; j++)
			{
			   SAFEDELETE(ppImage[j]); 
			}
			SAFEDELETEA(ppImage);

			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}

        ppImage[i]->LoadStripes(nImageWidth,nImageHeight,1,&GreenStripes[i],false); 
    }

	// Create the volume texture
	bResult = CreateVolumeTexture(nImageWidth,nImageHeight,nImageDepth,1,0,m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],POOL_DEFAULT,&pMagVolTexture);

	// Tell log about CreateVolumeTexture failure
    if (!bResult || NULL == pMagVolTexture) 
	{
    	WriteToLog("CreateVolumeTexture(pMagVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

	// Set the palette
	if (pMagVolTexture->IsPalettized())
        pMagVolTexture->SetPalette(NULL);

	// Load the images
	if (!pMagVolTexture->LoadImage(1, ppImage, TEXLOAD_UPDATETEXTURE))
	{
		WriteToLog("LoadImage(pMagVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return D3DTESTINIT_ABORT;
	}

    // Cleanup image data
    for (i=0; i<nImageDepth; i++)
    {
       SAFEDELETE(ppImage[i]); 
    }
	SAFEDELETEA(ppImage);

    // Check if MipVolumes are available	
	if (dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP)
	{
		// Setup the colors for the BlueStripes array
		D3DCOLOR		BlueStripes[8];

		BlueStripes[0] = 0xff00004b;    // Off Blue
		BlueStripes[1] = 0xff0000ff;	// Blue
		BlueStripes[2] = 0xff00004b;    // Off Blue
		BlueStripes[3] = 0xff0000ff;	// Blue
		BlueStripes[4] = 0xff00004b;    // Off Blue
		BlueStripes[5] = 0xff0000ff;	// Blue
		BlueStripes[6] = 0xff00004b;    // Off Blue
		BlueStripes[7] = 0xff0000ff;	// Blue

		// Setup the colors for the YellowStripes array
		D3DCOLOR		YellowStripes[4];

		YellowStripes[0] = 0xff4b4b00;  // Off Yellow
		YellowStripes[1] = 0xffffff00;  // Yellow
		YellowStripes[2] = 0xff4b4b00;  // Off Yellow
		YellowStripes[3] = 0xffffff00;  // Yellow

		// Setup the colors for the MagentaStripes array
		D3DCOLOR		MagentaStripes[2];

		MagentaStripes[0] = 0xff4b004b; // Off Magenta
		MagentaStripes[1] = 0xffff00ff;	// Magenta

		D3DCOLOR dwRed[1]   = {0xffff0000};	// Red
		D3DCOLOR dwGreen[1] = {0xff00ff00};  // Green

		// Setup the Image depth, width, and height for the mip volume texture
		UINT nMipLevels = NUM_LEVELS;

		nImageDepth = 8;
		nImageWidth = 256;
		nImageHeight = 256;

		// Create the stripe image data
		CImage *** pppImages = new CImage**[nMipLevels];
		if (NULL == pppImages)
		{
			WriteToLog("TestInitialize() - Out of memory.\n");
			return D3DTESTINIT_ABORT;
		}

		D3DCOLOR* dwStripeIndex[] = { (D3DCOLOR*)BlueStripes, (D3DCOLOR*)YellowStripes, 
									  (D3DCOLOR*)MagentaStripes, (D3DCOLOR*)dwRed, (D3DCOLOR*)dwGreen};

		// Allocate/load the CImages
		for (i=0; i<nMipLevels; i++)
		{
			UINT nDepth = nImageDepth / min((UINT)((DWORD)1 <<  i),nImageDepth);

			pppImages[i] = new CImage*[nDepth];
			if (NULL == pppImages[i])
			{
				WriteToLog("TestInitialize() - Out of memory.\n");
				return D3DTESTINIT_ABORT;
			}
    
			for (j=0; j<nDepth; j++)
			{
				UINT nWidth = nImageWidth / min((UINT)((DWORD)1 <<  i),nImageWidth);
				UINT nHeight = nImageHeight / min((UINT)((DWORD)1 <<  i),nImageHeight);
				pppImages[i][j] = new CImage();
				if (NULL == pppImages[i][j])
				{
					WriteToLog("TestInitialize() - Out of memory.\n");
					return D3DTESTINIT_ABORT;
				}
				pppImages[i][j]->LoadStripes(nWidth,nHeight,1,&(dwStripeIndex[i][j]),false); 
			}
		}

		// Create the mipmap volume texture
		bResult = CreateVolumeTexture(nImageWidth,nImageHeight,nImageDepth,nMipLevels,FMWK_DEFAULT,m_pCommonVolumeFormats[m_pCurrentMode->nVolumeFormat],POOL_DEFAULT,&pMipVolTexture);

		// Tell log about CreateVolumeTexture failure
		if (!bResult || NULL == pMipVolTexture) 
		{
    		WriteToLog("CreateVolumeTexture(pMipVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			return D3DTESTINIT_ABORT;
		}

		// Set the palette
		if (pMipVolTexture->IsPalettized())
			pMipVolTexture->SetPalette(NULL);

		// Load the images
		if (!pMipVolTexture->LoadImage(pppImages, TEXLOAD_UPDATETEXTURE))
		{
			WriteToLog("LoadImage(pMipVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			return D3DTESTINIT_ABORT;
		}

		// Cleanup mip image data
		for (i=0; i<nMipLevels; i++)
		{
			UINT nDepth = nImageDepth / min((UINT)((DWORD)1 <<  i),nImageDepth);
			for (j=0; j<nDepth; j++)
			{
				SAFEDELETE(pppImages[i][j]);
			}
			SAFEDELETEA(pppImages[i]);
		}
		SAFEDELETEA(pppImages);
	}

    return D3DTESTINIT_RUN;
}

bool CVolFilterTest::ExecuteTest(UINT uTestNum)
{
	int nTest = (uTestNum - 1) % (NUM_LEVELS * NUM_ANGLES);
	int nGroup = (uTestNum - 1) / (NUM_LEVELS * NUM_ANGLES);
	float fAngle = (360.0f / NUM_ANGLES) * nTest;
	while (fAngle>=360.0f) { fAngle-=360.0f; }
    D3DVECTOR ZVector = {0.0f,0.0f,3.0f};
    D3DXMATRIX Matrix, RotMatrix, TransMatrix;
    DWORD dwPasses;
	msgString[0] = '\0';

	// Get the Z distance of the current level
    if (nGroup <= 7)
    {
        float fZVals[5] = {3.0f, 5.0f, 8.0f, 10.0f, 13.0f};

	    ZVector.z = fZVals[nTest / NUM_ANGLES];
    }
    else
    {
        float fZVals[5] = {3.0f, 4.0f, 6.0f, 11.0f, 16.0f};

	    ZVector.z = fZVals[nTest / NUM_ANGLES];
    }

	// Reset some image compare variables to defaults
    m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.15f);
    m_pFramework->GetImageCmp()->SetTolPerPixel(0.10f);
    m_pFramework->GetImageCmp()->SetImgDeltaWght(1.00f);

    // Reset current texture being used
	SetTexture(0,NULL);

    // Check MipVolume cap for mipmap cases
    if (nGroup >= 8)
    {
    	DWORD dwTextureCaps = m_pSrcDevice->GetCaps()->dwTextureCaps;
        DWORD dwVolumeTextureFilterCaps = m_pSrcDevice->GetCaps()->dwVolumeTextureFilterCaps;

        // Check if MipVolumes are available	
	    if (!(dwTextureCaps & D3DPTEXTURECAPS_MIPVOLUMEMAP)) 
	    {
            // Do VolumeTextureFilter cap consistency checking
            if ((dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFPOINT) ||
                (dwVolumeTextureFilterCaps & D3DPTFILTERCAPS_MIPFLINEAR))
            {
	    	    WriteToLog("\nDevice capability inconsistency: Texture MipVolumeMap should be set.");
                TestFinish(D3DTESTFINISH_ABORT);
    	    	return false;
            }

		    WriteToLog("\nDevice capability not supported: Texture MipVolumeMap.\n");
            TestFinish(D3DTESTFINISH_SKIPREMAINDER);
	    	return false;
	    }

		// Setup custom image compare to avoid LOD approximations
		m_pFramework->GetImageCmp()->SetTolPerPixelChannel(0.20f);
		m_pFramework->GetImageCmp()->SetTolPerPixel(0.15f);
		m_pFramework->GetImageCmp()->SetImgDeltaWght(0.80f);
    }

    // Setup the filter mode
    UINT Result = SetupFilterMode(nGroup);
    
    if (Result == D3DTESTINIT_SKIPALL)
    {
        SkipTests(NUM_ANGLES * NUM_LEVELS);
	    return false;
    }

    if (Result == D3DTESTINIT_ABORT)
    {
        TestFinish(D3DTESTFINISH_ABORT);
	    return false;
    }

    // Indicate what depth we are rending at
   	sprintf(msgString,"%sZ: %.2f, %.2f degrees, ",msgString,ZVector.z,fAngle);

	// Set the volume texture according to group
    if (nGroup <= 7)
    {
        if (nGroup <= 3)
        {
            // Log texture size information
            sprintf(msgString,"%sTexSize: 32x32x4 ",msgString);
        }
        else    
        {
            // Log texture size information
            sprintf(msgString,"%sTexSize: 128x128x4 ",msgString);
        }
    }
    else
    {
        // Log texture size information
        sprintf(msgString,"%sTexSize: 256x256x8 ",msgString);
    }

    // Tell the log that we are starting
	BeginTestCase(msgString,uTestNum);

	if (nGroup <= 3)
	{
        // Use the min volume texture
		if (!SetTexture(0,pVolTexture))
		{
			WriteToLog("SetTexture(pVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
	}
	else if (nGroup <= 7)
	{
        // Use the mag volume texture
        if (!SetTexture(0,pMagVolTexture))
		{
			WriteToLog("SetTexture(pMagVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
	}
	else
	{
        // Use the mipmap volume texture
      	if (!SetTexture(0,pMipVolTexture))
		{
			WriteToLog("SetTexture(pMipVolTexture) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
			Fail();
			return false;
		}
	}

    // Check with ValidateDevice, just to be sure
	if (!ValidateDevice(&dwPasses))
	{
        WriteToLog("ValidateDevice() failed with Passes = %d and HResult = %s.\n",dwPasses,m_pD3D->HResultToString(GetLastError()));
        SkipTests(NUM_ANGLES * NUM_LEVELS);
		return false;
	}

    // Setup the world matrix for the translating/rotating sphere
    D3DXMatrixIdentity(&Matrix);
    D3DXMatrixTranslation(&TransMatrix, ZVector.x, ZVector.y, ZVector.z);
    D3DXMatrixMultiply(&Matrix, &TransMatrix, &Matrix);
    D3DXMatrixRotationX(&RotMatrix, fAngle*pi/180);
    D3DXMatrixMultiply(&Matrix, &RotMatrix, &Matrix);
    if (!SetTransform(D3DTS_WORLD, (D3DMATRIX *) &Matrix))
	{
        // Tell the logfile about the SetTransform failure
		WriteToLog("SetTransform(WorldMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        Fail();
        return false;
