//////////////////////////////////////////////////////////////////////
// File:  ValidateModeListTestGroup.cpp
//
// Copyright (c) Microsoft Corporation.  All rights reserved.
//
// Purpose:
// Implementation of a DXGI mode enumeration test that enumerates
// modes and then validates the mode list.
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


#pragma warning(disable: 4355)


/////////////////////////////////////////////////////////////////
//
// CValidateModeListTestGroup implementation
//
/////////////////////////////////////////////////////////////////

CValidateModeListTestGroup::CValidateModeListTestGroup()
:
	m_iCurrentScenario( 0 )
{
};


CValidateModeListTestGroup::~CValidateModeListTestGroup()
{
};


void CValidateModeListTestGroup::InitTestParameters()
{
	DebugOut.Write("Entering InitTestParameters.\n");


	//
	// Call the base class implementation
	//

	CDXGITest::InitTestParameters();


	tstring strPath = GetPath().ToString();


	//
	// Iterate over an array of scenarios.
	//

	AddParameter< UINT >
	(
		_T("Scenario"),
		&m_iCurrentScenario,
		RangeValueSet< UINT >
		(
			0,
			0,
			1
		)
	);


	DebugOut.Write("Exiting InitTestParameters.\n");
};


TEST_RESULT CValidateModeListTestGroup::ExecuteTestCase()
{
	//
	// Initialize locals.
	//

	TEST_RESULT OverallResult = RESULT_FAIL;   
	TEST_RESULT CheckForDuplicatesResult = RESULT_FAIL;


	//
	// Enumerate modes.
	//

	EnumerateModes();

	if ( TestedModes.size() == 0 )
	{
		WriteToLog
		(
            "CValidateModeListTestGroup::ExecuteTestCase: No %smodes detected."
            "\nIf there is no any video adapter in the test machine, it is not an error;"
            "\nOtherwise the reason shall be checked.",
			m_bTestStereoModes ? _T("STEREO ") : _T("")
		);

		// No stereo modes is valid since it's an optional feature, just skip
		if( m_bTestStereoModes )
		{
			OverallResult = RESULT_SKIP;
			CheckForDuplicatesResult = RESULT_SKIP;
		}

		goto Cleanup;
	};


	//
	// Check the mode list for duplicate modes.
	//

	CheckForDuplicatesResult = CheckForDuplicateModes();

	if ( CheckForDuplicatesResult != RESULT_PASS )
	{
		if ( CheckForDuplicatesResult != RESULT_SKIP )
		{
			WriteToLog
			(
                "CValidateModeListTestGroup::ExecuteTestCase: CheckForDuplicateModes failed."
			);
		};

		OverallResult = CheckForDuplicatesResult;
		goto Cleanup;
	};


	//
	// Scan the mode list for modes in required formats.
	//

	for
	(
		UINT iCurrentFormat = 0;
		iCurrentFormat < ARRAYSIZE( CFormatsToTest::FormatList );
		++iCurrentFormat
	)
	{
		if
		(
			(
				m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_1 ||
				m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_2 ||
				m_TestFeatureLevel == TEST_FEATURE_10_1_LEVEL_9_3
			) &&
			CFormatsToTest::FormatList[ iCurrentFormat ] != DXGI_FORMAT_R8G8B8A8_UNORM
		)
		{
			continue;
		};

        //
        // If we're testing stereo modes, only certain pixel formats are compatible.
        //
        if (m_bTestStereoModes)
        {
            if (IsStereoCompatibleFormat(CFormatsToTest::FormatList[ iCurrentFormat ]) == false)
                continue; //Skip this format, and go on to the next one.
        }


		bool bFoundAtLeastOneModeForThisFormat = false;
		for
		(
			std::vector< TEST_MODE >::const_iterator pCurrentMode = TestedModes.begin();
			pCurrentMode < TestedModes.end();
			++pCurrentMode
		)
		{
			if ( pCurrentMode->Mode.Format == CFormatsToTest::FormatList[ iCurrentFormat ] )
			{
				bFoundAtLeastOneModeForThisFormat = true;
				break;
			};
		};

		if ( bFoundAtLeastOneModeForThisFormat != true )
		{
			WriteToLog
			(
                _T( "CValidateModeListTestGroup::ExecuteTestCase: No modes found for required format %s. Please check if it is really not supported." ),
				ToString( CFormatsToTest::FormatList[ iCurrentFormat ] ).c_str()
			);
            //******************************************************************************************
            // FUTURE WORK ITEM
            //
            // Certain modes (or at least display formats) are required for different D3D Feature levels.   
            // This test case should be enforcing those requirements.
            //******************************************************************************************
        	OverallResult = RESULT_SKIP;
			goto Cleanup;
		};
	};


	OverallResult = RESULT_PASS;


Cleanup:
    DebugOut.Write("CValidateModeListTestGroup::ExecuteTestCase: Exiting.\n");

	return OverallResult;
};
