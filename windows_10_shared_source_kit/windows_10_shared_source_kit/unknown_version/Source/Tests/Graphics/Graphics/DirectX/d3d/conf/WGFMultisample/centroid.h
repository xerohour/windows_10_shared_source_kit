//////////////////////////////////////////////////////////////////////////
//  Centroid.h
//  created:	2005/07/18
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once
#include "multisample.h"
	
class CMultisampleTest_Centroid : public CMultisampleTest
{
public:
	CMultisampleTest_Centroid() 
		: CMultisampleTest(),
		  m_bCentroid( false ),
		  m_uSampleIndex( 0 ),
		  m_pTexView( NULL ),
		  m_pPSCentroid( NULL ),
		  m_pSampler( NULL ),
          m_pTexture( NULL )
	{}

	void InitTestParameters();

protected:
	TEST_RESULT ExecuteTestCase();
	TEST_RESULT SetupTestCase();
	BOOL Validate( BOOL bResult );
	void CleanupTestCase();

	bool m_bCentroid;
	UINT m_uSampleIndex;
	ID3D11Texture2D			*m_pTexture;
	ID3D11ShaderResourceView	*m_pTexView;
	ID3D11PixelShader		*m_pPSCentroid;
	ID3D11SamplerState		*m_pSampler;
};

// Test for Centroid when the hardware supports FeatureLevel11
// (even if a lesser feature level is used against the hardware)
enum CentroidInterpolationMode
{
	CentroidInterpolationMode_Linear = 0,
	CentroidInterpolationMode_Linear_NoPerspective = 1,
	CentroidInterpolationMode_Max, // Always at end
};
DECLARE_NAMED_VALUES(CentroidInterpolationMode);
inline CentroidInterpolationMode& operator++( CentroidInterpolationMode& e )
{ e = CentroidInterpolationMode( UINT( e ) + 1 ); return e; }
inline CentroidInterpolationMode operator++( CentroidInterpolationMode& e, int )
{ const CentroidInterpolationMode eRet = e; ++e; return eRet; }

enum TriangleOffsetAxis
{
	TriangleOffsetAxis_X = 0,
	TriangleOffsetAxis_Y = 1,
};
DECLARE_NAMED_VALUES(TriangleOffsetAxis);
inline TriangleOffsetAxis& operator++( TriangleOffsetAxis& e )
{ e = TriangleOffsetAxis( UINT( e ) + 1 ); return e; }
inline TriangleOffsetAxis operator++( TriangleOffsetAxis& e, int )
{ const TriangleOffsetAxis eRet = e; ++e; return eRet; }

class CMultisampleTest_Centroid11 : public CMultisampleTest
{
public:
	CMultisampleTest_Centroid11() 
		: CMultisampleTest(),
		  m_pSRView( NULL ),
		  m_pRTTexture2DSS( NULL ),
		  m_pSSRTView( NULL ),
		  m_pSTexture2DSS( NULL ),
		  m_pPSConvertMSToSS( NULL )
	{
		ZeroMemory( m_pPSCentroid, sizeof( m_pPSCentroid ) );
	}

	void InitTestParameters();

protected:
	TEST_RESULT SetupPipeline();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();

	ID3D11PixelShader *m_pPSCentroid[ CentroidInterpolationMode_Max ];
	ID3D11ShaderResourceView *m_pSRView;
	ID3D11Texture2D *m_pRTTexture2DSS;
	ID3D11RenderTargetView *m_pSSRTView;
	ID3D11Texture2D *m_pSTexture2DSS;
	ID3D11PixelShader *m_pPSConvertMSToSS;
	UINT m_FullyCoveredSampleMask;
	const float *m_pSamplePixelOffsets;
	const UINT16 *m_pSamplesCoveredByTriangleXOffset;
	const UINT16 *m_pSamplesCoveredByTriangleYOffset;

	// External Test Paramters:
	INT m_MinCoveredSampleFromMask;
	CentroidInterpolationMode m_CentroidInterpolationMode;
	TriangleOffsetAxis m_TriangleOffsetAxis;
	INT m_TriangleOffset;

	UINT m_SamplesCoveredByTriangleOffset;
	UINT m_PixelX;
	UINT m_PixelY;
	VERTEX m_TriangleVertex[ 3 ];
};

// Test for Centroid when the hardware supports FeatureLevel11
// (even if a lesser feature level is used against the hardware)
// This test focuses on the centroid value of a pixel that is not covered
class CMultisampleTest_Centroid11Uncovered : public CMultisampleTest
{
public:
	CMultisampleTest_Centroid11Uncovered() 
		: CMultisampleTest(),
		  m_pSRView( NULL ),
		  m_pRTTexture2DSS( NULL ),
		  m_pSSRTView( NULL ),
		  m_pPSConvertMSToSS( NULL )
	{
		ZeroMemory( m_pPSCentroid, sizeof( m_pPSCentroid ) );
		ZeroMemory( m_pPSCentroidUncovered, sizeof( m_pPSCentroidUncovered ) );
		ZeroMemory( m_apSTexture2DSS, sizeof( m_apSTexture2DSS ) );
	}

	void InitTestParameters();

protected:
	TEST_RESULT SetupPipeline();
	TEST_RESULT SetupTestCase();
	TEST_RESULT ExecuteTestCase();
	void CleanupTestCase();

	ID3D11PixelShader *m_pPSCentroid[ CentroidInterpolationMode_Max ];
	ID3D11PixelShader *m_pPSCentroidUncovered[ CentroidInterpolationMode_Max ];
	ID3D11ShaderResourceView *m_pSRView;
	ID3D11Texture2D *m_pRTTexture2DSS;
	ID3D11RenderTargetView *m_pSSRTView;
	ID3D11Texture2D *m_apSTexture2DSS[ 2 ];
	ID3D11PixelShader *m_pPSConvertMSToSS;
	UINT m_FullyCoveredSampleMask;
	const float *m_pSamplePixelOffsets;

	// External Test Paramters:
	INT m_MinCoveredSampleFromMask;
	CentroidInterpolationMode m_CentroidInterpolationMode;
};


