ock.

			default:
				// These are the only formats available on feature level 9.
				switch (m_ModeInformation.ModeDesc.Format)
				{
					case DXGI_FORMAT_B8G8R8X8_UNORM:
					case DXGI_FORMAT_B8G8R8A8_UNORM:
					case DXGI_FORMAT_R8G8B8A8_UNORM:
					break;

					default:
						WriteToLog("*** Flip model swapchains not supported with this pixel format.  Skipping test case. ***");
						test_result = RESULT_SKIP;
						goto Cleanup;
				}
		}
	} // End of Flip Model compatibility checks

	m_RotationMode = DisplayOrientationToDxgiModeRotation(m_ModeInformation.devmode.dmDisplayOrientation);


	//
	// Increment the current test case index.
	//

	++m_iCurrentTestCase;

	if(m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_TEST || m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_DO_NOT_SEQUENCE)
	{
		texturesToLoad += 1;
	}


	//
	// Check adapter sys/vid memory and skip cases that might fail due to too large memory usuage
	//
	
	// Backbuffers - in video memory
	SIZE_T backBufferDim = CFormatInfoMap::Find( m_ModeInformation.ModeDesc.Format )->uBitsPerElement;	// get bitsPerElement
	backBufferDim =	( backBufferDim >> 3 ) * m_ModeInformation.ModeDesc.Height * m_ModeInformation.ModeDesc.Width; // in bytes
	
	// In video memory: Backbuffers + textures bound as shader resources
	SIZE_T vidMemSize = backBufferDim * m_BufferCount;
	if( m_bNonPrerotated )
		vidMemSize *= 2;	// NonPrerotated swapchains induces memory cost of 1 extra back buffer
	vidMemSize *= ( m_ModeInformation.uiMultisampleCount + 1 );	// multisample swapchain backbuffers with size*sample count +1 regular swapchain backbuffer

	vidMemSize += backBufferDim * texturesToLoad;

	// Staging textures: loaded reference textures + validation textures for screen grab
	SIZE_T sysMemSize = backBufferDim * texturesToLoad;
	sysMemSize += backBufferDim * 2 ; // 1 each for regular and multisample  

	CDXGITestFramework * pFramework = dynamic_cast< CDXGITestFramework * >( GetFramework() );
	if( m_ModeInformation.uiDXGIAdapterOrdinal < m_vecAdapterDescriptors.size() )	
	{		
		pFramework->WriteToLogF
		(
			L"CPresentFS::ExecuteTestCase: Adapter %u : %s : vidMemReq=%Iu MB, sharedsysMemReq= %Iu MB, DedicatedVideoMemory= %Iu MB, DedicatedSystemMemory= %Iu MB, SharedSystemMemory= %Iu MB",
			m_ModeInformation.uiDXGIAdapterOrdinal,
			(const WCHAR*)m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].Description,
			(vidMemSize >> 20) + 1,
			(sysMemSize >> 20) + 1,
			m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].DedicatedVideoMemory >> 20,
			m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].DedicatedSystemMemory >> 20,
			m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].SharedSystemMemory >> 20
		);

		if( (m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].DedicatedVideoMemory / 2 ) <  vidMemSize || // rough estimate
			(m_vecAdapterDescriptors[m_ModeInformation.uiDXGIAdapterOrdinal].SharedSystemMemory / 2 ) <  (vidMemSize + sysMemSize) )
		{
			WriteToLog( "****WARNING: CPresentFS::ExecuteTestCase: Testcase uses large percentage of available video memory reported by the adapter. Unexpected behavior might occur." );
			m_bLargeMemoryUsage = true;
		}
	}
	else
	{
		WriteToLog
		(
			"CPresentFS::ExecuteTestCase: Could not find adapter description."
		);        

		test_result = RESULT_ABORT;
		goto Cleanup;
	}


	//
	// Set up our device and swap chain.
	//

	CreateDeviceAndSwapChainResult = CreateDeviceAndSwapChain();

	if ( CreateDeviceAndSwapChainResult == RESULT_FAIL )
	{
		WriteToLog
		(
			"CreateDeviceAndSwapChain failed."
		);

		test_result = CreateDeviceAndSwapChainResult;
		goto Cleanup;
	}
	else if ( CreateDeviceAndSwapChainResult != RESULT_PASS )
	{
		test_result = CreateDeviceAndSwapChainResult;
		goto Cleanup;
	};


	//
	// Get a descriptor for our swap chain.
	//

	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	hr = m_pSwapChain->GetDesc( &swapChainDesc );
	if ( FAILED( hr ) )
	{
		WriteToLog
		(
			"GetDesc on swap chain failed with error 0x%x.",
			hr
		);

		goto Cleanup;
	};


	//
	// Make our multisampled swap chain full-screen.
	//

	if ( m_ModeInformation.uiMultisampleCount > 1 )
	{
		hr = m_pSwapChain->SetFullscreenState
		(
			FALSE,
			NULL
		);

		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"SetFullscreenState failed with error 0x%x.",
				hr
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );

			goto Cleanup;
		};


		hr = m_pSwapChainMS->SetFullscreenState
		(
			TRUE,
			pDeviceWindowOutput
		);

		if
		(
			hr == DXGI_ERROR_NOT_CURRENTLY_AVAILABLE ||
			hr == DXGI_STATUS_OCCLUDED
		)
		{
			WriteToLog
			(
				"Could not make swap chain full-screen.  This may indicate that another window has stolen focus from the test."
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );

			test_result = RESULT_SKIP;
			goto Cleanup;
		}
		else if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"SetFullscreenState on multisampled swap chain failed with error 0x%x.",
				hr
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed even though the swap chain window is in the foreground
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );

			goto Cleanup;
		};
	};

	if ( m_pSwapChain == NULL )
	{
		WriteToLog
		(
			"CPresentFS::ExecuteTestCase:  Swap chain is not initialized."
		);

		// Dump DXGI Journal to get clues as to why Swapchain is NULL
		if( g_Application.m_bLogDXGIJounrnal )
			g_Application.InitiateJournalDump( true );

		goto Cleanup;
	};

	WINDOWINFO deviceWindowInfo;
	deviceWindowInfo.cbSize = sizeof( deviceWindowInfo );

	BOOL bGetWindowInfoResult = GetWindowInfo
	(
		m_hwndDeviceWindow,
		&deviceWindowInfo
	);

	if ( bGetWindowInfoResult != TRUE )
	{
		WriteToLog
		(
			"CPresentFS::ExecuteTestCase:  GetWindowInfo failed."
		);

		goto Cleanup;
	};


	if ( !UpdateOutputWindowInformation( m_pSwapChain, deviceWindowInfo ) )
	{
		WriteToLog
		(
			"UpdateOutputWindowInformation failed."
		);

		goto Cleanup;
	};


	//
	// Create our textures.
	//

	HRESULT hrCreateTexturesResult = CreateTextures
	(
		swapChainDesc.BufferDesc.Format,
		rectDeviceWindow.right - rectDeviceWindow.left,
		rectDeviceWindow.bottom - rectDeviceWindow.top,
		uiDeviceOutputWidth,
		uiDeviceOutputHeight,
		texturesToLoad
	);

	if( FAILED( hrCreateTexturesResult ) )
	{
		WriteToLog
		(
			"CreateTextures failed."
		);

		if ( m_bLargeMemoryUsage && ( hrCreateTexturesResult == E_OUTOFMEMORY ) )
		{
			test_result = RESULT_SKIP;
			WriteToLog( "ExecuteTestCase: Test case using large amount of memory. Resource creation failure expected. Skipping test case. " );
		}

		goto Cleanup;
	};


	DXGI_SWAP_CHAIN_DESC swapChainDescMS;

	if ( m_ModeInformation.uiMultisampleCount > 1 )
	{
		UpdateOutputWindowInformation( m_pSwapChainMS, deviceWindowInfo );

		hr = m_pSwapChainMS->GetDesc( &swapChainDescMS );
		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"GetDesc on multisampled swap chain failed with error 0x%x.",
				hr
			);

			goto Cleanup;
		};


		if ( m_bNonPrerotated )
		{
			swapChainDescMS.Flags |= DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;
		}
		else
		{
			swapChainDescMS.Flags &= ~DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;
		};


		hr = ResizeBuffers
		(
			m_pSwapChainMS,
			&swapChainDescMS
		);

		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"ResizeBuffer failed with error 0x%x.",
				hr
			);

			if( hr == E_OUTOFMEMORY  && m_bLargeMemoryUsage )
			{
				test_result = RESULT_SKIP;
				WriteToLog( "ExecuteTestCase: Test case using large amount of memory. ResizeBuffers failure expected. Skipping test case. " );
			}

			goto Cleanup;
		};


		bool bClearResult = TestedSwapChainMS.InitBBViaClear( 0 );
		if ( bClearResult != true )
		{
			WriteToLog
			(
				"Clear on multisampled swap chain failed."
			);

			goto Cleanup;
		};

		HRESULT hrInitializeBackBufferResult = InitializeBackBuffer
		(
			&TestedSwapChainMS,
			0
		);

		if ( FAILED( hrInitializeBackBufferResult ) )
		{
			WriteToLog
			(
				"InitializeBackBuffer failed."
			);

			if( hrInitializeBackBufferResult == E_OUTOFMEMORY && m_bLargeMemoryUsage)
			{
				test_result = RESULT_SKIP;
				WriteToLog( "ExecuteTestCase: Test case using large amount of memory. InitializeBackBuffer failure expected. Skipping test case. " );
			}

			goto Cleanup;
		};

		bool bPerformPresentResult = PerformPresent
		(
			m_pSwapChainMS,
			0,
			0
		);

		if
		(
			g_Application.GetDebugPause() > 0
		)
		{
			Sleep( g_Application.GetDebugPause() );
		};


		if ( bPerformPresentResult != true )
		{
			WriteToLog
			(
				"PerformPresent on multisampled swap chain failed."
			);

			goto Cleanup;
		};

		bool bGetDisplaySurfaceDataResult = GetDisplaySurfaceData
		(
			m_pGetDSDSurfaceMS,
			true,
			false
		);

		if ( bGetDisplaySurfaceDataResult != true )
		{
			WriteToLog
			(
				"GetDisplaySurfaceData on multisampled swap chain failed."
			);

			goto Cleanup;
		};


		hr = m_pSwapChainMS->SetFullscreenState
		(
			FALSE,
			NULL
		);

		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"SetFullscreenState while returning multisampled swap chain to windowed mode failed with error 0x%x.",
				hr
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );

			goto Cleanup;
		};


		//
		// Re-assert the mode.
		//

		hr = SetDisplayMode( pDeviceWindowOutput );
		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"SetDisplayMode  failed with error 0x%x.",
				hr
			);

			goto Cleanup;
		};


		//
		// Restore the original swap chain to full-screen.
		//

		hr = m_pSwapChain->SetFullscreenState
		(
			TRUE,
			pDeviceWindowOutput
		);

		if ( FAILED( hr ) )
		{
			WriteToLog
			(
				"SetFullscreenState while returning swap chain to full-screen failed with error 0x%x.",
				hr
			);

			CheckForegroundWindow( m_szFocusWindowClassName );
			// Dump DXGI Journal to get clues as to why SetFullscreenState failed
			if( g_Application.m_bLogDXGIJounrnal )
				g_Application.InitiateJournalDump( true );

			goto Cleanup;
		};

		WINDOWINFO deviceWindowInfo;
		deviceWindowInfo.cbSize = sizeof( deviceWindowInfo );

		BOOL bGetWindowInfoResult = GetWindowInfo
		(
			m_hwndDeviceWindow,
			&deviceWindowInfo
		);

		if ( bGetWindowInfoResult != TRUE )
		{
			WriteToLog
			(
				"CPresentFS::ExecuteTestCase:  GetWindowInfo failed."
			);

			goto Cleanup;
		};


		UpdateOutputWindowInformation( m_pSwapChain, deviceWindowInfo );
	};


	if ( m_bNonPrerotated )
	{
		swapChainDesc.Flags |= DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;
	}
	else
	{
		swapChainDesc.Flags &= ~DXGI_SWAP_CHAIN_FLAG_NONPREROTATED;
	};


	hr = ResizeBuffers
	(
		m_pSwapChain,
		&swapChainDesc
	);

	if ( FAILED( hr ) )
	{
		WriteToLog
		(
			"ResizeBuffers failed with error 0x%x.",
			hr
		);

		if( hr == E_OUTOFMEMORY  && m_bLargeMemoryUsage )
		{
			test_result = RESULT_SKIP;
			WriteToLog( "ExecuteTestCase: Test case using large amount of memory. ResizeBuffers failure expected. Skipping test case. " );
		}

		goto Cleanup;
	};


	/*bool bClearResult = TestedSwapChain.InitBBViaClear( 0 );
	if ( bClearResult != true )
	{
		WriteToLog
		(
			"Clear on swap chain back buffer failed."
		);

		goto Cleanup;
	};*/

	bool bPerformPresentResult = false;
	UINT uiCount;
	if( swapChainDesc.SwapEffect != DXGI_SWAP_EFFECT_DISCARD )
	{
		// Render to all of the backbuffers before entering the Present() loop. T
		// This is to test that DXGI_SWAP_EFFECT_SEQUENTIAL* preserves backbuffers
		for
		(
			uiCount = 0;
			uiCount < m_BufferCount;
			++uiCount
		)
		{
			HRESULT hrInitializeBackBufferResult = InitializeBackBuffer
			(
				&TestedSwapChain,
				uiCount % texturesToLoad
			);

			if ( FAILED( hrInitializeBackBufferResult ) )
			{
				WriteToLog
				(
					"InitializeBackBuffer failed."
				);

				if( hrInitializeBackBufferResult == E_OUTOFMEMORY && m_bLargeMemoryUsage)
				{
					test_result = RESULT_SKIP;
					WriteToLog( "ExecuteTestCase: Test case using large amount of memory. InitializeBackBuffer failure expected. Skipping test case. " );
				}

				goto Cleanup;
			};

			// Must Present() to rotate to the next backbuffer
			if( !PerformPresent( m_pSwapChain, 0, 0 ) )
			{
				WriteToLog
				(
					"PerformPresent on swap chain failed."
				);

				goto Cleanup;
			}
		}
	}

	for
	(
		uiCount = 0;
		uiCount < m_NumberOfPresents;
		++uiCount
	)
	{
		if( swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_DISCARD )
		{
			HRESULT hrInitializeBackBufferResult = InitializeBackBuffer
			(
				&TestedSwapChain,
				uiCount % texturesToLoad
			);

			if ( FAILED(hrInitializeBackBufferResult) )
			{
				WriteToLog
				(
					"InitializeBackBuffer failed."
				);

				if( hrInitializeBackBufferResult == E_OUTOFMEMORY && m_bLargeMemoryUsage)
				{
					test_result = RESULT_SKIP;
					WriteToLog( "ExecuteTestCase: Test case using large amount of memory. InitializeBackBuffer failure expected. Skipping test case. " );
				}

				goto Cleanup;
			};
		}


		bPerformPresentResult = PerformPresent
		(
			m_pSwapChain,
			0,
			0
		);

		if ( bPerformPresentResult != true )
		{
			WriteToLog
			(
				"PerformPresent on swap chain failed."
			);

			goto Cleanup;
		};


		if
		(
			g_Application.GetDebugPause() > 0
		)
		{
			Sleep( g_Application.GetDebugPause() );
		};
	}


	//
	// This is to test DXGI_PRESENT_DO_NOT_SEQUENCE and DXGI_PRESENT_TEST the following present should have no effect
	// on what is displayed
	//

	if(m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_TEST || m_PresentFlag.dxgiPresentFlag == DXGI_PRESENT_DO_NOT_SEQUENCE)
	{
		HRESULT hrInitializeBackBufferResult = InitializeBackBuffer
		(
			&TestedSwapChain,
			m_NumberOfPresents % texturesToLoad
		);

		if ( FAILED(hrInitializeBackBufferResult) )
		{
			WriteToLog
			(
				"InitializeBackBuffer failed."
			);

			goto Cleanup;
		};

		bPerformPresentResult = PerformPresent
		(
			m_pSwapChain,
			0,
			m_PresentFlag.dxgiPresentFlag
		);

		if ( bPerformPresentResult != true )
		{
			WriteToLog
			(
				"PerformPresent on swap chain failed."
			);

			goto Cleanup;
		};

		if
		(
			g_Application.GetDebugPause() > 0
		)
		{
			Sleep( g_Application.GetDebugPause() );
		};
	}

	bool bGetDisplaySurfaceDataResult = GetDisplaySurfaceData
	(
		m_pGetDSDSurface,
		false,
		false
	);

	if ( bGetDisplaySurfaceDataResult != true )
	{
		WriteToLog
		(
			"GetDisplaySurfaceData on swap chain failed."
		);

		goto Cleanup;
	};


	//
	// For sRGB and fp16  backbuffers, we need to adjust the initial texture.
	//

	if ( DXGI_FORMAT_R8G8B8A8_UNORM_SRGB == swapChainDesc.BufferDesc.Format || 
		DXGI_FORMAT_B8G8R8A8_UNORM_SRGB == swapChainDesc.BufferDesc.Format ||
		DXGI_FORMAT_R16G16B16A16_FLOAT == swapChainDesc.BufferDesc.Format )   
	{
		hr = LinearToGamma(pBBInitTextureSys[(m_NumberOfPresents -1) % texturesToLoad]);
		if ( FAILED(hr))
		{
			WriteToLog(_T("LinearToGamma failed. 0x%x"), hr);
			goto Cleanup;
		};

	}

	if 
	(
		(
			DXGI_FORMAT_R8G8B8A8_UNORM_SRGB == swapChainDesc.BufferDesc.Format || 
			DXGI_FORMAT_R8G8B8A8_UNORM == swapChainDesc.BufferDesc.Format
		) && 
		m_ModeInformation.uiMultisampleCount > 1 &&
		g_Application.m_D3DOptions.SrcDriverType == D3D10_DRIVER_TYPE_SOFTWARE
	)
	{
		hr = MaskAlphaChannel( m_pGetDSDTexture );
		if ( FAILED(hr))
		{
			WriteToLog(_T("MaskAlphaChannel of Test Texture failed. 0x%x"), hr);
			goto Cleanup;
		};

		hr = MaskAlphaChannel( m_pGetDSDTextureMS );
		if ( FAILED(hr))
		{
			WriteToLog(_T("MaskAlphaChannel of Ref Texture failed. 0x%x"), hr);
			goto Cleanup;
		};
	}


	float fNonConformanceFraction = 0.f;
	bool bEvaluateImageResult = EvaluatePresentedImage
	(
		m_pGetDSDTexture, 
		m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : pBBInitTextureSys[(m_NumberOfPresents -1) % texturesToLoad],
		&fNonConformanceFraction
	);

	if ( bEvaluateImageResult != true )
	{
		WriteToLog
		(
			"EvaluatePresentedImage failed."
		);

		goto Cleanup;
	};


	g_Application.SetLastTestConformance( 1.f - fNonConformanceFraction );
	if ( fNonConformanceFraction > m_fMaxNonConformanceFractionFS)
	{
		WriteToLog
		(
			"FAIL:  Actual conformance of %3.3f%% does not meet expected threshold of %3.3f%%.",
			100.f * ( 1.f - fNonConformanceFraction ),
			100.f * ( 1.f - m_fMaxNonConformanceFractionFS )
		);

		test_result = RESULT_FAIL;


		//
		// Save the result as a DDS file.
		//

		bool bSaveDDS = g_Application.IsSaveDDS();
		if ( bSaveDDS == true )
		{
			//
			// Save the test image.
			//

			TCHAR DumpFileName[ MAX_PATH ];
			StringCchPrintf
			(
				DumpFileName,
				ARRAYSIZE( DumpFileName ),
				m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Test.dds" ) : _T( "DXGIPresent_%s_%u_Ref.dds" ),
				GetPath().ToString().c_str(),
				m_iCurrentTestCase
			);

			HRESULT hSaveFileResult = D3DX10SaveTextureToFile
			(
				(ID3DResource *) m_pGetDSDTexture,
				D3DX10_IFF_DDS,
				DumpFileName
			);

			if ( FAILED( hSaveFileResult ) )
			{
				WriteToLog
				(
					"D3DX10SaveTextureToFile saving %s image to DDS file failed with error 0x%x.",
					m_ModeInformation.uiMultisampleCount <= 1 ? "test" : "reference",
					hSaveFileResult
				);
			}
			else
			{
				WriteToLog
				(
					_T( "Saved %s image to %s." ),
					m_ModeInformation.uiMultisampleCount <= 1 ? "test" : "reference",
					DumpFileName
				);
			};


			//
			// Save the reference image.
			//
			StringCchPrintf
			(
				DumpFileName,
				ARRAYSIZE( DumpFileName ),
				m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Ref.dds" ) : _T( "DXGIPresent_%s_%u_Test.dds" ),
				GetPath().ToString().c_str(),
				m_iCurrentTestCase
			);

			hSaveFileResult = D3DX10SaveTextureToFile
			(
				(ID3DResource *) ( m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : pBBInitTextureSys[(m_NumberOfPresents -1) % texturesToLoad] ),
				D3DX10_IFF_DDS,
				DumpFileName
			);

			if ( FAILED( hSaveFileResult ) )
			{
				WriteToLog
				(
					"D3DX10SaveTextureToFile saving %s image to DDS file failed with error 0x%x.",
					m_ModeInformation.uiMultisampleCount <= 1 ? "reference" : "test",
					hSaveFileResult
				);
			}
			else
			{
				WriteToLog
				(
					_T( "Saved %s image to %s." ),
					m_ModeInformation.uiMultisampleCount <= 1 ? "reference" : "test",
					DumpFileName
				);
			};
		};


		//
		// Save the result as a bitmap.
		//
		bool bSaveBMP = g_Application.IsSaveBMP();
		if ( bSaveBMP == true )
		{
			//
			// Save the test image.
			//

			TCHAR DumpFileName[ MAX_PATH ];
			StringCchPrintf
			(
				DumpFileName,
				ARRAYSIZE( DumpFileName ),
				m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Test.bmp" ) : _T( "DXGIPresent_%s_%u_Ref.bmp" ),
				GetPath().ToString().c_str(),
				m_iCurrentTestCase
			);

			HRESULT hSaveFileResult = D3DX10SaveTextureToFile
			(
				(ID3DResource *) m_pGetDSDTexture,
				D3DX10_IFF_BMP,
				DumpFileName
			);

			if ( FAILED( hSaveFileResult ) )
			{
				WriteToLog
				(
					"D3DX10SaveTextureToFile saving %s image to bitmap failed with error 0x%x. Try to save as DDS",
					m_ModeInformation.uiMultisampleCount <= 1 ? "test" : "reference",
					hSaveFileResult
				);


				//
				// Added this fallback because D3DX10_IFF_BMP is known to fail with DXGI_FORMAT_R8G8B8A8_UNORM_SRGB.
				//

				StringCchPrintf
				(
					DumpFileName,
					ARRAYSIZE( DumpFileName ),
					m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Test.dds" ) : _T( "DXGIPresent_%s_%u_Ref.dds" ),
					GetPath().ToString().c_str(),
					m_iCurrentTestCase
				);

				hSaveFileResult = D3DX10SaveTextureToFile
				(
					(ID3DResource *) m_pGetDSDTexture,
					D3DX10_IFF_DDS,
					DumpFileName
				);

				if ( FAILED( hSaveFileResult ) )
				{
					WriteToLog
					(
						"D3DX10SaveTextureToFile saving %s image to DDS also failed with error 0x%x. Nothing saved",
						m_ModeInformation.uiMultisampleCount <= 1 ? "test" : "reference",
						hSaveFileResult
					);                
				}
			}

			if ( SUCCEEDED( hSaveFileResult ) )
			{
				WriteToLog
				(
					_T( "Saved %s image to %s." ),
					m_ModeInformation.uiMultisampleCount <= 1 ? "test" : "reference",
					DumpFileName
				);
			};


			//
			// Save the reference image.
			//

			StringCchPrintf
			(
				DumpFileName,
				ARRAYSIZE( DumpFileName ),
				m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Ref.bmp" ) : _T( "DXGIPresent_%s_%u_Test.bmp" ),
				GetPath().ToString().c_str(),
				m_iCurrentTestCase
			);

			hSaveFileResult = D3DX10SaveTextureToFile
			(
				(ID3DResource *) ( m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : pBBInitTextureSys[((m_NumberOfPresents -1) % texturesToLoad)] ),
				D3DX10_IFF_BMP,
				DumpFileName
			);

			if ( FAILED( hSaveFileResult ) )
			{
				WriteToLog
				(
					"D3DX10SaveTextureToFile saving %s image to bitmap failed with error 0x%x.",
					m_ModeInformation.uiMultisampleCount <= 1 ? "reference" : "test",
					hSaveFileResult
				);


				//
				// Added this fallback because D3DX10_IFF_BMP is known to fail with DXGI_FORMAT_R8G8B8A8_UNORM_SRGB.
				//

				StringCchPrintf
				(
					DumpFileName,
					ARRAYSIZE( DumpFileName ),
					m_ModeInformation.uiMultisampleCount <= 1 ? _T( "DXGIPresent_%s_%u_Ref.dds" ) : _T( "DXGIPresent_%s_%u_Test.dds" ),
					GetPath().ToString().c_str(),
					m_iCurrentTestCase
				);

				hSaveFileResult = D3DX10SaveTextureToFile
				(
					(ID3DResource *) ( m_ModeInformation.uiMultisampleCount > 1 ? m_pGetDSDTextureMS : pBBInitTextureSys[((m_NumberOfPresents -1) % texturesToLoad)] ),
					D3DX10_IFF_DDS,
					DumpFileName
				);

				if ( FAILED( hSaveFileResult ) )
				{
					WriteToLog
					(
						"D3DX10SaveTextureToFile saving %s image to DDS also failed with error 0x%x. Nothing saved",
						m_ModeInformation.uiMultisampleCount <= 1 ? "reference" : "test",
						hSaveFileResult
					);                
				}			
			}

			if ( SUCCEEDED( hSaveFileResult ) )
			{
				WriteToLog
				(
					_T( "Saved %s image to %s." ),
					m_ModeInformation.uiMultisampleCount <= 1 ? "reference" : "test",
					DumpFileName
				);
			};
		};
	}
	else
	{
		test_result = RESULT_PASS;
	};


Cleanup:
	ReleaseTestCaseObjects();


	if ( m_DevicePointer.pDevice != NULL )
	{
		// @DX11PORT [SEdmison]:
		// Replace this.
		//D3DX10UnsetAllDeviceObjects( m_DevicePointer.pDevice );

		m_DevicePointer.pDevice->Release();
		m_DevicePointer.pDevice = NULL;
	};

	PrimaryAdapterDevice.ReleaseAllObjects();


	DebugOut.Write("Exiting ExecuteTestCase.\n");

	return test_result;
};


CPresentFSMS::CPresentFSMS()
:
m_uMaxQuality( 32 )
{
	m_MultisampleTest = TRUE;
	m_bIgnoreRefresh = true;
	m_bAlwaysFilterLowResolution = true;
	m_fMaxNonConformanceFractionFS = 0.2f;
	PrimaryAdapterDevice.SetAlternateRenderPath( true );
};


HRESULT CPresentFSMS::ModeListTransform
(
	D3D_DEVICE_POINTER devicePointer,
	__in std::vector< ModeInformation >* pModes,
	__out std::vector< ModeInformation >* pTransformedModes
)
{
	HRESULT hr = GetMultisampleQualityLevels
	(
		devicePointer,
		pModes,
		pTransformedModes,
		D3D10_MAX_MULTISAMPLE_SAMPLE_COUNT,
		g_Application.IsCapMultiSampleQuality()
	);

	if(SUCCEEDED(hr) && g_Application.IsMinMultiSampleSet())
	{
		//
		// Only test the highest and lowest MultisampleCounts with the highest and lowest MultisampleQuality
		//  
		GetHighLowMidMultisampleQuality(pTransformedModes);
		GetHighLowMidMultisampleCount(pTransformedModes);
	}

	if(pTransformedModes->size() == 0)
	{
		WriteToLog(_T("CPresentFSMS::ModeListTransform returned 0 modes.  Is Multisampling supported?"));
	}

	return hr;
}


void CPresentFSMS::InitFSMSTestParameters()
{
	//
	// This test group doesn't run under TS.
	//

	if
	(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		goto Cleanup;
	};


	//
	// Call the base class imlementation.
	//
	CPresentFS::InitFSTestParameters();

Cleanup:;
};


void CPresentFSMS::InitTestParameters()
{
	CPresentFSMS::InitFSMSTestParameters();

	RFactor testRFactors = IntitTestRFactors();


	if(m_globalRFactor.Valid() && m_FeatureLevelFactor.Valid())
	{
		//
		// If derived test sets some rfactors use them. If there is an error or they dont exist dont use the.
		//
		if(testRFactors.Valid())
		{
			SetRootTestFactor(m_FeatureLevelFactor * testRFactors * m_globalRFactor );
		}
		else
		{
			SetRootTestFactor(m_FeatureLevelFactor * m_globalRFactor );    
		}
	}
	else
	{
		if( m_FeatureLevelFactor.Valid() )
			SetRootTestFactor( m_FeatureLevelFactor );

		WriteToLog("CPresentFSMS::SetRootTestFactor parameter combination is invalid. m_globalRFactor.Valid=%d m_FeatureLevelFactor.Valid=%d", m_globalRFactor.Valid(), m_FeatureLevelFactor.Valid());
	}
};


TEST_RESULT CPresentFSMS::ExecuteTestCase()
{
	TEST_RESULT retVal = RESULT_PASS;

	retVal = CPresentFS::ExecuteTestCase();

	return retVal;
}


RFactor CPresentFSMS::IntitTestRFactors()
{
	RFactor bufferCount =  AddParameter
	(
		_T("BufferCount"),
		&m_BufferCount,
		RangeValueSet
		(
			(UINT)m_MaxBufferCount,
			(UINT)m_MaxBufferCount,
			(UINT)1
		)
	);

	CUserValueSet< DXGIPresentFlagRecord > *pPresentFlags = new CUserValueSet< DXGIPresentFlagRecord >();

	pPresentFlags->AddValue(DXGIPresentFlags[0]);

	RFactor presentFlags = AddParameter< DXGIPresentFlagRecord >( _T("PresentFlags") , &m_PresentFlag, pPresentFlags);


	CUserValueSet< DXGISwapEffectRecord > *pSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();

	pSwapEffect->AddValue(DXGISwapEffects[0]);

	RFactor swapEffect =  AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

	return bufferCount * presentFlags * swapEffect;
}


CPresentFSRotation::CPresentFSRotation()
{
	m_bIgnoreRefresh = true;
	m_bAlwaysFilterLowResolution = true;
	m_bTestRotatedModes = true;
	m_bUseMinimalFormatList = true;
};


void CPresentFSRotation::InitTestParameters()
{
	//
	// This test group doesn't run under TS.
	//

	if
	(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		return;
	};


	//
	// Call the base class implementation.
	//

	CPresentFS::InitFSTestParameters();

	RFactor testRFactors = IntitTestRFactors();

	RFactor nonPrerotated = AddParameter( _T( "NonPrerotated" ), (int*)(&m_bNonPrerotated), RangeValueSet( 0, 1, 1 ) );

	if(m_globalRFactor.Valid() && m_FeatureLevelFactor.Valid())
	{
		//
		// If derived test sets some rfactors use them. If there is an error or they dont exist dont use the.
		//

		if(testRFactors.Valid())
		{
			SetRootTestFactor(m_FeatureLevelFactor * nonPrerotated * testRFactors * m_globalRFactor );
		}
		else
		{
			SetRootTestFactor(m_FeatureLevelFactor * nonPrerotated * m_globalRFactor );
		}
	}
	else
	{
		if( m_FeatureLevelFactor.Valid() )
			SetRootTestFactor( m_FeatureLevelFactor );

		WriteToLog("CPresentFSRotation::SetRootTestFactor parameter combination is invalid. m_globalRFactor.Valid=%d m_FeatureLevelFactor.Valid=%d", m_globalRFactor.Valid(), m_FeatureLevelFactor.Valid());
	}
};


RFactor CPresentFSRotation::IntitTestRFactors()
{
	RFactor bufferCount =  AddParameter
	(
		_T("BufferCount"),
		&m_BufferCount,
		RangeValueSet
		(
			(UINT)m_MaxBufferCount,
			(UINT)m_MaxBufferCount,
			(UINT)1
		)
	);

	CUserValueSet< DXGIPresentFlagRecord > *pPresentFlags = new CUserValueSet< DXGIPresentFlagRecord >();


	pPresentFlags->AddValue(DXGIPresentFlags[0]);

	RFactor presentFlags = AddParameter< DXGIPresentFlagRecord >( _T("PresentFlags") , &m_PresentFlag, pPresentFlags);

	CUserValueSet< DXGISwapEffectRecord > *pSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();

	pSwapEffect->AddValue(DXGISwapEffects[0]);

	RFactor swapEffect =  AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

	return bufferCount * presentFlags * swapEffect;
}


CPresentFSMulti::CPresentFSMulti()
{
	//PrimaryAdapterDevice.SetAlternateRenderPath( false );
	m_bTestRotatedModes = true;
	m_bOneModeOnly = true;
	m_bUseMinimalFormatList = true;
	// this is not the main test for quality levels, so limit it
	m_uMaxQuality = 2;
};


void CPresentFSMulti::InitTestParameters()
{
	//
	// This test group doesn't run under TS.
	//

	if
	(
		g_Application.IsTerminalServices() == true ||
		GetSystemMetrics( SM_REMOTESESSION ) != 0
	)
	{
		return;
	};


	//
	// Call the base class implementation.
	//

	CPresentFSMS::InitFSMSTestParameters();

	RFactor scaling = AddParameter( _T( "Scaling" ), &m_Scaling, RangeValueSet( 75, 100, 125 ) );
	RFactor scalingMS = AddParameter( _T( "ScalingMS" ), &m_ScalingMS, RangeValueSet( 75, 100, 125 ) );
	RFactor nonPrerotated = AddParameter( _T( "NonPrerotated" ), (int*)(&m_bNonPrerotated), RangeValueSet( 0, 1, 1 ) );


	RFactor testRFactors= IntitTestRFactors();

	if( m_globalRFactor.Valid() && m_FeatureLevelFactor.Valid() )
	{
		//
		// If derived test sets some rfactors use them. If there is an error or they dont exist dont use the.
		//
		if(testRFactors.Valid())
		{
			SetRootTestFactor(m_FeatureLevelFactor * testRFactors * scaling * scalingMS * nonPrerotated * m_globalRFactor );
		}
		else
		{
			SetRootTestFactor(m_FeatureLevelFactor * scaling * scalingMS * nonPrerotated * m_globalRFactor );    
		}
	}
	else
	{
		//
		// Error no modes set.  Execute test case will pick this up
		//
		if( m_FeatureLevelFactor.Valid() )
			SetRootTestFactor( m_FeatureLevelFactor );		        

		WriteToLog("CPresentFSMulti::SetRootTestFactor parameter combination is invalid. m_globalRFactor.Valid=%d m_FeatureLevelFactor.Valid=%d", m_globalRFactor.Valid(), m_FeatureLevelFactor.Valid());
	}
};


HRESULT CPresentFSMulti::ResizeBuffers
(
	IDXGISwapChain * pSwapChain,
	DXGI_SWAP_CHAIN_DESC * pSwapChainDesc
)
{
	int Scaling = pSwapChainDesc->SampleDesc.Count > 1? m_ScalingMS : m_Scaling;

	return pSwapChain->ResizeBuffers
	(
		pSwapChainDesc->BufferCount,
		pSwapChainDesc->BufferDesc.Width * Scaling / 100,
		pSwapChainDesc->BufferDesc.Height * Scaling / 100,
		pSwapChainDesc->BufferDesc.Format,
		pSwapChainDesc->Flags
	);
};


RFactor CPresentFS::IntitTestRFactors()
{
	RFactor bufferCount =  AddParameter
	(
		_T("BufferCount"),
		&m_BufferCount,
		RangeValueSet
		(
			(UINT)m_MaxBufferCount,
			(UINT)m_MaxBufferCount,
			(UINT)1
		)
	);

	CUserValueSet< DXGIPresentFlagRecord > *pPresentFlags = new CUserValueSet< DXGIPresentFlagRecord >();

	pPresentFlags->AddValue(DXGIPresentFlags[0]);

	RFactor presentFlags = AddParameter< DXGIPresentFlagRecord >( _T("PresentFlags") , &m_PresentFlag, pPresentFlags);


	CUserValueSet< DXGISwapEffectRecord > *pSwapEffect = new CUserValueSet< DXGISwapEffectRecord >();
	for( UINT i = 0; i < cuiNoOfSwapEffects; i++ )
	{
		pSwapEffect->AddValue(DXGISwapEffects[i]);
	}

	RFactor swapEffect =  AddParameter< DXGISwapEffectRecord >( _T("SwapEffect") , &m_SwapEffect, pSwapEffect);

	return bufferCount * presentFlags * swapEffect;
}
// Copyright(C) Microsoft. All rights reserved.

#include "stdafx.h"
#include <kaiser.h>
//#include <mfrate.h>
//#include <wmcodecdsp.h>

#define MAX_RATES 16

//
// TODO: figure out why the 2 channel doesn't work for KMixer and large frequencies!!
// Weird!
DWORD Test_FixedFormatConvertor_SNR32(SRC_ALGORITHM_TYPE SrcType, 
									  UINT nChannels = 1, 
									  AUDIO_FLOW_TYPE SrcFlow = AUDIO_FLOW_PUSH, 
									  BOOL bUseVariableFrequency = false)
{
    SLOG(eInfo1, "Create a graph Mem-->32-->Mixer-->SRC-->-->32-->Mem");
    SLOG(eInfo1, "The test determines the SNR of the converter for %s quality %ld channels and flow %s (24 bit)", 
					(SrcType==SRC_QUALITY_LINEAR) ? "LINEAR" : "OTHER",
					 nChannels,
					(SrcFlow==AUDIO_FLOW_PUSH) ? "PUSH" : "PULL");
 
    //
    // Input/output formats
    AUDIO_FRAME_FORMAT InputFormat, OutputFormat;


    //
    // Structure used for test pairs
    typedef struct _SRPAIR
    {
        float  finputSR, foutputSR, fFrequency;
        double dSNR;
    } SRPAIR;

    UINT32 u,v;

    //
    // Initialize test matrix
    float aFrequencies[] = { 8000, 15999, 16000, 11025, 22050, 31999, 32000, 44100, 48000, 96000 }; 
    UINT32 u32MatrixDimension = sizeof(aFrequencies)/sizeof(aFrequencies[0]);
    SRPAIR  aTestMatrix[MAX_RATES][MAX_RATES];
	
	//
	// For stereo mode we only test a restricted number of conversion
	if (nChannels > 1)
		u32MatrixDimension = 2;

	//
	// Fill the test matrix
    for (u=0; u < u32MatrixDimension; u++)
        for (v=0; v < u32MatrixDimension; v++)
        {
            aTestMatrix[u][v].finputSR  = aFrequencies[u];
            aTestMatrix[u][v].foutputSR = aFrequencies[v];
			if (bUseVariableFrequency)
			{
				// always use a frequency near input signal Nyquist
				// note that if we downsample we expect the SNR to be zero
				// otherwise as good as possible
	            aTestMatrix[u][v].fFrequency= min(20000, min(aTestMatrix[u][v].finputSR/2,aTestMatrix[u][v].foutputSR/2)) - 1000;
			}
			else
	            aTestMatrix[u][v].fFrequency= 997.00;

            aTestMatrix[u][v].dSNR= 0;
        }
    

    //
    // For each pair of input/output frequencies
    for (u=0; u < u32MatrixDimension; u++)
        for (v=0; v < u32MatrixDimension; v++)
        {
			//
            // create input out/formats
            BUILD_FORMAT(&InputFormat,
                        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 
                        nChannels,
                        4,
                        32,
                        aTestMatrix[u][v].finputSR);

            BUILD_FORMAT(&OutputFormat,
                        KSDATAFORMAT_SUBTYPE_IEEE_FLOAT, 
                        nChannels,
                        4,
                        32,
                        aTestMatrix[u][v].foutputSR);

            // create graph descriptors
            GRAPH_APO_DESCRIPTOR aApoDescriptors[] = { { APO_MIXER, NULL } , { APO_SRC , NULL } };
            GRAPH_BINDING_DESCRIPTOR aBindingDescriptors[] = { 
                { 0, 0, true  }, 
                { 0, 1, false },
                { 1, 1, true  },
                { 1, 2, false } };
            GRAPH_CONNECTION_DESCRIPTOR aConnectionDescriptors[3]; 
            
			ZeroMemory(aConnectionDescriptors, sizeof(aConnectionDescriptors));
            CopyMemory(&aConnectionDescriptors[0].FrameFormat, &InputFormat, sizeof(InputFormat)); 
            CopyMemory(&aConnectionDescriptors[1].FrameFormat, &InputFormat, sizeof(InputFormat)); 
            CopyMemory(&aConnectionDescriptors[2].FrameFormat, &OutputFormat, sizeof(OutputFormat)); 


			//
			// In the pull case we want to get this much data out 
			// in a pass (10 ms at the output rate) 
			// In the push case we will always push 10 ms at the inpu rate
			// trough the graph

			UINT32 u32Ms10=1;

			//
			// The max sampling ratio is 1:24 (8:192)
			if (SrcFlow == AUDIO_FLOW_PULL)
			{
				//
				// For the given amount of data that we expect as output we might
				// require in the case output = 8 and input = 192, 24 x amount of input frames
				u32Ms10 = (UINT32)(OutputFormat.f32FramesPerSecond/100.00 + 0.5);
	            aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10 * 24 + 4; 
				aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10 * 24 + 4; 
				aConnectionDescriptors[2].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10; 
			}
			else
			{
				//
				// For the given amount of data that we will push trough the graph
				// we might require in the case input = 8 and output = 192, 24 x amount of output frames 
				u32Ms10 = (UINT32)(InputFormat.f32FramesPerSecond/100 + 0.5);
	            aConnectionDescriptors[0].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10; 
				aConnectionDescriptors[1].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10; 
				aConnectionDescriptors[2].ConnectionDescriptor.u32MaxFrameCount =  u32Ms10*24 + 4; 
			}

			//aConnectionDescriptors[1].ConnectionDescriptor.u32ExtraFrameCount =  4; 
            //aConnectionDescriptors[2].ConnectionDescriptor.u32ExtraFrameCount =  0; 

            SLOG(eInfo1, "");
            SLOG(eInfo1, "");
            SLOG(eInfo1, "*********************************************************************************************");
            SLOG(eInfo1, "Testing conversion %.5lf --> %.5lf", aTestMatrix[u][v].finputSR, aTestMatrix[u][v].foutputSR);
            SLOG(eInfo1, "*********************************************************************************************");
            SLOG(eInfo1, "");
            SLOG(eInfo1, "");

            HRESULT hr;

			//
			// Create the graph
            CGraphRepresentation graph(aConnectionDescriptors,
                                    sizeof(aConnectionDescriptors)/sizeof(aConnectionDescriptors[0]), 
                                    aBindingDescriptors,
                                    sizeof(aBindingDescriptors)/sizeof(aBindingDescriptors[0]), 
                                    aApoDescriptors,
                                    sizeof(aApoDescriptors)/sizeof(aApoDescriptors[0]),                                     
                                    (UINT32)InputFormat.f32FramesPerSecond*3,  // process 2 seconds
                                    (UINT32)OutputFormat.f32FramesPerSecond*3, // process 2 seconds
                                    NULL,                                      // use output memory endpoint
                                    aTestMatrix[u][v].fFrequency
                                    );

			//
            // Change SRC type
            graph.SetSRCAlgorithm(SrcType);

			//
			// Change SRC flow 
            graph.SetSRCFlow(SrcFlow);

			//
			// Build graph
            if (FAILED(hr = graph.BuildGraph()))
            {
                SLOG(eError, "ERROR: Can't build graph");
                return FNS_FAIL;
            }

            MFTIME mfTime = 0;
            MFTIME mfPeriod = (MFTIMEFREQUENCY/1000)*10; // 10 ms per clock

			//
            // Start the endpoints
            graph.StartEndpoints();
            graph.StartEndpoints(NULL, true);

			CComPtr<IAudioMemoryEndpointEx> pMemoryInputEndpoint;
			CComPtr<IAudioMemoryEndpointEx> pMemoryOutputEndpoint;


			//
            // Get the endpoints OOB interfaces
            graph.GetEndpoint(true)->QueryInterface(__uuidof(IAudioMemoryEndpointEx), (LPVOID*)&pMemoryInputEndpoint);  
            graph.GetEndpoint(false)->QueryInterface(__uuidof(IAudioMemoryEndpointEx), (LPVOID*)&pMemoryOutputEndpoint);  


			//
			// Set the input endpoint to push mode and to process always 10 ms of input data
//			if (SrcFlow == AUDIO_FLOW_PUSH)
//			{
//				SLOG(eInfo1, "PUSH'ing %d frames trough the graph each time", u32Ms10);
//				pMemoryInputEndpoint->SetPushMode(u32Ms10/2);
//			}

			UINT32 u32InputPosition=0;
			UINT32 u32OutputPosition=0;

	  
            // See a detailed explanation bellow in the format converter case
			if (SrcFlow == AUDIO_FLOW_PUSH) {
				mfPeriod/=10;
			}

			//
            // Call process
            while (1)  
            {
				if (SrcFlow == AUDIO_FLOW_PUSH)
				{
					//
					// check that we have enough input space
					if (u32InputPosition + (UINT32)4*u32Ms10*nChannels*InputFormat.f32FramesPerSecond/1000 >=
						nChannels*InputFormat.f32FramesPerSecond*3*4)
						break;
				}
				else
				{
					//
					// check that we have enough output space
					if (u32OutputPosition + (UINT32)nChannels*4*u32Ms10*OutputFormat.f32FramesPerSecond/1000 >=
						nChannels*OutputFormat.f32FramesPerSecond*3*4)
						break;
				}

				//
				// Call process on the processor
                graph.Step(&mfTime, &mfPeriod); 
				u32InputPosition  = pMemoryInputEndpoint->GetPosition();
				u32OutputPosition = pMemoryOutputEndpoint->GetPosition();
//				SLOG(eInfo1, "Pos input = %ld, Pos output = %ld", u32InputPosition, u32OutputPosition);
            }


            u32InputPosition  = pMemoryInputEndpoint->GetPosition();
            u32OutputPosition = pMemoryOutputEndpoint->GetPosition();

            WAVEFORMATEX wfx;
            FORMAT2WAVEXIEEE(&OutputFormat, &wfx);

			//
            // validate samples in the output buffer
            UINT32 nAvailableFrames = u32OutputPosition/wfx.nBlockAlign;
            UINT32 nOffset = (UINT32)OutputFormat.f32FramesPerSecond/2;
            UINT32 nFrames = (UINT32)OutputFormat.f32FramesPerSecond;
            if (nAvailableFrames < 3*OutputFormat.f32FramesPerSecond/2)
            {
                SLOG(eError, "ERROR: Not enough output frames - expected ~2s");
                return FNS_FAIL;
            }

			//
            // design filter
            float fNormalFrequency = aTestMatrix[u][v].fFrequency*2/OutputFormat.f32FramesPerSecond; 
			// this means that the old frequency should be completely attenuated!
			// in oher words we should have only noise in the signal meaning that
			// we can pick any frequency and get ~0dB Snr
			// in