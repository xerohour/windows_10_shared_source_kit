// ***************************************************************
//  StreamOut   version:  1.0   ·  date: 12/16/2005
//  -------------------------------------------------------------
//  
//  -------------------------------------------------------------
//  Copyright (C) 2005 - All Rights Reserved
// ***************************************************************
// 
// ***************************************************************

#pragma once

#include <D3D11Test.h>

#define MAGIC_NUMBER 0xfef1f0fa

enum DECL_MODE
{
	DECL_MODE_SIMPLE,
	DECL_MODE_GAPS,
	DECL_MODE_SPLIT_ELEMENTS,
	DECL_MODE_SCRAMBLE_ELEMENTS,
	DECL_MODE_PARTIAL_ELEMENTS,
	DECL_MODE_SKIP_SLOT,
	DECL_MODE_SINGLE_SLOT, // Single buffer bound to non-zero-index slot
};

DECLARE_NAMED_VALUES( DECL_MODE );

enum SHADER_MODEL
{
	SHADER_MODEL_4_0,
	SHADER_MODEL_4_1,
	SHADER_MODEL_5_0,
};

DECLARE_NAMED_VALUES( SHADER_MODEL );

inline UINT Encode( UINT i ) { return i + 1; } //return ( i >> 1 ) ^ i; }

//////////////////////////////////////////////////////////////////////////
// A small cache of shaders that have been previously compiled
// Used to avoid compiling the same shader code many times
//////////////////////////////////////////////////////////////////////////
class CShaderCache
{
public:
	CShaderCache();
	~CShaderCache();

	HRESULT CompileFromResource(LPCSTR pSrcResource, 
								const D3D10_SHADER_MACRO* pDefines, 
								LPCSTR pFunctionName, 
								LPCSTR pProfile, 
								UINT flags1, 
								UINT flags2, 
								ID3D10Blob** ppBlob,
								ID3D10Blob** ppErrorMessages);

private:
	void Clear();

	static const UINT CACHE_SIZE = 32;

	struct KEY
	{
		std::string srcResource;
		std::vector< std::pair<std::string, std::string> > defines;
		std::string functionName;
		std::string profile;
		UINT flags1;
		UINT flags2;

		bool operator==(const KEY& rhs) const
		{
			return (srcResource == rhs.srcResource) &&
				   (defines == rhs.defines) &&
				   (functionName == rhs.functionName) &&
				   (profile == rhs.profile) &&
				   (flags1 == rhs.flags1) &&
				   (flags2 == rhs.flags2);
		}
	};

	struct
	{
		KEY         key;
		ID3D10Blob* pBlob;
		UINT        age;
	} _Cache[CACHE_SIZE];

	UINT m_Age;
};


//////////////////////////////////////////////////////////////////////////
// CStreamOutTest
//////////////////////////////////////////////////////////////////////////
class CStreamOutTest : public CD3D11Test
{
public:
	struct VSIN
	{
		float pos[5];
		UINT data;
	};

	struct GSIN
	{
		float pos[4];
		UINT data;
	};

protected:
	ID3D11RasterizerState *m_pRS;
	ID3D11DepthStencilState *m_pDSDisable;
	
	ID3D11PixelShader *m_pPS;
	ID3D11GeometryShader *m_pGS;
	ID3D11VertexShader *m_pVS;
	ID3D11InputLayout *m_pInputLayout;
	
	ID3D11Buffer *m_pVB;
	ID3D11Buffer *m_pCB;
	
	ID3D11Query *m_pSOStatsQuery;
	ID3D11Query *m_pSOOverflowPredicate;
	ID3D11Query *m_pPipelineStatsQuery;

	ID3D11Buffer *m_soBuffers[D3D11_SO_BUFFER_SLOT_COUNT];
	D3D11_BUFFER_DESC m_soBufferDesc[D3D11_SO_BUFFER_SLOT_COUNT];
	UINT m_soOffsets[D3D11_SO_BUFFER_SLOT_COUNT];
	
	UINT m_rasterizeStream;
	UINT m_soBufferSize[D3D11_SO_BUFFER_SLOT_COUNT];
	DECL_MODE m_declMode;
	UINT m_gsMaxVertexCount;
	D3D11_PRIMITIVE_TOPOLOGY m_inputTopology;
	tstring m_gsOutputPrimitiveType;
	bool m_useNullGS;
	bool m_haveRuntimeCalculateSOStrides;
	SHADER_MODEL m_sm;
		
	ID3D11VertexShader *m_pVSClear;
	ID3D11GeometryShader *m_pGSClear;

	CShaderCache m_shaderCache;

	static VSIN s_vbdataPoints[];
	static VSIN s_vbdataLineList[];
	static VSIN s_vbdataLineListAdj[];
	static VSIN s_vbdataLineStrip[];
	static VSIN s_vbdataLineStripAdj[];
	static VSIN s_vbdataTriangleList[];
	static VSIN s_vbdataTriangleListAdj[];
	static VSIN s_vbdataTriangleStrip[];
	static VSIN s_vbdataTriangleStripAdj[];
	static VSIN s_vbdata32CPPatchList[];
	static UINT s_pointsSize;
	static UINT s_lineListSize;
	static UINT s_lineListAdjSize;
	static UINT s_triangleListSize;
	static UINT s_triangleListAdjSize;
	static UINT s_lineStripSize;
	static UINT s_triangleStripSize;
	static UINT s_lineStripAdjSize;
	static UINT s_triangleStripAdjSize;
	static UINT s_32CPPatchListSize;

public:
	CStreamOutTest();

protected:	
	// Helper functions
	tstring GetShaderModelSuffix();
	TEST_RESULT CreateSOBuffers( ID3D11Buffer *SOBuffers[4], D3D11_BUFFER_DESC SOBufferDesc[4] );
	TEST_RESULT SetupVSFromResource( LPCTSTR resourceID, LPCSTR entryProc, D3D11_SO_DECLARATION_ENTRY *soDecl, UINT numSOElements, const D3D10_SHADER_MACRO *shaderDefines, const UINT* outputBufferStride, UINT numStrides, ID3D11VertexShader **ppVS, ID3D11InputLayout **ppInputLayout );
	TEST_RESULT SetupGSFromResource( LPCTSTR resourceID, LPCSTR entryProc, D3D11_SO_DECLARATION_ENTRY decl[], UINT numElements, const D3D10_SHADER_MACRO* pDefines, const UINT* outputBufferStride, UINT numStrides, bool multiStream, ID3D11GeometryShader **ppGS );
	TEST_RESULT SetupPSFromResource( LPCTSTR resourceID, LPCSTR entryProc, ID3D11PixelShader **ppPS );
	
	TEST_RESULT SetupSOClearObjects();
	TEST_RESULT ClearSOBuffer( ID3D11Buffer *soBuffer, UINT size );
	void CleanupSOClearObjects();
	
	TEST_RESULT SetupVertexBuffer( UINT numVerts, void *pVertexData );
	TEST_RESULT SetupRasterizer();
	TEST_RESULT SetupQueryObjects();
	
	void WriteDeclToLog( D3D11_SO_DECLARATION_ENTRY *decl, UINT numEntries );
	
	tstring ComposeGSName();
};

//////////////////////////////////////////////////////////////////////////
// CStreamOutPingPongTest
//////////////////////////////////////////////////////////////////////////
class CStreamOutPingPongTest : public CStreamOutTest
{
};

//////////////////////////////////////////////////////////////////////////
// CStreamOutDeclChangeTest
//////////////////////////////////////////////////////////////////////////
class CStreamOutDeclChangeTest : public CStreamOutTest
{
};

//////////////////////////////////////////////////////////////////////////
// CStreamOutMixedPrimTypeTest
//////////////////////////////////////////////////////////////////////////
class CStreamOutMixedPrimTypeTest : public CStreamOutTest
{
};

//////////////////////////////////////////////////////////////////////////
// CStreamOutStatsQueryTest
//////////////////////////////////////////////////////////////////////////
class CStreamOutStatsQueryTest : public CStreamOutTest
{
};

//////////////////////////////////////////////////////////////////////////
// CStreamOutOverflowPredicateTest
//////////////////////////////////////////////////////////////////////////
class CStreamOutOverflowPredicateTest : public CStreamOutTest
{
};


