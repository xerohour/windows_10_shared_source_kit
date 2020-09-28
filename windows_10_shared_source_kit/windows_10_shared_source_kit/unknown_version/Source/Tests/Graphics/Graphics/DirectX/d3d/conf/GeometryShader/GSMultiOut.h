// ***************************************************************
//  GSMultiOut   version:  1.0   ·  date: 08/30/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include "GeometryShader.h"

enum eVertexSize
{
	VertSize_Tiny = 0,
	VertSize_Normal,
	VertSize_Huge,
};

DECLARE_NAMED_VALUES(eVertexSize)

class CGSMultiOutTest : public CGSConfTest
{
private:
	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	bool m_bStreamOut;
	bool m_bRasterize;
	UINT m_numIndices;
	eVertexSize m_VertexSize;
	UINT m_RestartFrequency;
	UINT m_numInstances;

	ID3D11Buffer *m_pSOBuffer;
	ID3D11Buffer *m_pInputVertsPoint;
	ID3D11Buffer *m_pInputVertsLine;
	ID3D11Buffer *m_pInputVertsTriangle;
	ID3D11Buffer *m_pInputIndices;

	// array contains {standard, tiny, huge} gs output declarations
	ID3D11GeometryShader *m_pGSPoint[3];
	ID3D11GeometryShader *m_pGSLine[3];
	ID3D11GeometryShader *m_pGSTriangle[3];
	ID3D11GeometryShader *m_pGSLineAdj[3];
	ID3D11GeometryShader *m_pGSTriangleAdj[3];

public:
	CGSMultiOutTest();
	~CGSMultiOutTest();

	TEST_RESULT Setup();
	void Cleanup();

	TEST_RESULT ExecuteTestCase();

	void InitTestParameters();
};