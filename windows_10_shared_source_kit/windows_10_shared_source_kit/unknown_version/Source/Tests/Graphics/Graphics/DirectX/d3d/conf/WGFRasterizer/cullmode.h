//////////////////////////////////////////////////////////////////////////
//  Cull.h
//  created:	2006/07/17
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "rasterizer.h"

class CCullMode : public CRasterizerTest
{
public:
	CCullMode();
	void InitTestParameters();

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();
	bool Expected();
};