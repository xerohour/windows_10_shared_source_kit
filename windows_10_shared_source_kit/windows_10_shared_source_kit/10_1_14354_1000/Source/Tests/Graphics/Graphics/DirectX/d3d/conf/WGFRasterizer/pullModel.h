//////////////////////////////////////////////////////////////////////////
//  pullModel.h
//  created:	2009/04/21
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "Interpolator.h"
#include "d3dx11.h"

enum PULL_EVAL_FUNCS 
{	
	EVAL_SNAPPED = 0,
	EVAL_SAMPLE,
	EVAL_CENTROID,
	USE_ALL_EVALS
};
DECLARE_NAMED_VALUES(PULL_EVAL_FUNCS);


// struct for tables mapping smaple index to x/y position of the sample. 
// x/y position uses [-8, 7] sample grid, 
struct SamplePosition
{
	int pos_x;
	int pos_y;
	SamplePosition ( int x, int y)
	{
		pos_x = x;
		pos_y = y;
	}
	SamplePosition ( )
	{
		pos_x = 0;
		pos_y = 0;
	}
} ;

struct Vertex1Value
{
	float x;
	float y;
	float z;
	float Data0;
} ;

struct Vertex4Value
{
	float x;
	float y;
	float z;
	float Data0;
	float Data1;
	float Data2;
	float Data3;
} ;


class CPullModel : public CInterpolator
{
public:
	CPullModel();
	void InitTestParameters();
	
	TEST_RESULT Setup();
	void Cleanup();

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	void CleanupTestCase();

	ID3D11DepthStencilState* m_pDepthStateDefault;
	ID3D11DepthStencilState* m_pDepthStateDisabled;
	ID3D11RasterizerState1* m_pRastState;
	D3D11_VIEWPORT m_vp;
	
	PULL_EVAL_FUNCS m_evalFunc;
	UINT m_numInterValues;
	UINT m_SampleCount;
	UINT m_SampleIndex;
	SamplePosition m_SamplePos;



	// tables to map sample index to position
	// 2 tables for sampleCount 1, 2, 4, 8, 16
	std::vector<SamplePosition> m_samplePosTable[5];
	// the index of the centroid sample when the upper left half of the pixel is covered
	int m_UpperLeftCentoid[5];

	// precalculated interpolated values for triangle C1-C2-C3 being drawn
	// Snorm attribute associated with each vertex: C1(0.0), C2(1.0), C3(0.0)
	float m_SampleGridValuesSnorm[3][16][16];
	// Float attribute associated with each vertex: C1(16.0), C2(64.0), C3(0.0)
	float m_SampleGridValuesFloat[3][16][16];



	TEST_RESULT VerifyResult( float* pResultData );
	TEST_RESULT CalcExpectedValues( float valVert0, float valVert1, float valVert2, float valueTable[3][16][16] );
	TEST_RESULT CompareToExpected(UINT pixId, float resultValue, SamplePosition sampPos, float expectedValues[16][16]);
	TEST_RESULT SetupSampleTables() ;
	TEST_RESULT SetupVSStage();
	TEST_RESULT SetupRenderTarget();
	TEST_RESULT CreateVBValues( float** ppRetVBValues);
	TEST_RESULT BuildPixelShader();

};

