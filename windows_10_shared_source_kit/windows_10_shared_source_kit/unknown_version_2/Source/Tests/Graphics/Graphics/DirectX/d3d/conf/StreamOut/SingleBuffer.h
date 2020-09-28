// ***************************************************************
//  SingleBuffer   version:  1.0   ·  date: 01/24/2006
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
// CStreamOutSingleBuffer
//////////////////////////////////////////////////////////////////////////
class CStreamOutSingleBuffer : public CStreamOutTest
{
public:
	struct GSOUT
	{
		float pos[4];
		UINT data[1][2];
	};
	
	// 8-Component output
	struct GSOUT8
	{
		float pos[4];
		UINT data[2][2];
	};

	// 32-component output
	struct GSOUT32
	{
		float pos[4];
		UINT data[14][2];
	};

	// 64-component output
	struct GSOUT64
	{
		float pos[4];
		UINT data[30][2];
	};

	// 128-component output
	struct GSOUT128
	{
		float pos[4];
		UINT data[62][2];
		// the real data is [31][4] -- but that doesn't fit with the current design.
	};

	struct CONSTDATA
	{
		UINT numPrimsPerInputVertex[4];
		UINT drawIndex[4];
	};


protected:
	UINT     m_numInputVerts;
	UINT     m_numPrimsPerInputVertex;
	UINT     m_numDrawCalls;
	UINT     m_soNumVertexComponents;
	bool     m_forceRebindEachDraw;
	UINT     m_outputSlot;
	UINT     m_bufferSize;
	UINT     m_bufferOffset;
	tstring  m_extraBindAttributes;
	UINT     m_soVertexStride;

public:	
	CStreamOutSingleBuffer();

protected:
	void InitTestParameters();
	TEST_RESULT SetupStreamOut();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	TEST_RESULT Setup();
	void Cleanup();

	TEST_RESULT TestTriangleList( UINT numExpectedPrimitives, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT gsMaxPrimitiveCount, UINT numDataElements, UINT componentCount = 2 );
	TEST_RESULT TestLineList( UINT numExpectedPrimitives, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT gsMaxPrimitiveCount, UINT numDataElements, UINT componentCount = 2 );
	TEST_RESULT TestPointList( UINT numExpectedPrimitives, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT gsMaxPrimitiveCount, UINT numDataElements, UINT componentCount = 2 );

	TEST_RESULT TestTriangleListNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements );
	TEST_RESULT TestTriangleListAdjNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements );
	TEST_RESULT TestTriangleStripNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements );
	TEST_RESULT TestTriangleStripAdjNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements );
	TEST_RESULT TestLineListNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements );
	TEST_RESULT TestLineListAdjNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements );
	TEST_RESULT TestLineStripNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements );
	TEST_RESULT TestLineStripAdjNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements );
	TEST_RESULT TestPointListNoGS( UINT numPrimsPerDraw, VSIN *pVertexIn, GSOUT *pGSOutData, void *pSrcSOVerts, UINT numDataElements );

	// Helpers
	virtual void GetCurSODecl( std::vector< D3D11_SO_DECLARATION_ENTRY > *soDecl );
	TEST_RESULT CompareSOVertex( GSOUT *refVert, GSOUT *srcVert, UINT numDataElements, UINT drawIndex, UINT vertID, UINT primID, UINT primVertID, void *pBeginBuffer, void *pEndBuffer, UINT componentCount = 2 );
};


