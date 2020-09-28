#include "Blend.h"
#include <d3d11.h>

#define MAX_DEVICES (32)
static UINT D3D_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT(UINT deviceVer)
{
	switch(deviceVer)
	{
	case 0x1000:
	case 0x1010:
		return D3D10_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT;
	case 0x1100:
		return D3D11_REQ_BLEND_OBJECT_COUNT_PER_DEVICE;
	default:
		assert(!"Unexpected D3D Version");
		return 0;
	}
}

static const int c_ValidD3DCreateFlags = D3D11_CREATE_DEVICE_DEBUG | D3D11_CREATE_DEVICE_SWITCH_TO_REF | D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;

static
bool
isDualSource(D3D11_BLEND blend)
{
	switch(blend)
	{
	case D3D11_BLEND_SRC1_COLOR:
	case D3D11_BLEND_INV_SRC1_COLOR:
	case D3D11_BLEND_SRC1_ALPHA:
	case D3D11_BLEND_INV_SRC1_ALPHA:
		return true;
	default:
		return false;
	}
}

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
	ID3D11DeviceContext *pDeviceContext = NULL;
	g_App.CreateDevice(pAdapter, g_App.m_D3DOptions.DriverType, createFlags, g_App.m_D3DOptions.FeatureLevel, pNewDevice, &pDeviceContext);
	SAFE_RELEASE( pDeviceContext );

func_end:
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	return hr;
}

void 
CBlendStateTest::InitTestParameters()
{
	m_nPrimitiveW = g_App.Width();
	m_nPrimitiveH = g_App.Height();

	g_App.m_RenderTargetOptions.SwapChainDesc.Width = m_nPrimitiveW*2;
	g_App.m_RenderTargetOptions.SwapChainDesc.Height = m_nPrimitiveH;
	g_App.m_RenderTargetOptions.DepthTextureDesc.Width = m_nPrimitiveW*2;
	g_App.m_RenderTargetOptions.DepthTextureDesc.Height = m_nPrimitiveH;

	AddParameter( _T("NumDevices"), &m_numDevices );
	testfactor::RFactor rNumDevices = AddParameterValue( _T("NumDevices"), 16 );

	SetRootTestFactor(rNumDevices);
}

TEST_RESULT
CBlendStateTest::ExecuteTestCase()
{
	TEST_RESULT tRes = RESULT_PASS;
	HRESULT hr = E_UNEXPECTED;

	assert(m_numDevices <= MAX_DEVICES);

	ID3D11Device* pDevices[MAX_DEVICES] = {NULL};
	ID3D11BlendState** pStates[MAX_DEVICES] = {NULL};
	ID3D11BlendState** pStates_1[MAX_DEVICES] = {NULL};
	D3D11_BLEND_DESC defaultStateDesc, stateDesc;
	D3D11_BLEND_DESC defaultStateDesc_1, stateDesc_1;
	ZeroStructMem(defaultStateDesc);
	ZeroStructMem(defaultStateDesc_1);
	// defaults first
	defaultStateDesc.AlphaToCoverageEnable = FALSE;
	for(int n = 0; n < 8; ++n)
		defaultStateDesc.RenderTarget[n].BlendEnable = FALSE;
	defaultStateDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_ONE;
	defaultStateDesc.RenderTarget[0].DestBlend = D3D11_BLEND_ZERO;
	defaultStateDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	defaultStateDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	defaultStateDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	defaultStateDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	for(int n = 0; n < 8; ++n)
		defaultStateDesc.RenderTarget[n].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	defaultStateDesc_1.AlphaToCoverageEnable = FALSE;
	for(int n = 0; n < 8; ++n)
	{
		defaultStateDesc_1.RenderTarget[n].BlendEnable = FALSE;
		defaultStateDesc_1.RenderTarget[n].SrcBlend = D3D11_BLEND_ONE;
		defaultStateDesc_1.RenderTarget[n].DestBlend = D3D11_BLEND_ZERO;
		defaultStateDesc_1.RenderTarget[n].BlendOp = D3D11_BLEND_OP_ADD;
		defaultStateDesc_1.RenderTarget[n].SrcBlendAlpha = D3D11_BLEND_ONE;
		defaultStateDesc_1.RenderTarget[n].DestBlendAlpha = D3D11_BLEND_ZERO;
		defaultStateDesc_1.RenderTarget[n].BlendOpAlpha = D3D11_BLEND_OP_ADD;
		defaultStateDesc_1.RenderTarget[n].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
	}

	CUserValueSet<D3D11_BLEND> srcBlendValues;
	CUserValueSet<D3D11_BLEND> srcBlendAlphaValues;
	CUserValueSet<D3D11_BLEND> destBlendValues;
	CUserValueSet<D3D11_BLEND> destBlendAlphaValues;
	CUserValueSet<D3D11_BLEND_OP> blendOpValues;
	CUserValueSet<D3D11_BLEND_OP> blendOpAlphaValues;

	srcBlendValues.SetFromNamedValues();
	srcBlendAlphaValues.SetFromNamedValues();
	destBlendValues.SetFromNamedValues();
	destBlendAlphaValues.SetFromNamedValues();
	blendOpValues.SetFromNamedValues();
	blendOpAlphaValues.SetFromNamedValues();

	srcBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_SRC_COLOR);
	srcBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_INV_SRC_COLOR);
	srcBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_DEST_COLOR);
	srcBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_INV_DEST_COLOR);
	srcBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_SRC1_COLOR);
	srcBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_INV_SRC1_COLOR);

	destBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_SRC_COLOR);
	destBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_INV_SRC_COLOR);
	destBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_DEST_COLOR);
	destBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_INV_DEST_COLOR);
	destBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_SRC1_COLOR);
	destBlendAlphaValues.RemoveValueSlowly(D3D11_BLEND_INV_SRC1_COLOR);

	assert(g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_0);
	int numDeviceVersions = 1;
	if (g_App.m_D3DOptions.FeatureLevel >= D3D_FEATURE_LEVEL_10_1)
		numDeviceVersions = 2;
	for (int nDeviceVer = 0; nDeviceVer < numDeviceVersions; ++nDeviceVer)
	{
		const UINT deviceVersion = (nDeviceVer == 0) ? 0x1000 : 0x1010;
		UINT numStatesMade = 0;
		int deviceNum = 0;
		for (UINT srcBlend = 0; srcBlend < srcBlendValues.GetSize(); srcBlend = (srcBlend+1)%srcBlendValues.GetSize())
		{
			srcBlendValues.GetValue(srcBlend, &stateDesc.RenderTarget[0].SrcBlend);
			for (int n = 0; n < 8; ++n)
				stateDesc_1.RenderTarget[n].SrcBlend = stateDesc.RenderTarget[0].SrcBlend;
			for (UINT srcBlendAlpha = 0; srcBlendAlpha < srcBlendAlphaValues.GetSize(); ++srcBlendAlpha)
			{
				srcBlendAlphaValues.GetValue(srcBlendAlpha, &stateDesc.RenderTarget[0].SrcBlendAlpha);
				for (int n = 0; n < 8; ++n)
					stateDesc_1.RenderTarget[n].SrcBlendAlpha = stateDesc.RenderTarget[0].SrcBlendAlpha;
				for (UINT destBlend = 0; destBlend < destBlendValues.GetSize(); ++destBlend)
				{
					destBlendValues.GetValue(destBlend, &stateDesc.RenderTarget[0].DestBlend);
					for (int n = 0; n < 8; ++n)
						stateDesc_1.RenderTarget[n].DestBlend = stateDesc.RenderTarget[0].DestBlend;
					for (UINT destBlendAlpha = 0; destBlendAlpha < destBlendAlphaValues.GetSize(); ++destBlendAlpha)
					{
						destBlendAlphaValues.GetValue(destBlendAlpha, &stateDesc.RenderTarget[0].DestBlendAlpha);
						for (int n = 0; n < 8; ++n)
							stateDesc_1.RenderTarget[n].DestBlendAlpha = stateDesc.RenderTarget[0].DestBlendAlpha;
						for (UINT blendOp = 0; blendOp < blendOpValues.GetSize(); ++blendOp)
						{
							blendOpValues.GetValue(blendOp, &stateDesc.RenderTarget[0].BlendOp);
							for (int n = 0; n < 8; ++n)
								stateDesc_1.RenderTarget[n].BlendOp = stateDesc.RenderTarget[0].BlendOp;
							for (UINT blendOpAlpha = 0; blendOpAlpha < blendOpAlphaValues.GetSize(); ++blendOpAlpha)
							{
								blendOpAlphaValues.GetValue(blendOpAlpha, &stateDesc.RenderTarget[0].BlendOpAlpha);
								for (int n = 0; n < 8; ++n)
									stateDesc_1.RenderTarget[n].BlendOpAlpha = stateDesc.RenderTarget[0].BlendOpAlpha;
								for (BOOL a2cEnable = 0; a2cEnable < 2; ++a2cEnable)
								{
									stateDesc.AlphaToCoverageEnable = a2cEnable;
									stateDesc.RenderTarget[0].BlendEnable = TRUE;
									stateDesc.RenderTarget[0].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
									for (int n = 1; n < 8; ++n)
									{
										stateDesc.RenderTarget[n].BlendEnable = FALSE;
										stateDesc.RenderTarget[n].RenderTargetWriteMask = D3D11_COLOR_WRITE_ENABLE_ALL;
									}
									for (int n = 0; n < 8; ++n)
									{
										stateDesc_1.RenderTarget[n].BlendEnable = stateDesc.RenderTarget[n].BlendEnable;
										stateDesc_1.RenderTarget[n].RenderTargetWriteMask = stateDesc.RenderTarget[n].RenderTargetWriteMask;
									}
									stateDesc_1.AlphaToCoverageEnable = stateDesc.AlphaToCoverageEnable;

									bool bDualSource 
										= isDualSource(stateDesc_1.RenderTarget[0].SrcBlend)
										|| isDualSource(stateDesc_1.RenderTarget[0].DestBlend);
									bool bDualSourceAlpha
										= isDualSource(stateDesc_1.RenderTarget[0].SrcBlendAlpha)
										|| isDualSource(stateDesc_1.RenderTarget[0].DestBlendAlpha);

									if (  (bDualSource 
											&& (   stateDesc_1.RenderTarget[0].BlendOp == D3D11_BLEND_OP_MIN
												|| stateDesc_1.RenderTarget[0].BlendOp == D3D11_BLEND_OP_MAX))
										||(bDualSourceAlpha 
											&& (   stateDesc_1.RenderTarget[0].BlendOpAlpha == D3D11_BLEND_OP_MIN
												|| stateDesc_1.RenderTarget[0].BlendOpAlpha == D3D11_BLEND_OP_MAX))
										)
									{
										continue;
									}

									if (deviceNum >= m_numDevices)
										goto testDone;
									if (!pDevices[deviceNum])
									{
										hr = CreateTempDevice(m_pDevice1, &pDevices[deviceNum]);
										if (FAILED(hr))
										{
											tRes = RESULT_FAIL;
											WriteToLog( _T("Failed to create another device. device#=%d, hr=%s\n"), deviceNum+1, D3DHResultToString(hr).c_str());
											goto testDone;
										}
										if (deviceVersion == 0x1000)
										{
											pStates[deviceNum] = new ID3D11BlendState*[D3D_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT(deviceVersion)+1];
											ZeroMemory(pStates[deviceNum], sizeof(ID3D11BlendState*)*(D3D_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT(deviceVersion)+1));
											hr = pDevices[deviceNum]->CreateBlendState(&defaultStateDesc, &pStates[deviceNum][0]);
										}
										else if (deviceVersion == 0x1010 || deviceVersion == 0x1100)
										{
											pStates_1[deviceNum] = new ID3D11BlendState*[D3D_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT(deviceVersion)+1];
											ZeroMemory(pStates_1[deviceNum], sizeof(ID3D11BlendState*)*(D3D_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT(deviceVersion)+1));
											hr = pDevices[deviceNum]->CreateBlendState(&defaultStateDesc_1, &pStates_1[deviceNum][0]);
										}
										numStatesMade = 1;
									}

									if (deviceVersion == 0x1000)
									{
										hr = (hr!=S_OK)?hr:pDevices[deviceNum]->CreateBlendState(&stateDesc, &pStates[deviceNum][numStatesMade]);
									}
									else if (deviceVersion == 0x1010 || deviceVersion == 0x1100)
									{
										hr = (hr!=S_OK)?hr:pDevices[deviceNum]->CreateBlendState(&stateDesc_1, &pStates_1[deviceNum][numStatesMade]);
									}
									numStatesMade += 1;
									if (FAILED(hr))
									{
										if (numStatesMade < D3D_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT(deviceVersion) 
											|| hr != D3D11_ERROR_TOO_MANY_UNIQUE_STATE_OBJECTS)
										{
											tRes = RESULT_FAIL;
											WriteToLog( _T("Failed on call to CreateBlendState.\n"));
											WriteToLog( _T("Device#=%d, StateNum=%d, TotalStates=%d, hr=%s\n"), deviceNum+1, numStatesMade, deviceNum*4096+numStatesMade, D3DHResultToString(hr).c_str() );
											goto testDone;
										}
									}
									else if (numStatesMade > D3D_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT(deviceVersion))
									{
										tRes = RESULT_FAIL;
										WriteToLog( _T("Too many unique objects were created without failure.\n"));
										WriteToLog( _T("Device#=%d, StateNum=%d, TotalStates=%d, hr=%s\n"), deviceNum+1, numStatesMade, deviceNum*4096+numStatesMade, D3DHResultToString(hr).c_str() );
										goto testDone;
									}

									if (numStatesMade > D3D_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT(deviceVersion) )
									{
										numStatesMade = 0;
										deviceNum += 1;
									}
								}
							}
						}
					}
				}
			}
		}

	testDone:
		for (int nDevice = 0; nDevice < m_numDevices; ++nDevice)
		{
			if (pStates[nDevice] != NULL)
			{
				for (UINT n = 0; n <= D3D_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT(deviceVersion); ++n)
				{
					SAFE_RELEASE(pStates[nDevice][n]);
				}
				delete[] pStates[nDevice];
				pStates[nDevice] = NULL;
			}
			if (pStates_1[nDevice] != NULL)
			{
				for (UINT n = 0; n <= D3D_REQ_BLEND_OBJECT_COUNT_PER_CONTEXT(deviceVersion); ++n)
				{
					SAFE_RELEASE(pStates_1[nDevice][n]);
				}
				delete[] pStates_1[nDevice];
				pStates_1[nDevice] = NULL;
			}
			SAFE_RELEASE(pDevices[nDevice]);
		}
	}

	return tRes;
}
