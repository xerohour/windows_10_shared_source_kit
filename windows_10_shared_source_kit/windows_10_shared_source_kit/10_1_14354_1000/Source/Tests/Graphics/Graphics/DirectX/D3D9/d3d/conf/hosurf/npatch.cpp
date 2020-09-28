// ============================================================================
//
//  TriSurf.cpp
//  Written by robald.
//
// ============================================================================

#include "HOSurf.h"
float  g_NPTess[]  = {
	1.f,
	2.f,
	5.5f,
	10.1f
};

int g_nNPTess = sizeof(g_NPTess) / sizeof(float);

D3DDEGREETYPE g_NormalDegree[] = {
	D3DDEGREE_LINEAR,
	D3DDEGREE_QUADRATIC
};
int g_nNormal = sizeof(g_NormalDegree) / sizeof(float);

D3DDEGREETYPE g_PositionDegree[] = {
	D3DDEGREE_LINEAR,
	D3DDEGREE_CUBIC
};
int g_nPosition = sizeof(g_PositionDegree) / sizeof(float);
																				////////////// AAAAAARRRRGGGHHH!!!   The following is never used!  How misleading is that!
HOTEST g_NPTests[] = {
	{ (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 2, 2, 0, false, false, ONEPATCH, CSPHERE,   D3DPATCHEDGE_DISCRETE,   5.f, 1 },
    { (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 2, 2, 0, false, false, ONEPATCH, CSPHERE,   D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	{ (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 2, 2, 1, true,  true,  ONEPATCH, CSPHERE,   D3DPATCHEDGE_DISCRETE,   5.f, 1 },
    { (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 2, 2, 1, true,  true,  ONEPATCH, CSPHERE,   D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
    { (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 2, 2, 2, true,  true,  ONEPATCH, CSPHERE,   D3DPATCHEDGE_DISCRETE,   5.f, 1 },
    { (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 2, 2, 2, true,  true,  ONEPATCH, CSPHERE,   D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },

	{ (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 0, false, false, ONEPATCH, XTORUS,    D3DPATCHEDGE_DISCRETE,   5.f, 1 },
    { (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 0, false, false, ONEPATCH, XTORUS,    D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	{ (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 1, true,  false, ONEPATCH, XTORUS,    D3DPATCHEDGE_DISCRETE,   5.f, 1 },
    { (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 1, true,  false, ONEPATCH, XTORUS,    D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	{ (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 2, true,  false, ONEPATCH, XTORUS,    D3DPATCHEDGE_DISCRETE,   5.f, 1 },
    { (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 2, true,  false, ONEPATCH, XTORUS,    D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
    
	{ (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 0, false, false, ONEPATCH, XCYLINDER, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
    { (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 0, false, false, ONEPATCH, XCYLINDER, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
    { (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 1, false, false, ONEPATCH, XCYLINDER, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
    { (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 1, false, false, ONEPATCH, XCYLINDER, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	{ (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 2, true,  false, ONEPATCH, XCYLINDER, D3DPATCHEDGE_DISCRETE,   5.f, 1 },
    { (D3DBASISTYPE)0,      (D3DDEGREETYPE)0, 3, 3, 2, true,  false, ONEPATCH, XCYLINDER, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
};
int g_nNPTests = sizeof(g_NPTests) / sizeof(HOTEST);

// ----------------------------------------------------------------------------
   
CNPSurf::CNPSurf()
{
    m_szTestName= _T("HOSurf - N-Patch tests");
	m_szCommandKey= _T("NPATCH");
	m_dwUsage = D3DUSAGE_NPATCHES;
	m_vLoc = D3DXVECTOR3(0.0f, 0.0f, -2.5f);

	memset(&m_RPOptions, 0, sizeof(RPOPTIONS));
	m_RPOptions.Pool = D3DPOOL_SYSTEMMEM;
	m_RPOptions.Usage = D3DUSAGE_NPATCHES;
	m_RPOptions.uIBBitCount = 16;
}

// ----------------------------------------------------------------------------

void CNPSurf::RenderNotify(TCHAR *pcsDraw, TCHAR *pcsVertex)
{
	m_tcsDrawType[1023] = TCHAR(0);
	_tcsncpy(m_tcsDrawType, pcsDraw, 1023);
}

// ----------------------------------------------------------------------------

bool CNPSurf::CheckCaps()
{
    if(!(m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_NPATCHES))
	{
		WriteToLog(_T("CNPSurf::CheckCaps() - D3DDEVCAPS_NPATCHES not set.\n"));
		return false;
	}
	return true;
}

// ----------------------------------------------------------------------------

bool CNPSurf::SetDefaultLights(void)
{
    DWORD dwPosLightsSupported = 1;
    
	// Set the total light count
	m_nLightCount = 2;

    // Create the CLights
    m_pLights = new LIGHT[m_nLightCount];
	memset(m_pLights, 0, sizeof(LIGHT) * m_nLightCount);
    
    // Check what kind of lights are available
    if(m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
    {
        dwPosLightsSupported = (m_pSrcDevice->GetCaps()->dwVertexProcessingCaps & D3DVTXPCAPS_POSITIONALLIGHTS);

        if(!dwPosLightsSupported && !(m_pSrcDevice->GetCaps()->dwVertexProcessingCaps & D3DVTXPCAPS_DIRECTIONALLIGHTS))
        {
            WriteToLog("(FAIL) T&L Device does not support at least Directional Lights.  Test cannot continue.\n");
            return false;
        }
    }                                    

    // Light0
    m_pLights[0].Type             = dwPosLightsSupported ? D3DLIGHT_POINT : D3DLIGHT_DIRECTIONAL;
    m_pLights[0].Diffuse.r        = 1.0f;
    m_pLights[0].Diffuse.g        = 0.0f;
    m_pLights[0].Diffuse.b        = 0.0f;
    m_pLights[0].Ambient.r        = 1.0f;
    m_pLights[0].Ambient.g        = 1.0f;
    m_pLights[0].Ambient.b        = 1.0f;
    m_pLights[0].Specular.r       = 0.0f;
    m_pLights[0].Specular.g       = 0.0f;
    m_pLights[0].Specular.b       = 1.0f;
    m_pLights[0].Position       = D3DXVECTOR3(m_vLoc.x-1.5f, m_vLoc.y, m_vLoc.z);
    m_pLights[0].Direction      = m_vAt;
    m_pLights[0].Attenuation0   = 0.98f;
    m_pLights[0].Attenuation1   = 0.015f;
    m_pLights[0].Attenuation2   = 0.005f;
    m_pLights[0].Range          = 100.0f;
	m_pLights[0].Falloff		   = 1.0f;   
    SetLight(0, &(m_pLights[0]));
	LightEnable(0, TRUE);

    // Light1
	m_pLights[1].Type             = dwPosLightsSupported ? D3DLIGHT_POINT : D3DLIGHT_DIRECTIONAL;
	m_pLights[1].Diffuse.r        = 0.0f;
	m_pLights[1].Diffuse.g        = 1.0f;
	m_pLights[1].Diffuse.b        = 0.0f;
	m_pLights[1].Ambient.r        = 1.0f;
	m_pLights[1].Ambient.g        = 1.0f;
	m_pLights[1].Ambient.b        = 1.0f;
	m_pLights[1].Specular.r       = 0.0f;
	m_pLights[1].Specular.g       = 1.0f;
	m_pLights[1].Specular.b       = 0.0f;
	m_pLights[1].Position       = D3DXVECTOR3(m_vLoc.x+1.5f, m_vLoc.y, m_vLoc.z);
	m_pLights[1].Direction      = m_vAt;
	m_pLights[1].Attenuation0   = 0.98f;
	m_pLights[1].Attenuation1   = 0.015f;
	m_pLights[1].Attenuation2   = 0.005f;
	m_pLights[1].Range          = 100.0f;
	m_pLights[1].Falloff		   = 1.0f;
    SetLight(1, &(m_pLights[1]));
	LightEnable(1, TRUE);

    return true;
}

// ----------------------------------------------------------------------------

bool CNPSurf::SetDefaultMatrices(void)
{   
	D3DXMATRIX	Matrix;
    HRESULT     hr;
	float 		fAspect = 1.33f;
	float		fFOV = pi/4.0f;
    

    D3DXMatrixPerspectiveFovLH(&m_ProjMatrix, fFOV, fAspect, 1.0f, 1000.0f);
    if (!SetTransform(D3DTS_PROJECTION,&m_ProjMatrix))
        return false;

    D3DXMatrixIdentity(&Matrix);
    if (!SetTransform(D3DTS_WORLD, &Matrix))
        return false;

	// Setup the view matrices
    //D3DXMatrixLookAtLH(&(m_ViewMatrix[m_nViewMatrix++]), &m_vLoc, &m_vAt, &m_vUp);
    
	// Setup the view matrices
	D3DXVECTOR3 v;
    D3DXMatrixLookAtLH(&(m_ViewMatrix[m_nViewMatrix++]), &m_vLoc, &m_vAt, &m_vUp);
/*    v = D3DXVECTOR3(0.0f, 3.0f, -1.0f);
    D3DXMatrixLookAtLH(&(m_ViewMatrix[m_nViewMatrix++]), &v, &m_vAt, &m_vUp);
    v = D3DXVECTOR3(0.0f, -3.0f, -1.0f);
	D3DXMatrixLookAtLH(&(m_ViewMatrix[m_nViewMatrix++]), &v, &m_vAt, &m_vUp);
*/
	if (!SetTransform(D3DTS_VIEW, &(m_ViewMatrix[0])))
		return false;

    // Set the texture transform
    D3DXMatrixRotationZ(&(m_TexTrans[0]), 0);
    D3DXMatrixRotationZ(&(m_TexTrans[1]), pi);
	D3DXMatrixRotationZ(&(m_TexTrans[2]), 0);

    return true;
}

// ----------------------------------------------------------------------------

bool CNPSurf::LoadGeometry()
{
    LPD3DXMESH pMesh=NULL;
	CTestShapes Shape;
	HOTEST Test;

	for(int g=0; g<2; g++)
    for(int j=0; j<g_nNormal; j++)
    for(int k=0; k<g_nPosition; k++)
	for(int i=0; i<g_nNPTess; i++)
	{
		Test.HOBasis = (D3DBASISTYPE)g_NormalDegree[j];
		Test.HODegree = g_PositionDegree[k];
		Test.nTessIndex = i;
		Test.bSpecular = g > 0 ? true : false;
        Test.DrawMode = ONEPATCH;
		Test.PatchType = CSPHERE;
		Test.fRSTess = 1.f;
		Test.nShader = 1;				// <-- was 0, but won't work for us, since it will always skip if m_bVS is false (which it is for D3DX Tesselation)

		for(int nSeg=3; nSeg<=7; nSeg+=2)
		{
			Test.nWidth = nSeg;
			Test.nHeight = nSeg;
			Test.PatchType = FAN;
			Shape.NewShape(CS_MESH, Test.nWidth-1, Test.nHeight-1);
			Shape.Scale(2.7f,2,2);
			TweakGeometry(&Test, Shape.m_pVertices);

            Test.dwTexture = TEX_NONE;
			m_pSurfs[m_nSurfCount] = CreateHOSurf(&Shape, &Test, D3DFVF_VERTEX);
			if(m_pSurfs[m_nSurfCount])
				m_nSurfCount++;

            Test.dwTexture = TEX_COORD0;
            m_pSurfs[m_nSurfCount] = CreateHOSurf(&Shape, &Test, D3DFVF_VERTEX);
			if(m_pSurfs[m_nSurfCount])
				m_nSurfCount++;
		}
		
		Test.nWidth = 2;
		Test.nHeight = 2;
        Test.dwTexture = TEX_NONE;
		Shape.NewShape(CS_SPHERE, Test.nWidth, Test.nHeight);
		Shape.Scale(2,2,2);
			
		m_pSurfs[m_nSurfCount] = CreateHOSurf(&Shape, &Test, D3DFVF_VERTEX);
		if(m_pSurfs[m_nSurfCount])
			m_nSurfCount++;

        Test.dwTexture = TEX_COORD0;
		m_pSurfs[m_nSurfCount] = CreateHOSurf(&Shape, &Test, D3DFVF_VERTEX);
		if(m_pSurfs[m_nSurfCount])
			m_nSurfCount++;
		
	}

	return true;
}

// ----------------------------------------------------------------------------

void CNPSurf::GetLogTags(TCHAR *pcsName, TCHAR *pcsLine)
{
   	_stprintf(pcsName, _T("%s Pos, %s Normal, %.2f tess"),
				g_sDegree[(int)m_pSurfs[m_nCurrentSurf]->pTest->HODegree], 
				g_sDegree[(int)m_pSurfs[m_nCurrentSurf]->pTest->HOBasis],
		        g_NPTess[m_pSurfs[m_nCurrentSurf]->pTest->nTessIndex]);
	_stprintf(pcsLine, _T("Specular %s, Texture %s"),
			  m_pSurfs[m_nCurrentSurf]->pTest->bSpecular ? _T("On") : _T("Off"), 
			  m_pSurfs[m_nCurrentSurf]->pTest->dwTexture == TEX_NONE ? _T("Off") : (m_pSurfs[m_nCurrentSurf]->pTest->dwTexture == TEX_COORD0 ? _T("On(1 texcoord)") : _T("On(2 texcoords)")));
}

// ----------------------------------------------------------------------------

bool CNPSurf::SetupVertexShader()
{
	HRESULT hr;
	UINT uMaxStreams = m_pSrcDevice->GetCaps()->dwMaxStreams;

	for(int i=0; i<uMaxStreams; i++)
	{
		if(m_pD3D->GetVersion() < 9.f)
		{
            		
			DWORD decl0_8[] =
			{
				D3DVSD_STREAM(i),
				D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT3),
				D3DVSD_REG( D3DVSDE_NORMAL,  D3DVSDT_FLOAT3),
				D3DVSD_REG( D3DVSDE_TEXCOORD0,  D3DVSDT_FLOAT2),
				D3DVSD_END()
			};
		
			if(!CreateVertexShader(decl0_8, NULL, &(m_pShaders8[m_nShaders]), 0))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexShader() failed.  hr=%s\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}

		}
		else
		{
			VERTEXELEMENT decl1[] = 
			{
				{(BYTE)i, 0, DECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{(BYTE)i, 12, DECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL, 0},
				{(BYTE)i, 24, DECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
				DECL_END()
			};
			
			VERTEXELEMENT decl1_out[] = 
			{
				{(BYTE)i, 0, DECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
				{(BYTE)i, 12, DECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL, 0},
				{(BYTE)i, 24, DECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
				DECL_END()
			};
			
			if(!CreateVertexDeclaration(decl1, &(m_pDecls[i])))
			{
				WriteToLog(_T("CNPSurf::SetupVertexShader -- CreateVertexDeclaration() failed\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}
	
			if(!CreateVertexDeclaration(decl1_out, &(m_pDecls_Out[i])))
			{
				WriteToLog(_T("CNPSurf::SetupVertexShader -- CreateVertexDeclaration() failed\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}
	
			if(!CreateVertexShader(NULL, &(m_pShaders[i])))
			{
				WriteToLog(_T("CNPSurf::SetupVertexShader -- CreateVertexShader() failed\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}
		}

		m_nShaders++;
	}

	return true;
}

// ----------------------------------------------------------------------------

bool CNPSurf::PreExecuteTest(UINT uTestNumber)
{
	if(m_bD3DXTessellation)
	{
		if(m_pSurfs[m_nCurrentSurf]->pTest->HODegree == D3DDEGREE_LINEAR)		// we can't do linear, so skip.
		{
	        SkipTestRange(uTestNumber, uTestNumber+m_nViewMatrix-1);
			return false;
		}
	}

	return true;		// continue with test
}

// ----------------------------------------------------------------------------

bool CNPSurf::DrawSurface(HOSURFACE *pSurf)
{
	bool bRes = true;
	CnVertexBuffer *pVB = NULL;
	HRESULT hr;

	DWORD dwDeviceFlags = CD3D_ALL;

	// used for D3DX Tesselation:
	ID3DXMesh* pMeshIn = 0;
	ID3DXMesh* pMeshOut = 0;
	DWORD* pAdjIn = 0;
	ID3DXBuffer* pAdjOut = 0;

	// Create temp buffers
	WORD *pIndices = new WORD[pSurf->uIndices], *pTmpInd;
	BYTE *pVertices = new BYTE[GetVertexSize(pSurf->dwFVF) * pSurf->uVertices], *pTmpVrt;
	BYTE *pNonIndexedVertices = new BYTE[GetVertexSize(pSurf->dwFVF) * pSurf->uIndices];

	if(!pIndices || !pVertices || !pNonIndexedVertices)
	{
		WriteToLog(_T("CNPSurf::DrawSurface() - Out of memory.\n"));
		bRes = false;
		goto Exit;
	}

	if(!CreateVertexBuffer(GetVertexSize(pSurf->dwFVF) * pSurf->uIndices, m_dwUsage, pSurf->dwFVF, POOL_SYSTEMMEM, &pVB))
	{
		WriteToLog(_T("CNPSurf::DrawSurface() - CreateVertexBuffer failed. hr=%s.\n"), 
				   m_pD3D->HResultToString(GetLastError()));
		bRes = false;
		goto Exit;
	}

	// Copy the VB/IB into the temp buffers
	pSurf->pVB->Lock(0,0, &pTmpVrt, LOCK_READONLY);
	memcpy(pVertices, pTmpVrt, GetVertexSize(pSurf->dwFVF) * pSurf->uVertices);
	pSurf->pIB->Lock(0,0, (BYTE**)&pTmpInd, LOCK_READONLY);
	memcpy(pIndices, pTmpInd, sizeof(WORD) * pSurf->uIndices);

	// Unlock the buffers
	pSurf->pVB->Unlock();
	pSurf->pIB->Unlock();

	// Create the non-indexed VB
	if(!pVB->Lock(0,0,&pTmpVrt, 0))
	{
		WriteToLog(_T("CNPSurf::DrawSurface() - Lock() failed. hr=%s.\n"), 
				   m_pD3D->HResultToString(GetLastError()));
		bRes = false;
		goto Exit;		
	}

	for(int i=0; i<pSurf->uIndices; i++)
	{
		memcpy(&(((D3DVERTEX*)pNonIndexedVertices)[i]), &(((D3DVERTEX*)pVertices)[pIndices[i]]), GetVertexSize(pSurf->dwFVF));
		memcpy(&(((D3DVERTEX*)pTmpVrt)[i]), &(((D3DVERTEX*)pVertices)[pIndices[i]]), GetVertexSize(pSurf->dwFVF));
	}

	pVB->Unlock();

	if(m_bD3DXTessellation)
	{
		dwDeviceFlags = CD3D_REF;		// only do device render on ref if we are testing D3DX Tessellation

		UINT uFaces = pSurf->uIndices / 3;
		UINT uEdges = pSurf->uIndices;		// faces * 3

		BYTE* pBuffer = 0;

		if(FAILED(hr = m_pSrcDevice->D3DXCreateMeshFVF(uFaces, pSurf->uVertices, D3DXMESH_SYSTEMMEM | D3DXMESH_DYNAMIC, pSurf->dwFVF, &pMeshIn)))
		{
			WriteToLog(_T("CNPSurf::DrawSurface() - CDevice::D3DXCreateMeshFVF() failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
			bRes = false;
			goto Exit;
		}

		
		// Copy vb/ib:
		if(FAILED(hr = pMeshIn->LockVertexBuffer(D3DLOCK_NOSYSLOCK, (void**)&pBuffer)))
		{
			WriteToLog(_T("CNPSurf::DrawSurface() - LockVertexBuffer failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
			bRes = false;
			goto Exit;		
		}
		memcpy(pBuffer, pVertices, GetVertexSize(pSurf->dwFVF) * pSurf->uVertices);
		pMeshIn->UnlockVertexBuffer();
		pBuffer = 0;

		if(FAILED(hr = pMeshIn->LockIndexBuffer(D3DLOCK_NOSYSLOCK, (void**)&pBuffer)))
		{
			WriteToLog(_T("CNPSurf::DrawSurface() - LockIndexBuffer failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
			bRes = false;
			pMeshIn->UnlockVertexBuffer();	// unlock locked one
			goto Exit;		
		}
		memcpy(pBuffer, pIndices, sizeof(WORD) * pSurf->uIndices);
		pMeshIn->UnlockIndexBuffer();
		pBuffer = 0;


		// Generate adjacency:
		pAdjIn = new DWORD[uEdges];
		if(NULL == pAdjIn)
		{
			WriteToLog(_T("CNPSurf::DrawSurface() - Memory allocation failed. Tried to allocate %d bytes.\n"), uEdges * sizeof(DWORD));
			bRes = false;
			goto Exit;
		}

		if(FAILED(hr = pMeshIn->ConvertPointRepsToAdjacency(NULL, pAdjIn)))
		{
			WriteToLog(_T("CNPSurf::DrawSurface() - ID3DXMesh::ConvertPointRepsToAdjacency() failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
			bRes = false;
			goto Exit;		
		}

		BOOL bQuadInterpNormals = ((D3DDEGREETYPE)(pSurf->pTest->HOBasis) == D3DDEGREE_QUADRATIC) ? true : false;
		if(FAILED(hr = D3DXTessellateNPatches(pMeshIn, pAdjIn, g_NPTess[pSurf->pTest->nTessIndex], bQuadInterpNormals, &pMeshOut, &pAdjOut)))
		{
			WriteToLog(_T("CNPSurf::DrawSurface() - D3DXTessellateNPatches() failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
			bRes = false;
			goto Exit;
		}

		if(NULL == pMeshOut)
		{
			WriteToLog(_T("CNPSurf::DrawSurface() - D3DXTessellateNPatches successful, but pMesh is NULL.\n"));
			bRes = false;
			goto Exit;
		}

		if(FAILED(hr = pMeshOut->DrawSubset(0)))
		{
			WriteToLog(_T("CNPSurf::DrawSurface() - ID3DXMesh::DrawSubset() failed. hr=%s.\n"), m_pD3D->HResultToString(hr));
			bRes = false;
			goto Exit;
		}
	}

   	SetNPatchMode(g_NPTess[pSurf->pTest->nTessIndex], dwDeviceFlags);
	SetRenderState(D3DRS_POSITIONDEGREE, (DWORD)pSurf->pTest->HODegree, dwDeviceFlags);
	SetRenderState(D3DRS_NORMALDEGREE, (DWORD)pSurf->pTest->HOBasis, dwDeviceFlags);

	UINT uTotalPrims= pSurf->uIndices / 3;
	UINT uMod = uTotalPrims % 4;
	UINT uPrims = (uTotalPrims - uMod) / 4;

	if(m_dwVersion >= 0x900)
		SetVertexShader((CnVertexShader*)NULL, dwDeviceFlags);
	SetFVF(pSurf->dwFVF, dwDeviceFlags);
	DrawIndexedPrimitiveUP(D3DPT_TRIANGLELIST, 0, (DWORD)pSurf->uVertices, (DWORD)uPrims, 
						   pIndices, FMT_INDEX16, pVertices, GetVertexSize(pSurf->dwFVF), dwDeviceFlags);
    if(m_dwVersion < 0x900)
		SetVertexShader(m_pShaders8[0]);
	else
	{
		SetVertexDeclaration(m_pDecls[0], dwDeviceFlags);
		SetVertexShader(m_pShaders[0], dwDeviceFlags);
	}
	DrawPrimitiveUP(D3DPT_TRIANGLELIST, (DWORD)uPrims, 
						   ((D3DVERTEX*)pNonIndexedVertices)+(uPrims*3), GetVertexSize(pSurf->dwFVF), dwDeviceFlags);

    if(m_dwVersion < 0x900)
		SetVertexShader(m_pShaders8[m_uTestNumber % m_nShaders]);
	else
	{
		SetVertexDeclaration(m_pDecls[m_uTestNumber % m_nShaders], dwDeviceFlags);
		SetVertexShader(m_pShaders[m_uTestNumber % m_nShaders], dwDeviceFlags);
	}
	SetStreamSource((m_uTestNumber % m_nShaders), pSurf->pVB, GetVertexSize(pSurf->dwFVF), 0, dwDeviceFlags);
	SetIndices(pSurf->pIB, 0, dwDeviceFlags);
	DrawIndexedPrimitive(D3DPT_TRIANGLELIST, 0, (DWORD)pSurf->uVertices, (uPrims*2)*3, (DWORD)uPrims, 0, dwDeviceFlags);

    if(m_dwVersion < 0x900)
		SetVertexShader(m_pShaders8[(m_uTestNumber+1) % m_nShaders]);
	else
	{
		SetVertexDeclaration(m_pDecls[(m_uTestNumber+1) % m_nShaders], dwDeviceFlags);
		SetVertexShader(m_pShaders[(m_uTestNumber+1) % m_nShaders], dwDeviceFlags);
	}
	SetStreamSource((m_uTestNumber+1) % m_nShaders, pVB, GetVertexSize(pSurf->dwFVF), 0, dwDeviceFlags);
	DrawPrimitive(D3DPT_TRIANGLELIST, (uPrims*9), uPrims, dwDeviceFlags);



Exit:
	// D3DX stuff:
	SAFEDELETEA(pAdjIn);
	RELEASE(pAdjOut);
	RELEASE(pMeshIn);
	RELEASE(pMeshOut);

	SAFEDELETEA(pIndices);
	SAFEDELETEA(pVertices);
	SAFEDELETEA(pNonIndexedVertices);
	RELEASE(pVB);

	return bRes;
}

// ----------------------------------------------------------------------------

void CNPSurf::UpdateStatus()
{
	TCHAR tcsBuffer[256];

	CD3DTest::UpdateStatus();

    WriteStatus(_T("$wNormal Degree"), _T("$a%s"), g_sDegree[(int)m_pSurfs[m_nCurrentSurf]->pTest->HOBasis]);
    WriteStatus(_T("$wPosition Degree"), _T("$a%s"), g_sDegree[(int)m_pSurfs[m_nCurrentSurf]->pTest->HODegree]);
	WriteStatus(_T("$wTesselation"), _T("$a%.2f"), g_NPTess[m_pSurfs[m_nCurrentSurf]->pTest->nTessIndex]);
	WriteStatus(_T("$wSpecular"), _T("$a%s"), m_pSurfs[m_nCurrentSurf]->pTest->bSpecular ? _T("On") : _T("Off"));
    WriteStatus(_T("$wTexture\t"), _T("$a%s"), m_pSurfs[m_nCurrentSurf]->pTest->dwTexture == TEX_NONE ? _T("Off") : _T("On"));

	// Set this as the last surface
    m_nLastSurf = m_nCurrentSurf;
}
