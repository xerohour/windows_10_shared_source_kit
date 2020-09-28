#include "NonPowTwoStressTest.h"
#include "D3DX9.h"

#define TEST_MULTIPLIER 2

CNonPowTwoStressTest::CNonPowTwoStressTest()
{
	m_szTestName   = _T("NonPowTwo Stress");
	m_szCommandKey = _T("StressTest");

	SetTestType(TESTTYPE_STRESS);

	m_Options.D3DOptions.fMinDXVersion = 9.0f;//dx8 ref only does pow2
	m_Options.D3DOptions.dwIncVersions = DXVERSION_9_OR_91;

	//we skip stress test unless command line is specified
	//thus we set only one mode so that we'll skip only one group
	//we need to use ReadInteger instead of KeySet in case of user wants to run some specific test cases
	int i;
	if ( !ReadInteger("StressTest", &i) )
	{
		m_Options.D3DOptions.dwDevTypes = DEVICETYPE_HAL;
        m_Options.D3DOptions.dwBehaviors = DEVICECREATE_STANDARD | DEVICECREATE_BESTAVAILABLE;
		m_Options.D3DOptions.dwWinMode = WINMODE_WINDOWED;
		m_Options.ModeOptions.TextureFmt 		= FMT_ANY;
		m_Options.ModeOptions.ZBufferFmt 		= FMT_ANY;
		m_Options.ModeOptions.CubeFmt 			= FMT_ANY;
		m_Options.ModeOptions.VolumeFmt 		= FMT_ANY;
		m_Options.ModeOptions.BackBufferFmt		= FMT_ANY;
		m_Options.ModeOptions.MultisampType 	= MULTISAMPLE_ANY;
		m_Options.ModeOptions.uMaxDisplayModes 	= 1;

		m_bSkipAll = true;
	}
	else
		m_bSkipAll = false;

	m_fTime = 0.0f;
}

//------------------------------------------------------------------------------

CNonPowTwoStressTest::~CNonPowTwoStressTest()
{
}

//------------------------------------------------------------------------------

void CNonPowTwoStressTest::Cleanup()
{
	CNonPowTwoTest::Cleanup();

	m_SphereTree.Cleanup();
}

//------------------------------------------------------------------------------

void CNonPowTwoStressTest::SetTestCaseRange(CTestMatrix* pTestMatrix)
{
	//Only run if -StressTest is set
	//we need to use ReadInteger instead of KeySet in case of user wants to run some specific test cases
	int i;
	if (ReadInteger("StressTest", &i))
	{
		pTestMatrix->SetCombinationRange(this, TEST_MULTIPLIER);
	}
	else
	{
		SetTestRange(0, 0);
	}
}

//------------------------------------------------------------------------------

CTestMatrix* CNonPowTwoStressTest::GetTestMatrix()
{
	return &m_TestMatrix;
}

//------------------------------------------------------------------------------

bool CNonPowTwoStressTest::InitGeometry()
{
	m_fTime = 0.0f;

	m_uRectWidth  = m_pCurrentMode->nBackBufferWidth  / 4;
	m_uRectHeight = m_pCurrentMode->nBackBufferHeight / 4;
	
	m_SphereTree.Cleanup();

	D3DXVECTOR3 vCenter(0.0f, 0.0f, 0.0f);

	if(!m_SphereTree.CreateTree(this, 4, 10, 0.2f, 0.5f, vCenter))
	{
		WriteToLog(_T("CNonPowTwoStressTest::InitGeometry - Failed to create the sphere tree\n"));
		return false;
	}

	m_SphereTree.ForEach(InitRotVelCallBack);

	if(!m_SphereTree.ChangeFVF(this, FVF, sizeof(VERTEX), VertexChanger))
	{
		WriteToLog(_T("CNonPowTwoStressTest::InitGeometry - Failed to change sphere FVF"));	
		return false;
	}

	if (!EnableLighting())
	{
		WriteToLog(_T("CNonPowTwoStressTest::InitGeometry - EnableLighting failed."));	
		return false;
	}

	return true;
};

//------------------------------------------------------------------------------

bool CNonPowTwoStressTest::ExecuteTest(UINT nTestNum)
{
	CNonPowTwoTest::ExecuteTest(nTestNum);
	GetLastError();

	return true;
}

//------------------------------------------------------------------------------

bool CNonPowTwoStressTest::RenderScene()
{
	D3DXMATRIX mWorld;
	D3DXMatrixIdentity(&mWorld);

    m_fTime += 0.001f;

	return RenderSphereTree(this, &m_SphereTree, mWorld, m_mView, m_mProj, m_fTime, g_isVShaderCase);
}

//------------------------------------------------------------------------------

bool CNonPowTwoStressTest::SetupTestState(TCHAR *testName, CTestMatrix* pTestMatrix, const UINT nTestNum)
{
	ASSERT(NULL != pTestMatrix);

	return pTestMatrix->SetupCombination(testName, (nTestNum - 1) / TEST_MULTIPLIER, this);
}

//------------------------------------------------------------------------------

bool CNonPowTwoStressTest::ShouldCapsCheck()
{
	//bug 78420 changed return value from false to true
	return true;
}

//------------------------------------------------------------------------------

void InitRotVelCallBack(CSphereTree* pTree)
{
	float fRot = 10.0f / pTree->GetRadius();
	D3DXVECTOR3 vRot(fRot, fRot, fRot);

	pTree->SetRotationalVelocity(vRot);
}

//------------------------------------------------------------------------------

void VertexChanger(const D3DVERTEX& In, void* pOut)
{
	VERTEX * pVertOut = (VERTEX*) pOut;

	pVertOut->Position[0] = In.x;
	pVertOut->Position[1] = In.y;
	pVertOut->Position[2] = In.z;

	pVertOut->Normal[0] = In.nx;
	pVertOut->Normal[1] = In.ny;
	pVertOut->Normal[2] = In.nz;

	pVertOut->_2DCoords[0] = In.tu;
	pVertOut->_2DCoords[1] = In.tv;

	pVertOut->_3DCoords[0] = In.x;
	pVertOut->_3DCoords[1] = In.y;
	pVertOut->_3DCoords[2] = In.z;

	pVertOut->CubeCoords[0] = In.x;
	pVertOut->CubeCoords[1] = In.y;
	pVertOut->CubeCoords[2] = In.z;

}

//------------------------------------------------------------------------------

bool CNonPowTwoStressTest::EnableLighting()
{
    bool bRes = true;

    LIGHT TheLight;

    ZeroMemory(&TheLight, sizeof(LIGHT));

    TheLight.Ambient.a = 0.0f;
    TheLight.Ambient.r = 0.0f;
    TheLight.Ambient.g = 0.0f;
    TheLight.Ambient.b = 0.0f;

    TheLight.Attenuation0 = 1;
    TheLight.Attenuation1 = 1;
    TheLight.Attenuation2 = 1;

    TheLight.Diffuse.a = 0.5f;
    TheLight.Diffuse.r = 0.5f;
    TheLight.Diffuse.g = 0.5f;
    TheLight.Diffuse.b = 0.5f;

    TheLight.Specular.a = 0.5f;
    TheLight.Specular.r = 0.5f;
    TheLight.Specular.g = 0.5f;
    TheLight.Specular.b = 0.5f;

    TheLight.Direction.x = -0.5f;
    TheLight.Direction.y = -0.5f;
    TheLight.Direction.z = 0.3f;

    TheLight.Type = D3DLIGHT_DIRECTIONAL;

    MATERIAL TheMaterial;

    ZeroMemory(&TheMaterial, sizeof(TheMaterial));

    TheMaterial.Ambient.a = 0.0f;
    TheMaterial.Ambient.r = 0.0f;
    TheMaterial.Ambient.g = 0.0f;
    TheMaterial.Ambient.b = 0.0f;

    TheMaterial.Diffuse.a = 0.5f;
    TheMaterial.Diffuse.r = 1.0f;
    TheMaterial.Diffuse.g = 1.0f;
    TheMaterial.Diffuse.b = 1.0f;

    TheMaterial.Emissive.a = 0.0f;
    TheMaterial.Emissive.r = 0.0f;
    TheMaterial.Emissive.g = 0.0f;
    TheMaterial.Emissive.b = 0.0f;

    TheMaterial.Power = 10.0f;

    TheMaterial.Specular.a = 0.2f;
    TheMaterial.Specular.r = 0.2f;
    TheMaterial.Specular.g = 0.2f;
    TheMaterial.Specular.b = 0.2f;

    bRes &= SetLight(0, &TheLight);
    bRes &= LightEnable(0, TRUE);

    bRes &= SetMaterial(&TheMaterial);

    bRes &= SetRenderState(D3DRS_LIGHTING, TRUE);
    bRes &= SetRenderState(D3DRS_AMBIENT, 0xFFFFFFFF);

    return bRes;
}