#include "SharedResourceChild.h"
#include "CreateDevice.h"

extern SharedResourcesTestApp g_App;

TEST_RESULT SharedResourcesChild::Setup()
{
	TEST_RESULT tr = CD3D11Test::Setup( );
	if( tr != RESULT_PASS )
		return tr;

	//create 9 object
	if (!GetFramework()->IsSession0() && !GetFramework()->IsMobile())
	{
		HRESULT hr = S_FALSE;
		hr = Direct3DCreate9Ex(D3D_SDK_VERSION, &m_pD3D9);

		if (hr != S_OK)
		{
			WriteToLog(_T("SharedResourcesChild::Setup() - Direct3DCreate9Ex() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
			return RESULT_FAIL;
		}
	}

	m_isChildProcess = true;
	
	return tr;
}

TEST_RESULT SharedResourcesChild::SetupTestCase()
{	
	//minimize window
	if (!GetFramework()->IsMobile())
	{
		ShowWindow(GetActiveWindow(), SW_MINIMIZE);
	}
	return RESULT_PASS;
}

TEST_RESULT SharedResourcesChild::ExecuteTestCase()
{
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	IDXGIKeyedMutex *pDXGIKeyedMutex = NULL;

	//Open the file mapping to the shared memory
	m_hFileMap = ::OpenFileMapping(	FILE_MAP_ALL_ACCESS, 
									FALSE, 
									_T("SharedResourcesMem") );

	if( NULL == m_hFileMap )
	{
		WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase - OpenFileMapping failed. %d " ), GetLastError());
		return RESULT_SKIP;
	}
	 
	// Obtain a pointer of SHAREDMEMORY from the handle to file mapping object
	m_sharedMemory = (RESOURCE_CASE*)::MapViewOfFile(	m_hFileMap,
														FILE_MAP_WRITE,
														0,
														0,
														sizeof(RESOURCE_CASE));	

	m_startChildResAccessEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "StartChildResAccess");
	if (m_startChildResAccessEvent == NULL) 
	{ 
		WriteToLog(_T( "SharedResourcesChild::ExecuteTestCase - OpenEvent failed (StartChildResAccess) (%d)\n"), 
			GetLastError());
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"SharedResourcesChild::ExecuteTestCase - OpenEvent failed (StartChildResAccess).");
		m_sharedMemory->Result = S_FALSE;
		return RESULT_SKIP;
	}

	m_endChildResAccessEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "EndChildResAccess");
	if (m_endChildResAccessEvent == NULL) 
	{ 
		WriteToLog(_T( "SharedResourcesChild::ExecuteTestCase - OpenEvent failed (EndChildResAccess) (%d)"), 
			GetLastError());
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"SharedResourcesChild::ExecuteTestCase - OpenEvent failed (EndChildResAccess).");
		m_sharedMemory->Result = S_FALSE;
		return RESULT_SKIP;
	}

	m_exitChildProcessEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ExitChildProcess");
	if (m_exitChildProcessEvent == NULL) 
	{ 
		WriteToLog(_T( "SharedResourcesChild::ExecuteTestCase - OpenEvent failed (ExitChildProcess) (%d)"), 
			GetLastError());
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"SharedResourcesChild::ExecuteTestCase - OpenEvent failed (ExitChildProcess).");
		m_sharedMemory->Result = S_FALSE;
		return RESULT_SKIP;
	}

	m_finishUpdateSharedMemEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "FinishUpdateSharedMem");
	if (m_finishUpdateSharedMemEvent == NULL) 
	{ 
		WriteToLog(_T( "SharedResourcesChild::ExecuteTestCase - OpenEvent failed (FinishUpdateSharedMem) (%d)"), 
			GetLastError());
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"SharedResourcesChild::ExecuteTestCase - OpenEvent failed (FinishUpdateSharedMem).");
		m_sharedMemory->Result = S_FALSE;
		return RESULT_SKIP;
	}

	m_endChildCaseWithError = OpenEvent(EVENT_ALL_ACCESS, FALSE, "EndChildCaseWithError");
	if (m_endChildCaseWithError == NULL) 
	{ 
		WriteToLog(_T( "SharedResourcesChild::ExecuteTestCase - OpenEvent failed (EndChildCaseWithError) (%d)"), 
			GetLastError());
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"SharedResourcesChild::ExecuteTestCase - OpenEvent failed (EndChildCaseWithError).");
		m_sharedMemory->Result = S_FALSE;
		return RESULT_SKIP;
	} 

	


	while(TRUE) //not signaled to start
	{
		if (WaitForSingleObject(m_exitChildProcessEvent, 10) == WAIT_OBJECT_0)
		{
			WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase - Recieved exitChildProcessEvent signal from parent."));
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"SharedResourcesChild::ExecuteTestCase - Recieved exitChildProcessEvent signal from parent.");
			break;
		}

		//wait for signal to access the shared memory. 
		// The wait time has to be large enough since the main process might execute the SAME_PROCESS test cases first. 
		if (WaitForSingleObject(m_finishUpdateSharedMemEvent, 3600000) != WAIT_OBJECT_0)
		{
			WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase - Failed to recieve FinishUpdateSharedMem signal from parent."));
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"SharedResourcesChild::ExecuteTestCase - Failed to recieve FinishUpdateSharedMem signal from parent.");
			m_sharedMemory->Result = S_FALSE;
			tr = RESULT_SKIP;
			goto exitChild;
		}
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase - No Errors.");

		//get test params
		m_destDevice = m_sharedMemory->Device;
		m_destDeviceFL = m_sharedMemory->DeviceFeatureLevel;
		m_srcDeviceFL = m_sharedMemory->SrcFeatureLevel;
		m_Format = m_sharedMemory->Format;
		m_sharedMemory->Result = S_OK; //send back pass if nothing bad happens
		m_SyncMethod = m_sharedMemory->SyncMethod;
		m_WriteMethod = m_sharedMemory->WriteMethod;
		m_ChildAccessMethod = m_sharedMemory->ChildAccessMethod;
		m_ResWidth = m_sharedMemory->Width;
		m_ResHeight = m_sharedMemory->Height;
		m_ArraySize = m_sharedMemory->ArraySize;
		m_MipLevels = m_sharedMemory->MipLevels;
		m_bMiscSharedNTHandle = m_sharedMemory->isNTHandle;
		m_bMiscGDICompatible = m_sharedMemory->isGDICompatible;
		m_bBindShaderResource = m_sharedMemory->isShaderResource;
		m_bBindRenderTarget = m_sharedMemory->isRenderTarget;
		m_bBindCapture = m_sharedMemory->isCapture;
		m_bMiscGenMips = m_sharedMemory->isMiscGenMips;

		m_FormatBlockSize = 1;
		switch(m_Format)
		{
		case DXGI_FORMAT_BC1_UNORM:     
		case DXGI_FORMAT_BC1_UNORM_SRGB:
			m_FormatBlockSize = 8;
			break;
		case DXGI_FORMAT_BC2_UNORM:     
		case DXGI_FORMAT_BC2_UNORM_SRGB: 
		case DXGI_FORMAT_BC3_UNORM:     
		case DXGI_FORMAT_BC3_UNORM_SRGB:
			m_FormatBlockSize = 16;
			break;
		}
	
		// cleanup previous data
		if (m_pDestObjs)
		{
			delete m_pDestObjs;
			m_pDestObjs = NULL;
		}
		if (m_pDataByte1)
		{
			delete[] m_pDataByte1;
			m_pDataByte1 = NULL;
		}
		if (m_pDataByte2)
		{
			delete[] m_pDataByte2;
			m_pDataByte2 = NULL;
		}

		switch (m_destDevice)
		{
		case (1100):	m_pDestObjs = new DestObjects11(this, m_destDeviceFL); break;
		case (1101):	m_pDestObjs = new DestObjectsVideo(this, m_destDeviceFL); break;
		case (1102):	m_pDestObjs = new DestObjectsGDI(this, m_destDeviceFL); break;
		case (1010):	m_pDestObjs = new DestObjects10_1(this, m_destDeviceFL);		break;
		case (1000):	m_pDestObjs = new DestObjects10(this);		break;
		case (900):		m_pDestObjs = new DestObjects9(this);		break;
		
		default: 
			WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase(): wrong m_destDevice." ));
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase: wrong m_destDevice.");
			m_sharedMemory->Result =E_FAIL;
			goto testdone;
		}
		
		if (!m_pDestObjs)
		{
			WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase(): Create m_pDestObjs failed." ));
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase: Create m_pDestObjs failed.");
			m_sharedMemory->Result =E_FAIL;
			goto testdone;
		}
		
		m_pDataByte1 = new BYTE[MAX_DATA_BYTES];
		m_pDataByte2 = new BYTE[MAX_DATA_BYTES];
		memcpy(m_pDataByte1, &m_sharedMemory->TestData, MAX_DATA_BYTES);
		memcpy(m_pDataByte2, &m_sharedMemory->TestData2, MAX_DATA_BYTES);
		
		if (!(RESULT_PASS == (tr = m_pDestObjs->SetupTestCase(this))))
		{
			WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase(): m_pDestObjs->SetupTestCase failed." ));
			strcat_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase: m_pDestObjs->SetupTestCase failed.");
			if ( tr == RESULT_SKIP )
				m_sharedMemory->Result =S_FALSE;
			else
				m_sharedMemory->Result =E_FAIL;
			goto testdone;
		}

		

		 //OPEN RESOURCE
		if (!(RESULT_PASS == (tr = m_pDestObjs->ExecuteResourceOpen(this, m_sharedMemory->SharedHandle))))
		{
			WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase(): m_pDestObjs->ExecuteResourceOpen failed." ));
			strcat_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase: m_pDestObjs->ExecuteResourceOpen failed.");
			m_sharedMemory->Result =E_FAIL;
			goto testdone;
		}
		
		// syncing with the parent process
		if ( m_SyncMethod == SR_KEYED_MUTEX )
		{
			hr = m_pDestObjs->QIKeyedMutex( &pDXGIKeyedMutex );
			if ( hr != S_OK )
			{
				WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase() - QIKeyedMutex() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
				strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase: QIKeyedMutex() failed.");
				m_sharedMemory->Result =E_FAIL;
				tr = RESULT_FAIL;
				goto exitChild;
			}

			// Acquire a lock to the resource.
			hr = pDXGIKeyedMutex->AcquireSync(1, INFINITE);
			if ( hr != S_OK)
			{
				WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase() - AcquireSync() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
				strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase: AcquireSync() failed.");
				m_sharedMemory->Result =E_FAIL;
				tr = RESULT_FAIL;
				goto exitChild;
			}
		}
		else
		{
			//wait for signal to access the shared resources
			if (WaitForSingleObject(m_startChildResAccessEvent, 50000) != WAIT_OBJECT_0)
			{
				WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase - Failed to recieve StartChildResAccessEvent signal from parent."));
				strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
					"SharedResourcesChild::ExecuteTestCase - Failed to recieve StartChildResAccessEvent signal from parent.");
				m_sharedMemory->Result =S_FALSE;
				tr = RESULT_SKIP;
				goto exitChild;
			}
		}

		//ACCESSING RESOURCE
		if (!(RESULT_PASS == (tr = m_pDestObjs->ExecuteResourceAccess(this, m_pDataByte1, m_pDataByte2))))
		{
			WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase(): m_pDestObjs->ExecuteResourceAccess failed." ));
			strcat_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase: m_pDestObjs->ExecuteResourceAccess failed.");
			m_sharedMemory->Result =E_FAIL;
		}

		// syncing with the parent process
		if ( m_SyncMethod == SR_KEYED_MUTEX )
		{
			// Release the lock to the parent process.
			hr = pDXGIKeyedMutex->ReleaseSync(0);
			SAFE_RELEASE( pDXGIKeyedMutex );
			if ( hr != S_OK)
			{
				WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase() - ReleaseSync() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
				strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase:  ReleaseSync() failed.");
				m_sharedMemory->Result =E_FAIL;
				tr = RESULT_FAIL;
				goto exitChild;
			}
		}
		else
		{
			//signal test complete
			if (! SetEvent(m_endChildResAccessEvent) ) 
			{ 
				WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase - Failed to signal EndChildResAccessEvent to the parent process."));
				strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
					"SharedResourcesChild::ExecuteTestCase - Failed to signal EndChildResAccessEvent to the parent process.");
				m_sharedMemory->Result =S_FALSE;
				tr = RESULT_SKIP;
				goto exitChild;
			}
		}
		continue;

		// If synchronization failure happens, goto exitChild directly as there's no need to continue the loop; 
		// for other failures that happen before ExecuteResourceAccess is called, goto testdone to skip the current test case and continue.
testdone:
		// signal test case finished with error
		if (! SetEvent(m_endChildCaseWithError) ) 
		{ 
			WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase - Failed to signal EndChildCaseWithError to the parent process."));
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"SharedResourcesChild::ExecuteTestCase - Failed to signal EndChildCaseWithError to the parent process.");
			goto exitChild;
		}

		continue;
		
	}

exitChild:
	return tr;
}

TEST_RESULT DestObjects9::ExecuteResourceOpen(SharedResourcesTest *pTest, HANDLE DXGIHandle )
{
	HRESULT hr;
	
	if (FAILED(hr = m_pDevice->CreateTexture(m_Width, m_Height, 1, 0, m_D3DFormat, D3DPOOL_DEFAULT, &m_pTexShared, &DXGIHandle)))
	{
		pTest->WriteToLog( _T( "DestObjects9::ExecuteResourceOpen() - CreateTexture() - Open shared texture on IDirect3DDevice9 failed. hr = %s" ), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects9::ExecuteResourceOpen() - CreateTexture() - Open shared texture on IDirect3DDevice9 failed.");
		pTest->m_sharedMemory->Result = hr;
		return RESULT_FAIL;
	}

	return RESULT_PASS;
	
}

TEST_RESULT DestObjects9::ExecuteResourceAccess(SharedResourcesTest *pTest, BYTE* pDataByte1, BYTE* pDataByte2)
{
	HRESULT hr;
	TEST_RESULT tr = RESULT_PASS;
	IDirect3DTexture9* pTempTex9 = NULL;

	//verify data
	if (FAILED(hr = pTest->VerifyIDirect3DTexture9(m_pTexShared, pDataByte1, m_D3DFormat)))
	{
		pTest->WriteToLog( _T( "DestObjects9::ExecuteResourceAccess() - VerifyIDirect3DTexture9 - Verifying data in resource failed."));
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects9::ExecuteResourceAccess() - VerifyIDirect3DTexture9 - Verifying data in resource failed.");
		pTest->m_sharedMemory->Result = hr;
		return RESULT_FAIL;
	}

	// Create a staging texture to update the opened shared texture
	if (FAILED (hr = m_pDevice->CreateTexture(
		m_Width,
		m_Height,
		1,
		0,
		m_D3DFormat,
		D3DPOOL_SYSTEMMEM,
		&pTempTex9,
		NULL)))
	{
		pTest->WriteToLog(_T("DestObjects9::ExecuteResourceAccess() - CreateTexture Failed hr = %s"), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects9::ExecuteResourceAccess() - CreateTexture Failed. ");
		pTest->m_sharedMemory->Result = hr;
		return RESULT_FAIL;
	} 

	D3DLOCKED_RECT LockedRect;
	if( FAILED( hr = pTempTex9->LockRect(0, &LockedRect, NULL, 0 ) ) )
	{
		pTest->WriteToLog(_T("DestObjects9::ExecuteResourceAccess() - Failed to lock the d3d9 surface. hr = %s\n"), D3DHResultToString(hr).c_str() );
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects9::ExecuteResourceAccess() - Failed to lock the d3d9 surface. ");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto testDone;
	}

	const UINT bytesPerElement = GetBitsPerElement( m_DXGIFormat ) / 8;
	BYTE* pData=(BYTE*) LockedRect.pBits;
	for( UINT y = 0; y < m_Height; y++ )
	{
		for( UINT x = 0; x < m_Width * bytesPerElement; x++ )
		{
			pData[(y * LockedRect.Pitch) + x] = pDataByte2[((y * m_Width * bytesPerElement) + x)];
		}
	}
	pTempTex9->UnlockRect(0);

	hr = pTest->VerifyIDirect3DTexture9(pTempTex9, pDataByte2, m_D3DFormat);
	if (FAILED(hr))
	{
		pTest->WriteToLog( _T( "DestObjects9::ExecuteResourceAccess() - VerifyIDirect3DTexture9 - Verifying data in resource failed."));
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects9::ExecuteResourceAccess() - VerifyIDirect3DTexture9 - Verifying data in resource failed.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto testDone;
	}

	//update data in opened shared resource using staging texture
	hr = m_pDevice->UpdateTexture(pTempTex9, m_pTexShared);

	if( FAILED(hr) )
	{
		pTest->WriteToLog(_T("DestObjects9::ExecuteResourceAccess() - UpdateTexture Failed"));
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects9::ExecuteResourceAccess() - UpdateTexture Failed");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto testDone;
	}

	if(FAILED( hr = pTest->WaitForEvent(NULL, NULL, m_pDevice) ))
	{
		pTest->WriteToLog( _T( "DestObjects9::ExecuteResourceAccess() - WaitForEvent() failed. hr = %s" ), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects9::ExecuteResourceAccess() - WaitForEvent() failed.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto testDone;
	}

testDone:
	SAFE_RELEASE(pTempTex9);
	SAFE_RELEASE(m_pTexShared);
	return tr;

}

TEST_RESULT DestObjects10::ExecuteResourceOpen(SharedResourcesTest *pTest, HANDLE DXGIHandle)
{
	HRESULT hr;
	if ( pTest->GetMiscSharedNTHandle() && 	pTest->GetSyncMethod() == SR_KEYED_MUTEX )
	{
		hr = m_pDevice->QueryInterface(__uuidof( ID3D11Device1 ), (void **) &m_pDevice11_1 );
		if (FAILED(hr) || m_pDevice11_1 == NULL)
		{
			pTest->WriteToLog( _T( "DestObjects10::ExecuteResourceOpen():  QI for ID3D11Device1 failed." ) );
			strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"DestObjects10::ExecuteResourceOpen():  QI for ID3D11Device1 failed.");
			pTest->m_sharedMemory->Result = hr;
			return RESULT_FAIL;
		}

		hr = m_pDevice11_1->OpenSharedResourceByName(
			HANDLE_NAME_VALID, 
			DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE, 
			__uuidof(ID3D10Texture2D), 
			(void **)&m_pTexShared);
		

		if (FAILED(hr))
		{
			pTest->WriteToLog( _T( "DestObjects10::ExecuteResourceOpen() - Failed to open shared resource. hr = %s" ), D3DHResultToString(hr).c_str());
			strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"DestObjects10::ExecuteResourceOpen - OpenSharedResourceByName() Failed to open shared resource.");
			pTest->m_sharedMemory->Result = hr;
			return RESULT_FAIL;
		}
	}
	else
	{
		if (FAILED(hr = m_pDevice->OpenSharedResource(DXGIHandle, __uuidof(ID3D10Texture2D), (void**) &m_pTexShared)))
		{
			pTest->WriteToLog( _T( "DestObjects10::ExecuteResourceOpen() - Failed to open shared resource. hr = %s" ), D3DHResultToString(hr).c_str());
			strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects10::ExecuteResourceOpen - Failed to open shared resource.");
			pTest->m_sharedMemory->Result = hr;
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}

TEST_RESULT DestObjects10::ExecuteResourceAccess(SharedResourcesTest *pTest, BYTE* pDataByte1, BYTE* pDataByte2)
{
	
	void* pData = NULL;
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	tstring failureString;

	//verify data
	for( UINT i = 0; i < m_ArraySize; i++ )
	{
		for ( UINT j = 0; j < m_MipLevels; j++ )
		{
			if (FAILED(hr = pTest->VerifyTex2D( m_pDevice, i, j, m_pTexShared, pDataByte1, &failureString)))
			{
				pTest->WriteToLog( _T( "DestObjects10::ExecuteResourceAccess - Verifying data in resource failed. \
									   arraySlice = %d, mipSlice = %d. hr = %s" ), 
										i, j, D3DHResultToString(hr).c_str());
				strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, failureString.c_str());
				strcat_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
					"DestObjects10::ExecuteResourceAccess - Verifying data in resource failed.");
				pTest->m_sharedMemory->Result = hr;
				tr = RESULT_FAIL;
				goto Done;
			}
		}
	}
	
	//update data
	pData = pDataByte2;
	UINT bytesPerElement = GetBitsPerElement(m_DXGIFormat) / 8;

	switch ( pTest->GetChildAccessMethod() )
	{
	// For tex2DArrays, the child process only updated the first and the last array slices with DataByte2.
	case SR_UPDATE_RESOURCE:
		{
			UINT subResourceWidth = m_Width;
			UINT subResourceHeight = m_Height;
			// the first array slice
			for ( UINT j = 0; j < m_MipLevels; j++ )
			{
				UINT srcRowPitch =  bytesPerElement * subResourceWidth;
				UINT srcDepthPitch = srcRowPitch * subResourceHeight;
				m_pDevice->UpdateSubresource(m_pTexShared, j, NULL, pData, srcRowPitch, srcDepthPitch); 
				if (m_ArraySize > 1)
				{
					// the last array slice
					UINT subResource = ( m_ArraySize - 1 ) * m_MipLevels + j;
					m_pDevice->UpdateSubresource(m_pTexShared, subResource, NULL, pData, srcRowPitch, srcDepthPitch); 
				}
				subResourceWidth = subResourceWidth >> 1;
				subResourceHeight = subResourceHeight >> 1;			
			}
			
			break;
		}
	case SR_DRAWTO_RESOURCE:
		{
			// the first array slice
			for ( UINT j = 0; j < m_MipLevels; j++ )
			{
				tr = DrawToResource(pTest, 0, j, pData, m_pTexShared);
				if (tr != RESULT_PASS)
				{
					pTest->WriteToLog( _T( "DestObjects10::ExecuteResourceAccess() - DrawToResource() failed. \
										   arraySlice = 0, MipSlice = %d. "), j);
					strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
						"DestObjects10::ExecuteResourceAccess() - DrawToResource() failed.");
					pTest->m_sharedMemory->Result = E_FAIL;
					goto Done;
				}
			}
			if (m_ArraySize > 1)
			{
				// the last array slice
				for ( UINT j = 0; j < m_MipLevels; j++ )
				{
					tr = DrawToResource(pTest, m_ArraySize - 1, j, pData, m_pTexShared);
					if (tr != RESULT_PASS)
					{
						pTest->WriteToLog( _T( "DestObjects10::ExecuteResourceAccess() - DrawToResource() failed. \
											   arraySlice = %d, MipSlice = %d."), m_ArraySize - 1, j);
						strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
							"DestObjects10::ExecuteResourceAccess() - DrawToResource() failed.");
						pTest->m_sharedMemory->Result = E_FAIL;
						goto Done;
					}
				}
			}
			break;
		}
	case SR_COPY_RESOURCE:
		{
			D3D10_SUBRESOURCE_DATA	 *pSubResData = NULL;
			pSubResData = new D3D10_SUBRESOURCE_DATA[m_ArraySize * m_MipLevels];
			ID3D10Texture2D *pTexStaging = NULL;

			float bytesPerElement = (float)GetBitsPerElement(m_DXGIFormat) / 8;
			for ( UINT i = 0; i < m_ArraySize; i++ )
			{
				UINT subResWidth = m_Width;
				for ( UINT j = 0; j < m_MipLevels; j++ )
				{
					if( i == 0 || i == m_ArraySize - 1)
						pSubResData[i * m_MipLevels + j].pSysMem = pDataByte2;
					else
						pSubResData[i * m_MipLevels + j].pSysMem = pDataByte1;
				
					pSubResData[i * m_MipLevels + j].SysMemPitch = (UINT)(subResWidth * bytesPerElement);
					subResWidth = subResWidth >> 1;
				}
			}

			//create staging texture
			D3D10_TEXTURE2D_DESC tex2DDesc;
			tex2DDesc.ArraySize			= m_ArraySize;
			tex2DDesc.BindFlags			= 0;
			tex2DDesc.Format			= m_DXGIFormat;
			tex2DDesc.Height			= m_Height;
			tex2DDesc.Width				= m_Width;
			tex2DDesc.MipLevels			= m_MipLevels;
			tex2DDesc.MiscFlags			= 0;
			tex2DDesc.SampleDesc.Count	= 1;
			tex2DDesc.SampleDesc.Quality= 0;
			tex2DDesc.Usage				= D3D10_USAGE_DEFAULT;
			tex2DDesc.CPUAccessFlags	= 0;

			if( FAILED( hr = m_pDevice->CreateTexture2D( &tex2DDesc, pSubResData, &pTexStaging) ) )
			{
				pTest->WriteToLog( _T( "DestObjects10::ExecuteResourceAccess() - CreateTexture2D() failed for staging resource, hr = %s" ), D3DHResultToString(hr).c_str());
				strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
						"DestObjects10::ExecuteResourceAccess() - CreateTexture2D() failed for staging resource.");
				pTest->m_sharedMemory->Result = E_FAIL;
				tr = RESULT_FAIL;
				goto Done;
			}

			m_pDevice->CopyResource(m_pTexShared, pTexStaging);
			SAFE_RELEASE(pTexStaging);
			break;
		}
	default:
		{
			pTest->WriteToLog( _T( "DestObjects10::ExecuteResourceAccess() - WriteMethod doesn't exist." ));
			strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects10::ExecuteResourceAccess() - WriteMethod doesn't exist.");
			pTest->m_sharedMemory->Result = E_FAIL;
			goto Done;
		}

	}
	
	m_pDevice->Flush();
	if(FAILED( hr = pTest->WaitForEvent(NULL, m_pDevice) ))
	{
		pTest->WriteToLog( _T( "DestObjects10::ExecuteResourceAccess() - WaitForEvent() failed. hr = %s" ), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects10::ExecuteResourceAccess() - WaitForEvent() failed.");
		pTest->m_sharedMemory->Result = S_FALSE;
		tr = RESULT_FAIL;
	}

Done: // releasing interfaces that've been used by DrawToResource()
	SAFE_RELEASE( m_pTexShared );
	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pSRVTex );
	SAFE_RELEASE( m_pRTV );

	return tr;
}

TEST_RESULT DestObjects11::ExecuteResourceOpen(SharedResourcesTest *pTest, HANDLE DXGIHandle)
{
	HRESULT hr;
	if ( pTest->GetMiscSharedNTHandle() && 	pTest->GetSyncMethod() == SR_KEYED_MUTEX )
	{
		if ( m_pDevice11_1 == NULL )
		{
			hr = m_pDevice->QueryInterface(__uuidof( ID3D11Device1 ), (void **) &m_pDevice11_1 );
			if (FAILED(hr) || m_pDevice11_1 == NULL)
			{
				pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceOpen():  QI for ID3D11Device1 failed." ) );
				strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
					"DestObjects11::ExecuteResourceOpen():  QI for ID3D11Device1 failed.");
				pTest->m_sharedMemory->Result = hr;
				return RESULT_FAIL;
			}
		}

		if ( pTest->GetCompBuffer() )
		{
			std::wstringstream out;
			out << pTest->GetCurrentCompBuffer();
			std::wstring handleName = HANDLE_NAME_VALID + out.str();

			hr = m_pDevice11_1->OpenSharedResourceByName(
				handleName.c_str(),
				DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE,
				__uuidof(ID3D11Texture2D), 
				(void **)&m_pTexShared);

			if (FAILED(hr))
			{
				pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceOpen() - OpenSharedResourceByName() failed to open shared composition resource. hr = %s" ), D3DHResultToString(hr).c_str());
				strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
					"DestObjects11::ExecuteResourceOpen - OpenSharedResourceByName() Failed to open shared resource.");
				pTest->m_sharedMemory->Result = hr;
				return RESULT_FAIL;
			}
		}
		else
		{
			hr = m_pDevice11_1->OpenSharedResourceByName(
				HANDLE_NAME_VALID,
				DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE,
				__uuidof(ID3D11Texture2D), 
				(void **)&m_pTexShared);

			if (FAILED(hr))
			{
				pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceOpen() - OpenSharedResourceByName() failed to open shared resource. hr = %s" ), D3DHResultToString(hr).c_str());
				strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
					"DestObjects11::ExecuteResourceOpen - OpenSharedResourceByName() Failed to open shared resource.");
				pTest->m_sharedMemory->Result = hr;
				return RESULT_FAIL;
			}
		}
	}
	else
	{
		if (FAILED(hr = m_pDevice->OpenSharedResource(DXGIHandle, __uuidof(ID3D11Texture2D), (void**) &m_pTexShared)))
		{
			pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceOpen() - OpenSharedResource() failed to open shared resource. hr = %s" ), D3DHResultToString(hr).c_str());
			strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects11::ExecuteResourceOpen - Failed to open shared resource.");
			pTest->m_sharedMemory->Result = hr;
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}

TEST_RESULT DestObjects11::ExecuteResourceAccess(SharedResourcesTest *pTest, BYTE* pDataByte1, BYTE* pDataByte2)
{

	void* pData = NULL;
	HRESULT hr = S_OK;
	TEST_RESULT tr = RESULT_PASS;
	tstring failureString;

	//verify data
	for( UINT i = 0; i < m_ArraySize; i++ )
	{
		for ( UINT j = 0; j < m_MipLevels; j++ )
		{
			if (FAILED(hr = pTest->VerifyTex2D( m_pDevice, m_pImmediateContext, i, j, m_pTexShared, pDataByte1, &failureString)))
			{
				pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceAccess - Verifying data in resource failed.\
									   arraySlice = %d, mipSlice = %d. hr = %s" ), 
										i, j, D3DHResultToString(hr).c_str());
				strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, failureString.c_str());
				strcat_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects11::ExecuteResourceAccess - Verifying data in resource failed.");
				pTest->m_sharedMemory->Result = hr;
				tr = RESULT_FAIL;
				goto Done;
			}
		}
	}

	// test sample from shared resource on the first subresource
	// BUGBUG: skip BC formats for sample-from testing for now. They need more tweaks and are lower priority formats for sharing. 
	if( pTest->GetBindShaderResource() && 
		( m_DXGIFormat != DXGI_FORMAT_BC1_UNORM && m_DXGIFormat != DXGI_FORMAT_BC1_UNORM_SRGB && 
		m_DXGIFormat != DXGI_FORMAT_BC2_UNORM && m_DXGIFormat != DXGI_FORMAT_BC2_UNORM_SRGB &&
		m_DXGIFormat != DXGI_FORMAT_BC3_UNORM && m_DXGIFormat != DXGI_FORMAT_BC3_UNORM_SRGB )
		)
	{
		tr = DrawToResource(pTest, 0, 0, pData, m_pTexShared, true);
		if( tr != RESULT_PASS )
		{
			pTest->WriteToLog( "DestObjects11::ExecuteResourceAccess - DrawToResource() failed to sample from shared resource." );
					strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, failureString.c_str());
					strcat_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects11::ExecuteResourceAccess - DrawToResource() failed to sample from sahred resource.");
					pTest->m_sharedMemory->Result = E_FAIL;
					goto Done;
		}
		if (FAILED(hr = pTest->VerifyTex2D( m_pDevice, m_pImmediateContext, 0, 0, m_pSRRTTex, pDataByte1, &failureString)))
		{
			pTest->WriteToLog(  "DestObjects11::ExecuteResourceAccess - Verifying data in RenderTarget failed. hr = %s", D3DHResultToString(hr).c_str() );
			strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, failureString.c_str());
			strcat_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects11::ExecuteResourceAccess - Verifying data in RenderTarget failed.");
			pTest->m_sharedMemory->Result = hr;
			tr = RESULT_FAIL;
			goto Done;
		}
	}
	
	//update data
	pData = pDataByte2;
	UINT bytesPerElement = GetBitsPerElement(m_DXGIFormat) / 8;
	switch ( pTest->GetChildAccessMethod() )
	{
	// For tex2DArrays, the child process only updated the first and the last array slices with DataByte2.
	case SR_UPDATE_RESOURCE:
		{
			// the first array slice
			UINT subResourceWidth = m_Width;
			UINT subResourceHeight = m_Height;
			for ( UINT j = 0; j < m_MipLevels; j++ )
			{
				UINT srcRowPitch =  bytesPerElement * subResourceWidth;
				UINT srcDepthPitch = srcRowPitch * subResourceHeight;
				m_pEffectiveContext->UpdateSubresource(m_pTexShared, j, NULL, pData, srcRowPitch, srcDepthPitch); 
				if (m_ArraySize > 1)
				{
					// the last array slice
					UINT subResource = ( m_ArraySize - 1 ) * m_MipLevels + j;
					m_pEffectiveContext->UpdateSubresource(m_pTexShared, subResource, NULL, pData, srcRowPitch, srcDepthPitch);
				}
				subResourceWidth = subResourceWidth >> 1;
				subResourceHeight = subResourceHeight >> 1;
			}
			
			break;
		}
	case SR_DRAWTO_RESOURCE:
		{
			// the first array slice
			for ( UINT j = 0; j < m_MipLevels; j++ )
			{
				tr = DrawToResource(pTest, 0, j, pData, m_pTexShared);
				if (tr != RESULT_PASS)
				{
					pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceAccess() - DrawToResource() failed. \
										   arraySlice = 0, MipSlice = %d. "), j);
					strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
						"DestObjects11::ExecuteResourceAccess() - DrawToResource() failed.");
					pTest->m_sharedMemory->Result = E_FAIL;
					goto Done;
				}
			}
			if (m_ArraySize > 1)
			{
				// the last array slice
				for ( UINT j = 0; j < m_MipLevels; j++ )
				{
					tr = DrawToResource(pTest, m_ArraySize - 1, j, pData, m_pTexShared);
					if (tr != RESULT_PASS)
					{
						pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceAccess() - DrawToResource() failed. \
											   arraySlice = %d, MipSlice = %d."), m_ArraySize - 1, j);
						strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
							"DestObjects11::ExecuteResourceAccess() - DrawToResource() failed.");
						pTest->m_sharedMemory->Result = E_FAIL;
						goto Done;
					}
				}
			}
			break;
		}
	case SR_COPY_RESOURCE:
		{
			D3D11_SUBRESOURCE_DATA	 *pSubResData = NULL;
			pSubResData = new D3D11_SUBRESOURCE_DATA[m_ArraySize * m_MipLevels];
			ID3D11Texture2D *pTexStaging = NULL;

			float bytesPerElement = (float)GetBitsPerElement(m_DXGIFormat) / 8;
			if ( GetFlags(m_DXGIFormat) & FF_VIDEO )
			{
				bytesPerElement = (float)GetBitsPerElementVideo(m_DXGIFormat) / 8;
			}

			UINT blockSize = 1;
			switch(m_DXGIFormat)
			{
			case DXGI_FORMAT_BC1_UNORM:     
			case DXGI_FORMAT_BC1_UNORM_SRGB:
				blockSize = 8;
				break;
			case DXGI_FORMAT_BC2_UNORM:     
			case DXGI_FORMAT_BC2_UNORM_SRGB: 
			case DXGI_FORMAT_BC3_UNORM:     
			case DXGI_FORMAT_BC3_UNORM_SRGB:
				blockSize = 16;
				break;
			}

			for ( UINT i = 0; i < m_ArraySize; i++ )
			{
				UINT subResWidth = m_Width;
				for ( UINT j = 0; j < m_MipLevels; j++ )
				{
					if( i == 0 || i == m_ArraySize - 1)
						pSubResData[i * m_MipLevels + j].pSysMem = pDataByte2;
					else
						pSubResData[i * m_MipLevels + j].pSysMem = pDataByte1;
					if( blockSize > 1 )
						pSubResData[i * m_MipLevels + j].SysMemPitch = ( subResWidth / 4) * blockSize;
					else 
						pSubResData[i * m_MipLevels + j].SysMemPitch = (UINT)(subResWidth * bytesPerElement);
					subResWidth = subResWidth >> 1;
				}
			}

			//create staging texture
			D3D11_TEXTURE2D_DESC tex2DDesc;
			tex2DDesc.ArraySize			= m_ArraySize;
			tex2DDesc.BindFlags			= 0;
			tex2DDesc.Format			= m_DXGIFormat;
			tex2DDesc.Height			= m_Height;
			tex2DDesc.Width				= m_Width;
			tex2DDesc.MipLevels			= m_MipLevels;
			tex2DDesc.MiscFlags			= 0;
			tex2DDesc.SampleDesc.Count	= 1;
			tex2DDesc.SampleDesc.Quality= 0;

			if ( GetFlags(m_DXGIFormat) & FF_VIDEO )
			{
				// create staging resource for video format resource
				tex2DDesc.Usage				= D3D11_USAGE_STAGING;
				tex2DDesc.CPUAccessFlags	= D3D11_CPU_ACCESS_WRITE;
				if( FAILED( hr = m_pDevice->CreateTexture2D( &tex2DDesc, NULL, &pTexStaging) ) )
				{
					pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceAccess() - CreateTexture2D() failed for staging video resource, hr = %s" ), D3DHResultToString(hr).c_str());
					strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
							"DestObjects11::ExecuteResourceAccess() - CreateTexture2D() failed for staging video resource.");
					pTest->m_sharedMemory->Result = E_FAIL;
					tr = RESULT_FAIL;
					goto Done;
				}

				tr = pTest->WriteStagingVideoResource(m_Width, m_Height, m_DXGIFormat, m_pDevice, m_pImmediateContext, pTexStaging, pDataByte2);
				if (tr != RESULT_PASS)
				{
					pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceAccess() - WriteStagingVideoResource() failed." ));
					strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
							"DestObjects11::ExecuteResourceAccess() - WriteStagingVideoResource() failed.");
					pTest->m_sharedMemory->Result = E_FAIL;
					SAFE_RELEASE(pTexStaging);
					goto Done;
				}
			}
			else
			{
				tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
				tex2DDesc.CPUAccessFlags	= 0;

				// need a bind flag for default resource on FL9
				if( pTest->GetSourceDeviceFeatureLevel() < D3D_FEATURE_LEVEL_10_0 || pTest->GetDestinationDeviceFeatureLevel() < D3D_FEATURE_LEVEL_10_0 )
				{
					UINT formatSupport;
					hr = m_pDevice->CheckFormatSupport(m_DXGIFormat, &formatSupport);
					if (FAILED(hr))
					{
						pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceAccess() - CheckFormatSupport(%s) failed. hr = %s" ), ToString(m_DXGIFormat).c_str(), D3DHResultToString(hr).c_str());
						strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
							"DestObjects11::ExecuteResourceAccess() - CheckFormatSupport() failed");
						pTest->m_sharedMemory->Result = E_FAIL;
						tr = RESULT_SKIP;
						goto Done;
					}
		
					if( formatSupport & ( D3D11_FORMAT_SUPPORT_RENDER_TARGET | D3D11_FORMAT_SUPPORT_SHADER_LOAD | D3D11_FORMAT_SUPPORT_SHADER_SAMPLE ) )
						tex2DDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
					else if( formatSupport & D3D11_FORMAT_SUPPORT_RENDER_TARGET )
						tex2DDesc.BindFlags = D3D11_BIND_RENDER_TARGET;
					else
					{
						pTest->WriteToLog( "DestObjects11::ExecuteResourceAccess(): Format %s does not support RenderTarget or ShaderResource on this device. Skip test case.", ToString(m_DXGIFormat).c_str() );
						strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
							"DestObjects11::ExecuteResourceAccess() -Format %s does not support RenderTarget or ShaderResource on this device. Skip test case.");
						pTest->m_sharedMemory->Result = E_FAIL;
						tr = RESULT_SKIP;
						goto Done;
					}
				}

				if( FAILED( hr = m_pDevice->CreateTexture2D( &tex2DDesc, pSubResData, &pTexStaging) ) )
				{
					pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceAccess() - CreateTexture2D() failed for staging resource, hr = %s" ), D3DHResultToString(hr).c_str());
					strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
							"DestObjects11::ExecuteResourceAccess() - CreateTexture2D() failed for staging resource.");
					pTest->m_sharedMemory->Result = E_FAIL;
					tr = RESULT_FAIL;
					goto Done;
				}
			}

			m_pEffectiveContext->CopyResource(m_pTexShared, pTexStaging);
			SAFE_RELEASE(pTexStaging);
			break;
		}

	default:
		{
			pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceAccess() - WriteMethod doesn't exist." ));
			strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects11::ExecuteResourceAccess() - WriteMethod doesn't exist.");
			pTest->m_sharedMemory->Result = E_FAIL;
			goto Done;
		}

	}
	
	if (m_pEffectiveContext != m_pImmediateContext)
	{
		ID3D11CommandList* pCommandList = NULL;
		if (FAILED (hr = m_pEffectiveContext->FinishCommandList(TRUE, &pCommandList)))
		{
			pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceAccess() - ID3D11CommandList::FinishCommandList() failed. hr = %s" ), D3DHResultToString(hr).c_str());
			strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects11::ExecuteResourceAccess() - ID3D11CommandList::FinishCommandList() failed.");
			pTest->m_sharedMemory->Result = E_FAIL;
			tr = RESULT_BLOCKED;
			goto Done;
		}
		m_pImmediateContext->ExecuteCommandList(pCommandList, TRUE);
		pCommandList->Release();
	}
	m_pImmediateContext->Flush();


	if(FAILED( hr = pTest->WaitForEvent( m_pDevice ) ))
	{
		pTest->WriteToLog( _T( "DestObjects11::ExecuteResourceAccess() - WaitForEvent() failed. hr = %s" ), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjects11::ExecuteResourceAccess() - WaitForEvent() failed.");
		pTest->m_sharedMemory->Result = S_FALSE;
		tr = RESULT_FAIL;
	}

Done: // releasing interfaces that've been used by DrawToResource()
	SAFE_RELEASE( m_pTexShared );
	SAFE_RELEASE( m_pVertexShader );
	SAFE_RELEASE( m_pPixelShader );
	SAFE_RELEASE( m_pInputLayout );
	SAFE_RELEASE( m_pVertexBuffer );
	SAFE_RELEASE( m_pSRV );
	SAFE_RELEASE( m_pSRRTTex );
	SAFE_RELEASE( m_pRTV );

	return tr;
}


TEST_RESULT DestObjectsVideo::ExecuteResourceOpen(SharedResourcesTest *pTest, HANDLE DXGIHandle)
{
	HRESULT hr;
	if ( pTest->GetMiscSharedNTHandle() && 	pTest->GetSyncMethod() == SR_KEYED_MUTEX )
	{
		hr = m_pDevice->QueryInterface(__uuidof( ID3D11Device1 ), (void **) &m_pDevice11_1 );
		if (FAILED(hr) || m_pDevice11_1 == NULL)
		{
			pTest->WriteToLog( _T( "DestObjectsVideo::ExecuteResourceOpen():  QI for ID3D11Device1 failed." ) );
			strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"DestObjectsVideo::ExecuteResourceOpen():  QI for ID3D11Device1 failed.");
			pTest->m_sharedMemory->Result = hr;
			return RESULT_FAIL;
		}

		hr = m_pDevice11_1->OpenSharedResourceByName(
			HANDLE_NAME_VALID,
			DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE,
			__uuidof(ID3D11Texture2D), 
			(void **)&m_pTexShared);

		if (FAILED(hr))
		{
			pTest->WriteToLog( _T( "DestObjectsVideo::ExecuteResourceOpen() - OpenSharedResourceByName() Failed to open shared resource. hr = %s" ), D3DHResultToString(hr).c_str());
			strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"DestObjectsVideo::ExecuteResourceOpen - OpenSharedResourceByName() Failed to open shared resource.");
			pTest->m_sharedMemory->Result = hr;
			return RESULT_FAIL;
		}
	}
	else
	{
		if (FAILED(hr = m_pDevice->OpenSharedResource(DXGIHandle, __uuidof(ID3D11Texture2D), (void**) &m_pTexShared)))
		{
			pTest->WriteToLog( _T( "DestObjectsVideo::ExecuteResourceOpen() - Failed to open shared resource. hr = %s" ), D3DHResultToString(hr).c_str());
			strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjectsVideo::ExecuteResourceOpen - Failed to open shared resource.");
			pTest->m_sharedMemory->Result = hr;
			return RESULT_FAIL;
		}
	}

	return RESULT_PASS;
}

TEST_RESULT DestObjectsVideo::ExecuteResourceAccess(SharedResourcesTest *pTest, BYTE* pDataByte1, BYTE* pDataByte2)
{

	//verify data
	tstring failureString;
	HRESULT hr = S_OK;
	
	if (FAILED(hr = pTest->VerifyTex2D( m_pDevice, m_pImmediateContext, 0, 0, m_pTexShared, pDataByte1, &failureString)))
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::ExecuteResourceAccess - Verifying data in resource failed.\
								hr = %s" ), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, failureString.c_str());
		strcat_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjectsVideo::ExecuteResourceAccess - Verifying data in resource failed.");
		pTest->m_sharedMemory->Result = hr;
		return RESULT_FAIL;
	}
	

	// udpate data
	TEST_RESULT tr = UpdateDataWithVideoProcessor(pTest, m_pTexShared, pDataByte2);
	if ( tr != RESULT_PASS )
	{
		pTest->WriteToLog( _T( "DestObjectsVideo::ExecuteResourceAccess - UpdateDataWithVideoProcessor() failed. " ));
				strcat_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjectsVideo::ExecuteResourceAccess - UpdateDataWithVideoProcessor() failed.");
				pTest->m_sharedMemory->Result = E_FAIL;
				return tr;
	}

	return tr;
}


TEST_RESULT DestObjectsGDI::ExecuteResourceAccess(SharedResourcesTest *pTest, BYTE* pDataByte1, BYTE* pDataByte2)
{

	//verify data
	tstring failureString;
	HRESULT hr = S_OK;
	IDXGISurface1 *pDXGISurface1 = NULL;
	HDC hdc = NULL;
	TEST_RESULT tr = RESULT_PASS;
	
	if (FAILED(hr = pTest->VerifyTex2D( m_pDevice, m_pImmediateContext, 0, 0, m_pTexShared, pDataByte1, &failureString)))
	{
		pTest->WriteToLog( _T( "DestObjectsGDI::ExecuteResourceAccess - Verifying data in resource failed.\
								hr = %s" ), D3DHResultToString(hr).c_str());
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, failureString.c_str());
		strcat_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjectsGDI::ExecuteResourceAccess - Verifying data in resource failed.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto cleanup;
	}
	

	// QI for pDXGISurface1 and GetDC
	hr = m_pTexShared->QueryInterface(__uuidof( pDXGISurface1 ), reinterpret_cast<void**>( &pDXGISurface1 ) );
	if ( FAILED (hr) || pDXGISurface1 == NULL )
	{
		pTest->WriteToLog(_T("DestObjectsGDI::ExecuteResourceAccess(): QI for IDXGISurface1 failed. hr = %s"), D3DHResultToString(hr).c_str() );
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjectsGDI::ExecuteResourceAccess - QI for IDXGISurface1 failed.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto cleanup;
	}
	hr = pDXGISurface1->GetDC(FALSE, &hdc);
	if ( FAILED (hr) || hdc == NULL )
	{
		pTest->WriteToLog(_T("DestObjectsGDI::ExecuteResourceAccess(): GetDC failed. hr = %s"), D3DHResultToString(hr).c_str() );
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjectsGDI::ExecuteResourceAccess - GetDC failed.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto cleanup;
	}
	 
	// Write to the shared resource with GDI
	COLORREF Color;
	BYTE* pColorData = pDataByte2;
	for (UINT row = 0; row < m_Height; row++)
	{
		for (UINT col = 0; col < m_Width; col++)
		{
			// reverse RGB order since the format for GDI is BGR
			Color = RGB(pColorData[2], pColorData[1], pColorData[0]);
			::SetPixel(hdc, col, row, Color);
			pColorData += 4;
		}
	}
					   
	hr = pDXGISurface1->ReleaseDC(NULL);
	// wait for a sec to let GDI finish its job
	Sleep(1000); 
	if ( FAILED (hr) )
	{
		pTest->WriteToLog(_T("DestObjectsGDI::ExecuteResourceAccess(): ReleaseDC failed. hr = %s"), D3DHResultToString(hr).c_str() );
		strcpy_s(pTest->m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "DestObjectsGDI::ExecuteResourceAccess - ReleaseDC failed.");
		pTest->m_sharedMemory->Result = hr;
		tr = RESULT_FAIL;
		goto cleanup;
	}
  
cleanup:
	SAFE_RELEASE(pDXGISurface1);
	return tr;


}

TEST_RESULT ShareTilePoolChild::Setup()
{
	TEST_RESULT tr = CD3D11Test::Setup( );
	if( tr != RESULT_PASS )
		return tr;

	m_isChildProcess = true;
	
	return tr;
}

static const UINT c_ValidD3DCreateFlags = D3D11_CREATE_DEVICE_SINGLETHREADED | 
									 D3D11_CREATE_DEVICE_DEBUG | 
									 D3D11_CREATE_DEVICE_SWITCH_TO_REF | 
									 D3D11_CREATE_DEVICE_PREVENT_INTERNAL_THREADING_OPTIMIZATIONS;


TEST_RESULT ShareTilePoolChild::SetupD3DObjects()
{	
	IDXGIDevice* pDXGI = NULL;
	IDXGIAdapter* pAdapter = NULL;
	ID3D11Device* pExisting = GetDevice();
	ID3D11Device* pDevice = NULL;
	TEST_RESULT tr = RESULT_PASS;

	HRESULT hr = pExisting->QueryInterface(__uuidof(IDXGIDevice), (void**)(&pDXGI));
	if (FAILED(hr) || pDXGI == NULL)
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupD3DObjects - QueryInterface failed. hr = %s" ), D3DHResultToString(hr).c_str());
		goto func_end;
	}
	hr = pDXGI->GetAdapter(&pAdapter);
	const UINT createFlags = c_ValidD3DCreateFlags & pExisting->GetCreationFlags();
	D3D_FEATURE_LEVEL fl = m_FeatureLevel;
	hr = D3D11CreateDevice_11or12(
        g_App.m_D3DOptions.Force11On12 ? UseD3D11On12_D3D11CreateDeviceFlag : UseD3D11,
		pAdapter, 
		m_DriverType, 
		NULL, createFlags, 
		&fl, 
		1, 
		D3D11_SDK_VERSION, 
		&pDevice, 
		NULL, 
		NULL,
        NULL);
	
	if ( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupD3DObjects - CreateDevice failed. hr = %s" ), D3DHResultToString(hr).c_str());
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"ShareTilePoolTest::SetupD3DObjects - CreateDevice failed.");
		m_sharedMemory->Result = hr;
		goto func_end;
	}

	hr = pDevice->QueryInterface( __uuidof(ID3D11Device2), (void**)&m_pDevice );
	if( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupD3DObjects - QI Device2 failed. hr = %s" ), D3DHResultToString(hr).c_str());
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
			"ShareTilePoolTest::SetupD3DObjects - CreateDevice failed.");
		m_sharedMemory->Result = hr;
		goto func_end;
	}
	m_pDevice->GetImmediateContext2(&m_pContext);

	if ( m_bMiscSharedNTHandle )
	{
		hr = m_pDevice->OpenSharedResourceByName(
			HANDLE_NAME_VALID, 
			DXGI_SHARED_RESOURCE_READ | DXGI_SHARED_RESOURCE_WRITE, 
			__uuidof(ID3D11Buffer), 
			(void **)&m_pTilePool);

		if (FAILED(hr))
		{
			WriteToLog( _T( "ShareTilePoolTest::SetupD3DObjects() - Failed to open shared resource. hr = %s" ), D3DHResultToString(hr).c_str());
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"ShareTilePoolTest::SetupD3DObjects - OpenSharedResourceByName() Failed to open shared resource.");
			m_sharedMemory->Result = hr;
			goto func_end;
		}
	}
	else
	{
		hr = m_pDevice->OpenSharedResource(m_SharedHandle, __uuidof(ID3D11Buffer), (void**) &m_pTilePool);
		if (FAILED(hr))
		{
			WriteToLog( _T( "ShareTilePoolTest::SetupD3DObjects() - Failed to open shared resource. hr = %s" ), D3DHResultToString(hr).c_str());
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "ShareTilePoolTest::SetupD3DObjects() - Failed to open shared resource.");
			m_sharedMemory->Result = hr;
			goto func_end;
		}
	}

	D3D11_BUFFER_DESC DescBuffer;
	DescBuffer.ByteWidth = g_NumTiles*D3D11_2_TILED_RESOURCE_TILE_SIZE_IN_BYTES;
	DescBuffer.BindFlags = 0;
	DescBuffer.Usage = D3D11_USAGE_STAGING;
	DescBuffer.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
	DescBuffer.MiscFlags = 0;
	hr = m_pDevice->CreateBuffer( &DescBuffer, NULL, &m_pStagingBuffer );
	if( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - Creating staging buffer failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		m_sharedMemory->Result = hr;
		goto func_end;
	}

	D3D11_TEXTURE2D_DESC tex2DDesc;
	tex2DDesc.ArraySize			= 1;
	tex2DDesc.BindFlags			= D3D11_BIND_SHADER_RESOURCE;
	tex2DDesc.CPUAccessFlags	= 0;
	tex2DDesc.Format			= DXGI_FORMAT_R32_UINT;
	tex2DDesc.Height			= 1024;
	tex2DDesc.Width				= 1024;
	tex2DDesc.MipLevels			= 1;
	tex2DDesc.MiscFlags			= D3D11_RESOURCE_MISC_TILED;
	tex2DDesc.SampleDesc.Count	= 1;
	tex2DDesc.SampleDesc.Quality= 0;
	tex2DDesc.Usage				= D3D11_USAGE_DEFAULT;
	hr = m_pDevice->CreateTexture2D( &tex2DDesc, NULL, &m_pTiledTexture );
	if( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolTest::SetupTestCase - Creating tiled texture failed. hr = %s" ), 
			D3DHResultToString(hr).c_str());
		m_sharedMemory->Result = hr;
		goto func_end;
	}
func_end:
	SAFE_RELEASE(pDXGI);
	SAFE_RELEASE(pAdapter);
	SAFE_RELEASE(pDevice);
	if( FAILED(hr) ) return RESULT_FAIL;
	return tr;
}

TEST_RESULT ShareTilePoolChild::SetupTestCase()
{
	//minimize window
	if (!GetFramework()->IsMobile())
	{
		ShowWindow(GetActiveWindow(), SW_MINIMIZE);
	}
	TEST_RESULT tr = RESULT_PASS;
	return tr;
}

TEST_RESULT ShareTilePoolChild::ExecuteTestCase()
{
	HRESULT hr = S_OK;
	TEST_RESULT tr;
	IDXGIKeyedMutex *pDXGIKeyedMutex = NULL;

	//Open the file mapping to the shared memory
	m_hFileMap = ::OpenFileMapping(	FILE_MAP_ALL_ACCESS, 
									FALSE, 
									_T("SharedTilePoolMem") );

	if( NULL == m_hFileMap )
	{
		WriteToLog( _T( "SharedResourcesChild::ExecuteTestCase - OpenFileMapping failed. %d " ), GetLastError());
		return RESULT_SKIP;
	}
	 
	// Obtain a pointer of SHAREDMEMORY from the handle to file mapping object
	m_sharedMemory = (TILE_POOL_SHARED_MEMORY*)::MapViewOfFile(	m_hFileMap,
														FILE_MAP_WRITE,
														0,
														0,
														sizeof(TILE_POOL_SHARED_MEMORY));	

	m_startChildResAccessEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "StartChildResAccess");
	if (m_startChildResAccessEvent == NULL) 
	{ 
		WriteToLog(_T( "ShareTilePoolChild::ExecuteTestCase - OpenEvent failed (StartChildResAccess) (%d)\n"), 
			GetLastError());
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"ShareTilePoolChild::ExecuteTestCase - OpenEvent failed (StartChildResAccess).");
		m_sharedMemory->Result = S_FALSE;
		return RESULT_SKIP;
	}

	m_endChildResAccessEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "EndChildResAccess");
	if (m_endChildResAccessEvent == NULL) 
	{ 
		WriteToLog(_T( "ShareTilePoolChild::ExecuteTestCase - OpenEvent failed (EndChildResAccess) (%d)"), 
			GetLastError());
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"ShareTilePoolChild::ExecuteTestCase - OpenEvent failed (EndChildResAccess).");
		m_sharedMemory->Result = S_FALSE;
		return RESULT_SKIP;
	}

	m_exitChildProcessEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "ExitChildProcess");
	if (m_exitChildProcessEvent == NULL) 
	{ 
		WriteToLog(_T( "ShareTilePoolChild::ExecuteTestCase - OpenEvent failed (ExitChildProcess) (%d)"), 
			GetLastError());
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"ShareTilePoolChild::ExecuteTestCase - OpenEvent failed (ExitChildProcess).");
		m_sharedMemory->Result = S_FALSE;
		return RESULT_SKIP;
	}

	m_finishUpdateSharedMemEvent = OpenEvent(EVENT_ALL_ACCESS, FALSE, "FinishUpdateSharedMem");
	if (m_finishUpdateSharedMemEvent == NULL) 
	{ 
		WriteToLog(_T( "ShareTilePoolChild::ExecuteTestCase - OpenEvent failed (FinishUpdateSharedMem) (%d)"), 
			GetLastError());
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"ShareTilePoolChild::ExecuteTestCase - OpenEvent failed (FinishUpdateSharedMem).");
		m_sharedMemory->Result = S_FALSE;
		return RESULT_SKIP;
	}

	m_endChildCaseWithError = OpenEvent(EVENT_ALL_ACCESS, FALSE, "EndChildCaseWithError");
	if (m_endChildCaseWithError == NULL) 
	{ 
		WriteToLog(_T( "ShareTilePoolChild::ExecuteTestCase - OpenEvent failed (EndChildCaseWithError) (%d)"), 
			GetLastError());
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"ShareTilePoolChild::ExecuteTestCase - OpenEvent failed (EndChildCaseWithError).");
		m_sharedMemory->Result = S_FALSE;
		return RESULT_SKIP;
	} 

	while(TRUE) //not signaled to start
	{
		if (WaitForSingleObject(m_exitChildProcessEvent, 10) == WAIT_OBJECT_0)
		{
			WriteToLog( _T( "ShareTilePoolChild::ExecuteTestCase - Recieved exitChildProcessEvent signal from parent."));
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"ShareTilePoolChild::ExecuteTestCase - Recieved exitChildProcessEvent signal from parent.");
			break;
		}

		//wait for signal to access the shared memory. 
		if (WaitForSingleObject(m_finishUpdateSharedMemEvent, 30000) != WAIT_OBJECT_0)
		{
			WriteToLog( _T( "ShareTilePoolChild::ExecuteTestCase - Failed to recieve FinishUpdateSharedMem signal from parent."));
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"ShareTilePoolChild::ExecuteTestCase - Failed to recieve FinishUpdateSharedMem signal from parent.");
			m_sharedMemory->Result = S_FALSE;
			tr = RESULT_SKIP;
			goto exitChild;
		}
		strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase - No Errors.");

		// cleanup previous data
		CleanupAllD3DObjects();

		//get test params
		m_FeatureLevel = m_sharedMemory->FeatureLevel;
		m_DriverType = m_sharedMemory->DriverType;
		m_SharedHandle = m_sharedMemory->SharedHandle;
		m_SyncMethod = m_sharedMemory->SyncMethod;
		m_bMiscSharedNTHandle = m_sharedMemory->isNTHandle;

		tr = SetupD3DObjects();
		if( tr != RESULT_PASS )
		{
			WriteToLog( _T( "ShareTilePoolChild::ExecuteTestCase() - Failed to set up D3D objects."));
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase: Failed to set up D3D objects.");
			m_sharedMemory->Result =E_FAIL;
			tr = RESULT_FAIL;
			goto exitChild;
		}
		
		// syncing with the parent process
		if ( m_SyncMethod == SR_KEYED_MUTEX )
		{
			hr = m_pTilePool->QueryInterface(__uuidof(IDXGIKeyedMutex), (void**)( &pDXGIKeyedMutex ));
			if ( hr != S_OK )
			{
				WriteToLog( _T( "ShareTilePoolChild::ExecuteTestCase() - QIKeyedMutex() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
				strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase: QIKeyedMutex() failed.");
				m_sharedMemory->Result =E_FAIL;
				tr = RESULT_FAIL;
				goto exitChild;
			}

			// Acquire a lock to the resource.
			hr = pDXGIKeyedMutex->AcquireSync(1, INFINITE);
			if ( hr != S_OK)
			{
				WriteToLog( _T( "ShareTilePoolChild::ExecuteTestCase() - AcquireSync() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
				strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase: AcquireSync() failed.");
				m_sharedMemory->Result =E_FAIL;
				tr = RESULT_FAIL;
				goto exitChild;
			}
		}
		else
		{
			//wait for signal to access the shared resources
			if (WaitForSingleObject(m_startChildResAccessEvent, 5000) != WAIT_OBJECT_0)
			{
				WriteToLog( _T( "ShareTilePoolChild::ExecuteTestCase - Failed to recieve StartChildResAccessEvent signal from parent."));
				strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
					"ShareTilePoolChild::ExecuteTestCase - Failed to recieve StartChildResAccessEvent signal from parent.");
				m_sharedMemory->Result =S_FALSE;
				tr = RESULT_SKIP;
				goto exitChild;
			}
		}
		// See if tile pool sharing worked
		tr = VerifySharedTilePool();
		if( tr != RESULT_PASS )
		{
			WriteToLog( _T( "ShareTilePoolChild::VerifySharedTilePool() - Tile Pool did not share correctly. (HR = %s)"), D3DHResultToString(hr).c_str());
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "SharedResourcesChild::ExecuteTestCase: Tile Pool did not share correctly.");
			m_sharedMemory->Result =E_FAIL;
			if ( m_SyncMethod == SR_KEYED_MUTEX )
			{
				pDXGIKeyedMutex->ReleaseSync(0);
			}
			else
			{
				SetEvent(m_endChildResAccessEvent);
			}
			tr = RESULT_FAIL;
			goto exitChild;
		}

		// syncing with the parent process
		if ( m_SyncMethod == SR_KEYED_MUTEX )
		{
			// Release the lock to the parent process.
			hr = pDXGIKeyedMutex->ReleaseSync(0);
			SAFE_RELEASE( pDXGIKeyedMutex );
			if ( hr != S_OK)
			{
				WriteToLog( _T( "ShareTilePoolChild::ExecuteTestCase() - ReleaseSync() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
				strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, "ShareTilePoolChild::ExecuteTestCase:  ReleaseSync() failed.");
				m_sharedMemory->Result =E_FAIL;
				tr = RESULT_FAIL;
				goto exitChild;
			}
		}
		else
		{
			//signal test complete
			if (! SetEvent(m_endChildResAccessEvent) ) 
			{ 
				WriteToLog( _T( "ShareTilePoolChild::ExecuteTestCase - Failed to signal EndChildResAccessEvent to the parent process."));
				strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
					"ShareTilePoolChild::ExecuteTestCase - Failed to signal EndChildResAccessEvent to the parent process.");
				m_sharedMemory->Result =S_FALSE;
				tr = RESULT_SKIP;
				goto exitChild;
			}
		}
		continue;

		// If synchronization failure happens, goto exitChild directly as there's no need to continue the loop; 
		// for other failures that happen before ExecuteResourceAccess is called, goto testdone to skip the current test case and continue.

// testdone: <-- unused for this test

		// signal test case finished with error
		if (! SetEvent(m_endChildCaseWithError) ) 
		{ 
			WriteToLog( _T( "ShareTilePoolChild::ExecuteTestCase - Failed to signal EndChildCaseWithError to the parent process."));
			strcpy_s(m_sharedMemory->ResultString, SR_RESULT_STRING_LENGTH, 
				"ShareTilePoolChild::ExecuteTestCase - Failed to signal EndChildCaseWithError to the parent process.");
			goto exitChild;
		}

		continue;
		
	}

exitChild:
	SAFE_RELEASE( pDXGIKeyedMutex );
	return tr;
}

TEST_RESULT ShareTilePoolChild::VerifySharedTilePool()
{
	TEST_RESULT tr = RESULT_PASS;
	HRESULT hr = S_OK;

	D3D11_TILED_RESOURCE_COORDINATE TRC;
	D3D11_TILE_REGION_SIZE TRS;
	TRC.X = 1;
	TRC.Y = 1;
	TRC.Z = 0;
	TRC.Subresource = 0;
	TRS.NumTiles = g_NumTiles;
	TRS.bUseBox = TRUE;
	TRS.Width = 5;
	TRS.Height = 5;
	TRS.Depth = 1;
	const UINT NumRanges = 1;
	UINT TilePoolRangeStartOffsets[NumRanges] = {0};
	UINT RangeTileCounts[NumRanges] = {25};
	hr = m_pContext->UpdateTileMappings(m_pTiledTexture,1,&TRC,&TRS,m_pTilePool,NumRanges,NULL,TilePoolRangeStartOffsets,RangeTileCounts,0);
	if( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolChild::VerifySharedTilePool() - UpdateTileMappings() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto testDone;
	}

	// Examine what the parent process wrote to us in the tile pool
	m_pContext->CopyTiles(m_pTiledTexture,&TRC,&TRS,m_pStagingBuffer,0,D3D11_TILE_COPY_SWIZZLED_TILED_RESOURCE_TO_LINEAR_BUFFER);
	D3D11_MAPPED_SUBRESOURCE mapped;
	hr = m_pContext->Map(m_pStagingBuffer,0,D3D11_MAP_READ_WRITE,0,&mapped);
	if( FAILED(hr) )
	{
		WriteToLog( _T( "ShareTilePoolChild::VerifySharedTilePool() - Map() failed. (HR = %s)"), D3DHResultToString(hr).c_str());
		tr = RESULT_FAIL;
		goto testDone;
	}
	UINT* pData = (UINT*)mapped.pData;
	for(UINT i = 0; i < g_NumTiles*D3D11_2_TILED_RESOURCE_TILE_SIZE_IN_BYTES/sizeof(UINT); i++)
	{
		UINT expected = i;// parent wrote i
		if( pData[i] != expected ) 
		{
			WriteToLog( _T( "ShareTilePoolChild::VerifySharedTilePool() - Data written by parent process not correct. Expected %u at UINT[%u] but saw %u instead."), 
				expected,i,pData[i]);
			m_pContext->Unmap(m_pStagingBuffer,0);
			tr = RESULT_FAIL;
			goto testDone;
		}
		pData[i] = i*2; // write i*2 back to parent
	}
	m_pContext->Unmap(m_pStagingBuffer,0);

	m_pContext->CopyTiles(m_pTiledTexture,&TRC,&TRS,m_pStagingBuffer,0,D3D11_TILE_COPY_LINEAR_BUFFER_TO_SWIZZLED_TILED_RESOURCE);

	// Force GPU to complete all pending operations before leaving
	m_pContext->Flush();
	if(!BlockingFinish())
	{
		tr = RESULT_FAIL;
		goto testDone;
	}

testDone:
	return tr;
}

