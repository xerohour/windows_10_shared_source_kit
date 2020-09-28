// The ActionMacros.h file requires the following three headers
#include <nt.h>
#include <ntrtl.h>
#include <nturtl.h>
                
// Included for friendly macros
#include <ActionMacros.h>

#include <tchar.h>
#include <dxgi.h>

#include <tdh.h>
#include <EventHelper.h>
#include <etwLibEvents.h>

#include <Provider.h>
#include "etwLibEventHelper.h"

#include <D3Dx9math.h>
#include <d3d11_2.h>

#include "WexTestClass.h"
#include <vector>
#include "XperfHelper.h"
#include "DxEtwHelper.h"
#include "SchedPerfCommon.h"

#include <atlbase.h>
#include "GraphicsWorkload.h"

using namespace WEX::Logging;
using namespace WEX::TestExecution;
using namespace WEX::Common;

#define BASIC_RENDER_VENDOR 0x1414
#define BASIC_RENDER_DEVICE 0x8c

class HybridPerf
{
public:
    BEGIN_TEST_CLASS(HybridPerf)
        TEST_CLASS_PROPERTY( L"Kits.Specification", L"System.Fundamentals.Graphics.HybridGraphics.MultiGPU" )
		TEST_CLASS_PROPERTY( L"Kits.RunElevated", L"true" )
    END_TEST_CLASS();

    TEST_CLASS_SETUP(Setup);

    void RunTest(UINT numShaderLoops, UINT numPrimitives, wchar_t* testName);

#if 0
    // Disabling as part of bug 2242775
    BEGIN_TEST_METHOD(FillRate)
		TEST_METHOD_PROPERTY( L"Kits.Description", L"This test verifies whether the discrete GPU is able to rasterize more quickly than the integrated GPU." )

		TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test for hybrid fill rate performance" )
		TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
		TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )
		TEST_METHOD_PROPERTY( L"Kits.Usage", L"This test ensures that the discrete GPU in a hybrid configuration is able to produce pixels with a simple shader faster than the integrated GPU in the system." )

		TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
        TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
        TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
		TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
		TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

		TEST_METHOD_PROPERTY( L"Kits.TestId", L"da83732d-d42e-42ce-ad66-44002bb9a239" )
		TEST_METHOD_PROPERTY( L"Kits.TestId2", L"ecb71287-959e-4895-b343-d7f6eb533d10" )
		TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x86" )
		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
		TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
		TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )
        
		TEST_METHOD_PROPERTY( L"Kits.DeploymentItem", L"NTTEST\\windowstest\\graphics\\d3d\\support\\D3DCompiler_Test.dll" )

    END_TEST_METHOD();

    BEGIN_TEST_METHOD(ShaderThroughput)
		TEST_METHOD_PROPERTY( L"Kits.Description", L"This test verifies whether the discrete GPU is able to compute more quickly than the integrated GPU." )

		TEST_METHOD_PROPERTY( L"Kits.TestName", L"Test for hybrid shader throughput performance" )
		TEST_METHOD_PROPERTY( L"Kits.ExpectedRuntime", L"2" )
		TEST_METHOD_PROPERTY( L"Kits.IsManual", L"false" )
		TEST_METHOD_PROPERTY( L"Kits.Usage", L"This test ensures that the discrete GPU in a hybrid configuration is able to handle shader invocations faster than the integrated GPU in the system." )

		TEST_METHOD_PROPERTY( L"Kits.ProvidesAssessmentResult", L"false" )
        TEST_METHOD_PROPERTY( L"Kits.HasSupplementalContent", L"false" )
        TEST_METHOD_PROPERTY( L"Kits.DevelopmentPhase", L"Tuning and Validation" )
		TEST_METHOD_PROPERTY( L"Kits.PublishingOrganization", L"Microsoft Corporation" )
		TEST_METHOD_PROPERTY( L"Kits.TestType", L"Development" )

		TEST_METHOD_PROPERTY( L"Kits.TestId", L"47597ee0-e421-4477-b605-841137bf360a" )
		TEST_METHOD_PROPERTY( L"Kits.TestId2", L"f77582ce-2189-45ee-8067-a8e8d31fd45e" )
		TEST_METHOD_PROPERTY( L"Kits.TimeoutInMinutes", L"4" )

		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x86" )
		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Client x64" )
		TEST_METHOD_PROPERTY( L"Kits.SupportedOS", L"Windows v10.0 Server x64" )
		TEST_METHOD_PROPERTY( L"Kits.MinRelease", L"TH1" )
		TEST_METHOD_PROPERTY( L"Kits.CorePackageComposition", L"Full" )

		TEST_METHOD_PROPERTY( L"Kits.DeploymentItem", L"NTTEST\\windowstest\\graphics\\d3d\\support\\D3DCompiler_Test.dll" )

    END_TEST_METHOD();
#endif
};
