//////////////////////////////////////////////////////////////////////////
//  Fill.h
//  created:	2005/07/18
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "multisample.h"
	
class CMultisampleTest_Fill : public CMultisampleTest
{
public:
	CMultisampleTest_Fill() : CMultisampleTest() {}

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupSampleFinder();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();

	virtual void Draw( float x, float y ) = 0;
	virtual TEST_RESULT Validate(BOOL result) = 0;
	virtual void LogInfo() = 0;
	
	UINT m_uSampleIndex;
	bool m_bOrder;
};

class CMultisampleTest_FillTriangle : public CMultisampleTest_Fill
{
public:
	CMultisampleTest_FillTriangle() : CMultisampleTest_Fill() {}
	void InitTestParameters();

protected:
	void Draw( float x, float y );
	TEST_RESULT Validate(BOOL bResult);
	void LogInfo();

	UINT m_TriangleID;
};

class CMultisampleTest_FillPoint : public CMultisampleTest_Fill
{
public:
	CMultisampleTest_FillPoint() : CMultisampleTest_Fill() {}
	void InitTestParameters();

protected:
	void Draw( float x, float y );
	TEST_RESULT Validate(BOOL bResult);
	void LogInfo();

	UINT m_PointID;
};
	
//TODO
class CMultisampleTest_FillMSLine : public CMultisampleTest_Fill
{
public:
	CMultisampleTest_FillMSLine() : CMultisampleTest_Fill() {}
	void InitTestParameters() {};

protected:
	void Draw( float x, float y ) {}
	TEST_RESULT Validate(BOOL bResult) { return RESULT_PASS; }
};