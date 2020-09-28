#include "D3DTestPCH.h"

// --------------------------------------------------------------------------------------
// Including these definitions in CD3DTest.h adds dependancies on
// CD3D.h and CDevice.h to CD3DTest.h.  To avoid that, these defs are here.
// --------------------------------------------------------------------------------------

// --------------------------------------------------------------------------------------

CApiTest::CApiTest()
{
    SetTestType(TESTTYPE_API);
	
	m_nTest	= 0;
    m_nTestCounter = 0;
	m_bRunInvalid = false;
	m_bRunCrash = false;
    m_Options.D3DOptions.fMinDXVersion = 8.f;
}

// --------------------------------------------------------------------------------------

LPDIRECT3D8 CApiTest::GetD3D8()
{
	return m_pD3D->m_pD3D8;
}

// --------------------------------------------------------------------------------------

LPDIRECT3D9 CApiTest::GetD3D9()
{
	return m_pD3D->m_pSrcD3D9;
}

// --------------------------------------------------------------------------------------

LPDIRECT3D9EX CApiTest::GetD3D9Ex()
{
	return m_pD3D->m_pSrcD3D9Ex;
}

// --------------------------------------------------------------------------------------

LPDIRECT3DDEVICE8 CApiTest::GetDevice8(UINT n)
{
	if( m_DeviceList[n] )
		return m_DeviceList[n]->m_pDevice8;
	else
		return NULL;
}

// --------------------------------------------------------------------------------------

LPDIRECT3DDEVICE9 CApiTest::GetDevice9(UINT n)
{
	if( m_DeviceList[n] )
		return m_DeviceList[n]->m_pDevice9;
	else
		return NULL;
}

// --------------------------------------------------------------------------------------

LPDIRECT3DDEVICE9EX CApiTest::GetDevice9Ex(UINT n)
{
	if( m_DeviceList[n] )
		return m_DeviceList[n]->m_pDevice9Ex;
	else
		return NULL;
}

// --------------------------------------------------------------------------------------

LPDIRECT3DSURFACE9 CApiTest::GetResource9(CSurface *pSurface)
{
	return (LPDIRECT3DSURFACE9)pSurface->GetInternalPointer();
}

// --------------------------------------------------------------------------------------

LPDIRECT3DTEXTURE9 CApiTest::GetResource9(CTexture *pTexture)
{
	return (LPDIRECT3DTEXTURE9)pTexture->GetInternalPointer();
}

// --------------------------------------------------------------------------------------

LPDIRECT3DCUBETEXTURE9 CApiTest::GetResource9(CCubeTexture *pTexture)
{
	return (LPDIRECT3DCUBETEXTURE9)pTexture->GetInternalPointer();
}

// --------------------------------------------------------------------------------------

LPDIRECT3DVOLUMETEXTURE9 CApiTest::GetResource9(CVolTexture *pTexture)
{
	return (LPDIRECT3DVOLUMETEXTURE9)pTexture->GetInternalPointer();
}

// --------------------------------------------------------------------------------------

LPDIRECT3DVOLUME9 CApiTest::GetResource9(CVolume *pVolume)
{
	return (LPDIRECT3DVOLUME9)pVolume->GetInternalPointer();
}

// --------------------------------------------------------------------------------------

DWORD CApiTest::GetMemoryUsage()
{
	MEMORYSTATUS ms;
	GlobalMemoryStatus( &ms );
	return ms.dwMemoryLoad;
}

// --------------------------------------------------------------------------------------

UINT CApiTest::CountTestCases()
{
	m_nTestCounter = 0; 
	ExecuteTest(0); 
	return m_nTestCounter; 
}

// --------------------------------------------------------------------------------------

bool CApiTest::ParseCmdLine()
{
    if(KeySet(_T("INV")))
        m_bRunInvalid = true;
    if(KeySet(_T("CRASH")))
        m_bRunCrash = true;
	return CD3DTest::ParseCmdLine();
}

// --------------------------------------------------------------------------------------

UINT CApiTest::TestInitialize()
{
	SetTestRange(1, CountTestCases());
	return D3DTESTINIT_RUN;
}
