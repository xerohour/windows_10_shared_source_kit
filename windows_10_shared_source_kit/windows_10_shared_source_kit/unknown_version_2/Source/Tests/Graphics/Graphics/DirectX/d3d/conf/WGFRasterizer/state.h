//////////////////////////////////////////////////////////////////////////
//  State.h
//  created:	2006/07/17
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "rasterizer.h"

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
