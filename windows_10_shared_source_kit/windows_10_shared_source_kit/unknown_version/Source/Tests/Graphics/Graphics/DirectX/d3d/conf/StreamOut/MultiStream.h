#pragma once

#include "StreamOut.h"

enum MULTI_STREAM_DECL_MODE
{
	ONE_STREAM_FOUR_BUFFER,
	TWO_STREAM_TWO_BUFFER,
	THREE_STREAM_FOUR_BUFFER,
	FOUR_STREAM_FOUR_BUFFER,
};

//////////////////////////////////////////////////////////////////////////
// CStreamOutMultiStream
//////////////////////////////////////////////////////////////////////////
class CStreamOutMultiStream : public CStreamOutTest
{
public:
	// 128-Component output (the maximum allowed by fl 11.0)
	struct GSOUT128
	{
		float pos[4];
		UINT data[31][4];
	};

	struct CONSTDATA
	{
		UINT numPrimsPerInputVertex[4];
		UINT drawIndex[4];
		UINT numDrawCalls[4];
		UINT viewportHeight[4];
	};

	struct PIXELRESULT
	{
		UINT data[4];
	};

protected:
	UINT                    m_numInputVerts;
	UINT                    m_numShaderLoopIterations;
	UINT                    m_numDrawCalls;
	MULTI_STREAM_DECL_MODE  m_declMode;
	UINT                    m_stridePadding[4];
	UINT                    m_soVertexStride[4];
	ID3D11Texture2D			*m_pRTTexture2D;
	ID3D11Texture2D			*m_pRTTexture2DStaging;
	ID3D11RenderTargetView  *m_pRTV;
	UINT					m_numExpectedPixelsWritten;
	UINT					m_maxNumPixelsWritten;
	bool					*m_pPixelShouldBeOutput;
	PIXELRESULT				*m_pExpectedPixelResult;


public:	
	CStreamOutMultiStream();

protected:
	void InitTestParameters();
	TEST_RESULT SetupStreamOut();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();
	TEST_RESULT Setup();
	void Cleanup();

	TEST_RESULT TestPointList( UINT bufferIndex, const VSIN *pVertexIn, const BYTE *pGSOutData, UINT* pAssociatedStreamOut, D3D11_QUERY_DATA_SO_STATISTICS* pSOStatsOut, BOOL* pOverflowOut );

	// Helpers
	virtual void GetCurSODecl( std::vector< D3D11_SO_DECLARATION_ENTRY > *soDecl );
	TEST_RESULT CompareSOVertex( UINT bufferIndex, UINT streamIndex, const GSOUT128* refVert, const BYTE* pGSOutData, UINT outputVertexIndex, const D3D11_SO_DECLARATION_ENTRY* soDecl, UINT numEntries, bool inRange );
	UINT GetNumStreams();
	TEST_RESULT ValidateRenderTarget();
	bool PixelShouldHaveData( const UINT &r, const UINT &c );
};


