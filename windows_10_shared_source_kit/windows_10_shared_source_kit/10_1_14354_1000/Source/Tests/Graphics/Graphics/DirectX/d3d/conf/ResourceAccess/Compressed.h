#pragma once

#include "ResourceAccess.h"

#include <map>
#include <list>
using namespace std;

#define DESC_PARAM_ARRAYSIZE		"The array size of the created resources"		
#define DESC_PARAM_WIDTH			"The width of the created resources"
#define DESC_PARAM_HEIGHT			"The height of the created resources, if applicable to its dimensions"
#define DESC_PARAM_DEPTH			"The depth of the created resources, if applicable to its dimensions"	
#define DESC_PARAM_RESOURCESCENARIO "The type of resource to be tested (i.e. tex1d/2d/3d/etc.)"
#define DESC_PARAM_SRCFORMAT		"The format of the source texture, can be either a compressed or corresponding structured format"	
#define DESC_PARAM_DESTFORMAT		"The format of the destination texture, can be either a compressed or corresponding structured format"	
#define DESC_PARAM_SRCFORMATSAME	"The format when both textures use the same format, special cases with different values than the other two format parameters"

typedef map<DXGI_FORMAT, list<DXGI_FORMAT>> FormatMap;

class CCopyCompressedTest : public CResourceAccessTest
{
public:
	
	CCopyCompressedTest( ) 
	{
		m_AccessMethod = AM_COPY;

		list<DXGI_FORMAT> sharedExpList;
		sharedExpList.push_back( DXGI_FORMAT_R9G9B9E5_SHAREDEXP );

		list<DXGI_FORMAT> bc14List;
		bc14List.push_back( DXGI_FORMAT_BC1_UNORM );
		bc14List.push_back( DXGI_FORMAT_BC1_UNORM_SRGB );
		bc14List.push_back( DXGI_FORMAT_BC4_UNORM );
		bc14List.push_back( DXGI_FORMAT_BC4_SNORM );

		list<DXGI_FORMAT> bc235List;
		bc235List.push_back( DXGI_FORMAT_BC2_UNORM );
		bc235List.push_back( DXGI_FORMAT_BC2_UNORM_SRGB );
		bc235List.push_back( DXGI_FORMAT_BC3_UNORM );
		bc235List.push_back( DXGI_FORMAT_BC3_UNORM_SRGB );
		bc235List.push_back( DXGI_FORMAT_BC5_UNORM );
		bc235List.push_back( DXGI_FORMAT_BC5_SNORM );

		m_Formats.clear();
		m_Formats[ DXGI_FORMAT_R32_UINT ] = sharedExpList;
		m_Formats[ DXGI_FORMAT_R32_SINT ] = sharedExpList;
		m_Formats[ DXGI_FORMAT_R16G16B16A16_UINT ] = bc14List;
		m_Formats[ DXGI_FORMAT_R16G16B16A16_SINT ] = bc14List;
		m_Formats[ DXGI_FORMAT_R32G32_UINT ] = bc14List;
		m_Formats[ DXGI_FORMAT_R32G32_SINT ] = bc14List;
		m_Formats[ DXGI_FORMAT_R32G32B32A32_UINT ] = bc235List;
		m_Formats[ DXGI_FORMAT_R32G32B32A32_SINT ] = bc235List;
	}

	void			InitTestParameters();
	TEST_RESULT		Setup();
	TEST_RESULT		SetupTestCase();
	TEST_RESULT		CreateResources();
	TEST_RESULT		ExecuteTestCase();
	
	bool			IsFormatCompatible( DXGI_FORMAT structuredFmt, DXGI_FORMAT compressedFmt );

protected:
	FormatMap		m_Formats;
};
