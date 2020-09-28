//////////////////////////////////////////////////////////////////////
// File:  BaseModeEnumerationTestGroup.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a DXGI mode enumeration test group base class.
//
// History:
// 08 Sep 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "DXGIModeEnum.h"
#include "TestApp.h"
#include "winuserp.h"

#pragma warning(disable: 4355)


/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

const DXGI_FORMAT CFormatsToTest::FormatList[ MAX_DXGI_FORMATS ] =
{
	DXGI_FORMAT_R10G10B10A2_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM,
	DXGI_FORMAT_R8G8B8A8_UNORM_SRGB,
	DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM,
	DXGI_FORMAT_R16G16B16A16_FLOAT,
	DXGI_FORMAT_B8G8R8A8_UNORM,       
	DXGI_FORMAT_B8G8R8A8_UNORM_SRGB,
};


// This is the list of spatial resolutions we'll test if "-priority:1" or "-WHQL" is specified.
// We also include the current desktop resolution.
const PRIORITYPATTERN CBaseModeEnumerationTestGroup::m_ModePriorityPatterns[] = 
{
	{1024, 768},
	{768, 1024},
};

bool CFormatsToTest::IsFormatInList
(
	DXGI_FORMAT Format
)
{
	for
	(
		UINT iCurrentFormat = 0;
		iCurrentFormat < ARRAYSIZE( FormatList );
		++iCurrentFormat
	)
	{
		if ( FormatList[ iCurrentFormat ] == Format )
		{
			return true;
		};
	};


	return false;
};


/////////////////////////////////////////////////////////////////
//
// Helper method implementation
//
/////////////////////////////////////////////////////////////////

int __cdecl CompareModes
(
	__in_bcount(sizeof(TEST_MODE)) const void * pv1,
	__in_bcount(sizeof(TEST_MODE)) const void * pv2
)
{
	return CompareModes
	(
		(TEST_MODE *) pv1,
		(TEST_MODE *) pv2
	);
};


int __cdecl CompareModes
(
	__in const TEST_MODE * pModeDesc1,
	__in const TEST_MODE * pModeDesc2
)
{
	if ( pModeDesc1->AdapterOrdinal < pModeDesc2->AdapterOrdinal )
	{
		return -1;
	}
	else if ( pModeDesc1->AdapterOrdinal > pModeDesc2->AdapterOrdinal )
	{
		return 1;
	};

	// AdapterOrdinal is EQUAL


	if ( pModeDesc1->OutputOrdinal < pModeDesc2->OutputOrdinal )
	{
		return -1;
	}
	else if ( pModeDesc1->OutputOrdinal > pModeDesc2->OutputOrdinal )
	{
		return 1;
	};

	// OutputOrdinal is EQUAL


	if ( pModeDesc1->Mode.Width < pModeDesc2->Mode.Width )
	{
		return -1;
	}
	else if ( pModeDesc1->Mode.Width > pModeDesc2->Mode.Width )
	{
		return 1;
	};

	// Width is EQUAL


	if ( pModeDesc1->Mode.Height < pModeDesc2->Mode.Height )
	{
		return -1;
	}
	else if ( pModeDesc1->Mode.Height > pModeDesc2->Mode.Height )
	{
		return 1;
	};

	// Height is EQUAL


	if ( pModeDesc1->Mode.RefreshRate.Numerator < pModeDesc2->Mode.RefreshRate.Numerator )
	{
		return -1;
	}
	else if ( pModeDesc1->Mode.RefreshRate.Numerator > pModeDesc2->Mode.RefreshRate.Numerator )
	{
		return 1;
	};

	// RefreshRate.Numerator is EQUAL


	if ( pModeDesc1->Mode.RefreshRate.Denominator < pModeDesc2->Mode.RefreshRate.Denominator )
	{
		return -1;
	}
	else if ( pModeDesc1->Mode.RefreshRate.Denominator > pModeDesc2->Mode.RefreshRate.Denominator )
	{
		return 1;
	};

	// RefreshRate.Denominator is EQUAL


	if ( pModeDesc1->Mode.Format < pModeDesc2->Mode.Format )
	{
		return -1;
	}
	else if ( pModeDesc1->Mode.Format > pModeDesc2->Mode.Format )
	{
		return 1;
	};

	// Format is EQUAL


	if ( pModeDesc1->Mode.ScanlineOrdering < pModeDesc2->Mode.ScanlineOrdering )
	{
		return -1;
	}
	else if ( pModeDesc1->Mode.ScanlineOrdering > pModeDesc2->Mode.ScanlineOrdering )
	{
		return 1;
	};

	// ScanlineOrdering is EQUAL


	if ( pModeDesc1->Mode.Scaling < pModeDesc2->Mode.Scaling )
	{
		return -1;
	}
	else if ( pModeDesc1->Mode.Scaling > pModeDesc2->Mode.Scaling )
	{
		return 1;
	};

	// Scaling is EQUAL


	if ( pModeDesc1->Rotation < pModeDesc2->Rotation )
	{
		return -1;
	}
	else if ( pModeDesc1->Rotation > pModeDesc2->Rotation )
	{
		return 1;
	};

	// Rotation is EQUAL


	return 0;
};


/////////////////////////////////////////////////////////////////
//
// CBaseModeEnumerationTestGroup implementation
//
/////////////////////////////////////////////////////////////////

CBaseModeEnumerationTestGroup::CBaseModeEnumerationTestGroup()
:
	m_bTestStereoModes(false)
{
};


CBaseModeEnumerationTestGroup::~CBaseModeEnumerationTestGroup()
{
};


LONG CBaseModeEnumerationTestGroup::ChangeDisplayMode
(
	__in DEVMODEW * pDevModeW,
	__in LPWSTR pswzDeviceName           
)
{
	LONG lResult = 0;

	assert (pDevModeW != NULL);

	UINT i = 0;
	DWORD dwFlags = CDS_RESET;

	// our goal is to rotate desktop here. Use any possible methods to achieve it.
	for (i = 0; i < 2; i++)
	{
		lResult = ChangeDisplaySettingsExW
		(
			pswzDeviceName,
			pDevModeW,
			NULL,
			dwFlags,
			NULL
		);

		// Windows Blue Bugs:  519607:  DXGIModeEnumeration:  Causes mode changes so quickly as to trigger repeated DWM failures, eventually triggering a logout
		// Sleep after the mode change to give DWM a chance to catch up.
		Sleep( 3 * 1000 );

		if (lResult != DISP_CHANGE_SUCCESSFUL)
		{
			pDevModeW->dmFields &= ~DM_DISPLAYFIXEDOUTPUT;
			pDevModeW->dmDisplayFixedOutput = 0;
			dwFlags |= CDS_TRYCLOSEST;

			continue;
		}

		break;
	}

	if (lResult != DISP_CHANGE_SUCCESSFUL)
	{
		switch ( lResult )
		{
			case DISP_CHANGE_BADDUALVIEW:
				WriteToLog
				(
					"CBaseModeEnumerationTestGroup::ChangeDisplayMode:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADDUALVIEW."
				);
			break;

			case DISP_CHANGE_BADFLAGS:
				WriteToLog
				(
					"CBaseModeEnumerationTestGroup::ChangeDisplayMode:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADFLAGS."
				);
			break;

			case DISP_CHANGE_BADMODE:
				WriteToLog
				(
					"CBaseModeEnumerationTestGroup::ChangeDisplayMode:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADMODE."
				);
			break;

			case DISP_CHANGE_BADPARAM:
				WriteToLog
				(
					"CBaseModeEnumerationTestGroup::ChangeDisplayMode:  ChangeDisplaySettingsW failed with DISP_CHANGE_BADPARAM."
				);
			break;

			case DISP_CHANGE_FAILED:
				WriteToLog
				(
					"CBaseModeEnumerationTestGroup::ChangeDisplayMode:  ChangeDisplaySettingsW failed with DISP_CHANGE_FAILED."
				);
			break;

			case DISP_CHANGE_NOTUPDATED:
				WriteToLog
				(
					"CBaseModeEnumerationTestGroup::ChangeDisplayMode:  ChangeDisplaySettingsW failed with DISP_CHANGE_NOTUPDATED."
				);
			break;

			case DISP_CHANGE_RESTART:
				WriteToLog
				(
					"CBaseModeEnumerationTestGroup::ChangeDisplayMode:  ChangeDisplaySettingsW failed with DISP_CHANGE_RESTART."
				);
			break;

			default:
				WriteToLog
				(
					"CBaseModeEnumerationTestGroup::ChangeDisplayMode:  ChangeDisplaySettingsW failed with unexpected error %d.",
				lResult
				);
			break;
		}

		WriteToLog
		(
			"Failed with pDevModeW->dmDisplayOrientation = %u"
			"[DMDO_DEFAULT(Identity) - %u, DMDO_90(Rotating 90 degree) - %u,"
			"DMDO_180(Rotating 180 degree) - %u, DMDO_270(Rotating 270 degree) - %u],"
			"dmPelsWidth = %u, dmPelsHeight = %u, dmDisplayFrequency = %u,"
			"%s",
			pDevModeW->dmDisplayOrientation,
			(UINT)DMDO_DEFAULT, 
			(UINT)DMDO_90,
			(UINT)DMDO_180,
			(UINT)DMDO_270,
			pDevModeW->dmPelsWidth, pDevModeW->dmPelsHeight, pDevModeW->dmDisplayFrequency,
			(pDevModeW->dmDisplayFlags & DM_INTERLACED)? "DM_INTERLACED" : "" 
		);
	}

	return lResult;
}


//****************************************************************************
// IsStereoCompatibleFormat
//
// Returns true if a pixel format is stereo compatible, otherwise false.
//
//****************************************************************************
bool CBaseModeEnumerationTestGroup::IsStereoCompatibleFormat(DXGI_FORMAT format)
{
	switch (format)
	{
		case DXGI_FORMAT_R8G8B8A8_UNORM:
		case DXGI_FORMAT_R16G16B16A16_FLOAT:
		case DXGI_FORMAT_B8G8R8A8_UNORM:
		case DXGI_FORMAT_B8G8R8X8_UNORM:
			return true;

		default:
			return false;
	}
}


//****************************************************************************
// IsModeADupeExceptForRefreshRate
//
// Compares a provided TEST_MODE against a list of TEST_MODEs.  Returns true
// if the provided mode matches an existing entry in the list differing only
// by refreshrate.
//****************************************************************************
bool CBaseModeEnumerationTestGroup::IsModeADupeExceptForRefreshRate
(
	std::vector <TEST_MODE> & ModeList,
	TEST_MODE ModeToCheck
)
{
	ModeToCheck.Mode.RefreshRate.Denominator = 0;
	ModeToCheck.Mode.RefreshRate.Numerator = 0;

	for(std::vector<TEST_MODE>::iterator i = ModeList.begin(); i != ModeList.end(); i++)
	{
		TEST_MODE Modified = *i;
		// Zero out the refresh rate fields so they will match
		Modified.Mode.RefreshRate.Denominator = 0;
		Modified.Mode.RefreshRate.Numerator = 0;

		if (CompareModes(&ModeToCheck, &Modified) == 0)
		{
			return true;
		}
	}

	return false;
}


HRESULT CBaseModeEnumerationTestGroup::EnumerateAdapterModes
(
	__in IDXGIAdapter * pDXGIAdapterEnumMode,
	__in UINT uiAdapterOrdinal
)
{
	//
	// Initialize locals.
	//

	HRESULT hresult = E_FAIL;
	IDXGIOutput * pDXGIOutputEnumMode = NULL;
	IDXGIOutput1 * pDXGIOutputEnumMode1 = NULL;
	DXGI_ADAPTER_DESC descAdapter;


	// Get a friendly name for the adapter for logging purposes.
	hresult = pDXGIAdapterEnumMode->GetDesc(&descAdapter);
	if (FAILED(hresult))
	{
		WriteToLog
		(
			"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  Adapter->GetDesc failed on adapter %u.  Error %s 0x%x.",
			uiAdapterOrdinal,
			D3DHResultToString(hresult).c_str(),
			hresult
		);

		goto Cleanup;

	}


	//
	// Enumerate outputs.
	//
	for
	(
		UINT uiOutputOrdinal = 0;
		uiOutputOrdinal < 10;
		++uiOutputOrdinal
	)
	{
		//
		// Release the output from the prior iteration.
		//

		SAFE_RELEASE(pDXGIOutputEnumMode);
		SAFE_RELEASE(pDXGIOutputEnumMode1);


		//
		// Enumerate the output at the current ordinal.
		//

		hresult = pDXGIAdapterEnumMode->EnumOutputs
		(
			uiOutputOrdinal,
			&pDXGIOutputEnumMode
		);

		if ( hresult == DXGI_ERROR_NOT_FOUND )
		{
			break;
		}
		else if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  EnumOutputs on output %u failed with error 0x%x.",
				uiOutputOrdinal,
				hresult
			);

			goto Cleanup;
		};


		if ( pDXGIOutputEnumMode == NULL )
		{
			WriteToLog
			(
				"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  EnumOutputs succeeded but returned NULL output pointer."
			);

			hresult = E_FAIL;
			goto Cleanup;
		};


		//
		// Get a descriptor for the output.
		//

		DXGI_OUTPUT_DESC outputDesc;
		hresult = pDXGIOutputEnumMode->GetDesc( &outputDesc );

		if ( FAILED( hresult ) )
		{
			WriteToLog
			(
				"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  GetDesc on output %u failed with error 0x%x.",
				uiOutputOrdinal,
				hresult
			);

			goto Cleanup;
		};

		WriteToLog(""); // Only way to ensure we get a blank line in the log file.
		WriteToLog("----------------------------------------------");
		WriteToLog(L"Adapter %d - %s", uiAdapterOrdinal, descAdapter.Description);
		WriteToLog(L"Output %d - %s", uiOutputOrdinal, outputDesc.DeviceName);
		WriteToLog("----------------------------------------------");

		//
		// Determine the current mode for the output.
		//

		DEVMODEW Mode;
		ZeroMemory( &Mode, sizeof( Mode ) );
		Mode.dmSize = sizeof( Mode );

		BOOL bGetCurrentModeResult = EnumDisplaySettingsW
		(
			outputDesc.DeviceName,
			ENUM_CURRENT_SETTINGS,
			&Mode
		);

		if ( bGetCurrentModeResult != TRUE )
		{
			DWORD GetCurrentModeError = GetLastError();

			WriteToLog
			(
				"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  EnumDisplaySettingsW failed with error %u.",
				GetCurrentModeError
			);

			if ( GetCurrentModeError != ERROR_SUCCESS )
			{
				hresult = HRESULT_FROM_WIN32( GetCurrentModeError );
			};

			goto Cleanup;
		};

		//
		// In case that other tests leave the desktop mode to rotated. 
		//
		if 
		(
			Mode.dmDisplayOrientation == DMDO_90 ||
			Mode.dmDisplayOrientation == DMDO_270
		)
		{
			UINT uiTemp = Mode.dmPelsWidth;
			Mode.dmPelsWidth = Mode.dmPelsHeight;
			Mode.dmPelsHeight = uiTemp;
		}


		//
		// Collect a list of modes for all orientations
		//
		for
		(
			UINT modeRotation = DXGI_MODE_ROTATION_IDENTITY;
			modeRotation <= DXGI_MODE_ROTATION_ROTATE270; 
			++modeRotation
		)
		{
			if
			(
				DXGI_MODE_ROTATION_IDENTITY != modeRotation &&
				FALSE // !m_bTestRotatedModes
			)
			{
				continue;
			};

			DEVMODEW rotatedMode;
			memcpy( &rotatedMode, &Mode, sizeof( DEVMODEW ) );
			rotatedMode.dmSize = sizeof( DEVMODEW );
			rotatedMode.dmDisplayFrequency = 0;
			rotatedMode.dmSpecVersion = DM_SPECVERSION;
			rotatedMode.dmFields |= DM_DISPLAYORIENTATION;
			rotatedMode.dmFields |= DM_PELSWIDTH;
			rotatedMode.dmFields |= DM_PELSHEIGHT;
			rotatedMode.dmFields |= DM_DISPLAYFLAGS;
			rotatedMode.dmFields |= DM_DISPLAYFREQUENCY;
			rotatedMode.dmFields |= DM_DISPLAYFIXEDOUTPUT;
			rotatedMode.dmDisplayFixedOutput = DMDFO_DEFAULT;

			switch ( modeRotation )
			{
				case DXGI_MODE_ROTATION_IDENTITY:
					rotatedMode.dmDisplayOrientation = DMDO_DEFAULT;
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateAdapterModes: DMDO_DEFAULT."
					);
				break;

				case DXGI_MODE_ROTATION_ROTATE90:
					std::swap( rotatedMode.dmPelsWidth, rotatedMode.dmPelsHeight );
					rotatedMode.dmDisplayOrientation = DMDO_90;
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateAdapterModes: DMDO_90."
					);
				break;

				case DXGI_MODE_ROTATION_ROTATE180:
					rotatedMode.dmDisplayOrientation = DMDO_180;
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateAdapterModes: DMDO_180."
					);
				break;

				case DXGI_MODE_ROTATION_ROTATE270:
					std::swap( rotatedMode.dmPelsWidth, rotatedMode.dmPelsHeight );
					rotatedMode.dmDisplayOrientation = DMDO_270;
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateAdapterModes: DMDO_270."
					);
				break;

				default:
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  modeRotation[%u] returned from EnumDisplaySettings() is unknown.",
						modeRotation
					);
				break;
			};

			LONG lResult = 0;
			lResult = ChangeDisplayMode
			(
				&rotatedMode, 
				outputDesc.DeviceName
			);

			if (lResult != DISP_CHANGE_SUCCESSFUL)
			{
				if (lResult == DISP_CHANGE_BADMODE)
				{
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateAdapterModes: The mode not supported. skip it."
					);

					continue;
				}
				else
				{
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateAdapterModes: ChangeDisplayMode failed."
					);

					goto Cleanup;
				}
			}

			//
			// Collect modes for all formats
			//
			for
			(
				UINT uiFormatIndex = 0;
				uiFormatIndex < ARRAYSIZE( CFormatsToTest::FormatList );
				++uiFormatIndex
			)
			{
				UINT uiModeCount = 0;
                
				//
				// If we're testing stereo modes, only certain pixel formats are compatible.
				//
				if (m_bTestStereoModes)
				{
					if (IsStereoCompatibleFormat(CFormatsToTest::FormatList[ uiFormatIndex ]) == false)
					{
						continue; //Skip this format, and go on to the next one.
					}
				}

				//
				// Determine the number of supported modes for
				// this format.
				//

				if( m_bTestStereoModes )
				{
					hresult = pDXGIOutputEnumMode->QueryInterface(__uuidof(IDXGIOutput1), (void**)&pDXGIOutputEnumMode1);
					if( FAILED(hresult) )
					{
						WriteToLog
						(
							"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  QueryInterface for IDXGIOutput1 failed with error 0x%x.",
							hresult
						);

						goto Cleanup;
					}

					hresult = pDXGIOutputEnumMode1->GetDisplayModeList1
					(
						CFormatsToTest::FormatList[ uiFormatIndex ],
						DXGI_ENUM_MODES_STEREO,
						&uiModeCount,
						NULL
					);
				}
				else
				{
					hresult = pDXGIOutputEnumMode->GetDisplayModeList
					(
						CFormatsToTest::FormatList[ uiFormatIndex ],
						DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
						&uiModeCount,
						NULL
					);
				}

				if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  GetDisplayModeList (1st call) failed with error 0x%x.",
						hresult
					);

					goto Cleanup;
				};


				if ( uiModeCount == 0 )
				{
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  GetDisplayModeList returned 0 modes for the format %u.",
						uiFormatIndex
					);

					continue;   // for automated test, continue is reasonable
				};


				DXGI_MODE_DESC * pModeDescArray = new DXGI_MODE_DESC[ uiModeCount ];
				DXGI_MODE_DESC1 * pModeDescArray1 = new DXGI_MODE_DESC1[ uiModeCount ];

				if ( pModeDescArray == NULL || pModeDescArray1 == NULL )
				{
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  Failed to allocate mode descriptor array."
					);

					hresult = E_OUTOFMEMORY;
					goto Cleanup;
				};

				ZeroMemory( pModeDescArray, uiModeCount * sizeof( DXGI_MODE_DESC ) );
				ZeroMemory( pModeDescArray1, uiModeCount * sizeof( DXGI_MODE_DESC1 ) );


				//
				// Get the array of supported modes for this format.
				//

				if( m_bTestStereoModes )
				{
					hresult = pDXGIOutputEnumMode1->GetDisplayModeList1
					(
						CFormatsToTest::FormatList[ uiFormatIndex ],
						DXGI_ENUM_MODES_STEREO,
						&uiModeCount,
						pModeDescArray1
					);

					// By design, enumeration will include all centered modes.  Since we're only interested in stereo
					// modes we will filter the non-stereo modes from our test matrix.
					UINT stereoModesFound = 0;
					for( UINT uiMode = 0; uiMode < uiModeCount; ++uiMode )
					{
						if( pModeDescArray1[uiMode].Stereo )
						{
							pModeDescArray[stereoModesFound] = clsDXGISwapChain::CopyToModeDesc(pModeDescArray1[uiMode]);
							stereoModesFound++;
						}
					}

					uiModeCount = stereoModesFound;

					if ( uiModeCount == 0 )
					{
						WriteToLog
						(
							"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  GetDisplayModeList returned 0 stereo modes for the format %u.",
							uiFormatIndex
						);

						continue;   // for automated test, continue is reasonable
					};
				}
				else
				{
					hresult = pDXGIOutputEnumMode->GetDisplayModeList
					(
						CFormatsToTest::FormatList[ uiFormatIndex ],
						DXGI_ENUM_MODES_INTERLACED | DXGI_ENUM_MODES_SCALING,
						&uiModeCount,
						pModeDescArray
					);
				}

				if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  GetDisplayModeList failed with error 0x%x.",
						hresult
					);

					delete [] pModeDescArray;
					pModeDescArray = NULL;

					goto Cleanup;
				};


				//
				// Add this mode to the list of modes to test.
				//

				for
				(
					UINT iCurrentMode = 0;
					iCurrentMode < uiModeCount;
					++iCurrentMode
				)
				{
					TEST_MODE ModeInit;

					// Check if the color format is same as the requested
					if ( pModeDescArray[ iCurrentMode ].Format != CFormatsToTest::FormatList[ uiFormatIndex ] )
					{
						WriteToLog
						(
							"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  Color format returned[%u] mismatches the requested[%u]!",
							pModeDescArray[ iCurrentMode ].Format,
							CFormatsToTest::FormatList[ uiFormatIndex ]
						);

						hresult = E_INVALIDARG;
						goto Cleanup;
					}

					ModeInit.AdapterOrdinal = uiAdapterOrdinal;
					ModeInit.OutputOrdinal = uiOutputOrdinal;
					ModeInit.OutputDescriptor = outputDesc;
					memcpy( &ModeInit.Mode, &pModeDescArray[ iCurrentMode ], sizeof( ModeInit.Mode ) );
					ModeInit.Rotation = (DXGI_MODE_ROTATION) modeRotation;

					//
					// Display Mode changes are relatively slow.  Testing all enumerated modes takes significant time.
					// To speed things up we'll only test a subset in WHQL or when "-priority: 1" is specified.
					// the list of modes we'll test is housed in m_ModePriorityPatterns.   We'll also include the current
					// desktop mode to improve odds that at least one of these spatial resolutions is supported.
					// 
					if ( g_Application.GetExecutionPriorityLevel() == 1 
						||   GetFramework()->GetTestMode() == WGFTEST_MODE_WHQL )
					{
						// Does the enumerated mode match the spatial resolution of the current desktop mode (or it's rotated representation)?
						if ((pModeDescArray[ iCurrentMode ].Width == Mode.dmPelsWidth && pModeDescArray[ iCurrentMode ].Height == Mode.dmPelsHeight) ||
							(pModeDescArray[ iCurrentMode ].Width == Mode.dmPelsHeight && pModeDescArray[ iCurrentMode ].Height == Mode.dmPelsWidth))
						{
							// This is the desktop mode.  Do nothing so it'll get added to the list we test.
						}
						else
						{
							UINT i = 0;
							for 
							(
								i = 0;
								i < ARRAYSIZE(m_ModePriorityPatterns);
								i++
							)
							{
								if
								(
									pModeDescArray[ iCurrentMode ].Width == m_ModePriorityPatterns[i].uiModeWidth &&
									pModeDescArray[ iCurrentMode ].Height == m_ModePriorityPatterns[i].uiModeHeight
								)
								{
									break;
								}
							}

							if ( i == ARRAYSIZE(m_ModePriorityPatterns) )
							{
								continue; // Continue on to next mode.  Discard this one.
							}
						}

						// Skip this mode if it only differs by refreshrate.
						if (IsModeADupeExceptForRefreshRate(TestedModes, ModeInit))
						{
							continue; // Continue on to next mode.  Discard this one.
						}
					}

					assert(pModeDescArray[ iCurrentMode ].RefreshRate.Denominator != 0); // We're about to divide by this value.  It better not be zero.

					WriteToLog
					(
						"    [Adapter %u Output %u]Mode %u: %ux%ux%uHz, %s, %s",
						uiAdapterOrdinal,
						uiOutputOrdinal,
						iCurrentMode,
						pModeDescArray[ iCurrentMode ].Width,
						pModeDescArray[ iCurrentMode ].Height,
						pModeDescArray[ iCurrentMode ].RefreshRate.Numerator/pModeDescArray[ iCurrentMode ].RefreshRate.Denominator,
						ToString( pModeDescArray[ iCurrentMode ].ScanlineOrdering ).c_str(),
						ToString( pModeDescArray[ iCurrentMode ].Format ).c_str()                        
					);

					TestedModes.push_back( ModeInit );
				};


				//
				// Free the mode list.
				//

				delete [] pModeDescArray;
				pModeDescArray = NULL;

				delete [] pModeDescArray1;
				pModeDescArray1 = NULL;
			};
		};


		//
		// Release the output.
		//

		SAFE_RELEASE(pDXGIOutputEnumMode);
		SAFE_RELEASE(pDXGIOutputEnumMode1);


		//
		// Restore the output to the original mode.
		//

		LONG lResult = ChangeDisplaySettingsExW
		(
			outputDesc.DeviceName,
			&Mode,
			NULL,
			0,
			NULL
		);

		// Windows Blue Bugs:  519607:  DXGIModeEnumeration:  Causes mode changes so quickly as to trigger repeated DWM failures, eventually triggering a logout
		// Sleep after the mode change to give DWM a chance to catch up.
		Sleep( 3 * 1000 );

		if ( lResult != DISP_CHANGE_SUCCESSFUL )
		{
			WriteToLog
			(
				"CBaseModeEnumerationTestGroup::EnumerateAdapterModes:  ChangeDisplaySettingsExW failed with error %d while restoring the mode.",
				lResult
			);
		};
	};


	//
	// If we made it this far, then we're good to go.
	//

	hresult = S_OK;


Cleanup:
	SAFE_RELEASE(pDXGIOutputEnumMode);
	SAFE_RELEASE(pDXGIOutputEnumMode1);

	return hresult;
};


HRESULT CBaseModeEnumerationTestGroup::EnumerateModes()
{
	//
	// Initialize locals.
	//

	HRESULT hresult = E_FAIL;
	DXGI_FACTORY_POINTER  FactoryPointer = {NULL};
	IDXGIAdapter * pDXGIAdapterEnumMode = NULL;
	HMODULE hRasterizer = NULL;


	//
	// Clear the list of modes to test.
	//

	TestedModes.clear();


	//
	// Create a DXGI factory.
	//

	hresult = g_Application.CreateDXGIFactory
	(
		&FactoryPointer
	);

	if ( FAILED( hresult ) )
	{
		WriteToLog
		(
			"CBaseModeEnumerationTestGroup::EnumerateModes:  CreateDXGIFactory failed with error 0x%x.",
			hresult
		);

		goto Cleanup;
	};

	if ( FactoryPointer.pFactory == NULL )
	{
		WriteToLog
		(
			"CBaseModeEnumerationTestGroup::EnumerateModes:  CreateFactory succeeded but returned a NULL factory pointer."
		);

		hresult = E_FAIL;
		goto Cleanup;
	};


	switch ( g_Application.m_D3DOptions.SrcDriverType )
	{
	case D3D10_DRIVER_TYPE_SOFTWARE:
		case D3D10_DRIVER_TYPE_REFERENCE:
		case D3D10_DRIVER_TYPE_NULL:
			if ( g_Application.m_D3DOptions.SrcDriverType == D3D10_DRIVER_TYPE_REFERENCE )
			{
				hRasterizer = LoadLibraryExW
				(
					L"d3d10ref.dll",
					NULL,
					0
				);

				if ( hRasterizer == NULL )
				{
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateModes:  Failed to load reference rasterizer."
					);

					goto Cleanup;
				};
			}
			else if ( g_Application.m_D3DOptions.SrcDriverType == D3D10_DRIVER_TYPE_SOFTWARE )
			{
				hRasterizer = LoadLibraryExW
				(
					//g_Application.m_D3DOptions.RasterizerDLL
					L"d3d10Warp.dll",
					NULL,
					0
				);

				if ( hRasterizer == NULL )
				{
					WriteToLog
						(
						"CBaseModeEnumerationTestGroup::EnumerateModes:  Failed to load software rasterizer."
						);

					goto Cleanup;
				};
			};


			//
			// Create a software adapter.
			//

			hresult = FactoryPointer.pFactory->CreateSoftwareAdapter
			(
				hRasterizer,
				&pDXGIAdapterEnumMode
			);


			if ( hRasterizer != NULL )
			{
				FreeLibrary( hRasterizer );
				hRasterizer = NULL;
			};

			if ( FAILED( hresult ) )
			{
				WriteToLog
				(
					"CBaseModeEnumerationTestGroup::EnumerateModes:  CreateSoftwareAdapter failed with error 0x%x.",
					hresult
				);

				goto Cleanup;
			};

			if ( pDXGIAdapterEnumMode == NULL )
			{
				WriteToLog
				(
					"CBaseModeEnumerationTestGroup::EnumerateModes:  CreateSoftwareAdapter succeeded but returned a NULL software adapter pointer."
				);

				goto Cleanup;
			};


			//
			// Enumerate outputs and modes for this adapter.
			//

			hresult = EnumerateAdapterModes
			(
				pDXGIAdapterEnumMode,
				0
			);

			if ( FAILED( hresult ) )
			{
				WriteToLog
				(
					"CBaseModeEnumerationTestGroup::EnumerateModes:  EnumerateAdapterModes failed with error 0x%x.",
					hresult
				);

				goto Cleanup;
			};
		break;


		case D3D10_DRIVER_TYPE_HARDWARE:
			//
			// Enumerate outputs and modes for all adapters.
			//

			for
			(
				UINT uiAdapterOrdinal = 0;
				uiAdapterOrdinal < 10;
				++uiAdapterOrdinal
			)
			{
				//
				// Release the adapter from the prior iteration.
				//

				if ( pDXGIAdapterEnumMode != NULL )
				{
					pDXGIAdapterEnumMode->Release();
					pDXGIAdapterEnumMode = NULL;
				};


				//
				// Enumerate the adapter at the specified ordinal.
				//

				hresult = FactoryPointer.pFactory->EnumAdapters
				(
					uiAdapterOrdinal,
					&pDXGIAdapterEnumMode
				);

				if ( hresult == DXGI_ERROR_NOT_FOUND )
				{
					if (uiAdapterOrdinal == 0)  // no any adapter found
					{
						WriteToLog
						(
							"CBaseModeEnumerationTestGroup::EnumerateModes:  No video adapter found."
						);

						goto Cleanup;
					}

					break;
				}
				else if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateModes:  EnumAdapters on adapter %u failed with error 0x%x.",
						uiAdapterOrdinal,
						hresult
					);

					goto Cleanup;
				};

				if ( pDXGIAdapterEnumMode == NULL )
				{
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateModes:  EnumAdapters on adapter %u succeeded but returned a NULL adapter pointer.",
						uiAdapterOrdinal
					);

					hresult = E_FAIL;
					goto Cleanup;
				};


				//
				// Enumerate outputs and modes for this adapter.
				//

				hresult = EnumerateAdapterModes
				(
					pDXGIAdapterEnumMode,
					uiAdapterOrdinal
				);

				if ( FAILED( hresult ) )
				{
					WriteToLog
					(
						"CBaseModeEnumerationTestGroup::EnumerateModes:  EnumerateAdapterModes failed with error 0x%x.",
						hresult
					);

					goto Cleanup;
				};


				//
				// Release the adapter.
				//

				if ( pDXGIAdapterEnumMode != NULL )
				{
					pDXGIAdapterEnumMode->Release();
					pDXGIAdapterEnumMode = NULL;
				};
			};


			//
			// Release the factory.
			//

			FactoryPointer.pFactory->Release();
			FactoryPointer.pFactory = NULL;
			break;

		default:
			WriteToLog
			(
				"CBaseModeEnumerationTestGroup::EnumerateModes: unknown g_Application.m_D3DOptions.SrcDriverType[%u]",
				g_Application.m_D3DOptions.SrcDriverType
			);
		break;
	};


	hresult = S_OK;


Cleanup:
	if ( pDXGIAdapterEnumMode != NULL )
	{
		pDXGIAdapterEnumMode->Release();
		pDXGIAdapterEnumMode = NULL;
	};


	if ( FactoryPointer.pFactory != NULL )
	{
		FactoryPointer.pFactory->Release();
		FactoryPointer.pFactory = NULL;
	};


	return hresult;
};


TEST_RESULT CBaseModeEnumerationTestGroup::CheckForDuplicateModes()
{
	//
	// Initialize locals.
	//

	TEST_RESULT test_result = RESULT_FAIL;
	TEST_MODE * pBuffer = NULL;

	WriteToLog
	(
		"CBaseModeEnumerationTestGroup::CheckForDuplicateModes: Checking for duplicate modes in the mode list."
	);


	//
	// Verify that the mode list is not empty.
	//

	if ( TestedModes.size() == 0 )
	{
		WriteToLog
		(
			"CBaseModeEnumerationTestGroup::CheckForDuplicateModes: Mode list is empty, so there are no duplicates."
		);

		test_result = RESULT_SKIP;
		goto Cleanup;
	};


	//
	// Allocate a buffer to hold the mode list.
	//

	pBuffer = new TEST_MODE[ TestedModes.size() ];
	if ( pBuffer == NULL )
	{
		WriteToLog
		(
			"CBaseModeEnumerationTestGroup::CheckForDuplicateModes:  Failed to allocate mode descriptor array."
		);

		goto Cleanup;
	};

	ZeroMemory
	(
		pBuffer,
		TestedModes.size() * sizeof( pBuffer[ 0 ] )
	);


	for
	(
		UINT iCurrentMode = 0;
		iCurrentMode < TestedModes.size();
		++iCurrentMode
	)
	{
		pBuffer[ iCurrentMode ] = TestedModes[ iCurrentMode ];
	};


	//
	// Sort the mode list so we can check for duplicates by comparing adjacent modes.
	//

	qsort
	(
		pBuffer,
		TestedModes.size(),
		sizeof( TEST_MODE ),
		CompareModes
	);


	//
	// Compare adjacent modes.
	//

	for
	(
		UINT i = 0,
			j = 1;
		j < TestedModes.size();
		++i,
			++j
	)
	{
		if
		(
			// For stereo, this check still works since the modes that made it in to the initial
			// list HAD to support stereo.  Therefore it doesnt matter if the TEST_MODE struct and
			// the CompareModes function use DXGI_MODE_DESC and not DXGI_MODE_DESC1.
			CompareModes
			(
				&pBuffer[ i ],
				&pBuffer[ j ]
			) == 0
		)
		{
			float uiRefreshRate =
				(float) pBuffer[ j ].Mode.RefreshRate.Numerator /
				(float) pBuffer[ j ].Mode.RefreshRate.Denominator;

			const DXGIFormatInfo * pEnumeratedModeFormatRecord = CFormatInfoMap::Find
			(
				pBuffer[ j ].Mode.Format
			);


			WriteToLog
			(
				_T( "CBaseModeEnumerationTestGroup::CheckForDuplicateModes: Duplicate mode detected:  %u x %u @ %f Hz @ %d degrees, %s, %s, %s, Stereo: %s." ),
				pBuffer[ j ].Mode.Width,
				pBuffer[ j ].Mode.Height,
				uiRefreshRate,
				( pBuffer[ j ].Rotation - DXGI_MODE_ROTATION_IDENTITY ) * 90,
				pEnumeratedModeFormatRecord ? pEnumeratedModeFormatRecord->szDesc : _T( "Unrecognized format" ),
				ToString( pBuffer[ j ].Mode.ScanlineOrdering ).c_str(),
				ToString( pBuffer[ j ].Mode.Scaling ).c_str(),
				m_bTestStereoModes ? _T( "On" ) : _T( "Off" )
			);

			goto Cleanup;
		};
	};


	test_result = RESULT_PASS;


Cleanup:
	if ( pBuffer != NULL )
	{
		delete [] pBuffer;
		pBuffer = NULL;
	};


	return test_result;
};
