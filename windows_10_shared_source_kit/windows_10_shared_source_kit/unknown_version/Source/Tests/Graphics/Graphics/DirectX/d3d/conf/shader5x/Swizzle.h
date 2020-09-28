//////////////////////////////////////////////////////////////////////////
//  Swizzle.h
//  created:	2005/05/11
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"

class CShader5xTest_MaskSwizzle : public CShader5xTest
{
public:
	CShader5xTest_MaskSwizzle() : CShader5xTest( DXGI_FORMAT_R32G32B32A32_UINT ) {}
	typedef DWORD ResultType[4];
	
	std::string m_strSwizzle;
	std::string m_strMask;
	
	DWORD m_dwData[4];
	
	void InitTestParameters();
	
	TEST_RESULT SetupTestCase();
	virtual TEST_RESULT Setup();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	
	TEST_RESULT BuildShaders();
	bool CompareResult( ResultType &result );

private:	
	DWORD m_dwValues[16];
};