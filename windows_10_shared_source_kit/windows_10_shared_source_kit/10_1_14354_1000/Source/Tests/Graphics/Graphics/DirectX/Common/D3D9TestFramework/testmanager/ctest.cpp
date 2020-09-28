#include "TestManagerPCH.h"

#include "CTestManager.h"
#include "HelperFunctions.h"
#include "stdarg.h"
#include "stdio.h"

// --------------------------------------------------------------------------------------

CTest::CTest()
{
    m_uTestNumber = 0;
    m_pRangeList = NULL;
    m_pConstraints = NULL;
    m_szTestName = "Set m_szTestName and m_szCommandKey in constructor.";
    m_szCommandKey = m_szTestName;
	m_szLogName = NULL;
    m_uTotalTests = 0;
	m_bSelected = false;

    SetTestRange(1, DEFAULT_RANGE); 
}

// --------------------------------------------------------------------------------------

CTest::~CTest()
{
	PTESTRANGE pTemp = NULL;

    // Release the test range list
    while (NULL != m_pRangeList)
    {
        pTemp = m_pRangeList;
		m_pRangeList = m_pRangeList->pNext;
        SAFEDELETE(pTemp);
    }

    while (NULL != m_pConstraints)
    {
        pTemp = m_pConstraints;
        m_pConstraints = m_pConstraints->pNext;
        SAFEDELETE(pTemp);
    }
}

// --------------------------------------------------------------------------------------

bool CTest::SetCurrentTest(UINT nTestNum)
{
	PTESTRANGE pWalk = m_pRangeList;

	while(pWalk)
	{
		if(nTestNum >= pWalk->uStart && nTestNum <= pWalk->uStop)
		{
			m_uTestNumber = nTestNum;
			return true;
		}
		pWalk = pWalk->pNext;
	}
	return false;
}

// --------------------------------------------------------------------------------------

UINT CTest::FindNextTest(void)
{
    PTESTRANGE  pWalk;
    PTESTRANGE  pHold, pTempHold = NULL;

    if (TERMINATE_NUMBER == m_uTestNumber || NULL == m_pRangeList)
	{
		m_uTestNumber = TERMINATE_NUMBER;
        return TERMINATE_NUMBER;
	}

    pWalk = m_pRangeList;
    do
    {
        while (NULL != pWalk)
        {
            if (m_uTestNumber < pWalk->uStart)
            {
                m_uTestNumber = pWalk->uStart;
                break;
            }

            // If the test number is the same as the start, but less than stop, increment.
            // If the test number == stop, incrementing will put it beyond the range.

            if ((m_uTestNumber >= pWalk->uStart) && (m_uTestNumber < pWalk->uStop))
            {
                m_uTestNumber++;
                break;
            }

            pWalk=pWalk->pNext;
        }

        if (NULL == pWalk)
        {
            // We're done with the test. Signal termination
            m_uTestNumber = TERMINATE_NUMBER;
            return TERMINATE_NUMBER;
        }

        pHold = m_pConstraints;

        if (NULL == pHold)
            break;

        while (NULL != pHold)
        {
            // This number is less than any of constraints, then then check the first number in constraints
			if (m_uTestNumber < pHold->uStart)
			{
				pTempHold = pHold;
				m_uTestNumber = pHold->uStart - 1;
				pHold = NULL;
				break;
			}
			// This number was valid for the constraints, then return it.
            if (m_uTestNumber >= pHold->uStart && m_uTestNumber <= pHold->uStop)
                break;
			pTempHold = pHold;
            pHold=pHold->pNext;
        }

        if (NULL == pHold && m_uTestNumber > pTempHold->uStop)
        {
            m_uTestNumber = TERMINATE_NUMBER;
            return TERMINATE_NUMBER;
        }

    }
    while ( pHold == NULL );
        
    return m_uTestNumber;    
}

// --------------------------------------------------------------------------------------

bool CTest::Initialize()
{
	m_bSelected = true;

	return true;
}

// --------------------------------------------------------------------------------------

void CTest::Reset(void)
{
    m_uTestNumber = 0;
}

// --------------------------------------------------------------------------------------

bool CTest::IsTestFinished()
{
	return m_uTestNumber == TERMINATE_NUMBER;
}

// --------------------------------------------------------------------------------------

void CTest::NextGroup()
{
	m_uTestNumber = TERMINATE_NUMBER;
}


// --------------------------------------------------------------------------------------

bool CTest::PreviousGroup()
{
	return false;
}

// --------------------------------------------------------------------------------------

bool CTest::AddTestRange(PTESTRANGE * ppRange, UINT uStart, UINT uStop)
{
    PTESTRANGE pWalk = *ppRange;
    PTESTRANGE pHold = NULL;

    if (uStart > uStop)
	{
		OutputDebugString("CTest::AddTestRange() - invalid range: uStart > uStop.\n");
		return false;
	}

	if (NULL == pWalk)
    {
        *ppRange = new TESTRANGE;

        if (NULL == *ppRange)
            return false;

        (*ppRange)->uStart = uStart;
        (*ppRange)->uStop = uStop;
        (*ppRange)->pNext = NULL;
        (*ppRange)->pPrev = NULL;

		m_uTotalTests = uStop - uStart + 1;

        return true;
    }

    // Walk the current test range.

    while (NULL != pWalk)
    {
        // Are we within the current block?

        if (pWalk->uStart <= uStart && pWalk->uStop >= uStart)
        {
            // Potential to grow this test range.

            // Does this range already fit?

            if (uStop <= pWalk->uStop)
                return true;

            // Grow.

            if (NULL != pWalk->pNext)
            {
                if (pWalk->pNext->uStart <= uStop)
                {
                    PTESTRANGE  pTempHold = pWalk->pNext;

                    // We grew the current block into the next block.
					m_uTotalTests += pWalk->pNext->uStart - pWalk->uStop - 1;
                    pWalk->uStop = pWalk->pNext->uStop;
					if ( pWalk->pNext->pNext )
					{
						pWalk->pNext =  pWalk->pNext->pNext;
						pWalk->pNext->pPrev =  pWalk;
					}
					else 
						pWalk->pNext =  NULL;

                    delete pTempHold;

					if (pWalk->uStop < uStop)
						return AddTestRange(ppRange, uStart, uStop);
                }
            }

            // Simply grow this test range.

            pWalk->uStop = uStop;
			m_uTotalTests += uStop - pWalk->uStop;
			//if thest ranges are joined then we merge them
			if (pWalk->pNext && uStop+1 == pWalk->pNext->uStart)
			{
				PTESTRANGE  pTempHold = pWalk->pNext;

                pWalk->uStop = pWalk->pNext->uStop;
				if ( pWalk->pNext->pNext )
				{
					pWalk->pNext =  pWalk->pNext->pNext;
					pWalk->pNext->pPrev =  pWalk;
				}
				else 
					pWalk->pNext =  NULL;
                delete pTempHold;
			}

            return true;
        }

        // Expand backward

        if (pWalk->uStart > uStart && pWalk->uStart < uStop)
        {
            // We know from the previous check that we didn't fall anywhere into the previous
            // block, so it's safe to expand this block back.

            pWalk->uStart = uStart;
			m_uTotalTests = pWalk->uStart - uStart;
			//if thest ranges are joined then we merge them
			if (pWalk->pPrev && uStart-1 == pWalk->pPrev->uStop)
			{
				PTESTRANGE  pTempHold = pWalk->pPrev;

                pWalk->uStart = pWalk->pPrev->uStart;
				if ( pWalk->pPrev->pPrev )
				{
					pWalk->pPrev =  pWalk->pPrev->pPrev;
					pWalk->pPrev->pNext =  pWalk;
				}
				else 
					pWalk->pPrev =  NULL;
                delete pTempHold;
			}

            if (uStop > pWalk->uStop)
            {
                // Whoah! We need to grow this, and we may grow into the next block.
                // Handle this recursively. We just reset the start, so we should fall in above.

                return AddTestRange(ppRange,uStart,uStop);
            }

            // Appearently we only needed to expand backwards, so we'll just return.

            return true;
        }

        // Determine if we just passed our range.

        if (pWalk->uStart > uStop)
        {
            PTESTRANGE pNew;

            // We terminated before this block was found. We'll be inserting a new block here.

            pNew = new TESTRANGE;

            if (NULL == pNew)
                return false;

            pNew->uStart = uStart;
            pNew->uStop  = uStop;
            pNew->pNext  = pWalk;
            pNew->pPrev  = pWalk->pPrev;

			if (pWalk->pPrev != NULL)
				pWalk->pPrev->pNext = pNew;
            pWalk->pPrev = pNew;

			// Adjust head of list if necessary
            if (*ppRange == pWalk)
                *ppRange = pNew;

			m_uTotalTests += uStop - uStart + 1;
			//if thest ranges are joined at the front then we merge them
			if (pNew->pPrev && uStart-1 == pNew->pPrev->uStop)
			{
				PTESTRANGE  pTempHold = pNew->pPrev;

                pNew->uStart = pNew->pPrev->uStart;
				if ( pNew->pPrev->pPrev )
				{
					pNew->pPrev =  pNew->pPrev->pPrev;
					pNew->pPrev->pNext = pNew;
				}
				else 
					pNew->pPrev =  NULL;
				// Adjust head of list if necessary
				if (*ppRange == pTempHold)
					*ppRange = pNew;
                delete pTempHold;
			}
			//if thest ranges are joined at the back then we merge them
			if (pNew->pNext && uStop+1 == pNew->pNext->uStart)
			{
				PTESTRANGE  pTempHold = pNew->pNext;

                pNew->uStop = pNew->pNext->uStop;
				if ( pNew->pNext->pNext )
				{
					pNew->pNext =  pNew->pNext->pNext;
					pNew->pNext->pPrev =  pNew;
				}
				else 
					pNew->pNext =  NULL;
                delete pTempHold;
			}

            return true;
        }

        pHold = pWalk;
        pWalk=pWalk->pNext;
    }

    // The range was at the end of the list.
	m_uTotalTests += uStop - uStart + 1;
	//if thest ranges are joined then we merge them
	//else create new range and insert it at the end of the list
	if (uStart-1 == pHold->uStop)
	{
		pHold->uStop = uStop;
	}
	else
	{
		pWalk = new TESTRANGE;
		if (NULL == pWalk)
			return false;

		pWalk->uStart = uStart;
		pWalk->uStop  = uStop;
		pWalk->pNext = NULL;
		pWalk->pPrev = pHold;

		pHold->pNext = pWalk;
	}

    return true;
}

// --------------------------------------------------------------------------------------

bool CTest::SetTestRange(PTESTRANGE * ppRange, UINT uStart, UINT uStop)
{
    PTESTRANGE pWalk = *ppRange;
    PTESTRANGE pHold = NULL;

    if (uStart > uStop)
	{
		OutputDebugString("CTest::SetTestRange() - invalid range: uStart > uStop.\n");
		return false;
	}

	ClearRangeList();

	return AddTestRange(ppRange, uStart, uStop);
}

// --------------------------------------------------------------------------------------

bool CTest::SkipTestRange(PTESTRANGE * ppRange, UINT uStart, UINT uStop)
{
    PTESTRANGE pWalk = *ppRange;

	if (uStart > uStop)
	{
		OutputDebugString("CTest::SkipTestRange() - invalid range: uStart > uStop.\n");
		return false;
	}

    if (NULL == m_pRangeList)
        return false;

    // Walk the test range list, and exclude tests

    while (NULL != pWalk)
    {
        if (uStart <= pWalk->uStart)
        {
            // We've already gone too far.

            if (uStop < pWalk->uStart)
                return true;

            if (uStop >= pWalk->uStop)
            {
                if (NULL != pWalk->pPrev)
                    pWalk->pPrev->pNext = pWalk->pNext;

                if (NULL != pWalk->pNext)
                    pWalk->pNext->pPrev = pWalk->pPrev;

				if (*ppRange == pWalk)
                *ppRange = pWalk->pNext;

                delete pWalk;
			
                return CTest::SkipTestRange(ppRange,uStart,uStop);
            }

            // Moving this range forward
            pWalk->uStart = uStop + 1;

            return true;
        }
        else
        if (uStart <= pWalk->uStop)
        {
            if (uStop < pWalk->uStop)
            {
                UINT uOldStop = pWalk->uStop;
                // Cut this	existing range in half
				pWalk->uStop = uStart - 1;
				// AddTestRange will add the new range into the total, so we subtract it off now.
				m_uTotalTests -= uOldStop - uStop;
                return CTest::AddTestRange(ppRange,uStop+1,uOldStop);
            }

            // Cut this range short
            pWalk->uStop = uStart - 1;

            return CTest::SkipTestRange(ppRange,uStart,uStop);
        }

        pWalk=pWalk->pNext;
    }

    return true;
}

// --------------------------------------------------------------------------------------

bool CTest::AddTestRange(UINT uStart, UINT uStop, bool bConstraint)
{
    if (bConstraint)
        return AddTestRange(&m_pConstraints,uStart,uStop);

    return AddTestRange(&m_pRangeList,uStart,uStop);

}

// --------------------------------------------------------------------------------------

bool CTest::SetTestRange(UINT uStart, UINT uStop, bool bConstraints)
{
    if (bConstraints)
        return SetTestRange(&m_pConstraints,uStart,uStop);

    return SetTestRange(&m_pRangeList,uStart,uStop);
}

// --------------------------------------------------------------------------------------

bool CTest::SkipTestRange(UINT uStart, UINT uStop, bool bConstraints)
{
    if (bConstraints)
        return SkipTestRange(&m_pConstraints,uStart,uStop);

    return SkipTestRange(&m_pRangeList,uStart,uStop);
}

// --------------------------------------------------------------------------------------

bool CTest::SkipTests(UINT uCount)
{
    UINT uStart = m_uTestNumber;

    uCount--;

    UINT uStop = m_uTestNumber + uCount;

    return SkipTestRange(uStart,uStop);
}

// --------------------------------------------------------------------------------------

void CTest::ClearRangeList()
{
	m_uTotalTests = 0;

    // Release the test range list
    while (NULL != m_pRangeList)
    {
        PTESTRANGE pHold = m_pRangeList->pNext;

        delete m_pRangeList;
        m_pRangeList = pHold;
    }
}

// --------------------------------------------------------------------------------------

void CTest::NextTest()
{
	m_uTestNumber = FindNextTest();
}

// --------------------------------------------------------------------------------------

UINT CTest::GetRemainingTests()
{
	PTESTRANGE pHold = m_pRangeList;
	UINT nCount = 0;

    // Release the test range list
    while (NULL != pHold)
    {
		nCount += pHold->uStop - pHold->uStart + 1;
		pHold = pHold->pNext;
	}

	return nCount;
}

// --------------------------------------------------------------------------------------

LRESULT CTest::OnMouse(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

// --------------------------------------------------------------------------------------

LRESULT CTest::OnKeydown(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

// --------------------------------------------------------------------------------------

LRESULT CTest::OnKeyup(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

// --------------------------------------------------------------------------------------

LRESULT CTest::OnCommand(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}

// --------------------------------------------------------------------------------------

LRESULT	CTest::OnSetCursor(WPARAM wParam, LPARAM lParam)
{
	return FALSE;
}
