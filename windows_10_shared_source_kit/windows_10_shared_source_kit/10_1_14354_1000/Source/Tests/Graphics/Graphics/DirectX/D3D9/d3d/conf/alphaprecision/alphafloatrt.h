// ============================================================================
//
//  alphaprecision.h
//  Written by billkris.
//
// ============================================================================

#pragma once

#define D3D_OVERLOADS
#include "d3dx9.h"
#include "CD3DTest.h"
#include "Types.h"

#include "CD3DTest.h"
#include <vector>

class AlphaPrecParams
{
public:
	float fDestRGBConst;
	float fSrcRGBConst;
	float fSrcAlpha;
	
	D3DBLEND srcBlend;
	D3DBLEND destBlend;
	D3DBLENDOP blendOp;
	
	float fExpectedResult;
	
public:
	AlphaPrecParams( float app_fSrcRGBConst, float app_fSrcAlpha, float app_fDestRGBConst, D3DBLEND app_srcBlend, D3DBLEND app_destBlend, D3DBLENDOP app_blendOp, float app_fExpectedResult ) :
		fSrcRGBConst( app_fSrcRGBConst ),
		fSrcAlpha( app_fSrcAlpha ),
		fDestRGBConst( app_fDestRGBConst ),
		srcBlend( app_srcBlend ),
		destBlend( app_destBlend ),
		blendOp( app_blendOp ),
		fExpectedResult( app_fExpectedResult )
	{
	}

	AlphaPrecParams( const AlphaPrecParams &app ) :
		fSrcRGBConst( app.fSrcRGBConst ),
		fSrcAlpha( app.fSrcAlpha ),
		fDestRGBConst( app.fDestRGBConst ),
		srcBlend( app.srcBlend ),
		destBlend( app.destBlend ),
		blendOp( app.blendOp ),
		fExpectedResult( app.fExpectedResult )
	{
	}
	
	AlphaPrecParams &operator=( const AlphaPrecParams &app )
	{
		fSrcRGBConst = app.fSrcRGBConst;
		fSrcAlpha = app.fSrcAlpha;
		fDestRGBConst = app.fDestRGBConst;
		srcBlend = app.srcBlend;
		destBlend = app.destBlend;
		blendOp = app.blendOp;
		fExpectedResult = app.fExpectedResult;
		
		return *this;
	}
};

class CAlphaPrecisionFloatRT : public CD3DTest
{
protected:
	bool m_bValidate;
	VIEWPORT m_viewportRT;

    bool m_bOffscreen, m_bInvalid, m_bNull;

    CnSurface *m_pOriginalRT; 
    int m_nTexFormat;
    
    enum Constants
    {
		BackgroundDensity = 8,
		NumBackgroundPrimitives = BackgroundDensity * 2,
		NumBackgroundVerts = 2 * ( BackgroundDensity + 1 ),
		ForegroundDensity = 8,
		NumForegroundPrimitives = ForegroundDensity * 2,
		NumForegroundVerts = 2 * ( ForegroundDensity + 1 ),
    };
    
    D3DTLVERTEX m_tlVerts[4];
    
    CnTexture *m_pTargetTexture;
    CnTexture *m_pSysMemTexture;
    CnTexture *m_pCmpTexture;
    
    CnVertexDeclaration m_pVertexDecl16f;
    CnVertexDeclaration m_pVertexDecl32f;
    
    static char m_MsgString[128];
    
	CnPixelShader *m_pPixelShader;
	
	const AlphaPrecParams *m_pParams;
	const AlphaPrecParams *m_pCurrentParams;
	int m_nNumParams;

public:
	CAlphaPrecisionFloatRT();
	~CAlphaPrecisionFloatRT();
	
	// CD3DTest overloads
	virtual bool CapsCheck();
	virtual UINT TestInitialize();
	virtual bool TestTerminate();
	virtual bool Setup();
	virtual void Cleanup();
	virtual bool ExecuteTest( UINT uTestNumber );
	virtual void UpdateStatus();
	
	virtual bool ProcessFrame(void);
	
	bool BuildPixelShader();
	
protected:
	// CAlphaPrecisionFloatRT Methods
	virtual bool CreateQuad();
	
	virtual const char *GetTitle() const = 0;
	virtual int GetPixelShaderVersionMajor() const = 0;
	bool CompareResults( float r, float g, float b );
};


//
// CAlphaPrecisionFloatRTPS20
//
class CAlphaPrecisionFloatRTPS20 : public CAlphaPrecisionFloatRT
{	
public:
	CAlphaPrecisionFloatRTPS20();
	
protected:
	virtual bool CapsCheck();
	virtual UINT TestInitialize();
	
	virtual const char *GetTitle() const { return "PS v2.0"; }
	virtual int GetPixelShaderVersionMajor() const { return 2; }
};


//
// CAlphaPrecisionFloatRTPS30
//
class CAlphaPrecisionFloatRTPS30 : public CAlphaPrecisionFloatRT
{
public:
	CAlphaPrecisionFloatRTPS30();
	
protected:
	virtual bool CapsCheck();
	virtual UINT TestInitialize();
	
	virtual const char *GetTitle() const { return "PS v3.0"; }
	virtual int GetPixelShaderVersionMajor() const { return 3; }
};

