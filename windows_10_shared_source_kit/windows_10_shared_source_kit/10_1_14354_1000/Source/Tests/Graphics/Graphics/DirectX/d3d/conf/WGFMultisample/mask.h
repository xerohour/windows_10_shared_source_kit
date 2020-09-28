//////////////////////////////////////////////////////////////////////////
//  Mask.h
//  created:	2005/07/19
//
//  purpose:  
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "multisample.h"
	
class CMultisampleTest_Mask : public CMultisampleTest
{
public:
	CMultisampleTest_Mask() : 
	  CMultisampleTest()
	  {}

	void InitTestParameters();

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();
};