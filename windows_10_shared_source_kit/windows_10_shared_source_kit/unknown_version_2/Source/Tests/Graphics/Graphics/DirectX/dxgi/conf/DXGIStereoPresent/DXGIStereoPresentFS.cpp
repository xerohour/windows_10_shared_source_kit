#include "DXGIStereoPresent.h"

CDXGIStereoPresentFS::CDXGIStereoPresentFS() 
{
	IDXGIAdapter1 *pAdapter = NULL;
	IDXGIOutput *pOutput = NULL;

	m_bWindowed = false;
	m_bTestDirtyRects = false;

	if(m_pFactory == NULL)
	{
		goto Done;
	}

	if(FAILED(m_pFactory->EnumAdapters1(0, &pAdapter)))
	{
		goto Done;
	}

	if(FAILED(pAdapter->EnumOutputs(0, &pOutput)))
	{
		goto Done;
	}

	if(FAILED(pOutput->QueryInterface(__uuidof(IDXGIOutput1), (void**)&m_pOutput)))
	{
		goto Done;
	}

	DXGI_MODE_DESC1 *pModes = NULL;

	m_supportedModes.clear();

	for(UINT i = 0; i < NUM_FORMATS; ++i)
	{
		UINT numModes = 0;
		if(SUCCEEDED(m_pOutput->GetDisplayModeList1(g_pFormatsToTest[i], DXGI_ENUM_MODES_STEREO, &numModes, NULL)) && numModes > 0)
		{
			pModes = new DXGI_MODE_DESC1[numModes];

			if(SUCCEEDED(m_pOutput->GetDisplayModeList1(g_pFormatsToTest[i], DXGI_ENUM_MODES_STEREO, &numModes, pModes)))
			{
				UINT formatModesAdded = 0;
				for(UINT m = 0; m < numModes; ++m)
				{
					if(pModes[m].Stereo == TRUE && formatModesAdded < MAX_FORMATMODES)
					{
						// Make sure there is a corresponding mono mode as required
						DXGI_MODE_DESC1 matchedMode; 
						DXGI_MODE_DESC1 filterMode = pModes[m];
						filterMode.Stereo = FALSE;

						if(FAILED(m_pOutput->FindClosestMatchingMode1(&filterMode, &matchedMode, NULL)))
						{
							continue;
						}

						if(matchedMode != filterMode)
						{
							continue;
						}

						m_supportedModes.push_back(pModes[m]);
						formatModesAdded++;
					}
				}
			}
			
			delete [] pModes;
			pModes = NULL;
		}
	}

Done:
	SAFE_RELEASE(pOutput);
	SAFE_RELEASE(pAdapter);
}

HRESULT CDXGIStereoPresentFS::CreateStereoSwapChain()
{
	HRESULT hr = S_OK;

	DXGI_MODE_DESC1 modeDesc = m_supportedModes[m_currentMode];

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
    swapChainDesc.Width = modeDesc.Width;
	swapChainDesc.Height = modeDesc.Height;
	swapChainDesc.Format = modeDesc.Format;
	swapChainDesc.Stereo = modeDesc.Stereo;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_BACK_BUFFER;
	swapChainDesc.BufferCount = m_BufferCount;
	swapChainDesc.Scaling = DXGI_SCALING_NONE;
	swapChainDesc.SwapEffect = m_SwapEffect;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	
	DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreenDesc;
	fullscreenDesc.RefreshRate = modeDesc.RefreshRate;
	fullscreenDesc.ScanlineOrdering = modeDesc.ScanlineOrdering;
	fullscreenDesc.Scaling = modeDesc.Scaling;
    fullscreenDesc.Windowed = FALSE;

	if(FAILED(hr = m_pFactory->CreateSwapChainForHwnd(GetDevice(), m_hWnd, &swapChainDesc, &fullscreenDesc, NULL, &m_pSwapChain)))
	{
		WriteToLog(_T("CDXGIStereoPresentFS::CreateStereoSwapChain - CreateSwapChain1() failed, hr = %s"), D3DHResultToString(hr).c_str());
		goto Done;
	}

Done:

	return hr;
}

void CDXGIStereoPresentFS::InitTestParameters()
{
	testfactor::RFactor rfModeIndex = AddParameter<UINT>(_T("ModeIndex"), &m_currentMode, new CRangeValueSet<UINT>(0, m_supportedModes.size() - 1, 1));

	testfactor::RFactor rfSyncInterval = AddParameter<UINT>(_T("SyncInterval"), &m_SyncInterval, new CRangeValueSet<UINT>(0, 4, 1));

	CUserValueSet<UINT> *pPresentFlagValues = new CUserValueSet<UINT>();
	pPresentFlagValues->AddValue(0);
	pPresentFlagValues->AddValue(DXGI_PRESENT_STEREO_TEMPORARY_MONO);
	testfactor::RFactor rfPresentFlags = AddParameter<UINT>(_T("PresentFlags"), &m_PresentFlags, pPresentFlagValues);

	SetRootTestFactor(rfModeIndex * rfSyncInterval * rfPresentFlags);
}

TEST_RESULT CDXGIStereoPresentFS::Setup()
{
	if(m_supportedModes.size() == 0)
	{
		WriteToLog(_T("CDXGIStereoPresentFS::Setup - No supported stereo modes found, skipping test group"));
		return RESULT_SKIP;
	}

	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);

	if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2))
	{
		WriteToLog(_T("CDXGIStereoPresentFS::Setup - Stereo modes found on non Win8 OS, failing test group"));
		return RESULT_FAIL;
	}

	TEST_RESULT tr = CDXGIStereoPresent::Setup();
	if(tr != RESULT_PASS)
	{
		return tr;
	}

	// Fail if output failed to initialize during construction
	if(m_pOutput == NULL) 
	{
		WriteToLog(_T("Failed to initialize output during construction"));
		return RESULT_FAIL;
	}

	SAFE_RELEASE(m_pOutput);

	return RESULT_PASS;
}

TEST_RESULT CDXGIStereoPresentFS::ValidateResults()
{
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	IDXGIResource *pResource = NULL;
	BYTE *eyeData = NULL;
	FLOAT_COLOR *eyeDataFloat = NULL;

	GetEffectiveContext()->Flush();

	if(FAILED(hr = m_pStagingBB->QueryInterface(__uuidof(IDXGIResource), (void**)&pResource)))
	{
		WriteToLog(_T("CDXGIStereoPresentFS::ValidateResults - QueryInterface for IDXGIResource failed, hr=%s"), D3DHResultToString(hr).c_str());
		goto Fail;
	}

	if(FAILED(hr = m_pOutput->GetDisplaySurfaceData1(pResource)))
	{
		WriteToLog(_T("CDXGIStereoPresentFS::ValidateResults - GetDisplaySurfaceData1 failed, hr=%s"), D3DHResultToString(hr).c_str());
		goto Fail;
	}

	const UINT width = m_supportedModes[m_currentMode].Width;
	const UINT height = m_supportedModes[m_currentMode].Height;
	const UINT elementSize = GetBitsPerElement(m_supportedModes[m_currentMode].Format) / 8;
	const DXGIFormatInfo *pFormatInfo = CFormatInfoMap::Find(m_supportedModes[m_currentMode].Format);
	const float maxULP = 1.0f;

	float ulpDiff = 0.0f;
	eyeData = new BYTE[elementSize * width * height]; 
	eyeDataFloat = new FLOAT_COLOR[width * height];

	const UINT subresourcesToTest = (m_PresentFlags & DXGI_PRESENT_STEREO_TEMPORARY_MONO) ? 1 : 2;

	for(UINT i = 0; i < subresourcesToTest; ++i)
	{
		D3D11_MAPPED_SUBRESOURCE mappedSubres;
		if(FAILED(hr = GetEffectiveContext()->Map(m_pStagingBB, i, D3D11_MAP_READ, 0, &mappedSubres)))
		{
			WriteToLog(_T("CDXGIStereoPresentFS::ValidateResult - Failed to map expected result texture, hr=%s"), D3DHResultToString(hr).c_str());
			goto Fail;
		}

		BYTE *pData = (BYTE*)mappedSubres.pData;
		for(UINT y = 0; y < height; ++y)
		{
			memcpy(eyeData + y * elementSize * width, pData, elementSize * width); 

			pData += mappedSubres.RowPitch;
		}

		if(!CFormatHelper::ConvertPixels(CONV_FROM_FORMAT, m_supportedModes[m_currentMode].Format, eyeDataFloat, height, width, eyeData))
		{
			WriteToLog(_T("CDXGIStereoPresent::ValidateResult - Failed to convert expected data"));
			goto Fail;
		}

		for(UINT y = 0; y < height; ++y)
		{
			for(UINT x = 0; x < width; ++x)
			{
				for(UINT c = 0; c < 4; ++c)
				{
					if(!MinMaxRange::TestFloatsSame(g_pEyeColors[i].comp[c], eyeDataFloat[y * width + x].comp[c], pFormatInfo, c, maxULP, &ulpDiff))
					{
						WriteToLog(_T("CDXGIStereoPresent::ValidateResult - Pixel at (%i,%i) component %i of subresource %i failed, expected=%f, actual=%f"),
								x, y, c, i, g_pEyeColors[i].comp[c], eyeDataFloat[y * width + x].comp[c]);
						goto Fail;
					}
				}
			}

			pData += mappedSubres.RowPitch;
		}

		GetEffectiveContext()->Unmap(m_pStagingBB, i);
	}

	goto Done;

Fail:
	tr = RESULT_FAIL;

Done:
	SAFE_RELEASE(pResource);

	if(eyeData != NULL)
	{
		delete [] eyeData;
		eyeData = NULL;
	}

	if(eyeDataFloat != NULL)
	{
		delete [] eyeDataFloat;
		eyeDataFloat = NULL;
	}

	return tr;
}
