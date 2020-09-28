#include "DisplacementTestMatrix.h"
#include "testcasestruct.h"

UINT g_uTexCaseIndex;  //extern variable from testcasestruct.h

CDisplacementTestMatrix::CDisplacementTestMatrix()
{
	m_pDeviantVariables     = NULL;
	m_pPermutationVariables = NULL;

	m_pCurrentDeviantVariable     = NULL;
	m_pCurrentPermutationVariable = NULL;
	
	m_uDeviantTestCount     = 0;
	m_uPermutationTestCount = 0;
}

//------------------------------------------------------------------------------

CDisplacementTestMatrix::~CDisplacementTestMatrix()
{
}

//------------------------------------------------------------------------------

void CDisplacementTestMatrix::Cleanup()
{
	//Delete all permutation variables
	SDisplacementMatrixNode* pCurrent = m_pPermutationVariables;

	while(NULL != pCurrent)
	{
		if(NULL != m_pDeletionCallBack)
		{
			SDisplacementMatrixNode* pNext = pCurrent->pNext;

			m_pDeletionCallBack(pCurrent->pVariable);
			delete pCurrent;

			pCurrent = pNext;
		}
	}

	m_pPermutationVariables = NULL;

	//delete all deviant variables
	pCurrent = m_pDeviantVariables;

	while(NULL != pCurrent)
	{
		if(NULL != m_pDeletionCallBack)
		{
			SDisplacementMatrixNode* pNext = pCurrent->pNext;

			m_pDeletionCallBack(pCurrent->pVariable);
			delete pCurrent;

			pCurrent = pNext;
		}
	}

	m_pDeviantVariables = NULL;

	m_uDeviantTestCount     = 0;
	m_uPermutationTestCount = 0;
}

//------------------------------------------------------------------------------

bool CDisplacementTestMatrix::AddTestVariable(CTestVariable* pVariable, bool bPermutation)
{
	if(NULL == pVariable)
	{
		return false;
	}

	SDisplacementMatrixNode* pNewNode = new SDisplacementMatrixNode;
	if(NULL == pNewNode)
	{
		return false;
	}

	pNewNode->pVariable = pVariable;

	if(bPermutation)
	{
		pNewNode->pNext = m_pPermutationVariables;
		m_pPermutationVariables = pNewNode;
	}
	else
	{
		pNewNode->pNext = m_pDeviantVariables;
		m_pDeviantVariables = pNewNode;

		//m_nDeviantVariables++;
	}

	return true;
}

//------------------------------------------------------------------------------

bool CDisplacementTestMatrix::InitializeTestVariables(CD3DTest* pTest)
{
	SDisplacementMatrixNode* pCurrent = m_pDeviantVariables;

	bool bResult = true;

	while(NULL != pCurrent)
	{
		bResult &= pCurrent->pVariable->Initialize(pTest);
		m_uDeviantTestCount += pCurrent->pVariable->GetVariationCount();

		pCurrent = pCurrent->pNext;
	}

	pCurrent = m_pPermutationVariables;

	while(NULL != pCurrent)
	{
		bResult &= pCurrent->pVariable->Initialize(pTest);
		m_uPermutationTestCount += pCurrent->pVariable->GetVariationCount();

		pCurrent = pCurrent->pNext;
	}

	return bResult;
}

//------------------------------------------------------------------------------

void CDisplacementTestMatrix::SetCombinationRange(CD3DTest* pTest, UINT uRepeat)
{
	SDisplacementMatrixNode* pDeviantCurrent     = NULL;
	SDisplacementMatrixNode* pPermutationCurrent = NULL;
	UINT                     uDeviantCount       = 0;
	UINT                     uPermutationCount   = 0;
	UINT                     uStartTest          = 1;
	UINT                     uFinalTest          = 0;

	ASSERT( NULL != pTest);

	//insert valid cases to test range list 
	pPermutationCurrent = m_pPermutationVariables;

	while (NULL != pPermutationCurrent)
	{
		uPermutationCount = pPermutationCurrent->pVariable->GetVariationCount();
		
		for (UINT iP = 0; iP < uPermutationCount; iP++)
		{
  			if (!pPermutationCurrent->pVariable->CheckRequiredCaps(0, iP, pTest) ) //1nd parameter is not used in this case
			{
				if (uStartTest <= uFinalTest)
				{
					pTest->AddTestRange(uStartTest, uFinalTest);
				}
				uFinalTest += m_uDeviantTestCount * uRepeat;
				uStartTest  = uFinalTest + 1;
				
				continue;
			}

			pDeviantCurrent = m_pDeviantVariables;

			while (NULL != pDeviantCurrent)
			{
				uDeviantCount = pDeviantCurrent->pVariable->GetVariationCount();

				for (UINT iD = 0; iD < uDeviantCount; iD++)
				{
					if ( !pDeviantCurrent->pVariable->CheckRequiredCaps(iP, iD, pTest) )
					{
						if (uStartTest <= uFinalTest)
						{
							pTest->AddTestRange(uStartTest, uFinalTest);
						}
						uStartTest = uFinalTest + uRepeat + 1;
	 				} 
					uFinalTest += uRepeat;
				}

				pDeviantCurrent = pDeviantCurrent->pNext;
			}
		}

		pPermutationCurrent = pPermutationCurrent->pNext;
	}

	//add the rest of tests cases
	if (uStartTest <= uFinalTest)
		pTest->AddTestRange(uStartTest, uFinalTest);

}

//------------------------------------------------------------------------------

void CDisplacementTestMatrix::UpdateStatus(CD3DTest* pTest)
{
	if(NULL == pTest)
	{
		return;
	}

	if ( NULL != m_pCurrentDeviantVariable)
		m_pCurrentDeviantVariable->UpdateStatus(pTest);

	if ( NULL != m_pCurrentPermutationVariable)
		m_pCurrentPermutationVariable->UpdateStatus(pTest);

}

//------------------------------------------------------------------------------

bool CDisplacementTestMatrix::SetupCombination(TCHAR *testName, UINT nCombination, CD3DTest* pTest)
{
	if(NULL == pTest)
	{
		return false;
	}

	UINT uPermutationIndex = 0; 
	
	nCombination = nCombination % (m_uDeviantTestCount * m_uPermutationTestCount);

	if(!SetupPermutations(testName, pTest, nCombination))
	{
		return false;
	}

	if(!SetupDeviantVariables(testName, pTest, nCombination))
	{
		return false;
	}

	return true;
}

//------------------------------------------------------------------------------

bool CDisplacementTestMatrix::SetupPermutations(TCHAR *testName, CD3DTest* pTest, const UINT nCombination)
{
	if(NULL == pTest)
	{
		return false;
	}

	SDisplacementMatrixNode* pCurrent = m_pPermutationVariables;

	bool bResult           = true;
	UINT uPermutationIndex = 0;
	UINT uPermutationCount = 0;

	g_uTexCaseIndex = 0;

	if(NULL == m_pDeviantVariables)
		uPermutationIndex = nCombination;
	else
		uPermutationIndex = nCombination / m_uDeviantTestCount;

	while(NULL != pCurrent)
	{
		uPermutationCount = pCurrent->pVariable->GetVariationCount();

		if (uPermutationIndex < uPermutationCount)
		{
			bResult &= pCurrent->pVariable->Execute(uPermutationIndex, pTest);
			pCurrent->pVariable->ExpendTestName(testName);
			m_pCurrentPermutationVariable = pCurrent->pVariable;
			g_uTexCaseIndex = uPermutationIndex; 
			//m_uCurrentPermutationIndex = uPermutationIndex;
			break;
		}
		else
			uPermutationIndex -= uPermutationCount;
		
		pCurrent = pCurrent->pNext;
	}

	return bResult;
}

//------------------------------------------------------------------------------

bool CDisplacementTestMatrix::SetupDeviantVariables(TCHAR *testName, CD3DTest* pTest, const UINT nCount)
{
	if(NULL == pTest)
	{
		return false;
	}

	bool bResult         = true;
	UINT uVariationCount = 0;
	UINT uDeviantIndex   = nCount % m_uDeviantTestCount;

	SDisplacementMatrixNode* pCurrent = m_pDeviantVariables;

	while(NULL != pCurrent)
	{
		uVariationCount = pCurrent->pVariable->GetVariationCount();

		if (uDeviantIndex < uVariationCount)
		{
			bResult &= pCurrent->pVariable->Execute(uDeviantIndex, pTest);
			pCurrent->pVariable->ExpendTestName(testName);
			m_pCurrentDeviantVariable = pCurrent->pVariable;
			pCurrent = pCurrent->pNext;
			break;
		}
		else
		{
			uDeviantIndex -= uVariationCount;
		}
		
		pCurrent = pCurrent->pNext;
	}

	return bResult;
}

