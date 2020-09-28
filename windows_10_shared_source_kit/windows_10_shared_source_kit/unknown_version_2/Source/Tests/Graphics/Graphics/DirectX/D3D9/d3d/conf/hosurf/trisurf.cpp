// ============================================================================
//
//  TriSurf.cpp
//  Written by robald.
//
// ============================================================================

#include "HOSurf.h"

#include "cd3dx.h"

float g_TriTess[][4] = {
	{ 1.f, 1.f, 1.f, 1.f },
    { 5.f, 5.f, 5.f, 5.f },
    { 2.f, 4.f, 6.f, 8.f },
    { 9.1f, 2.5f, 7.75f, 5.3f },
	{ 15.f, 15.f, 15.f, 15.f}
};
int g_nTriTess = sizeof(g_TriTess) / (sizeof(float) * 4);

//bug 84803: DrawTriPatch with NULL as pNumSegs parameter causes access violation in DX8
//removed all cases with g_TriTests[i].nTessIndex == 5 (they are the ones where pNumSegs == NULL)
HOTEST g_TriTests[] = {
	  { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   2,  2,  0, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   2,  2,  0, false, TEX_NONE, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 0 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   2,  2,  1, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   2,  2,  2, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   2,  2,  3, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
	  //{ D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   2,  2,  5, true,  TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   2,  2,  5, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   2,  2,  0, true,  TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   3,  3,  1, false, TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   3,  3,  1, false, TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE, 5.f, 0 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   3,  3,  2, true,  TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   3,  3,  3, true,  TEX_COORD0, ONEPATCH, WART  , D3DPATCHEDGE_CONTINUOUS, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   3,  3,  3, true,  TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_CONTINUOUS, 5.f, 2 },
	  //{ D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   7,  7,  5, false, TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_DISCRETE, 5.9f, 0 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   7,  7,  0, true,  TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   7,  7,  1, true,  TEX_COORD0, ONEPATCH, DIMPLE, D3DPATCHEDGE_CONTINUOUS, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   7,  7,  1, true,  TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_CONTINUOUS, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   7,  7,  2, false, TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_DISCRETE, 5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   7,  7,  3, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   7,  7,  5, true,  TEX_COORD0, ONEPATCH, WAVE  , D3DPATCHEDGE_CONTINUOUS, 4.1f, 1 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,   7,  7,  5, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_CONTINUOUS, 7.3f, 0 },

	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  0, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  0, false, TEX_NONE, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 0 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  1, true,  TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  1, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  2, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  3, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
	  //{ D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  5, true,  TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 9.49f, 2 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  5, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 9.49f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  0, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  1, false, TEX_COORD0, ONEPATCH, WART  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  2, true,  TEX_COORD0, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  2, true,  TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  3, true,  TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  5, false, TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_DISCRETE, 5.1f, 0 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  0, true,  TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  1, true,  TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  2, false, TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_DISCRETE, 5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  3, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  5, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 0 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  0, true,  TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  0, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 0 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  1, false, TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  1, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  2, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  3, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 0 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  5, false, TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  5, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  0, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  1, false, TEX_COORD0, ONEPATCH, WART  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  2, true,  TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  3, true,  TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_CONTINUOUS, 5.f, 0 },
	  //{ D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  5, false, TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  0, true,  TEX_COORD0, ONEPATCH, DIMPLE, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  0, true,  TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  1, true,  TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  2, false, TEX_COORD0, ONEPATCH, WAVE  , D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  2, false, TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_DISCRETE, 5.f, 0 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  3, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  5, true,  TEX_COORD0, ONEPATCH, WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 2 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    7,  7,  5, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 2 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  0, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  1, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },	  
      { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,    4,  4,  2, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },	  

	  { D3DBASIS_BEZIER,      D3DDEGREE_LINEAR,  2,  2,  1, true, TEX_COORD1,  ONEPATCH, DEGEN0, D3DPATCHEDGE_DISCRETE,    5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_CUBIC,   4,  4,  1, true, TEX_COORD1,  ONEPATCH, DEGEN0, D3DPATCHEDGE_CONTINUOUS,  5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 6,  6,  1, true, TEX_COORD1,  ONEPATCH, DEGEN0, D3DPATCHEDGE_CONTINUOUS,  5.f, 1 },

	  { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  0, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  0, false, TEX_NONE, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 0 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  1, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  2, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  3, false, TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
	  //{ D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  5, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  0, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  1, false, TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  1, false, TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE, 5.f, 0 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  2, true,  TEX_COORD0, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  3, true,  TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  5, false, TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  0, true,  TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  1, true,  TEX_COORD0, ONEPATCH, DIMPLE, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  2, false, TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_DISCRETE, 5.f, 0 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  3, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC,  6,  6,  5, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  0, false, TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  1, true,  TEX_COORD0, ONEPATCH, PLANAR, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  2, true,  TEX_COORD1, ONEPATCH, PLANAR, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  3, false, TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE, 5.f, 0 },
	  //{ D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  5, true,  TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  0, true,  TEX_COORD1, ONEPATCH, WART  , D3DPATCHEDGE_DISCRETE, 5.f, 1 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  1, false, TEX_COORD0, ONEPATCH, DIMPLE, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  2, true,  TEX_COORD0, ONEPATCH, DIMPLE, D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  3, true,  TEX_COORD1, ONEPATCH, DIMPLE, D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
      //{ D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  5, false, TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_DISCRETE, 5.f, 0 },
	  { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  0, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_DISCRETE, 5.f, 2 },
      { D3DBASIS_BEZIER,      D3DDEGREE_QUINTIC, 11, 11,  1, true,  TEX_COORD1, ONEPATCH, WAVE  , D3DPATCHEDGE_CONTINUOUS, 5.f, 1 },
};                                                                                       
int g_nTriTests = sizeof(g_TriTests) / sizeof(HOTEST);                                   
																						 
// ----------------------------------------------------------------------------          
																						 
CTriSurf::CTriSurf()                                                                     
{                                                                                        
    m_szTestName= _T("HOSurf - TriSurface tests");                                       
	m_szCommandKey= _T("TRI");                                                           
    m_dwUsage = D3DUSAGE_RTPATCHES;                                                      
}                                                                                        

// ----------------------------------------------------------------------------

bool CTriSurf::SetupVertexShader()
{
	HRESULT hr;

	DWORD dwDeviceFlag = m_bD3DXTessellation ? CD3D_REF : CD3D_ALL;

	if(m_bVS)
	{
		LPD3DXBUFFER pShader = NULL;
		if(m_dwVersion < 0x900)
		{
			DWORD decl0_8[] =
			{
				D3DVSD_STREAM(0),
				D3DVSD_REG( 8, D3DVSDT_FLOAT3),
				D3DVSD_REG( 6, D3DVSDT_FLOAT3),
				D3DVSD_REG( 11, D3DVSDT_FLOAT2),
				D3DVSD_STREAM_TESS(),
				D3DVSD_TESSNORMAL( 8, 9 ),
				D3DVSD_TESSUV( 10 ),
				D3DVSD_STREAM(1),
				D3DVSD_REG(12, D3DVSDT_D3DCOLOR),
				D3DVSD_END()
			};

			TCHAR *szShader = _T("vs.1.0\n"
								"dp4 oPos.x, v8, c0\n"
								"dp4 oPos.y, v8, c1\n"
								"dp4 oPos.z, v8, c2\n"
								"dp4 oPos.w, v8, c3\n"
								"mul oT1.xy, c4.xy, v10.xy\n"
								"mov oT0, v11\n"
								"mov oD0, v12\n");

			hr = D3DXAssembleShader8(szShader, _tcslen(szShader), 0, NULL, &pShader, NULL);
			if (FAILED(hr))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- D3DXAssembleShader() failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
				return false;
			}

			if(!CreateVertexShader(decl0_8, (DWORD*)pShader->GetBufferPointer(), &(m_pShaders8[m_nShaders]), 0))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexShader() failed.  hr=%s\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}
		}
		else
		{
			static VERTEXELEMENT decl0[] = 
			{
				{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
				{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL, 0},
				//{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_UV, DECLUSAGE_TEXCOORD, 3},
				{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 2},
				{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 4},
				{1, 0, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 5},
				DECL_END()
			};
			m_pRawDecls[m_nShaders] = decl0;

			// NOTE: I don't believe the following is used anywhere:
			static VERTEXELEMENT decl0_out[] = 
			{
				{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 1},
				{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL, 0},
				//{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 3},
				{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 2},
				{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 4},
				{1, 0, (DECLTYPE)D3DDECLTYPE_D3DCOLOR, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 5},
				DECL_END()
			};
			m_pRawDecls_Out[m_nShaders] = decl0_out;

			if(!CreateVertexDeclaration(decl0, &(m_pDecls[m_nShaders]), dwDeviceFlag))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}

			if(!CreateVertexDeclaration(decl0_out, &(m_pDecls_Out[m_nShaders])))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}


			TCHAR *szShader = _T("vs_1_1\n"

								"dcl_texcoord1 v8\n"
							//	"dcl_texcoord3 v10\n"
								"dcl_texcoord4 v11\n"
								"dcl_texcoord5 v12\n"

								"dp4 oPos.x, v8, c0\n"
								"dp4 oPos.y, v8, c1\n"
								"dp4 oPos.z, v8, c2\n"
								"dp4 oPos.w, v8, c3\n"
							//	"mul oT1.xy, c4.xy, v10.xy\n"
								"mov oT0, v11\n"
								"mov oD0, v12\n");

		
			hr = D3DXAssembleShader9(szShader, _tcslen(szShader), NULL, NULL, 0, &pShader, NULL);
			if (FAILED(hr))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- D3DXAssembleShader(vertex) failed, hr=%s\n"),
							m_pD3D->HResultToString(hr));
				return false;
			}

			if(!CreateVertexShader((DWORD*)pShader->GetBufferPointer(), &(m_pShaders[m_nShaders])))
			{
				WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexShader() failed.  hr=%s\n"),
							m_pD3D->HResultToString(GetLastError()));
				return false;
			}
		}		// end if version 9 or greater

		if(pShader)
			pShader->Release();
	}

	// Even if the above shader isn't defined, we must keep the numbering consistent, then fill in the missing shader (by copying another) later:
	m_nShaders++;
	
	if(m_dwVersion < 0x900)
	{
		DWORD decl1_8[] =
		{
			D3DVSD_STREAM(0),
			D3DVSD_REG( D3DVSDE_POSITION,  D3DVSDT_FLOAT3),
			D3DVSD_REG( D3DVSDE_BLENDWEIGHT,   D3DVSDT_FLOAT3),
			D3DVSD_REG( D3DVSDE_TEXCOORD2, D3DVSDT_FLOAT2),
			D3DVSD_STREAM_TESS(),
			D3DVSD_TESSNORMAL( D3DVSDE_POSITION, D3DVSDE_NORMAL ),
			D3DVSD_TESSUV( D3DVSDE_TEXCOORD0 ),
			D3DVSD_TESSUV( D3DVSDE_TEXCOORD1 ),
			D3DVSD_END()
		};

		if(!CreateVertexShader(decl1_8, NULL, &(m_pShaders8[m_nShaders]), 0))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexShader() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	else
	{
		static VERTEXELEMENT decl1[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_BLENDWEIGHT, 0},
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL, 0},
//			{0, 0, D3DDECLTYPE_UNUSED, DECLMETHOD_UV, DECLUSAGE_TEXCOORD, 0},
//			{0, 0, D3DDECLTYPE_UNUSED, DECLMETHOD_UV, DECLUSAGE_TEXCOORD, 1},
			{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			DECL_END()
		};
		m_pRawDecls[m_nShaders] = decl1;

		static VERTEXELEMENT decl1_out[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_BLENDWEIGHT, 0},
			{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL, 0},
//			{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
//			{0, 44, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
			{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			DECL_END()
		};
		m_pRawDecls_Out[m_nShaders] = decl1_out;

		if(!CreateVertexDeclaration(decl1, &(m_pDecls[m_nShaders]), dwDeviceFlag))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}

		if(!CreateVertexDeclaration(decl1_out, &(m_pDecls_Out[m_nShaders])))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}

		if(!CreateVertexShader(NULL, &(m_pShaders[m_nShaders])))
		{
			WriteToLog(_T("CStateBeast::Init8 -- CreateVertexShader() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}

	m_nShaders++;

	if(m_dwVersion < 0x900)
	{
		DWORD decl2_8[] =
		{
			D3DVSD_STREAM(0),
			D3DVSD_REG( D3DVSDE_POSITION,    D3DVSDT_FLOAT3),
			D3DVSD_REG( D3DVSDE_BLENDWEIGHT, D3DVSDT_FLOAT3),
			D3DVSD_REG( D3DVSDE_TEXCOORD0,   D3DVSDT_FLOAT2),
			D3DVSD_STREAM_TESS(),
			D3DVSD_TESSNORMAL( D3DVSDE_POSITION, D3DVSDE_NORMAL ),
			D3DVSD_TESSUV(D3DVSDE_TEXCOORD1),
			D3DVSD_END()
		};

		if(!CreateVertexShader(decl2_8, NULL, &(m_pShaders8[m_nShaders]), 0))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexShader() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}
	else
	{
		static VERTEXELEMENT decl2[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_BLENDWEIGHT, 0},
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL, 0},
			{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
//			{0, 0, D3DDECLTYPE_UNUSED, DECLMETHOD_UV, DECLUSAGE_TEXCOORD, 1},
			DECL_END()
		};
		static VERTEXELEMENT decl2_out[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_BLENDWEIGHT, 0},
			{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL, 0},
			{0, 36, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
//			{0, 44, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 1},
			DECL_END()
		};

		/*
		static VERTEXELEMENT decl2[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_CROSSUV, DECLUSAGE_NORMAL, 0},
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_UV, DECLUSAGE_TEXCOORD, 0},
			DECL_END()
		};
		static VERTEXELEMENT decl2_out[] = 
		{
			{0, 0, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_POSITION, 0},
			{0, 12, (DECLTYPE)D3DDECLTYPE_FLOAT3, DECLMETHOD_DEFAULT, DECLUSAGE_NORMAL, 0},
			//{0, 24, (DECLTYPE)D3DDECLTYPE_FLOAT2, DECLMETHOD_DEFAULT, DECLUSAGE_TEXCOORD, 0},
			DECL_END()
		};
		*/
		m_pRawDecls[m_nShaders] = decl2;
		m_pRawDecls_Out[m_nShaders] = decl2_out;

		if(!CreateVertexDeclaration(decl2, &(m_pDecls[m_nShaders]), dwDeviceFlag))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}

		if(!CreateVertexDeclaration(decl2_out, &(m_pDecls_Out[m_nShaders])))
		{
			WriteToLog(_T("CHOSurf::SetupVertexShader -- CreateVertexDeclaration() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}

		if(!CreateVertexShader(NULL, &(m_pShaders[m_nShaders])))
		{
			WriteToLog(_T("CStateBeast::Init8 -- CreateVertexShader() failed.  hr=%s\n"),
						m_pD3D->HResultToString(GetLastError()));
			return false;
		}
	}

	m_nShaders++;

/*	Should be no reason to do this, as this case SHOULD be skipped:
	if(!m_bVS)		// Then first shader was skipped, fill it in by copying one of the other shaders (how about the second one)
	{
		m_pDecls[0] = m_pDecls[1];
		m_pDecls_Out[0] = m_pDecls_Out[1];
		m_pShaders[0] = m_pShaders[1];

		// Add to their reference count, so they don't get released too many times:
		m_pDecls[0]->AddRef();
		m_pDecls_Out[0]->AddRef();
		m_pShaders[0]->AddRef();
	}
*/
	return true;
}

// ----------------------------------------------------------------------------

void CTriSurf::QuadToTri(HOTEST *pTest, CTestShapes *pShape, TRIPATCH *pOut)
{
	int nSpan = (int)pTest->HODegree + 1;
	int nVertices = ((nSpan * nSpan) - nSpan) / 2 + nSpan;
	int nIndices = nVertices;
    int nMove = pTest->HOBasis == D3DBASIS_BEZIER ? ((int)pTest->HODegree) : 1;
	int nXPatch = pTest->HOBasis == D3DBASIS_BEZIER ? (pTest->nWidth-1)  / nMove : 
				     							 (pTest->nWidth- (int)pTest->HODegree);
	int nYPatch = pTest->HOBasis == D3DBASIS_BEZIER ? (pTest->nHeight-1) / nMove : 
														 (pTest->nHeight- (int)pTest->HODegree);
	int nPatches = nXPatch * nYPatch;
	int nOffset = pTest->nWidth;
	

	memset(pOut, 0, sizeof(TRIPATCH));
	pOut->nVertices = nVertices * 2 * nPatches;
	pOut->nIndices =  nIndices * 2 * nPatches;
	int nTri1Off = 0;
	int nTri2Off = nVertices;
	pOut->pVertices = (D3DVERTEX *)malloc(sizeof(D3DVERTEX) * pOut->nVertices);
	pOut->pIndices = (WORD *)malloc(sizeof(WORD) * pOut->nIndices);

	if(NULL == pOut->pVertices || NULL == pOut->pIndices)
	{
		if(NULL != pOut->pVertices) free(pOut->pVertices);
		if(NULL != pOut->pIndices) free(pOut->pIndices);
		return;
	}

	for(int nY=0; nY<nYPatch; nY++)
	for(int nX=0; nX<nXPatch; nX++)
	{
		int nPivot = 0;
		int nOut1 = nTri1Off;
		int nOut2 = nTri2Off;
		int nPatchOff = (nY * nMove * nOffset) + (nX * (nSpan-1));
		
		// Generate the vertices
		for(int i=0; i<nSpan; i++)
		{
			for(int j=0; j<nSpan; j++)
			{
				int nIn = nPatchOff +  (i * nOffset + j);

				if(j <= nPivot)
				{
					memcpy(&(pOut->pVertices[nOut1++]), &(pShape->m_pVertices[nIn]), sizeof(D3DVERTEX));
				}
				if(j >= nPivot)
				{
					memcpy(&(pOut->pVertices[nOut2++]), &(pShape->m_pVertices[nIn]), sizeof(D3DVERTEX));
				}
			}
			nPivot++;
		}

		// Reverse the winding order of the second triangle
		int nMid = nVertices / 2;
		for(int i=0; i<nMid; i++)
		{
			DWORD dwSize = sizeof(D3DVERTEX);
			D3DVERTEX v;
			int n1 = nTri2Off + i;
			int n2 = nTri2Off + nVertices-i-1;
			memcpy(&v, &(pOut->pVertices[n1]), dwSize);
			memcpy(&(pOut->pVertices[n1]), &(pOut->pVertices[n2]), dwSize);
			memcpy(&(pOut->pVertices[n2]), &v, dwSize);
		}
		
		nTri1Off += nVertices * 2;
		nTri2Off += nVertices * 2;
	}

	// Set the indices
	for(int i=0; i<pOut->nIndices; i++)
		pOut->pIndices[i] = (WORD)i;
}

// ----------------------------------------------------------------------------

bool CTriSurf::CheckCaps()
{
    if(!(m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_RTPATCHES))
	{
		WriteToLog(_T("CTriSurf::CheckCaps() - D3DDEVCAPS_RTPATCHES not set.\n"));
		return false;
	}
	
	m_bQuintic = m_pSrcDevice->GetCaps()->dwDevCaps & D3DDEVCAPS_QUINTICRTPATCHES ? true : false;

	return true;
}

// ----------------------------------------------------------------------------

bool CTriSurf::LoadGeometry()
{
	CTestShapes Shape;
	TRIPATCH Tri1;
	
    // Generate the geometry
   	for(int nTest=0; nTest<g_nTriTests; nTest++)
	{
		Shape.NewShape(CS_MESH, g_TriTests[nTest].nWidth-1, g_TriTests[nTest].nHeight-1);
		Shape.Scale(2,2,2);
        TweakGeometry(&g_TriTests[nTest], Shape.m_pVertices);
		QuadToTri(&g_TriTests[nTest], &Shape, &Tri1);
			
		// Create the surface and buffers
		m_pSurfs[m_nSurfCount] = CreateHOSurf(&Tri1, &(g_TriTests[nTest]), D3DFVF_VERTEX);
		if(m_pSurfs[m_nSurfCount])
		{
			_stprintf(m_pSurfs[m_nSurfCount]->tcsSurf, _T("%sTri patch(s)"), 
					  m_pSurfs[m_nSurfCount]->pTest->DrawMode == ONEPATCH || m_pSurfs[m_nSurfCount]->pTest->DrawMode == SEQUENTIAL ? _T("Individual ") : _T("Mosaic "));
            
			if(m_pSurfs[m_nSurfCount]->pTest->nShader == 0)
				_tcscpy(m_pSurfs[m_nSurfCount]->tcsShader, _T("Custom"));
			else if(m_pSurfs[m_nSurfCount]->pTest->nShader == 1)
				_tcscpy(m_pSurfs[m_nSurfCount]->tcsShader, _T("Fixed(generated TexCoords)"));
            else if(m_pSurfs[m_nSurfCount]->pTest->nShader == 2)
				_tcscpy(m_pSurfs[m_nSurfCount]->tcsShader, _T("Fixed(user-defined TexCoords)"));
			else
                _tcscpy(m_pSurfs[m_nSurfCount]->tcsShader, _T("Unknown"));

			// TODO: For D3D9 there is not D3DRS_PATCHSEGMENTS, so eliminate this branch
			if(m_pSurfs[m_nSurfCount]->pTest->nTessIndex >= g_nTriTess)
			{
				_stprintf(m_pSurfs[m_nSurfCount]->tcsTess, _T("%.2f set in D3DRS_PATCHSEGMENTS"), m_pSurfs[m_nSurfCount]->pTest->fRSTess);
			}
			else
			{
				_stprintf(m_pSurfs[m_nSurfCount]->tcsTess, _T("%.2f,%.2f,%.2f set in PATCH_INFO"),
						  g_TriTess[m_pSurfs[m_nSurfCount]->pTest->nTessIndex][0], 
						  g_TriTess[m_pSurfs[m_nSurfCount]->pTest->nTessIndex][1], 
						  g_TriTess[m_pSurfs[m_nSurfCount]->pTest->nTessIndex][2]);
			}

			m_nSurfCount++;
		}

		if(Tri1.pVertices)
			free(Tri1.pVertices);
		if(Tri1.pIndices)
			free(Tri1.pIndices);

	}

	return true;
}


UINT CTriSurf::TestInitialize(void)
{
	UINT uRes = CHOSurf::TestInitialize();

	// With proper tessellation, the following tests fail, because ref's tessellation is bad.
	// Some are partly due to ref not implementing fractional tessellation...
	if(uRes == D3DTESTINIT_RUN)
	{
		// The following tests don't pass 85% conformance
		SkipTestRange(157,160);
		SkipTestRange(162,165);
		SkipTestRange(282,285);
		// The following tests don't pass 90% conformance
		// uncomment if conformance % raised to 90%
/*		SkipTestRange(154,154);
		SkipTestRange(194,195);
		SkipTestRange(295,295);
*/
	}
	
	return uRes;
}

// ----------------------------------------------------------------------------

bool CTriSurf::PreExecuteTest(UINT uTestNumber)
{
	if(!CHOSurf::PreExecuteTest(uTestNumber))
		return false;
	else
	{
		if(m_bD3DXTessellation)
		{
/*			if(m_pSurfs[m_nCurrentSurf]->pTest->HODegree == D3DDEGREE_LINEAR)		// we can't do linear right now, so fail.  Remove this when linear is implemented
			{
	            SkipTestRange(uTestNumber, uTestNumber+m_nViewMatrix-1);
				return false;
			}
*/		}
	}

	return true;		// continue with test
}

// ----------------------------------------------------------------------------

bool CTriSurf::DrawSurface(HOSURFACE *pSurf)
{
	bool bRes = true;
	int nCached = 0;
    D3DTRIPATCH_INFO TriInfo, *pSurfInfo;
    float *f = pSurf->pTest->nTessIndex >= g_nTriTess ? NULL : g_TriTess[pSurf->pTest->nTessIndex];

	DWORD dwDeviceFlag = m_bD3DXTessellation ? CD3D_REF : CD3D_ALL;

	// D3DX Tessellation takes 3 edge values
	float pfEdgeTess[3];
	for(int i=0; i<3; i++)
		pfEdgeTess[i] = pSurf->pTest->fRSTess;

	float* pfEitherTess = pSurf->pTest->nTessIndex >= g_nTriTess ? pfEdgeTess : g_TriTess[pSurf->pTest->nTessIndex];

    // Set the patch style
	SetRenderState(D3DRS_PATCHEDGESTYLE, (DWORD)pSurf->pTest->HOPStyle, dwDeviceFlag);
   	if(m_dwVersion < 0x900)
	{
		// D3DRS_PATCHSEGMENTS = 164		TODO: Put this in framework header somewhere
	    SetRenderState((D3DRENDERSTATETYPE)164, (DWORD)*((unsigned*)&(pSurf->pTest->fRSTess)), dwDeviceFlag);
	}
	else
	{
		// no longer allowed to specify D3DRS_PATCHSEGMENTS and null under tess level here.  This makes sure
		// there is always a tess level specified here:
		f = pfEitherTess;
	}

    TriInfo.Basis = pSurf->pTest->HOBasis;
	TriInfo.Degree = pSurf->pTest->HODegree;

	if(pSurf->pTest->DrawMode == ONEPATCH)
	{
		int nSpan = (int)pSurf->pTest->HODegree+1;
		TriInfo.NumVertices = ((nSpan * nSpan) - nSpan) / 2 + nSpan;
		int nTris = pSurf->uVertices / TriInfo.NumVertices;
		pSurfInfo = m_nCurrentSurf == m_nLastSurf ? NULL : &TriInfo;

		for(int i=0; i<nTris; i++)
		{	
			if(!m_bD3DXTessellation && (pSurf->pTest->nShader != 0) && !(pSurf->pTest->nShader == 2 && pSurf->pTest->dwTexture == TEX_COORD0) && i & 1)
			{
				float f[]= {-1.f, -1.f, 0.f, 0.f};
                SetVertexShaderConstant(4, f, 1);
                SetTransform(D3DTS_TEXTURE0, &(m_TexTrans[pSurf->pTest->nShader]));
                SetTransform(D3DTS_TEXTURE1, &(m_TexTrans[1]));

				SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT2));
				SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)(D3DTTFF_COUNT2));
			}
			else
			{
				float f[]= {1.f, 1.f, 0.f, 0.f};
                SetVertexShaderConstant(4, f, 1);
				SetTextureStageState(0, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)D3DTTFF_DISABLE);
                SetTextureStageState(1, D3DTSS_TEXTURETRANSFORMFLAGS, (DWORD)D3DTTFF_DISABLE);
			}

			TriInfo.StartVertexOffset = i * TriInfo.NumVertices;
			bRes &= DrawTriPatch(pSurfInfo ? ++m_nSurfHandles : ++nCached, f, pSurfInfo, dwDeviceFlag);
			if(m_bD3DXTessellation)
			{
				// Now draw to src using D3DX to tessellate:
				bRes &= DrawD3DXTriPatch(m_pSurfs[m_nCurrentSurf]->pVB, pfEitherTess, m_pSurfs[m_nCurrentSurf]->pTest->nShader, &TriInfo, CD3D_SRC);
			}
		}
	}
	return bRes;
}

