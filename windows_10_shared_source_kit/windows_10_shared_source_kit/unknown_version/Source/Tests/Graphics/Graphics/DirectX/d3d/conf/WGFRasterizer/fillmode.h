//////////////////////////////////////////////////////////////////////////
//  FillMode.h
//  created:	2006/07/17
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "rasterizer.h"
	
class CFillMode : public CRasterizerTest
{
public:
	CFillMode();
	void InitTestParameters();

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();
	UINT ConvertToLines();
};