#pragma once

#include "testapp.h"

extern CDepthStencilTestApp g_TestApp;

struct StencilBitMask
{
	UINT8 Mask;

	StencilBitMask() : Mask(0xFF) {}
	StencilBitMask( const StencilBitMask &val ) : Mask(val.Mask) {}
	StencilBitMask( const UINT8 &val ) : Mask(val) {}

	operator UINT8() { return Mask;	}
	friend bool operator== (const StencilBitMask &val1, const StencilBitMask &val2)
	{
		return val1.Mask == val2.Mask;
	}
};

tstring ToString( const StencilBitMask &type );
bool FromString( StencilBitMask *pPos, const tstring &str );

class CStencilTest : public CD3D11Test
{
public:
    CStencilTest();
    virtual ~CStencilTest();

	virtual TEST_RESULT	Setup();
	virtual void		Cleanup();
	virtual TEST_RESULT	SetupTestCase();
	virtual void		CleanupTestCase();

	virtual TEST_RESULT ExecuteTestCase() = 0;

	int GetNumRenderWindows() const { return GetFramework()->GetShell()->IsConsole() ? 0 : 1; }
    bool CreateRTFromSwapChainProps() const { return true; }

protected:
	D3D11_DEPTH_STENCIL_DESC	m_dsDesc;
	ID3D11DepthStencilState		*m_pDSState;
	ID3D11DepthStencilState		*m_pDSVerify;

	UINT					m_RTHeight, m_RTWidth;

	ID3D11Texture2D				*m_pRTCopy;

	ID3D11Buffer					*m_pConstBuffer;
	ID3D11Buffer					*m_pVertexBuffer;
	ID3D11InputLayout			*m_pVertexLayout;

	ID3D11RasterizerState		*m_pRS;
	ID3D11BlendState				*m_pBS;

	ID3D11VertexShader			*m_pVS;
	ID3D11PixelShader			*m_pPS;

    D3D_RENDER_TARGET_OPTIONS m_origRenderTargetOptions;

	TEST_RESULT DrawFront( FLOAT depth = 0.0f );
	TEST_RESULT DrawBack( FLOAT depth = 0.0f );

	enum VERIFYRANGE
	{
		TOP,
		BOTTOM,
		BOTH
	};
	TEST_RESULT Verify( VERIFYRANGE range = BOTH, bool expectBlack = false );
	HRESULT ClearDepthStencil(FLOAT depthVal, UINT8 stencilVal);
};

class CStencilOps : public CStencilTest
{
public:
	CStencilOps();

	virtual void InitTestParameters();

	virtual TEST_RESULT ExecuteTestCase();
};

// verifies that Read-only depth has no effect on stencil ops
class CStencilOpsDepthRO : public CStencilOps
{
public:
	virtual void InitTestParameters();
};

class CStencilFuncs : public CStencilTest
{
public:
	CStencilFuncs();

	virtual void InitTestParameters();

	virtual TEST_RESULT ExecuteTestCase();
};

// verifies that Read-only depth has no effect on stencil funcs
class CStencilFuncsDepthRO : public CStencilFuncs
{
public:
	virtual void InitTestParameters();
};

class CStencilDepthOp : public CStencilTest
{
public:
	CStencilDepthOp();

	virtual void InitTestParameters();

	virtual TEST_RESULT ExecuteTestCase();
};

// verifies that Read-only depth has no effect on DepthFail ops
class CStencilDepthOpDepthRO : public CStencilDepthOp
{
public:
	virtual void InitTestParameters();
};

class CStencilMask : public CStencilTest
{
protected:
	StencilBitMask ReadMask, WriteMask;

public:
	CStencilMask();

	virtual void InitTestParameters();
	virtual TEST_RESULT SetupTestCase();

	virtual TEST_RESULT ExecuteTestCase();
};

// verifies that Read-only depth has no effect on stencil masks
class CStencilMaskDepthRO : public CStencilMask
{
public:
	virtual void InitTestParameters();
};