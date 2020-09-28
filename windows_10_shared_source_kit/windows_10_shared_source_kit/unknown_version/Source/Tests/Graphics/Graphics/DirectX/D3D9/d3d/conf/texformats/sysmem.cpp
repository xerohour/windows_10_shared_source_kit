#define D3D_OVERLOADS
#include <d3d.h>
#include "TexFormats.h"

//************************************************************************
// SysMem Test functions

CSysMemTest::CSysMemTest()
{
	m_szTestName = "System Memory Texture Formats";
	m_szCommandKey = "SysMem";

    // Tell parent that we only want system memory textures
    bSysMem = true;
}

// --------------------------------------------------------------------------------------

HRESULT CSysMemTest::CreateDevices(DEVICEMODE *pMode)
{
    if(!m_pD3D || !pMode)
	{
		DPF(1, _T("CD3DTest::CreateDevices() - Invalid params.\n"));
		return E_FAIL;
	}

	DWORD		dwPSGP = 0;
	DWORD		dwBeh = pMode->dwBehaviors;
	DWORD		dwRefBeh = m_Options.D3DOptions.dwRefBehaviors;
	bool		bRestore = false;
	HRESULT		hr = S_OK;

	// Create the SRC device
	hr = m_pD3D->CreateDevice(pMode->nAdapter, m_Options.D3DOptions.dwRefDevType, m_pFramework->m_hWindow,
								dwBeh, NULL, &m_pSrcDevice);
	if(FAILED(hr))
	{
		DPF(2, _T("CD3DTest::CreateDevices() - failed to create SRC device. hr=%s(%x)\n"),
			m_pD3D->HResultToString(hr), hr);
		goto Exit;
	}
	else
	{
		m_pSrcDevice->SetPSGPRegChange(pMode->dwPSGPRegChange);
		AddDeviceToList(m_pSrcDevice);
	}

	
	// Create the REF device if the test uses one
	if(m_Options.D3DOptions.bReference)
	{
		if(m_Options.D3DOptions.bRefEqualsSrc)
			m_pD3D->CreateDevice(pMode->nAdapter, pMode->dwDevType, m_pFramework->m_hWindow,dwBeh, NULL, &m_pRefDevice);
		else
			m_pD3D->CreateDevice(pMode->nAdapter, m_Options.D3DOptions.dwRefDevType, m_pFramework->m_hWindow, dwRefBeh, NULL, &m_pRefDevice);

		if(FAILED(hr))
		{
			DPF(2, _T("CD3DTest::CreateDevices() - failed to create REF device. hr=%s(%x)\n"),
			m_pD3D->HResultToString(hr), hr);
			goto Exit;
		}
		else
		{
			m_pRefDevice->SetPSGPRegChange(m_Options.D3DOptions.dwRefPSGPRegChange);
			AddDeviceToList(m_pRefDevice);
		}
	}

Exit:
	if(FAILED(hr))
	{
		ReleaseDevices();
	}
	return hr;
}

