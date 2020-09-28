#if !defined(_NTSTATUS_)
typedef long NTSTATUS;
#define STATUS_SUCCESS                   ((NTSTATUS)0x00000000L)
#endif
#include "windows.h"
#include "d3dkmthk.h"
#include "CheckPostAdapterBIOSColorDepth.h"
#include "AdapterUtil.h"
#include "CustomTestFactors.h"

#include <BDD_DIAG.h>

bool DoesConfigMeetMultiMonRequirements();

CheckPostAdapterBIOSColorDepth::CheckPostAdapterBIOSColorDepth( LoggerWrapper* pLogger )
    : AbstractTest(pLogger)
{
}

void CheckPostAdapterBIOSColorDepth::InitializeParameters()
{
    SetRootTestFactor( AddParameter<UINT32>( _T("TestCase"), &m_Value, 1 ) );
}

TestResult::Type CheckPostAdapterBIOSColorDepth::SetupTest()
{
    // Since this test is only checking the POST adapter, multimon isn't really relevant to what is being tested
    if( !DoesConfigMeetMultiMonRequirements() )
    {
        LOG_MESSAGE( _T("Test machine doesn't meet multimon config requirements.") );
        return TestResult::Skip;
    }
    return TestResult::Pass;
}

TestResult::Type CheckPostAdapterBIOSColorDepth::SetupTestCase()
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

    return TestResult::Pass;
}

TestResult::Type CheckPostAdapterBIOSColorDepth::ExecuteTestCase()
{
    using namespace DevX::GRFX;
    TestResult::Type result = TestResult::Fail;

    // Ideally the min width and height values should be parameterized rather than coded locally to each test
    const USHORT MinWidth = 1024;
    const USHORT MinHeight = 768;

    ccd::DisplayTopology displayTopology;
    ccd::DisplaySettings displaySettings;
    LONG ret = displayTopology.WrappedQueryDisplayConfig( QDC_ONLY_ACTIVE_PATHS, &displaySettings );
    if( ERROR_SUCCESS != ret )
    {
        LOG_ERROR( ret, _T("Failed to initialize test parameters due to previous error.") );
        return TestResult::Fail;
    }

    if( displaySettings.m_NumPaths == 0 )
    {
        LOG_ERROR( E_FAIL, _T("Test machine unexpectedly has no active paths.") );
        return TestResult::Fail;
    }

    std::unique_ptr<AdapterDescriptor> rAdapter( AdapterDescriptor::GetAdapterDescriptor( displaySettings.m_pPaths[0].sourceInfo.adapterId ) );

    BDD_DIAG BddDiag = {0};

    D3DKMT_ESCAPE sBddEsc = {0};
    //sBddEsc.hAdapter      Doesn't matter which adapter handle we send down, call will find fallback driver
    sBddEsc.hDevice = NULL;
    sBddEsc.Type = D3DKMT_ESCAPE_BDD_FALLBACK;
    sBddEsc.pPrivateDriverData = &BddDiag;
    sBddEsc.PrivateDriverDataSize = sizeof(BddDiag);

    HRESULT hr = rAdapter->DXGKrnlEscape( sBddEsc );
    if( FAILED(hr) )
    {
        LOG_ERROR( hr, _T("Failed on BDD diagnostic request.") );
        return TestResult::Fail;
        // Not returning so can do the pruned out modes
    }

    // Look for any 32bpp mode with at least the minimum required size
    // It is OK if there are other modes in the list which don't support 32bpp

    for( DWORD i = 0;
        (i < MAX_VBE_MODES);
        ++i )
    {
        if (BddDiag.BddModes[i].Flags.IsValidMode &&
            (BddDiag.BddModes[i].VbeModeInfo.BitsPerPixel >= 32) &&
            ((BddDiag.BddModes[i].VbeModeInfo.XResolution*BddDiag.BddModes[i].VbeModeInfo.YResolution) >= (MinWidth*MinHeight)))
        {
            result = TestResult::Pass;
            break;
        }
    }

    if (result == TestResult::Fail)
    {
        LOG_ERROR(
        E_FAIL,
        _T("No 32bpp BIOS mode found with at least the minimum required resolution of %d x %d"),
        MinWidth,
        MinHeight);
    }

    return result;
}

void CheckPostAdapterBIOSColorDepth::CleanupTestCase()
{
}

void CheckPostAdapterBIOSColorDepth::CleanupTest()
{
}

tstring CheckPostAdapterBIOSColorDepth::GetTestName()
{
    return _T("Check POST Adapter BIOS Modes.");
}
