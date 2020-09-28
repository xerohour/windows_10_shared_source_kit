//////////////////////////////////////////////////////////////////////////
//  State.cpp
//  created:	2006/07/14
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////


#include <WGFTestCore.h>
#include <stdlib.h>

#include "rasterizer.h"
#include "state.h"

#define D3D_REQ_RASTERIZER_OBJECT_COUNT_PER_CONTEXT (D3D10_REQ_RASTERIZER_OBJECT_COUNT_PER_CONTEXT)
#define MAX_DEVICES (32)

static const int c_ValidD3DCreateFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_SWITCH_TO_REF | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;

static HRESULT CreateTempDevice(CD3D11TestFramework *pFramework, ID3D11Device* pExisting, ID3D11Device** pNewDevice)
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
	ID3D11DeviceContext *pDevContext;
	pFramework->CreateDevice(pAdapter, pFramework->m_D3DOptions.DriverType, createFlags, pFramework->m_D3DOptions.FeatureLevel, pNewDevice, &pDevContext );
	SAFE_RELEASE( pDevContext );

func_end:
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	return hr;
}

TEST_RESULT CStateLimitsTest::Setup()
{
	m_pDevice = GetFramework()->GetDevice();
	return RESULT_PASS;
}

void CStateLimitsTest::Cleanup()
{

}

void CStateLimitsTest::InitTestParameters()
{
	int nPrimitiveW = 16;
	int nPrimitiveH = 16;

	GetFramework()->m_RenderTargetOptions.SwapChainDesc.Width = nPrimitiveW*2;
	GetFramework()->m_RenderTargetOptions.SwapChainDesc.Height = nPrimitiveH;
	GetFramework()->m_RenderTargetOptions.DepthTextureDesc.Width = nPrimitiveW*2;
	GetFramework()->m_RenderTargetOptions.DepthTextureDesc.Height = nPrimitiveH;

	AddParameter<UINT>( _T("NumDevices"), &m_numDevices );
	testfactor::RFactor rNumDevices = AddParameterValue( _T("NumDevices"), (UINT)16 );

	SetRootTestFactor(rNumDevices);
}

TEST_RESULT CStateLimitsTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = E_UNEXPECTED;

	assert(m_numDevices <= MAX_DEVICES);

	std::vector< std::vector<ID3D11RasterizerState*> > vStates(MAX_DEVICES);
	std::vector< ID3D11Device* > vDevices(MAX_DEVICES);

	D3D11_RASTERIZER_DESC defaultDesc, stateDesc;
	// defaults first
	defaultDesc.FillMode				= D3D11_FILL_SOLID;
	defaultDesc.CullMode				= D3D11_CULL_BACK;
	defaultDesc.FrontCounterClockwise	= FALSE;
	defaultDesc.DepthBias				= 0;
	defaultDesc.SlopeScaledDepthBias	= 0.0f;
	defaultDesc.DepthBiasClamp			= 0.0f;
	defaultDesc.DepthClipEnable			= TRUE;
	defaultDesc.ScissorEnable			= FALSE;
	defaultDesc.MultisampleEnable		= FALSE;
	defaultDesc.AntialiasedLineEnable	= FALSE;
	// then incremental values
	stateDesc = defaultDesc;

	for (UINT uDevice = 0; uDevice < m_numDevices; ++uDevice)
	{
		// create device
		if( FAILED( hr = CreateTempDevice(GetFramework(), m_pDevice, &vDevices[uDevice]) ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Failed to create another device. device#=%d, hr=%s\n"), uDevice+1, D3DHResultToString(hr).c_str());
			goto testDone;
		}
		
		// allocate room for 4096 states for this device
		vStates[uDevice].resize(D3D_REQ_RASTERIZER_OBJECT_COUNT_PER_CONTEXT+1);

		// create default state
		if( FAILED( hr = vDevices[uDevice]->CreateRasterizerState(&defaultDesc, &vStates[uDevice][0]) ) )
		{
			tRes = RESULT_FAIL;
			WriteToLog( _T("Failed on call CreateRasterizerState when creating default state. device#=%d, hr=%s\n"), uDevice+1, D3DHResultToString(hr).c_str());
			goto testDone;
		}

		for (UINT n = 1; n < D3D_REQ_RASTERIZER_OBJECT_COUNT_PER_CONTEXT; ++n)
		{
			// create a unique state
			stateDesc = defaultDesc;
			stateDesc.DepthBias = n;

			if( FAILED( hr = vDevices[uDevice]->CreateRasterizerState(&stateDesc, &vStates[uDevice][n]) ) )
			{
				if (n < D3D_REQ_RASTERIZER_OBJECT_COUNT_PER_CONTEXT 
					|| hr != D3D10_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS)
				{
					tRes = RESULT_FAIL;
					WriteToLog( _T("Failed on call to CreateRasterizerState.\n"));
					WriteToLog( _T("Device#=%d, StateNum=%d, TotalStates=%d, hr=%s\n"), uDevice+1, n, uDevice*4096+n, D3DHResultToString(hr).c_str() );
					goto testDone;
				}
			}
			else if (n >= D3D_REQ_RASTERIZER_OBJECT_COUNT_PER_CONTEXT)
			{
				tRes = RESULT_FAIL;
				WriteToLog( _T("Too many unique objects were created without failure.\n"));
				WriteToLog( _T("Device#=%d, StateNum=%d, TotalStates=%d, hr=%s\n"), uDevice+1, n, uDevice*4096+n, D3DHResultToString(hr).c_str() );
				goto testDone;
			}
		}
	}

testDone:
	for (UINT uDevice = 0; uDevice < m_numDevices; ++uDevice)
	{
		if (vStates[uDevice].size() > 0)
		{
			for (UINT n = 0; n <= D3D_REQ_RASTERIZER_OBJECT_COUNT_PER_CONTEXT; ++n)
			{
				SAFE_RELEASE(vStates[uDevice][n]);
			}
		}
		SAFE_RELEASE(vDevices[uDevice]);
	}

	return tRes;
}
