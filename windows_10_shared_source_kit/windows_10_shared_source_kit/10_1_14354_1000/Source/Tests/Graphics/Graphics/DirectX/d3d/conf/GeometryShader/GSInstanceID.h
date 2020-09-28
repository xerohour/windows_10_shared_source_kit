#pragma once

// ***************************************************************
//  GSInstanceID   version:  1.0   ·  date: 03/05/2009
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

// Includes
#include "geometryshader.h"

class CGSInstanceIDTest :
	public CGSConfTest
{
	// Member variables
	ID3D11Buffer *m_pStreamOutputBuffer;
	ID3D11Buffer *m_pStreamOutputBufferCopy;

	UINT m_numInputPrimitives;
	UINT m_numInstances;
	GS_INPUT_PRIM_TYPE m_inputPrimType;
	
public:
	CGSInstanceIDTest(void);
	~CGSInstanceIDTest(void);

	TEST_RESULT Setup();
	void InitTestParameters();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
};

extern tstring ToString(const GS_INPUT_PRIM_TYPE &type);
extern bool FromString(GS_INPUT_PRIM_TYPE*, const tstring &str);
