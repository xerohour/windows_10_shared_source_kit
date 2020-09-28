/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   DXGIHybridPresentTDR.cpp
//
//	Abstract:
//	This file contains implementation of DXGIHybridPresent tests
//
//	History:
//	1/14/2013	MarMel	Created.
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "DXGIHybridPresentTDR.h"
#include "TestApp.h"

#pragma warning(disable: 4355)

extern bool g_bSkipIfNotHybrid;

BEGIN_NAMED_VALUES(HELPER_THREAD_SCENARIO)
	NAMED_VALUE(_T("HELPER_THREAD_SCENARIO_IGPU"), HELPER_THREAD_SCENARIO_IGPU)
	NAMED_VALUE(_T("HELPER_THREAD_SCENARIO_DGPU"), HELPER_THREAD_SCENARIO_DGPU)
	NAMED_VALUE(_T("HELPER_THREAD_SCENARIO_IGPU_DGPU"), HELPER_THREAD_SCENARIO_IGPU_DGPU)
END_NAMED_VALUES(HELPER_THREAD_SCENARIO)

CHybridPresentTDR::CHybridPresentTDR()
{
	m_dwPresentTime = g_TimeToExecute;
}

void CHybridPresentTDR::InitTestParameters()
{
	//
	// Call the base class implementation.
	//

	CDXGITest::InitTestParameters();

	//
	// Enumerate\Initailize modes
	//

	// If testing on BasicRender device, then enable adapters with no outputs
	if ( _wcsicmp( g_Application.m_D3DOptions.SoftwareRasterizerDLLName, L"BasicRender.sys" ) == 0 )
		m_adapterNoOutputs = true;

	//
	// We'll need to create different test factors for Blt vs. Flip Model.   Flip-Model does not support the 1 buffer swapchain case.
	//

	// Blt-Model Back Buffer Counts
	RFactor rfBltModelBufferCount = AddParameter
	(
		_T("BufferCount"),
		&m_BufferCount,
		RangeValueSet
		(
			(UINT)1,
			(UINT)1,
			(UINT)1
		)
	);

	// Flip-Model Back Buffer Counts
	RFactor rfFlipModelBufferCount = AddParameterValueSet
	(
		_T("BufferCount"),
		RangeValueSet
		(
			(UINT)2,
			(UINT)2,
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

	RFactor rfBltModelCombo =  (rfBltModelSwapEffect * rfBltModelBufferCount);
	RFactor rfFlipModelCombo =  (rfFlipModelSwapEffect * rfFlipModelBufferCount);

	//
	// We need separate Test factors for choosing between DCOMP (Windowless) and regular Windowed Presents.  
	// DCOMP is only supported for FlipModel when DWM is on.
	//
	// (Note that the name "windowless" is somewhat misleading since the visuals themselves are still contained within a Window.)   
	//
	// Use DCOMP
	RFactor UseDCOMPFactor = AddParameter< bool >( _T("UseDCOMP") , &m_tp_bWindowless, true);

	// Don't use DCOMP (use regular Windowed Presents)
	RFactor DontUseDCOMPFactor = AddParameterValue< bool >( _T("UseDCOMP"), false);

	// Full screen vs. Windowed.
	// DCOMP can't be in Full screen mode
	RFactor FullScreen = AddParameter<bool>(_T("FullScreen"), &m_StartFullScreen, true);
	RFactor Windowed = AddParameterValue<bool>(_T("FullScreen"), false);

	// RFactor for single bb mode
	RFactor SingleModeRF = CreateSingleModeFactor();

	CTestCaseParameter<HELPER_THREAD_SCENARIO> *pHelperThreadScenarioType = AddParameter<HELPER_THREAD_SCENARIO>(_T("HelperThreadScenario"), &m_HelperThreadScenario);
	CUserValueSet<HELPER_THREAD_SCENARIO> * pHelperThreadScenarioValues = new CUserValueSet<HELPER_THREAD_SCENARIO>();
	pHelperThreadScenarioValues->AddValue(HELPER_THREAD_SCENARIO_IGPU);
	pHelperThreadScenarioValues->AddValue(HELPER_THREAD_SCENARIO_DGPU);
	pHelperThreadScenarioValues->AddValue(HELPER_THREAD_SCENARIO_IGPU_DGPU);

	testfactor::RFactor HelperThreadScnearioFactor = AddParameterValueSet<HELPER_THREAD_SCENARIO>(pHelperThreadScenarioType, pHelperThreadScenarioValues);


	// DWM ON - DCOMP Supported, but only for FlipModel.  for Regular Windowed Presents test both blt and Flip model.
	RFactor Bpp32_DwmOn = m_FeatureLevelFactor  * SingleModeRF * ((Windowed + FullScreen) * DontUseDCOMPFactor * (rfBltModelCombo + rfFlipModelCombo) + UseDCOMPFactor*rfFlipModelCombo*Windowed) * HelperThreadScnearioFactor;
    
	SetRootTestFactor( Bpp32_DwmOn );
    
	DeclarePriorityLevelByPriorityValue(1, 5.0f);
	DeclarePriorityLevelByPriorityValue(2, 1.0f);
}


RFactor CHybridPresentTDR::CreateSingleModeFactor()
{
	RFactor hmonitor, devmode, rotation, format, width, height, refreshrate[2], scanlineordering, scaling, adapter, output, multisamplecount, multisamplequality, isInitialized;
	RFactor SingleModeRF;

	InitModeParameters(); // we want to enumerate modes, but won't use all of them for this test. This test will not use m_globalRFactorDwmOnBpp

	UINT Index = 0;

	// Find output mode whith format that won't result in VistaBLT presents
	for(Index; Index < MAX_DISPLAYMODES; Index++)
	{
		if(m_OutputModeSet[Index].ModeDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM ||
			m_OutputModeSet[Index].ModeDesc.Format == DXGI_FORMAT_R8G8B8A8_UNORM_SRGB ||
			m_OutputModeSet[Index].ModeDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM ||
			m_OutputModeSet[Index].ModeDesc.Format == DXGI_FORMAT_B8G8R8A8_UNORM_SRGB ||
			m_OutputModeSet[Index].ModeDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM ||
			m_OutputModeSet[Index].ModeDesc.Format == DXGI_FORMAT_B8G8R8X8_UNORM_SRGB) 
		{
			// Found format which won't result in VistaBlt
			break;
		}
	}

	hmonitor = AddParameter(
		_T("hmonitor"), 
		(UINT*)&m_ModeInformation.hmonitor,
		new CTableValueSet< UINT >(
			(UINT*)&m_OutputModeSet[Index].hmonitor,
			sizeof( m_OutputModeSet[Index] ),
			1 ) );

	devmode = AddParameter(
		_T("devmode"),
		&m_ModeInformation.devmode,
		new CTableValueSet< DEVMODEW >(
			&m_OutputModeSet[Index].devmode,
			sizeof( m_OutputModeSet[Index] ),
			1 ) );

	rotation = AddParameter(
		_T( "Rotation" ),
		&m_ModeInformation.devmode.dmDisplayOrientation,
		m_OutputModeSet[Index].devmode.dmDisplayOrientation);

	format = AddParameter(
		_T( "ModeDesc.Format" ),
		&m_ModeInformation.ModeDesc.Format,
		new CTableValueSet< DXGI_FORMAT >(
			&m_OutputModeSet[Index].ModeDesc.Format,
			sizeof( m_OutputModeSet[Index] ),
			1 ) );

	width = AddParameter(
		_T( "ModeDesc.Width" ),
		&m_ModeInformation.ModeDesc.Width,
		m_OutputModeSet[Index].ModeDesc.Width);

	height = AddParameter(
		_T( "ModeDesc.Height" ),
		&m_ModeInformation.ModeDesc.Height,
		m_OutputModeSet[Index].ModeDesc.Height);

	refreshrate[0] = AddParameter(
		_T( "ModeDesc.RefreshRate.Numerator" ),
		&m_ModeInformation.ModeDesc.RefreshRate.Numerator,
		m_OutputModeSet[Index].ModeDesc.RefreshRate.Numerator);

	refreshrate[1] = AddParameter(
		_T( "ModeDesc.RefreshRate.Denominator" ),
		&m_ModeInformation.ModeDesc.RefreshRate.Denominator,
		m_OutputModeSet[Index].ModeDesc.RefreshRate.Denominator);

	scanlineordering = AddParameter(
		_T( "ModeDesc.ScanlineOrdering" ),
		&m_ModeInformation.ModeDesc.ScanlineOrdering,
		m_OutputModeSet[Index].ModeDesc.ScanlineOrdering);

	scaling = AddParameter(
		_T( "ModeDesc.Scaling" ),
		&m_ModeInformation.ModeDesc.Scaling,
		m_OutputModeSet[Index].ModeDesc.Scaling);

	adapter = AddParameter(
		_T( "DXGIAdapterOrdinal" ),
		&m_ModeInformation.uiDXGIAdapterOrdinal,
		m_OutputModeSet[Index].uiDXGIAdapterOrdinal);

	output = AddParameter(
		_T( "DXGIOutputOrdinal" ),
		&m_ModeInformation.uiDXGIOutputOrdinal,
		m_OutputModeSet[Index].uiDXGIOutputOrdinal);

	multisamplecount = AddParameter(
		_T( "MultisampleCount" ),
		&m_ModeInformation.uiMultisampleCount,
		m_OutputModeSet[Index].uiMultisampleCount);

	multisamplequality = AddParameter(
		_T( "MultisampleQuality" ),
		&m_ModeInformation.uiMultisampleQuality,
		m_OutputModeSet[Index].uiMultisampleQuality);

	isInitialized = AddParameter(
		_T( "isInitialized" ),
		&m_ModeInformation.isInitialized,
		m_OutputModeSet[Index].isInitialized);

	SingleModeRF = hmonitor % devmode % rotation % format % width % height % refreshrate[0] % refreshrate[1] % scanlineordering % scaling % adapter % output % multisamplecount % multisamplequality % isInitialized;

	return SingleModeRF;
}


TEST_RESULT CHybridPresentTDR::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	HANDLE hTDRThread = NULL;
	GPUStopThreadParams params;
	bool bHybridSystem = false;
	bool bFoundBDAOrBDDAdapter = false;

	//
	// Check if we are running on a hybrid system.
	//

	if( !CHybridPresentRedirection::CheckSystemAdapters( bHybridSystem, bFoundBDAOrBDDAdapter ) )
	{
		WriteToLog(_T( "ExecuteTestCase:  CheckSystemAdapters() failed." ));
		tr = RESULT_ABORT;
	}

	if( !bHybridSystem )
	{
		if(g_bSkipIfNotHybrid)
		{
			// SKIP when running with "-skipifnothybrid" parameter - we expect this test to be run on non-hybrid configurations
			WriteToLog(_T( "Setup:  CHybridPresentTDR is being run on a non-hybrid system. Skipping group." ));
			tr = RESULT_SKIP;
		}
		else
		{
			// FAIL when running without "-skipifnothybrid" parameter is not specified - we don't expect this test to be run on non-hybrid
			// configurations (i.e. in WTT environment (Hybrid support is determined by job constraint) or LOGO environment (Hybrid support is
			// determined by DisplayGatherer)
			WriteToLog(_T( "Setup:  CHybridPresentTDR is being run on a non-hybrid system. Failing group." ));
			tr = RESULT_FAIL;
		}   
	}

	if( bFoundBDAOrBDDAdapter )
	{
		g_bBDDHybrid = true;	// WinBlue: 195417
	}
    
	// Launch TDR thread
	{
		CSLocker Lock(params.csCritSection);
		params.bIGPU = false;
		params.bDGPU = false;
		params.pbExpectDeviceLost = &m_bExpectDeviceLoss;
		params.pbStopNow = &m_bStopNow;
		params.pbIsWindowed = &m_bIsWindowed;
		params.action = (int)m_HelperThreadScenario;
		params.hr = S_OK;
	}
    
	HANDLE hStartThreadEvent = CreateEvent(NULL, FALSE, FALSE, START_THREAD_EVENT);
	HANDLE hStopThreadEvent = CreateEvent(NULL, FALSE, FALSE, STOP_THREAD_EVENT);

	if(!hStartThreadEvent || !hStopThreadEvent)
	{
		tr = RESULT_ABORT;
		WriteToLog(_T("CHybridPresentTDR::ExecuteTestCase() - ABORT: Failed to create sync events, error = %d"), GetLastError());
	}

	if(tr == RESULT_PASS)
	{
		hTDRThread = CreateThread(0, 0, TDRThread, &params, 0, 0);

		if(!hTDRThread)
		{
			tr = RESULT_ABORT;
			WriteToLog(_T("CHybridPresentTDR::ExecuteTestCase() - ABORT: Failed to create TDR Thread, error = %d"), GetLastError());
		}
	}

	if(tr == RESULT_PASS)
	{
		// Wait for thread to start
		if(WAIT_OBJECT_0 != WaitForSingleObject(hStartThreadEvent, TDR_THREAD_WAIT_TIME))
		{
			tr = RESULT_ABORT;
			WriteToLog(_T("CHybridPresentTDR::ExecuteTestCase() - ABORT: TDR Thread failed to start"));
		}
	}

	if(tr == RESULT_PASS)
	{
		try
		{
			tr = CHybridPresentRedirection::ExecuteTestCase();
		}
		catch(...)
		{
			{
				CSLocker Lock(params.csCritSection);

				WriteToLog(
					_T("CHybridPresentTDR::ExecuteTestCase() - ERROR: Unhandled exception in CHybridPresentRedirection::ExecuteTestCase(), bResettingIGPU = %s, bResettingDGPU = %s, hr = 0x%X; stopping TDR thread"),
					(params.bIGPU ? "true" : "false"),
					(params.bDGPU ? "true" : "false"),
					params.hr);
			}

			SetEvent(hStopThreadEvent);

			CloseHandle(hStopThreadEvent);
			hStopThreadEvent = NULL;

			WaitForSingleObject(hTDRThread, INFINITE);

			CloseHandle(hTDRThread);
			hTDRThread = NULL;

			// pass exception further
			throw;
		}
	}

	// Check how's TDR thread is doing
	{
		CSLocker Lock(params.csCritSection);	

		if(FAILED(params.hr))
		{
			WriteToLog(
				_T("CHybridPresentTDR::ExecuteTestCase() - ERROR: Failure is detected on TDR thread, bResettingIGPU = %s, bResettingDGPU = %s, hr = 0x%X"),
				(params.bIGPU ? "true" : "false"),
				(params.bDGPU ? "true" : "false"),
				params.hr);

			tr = RESULT_FAIL;
		}
	}

	if(hStopThreadEvent)
	{
		SetEvent(hStopThreadEvent);

		CloseHandle(hStopThreadEvent);
		hStopThreadEvent = NULL;
	}

	if(hStartThreadEvent)
	{
		SetEvent(hStartThreadEvent);

		CloseHandle(hStartThreadEvent);
		hStartThreadEvent = NULL;
	}

	if(hTDRThread)		
	{
		WaitForSingleObject(hTDRThread, INFINITE);

		CloseHandle(hTDRThread);
		hTDRThread = NULL;
	}

	return tr;
}


DWORD WINAPI TDRThread(LPVOID _TDRThreadParams)
{
	DBGPrint(L"TDRThread - Enter");

	GPUStopThreadParams* pParams = (GPUStopThreadParams*) _TDRThreadParams;
	AdapterUtil* pAdapterUtil = NULL;
	HANDLE hStartThreadEvent = NULL;
	HANDLE hStopThreadEvent = NULL;

	try
	{
		// Enumerate all devices
		pAdapterUtil = AdapterUtil::CreateAdapterUtil();
		vector<AdapterDescriptor*> vAdapters = pAdapterUtil->GetAllAdapters();
		UINT uiAdapterCount                  = 0;
		AdapterDescriptor* pIntegrated = NULL;
		AdapterDescriptor* pDiscrete = NULL;
    
		for (AdapterDescriptor* pAdapter : vAdapters)
		{
			bool bSoftwareDevice = false;

			// skip BRD
			pAdapter->IsSoftwareDevice(bSoftwareDevice);
			if (bSoftwareDevice)
			{
				continue;
			}

			bool bHybridDiscrete = false;

			if(SUCCEEDED(pAdapter->IsHybridDiscrete(bHybridDiscrete)))
			{
				if(bHybridDiscrete)
					pDiscrete = pAdapter;
				else
					pIntegrated = pAdapter;
			}
			else
			{
				DBGPrint(L"TDRThread -Unable to determine adapter type");
				if(pAdapterUtil)
				{
					delete pAdapterUtil;
					pAdapterUtil = NULL;
				}

				return -1;
			}

			uiAdapterCount++;
		}

		DBGPrint(L"TDRThread - Enumerated %d devices", uiAdapterCount);

		if(uiAdapterCount != 2 || !pDiscrete || !pIntegrated)
		{
			CSLocker Lock(pParams->csCritSection);
			pParams->hr = E_INVALIDARG;
			*(pParams->pbStopNow) = true;

			DBGPrint(L"TDRThread - Enumerated devices don't meet expected configuration: AdapterCount = %d (expected %d), Discrete device count = %d (expected %d), Integrated device count = %d (expected %d)",
					uiAdapterCount,
					2,
					(pDiscrete) ? 1 : 0,
					1,
					(pIntegrated) ? 1 : 0,
					1);
			if(pAdapterUtil)
			{
				delete pAdapterUtil;
				pAdapterUtil = NULL;
			}

			return -1;
		}

		// create sync Events
		hStartThreadEvent = CreateEvent(NULL, FALSE, FALSE, START_THREAD_EVENT);
		hStopThreadEvent = CreateEvent(NULL, FALSE, FALSE, STOP_THREAD_EVENT);
		if(!hStartThreadEvent || !hStopThreadEvent)
		{
			DBGPrint(L"TDRThread - Failed to create synchronization events, error = %d", GetLastError());

			CSLocker Lock(pParams->csCritSection);
			pParams->hr = E_FAIL;
			*(pParams->pbStopNow) = true;

			if(pAdapterUtil)
			{
				delete pAdapterUtil;
				pAdapterUtil = NULL;
			}

			return -1;
		}

		// Set Event to let main thread know that we are ready
		SetEvent(hStartThreadEvent);

		// Give main thread a chance to initialize and start presenting
		// We need to give plenty of time so that dGPU has a chance to present some content
		// Otherwise we might end up in situation where dGPU TDR'ed at the end of previous test case and then it TDRs in the begining
		// of another test case without having a chance to process any DMAs. This will lead to unrecoverable TDR
		Sleep(10000); 
		srand(123);

		DBGPrint(L"TDRThread - Starting TDR loop");
		do
		{
			UINT action = 0;
			{
				CSLocker Lock(pParams->csCritSection);
				action = pParams->action;
			}
            
			UINT nodeOrdinal = 0;

			switch(action)
			{
				case 0:
				{
					DBGPrint(L"TDRThread - Prepare to force TDR on iGPU adapter");

					CSLocker Lock(pParams->csCritSection);
					pParams->bDGPU = false;
					pParams->bIGPU = true;

					if(*(pParams->pbIsWindowed) == false)
					{
						DBGPrint(L"TDRThread - When running in Full-Screen mode, application is expected to get device loss error when iGPU TDRs");
						*(pParams->pbExpectDeviceLost) = true;
					}
					else
					{
						DBGPrint(L"TDRThread - When running in Full-Screen mode, application is NOT expected to get device loss error when iGPU TDRs");
					}

					pParams->hr = pIntegrated->ForceGPUTDR(nodeOrdinal);

					if(FAILED(pParams->hr))
					{
						DBGPrint(L"TDRThread - Failed to force TDR on iGPU, hr = 0x%X", pParams->hr);
						continue;
					}

					DBGPrint(L"TDRThread - Successfully forced TDR on iGPU");
					break;
				}

				case 1:
				{
					DBGPrint(L"TDRThread - Prepare to force TDR on dGPU adapter");

					CSLocker Lock(pParams->csCritSection);
					pParams->bIGPU = false;
					pParams->bDGPU = true;

					DBGPrint(L"TDRThread - application is expected to get device loss error when dGPU TDRs");
					*(pParams->pbExpectDeviceLost) = true;

					pParams->hr = pDiscrete->ForceGPUTDR(nodeOrdinal);

					if(FAILED(pParams->hr))
					{
						DBGPrint(L"TDRThread - Failed to force TDR on dGPU, hr = 0x%X", pParams->hr);
						continue;
					}

					DBGPrint(L"TDRThread - Successfully forced TDR on dGPU");
					break;
				}

				case 2:
				{
					DBGPrint(L"TDRThread - Prepare to force TDR on dGPU and iGPU adapters");

					CSLocker Lock(pParams->csCritSection);
					pParams->bIGPU = true;
					pParams->bDGPU = true;

					DBGPrint(L"TDRThread - application is expected to get device loss error when both iGPU and dGPU TDR");
					*(pParams->pbExpectDeviceLost) = true;

					pParams->hr = pDiscrete->ForceGPUTDR(nodeOrdinal);

					if(FAILED(pParams->hr))
					{
						DBGPrint(L"TDRThread - Failed to force TDR on dGPU, hr = 0x%X", pParams->hr);
						continue;
					}

					DBGPrint(L"TDRThread - Successfully forced TDR on dGPU");

					pParams->hr = pIntegrated->ForceGPUTDR(nodeOrdinal);

					if(FAILED(pParams->hr))
					{
						DBGPrint(L"TDRThread - Failed to force TDR on iGPU, hr = 0x%X", pParams->hr);
						continue;
					}

					DBGPrint(L"TDRThread - Successfully forced TDR on iGPU");

					break;
				}

				default:
				{
					DBGPrint(L"TDRThread - invalid action");
					break;
				}
			}
		}
		while(WAIT_TIMEOUT == WaitForSingleObject(hStopThreadEvent, TDR_THREAD_WAIT_TIME));
	}
	catch(...)
	{
		DBGPrint(L"TDRThread - exception is caught in TDR thread");
		CSLocker Lock(pParams->csCritSection);
		if(SUCCEEDED(pParams->hr))
			pParams->hr = E_FAIL;
		*(pParams->pbStopNow) = true;

		if(pAdapterUtil)
		{
			delete pAdapterUtil;
			pAdapterUtil = NULL;
		}

		return -1;
	}

	if(pAdapterUtil)
	{
		delete pAdapterUtil;
		pAdapterUtil = NULL;
	}

	if(hStopThreadEvent)
	{
		CloseHandle(hStopThreadEvent);
		hStopThreadEvent = NULL;
	}

	if(hStartThreadEvent)
	{
		CloseHandle(hStartThreadEvent);
		hStartThreadEvent = NULL;
	}

	return 1;
}
