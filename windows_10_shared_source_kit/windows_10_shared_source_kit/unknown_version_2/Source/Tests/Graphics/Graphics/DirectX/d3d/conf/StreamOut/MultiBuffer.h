// ***************************************************************
//  MultiBuffer   version:  1.0   ·  date: 01/24/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include "StreamOut.h"

//////////////////////////////////////////////////////////////////////////
// CStreamOutMultiBuffer
//////////////////////////////////////////////////////////////////////////
class CStreamOutMultiBuffer : public CStreamOutTest
{
public:
	struct GSOUT
	{
		float pos[4];
		UINT data_a[4];
		UINT data_b[4];
		UINT data_c[4];
		UINT data_d[4];
	};

	struct CONSTDATA
	{
		UINT gsInnerLoopCount[4];
		UINT drawIndex[4];
	};

protected:
	UINT m_numInputPrimitives;
	UINT m_gsInnerLoopCount;
	UINT m_numDrawCalls;
	UINT m_verticesPerPrimitive;

public:
	CStreamOutMultiBuffer();
	
protected:
	void InitTestParameters();

	TEST_RESULT Setup();
	void Cleanup();
	
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();
	TEST_RESULT ExecuteTestCase();
	
	void GetCurSODecl( D3D11_SO_DECLARATION_ENTRY **soDecl, UINT *numEntries );
	void CalculateVSOut( GSIN *dest );
	void CalculateGSOut( const GSIN *in, GSOUT *out, UINT drawIndex, UINT numInputVertsPerInvocation, UINT outputPrimitiveCount );

	TEST_RESULT GetInputVertexData( UINT* pCountOut, VSIN** pVertexDataOut, UINT* pInputVertsPerInvocationOut );
	UINT GetInputVertexCount( UINT numPrimitives );
	void GetVertexIndices(UINT primitiveIdx, UINT primitiveCount, UINT indicesOut[32]);
};
