//////////////////////////////////////////////////////////////////////////
//  Topology.h
//  created:	2005/03/10
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "Shader5x.h"
const UINT MAX_SHADERSTR = 4000;

class CShader5xTest_Topology : public CShader5xTest
{
public:
	enum OUTPUT_TOPOLOGY
	{
		OT_POINTLIST,
		OT_LINESTRIP,
		OT_TRIANGLESTRIP,
	};
	
	OUTPUT_TOPOLOGY m_OutputTopology;
	UINT m_nEmitCount;
	UINT m_nCutEvery;
	UINT m_gsOutputComponentCount;
	
	CShader5xTest_Topology() :
		m_pVertexBuffer( NULL ),
		m_pInputLayout( NULL ),
		m_pRS( NULL ),
		m_nCutEvery( 0 ),
		m_bStreamsDeclared( false ),
		m_stream( 0 ),
		m_numSOQueries( 1 )
		{
		    m_pSOStats[0] = NULL;
		    m_pSOStats[1] = NULL;
		    m_pSOStats[2] = NULL;
		    m_pSOStats[3] = NULL;
        }
	
	TEST_RESULT BuildGS();

	TEST_RESULT Setup();
	void Cleanup();
	
	virtual const char *GetGSSource() = 0;

	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();

	TCHAR m_szGSSourceShader[ MAX_SHADERSTR ];
	UINT m_stream;
	UINT m_numSOQueries;
	bool m_bStreamsDeclared;


	ID3D11Buffer			*m_pVertexBuffer;
	ID3D11InputLayout		*m_pInputLayout;
	ID3D11RasterizerState	*m_pRS;
	ID3D11Query				*m_pSOStats[4];
	const char				*m_szGSSource;
};

class CShader5xTest_TopologyStream : public CShader5xTest_Topology
{
public:
	virtual TEST_RESULT Setup();
};

class CShader5xTest_emit : public CShader5xTest_Topology
{
public:
	CShader5xTest_emit();
	
	void InitTestParameters();
	virtual const char *GetGSSource();
};

class CShader5xTest_cut  : public CShader5xTest_Topology
{
public:
	CShader5xTest_cut();
	
	void InitTestParameters();
	TEST_RESULT BuildGS();
	virtual const char *GetGSSource();
};

class CShader5xTest_emitThenCut : public CShader5xTest_Topology
{
public:
	CShader5xTest_emitThenCut();
	
	void InitTestParameters();
	TEST_RESULT BuildGS();
	virtual const char *GetGSSource();
};

class CShader5xTest_emitStream : public CShader5xTest_TopologyStream
{
public:
	CShader5xTest_emitStream();

	void InitTestParameters();
	virtual const char *GetGSSource();
};

class CShader5xTest_cutStream  : public CShader5xTest_TopologyStream
{
public:
	CShader5xTest_cutStream();

	void InitTestParameters();
	TEST_RESULT BuildGS();
	virtual const char *GetGSSource();
};

class CShader5xTest_emitThenCutStream : public CShader5xTest_TopologyStream
{
public:
	CShader5xTest_emitThenCutStream();
	
	void InitTestParameters();
	TEST_RESULT BuildGS();
	virtual const char *GetGSSource();
};