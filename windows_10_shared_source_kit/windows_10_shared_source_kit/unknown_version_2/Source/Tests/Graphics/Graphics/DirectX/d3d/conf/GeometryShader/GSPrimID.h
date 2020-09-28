// ***************************************************************
//  GSPrimID   version:  1.0   ·  date: 02/23/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include "GeometryShader.h"

class CGSPrimIDTest : public CGSConfTest
{
	UINT m_numInputPrimitives;
	UINT m_numInstances;
	GS_INPUT_PRIM_TYPE m_inputPrimType;
	
public:
	CGSPrimIDTest();
	
	TEST_RESULT Setup();
	void Cleanup();
	
	void InitTestParameters();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
};