#include "DXGIStereoPresent.h"
#include "DirectX.h"

BEGIN_NAMED_VALUES(DIRTY_RECT_TEST)
	NAMED_VALUE(_T("TopLeft"), DIRTY_RECT_TOP_LEFT)
	NAMED_VALUE(_T("BottomRight"), DIRTY_RECT_BOTTOM_RIGHT)
	NAMED_VALUE(_T("Internal"), DIRTY_RECT_INTERNAL)
	NAMED_VALUE(_T("Pixel"), DIRTY_RECT_PIXEL)
	NAMED_VALUE(_T("Full"), DIRTY_RECT_FULL)
	NAMED_VALUE(_T("Multiple"), DIRTY_RECT_MULTIPLE)
	NAMED_VALUE(_T("ScrollBottomRight"), DIRTY_RECT_SCROLL_BOTTOM_RIGHT)
	NAMED_VALUE(_T("ScrollTopLeft"), DIRTY_RECT_SCROLL_TOP_LEFT)
END_NAMED_VALUES(DIRTY_RECT_TEST)

CDXGIStereoPresentWin::CDXGIStereoPresentWin()  
{
	IDXGIAdapter1 *pAdapter = NULL;
	IDXGIOutput *pOutput = NULL;

	m_bWindowed = true;

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

void CDXGIStereoPresentWin::InitTestParameters()
{
	testfactor::RFactor rfSyncInterval = AddParameter<UINT>(_T("SyncInterval"), &m_SyncInterval, new CRangeValueSet<UINT>(0, 4, 1));

	CUserValueSet<UINT> *pPresentFlagValues = new CUserValueSet<UINT>();
	pPresentFlagValues->AddValue(0);
	pPresentFlagValues->AddValue(DXGI_PRESENT_STEREO_TEMPORARY_MONO);
	testfactor::RFactor rfPresentFlags = AddParameter<UINT>(_T("PresentFlags"), &m_PresentFlags, pPresentFlagValues);

	testfactor::RFactor rfTestDirtyRectsT = AddParameter<BOOL>(_T("TestDirtyRects"), &m_bTestDirtyRects, TRUE);
	testfactor::RFactor rfTestDirtyRectsF = AddParameterValue<BOOL>(_T("TestDirtyRects"), FALSE);

	CUserValueSet<DIRTY_RECT_TEST> *pDirtyRectTestValues = new CUserValueSet<DIRTY_RECT_TEST>();
	pDirtyRectTestValues->AddValue(DIRTY_RECT_TOP_LEFT);
	pDirtyRectTestValues->AddValue(DIRTY_RECT_BOTTOM_RIGHT);
	pDirtyRectTestValues->AddValue(DIRTY_RECT_INTERNAL);
	pDirtyRectTestValues->AddValue(DIRTY_RECT_PIXEL);
	pDirtyRectTestValues->AddValue(DIRTY_RECT_FULL);
	pDirtyRectTestValues->AddValue(DIRTY_RECT_MULTIPLE);
	pDirtyRectTestValues->AddValue(DIRTY_RECT_SCROLL_BOTTOM_RIGHT);
	pDirtyRectTestValues->AddValue(DIRTY_RECT_SCROLL_TOP_LEFT);
	testfactor::RFactor rfDirtyRectTest = AddParameter<DIRTY_RECT_TEST>(_T("DirtyRectTest"), &m_DirtyRectTest, pDirtyRectTestValues);

	SetRootTestFactor(
		rfSyncInterval * rfPresentFlags * rfTestDirtyRectsT * rfDirtyRectTest +
		rfSyncInterval * rfPresentFlags * rfTestDirtyRectsF 
	);
}

TEST_RESULT CDXGIStereoPresentWin::Setup()
{
	TEST_RESULT tr = CDXGIStereoPresent::Setup();
	if(tr != RESULT_PASS)
	{
		return tr;
	}

	OSVERSIONINFO versionInfo;
	versionInfo.dwOSVersionInfoSize = sizeof(versionInfo);
	GetVersionEx(&versionInfo);

	// if stereo capable hardware isnt available, skip
	IDXGIFactory2 *pFactory2 = GetFramework()->GetDXGIFactory2();
	if( pFactory2->IsWindowedStereoEnabled() == FALSE )
	{
		WriteToLog(_T("CDXGIStereoPresentWin::Setup - Windowed stereo is not available, skipping test group."));
		return RESULT_SKIP;
	}
	else if(versionInfo.dwMajorVersion < 6 || (versionInfo.dwMajorVersion == 6 && versionInfo.dwMinorVersion < 2))
	{
		WriteToLog(_T("CDXGIStereoPresentWin::Setup - IsWindowedStereoEnabled() returned TRUE on non Win8 OS, failing test group"));
		return RESULT_FAIL;
	}

	if(m_supportedModes.size() == 0)
	{
		WriteToLog(_T("CDXGIStereoPresentWin::Setup - No supported stereo modes found, skipping test group"));
		return RESULT_SKIP;
	}

	m_currentMode = 0;

	return RESULT_PASS;
}

HRESULT CDXGIStereoPresentWin::CreateStereoSwapChain()
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
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = m_SwapEffect;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	if(m_bTestDComp)
	{
		if(FAILED(hr = m_pFactory->CreateSwapChainForComposition(GetDevice(), &swapChainDesc, NULL, &m_pSwapChain)))
		{
			WriteToLog(_T("CDXGIStereoPresentWin::CreateStereoSwapChain - CreateSwapChainForComposition() failed, hr = %s"), D3DHResultToString(hr).c_str());
			goto Done;
		}

		if (FAILED(hr = m_DCompIsolation.CreateAndAssociateDCOMPVisual(m_pSwapChain, m_hWnd)))
		{
			WriteToLog(_T("CDXGIStereoPresentWin::CreateStereoSwapChain - CreateAndAssociateDCOMPVisual() failed, hr = %s"), D3DHResultToString(hr).c_str());
			goto Done;
		}
	}
	else
	{
		if(FAILED(hr = m_pFactory->CreateSwapChainForHwnd(GetDevice(), m_hWnd, &swapChainDesc, NULL, NULL, &m_pSwapChain)))
		{
			WriteToLog(_T("CDXGIStereoPresentWin::CreateStereoSwapChain - CreateSwapChainForHwnd() failed, hr = %s"), D3DHResultToString(hr).c_str());
			goto Done;
		}
	}

Done:

	return hr;
}

TEST_RESULT CDXGIStereoPresentWin::ValidateResults() 
{ 
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	BYTE *eyeData = NULL;
	FLOAT_COLOR *eyeDataFloat = NULL;

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
		m_frame = m_captureAtlas[i]->CreateCompatibleFrame(width, height, true);
		if(!m_captureAtlas[i]->CopySubtexture(m_frame, m_framesCaptured - 2))
		{
			WriteToLog(_T("CDXGIStereoPresentWin::ValidateResult - Failed to copy from atlas texture"));
			goto Fail;
		}

		// get frame's immediate context from it's seperate device
		ID3D11Device *pDevice = NULL;
		ID3D11DeviceContext *pSlicerIC = NULL;
		m_frame->GetTexture()->GetDevice(&pDevice);
		pDevice->GetImmediateContext(&pSlicerIC);

		// leave commented out in case we need to debug slicer results
		//m_frame->Save(i == 0 ? L"left.png" : L"right.png");

		D3D11_TEXTURE2D_DESC desc;
		m_frame->GetTexture()->GetDesc(&desc);

		if(m_frame.Get()->GetResource() == NULL)
		{
			WriteToLog(_T("CDXGIStereoPresentWin::ValidateResult - Texture associated with copied capture data is NULL"));
			goto Fail;
		}

		D3D11_MAPPED_SUBRESOURCE mappedSubres;
		if(FAILED(hr = pSlicerIC->Map(m_frame->GetTexture(), 0, D3D11_MAP_READ, 0, &mappedSubres)))
		{
			WriteToLog(_T("CDXGIStereoPresentWin::ValidateResult - Failed to map expected result texture, hr=%s"), D3DHResultToString(hr).c_str());
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
			WriteToLog(_T("CDXGIStereoPresentWin::ValidateResult - Failed to convert expected data"));
			goto Fail;
		}

		RECT scrollRect = m_Rects[0];
		scrollRect.left -= m_ScrollOffset.x;
		scrollRect.right -= m_ScrollOffset.x;
		scrollRect.top -= m_ScrollOffset.y;
		scrollRect.bottom -= m_ScrollOffset.y;
		
		for(UINT y = 0; y < height; ++y)
		{
			for(UINT x = 0; x < width; ++x)
			{
				for(UINT c = 0; c < 4; ++c)
				{
					if(m_bTestDirtyRects)
					{
						bool bInRect = false;

						if(m_DirtyRectTest == DIRTY_RECT_SCROLL_BOTTOM_RIGHT || m_DirtyRectTest == DIRTY_RECT_SCROLL_TOP_LEFT)
						{
							if((int)x >= scrollRect.left && (int)x < scrollRect.right && (int)y >= scrollRect.top && (int)y < scrollRect.bottom)
							{
								bInRect = true;	
							}

							// invert rect check since area outside rect should be updated for scroll
							bInRect = !bInRect;
						}
						else
						{
							for(UINT r = 0; r < m_PresentParams.DirtyRectsCount; ++r)
							{
								if((int)x >= m_Rects[r].left && (int)x < m_Rects[r].right && (int)y >= m_Rects[r].top && (int)y < m_Rects[r].bottom)
								{
									bInRect = true;	
								}
							}
						}

						if(bInRect)
						{
							// pixel should have dirty rect values
							if(!MinMaxRange::TestFloatsSame(g_pEyeColors[i + 3].comp[c], eyeDataFloat[y * width + x].comp[c], pFormatInfo, c, maxULP, &ulpDiff))
							{
								WriteToLog(_T("CDXGIStereoPresentWin::ValidateResult - Pixel (inside dirty rect) at (%i,%i) component %i of subresource %i failed, expected=%f, actual=%f"),
										x, y, c, i, g_pEyeColors[i + 3].comp[c], eyeDataFloat[y * width + x].comp[c]);
								goto Fail;
							}
						}
						else
						{
							// pixel should have original present values
							if(!MinMaxRange::TestFloatsSame(g_pEyeColors[i].comp[c], eyeDataFloat[y * width + x].comp[c], pFormatInfo, c, maxULP, &ulpDiff))
							{
								WriteToLog(_T("CDXGIStereoPresentWin::ValidateResult - Pixel (outside dirty rect) at (%i,%i) component %i of subresource %i failed, expected=%f, actual=%f"),
										x, y, c, i, g_pEyeColors[i].comp[c], eyeDataFloat[y * width + x].comp[c]);
								goto Fail;
							}
						}
					}
					else if(!MinMaxRange::TestFloatsSame(g_pEyeColors[i].comp[c], eyeDataFloat[y * width + x].comp[c], pFormatInfo, c, maxULP, &ulpDiff))
					{
						WriteToLog(_T("CDXGIStereoPresentWin::ValidateResult - Pixel at (%i,%i) component %i of subresource %i failed, expected=%f, actual=%f"),
								x, y, c, i, g_pEyeColors[i].comp[c], eyeDataFloat[y * width + x].comp[c]);
						goto Fail;
					}
				}
			}

			pData += mappedSubres.RowPitch;
		}

		pSlicerIC->Unmap(m_frame->GetTexture(), 0);
	}

	goto Done;

Fail:
	tr = RESULT_FAIL;

Done:
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
