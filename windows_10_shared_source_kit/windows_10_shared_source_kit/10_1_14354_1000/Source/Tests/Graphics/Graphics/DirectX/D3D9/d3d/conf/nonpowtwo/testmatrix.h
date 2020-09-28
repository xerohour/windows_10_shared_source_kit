#ifndef __TESTMATRIX
#define __TESTMATRIX


#include "TestVariable.h"


/*A test matrix is an abstract container class which holds TestVariables
and puts them into different combinations*/

class CTestMatrix
{
public:
	CTestMatrix()
	{
		m_pDeletionCallBack = NULL;
	}

	virtual ~CTestMatrix()
	{
		Cleanup();
	}

	inline void RegisterDeletionCallback(void (*pCallBack)(CTestVariable*))
	{
		m_pDeletionCallBack = pCallBack;
	}


	//deallocate all resources held by this matrix and call m_pDeletionCallBack() on every test variable which was added
	virtual void Cleanup() 
	{
	}

	//return the total number of combinations this variable can be in
	virtual void SetCombinationRange(CD3DTest* pTest, const UINT uRepeat = 1) = 0;

	//add a test variable to the matrix
	virtual bool AddTestVariable(CTestVariable* pVariable, bool bPermutation) = 0;

	//call CTestVariable::Initialize() on all test variables which have been added thus far
	virtual bool InitializeTestVariables(CD3DTest* pTest) = 0;

	//setup a specific test case by putting pTest in a certain state.  
	//nCombination starts counting at 0
	virtual bool SetupCombination(TCHAR *testName, const UINT nCombination, CD3DTest* pTest) = 0;

	//Call pTest->WriteStatus for the current state.
	virtual void UpdateStatus(CD3DTest* pTest) = 0;

protected:
	void (*m_pDeletionCallBack)(CTestVariable*);

};


#endif