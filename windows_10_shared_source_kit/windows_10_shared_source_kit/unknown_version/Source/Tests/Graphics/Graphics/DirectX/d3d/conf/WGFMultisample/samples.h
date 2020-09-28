//////////////////////////////////////////////////////////////////////////
//  Samples.h
//  created:	2005/07/01
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "multisample.h"
	
class CCountTest : public CMultisampleTest
{
public:
	CCountTest() : CMultisampleTest() {}
	virtual void InitTestParameters();

protected:
	virtual TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();

	int m_nCounter;
};

class CSupportTest : public CMultisampleTest
{
public:
	CSupportTest() : CMultisampleTest() {}
	void InitTestParameters();
protected:
	TEST_RESULT Setup() { return RESULT_PASS; }
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase() = 0;
	void CleanupTestCase() {}
	void Cleanup() {}
};

class CSupport4xTest : public CSupportTest
{
public:
	CSupport4xTest() : CSupportTest() {}
protected:
	TEST_RESULT ExecuteTestCase();
};

class CSupport8xTest : public CSupportTest
{
public:
	CSupport8xTest() : CSupportTest() {}
protected:
	TEST_RESULT ExecuteTestCase();
};

// sample frequency interpolations
class CSampleTest : public CMultisampleTest
{
public:
	CSampleTest() : CMultisampleTest(), m_bTestSampleFreq( true ) {}
	void InitTestParameters();
protected:
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();

	ID3D11PixelShader *pPSLinear, *pPSSample;

	D3DXVECTOR2 m_vStart, m_vEnd;
	D3D11_PRIMITIVE_TOPOLOGY m_Topology;
	BOOL m_bTestSampleFreq;
};