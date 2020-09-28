#ifndef __TESTVARIABLE_H
#define __TESTVARIABLE_H

#include "testcasestruct.h"
/* A test variable is one single variable of a device's state.
For example, a test variable might be wether lighting is on or off*/

#include "CD3DTest.h"

//------------------------------------------------------------------------------

class CTestVariable
{
public:
	virtual ~CTestVariable() {}

	virtual void Cleanup() = 0;

	//Allocate memory and filter invalid device states based on Caps if bCapsCheck is true
	virtual bool Initialize(CD3DTest* pTest) = 0;

	//returns the total # of values this state can be in(after filtering values which aren't supported by the device)
	virtual UINT GetVariationCount() = 0;

	//put the test in the specified state.
	virtual bool Execute(const UINT nVariation, CD3DTest* pTest) = 0;
	
	//expend test name by name of the current state
	virtual void ExpendTestName(TCHAR *testName) = 0;
			
	//call pTest->WriteStatus() to inform the user of the current status
	virtual void UpdateStatus(CD3DTest* pTest) = 0;

	virtual bool CheckRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest) = 0;
};

//------------------------------------------------------------------------------

template<typename Type>
class CTestVariableImpl : public CTestVariable
{
public:
	CTestVariableImpl();
	virtual ~CTestVariableImpl();

	//Allocate memory and filter invalid device states based on Caps if bCapsCheck is true
	virtual bool Initialize(CD3DTest* pTest);

	virtual void Cleanup();

	//returns the total # of values this state can be in(after filtering values which aren't supported by the device)
	virtual UINT GetVariationCount();

	virtual bool CheckRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest);

	//expend test name by the name of the current state
	virtual void ExpendTestName(TCHAR *testName);

	//put the test in the specified state.
	virtual bool Execute(const UINT nVariation, CD3DTest* pTest);

	virtual void UpdateStatus(CD3DTest* pTest);

	//The following will be called by CTestVariable in response to public calls
protected:
	//Allocate any resources needed by this test variable.
	bool InitializeResources(CD3DTest* pTest);

	//Free all resources that were allocated by this test variable.
	void CleanupResources();

	/*Return the total number of values this variable can attain, (ignoring device caps)*/
	UINT GetUnfilteredDomainSize() const;

	/*Fill pOut with the information necessary to put the device into the state given by nIndex.
	  nIndex will be < GetDomainSize(); pOut will be != NULL */
	void GetVariation(Type* pOut, const UINT nIndex) const;

	bool CheckTestRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest);

	void ExpendTestStateName(TCHAR *testName);

	/*Call the correct functions on pTest to put the device into the state given by "State".
	  Return true the test was sucessfully put into the indicated state.
	  pTest is be != NULL*/
	bool PutTestIntoState(CD3DTest* pTest);

	//Call pTest->WriteStatus() with information from State
	void UpdateTestStatus(CD3DTest* pTest);

private:
	Type*		m_pDomain;//all filtered device states
	UINT		m_nDomain;
	Type		m_CurrentState;//cached in Execute, used in WriteStatus
};


//------------------------------------------------------------------------------

template<typename Type>
CTestVariableImpl<Type>::CTestVariableImpl()
{
	m_pDomain = NULL;
	m_nDomain = 0;
}

//------------------------------------------------------------------------------

template<typename Type>
CTestVariableImpl<Type>::~CTestVariableImpl()
{
	Cleanup();
}

//------------------------------------------------------------------------------

template<typename Type>
bool CTestVariableImpl<Type>::Initialize(CD3DTest* pTest)
{
	Cleanup();

	if(!InitializeResources(pTest))
	{
		return false;
	}

	//Figure out how many states are supported by the caps
	UINT uUnfilteredDomainSize = GetUnfilteredDomainSize();
	
	m_pDomain = new Type[uUnfilteredDomainSize];

	if(NULL == m_pDomain)
	{
		return false;
	}

	m_nDomain = uUnfilteredDomainSize;

	for(UINT i = 0; i < uUnfilteredDomainSize; i++)
	{
		GetVariation(&(m_pDomain[i]), i);
	}

	ZeroMemory(&m_CurrentState, sizeof(m_CurrentState));

	return true;
}

//------------------------------------------------------------------------------

template<typename Type>
void CTestVariableImpl<Type>::Cleanup()
{
	CleanupResources();

	delete[] m_pDomain;

	m_nDomain = 0;
}

//------------------------------------------------------------------------------

template<typename Type>
UINT CTestVariableImpl<Type>::GetVariationCount()
{
	return m_nDomain;
}

//------------------------------------------------------------------------------

template<typename Type>
bool CTestVariableImpl<Type>::CheckRequiredCaps(const UINT uPIndex, const UINT uDIndex, CD3DTest* pTest)
{
	return CheckTestRequiredCaps(uPIndex, uDIndex, pTest);
}

//------------------------------------------------------------------------------
 
template<typename Type>
void CTestVariableImpl<Type>::ExpendTestName(TCHAR *testName)
{
	ExpendTestStateName(testName);
}

//------------------------------------------------------------------------------
 
template<typename Type>
bool CTestVariableImpl<Type>::Execute(const UINT nVariation, CD3DTest* pTest)
{
	if(NULL == pTest)
	{
		return false;
	}

	if(nVariation >= m_nDomain)
	{
		return false;
	}

	const Type& State = m_pDomain[nVariation];

	memcpy(&m_CurrentState, &State, sizeof(Type));

	return PutTestIntoState(pTest);
}

//------------------------------------------------------------------------------

template<typename Type>
void CTestVariableImpl<Type>::UpdateStatus(CD3DTest* pTest)
{
	UpdateTestStatus(pTest);
}

#endif