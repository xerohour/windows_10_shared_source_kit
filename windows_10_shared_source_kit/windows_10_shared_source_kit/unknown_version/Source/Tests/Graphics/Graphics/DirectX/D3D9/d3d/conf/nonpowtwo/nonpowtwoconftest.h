/*Header for a non-power of 2 texture conformance test
Blake Pelton*/

#ifndef __NONPOWTWOCONFTEST_H
#define __NONPOWTWOCONFTEST_H

#include "NonPowTwoTest.h"
#include "DisplacementTestMatrix.h"
//#include "CBuffers.h"

#define ROW_COUNT 6

class CNonPowTwoConfTest : public CNonPowTwoTest
{
public:
	CNonPowTwoConfTest();
	virtual ~CNonPowTwoConfTest();

	virtual void Cleanup();

protected:
	//Returns true if only those tests which the device caps supports should be run
	virtual bool ShouldCapsCheck();

	//Returns a pointer to the test matrix to be used by this test
	virtual CTestMatrix* GetTestMatrix();

	//Call pTestMatrix->SetupCombination() based on nTestNum
	virtual bool SetupTestState(TCHAR *testName, CTestMatrix* pTestMatrix, const UINT nTestNum);

	//Allocate all geometry related resources
	virtual bool InitGeometry();

	void TransformVertex(D3DXMATRIX mTransform, D3DXVECTOR3 *V_3D, D3DXVECTOR4 *V_4D, UINT Size);

	//Render the scene.  This function will be called in the middle of a BeginScene() / EndScene() block.
	virtual bool RenderScene();

	//Return the number of test cases to be run using pTestMatrix.
	virtual void SetTestCaseRange(CTestMatrix* pTestMatrix);

private:
	CDisplacementTestMatrix	m_TestMatrix;

	TLVERTEX*			    m_pTLVertices;
	VERTEX*			        m_pVertices;
	unsigned short int*		m_pIndices;
	TLVERTEX*		        m_pTLVertices1; //used for wrap mode cases only
	VERTEX*			        m_pVertices1  ; //used for wrap mode cases only
	unsigned short int*		m_pIndices1   ; //used for wrap mode cases only
	
	UINT					m_nVertices;
	UINT					m_nIndices;
	UINT					m_nVertices1;  //used for wrap mode cases only
	UINT					m_nIndices1;   //used for wrap mode cases only
};

#endif