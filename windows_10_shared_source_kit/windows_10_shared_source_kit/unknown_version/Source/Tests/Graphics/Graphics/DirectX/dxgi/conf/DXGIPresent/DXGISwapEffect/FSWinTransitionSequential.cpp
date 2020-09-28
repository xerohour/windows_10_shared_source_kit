//
// Test that DXGI properly handles it backbuffers
// after a  fullscreen to windowed transtion
//

#include "DXGISwapEffect.h"
#include "TestApp.h"
#include <DXGIValueSets.h>


#pragma warning(disable: 4355)


CFSWinTransitionSequential::CFSWinTransitionSequential()
	:
m_HoldReferenceToPrimary(false),
	m_ResizeBuffers(false)
{
	m_bStretchBB = false;
	m_MaxBufferCount = 3;
	m_StartFullScreen = true;
	m_bStretchBB = false;

	//
	// Only turn DWM on when "DWM" test parameter says so.
	// Going fullscreen in 888+ modes can cause DWM to turn on
	// on secondary monitors even if the test has specified 16bpp.
	// Not a big problem for the test but waiting for DWM to turn off again
	// slows down the test.
	//
	m_bDWM = false;

	for( UINT i = 0; i < cuiNoOfSwapChainFlag; i++ )
	{
		if(DXGISwapChainFlag[i].dxgiSwapChainFlag == NULL)
		{
			m_SwapChainFlags = DXGISwapChainFlag[i];
		}
	}

	//
	// Just test one 32 bit mode.
	//
	m_DesktopModes[0].uiWidth = 1024;
	m_DesktopModes[0].uiHeight = 768;
	m_DesktopModes[0].uiBPP = 32;


	m_numberOfModes = 1;
};


CFSWinTransitionSequential::~CFSWinTransitionSequential()
{

};

TEST_RESULT CFSWinTransitionSequential::Setup()
{
	//
	// Initialize locals.
	//

	TEST_RESULT tResult = RESULT_FAIL;
	HRESULT hResult = S_OK;


	if
		(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION ) != 0
		)
	{
		WriteToLog
			(
			_T( "Setup:  CFSWinTransitionSequential not run TerminalServices." )
			);
		tResult = RESULT_SKIP;
	}
	else
	{
		tResult = CPresentWinConvertStretch::Setup();
	}

	return tResult;
};

TEST_RESULT CFSWinTransitionSequential::ExecuteTestCase()
{

	//
	// After the transition need to go around the buffers again to
	// validate the one that was displaying when transitioned is correct
	//
	m_NumberOfPresents = m_BufferCount * 2;

	//
	// Present one more time to make sure it is the first buffer
	//
	if(m_ValidateNoExtraBuffers)
	{
		m_NumberOfPresents += 1;
	}

	m_TexturesToLoad = m_BufferCount;

	return CPresentWinConvertStretch::ExecuteTestCase();
};


//****************************************************************************
// CFSWinTransitionSequential::InitTestParameters
//
// Replaces the CPresentWinConvertStretch base class InitTestParameters with
// this version.
//****************************************************************************
void CFSWinTransitionSequential::InitTestParametersImpl()
{
	CUserValueSet< bool > *pHoldReferenceToPrimary = new CUserValueSet< bool >();
	pHoldReferenceToPrimary->AddValue( true );
	pHoldReferenceToPrimary->AddValue( false );

	RFactor holdReferenceToPrimary = AddParameter< bool >
		(
		_T("HoldReferenceToPrimary"),
		&m_HoldReferenceToPrimary,
		pHoldReferenceToPrimary
		);

	//
	// ResizeBuffers == false should send down the non true flip code path
	//
	CUserValueSet< bool > *pResizeBuffers = new CUserValueSet< bool >();
	pResizeBuffers->AddValue( true );
	pResizeBuffers->AddValue( false );

	RFactor resizeBuffers = AddParameter< bool >
		(
		_T("ResizeBuffers"),
		&m_ResizeBuffers,
		pResizeBuffers
		);

	//
	// We'll need to create different test factors for Blt vs. Flip Model.
	//
	//
	// Starting with Win8 M2, FlipMode requires that we call ResizeBuffers
	// after all fullscreen<->Windowed Transitions.  No more seemless transitions.
	//
	CUserValueSet< bool > *pResizeBuffersFlipModel = new CUserValueSet< bool >();
	pResizeBuffersFlipModel->AddValue( true );

	RFactor rfResizeBuffersFlipModel = AddParameterValueSet< bool >
		(
		_T("ResizeBuffers"),
		pResizeBuffersFlipModel
		);

	CUserValueSet< bool > *pValidateNoExtraBuffers = new CUserValueSet< bool >();
	pValidateNoExtraBuffers->AddValue( true );
	pValidateNoExtraBuffers->AddValue( false );

	RFactor validateNoExtraBuffers = AddParameter< bool >
		(
		_T("ValidateNoExtraBuffers"),
		&m_ValidateNoExtraBuffers,
		pValidateNoExtraBuffers
		);


	//
	// Blt-Model Back Buffer Counts
	//
	RFactor rfBltModelBufferCount = AddParameter
		(
		_T("BufferCount"),
		&m_BufferCount,
		RangeValueSet
		(
		(UINT)1,
		(UINT)m_MaxBufferCount,
		(UINT)1
		)
		);

	//
	// Flip-Model Back Buffer Counts
	// Flip-Model does not support the 1 buffer swapchain case.
	//
	RFactor rfFlipModelBufferCount = AddParameterValueSet
		(
		_T("BufferCount"),
		RangeValueSet
		(
		(UINT)2,
		(UINT)m_MaxBufferCount,
		(UINT)1
		)
		);

	// Blt-Model Swap Effects
	CUserValueSet< DXGISwapEffectRecord > *pBltModelSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
	for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
	{
		if(DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL)
		{
			pBltModelSwapEffect->AddValue(DXGISwapEffects[i]);
		}
	}

	RFactor rfBltModelSwapEffect =  AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pBltModelSwapEffect);


	// Flip-Model Swap Effects
	CUserValueSet< DXGISwapEffectRecord > *pFlipModelSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
	for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
	{
		if(DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL)
		{
			pFlipModelSwapEffect->AddValue(DXGISwapEffects[i]);
		}
	}

	RFactor rfFlipModelSwapEffect =  AddParameterValueSet< DXGISwapEffectRecord >( _T("SwapEffect") , pFlipModelSwapEffect);


	RFactor rfFlipModelCombo = rfFlipModelSwapEffect * rfFlipModelBufferCount * rfResizeBuffersFlipModel;
	RFactor rfBltModelCombo = rfBltModelSwapEffect * rfBltModelBufferCount * resizeBuffers;

	CUserValueSet< DXGIPresentFlagRecord > *pPresentFlags = new CUserValueSet< DXGIPresentFlagRecord >();
	pPresentFlags->AddValue(DXGIPresentFlags[0]);
	RFactor presentFlags =  AddParameter< DXGIPresentFlagRecord >( _T("PresentFlags") , &m_PresentFlag, pPresentFlags);


	RFactor rfCombo = (rfFlipModelCombo +  rfBltModelCombo) * presentFlags * validateNoExtraBuffers * holdReferenceToPrimary;

	//
	// Test Windowed Presents only (No DCOMP).   DCOMP does not support fullscreen.
	//
	CUserValueSet< bool > *pDCOMPUsage= new CUserValueSet< bool >();
	pDCOMPUsage->AddValue(false);
	RFactor rfDontUseDCOMP = AddParameter< bool >( _T("UseDCOMP") , &m_tp_bWindowless, pDCOMPUsage);

	CUserValueSet< bool > *pDwmCompositionOn = new CUserValueSet< bool >();
	if ( g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION) != 0)
	{
		//
		// Dont add the toggle for DWM on and off in the remote case.
		//
	}
	else
	{
		pDwmCompositionOn->AddValue( false );
	}

	// Invlid dwm factor when in the remote case
	RFactor dwmCompositionOff_32bpp = AddParameter< bool >( _T("DWM"), &m_bDWM, pDwmCompositionOn);
	// Always valid dwm factors
	RFactor v_dwmCompositionOn = AddParameterValue< bool >( _T("DWM"), true);
	RFactor v_dwmCompositionOff = AddParameterValue< bool >( _T("DWM"), false);

	// Separate out Bpp32_DwmOn and DwmOff when adding test parameters together so that
	// the order of test cases are preserved (
	RFactor Bpp16_DwmOff = m_FeatureLevelFactor * m_globalRFactorDwmOffBpp * v_dwmCompositionOff * rfDontUseDCOMP * rfCombo;
	RFactor Bpp32_DwmOn = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * v_dwmCompositionOn * rfDontUseDCOMP * rfCombo ;
	RFactor Bpp32_DwmOff = m_FeatureLevelFactor  * m_globalRFactorDwmOnBpp * dwmCompositionOff_32bpp * rfDontUseDCOMP * rfCombo;

	// Only test dwm off cases on Win7+8IP and below
	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);
	if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2) )
	{
		// Check which set of enumerated mode set parameters are valid
		if( Bpp16_DwmOff.Valid() && Bpp32_DwmOn.Valid())
			SetRootTestFactor( Bpp32_DwmOn + Bpp32_DwmOff + Bpp16_DwmOff );
		else if( !Bpp16_DwmOff.Valid() && Bpp32_DwmOn.Valid())
			SetRootTestFactor(Bpp32_DwmOn + Bpp32_DwmOff);
		else if( Bpp16_DwmOff.Valid() && !Bpp32_DwmOn.Valid())
			SetRootTestFactor( Bpp16_DwmOff );
		else
		{
			SetRootTestFactor( m_FeatureLevelFactor );
			m_ParameterError = -1;
		}
		AddPriorityPatternFilter(FilterEqual< bool >(_T("DWM"), false ), WeightFactorVerySmall);
	}
	else
	{
		if( Bpp32_DwmOn.Valid())
			SetRootTestFactor( Bpp32_DwmOn );
	}
	DeclarePriorityLevelByPriorityValue(1, 11.0f);
	DeclarePriorityLevelByPriorityValue(2, 5.0f);

}



HRESULT CFSWinTransitionSequential::TransitionBeforePresent()
{
	HRESULT hr = S_OK;
    IDXGIDevice *pDXGIDevice = NULL;
    IDXGIAdapter *pAdapter = NULL;
	IDXGIOutput* pDXGIOutput = NULL;
    DXGI_MODE_DESC  matchDesc = { 0 };
    DXGI_MODE_DESC  outDesc = { 0 };


    if(!m_adapterNoOutputs) // does our Adapter have outputs?  If not, we can't switch to fullscreen mode.
	{
		hr = m_pSwapChain->SetFullscreenState( TRUE, NULL );
		if(FAILED(hr))
		{
			WriteToLog(_T("CFSWinTransitionSequential::TransitionBeforePresent SetFullscreenState failed hr=0x%x"),hr);
            goto Cleanup;
		}

		if( m_ResizeBuffers)
		{
			//
			// Use the current desktop mode to get a matching DXGI_MODE_DESC to give to the ResizeTarget
			//
            // We need to get an adapter from our D3D device to ensure it has FeatureLevel9 support.   The raw adapter
            // we created directly from the DXGIFactory may not claim to support R8G8B8A8 modes.   It seems to depend
            // on the HW.  Some DX9 devices do enumerate it, some don't.   Adapters created from a FL9 D3DDevice emulate
            // R8G8B8A8 support if necessary.   See Windows 8 bug 45815 for more information.
            //
            hr = m_DevicePointer.pDevice->QueryInterface(IID_IDXGIDevice, (void **)&pDXGIDevice);
            if (FAILED(hr))
            {
                WriteToLog("CFSWinTransitionSequential::TransitionBeforePresent: QueryInterface for IID_IDXGIDevice failed with %s (0x%x)", D3DHResultToString(hr).c_str(), hr);
                goto Cleanup;
            }

            hr = pDXGIDevice->GetAdapter(&pAdapter);
            if (FAILED(hr))
            {
                WriteToLog("CFSWinTransitionSequential::TransitionBeforePresent: GetApdapter failed with %s (0x%x)", D3DHResultToString(hr).c_str(), hr);
                goto Cleanup;
            }

			hr = pAdapter->EnumOutputs(m_ModeInformation.uiDXGIOutputOrdinal, &pDXGIOutput);
			if(FAILED(hr))
			{
				WriteToLog(
					"CFSWinTransitionSequential::TransitionBeforePresent:  EnumOutputs failed for uiDXGIOutputOrdinal %d. hr = 0x%x",
					m_ModeInformation.uiDXGIOutputOrdinal,
					hr);
                goto Cleanup;
			}

			matchDesc.Width = m_ModeInformation.ModeDesc.Width;
			matchDesc.Height = m_ModeInformation.ModeDesc.Height;
			matchDesc.RefreshRate.Numerator = m_ModeInformation.devmode.dmDisplayFrequency;
			matchDesc.RefreshRate.Denominator = 1;
			matchDesc.Format = m_ModeInformation.ModeDesc.Format;

			//
			// Find closest match does not work with _SRGB
			//
			if(m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB)
			{
				matchDesc.Format = DXGI_FORMAT_B8G8R8A8_UNORM;
			}
			else if(m_ModeInformation.ModeDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB)
			{
				matchDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			}

			hr = pDXGIOutput->FindClosestMatchingMode(
				&matchDesc,
				&outDesc,
				NULL);


			if(FAILED(hr))
			{
				WriteToLog(
					"CFSWinTransitionSequential::TransitionBeforePresent: FindClosestMatchingMode failed for width %d x height %d refresh (%d/%d). format %s. hr = 0x%x",
					matchDesc.Width,
					matchDesc.Height,
					matchDesc.RefreshRate.Numerator,
					matchDesc.RefreshRate.Denominator,
					CFormatInfoMap::Find( matchDesc.Format )->szDesc,
					hr);
                goto Cleanup;
			}

			//
			// Maybe an issue with setting outDesc.Format back to _SRGB if FindClosestMatchingMode has changed the format
			//
			assert( matchDesc.Format == outDesc.Format );
			outDesc.Format = m_ModeInformation.ModeDesc.Format;

			//
			// Make dxgi set the display mode
			//
			hr = m_pSwapChain->ResizeTarget( &outDesc );
			if(FAILED(hr))
			{
				WriteToLog(_T("CFSWinTransitionSequential::TransitionBeforePresent ResizeTarget failed hr=0x%x"),hr);
                goto Cleanup;
			}

			//
			// Resize the swapchain to match the display mode
			//
			hr = m_pSwapChain->ResizeBuffers
				(
				m_SCDesc.BufferCount,
				outDesc.Width,
				outDesc.Height,
				m_SCDesc.BufferDesc.Format,
				m_SCDesc.Flags | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
				);
			if(FAILED(hr))
			{
				WriteToLog(_T("CFSWinTransitionSequential::TransitionBeforePresent ResizeBuffers failed hr=0x%x"),hr);
                goto Cleanup;
			}
		}
	}
	else
	{
		WriteToLog(_T("Fullscreen on adapter with no outputs not implemented in CPresentWinConvertStretch"));
		hr = E_NOTIMPL;
	}

Cleanup:
	SAFE_RELEASE(pDXGIOutput);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pDXGIDevice);

	return hr;
}

HRESULT CFSWinTransitionSequential::TransitionAfterPresent()
{
    HRESULT hr = S_OK;

    ID3DTexture2D * pBackBuffer = NULL;

    //
    // Test that buffers are not disturbed when holding a reference to the primary
    // and transitioning from fullscreen.
    //
    if(m_HoldReferenceToPrimary)
    {
        //
        // Get a reference to primary.
        //

        hr = m_pSwapChain->GetBuffer
            (
            (m_BufferCount - 1),
            __uuidof(pBackBuffer),
            reinterpret_cast <void**>( &pBackBuffer)
            );

        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                "CPresentWinConvertStretch::CreateTextureAndCopyBackBuffer:  GetBuffer 0 failed with error 0x%x.",
                hr
                );
        };

    }

    //
    // Test that buffers are not disturbed after transitioning from fullscreen.
    //
    if((SUCCEEDED(hr)))
    {
        BOOL bDWMState = FALSE;
        DwmIsCompositionEnabled( &bDWMState );

        hr = m_pSwapChain->SetFullscreenState( FALSE, NULL );
        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                _T("CFSWinTransitionSequential::TransitionAfterPresent:  SetFullscreenState failed with error 0x%x."),
                hr
                );
        };

        if(SUCCEEDED(hr))
        {
            Sleep(100);
            ProcessMessages();
            Sleep(bDWMState ? 2500 : 1500);
            ProcessMessages();
        }
    }

    if(pBackBuffer)
    {
        pBackBuffer->Release();
    }

    // For Flip-Model we must call ResizeBuffers after any fullscreen/window
    // transition.  Test parameters are currently set so that we only test
    // Flip-Model when m_ResizeBuffers is set to true, however, in case we want
    // to explore not calling ResizeBuffers in the future, let's only call it
    // when asked to.
    // Calling Resize in some ways completely invalidates the usefulness of
    // the FSWinTransitionSequential test group inside DXGISwap.  The primary
    // purpose of those tests is to explore buffer preservation across
    // transitions.  (Which is simply not supported for Flip-Model).  The tests
    // still have some small value for Flip-Model by verifying that we can
    // recover from transitions.
    //
    if (m_ResizeBuffers || (m_SwapEffect.dxgiSwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL))
    {
        //
        // Get the current desc and reapply it.
        //
        DXGI_SWAP_CHAIN_DESC scd;
        m_pSwapChain->GetDesc(&scd);

        //
        // Call ResizeBuffers with the current values.
        //
        hr = m_pSwapChain->ResizeBuffers
            (
            scd.BufferCount,
            scd.BufferDesc.Width,
            scd.BufferDesc.Height,
            scd.BufferDesc.Format,
            scd.Flags | DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH
            );
        if ( FAILED( hr ) )
        {
            WriteToLog
                (
                _T("CFSWinTransitionSequential::TransitionAfterPresent:  ResizeBuffers failed with error 0x%x."),
                hr
                );
			return hr;
        };

        // Reinitialize buffer content after ResizeBuffers
        // TransitionAfterPresent is only on the first Present loop. ie: always start on the texture after texture 0.
        for
            (
            UINT uiCount = 1;
        uiCount <= m_BufferCount;
        ++uiCount
            )
        {
            HRESULT hrInitializeBackBufferResult = InitializeBackBuffer(
                &TestedSwapChain,
                ( uiCount ) % m_TexturesToLoad );

			if ( FAILED( hrInitializeBackBufferResult ) )
			{
				WriteToLog
					(
					"InitializeBackBuffer failed."
					);
				if( hrInitializeBackBufferResult == E_OUTOFMEMORY && m_bLargeMemoryUsage)
						WriteToLog( "ExecuteTestCase: Test case using large amount of memory. InitializeBackBuffer failure expected. " );
				return hrInitializeBackBufferResult;
			};


            if ( !PerformPresent( m_pSwapChain, 0, 0 ) )
            {
                WriteToLog
                    (
                    "CFSWinTransitionSequential::TransitionAfterPresent: PerformPresent on swap chain failed."
                    );

                return E_FAIL;
            }

        }
    }

    return hr;
}
