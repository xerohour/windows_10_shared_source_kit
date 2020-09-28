/*Header for a non-power of 2 texture stress test
Blake Pelton*/

#ifndef __NONPOWTWOSTRESSTEST_H
#define __NONPOWTWOSTRESSTEST_H

#include "NonPowTwoTest.h"
#include "DisplacementTestMatrix.h"
#include "SphereTree.h"


class CNonPowTwoStressTest : public CNonPowTwoTest
{
public:
	CNonPowTwoStressTest();
	virtual ~CNonPowTwoStressTest();

	virtual bool	ExecuteTest(UINT nTestNum);   

    virtual void Cleanup();

protected:
    bool    EnableLighting();


	//Returns true if only those tests which the device caps supports should be run
	virtual bool ShouldCapsCheck();

	//Returns a pointer to the test matrix to be used by this test
	virtual CTestMatrix* GetTestMatrix();

	//Call pTestMatrix->SetupCombination() based on nTestNum
	virtual bool SetupTestState(TCHAR *testName, CTestMatrix* pTestMatrix,  const UINT nTestNum);

	//Allocate all geometry related resources
	virtual bool InitGeometry();

	//Render the scene.  This function will be called in the middle of a BeginScene() / EndScene() block.
	virtual bool RenderScene();

	//Return the number of test cases to be run using pTestMatrix.
	virtual void SetTestCaseRange(CTestMatrix* pTestMatrix);


private:
	CDisplacementTestMatrix	m_TestMatrix;

	CSphereTree				m_SphereTree;

	float					m_fTime;
};



void InitRotVelCallBack(CSphereTree* pTree);

void VertexChanger(const D3DVERTEX& In, void* pOut);

#endif