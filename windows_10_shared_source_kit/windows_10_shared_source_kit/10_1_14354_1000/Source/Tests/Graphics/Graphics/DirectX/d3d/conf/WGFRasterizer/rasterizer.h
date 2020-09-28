//////////////////////////////////////////////////////////////////////////
//  Rasterizer.h
//  created:	2006/07/14
//
//  purpose: 
//////////////////////////////////////////////////////////////////////////

#pragma once

#include "new_off.h"
#include "math.h"
#include "D3D11Test.h"
#include "new_on.h"
#include <string>
#include <WGFTestCore.h>
#include <D3DSampleLocation.h>
#include <Direct3DFailureCategoryStrings.h>

//
//defines
//
#if !defined(PASTER2)
#define PASTER2(x,y) x ## y
#endif

#if !defined(PASTER)
#define PASTER(x,y) PASTER2(x,y)
#endif

#if !defined(UNICODIFY)
#define UNICODIFY( x ) PASTER(L,x)
#endif

#if !defined(__FILEW__)
#define __FILEW__ UNICODIFY(__FILE__)
#endif

#define FAILSKIP(x) CFailSkip::Check(x)
#define FAILSKIP_RESULT() CFailSkip::GetPrevResult()
#define MAX_INDEX_ARRAY 30

//
// structs
//
struct Vertex {	
	float pos[2];
	float fData;
	UINT  uData;
	int   iData;
};

enum WindingOrder {	CW = 0, CCW };

struct IndexBuffer {
	D3D11_PRIMITIVE_TOPOLOGY top;
	bool bCC;  // winding order
	UINT size;
	UINT data[MAX_INDEX_ARRAY];
};

//
// CRasterizerTest
//
class CRasterizerTest : public CD3D11Test
{
public:
	CRasterizerTest();

protected:
	void InitRasterizerParameters();
	virtual TEST_RESULT Setup();
	virtual TEST_RESULT SetupTestCase() = 0;
	void SetupRasterizerTestCase();
	
	virtual void Cleanup();
	virtual void CleanupTestCase() = 0;
	void CleanupRasterizerTestCase();
	
	TEST_RESULT SetupD3D();
	TEST_RESULT SetupRenderTarget();
	virtual void CleanupD3D();

	void CreateVB();
	void FillIndexBuffer(IndexBuffer* pBuffer);
	UINT CompareBuffers();
	void Present();
	void BeginQuery();
	UINT64 EndQuery();
	UINT GetIBIndex();
	void WriteIBToLog();

protected:
	ID3D11Device1				*m_pDevice;
	ID3D11DeviceContext1		*m_pDeviceContext;

	// render target
	DXGI_FORMAT			m_RTFormat;
	UINT				m_uRTWidth;
	UINT				m_uRTHeight;
	UINT                m_uRTSampleCount;
	UINT                m_uForcedSampleCount;
	ID3D11Texture2D			*m_pRTTexture2D;
	ID3D11RenderTargetView	*m_pRTView;

	// staging
	ID3D11Texture2D			*m_pTexA;
	ID3D11Texture2D			*m_pTexB;
	D3D11_BOX				m_TexBox;
	
	ID3D11RasterizerState1	*m_pRSEnabled;
	ID3D11BlendState1 *m_pBSLogicOR;

	// buffers
	ID3D11Buffer				*m_pVB;
	ID3D11Buffer				*m_pIB;
	ID3D11InputLayout		*m_pVertexLayout;

	// vb data
	std::vector<Vertex>	m_VBData;

	// shaders
	ID3D11VertexShader		*m_pVS;
	ID3D11PixelShader		*m_pPS;
	ID3D11Buffer				*m_pConstBuffer;

	// query
	ID3D11Query				*m_pQuery;
	
	// test paramaters
	IndexBuffer				*m_pBuffer;
	D3D11_FILL_MODE			m_FillMode;
	D3D11_CULL_MODE			m_CullMode;
	bool					m_bFCC;

	// test factors
	testfactor::RFactor		m_rfIBSet, m_rfSingleTriSet, m_rfQuadSet, m_rfDegenTriSet;
	testfactor::RFactor		m_rfFillMode, m_rfFillModeSolid, m_rfFillModeWire;
	testfactor::RFactor		m_rfCullMode, m_rfCullModeNone;
	testfactor::RFactor		m_rfFCC, m_rfFCCTrue;
};

//
// CRasterizerTestApp
//
class CRasterizerTestApp : public CD3D11TestFramework
{
private:
	TESTTYPE GetTestType() const { return TESTTYPE_CONF; }
	
	void InitOptionVariables();
	bool InitTestGroups();

protected:
	bool m_bLogShaders;

public:
	bool m_bPresent;

	bool IsDebugRuntime() const { return m_D3DOptions.Debug; }
	bool IsLoggingShaders() const { return m_bLogShaders; }
	D3D_DRIVER_TYPE GetSrcDriverType() const { return m_D3DOptions.DriverType; }
	UINT GetDefaultQueryTimeout() { return 10000; }

};
extern CRasterizerTestApp g_TestApp;

