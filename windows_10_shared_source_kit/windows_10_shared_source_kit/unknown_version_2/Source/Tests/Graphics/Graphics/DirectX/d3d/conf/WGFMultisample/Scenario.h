//////////////////////////////////////////////////////////////////////////
//  Scenario.h
//  created:	2006/09/15
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "multisample.h"

enum SCENARIO
{
	Array_Resolve,
	MRT_Resolve,
	Re_Resolve,
	Load_Resolve,
	Resolve_Load,
	Fullscreen_Resolve,
	Fullscreen_Present_Resolve,
	Fullscreen_Resolve_Present,
	Fullscreen_Present_Load,
	Fullscreen_Load_Present,
	
	Cast_Resolve,
	NonPow2_Resolve,
};

class CMultisampleTest_Scenario : public CMultisampleTest
{
public:
	CMultisampleTest_Scenario() 
		: CMultisampleTest(),
		m_uSampleIndex( 0 ),
		m_bUseSwapChain(true),
		m_pRTTexture2DCopy(NULL),
		m_pRTTexture2DResolve(NULL),
		m_pGS(NULL)
	{
	}

	void InitTestParameters();

protected:
	TEST_RESULT PreSetupTestCase();
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	BOOL Validate( BOOL bResult );
	void CleanupTestCase();

	ID3D11Texture2D			*m_pRTTexture2DCopy, *m_pRTTexture2DResolve;
	ID3D11GeometryShader		*m_pGS;

	UINT				m_uSampleIndex;
	SCENARIO			m_Scenario;

	bool				m_bUseSwapChain;
};