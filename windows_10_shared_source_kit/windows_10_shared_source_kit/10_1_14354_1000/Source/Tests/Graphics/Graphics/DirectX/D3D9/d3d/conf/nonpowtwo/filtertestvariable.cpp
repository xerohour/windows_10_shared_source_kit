#include "TestCaseStruct.h"
#include "TestVariable.h"

#ifdef FAST_TEST
D3DTEXTUREFILTERTYPE g_Filters[] = { D3DTEXF_NONE,
									 D3DTEXF_POINT,        
									 D3DTEXF_LINEAR,       
									 D3DTEXF_ANISOTROPIC,  
								   };
#else
D3DTEXTUREFILTERTYPE g_Filters[] = { D3DTEXF_NONE,
                                     D3DTEXF_POINT,
                                     D3DTEXF_LINEAR,
                                     D3DTEXF_ANISOTROPIC,
                                     D3DTEXF_PYRAMIDALQUAD,
                                     D3DTEXF_GAUSSIANQUAD
                                     };
#endif


DWORD g_MagFilterCaps[] = {	0xFFFFFFFF,                          //NONE filter is invalid for mag filter
							D3DPTFILTERCAPS_MAGFPOINT,        
							D3DPTFILTERCAPS_MAGFLINEAR,       
							D3DPTFILTERCAPS_MAGFANISOTROPIC,  
							D3DPTFILTERCAPS_MAGFPYRAMIDALQUAD,
							D3DPTFILTERCAPS_MAGFGAUSSIANQUAD
						  };

DWORD g_MinFilterCaps[] = { 0xFFFFFFFF,                          //NONE filter is invalid for mag filter
							D3DPTFILTERCAPS_MINFPOINT,        
							D3DPTFILTERCAPS_MINFLINEAR,       
							D3DPTFILTERCAPS_MINFANISOTROPIC,  
							D3DPTFILTERCAPS_MINFPYRAMIDALQUAD,
							D3DPTFILTERCAPS_MINFGAUSSIANQUAD
						  };

DWORD g_MipFilterCaps[] = {	0,
							D3DPTFILTERCAPS_MIPFPOINT,        
							D3DPTFILTERCAPS_MIPFLINEAR,       
						  };

#define FILTER_ARRAY_SIZE ( sizeof(g_Filters) / (sizeof(g_Filters[0])) )
#define SZFILTER_ARRAY_SIZE 8   //D3DTEXF_GAUSSIANQUAD = 7 in the highest index

SFilterCase *g_pFilterCases = NULL;

const TCHAR* g_szFilters[SZFILTER_ARRAY_SIZE];

#define ADD_FILTER(x) {g_szFilters[x] = _T(#x) + 8;} //8 is length of "D3DTEXF_"

//------------------------------------------------------------------------------

bool CTestVariableImpl<SFilterCase>::InitializeResources(CD3DTest* pTest)
{
	//initialize array with names of filters
	for (int i = 0; i < SZFILTER_ARRAY_SIZE; i++)
		g_szFilters[i] = "UNKNOWN";

	ADD_FILTER(D3DTEXF_NONE         );
	ADD_FILTER(D3DTEXF_POINT        );        
	ADD_FILTER(D3DTEXF_LINEAR       );       
	ADD_FILTER(D3DTEXF_ANISOTROPIC  );
	ADD_FILTER(D3DTEXF_PYRAMIDALQUAD);
	ADD_FILTER(D3DTEXF_GAUSSIANQUAD );
	
	
	UINT uFilterTestCaseArraySize = (FILTER_ARRAY_SIZE-1) * 3; 

	g_pFilterCases = new SFilterCase[uFilterTestCaseArraySize];

	ZeroMemory(g_pFilterCases, sizeof(SFilterCase) * uFilterTestCaseArraySize);

	for (UINT i = 1; i < FILTER_ARRAY_SIZE; i++)
	{
		for (UINT j = 0; j < 3; j ++)
			{
				g_pFilterCases[(i-1)*3 + j].MagFilter = g_Filters[i];
				g_pFilterCases[(i-1)*3 + j].MinFilter = g_Filters[i];
				g_pFilterCases[(i-1)*3 + j].MipFilter = g_Filters[j];
				
				g_pFilterCases[(i-1)*3 + j].dwRequiredCaps = g_MagFilterCaps[i] | g_MinFilterCaps[i] | g_MipFilterCaps[j];
			}
		}
	
	//set 3 combinations of mag/min/mip filters per each FilterTestCase. Since we use 6 texture then it'll cover all posible cases
	//NONE is not supported as Mag and Min Filters -> use (FILTER_ARRAY_SIZE-1)
	//use 3 as count for mip filter since NONE, POINT and LINEAR are the only supported values for MipFilter
	/*UINT uFilterTestCaseArraySize = (UINT) ceil( (float)(((FILTER_ARRAY_SIZE-1) * (FILTER_ARRAY_SIZE-1) * 3 ) / MAX_TEXTURE_COUNT) ); 

	g_pFilterCases = new SFilterCase[uFilterTestCaseArraySize];

	ZeroMemory(g_pFilterCases, sizeof(SFilterCase) * uFilterTestCaseArraySize);

	UINT index = 0;
	for (UINT iMag = 1; iMag < FILTER_ARRAY_SIZE; iMag++)
	{
		for (UINT iMin = 1; iMin < FILTER_ARRAY_SIZE; iMin++)
		{
			for (UINT iMip = 0; iMip < 3; iMip++)
			{
				UINT i = index/MAX_TEXTURE_COUNT;
				UINT j = index%MAX_TEXTURE_COUNT;

				if ( i >= uFilterTestCaseArraySize)
					break;

				g_pFilterCases[i].MagFilter[j] = g_Filters[iMag];
				g_pFilterCases[i].MinFilter[j] = g_Filters[iMin];
				g_pFilterCases[i].MipFilter[j] = g_Filters[iMip];
				
				g_pFilterCases[i].dwRequiredCaps[j] |= g_MagFilterCaps[iMag] | g_MinFilterCaps[iMin] | g_MipFilterCaps[iMip];

				index++;
			}
		}
	}

	//fill out last element in case if it didn't get filled out 
	for (UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (0 == g_pFilterCases[uFilterTestCaseArraySize-1].MagFilter[i])
		{
			g_pFilterCases[uFilterTestCaseArraySize-1].MagFilter[i] = g_Filters[1];
			g_pFilterCases[uFilterTestCaseArraySize-1].MinFilter[i] = g_Filters[1];
			g_pFilterCases[uFilterTestCaseArraySize-1].MipFilter[i] = g_Filters[0];
				
			g_pFilterCases[uFilterTestCaseArraySize-1].dwRequiredCaps[i] |= g_MagFilterCaps[1] | g_MinFilterCaps[1] | g_MipFilterCaps[0];
		}
	}*/

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SFilterCase>::CleanupResources()
{
	delete [] g_pFilterCases;
	g_pFilterCases = NULL;
}

//------------------------------------------------------------------------------

UINT CTestVariableImpl<SFilterCase>::GetUnfilteredDomainSize() const
{
	return (FILTER_ARRAY_SIZE-1) * 3;//(UINT) ceil( (float)(((FILTER_ARRAY_SIZE-1) * (FILTER_ARRAY_SIZE-1) * 3 ) / MAX_TEXTURE_COUNT) );
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SFilterCase>::GetVariation(SFilterCase* pOut, UINT nIndex) const 
{
	memcpy(pOut,&(g_pFilterCases[nIndex]), sizeof(SFilterCase));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SFilterCase>::CheckTestRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest)
{
	DEVICEDESC* DeviceCaps = pTest->m_pSrcDevice->GetCaps();

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[uPIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[uPIndex].TexIndices[i]] )
			continue;

		DWORD RequiredCaps = g_pFilterCases[uDIndex].dwRequiredCaps/*[i]*/;
		
		switch (g_TexInfo[g_TextureCases[uPIndex].TexIndices[i]].TexType)
		{
			case TEXTURE_2D:
			{
				//check if current mip filter is valid for current texture format 
				if (D3DTEXF_POINT < g_pFilterCases[uDIndex].MinFilter ||
					D3DTEXF_POINT < g_pFilterCases[uDIndex].MagFilter ||
					D3DTEXF_POINT < g_pFilterCases[uDIndex].MipFilter/*[i]*/)
				{
					int nAdapter = pTest->m_pSrcDevice->GetAdapterID();
					DISPLAYMODE Mode;
					FORMAT Format = g_TexInfo[g_TextureCases[uPIndex].TexIndices[i]].Format;

					if (FAILED(pTest->m_pD3D->GetAdapterDisplayMode( pTest->m_pSrcDevice->GetAdapterID(), &Mode )) ||
					    FAILED(pTest->m_pD3D->CheckDeviceFormat(nAdapter, pTest->m_pSrcDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_FILTER, RTYPE_TEXTURE, Format)))
						return false;
				}

				if (RequiredCaps != (RequiredCaps & DeviceCaps->dwTextureFilterCaps) )
					return false;

				break;
			}
			case TEXTURE_CUBE:
			{
				//REF doesn't support D3DTEXF_ANISOTROPIC, D3DTEXF_PYRAMIDALQUAD and D3DTEXF_GAUSSIANQUAD filters for cube maps (bug 87964)
				if (D3DTEXF_PYRAMIDALQUAD == g_pFilterCases[uDIndex].MinFilter ||
					D3DTEXF_GAUSSIANQUAD  == g_pFilterCases[uDIndex].MinFilter ||
					D3DTEXF_ANISOTROPIC   == g_pFilterCases[uDIndex].MinFilter  )
					return false;

				//check if current mip filter is valid for current texture format 
				if (D3DTEXF_POINT < g_pFilterCases[uDIndex].MinFilter ||
					D3DTEXF_POINT < g_pFilterCases[uDIndex].MagFilter ||
					D3DTEXF_POINT < g_pFilterCases[uDIndex].MipFilter/*[i]*/)
				{
					int nAdapter = pTest->m_pSrcDevice->GetAdapterID();
					DISPLAYMODE Mode;
					FORMAT Format = g_TexInfo[g_TextureCases[uPIndex].TexIndices[i]].Format;

					if (FAILED(pTest->m_pD3D->GetAdapterDisplayMode( pTest->m_pSrcDevice->GetAdapterID(), &Mode )) ||
					    FAILED(pTest->m_pD3D->CheckDeviceFormat(nAdapter, pTest->m_pSrcDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_FILTER, RTYPE_CUBETEXTURE, Format)))
						return false;
				}

				if (RequiredCaps != (RequiredCaps & DeviceCaps->dwCubeTextureFilterCaps) )
					return false;

				break;
			}
			case TEXTURE_3D:
			{
				//REF doesn't support D3DTEXF_ANISOTROPIC filters for volumes
				if ( D3DTEXF_ANISOTROPIC == g_pFilterCases[uDIndex].MinFilter )
					return false;

				//check if current mip filter is valid for current texture format 
				if (D3DTEXF_POINT < g_pFilterCases[uDIndex].MinFilter ||
					D3DTEXF_POINT < g_pFilterCases[uDIndex].MagFilter ||
					D3DTEXF_POINT < g_pFilterCases[uDIndex].MipFilter/*[i]*/)
				{
					int nAdapter = pTest->m_pSrcDevice->GetAdapterID();
					DISPLAYMODE Mode;
					FORMAT Format = g_TexInfo[g_TextureCases[uPIndex].TexIndices[i]].Format;
					
					if (FAILED(pTest->m_pD3D->GetAdapterDisplayMode( pTest->m_pSrcDevice->GetAdapterID(), &Mode )) ||
					    FAILED(pTest->m_pD3D->CheckDeviceFormat(nAdapter, pTest->m_pSrcDevice->GetDevType(), Mode.Format, D3DUSAGE_QUERY_FILTER, RTYPE_VOLUMETEXTURE, Format)))
						return false;
				}

				if (RequiredCaps != (RequiredCaps & DeviceCaps->dwVolumeTextureFilterCaps) )
					return false;

				break;
			}
			default:
				return true;
		}
	}

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SFilterCase>::ExpendTestStateName(TCHAR *testName)
{
	TCHAR tempName[256];

	strcpy(tempName, "Filters: ");
	
	/*for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;*/

		if(m_CurrentState.MagFilter/*[i]*/ < SZFILTER_ARRAY_SIZE)
		{
			_stprintf(tempName, _T("%s%s, "), tempName, g_szFilters[m_CurrentState.MagFilter/*[i]*/]);
		}
		else
		{
			strcat(tempName, "Unknown, ");
		}

		if(m_CurrentState.MinFilter/*[i]*/ < SZFILTER_ARRAY_SIZE)
		{
			_stprintf(tempName, _T("%s%s, "), tempName, g_szFilters[m_CurrentState.MinFilter/*[i]*/]);
		}
		else
		{
			strcat(tempName, "Unknown, ");
		}

		if(m_CurrentState.MipFilter/*[i]*/ < SZFILTER_ARRAY_SIZE)
		{
			_stprintf(tempName, _T("%s%s; "), tempName, g_szFilters[m_CurrentState.MipFilter/*[i]*/]);
		}
		else
		{
			strcat(tempName, "Unknown, ");
		}
	//}

	strncat(testName, tempName, NAME_SIZE-strlen(testName));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SFilterCase>::PutTestIntoState(CD3DTest* pTest)
{
	bool bResultVal = true;
	float fBias = 0.02f;

	// This allows command line control of the Bias amount
	pTest->ReadFloat("Rounding_Bias", &fBias);

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		DWORD uStage = g_TextureCases[g_uTexCaseIndex].StageIndices[i];

		//we don't want to duplicate the default value to avoid warning messages
		if (D3DTEXF_POINT != m_CurrentState.MagFilter)
			bResultVal &= pTest->SetSamplerState(uStage, SAMP_MAGFILTER, m_CurrentState.MagFilter/*[i]*/);
		if (D3DTEXF_POINT != m_CurrentState.MinFilter)
			bResultVal &= pTest->SetSamplerState(uStage, SAMP_MINFILTER, m_CurrentState.MinFilter/*[i]*/);
		if (D3DTEXF_NONE != m_CurrentState.MipFilter)
		{
			bResultVal &= pTest->SetSamplerState(uStage, SAMP_MIPFILTER, m_CurrentState.MipFilter/*[i]*/);

			// Point sampled filtering needs to be biased to allow for different texture coordinate rounding modes
			if ((m_CurrentState.MipFilter == D3DTEXF_POINT) && (pTest->m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS))
				bResultVal &= pTest->SetSamplerState(uStage, SAMP_MIPMAPLODBIAS, *((DWORD*)(&fBias)), CD3D_SRC);
		}


		//this is special case for following bug
		//1315933: Non Power 2 Conditional ConfTest false fails hardware that does not filter across cube map seams
		if ( g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].TexType			== TEXTURE_CUBE			&&
			 g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].uWidth			== 6					&&
			 g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].Format.d3dfFormat == FMT_A16B16G16R16F	&&
			 g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].uMipCount			== 1					&&
			 m_CurrentState.MagFilter == D3DTEXF_LINEAR )
		{
			bResultVal &= pTest->SetSamplerState( uStage, SAMP_MAGFILTER, D3DTEXF_POINT );
			bResultVal &= pTest->SetSamplerState( uStage, SAMP_MINFILTER, D3DTEXF_POINT );
			bResultVal &= pTest->SetSamplerState( uStage, SAMP_MIPFILTER, D3DTEXF_POINT );

			// Point sampled filtering needs to be biased to allow for different texture coordinate rounding modes
			if (pTest->m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_MIPMAPLODBIAS)
				bResultVal &= pTest->SetSamplerState(uStage, SAMP_MIPMAPLODBIAS, *((DWORD*)(&fBias)), CD3D_SRC);
		}
	}

	return bResultVal;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SFilterCase>::UpdateTestStatus(CD3DTest* pTest)
{
	/*for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;*/

		TCHAR szFilters[120];

		//DWORD uStage = g_TextureCases[g_uTexCaseIndex].StageIndices[i];

		szFilters[0] = '\0';//_stprintf(szFilters, "$gStage %u: ", uStage);

		_stprintf(szFilters, _T("%s$ymag %s, "), szFilters, g_szFilters[m_CurrentState.MagFilter/*[i]*/]);
		_stprintf(szFilters, _T("%smin %s, "),   szFilters, g_szFilters[m_CurrentState.MinFilter/*[i]*/]);
		_stprintf(szFilters, _T("%smip %s"),     szFilters, g_szFilters[m_CurrentState.MipFilter/*[i]*/]);
		
		pTest->WriteStatus(_T("$gFilters    "), szFilters);

		//set up default filter settings
		//pTest->SetSamplerState(uStage, SAMP_MAGFILTER, D3DTEXF_POINT);
		//pTest->SetSamplerState(uStage, SAMP_MINFILTER, D3DTEXF_POINT);
		//pTest->SetSamplerState(uStage, SAMP_MIPFILTER, D3DTEXF_NONE);
	//}

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		DWORD uStage = g_TextureCases[g_uTexCaseIndex].StageIndices[i];

		//set up default filter settings
		//we don't want to duplicate the default value to avoid warning messages
		if (D3DTEXF_POINT != m_CurrentState.MagFilter)
			pTest->SetSamplerState(uStage, SAMP_MAGFILTER, D3DTEXF_POINT);
		if (D3DTEXF_POINT != m_CurrentState.MinFilter)
			pTest->SetSamplerState(uStage, SAMP_MINFILTER, D3DTEXF_POINT);
		if (D3DTEXF_NONE != m_CurrentState.MipFilter)
			pTest->SetSamplerState(uStage, SAMP_MIPFILTER, D3DTEXF_NONE);
	}
}

