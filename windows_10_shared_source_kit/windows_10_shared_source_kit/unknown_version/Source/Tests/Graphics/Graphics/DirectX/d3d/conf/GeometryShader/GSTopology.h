// ***************************************************************
//  GSTopology   version:  1.0   ·  date: 03/27/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include "GeometryShader.h"

class CGSTopologyTest : public CGSConfTest
{
	D3D11_PRIMITIVE_TOPOLOGY m_topology;
	bool m_bUseGS;
	bool m_bStreamOut;
	bool m_bRasterize;
	UINT m_numIndices;
	UINT m_cutInterval;
	
	ID3D11Buffer *m_pSOBuffer;
	ID3D11Buffer *m_pInputVertsPoint;
	ID3D11Buffer *m_pInputVertsLine;
	ID3D11Buffer *m_pInputVertsLineAdj;
	ID3D11Buffer *m_pInputVertsTriangle;
	ID3D11Buffer *m_pInputVertsTriangleAdj;
	ID3D11Buffer *m_pInputIndices;
	ID3D11Buffer *m_pCBAdjacencyTable;
	
	ID3D11VertexShader *m_pVSPoint;
	ID3D11VertexShader *m_pVSLine;
	ID3D11VertexShader *m_pVSTriangle;
	ID3D11VertexShader *m_pVSTriangleAdj;
	
	// array contains {with,without,vsso} streamout
	ID3D11GeometryShader *m_pGSPoint[3];
	ID3D11GeometryShader *m_pGSLine[3];
	ID3D11GeometryShader *m_pGSTriangle[3];
	ID3D11GeometryShader *m_pGSLineAdj[3];
	ID3D11GeometryShader *m_pGSTriangleAdj[3];
	
public:
	CGSTopologyTest();
	~CGSTopologyTest();
	
	TEST_RESULT Setup();
	void Cleanup();
	
	TEST_RESULT ExecuteTestCase();
	
	void InitTestParameters();
};