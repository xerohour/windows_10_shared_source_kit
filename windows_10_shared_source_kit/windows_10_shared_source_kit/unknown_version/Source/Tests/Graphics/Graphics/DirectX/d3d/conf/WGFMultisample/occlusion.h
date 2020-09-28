//////////////////////////////////////////////////////////////////////////
//  Occlusion.h
//  created:	2005/07/18
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "multisample.h"
	
class CMultisampleTest_Occ : public CMultisampleTest
{
public:
	CMultisampleTest_Occ() 
		: CMultisampleTest()
		, m_pDSView( NULL )
		, m_pDSState( NULL )
		, m_bDS( false )
	{}
	void InitTestParameters();

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	TEST_RESULT SetupDS();
	void CleanupTestCase();
	
	ID3D11DepthStencilView  *m_pDSView;
	ID3D11DepthStencilState *m_pDSState;

	bool m_bDS;

	virtual TEST_RESULT Begin() = 0;
	virtual TEST_RESULT Validate() = 0;
};


class CMultisampleTest_OccQuery : public CMultisampleTest_Occ
{
protected:
	ID3D11Query* m_pQuery;

	TEST_RESULT Begin();
	TEST_RESULT Validate();
};

class CMultisampleTest_OccPredicate : public CMultisampleTest_Occ
{
protected:
	ID3D11Predicate* m_pPredicate;
	
	TEST_RESULT Begin();
	TEST_RESULT Validate();
};
