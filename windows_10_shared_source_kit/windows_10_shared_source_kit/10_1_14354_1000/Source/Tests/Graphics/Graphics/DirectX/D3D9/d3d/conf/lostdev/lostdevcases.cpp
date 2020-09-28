#include "LostDev.h"

// ----------------------------------------------------------------------------

void CLostDev::ValidateTestType()
{
#if FAST_TEST
    m_casesTestType1Res.AddCase(TEST_LOSTWHILERENDERING, _T("LostWhileRendering"));
#else //FAST_TEST
	if (m_uCases & TEST_CREATIONWHILELOST)
	{
		m_casesTestType1Res.AddCase(TEST_CREATIONWHILELOST,	"CreationWhileLost");
	}

	if (m_uCases & TEST_CREATIONWHILELOSTFILLAFTER)
	{
		m_casesTestType1Res.AddCase(TEST_CREATIONWHILELOSTFILLAFTER, _T("CreationWhileLostFillAfter"));
	}

	if (m_uCases & TEST_LOCKWHILELOST)
	{
		m_casesTestType1Res.AddCase(TEST_LOCKWHILELOST, _T("LockWhileLost"));
	}

	if (m_uCases & TEST_LOSTWHILELOCKED)
	{
		m_casesTestType1Res.AddCase(TEST_LOSTWHILELOCKED, _T("LostWhileLocked"));
	}

	if (m_uCases & TEST_LOSTWHILERENDERING)
	{
		m_casesTestType1Res.AddCase(TEST_LOSTWHILERENDERING, _T("LostWhileRendering"));
	}
#ifdef USE_FUS
	if ((m_uLostFlags & LOST_FUS) && (m_uCases & TEST_FUSCREATEFUSBACK))
	{
		m_casesTestType1Res.AddCase(TEST_FUSCREATEFUSBACK, _T("FUSCreateFUSBack"));
	}

	if ((m_uLostFlags & LOST_FUS) && (m_uCases & TEST_CREATEFUSDELETEFUSBACK))
	{
		m_casesTestType1Res.AddCase(TEST_CREATEFUSDELETEFUSBACK, _T("CreateFUSDeleteFUSBack"));
	}
#endif
	if (m_uCases & TEST_CREATEAANDBTHENLOSE)
	{
		m_casesTestType2Res.AddCase(TEST_CREATEAANDBTHENLOSE, _T("CreateAAndBThenLose"));
	}

	if (m_uCases & TEST_CREATEALOSECREATEB)
	{
		m_casesTestType2Res.AddCase(TEST_CREATEALOSECREATEB, _T("CreateAThenLoseThenCreateB"));
	}

	if (m_uCases & TEST_LOSETHENCREATEAANDB)
	{
		m_casesTestType2Res.AddCase(TEST_LOSETHENCREATEAANDB, _T("LoseThenCreateAAndB"));
	}
	//...

	if (m_uCases & TEST_DEFAULTPOOLTEST)
	{
		m_casesTestTypeDefault.AddCase(TEST_DEFAULTPOOLTEST, _T("DefaultPoolTest"));
	}

	if ( (m_uLostFlags & LOST_STANDBY) && (m_uCases & TEST_CREATEAANDBTHENSUSPENDSYSTEM) )
	{
		m_casesTestTypeDefault.AddCase(TEST_CREATEAANDBTHENSUSPENDSYSTEM, _T("CreateAAndBThenSuspendSystem(standby)"));
	}

	if ( (m_uLostFlags & LOST_HIBERNATE) && (m_uCases & TEST_CREATEAANDBTHENSUSPENDSYSTEM) )
	{
		m_casesTestTypeDefault.AddCase(TEST_CREATEAANDBTHENSUSPENDSYSTEM, _T("CreateAAndBThenSuspendSystem(hibernate)"));
	}

	if (m_uCases & TEST_RELEASEAFTERRESETFAILED)
	{
		//Do not test until this is debugged
		m_casesTestType1Res.AddCase(TEST_RELEASEAFTERRESETFAILED, _T("ReleaseAfterResetFailed"));
	}
#endif//FAST_TEST
}


void CLostDev::ValidatePools()
{
#if FAST_TEST
    m_casesPools.AddCase(POOL_SYSTEMMEM, _T("SYSTEMMEM"));
#else FAST_TEST//FAST_TEST
	if ( m_bTestSystemMem && (m_uPoolsToTest & TEST_SYSTEMMEM) )
	{
		m_casesPools.AddCase(POOL_SYSTEMMEM, _T("SYSTEMMEM"));
	}

	if ( m_bTestManaged && (m_uPoolsToTest & TEST_MANAGED) )
	{
		m_casesPools.AddCase(POOL_MANAGED, _T("MANAGED"));
	}

	if ( m_bTestDefault && (m_uPoolsToTest & TEST_DEFAULT) )
	{
		m_casesPools.AddCase(POOL_DEFAULT, _T("DEFAULT"));
	}

	if (m_bTestScratch && (m_uPoolsToTest & TEST_SCRATCH) )
	{
		m_casesPools.AddCase(POOL_SCRATCH, _T("SCRATCH"));
	}

	if (m_casesPools.GetNbCases() == 0)
	{
		m_bSkipGroup = true;
	}
#endif
}

void CLostDev::ValidateLostWays()
{
	//if (m_uLostFlags & LOST_CHANGEDISPLAY)
	//{
	//	m_casesLostWays.AddCase(LOST_CHANGEDISPLAY, _T("ChangeDisplay"));
	//}
#if FAST_TEST
    m_casesLostWays.AddCase(LOST_CDSBYOTHERPROCESS, _T("ChangeDisplayByOtherProcess"));
#else//FAST_TEST
	if (m_uLostFlags & LOST_FUS)
	{
		m_casesLostWays.AddCase(LOST_FUS, _T("FUS"));
	}

	if (m_uLostFlags & LOST_CDSBYOTHERPROCESS)
	{
		m_casesLostWays.AddCase(LOST_CDSBYOTHERPROCESS, _T("ChangeDisplayByOtherProcess"));
	}

	if (m_uLostFlags & LOST_STANDBY)
	{
		m_casesLostWays.AddCase(LOST_STANDBY, _T("Standby"));
	}

	if (m_uLostFlags & LOST_HIBERNATE)
	{
		m_casesLostWays.AddCase(LOST_HIBERNATE, _T("Hibernate"));
	}

	if (m_casesLostWays.GetNbCases() == 0)
	{
		//m_casesLostWays.AddCase(LOST_CHANGEDISPLAY, _T("ChangeDisplay"));
		//m_casesLostWays.AddCase(LOST_FUS, _T("FUS"));
		m_casesLostWays.AddCase(LOST_CDSBYOTHERPROCESS, _T("ChangeDisplayByOtherProcess"));
	}
#endif//FAST_TEST
}


void CLostDev::SetTestsToRun()
{
	m_uPoolsToTest = TEST_ALLPOOLS;

	//Determine which pools to test
	if ( KeySet(_T("POOL_SYSTEMMEM")) )
	{
		m_uPoolsToTest = TEST_SYSTEMMEM;
	}
	if ( KeySet(_T("!POOL_SYSTEMMEM")) || KeySet(_T("WHQL")))
	{
		m_uPoolsToTest &= ~TEST_SYSTEMMEM;
	}

	if ( KeySet(_T("POOL_MANAGED")) )
	{
		m_uPoolsToTest = TEST_MANAGED;
	}
	if ( KeySet(_T("!POOL_MANAGED")) )
	{
		m_uPoolsToTest &= ~TEST_MANAGED;
	}

	if ( KeySet(_T("POOL_DEFAULT")) )
	{
		m_uPoolsToTest = TEST_DEFAULT;
	}
	if ( KeySet(_T("!POOL_DEFAULT")) )
	{
		m_uPoolsToTest &= ~TEST_DEFAULT;
	}

	if ( KeySet(_T("POOL_SCRATCH")) )
	{
		m_uPoolsToTest = TEST_SCRATCH;
	}
	if ( KeySet(_T("!POOL_SCRATCH")) || KeySet(_T("WHQL")))
	{
		//Do not test scratch for whql
		m_uPoolsToTest &= ~TEST_SCRATCH;
	}
	if (0 == m_uPoolsToTest)
	{
		m_uPoolsToTest = TEST_ALLPOOLS;
	}

	//Individual test cases to run
	m_uCases = TEST_ALLCASES;
	if ( KeySet(_T("TEST_CREATIONWHILELOST")) )
	{
		m_uCases = TEST_CREATIONWHILELOST;
	}
	if ( KeySet(_T("!TEST_CREATIONWHILELOST")) )
	{
		m_uCases &= ~TEST_CREATIONWHILELOST;
	}

	if ( KeySet(_T("TEST_CREATIONWHILELOSTFILLAFTER")) )
	{
		m_uCases = TEST_CREATIONWHILELOSTFILLAFTER;
	}
	if ( KeySet(_T("!TEST_CREATIONWHILELOSTFILLAFTER")) )
	{
		m_uCases &= ~TEST_CREATIONWHILELOSTFILLAFTER;
	}

	if ( KeySet(_T("TEST_LOCKWHILELOST")) )
	{
		m_uCases = TEST_LOCKWHILELOST;
	}
	if ( KeySet(_T("!TEST_LOCKWHILELOST")) )
	{
		m_uCases &= ~TEST_LOCKWHILELOST;
	}

	if ( KeySet(_T("TEST_LOSTWHILELOCKED")) )
	{
		m_uCases = TEST_LOSTWHILELOCKED;
	}
	if ( KeySet(_T("!TEST_LOSTWHILELOCKED")) )
	{
		m_uCases &= ~TEST_LOSTWHILELOCKED;
	}

	if ( KeySet(_T("TEST_LOSTWHILERENDERING")) )
	{
		m_uCases = TEST_LOSTWHILERENDERING;
	}
	if ( KeySet(_T("!TEST_LOSTWHILERENDERING")) )
	{
		m_uCases &= ~TEST_LOSTWHILERENDERING;
	}

	if ( KeySet(_T("TEST_CREATEAANDBTHENLOSE")) )
	{
		m_uCases = TEST_CREATEAANDBTHENLOSE;
	}
	if ( KeySet(_T("!TEST_CREATEAANDBTHENLOSE")) )
	{
		m_uCases &= ~TEST_CREATEAANDBTHENLOSE;
	}

	if ( KeySet(_T("TEST_CREATEALOSECREATEB")) )
	{
		m_uCases = TEST_CREATEALOSECREATEB;
	}
	if ( KeySet(_T("!TEST_CREATEALOSECREATEB")) )
	{
		m_uCases &= ~TEST_CREATEALOSECREATEB;
	}

	if ( KeySet(_T("TEST_LOSETHENCREATEAANDB")) )
	{
		m_uCases = TEST_LOSETHENCREATEAANDB;
	}
	if ( KeySet(_T("!TEST_LOSETHENCREATEAANDB")) )
	{
		m_uCases &= ~TEST_LOSETHENCREATEAANDB;
	}

	if ( KeySet(_T("TEST_FUSCREATEFUSBACK")) )
	{
		m_uCases = TEST_FUSCREATEFUSBACK;
	}
	if ( KeySet(_T("!TEST_FUSCREATEFUSBACK")) )
	{
		m_uCases &= ~TEST_FUSCREATEFUSBACK;
	}

	if ( KeySet(_T("TEST_CREATEFUSDELETEFUSBACK")) )
	{
		m_uCases = TEST_CREATEFUSDELETEFUSBACK;
	}
	if ( KeySet(_T("!TEST_CREATEFUSDELETEFUSBACK")) )
	{
		m_uCases &= ~TEST_CREATEFUSDELETEFUSBACK;
	}

	if ( KeySet(_T("TEST_CREATEAANDBTHENSUSPENDSYSTEM")) )
	{
		m_uCases = TEST_CREATEAANDBTHENSUSPENDSYSTEM;
	}
	if ( KeySet(_T("!TEST_CREATEAANDBTHENSUSPENDSYSTEM")) )
	{
		m_uCases &= ~TEST_CREATEAANDBTHENSUSPENDSYSTEM;
	}


	if (! KeySet(_T("WHQL")) )
	{
		if ( KeySet(_T("TEST_RELEASEAFTERRESETFAILED")) )
		{
			m_uCases = TEST_RELEASEAFTERRESETFAILED;
		}
		if ( KeySet(_T("!TEST_RELEASEAFTERRESETFAILED")) )
		{
			m_uCases &= ~TEST_RELEASEAFTERRESETFAILED;
		}
	}


/*
	//Flags
	m_uFlags = FLAG_ALLFLAGS;
	if ( KeySet(_T("FLAG_READONLY")) )
	{
		m_uFlags = FLAG_READONLY;
	}
	if ( KeySet(_T("!FLAG_READONLY")) )
	{
		m_uFlags &= ~FLAG_READONLY;
	}

	if ( KeySet(_T("FLAG_READWRITE")) )
	{
		m_uFlags = FLAG_READWRITE;
	}
	if ( KeySet(_T("!FLAG_READWRITE")) )
	{
		m_uFlags &= ~FLAG_READWRITE;
	}
	if (! m_uFlags)
		m_uFlags = FLAG_ALLFLAGS;
*/


	//Ways to lose the device
	m_uLostFlags = LOST_ALLLOSTWAYS;
	if ( KeySet(_T("LOST_CHANGEDISPLAY")) )
	{
		m_uLostFlags = LOST_CHANGEDISPLAY;
	}
	if ( KeySet(_T("!LOST_CHANGEDISPLAY")) || KeySet(_T("WHQL")) )
	{
		m_uLostFlags &= ~LOST_CHANGEDISPLAY;
	}

	if ( KeySet(_T("LOST_FUS")) )
	{
		m_uLostFlags = LOST_FUS;
	}
	if ( KeySet(_T("!LOST_FUS")) )
	{
		m_uLostFlags &= ~LOST_FUS;
	}

	if ( KeySet(_T("LOST_CDSBYOTHERPROCESS")) )
	{
		m_uLostFlags = LOST_CDSBYOTHERPROCESS;
	}
	if ( KeySet(_T("!LOST_CDSBYOTHERPROCESS")) )
	{
		m_uLostFlags &= ~LOST_CDSBYOTHERPROCESS;
	}

	if ( KeySet( _T("LOST_STANDBY" ) ) )
	{
		m_uLostFlags = LOST_STANDBY;
	}
	else
	{
		m_uLostFlags &= ~LOST_STANDBY;
	}

	if ( KeySet( _T("LOST_HIBERNATE") ) )
	{
		m_uLostFlags = LOST_HIBERNATE;
	}
	else
	{
		m_uLostFlags &= ~LOST_HIBERNATE;
	};


	m_bSeeResult = false;
	if (KeySet(_T("SEERESULT")) || KeySet(_T("SEERESULTS")))
	{
		m_bSeeResult = true;
	};


	m_bSeeFailures = false;
	if (KeySet(_T("SEEFAILURE")) || KeySet(_T("SEEFAILURES")))
	{
		m_bSeeFailures = true;
	};


#ifndef USE_FUS
	//Disable FUS if it's not enabled
	m_uLostFlags &= ~LOST_FUS;
#endif
}


bool CLostDev::CreateTestFactors()
{
	ValidatePools();
	ValidateTestType();
	ValidateLostWays();

	UINT uNbDefaultCases = m_casesPools.DoesCaseExist(POOL_DEFAULT) ? m_casesTestTypeDefault.GetNbCases() : 0;
	UINT uNbScratch1ResCases = m_casesPools.DoesCaseExist(POOL_SCRATCH) ? m_casesTestType1Res.GetNbCases() : 0;
	UINT uNbScratch2ResCases = m_casesPools.DoesCaseExist(POOL_SCRATCH) ? m_casesTestType2Res.GetNbCases() : 0;
	UINT uNbManaged1ResCases = m_casesPools.DoesCaseExist(POOL_MANAGED) ? m_casesTestType1Res.GetNbCases() : 0;
	UINT uNbManaged2ResCases = m_casesPools.DoesCaseExist(POOL_MANAGED) ? m_casesTestType2Res.GetNbCases() : 0;
	UINT uNbSystemMem1ResCases = m_casesPools.DoesCaseExist(POOL_SYSTEMMEM) ? m_casesTestType1Res.GetNbCases() : 0;
	UINT uNbSystemMem2ResCases = m_casesPools.DoesCaseExist(POOL_SYSTEMMEM) ? m_casesTestType2Res.GetNbCases() : 0;

	//Create the factors for pools and lost ways
	if(	!CHECKREF(m_rPoolDefault = NewFactor( uNbDefaultCases )) ||
		!CHECKREF(m_rPoolManaged1Res = NewFactor( uNbManaged1ResCases )) ||
		!CHECKREF(m_rPoolManaged2Res = NewFactor( uNbManaged2ResCases )) ||
		!CHECKREF(m_rPoolSystemMem1Res = NewFactor( uNbSystemMem1ResCases )) ||
		!CHECKREF(m_rPoolSystemMem2Res = NewFactor( uNbSystemMem2ResCases )) ||
		!CHECKREF(m_rPoolScratch1Res = NewFactor( uNbScratch1ResCases )) ||
		!CHECKREF(m_rPoolScratch2Res = NewFactor( uNbScratch2ResCases )) ||
		!CHECKREF(m_rLostWays = NewFactor( m_casesLostWays.GetNbCases() )))
	{
		DPF(0, "ERROR setting the cases in SetTestsToRun()");
		return false;
	}

	m_rPoolManaged = m_rPoolManaged1Res + m_rPoolManaged2Res;
	m_rPoolSystemMem = m_rPoolSystemMem1Res + m_rPoolSystemMem2Res;
	m_rPoolScratch = m_rPoolScratch1Res + m_rPoolScratch2Res;
	return true;
}


void CLostDev::SetTestFactors()
{
	m_rCasesManager =
		m_rLostWays *
		(
			m_rPoolManaged + 
			m_rPoolSystemMem +
			m_rPoolScratch +
			m_rPoolDefault 
		);
}

// ----------------------------------------------------------------------------

bool CLostDev::SetTestCase(UINT uTestNumber)
{
//	m_uTestNumber = uTestNumber;

	//Set the pool and test type
	if (m_rPoolDefault->Active())
	{
		m_Pool = POOL_DEFAULT;
		m_szPoolType = _T("Default");
		m_uTestType = m_casesTestTypeDefault[ m_rPoolDefault->Value() ]->GetValue();
		m_szSubTest = m_casesTestTypeDefault[ m_rPoolDefault->Value() ]->GetDesc();
	}
	else if (m_rPoolManaged1Res->Active() || m_rPoolManaged2Res->Active())
	{
		m_Pool = POOL_MANAGED;
		m_szPoolType = _T("Managed");
		if (m_rPoolManaged1Res->Active())
		{
			m_uTestType = m_casesTestType1Res[ m_rPoolManaged1Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType1Res[ m_rPoolManaged1Res->Value() ]->GetDesc();
		}
		else //if (m_rPoolManaged2Res->Active())
		{
			m_uTestType = m_casesTestType2Res[ m_rPoolManaged2Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType2Res[ m_rPoolManaged2Res->Value() ]->GetDesc();
		}
	}
	else if (m_rPoolSystemMem1Res->Active() || m_rPoolSystemMem2Res->Active())
	{
		m_Pool = POOL_SYSTEMMEM;
		m_szPoolType = _T("SystemMem");
		if (m_rPoolSystemMem1Res->Active())
		{
			m_uTestType = m_casesTestType1Res[ m_rPoolSystemMem1Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType1Res[ m_rPoolSystemMem1Res->Value() ]->GetDesc();
		}
		else //if (m_rPoolSystemMem2Res->Active())
		{
			m_uTestType = m_casesTestType2Res[ m_rPoolSystemMem2Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType2Res[ m_rPoolSystemMem2Res->Value() ]->GetDesc();
		}
	}
	else if (m_rPoolScratch1Res->Active() || m_rPoolScratch2Res->Active())
	{
		m_Pool = POOL_SCRATCH;
		m_szPoolType = _T("Scratch");
		if (m_rPoolScratch1Res->Active())
		{
			m_uTestType = m_casesTestType1Res[ m_rPoolScratch1Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType1Res[ m_rPoolScratch1Res->Value() ]->GetDesc();
		}
		else //if (m_rPoolScratch2Res->Active())
		{
			m_uTestType = m_casesTestType2Res[ m_rPoolScratch2Res->Value() ]->GetValue();
			m_szSubTest = m_casesTestType2Res[ m_rPoolScratch2Res->Value() ]->GetDesc();
		}
	}
	else //Error 
	{
		DPF(0, "ERROR in lostdev, no pool active");
		return false;
	}

	//Set the lost way
	m_uLostType = (UINT)m_casesLostWays[ m_rLostWays->Value() ]->GetValue();
	m_szLostType = m_casesLostWays[ m_rLostWays->Value() ]->GetDesc();
	return true;
}

// ----------------------------------------------------------------------------

void CLostDev::CommandLineHelp(void)
{
	WriteCommandLineHelp("$gPOOLS");
	WriteCommandLineHelp("$y-POOL_SYSTEMMEM\t$wTest only the SystemMem pool");
	WriteCommandLineHelp("$y-POOL_MANAGED\t$wTest only the Managed pool");
	WriteCommandLineHelp("$y-POOL_DEFAULT\t$wTest only the Default pool");

	WriteCommandLineHelp("$y-!POOL_SYSTEMMEM\t$wDo NOT test the SystemMem pool");
	WriteCommandLineHelp("$y-!POOL_MANAGED\t$wDo NOT test the Managed pool");
	WriteCommandLineHelp("$y-!POOL_DEFAULT\t$wDo NOT test the Default pool");

	WriteCommandLineHelp("\n$gTEST CASES");
	WriteCommandLineHelp("\n$g\tFor all the following flag, if '!' is put before 'TEST', the test case will");
	WriteCommandLineHelp("\n$g\tbe excluded(eg. -!TEST_LOCKWHILELOST).  Many test cases can be excluded.  Otherwise,");
	WriteCommandLineHelp("\n$g\tONLY the test case of the TEST parameter will be executed.\n");
	WriteCommandLineHelp("$y-TEST_CREATIONWHILELOST\t$wTest case that creates a resource while the dev is lost");
	WriteCommandLineHelp("$y-TEST_CREATIONWHILELOSTFILLAFTER\t$wTest case that creates a res while the dev is lost but fills it after the reset");
	WriteCommandLineHelp("$y-TEST_LOCKWHILELOST\t$wTest case that lock a res while the dev is lost");
	WriteCommandLineHelp("$y-TEST_LOSTWHILELOCKED\t$wTest case that lose the dev while a res is locked");
	WriteCommandLineHelp("$y-TEST_LOSTWHILERENDERING\t$wTest case that lose the dev after rendering, then re-render with same res");
	WriteCommandLineHelp("$y-TEST_CREATEAANDBTHENLOSE\t$wTest case that creates res A and res B, loses the dev and copies A to B");
	WriteCommandLineHelp("$y-TEST_CREATEALOSECREATEB\t$wTest case that creates res A, loses the dev, creates res B, then copies A to B");
	WriteCommandLineHelp("$y-TEST_LOSETHENCREATEAANDB\t$wTest case that loses the dev, then creates res A and res B and copies A to B");
	WriteCommandLineHelp("$y-TEST_RELEASEAFTERRESETFAILED\t$wTest case that creates a resource then causes the Reset to fail and try to release after");

/*    WriteCommandLineHelp("\n$gFLAGS");
	WriteCommandLineHelp("$y-FLAG_READONLY\t$wRun test cases with READONLY Flag");
	WriteCommandLineHelp("$y-FLAG_READWRITE\t$wRun test cases with no flag");
	WriteCommandLineHelp("$y-!FLAG_READONLY\t$wDo NOT run test cases with READONLY Flag");
	WriteCommandLineHelp("$y-!FLAG_READWRITE\t$wDo NOT run test cases with no flag");
*/
	WriteCommandLineHelp("\n$gWAYS TO LOSE DEV");
	WriteCommandLineHelp("$y-LOST_CHANGEDISPLAY\t$wLose the device with ChangeDisplaySettings");
	WriteCommandLineHelp("$y-LOST_CDSBYOTHERPROCESS\t$wLose the device by another process calling ChangeDisplaySettings");
	WriteCommandLineHelp("$y-LOST_FUS\t$wLose the device with Fast User Switching");
	WriteCommandLineHelp("$y-!LOST_CHANGEDISPLAY\t$wDo NOT lose the device with ChangeDisplaySettings");
	WriteCommandLineHelp("$y-!LOST_CDSBYOTHERPROCESS\t$wDo NOT lose the device by another process calling ChangeDisplaySettings");
	WriteCommandLineHelp("$y-!LOST_FUS\t$wDo NOT lose the device with Fast User Switching");


	WriteCommandLineHelp("\n$gOPTIONS");
	WriteCommandLineHelp("$y-SEERESULT\t$wWaits for 2 seconds after rending.");
	WriteCommandLineHelp("$y-SEEFAILURE\t$wWaits for 2 seconds after rendering a failure.");
}
