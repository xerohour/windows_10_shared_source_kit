//-----------------------------------------------------------------------------	
//	File:			zbias.cpp
//	Description:    zbias class methods for zbias conformance tests
//	Author:			Todd M. Frost
//	Modifications:	Matthew E. Haigh - 07/2003 thru 08/2003
//						-Improved test for better feature coverage.
//-----------------------------------------------------------------------------

#include "zbias.h"

CD3DWindowFramework App;          //global instance of D3D window framework class
CZBiasZBuffer       ZBiasZBuffer; //global instance of CZBiasZBuffer class
//CZBiasWBuffer       ZBiasWBuffer; //global instance of CZBiasWBuffer class

const DOUBLE ZERO = 1.0e-12;

#define OUTPUT_DEBUG_STATUS 0

#ifdef FAST_TEST
float gBiases[NBIASTESTS][12] =
{
	{ -0.500f, -0.400f, -0.300f, -0.200f, -0.100f, 0.000f, 0.100f, 0.200f, 0.300f, 0.400f, 0.500f },
	{ 0.500f, 0.400f, 0.300f, 0.200f, 0.100f, 0.000f, -0.100f, -0.200f, -0.300f, -0.400f, -0.500f },
	{ 0.490f, 0.491f, 0.492f, 0.493f, 0.494f, 0.495f, 0.496f, 0.497f, 0.498f, 0.499f, 0.500f },
};
#else
float gBiases[NBIASTESTS][12] =
{
	/*{ 0.000f,  0.100f,  0.200f,  0.300f,  0.400f,  0.500f,  0.600f,  0.700f,  0.800f,  0.900f,  1.000f,  1.100f },
	{-0.000f, -0.100f, -0.200f, -0.300f, -0.400f, -0.500f, -0.600f, -0.700f, -0.800f, -0.900f, -1.000f, -1.100f },
	{ 0.450f,  0.460f,  0.470f,  0.480f,  0.490f,  0.500f,  0.510f,  0.520f,  0.530f,  0.540f,  0.550f,  0.560f },
	{-0.450f, -0.460f, -0.470f, -0.480f, -0.490f, -0.500f, -0.510f, -0.520f, -0.530f, -0.540f, -0.550f, -0.560f },
	{ 0.495f,  0.496f,  0.497f,  0.498f,  0.499f,  0.500f,  0.501f,  0.502f,  0.503f,  0.504f,  0.505f,  0.506f },
	{-0.495f, -0.496f, -0.497f, -0.498f, -0.499f, -0.500f, -0.501f, -0.502f, -0.503f, -0.504f, -0.505f, -0.506f }*/
	{ -0.500f, -0.400f, -0.300f, -0.200f, -0.100f, 0.000f, 0.100f, 0.200f, 0.300f, 0.400f, 0.500f },
	{ 0.500f, 0.400f, 0.300f, 0.200f, 0.100f, 0.000f, -0.100f, -0.200f, -0.300f, -0.400f, -0.500f },
	{ 0.400f, 0.410f, 0.420f, 0.430f, 0.440f, 0.450f, 0.460f, 0.470f, 0.480f, 0.490f, 0.500f },
	{ -0.400f, -0.410f, -0.420f, -0.430f, -0.440f, -0.450f, -0.460f, -0.470f, -0.480f, -0.490f, -0.500f },
	{ 0.490f, 0.491f, 0.492f, 0.493f, 0.494f, 0.495f, 0.496f, 0.497f, 0.498f, 0.499f, 0.500f },
	{ -0.490f, -0.491f, -0.492f, -0.493f, -0.494f, -0.495f, -0.496f, -0.497f, -0.498f, -0.499f, -0.500f },
};
#endif


#ifdef FAST_TEST
float gSlopes[8][8] =
{
	{ 0.0f, -10.0f, -20.0f, -30.0f, -40.0f, -50.0f, -60.0f, -70.0f },
	{ 0.0f, -1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f },
	{ 0.0f, -0.1f, -0.2f, -0.3f, -0.4f, -0.5f, -0.6f, -0.7f },
	{ 0.0f, 0.01f, 0.02f, 0.03f, 0.04f, 0.05f, 0.06f, 0.07f },
	{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f },
	{ 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f },
	{ 0.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f },
	{ 20.f, 10.0f, 0.0f, -10.0f, -20.0f, -30.0f, -40.0f, -50.0f },
};
#else
float gSlopes[8][8] =
{
	{ 0.0f, -10.0f, -20.0f, -30.0f, -40.0f, -50.0f, -60.0f, -70.0f },
	{ 0.0f, -1.0f, -2.0f, -3.0f, -4.0f, -5.0f, -6.0f, -7.0f },
	{ 0.0f, -0.1f, -0.2f, -0.3f, -0.4f, -0.5f, -0.6f, -0.7f },
	{ 0.0f, 0.01f, 0.02f, 0.03f, 0.04f, 0.05f, 0.06f, 0.07f },
	{ 0.0f, 0.1f, 0.2f, 0.3f, 0.4f, 0.5f, 0.6f, 0.7f },
	{ 0.0f, 1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f },
	{ 0.0f, 10.0f, 20.0f, 30.0f, 40.0f, 50.0f, 60.0f, 70.0f },
	{ 20.f, 10.0f, 0.0f, -10.0f, -20.0f, -30.0f, -40.0f, -50.0f },
};
#endif

//-----------------------------------------------------------------------------
//	Name:		CZBias()
//	Desc:		constructor for z bias class
//-----------------------------------------------------------------------------

CZBias::CZBias ()
{
    SetTestType (TESTTYPE_CONF);

    m_szCommandKey = "ZBias";

	//clear the status buffer - useful for debugging
#if OUTPUT_DEBUG_STATUS
	ClearStatus();
#endif

    m_fBias  = 0.0f;
    m_fScale = 0.0f;

	//set the test options
    m_Options.ModeOptions.TextureFmt = FMT_ANY;
    m_Options.ModeOptions.CubeFmt    = FMT_ANY;
    m_Options.ModeOptions.VolumeFmt  = FMT_ANY;
    m_Options.ModeOptions.ZBufferFmt = FMT_ALL;

    m_Options.D3DOptions.bZBuffer = true;
    m_Options.D3DOptions.fMinDXVersion = 9.0f;

	// Force test to cycle desktop resolutions 16bpp/32bpp
	m_Options.D3DOptions.DisplayMode.Format.d3dfFormat = (D3DFORMAT)FMT_ALL;
}


//-----------------------------------------------------------------------------
//	Name:		~CZBias()
//	Desc:		destructor for z bias class
//-----------------------------------------------------------------------------

CZBias::~CZBias ()
{
}

//-----------------------------------------------------------------------------
//	Name:		CommandLineHelp()
//	Output:		command line help information
//-----------------------------------------------------------------------------

void CZBias::CommandLineHelp ()
{
    WriteCommandLineHelp ("$y!caps: $wcaps ignored $c($wOn$c/Off)");
}

//-----------------------------------------------------------------------------
//	Name:        CapsCheck()
//	Desc:        caps check
//	Returns:     true if successful, false if not
//-----------------------------------------------------------------------------

bool CZBias::CapsCheck ()
{
	//check if source device exists
	if (m_pSrcDevice != NULL)
	{
		//check if slope scale is supported in source device
		if (!(m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_SLOPESCALEDEPTHBIAS))
		{
			WriteToLog ("Device does not support true slope scale based depth bias. ");

			if (!KeySet("!caps"))
			{
				WriteToLog ("Aborting group.\n");
				return false;
			}
			else
				WriteToLog ("Ignoring.\n");
		}

		//check if fog is supported
		if (!(m_pSrcDevice->GetCaps()->dwRasterCaps & D3DPRASTERCAPS_ZFOG))
			m_Flags.Clear (CF_FOG);
		else
			m_Flags.Set (CF_FOG);

		if (((m_pSrcDevice->GetCaps()->dwPixelShaderVersion & 0x0000FF00) >> 8) < 2)
			m_Flags.Clear (CF_PIXELSHADER);
		else
			m_Flags.Set (CF_PIXELSHADER);

	}

    return true;
}

//-----------------------------------------------------------------------------
//	Name:		Setup()
//	Desc:		sets up the test environment
//	Returns:	true if successful, false if not
//-----------------------------------------------------------------------------

bool CZBias::Setup ()
{
    if (!SetupLights())
    {
        WriteToLog("Unable to set lights.\n");
        return false;
    }

    if (!SetupMaterials())
    {
        WriteToLog("Unable to set materials.\n");
        return false;
    }

    if (!SetupMatrices())
    {
        WriteToLog("Unable to set matrices.\n");
        return false;
    }

    if (!SetupRenderStates())
    {
        WriteToLog("Unable to set render states.\n");
        return false;
    }

    if (!SetupShapes())
    {
        WriteToLog("Unable to set shapes.\n");
        return false;
    }

	if (!SetupFactors ())
	{
		WriteToLog ("Unable to set test factors.\n");
		return false;
	}

    return true;
}

//-----------------------------------------------------------------------------
//	Name:		SetupLights()
//	Desc:		sets up the lights
//	Returns:	true if successful, false if not
//-----------------------------------------------------------------------------

bool CZBias::SetupLights ()
{
    LIGHT Light;

    Light.dltType = D3DLIGHT_POINT;

    Light.dcvAmbient.r = 1.0f;
    Light.dcvAmbient.g = 1.0f;
    Light.dcvAmbient.b = 1.0f;
    Light.dcvAmbient.a = 1.0f;

    Light.dcvDiffuse.r = 1.0f;
    Light.dcvDiffuse.g = 1.0f;
    Light.dcvDiffuse.b = 1.0f;
    Light.dcvDiffuse.a = 1.0f;

    Light.dcvSpecular.r = 1.0f;
    Light.dcvSpecular.g = 1.0f;
    Light.dcvSpecular.b = 1.0f;
    Light.dcvSpecular.a = 1.0f;

    Light.dvPosition.x = 0.0f;
    Light.dvPosition.y = 0.0f;
    Light.dvPosition.z = -20.0f;

    Light.dvDirection.x = 0.0;
    Light.dvDirection.y = 0.0;
    Light.dvDirection.z = 0.0;

    Light.dvAttenuation0 = 1.0f;
    Light.dvAttenuation1 = 0.0f;
    Light.dvAttenuation2 = 0.0f;

    Light.dvFalloff = 0.0f;
    Light.dvPhi = 0.0f;
    Light.dvRange = 20000.0f;
    Light.dvTheta = 0.0f;

    if (!SetLight((DWORD) 0, &Light))
    {
        WriteToLog("SetLight() failed (white light 0): %s.\n", GLE(GetLastError()));
        return false;
    }
    else if (!LightEnable((DWORD) 0, TRUE))
    {
        WriteToLog("LightEnable() failed (white light 0): %s.\n", GLE(GetLastError()));
        return false;
    }

	Light.dvPosition.x = 20.0f;
	Light.dvPosition.y = 0.0f;
	Light.dvPosition.z = 0.0f;

    if (!SetLight((DWORD) 1, &Light))
    {
        WriteToLog("SetLight() failed (white light 1): %s.\n", GLE(GetLastError()));
        return false;
    }
    else if (!LightEnable((DWORD) 1, TRUE))
    {
        WriteToLog("LightEnable() failed (white light 1): %s.\n", GLE(GetLastError()));
        return false;
    }

	Light.dvPosition.x = -20.0f;
	Light.dvPosition.y = 0.0f;
	Light.dvPosition.z = 0.0f;

    if (!SetLight((DWORD) 2, &Light))
    {
        WriteToLog("SetLight() failed (white light 2): %s.\n", GLE(GetLastError()));
        return false;
    }
    else if (!LightEnable((DWORD) 2, TRUE))
    {
        WriteToLog("LightEnable() failed (white light 2): %s.\n", GLE(GetLastError()));
        return false;
    }	

    return true;
}

//-----------------------------------------------------------------------------
//	Name:		SetupMaterials()
//	Desc:		sets up the materials
//	Returns:	true if successful, false if not
//-----------------------------------------------------------------------------

bool CZBias::SetupMaterials ()
{
    m_pMaterials[MATERIAL_WHITE].dcvEmissive.r = 0.0f;
    m_pMaterials[MATERIAL_WHITE].dcvEmissive.g = 0.0f;
    m_pMaterials[MATERIAL_WHITE].dcvEmissive.b = 0.0f;
    m_pMaterials[MATERIAL_WHITE].dcvEmissive.a = 0.0f;

    m_pMaterials[MATERIAL_WHITE].dcvAmbient.r = 0.0f;
    m_pMaterials[MATERIAL_WHITE].dcvAmbient.g = 0.0f;
    m_pMaterials[MATERIAL_WHITE].dcvAmbient.b = 0.0f;
    m_pMaterials[MATERIAL_WHITE].dcvAmbient.a = 0.0f;

    m_pMaterials[MATERIAL_WHITE].dcvDiffuse.r = 1.0f;
    m_pMaterials[MATERIAL_WHITE].dcvDiffuse.g = 1.0f;
    m_pMaterials[MATERIAL_WHITE].dcvDiffuse.b = 1.0f;
    m_pMaterials[MATERIAL_WHITE].dcvDiffuse.a = 1.0f;

    m_pMaterials[MATERIAL_WHITE].dcvSpecular.r = 0.0f;
    m_pMaterials[MATERIAL_WHITE].dcvSpecular.g = 0.0f;
    m_pMaterials[MATERIAL_WHITE].dcvSpecular.b = 0.0f;
    m_pMaterials[MATERIAL_WHITE].dcvSpecular.a = 1.0f;

    m_pMaterials[MATERIAL_WHITE].dvPower = 1.0f;

    memcpy(&m_pMaterials[MATERIAL_RED], &m_pMaterials[MATERIAL_WHITE], sizeof(MATERIAL));
    m_pMaterials[MATERIAL_RED].dcvDiffuse.r = 1.0f;
    m_pMaterials[MATERIAL_RED].dcvDiffuse.g = 0.0f;
    m_pMaterials[MATERIAL_RED].dcvDiffuse.b = 0.0f;
    m_pMaterials[MATERIAL_RED].dcvDiffuse.a = 1.0f;

    memcpy(&m_pMaterials[MATERIAL_GREEN], &m_pMaterials[MATERIAL_WHITE], sizeof(MATERIAL));
    m_pMaterials[MATERIAL_GREEN].dcvDiffuse.r = 0.0f;
    m_pMaterials[MATERIAL_GREEN].dcvDiffuse.g = 1.0f;
    m_pMaterials[MATERIAL_GREEN].dcvDiffuse.b = 0.0f;
    m_pMaterials[MATERIAL_GREEN].dcvDiffuse.a = 1.0f;

    memcpy(&m_pMaterials[MATERIAL_BLUE], &m_pMaterials[MATERIAL_WHITE], sizeof(MATERIAL));
    m_pMaterials[MATERIAL_BLUE].dcvDiffuse.r = 0.0f;
    m_pMaterials[MATERIAL_BLUE].dcvDiffuse.g = 0.0f;
    m_pMaterials[MATERIAL_BLUE].dcvDiffuse.b = 1.0f;
    m_pMaterials[MATERIAL_BLUE].dcvDiffuse.a = 1.0f;

    memcpy(&m_pMaterials[MATERIAL_YELLOW], &m_pMaterials[MATERIAL_WHITE], sizeof(MATERIAL));
    m_pMaterials[MATERIAL_YELLOW].dcvDiffuse.r = 230.0f / 255.0f;
    m_pMaterials[MATERIAL_YELLOW].dcvDiffuse.g = 1.0f;
    m_pMaterials[MATERIAL_YELLOW].dcvDiffuse.b = 20.0f / 255.0f;
    m_pMaterials[MATERIAL_YELLOW].dcvDiffuse.a = 1.0f;

	memcpy(&m_pMaterials[MATERIAL_CYAN], &m_pMaterials[MATERIAL_WHITE], sizeof(MATERIAL));
    m_pMaterials[MATERIAL_CYAN].dcvDiffuse.r = 0.0f;
    m_pMaterials[MATERIAL_CYAN].dcvDiffuse.g = 205.0f / 255.0f;
    m_pMaterials[MATERIAL_CYAN].dcvDiffuse.b = 205.0f / 255.0f;
    m_pMaterials[MATERIAL_CYAN].dcvDiffuse.a = 1.0f;

	memcpy(&m_pMaterials[MATERIAL_ORANGE], &m_pMaterials[MATERIAL_WHITE], sizeof(MATERIAL));
    m_pMaterials[MATERIAL_ORANGE].dcvDiffuse.r = 238.0f / 255.0f;
    m_pMaterials[MATERIAL_ORANGE].dcvDiffuse.g = 118.0f / 255.0f;
    m_pMaterials[MATERIAL_ORANGE].dcvDiffuse.b = 0.0f;
    m_pMaterials[MATERIAL_ORANGE].dcvDiffuse.a = 1.0f;

    if (!SetMaterial(&m_pMaterials[MATERIAL_RED]))
    {
        WriteToLog("SetMaterial() failed (red): %s.\n", GLE(GetLastError()));
        return false;
    }

    if (!SetMaterial(&m_pMaterials[MATERIAL_GREEN]))
    {
        WriteToLog("SetMaterial() failed (green): %s.\n", GLE(GetLastError()));
        return false;
    }

    if (!SetMaterial(&m_pMaterials[MATERIAL_BLUE]))
    {
        WriteToLog("SetMaterial() failed (blue): %s.\n", GLE(GetLastError()));
        return false;
    }

    if (!SetMaterial(&m_pMaterials[MATERIAL_WHITE]))
    {
        WriteToLog("SetMaterial() failed (white): %s.\n", GLE(GetLastError()));
        return false;
    }

    if (!SetMaterial(&m_pMaterials[MATERIAL_YELLOW]))
    {
        WriteToLog("SetMaterial() failed (yellow): %s.\n", GLE(GetLastError()));
        return false;
    }

    if (!SetMaterial(&m_pMaterials[MATERIAL_CYAN]))
    {
        WriteToLog("SetMaterial() failed (cyan): %s.\n", GLE(GetLastError()));
        return false;
    }

    if (!SetMaterial(&m_pMaterials[MATERIAL_ORANGE]))
    {
        WriteToLog("SetMaterial() failed (orange): %s.\n", GLE(GetLastError()));
        return false;
    }


    return true;
}

//-----------------------------------------------------------------------------
//	Name:       SetupMatrices()
//	Desc:       setup the matrices
//	Returns:	true if successful, false if not
//-----------------------------------------------------------------------------

bool CZBias::SetupMatrices ()
{
    D3DXMATRIX Matrix;
    D3DVALUE dvAspect = ((float) m_pCurrentMode->nBackBufferWidth) /
                        ((float) m_pCurrentMode->nBackBufferHeight);
    D3DXVECTOR3 From (0.0f, 0.0f, -10.0f);
    D3DXVECTOR3 At (0.0f, 0.0f, 0.0f);
    D3DXVECTOR3 Up (0.0f, 1.0f, 0.0f);

    D3DXMatrixIdentity (&Matrix);

    if (!SetTransform (D3DTS_WORLD, (D3DMATRIX *) &Matrix))
    {
        WriteToLog("SetTransform (WORLD) failed: %s.\n", GLE (GetLastError()));
        return false;
    }

    D3DXMatrixPerspectiveFovLH (&m_Projection, VIEW_ANGLE, dvAspect, VIEW_NEAR, VIEW_FAR);

    if (!SetTransform (D3DTS_PROJECTION, (D3DMATRIX *) &m_Projection))
    {
        WriteToLog("SetTransform (PROJECTION) failed: %s.\n", GLE (GetLastError()));
        return false;
    }

    D3DXMatrixLookAtLH (&m_View, &From, &At, &Up);

    if (!SetTransform (D3DTS_VIEW, (D3DMATRIX *) &m_View))
    {
        WriteToLog("SetTransform (VIEW) failed: %s.\n", GLE (GetLastError()));
        return false;
    }

    return true;
}

//-----------------------------------------------------------------------------
//	Name:		SetupRenderStates()
//	Desc:		setup D3D render states
//	Returns:	true if successful, false if not
//-----------------------------------------------------------------------------

bool CZBias::SetupRenderStates ()
{
    SetRenderState(D3DRS_COLORVERTEX, (DWORD) TRUE);
    SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, D3DMCS_MATERIAL);
    SetRenderState(D3DRS_LIGHTING, (DWORD) TRUE);
    SetRenderState(D3DRS_ZFUNC, (DWORD) D3DCMP_LESS);

	SetRenderState (D3DRS_AMBIENT, 0xFFFFFFFF);

    SetTextureStageState(0, D3DTSS_COLOROP,   (DWORD) D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_COLORARG1, (DWORD) D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_COLORARG2, (DWORD) D3DTA_TEXTURE);
    SetTextureStageState(0, D3DTSS_ALPHAOP,   (DWORD) D3DTOP_SELECTARG1);
    SetTextureStageState(0, D3DTSS_ALPHAARG1, (DWORD) D3DTA_DIFFUSE);
    SetTextureStageState(0, D3DTSS_ALPHAARG2, (DWORD) D3DTA_TEXTURE); 
    
	SetTexture(0, NULL);

    SetTextureStageState(1, D3DTSS_COLOROP, (DWORD) D3DTOP_DISABLE);
    SetTextureStageState(1, D3DTSS_ALPHAOP, (DWORD) D3DTOP_DISABLE);

    return (D3D_OK == GetLastError()) ? true : false;
}

//-----------------------------------------------------------------------------
//	Name:		SetupShapes()
//	Desc:		creates the meshes
//	Returns:	true if successful, false if not
//-----------------------------------------------------------------------------

bool CZBias::SetupShapes ()
{
	//setup the spheres
    if (!m_Sphere.NewShape(CS_SPHERE, 24, 12, 0))
    {
        WriteToLog("Unable to create sphere.\n");
        return false;
    }
    if (!m_FarSphere.NewShape(CS_SPHERE, 24, 12, 0))
    {
        WriteToLog("Unable to create far sphere.\n");
        return false;
    }

	//setup the planes
	if (!m_Plane.NewShape(CS_MESH, 24, 24, 0))
    {
        WriteToLog("Unable to create plane.\n");
        return false;
    }
    if (!m_FarPlane.NewShape(CS_MESH, 24, 24, 0))
    {
        WriteToLog("Unable to create far plane.\n");
        return false;
    }

    m_Sphere.Scale (12.0);
	m_Plane.Scale (18.0f, 18.0f, 1.0f);

	m_FarSphere.Scale (12.0f * SCALE_VALUE);
	m_FarPlane.Scale (18.0f * SCALE_VALUE, 18.0f * SCALE_VALUE, 1.0f);
	m_FarSphere.Translate (0.0f, 0.0f, TRANSLATE_VALUE);
	m_FarPlane.Translate (0.0f, 0.0f, TRANSLATE_VALUE);

    return true;
}

//-----------------------------------------------------------------------------
//	Name:		SetupFactors()
//	Desc:		initializes the factor arrays
//	Returns:	true if successful, false if not
//-----------------------------------------------------------------------------
#ifdef FAST_TEST
bool CZBias::SetupFactors()
{
	float tempBFact[NBIASTESTS][2] =
	{
		{ 0.100f, -0.500f },
		{ -0.100f, 0.500f },
		{ 0.010f, 0.400f },
	};

	UINT uBiasIndices[NBIASTESTS] = { 0, 1, 2 };

	//setup the slope scale test factors
	float tempSSFact[NSLOPESCALETESTS][4] =
	{
		{ -10.0f, 0.0f, 0.0001f, 0.0f },
		{ -10.0f, 0.0f, 0.001f, 0.0f },
		{ -10.0f, 20.0f, 0.0025f, 0.0f },		//0.005 for these two cases seems to be
		{ -10.0f, 0.0f, 0.0f, 0.0025f },	//a bit extreme for -10 increment

		{ -1.0f, 0.0f, 0.0001f, 0.0f },	//5
		{ -1.0f, 0.0f, 0.001f, 0.0f },
		{ -1.0f, 0.0f, 0.005f, 0.0f },
		{ -1.0f, 0.0f, 0.0f, 0.005f },

		{ -0.1f, 0.0f, 0.0005f, 0.0f },
		{ -0.1f, 0.0f, 0.001f, 0.0f },	//10
		{ -0.1f, 0.0f, 0.005f, 0.0f },
		{ -0.1f, 0.0f, 0.0f, 0.005f },

		{ 0.0f, 0.0f, 0.001f, 0.0f },
		{ 0.0f, 0.0f, 0.0f, 0.001f },
		{ 0.0f, 0.0f, 0.0f, 0.0f },		//15
	};

	UINT uSSIndices[NSLOPESCALETESTS] = { 0, 0, 7, 0,
		1, 1, 1, 1,
		2, 2, 2, 2,
		3, 3, 3
	};

	//setup the primitve tests factors
	float tempFactors[NTEST_VALUES][2] =
	{
		{ -0.001f, -0.01f },
		{ -0.001f, 0.0f },
		{ -0.001f, 0.01f },
		{ 0.0f, -0.01f },
		{ 0.0f, 0.0f },
	};

	memcpy(fpBiasFactors, tempBFact, sizeof(float) * NBIASTESTS * 2);
	memcpy(upBiasIndices, uBiasIndices, sizeof(UINT) * NBIASTESTS);
	memcpy(fpSSFactors, tempSSFact, sizeof(float) * NSLOPESCALETESTS * 4);
	memcpy(upSSIndices, uSSIndices, sizeof(UINT) * NSLOPESCALETESTS);
	memcpy(fpFactors, tempFactors, sizeof(float) * NTEST_VALUES * 2);

	return true;
}
#else
bool CZBias::SetupFactors()
{
	//setup the bias test factors
	float tempBFact[NBIASTESTS][2] = 
	{
		{  0.100f, -0.500f },
		{ -0.100f,  0.500f },
		{  0.010f,  0.400f },
		{ -0.010f, -0.400f },
		{  0.001f,  0.490f },
		{ -0.001f, -0.490f }
	};

	memcpy (fpBiasFactors, tempBFact, sizeof (float) * NBIASTESTS * 2);

	UINT uBiasIndices[NBIASTESTS] = {0, 1, 2, 3, 4, 5 };

	memcpy (upBiasIndices, uBiasIndices, sizeof (UINT) * NBIASTESTS );

	//setup the slope scale test factors
	float tempSSFact[NSLOPESCALETESTS][4] = 
	{
		{-10.0f,  0.0f,  0.0001f,	0.0f   },
		{-10.0f,  0.0f,  0.001f,	0.0f   },
		{-10.0f, 20.0f,  0.0025f,	0.0f   },		//0.005 for these two cases seems to be
		{-10.0f,  0.0f,  0.0f,		0.0025f},	//a bit extreme for -10 increment

		{-1.0f,	  0.0f,  0.0001f,	0.0f  },	//5
		{-1.0f,	  0.0f,  0.001f,	0.0f  },
		{-1.0f,	  0.0f,  0.005f,	0.0f  },
		{-1.0f,	  0.0f,  0.0f,		0.005f},

		{-0.1f,	  0.0f,  0.0005f,	0.0f  },
		{-0.1f,	  0.0f,  0.001f,	0.0f  },	//10
		{-0.1f,	  0.0f,  0.005f,	0.0f  },
		{-0.1f,	  0.0f,  0.0f,		0.005f},

		{0.0f,	  0.0f,  0.001f,	0.0f  },	
		{0.0f,	  0.0f,  0.0f,		0.001f},
		{0.0f,	  0.0f,  0.0f,		0.0f  },		//15

		{0.1f,	  0.0f,  0.0001f,	0.0f  },
		{0.1f,	  0.0f,  0.001f,	0.0f  },
		{0.1f,	  0.0f,  0.005f,	0.0f  },
		{0.1f,	  0.0f,  0.0f,		0.005f},

		{1.0f,	  0.0f,  0.0001f,	0.0f   },	//20
		{1.0f,	  0.0f,  0.001f,	0.0f   },
		{1.0f,	  0.0f,  0.005f,	0.0f   },
		{1.0f,	  0.0f,  0.0f,		0.0001f},
		{1.0f,	  0.0f,  0.0f,		0.001f },
		{ 1.0f, 0.0f, 0.0f, 0.005f },//25

		{ 10.0f, 0.0f, 0.0001f, 0.0f },
		{ 10.0f, 0.0f, 0.001f, 0.0f },
		{ 10.0f, 0.0f, 0.005f, 0.0f },
		{ 10.0f, 0.0f, 0.0f, 0.005f },//29
};
	memcpy(fpSSFactors, tempSSFact, sizeof(float) * NSLOPESCALETESTS * 4);

	UINT uSSIndices[NSLOPESCALETESTS] = { 0, 0, 7, 0,
		1, 1, 1, 1,
		2, 2, 2, 2,
		3, 3, 3,
		4, 4, 4, 4,
		5, 5, 5, 5, 5, 5,
		6, 6, 6, 6 };

	memcpy(upSSIndices, uSSIndices, sizeof(UINT) * NSLOPESCALETESTS);

	//setup the primitve tests factors
	float tempFactors[NTEST_VALUES][2] =
	{
		{ -0.001f, -0.01f },
		{ -0.001f, 0.0f },
		{ -0.001f, 0.01f },
		{ 0.0f, -0.01f },
		{ 0.0f, 0.0f },
		{ 0.0f, 0.01f },
		{ 0.001f, -0.01f },
		{ 0.001f, 0.0f },
		{ 0.001f, 0.01f }
	};
	memcpy(fpFactors, tempFactors, sizeof(float) * NTEST_VALUES * 2);

	return true;
}
#endif

//-----------------------------------------------------------------------------
//	Name:		TestInitialize()
//	Desc:		initialize test - check flags for support and set group strings
//	Returns:	appropriate D3DTESTINIT code
//-----------------------------------------------------------------------------

UINT CZBias::TestInitialize ()
{
    //set the test range
    SetTestRange((UINT) 1, NTESTS);

	if (m_Flags.Check (CF_WENABLED))
	{
		WriteToLog ("Slope scale biasing is undefined for w-buffers.\n");

		//skip the slope scale tests
		//note that this assumes slope scale tests are first
		SkipTestRange (1, NSLOPESCALETESTS);
	}

	//elimate the fog test if fog is not supported
	if (!m_Flags.Check (CF_FOG))
	{
		WriteToLog ("Device does not support fog.\n");

		//skip the fog tests
		//note that this assumes fog tests are second to last (behind pixel shader tests)
		SkipTestRange (NTESTS - NFOGTESTS - NSHADERTESTS + 1, NTESTS - NSHADERTESTS);
	}

	if (!m_Flags.Check (CF_PIXELSHADER))
	{
		WriteToLog ("Device does not support minimum pixel shader version (ps_2_0).\n");

		//skip the shader tests
		//note that this assumes shader tests are last
		SkipTestRange (NTESTS - NSHADERTESTS + 1, NTESTS);
	}

	return D3DTESTINIT_RUN;
}

//-----------------------------------------------------------------------------
//	Name:		ExecuteTest()
//	Desc:		Executes the test associated with each test number
//	Returns:	true if executed successfully, false if not
//	Arguments:	uTest - the test number to execute
//	Outputs:	prints to the log any errors or failures that may occur
//	Comments:	See zbias spec for more information on test cases
//-----------------------------------------------------------------------------

bool CZBias::ExecuteTest (UINT uTest)
{
    bool bResult = true;
    FLOATDWORD Bias, Scale;
	int testNum = (int) uTest;
	D3DVALUE dvAspect;

	GenerateTestName(testNum);

	//begin the test
	BeginTestCase(m_szBuffer, uTest);

    if (BeginScene())
    {
		testNum = uTest;	

		if (testNum <= NSLOPESCALETESTS && testNum > 0)
		{
			bResult = ExecuteSlopeScaleTest (upSSIndices[testNum-1], fpSSFactors[testNum-1][0], fpSSFactors[testNum-1][1], 
											 fpSSFactors[testNum-1][2], fpSSFactors[testNum-1][3]);
		}

		testNum -= NSLOPESCALETESTS;

		if (testNum <= NBIASTESTS && testNum > 0)
		{
			bResult = ExecuteBiasTest (upBiasIndices[testNum-1], fpBiasFactors[testNum-1][0], fpBiasFactors[testNum-1][1]);
		}

		testNum -= NBIASTESTS;

		//determine which test case should be executed
		if (testNum <= (NTEST_VALUES * NTEST_TYPES) && testNum > 0)
		{
			Bias.f = fpFactors[(testNum-1)%NTEST_VALUES][0];
			
			//for w-buffers slope scale is undefined -> we'll use 0 as slope scale value
			if (m_Flags.Check (CF_WENABLED))
				Scale.f = 0.f;
			else
				Scale.f = fpFactors[(testNum-1)%NTEST_VALUES][1];
	
			switch ((testNum-1)/NTEST_VALUES)
			{
			case SPHERE_LINE_TEST:
				bResult = ExecuteSphereLineTest (Bias, Scale);
				break;
			case LINE_SPHERE_TEST:
				bResult = ExecuteLineSphereTest (Bias, Scale);
				break;
			case PLANE_POINT_TEST:
				bResult = ExecutePlanePointTest (Bias, Scale);
				break;
			case POINT_PLANE_TEST:
				bResult = ExecutePointPlaneTest (Bias, Scale);
				break;
			case PLANE_SPHERE_TEST:
				bResult = DrawPlaneSphere (Bias, Scale, (CShapes *) &m_Plane, (CShapes *) &m_Sphere);
				break;
			case FAR_PLANE_SPHERE_TEST:
				//if we are doing the far plane & sphere test, switch to a large near / far view plane ratio
				dvAspect = ((float) m_pCurrentMode->nBackBufferWidth) / ((float) m_pCurrentMode->nBackBufferHeight);
				D3DXMatrixPerspectiveFovLH(&m_Projection, VIEW_ANGLE, dvAspect, VIEW_NEAR, LARGE_VIEW_FAR);

				if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &m_Projection))
				{
					WriteToLog("SetTransform(PROJECTION) failed: %s.\n", GLE(GetLastError()));
					Fail();
					bResult = false;
				}

				bResult = DrawPlaneSphere (Bias, Scale, (CShapes *) &m_FarPlane, (CShapes *) &m_FarSphere);
				break;
			default:
				WriteToLog ("Unknown test case.\n");
				Fail();
				bResult = false;
				break;
			}
		}

		testNum -= (NTEST_VALUES * NTEST_TYPES);

		if (testNum <= NFOGTESTS && testNum > 0)
		{
			bResult = ExecuteFogTest();
		}

		testNum -= NFOGTESTS;

		if (testNum <= NSHADERTESTS && testNum > 0)
		{
			bResult = ExecutePixelShaderText();
		}

		testNum -= NSHADERTESTS;


		//end the scene
        if (!EndScene())
        {
            WriteToLog("EndScene() failed: %s.\n", GLE(GetLastError()));
            Fail();
            bResult = false;
        }
    }
    else
    {
        WriteToLog("BeginScene() failed: %s.\n", GLE(GetLastError()));
        Fail();
        bResult = false;
    }

	//if we executed the far plane & sphere test, we need to switch back to a normal near/far view plane ratio
	if (testNum > 0 && ((testNum-1)/NTEST_VALUES) == FAR_PLANE_SPHERE_TEST)
	{
		dvAspect = ((float) m_pCurrentMode->nBackBufferWidth) / ((float) m_pCurrentMode->nBackBufferHeight);

		D3DXMatrixPerspectiveFovLH(&m_Projection, VIEW_ANGLE, dvAspect, VIEW_NEAR, VIEW_FAR);

		if (!SetTransform(D3DTS_PROJECTION, (D3DMATRIX *) &m_Projection))
		{
			WriteToLog("SetTransform(PROJECTION) failed: %s.\n", GLE(GetLastError()));
			Fail();
			bResult = false;
		}
	}

	//output the meshes 'm' values to the status buffer
//	sprintf(m_szStatus[0][0], "$gPlane rates");
//	sprintf(m_szStatus[0][1], "$yDzDxMin %f, DzDxMax %f, DzDyMin %f, DzDyMax %f", Plane.fDzDxMin, Plane.fDzDxMax, Plane.fDzDyMin, Plane.fDzDyMax);
//	sprintf(m_szStatus[1][0], "$gSphere rates");
//	sprintf(m_szStatus[1][1], "$y%f,%f,%f,%f", Sphere.fDzDxMin, Sphere.fDzDxMax, Sphere.fDzDyMin, Sphere.fDzDyMax);

	return bResult;
}

//-----------------------------------------------------------------------------
//	Name:		GetTestName()
//	Desc:		generates name of the current test case
//	Arguments:	uTest- current test case value
//	Returns:	void
//-----------------------------------------------------------------------------

void CZBias::GenerateTestName(int testNum)
{
	if (testNum <= NSLOPESCALETESTS && testNum > 0)
	{
		_stprintf (m_szBuffer, _T("Slope Scale test: inc %3.2f start %3.2f dzdx %3.4f dzdy %3.4f"),  
								fpSSFactors[testNum-1][0], fpSSFactors[testNum-1][1], 	
								fpSSFactors[testNum-1][2], fpSSFactors[testNum-1][3]);		
	}

	testNum -= NSLOPESCALETESTS;

	if (testNum <= NBIASTESTS && testNum > 0)
	{
		_stprintf (m_szBuffer, _T("Bias test: increment %f, start %f"), fpBiasFactors[testNum-1][0], fpBiasFactors[testNum-1][1]);
	}

	testNum -= NBIASTESTS;

	//determine which test case message should be displayed
	if (testNum <= (NTEST_VALUES * NTEST_TYPES) && testNum > 0)
	{
		//lookup primitive tests proper factor
		FLOAT fBias, fScale;

		fBias  = fpFactors[(testNum-1)%NTEST_VALUES][0];
		//for w-buffers slope scale is undefined -> we'll use 0 as slope scale value
		if (m_Flags.Check (CF_WENABLED))
			fScale = 0.f;
		else
			fScale = fpFactors[(testNum-1)%NTEST_VALUES][1];

		switch ((testNum-1)/NTEST_VALUES)
		{
		case SPHERE_LINE_TEST:
			_stprintf (m_szBuffer, _T("Biased lines: bias %3.4f, scale %3.4f"), fBias, fScale);
			break;
		case LINE_SPHERE_TEST:
			_stprintf (m_szBuffer, _T("Biased sphere: bias %3.4f, scale %3.4f"), fBias, fScale);
			break;
		case PLANE_POINT_TEST:
			_stprintf (m_szBuffer, _T("Biased points: bias %3.4f, scale %3.4f"), fBias, fScale);
			break;
		case POINT_PLANE_TEST:
			_stprintf (m_szBuffer, _T("Biased plane: bias %3.4f, scale %3.4f"), fBias, fScale);
			break;
		case PLANE_SPHERE_TEST:
			_stprintf (m_szBuffer, _T("Biased wireframe & point: bias %3.4f, scale %3.4f"), fBias, fScale);
			break;
		case FAR_PLANE_SPHERE_TEST:
			_stprintf (m_szBuffer, _T("Far wireframe & point: bias %3.4f, scale %3.4f"), fBias, fScale);
			break;
		default:
			break;
		}
	}

	testNum -= (NTEST_VALUES * NTEST_TYPES);

	if (testNum <= NFOGTESTS && testNum > 0)
	{
		_stprintf (m_szBuffer, _T("Fog test: top bias -0.001, bottom bias 0.001"));
	}

	testNum -= NFOGTESTS;

	if (testNum <= NSHADERTESTS && testNum > 0)
	{
		_stprintf (m_szBuffer, _T("Pixel shader depth test."));
	}
}

//-----------------------------------------------------------------------------
//	Name:		ExecuteLineSphereTest()
//	Desc:		Renders the sphere as a let of non biased line primitives, then
//				again as a biased solid sphere
//	Arguments:	bias - value to bias the solid sphere by
//				scale - value to set to 'slope scale' 
//	Returns:	true if successful, false if not
//	Outputs:	prints to the log any failures that may occur
//-----------------------------------------------------------------------------

bool CZBias::ExecuteLineSphereTest (FLOATDWORD bias, FLOATDWORD scale)
{
	bool testResult = true;
	FLOATDWORD tempZero;
	tempZero.f = 0.0f;

	//turn zbiasing off
	SetRenderState (D3DRS_DEPTHBIAS, tempZero.dw);
    SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, tempZero.dw);

	//draw the non biased lines
	SetMaterial (&m_pMaterials[MATERIAL_WHITE]);
    if (!RenderPrimitive (D3DPT_LINELIST, D3DFMT,
                            m_Sphere.m_pVertices, m_Sphere.m_nVertices,
                            m_Sphere.m_pIndices, m_Sphere.m_nIndices, NULL))
    {
        WriteToLog("RenderPrimitive() failed (sphere): %s.\n", GLE(GetLastError()));
        Fail();
        testResult = false;
    }

	//turn zbiasing on
	SetRenderState (D3DRS_DEPTHBIAS, bias.dw);
    SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, scale.dw);

	//draw the biased sphere
	SetMaterial (&m_pMaterials[MATERIAL_RED]);
    SetRenderState (D3DRS_FILLMODE, D3DFILL_SOLID);
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT,
                            m_Sphere.m_pVertices, m_Sphere.m_nVertices,
                            m_Sphere.m_pIndices, m_Sphere.m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (sphere): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	return testResult;
}

//-----------------------------------------------------------------------------
//	Name:		ExecuteSphereLineTest()
//	Desc:		Renders a non biased solid sphere, then again as a set of 
//				biased line primitives.  Also renders a border of 
//				overlapping non biased and biased lines.
//	Arguments:	bias - value to bias the line primitive sphere by
//				scale - value to set to 'slope scale' 
//	Returns:	true if successful, false if not
//	Outputs:	prints to the log any failures that may occur
//-----------------------------------------------------------------------------

bool CZBias::ExecuteSphereLineTest (FLOATDWORD bias, FLOATDWORD scale)
{
	bool testResult = true;
	FLOATDWORD tempZero;
	tempZero.f = 0.0f;
	
	//turn zbiasing off
	SetRenderState (D3DRS_DEPTHBIAS, tempZero.dw);
    SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, tempZero.dw);

	//draw the non biased sphere
	SetMaterial (&m_pMaterials[MATERIAL_WHITE]);
    SetRenderState (D3DRS_FILLMODE, D3DFILL_SOLID);
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT,
                            m_Sphere.m_pVertices, m_Sphere.m_nVertices,
                            m_Sphere.m_pIndices, m_Sphere.m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (sphere): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	//draw the non biased line border
	if (!DrawBorderLinePrimitives (MATERIAL_WHITE))
	{
		Fail();
		testResult = false;
	}

	//turn zbiasing on
	SetRenderState (D3DRS_DEPTHBIAS, bias.dw);
    SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, scale.dw);

	//draw the line overlay
	SetMaterial (&m_pMaterials[MATERIAL_RED]);
    if (!RenderPrimitive (D3DPT_LINELIST, D3DFMT,
                            m_Sphere.m_pVertices, m_Sphere.m_nVertices,
                            m_Sphere.m_pIndices, m_Sphere.m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (lines): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	//draw the biased line border
	if (!DrawBorderLinePrimitives (MATERIAL_BLUE))
	{
		Fail();
		testResult = false;
	}

	return testResult;
}

//-----------------------------------------------------------------------------
//	Name:		ExecutePointPlaneTest()
//	Desc:		Renders a plane as a let of non biased point primitives, then
//				again as a biased solid plane
//	Arguments:	bias - value to bias the solid plane by
//				scale - value to set to 'slope scale' 
//	Returns:	true if successful, false if not
//	Outputs:	prints to the log any failures that may occur
//-----------------------------------------------------------------------------

bool CZBias::ExecutePointPlaneTest (FLOATDWORD bias, FLOATDWORD scale)
{
	bool testResult = true;
	FLOATDWORD tempZero;
	tempZero.f = 0.0f;

	//turn zbiasing off
	SetRenderState (D3DRS_DEPTHBIAS, tempZero.dw);
    SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, tempZero.dw);

	//set the point size
    tempZero.f = 4.0f;
    SetRenderState (D3DRS_POINTSIZE, tempZero.dw);

	//draw the non biased points
	SetMaterial (&m_pMaterials[MATERIAL_WHITE]);
    if (!RenderPrimitive (D3DPT_POINTLIST, D3DFMT, m_Plane.m_pVertices, m_Plane.m_nVertices, NULL, 0, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (points): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	//turn zbiasing on
	SetRenderState (D3DRS_DEPTHBIAS, bias.dw);
    SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, scale.dw);

	//draw the biased plane
	SetMaterial (&m_pMaterials[MATERIAL_RED]);
    SetRenderState (D3DRS_FILLMODE, D3DFILL_SOLID);
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT, m_Plane.m_pVertices, m_Plane.m_nVertices, m_Plane.m_pIndices, m_Plane.m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (plane): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	return testResult;
}

//-----------------------------------------------------------------------------
//	Name:		ExecutePlanePointTest()
//	Desc:		Renders a non biased solid plane, then again as a set of 
//				biased point primitives.  Also renders a border of 
//				overlapping non biased and biased points.	
//	Arguments:	bias - value to bias the point primitives by
//				scale - value to set to 'slope scale' 
//	Returns:	true if successful, false if not
//	Outputs:	prints to the log any failures that may occur
//-----------------------------------------------------------------------------

bool CZBias::ExecutePlanePointTest (FLOATDWORD bias, FLOATDWORD scale)
{
	bool testResult = true;
	FLOATDWORD tempZero;
	tempZero.f = 0.0f;
	
	//turn zbiasing off
	SetRenderState (D3DRS_DEPTHBIAS, tempZero.dw);
    SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, tempZero.dw);

	//draw the non biased plane
	SetMaterial (&m_pMaterials[MATERIAL_WHITE]);
    SetRenderState (D3DRS_FILLMODE, D3DFILL_SOLID);
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT,
                            m_Plane.m_pVertices, m_Plane.m_nVertices,
                            m_Plane.m_pIndices, m_Plane.m_nIndices, NULL))
    {
        WriteToLog("RenderPrimitive() failed (plane): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	//draw the non biased point border
	if (!DrawBorderPointPrimitives (MATERIAL_WHITE))
	{
		Fail();
		testResult = false;
	}

	//set the point size
    tempZero.f = 4.0f;
    SetRenderState (D3DRS_POINTSIZE, tempZero.dw);

	//turn zbiasing on
	SetRenderState (D3DRS_DEPTHBIAS, bias.dw);
    SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, scale.dw);

	//draw the point overlay
	SetMaterial (&m_pMaterials[MATERIAL_RED]);
    if (!RenderPrimitive (D3DPT_POINTLIST, D3DFMT, m_Plane.m_pVertices, m_Plane.m_nVertices, NULL, 0, NULL))
    {
        WriteToLog("RenderPrimitive() failed (points): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	//draw the biased point border
	if (!DrawBorderPointPrimitives (MATERIAL_BLUE))
	{
		Fail();
		testResult = false;
	}

	return testResult;
}

//-----------------------------------------------------------------------------
//	Name:		DrawPlaneSphere()
//	Desc:		Renders a non biased solid plane and sphere, then renders
//				the plane and sphere in wireframe mode with biasing, then 
//				renders the plane and sphere in point mode with twice the
//				biasing.
//	Arguments:	bias - value to bias the wireframe meshes by
//				scale - value to set to 'slope scale' 
//				pPlane - pointer to a plane mesh
//				pSphere - pointer to a sphere mesh
//	Returns:	true if successful, false if not
//	Outputs:	prints to the log any failures that may occur
//-----------------------------------------------------------------------------

bool CZBias::DrawPlaneSphere (FLOATDWORD bias, FLOATDWORD scale, CShapes * pPlane, CShapes * pSphere)
{
	bool testResult = true;
	FLOATDWORD tempZero;
	tempZero.f = 0.0f;

	//turn zbiasing off
	SetRenderState (D3DRS_DEPTHBIAS, tempZero.dw);
    SetRenderState( D3DRS_SLOPESCALEDEPTHBIAS, tempZero.dw);
	
    SetRenderState (D3DRS_FILLMODE, D3DFILL_SOLID);
    SetMaterial (&m_pMaterials[MATERIAL_RED]);

	//render the solid plane
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT, pPlane->m_pVertices, pPlane->m_nVertices, 
													  pPlane->m_pIndices, pPlane->m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (plane): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

    SetMaterial (&m_pMaterials[MATERIAL_BLUE]);

	//render the solid sphere
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT, pSphere->m_pVertices, pSphere->m_nVertices, 
													  pSphere->m_pIndices, pSphere->m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (sphere): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;        
    }

	//turn zbiasing on
	SetRenderState (D3DRS_DEPTHBIAS, bias.dw);
    SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, scale.dw);

	//set to wireframe mode
    SetRenderState (D3DRS_FILLMODE, D3DFILL_WIREFRAME);
    SetMaterial (&m_pMaterials[MATERIAL_WHITE]);

	//render the wireframe plane
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT, pPlane->m_pVertices, pPlane->m_nVertices, 
													  pPlane->m_pIndices, pPlane->m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (plane): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	//render the wireframe sphere
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT, pSphere->m_pVertices, pSphere->m_nVertices, 
													  pSphere->m_pIndices, pSphere->m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (sphere): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	bias.f *= 2.0f;
	scale.f *= 2.0f;
	SetRenderState (D3DRS_DEPTHBIAS, bias.dw);
    SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, scale.dw);

	//set the point size
	tempZero.f = 4.0f;
	SetRenderState (D3DRS_POINTSIZE, tempZero.dw);

    SetRenderState (D3DRS_FILLMODE, D3DFILL_POINT);
    SetMaterial (&m_pMaterials[MATERIAL_GREEN]);

	//render the plane vertex points
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT, pPlane->m_pVertices, pPlane->m_nVertices, 
													  pPlane->m_pIndices, pPlane->m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (plane): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	//render the sphere vertex points
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT, pSphere->m_pVertices, pSphere->m_nVertices,
													  pSphere->m_pIndices, pSphere->m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (sphere): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	return testResult;
}

//-----------------------------------------------------------------------------
//	Name:		ExecuteFogTest()
//	Desc:		Renders two planes, one with negative y coordinates and one
//				with positive y coordinates.  The bottom plane is positivly
//				biased, and the top plane is negativly biased.  The planes
//				are rendered at an angle through a blue fog field.
//	Returns:	true if successful, false if not
//	Outputs:	prints to the log any failures that may occur
//-----------------------------------------------------------------------------

bool CZBias::ExecuteFogTest ()
{
	FLOATDWORD start, end, temp;
	start.f = 7.0f;
	end.f = 12.0f;
	temp.f = 0.0f;
	bool testResult = true;
	CShapes newPlane;

	//copy the plane
	if (!newPlane.CopyShape (m_Plane))
    {
        WriteToLog ("Unable to copy plane.\n");
        return false;
    }

	// Enable fog blending.
	SetRenderState (D3DRS_FOGENABLE, TRUE);

	// Set the fog color. (blue)
	SetRenderState (D3DRS_FOGCOLOR, RGBA_MAKE((DWORD) 0.0f, (DWORD) 0.0f, (DWORD) 255.0f, (DWORD) 255.0f));

	// Set fog parameters.
	SetRenderState (D3DRS_FOGTABLEMODE, D3DFOG_LINEAR);
	SetRenderState (D3DRS_FOGSTART, start.dw);
	SetRenderState (D3DRS_FOGEND, end.dw);

	//rotate the plane, scale it down, and translate it down		
	newPlane.Rotate (0.0f, 20.0f * (pi / 180.0f), 0.0f);
	newPlane.Scale (1.0f, 0.5f, 1.0f);
	newPlane.Translate (0.0f, 4.5f, 0.0f);

	//turn positive zbiasing on 
	temp.f = 0.001f;
	SetRenderState (D3DRS_DEPTHBIAS, temp.dw);
	temp.f = 0.01f;
	SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, temp.dw);
	
    SetRenderState (D3DRS_FILLMODE, D3DFILL_SOLID);
    SetMaterial (&m_pMaterials[MATERIAL_WHITE]);

	//render the solid plane
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT, newPlane.m_pVertices, newPlane.m_nVertices, 
													  newPlane.m_pIndices, newPlane.m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (plane): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	//turn negative zbiasing on
	temp.f = -0.001f;
	SetRenderState (D3DRS_DEPTHBIAS, temp.dw);
	temp.f = -0.01f;
	SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, temp.dw);

	//move the plane up    
	newPlane.Translate (0.0f, -9.0f, 0.0f);
	
    SetMaterial (&m_pMaterials[MATERIAL_WHITE]);

	//render the solid plane
    if (!RenderPrimitive (D3DPT_TRIANGLELIST, D3DFMT, newPlane.m_pVertices, newPlane.m_nVertices, 
													  newPlane.m_pIndices, newPlane.m_nIndices, NULL))
    {
        WriteToLog ("RenderPrimitive() failed (plane): %s.\n", GLE (GetLastError()));
        Fail();
        testResult = false;
    }

	//turn off fog
	SetRenderState (D3DRS_FOGENABLE, FALSE);

	return testResult;
}

//-----------------------------------------------------------------------------
//	Name:		ExecutePixelShaderText()
//	Desc:		Renders two planes.  The green plane is rendered with no biasing
//				and no shader at a depth of 0.5.  The red plane is rendered 
//				with a bias value of -0.1 and a shader that sets the depth to
//				0.55.  The shader should completly override the biasing, so the
//				red plane should appear behind the green plane.
//	Returns:	true if successful, false if not
//	Outputs:	prints to the log any failures that may occur
//-----------------------------------------------------------------------------

bool CZBias::ExecutePixelShaderText ()
{
	bool bReturn = true;
	CnEffect* pEffect = NULL;
	LPD3DXBUFFER lpErrorBuffer = NULL;
    FLEXVERT pRectOne[6];
	FLEXVERT pRectTwo[6];
    float fWidth = (float) m_pCurrentMode->nBackBufferWidth;
    float fHeight = (float) m_pCurrentMode->nBackBufferHeight;
	FLOATDWORD tempZero;
	tempZero.f = 0.0f;

	//make the rectangles - one should be short and wide, the other tall and narrow
	pRectOne[0] = FLEXVERT (fWidth / 2.0f - 25.0f, 10.0f,			0.5f, COLOR_GREEN);
	pRectOne[1] = FLEXVERT (fWidth / 2.0f + 25.0f, 10.0f,			0.5f, COLOR_GREEN);
	pRectOne[2] = FLEXVERT (fWidth / 2.0f - 25.0f, fHeight - 10.0f, 0.5f, COLOR_GREEN);
	pRectOne[3] = FLEXVERT (fWidth / 2.0f + 25.0f, fHeight - 10.0f, 0.5f, COLOR_GREEN);
	pRectOne[4] = pRectOne[2];
	pRectOne[5] = pRectOne[1];
	pRectTwo[0] = FLEXVERT (10.0f,  		fHeight / 2.0f - 25.0f, 0.5f, COLOR_RED);
	pRectTwo[1] = FLEXVERT (fWidth - 10.0f, fHeight / 2.0f - 25.0f, 0.5f, COLOR_RED);
	pRectTwo[2] = FLEXVERT (10.0f,			fHeight / 2.0f + 25.0f, 0.5f, COLOR_RED);
	pRectTwo[3] = FLEXVERT (fWidth - 10.0f, fHeight / 2.0f + 25.0f, 0.5f, COLOR_RED);
	pRectTwo[4] = pRectTwo[2];
	pRectTwo[5] = pRectTwo[1];

	//make the effect
	if (!CreateEffectFromResource (NULL, "zbias.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &pEffect, &lpErrorBuffer))
	{
		WriteToLog ("CreateEffectFromFile() failed on %s.\n", "zbias.fx");
		if (NULL != lpErrorBuffer)
		{
			WriteToLog ("CreateEffectFromFile() compile errors: %s.\n", lpErrorBuffer->GetBufferPointer());
		}
		FAIL_AND_EXIT;
	}

	//turn zbiasing off
	SetRenderState (D3DRS_DEPTHBIAS, tempZero.dw);
    SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, tempZero.dw);

	//render the first rectangle (non biased, no shader)
	if (!RenderPrimitive (D3DPT_TRIANGLELIST, FLEXFMT, pRectOne, 6, NULL, 0, NULL))
	{
		WriteToLog ("RenderPrimitive() failed (traingles): %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

	//turn zbiasing on
	tempZero.f = -0.1f;
	SetRenderState (D3DRS_DEPTHBIAS, tempZero.dw);

	//turn on the shader 
	if (!pEffect->SetTechniqueByName ("ChangeDepthTech"))
	{
		WriteToLog  ("CnEffect::SetTechnique() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}	
	if (!pEffect->Begin (NULL, 0))
	{
		WriteToLog  ("CnEffect::Begin() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!pEffect->BeginPass (0))
	{
		WriteToLog  ("CnEffect::BeginPass() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!RenderPrimitive (D3DPT_TRIANGLELIST, FLEXFMT, pRectTwo, 6, NULL, 0, 0))
	{
		WriteToLog  ("RenderPrimitive() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!pEffect->EndPass ())
	{
		WriteToLog  ("CnEffect::EndPass() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}
	if (!pEffect->End())
	{
		WriteToLog  ("CnEffect::End() failed: %s.\n", GLE (GetLastError()));
		FAIL_AND_EXIT;
	}

Exit:	
	RELEASE (lpErrorBuffer);
	RELEASE (pEffect);

	return bReturn;
}

//-----------------------------------------------------------------------------
//	Name:		ExecuteBiasTest()
//	Desc:		By default renders twelve overlaping vertical rectangles.  Each
//				rectangle is biased by a larger and larger amount.
//	Arguments:	inc - value to increment the bias amount by for each plane
//				biasStart - value to set the inital bias to
//	Returns:	true if successful, false if not
//	Outputs:	prints to the log any failures that may occur
//-----------------------------------------------------------------------------


bool CZBias::ExecuteBiasTest (UINT index, float inc, float biasStart)
{
	bool testResult = true;
    FLEXVERT pVertices[6];
	UINT nRects = 11;
    float fWidth = (float) m_pCurrentMode->nBackBufferWidth;
    float fHeight = (float) m_pCurrentMode->nBackBufferHeight;
	float fOffset = (fWidth - 20.0f) / (nRects+1);
	FLOATDWORD bias, scale;
	bias.f = biasStart;
	scale.f = 0.0f;

	//create the first plane
	pVertices[0] = FLEXVERT (10.0f,					 10.0f,			  0.5f, COLORARRAY[0]);
	pVertices[1] = FLEXVERT (10.0f + 2.0f * fOffset, 10.0f,			  0.5f, COLORARRAY[0]);
	pVertices[2] = FLEXVERT (10.0f,					 fHeight - 10.0f, 0.5f, COLORARRAY[0]);
	pVertices[3] = FLEXVERT (10.0f + 2.0f * fOffset, fHeight - 10.0f, 0.5f, COLORARRAY[0]);
	pVertices[4] = pVertices[2];
	pVertices[5] = pVertices[1];

	for (UINT i = 0; i < nRects; i++)
	{
		bias.f = gBiases[index][i];
		
		SetRenderState (D3DRS_DEPTHBIAS, bias.dw);
		SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, scale.dw);

		if (!RenderPrimitive (D3DPT_TRIANGLELIST, FLEXFMT, pVertices, 6, NULL, 0, NULL))
		{
			WriteToLog ("RenderPrimitive() failed (traingles): %s.\n", GLE (GetLastError()));
			Fail();
			testResult = false;
		}

		//bias.f += inc;

		//bottom left
		pVertices[0].x += fOffset;				
		pVertices[0].y += 2.0f;
	    pVertices[0].diffuse = COLORARRAY[(i+1)%NCOLORS];

		//bottom right
		pVertices[1].x += fOffset;
		pVertices[1].y += 2.0f;
	    pVertices[1].diffuse = COLORARRAY[(i+1)%NCOLORS];

		//top left
		pVertices[2].x += fOffset;
		pVertices[2].y += -2.0f;
	    pVertices[2].diffuse = COLORARRAY[(i+1)%NCOLORS];

		//top right
		pVertices[3].x += fOffset;
		pVertices[3].y += -2.0f;
	    pVertices[3].diffuse = COLORARRAY[(i+1)%NCOLORS];

		pVertices[4] = pVertices[2];
		pVertices[5] = pVertices[1];
	}

	return testResult;
}

//-----------------------------------------------------------------------------
//	Name:		ExecuteSlopeScaleTest()
//	Desc:		Renders a set of increasingly biased rectangles.  The rectangles
//				have slope equal to dzdx (horizontal) or dzdy (vertical).  If 
//				values are passed for both dzdx and dzdy, dzdx is ignored.  The
//				rectangles each overlap one aother by half, and their vertices
//				depth values are set to create the proper dzdx slope.  The 
//				minimum rectangle size is by default 100, thus the maximum slope
//				value that can be set is 0.01 -> (zmax-zmin / width) = (1-0 / 100)
//	Arguments:	inc - value to increment slope scale by
//				slopeScaleStart - value to set to slope scale initially
//				dzdx - horizontal slope to set the planes to
//				dzdy - vertical slope to set the planes to
//	Returns:	true if successful, false if not
//	Outputs:	prints to the log any failures that may occur
//-----------------------------------------------------------------------------

bool CZBias::ExecuteSlopeScaleTest (UINT index, float inc, float slopeScaleStart, float dzdx, float dzdy)
{
	bool testResult = true;
    FLEXVERT pVertices[6];
	UINT nRects = 8;
	FLOATDWORD bias, scale;
    float fWidth = (float) m_pCurrentMode->nBackBufferWidth;
    float fHeight = (float) m_pCurrentMode->nBackBufferHeight;
	float squareSize = (fWidth - 20.0f) < (fHeight - 20.0f) ? (fWidth - 20.0f) : (fHeight - 20.0f);
	float xSlide = (fWidth - 20.0f) / (float)(nRects + 1);
	float ySlide = (fHeight - 20.0f) / (float)(nRects + 1);
	float minSize = 100.0f;
	float z0, z1;
	
	bias.f = 0.0f;
	scale.f = slopeScaleStart;

	//create horizontal planes if dzdy is zero
	if (dzdy == 0)
	{
		float dz, xSize;

		//check if the desired dzdx value does not exceed what is possible using full width
		//(maximum width of the rectangle is fWidth minus a border size (20)
		if (dzdx < (1.0f / (fWidth - 20.0f)))
		{
			dz = dzdx * (fWidth - 20.0f);
			xSize = fWidth - 20.0f;

			z0 = (1.0f - dz) / 2.0f;
			z1 = z0 + dz;
		}
		//check if the desired dzdx value does not exceed what is possible using minimum width
		else if (dzdx < (1.0f / minSize))
		{
			z0 = dzdx * max(abs(gSlopes[index][0]), abs(gSlopes[index][nRects-1]));
			z1 = 1.f - z0;
			dz = abs(z1 - z0);
			xSize = dz / dzdx;
		}
		//otherwise desired value is above the maximum allowable, so we will just use the maximum
		else
		{
			dz = 1.0f;
			xSize = minSize;

			z0 = (1.0f - dz) / 2.0f;
			z1 = z0 + dz;
		}

		pVertices[0] = FLEXVERT ((fWidth - xSize) / 2.0f,	10.0f,			z0,	 COLORARRAY[0]);	//(0,0) upper left
		pVertices[1] = FLEXVERT (pVertices[0].x + xSize,	10.0f,			z1,	 COLORARRAY[0]);	//(X,0)	upper right
		pVertices[2] = FLEXVERT ((fWidth - xSize) / 2.0f,	2.0f * ySlide,	z0,	 COLORARRAY[0]);	//(0,Y) lower left
		pVertices[3] = FLEXVERT (pVertices[2].x + xSize,	2.0f * ySlide,	z1,	 COLORARRAY[0]);	//(X,Y) lower right
	}
	else
	{
		float dz, ySize;
		//check if the desired dzdy value does not exceed what is possible using full height
		if (dzdy < (1.0f / (fHeight - 20.0f)))
		{
			dz = dzdy * (fHeight - 20.0f);
			ySize = fHeight - 20.0f;
			
			z0 = (1.0f - dz) / 2.0f;
			z1 = z0 + dz;
		}
		//check if the desired dzdy value does not exceed what is possible using minimum size
		else if (dzdy < (1.0f / minSize))
		{
			z0 = dzdy * max(abs(gSlopes[index][0]), abs(gSlopes[index][nRects-1]));
			z1 = 1.f - z0;
			dz = abs(z1 - z0);
			ySize = dz / dzdy;
		}
		//otherwise desired value is above the maximum, set it to the max
		else
		{
			dz = 1.0f;
			ySize = minSize;
			
			z0 = (1.0f - dz) / 2.0f;
			z1 = z0 + dz;
		}

		pVertices[0] = FLEXVERT (10.0f,			(fHeight - ySize) / 2.0f,	z0,	COLORARRAY[0]);	//(0,0) upper left
		pVertices[1] = FLEXVERT (2.0f * xSlide,	(fHeight - ySize) / 2.0f,	z0,	COLORARRAY[0]);	//(X,0)	upper right
		pVertices[2] = FLEXVERT (10.0f,			pVertices[0].y + ySize,		z1,	COLORARRAY[0]);	//(0,Y) lower left
		pVertices[3] = FLEXVERT (2.0f * xSlide,	pVertices[1].y + ySize,		z1,	COLORARRAY[0]);	//(X,Y) lower right
	}

	//create the first plane
	pVertices[4] = pVertices[2];
	pVertices[5] = pVertices[1];
	
	float ullrGrad = (pVertices[3].z - pVertices[0].z) / squareSize;
	float llurGrad = (pVertices[1].z - pVertices[2].z) / squareSize;

	for (UINT i = 0; i < nRects; i++)
	{
		scale.f = gSlopes[index][i];
		
		SetRenderState (D3DRS_DEPTHBIAS, bias.dw);
		SetRenderState (D3DRS_SLOPESCALEDEPTHBIAS, scale.dw);
		
		if (!RenderPrimitive (D3DPT_TRIANGLELIST, FLEXFMT, pVertices, 6, NULL, 0, NULL))
		{
			WriteToLog ("RenderPrimitive() failed (traingles): %s.\n", GLE (GetLastError()));
			Fail();
			testResult = false;
		}

		//scale.f += inc;

		if (dzdy == 0)
		{
			pVertices[0].y += ySlide;
			pVertices[1].y += ySlide;
			pVertices[2].y += ySlide;
			pVertices[3].y += ySlide;
		}
		else if (dzdx == 0)
		{	
			pVertices[0].x += xSlide;
			pVertices[1].x += xSlide;
			pVertices[2].x += xSlide;
			pVertices[3].x += xSlide;
		}

		pVertices[0].diffuse = COLORARRAY[(i+1)%NCOLORS];
		pVertices[1].diffuse = COLORARRAY[(i+1)%NCOLORS];
		pVertices[2].diffuse = COLORARRAY[(i+1)%NCOLORS];
		pVertices[3].diffuse = COLORARRAY[(i+1)%NCOLORS];
		pVertices[4] = pVertices[2];
		pVertices[5] = pVertices[1];
	}

	return testResult;
}
//-----------------------------------------------------------------------------
//	Name:        Cleanup()
//	Desc:        cleanup function
//-----------------------------------------------------------------------------

void CZBias::Cleanup ()
{
}

//-----------------------------------------------------------------------------
//	Name:		UpdateStatus()
//	Desc:		updates the status, i.e. outputs the contents of the 
//				status buffer.				
//-----------------------------------------------------------------------------

void CZBias::UpdateStatus ()
{
    CD3DTest::UpdateStatus();
	   
#if OUTPUT_DEBUG_STATUS

	for (UINT i = 0; i < NSTATUS; i++)
		WriteStatus(m_szStatus[i][0], m_szStatus[i][1]);

#endif
}

//-----------------------------------------------------------------------------
//	Name:		ClearStatus()
//	Desc:		clears the status buffer
//-----------------------------------------------------------------------------

void CZBias::ClearStatus ()
{
    for (UINT i = 0; i < NSTATUS; i++)
    {
        _stprintf(m_szStatus[i][0], _T("$gNA"));
        _stprintf(m_szStatus[i][1], _T("\t$y-"));
    }
}

//-----------------------------------------------------------------------------
//	Name:		DrawBorderPointPrimitives()
//	Desc:		draws a border of point primitives
//	Arguments:	nMaterial - the color to set the points to
//	Returns:	true if successful, false otherwise
//-----------------------------------------------------------------------------

bool CZBias::DrawBorderPointPrimitives (UINT nMaterial)
{
    FLEXVERT pVertices[NVERTICES];
    DWORD dwColor;
    FLOAT fWidth = (FLOAT) m_pCurrentMode->nBackBufferWidth;
    FLOAT fHeight = (FLOAT) m_pCurrentMode->nBackBufferHeight;
    UINT i, j, nVertices;

	//compact the color into a RGBA dword
    dwColor = RGBA_MAKE((DWORD) (255.0*m_pMaterials[nMaterial].diffuse.r),
                        (DWORD) (255.0*m_pMaterials[nMaterial].diffuse.g),
                        (DWORD) (255.0*m_pMaterials[nMaterial].diffuse.b),   
                        (DWORD) (255.0*m_pMaterials[nMaterial].diffuse.a));    

	//set the point size
	float temp = 8.0f;
    SetRenderState (D3DRS_POINTSIZE, *((DWORD*) &temp));

    nVertices = (UINT) 0;
    for (i = 20; i < (m_pCurrentMode->nBackBufferHeight - 12); i += PSPRITE_SPACING)
    {
        pVertices[nVertices].x = PSPRITE_BORDER_OFFSET;
        pVertices[nVertices].y = (D3DVALUE) i;
        pVertices[nVertices].z = 0.5f;
        pVertices[nVertices].rhw = 1.0f / pVertices[0].z;
        pVertices[nVertices].diffuse = dwColor;
        nVertices++;

		//if we have filled the point array before finishing the border, draw the existing
		//points and then empty the point array before continuing
        if (NVERTICES == nVertices)
        {
            if (!RenderPrimitive (D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
            {
                WriteToLog ("RenderPrimitive() failed (points): %s.\n", GLE (GetLastError()));
                return false;
            }

			//shift the points to the other side of the screen
            for (j = 0; j < nVertices; j++)
                pVertices[j].x = fWidth - PSPRITE_BORDER_OFFSET;

            if (!RenderPrimitive (D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
            {
                WriteToLog ("RenderPrimitive() failed (points): %s.\n", GLE (GetLastError()));
                return false;
            }

            nVertices = (UINT) 0;
        }
    }

	//render any remaining points in the buffer
    if (nVertices > 0)
    {
        if (!RenderPrimitive(D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed (points): %s.\n", GLE(GetLastError()));
            return false;
        }

        for (j = 0; j < nVertices; j++)
            pVertices[j].x = fWidth - PSPRITE_BORDER_OFFSET;

        if (!RenderPrimitive(D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed (points): %s.\n", GLE(GetLastError()));
            return false;
        }
    }

	//render another border of pixel size sprites
	temp = 1.0f;
	SetRenderState (D3DRS_POINTSIZE, *((DWORD*) &temp));
   
    nVertices = 0; 
    for (i = 3; i < (m_pCurrentMode->nBackBufferWidth - 3); i++)
    {
        pVertices[nVertices].x = (D3DVALUE) i;
        pVertices[nVertices].y = 3.0f;
        pVertices[nVertices].z = 0.5f;
        pVertices[nVertices].rhw = 1.0f / pVertices[0].z;
        pVertices[nVertices].diffuse = dwColor;
        nVertices++;

        if (NVERTICES == nVertices)
        {
            if (!RenderPrimitive(D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
            {
                WriteToLog("RenderPrimitive() failed (points): %s.\n", GLE(GetLastError()));
                return false;
            }

            for (j = 0; j < nVertices; j++)
                pVertices[j].y = fHeight - 4.0f; 

            if (!RenderPrimitive(D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
            {
                WriteToLog("RenderPrimitive() failed (points): %s.\n", GLE(GetLastError()));
                return false;
            }

            nVertices = (UINT) 0;
        }       
    }

    if (nVertices > 0)
    {
        if (!RenderPrimitive(D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed (points): %s.\n", GLE(GetLastError()));
            return false;
        }

        for (j = 0; j < nVertices; j++)
            pVertices[j].y = fHeight - 4.0f;

        if (!RenderPrimitive(D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed (points): %s.\n", GLE(GetLastError()));
            return false;
        }
    }

	//render the top and bottom rows of this additional border
    nVertices = 0;
    for (i = 3; i < (m_pCurrentMode->nBackBufferHeight - 3); i++)
    {
        pVertices[nVertices].x = 3.0f;
        pVertices[nVertices].y = (D3DVALUE) i;
        pVertices[nVertices].z = 0.5f;
        pVertices[nVertices].rhw = 1.0f / pVertices[0].z;
        pVertices[nVertices].diffuse = dwColor;
        nVertices++;

        if (NVERTICES == nVertices)
        {
            if (!RenderPrimitive(D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
            {
                WriteToLog("RenderPrimitive() failed (points): %s.\n", GLE(GetLastError()));
                return false;
            }

            for (j = 0; j < nVertices; j++)
                pVertices[j].x = fWidth - 4.0f;

            if (!RenderPrimitive(D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
            {
                WriteToLog("RenderPrimitive() failed (points): %s.\n", GLE(GetLastError()));
                return false;
            }

            nVertices = (UINT) 0;
        }
    }

    if (nVertices > 0)
    {
        if (!RenderPrimitive(D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed (points): %s.\n", GLE(GetLastError()));
            return false;
        }

        for (j = 0; j < nVertices; j++)
            pVertices[j].x = fWidth - 4.0f;

        if (!RenderPrimitive(D3DPT_POINTLIST, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
        {
            WriteToLog("RenderPrimitive() failed (points): %s.\n", GLE(GetLastError()));
            return false;
        }
    }

	return true;
}

//-----------------------------------------------------------------------------
//	Name:		DrawBorderLinePrimitives()
//	Desc:		draws several borders of line primitives
//	Arguments:	nMaterial - the color to set the lines to
//	Returns:	true if successful, false otherwise
//-----------------------------------------------------------------------------

bool CZBias::DrawBorderLinePrimitives (UINT nMaterial)
{
    FLEXVERT pVertices[NVERTICES];
    DWORD dwColor;
    FLOAT fWidth = (FLOAT) m_pCurrentMode->nBackBufferWidth;
    FLOAT fHeight = (FLOAT) m_pCurrentMode->nBackBufferHeight;
    UINT nVertices, i;

	//pack the color into a RGBA dword
    dwColor = RGBA_MAKE((DWORD) (255.0*m_pMaterials[nMaterial].diffuse.r),
                        (DWORD) (255.0*m_pMaterials[nMaterial].diffuse.g),
                        (DWORD) (255.0*m_pMaterials[nMaterial].diffuse.b),   
                        (DWORD) (255.0*m_pMaterials[nMaterial].diffuse.a));    

	//declare the lines
    pVertices[0].x = 3.0f;
    pVertices[0].y = 3.0f;
    pVertices[0].z = 0.5f;
    pVertices[0].rhw = 1.0f / pVertices[0].z;
    pVertices[0].diffuse = dwColor;

    pVertices[1].x = fWidth - 3.0f;
    pVertices[1].y = 3.0f;
    pVertices[1].z = 0.5f;
    pVertices[1].rhw = 1.0f / pVertices[1].z;
    pVertices[1].diffuse = dwColor;

    pVertices[2].x = fWidth - 3.0f;
    pVertices[2].y = fHeight - 3.0f;
    pVertices[2].z = 0.5f;
    pVertices[2].rhw = 1.0f / pVertices[2].z;
    pVertices[2].diffuse = dwColor;

    pVertices[3].x = 3.0f;
    pVertices[3].y = fHeight - 3.0f;
    pVertices[3].z = 0.5f;
    pVertices[3].rhw = 1.0f / pVertices[3].z;
    pVertices[3].diffuse = dwColor;

    pVertices[4].x = 3.0f;
    pVertices[4].y = 3.0f;
    pVertices[4].z = 0.5f;
    pVertices[4].rhw = 1.0f / pVertices[0].z;
    pVertices[4].diffuse = dwColor;

    nVertices = (UINT) 5;    
	
	//draw four borders
	for (i = 0; i < 4; i++)
	{
		if (!RenderPrimitive (D3DPT_LINESTRIP, FLEXFMT, pVertices, nVertices, NULL, 0, NULL))
		{
			WriteToLog ("RenderPrimitive() failed (lines): %s.\n", GLE (GetLastError()));
			return false;
		}

		//for each subsequent border, move them inwards slightly
		pVertices[0].x += 3.0f;
		pVertices[0].y += 3.0f;

		pVertices[1].x -= 3.0f;
		pVertices[1].y += 3.0f;

		pVertices[2].x -= 3.0f;
		pVertices[2].y -= 3.0f;

		pVertices[3].x += 3.0f;
		pVertices[3].y -= 3.0f;

		pVertices[4].x += 3.0f;
		pVertices[4].y += 3.0f;
	}

    return true;
}

//-----------------------------------------------------------------------------
//	Name:           GetRange()
//	Desc:           get range
//	Arguments:		pRates - address of RATES data
//	                pShape - address of CShapes data
//	Returns:		true if successful, false if not
//	Comments:		this function is not used in the current final build
//					of the zbias test. It was used only for informational
//					and debugging purposes.
//-----------------------------------------------------------------------------

bool CZBias::GetRange (PRATES pRates, CShapes *pShape)
{
    D3DXMATRIX C;
    D3DXVECTOR3 pTri[3];
    FLOAT fDzDx, fDzDy;
    FLOAT fWidth = (FLOAT) m_pCurrentMode->nBackBufferWidth;
    FLOAT fHeight = (FLOAT) m_pCurrentMode->nBackBufferHeight;
    UINT i, nTriangles;

    if (!pRates || !pShape)
        return false;

    D3DXMatrixMultiply(&C, &m_Projection, &m_View); // M*P*V

    pRates->fDzDxMin =  1.0f;
    pRates->fDzDxMax = -1.0f;
    pRates->fDzDyMin =  1.0f;
    pRates->fDzDyMax = -1.0f;

    nTriangles = pShape->m_nIndices/3;

    for (i = 0; i < nTriangles; i += 3)
    {
        pTri[0].x = pShape->m_pVertices[pShape->m_pIndices[i]].x;
        pTri[0].y = pShape->m_pVertices[pShape->m_pIndices[i]].y;
        pTri[0].z = pShape->m_pVertices[pShape->m_pIndices[i]].z;
        D3DXVec3TransformCoord(&pTri[0], &pTri[0], &C);

        pTri[1].x = pShape->m_pVertices[pShape->m_pIndices[i+1]].x;
        pTri[1].y = pShape->m_pVertices[pShape->m_pIndices[i+1]].y;
        pTri[1].z = pShape->m_pVertices[pShape->m_pIndices[i+1]].z;
        D3DXVec3TransformCoord(&pTri[1], &pTri[1], &C);

        pTri[2].x = pShape->m_pVertices[pShape->m_pIndices[i+2]].y;
        pTri[2].y = pShape->m_pVertices[pShape->m_pIndices[i+2]].x;
        pTri[2].z = pShape->m_pVertices[pShape->m_pIndices[i+2]].z;
        D3DXVec3TransformCoord(&pTri[2], &pTri[2], &C);
        
        if (!GetRates(&fDzDx, &fDzDy, pTri))
        {
            WriteToLog("GetRates() failed.\n");
            return false;
        }

        pRates->fDzDxMin = (pRates->fDzDxMin > fDzDx) ? fDzDx : pRates->fDzDxMin;
        pRates->fDzDxMax = (pRates->fDzDxMax < fDzDx) ? fDzDx : pRates->fDzDxMax;
        pRates->fDzDyMin = (pRates->fDzDyMin > fDzDy) ? fDzDy : pRates->fDzDyMin;
        pRates->fDzDyMax = (pRates->fDzDyMax < fDzDy) ? fDzDy : pRates->fDzDyMax;
    }

    return true;
}

//-----------------------------------------------------------------------------
//	Name:		GetRates()
//	Desc:		get rates
//	Arguments:	pfDzDx - change in z with respect to x
//				pfDzDy - change in z with respect to y
//				pTri - pointer to D3DXVECTOR3 data
//	Comments:   equation of the plane defined by triangle with normal (a,b,c)
//	              ax + by + cz + d = 0
//				  dz/dx = -a/c
//				  dz/dy = -b/c
//			    
//				Note that this function is not used in the current final build
//				of the zbias test.  It was used only for informational and 
//				debugging purposes.
//-----------------------------------------------------------------------------

bool CZBias::GetRates (float *pfDzDx, float *pfDzDy, D3DXVECTOR3 *pTri)
{
    double dTemp;
    D3DXVECTOR3 U, V, N;

    if (!pfDzDx || !pfDzDy || !pTri)
        return false;

    U.x = pTri[1].x - pTri[0].x;
    U.y = pTri[1].y - pTri[0].y;
    U.z = pTri[1].z - pTri[0].z;

    V.x = pTri[2].x - pTri[0].x;
    V.y = pTri[2].y - pTri[0].y;
    V.z = pTri[2].z - pTri[0].z;

    N.x = U.y*V.z - U.z*V.y;
    N.y = U.z*V.x - U.x*V.z;
    N.z = U.x*V.y - U.y*V.x;

    dTemp = N.x*N.x + N.y*N.y + N.z*N.z;

    if (dTemp > ZERO)
    {
        dTemp = 1.0 / sqrt(dTemp);
        N.x = (D3DVALUE) (N.x*dTemp);
        N.y = (D3DVALUE) (N.y*dTemp);
        N.z = (D3DVALUE) (N.z*dTemp);
        *pfDzDx = -N.x/N.z;
        *pfDzDy = -N.y/N.z;
    }
    else
    {
        *pfDzDx = 0.0f;
        *pfDzDy = 0.0f;
    }

    return true;
}
