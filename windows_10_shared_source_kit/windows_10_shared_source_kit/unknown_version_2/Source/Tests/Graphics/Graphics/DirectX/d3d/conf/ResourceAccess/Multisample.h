#pragma once

#include "ResourceAccess.h"

class CCopyMultisampleTest : public CResourceAccessTest
{
public:
	CCopyMultisampleTest( )
	{}

	void		InitTestParameters();
	TEST_RESULT	Setup();
	TEST_RESULT	SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT	CreateResources();

protected:
	UINT32		m_uNumSamples;
	bool		m_bIntFormat;
};