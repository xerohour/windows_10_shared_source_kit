#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "Fog.h"

//************************************************************************
// WLinear Test functions

CWLinearTest::CWLinearTest()
{
	m_szTestName = "Linear W Fog";
	m_szCommandKey = "WLinear";

	// Tell parent class which Fog we are
	dwFog = W_LINEAR;

	// Don't request any texture formats
	m_Options.ModeOptions.dwTexInclude = 0;
}

bool CWLinearTest::SetDefaultMatrices(void)
{
	D3DXMATRIX Matrix;

	// Setup the projection matrix for Eye-Relative pixel fog
    D3DXMatrixPerspectiveFovLH(&Matrix, pi/3.0f, 1, 1.0f, 10.0f);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CWLinearTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Linear
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_LINEAR);
	}
	return true;
}

UINT CWLinearTest::TestInitialize(void)
{
#if FAST_TEST
    m_totalNumTestCases = W_LINEAR_TESTS;
    SetTestRange(1, 1);
#else
	// Setup the Test range
	SetTestRange(1, W_LINEAR_TESTS);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

	return D3DTESTINIT_RUN;
}

//************************************************************************
// WExp Test functions

CWExpTest::CWExpTest()
{
	m_szTestName = "Exp W Fog";
	m_szCommandKey = "WExp";

	// Tell parent class which Fog we are
	dwFog = W_EXP;

	// Don't request any texture formats
	m_Options.ModeOptions.dwTexInclude = 0;
}

bool CWExpTest::SetDefaultMatrices(void)
{
	D3DXMATRIX Matrix;

	// Setup the projection matrix for Eye-Relative pixel fog
    D3DXMatrixPerspectiveFovLH(&Matrix, pi/3.0f, 1, 1.0f, 10.0f);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CWExpTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Exp
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_EXP);
	}
	return true;
}

UINT CWExpTest::TestInitialize(void)
{
#if FAST_TEST
    m_totalNumTestCases = W_EXP_TESTS;
    SetTestRange(1, 1);
#else
	// Setup the Test range
	SetTestRange(1, W_EXP_TESTS);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

	return D3DTESTINIT_RUN;
}

//************************************************************************
// WExp2 Test functions

CWExp2Test::CWExp2Test()
{
	m_szTestName = "Exp2 W Fog";
	m_szCommandKey = "WExp2";

	// Tell parent class which Fog we are
	dwFog = W_EXP2;

	// Don't request any texture formats
	m_Options.ModeOptions.dwTexInclude = 0;
}

bool CWExp2Test::SetDefaultMatrices(void)
{
	D3DXMATRIX Matrix;

	// Setup the projection matrix for Eye-Relative pixel fog
    D3DXMatrixPerspectiveFovLH(&Matrix, pi/3.0f, 1, 1.0f, 10.0f);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CWExp2Test::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Exp2
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_EXP2);
	}
	return true;
}

UINT CWExp2Test::TestInitialize(void)
{
#if FAST_TEST
    m_totalNumTestCases = W_EXP2_TESTS;
    SetTestRange(1, 1);
#else
	// Setup the Test range
	SetTestRange(1, W_EXP2_TESTS);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

	return D3DTESTINIT_RUN;
}

//************************************************************************
// Texture WLinear Test functions

CTextureWLinearTest::CTextureWLinearTest()
{
	m_szTestName = "Textured Linear W Fog";
	m_szCommandKey = "TextureWLinear";

	// Tell parent class which Fog we are
	dwFog = W_LINEAR;

	// Request all texture formats except bumpmap
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE;
	m_Options.ModeOptions.dwTexExclude = PF_METN;
}

bool CTextureWLinearTest::SetDefaultMatrices(void)
{
	D3DXMATRIX Matrix;

	// Setup the projection matrix for Eye-Relative pixel fog
    D3DXMatrixPerspectiveFovLH(&Matrix, pi/3.0f, 1, 1.0f, 10.0f);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CTextureWLinearTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Linear
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_LINEAR);
	}
	return true;
}

UINT CTextureWLinearTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#if FAST_TEST
    m_totalNumTestCases = TEXTURE_W_TESTS * m_uCommonTextureFormats;
    SetTestRange(1, 1);
#else
    // Setup the Test range
	SetTestRange(1, TEXTURE_W_TESTS * m_uCommonTextureFormats);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

    // Setup the colors for the Stripes array
	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

	return D3DTESTINIT_RUN;
}

//************************************************************************
// Texture WExp Test functions

CTextureWExpTest::CTextureWExpTest()
{
	m_szTestName = "Textured Exp W Fog";
	m_szCommandKey = "TextureWExp";

	// Tell parent class which Fog we are
	dwFog = W_EXP;

	// Request all texture formats except bumpmap
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE;
	m_Options.ModeOptions.dwTexExclude = PF_METN;
}

bool CTextureWExpTest::SetDefaultMatrices(void)
{
	D3DXMATRIX Matrix;

	// Setup the projection matrix for Eye-Relative pixel fog
    D3DXMatrixPerspectiveFovLH(&Matrix, pi/3.0f, 1, 1.0f, 10.0f);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CTextureWExpTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Exp
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_EXP);
	}
	return true;
}

UINT CTextureWExpTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#if FAST_TEST
    m_totalNumTestCases = TEXTURE_W_TESTS * m_uCommonTextureFormats;
    SetTestRange(1, 1);
#else
    // Setup the Test range
	SetTestRange(1, TEXTURE_W_TESTS * m_uCommonTextureFormats);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

    // Setup the colors for the Stripes array
	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

    return D3DTESTINIT_RUN;
}

//************************************************************************
// Texture WExp2 Test functions

CTextureWExp2Test::CTextureWExp2Test()
{
	m_szTestName = "Textured Exp2 W Fog";
	m_szCommandKey = "TextureWExp2";

	// Tell parent class which Fog we are
	dwFog = W_EXP2;

	// Request all texture formats except bumpmap
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE;
	m_Options.ModeOptions.dwTexExclude = PF_METN;
}

bool CTextureWExp2Test::SetDefaultMatrices(void)
{
	D3DXMATRIX Matrix;

	// Setup the projection matrix for Eye-Relative pixel fog
    D3DXMatrixPerspectiveFovLH(&Matrix, pi/3.0f, 1, 1.0f, 10.0f);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CTextureWExp2Test::SetDefaultRenderStates(void)
{
	// Make sure that we are using W Fog Exp2
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_EXP2);
	}
	return true;
}

UINT CTextureWExp2Test::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#if FAST_TEST
    m_totalNumTestCases = TEXTURE_W_TESTS * m_uCommonTextureFormats;
    SetTestRange(1, 1);
#else
    // Setup the Test range
	SetTestRange(1, TEXTURE_W_TESTS * m_uCommonTextureFormats);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Red
	FogColor = RGB_MAKE(255,0,0);

    // Setup the colors for the Stripes array
	Stripes[0] = RGBA_MAKE(255,255,255,255);// White
	Stripes[1] = RGBA_MAKE(255,0,0,255);	// Red
	Stripes[2] = RGBA_MAKE(0,255,0,255);	// Green
	Stripes[3] = RGBA_MAKE(0,0,255,255);	// Blue
	Stripes[4] = RGBA_MAKE(255,255,0,255);	// Yellow
	Stripes[5] = RGBA_MAKE(255,0,255,255);	// Magenta
	Stripes[6] = RGBA_MAKE(0,255,255,255);	// Cyan
	Stripes[7] = RGBA_MAKE(128,128,128,255);// Grey

	return D3DTESTINIT_RUN;
}

//************************************************************************
// Internal API:    WTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CFogTest::WTests(void)
{
	// Thin Triangle Strip on the top
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 16.5f, 56.5f, 0.001111f), 0.999f, RGBA_MAKE(0,255,0,255),0, 0.0f, 0.125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 16.5f, 28.5f, 0.001111f), 0.999f, RGBA_MAKE(0,255,0,255),0, 0.0f, 0.000f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(304.5f, 56.5f, 1.000000f), 0.100f, RGBA_MAKE(0,255,0,255),0, 1.0f, 0.125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(304.5f, 28.5f, 1.000000f), 0.100f, RGBA_MAKE(0,255,0,255),0, 1.0f, 0.000f);

	// Thin Triangle Strip on the bottom
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 16.5f, 252.5f, 1.000000f), 0.100f, RGBA_MAKE(0,255,0,255),0, 0.0f, 1.000f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 16.5f, 224.5f, 1.000000f), 0.100f, RGBA_MAKE(0,255,0,255),0, 0.0f, 0.875f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(304.5f, 252.5f, 0.001111f), 0.999f, RGBA_MAKE(0,255,0,255),0, 1.0f, 1.000f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(304.5f, 224.5f, 0.001111f), 0.999f, RGBA_MAKE(0,255,0,255),0, 1.0f, 0.875f);

	// Four white Triangles in the middle
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 16.5f, 210.5f, 0.222222f), 0.8f, RGBA_MAKE(255,255,255,255),0, 0.0000f, 0.8125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 64.5f,  70.5f, 0.222222f), 0.8f, RGBA_MAKE(255,255,255,255),0, 0.1667f, 0.1875f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(112.5f, 210.5f, 0.222222f), 0.8f, RGBA_MAKE(255,255,255,255),0, 0.3333f, 0.8125f);

	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(128.5f, 210.5f, 0.444444f), 0.6f, RGBA_MAKE(255,255,255,255),0, 0.3889f, 0.8125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 80.5f,  70.5f, 0.444444f), 0.6f, RGBA_MAKE(255,255,255,255),0, 0.2222f, 0.1875f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(176.5f,  70.5f, 0.444444f), 0.6f, RGBA_MAKE(255,255,255,255),0, 0.5556f, 0.1875f);

	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(144.5f, 210.5f, 0.666666f), 0.4f, RGBA_MAKE(255,255,255,255),0, 0.4444f, 0.8125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(192.5f,  70.5f, 0.666666f), 0.4f, RGBA_MAKE(255,255,255,255),0, 0.6111f, 0.1875f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(240.5f, 210.5f, 0.666666f), 0.4f, RGBA_MAKE(255,255,255,255),0, 0.7778f, 0.8125f);

	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(256.5f, 210.5f, 0.888888f), 0.2f, RGBA_MAKE(255,255,255,255),0, 0.8333f, 0.8125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(208.5f,  70.5f, 0.888888f), 0.2f, RGBA_MAKE(255,255,255,255),0, 0.6667f, 0.1875f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(304.5f,  70.5f, 0.888888f), 0.2f, RGBA_MAKE(255,255,255,255),0, 1.0000f, 0.1875f);
}
