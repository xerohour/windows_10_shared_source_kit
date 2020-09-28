#include "TestManagerPCH.h"

#include "CTestManager.h"
#include "HelperFunctions.h"

// --------------------------------------------------------------------------------------

CTestManager::CTestManager()
{
    m_pCurrentTest = NULL;
    m_pTestList = NULL;
    m_dwExecuteState = EXECUTE_RUN;
	m_uTotalTests = 0;
    m_uTestNumber = 0;
	m_bReverseGroup = false;
}

// --------------------------------------------------------------------------------------

CTestManager::~CTestManager()
{   
}

// --------------------------------------------------------------------------------------

void CTestManager::ReleaseAll()
{
	PTESTLIST   pWalk = m_pTestList;
    PTESTRANGE  pRWalk;

	m_uTotalTests = 0;
    m_uTestNumber = 0;

    while (pWalk)
    {
		pRWalk = pWalk->pTestRange;
		while (pRWalk)
        {
            PTESTRANGE pTempRange = pRWalk;
			pRWalk = pRWalk->pNext;
            SAFEDELETE(pTempRange);
        }

        PTESTLIST pTempList = pWalk;
		pWalk = pWalk->pNext;
        SAFEDELETE(pTempList);
    }
	
	m_pCurrentTest = NULL;
	m_pTestList = NULL;
}

// --------------------------------------------------------------------------------------

CTest * CTestManager::GetCurrentTest(void)
{
    if (NULL == m_pCurrentTest)
        return NULL;

    return m_pCurrentTest->pTest;
}

// --------------------------------------------------------------------------------------

bool CTestManager::SetCurrentTest(CTest * pTest)
{
    PTESTLIST pWalk = m_pTestList;
	UINT uCount = 1;

    while (NULL != pWalk)
    {
        if (pWalk->pTest == pTest)
        {
            m_pCurrentTest = pWalk;
            m_uTestNumber = uCount;
            return true;
        }

        pWalk=pWalk->pNext;
		uCount++;
    }

    return false;
}

// --------------------------------------------------------------------------------------

bool CTestManager::AddTestRange(CTest * pTest, UINT uStart, UINT uStop)
{
    PTESTLIST   pWalk = m_pTestList;
    PTESTRANGE  pRangeWalk;
    PTESTRANGE  pNew;

    // Walk the test list looking for the test

    while (NULL != pWalk)
    {
        if (pWalk->pTest == pTest)
            break;

        pWalk=pWalk->pNext;
    }

    // Determine if the test has been located

    if (NULL == pWalk)
        return false;

    // allocate a new test range

    pNew = new TESTRANGE;

    if (NULL == pNew)
        return false;

    // Setup the range

    pNew->uStart = uStart;
    pNew->uStop = uStop;
    pNew->pNext = NULL;
    pNew->pPrev = NULL;

    // Walk the list looking for the last range

    pRangeWalk = pWalk->pTestRange;

    while (NULL != pRangeWalk && NULL != pRangeWalk->pNext)
        pRangeWalk = pRangeWalk->pNext;

    // Insert execution range into list

    if (NULL == pRangeWalk)
        pWalk->pTestRange = pNew;
    else
    {
        pRangeWalk->pNext = pNew;
        pNew->pPrev = pRangeWalk;
    }

    return true;
}

// --------------------------------------------------------------------------------------

bool CTestManager::AddTest(CTest * pTest, UINT uStart, UINT uStop)
{
    PTESTLIST   pNew = new TESTLIST;
    PTESTLIST   pWalk = m_pTestList;
    PTESTRANGE  pRange;

    if (NULL == pNew)
        return false;

    pRange = new TESTRANGE;

    if (NULL == pRange)
    {
        delete pNew;
        return false;
    }

    // Initialize test range

    pRange->uStart = uStart;
    pRange->uStop = uStop;
    pRange->pNext = NULL;
    pRange->pPrev = NULL;

    // Initialize test data

    pNew->pPrev = NULL;
    pNew->pNext = NULL;
    pNew->pTest = pTest;
    pNew->pTestRange = pRange;

    // Find the end of the list

    while (NULL != pWalk && NULL != pWalk->pNext)
		pWalk = pWalk->pNext;

    // Add test to list

    if (NULL == pWalk)
        m_pTestList = pNew;
    else
    {
        pNew->pPrev = pWalk;
        pWalk->pNext = pNew;
    }

    // If the current test hasn't been set, set it to the first one

    if (NULL == m_pCurrentTest)
        m_pCurrentTest = m_pTestList;

	// Update the stats
	m_uTotalTests++;

    return true;
}

// --------------------------------------------------------------------------------------

bool CTestManager::AddTest(CTest * pTest)
{
    return AddTest(pTest,1,0xFFFFFFFF);
}

// --------------------------------------------------------------------------------------

CTest* CTestManager::GetActiveTest(UINT uIndex)
{
    PTESTLIST pWalk = m_pTestList;

    while (NULL != pWalk)
    {
		if(pWalk->pTest->IsSelected())
		{
			if (0 == uIndex)
				return pWalk->pTest;

			--uIndex;
		}

        pWalk=pWalk->pNext;
    }

    return NULL;
}

// --------------------------------------------------------------------------------------

CTest * CTestManager::GetTest(UINT uIndex)
{
    PTESTLIST pWalk = m_pTestList;

    while (NULL != pWalk)
    {
        if (0 == uIndex)
            return pWalk->pTest;

        --uIndex;

        pWalk=pWalk->pNext;
    }

    return NULL;
}

// --------------------------------------------------------------------------------------

bool CTestManager::QueryTestRange(CTest * pTest)
{
    // By default, add the test and leave the range alone.

    return true;
}

// --------------------------------------------------------------------------------------

void CTestManager::Run()
{
	m_dwExecuteState = EXECUTE_RUN;
}

// --------------------------------------------------------------------------------------

void CTestManager::Pause()
{
	m_dwExecuteState = EXECUTE_PAUSE;
}

// --------------------------------------------------------------------------------------

void CTestManager::Step()
{
	// If we're currently running, then pause on the next frame
	if(m_dwExecuteState == EXECUTE_RUN)
        m_dwExecuteState = EXECUTE_PAUSE;
	// Otherwise, step to the next frame
	else
		m_dwExecuteState = EXECUTE_STEP;
}

// --------------------------------------------------------------------------------------

void CTestManager::Replay()
{
	m_dwExecuteState = EXECUTE_REPLAY;
}

// --------------------------------------------------------------------------------------

bool CTestManager::IsTestPaused()
{
	// If we've want to step, pause and execute the current test
	if(m_dwExecuteState == EXECUTE_STEP)
	{
		m_dwExecuteState = EXECUTE_PAUSE;
		return false;
	}
	else
		return m_dwExecuteState == EXECUTE_PAUSE;
}

// --------------------------------------------------------------------------------------

bool CTestManager::IsTestFinished()
{
	return m_dwExecuteState == EXECUTE_TERMINATE;
}

// --------------------------------------------------------------------------------------

bool CTestManager::IsTestReplaying()
{
	return m_dwExecuteState == EXECUTE_REPLAY;
}

// --------------------------------------------------------------------------------------

void CTestManager::NextGroup(void)
{
	m_bReverseGroup = false;

	if(NULL != m_pCurrentTest)
		m_pCurrentTest->pTest->NextGroup();
    if(m_dwExecuteState == EXECUTE_REPLAY || m_dwExecuteState == EXECUTE_PAUSE)
        m_dwExecuteState = EXECUTE_STEP;
}

// --------------------------------------------------------------------------------------

void CTestManager::PreviousGroup(void)
{
	bool bReversedTest = false;

	m_bReverseGroup = false;

	if(NULL != m_pCurrentTest)
	{
		if( (bReversedTest = m_pCurrentTest->pTest->PreviousGroup()) ||
			(NULL != m_pCurrentTest->pPrev ))

		{
			if(!bReversedTest)
				m_bReverseGroup = true;
			// Move test execution to the next group
			m_pCurrentTest->pTest->NextGroup();
			// Trigger test execution of the next test case
			if(m_dwExecuteState == EXECUTE_REPLAY || m_dwExecuteState == EXECUTE_PAUSE)
				m_dwExecuteState = EXECUTE_STEP;
		}
	}
}