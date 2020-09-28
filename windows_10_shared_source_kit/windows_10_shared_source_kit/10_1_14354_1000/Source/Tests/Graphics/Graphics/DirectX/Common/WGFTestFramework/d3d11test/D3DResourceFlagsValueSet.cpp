#include "D3DResourceFlagsValueSet.h"


void __stdcall Tokenize( const tstring &str, std::vector<tstring> *pTokens )
{
	std::list<UINT> vIdx;
	for( UINT i = 0; i < str.length( ); ++i )
		if( str[ i ] == ',' )
			vIdx.push_back( i );

	std::list<UINT>::const_iterator itr = vIdx.begin( );
	int last = 0;
	while( itr != vIdx.end( ) )
	{
		pTokens->push_back( str.substr( last, *itr - last ) );
		last = *itr++ + 2;
	}
	pTokens->push_back( str.substr( last ) );
}

tstring __stdcall ToString( const D3DResourceFlags *pFlags )
{
	return	
		ToString( pFlags->Usage ) + ", " +
		ToString( (D3D11_BIND_FLAG) pFlags->BindFlags ) + ", " +
		ToString( (D3D11_CPU_ACCESS_FLAG) pFlags->CPUAccessFlags ) + ", " +
		ToString( (D3D11_RESOURCE_MISC_FLAG) pFlags->MiscFlags ) + ", " +
		ToString( (D3D11_MAP) pFlags->Map );
}

bool __stdcall FromString( D3DResourceFlags **ppFlags, const tstring &str )
{
	std::vector<tstring> vStr;
	Tokenize( str, &vStr );

	FromString( &(*ppFlags)->Usage, vStr[ 1 ] );
	FromString( (D3D11_BIND_FLAG*) &(*ppFlags)->BindFlags, vStr[ 0 ] );
	FromString( (D3D11_CPU_ACCESS_FLAG*) &(*ppFlags)->CPUAccessFlags, vStr[ 2 ] );
	FromString( (D3D11_RESOURCE_MISC_FLAG*) &(*ppFlags)->MiscFlags, vStr[ 3 ] );
	FromString( (D3D11_MAP*) &(*ppFlags)->Map, vStr[ 4 ] );

	return true;
}

//testfactor::RFactor __stdcall AddParametersForResourceFlagsValueSet( D3DResourceFlags *pFlags, const CD3DResourceFlagsValueSet* pValueSet )
//{
//	//testfactor::RFactor rVBIBCopySrc = AddParameterValueSet< D3DResourceFlags* >( pSrID3D11ResourceFlags, pVBIBCopySrcValueSet );
//	return NULL;
//}

BEGIN_NAMED_VALUES( RESOURCE_ACCESS_METHOD )
	NAMED_VALUE( "None", RAC_NONE )
	NAMED_VALUE( "Map", RAC_MAP )
	NAMED_VALUE( "Update_Src", RAC_UPDATE_SRC )
	NAMED_VALUE( "Update_Dest", RAC_UPDATE_DEST )
	NAMED_VALUE( "Copy_Src", RAC_COPY_SRC )
	NAMED_VALUE( "Copy_Dest", RAC_COPY_DEST )
	NAMED_VALUE( "CopyRegion_Src", RAC_COPYREGION_SRC )
	NAMED_VALUE( "CopyRegion_Dest", RAC_COPYREGION_DEST )
	NAMED_VALUE( "Resolve_Src", RAC_RESOLVE_SRC )
	NAMED_VALUE( "Resolve_Dest", RAC_RESOLVE_DEST )
END_NAMED_VALUES( RESOURCE_ACCESS_METHOD )

bool ValidD3DResourceFlags(D3D11_RESOURCE_DIMENSION ResourceDimension, D3D11_USAGE Usage, UINT BindFlags, UINT CPUAccessFlags, UINT MiscFlags, D3D11_MAP Map, RESOURCE_ACCESS_METHOD AccessMethod, bool bGenerateMapCombos)
{
	if (CPUAccessFlags & D3D11_CPU_ACCESS_WRITE)
	{
		//CPU Write only valid with Dynamic or Staging
		if (Usage != D3D11_USAGE_DYNAMIC && Usage != D3D11_USAGE_STAGING)
			return false;

		//CPU Write is invalid with GPU Output bind flags
		if (BindFlags & (D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_RENDER_TARGET | D3D11_BIND_DEPTH_STENCIL))
			return false;
	}

	if (CPUAccessFlags & D3D11_CPU_ACCESS_READ)
	{
		//CPU Read only valid on Stating
		if (Usage != D3D11_USAGE_STAGING)
			return false;

		//CPU Read can't be bound
		if (BindFlags != 0)
			return false;
	}

	if (Usage == D3D11_USAGE_STAGING)
	{
		//Staging must have Read or Write CPU Access
		if (CPUAccessFlags == 0)
			return false;

		//Staging can't be bound
		if (BindFlags != 0)
			return false;
	}

	if (Usage == D3D11_USAGE_DYNAMIC)
	{
		//Dynamic must have only WRITE CPUAccess
		if (CPUAccessFlags != D3D11_CPU_ACCESS_WRITE)
			return false;

		//Dynamic needs to be used with exactly one GPU Input bound flag
		if (BindFlags == 0 || (BindFlags != D3D11_BIND_VERTEX_BUFFER && BindFlags != D3D11_BIND_INDEX_BUFFER && BindFlags != D3D11_BIND_CONSTANT_BUFFER && BindFlags != D3D11_BIND_SHADER_RESOURCE) )
			return false;
	}

	if (Usage == D3D11_USAGE_IMMUTABLE)
	{
		//Immutable needs to be used with at least one GPU Input bound flag
		if (BindFlags == 0)
			return false;

		//Immutable needs to be used with at least one GPU Input bound flag
		if (0 == (BindFlags & (D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER | D3D11_BIND_CONSTANT_BUFFER | D3D11_BIND_STREAM_OUTPUT)))
			return false;

		//Immutable not allowed with GPU output
		if (BindFlags & (D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_RENDER_TARGET | D3D11_BIND_DEPTH_STENCIL))
			return false;
	}

	if (BindFlags & (D3D11_BIND_STREAM_OUTPUT | D3D11_BIND_RENDER_TARGET | D3D11_BIND_DEPTH_STENCIL))
	{
		//GPU Output flags needs to be Default
		if (Usage != D3D11_USAGE_DEFAULT)
			return false;
	}

	if (BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		//Can't have buffer or texture3d depthStencil
		if (ResourceDimension == D3D11_RESOURCE_DIMENSION_BUFFER || ResourceDimension == D3D11_RESOURCE_DIMENSION_TEXTURE3D )
			return false;
	}

	if (BindFlags & D3D11_BIND_CONSTANT_BUFFER)
	{
		//Can't have any other bind flag
		if (BindFlags & ~D3D11_BIND_CONSTANT_BUFFER)
			return false;

		//Required to by dynamic
		if (Usage != D3D11_USAGE_DYNAMIC)
			return false;
	}

	if (ResourceDimension != D3D11_RESOURCE_DIMENSION_BUFFER)
	{
		//Some bind flags are restricted to buffers
		if (BindFlags & (D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER | D3D11_BIND_CONSTANT_BUFFER | D3D11_BIND_STREAM_OUTPUT))
			return false;
	}

	if (MiscFlags & D3D11_RESOURCE_MISC_GENERATE_MIPS)
	{
		//Mipgen invalid for buffers
		if (ResourceDimension == D3D11_RESOURCE_DIMENSION_BUFFER)
			return false;
	}

	//if (MiscFlags & D3D11_RESOURCE_MISC_SHARED)
	//{
	//	//Shared resources not yet supported
	//	return false;
	//}

	if (MiscFlags & D3D11_RESOURCE_MISC_TEXTURECUBE)
	{
		//Texture cubes needs to be 2D textures
		if ( ResourceDimension != D3D11_RESOURCE_DIMENSION_TEXTURE2D )
			return false;
	}

	//Maps needs to be in one spot so that they can be skipped
	if (bGenerateMapCombos)
	{
		if (Map != (D3D11_MAP)0)
		{
			//Can't have a map access if it's not set in CPUAccess
			if (CPUAccessFlags == 0)
				return false;

			if (Map == D3D11_MAP_WRITE || Map == D3D11_MAP_WRITE_DISCARD || Map == D3D11_MAP_WRITE_NO_OVERWRITE || Map == D3D11_MAP_READ_WRITE)
			{
				//Can't have write map access if no write CPUAccess
				if (0 == (CPUAccessFlags & D3D11_CPU_ACCESS_WRITE))
					return false;
			}

			if (Map == D3D11_MAP_READ || Map == D3D11_MAP_READ_WRITE)
			{
				//Can't have read map access if no read CPUAccess
				if (0 == (CPUAccessFlags & D3D11_CPU_ACCESS_READ))
					return false;
			}

			if (Map == D3D11_MAP_WRITE_NO_OVERWRITE)
			{
				//Only VB and IB can use this write access
				if (0 == (BindFlags & (D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_INDEX_BUFFER)))
					return false;
			}

			if (BindFlags & D3D11_BIND_CONSTANT_BUFFER)
			{
				//constant buffers needs discard
				if (Map != D3D11_MAP_WRITE_DISCARD)
					return false;
			}

			if (Usage == D3D11_USAGE_STAGING)
			{
				//Staging needs read, write or readwrite
				if (Map != D3D11_MAP_READ && Map != D3D11_MAP_WRITE && Map != D3D11_MAP_READ_WRITE)
					return false;
			}
		}
		else //Map == 0 (means can't map)
		{
			//Required to have a map flag
			if (CPUAccessFlags != 0)
				return false;
		}
	}

	if (Usage == D3D11_USAGE_IMMUTABLE || BindFlags & D3D11_BIND_DEPTH_STENCIL)
	{
		if (AccessMethod & (RAC_UPDATE_DEST | RAC_COPY_DEST | RAC_COPYREGION_DEST) )
			return false;
	}

	if (AccessMethod & RAC_RESOLVE_SRC)
	{
		//Resolve can't have DS source
		if (BindFlags & D3D11_BIND_DEPTH_STENCIL)
			return false;
	}

	if (AccessMethod & (RAC_RESOLVE_DEST | RAC_UPDATE_DEST) )
	{
		//Resolve and Update dest needs to be DEFAULT
		if (Usage != D3D11_USAGE_DEFAULT)
			return false;

		//Resolve and Update can't have DS dest
		if (BindFlags & D3D11_BIND_DEPTH_STENCIL)
			return false;
	}

	if (AccessMethod & (RAC_UPDATE_DEST) )
	{
		//Update dest can't be Staging
		if (Usage == D3D11_USAGE_STAGING)
			return false;
	}

	return true;
}

//	ResourceDimension 	// Resource type to get the value set
//	bValid 				// Return valid or invalid combinations
//	bGenerateMapCombos 	// Set to true to get the possible map values for that combination
//	pIncludedFlags		// Flags that must be in the combinations.  If you combine flags, it will add combinations 
//						// that uses either. For example, BIND_VB | BIND_IB will return all combinations with 
//						// either BIND_VB or BIND_IB or both. 
//	pExcludedFlags		// Flags that won’t be in the combinations.  If you combine flags, it will exlude combinations 
//						// that uses either. For example, BIND_VB | BIND_IB will return all combinations that excludes 
//						// both BIND_VB and BIND_IB. 
//	AccessMethod		// only return combinations that are bValid for that method
CD3DResourceFlagsValueSet::CD3DResourceFlagsValueSet( CTest *pTest, D3D11_RESOURCE_DIMENSION ResourceDimension, bool bValid, bool bGenerateMapCombos, D3DResourceFlags *pIncludedFlags, D3DResourceFlags *pExcludedFlags, RESOURCE_ACCESS_METHOD AccessMethod ) :
		m_ResourceDimension( ResourceDimension ),
		m_NumD3DResourceFlags( 0 ),
		m_bValid( bValid )
{
	m_AccessMethod = AccessMethod;
	m_bGenerateMapCombos = bGenerateMapCombos;
	m_pTest = pTest;

	if (!pIncludedFlags)
	{
		m_IncludedFlags.Usage = (D3D11_USAGE)DO_NOT_FILTER_FLAG;
		m_IncludedFlags.BindFlags = DO_NOT_FILTER_FLAG;
		m_IncludedFlags.CPUAccessFlags = DO_NOT_FILTER_FLAG;
		m_IncludedFlags.MiscFlags = DO_NOT_FILTER_FLAG;
		m_IncludedFlags.Map = (D3D11_MAP)DO_NOT_FILTER_FLAG;
	}
	else
	{
		m_IncludedFlags = *pIncludedFlags;
	}
	if (!pExcludedFlags)
	{
		m_ExcludedFlags.Usage = (D3D11_USAGE)DO_NOT_FILTER_FLAG;
		m_ExcludedFlags.BindFlags = DO_NOT_FILTER_FLAG;
		m_ExcludedFlags.CPUAccessFlags = DO_NOT_FILTER_FLAG;
		m_ExcludedFlags.MiscFlags = DO_NOT_FILTER_FLAG;
		m_ExcludedFlags.Map = (D3D11_MAP)DO_NOT_FILTER_FLAG;
	}
	else
	{
		m_ExcludedFlags = *pExcludedFlags;
	}

	GenerateD3DResourceFlags();

	m_pUsageValueSet = new CTableValueSet<D3D11_USAGE>( &m_ArrayD3DResourceFlags[0].Usage, sizeof( D3DResourceFlags ), m_NumD3DResourceFlags );
	m_pBindFlagsValueSet = new CTableValueSet<D3D11_BIND_FLAG>( (D3D11_BIND_FLAG*)&m_ArrayD3DResourceFlags[0].BindFlags, sizeof( D3DResourceFlags ), m_NumD3DResourceFlags );
	m_pCPUAccessFlagsValueSet = new CTableValueSet<D3D11_CPU_ACCESS_FLAG>( (D3D11_CPU_ACCESS_FLAG*)&m_ArrayD3DResourceFlags[0].CPUAccessFlags, sizeof( D3DResourceFlags ), m_NumD3DResourceFlags );
	m_pMiscFlagsValueSet = new CTableValueSet<D3D11_RESOURCE_MISC_FLAG>( (D3D11_RESOURCE_MISC_FLAG*)&m_ArrayD3DResourceFlags[0].MiscFlags, sizeof( D3DResourceFlags ), m_NumD3DResourceFlags );
	m_pMapValueSet = new CTableValueSet<D3D11_MAP>( &m_ArrayD3DResourceFlags[0].Map, sizeof( D3DResourceFlags ), m_NumD3DResourceFlags );
}

CD3DResourceFlagsValueSet::~CD3DResourceFlagsValueSet()
{
	//TODO: value sets should be deleted if CreateFactor never called.
	//delete m_pUsageValueSet;
	//delete m_pBindFlagsValueSet;
	//delete m_pCPUAccessFlagsValueSet;
	//delete m_pMiscFlagsValueSet;
	//delete m_pMapValueSet;
}

void CD3DResourceFlagsValueSet::GenerateD3DResourceFlags()
{
	FlagCombinations::CFlagCombinations::Flags bindFlags;
	if (m_IncludedFlags.BindFlags == 0)
		bindFlags[0] = tstring(_T(""));
	bindFlags[D3D11_BIND_VERTEX_BUFFER] = ToString(D3D11_BIND_VERTEX_BUFFER);
	bindFlags[D3D11_BIND_INDEX_BUFFER] = ToString(D3D11_BIND_INDEX_BUFFER);
	bindFlags[D3D11_BIND_CONSTANT_BUFFER] = ToString(D3D11_BIND_CONSTANT_BUFFER);
	bindFlags[D3D11_BIND_SHADER_RESOURCE] = ToString(D3D11_BIND_SHADER_RESOURCE);
	bindFlags[D3D11_BIND_STREAM_OUTPUT] = ToString(D3D11_BIND_STREAM_OUTPUT);
	bindFlags[D3D11_BIND_RENDER_TARGET] = ToString(D3D11_BIND_RENDER_TARGET);
	bindFlags[D3D11_BIND_DEPTH_STENCIL] = ToString(D3D11_BIND_DEPTH_STENCIL);

	FlagCombinations::CFlagCombinations bindFlagCombo;
	bindFlagCombo.Init(bindFlags, bindFlags.size());
	bindFlagCombo.Expand();

	FlagCombinations::CFlagCombinations::Flags miscFlags;
	miscFlags[0] = tstring(_T(""));
	miscFlags[D3D11_RESOURCE_MISC_GENERATE_MIPS] = ToString(D3D11_RESOURCE_MISC_GENERATE_MIPS);
	miscFlags[D3D11_RESOURCE_MISC_SHARED] = ToString(D3D11_RESOURCE_MISC_SHARED);
	miscFlags[D3D11_RESOURCE_MISC_TEXTURECUBE] = ToString(D3D11_RESOURCE_MISC_TEXTURECUBE);
	FlagCombinations::CFlagCombinations miscFlagCombo;
	miscFlagCombo.Init(miscFlags, miscFlags.size());
	miscFlagCombo.Expand();

	FlagCombinations::CFlagCombinations::Flags CPUAccessFlags;
	CPUAccessFlags[0] = tstring(_T(""));
	CPUAccessFlags[D3D11_CPU_ACCESS_WRITE] = ToString(D3D11_CPU_ACCESS_WRITE);
	CPUAccessFlags[D3D11_CPU_ACCESS_READ] = ToString(D3D11_CPU_ACCESS_READ);
	FlagCombinations::CFlagCombinations CPUAccessFlagCombo;
	CPUAccessFlagCombo.Init(CPUAccessFlags, CPUAccessFlags.size());
	CPUAccessFlagCombo.Expand();

	for (UINT Usage = D3D11_USAGE_DEFAULT; Usage <= D3D11_USAGE_STAGING; Usage++)
	{
		if( ( ( m_IncludedFlags.Usage != (D3D11_USAGE)DO_NOT_FILTER_FLAG ) && ( m_IncludedFlags.Usage != Usage ) ) ||
			( ( m_ExcludedFlags.Usage != (D3D11_USAGE)DO_NOT_FILTER_FLAG ) && ( m_ExcludedFlags.Usage == Usage ) ) )
			continue;
		for (UINT iBind=0; iBind<bindFlagCombo.Count(); iBind++)
		{
			UINT BindFlags = bindFlagCombo[iBind];
			if( ( ( m_IncludedFlags.BindFlags != DO_NOT_FILTER_FLAG ) && !( m_IncludedFlags.BindFlags & BindFlags ) ) ||
				( ( m_ExcludedFlags.BindFlags != DO_NOT_FILTER_FLAG ) && ( m_ExcludedFlags.BindFlags & BindFlags ) ) )
				continue;
			for (UINT iCPU=0; iCPU<CPUAccessFlagCombo.Count(); iCPU++)
			{
				UINT CPUAccessFlags = CPUAccessFlagCombo[iCPU];
				if( ( ( m_IncludedFlags.CPUAccessFlags != DO_NOT_FILTER_FLAG ) && !( m_IncludedFlags.CPUAccessFlags & CPUAccessFlags ) ) ||
					( ( m_ExcludedFlags.CPUAccessFlags != DO_NOT_FILTER_FLAG ) && ( m_ExcludedFlags.CPUAccessFlags & CPUAccessFlags ) ) )
					continue;
				for (UINT iMisc=0; iMisc<miscFlagCombo.Count(); iMisc++)
				{
					UINT MiscFlags = miscFlagCombo[iMisc];
					if( ( ( m_IncludedFlags.MiscFlags != DO_NOT_FILTER_FLAG ) && !( m_IncludedFlags.MiscFlags & MiscFlags ) ) ||
						( ( m_ExcludedFlags.MiscFlags != DO_NOT_FILTER_FLAG ) && ( m_ExcludedFlags.MiscFlags & MiscFlags ) ) )
						continue;
					for (UINT Map = (D3D11_MAP)0; Map <= D3D11_MAP_WRITE_NO_OVERWRITE; Map++)
					{
						if( ( ( m_IncludedFlags.Map != (D3D11_MAP)DO_NOT_FILTER_FLAG ) && ( m_IncludedFlags.Map != Map ) ) ||
							( ( m_ExcludedFlags.Map != (D3D11_MAP)DO_NOT_FILTER_FLAG ) && ( m_ExcludedFlags.Map == Map ) ) )
							continue;

						if (!m_bGenerateMapCombos && Map > 0)
							continue;

						if (m_bValid == ValidD3DResourceFlags(m_ResourceDimension, (D3D11_USAGE)Usage, BindFlags, CPUAccessFlags, MiscFlags, (D3D11_MAP)Map, m_AccessMethod, m_bGenerateMapCombos))
						{
							assert(m_NumD3DResourceFlags < 1024);

							D3DResourceFlags *pFlags = &m_ArrayD3DResourceFlags[m_NumD3DResourceFlags];
							pFlags->Usage = (D3D11_USAGE)Usage;
							pFlags->BindFlags = BindFlags;
							pFlags->CPUAccessFlags = CPUAccessFlags;
							pFlags->MiscFlags = MiscFlags;
							pFlags->Map = (D3D11_MAP)Map;

							m_NumD3DResourceFlags++;
						}
					}
				}
			}
		}
	}
}

testfactor::RFactor CD3DResourceFlagsValueSet::CreateFactor( CTestCaseParameter<D3D11_USAGE> *pUsageParam, CTestCaseParameter<D3D11_BIND_FLAG> *pBindFlagsParam, CTestCaseParameter<D3D11_CPU_ACCESS_FLAG> *pCPUAccessFlagsParam, CTestCaseParameter<D3D11_RESOURCE_MISC_FLAG> *pMiscFlagsParam, CTestCaseParameter<D3D11_MAP> *pMapParam )
{
	//TODO: if param is null, then don't create the factor and don't '%' it
	testfactor::RFactor rUsage = m_pTest->AddParameterValueSet<D3D11_USAGE>(pUsageParam, m_pUsageValueSet);
	testfactor::RFactor rBindFlags = m_pTest->AddParameterValueSet<D3D11_BIND_FLAG>(pBindFlagsParam, m_pBindFlagsValueSet);
	testfactor::RFactor rCPUAccessFlags = m_pTest->AddParameterValueSet<D3D11_CPU_ACCESS_FLAG>(pCPUAccessFlagsParam, m_pCPUAccessFlagsValueSet);
	testfactor::RFactor rMiscFlags = m_pTest->AddParameterValueSet<D3D11_RESOURCE_MISC_FLAG>(pMiscFlagsParam, m_pMiscFlagsValueSet);
	testfactor::RFactor rMap = m_pTest->AddParameterValueSet<D3D11_MAP>(pMapParam, m_pMapValueSet);

	testfactor::RFactor rf = rUsage % rBindFlags % rCPUAccessFlags % rMiscFlags % rMap;
	return rf;
}
