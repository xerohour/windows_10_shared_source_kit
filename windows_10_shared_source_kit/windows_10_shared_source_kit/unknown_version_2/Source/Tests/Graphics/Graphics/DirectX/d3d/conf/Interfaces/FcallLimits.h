esolvedBehaviors &= ~DEVICECREATE_HWVP;
			ResolvedBehaviors &= ~DEVICECREATE_MIXED;
		};
	};


	// Make sure that there's some sort of device subtype.
	if ( (ResolvedBehaviors & 0xF00) == 0 )
	{
		ResolvedBehaviors |= DEVICECREATE_STANDARD;
	};


	//
	// Fill in our outgoing parameter.
	//

	*pfResolvedBehaviors = ResolvedBehaviors;

	return true;
};

// --------------------------------------------------------------------------------------

bool CD3D::DetectCaps9(CDevice* pDevice)
{
	HRESULT		hr;
	CAPS    	DevCaps;
	DWORD 		dwCount;
	FORMAT		**pFmt;
	bool		bRawCaps;
		
	if(m_pSrcD3D9 == NULL || pDevice == NULL)
	{
		DPF(1, _T("CD3D::DetectCaps9() - invalid pointer.\n"));
		return false;
	}

	// Get the adapter for this device
	int nAdapter = pDevice->GetAdapterID();
	ADAPTER *pAdapter = pDevice->GetAdapter();
	if(pAdapter == NULL)
	{
		DPF(1, _T("CD3D::DetectCaps9() - invalid adapter ID(#%d).\n"), nAdapter);
		return false;
	}

	// Get the device caps.  If there's an actual device available, use it otherwise use the raw adapter caps
	if(pDevice->m_pDevice9)
	{
		bRawCaps = false;

		hr = pDevice->GetDeviceCaps(&DevCaps);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3D::DetectCaps9() - GetDeviceCaps() failed. hr=%s(%x)\n"), HResultToString(hr), hr);
			return false;
		}
	}
	else
	{
		bRawCaps = true;

		hr = GetDeviceCaps(nAdapter, pDevice->GetDevType(), &DevCaps);
		if(FAILED(hr))
		{
			DPF(1, _T("CD3D::DetectCaps9() - GetDeviceCaps() failed. hr=%s(%x)\n"), HResultToString(hr), hr);
			return false;
		}

		// Check DDI level - we need raw caps for this
		if (DevCaps.DevCaps2 & D3DDEVCAPS2_STREAMOFFSET)
			pDevice->m_nDDILevel = 9;
		else if (DevCaps.MaxStreams > 0)
			pDevice->m_nDDILevel = 8;
		else if(DevCaps.DevCaps & D3DDEVCAPS_DRAWPRIMITIVES2EX)
			pDevice->m_nDDILevel = 7;
		else if(DevCaps.DevCaps & D3DDEVCAPS_DRAWPRIMITIVES2)
			pDevice->m_nDDILevel = 6;
		else if(DevCaps.DevCaps & D3DDEVCAPS_DRAWPRIMTLVERTEX)
			pDevice->m_nDDILevel = 5;
	}

	// Check the device caps
	if(DevCaps.DeviceType == D3DDEVTYPE_HAL && (DevCaps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT))
	   pAdapter->bHardwareVP = true;
	if(DevCaps.DeviceType == D3DDEVTYPE_HAL && (DevCaps.DevCaps & D3DDEVCAPS_PUREDEVICE))
		pAdapter->bPure = true;
	if(DevCaps.DevCaps & D3DDEVCAPS_TEXTURENONLOCALVIDMEM)
		pAdapter->eBusType = AGPBUS;
	else
		pAdapter->eBusType = PCIBUS;


	// Save the cap information
	pDevice->m_Desc.dwCaps = DevCaps.Caps;
	pDevice->m_Desc.dwCaps2 = DevCaps.Caps2;
	pDevice->m_Desc.dwCaps3 = DevCaps.Caps3;
	pDevice->m_Desc.dwPresentationIntervals = DevCaps.PresentationIntervals;
	pDevice->m_Desc.dwCursorCaps = DevCaps.CursorCaps;
	pDevice->m_Desc.dwDevCaps = DevCaps.DevCaps;
	pDevice->m_Desc.dwDevCaps2 = DevCaps.DevCaps2;
	pDevice->m_Desc.dpcTriCaps.dwMiscCaps = DevCaps.PrimitiveMiscCaps;
	pDevice->m_Desc.dpcTriCaps.dwRasterCaps = DevCaps.RasterCaps;
	pDevice->m_Desc.dpcTriCaps.dwZCmpCaps = DevCaps.ZCmpCaps;
	pDevice->m_Desc.dpcTriCaps.dwSrcBlendCaps = DevCaps.SrcBlendCaps;
	pDevice->m_Desc.dpcTriCaps.dwDestBlendCaps = DevCaps.DestBlendCaps;
	pDevice->m_Desc.dpcTriCaps.dwAlphaCmpCaps = DevCaps.AlphaCmpCaps;
	pDevice->m_Desc.dpcTriCaps.dwShadeCaps = DevCaps.ShadeCaps;
	pDevice->m_Desc.dpcTriCa