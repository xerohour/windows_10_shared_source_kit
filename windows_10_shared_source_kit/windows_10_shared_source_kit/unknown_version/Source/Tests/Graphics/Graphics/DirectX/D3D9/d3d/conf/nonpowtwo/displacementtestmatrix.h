/*A displacement test matrix puts all variables into their "default" state (#0)
execpt for one variable which goes into a deviant state.
This matrix test count grows linearly with the number of test cases added*/

#ifndef __DISPLACEMENT_TEST_MATRIX_H
#define __DISPLACEMENT_TEST_MATRIX_H

#include "TestMatrix.h"

//linked list node structure
struct SDisplacementMatrixNode
{
	CTestVariable* pVariable;
	SDisplacementMatrixNode* pNext;
};

class CDisplacementTestMatrix : public CTestMatrix
{
public:
	CDisplacementTestMatrix();

	virtual ~CDisplacementTestMatrix();

	//deallocate all resources held by this matrix and call m_pDeletionCallBack() on every test variable which was added
	virtual void Cleanup();

	//return the total number of combinations this variable can be in
	virtual void SetCombinationRange(CD3DTest* pTest, const UINT uRepeat = 1);

	//add a test variable to the matrix
	virtual bool AddTestVariable(CTestVariable* pVariable, bool bPermutation);

	//call CTestVariable::Initialize() on all test variables which have been added thus far
	virtual bool InitializeTestVariables(CD3DTest* pTest);

	//setup a specific test case by putting pTest in a certain state.  
	virtual bool SetupCombination(TCHAR *testName, const UINT nCombination, CD3DTest* pTest);


	//Call UpdateStatus on all test variables
	virtual void UpdateStatus(CD3DTest* pTest);

private:
	bool	SetupPermutations	 (TCHAR *testName, CD3DTest* pTest, const UINT nCount);
	bool	SetupDeviantVariables(TCHAR *testName, CD3DTest* pTest, const UINT nCount);

private:
	SDisplacementMatrixNode*	m_pDeviantVariables;
	SDisplacementMatrixNode*	m_pPermutationVariables;

	CTestVariable*              m_pCurrentDeviantVariable;
	CTestVariable*              m_pCurrentPermutationVariable;

	UINT						m_uDeviantTestCount;
	UINT					    m_uPermutationTestCount;
};

#endif