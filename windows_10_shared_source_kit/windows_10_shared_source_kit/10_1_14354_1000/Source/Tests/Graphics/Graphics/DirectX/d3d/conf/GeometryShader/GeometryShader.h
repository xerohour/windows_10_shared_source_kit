// ***************************************************************
//  GSConf   version:  1.0   ·  date: 02/22/2006
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2006 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include <D3D11Test.h>

//////////////////////////////////////////////////////////////////////////

enum GS_INPUT_PRIM_TYPE
{
	PRIM_TYPE_POINT,
	PRIM_TYPE_LINE,
	PRIM_TYPE_LINE_ADJ,
	PRIM_TYPE_TRIANGLE,
	PRIM_TYPE_TRIANGLE_ADJ
};

struct CONST_DATA
{
	float vpSize[4];
};

TEST_RESULT TRLatch(TEST_RESULT& origVal, TEST_RESULT& newVal);

#define MEMBER_OFFSET( member, instance ) \
	(int) ( ptrdiff_t( &(instance).member ) - ptrdiff_t( &(instance) ) )

//////////////////////////////////////////////////////////////////////////

class CGSConfTest : public CD3D11Test
{
protected:
	ID3D11InputLayout *m_pInputLayout;
	ID3D11VertexShader *m_pVS;
	ID3D11GeometryShader *m_pGS;
	ID3D11PixelShader *m_pPS;
	ID3D11Buffer *m_pSOTarget;
	ID3D11Buffer *m_pVB;
	ID3D11Buffer *m_pCB;
	ID3D11RasterizerState *m_pRS;
	ID3D11DepthStencilState *m_pDSS;
	ID3D11Texture2D *m_pMemTexture;
	
public:
	CGSConfTest();
	
	int GetNumRenderWindows() const;
	bool CreateRTFromSwapChainProps() const { return true; }
	TEST_RESULT CommonSetup();
	void CommonCleanup();
	
//	TEST_RESULT SetupRenderTarget( UINT w, UINT h, UINT slices );

	TEST_RESULT SetupVSFromResource( LPCTSTR resourceID, LPCSTR entryProc, D3D11_INPUT_ELEMENT_DESC *iedesc, UINT numInputElements, ID3D11VertexShader **ppVS, ID3D11InputLayout **ppInputLayout, const D3D10_SHADER_MACRO* pDefines = NULL );
	TEST_RESULT SetupGSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11GeometryShader **ppGS, const D3D10_SHADER_MACRO* pDefines = NULL );
	TEST_RESULT SetupGSWithSOFromResource( LPCTSTR resourceID, LPCSTR entryProc, D3D11_SO_DECLARATION_ENTRY decl[], UINT numElements, UINT outputStreamStride, ID3D11GeometryShader **ppGS, const D3D10_SHADER_MACRO* pDefines = NULL );
	TEST_RESULT SetupPSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11PixelShader **ppPS );
};

//////////////////////////////////////////////////////////////////////////
// CGSConfTestApp
//////////////////////////////////////////////////////////////////////////

class CGSConfTestApp : public CD3D11TestFramework
{
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	void InitOptionVariables();
	bool InitTestGroups();
};

extern CGSConfTestApp g_App;
