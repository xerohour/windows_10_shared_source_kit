#define D3D_OVERLOADS
#include <d3d8.h>
#include <d3dx8.h>
#include "types.h"
#include "Fog.h"

//************************************************************************
// TableLinear Test functions

CTableLinearTest::CTableLinearTest()
{
	m_szTestName = "Linear Table Fog";
	m_szCommandKey = "TableLinear";

	// Tell parent class which Fog we are
	dwFog = TABLE_LINEAR;

	// Don't request any texture formats
	m_Options.ModeOptions.dwTexInclude = 0;
}

bool CTableLinearTest::SetDefaultMatrices(void)
{
    D3DXMATRIX Matrix;

	// Setup the projection matrix for Z-based pixel fog
    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CTableLinearTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Linear
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_LINEAR);
	}
	return true;
}

UINT CTableLinearTest::TestInitialize(void)
{
#if FAST_TEST
    m_totalNumTestCases = TABLE_LINEAR_TESTS;
    SetTestRange(1, 1);
#else
	// Setup the Test range
	SetTestRange(1, TABLE_LINEAR_TESTS);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

	return D3DTESTINIT_RUN;
}

//************************************************************************
// TableExp Test functions

CTableExpTest::CTableExpTest()
{
	m_szTestName = "Exp Table Fog";
	m_szCommandKey = "TableExp";

	// Tell parent class which Fog we are
	dwFog = TABLE_EXP;

	// Don't request any texture formats
	m_Options.ModeOptions.dwTexInclude = 0;
}

bool CTableExpTest::SetDefaultMatrices(void)
{
    D3DXMATRIX Matrix;

	// Setup the projection matrix for Z-based pixel fog
    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CTableExpTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Exp
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_EXP);
	}
	return true;
}

UINT CTableExpTest::TestInitialize(void)
{
#if FAST_TEST
    m_totalNumTestCases = TABLE_EXP_TESTS;
    SetTestRange(1, 1);
#else
    // Setup the Test range
    SetTestRange(1, TABLE_EXP_TESTS);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

	return D3DTESTINIT_RUN;
}

//************************************************************************
// TableExp2 Test functions

CTableExp2Test::CTableExp2Test()
{
	m_szTestName = "Exp2 Table Fog";
	m_szCommandKey = "TableExp2";

	// Tell parent class which Fog we are
	dwFog = TABLE_EXP2;

	// Don't request any texture formats
	m_Options.ModeOptions.dwTexInclude = 0;
}

bool CTableExp2Test::SetDefaultMatrices(void)
{
    D3DXMATRIX Matrix;

	// Setup the projection matrix for Z-based pixel fog
    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CTableExp2Test::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Exp2
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_EXP2);
	}
	return true;
}

UINT CTableExp2Test::TestInitialize(void)
{
#if FAST_TEST
    m_totalNumTestCases = TABLE_EXP2_TESTS;
    SetTestRange(1, 1);
#else
	// Setup the Test range
	SetTestRange(1, TABLE_EXP2_TESTS);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

	return D3DTESTINIT_RUN;
}

//************************************************************************
// Texture TableLinear Test functions

CTextureTableLinearTest::CTextureTableLinearTest()
{
	m_szTestName = "Textured Linear Table Fog";
	m_szCommandKey = "TextureTableLinear";

	// Tell parent class which Fog we are
	dwFog = TABLE_LINEAR;

	// Request all texture formats except bumpmap
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE;
	m_Options.ModeOptions.dwTexExclude = PF_METN;
}

bool CTextureTableLinearTest::SetDefaultMatrices(void)
{
    D3DXMATRIX Matrix;

	// Setup the projection matrix for Z-based pixel fog
    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CTextureTableLinearTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Linear
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_LINEAR);
	}
	return true;
}

UINT CTextureTableLinearTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}
    
#if FAST_TEST
    m_totalNumTestCases = TEXTURE_TABLE_TESTS * m_uCommonTextureFormats;
    SetTestRange(1, 1);
#else
    // Setup the Test range
	SetTestRange(1, TEXTURE_TABLE_TESTS * m_uCommonTextureFormats);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

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
// Texture TableExp Test functions

CTextureTableExpTest::CTextureTableExpTest()
{
	m_szTestName = "Textured Exp Table Fog";
	m_szCommandKey = "TextureTableExp";

	// Tell parent class which Fog we are
	dwFog = TABLE_EXP;

	// Request all texture formats except bumpmap
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE;
	m_Options.ModeOptions.dwTexExclude = PF_METN;
}

bool CTextureTableExpTest::SetDefaultMatrices(void)
{
    D3DXMATRIX Matrix;

	// Setup the projection matrix for Z-based pixel fog
    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CTextureTableExpTest::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Exp
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_EXP);
	}
	return true;
}

UINT CTextureTableExpTest::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#if FAST_TEST
    m_totalNumTestCases = TEXTURE_TABLE_TESTS * m_uCommonTextureFormats;
    SetTestRange(1, 1);
#else
    // Setup the Test range
	SetTestRange(1, TEXTURE_TABLE_TESTS * m_uCommonTextureFormats);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

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
// Texture TableExp2 Test functions

CTextureTableExp2Test::CTextureTableExp2Test()
{
	m_szTestName = "Textured Exp2 Table Fog";
	m_szCommandKey = "TextureTableExp2";

	// Tell parent class which Fog we are
	dwFog = TABLE_EXP2;

	// Request all texture formats except bumpmap
	m_Options.ModeOptions.dwTexInclude = PF_RGB | PF_FOURCC | PF_LUMINANCE;
	m_Options.ModeOptions.dwTexExclude = PF_METN;
}

bool CTextureTableExp2Test::SetDefaultMatrices(void)
{
    D3DXMATRIX Matrix;

	// Setup the projection matrix for Z-based pixel fog
    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &Matrix))
	{
		WriteToLog("SetTransform(ProjectionMatrix) failed with HResult = %s.\n",m_pD3D->HResultToString(GetLastError()));
        return false;
	}

    return true;
}

bool CTextureTableExp2Test::SetDefaultRenderStates(void)
{
	// Make sure that we are using Table Fog Exp2
	if (!bCapAbort)
	{
		SetRenderState(D3DRS_FOGENABLE, (DWORD) TRUE);
		SetRenderState(D3DRS_FOGTABLEMODE, (DWORD) D3DFOG_EXP2);
	}
	return true;
}

UINT CTextureTableExp2Test::TestInitialize(void)
{
	// Make sure we get at least one common texture format
	if (m_uCommonTextureFormats == 0)
	{
		WriteToLog("No texture formats found.\n");
		return D3DTESTINIT_ABORT;
	}

#if FAST_TEST
    m_totalNumTestCases = TEXTURE_TABLE_TESTS * m_uCommonTextureFormats;
    SetTestRange(1, 1);
#else
    // Setup the Test range
	SetTestRange(1, TEXTURE_TABLE_TESTS * m_uCommonTextureFormats);
#endif

	// Check if CapsCheck requested us to abort
	if (bCapAbort)
		return D3DTESTINIT_ABORT;

	// Initialize FogColor variable to Blue
	FogColor = RGB_MAKE(0,0,255);

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
// Internal API:    TableTests
//
// Purpose:         
//
// Return:          None.
//************************************************************************

void CFogTest::TableTests(void)
{
	// Thin Triangle Strip on the top
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 16.5f, 56.5f, 0.0000f), 1.0f, RGBA_MAKE(255,0,0,255),0, 0.0f, 0.125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 16.5f, 28.5f, 0.0000f), 1.0f, RGBA_MAKE(255,0,0,255),0, 0.0f, 0.000f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(304.5f, 56.5f, 0.9999f), 1.0f, RGBA_MAKE(255,0,0,255),0, 1.0f, 0.125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(304.5f, 28.5f, 0.9999f), 1.0f, RGBA_MAKE(255,0,0,255),0, 1.0f, 0.000f);

	// Thin Triangle Strip on the bottom
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 16.5f, 252.5f, 0.9999f), 1.0f, RGBA_MAKE(255,0,0,255),0, 0.0f, 1.000f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 16.5f, 224.5f, 0.9999f), 1.0f, RGBA_MAKE(255,0,0,255),0, 0.0f, 0.875f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(304.5f, 252.5f, 0.0000f), 1.0f, RGBA_MAKE(255,0,0,255),0, 1.0f, 1.000f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(304.5f, 224.5f, 0.0000f), 1.0f, RGBA_MAKE(255,0,0,255),0, 1.0f, 0.875f);

	// Four white Triangles in the middle
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 16.5f, 210.5f, 0.2f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.0000f, 0.8125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 64.5f,  70.5f, 0.2f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.1667f, 0.1875f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(112.5f, 210.5f, 0.2f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.3333f, 0.8125f);

	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(128.5f, 210.5f, 0.4f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.3889f, 0.8125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3( 80.5f,  70.5f, 0.4f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.2222f, 0.1875f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(176.5f,  70.5f, 0.4f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.5556f, 0.1875f);
                                                                                                                                                                                         
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(144.5f, 210.5f, 0.6f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.4444f, 0.8125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(192.5f,  70.5f, 0.6f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.6111f, 0.1875f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(240.5f, 210.5f, 0.6f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.7778f, 0.8125f);
                                                                                                                                                                                         
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(256.5f, 210.5f, 0.8f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.8333f, 0.8125f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(208.5f,  70.5f, 0.8f), 1.0f, RGBA_MAKE(255,255,255,255),0, 0.6667f, 0.1875f);
	VertexList[dwVertexCount++] = D3DTLVERTEX(D3DXVECTOR3(304.5f,  70.5f, 0.8f), 1.0f, RGBA_MAKE(255,255,255,255),0, 1.0000f, 0.1875f);
}
