#pragma once

//////////////////////////////////////////////////////////////////////////
//  testapp.h
//  Created:	10/03/2007
//
//  Purpose: This is the main test application for testing the functionality
//			of the D3D10on9 feature levels.
//////////////////////////////////////////////////////////////////////////

// Includes
#include "D3D10Test.h"

class CTestApp : public CWGFTestFramework
{
public:
	CTestApp();

	TESTTYPE GetTestType() const { return TESTTYPE_FUNC; }
	
	bool InitTestGroups();
	void InitOptionVariables();
	bool Setup();

	tstring m_FeatureLevelLogPath;

	D3D_FEATURE_LEVEL GetMinimumFeatureLevel() const { return m_minimumFeatureLevel; }
	UINT GetMinimumFeatureLevelAsUInt() const { return (UINT) m_minimumFeatureLevel; }

protected:
	D3D_FEATURE_LEVEL m_minimumFeatureLevel;
};

extern CTestApp g_TestApp;

