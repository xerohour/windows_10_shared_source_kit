#include "DevicePCH.h"

#include "CDevice.h"
#include "D3D9.h"

// -------------------------------------------------------------------------------------

CVertexShader::CVertexShader()
{
	m_dwVS 			= 0;
	m_pVS9 			= NULL;
    m_pDevice		= NULL;
    m_pDecl			= NULL;
}

// -------------------------------------------------------------------------------------

CVertexShader::~CVertexShader()
{
	RELEASE(m_pVS9);
	RELEASE(m_pDecl);
}

// -------------------------------------------------------------------------------------

void CVertexShader::SetDecl(CVertexDeclaration *pDecl)
{
	RELEASE(m_pDecl); 
	m_pDecl = pDecl; 
	m_pDecl->AddRef(); 
}

// -------------------------------------------------------------------------------------

bool CVertexShader::Init(PVOID pShader, CDevice *pDevice)
{
	bool bRet = false;

	if(NULL == pDevice)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	if(m_pDevice->GetInterface() >= 9.f && m_pDevice->GetInterface() < 10.f)
	{
		pGetFunction = &CVertexShader::GetFunction9;
        pGetDevice = &CVertexShader::GetDevice9;
		bRet = Init9(pShader);
	}
    if(m_pDevice->GetInterface() >= 8.f && m_pDevice->GetInterface() < 9.f)
	{
		pGetFunction = &CVertexShader::GetFunctionUnsup;
        pGetDevice = &CVertexShader::GetDeviceUnsup;
		m_dwVS = *((DWORD*)pShader);
		bRet = true;
	}	

	return bRet;
}

ULONG WINAPI CVertexShader::AddRef(void)
{
    if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CVertexShader::Release(void)
{   
	if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// -------------------------------------------------------------------------------------
// For now, we'll do everything in one file so we don't explode with tiny CPPs.

bool CVertexShader::Init9(PVOID pShader)
{
	m_pVS9 = (LPDIRECT3DVERTEXSHADER9)pShader;
	
	if(NULL != m_pVS9)
		m_pVS9->AddRef();

	return true;
}

// -------------------------------------------------------------------------------------

HRESULT CVertexShader::GetFunction9(PVOID pData, UINT *pSizeOfData)
{
	return m_pVS9->GetFunction(pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CVertexShader::GetDevice9(PVOID *pDevice)
{
	return m_pVS9->GetDevice((LPDIRECT3DDEVICE9*)pDevice);
}

// -------------------------------------------------------------------------------------

HRESULT CVertexShader::GetFunctionUnsup(PVOID pData, UINT *pSizeOfDate)
{
	DPF(1, "CVertexShader: GetFunction() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
    return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CVertexShader::GetDeviceUnsup(PVOID *pDevice)
{
	DPF(1, "CVertexShader: GetDevice() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
    return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

CnVertexShader::CnVertexShader()
{
	m_pShaders = NULL;
	m_uDevices = 0;
}

CnVertexShader::~CnVertexShader()
{
	if(m_pShaders)
	{
		for( int i= 0; i< m_uDevices; i++ )
		{
			RELEASE( m_pShaders[i] );
		}

		SAFEDELETEA(m_pShaders);
	}
}

bool CnVertexShader::Init(UINT uDevices)
{
	m_uDevices = uDevices;

	if((m_pShaders = new CVertexShader*[m_uDevices]) == NULL)
		return false;
	memset(m_pShaders, 0, m_uDevices * sizeof(CVertexShader*));
	
	return true;
}

bool CnVertexShader::GetVShader(CVertexShader **ppVShader, UINT n)
{
	if( n >= m_uDevices || ppVShader == NULL || m_pShaders == NULL )
		return false;
	*ppVShader = m_pShaders[n];
	if(*ppVShader)
		(*ppVShader)->AddRef();
	return true;
}

bool CnVertexShader::SetVShader(CVertexShader *pVShader, UINT n)
{
	if( n >= m_uDevices )
		return false;

	if( m_pShaders[n] != NULL )
		return false;

	m_pShaders[n] = pVShader;
	return true;
}

bool CnVertexShader::SkipDevice(UINT uDevice, DWORD dwFlags)
{
	DWORD dwMask = 1;

	for(int i=0; i<uDevice; i++)
		dwMask <<= 1;

	return (dwMask & dwFlags) == 0;
}

// TODO: add additional vertex shader methods...

// -------------------------------------------------------------------------------------

CPShader::CPShader()
{
	m_dwPS 			= 0;
	m_pPS9 			= NULL;
    m_pDevice		= NULL;
}

// -------------------------------------------------------------------------------------

CPShader::~CPShader()
{
	RELEASE(m_pPS9);
}

// -------------------------------------------------------------------------------------

bool CPShader::Init(PVOID pShader, CDevice *pDevice)
{
	bool bRet = false;

	if(NULL == pDevice)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	if(m_pDevice->GetInterface() >= 9.f && m_pDevice->GetInterface() < 10.f)
	{
		pGetFunction = &CPShader::GetFunction9;
        pGetDevice = &CPShader::GetDevice9;
		bRet = Init9(pShader);
	}
	else if(m_pDevice->GetInterface() >= 8.f && m_pDevice->GetInterface() < 9.f)
	{
		pGetFunction = &CPShader::GetFunctionUnsup;
        pGetDevice = &CPShader::GetDeviceUnsup;
		m_dwPS = *((DWORD*)pShader);
		bRet = true;
	}

	return bRet;
}

ULONG WINAPI CPShader::AddRef(void)
{
    if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// --------------------------------------------------------------------------------------

CnPixelShader::CnPixelShader()
{
	m_pShaders = NULL;
	m_uDevices = 0;
}

CnPixelShader::~CnPixelShader()
{
	if(m_pShaders)
	{
		for( int i= 0; i< m_uDevices; i++ )
		{
			RELEASE( m_pShaders[i] );
		}

		SAFEDELETEA(m_pShaders);
	}
}

bool CnPixelShader::Init(UINT uDevices)
{
	m_uDevices = uDevices;

	if((m_pShaders = new CPShader*[m_uDevices]) == NULL)
		return false;
	memset(m_pShaders, 0, m_uDevices * sizeof(CPShader*));
	
	return true;
}

bool CnPixelShader::GetPShader(CPShader **ppPShader, UINT n)
{
	if( n >= m_uDevices || ppPShader == NULL || m_pShaders == NULL )
		return false;
	*ppPShader = m_pShaders[n];
	if(*ppPShader)
		(*ppPShader)->AddRef();
	return true;
}

bool CnPixelShader::SetPShader(CPShader *pPShader, UINT n)
{
	if( n >= m_uDevices )
		return false;

	if( m_pShaders[n] != NULL )
		return false;

	m_pShaders[n] = pPShader;
	return true;
}

bool CnPixelShader::SkipDevice(UINT uDevice, DWORD dwFlags)
{
	DWORD dwMask = 1;

	for(int i=0; i<uDevice; i++)
		dwMask <<= 1;

	return (dwMask & dwFlags) == 0;
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CPShader::Release(void)
{   
	if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// -------------------------------------------------------------------------------------
// For now, we'll do everything in one file so we don't explode with tiny CPPs.

bool CPShader::Init9(PVOID pShader)
{
	if(NULL == pShader)
		return false;

	m_pPS9 = (LPDIRECT3DPIXELSHADER9)pShader;
	m_pPS9->AddRef();

	return true;
}

// -------------------------------------------------------------------------------------

HRESULT CPShader::GetFunction9(PVOID pData, UINT *pSizeOfData)
{
	return m_pPS9->GetFunction(pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CPShader::GetDevice9(PVOID *pDevice)
{
	return m_pPS9->GetDevice((LPDIRECT3DDEVICE9*)pDevice);
}

// -------------------------------------------------------------------------------------

HRESULT CPShader::GetFunctionUnsup(PVOID pData, UINT *pSizeOfDate)
{
	DPF(1, "CPShader: GetFunction() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
    return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CPShader::GetDeviceUnsup(PVOID *pDevice)
{
	DPF(1, "CPShader: GetDevice() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
    return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

CVertexDeclaration::CVertexDeclaration()
{
	m_pDecl9 			= NULL;
    m_pDevice		= NULL;
}

// -------------------------------------------------------------------------------------

CVertexDeclaration::~CVertexDeclaration()
{
	RELEASE(m_pDecl9);
}

// -------------------------------------------------------------------------------------

bool CVertexDeclaration::Init(PVOID pDecl, CDevice *pDevice)
{
	bool bRet = false;

	if(NULL == pDevice)
		return false;

	m_pDevice = pDevice;
	m_pDevice->AddRef();

	if(m_pDevice->GetInterface() >= 9.f && m_pDevice->GetInterface() < 10.f)
	{
		pGetDeclaration = &CVertexDeclaration::GetDeclaration9;
        pGetDevice = &CVertexDeclaration::GetDevice9;
		bRet = Init9(pDecl);
	}

	return bRet;
}

ULONG WINAPI CVertexDeclaration::AddRef(void)
{
    if(m_pDevice)
		m_pDevice->AddRef();
	return CObject::AddRef();
}

// --------------------------------------------------------------------------------------

ULONG WINAPI CVertexDeclaration::Release(void)
{   
	if(m_pDevice)
		m_pDevice->Release();
	return CObject::Release();
}

// -------------------------------------------------------------------------------------
// For now, we'll do everything in one file so we don't explode with tiny CPPs.

bool CVertexDeclaration::Init9(PVOID pShader)
{
	if(NULL == pShader)
		return false;

	m_pDecl9 = (LPDIRECT3DVERTEXDECLARATION9)pShader;
	m_pDecl9->AddRef();

	return true;
}

// -------------------------------------------------------------------------------------

HRESULT CVertexDeclaration::GetDeclaration9(VERTEXELEMENT* pData, UINT *pSizeOfData)
{
	return m_pDecl9->GetDeclaration((D3DVERTEXELEMENT9*)pData, pSizeOfData);
}

// -------------------------------------------------------------------------------------

HRESULT CVertexDeclaration::GetDevice9(PVOID *pDevice)
{
	return m_pDecl9->GetDevice((LPDIRECT3DDEVICE9*)pDevice);
}

// -------------------------------------------------------------------------------------

HRESULT CVertexDeclaration::GetDeclarationUnsup(VERTEXELEMENT* pData, UINT *pSizeOfDate)
{
	DPF(1, "CVertexDeclaration: GetFunction() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
    return E_NOTIMPL;
}

// -------------------------------------------------------------------------------------

HRESULT CVertexDeclaration::GetDeviceUnsup(PVOID *pDevice)
{
	DPF(1, "CVertexDeclaration: GetDevice() unsupported on version %4.2f interfaces.\n", m_pDevice->GetInterface());
    return E_NOTIMPL;
}


// -------------------------------------------------------------------------------------
// -------------------------------------------------------------------------------------

CnVertexDeclaration::CnVertexDeclaration()
{
	m_pDecls = NULL;
	m_uDevices = 0;
}

CnVertexDeclaration::~CnVertexDeclaration()
{
	if(m_pDecls)
	{
		for( int i= 0; i< m_uDevices; i++ )
		{
			RELEASE( m_pDecls[i] );
		}

		SAFEDELETEA(m_pDecls);
	}
}

bool CnVertexDeclaration::SkipDevice(UINT uDevice, DWORD dwFlags)
{
	DWORD dwMask = 1;

	for(int i=0; i<uDevice; i++)
		dwMask <<= 1;

	return (dwMask & dwFlags) == 0;
}

bool CnVertexDeclaration::Init(UINT uDevices)
{
	m_uDevices = uDevices;

	if((m_pDecls = new CVertexDeclaration*[m_uDevices]) == NULL)
		return false;
	memset(m_pDecls, 0, m_uDevices * sizeof(CVertexDeclaration*));
	
	return true;
}

bool CnVertexDeclaration::GetVDecl(CVertexDeclaration **ppVDecl, UINT n)
{
	if( n >= m_uDevices || ppVDecl == NULL || m_pDecls == NULL )
		return false;
	*ppVDecl = m_pDecls[n];
	if(*ppVDecl)
		(*ppVDecl)->AddRef();
	return true;
}

bool CnVertexDeclaration::SetVDecl(CVertexDeclaration *pVDecl, UINT n)
{
	if( n >= m_uDevices )
		return false;

	if( m_pDecls[n] != NULL )
		return false;

	m_pDecls[n] = pVDecl;
	return true;
}
