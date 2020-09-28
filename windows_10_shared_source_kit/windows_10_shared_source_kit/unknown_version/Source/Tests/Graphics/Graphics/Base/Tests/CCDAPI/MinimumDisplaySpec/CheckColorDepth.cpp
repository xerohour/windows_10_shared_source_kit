#if !defined(_NTSTATUS_)
typedef long NTSTATUS;
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)
#endif
#include "windows.h"
#include "d3dkmthk.h"
#include "CheckColorDepth.h"
#include "AdapterUtil.h"
#include "CustomTestFactors.h"
#include <memory>

#include <d3dkmthk.h>
#include <dmmdiag.h>
#include <BDD_DIAG.h>

bool DoesConfigMeetMultiMonRequirements();

struct CheckColorDepth::Internal
{
	std::unique_ptr<DevX::GRFX::AdapterDescriptor> m_rAdapter;
};

CheckColorDepth::CheckColorDepth( LoggerWrapper* pLogger )
	: AbstractTest(pLogger)
	, m_pInternals(nullptr)
{
}

void CheckColorDepth::InitializeParameters()
{
	ccd::DisplaySettings displaySettings;
	LONG ret = m_DisplayTopology.WrappedQueryDisplayConfig( QDC_ONLY_ACTIVE_PATHS, &displaySettings );
	if( ERROR_SUCCESS != ret )
	{
		LOG_ERROR( ret, _T("Failed to initialize test parameters due to previous error.") );
		return;
	}

	CTestCaseParameter<LUID>* pAdapterParam = AddParameter( _T("AdapterLUID"), &m_AdapterLUID );
	CUserValueSet<LUID>* pAdapterLUIDs = new CUserValueSet<LUID>();
	CTestCaseParameter<UINT32>* pSourceIdParam = AddParameter( _T("SourceId"), &m_nSourceId );
	CUserValueSet<UINT32>* pSourceIds = new CUserValueSet<UINT32>();

	for( UINT32 i = 0; i < displaySettings.m_NumPaths; ++i )
	{
		DISPLAYCONFIG_PATH_INFO const* pPath = displaySettings.m_pPaths + i;
		if( pPath->flags & DISPLAYCONFIG_PATH_ACTIVE )
		{
			pAdapterLUIDs->AddValue( pPath->sourceInfo.adapterId );
			pSourceIds->AddValue( pPath->sourceInfo.id );
		}
	}

	testfactor::RFactor pFactors[] = 
	{
		AddParameterValueSet( pAdapterParam, pAdapterLUIDs ),
		AddParameterValueSet( pSourceIdParam, pSourceIds )
	};

	SetRootTestFactor( NewSimultaneousIndependentSetFactor( pFactors, pFactors + (sizeof(pFactors)/sizeof(*pFactors)) ) );
}

TestResult::Type CheckColorDepth::SetupTest()
{
	{
		OSVERSIONINFO info;
		memset( &info, 0, sizeof(info) );
		info.dwOSVersionInfoSize = sizeof(info);

		if( !GetVersionEx(&info) )
		{
			LOG_ERROR( HRESULT_FROM_WIN32(GetLastError()), _T("Failed to get OS version information.") );
			return TestResult::Fail;
		}

		if( (6 == info.dwMajorVersion && info.dwMinorVersion < 2) ||
			info.dwMajorVersion < 6 )
		{
			// This test is win8+ only
			LOG_MESSAGE( _T("Skipping test that can't run on win7 or below.") );
			return TestResult::Skip;
		}
	}

	if( !DoesConfigMeetMultiMonRequirements() )
	{
		LOG_MESSAGE( _T("Test machine doesn't meet multimon config requirements.") );
		return TestResult::Skip;
	}
	m_pInternals = new Internal();
	return TestResult::Pass;
}

TestResult::Type CheckColorDepth::SetupTestCase()
{
	using namespace DevX::GRFX;
	m_pInternals->m_rAdapter.reset( AdapterDescriptor::GetAdapterDescriptor( m_AdapterLUID ) );

	DXGI_ADAPTER_DESC1 adapterDesc = m_pInternals->m_rAdapter->GetDxgiAdapterDesc1();

	ccd::DisplayTopology displayTopology;
	DISPLAY_DEVICE dd;
	memset( &dd, 0, sizeof(dd) );
	dd.cb = sizeof(dd);
	if( !displayTopology.GetDisplayDevice( m_AdapterLUID, m_nSourceId, &dd ) )
	{
		LOG_ERROR( 
			E_FAIL, 
			_T("Failed to get GDI device info for adapter LUID: 0x%8X%8X"),
			m_AdapterLUID.HighPart,
			m_AdapterLUID.LowPart );
		return TestResult::Fail;
	}

	LOG_MESSAGE( 
		_T("Checking mode list for the following device:\n%s\n%s\n%s"),
		dd.DeviceName,
		dd.DeviceString,
		dd.DeviceID );
	
	// Activate the diagnostics we want
	D3DKMT_ACTIVATE_SPECIFIC_DIAG_ESCAPE ActivateEscape;
	ActivateEscape.Type = D3DKMT_ACTIVATE_SPECIFIC_DIAG_TYPE_MODES_PRUNED;
	ActivateEscape.Activate = TRUE;

	D3DKMT_ESCAPE sEsc = {0};
	sEsc.Type = D3DKMT_ESCAPE_ACTIVATE_SPECIFIC_DIAG;
	sEsc.pPrivateDriverData = &ActivateEscape;
	sEsc.PrivateDriverDataSize = sizeof(ActivateEscape);

	HRESULT hr = m_pInternals->m_rAdapter->DXGKrnlEscape( sEsc );
	if( FAILED(hr) )
	{
		LOG_ERROR( hr, _T("Failed dxgk escape for diagnostic activation.") );
		return TestResult::Fail;
	}

	return TestResult::Pass;
}

TestResult::Type CheckColorDepth::ExecuteTestCase()
{
	TestResult::Type result = TestResult::Pass;

	ccd::DisplayTopology displayTopology;
	ccd::DisplaySettings displaySettings;

	// Trigger the re-enumeration of the modes for this adapter.
	LONG status = displayTopology.WrappedQueryDisplayConfig( QDC_ONLY_ACTIVE_PATHS, &displaySettings );
	if( status != ERROR_SUCCESS )
	{
		// error reported by displayTopology object
		return TestResult::Fail;
	}

	status = displayTopology.SetDisplayConfig( &displaySettings, (SDC_FORCE_MODE_ENUMERATION | SDC_USE_SUPPLIED_DISPLAY_CONFIG | SDC_APPLY) );
	if( status != ERROR_SUCCESS )
	{
		// error reported by displayTopology object
		return TestResult::Fail;
	}

	D3DKMT_DISPLAYMODELIST dispModeListForSize = {0};
	dispModeListForSize.VidPnSourceId = m_nSourceId;

	D3DKMT_ESCAPE sEsc = {0};
	sEsc.Type = D3DKMT_ESCAPE_MODES_PRUNED_OUT;
	sEsc.pPrivateDriverData = &dispModeListForSize;
	sEsc.PrivateDriverDataSize = sizeof(dispModeListForSize);

	HRESULT hr = m_pInternals->m_rAdapter->DXGKrnlEscape( sEsc );
	if( FAILED(hr) )
	{
		LOG_ERROR( hr, _T("Failed dxgk escape for pruned mode size retrieval.") );
		return TestResult::Fail;
	}

	const UINT32 sizeNeeded = sizeof(D3DKMT_DISPLAYMODELIST) + (sizeof(D3DKMT_DISPLAYMODE) * dispModeListForSize.ModeCount);
	std::unique_ptr<BYTE[]> rModeList;
	rModeList.reset( new BYTE[sizeNeeded] );
	memset( rModeList.get(), 0, sizeNeeded );
	D3DKMT_DISPLAYMODELIST* pModeList = (D3DKMT_DISPLAYMODELIST*)rModeList.get();

	pModeList->ModeCount = dispModeListForSize.ModeCount;
	pModeList->VidPnSourceId = m_nSourceId;

	sEsc.pPrivateDriverData = pModeList;
	sEsc.PrivateDriverDataSize = sizeNeeded;

	hr = m_pInternals->m_rAdapter->DXGKrnlEscape( sEsc );
	if( FAILED(hr) )
	{
		LOG_ERROR( hr, _T("Failed dxgk escape for pruned mode retrieval.") );
		return TestResult::Fail;
	}

	if( pModeList->ModeCount > 0 )
	{
		result = TestResult::Fail;

		WCHAR* const pRotations[] = 
		{
			_T("Identity"),
			_T("90"),
			_T("180"),
			_T("270")
		};

		for( UINT32 i = 0; i < pModeList->ModeCount; ++i )
		{
			// list out mode that was filtered by the OS
			LOG_ERROR( 
				E_FAIL, 
				_T("Mode %d x %d D3DDDIFomrat: 0x%08X @ %dHz rotation: %s, should not be reported by the adapter."),
				pModeList->pModeList[i].Width,
				pModeList->pModeList[i].Height,
				pModeList->pModeList[i].Format,
				pModeList->pModeList[i].IntegerRefreshRate,
				pRotations[pModeList->pModeList[i].DisplayOrientation - 1] );
		}
	}

	return result;
}

void CheckColorDepth::CleanupTestCase()
{
	// Activate the diagnostics we want
    D3DKMT_ACTIVATE_SPECIFIC_DIAG_ESCAPE ActivateEscape; // Will reuse this for deactivation
    ActivateEscape.Type = D3DKMT_ACTIVATE_SPECIFIC_DIAG_TYPE_MODES_PRUNED;
    ActivateEscape.Activate = FALSE;

    D3DKMT_ESCAPE sEsc = {0}; // Reusing this struct all over the place, shouldn't cause problems so long as we're careful
    sEsc.Type = D3DKMT_ESCAPE_ACTIVATE_SPECIFIC_DIAG;
    sEsc.pPrivateDriverData = &ActivateEscape;
    sEsc.PrivateDriverDataSize = sizeof(ActivateEscape);

	HRESULT hr = m_pInternals->m_rAdapter->DXGKrnlEscape( sEsc );
	if( FAILED(hr) )
	{
		LOG_ERROR( hr, _T("Failed to disabled dxgk escape specific diagnostic.") );
	}

	m_pInternals->m_rAdapter.reset();
}

void CheckColorDepth::CleanupTest()
{
	delete m_pInternals;
	m_pInternals = nullptr;
}

tstring CheckColorDepth::GetTestName()
{
	return _T("Check Adapter Reported Color Depth.");
}
