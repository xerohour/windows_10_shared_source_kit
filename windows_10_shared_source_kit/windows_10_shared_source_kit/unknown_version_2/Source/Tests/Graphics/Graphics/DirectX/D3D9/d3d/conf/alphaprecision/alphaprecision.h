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

class CAlphaPrecision : public CD3DTest
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
    
    D3DTLVERTEX m_bgVerts[NumBackgroundVerts];
    D3DTLVERTEX m_fgVerts[NumForegroundVerts];
    
    CnTexture *m_pTargetTexture;
    CnTexture *m_pSysMemTexture;
    CnTexture *m_pCmpTexture;
    
    CnVertexDeclaration m_pVertexDecl16f;
    CnVertexDeclaration m_pVertexDecl32f;
    
    D3DTLVERTEX m_QuadVerts[4];
    
    static char m_MsgString[128];
    
    struct AlphaBlendMethod
    {
		DWORD dwSrcBlend;
		DWORD dwDestBlend;
		D3DBLENDOP d3dBlendOp;
		std::string strSrcBlend;
		std::string strDestBlend;
		std::string strBlendOp;
    };
    
    typedef std::vector< AlphaBlendMethod > AlphaBlendMethodArray;
    AlphaBlendMethodArray m_AlphaBlendMethods;
    
    struct PixelShaderInfo
    {
		CnPixelShader *pPixelShader;
		std::string strDesc;
    };
    
    typedef std::vector< PixelShaderInfo > PixelShaderArray;
    PixelShaderArray m_PixelShaders;
	
    const AlphaBlendMethod *m_pCurrentABM;
	const PixelShaderInfo *m_pCurrentPSI;
	
	float m_ShaderConst0[4];
	
	float m_fRTMax;

public:
	CAlphaPrecision();
	~CAlphaPrecision();
	
	// CD3DTest overloads
	virtual bool CapsCheck();
	virtual UINT TestInitialize();
	virtual bool TestTerminate();
	virtual bool Setup();
	virtual void Cleanup();
	virtual bool ExecuteTest( UINT uTestNumber );
	virtual void UpdateStatus();
	virtual void UpdateDetails();
	
	virtual bool ProcessFrame(void);
	
protected:
	// CAlphaPrecision Methods
	virtual bool CreateBackground();
	virtual bool CreateForeground();
	
	virtual bool CreatePixelShaders() { return true; }
	virtual bool CreateAlphaBlendMethods();
	
	virtual const char *GetTitle() = 0;
};


//
// CAlphaPrecisionFF
//
class CAlphaPrecisionFF : public CAlphaPrecision
{
public:
	CAlphaPrecisionFF();
	
protected:
	virtual bool CreatePixelShaders();
	
	virtual const char *GetTitle() { return "Fixed Function Pixel Pipeline"; }
};


//
// CAlphaPrecisionPS11
//
class CAlphaPrecisionPS11 : public CAlphaPrecision
{
public:
	CAlphaPrecisionPS11();
	
protected:
	virtual bool CreatePixelShaders();
	virtual bool CapsCheck();
	
	virtual const char *GetTitle() { return "PS v1.1"; }
};


//
// CAlphaPrecisionPS20
//
class CAlphaPrecisionPS20 : public CAlphaPrecision
{	
public:
	CAlphaPrecisionPS20();
	
protected:
	virtual bool CreatePixelShaders();
	virtual bool CapsCheck();
	
	virtual const char *GetTitle() { return "PS v2.0"; }
};


//
// CAlphaPrecisionPS30
//
class CAlphaPrecisionPS30 : public CAlphaPrecision
{
public:
	CAlphaPrecisionPS30();
	
protected:
	virtual bool CreatePixelShaders();
	virtual bool CapsCheck();
	
	virtual const char *GetTitle() { return "PS v3.0"; }
};

