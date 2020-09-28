//////////////////////////////////////////////////////////////////////
// File:  DXGITest.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a DXGI test base class.
//
// History:
// 11 Sep 2007  SEdmison  Created.
//////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////
//
// Includes
//
/////////////////////////////////////////////////////////////////

//
// Project headers
//

#include "DXGITest.h"


/////////////////////////////////////////////////////////////////
//
// Implementation
//
/////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
//
// Construction/destruction
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  CDXGITest
//
// Purpose:
// Constructor.
///////////////////////////////////////////////////////
CDXGITest::CDXGITest()
:
	m_TestFeatureLevel( TEST_FEATURE_10_0_LEVEL_10_0 )
{
};


///////////////////////////////////////////////////////
// Function:  CDXGITest
//
// Purpose:
// Destructor.
///////////////////////////////////////////////////////
CDXGITest::~CDXGITest()
{
};


////////////////////////////////////////////////////////////
//
// Framework override methods
//
////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////
// Function:  InitTestParameters
//
// Purpose:
// Builds the test matrix for the group.  This is done
// by registering parameters with the test framework,
// which takes care of iterating over them and passing
// them into our designated member variables.
///////////////////////////////////////////////////////
void CDXGITest::InitTestParameters()
{
	//
	// Get a pointer to the framework.
	//

	CDXGITestFramework * pFramework = dynamic_cast< CDXGITestFramework * >( GetFramework() );
	assert ( pFramework != NULL );


	//
	// Add the feature level parameter.
	//

	if ( pFramework->m_bRegisterFeatureLevel == true )
	{
		CTestCaseParameter< TEST_FEATURE_LEVEL > * pFeatureLevelParameter = AddParameter
		(
			_T("FeatureLevel"),
			&m_TestFeatureLevel
		);


		//
		// Fill in the default 11.0 feature level if necessary.
		//

		if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] == true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_UNSPECIFIED ] = false;

			bool bFoundAtLeastOneRequested11_0FeatureLevel = false;
			for
			(
				TEST_FEATURE_LEVEL iCurrentLevel = TEST_FEATURE_11_0_LEVEL_9_1;
				iCurrentLevel <= TEST_FEATURE_11_ALL_LEVEL_BESTAVAILABLE;
				iCurrentLevel = (TEST_FEATURE_LEVEL) ( (int) iCurrentLevel + 1 )
			)
			{
				if ( pFramework->m_RequestedFeatureLevels[ iCurrentLevel ] == true )
				{
					bFoundAtLeastOneRequested11_0FeatureLevel = true;
					break;
				};
			};

			if ( bFoundAtLeastOneRequested11_0FeatureLevel == false )
			{
				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_BESTAVAILABLE ] = true;
			};
		};


		//
		// Resolve feature 11.0 level 11_BESTAVAILABLE.
		//

		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_BESTAVAILABLE ] == true ||
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] == true
		)
		{
			ID3D10Device * pDevice = NULL;

			HRESULT hCreateDeviceResult = pFramework->CreateDevice
			(
				NULL,
				NULL,
				0,
				TEST_FEATURE_11_0_LEVEL_11_0,
				0,
				&pDevice,
				true
			);

			if ( SUCCEEDED( hCreateDeviceResult ) )
			{
				pDevice->Release();
				pDevice = NULL;

				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_0 ] = true;

				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = false;
			}
			else if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_BESTAVAILABLE ] == true )
			{
				WriteToLog
				(
					_T( "CDXGITest::InitTestParameters:  TEST_FEATURE_11_0_LEVEL_11_BESTAVAILABLE specified, but no 11 feature level is available." )
				);

				// @TODO [SEdmison]:
				// What should we do in this case?
			};

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_BESTAVAILABLE ] = false;
		};


		//
		// Resolve feature 11.0 level 10_BESTAVAILABLE.
		//

		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE ] == true ||
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] == true
		)
		{
			ID3D10Device * pDevice = NULL;

			HRESULT hCreateDeviceResult = pFramework->CreateDevice
			(
				NULL,
				NULL,
				0,
				TEST_FEATURE_11_0_LEVEL_10_1,
				0,
				&pDevice,
				true
			);

			if ( SUCCEEDED( hCreateDeviceResult ) )
			{
				pDevice->Release();
				pDevice = NULL;

				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_1 ] = true;

				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = false;
			}
			else
			{
				HRESULT hCreateDeviceResult = pFramework->CreateDevice
				(
					NULL,
					NULL,
					0,
					TEST_FEATURE_11_0_LEVEL_10_0,
					0,
					&pDevice,
					true
				);

				if ( SUCCEEDED( hCreateDeviceResult ) )
				{
					pDevice->Release();
					pDevice = NULL;

					pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_0 ] = true;

					pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = false;
				}
				else if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE ] == true )
				{
					WriteToLog
					(
						_T( "CDXGITest::InitTestParameters:  TEST_FEATURE_11_0_LEVEL_10_BESTAVAILABLE specified, but no 10 feature level is available." )
					);

					// @TODO [SEdmison]:
					// What should we do in this case?
				};
			};

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_BESTAVAILABLE ] = false;
		};


		//
		// Resolve feature 11.0 level 9_BESTAVAILABLE.
		//

		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE ] == true ||
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] == true
		)
		{
			ID3D10Device * pDevice = NULL;

			HRESULT hCreateDeviceResult = pFramework->CreateDevice
			(
				NULL,
				NULL,
				0,
				TEST_FEATURE_11_0_LEVEL_9_3,
				0,
				&pDevice,
				true
			);

			if ( SUCCEEDED( hCreateDeviceResult ) )
			{
				pDevice->Release();
				pDevice = NULL;

				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_3 ] = true;

				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = false;
			}
			else
			{
				HRESULT hCreateDeviceResult = pFramework->CreateDevice
				(
					NULL,
					NULL,
					0,
					TEST_FEATURE_11_0_LEVEL_9_2,
					0,
					&pDevice,
					true
				);

				if ( SUCCEEDED( hCreateDeviceResult ) )
				{
					pDevice->Release();
					pDevice = NULL;

					pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_2 ] = true;

					pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = false;
				}
				else
				{
					HRESULT hCreateDeviceResult = pFramework->CreateDevice
					(
						NULL,
						NULL,
						0,
						TEST_FEATURE_11_0_LEVEL_9_1,
						0,
						&pDevice,
						true
					);

					if ( SUCCEEDED( hCreateDeviceResult ) )
					{
						pDevice->Release();
						pDevice = NULL;

						pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_1 ] = true;

						pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = false;
					}
					else if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE ] == true )
					{
						WriteToLog
						(
							_T( "CDXGITest::InitTestParameters:  TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE specified, but no 9 feature level is available." )
						);

						// @TODO [SEdmison]:
						// What should we do in this case?
					};
				};
			};

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_BESTAVAILABLE ] = false;
		};


		//
		// Fill in the default 10.1 feature level if necessary.
		//

		if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] == true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_UNSPECIFIED ] = false;

			bool bFoundAtLeastOneRequested10_1FeatureLevel = false;
			for
			(
				TEST_FEATURE_LEVEL iCurrentLevel = TEST_FEATURE_10_1_LEVEL_10_0;
				iCurrentLevel <= TEST_FEATURE_10_ALL_LEVEL_BESTAVAILABLE;
				iCurrentLevel = (TEST_FEATURE_LEVEL) ( (int) iCurrentLevel + 1 )
			)
			{
				if ( pFramework->m_RequestedFeatureLevels[ iCurrentLevel ] == true )
				{
					bFoundAtLeastOneRequested10_1FeatureLevel = true;
					break;
				};
			};

			if ( bFoundAtLeastOneRequested10_1FeatureLevel == false )
			{
				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE ] = true;
			};
		};


		//
		// Resolve feature 10.1 level 10_BESTAVAILABLE.
		//

		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE ] == true ||
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] == true
		)
		{
			ID3D10Device * pDevice = NULL;
			HRESULT hCreateDeviceResult = pFramework->CreateDevice
			(
				NULL,
				NULL,
				0,
				TEST_FEATURE_10_1_LEVEL_10_1,
				0,
				&pDevice,
				true
			);

			if ( SUCCEEDED( hCreateDeviceResult ) )
			{
				pDevice->Release();
				pDevice = NULL;

				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_1 ] = true;

				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = false;
			}
			else
			{
				HRESULT hCreateDeviceResult = pFramework->CreateDevice
				(
					NULL,
					NULL,
					0,
					TEST_FEATURE_10_1_LEVEL_10_0,
					0,
					&pDevice,
					true
				);

				if ( SUCCEEDED( hCreateDeviceResult ) )
				{
					pDevice->Release();
					pDevice = NULL;

					pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_0 ] = true;

					pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = false;
				}
				else if ( pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE ] == true )
				{
					WriteToLog
					(
						_T( "CDXGITest::InitTestParameters:  TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE specified, but no 10 feature level is available." )
					);

					// @TODO [SEdmison]:
					// What should we do in this case?
				};
			};

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_BESTAVAILABLE ] = false;
		};


		//
		// Resolve feature 10.1 level 9_BESTAVAILABLE.
		//

		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE ] == true ||
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] == true
		)
		{
			ID3D10Device * pDevice = NULL;
			HRESULT hCreateDeviceResult = pFramework->CreateDevice
			(
				NULL,
				NULL,
				0,
				TEST_FEATURE_10_1_LEVEL_9_3,
				0,
				&pDevice,
				true
			);

			if ( SUCCEEDED( hCreateDeviceResult ) )
			{
				pDevice->Release();
				pDevice = NULL;

				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_3 ] = true;
				pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = false;
			}
			else
			{
				HRESULT hCreateDeviceResult = pFramework->CreateDevice
				(
					NULL,
					NULL,
					0,
					TEST_FEATURE_10_1_LEVEL_9_2,
					0,
					&pDevice,
					true
				);

				if ( SUCCEEDED( hCreateDeviceResult ) )
				{
					pDevice->Release();
					pDevice = NULL;

					pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_2 ] = true;
					pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = false;
				}
				else
				{
					HRESULT hCreateDeviceResult = pFramework->CreateDevice
					(
						NULL,
						NULL,
						0,
						TEST_FEATURE_10_1_LEVEL_9_1,
						0,
						&pDevice,
						true
					);

					if ( SUCCEEDED( hCreateDeviceResult ) )
					{
						pDevice->Release();
						pDevice = NULL;

						pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_1 ] = true;

						pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_ALL_LEVEL_BESTAVAILABLE ] = false;
					}
					else
					{
						WriteToLog
						(
							_T( "CDXGITest::InitTestParameters:  TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE specified, but no 9 feature level is available." )
						);

						// @TODO [SEdmison]:
						// What should we do in this case?
					};
				};
			};

			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_BESTAVAILABLE ] = false;
		};

		//
		// Build the set of feature levels over which to interate
		// based on the entries in our array of selected feature levels.
		//

		CUserValueSet< TEST_FEATURE_LEVEL > * pFeatureLevelValueSet = new CUserValueSet< TEST_FEATURE_LEVEL >();

		bool bAddedAtLeastOneFeatureLevel = false;


		// 11.0 level 11_0
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_11_0 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_11_0_LEVEL_11_0 );

			bAddedAtLeastOneFeatureLevel = true;
		};

		// 11.0 level 10_1
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_1 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_11_0_LEVEL_10_1 );

			bAddedAtLeastOneFeatureLevel = true;
		};

		// 11.0 level 10_0
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_10_0 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_11_0_LEVEL_10_0 );

			bAddedAtLeastOneFeatureLevel = true;
		};

		// 11.1 level 9_3
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_3 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_11_0_LEVEL_9_3 );

			bAddedAtLeastOneFeatureLevel = true;
		};

		// 11.0 level 9_2
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_2 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_11_0_LEVEL_9_2 );

			bAddedAtLeastOneFeatureLevel = true;
		};

		// 11.0 level 9_1
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_11_0_LEVEL_9_1 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_11_0_LEVEL_9_1 );

			bAddedAtLeastOneFeatureLevel = true;
		};


		// 10.1 level 10_1
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_1 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_10_1_LEVEL_10_1 );

			bAddedAtLeastOneFeatureLevel = true;
		};

		// 10.1 level 10_0
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_10_0 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_10_1_LEVEL_10_0 );

			bAddedAtLeastOneFeatureLevel = true;
		};

		// 10.1 level 9_3
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_3 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_10_1_LEVEL_9_3 );

			bAddedAtLeastOneFeatureLevel = true;
		};

		// 10.1 level 9_2
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_2 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_10_1_LEVEL_9_2 );

			bAddedAtLeastOneFeatureLevel = true;
		};

		// 10.1 level 9_1
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_1_LEVEL_9_1 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_10_1_LEVEL_9_1 );

			bAddedAtLeastOneFeatureLevel = true;
		};

		// 10.0 level 10_0
		if
		(
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_0_LEVEL_10_0 ] == true
		)
		{
			pFeatureLevelValueSet->AddValue( TEST_FEATURE_10_0_LEVEL_10_0 );

			bAddedAtLeastOneFeatureLevel = true;
		};


		//
		// Add a default feature level if necessary.
		//

		if ( bAddedAtLeastOneFeatureLevel != true )
		{
			pFramework->m_RequestedFeatureLevels[ TEST_FEATURE_10_0_LEVEL_10_0 ] = true;

			pFeatureLevelValueSet->AddValue( TEST_FEATURE_10_0_LEVEL_10_0 );
		};

		//
		// Associate the values set with the parameter.
		//

		m_FeatureLevelFactor = AddParameterValueSet
		(
			pFeatureLevelParameter,
			pFeatureLevelValueSet
		);
    };
};
