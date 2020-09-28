//////////////////////////////////////////////////////////////////////////
//  Resolve.h
//  created:	2007/07/15
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "multisample.h"
	
class CResolveTest : public CMultisampleTest
{
public:
	CResolveTest() : CMultisampleTest(),
		m_pRTStaging(nullptr),
		m_pRTResolve(nullptr),
		m_ResolveFormat(DXGI_FORMAT_UNKNOWN),
		m_DstFormat(DXGI_FORMAT_UNKNOWN)
	{}
	void InitTestParameters();
protected:
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();

	DXGI_FORMAT m_ResolveFormat;
	DXGI_FORMAT m_DstFormat;
	ID3D11Texture2D  *m_pRTResolve;
	ID3D11Texture2D  *m_pRTStaging;
};

