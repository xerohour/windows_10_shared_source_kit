#include "Filter.h"

#define D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT (D3D10_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT)
#define MAX_DEVICES (32)

static const c_ValidD3DCreateFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_SWITCH_TO_REF | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;

static
HRESULT
CreateTempDevice(ID3D11Device* pExisting, ID3D11Device** pNewDevice)
{
	HRESULT hr = E_UNEXPECTED;
	*pNewDevice = NULL;
	IDXGIDevice* pDXGI = NULL;
	IDXGIAdapter* pAdapter = NULL;

	hr = pExisting->QueryInterface(__uuidof(IDXGIDevice), (void**)(&pDXGI));
	if (FAILED(hr) || pDXGI == NULL)
		goto func_end;
	hr = pDXGI->GetAdapter(&pAdapter);
	const UINT createFlags = c_ValidD3DCreateFlags & pExisting->GetCreationFlags();
	ID3D11DeviceContext *pDevContext = NULL;
	g_App.CreateDevice(pAdapter, g_App.m_D3DOptions.DriverType, createFlags, g_App.m_D3DOptions.FeatureLevel, pNewDevice, &pDevContext);
	SAFE_RELEASE(pDevContext);

func_end:
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	return hr;
}

static bool isComparisonFilter(D3D11_FILTER filter)
{
	switch(filter)
	{
	default:
		return false;
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
	case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
	case D3D11_FILTER_COMPARISON_ANISOTROPIC:
		return true;
	}
}

static D3D11_FILTER NextFilterValue(D3D11_FILTER input, BOOL bIncludeMinMax)
{
	if( !bIncludeMinMax )
	{
		switch (input)
		{
		default:
			assert (!"Unexpected Filter Value");
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_ANISOTROPIC;
		case D3D11_FILTER_ANISOTROPIC:
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_ANISOTROPIC;
		case D3D11_FILTER_COMPARISON_ANISOTROPIC:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
	}
	else
	{
		switch (input)
		{
		default:
			assert (!"Unexpected Filter Value");
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_ANISOTROPIC;
		case D3D11_FILTER_ANISOTROPIC:
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_COMPARISON_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_COMPARISON_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_COMPARISON_ANISOTROPIC;
		case D3D11_FILTER_COMPARISON_ANISOTROPIC:
			return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MINIMUM_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_MINIMUM_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MINIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MINIMUM_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_MINIMUM_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_MINIMUM_ANISOTROPIC;
		case D3D11_FILTER_MINIMUM_ANISOTROPIC:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT;
		case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MAXIMUM_MIN_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR;
		case D3D11_FILTER_MAXIMUM_MIN_POINT_MAG_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT;
		case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR;
		case D3D11_FILTER_MAXIMUM_MIN_LINEAR_MAG_POINT_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT;
		case D3D11_FILTER_MAXIMUM_MIN_MAG_LINEAR_MIP_POINT:
			return D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR;
		case D3D11_FILTER_MAXIMUM_MIN_MAG_MIP_LINEAR:
			return D3D11_FILTER_MAXIMUM_ANISOTROPIC;
		case D3D11_FILTER_MAXIMUM_ANISOTROPIC:
			return D3D11_FILTER_MIN_MAG_MIP_POINT;
		}
	}
}

TEST_RESULT
ID3D11SamplerStateTest::Setup()
{
	m_pDevice = g_App.GetDevice();
	return RESULT_PASS;
}

void
ID3D11SamplerStateTest::Cleanup()
{

}

void 
ID3D11SamplerStateTest::InitTestParameters()
{
	int nPrimitiveW = g_App.Width();
	int nPrimitiveH = g_App.Height();

	g_App.m_RenderTargetOptions.SwapChainDesc.Width = nPrimitiveW*2;
	g_App.m_RenderTargetOptions.SwapChainDesc.Height = nPrimitiveH;
	g_App.m_RenderTargetOptions.DepthTextureDesc.Width = nPrimitiveW*2;
	g_App.m_RenderTargetOptions.DepthTextureDesc.Height = nPrimitiveH;
	g_App.m_RenderTargetOptions.NoDepthStencils = true;

	AddParameter( _T("NumDevices"), &m_numDevices );
	testfactor::RFactor rNumDevices = AddParameterValue( _T("NumDevices"), 16 );

	SetRootTestFactor(rNumDevices);
}

TEST_RESULT
ID3D11SamplerStateTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = E_UNEXPECTED;

	assert(m_numDevices <= MAX_DEVICES);

	ID3D11SamplerState** pStates[MAX_DEVICES] = {NULL};
	ID3D11Device* pDevices[MAX_DEVICES] = {NULL};
	D3D11_SAMPLER_DESC defaultStateDesc, stateDesc;
	ZeroStructMem(defaultStateDesc);
	// defaults first
	defaultStateDesc.AddressU = D3D11_TEXTURE_ADDRESS_CLAMP;
	defaultStateDesc.AddressV = D3D11_TEXTURE_ADDRESS_CLAMP;
	defaultStateDesc.AddressW = D3D11_TEXTURE_ADDRESS_CLAMP;
	defaultStateDesc.BorderColor[0] = 1.0f;
	defaultStateDesc.BorderColor[1] = 1.0f;
	defaultStateDesc.BorderColor[2] = 1.0f;
	defaultStateDesc.BorderColor[3] = 1.0f;
	defaultStateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
	defaultStateDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	defaultStateDesc.MaxAnisotropy = 1;
	defaultStateDesc.MaxLOD = FLT_MAX;
	defaultStateDesc.MinLOD = -FLT_MAX;
	defaultStateDesc.MipLODBias = 0;
	// then incremental values
	stateDesc = defaultStateDesc;

	for (int nDevice = 0; nDevice < m_numDevices; ++nDevice)
	{
		hr = CreateTempDevice(m_pDevice, &pDevices[nDevice]);
		if (FAILED(hr))
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Failed to create another device. device#=%d, hr=%s\n"), nDevice+1, D3DHResultToString(hr).c_str());
			goto testDone;
		}
		D3D11_FEATURE_DATA_D3D11_OPTIONS1 d3d11Options1;
		BOOL bSupportsMinMaxFiltering = FALSE;

		if (g_App.GetDriverVersion() >= KMT_DRIVERVERSION_WDDM_1_3)
		{
			if( FAILED( hr = m_pDevice->CheckFeatureSupport( D3D11_FEATURE_D3D11_OPTIONS1, &d3d11Options1, sizeof(d3d11Options1) ) ) )
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T( "CheckFeatureSupport() failed. HR = %s." ), D3DHResultToString(hr).c_str());
				goto testDone;
			}
			bSupportsMinMaxFiltering = d3d11Options1.MinMaxFiltering;
		}

		pStates[nDevice] = new ID3D11SamplerState*[D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT+1];
		ZeroMemory(pStates[nDevice], sizeof(ID3D11SamplerState*)*(D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT+1));
		hr = pDevices[nDevice]->CreateSamplerState(&defaultStateDesc, &pStates[nDevice][0]);
		//for (UINT n = 1; n <= D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT; ++n)
		for (UINT n = 1; n < D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT; ++n)
		{
			(UINT32&)(stateDesc.AddressU) += 1;
			if (stateDesc.AddressU > D3D11_TEXTURE_ADDRESS_MIRROR_ONCE)
			{
				stateDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
				(UINT32&)(stateDesc.AddressV) += 1;
				if (stateDesc.AddressV > D3D11_TEXTURE_ADDRESS_MIRROR_ONCE)
				{
					stateDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
					(UINT32&)(stateDesc.AddressW) += 1;
					if (stateDesc.AddressW > D3D11_TEXTURE_ADDRESS_MIRROR_ONCE)
					{
						stateDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
						stateDesc.Filter = NextFilterValue(stateDesc.Filter,bSupportsMinMaxFiltering);
						if (stateDesc.Filter == D3D11_FILTER_MIN_MAG_MIP_POINT)
						{
							(UINT32&)(stateDesc.MaxAnisotropy) += 1;
							if (stateDesc.MaxAnisotropy > 16)
							{
								stateDesc.MaxAnisotropy = 1;
								if (isComparisonFilter(stateDesc.Filter))
								{
									stateDesc.MaxLOD = FLT_MAX;
									(UINT32&)(stateDesc.ComparisonFunc) += 1;
									if (stateDesc.ComparisonFunc > D3D11_COMPARISON_ALWAYS)
									{
										stateDesc.ComparisonFunc = D3D11_COMPARISON_NEVER;
									}
								}
								else
								{
									stateDesc.MaxLOD /= 2;
									if (stateDesc.MaxLOD < 0.5)
									{
										stateDesc.MaxLOD = FLT_MAX;
									}
								}
							}
						}
					}
				}
			}
			hr = pDevices[nDevice]->CreateSamplerState(&stateDesc, &pStates[nDevice][n]);
			if (FAILED(hr))
			{
				if (n < D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT 
					|| hr != D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS)
				{
					tRes = RESULT_FAIL;
					WriteToLog( _T("Failed on call to CreateSamplerState.\n"));
					WriteToLog( _T("Device#=%d, StateNum=%d, TotalStates=%d, hr=%s\n"), nDevice+1, n, nDevice*4096+n, D3DHResultToString(hr).c_str() );
					goto testDone;
				}
			}
			else if (n >= D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT)
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T("Too many unique objects were created without failure.\n"));
				WriteToLog( _T("Device#=%d, StateNum=%d, TotalStates=%d, hr=%s\n"), nDevice+1, n, nDevice*4096+n, D3DHResultToString(hr).c_str() );
				goto testDone;
			}
		}
	}

testDone:
	for (int nDevice = 0; nDevice < m_numDevices; ++nDevice)
	{
		if (pStates[nDevice] != NULL)
		{
			for (UINT n = 0; n <= D3D_REQ_SAMPLER_OBJECT_COUNT_PER_CONTEXT; ++n)
			{
				SAFE_RELEASE(pStates[nDevice][n]);
			}
			delete[] pStates[nDevice];
		}
		SAFE_RELEASE(pDevices[nDevice]);
	}

	return tRes;
}
