//////////////////////////////////////////////////////////////////////////
//  state.h
//  created:	2006/11/10
//
//  purpose:    conformance for depth parameters in the depth/stencil state
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "depth.h"

class CStateTest : public CDepthTest
{
public:
	CStateTest();

protected:
	bool m_ulpLimits;

private:
	virtual void InitTestParameters();
	TEST_RESULT Setup();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	void Cleanup();

	ID3D11DepthStencilState	*m_pDSSAlways;
};

// Verifies that RO-stencil does not affect depth
class CStateStencilROTest : public CStateTest
{
private:
	void InitTestParameters();
};

class CStateLimitsTest : public CD3D11Test
{
public:
	CStateLimitsTest()
		: m_pDevice( NULL )
		, m_numDevices( 0 )
	{}
	virtual TEST_RESULT Setup();
	virtual void Cleanup( );

	virtual void InitTestParameters();
	virtual TEST_RESULT ExecuteTestCase();

protected:
	ID3D11Device*	m_pDevice;
	UINT		m_numDevices;
};
