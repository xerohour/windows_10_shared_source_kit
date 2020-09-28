/////////////////////////////////////////////////////////////////////////////////////////
//
//	Copyright (c) Microsoft Corporation
//
//	Module Name:
//	   DXGIHybridPresentChild.cpp
//
//	Abstract:
//	This file contains implementation of DXGIHybridPresentChild tests
//
//	History:
//	04/01/2013	MarMel	Created.
//
//////////////////////////////////////////////////////////////////////////////////////////

#include "DXGIHybridPresentChild.h"
#include "TestApp.h"

// this extern is defined in TestXKit which is referenced by CrossProceSharing. Define it here
// to some value, so that we don't have to link to the whole TestXKit
extern bool g_bCommonCtrlInit = false;

#define CLIENT_WAIT                                            \
DBGPrint(L"CLIENT_WAIT - Checking if manager asked us to change state");     \
                                                               \
State = m_pClientSync->CheckForRequest();                      \
if(State == eStateRun)                                         \
{                                                              \
	DBGPrint(L"CLIENT_WAIT - Client is running");              \
}                                                              \
else if(State == eStateTerminate)                              \
{                                                              \
	DBGPrint(L"CLIENT_WAIT - Client is terminating");          \
	goto MacroFailed;                                          \
}                                                              \
else                                                           \
{                                                              \
	DBGPrint(L"CLIENT_WAIT - Client is asked to move into unexpected state %d", State);\
	assert(0);                                                 \
}                                                              \
DBGPrint(L"CLIENT_WAIT - Waiting for manager signal");         \
if(WAIT_OBJECT_0 !=  m_pClientSync->WaitForSignal(SYNC_PAIR_INDEX, MANAGER_HANDLE, XPROC_TIMEOUT))         \
{                                                               \
	DBGPrint(L"CLIENT_WAIT - Client timed out waiting for manager's signal"); \
	tr = RESULT_BLOCKED;                                        \
	goto MacroFailed;                                           \
}                                                               \
DBGPrint(L"CLIENT_WAIT - Client got manager's signal");         \

#define CLIENT_SIGNAL                                           \
DBGPrint(L"CLIENT_SIGNAL - Checking if manager asked us to change state");      \
State = m_pClientSync->CheckForRequest();                       \
if(State == eStateRun)                                          \
{                                                               \
	DBGPrint(L"CLIENT_SIGNAL - Client is running");             \
}                                                               \
else if(State == eStateTerminate)                               \
{                                                               \
	DBGPrint(L"CLIENT_SIGNAL - Client is terminating");         \
	goto MacroFailed;                                           \
}                                                               \
else                                                            \
{                                                               \
	DBGPrint(L"CLIENT_SIGNAL - Client is asked to move into unexpected state %d", State); \
	assert(0);                                                  \
}                                                               \
DBGPrint(L"CLIENT_SIGNAL - Signaling manager");                 \
if(!m_pClientSync->Signal(SYNC_PAIR_INDEX, CLIENT_HANDLE))      \
{                                                               \
	DBGPrint(L"CLIENT_SIGNAL - Failed to signal manager");      \
	tr = RESULT_BLOCKED;                                        \
	goto MacroFailed;                                           \
}                                                               


CHybridPresentChild::CHybridPresentChild()
	: m_pClientSync(NULL),
		m_TestCasesExecuted(0)
{
	m_NumberOfPresents = 2;
}

CHybridPresentChild::~CHybridPresentChild()
{
	CleanupTestCase();
}

void CHybridPresentChild::InitTestParameters()
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

	m_MaxBufferCount = 2;

	// Blt-Model Swap Effects
	CUserValueSet< DXGISwapEffectRecord > *pBltModelSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();

	if(!pBltModelSwapEffect)
		throw std::exception("Out of memory");

	for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
	{
		if(DXGISwapEffects[i].dxgiSwapEffect == DXGI_SWAP_EFFECT_SEQUENTIAL)
		{
			pBltModelSwapEffect->AddValue(DXGISwapEffects[i]);
		}
	}

	RFactor rfBltModelSwapEffect =  AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pBltModelSwapEffect);

	m_StartFullScreen = false;

	// RFactor for single bb mode
	RFactor SingleModeRF = CreateSingleModeFactor();
	RFactor Bpp32_DwmOn = m_FeatureLevelFactor  * SingleModeRF * rfBltModelSwapEffect;
    
	SetRootTestFactor( Bpp32_DwmOn );
}

TEST_RESULT CHybridPresentChild::SetupTestCase()
{
	DBGPrint(L"CHybridPresentChild::SetupTestCase() - entering, tests executed = %d", m_TestCasesExecuted);
	if(m_TestCasesExecuted)
		return RESULT_SKIP;

	TEST_RESULT tr = RESULT_PASS;

	assert(!m_pClientSync);

	m_pClientSync = new CXProcSyncClient(HYBRID_BASIC_MAPPING_NAME, HYBRID_BASIC_TOTAL_SHARERS, sizeof(sHYBRID_PRESENT_SHARED_DATA));

	if(!m_pClientSync)
	{
		WriteToLog(_T("Out of memory"));
		tr = RESULT_BLOCKED;
	}
    
	if(tr == RESULT_PASS)
	{
		tr = CHybridPresentRedirection::SetupTestCase();
	}

	if(tr != RESULT_PASS)
	{
		tr = RESULT_BLOCKED;

		if(m_pClientSync->AcquireLock())
		{
			sHYBRID_PRESENT_SHARED_DATA *pSharedData = (sHYBRID_PRESENT_SHARED_DATA*)m_pClientSync->GetExtraDataPtr();
			assert(pSharedData);

			pSharedData->pTest->WriteToLog(_T("CHybridPresentChild::SetupTestCase() - SetupTestCase() of base class failed"));
			_stprintf_s(pSharedData->strError, ERROR_SIZE, _T("CHybridPresentChild::ExecuteTestCase() - ExecuteTestCase() of parent class failed."));	
			pSharedData->tr = tr;

			m_pClientSync->ReleaseLock();
		}
	}

	return tr;
}

void CHybridPresentChild::CleanupTestCase()
{
	DBGPrint(L"CHybridPresentChild::CleanupTestCase() - entering, tests executed = %d", m_TestCasesExecuted);
	if(m_pClientSync)
	{
		delete m_pClientSync;
		m_pClientSync = NULL;
	}

	CHybridPresentRedirection::CleanupTestCase();

	m_TestCasesExecuted++;
}

TEST_RESULT CHybridPresentChild::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	TCHAR strError[ERROR_SIZE];
	ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);
	STATE_TYPE State;

	DBGPrint(L"CHybridPresentChild::ExecuteTestCase() - entering, tests executed = %d", m_TestCasesExecuted);

	m_pClientSync->TransitionTo(eStateReady);
	CLIENT_WAIT

	tr = CHybridPresentRedirection::ExecuteTestCase();

	if(tr != RESULT_PASS)
	{
		_stprintf_s(strError, ERROR_SIZE, _T("CHybridPresentChild::ExecuteTestCase() - ExecuteTestCase() of parent class failed."));		
	}

	CLIENT_SIGNAL

MacroFailed:
	// This is our normal exit
	if(tr != RESULT_PASS)
	{
		if(m_pClientSync->AcquireLock())
		{
			sHYBRID_PRESENT_SHARED_DATA *pSharedData = (sHYBRID_PRESENT_SHARED_DATA*)m_pClientSync->GetExtraDataPtr();
			memcpy(pSharedData->strError, strError, sizeof(pSharedData->strError));
			pSharedData->tr = tr;
			m_pClientSync->ReleaseLock();
		}
		else
			assert(0);
	}

	return tr;
}


TEST_RESULT CHybridPresentChildIdle::ExecuteTestCase()
{
	TEST_RESULT tr = RESULT_PASS;
	TCHAR strError[ERROR_SIZE];
	ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);
	STATE_TYPE State; 

	DBGPrint(L"CHybridPresentChildIdle::ExecuteTestCase() - transitioning to Ready state");
	m_pClientSync->TransitionTo(eStateReady);

	DBGPrint(L"CHybridPresentChildIdle::ExecuteTestCase() - starting execution");
	// Client will transition to Ready state after Present
	tr = CHybridPresentRedirection::ExecuteTestCase();

	if(tr != RESULT_PASS)
	{
		DBGPrint(L"CHybridPresentChildIdle::ExecuteTestCase() - execution failed");
		_stprintf_s(strError, ERROR_SIZE, _T("CHybridPresentChildIdle::ExecuteTestCase() - ExecuteTestCase() of parent class failed."));		
	}
	else
	{
		DBGPrint(L"CHybridPresentChildIdle::ExecuteTestCase() - successfully executed");
	}

	DBGPrint(L"CHybridPresentChildIdle::ExecuteTestCase() - signaling master that we are done");
	CLIENT_SIGNAL

MacroFailed:
	// This is our normal exit
	if(tr != RESULT_PASS)
	{
		if(m_pClientSync->AcquireLock())
		{
			DBGPrint(L"CHybridPresentChildIdle::ExecuteTestCase() - errors have been encountered, writing out failure info into the the shared memory");
			sHYBRID_PRESENT_SHARED_DATA *pSharedData = (sHYBRID_PRESENT_SHARED_DATA*)m_pClientSync->GetExtraDataPtr();
			memcpy(pSharedData->strError, strError, sizeof(pSharedData->strError));
			pSharedData->tr = tr;
			m_pClientSync->ReleaseLock();
		}
		else
		{
			DBGPrint(L"CHybridPresentChildIdle::ExecuteTestCase() - errors have been encountered, failed to lock shared memory while attempting to write errors out");
			assert(0);
		}
	}

	DBGPrint(L"CHybridPresentChildIdle::ExecuteTestCase() - exiting");
	return tr;
}

HRESULT CHybridPresentChildIdle::TransitionAfterPresent()
{
	TEST_RESULT tr = RESULT_PASS;
	TCHAR strError[ERROR_SIZE];
	ZeroMemory(strError, sizeof(TCHAR) * ERROR_SIZE);
	STATE_TYPE State; 
	// We aren't going to transition, but we are going to use this method to cause our application to be idle
	// this method is a convinient way to do so, since it is called from the Present loop of the base class

	// We are going to be stuck here until main app signals us
	DBGPrint(L"CHybridPresentChildIdle::TransitionAfterPresent() - Waitig for master's signal after Present");
	CLIENT_WAIT
	DBGPrint(L"CHybridPresentChildIdle::TransitionAfterPresent() - Got master's signal");

MacroFailed:
	return S_OK;
}