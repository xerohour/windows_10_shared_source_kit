//////////////////////////////////////////////////////////////////////////
//  clamp.h
//  created:	2006/11/10
//
//  purpose:    conformance for depth clamp in the viewport struct
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "depth.h"

enum DEPTH_VALUE { 
	MIN_DEPTH,
	MAX_DEPTH,
	MID_DEPTH,
	NEAR_MIN_DEPTH,
	NEAR_MAX_DEPTH,
	PAST_MIN_DEPTH,
	PAST_MAX_DEPTH
};

class CClampTest : public CDepthTest
{
public:
	CClampTest();

private:
	void InitTestParameters();
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	void Cleanup();

	D3D11_VIEWPORT m_VP;
	DEPTH_VALUE m_DepthValue;
};
