#include "TestCaseStruct.h"
#include "TestVariable.h"

#ifdef FAST_TEST
D3DTEXTUREADDRESS g_TexAddresses[] = { D3DTADDRESS_CLAMP,
                                       D3DTADDRESS_WRAP,
                                       D3DTADDRESS_BORDER,
};
#else
D3DTEXTUREADDRESS g_TexAddresses[] = { D3DTADDRESS_CLAMP,
									   D3DTADDRESS_WRAP,
									   D3DTADDRESS_MIRROR,
									   D3DTADDRESS_BORDER,
									   D3DTADDRESS_MIRRORONCE
									 };
#endif

DWORD g_TexAddressCaps[] = { D3DPTADDRESSCAPS_CLAMP,
                             D3DPTADDRESSCAPS_WRAP,
                             D3DPTADDRESSCAPS_MIRROR,
                             D3DPTADDRESSCAPS_BORDER,
                             D3DPTADDRESSCAPS_MIRRORONCE
						   };

#define TEXADDRESS_ARRAY_SIZE ( sizeof(g_TexAddresses) / (sizeof(g_TexAddresses[0])) )
#define SZTEXADDRESS_ARRAY_SIZE 6   //D3DTADDRESS_MIRRORONCE = 5 in the highest index

SAddressModeCase *g_pAddressModeCases = NULL; 

const TCHAR* g_szAddressModes[SZTEXADDRESS_ARRAY_SIZE];

#define ADD_MODE(x) {g_szAddressModes[x] = _T(#x) + 12;} //12 is length of "D3DTADDRESS_"

//------------------------------------------------------------------------------

bool CTestVariableImpl<SAddressModeCase>::InitializeResources(CD3DTest* pTest)
{
	//initialize array with names of address modes
	for (int i = 0; i < SZTEXADDRESS_ARRAY_SIZE; i++)
		g_szAddressModes[i] = "UNKNOWN";

	ADD_MODE(D3DTADDRESS_WRAP      );
	ADD_MODE(D3DTADDRESS_MIRROR    );
	ADD_MODE(D3DTADDRESS_CLAMP     );
	ADD_MODE(D3DTADDRESS_BORDER    );
	ADD_MODE(D3DTADDRESS_MIRRORONCE);	

	UINT uTextureAddressArraySize = TEXADDRESS_ARRAY_SIZE;//ceil( pow((float)TEXADDRESS_ARRAY_SIZE, 3.f) / MAX_TEXTURE_COUNT ); 

	g_pAddressModeCases = new SAddressModeCase[uTextureAddressArraySize];

	ZeroMemory(g_pAddressModeCases, sizeof(SAddressModeCase) * uTextureAddressArraySize);

	UINT index = 0;

	/*for (UINT iU = 0; iU < TEXADDRESS_ARRAY_SIZE; iU++)
	{
		for (UINT iV = 0; iV < TEXADDRESS_ARRAY_SIZE; iV++)
		{
			for (UINT iW = 0; iW < TEXADDRESS_ARRAY_SIZE; iW++)
			{
				if (index/MAX_TEXTURE_COUNT >= uTextureAddressArraySize)
					break;

				UINT i = index/MAX_TEXTURE_COUNT;
				UINT j = index%MAX_TEXTURE_COUNT;

				g_pAddressModeCases[i].AddressModeU[j] = g_TexAddresses[iU];
				g_pAddressModeCases[i].AddressModeV[j] = g_TexAddresses[iV];
				g_pAddressModeCases[i].AddressModeW[j] = g_TexAddresses[iW];
				
				g_pAddressModeCases[i].dwRequiredCapsUV[j] |= g_TexAddressCaps[iU] | g_TexAddressCaps[iV];
				g_pAddressModeCases[i].dwRequiredCapsW [j] |= g_TexAddressCaps[iW];

				index++;
			}
		}
	}*/

	for (UINT i = 0; i < TEXADDRESS_ARRAY_SIZE; i++)
	{
		for(int j = 0; j < MAX_TEXTURE_COUNT; j++)
		{
			g_pAddressModeCases[i].AddressModeU[j] = g_TexAddresses[i];
			g_pAddressModeCases[i].AddressModeV[j] = g_TexAddresses[i];
			g_pAddressModeCases[i].AddressModeW[j] = g_TexAddresses[i];
					
			g_pAddressModeCases[i].dwRequiredCapsUV[j] = g_TexAddressCaps[i];
			g_pAddressModeCases[i].dwRequiredCapsW [j] = g_TexAddressCaps[i];
		}
	
	}

    //fill out last element in case if it didn't get filled out 
	/*for (UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (0 == g_pAddressModeCases[uTextureAddressArraySize-1].AddressModeU[i])
		{
			g_pAddressModeCases[uTextureAddressArraySize-1].AddressModeU[i] = g_TexAddresses[0];
			g_pAddressModeCases[uTextureAddressArraySize-1].AddressModeV[i] = g_TexAddresses[0];
			g_pAddressModeCases[uTextureAddressArraySize-1].AddressModeW[index%MAX_TEXTURE_COUNT] = g_TexAddresses[0];
				
			g_pAddressModeCases[uTextureAddressArraySize-1].dwRequiredCapsUV[i] |= g_TexAddressCaps[0];
			g_pAddressModeCases[uTextureAddressArraySize-1].dwRequiredCapsW [i] |= g_TexAddressCaps[0];
		}
	}*/

	return true;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SAddressModeCase>::CleanupResources()
{
	delete [] g_pAddressModeCases;
	g_pAddressModeCases = NULL;
}

//------------------------------------------------------------------------------

UINT CTestVariableImpl<SAddressModeCase>::GetUnfilteredDomainSize() const
{
	return TEXADDRESS_ARRAY_SIZE;//ceil( pow((float)TEXADDRESS_ARRAY_SIZE, 3.f) / MAX_TEXTURE_COUNT );
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SAddressModeCase>::GetVariation(SAddressModeCase* pOut, const UINT nIndex) const
{
	memcpy(pOut,&(g_pAddressModeCases[nIndex]), sizeof(SAddressModeCase));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SAddressModeCase>::CheckTestRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest)
{
	DEVICEDESC* DeviceCaps = pTest->m_pSrcDevice->GetCaps();
	UINT count = 0; //count how many active stages we have, if 0 then skip test case (this will happen if only cube map is used)

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[uPIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[uPIndex].TexIndices[i]] )
			continue;

		DWORD RequiredCaps = g_pAddressModeCases[uDIndex].dwRequiredCapsUV[i];
		UINT  texIndex = g_TextureCases[uPIndex].TexIndices[i];
		
		switch (g_TexInfo[texIndex].TexType)
		{
			case TEXTURE_2D:
			{
				if ( RequiredCaps != (RequiredCaps & DeviceCaps->dwTextureAddressCaps) )
					return false;

				//if NonPow2Cond cap is set and we use nonPow2 single level 2D textures then no only CLAMP mode is valid
				if ((DeviceCaps->dwTextureCaps & D3DPTEXTURECAPS_NONPOW2CONDITIONAL) &&
					1 == g_TexInfo[g_TextureCases[uPIndex].TexIndices[i]].uMipCount  &&
					!(pTest->KeySet("POW2")) &&
					(g_pAddressModeCases[uDIndex].AddressModeU[i] != D3DTADDRESS_CLAMP ||
					 g_pAddressModeCases[uDIndex].AddressModeV[i] != D3DTADDRESS_CLAMP ))
				{
					return false;
				}

				if (pTest->m_pD3D->GetRuntimeVersion() >= 9.029f && 
					(g_pAddressModeCases[uDIndex].AddressModeU[i] != D3DTADDRESS_CLAMP ||
					 g_pAddressModeCases[uDIndex].AddressModeV[i] != D3DTADDRESS_CLAMP ) &&
					FAILED(pTest->m_pD3D->CheckDeviceFormat(pTest->m_pSrcDevice->GetAdapterID(), pTest->m_pSrcDevice->GetDevType(), pTest->m_pCurrentMode->pDisplay->Format,
													        USAGE_QUERY_WRAPANDMIP, RTYPE_TEXTURE, g_TexInfo[texIndex].Format)) )
					return false;

				count++;
				break;
			}
			case TEXTURE_CUBE: //address modes aren't used with cube maps
			{
				break;
			}
			case TEXTURE_3D:
			{
				RequiredCaps |= g_pAddressModeCases[uDIndex].dwRequiredCapsW[i];

				if ( RequiredCaps != (RequiredCaps & DeviceCaps->dwVolumeTextureAddressCaps) )
					return false;

				if (pTest->m_pD3D->GetRuntimeVersion() >= 9.029f && 
					(g_pAddressModeCases[uDIndex].AddressModeU[i] != D3DTADDRESS_CLAMP ||
					 g_pAddressModeCases[uDIndex].AddressModeV[i] != D3DTADDRESS_CLAMP ||
					 g_pAddressModeCases[uDIndex].AddressModeW[i] != D3DTADDRESS_CLAMP  ) &&
					FAILED(pTest->m_pD3D->CheckDeviceFormat(pTest->m_pSrcDevice->GetAdapterID(), pTest->m_pSrcDevice->GetDevType(), pTest->m_pCurrentMode->pDisplay->Format,
													        USAGE_QUERY_WRAPANDMIP, RTYPE_VOLUMETEXTURE, g_TexInfo[texIndex].Format)) )
					return false;

				count++;
				break;
			}
			default: 
				return false;
		}
	}

	if (count > 0 )
		return true;
	else
		return false;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SAddressModeCase>::ExpendTestStateName(TCHAR *testName)
{
	TCHAR tempName[256];

	strcpy(tempName, _T("AddressModes: "));
	
	/*for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		if (TEXTURE_CUBE == g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].TexType)
			continue;*/

		_stprintf(tempName, _T("%su %s, "), tempName, g_szAddressModes[m_CurrentState.AddressModeU[0/*i*/]]);

		_stprintf(tempName, _T("%sv %s"), tempName, g_szAddressModes[m_CurrentState.AddressModeV[0/*i*/]]);

		//if (TEXTURE_3D == g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].TexType)
		//{
			_stprintf(tempName, _T("%s, w %s"), tempName, g_szAddressModes[m_CurrentState.AddressModeW[0/*i]*/]]);
		//}

		strcat(tempName, _T("; "));
	//}

	strncat(testName, tempName, NAME_SIZE-strlen(testName));
}

//------------------------------------------------------------------------------

bool CTestVariableImpl<SAddressModeCase>::PutTestIntoState(CD3DTest* pTest)
{
	bool bResult = true;

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		DWORD index = g_TextureCases[g_uTexCaseIndex].StageIndices[i];

		//address modes aren't used with cube maps -> we won't set them
		if (TEXTURE_CUBE == g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].TexType)
			continue;

		//we don't want to duplicate the default value to avoid warning messages
		if ( D3DTADDRESS_CLAMP != m_CurrentState.AddressModeU[i])
			bResult &= pTest->SetSamplerState(index, SAMP_ADDRESSU, m_CurrentState.AddressModeU[i]);
		if ( D3DTADDRESS_CLAMP != m_CurrentState.AddressModeV[i])
			bResult &= pTest->SetSamplerState(index, SAMP_ADDRESSV, m_CurrentState.AddressModeV[i]);

		//for volumetextures set w address mode
		if (TEXTURE_3D == g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].TexType &&
			D3DTADDRESS_CLAMP != m_CurrentState.AddressModeW[i])
			bResult &= pTest->SetSamplerState(index, SAMP_ADDRESSW, m_CurrentState.AddressModeW[i]);
				
		if((D3DTADDRESS_BORDER == m_CurrentState.AddressModeU[i]) || 
		   (D3DTADDRESS_BORDER == m_CurrentState.AddressModeV[i]) ||
		   (D3DTADDRESS_BORDER == m_CurrentState.AddressModeW[i])  )
		{
			FORMAT Format = g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].Format;
			D3DCOLOR color = RGBA_MAKE(pTest->m_uTestNumber%255, 20*i, 100, 100);

			//bug 88079
			/*if ( Format.d3dfFormat == FMT_A8 )
				color = RGBA_MAKE( 0, 0, 0, pTest->m_uTestNumber%255);
			else if ( Format.d3dfFormat == FMT_R16F || Format.d3dfFormat == FMT_R32F )
				color = RGBA_MAKE( pTest->m_uTestNumber%255, 255, 255, 255 );
			else if ( Format.d3dfFormat == FMT_G16R16F || Format.d3dfFormat == FMT_G32R32F || Format.d3dfFormat == FMT_G16R16 )
				color = RGBA_MAKE( pTest->m_uTestNumber%255, 20*i, 255, 255 );
			else if ( Format.d3dfFormat == FMT_L8 || Format.d3dfFormat == FMT_L16 || Format.d3dfFormat == FMT_A4L4 || Format.d3dfFormat == FMT_A8L8 )
				color = RGBA_MAKE( 255, 255, 255, 255 );*/
			if ( Format.d3dfFormat == FMT_A8 )
				color = RGBA_MAKE( 0, 0, 0, 0);
			if ( Format.d3dfFormat == FMT_R16F || Format.d3dfFormat == FMT_R32F ||
				Format.d3dfFormat == FMT_G16R16F || Format.d3dfFormat == FMT_G32R32F || Format.d3dfFormat == FMT_G16R16 || Format.d3dfFormat == FMT_A16B16G16R16 ||
				Format.d3dfFormat == FMT_L8 || Format.d3dfFormat == FMT_L16 || Format.d3dfFormat == FMT_A4L4 || Format.d3dfFormat == FMT_A8L8 )
				color = RGBA_MAKE( 255, 255, 255, 255 );

			bResult &= pTest->SetSamplerState(index, SAMP_BORDERCOLOR, color);
		}
	}

	return bResult;
}

//------------------------------------------------------------------------------

void CTestVariableImpl<SAddressModeCase>::UpdateTestStatus(CD3DTest* pTest)
{
	TCHAR szModes[120];

	for(UINT i = 0; i < MAX_TEXTURE_COUNT; i++)
	{
		if (  -1 == g_TextureCases[g_uTexCaseIndex].TexIndices[i]             || 
			NULL == g_pTextures[g_TextureCases[g_uTexCaseIndex].TexIndices[i]] )
			continue;

		if (TEXTURE_CUBE == g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].TexType)
			continue;

		TCHAR szKey[256];
		ZeroMemory(szKey, sizeof(szKey));

		DWORD index = g_TextureCases[g_uTexCaseIndex].StageIndices[i];

		_stprintf(szKey, _T("$gAddressModes"));
		_stprintf(szModes, _T("$gStage %u: "), index);
		
		_stprintf(szModes, _T("%s$yu %s"), szModes, g_szAddressModes[m_CurrentState.AddressModeU[i]]);

		_stprintf(szModes, _T("%s, $yv %s"), szModes, g_szAddressModes[m_CurrentState.AddressModeV[i]]);

		if (TEXTURE_3D == g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].TexType)
		{
			_stprintf(szModes, _T("%s, $yw %s"), szModes, g_szAddressModes[m_CurrentState.AddressModeW[i]]);
		}

		pTest->WriteStatus(szKey, _T("%s"), szModes);

		//set default mode
		//we don't want to duplicate the default value to avoid warning messages
		if ( D3DTADDRESS_CLAMP != m_CurrentState.AddressModeU[i] )
			pTest->SetSamplerState(index, SAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		if ( D3DTADDRESS_CLAMP != m_CurrentState.AddressModeV[i] )
			pTest->SetSamplerState(index, SAMP_ADDRESSV, D3DTADDRESS_CLAMP);
		if ( TEXTURE_3D == g_TexInfo[g_TextureCases[g_uTexCaseIndex].TexIndices[i]].TexType &&
			 D3DTADDRESS_CLAMP != m_CurrentState.AddressModeW[i] )
			pTest->SetSamplerState(index, SAMP_ADDRESSW, D3DTADDRESS_CLAMP);
	}
}
