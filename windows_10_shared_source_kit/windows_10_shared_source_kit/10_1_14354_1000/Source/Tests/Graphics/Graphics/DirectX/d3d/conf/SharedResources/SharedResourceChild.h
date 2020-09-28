//////////////////////////////////////////////////////////////////////////
//  SharedResourceChild.h
//  created:	2007/02/05
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#include "SharedResource.h"

class SharedResourcesChild : public SharedResourcesTest
{
public:
	SharedResourcesChild(){ };

	void InitTestParameters() {};
	
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
};

extern const UINT g_NumTiles;
class ShareTilePoolChild : public ShareTilePoolTest
{
public:
	ShareTilePoolChild(){
		m_FeatureLevel = D3D_FEATURE_LEVEL_11_0;
		m_DriverType = D3D_DRIVER_TYPE_UNKNOWN;
	};

	void InitTestParameters() {};
	
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();

	TEST_RESULT SetupD3DObjects();
	void CleanupAllD3DObjects()
	{
		CleanupD3DObjects();
	};

	TEST_RESULT VerifySharedTilePool();

	D3D_FEATURE_LEVEL		m_FeatureLevel;
	D3D_DRIVER_TYPE			m_DriverType;
};

