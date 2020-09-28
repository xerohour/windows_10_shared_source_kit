#include "D3DTestPCH.h"

#include "CD3DX.h"
#include "CD3D.h"
#include "CD3DTest.h"
#include "CImage.h"
#include "CSurface.h"
#include "CTextures.h"
#include "CSwapChain.h"
#include "CBuffers.h"
#include "CQuery.h"
#include "CEffect.h"


bool CD3DTest::Clear(DWORD Count, D3DRECT* pRects, DWORD Flags, D3DCOLOR Color, float Z, DWORD Stencil, DWORD dwFlags)
{
	HRESULT hr = E_FAIL;
	bool bRes = true;

	// Save off the clear color
	m_pFramework->GetImageCmp()->SetClearColor(Color);

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->Clear(Count, pRects, Flags, Color, Z, Stencil);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::Clear() - Clear failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::Present(RECT* pSourceRect, RECT* pDestRect, HWND hDestWindowOverride, RGNDATA* pDirtyRegion, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->Present(pSourceRect, pDestRect, hDestWindowOverride, pDirtyRegion);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::Present() - Present failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::IsDeviceLost()
{
	bool bRes = false;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		bRes |= m_DeviceList[i]->IsDeviceLost();
	}

	return bRes;
}


bool CD3DTest::ResetLostDevices()
{
	bool bRes;

	// Disable PAINT msg handling
	m_bHandlePaintMsg = false;

	// Notify the test
	OnDeviceLost();

	// Release D3D objects held by this class
	ReleaseObjects();

	// Loop here until
	// - the user requests an exit,
	// - we restore the device,
	// - or we've gone 5 times through the loop.
	for
	(
		UINT iCurrentResetAttempt = 0;
		iCurrentResetAttempt < 5 && ProcessMessages();
		++iCurrentResetAttempt
	)
	{
		bRes = true;

		for(UINT i=0; i<m_uDeviceList; i++)
		{
			if(!m_DeviceList[i]->HandleDeviceLost())
			{
				bRes = false;
				DPF(2, _T("CD3DTest::ResetLostDevices() - HandleDeviceLost failed on device #%d.\n"),i);
			}
		}
		
		if(bRes)
		{
			// Allow the app to reset
			OnReset();
			// Recreate all app D3D objects
			CreateObjects();
			// Re-enable PAINT msg handling
			if(m_pCurrentMode->bWindowed)
				m_bHandlePaintMsg = true;
			// Reset the device lost flag
			m_pD3D->SetDeviceLost(false);
			return true;
		}

		// We couldn't restore, so wait and try again
		Sleep(3000);
	}

	return false;
}

// --------------------------------------------------------------------------------------
// XXX ROBALD - This is a workaround for ManBugs #51787

bool CD3DTest::ResetTSConnect()
{
	// Disable PAINT msg handling
	m_bHandlePaintMsg = false;

	// Notify the test
	OnDeviceLost();

	// Release all D3D objects/devices held by this class
	ReleaseTestMode();

	// Loop here until either the user requests exit or we restore the device
	while(ProcessMessages())
	{
		if(!GetSystemMetrics(SM_REMOTESESSION))
		{
			// Recreate all devices/objects
			SetupTestMode(m_pCurrentMode);

			// Allow the app to reset
			OnReset();

			// Re-enable PAINT msg handling
			if(m_pCurrentMode->bWindowed)
				m_bHandlePaintMsg = true;

			// Reset the device lost flag
			m_pD3D->SetDeviceLost(false);
			return true;
		}

		// We couldn't restore, so wait and try again
		Sleep(3000);
	}

	return false;
}


bool CD3DTest::SetTransform(D3DTRANSFORMSTATETYPE Type, LPD3DMATRIX Matrix, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->SetTransform(Type, Matrix);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetTransform() - SetTransform failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


///////////////////////////////////////////////////////
// Function:  GetLostDevices
//
// Purpose:
// Returns a bit mask representing all devices that are
// non-NULL and lost (as reported by the IsDeviceLost
// on the device).
///////////////////////////////////////////////////////
DWORD CD3DTest::GetLostDevices()
{
	DWORD dwFlags = 0;
	UINT cValidDevices = 0;

	for
	(
		UINT i = 0;
		i < m_uDeviceList;
		i++
	)
	{
		DWORD dwMask = 1 << i;
		if ( m_DeviceList[i] == NULL )
		{
			continue;
		};

		if ( m_DeviceList[i]->IsDeviceLost() )
		{
			dwFlags |= dwMask;
		};
	};


	return dwFlags;
}


bool CD3DTest::CreateStateBlock(D3DSTATEBLOCKTYPE Type, DWORD *dwHandle, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	
	// Alloc the new handle
	if(!(*dwHandle = (DWORD)m_StateBlockHandles.AddHandle(&uHandles)))
	{
		DPF(1, _T("CD3DTest::CreateStateBlock() - failed to alloc new handle.\n"));
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;		
		
		hr = m_DeviceList[i]->CreateStateBlock(Type, uHandles + i);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::CreateStateBlock() - CreateStateBlock failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::CaptureStateBlock(DWORD dwHandle, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	
	// Find the handle
	if(!(uHandles = m_StateBlockHandles.GetHandle(dwHandle)))
	{
		DPF(1, _T("CD3DTest::CaptureStateBlock() - invalid handle(%x).\n"), dwHandle);
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CaptureStateBlock(*(uHandles + i));
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::CaptureStateBlock() - CaptureStateBlock failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::ApplyStateBlock(DWORD dwHandle, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	
	// Find the handle
	if(!(uHandles = m_StateBlockHandles.GetHandle(dwHandle)))
	{
		DPF(1, _T("CD3DTest::ApplyStateBlock() - invalid handle(%x).\n"), dwHandle);
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->ApplyStateBlock(*(uHandles + i));
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::ApplyStateBlock() - ApplyStateBlock failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::DeleteStateBlock(DWORD dwHandle, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	
	// Find the handle
	if(!(uHandles = m_StateBlockHandles.GetHandle(dwHandle)))
	{
		DPF(1, _T("CD3DTest::DeleteStateBlock() - invalid handle(%x).\n"), dwHandle);
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->DeleteStateBlock(*(uHandles + i));
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DeleteStateBlock() - DeleteStateBlock failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		*(uHandles + i) = 0;
	}

	return bRes;
}


bool CD3DTest::BeginStateBlock(DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->BeginStateBlock();
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::BeginStateBlock() - BeginStateBlock failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::EndStateBlock(DWORD *dwHandle, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	
	// Alloc the new handle
	if(!(*dwHandle = m_StateBlockHandles.AddHandle(&uHandles)))
	{
		DPF(1, _T("CD3DTest::EndStateBlock() - failed to alloc new handle.\n"));
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->EndStateBlock(uHandles + i);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::EndStateBlock() - EndStateBlock failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::LightEnable(DWORD dwLight, BOOL bOn, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->LightEnable(dwLight, bOn);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::LightEnable() - LightEnable failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetLightState(D3DLIGHTSTATETYPE Type, DWORD dwState, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->SetLightState(Type, dwState);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetLightState() - SetLightState failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetRenderState(D3DRENDERSTATETYPE Type, DWORD dwState, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
				
		hr = m_DeviceList[i]->SetRenderState(Type, dwState);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetRenderState() - SetRenderState failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetRenderTarget(
	LPSURFACES pRenderTarget,
	LPSURFACES pNewZStencil,
	DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	LPSURFACE RenderTarget;
	LPSURFACE NewZStencil;

	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		if (pRenderTarget != NULL )
		{
			if( !pRenderTarget->GetSurface( &RenderTarget, i ) )
				continue;
		}
		else
			RenderTarget = NULL;

		if (pNewZStencil != NULL )
		{
			if( !pNewZStencil->GetSurface( &NewZStencil, i ) )
				continue;
		}
		else
			NewZStencil = NULL;
		
		hr = m_DeviceList[i]->SetRenderTarget(RenderTarget, NewZStencil);

		RELEASE( RenderTarget );
		RELEASE( NewZStencil );

		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::SetRenderTarget() - "
					"SetRenderTarget failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetRenderTarget(
	DWORD RenderTargetIndex,
	LPSURFACES pRenderTarget,
	DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	LPSURFACE RenderTarget;

	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		if (pRenderTarget != NULL )
		{
			if( !pRenderTarget->GetSurface( &RenderTarget, i ) )
				continue;
		}
		else
			RenderTarget = NULL;
		
		hr = m_DeviceList[i]->SetRenderTarget(RenderTargetIndex, RenderTarget);

		RELEASE( RenderTarget );

		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::SetRenderTarget() - "
					"SetRenderTarget failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetDepthStencilSurface(LPSURFACES pZSurface, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	LPSURFACE NewZStencil;

	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		if (pZSurface != NULL )
		{
			if( !pZSurface->GetSurface( &NewZStencil, i ) )
				continue;
		}
		else
			NewZStencil = NULL;
		
		hr = m_DeviceList[i]->SetDepthStencilSurface(NewZStencil);

		RELEASE( NewZStencil );

		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::SetDepthStencilSurface() - "
					"SetDepthStencilSurface failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetMaterial(MATERIAL *pMat, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->SetMaterial(pMat);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetMaterial() - SetMaterial failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD dwValue, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->SetTextureStageState(dwStage, Type, dwValue);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetTextureStageState() - SetTextureStageState failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetTextureStageState(DWORD dwStage, D3DTEXTURESTAGESTATETYPE Type, DWORD *pdwValue, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetTextureStageState(dwStage,Type,pdwValue)));	
	}

	return false;
}


bool CD3DTest::SetLight(DWORD dwIndex, LIGHT *pLight, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->SetLight(dwIndex, pLight);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetLight() - SetLight failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::RenderPrimitive(
	D3DPRIMITIVETYPE PrimType,
	DWORD dwFVF,
	LPVOID pVertices,
	DWORD dwVertexCount,
	LPVOID pIndices,
	DWORD dwIndexCount, 
	RPOPTIONS *pOptions,
	DWORD dwFlags)
{
	bool bRes = true;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		if (!m_DeviceList[i]->RenderPrimitive(PrimType, dwFVF, pVertices, dwVertexCount, pIndices, dwIndexCount, pOptions))
		{
			DPF(1, _T("CD3DTest::RenderPrimitive() - RenderPrimitive failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(GetLastError()));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::BeginScene(DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->BeginScene();
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::BeginScene() - BeginScene failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::EndScene(DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->EndScene();
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::EndScene() - EndScene failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetViewport(LPVIEWPORT pViewport, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->SetViewport(pViewport);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetViewport() - SetViewport failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetClipPlane(DWORD dwIndex,D3DVALUE* pPlaneEquation, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->SetClipPlane(dwIndex, pPlaneEquation);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetClipPlane() - SetClipPlane failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::CreateRenderTarget(
	UINT Width,
	UINT Height,
	FORMAT Format,
	MULTISAMPLE_TYPE MultiSample,
	BOOL Lockable,
	LPSURFACES* ppSurfaces,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;

	if( ppSurfaces == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:CreateRenderTarget out of memory error.\n") );
		return false;
	}

	if( !pSurfaces->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:CreateRenderTarget out of memory error.\n") );
		return false;
	}


	LPSURFACE pSurface = NULL;

	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateRenderTarget(Width,Height,Format,MultiSample,Lockable,&pSurface);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateRenderTarget() - "
					"CreateRenderTarget failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppSurfaces = pSurfaces;
	return bRes;
}


bool CD3DTest::CreateRenderTarget(
	UINT Width,
	UINT Height,
	FORMAT Format,
	MULTISAMPLE_TYPE MultiSample,
	DWORD MultisampleQuality,
	BOOL Lockable,
	LPSURFACES* ppSurfaces,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;

	if( ppSurfaces == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:CreateRenderTarget out of memory error.\n") );
		return false;
	}
	if( !pSurfaces->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:CreateRenderTarget out of memory error.\n") );
		return false;
	}


	LPSURFACE pSurface = NULL;

	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateRenderTarget(Width,Height,Format,MultiSample,MultisampleQuality,Lockable,&pSurface, NULL);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateRenderTarget() - "
					"CreateRenderTarget failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppSurfaces = pSurfaces;
	return bRes;
}


bool CD3DTest::GetRenderTarget(LPSURFACES *ppRenderTarget, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSURFACE pSurface = NULL;

	if( ppRenderTarget == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:GetRenderTarget out of memory error.\n") );
		return false;
	}
	if( !pSurfaces->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:GetRenderTarget out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->GetRenderTarget(&pSurface);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::GetRenderTarget() - "
					"GetRenderTarget failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppRenderTarget = pSurfaces;
	return bRes;
}


bool CD3DTest::GetDepthStencilSurface(LPSURFACES *ppZStencilSurface, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSURFACE pSurface = NULL;

	if( ppZStencilSurface == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:GetDepthStencilSurface out of memory error.\n") );
		return false;
	}
	if( !pSurfaces->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:GetDepthStencilSurface out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->GetDepthStencilSurface(&pSurface);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::GetDepthStencilSurface() - "
					"GetDepthStencilSurface failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppZStencilSurface = pSurfaces;
	return bRes;
}


bool CD3DTest::CreateDepthStencilSurface(
	UINT Width,
	UINT Height,
	FORMAT Format,
	MULTISAMPLE_TYPE MultiSample,
	LPSURFACES *ppSurface,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSURFACE pSurface = NULL;

	if( ppSurface == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:CreateDepthStencilSurface out of memory error.\n") );
		return false;
	}

	if( !pSurfaces->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:CreateDepthStencilSurface out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateDepthStencilSurface(Width,Height,Format,MultiSample,&pSurface);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateDepthStencilSurface() - "
					"CreateDepthStencilSurface failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppSurface = pSurfaces;
	return bRes;
}


bool CD3DTest::CreateDepthStencilSurface(
	UINT Width,
	UINT Height,
	FORMAT Format,
	MULTISAMPLE_TYPE MultiSample,
	DWORD MultisampleQuality,
	BOOL Discard,
	LPSURFACES *ppSurface,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSURFACE pSurface = NULL;

	if( ppSurface == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:CreateDepthStencilSurface out of memory error.\n") );
		return false;
	}
	if( !pSurfaces->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:CreateDepthStencilSurface out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateDepthStencilSurface(Width,Height,Format,MultiSample,MultisampleQuality,Discard,&pSurface, NULL);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateDepthStencilSurface() - "
					"CreateDepthStencilSurface failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppSurface = pSurfaces;
	return bRes;
}


bool CD3DTest::CreateImageSurface(UINT Width,UINT Height,FORMAT Format,LPSURFACES* ppSurface, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSURFACE pSurface = NULL;

	if( ppSurface == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:CreateImageSurface out of memory error.\n") );
		return false;
	}

	if( !pSurfaces->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:CreateImageSurface out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateImageSurface(Width,Height,Format,&pSurface);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateImageSurface() - "
					"CreateImageSurface failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppSurface = pSurfaces;
	return bRes;
}


bool CD3DTest::CreateOffscreenPlainSurface(
	UINT Width,
	UINT Height,
	FORMAT Format,
	DWORD dwPool,
	LPSURFACES* ppSurface,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSURFACE pSurface = NULL;

	if( ppSurface == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:CreateOffscreenPlainSurface out of memory error.\n") );
		return false;
	}
	if( !pSurfaces->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:CreateOffscreenPlainSurface out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateOffscreenPlainSurface(Width,Height,Format,dwPool,&pSurface, NULL);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateOffscreenPlainSurface() - "
					"CreateOffscreenPlainSurface failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppSurface = pSurfaces;
	return bRes;
}


bool CD3DTest::GetBackBuffer(
	UINT BackBuffer,
	DWORD Type,
	LPSURFACES* ppBackBuffer,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSURFACE pSurface = NULL;

	if( ppBackBuffer == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:GetBackBuffer out of memory error.\n") );
		return false;
	}

	if( !pSurfaces->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:GetBackBuffer out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->GetBackBuffer(BackBuffer,Type,&pSurface);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::GetBackBuffer() - "
					"GetBackBuffer failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppBackBuffer = pSurfaces;
	return bRes;
}


bool CD3DTest::GetBackBuffer(UINT iSwapChain,UINT BackBuffer,DWORD Type,LPSURFACES* ppBackBuffer, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSURFACE pSurface = NULL;

	if( ppBackBuffer == NULL )
		return false;

	LPSURFACES pSurfaces = new CnSurface;
	if( pSurfaces == NULL )
	{
		DPF(1, _T("CD3DTest:GetBackBuffer out of memory error.\n") );
		return false;
	}

	if( !pSurfaces->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:GetBackBuffer out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->GetBackBuffer(iSwapChain,BackBuffer,Type,&pSurface);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::GetBackBuffer() - "
					"GetBackBuffer failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSurfaces->SetSurface( pSurface, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSurfaces );
		return bRes;
	}

	*ppBackBuffer = pSurfaces;
	return bRes;
}


bool CD3DTest::GetFrontBuffer(LPSURFACES pDestSurface, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSURFACE pSurface = NULL;

	if( pDestSurface == NULL )
	{
		DPF(1, _T("CD3DTest:GetFrontBuffer invalid parameter.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		if (!pDestSurface->GetSurface( &pSurface, i ))
			continue;

		hr = m_DeviceList[i]->GetFrontBuffer(pSurface);

		RELEASE( pSurface );

		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::GetFrontBuffer() - "
					"GetFrontBuffer failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetFrontBufferData(UINT iSwapChain,LPSURFACES pDestSurface, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSURFACE pSurface = NULL;

	if( pDestSurface == NULL )
	{
		DPF(1, _T("CD3DTest:GetFrontBufferData invalid parameter.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		if (!pDestSurface->GetSurface( &pSurface, i ))
			continue;

		hr = m_DeviceList[i]->GetFrontBufferData(iSwapChain,pSurface);

		RELEASE( pSurface );

		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::GetFrontBufferData() - "
					"GetFrontBufferData failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetRenderTargetData(LPSURFACES pSrcSurface,LPSURFACES pDestSurface, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSURFACE pSrcSurf = NULL;
	LPSURFACE pDstSurf = NULL;

	if( pDestSurface == NULL  || pSrcSurface == NULL)
	{
		DPF(1, _T("CD3DTest:GetRenderTargetData invalid parameter.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		if (!pSrcSurface->GetSurface( &pSrcSurf, i ))
			continue;
		if (!pDestSurface->GetSurface( &pDstSurf, i ))
			continue;

		hr = m_DeviceList[i]->GetRenderTargetData(pSrcSurf, pDstSurf);

		RELEASE( pSrcSurf );
		RELEASE( pDstSurf );

		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::GetRenderTargetData() - "
					"GetRenderTargetData failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


UINT CD3DTest::GetAvailableTextureMem(DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return m_DeviceList[i]->GetAvailableTextureMem();		
	}

	return 0;
}


bool CD3DTest::EvictManagedResources(DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->EvictManagedResources();
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::EvictManagedResources() - EvictManagedResources failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::ResourceManagerDiscardBytes(DWORD Bytes, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->ResourceManagerDiscardBytes(Bytes);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::ResourceManagerDiscardBytes() - ResourceManagerDiscardBytes failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetDisplayMode(DISPLAYMODE* pMode, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetDisplayMode(pMode)));		
	}

	return false;
}


bool CD3DTest::GetCreationParameters(DEVICE_CREATION_PARAMETERS *pParameters, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetCreationParameters(pParameters)));		
	}

	return false;
}


bool CD3DTest::SetCursorProperties(UINT XHotSpot,UINT YHotSpot,LPSURFACES pCursorBitmap, DWORD dwFlags)
{
	bool bRes = true;
	LPSURFACE pSurface = NULL;
	HRESULT hr = E_FAIL;

	if( pCursorBitmap == NULL )
	{
		DPF(1, _T("CD3DTest:GetFrontBuffer invalid parameter.\n") );
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		if(!pCursorBitmap->GetSurface( &pSurface, i ) )
			continue;

		hr = m_DeviceList[i]->SetCursorProperties(XHotSpot,YHotSpot,pSurface);

		RELEASE( pSurface );

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetCursorProperties() - SetCursorProperties failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


void CD3DTest::SetCursorPosition(UINT XScreenSpace,UINT YScreenSpace,DWORD Flags, DWORD dwFlags)
{
	// search for first available device - we can't have a separate cursor for every device can we?
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		m_DeviceList[i]->SetCursorPosition(XScreenSpace,YScreenSpace,Flags);		
		return;
	}
}


void CD3DTest::ShowCursor(BOOL bShow, DWORD dwFlags)
{
	bool bRes = true;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		m_DeviceList[i]->ShowCursor(bShow);
	}
}


bool CD3DTest::Reset(PRESENT_PARAMETERS* pPresentationParameters, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->Reset(pPresentationParameters == NULL ? m_DeviceList[i]->GetPresParams() : pPresentationParameters);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::Reset() - Reset failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetRasterStatus(RASTER_STATUS* pRasterStatus, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetRasterStatus(pRasterStatus)));		
	}

	return false;
}


void CD3DTest::SetGammaRamp(DWORD Flags,GAMMARAMP* pRamp, DWORD dwFlags)
{
	bool bRes = true;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		m_DeviceList[i]->SetGammaRamp(Flags,pRamp);
	}
}


void CD3DTest::GetGammaRamp(GAMMARAMP* pRamp, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		m_DeviceList[i]->GetGammaRamp(pRamp);
		return;		
	}
}


bool CD3DTest::CopyRects(LPSURFACES pSourceSurface,RECT* pSourceRectsArray,UINT cRects,LPSURFACES pDestinationSurface,POINT* pDestPointsArray, DWORD dwFlags)
{
	bool bRes = true;
	LPSURFACE pSrcSurface = NULL;
	LPSURFACE pDestSurface = NULL;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		pSrcSurface = NULL;
		if( pSourceSurface )
		{
			if(!pSourceSurface->GetSurface( &pSrcSurface, i ) )
				continue;
		}
		
		pDestSurface = NULL;
		if( pDestinationSurface )
		{
			if(!pDestinationSurface->GetSurface( &pDestSurface, i ) )
			{
				RELEASE(pSrcSurface);
				continue;
			}
		}

		hr = m_DeviceList[i]->CopyRects(pSrcSurface,pSourceRectsArray,cRects,pDestSurface,pDestPointsArray);

		RELEASE( pSrcSurface );
		RELEASE( pDestSurface );

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::CopyRects() - CopyRects failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::UpdateSurface(LPSURFACES pSourceSurface,RECT* pSourceRects,LPSURFACES pDestinationSurface,POINT* pDestPoint, DWORD dwFlags)
{
	bool bRes = true;
	LPSURFACE pSrcSurface = NULL;
	LPSURFACE pDestSurface = NULL;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		pSrcSurface = NULL;
		if( pSourceSurface )
		{
			if(!pSourceSurface->GetSurface( &pSrcSurface, i ) )
				continue;
		}
		
		pDestSurface = NULL;
		if( pDestinationSurface )
		{
			if(!pDestinationSurface->GetSurface( &pDestSurface, i ) )
			{
				RELEASE(pSrcSurface);
				continue;
			}
		}

		hr = m_DeviceList[i]->UpdateSurface(pSrcSurface,pSourceRects,pDestSurface,pDestPoint);

		RELEASE( pSrcSurface );
		RELEASE( pDestSurface );

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::UpdateSurface() - UpdateSurface failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::StretchRect(LPSURFACES pSourceSurface,RECT* pSourceRect,LPSURFACES pDestinationSurface,RECT* pDestRect, DWORD dwFilter, DWORD dwFlags)
{
	bool bRes = true;
	LPSURFACE pSrcSurface = NULL;
	LPSURFACE pDestSurface = NULL;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		pSrcSurface = NULL;
		if( pSourceSurface )
		{
			if(!pSourceSurface->GetSurface( &pSrcSurface, i ) )
				continue;
		}
		
		pDestSurface = NULL;
		if( pDestinationSurface )
		{
			if(!pDestinationSurface->GetSurface( &pDestSurface, i ) )
			{
				RELEASE(pSrcSurface);
				continue;
			}
		}

		hr = m_DeviceList[i]->StretchRect(pSrcSurface,pSourceRect,pDestSurface,pDestRect,dwFilter);

		RELEASE( pSrcSurface );
		RELEASE( pDestSurface );

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::StretchRect() - StretchRect failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetTransform(D3DTRANSFORMSTATETYPE State,D3DMATRIX* pMatrix, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetTransform(State,pMatrix)));		
	}

	return false;
}


bool CD3DTest::MultiplyTransform(D3DTRANSFORMSTATETYPE Type,D3DMATRIX* Matrix, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->MultiplyTransform(Type,Matrix);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::MultiplyTransform() - MultiplyTransform failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetViewport(LPVIEWPORT pViewport, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetViewport(pViewport)));		
	}

	return false;
}


bool CD3DTest::GetMaterial(MATERIAL *pMaterial, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetMaterial(pMaterial)));		
	}

	return false;
}


bool CD3DTest::GetLight(DWORD dwIndex, LIGHT *pLight, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetLight(dwIndex,pLight)));		
	}

	return false;
}


bool CD3DTest::GetLightEnable(DWORD dwLightNum, BOOL* pEnable, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetLightEnable(dwLightNum,pEnable)));		
	}

	return false;
}


bool CD3DTest::GetClipPlane(DWORD dwIndex, float* pPlane, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetClipPlane(dwIndex,pPlane)));		
	}

	return false;
}


bool CD3DTest::GetLightState(D3DLIGHTSTATETYPE Type, DWORD *lpdwState, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetLightState(Type, lpdwState)));
	}

	return false;
}


bool CD3DTest::GetRenderState(D3DRENDERSTATETYPE Type, DWORD *lpdwState, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetRenderState(Type, lpdwState)));		
	}

	return false;
}


bool CD3DTest::SetClipStatus(LPCLIPSTATUS pClipStatus, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->SetClipStatus(pClipStatus);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetClipStatus() - SetClipStatus failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetClipStatus(LPCLIPSTATUS pClipStatus, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetClipStatus(pClipStatus)));		
	}

	return false;
}


bool CD3DTest::ValidateDevice(DWORD *pdwPasses, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->ValidateDevice(pdwPasses)));		
	}

	return false;
}


bool CD3DTest::GetVerticalBlankStatus(LPBOOL lpbIsInVB, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetVerticalBlankStatus(lpbIsInVB)));
	}

	return false;
}


bool CD3DTest::GetInfo(DWORD dwDevInfoID,LPVOID pDevInfoStruct,DWORD dwSize, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetInfo(dwDevInfoID,pDevInfoStruct,dwSize)));		
	}

	return false;
}


bool CD3DTest::SetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->SetPaletteEntries(PaletteNumber,pEntries);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetPaletteEntries() - SetPaletteEntries failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetPaletteEntries(UINT PaletteNumber,PALETTEENTRY* pEntries, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetPaletteEntries(PaletteNumber,pEntries)));	
	}

	return false;
}


bool CD3DTest::SetCurrentTexturePalette(UINT PaletteNumber, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->SetCurrentTexturePalette(PaletteNumber);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetCurrentTexturePalette() - SetCurrentTexturePalette failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetCurrentTexturePalette(UINT *PaletteNumber, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetCurrentTexturePalette(PaletteNumber)));		
	}

	return false;
}


bool CD3DTest::CreateVertexDeclaration(VERTEXELEMENT* pVE, CnVertexDeclaration** ppDecl, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	CVertexDeclaration* pDecl = NULL;

	if(!ppDecl)
	{
		DPF(1, _T("CD3DTest:CreateVertexDeclaration() ppDecl is NULL.\n"));
		return false;
	}
	*ppDecl = NULL;

	CnVertexDeclaration* pDecls = new CnVertexDeclaration();
	if(!pDecls)
	{
		DPF(1, _T("CD3DTest:CreateVertexDeclaration() out of memory error.\n"));
		return false;
	}

	if (!pDecls->Init(m_uDeviceList)) {
		DPF(1, _T("CD3DTest:CreateVertexDeclaration() - CnVertexDeclaration::Init() failed.\n"));
		return false;
	}

	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->CreateVertexDeclaration(pVE, &pDecl);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::CreateVertexDeclaration() - CreateVertexDeclaration failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
			break;
		}

		if(!pDecls->SetVDecl(pDecl, i))
		{
			DPF(1, _T("CD3DTest::CreateVertexDeclaration() - CnVertexDeclaration::SetVDecl failed on device#%d.\n"), i);
			bRes= false;
			break;
		}
	}

	if(bRes)
		*ppDecl = pDecls;
	else
		RELEASE(pDecls);

	return bRes;
}


bool CD3DTest::CreateVertexShader(LPDWORD pdwDeclaration, LPDWORD pdwFunction, LPDWORD pdwHandle, DWORD dwUsage, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	DWORD dwHandle = 0;
	
	// Alloc the new handle
	if(!(dwHandle = m_VertexShaderHandles.AddHandle(&uHandles)))
	{
		DPF(1, _T("CD3DTest::CreateVertexShader() - failed to alloc new handle.\n"));
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;		
		
		hr = m_DeviceList[i]->CreateVertexShader(pdwDeclaration,pdwFunction,(CVertexShader**)(uHandles + i),dwUsage);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::CreateVertexShader() - CreateVertexShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	if(bRes)
	{
		*pdwHandle = dwHandle;
	}

	return bRes;
}


bool CD3DTest::CreateVertexShader(LPDWORD pdwFunction, CnVertexShader **ppVShaders, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	CVertexShader* pShader = NULL;

	if(!ppVShaders)
	{
		DPF(1, _T("CD3DTest:CreateVertexShader() pVShaders is NULL.\n"));
		return false;
	}
	*ppVShaders = NULL;

	CnVertexShader* pShaders = new CnVertexShader();
	if(!pShaders)
	{
		DPF(1, _T("CD3DTest:CreateVertexShader() out of memory error.\n"));
		return false;
	}

	if (!pShaders->Init(m_uDeviceList)) 
	{
		DPF(1, _T("CD3DTest:CreateVertexShader() - CnVertexShader::Init() failed.\n"));
		bRes = false;
	}
	else
	{
		for(UINT i=0; i<m_uDeviceList; i++)
		{
			if(SkipDevice(i, dwFlags))
				continue;		
			
			hr = m_DeviceList[i]->CreateVertexShader(pdwFunction, &pShader);
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::CreateVertexShader() - CreateVertexShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
				bRes= false;
			}
			else
			{
				if(!pShaders->SetVShader(pShader, i))
				{
					RELEASE(pShader);
					DPF(1, _T("CD3DTest::CreateVertexDeclaration() - CnVertexShader::SetVShader failed on device#%d.\n"), i);
					bRes= false;
				}
			}
		}
	}

	if(bRes)
		*ppVShaders = pShaders;
	else
		RELEASE(pShaders);

	return bRes;
}


bool CD3DTest::SetVertexShader(DWORD dwHandle, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR * uHandles = NULL;
	bool bCustomShader = true;
	
	if( ((dwHandle & 0x01) != 0) && !(uHandles = m_VertexShaderHandles.GetHandle(dwHandle)))
	{
		DPF(1, _T("CD3DTest::SetVertexShader() - invalid handle(%x).\n"), dwHandle);
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		if (dwHandle == 0)
		{
			hr = m_DeviceList[i]->SetVertexShader((CVertexShader*)0);
		}
		else if (dwHandle & 0x01)
		{
			ASSERT(uHandles != NULL);
			ASSERT(uHandles[i] != NULL);
			hr = m_DeviceList[i]->SetVertexShader((CVertexShader*)(*(uHandles + i)));
		}
		else
		{
			hr = m_DeviceList[i]->SetFVF(dwHandle);
		}

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetVertexShader() - SetVertexShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetFVF(DWORD dwFVF, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->SetFVF(dwFVF);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetFVF() - SetFVF failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetFVF(DWORD* dwFVF, DWORD dwFlags)
{
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		return (SUCCEEDED( m_DeviceList[i]->GetFVF(dwFVF) ) );
	}

	return false;
}


bool CD3DTest::GetVertexShader(DWORD* pHandle, DWORD dwFlags)
{
	HRESULT hr = E_FAIL;
	UINT_PTR* uHandles = NULL;
	DWORD  uHandle = 0;
	UINT nHandle = 0;
	UINT max = m_VertexShaderHandles.GetNumHandles();
	UINT i = 0;

	for(i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->GetVertexShader(&uHandle);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::GetVertexShader() - GetVertexShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			return false;
		}
		else if (uHandle == 0 )
		{
			*pHandle = (DWORD)0;
			return true;
		}
		nHandle = i;
		goto gvsOut;
	}

	return false;

gvsOut:
	for(i=0; i<max; i++);
	{
		uHandles = m_VertexShaderHandles.GetHandle(i);
		if( *(uHandles + nHandle) == uHandle )
		{
			*pHandle = (DWORD)i;
			return true;
		}
	}

	return false;
}


bool CD3DTest::SetVertexDeclaration(CnVertexDeclaration* pDecls, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	if(pDecls == NULL)
	{
		DPF(1, _T("CD3DTest::SetVertexDeclaration() - pDecls is NULL\n"));
		return false;
	}
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		CVertexDeclaration* pDecl;
		if(!pDecls->GetVDecl(&pDecl, i))
		{
			DPF(1, _T("CD3DTest::SetVertexDeclaration() - CnVertexShader::GetVDecl failed on device#%d\n"), i);
			bRes= false;
			continue;
		}

		hr = m_DeviceList[i]->SetVertexDeclaration(pDecl);

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetVertexDeclaration() - SetVertexDeclaration failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}

		RELEASE(pDecl);
	}

	return bRes;
}


bool CD3DTest::SetVertexShader(CnVertexShader* pShaders, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		CVertexShader* pShader = NULL;
		if(NULL != pShaders && !pShaders->GetVShader(&pShader, i))
		{
			DPF(1, _T("CD3DTest::SetVertexShader() - CnVertexShader::GetVShader failed on device#%d\n"), i);
			bRes= false;
			continue;
		}

		hr = m_DeviceList[i]->SetVertexShader(pShader);

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetVertexShader() - SetVertexShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}

		RELEASE(pShader);
	}

	return bRes;
}


bool CD3DTest::GetVertexShader(CnVertexShader** ppShader, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	if(!ppShader)
	{
		DPF(1, _T("CD3DTest:GetVertexShader() ppShader is NULL.\n"));
		return false;
	}
	*ppShader = NULL;

	CnVertexShader* pShaders = new CnVertexShader();
	if(!pShaders)
	{
		DPF(1, _T("CD3DTest:GetVertexShader() out of memory error.\n"));
		return false;
	}

	if (!pShaders->Init(m_uDeviceList)) {
		DPF(1, _T("CD3DTest:GetVertexShader() - CnVertexShader::Init() failed.\n"));
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		CVertexShader* pShader;
		hr = m_DeviceList[i]->GetVertexShader(&pShader);

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::GetVertexShader() - GetVertexShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			return false;
		}
		if(!pShaders->SetVShader(pShader, i))
		{
			DPF(1, _T("CD3DTest::GetVertexShader() - CnVertexShader::SetVShader failed on device#%d.\n"), i);
			bRes= false;
			break;
		}
	}

	if(bRes)
		*ppShader = pShaders;
	else
		RELEASE(pShaders);

	return bRes;
}


bool CD3DTest::DeleteVertexShader(DWORD dwHandle, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;

	// Find the handle
	if(!(uHandles = m_VertexShaderHandles.GetHandle(dwHandle)))
	{
		DPF(1, _T("CD3DTest::DeleteVertexShader() - invalid handle(%x).\n"), dwHandle);
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->DeleteVertexShader((CVertexShader*)(*(uHandles + i)));
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DeleteVertexShader() - DeleteVertexShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetVertexShaderConstantF(UINT dwRegister, CONST float* pConstantData, UINT dwConstantCount, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->SetVertexShaderConstantF(dwRegister, pConstantData, dwConstantCount);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetVertexShaderConstant() - SetVertexShaderConstant failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetVertexShaderConstantF(UINT Register,float* pConstantData,UINT ConstantCount, DWORD dwFlags)
{

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		return (SUCCEEDED( m_DeviceList[i]->GetVertexShaderConstantF(Register, pConstantData, ConstantCount) ) );
	}

	return false;
}


bool CD3DTest::SetVertexShaderConstantI(UINT dwRegister, CONST int* pConstantData, UINT dwConstantCount, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->SetVertexShaderConstantI(dwRegister, pConstantData, dwConstantCount);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetVertexShaderConstantI() - SetVertexShaderConstantI failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetVertexShaderConstantI(UINT Register, int* pConstantData,UINT ConstantCount, DWORD dwFlags)
{
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		return (SUCCEEDED( m_DeviceList[i]->GetVertexShaderConstantI(Register, pConstantData, ConstantCount) ) );
	}

	return false;
}


bool CD3DTest::SetVertexShaderConstantB(UINT StartRegister,CONST BOOL* pConstantData, UINT BoolCount, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->SetVertexShaderConstantB(StartRegister, pConstantData, BoolCount);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetVertexShaderConstantB() - SetVertexShaderConstantB failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetVertexShaderConstantB(UINT StartRegister,BOOL* pConstantData, UINT BoolCount, DWORD dwFlags)
{
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		return (SUCCEEDED( m_DeviceList[i]->GetVertexShaderConstantB(StartRegister,pConstantData, BoolCount) ) );
	}

	return false;
}


bool CD3DTest::GetVertexShaderDeclaration(DWORD Handle,void* pData,DWORD* pSizeOfData, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	
	// Find the handle
	if(!(uHandles = m_VertexShaderHandles.GetHandle(Handle)))
	{
		DPF(1, _T("CD3DTest::GetVertexShaderDeclaration() - invalid handle(%x).\n"), Handle);
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->GetVertexShaderDeclaration((DWORD)*(uHandles + i), pData, pSizeOfData);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::GetVertexShaderDeclaration() - GetVertexShaderDeclaration failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			return false;
		}
		return true;
	}

	return false;
}


bool CD3DTest::GetVertexShaderFunction(DWORD Handle,void* pData,DWORD* pSizeOfData, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	
	// Find the handle
	if(!(uHandles = m_VertexShaderHandles.GetHandle(Handle)))
	{
		DPF(1, _T("CD3DTest::GetVertexShaderFunction() - invalid handle(%x).\n"), Handle);
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->GetVertexShaderFunction((DWORD)*(uHandles + i), pData, pSizeOfData);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::GetVertexShaderFunction() - GetVertexShaderFunction failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			return false;
		}
		return true;
	}

	return false;
}


bool CD3DTest::SetStreamSource(
	UINT streamNumber,
	CnVertexBuffer *pStreamData,
	UINT stride,
	UINT offset,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPVERTEXBUFFER pVertexBuffer = NULL;

	if (0L == pStreamData)
	{
		DPF(1, _T("CD3DTest:SetStreamSource() pStreamData == 0L.\n"));
		return false;
	}

	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		pStreamData->GetVertexBuffer(&pVertexBuffer, i);
		hr = m_DeviceList[i]->SetStreamSource(streamNumber, pVertexBuffer, stride, offset);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::SetStreamSource() - "
					"SetStreamSource() failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		RELEASE(pVertexBuffer);
	}
	return bRes;
}


bool CD3DTest::GetStreamSource(
	UINT streamNumber,
	CnVertexBuffer **ppStreamData,
	UINT *pStride,
	DWORD dwFlags)
{
	bool bRes = true;
	bool bNotNull = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPVERTEXBUFFER pVertexBuffer = NULL;

	if (0L == ppStreamData)
	{
		DPF(1, _T("CD3DTest:GetStreamSource() ppStreamData == 0L.\n"));
		return false;
	}
	*ppStreamData = 0L;

	if (0L == pStride)
	{
		DPF(1, _T("CD3DTest:GetStreamSource() pStride == 0L.\n"));
		return false;
	}

	LPVERTEXBUFFERS pVertexBuffers = new CnVertexBuffer;
	if (0L == pVertexBuffers)
	{
		DPF(1, _T("CD3DTest:GetStreamSource() out of memory error.\n"));
		return false;
	}

	if (!pVertexBuffers->Init(m_uDeviceList))
	{
		DPF(1, _T("CD3DTest:GetStreamSource() ppStreamData Init() failed.\n"));
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))	continue;

		hr = m_DeviceList[i]->GetStreamSource(streamNumber, &pVertexBuffer, pStride);
		if (FAILED(hr))
		{
			DPF(1, _T("CD3DTest::GetStreamSource() - "
						"GetStreamSource() failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes = false;
		}
		else if( pVertexBuffer == 0L )
		{
			bNotNull = false;
		}
		else
		{
			pVertexBuffers->SetVertexBuffer(pVertexBuffer, i);
		}
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes || !bNotNull )
	{
		RELEASE( pVertexBuffers );
	}
	if( bRes )
		*ppStreamData = bNotNull ? pVertexBuffers : NULL;

	return bRes;
}


bool CD3DTest::SetIndices(
	CnIndexBuffer *pIndexData,
	UINT baseVertexIndex,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPINDEXBUFFER pIndexBuffer = NULL;

	if (NULL == pIndexData)
	{
		DPF(1, _T("CD3DTest:SetIndices() pIndexData == 0L.\n"));
		return false;
	}

	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		pIndexData->GetIndexBuffer(&pIndexBuffer, i);
		hr = m_DeviceList[i]->SetIndices(pIndexBuffer, baseVertexIndex);
		if (FAILED(hr))	{
			DPF(1,
				_T("CD3DTest::SetIndices() - "
					"SetIndices() failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}

		RELEASE(pIndexBuffer);
	}
	return bRes;
}


bool CD3DTest::GetIndices(
	CnIndexBuffer **ppIndexData,
	UINT *pBaseVertexIndex,
	DWORD dwFlags)
{
	bool bRes = true;
	bool bNotNull = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPINDEXBUFFER pIndexBuffer = NULL;

	if (0L == ppIndexData)
	{
		DPF(1, _T("CD3DTest:GetIndices() ppIndexData == 0L.\n"));
		return false;
	}
	*ppIndexData = 0L;

	if (0L == pBaseVertexIndex)
	{
		DPF(1, _T("CD3DTest:GetIndices() pBaseVertexIndex == 0L.\n"));
		return false;
	}

	LPINDEXBUFFERS pIndexBuffers = new CnIndexBuffer;
	if (0L == pIndexBuffers)
	{
		DPF(1, _T("CD3DTest:GetIndices() out of memory error.\n"));
		return false;
	}

	if (!pIndexBuffers->Init(m_uDeviceList))
	{
		DPF(1, _T("CD3DTest:GetIndices() ppStreamData Init() failed.\n"));
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))	continue;

		hr = m_DeviceList[i]->GetIndices(&pIndexBuffer, pBaseVertexIndex);
		if (FAILED(hr))	{
			DPF(1, _T("CD3DTest::GetIndices() - "
						"GetIndices() failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes = false;
		}
		else if( 0L == pIndexBuffer )
		{
			bNotNull = false;
		}
		else
		{
			pIndexBuffers->SetIndexBuffer(pIndexBuffer, i);
		}
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes || !bNotNull )
	{
		RELEASE( pIndexBuffers );
	}

	if( bRes )
		*ppIndexData = bNotNull ? pIndexBuffers : NULL;

	return bRes;
}


bool CD3DTest::CreatePixelShader(
	LPDWORD pdwFunction,
	LPDWORD pdwHandle,
	DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	DWORD dwHandle = 0;
	
	// Alloc the new handle
	if(!(dwHandle = m_PixelShaderHandles.AddHandle(&uHandles)))
	{
		DPF(1, _T("CD3DTest::CreatePixelShader() - failed to alloc new handle.\n"));
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;		
		
		hr = m_DeviceList[i]->CreatePixelShader(pdwFunction,(CPShader**)(uHandles + i));
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::CreatePixelShader() - CreatePixelShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	if(bRes)
	{
		*pdwHandle = dwHandle;
	}

	return bRes;
}


bool CD3DTest::CreatePixelShader(LPDWORD pdwFunction, CnPixelShader **ppPShaders, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	if(!ppPShaders)
	{
		DPF(1, _T("CD3DTest:CreatePixelShader() pPShaders is NULL.\n"));
		return false;
	}
	*ppPShaders = NULL;

	CnPixelShader* pShaders = new CnPixelShader();
	if(!pShaders)
	{
		DPF(1, _T("CD3DTest:CreatePixelShader() out of memory error.\n"));
		return false;
	}

	if (!pShaders->Init(m_uDeviceList)) 
	{
		DPF(1, _T("CD3DTest:CreatePixelShader() - CnPixelShader::Init() failed.\n"));
		bRes = false;
	}
	else
	{
		for(UINT i=0; i<m_uDeviceList; i++)
		{
			if(SkipDevice(i, dwFlags))
				continue;		
			
			CPShader* pShader = NULL;

			hr = m_DeviceList[i]->CreatePixelShader(pdwFunction, &pShader);
			if(FAILED(hr))
			{
				DPF(1, _T("CD3DTest::CreatePixelShader() - CreatePixelShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
				bRes= false;
			}
			else
			{
				if(!pShaders->SetPShader(pShader, i))
				{
					DPF(1, _T("CD3DTest::CreatePixelDeclaration() - CnPixelShader::SetPShader failed on device#%d.\n"), i);
					bRes= false;
				}
			}
		}
	}

	if(bRes)
		*ppPShaders = pShaders;
	else
		RELEASE(pShaders);

	return bRes;
}


bool CD3DTest::SetPixelShader(DWORD dwHandle, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	
	if ( dwHandle )
	{
		// Find the handle
		if(!(uHandles = m_PixelShaderHandles.GetHandle(dwHandle)))
		{
			DPF(1, _T("CD3DTest::SetPixelShader() - invalid handle(%x).\n"), dwHandle);
			return false;
		}
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		if ( uHandles )
			hr = m_DeviceList[i]->SetPixelShader((CPShader*)*(uHandles + i));
		else
			hr = m_DeviceList[i]->SetPixelShader((CPShader*)NULL);

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetPixelShader() - SetPixelShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetPixelShader(CnPixelShader* pShaders, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		CPShader* pShader = NULL;
		if(NULL != pShaders && !pShaders->GetPShader(&pShader, i))
		{
			DPF(1, _T("CD3DTest::SetPixelShader() - CnPixelShader::GetPShader failed on device#%d\n"), i);
			bRes= false;
			continue;
		}

		hr = m_DeviceList[i]->SetPixelShader(pShader);

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetPixelShader() - SetPixelShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}

		RELEASE(pShader);
	}

	return bRes;
}


bool CD3DTest::GetPixelShader(DWORD* pHandle, DWORD dwFlags)
{
	HRESULT hr = E_FAIL;
	UINT_PTR * uHandles = NULL;
	UINT_PTR uHandle = 0;
	UINT nHandle = 0;
	UINT max = m_PixelShaderHandles.GetNumHandles();
	UINT i = 0;

	for(i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->GetPixelShader((CPShader**)&uHandle);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::GetPixelShader() - GetVertexShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			return false;
		}
		else if( uHandle == NULL)
		{
			*pHandle = (DWORD)0;
			return true;
		}

		nHandle = i;
		goto gvsOut;
	}

	return false;

gvsOut:
	for(i=0; i<max; i++);
	{
		uHandles = m_PixelShaderHandles.GetHandle(i);
		if( *(uHandles + nHandle) == uHandle )
		{
			*pHandle = (DWORD)i;
			return true;
		}
	}

	return false;
}


bool CD3DTest::GetPixelShader(CnPixelShader** ppShader, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	if(!ppShader)
	{
		DPF(1, _T("CD3DTest:GetPixelShader() ppShader is NULL.\n"));
		return false;
	}
	*ppShader = NULL;

	CnPixelShader* pShaders = new CnPixelShader();
	if(!pShaders)
	{
		DPF(1, _T("CD3DTest:GetPixelShader() out of memory error.\n"));
		return false;
	}

	if (!pShaders->Init(m_uDeviceList))
	{
		DPF(1, _T("CD3DTest:GetPixelShader() - CnPixelShader::Init() failed.\n"));
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		CPShader* pShader;
		hr = m_DeviceList[i]->GetPixelShader(&pShader);

		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::GetPixelShader() - GetPixelShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			return false;
		}
		if(!pShaders->SetPShader(pShader, i))
		{
			DPF(1, _T("CD3DTest::GetPixelShader() - CnPixelShader::SetPShader failed on device#%d.\n"), i);
			bRes= false;
			break;
		}
	}

	if(bRes)
		*ppShader = pShaders;
	else
		RELEASE(pShaders);

	return bRes;
}


bool CD3DTest::DeletePixelShader(DWORD dwHandle, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	
	// Find the handle
	if(!(uHandles = m_PixelShaderHandles.GetHandle(dwHandle)))
	{
		DPF(1, _T("CD3DTest::DeletePixelShader() - invalid handle(%x).\n"), dwHandle);
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->DeletePixelShader((CPShader*)*(uHandles + i));
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DeletePixelShader() - DeletePixelShader failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetPixelShaderConstantF(UINT StartRegister,const float* pConstantData,UINT Vector4fCount,DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->SetPixelShaderConstant(StartRegister, pConstantData, Vector4fCount);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetPixelShaderConstant() - SetPixelShaderConstant failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetPixelShaderConstantF(UINT StartRegister,float* pConstantData,UINT Vector4fCount, DWORD dwFlags)
{

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		return (SUCCEEDED( m_DeviceList[i]->GetPixelShaderConstant(StartRegister, pConstantData, Vector4fCount) ) );
	}

	return false;
}


bool CD3DTest::SetPixelShaderConstantI(UINT StartRegister,const int* pConstantData,UINT Vector4iCount,DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->SetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetPixelShaderConstantI() - SetPixelShaderConstantI failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetPixelShaderConstantI(UINT StartRegister,int* pConstantData,UINT Vector4iCount, DWORD dwFlags)
{
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		return (SUCCEEDED( m_DeviceList[i]->GetPixelShaderConstantI(StartRegister, pConstantData, Vector4iCount) ) );
	}

	return false;
}


bool CD3DTest::SetPixelShaderConstantB(UINT StartRegister,const BOOL* pConstantData,UINT BoolCount,DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->SetPixelShaderConstantB(StartRegister, pConstantData, BoolCount);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetPixelShaderConstantB() - SetPixelShaderConstantB failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetPixelShaderConstantB(UINT StartRegister,BOOL* pConstantData,UINT BoolCount, DWORD dwFlags)
{
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		return (SUCCEEDED( m_DeviceList[i]->GetPixelShaderConstantB(StartRegister, pConstantData, BoolCount) ) );
	}

	return false;
}


bool CD3DTest::GetPixelShaderFunction(DWORD Handle,void* pData,DWORD* pSizeOfData, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	UINT_PTR *uHandles = NULL;
	
	// Find the handle
	if(!(uHandles = m_PixelShaderHandles.GetHandle(Handle)))
	{
		DPF(1, _T("CD3DTest::GetPixelShaderFunction() - invalid handle(%x).\n"), Handle);
		return false;
	}

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->GetPixelShaderFunction((DWORD)*(uHandles + i), pData, pSizeOfData);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::GetPixelShaderFunction() - GetPixelShaderFunction failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			return false;
		}

		return true;
	}

	return false;
}


bool CD3DTest::DrawRectPatch(UINT uHandle, float *pSegments, RECTPATCH_INFO *pInfo, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->DrawRectPatch(uHandle, pSegments, pInfo);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DrawRectPatch() - DrawRectPatch failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::DrawTriPatch(UINT uHandle, float *pSegments, TRIPATCH_INFO *pInfo, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->DrawTriPatch(uHandle, pSegments, pInfo);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DrawTriPatch() - DrawTriPatch failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::DeletePatch(UINT uHandle, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->DeletePatch(uHandle);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DeletePatch() - DeletePatch failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::SetNPatchMode(float nSegments, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->SetNPatchMode(nSegments);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetNPatchMode() - SetNPatchMode failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


float CD3DTest::GetNPatchMode(DWORD dwFlags)
{
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		return m_DeviceList[i]->GetNPatchMode();
	}

	DPF(1, _T("CD3DTest::GetNPatchMode() - no devices were specified"));
	return -1.f;
}


bool CD3DTest::GetDirect3D(CD3D **ppD3D, DWORD dwFlags)
{
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;
		
		return (SUCCEEDED( m_DeviceList[i]->GetDirect3D(ppD3D) ) );
	}

	return false;
}


void CD3DTest::SetFont( LOGFONT lf, DWORD dwFlags)
{
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		m_DeviceList[i]->SetFont(lf);
	}
}


bool CD3DTest::CreateTexture(
	UINT Width,
	UINT Height,
	UINT Levels,
	DWORD Usage,
	FORMAT Format,
	DWORD Pool,
	LPTEXTURES* ppTexture,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPTEXTURE pTexture = NULL;

	if( ppTexture == NULL )
	{
		DPF(1, _T("CD3DTest:CreateTexture invalid parameter.\n") );
		return false;
	}

	LPTEXTURES pTextures = new CnTexture;
	if( pTextures == NULL )
	{
		DPF(1, _T("CD3DTest:CreateTexture out of memory error.\n") );
		return false;
	}
	if( !pTextures->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:CreateTexture out of memory error.\n") );
		RELEASE( pTextures );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateTexture(Width,Height,Levels,Usage,Format,Pool,&pTexture);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateTexture() - "
					"CreateTexture failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pTextures->SetTexture( pTexture, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pTextures );
		return bRes;
	}

	*ppTexture = pTextures;
	return bRes;
}


bool CD3DTest::CreateVolumeTexture(
	UINT Width,
	UINT Height,
	UINT Depth,
	UINT Levels,
	DWORD Usage,
	FORMAT Format,
	DWORD Pool,
	LPVOLUMETEXTURES* ppVolumeTexture,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPVOLUMETEXTURE pTexture = NULL;

	if( ppVolumeTexture == NULL )
	{
		DPF(1, _T("CD3DTest:CreateVolumeTexture invalid parameter.\n") );
		return false;
	}

	LPVOLUMETEXTURES pTextures = new CnVolTexture;
	if( pTextures == NULL )
	{
		DPF(1, _T("CD3DTest:CreateVolumeTexture out of memory error.\n") );
		return false;
	}
	
	if( !pTextures->Init(m_uDeviceList) )
	{
		DPF(1, _T("CD3DTest:CreateVolumeTexture out of memory error.\n") );
		RELEASE( pTextures );
		return false;
	}

	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateVolumeTexture(Width,Height,Depth,Levels,Usage,Format,Pool,&pTexture);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateVolumeTexture() - "
					"CreateVolumeTexture failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pTextures->SetBaseTexture( (CBaseTexture*) pTexture, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pTextures );
		return bRes;
	}

	*ppVolumeTexture = pTextures;
	return bRes;
}


bool CD3DTest::CreateCubeTexture(
	UINT EdgeLength,
	UINT Levels,
	DWORD Usage,
	FORMAT Format,
	DWORD Pool,
	LPCUBETEXTURES* ppCubeTexture,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPCUBETEXTURE pTexture = NULL;

	if( ppCubeTexture == NULL )
	{
		DPF(1, _T("CD3DTest:CreateCubeTexture invalid parameter.\n") );
		return false;
	}

	LPCUBETEXTURES pTextures = new CnCubeTexture;
	if( pTextures == NULL )
	{
		DPF(1, _T("CD3DTest:CreateCubeTexture out of memory error.\n") );
		return false;
	}

	if( !pTextures->Init( m_uDeviceList) )
	{
		DPF(1, _T("CD3DTest:CreateCubeTexture out of memory error.\n") );
		RELEASE( pTextures );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateCubeTexture(EdgeLength,Levels,Usage,Format,Pool,&pTexture);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateCubeTexture() - "
					"CreateCubeTexture failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pTextures->SetBaseTexture( (CBaseTexture*) pTexture, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pTextures );
		return bRes;
	}

	*ppCubeTexture = pTextures;
	return bRes;
}


bool CD3DTest::SetTexture(DWORD dwStage, LPBASETEXTURES pTextures, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPBASETEXTURE pTexture;
	
	for (i=0; i<m_uDeviceList; i++)
	{
		pTexture = NULL;

		if (SkipDevice(i, dwFlags))
			continue;

		if(NULL == pTextures)
			hr = m_DeviceList[i]->SetTexture(dwStage, NULL);
		else
		{
			if (!pTextures->GetBaseTexture( &pTexture, i ) )
				continue;

			hr = m_DeviceList[i]->SetTexture(dwStage, pTexture);
		}

		if (FAILED(hr))
		{
			DPF(1,_T("CD3DTest::SetTexture() - SetTexture failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}

		RELEASE( pTexture );
	}

	return bRes;
}


bool CD3DTest::GetTexture(int nIndex, LPBASETEXTURES* ppTextures, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPBASETEXTURE pTexture = NULL;
	RESOURCETYPE rtType = (RESOURCETYPE)0;

	if( ppTextures == NULL )
		return false;

	
	LPBASETEXTURE pTempTexture = NULL;
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		if( SUCCEEDED( m_DeviceList[i]->GetTexture(nIndex,&pTempTexture) ) )
		{
			rtType = (rtType == (RESOURCETYPE)0) ? (pTempTexture->GetType()) : (rtType);
			if( rtType != pTempTexture->GetType() )
			{
				DPF(1, _T("CD3DTest:GetTexture multiple texture types found.\n") );
				return false;
			}
		}
	}

	LPBASETEXTURES pTextures = NULL;
	if( rtType == RTYPE_TEXTURE )
		pTextures = (LPBASETEXTURES) new CnTexture;
	else if ( rtType == RTYPE_VOLUMETEXTURE )
		pTextures = (LPBASETEXTURES) new CnVolTexture;
	else if ( rtType == RTYPE_CUBETEXTURE )
		pTextures = (LPBASETEXTURES) new CnCubeTexture;
	else
	{
		DPF(1, _T("CD3DTest:GetTexture unknown texture type.\n") );
		return false;
	}

	if( pTextures == NULL )
	{
		DPF(1, _T("CD3DTest:GetTexture out of memory error.\n") );
		return false;
	}
	
	if( !pTextures->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:GetTexture out of memory error.\n") );
		RELEASE( pTextures );
		return false;
	}

	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->GetTexture(nIndex,&pTexture);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::GetTexture() - "
					"GetTexture failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pTextures->SetBaseTexture( (CBaseTexture*)pTexture, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pTextures );
		return bRes;
	}

	*ppTextures = (LPBASETEXTURES)pTextures;
	return bRes;
}


bool CD3DTest::UpdateTexture(LPBASETEXTURES pSourceTextures, LPBASETEXTURES pDestinationTextures, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPBASETEXTURE pSourceTexture = NULL;
	LPBASETEXTURE pDestinationTexture = NULL;

	if( pSourceTextures == NULL || pDestinationTextures == NULL)
	{
		DPF(1, _T("CD3DTest:SetTexture invalid parameter.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		if (!pSourceTextures->GetBaseTexture( &pSourceTexture, i ))
			continue;

		if (!pDestinationTextures->GetBaseTexture( &pDestinationTexture, i ))
		{
			RELEASE(pSourceTexture);
			continue;
		}

		hr = m_DeviceList[i]->UpdateTexture(pSourceTexture,pDestinationTexture);

		RELEASE( pDestinationTexture );
		RELEASE( pSourceTexture );

		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::UpdateTexture() - "
					"UpdateTexture failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::CreateAdditionalSwapChain(PRESENT_PARAMETERS* pParams, LPSWAPCHAINS* ppSwapChain, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPSWAPCHAIN pSwapChain = NULL;

	if( ppSwapChain == NULL )
	{
		DPF(1, _T("CD3DTest:CreateAdditionalSwapChain invalid parameter.\n") );
		return false;
	}

	LPSWAPCHAINS pSwapChains = new CnSwapChain;
	if( pSwapChains == NULL )
	{
		DPF(1, _T("CD3DTest:CreateAdditionalSwapChain out of memory error.\n") );
		return false;
	}
	if( !pSwapChains->Init(m_uDeviceList) )
	{
		DPF(1, _T("CD3DTest:CreateAdditionalSwapChain out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateAdditionalSwapChain(pParams,&pSwapChain);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateAdditionalSwapChain() - "
					"CreateAdditionalSwapChain failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSwapChains->SetSwapChain( pSwapChain, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSwapChains );
		return bRes;
	}

	*ppSwapChain = pSwapChains;
	return bRes;
}


bool CD3DTest::CreateVertexBuffer(
	UINT Length,
	DWORD Usage,
	DWORD FVF,
	DWORD Pool,
	LPVERTEXBUFFERS* ppVertexBuffers,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPVERTEXBUFFER pVertexBuffer = NULL;

	if( ppVertexBuffers == NULL )
	{
		DPF(1, _T("CD3DTest:CreateVertexBuffer invalid parameter.\n") );
		return false;
	}

	LPVERTEXBUFFERS pVertexBuffers = new CnVertexBuffer;
	if( pVertexBuffers == NULL )
	{
		DPF(1, _T("CD3DTest:CreateVertexBuffer out of memory error.\n") );
		return false;
	}
	if( !pVertexBuffers->Init(m_uDeviceList) )
	{
		DPF(1, _T("CD3DTest:CreateVertexBuffer out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateVertexBuffer(Length,Usage,FVF,Pool,&pVertexBuffer);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateVertexBuffer() - "
					"CreateVertexBuffer failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pVertexBuffers->SetVertexBuffer( pVertexBuffer, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pVertexBuffers );
		return bRes;
	}

	*ppVertexBuffers = pVertexBuffers;
	return bRes;
}


bool CD3DTest::CreateIndexBuffer(
	UINT Length,
	DWORD Usage,
	FMT Format,
	DWORD Pool, 
	LPINDEXBUFFERS* ppIndexBuffers,
	DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPINDEXBUFFER pIndexBuffer = NULL;

	if( ppIndexBuffers == NULL )
	{
		DPF(1, _T("CD3DTest:CreateIndexBuffer invalid parameter.\n") );
		return false;
	}

	LPINDEXBUFFERS pIndexBuffers = new CnIndexBuffer;
	if( pIndexBuffers == NULL )
	{
		DPF(1, _T("CD3DTest:CreateIndexBuffer out of memory error.\n") );
		return false;
	}
	if( !pIndexBuffers->Init(m_uDeviceList) )
	{
		DPF(1, _T("CD3DTest:CreateIndexBuffer out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateIndexBuffer(Length,Usage,Format,Pool,&pIndexBuffer);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateIndexBuffer() - "
					"CreateIndexBuffer failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pIndexBuffers->SetIndexBuffer( pIndexBuffer, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pIndexBuffers );
		return bRes;
	}

	*ppIndexBuffers = pIndexBuffers;
	return bRes;
}


bool CD3DTest::ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,LPVERTEXBUFFERS pDestBuffer,DWORD dwPVFlags, DWORD dwDeviceFlags)
{
	return ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pDestBuffer, (CnVertexDeclaration*)NULL, dwPVFlags, dwDeviceFlags);
}


bool CD3DTest::ProcessVertices(UINT SrcStartIndex,UINT DestIndex,UINT VertexCount,LPVERTEXBUFFERS pDestBuffer,CnVertexDeclaration *pDestDecl, DWORD dwPVFlags, DWORD dwDeviceFlags)
{
	CVertexBuffer * pVB = NULL;
	CVertexDeclaration * pDecl = NULL;
	HRESULT hr = E_FAIL;
	bool bRes = true;

	if( pDestBuffer == NULL || VertexCount == 0)
	{
		DPF(1, _T("CD3DTest:ProcessVertices invalid parameter.\n") );
		return false;
	}
	
	for (UINT i = 0; i < m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwDeviceFlags))
			continue;

		if (!pDestBuffer->GetVertexBuffer(&pVB, i))
			continue;
		
		if(pDestDecl && !pDestDecl->GetVDecl(&pDecl, i))
			continue;
		
		hr = m_DeviceList[i]->ProcessVertices(SrcStartIndex, DestIndex, VertexCount, pVB, pDecl, dwPVFlags);

		RELEASE( pVB );
		RELEASE( pDecl );

		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::ProcessVertices() - "
					"ProcessVertices failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}

	}
		
	return bRes;
}


bool CD3DTest::DrawPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT StartVertex,UINT PrimitiveCount, DWORD dwDeviceFlags)
{
	HRESULT hr = E_FAIL;
	bool bRes = true;

	if( PrimitiveCount == 0)
	{
		DPF(1, _T("CD3DTest:DrawPrimitive invalid parameter.\n") );
		return false;
	}
	
	for (UINT i = 0; i < m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwDeviceFlags))
			continue;

		hr = m_DeviceList[i]->DrawPrimitive(PrimitiveType, StartVertex, PrimitiveCount);

		if (FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DrawPrimitive() - DrawPrimitive failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}

	}
		
	return bRes;
}


bool CD3DTest::DrawIndexedPrimitive(D3DPRIMITIVETYPE PrimitiveType,UINT MinIndex,UINT NumVertices,UINT StartIndex,UINT PrimitiveCount,INT BaseVertexIndex,DWORD dwDeviceFlags)
{
	HRESULT hr = E_FAIL;
	bool bRes = true;

	if(NumVertices == 0 || PrimitiveCount == 0)
	{
		DPF(1, _T("CD3DTest:DrawIndexedPrimitive invalid parameter.\n") );
		return false;
	}
	
	for (UINT i = 0; i < m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwDeviceFlags))
			continue;

		hr = m_DeviceList[i]->DrawIndexedPrimitive(PrimitiveType, MinIndex, NumVertices, StartIndex, PrimitiveCount, BaseVertexIndex);

		if (FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DrawIndexedPrimitive() - DrawIndexedPrimitive failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}

	}
		
	return bRes;
}


bool CD3DTest::DrawPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT PrimitiveCount,void* pVertexStreamZeroData,UINT VertexStreamZeroStride, DWORD dwDeviceFlags)
{
	HRESULT hr = E_FAIL;
	bool bRes = true;

	if(pVertexStreamZeroData == 0 || PrimitiveCount == 0)
	{
		DPF(1, _T("CD3DTest:DrawPrimitiveUP invalid parameter.\n") );
		return false;
	}
	
	for (UINT i = 0; i < m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwDeviceFlags))
			continue;

		hr = m_DeviceList[i]->DrawPrimitiveUP(PrimitiveType, PrimitiveCount, pVertexStreamZeroData, VertexStreamZeroStride);

		if (FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DrawPrimitiveUP() - DrawPrimitiveUP failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}

	}
		
	return bRes;
}


bool CD3DTest::DrawIndexedPrimitiveUP(D3DPRIMITIVETYPE PrimitiveType,UINT MinVertexIndex,UINT NumVertexIndices,UINT PrimitiveCount,void* pIndexData,FORMAT IndexDataFormat,void* pVertexStreamZeroData,UINT VertexStreamZeroStride, DWORD dwDeviceFlags)
{
	HRESULT hr = E_FAIL;
	bool bRes = true;

	if(pVertexStreamZeroData == 0 || PrimitiveCount == 0)
	{
		DPF(1, _T("CD3DTest:DrawIndexedPrimitiveUP invalid parameter.\n") );
		return false;
	}
	
	for (UINT i = 0; i < m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwDeviceFlags))
			continue;

		hr = m_DeviceList[i]->DrawIndexedPrimitiveUP(PrimitiveType, MinVertexIndex, NumVertexIndices, PrimitiveCount, pIndexData, IndexDataFormat, pVertexStreamZeroData, VertexStreamZeroStride);

		if (FAILED(hr))
		{
			DPF(1, _T("CD3DTest::DrawIndexedPrimitiveUP() - DrawIndexedPrimitiveUP failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}

	}
		
	return bRes;
}


bool CD3DTest::DrawPrimitiveUserMemory(D3DPRIMITIVETYPE PrimitiveType, DWORD dwVertexTypeDesc,LPVOID lpVertices, DWORD  dwVertexCount, LPVOID lpIndices, DWORD  dwIndexCount, DWORD  dwFlags, bool bIs32BitIndices, DWORD dwDeviceFlags)
{
	bool bRes = true;

	if(lpVertices == 0)
	{
		DPF(1, _T("CD3DTest:DrawPrimitiveUserMemory invalid parameter.\n") );
		return false;
	}
	
	for (UINT i = 0; i < m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwDeviceFlags))
			continue;

		if (!m_DeviceList[i]->DrawPrimitiveUserMemory(PrimitiveType, dwVertexTypeDesc, lpVertices, dwVertexCount, lpIndices, dwIndexCount, dwFlags, bIs32BitIndices))
		{
			DPF(1, _T("CD3DTest::DrawPrimitiveUserMemory() - DrawPrimitiveUserMemory failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(GetLastError()));
			bRes= false;
		}

	}
		
	return bRes;
}


bool CD3DTest::DrawPrimitiveVertexBuffer(D3DPRIMITIVETYPE PrimitiveType,CnVertexBuffer *pSrcBuffer, DWORD dwStartVertex, DWORD dwNumVertices, LPWORD lpwIndices, DWORD dwIndexCount, DWORD dwFlags, DWORD dwUsage, DWORD dwPool, bool bIs32BitIndices, DWORD dwDeviceFlags)
{
	CVertexBuffer * pVB = NULL;
	bool bRes = true;

	if(pSrcBuffer == 0)
	{
		DPF(1, _T("CD3DTest:DrawPrimitiveVertexBuffer invalid parameter.\n") );
		return false;
	}
	
	for (UINT i = 0; i < m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwDeviceFlags))
			continue;

		if (!pSrcBuffer->GetVertexBuffer(&pVB, i))
			continue;

		if (!m_DeviceList[i]->DrawPrimitiveVertexBuffer(PrimitiveType, pVB, dwStartVertex, dwNumVertices, lpwIndices, dwIndexCount, dwFlags, dwUsage, dwPool, bIs32BitIndices))
		{
			DPF(1, _T("CD3DTest::DrawPrimitiveVertexBuffer() - DrawPrimitiveVertexBuffer failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(GetLastError()));
			bRes= false;
		}

		RELEASE(pVB);
	}
		
	return bRes;
}


bool CD3DTest::DrawPrimitiveStridedData(D3DPRIMITIVETYPE PrimitiveType,DWORD dwVertexTypeDesc,LPDRAWPRIMITIVESTRIDEDDATA lpVertexArray,DWORD dwVertexCount,LPWORD lpwIndices,DWORD dwIndexCount,DWORD dwFlags,DWORD dwUsage,DWORD dwPool,bool bIs32BitIndices, DWORD dwDeviceFlags)
{
	bool bRes = true;

	if(lpVertexArray == 0)
	{
		DPF(1, _T("CD3DTest:DrawPrimitiveStrided invalid parameter.\n") );
		return false;
	}
	
	for (UINT i = 0; i < m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwDeviceFlags))
			continue;

		if (!m_DeviceList[i]->DrawPrimitiveStridedData(PrimitiveType, dwVertexTypeDesc, lpVertexArray, dwVertexCount, lpwIndices, dwIndexCount, dwFlags, dwUsage, dwPool, bIs32BitIndices))
		{
			DPF(1, _T("CD3DTest::DrawPrimitiveStrided() - DrawPrimitiveStrided failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(GetLastError()));
			bRes= false;
		}
	}
		
	return bRes;
}


bool CD3DTest::SetScissorRect(CONST RECT *pRect, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->SetScissorRect(pRect);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetScissorRect() - SetScissorRect failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetScissorRect(RECT *pRect, DWORD dwFlags)
{
	// search for first available device
	for (UINT i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetScissorRect(pRect)));
	}

	return false;
}


bool CD3DTest::SetSamplerState(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD Value, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->SetSamplerState(Sampler, Type, Value);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetSamplerState() - SetSamplerState failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
	}

	return bRes;
}


bool CD3DTest::GetSamplerState(DWORD Sampler, SAMPLERSTATETYPE Type, DWORD *Value, DWORD dwFlags)
{
	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		return (SUCCEEDED(m_DeviceList[i]->GetSamplerState(Sampler,Type,Value)));
	}

	return false;
}


bool CD3DTest::CreateQuery(QUERYTYPE Type, LPQUERYS* ppQuery, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;
	LPQUERY pQuery = NULL;

	if( ppQuery == NULL )
	{
		DPF(1, _T("CD3DTest:CreateQuery invalid parameter.\n") );
		return false;
	}

	LPQUERYS pQuerys = new CnQuery;
	if( pQuerys == NULL )
	{
		DPF(1, _T("CD3DTest:CreateQuery out of memory error.\n") );
		return false;
	}
	if( !pQuerys->Init(m_uDeviceList) )
	{
		DPF(1, _T("CD3DTest:CreateQuery out of memory error.\n") );
		return false;
	}
	
	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;
		
		hr = m_DeviceList[i]->CreateQuery(Type,&pQuery);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::CreateQuery() - CreateQuery failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pQuerys->SetQuery( pQuery, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pQuerys );
		return bRes;
	}

	*ppQuery = pQuerys;
	return bRes;
}


bool CD3DTest::SetSoftwareVertexProcessing( BOOL bSoftware, DWORD dwFlags )
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for( UINT i = 0; i < m_uDeviceList; ++i )
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		hr = m_DeviceList[i]->SetSoftwareVertexProcessing( bSoftware );
		if( FAILED( hr ) )
		{
			DPF(1, _T("CD3DTest::SetSoftwareVertexProcessing() - SetSoftwareVertexProcessing failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes = false;
		}
	}

	return bRes;
}


BOOL CD3DTest::GetSoftwareVertexProcessing( DWORD dwFlags )
{
	for( UINT i = 0; i < m_uDeviceList; ++i )
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		return m_DeviceList[i]->GetSoftwareVertexProcessing();
	}

	return FALSE;
}


UINT CD3DTest::GetNumberOfSwapChains( DWORD dwFlags )
{
	for( UINT i = 0; i < m_uDeviceList; ++i )
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		return m_DeviceList[i]->GetNumberOfSwapChains();
	}

	return 0;
}


bool CD3DTest::GetSwapChain( UINT iSwapChain, LPSWAPCHAINS* ppSwapChain, DWORD dwFlags )
{
	bool bRes = true;
	HRESULT hr = E_FAIL;
	LPSWAPCHAIN pSwapChain = NULL;

	if( ppSwapChain == NULL )
	{
		DPF(1, _T("CD3DTest:GetSwapChain invalid parameter.\n") );
		return false;
	}

	LPSWAPCHAINS pSwapChains = new CnSwapChain;
	if( pSwapChains == NULL || !pSwapChains->Init(m_uDeviceList) )
	{
		DPF(1, _T("CD3DTest:GetSwapChain out of memory error.\n") );
		return false;
	}
	
	for( UINT i = 0; i < m_uDeviceList; ++i )
	{
		if( SkipDevice( i, dwFlags ) )
			continue;

		hr = m_DeviceList[i]->GetSwapChain( iSwapChain, &pSwapChain );
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::GetSwapChain() - "
					"GetSwapChain failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pSwapChains->SetSwapChain( pSwapChain, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pSwapChains );
		return bRes;
	}

	*ppSwapChain = pSwapChains;

	return bRes;
}


bool CD3DTest::SetStreamSourceFreq(UINT nStreamNumber, UINT nDivider, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->SetStreamSourceFreq(nStreamNumber, nDivider);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::SetStreamSourceFreq() - SetStreamSourceFreq failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes = false;
		}
	}
	
	return bRes;
}


bool CD3DTest::GetStreamSourceFreq(UINT nStreamNumber, UINT* pDividerOut, DWORD dwFlags)
{
	bool bRes = true;
	HRESULT hr = E_FAIL;

	for(UINT i=0; i<m_uDeviceList; i++)
	{
		if(SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->GetStreamSourceFreq(nStreamNumber, pDividerOut);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::GetStreamSourceFreq() - GetStreamSourceFreq failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes = false;
		}
	}
	
	return bRes;
}


DWORD CD3DTest::GetVertexSize(DWORD dwFVF)
{
	for (UINT i = 0; i < m_uDeviceList; i++)
	{
		if (m_DeviceList[i] != NULL)
			return m_DeviceList[i]->GetVertexSize(dwFVF);
	}
    
	return 0; 
}


bool CD3DTest::D3DXLoadSurfaceFromSurface(CnSurface* pDestSurface,CONST PALETTEENTRY* pDestPalette, CONST RECT* pDestRect, CnSurface* pSrcSurface, CONST PALETTEENTRY* pSrcPalette, CONST RECT* pSrcRect, DWORD Filter, D3DCOLOR ColorKey, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;

	if( pDestSurface == NULL || pSrcSurface == NULL )
		return false;

	LPSURFACE pSrcSurf = NULL;
	LPSURFACE pDstSurf = NULL;

	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		if(!pDestSurface->GetSurface(&pDstSurf, i) || !pSrcSurface->GetSurface(&pSrcSurf, i))
		{
			DPF(1, _T("CD3DTest::D3DXLoadSurfaceFromSurface() - GetSurface() failed on Surface #%d.\n"),i);
			bRes= false;
			RELEASE(pDstSurf);
			RELEASE(pSrcSurf);
			continue;
		}
		
		hr = m_DeviceList[i]->D3DXLoadSurfaceFromSurface(pDstSurf, pDestPalette, pDestRect, pSrcSurf, pSrcPalette, pSrcRect, Filter, ColorKey);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3DTest::D3DXLoadSurfaceFromSurface() - D3DXLoadSurfaceFromSurface() failed on Surface #%d. hr=%s(%x)\n"),
				i, m_pD3D->HResultToString(hr), hr);
			bRes= false;
		}

		RELEASE(pDstSurf);
		RELEASE(pSrcSurf);
	}

	return bRes;
}


bool CD3DTest::D3DXCreateTextureFromResourceA(HMODULE hSrcModule, LPCSTR pSrcResource, LPTEXTURES* ppTextures, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;

	if( ppTextures == NULL )
		return false;

	LPTEXTURES pTextures = new CnTexture;
	if( pTextures == NULL )
	{
		DPF(1, _T("CD3DTest:D3DXCreateTextureFromResource out of memory error.\n") );
		return false;
	}
	if( !pTextures->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:D3DXCreateTextureFromResource out of memory error.\n") );
		RELEASE( pTextures );
		return false;
	}


	LPTEXTURE pTexture;

	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->D3DXCreateTextureFromResourceA(hSrcModule, pSrcResource, &pTexture);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::D3DXCreateTextureFromResource() - "
					"CreateRenderTarget failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pTextures->SetTexture( pTexture, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pTextures );
		return bRes;
	}

	*ppTextures = pTextures;
	return bRes;
}


bool CD3DTest::D3DXCreateTextureFromResourceExA(HMODULE hSrcModule, LPCSTR pSrcResource, UINT Width, UINT Height, UINT MipLevels, DWORD Usage, D3DFORMAT Format, D3DPOOL Pool, DWORD Filter, DWORD MipFilter, D3DCOLOR ColorKey, D3DXIMAGE_INFO* pScrInfo, PALETTEENTRY* pPalette, LPTEXTURES* ppTextures, DWORD dwFlags)
{
	bool bRes = true;
	UINT i = 0;
	HRESULT hr = E_FAIL;

	if( ppTextures == NULL )
		return false;

	LPTEXTURES pTextures = new CnTexture;
	if( pTextures == NULL )
	{
		DPF(1, _T("CD3DTest:D3DXCreateTextureFromResource out of memory error.\n") );
		return false;
	}
	if( !pTextures->Init( m_uDeviceList ) )
	{
		DPF(1, _T("CD3DTest:D3DXCreateTextureFromResource out of memory error.\n") );
		RELEASE( pTextures );
		return false;
	}


	LPTEXTURE pTexture = NULL;

	for (i=0; i<m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwFlags))
			continue;

		hr = m_DeviceList[i]->D3DXCreateTextureFromResourceExA(hSrcModule, pSrcResource, Width, Height, MipLevels, Usage, Format, Pool, Filter, MipFilter, ColorKey, pScrInfo, pPalette, &pTexture);
		if (FAILED(hr))
		{
			DPF(1,
				_T("CD3DTest::D3DXCreateTextureFromResource() - "
					"CreateRenderTarget failed on device#%d. hr=%s\n"),
				i, m_pD3D->HResultToString(hr));
			bRes= false;
		}
		else
			pTextures->SetTexture( pTexture, i );
	}

	// If Any of the intended calls failed, fail them all
	if( !bRes )
	{
		RELEASE( pTextures );
		return bRes;
	}

	*ppTextures = pTextures;
	return bRes;
}


bool CD3DTest::CreateEffectFromFile (LPCTSTR pSrcFile, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPEFFECTS* ppEffects, LPD3DXBUFFER* ppCompilationErrors, DWORD dwDevFlags)
{
	bool bRes = true;

	//validate arguments
	if (NULL == ppEffects)
		return false;

	//create new CnEffect only if *ppEffects == NULL
	if (NULL == *ppEffects)
	{
		*ppEffects = new CnEffect();
		if (NULL == *ppEffects)
		{
			DPF(1, _T("CD3DTest:CreateEffectFromFile out of memory error.\n") );
			return false;
		}
		else if (!(*ppEffects)->Init (m_uDeviceList))
		{
			DPF(1, _T("CD3DTest:CreateEffectFromFile could not initialize CnEffect.\n") );
			RELEASE (*ppEffects);
			return false;
		}
	}
	
	LPEFFECT pEffect = NULL;
	HRESULT hr = E_FAIL;

	//loop through the devices
	for (UINT i = 0; i < m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwDevFlags))
			continue;

		hr = m_DeviceList[i]->CreateEffectFromFile (pSrcFile, pDefines, pInclude, dwFlags, pPool, &pEffect, ppCompilationErrors);
		if (FAILED (hr) || NULL == pEffect)
		{
			DPF(1, _T("CD3DTest:CreateEffectFromFile failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes = false;
		}
		else
		{
			//SetEffect does not increase the ref count, so no need to release pEffect
			(*ppEffects)->SetEffect (pEffect, i);
			pEffect = NULL;
		}
	}

	//if anything failed, free the effect
	if (!bRes)
	{
		RELEASE (*ppEffects);
		return bRes;
	}

	return bRes;
}


bool CD3DTest::CreateEffectFromResource (HMODULE hSrcModule, LPCTSTR pSrcresource, CONST D3DXMACRO* pDefines, LPD3DXINCLUDE pInclude, DWORD dwFlags, LPD3DXEFFECTPOOL pPool, LPEFFECTS* ppEffects, LPD3DXBUFFER* ppCompilationErrors, DWORD dwDevFlags)
{
	bool bRes = true;

	if( ppCompilationErrors != NULL )
		*ppCompilationErrors = NULL;

	//validate arguments
	if (NULL == ppEffects)
		return false;

	//create new CnEffect only if *ppEffects == NULL
	if (NULL == *ppEffects)
	{
		*ppEffects = new CnEffect();
		if (NULL == *ppEffects)
		{
			DPF(1, _T("CD3DTest:CreateEffectFromFile out of memory error.\n") );
			return false;
		}
		else if (!(*ppEffects)->Init (m_uDeviceList))
		{
			DPF(1, _T("CD3DTest:CreateEffectFromFile could not initialize CnEffect.\n") );
			RELEASE (*ppEffects);
			return false;
		}
	}

	LPEFFECT pEffect = NULL;
	HRESULT hr = E_FAIL;

	//loop through the devices
	for (UINT i = 0; i < m_uDeviceList; i++)
	{
		if (SkipDevice(i, dwDevFlags))
			continue;

		if( ppCompilationErrors != NULL )
			RELEASE( *ppCompilationErrors );		

		hr = m_DeviceList[i]->CreateEffectFromResource (hSrcModule, pSrcresource, pDefines, pInclude, dwFlags, pPool, &pEffect, ppCompilationErrors);
		if (FAILED (hr) || NULL == pEffect)
		{
			DPF(1, _T("CD3DTest:CreateEffectFromFile failed on device#%d. hr=%s\n"), i, m_pD3D->HResultToString(hr));
			bRes = false;
		}
		else
		{
			//SetEffect does not increase the ref count, so no need to release pEffect
			(*ppEffects)->SetEffect (pEffect, i);
			pEffect = NULL;
		}
	}

	//if anything failed, free the effect
	if (!bRes)
	{
		RELEASE (*ppEffects);
		return bRes;
	}

	return bRes;
}


bool CD3DTest::ComposeRects( LPSURFACES ppSrc, LPSURFACES ppDst, LPVERTEXBUFFERS ppSrcRectDescs, UINT NumRects, LPVERTEXBUFFERS ppDstRectDescs, UINT Operation, int Xoffset, int Yoffset, DWORD dwFlags )
{
	bool bRes = true;
	
	for( UINT i = 0; i < m_uDeviceList; i++ )
	{
		LPSURFACE pSrc = NULL, pDst = NULL;
		LPVERTEXBUFFER pSrcRectDescs = NULL, pDstRectDescs = NULL;
		
		if( SkipDevice( i, dwFlags ) )
			continue;
		
		if( !ppSrc->GetSurface( &pSrc, i ) ||
			!ppDst->GetSurface( &pDst, i) ||
			!ppSrcRectDescs->GetVertexBuffer( &pSrcRectDescs, i ) ||
			!ppDstRectDescs->GetVertexBuffer( &pDstRectDescs, i ) )
			continue;

		HRESULT hr = m_DeviceList[i]->ComposeRects( pSrc, pDst, pSrcRectDescs, NumRects, pDstRectDescs, Operation, Xoffset, Yoffset );
		if( FAILED( hr ) )
		{
			DPF(1,_T("CD3DTest::ComposeRects() - ComposeRects failed on device#%d. hr =%s \n"),
				i, m_pD3D->HResultToString( hr ) );
			bRes = false;
		}
		
		RELEASE( pSrc );
		RELEASE( pDst );
		RELEASE( pSrcRectDescs );
		RELEASE( pDstRectDescs );
	}

	return bRes;
}


bool CD3DTest::SetConvolutionMonoKernel( UINT Width, UINT Height, float* RowWeights, float* ColumnWeights, DWORD dwFlags )
{
	bool bRes = true;
	
	for( UINT i = 0; i < m_uDeviceList; i++ )
	{
		if( SkipDevice( i, dwFlags ) )
			continue;
		
		HRESULT hr = m_DeviceList[i]->SetConvolutionMonoKernel( Width, Height, RowWeights, ColumnWeights );
		if( FAILED( hr ) )
		{
			DPF(1,_T("CD3DTest::SetConvolutionMonoKernel() - SetConvolutionMonoKernel failed on device#%d. hr =%s \n"),
				i, m_pD3D->HResultToString( hr ) );
			bRes = false;
		}
	}

	return bRes;
}
