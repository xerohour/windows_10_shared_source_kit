#include "multisample.h"
#include "samples.h"
#include "coverage.h"
#include "mask.h"
#include "occlusion.h"
#include "centroid.h"
#include "depth.h"
#include "resolve.h"
//#include "scenario.h"
//#include "present.h"


DXGI_FORMAT ScanOutFormats[] =
{
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB
};
UINT ScanOutFormatsSize = ARRAY_SIZE(ScanOutFormats);

bool CMultisampleTestApp::InitTestGroups()
{
	if ( RESULT_PASS != SetupDXGIAndD3D() )
	{
		ADD_TEST( _T("NoGroups"), CDummyTest );
		return true;
	}

	// required 4xMSAA support for dx10.1
	if (m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
	{
		if ( !ADD_TEST( _T("Support/4xMSAA"), CSupport4xTest ) )
			return false;
	}

	// required 8xMSAA support for dx11.0
	if (m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0)
	{
		if ( !ADD_TEST( _T("Support/8xMSAA"), CSupport8xTest ) )
			return false;
	}

	if( !ADD_TEST( _T( "Count" ), CCountTest ) ) return false;
	if( !ADD_TEST( _T( "Resolve" ), CResolveTest ) ) return false;

	if( !ADD_TEST( _T( "Coverage" ), CCoverageTest ) ) return false;
	if( !ADD_TEST( _T( "Mask/Single" ), CMultisampleTest_Mask ) ) return false;
	if( !ADD_TEST( _T( "Occlusion/Query" ), CMultisampleTest_OccQuery ) ) return false;
	if( !ADD_TEST( _T( "Occlusion/Predicate" ), CMultisampleTest_OccPredicate ) ) return false;
	if( !ADD_TEST( _T( "Interpolation/Centroid" ), CMultisampleTest_Centroid ) ) return false;
	if (m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
	{
		if( !ADD_TEST( _T( "Interpolation/Sample" ), CSampleTest ) ) return false;
	}
	if ( m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		if (!ADD_TEST( _T( "Interpolation/Centroid11" ), CMultisampleTest_Centroid11 ) ) 
			return false;
		if (!ADD_TEST( _T( "Interpolation/Centroid11_Uncovered" ), CMultisampleTest_Centroid11Uncovered ) ) 
			return false;
	}

	// SV_COVERAGE system generated value is only supported on feature level 11.0 and above
	// The test for this only works with _UINT formats
	if( m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_11_0 )
	{
		if( !ADD_TEST( _T( "InputCoverage" ), CInputCoverageTest ) ) 
			return false;

		if( !ADD_TEST( _T( "InputCoverageWithDepth" ), CInputCoverageWithDepthTest ) ) 
			return false;
	}

	if( !ADD_TEST( _T( "Depth" ), CMultisampleTest_Depth ) ) return false;
	CleanupDXGI();

	return true;

}
