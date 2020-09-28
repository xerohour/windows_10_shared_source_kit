// Includes
#include "ProxyRef11.h"

//
// Defines
//

#define RETURN_SETDDIHELPER_ALLDEVFUNCTABLES(TYPE,CALLBACKFN,PFN) return SetDDIHelper<TYPE>(CALLBACKFN, \
		&(m_pD3D10DDI_DEVFUNCS->PFN), \
		&(m_pD3D10_1DDI_DEVFUNCS->PFN), \
		&(m_pD3D11DDI_DEVFUNCS->PFN), \
		&(m_pD3D11_1DDI_DEVFUNCS->PFN));

#define SETDDI_FUNC_ALLDEVFUNCTABLES(TYPE,FUNCNAME,PFN) TYPE CProxyRef11::FUNCNAME(TYPE callBackFn)\
	{\
		RETURN_SETDDIHELPER_ALLDEVFUNCTABLES(TYPE, callBackFn, PFN)\
	}

#define RETURN_SETDDIHELPER_10_XDEVFUNCTABLES(TYPE,CALLBACKFN,PFN) return SetDDIHelper<TYPE>(CALLBACKFN, \
		&(m_pD3D10DDI_DEVFUNCS->PFN), \
		&(m_pD3D10_1DDI_DEVFUNCS->PFN), \
		NULL, \
		NULL);

#define SETDDI_FUNC_10_XDEVFUNCTABLES(TYPE,FUNCNAME,PFN) TYPE CProxyRef11::FUNCNAME(TYPE callBackFn)\
	{\
		RETURN_SETDDIHELPER_10_XDEVFUNCTABLES(TYPE, callBackFn, PFN)\
	}

#define RETURN_SETDDIHELPER_11_0DEVFUNCTABLE(TYPE,CALLBACKFN,PFN) return SetDDIHelper<TYPE>(CALLBACKFN, \
		NULL, \
		NULL, \
		&(m_pD3D11DDI_DEVFUNCS->PFN), \
		NULL);

#define SETDDI_FUNC_11_0DEVFUNCTABLES(TYPE,FUNCNAME,PFN) TYPE CProxyRef11::FUNCNAME(TYPE callBackFn)\
	{\
		RETURN_SETDDIHELPER_11_0DEVFUNCTABLE(TYPE, callBackFn, PFN)\
	}

#define RETURN_SETDDIHELPER_11_XDEVFUNCTABLES(TYPE,CALLBACKFN,PFN) return SetDDIHelper<TYPE>(CALLBACKFN, \
		NULL, \
		NULL, \
		&(m_pD3D11DDI_DEVFUNCS->PFN), \
		&(m_pD3D11_1DDI_DEVFUNCS->PFN));

#define SETDDI_FUNC_11_XDEVFUNCTABLES(TYPE,FUNCNAME,PFN) TYPE CProxyRef11::FUNCNAME(TYPE callBackFn)\
	{\
		RETURN_SETDDIHELPER_11_XDEVFUNCTABLES(TYPE, callBackFn, PFN)\
	}

#define RETURN_SETDDIHELPER_10_X_11_0DEVFUNCTABLES(TYPE,CALLBACKFN,PFN) return SetDDIHelper<TYPE>(CALLBACKFN, \
		&(m_pD3D10DDI_DEVFUNCS->PFN), \
		&(m_pD3D10_1DDI_DEVFUNCS->PFN), \
		&(m_pD3D11DDI_DEVFUNCS->PFN), \
		NULL);

#define SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(TYPE,FUNCNAME,PFN) TYPE CProxyRef11::FUNCNAME(TYPE callBackFn)\
	{\
		RETURN_SETDDIHELPER_10_X_11_0DEVFUNCTABLES(TYPE, callBackFn, PFN)\
	}

#define RETURN_SETDDIHELPER_11_1DEVFUNCTABLES(TYPE,CALLBACKFN,PFN) return SetDDIHelper<TYPE>(CALLBACKFN, \
		NULL, \
		NULL, \
		NULL, \
		&(m_pD3D11_1DDI_DEVFUNCS->PFN));

#define SETDDI_FUNC_11_1DEVFUNCTABLES(TYPE,FUNCNAME,PFN) TYPE CProxyRef11::FUNCNAME(TYPE callBackFn)\
	{\
		RETURN_SETDDIHELPER_11_1DEVFUNCTABLES(TYPE, callBackFn, PFN)\
	}

////////////////////////

CProxyRef11::CProxyRef11( ) 
	: m_hD3DProxyDLL( NULL )
	, m_pD3D10_1DDI_DEVFUNCS( NULL )
	, m_pD3D10DDI_DEVFUNCS( NULL )
	, m_pD3D11DDI_DEVFUNCS( NULL )
	, m_pD3D11_1DDI_DEVFUNCS( NULL )
	, m_pD3D10_2DDI_ADAPTERFUNCS( NULL )
	, m_pD3D10DDI_ADAPTERFUNCS( NULL )
	, m_pDXGI11DDI_BASEFUNCS( NULL )
	, m_pDXGI_KMTHUNKS( NULL )
	, m_proxySetup( PROXY_SETUP_OK )
	, PD_GetDeviceDLL( NULL )
	, PD_GetDeviceVersion( NULL )
	, PD_GetOpenAdapterVersion( NULL )
	, PD_GetRuntimeDevice( NULL )
	, PD_SetErrorCb( NULL )
	, PD_ResetRuntimeFunctions( NULL )
{
}

/////

CProxyRef11::~CProxyRef11(void)
{
	Cleanup();
}

/////

PROXY_SETUP CProxyRef11::Setup( tstring d3dProxyDriverDll, tstring d3d11DeviceDLL, 
							   PD_OPENADAPTER10_FP* openAdapterCallback )
{
	////////
	// Retrieve the Proxy Driver DLL: 
	// the proxy driver should have been loaded by D3D11Test before using Proxyref
	m_hD3DProxyDLL = LoadLibrary(d3dProxyDriverDll.c_str());
	if( !m_hD3DProxyDLL )
	{
		Cleanup();
		return PROXY_SETUP_PROXY_DLL_FAILED;
	}

	////////
	// Set the device dll
	// Check to see if null ref should be hooked
	if(d3d11DeviceDLL == "")
		d3d11DeviceDLL = D3D11_DEVICE_11NULLREF;

	// Capture the dll name and the device version
	PD_GetDeviceDLL = (PD_GET_DEVICE_DLL_FP)GetProcAddress(m_hD3DProxyDLL, "PD_GetDeviceDLL");
	PD_GetDeviceVersion = (PD_GET_DEVICE_VERSION_FP)GetProcAddress(m_hD3DProxyDLL, "PD_GetDeviceVersion");
	PD_GetOpenAdapterVersion = (PD_GET_OPEN_ADAPTER_VERSION_FP)GetProcAddress(m_hD3DProxyDLL, "PD_GetOpenAdapterVersion");
	PD_GetRuntimeDevice = (PD_GET_RT_DEVICE_FP)GetProcAddress(m_hD3DProxyDLL, "PD_GetRuntimeDevice");
	PD_SetErrorCb = (PD_SET_ERROR_CB_FP)GetProcAddress(m_hD3DProxyDLL, "PD_SetErrorCb");
	PD_ResetRuntimeFunctions = (PD_RESET_RUNTIME_FUNCS_FP)GetProcAddress(m_hD3DProxyDLL, "PD_ResetRuntimeFunctions");

	// Set the Proxy driver's device dll, for example: "d3d10ref.DLL"
	PD_SET_DEVICE_DLL_FP PD_SpecifyDeviceDLL = (PD_SET_DEVICE_DLL_FP)GetProcAddress(m_hD3DProxyDLL, "PD_SetDeviceDLL");
	PD_SpecifyDeviceDLL( d3d11DeviceDLL.c_str() );

	// Capture the dll name (Global variable in CD3D10ProxyDriver)
	PD_GetDeviceDLL(m_dllName);

	// Check the device dll was set in the proxy driver 
	if( !m_dllName[0] )
	{
		Cleanup();
		return PROXY_SETUP_DEVICE_DLL_FAILED;
	}

	/////////
	// Check to see if the CreateDevice should be hooked
	if(openAdapterCallback)
		SetOpenAdapterCallback(openAdapterCallback);

	
	return PROXY_SETUP_OK;
}


/////

void CProxyRef11::ResetDeviceFunctions( )
{
	if (PD_ResetRuntimeFunctions)
		PD_ResetRuntimeFunctions();
}

/////

void CProxyRef11::Cleanup( )
{
    if (m_hD3DProxyDLL)
    {
        // Let the proxy driver perform cleanup before being unloaded.
        PD_SET_DEVICE_DLL_FP PD_SetDeviceDLL = (PD_SET_DEVICE_DLL_FP)GetProcAddress(m_hD3DProxyDLL, "PD_SetDeviceDLL");
        PD_SetDeviceDLL(NULL);

        FreeLibrary(m_hD3DProxyDLL);
    }

	m_hD3DProxyDLL = NULL;
	PD_GetDeviceDLL = NULL;
	PD_GetDeviceVersion = NULL;
	PD_GetOpenAdapterVersion = NULL;
	PD_GetRuntimeDevice = NULL;
	PD_SetErrorCb = NULL;
	PD_ResetRuntimeFunctions = NULL;
}

/////

PROXY_SETUP CProxyRef11::LoadProxyDriverDLL( tstring d3d11DeviceDLL )
{
	return Setup( D3D11_PROXYDRIVER_DLLNAME, d3d11DeviceDLL, NULL );
}

/////

eOpenAdapterVersion CProxyRef11::GetOpenAdapterVersionUsed( )
{
	PD_GET_OPEN_ADAPTER_VERSION_FP PD_GetOpenAdapterVersion = (PD_GET_OPEN_ADAPTER_VERSION_FP)GetProcAddress(m_hD3DProxyDLL, "PD_GetOpenAdapterVersion");
	return PD_GetOpenAdapterVersion();
}

/////

D3D10DDI_HRTDEVICE CProxyRef11::GetRealRTDevice( )
{
	PD_GET_RT_DEVICE_FP PD_GetRuntimeDevice = (PD_GET_RT_DEVICE_FP)GetProcAddress(m_hD3DProxyDLL, "PD_GetRuntimeDevice");
	return PD_GetRuntimeDevice();
}

/////

void CProxyRef11::SetError( HRESULT hr )
{
	if (PD_SetErrorCb)
		PD_SetErrorCb( hr );
}

/////

void CProxyRef11::SetOpenAdapterCallback(PD_OPENADAPTER10_FP* callBack)
{
	// Setup create device callback
	PD_REGISTER_OPEN_ADAPTER_FP PD_RegisterOpenAdapterCallback = (PD_REGISTER_OPEN_ADAPTER_FP)GetProcAddress(m_hD3DProxyDLL, "PD_RegisterOpenAdapter10Callback");
	PD_RegisterOpenAdapterCallback(callBack);
}

/////

PROXY_LISTENERS CProxyRef11::CaptureDriverListenerFunctions()
{
	// Register Device functions
	PD_REGISTER_DRIVER_FUNCS_11_1_FP PD_RegisterDriverListenerFunctions11_1 = (PD_REGISTER_DRIVER_FUNCS_11_1_FP)GetProcAddress(m_hD3DProxyDLL, "PD_RegisterDriverListenerFunctions11_1");
	PD_REGISTER_DRIVER_FUNCS_11_0_FP PD_RegisterDriverListenerFunctions11_0 = (PD_REGISTER_DRIVER_FUNCS_11_0_FP)GetProcAddress(m_hD3DProxyDLL, "PD_RegisterDriverListenerFunctions11_0");
	PD_REGISTER_DRIVER_FUNCS_10_1_FP PD_RegisterDriverListenerFunctions10_1 = (PD_REGISTER_DRIVER_FUNCS_10_1_FP)GetProcAddress(m_hD3DProxyDLL, "PD_RegisterDriverListenerFunctions10_1");
	PD_REGISTER_DRIVER_FUNCS_10_0_FP PD_RegisterDriverListenerFunctions10_0 = (PD_REGISTER_DRIVER_FUNCS_10_0_FP)GetProcAddress(m_hD3DProxyDLL, "PD_RegisterDriverListenerFunctions10_0");
	PD_REGISTER_ADAPTER_FUNCS_10_2_FP PD_RegisterAdapterListenerFunctions10_2 = (PD_REGISTER_ADAPTER_FUNCS_10_2_FP)GetProcAddress(m_hD3DProxyDLL, "PD_RegisterAdapterListenerFunctions10_2");
	PD_REGISTER_ADAPTER_FUNCS_10_FP PD_RegisterAdapterListenerFunctions10_0 = (PD_REGISTER_ADAPTER_FUNCS_10_FP)GetProcAddress(m_hD3DProxyDLL, "PD_RegisterAdapterListenerFunctions10_0");
	PD_REGISTER_DRIVER_DXGI_BASE_CALLBACKS_FP PD_RegisterDriverDxgiCallbacks = (PD_REGISTER_DRIVER_DXGI_BASE_CALLBACKS_FP)GetProcAddress(m_hD3DProxyDLL, "PD_RegisterDriverDxgiCallbacks");
	PD_REGISTER_DRIVER_DXGI_BASE_FUNCS_11_FP PD_RegisterDriverDxgiBaseFuncs11 = (PD_REGISTER_DRIVER_DXGI_BASE_FUNCS_11_FP)GetProcAddress(m_hD3DProxyDLL, "PD_RegisterDriverDxgiBaseFuncs11");
	PD_REGISTER_DXGI_KMTHUNKS_FP PD_RegisterDxgiKMThunks = (PD_REGISTER_DXGI_KMTHUNKS_FP)GetProcAddress(m_hD3DProxyDLL, "PD_RegisterDxgiKMThunks");

	// Capture the driver listener functions
	m_pD3D11_1DDI_DEVFUNCS = PD_RegisterDriverListenerFunctions11_1();
	m_pD3D11DDI_DEVFUNCS = PD_RegisterDriverListenerFunctions11_0();
	m_pD3D10_1DDI_DEVFUNCS = PD_RegisterDriverListenerFunctions10_1();
	m_pD3D10DDI_DEVFUNCS = PD_RegisterDriverListenerFunctions10_0();
	m_pD3D10_2DDI_ADAPTERFUNCS = PD_RegisterAdapterListenerFunctions10_2();
	m_pD3D10DDI_ADAPTERFUNCS = PD_RegisterAdapterListenerFunctions10_0();
	m_pDXGIDDI_BASECALLBACKS = PD_RegisterDriverDxgiCallbacks();
	m_pDXGI11DDI_BASEFUNCS = PD_RegisterDriverDxgiBaseFuncs11();
	m_pDXGI_KMTHUNKS = PD_RegisterDxgiKMThunks();
	
	// Check to see if the device functions were loaded
	if( !m_pD3D11_1DDI_DEVFUNCS )
		return PROXY_LISTENERS_11_1_DDI_DEVFUNCTS_FAILED;

	if( !m_pD3D11DDI_DEVFUNCS )
		return PROXY_LISTENERS_11_DDI_DEVFUNCTS_FAILED;

	if( !m_pD3D10_1DDI_DEVFUNCS )
		return PROXY_LISTENERS_10_1_DDI_DEVFUNCTS_FAILED;

	if( !m_pD3D10DDI_DEVFUNCS )
		return PROXY_LISTENERS_10_DDI_DEVFUNCTS_FAILED;

	if( !m_pD3D10_2DDI_ADAPTERFUNCS )
		return PROXY_LISTENERS_10_2_DDI_ADAPFUNCTS_FAILED;

	if( !m_pD3D10DDI_ADAPTERFUNCS )
		return PROXY_LISTENERS_10_DDI_ADAPFUNCTS_FAILED;

	if (!m_pDXGIDDI_BASECALLBACKS)
		return PROXY_LISTENERS_DXGI_BASE_CALLBACKS_FAILED;

	if ( !m_pDXGI11DDI_BASEFUNCS )
		return PROXY_LISTENERS_DXGI_BASE_FUNCTS_FAILED;

	if ( !m_pDXGI_KMTHUNKS )
		return PROXY_LISTENERS_DXGI_KMTHUNKS_FAILED;

	// Successfully caqptured the driver listener functions
	return PROXY_LISTENERS_OK;
}

/////
// ??? In the APIFeatureLevelsTest, check if the feature level and device context 
// are dealt with correctly
HRESULT CProxyRef11::CreateDevice11(ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext,
									D3D_FEATURE_LEVEL fl, UINT createDeviceFlags)
{
	// Local variables
	HRESULT hr;
	D3D_FEATURE_LEVEL outFl;

	// Create the device
	// ??? For both in and out of "D3D_FEATURE_LEVEL* pFeatureLevels" parameters, put NULL for now
	hr = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_SOFTWARE, m_hD3DProxyDLL, createDeviceFlags, 
                                        &fl, 1, D3D11_SDK_VERSION, ppDevice, &outFl, ppContext );
	if(FAILED(hr))
		return hr;

	// Capture the device listener functions
	if(CaptureDriverListenerFunctions() != PROXY_LISTENERS_OK)
		return E_FAIL;

	return S_OK;
}

/////

// ??? In the APIFeatureLevelsTest, check if the feature level and device context 
// are dealt with correctly
HRESULT CProxyRef11::CreateDevice11(ID3D11Device** ppDevice, D3D_DRIVER_TYPE DriverType, ID3D11DeviceContext** ppContext,
                                    D3D_FEATURE_LEVEL fl, UINT createDeviceFlags)
{
	// Local variables
	HRESULT hr;
	D3D_FEATURE_LEVEL outFl;

	// Create the device
	// ??? For both in and out of "D3D_FEATURE_LEVEL* pFeatureLevels" parameters, put NULL for now
	hr = D3D11CreateDevice( NULL, DriverType, ( DriverType == D3D_DRIVER_TYPE_SOFTWARE ) ? m_hD3DProxyDLL : NULL, 
							createDeviceFlags, &fl, 1, D3D11_SDK_VERSION, ppDevice, &outFl, ppContext );
	if(FAILED(hr))
	return hr;

	// Capture the device listener functions
	if(CaptureDriverListenerFunctions() != PROXY_LISTENERS_OK)
		return E_FAIL;

	return S_OK;
}

/////

// ??? In the APIFeatureLevelsTest, check if the feature level and device context 
// are dealt with correctly
HRESULT CProxyRef11::CreateDevice11(ID3D11Device** ppDevice, ID3D11DeviceContext** ppContext,
									D3D_FEATURE_LEVEL* pFl, UINT numFeatureLevels,
									D3D_FEATURE_LEVEL* pOutFl, UINT createDeviceFlags)
{
	// Local variables
	HRESULT hr;

	// Create the device
	// ??? For both in and out of "D3D_FEATURE_LEVEL* pFeatureLevels" parameters, put NULL for now
	hr = D3D11CreateDevice( NULL, D3D_DRIVER_TYPE_SOFTWARE, m_hD3DProxyDLL, createDeviceFlags, 
                                        pFl, numFeatureLevels, D3D11_SDK_VERSION, ppDevice, pOutFl, ppContext );
	if(FAILED(hr))
		return hr;

	// Capture the device listener functions
	if(CaptureDriverListenerFunctions() != PROXY_LISTENERS_OK)
		return E_FAIL;

	return S_OK;
}

/////

HRESULT CProxyRef11::CreateDevice10_1(ID3D10Device1** ppDevice, D3D10_FEATURE_LEVEL1 fl, UINT createDeviceFlags)
{
	// Local variables
	HRESULT hr;

	// Create the device
	hr = D3D10CreateDevice1( NULL, D3D10_DRIVER_TYPE_SOFTWARE, m_hD3DProxyDLL, createDeviceFlags, 
                                        fl, D3D10_1_SDK_VERSION, ppDevice );
	if(FAILED(hr))
		return hr;

	// Capture the device listener functions
	if(CaptureDriverListenerFunctions() != PROXY_LISTENERS_OK)
		return E_FAIL;

	return S_OK;
}

/////

HRESULT CProxyRef11::CreateDevice10_1(ID3D10Device1** ppDevice, D3D10_DRIVER_TYPE DriverType, D3D10_FEATURE_LEVEL1 fl, UINT createDeviceFlags)
{
	// Local variables
	HRESULT hr;

	// Create the device
	hr = D3D10CreateDevice1( NULL, DriverType, ( DriverType == D3D_DRIVER_TYPE_SOFTWARE ) ? m_hD3DProxyDLL : NULL, 
							 createDeviceFlags, fl, D3D10_1_SDK_VERSION, ppDevice );
	if(FAILED(hr))
		return hr;

	// Capture the device listener functions
	if(CaptureDriverListenerFunctions() != PROXY_LISTENERS_OK)
		return E_FAIL;

	return S_OK;
}

/////

HRESULT CProxyRef11::CreateDevice10(ID3D10Device** ppDevice, UINT createDeviceFlags)
{
	// Local variables
	HRESULT hr;

	// Create the device
	hr = D3D10CreateDevice( NULL, D3D10_DRIVER_TYPE_SOFTWARE, m_hD3DProxyDLL, createDeviceFlags, D3D10_SDK_VERSION, ppDevice );

	if(FAILED(hr))
		return hr;

	// Capture the device listener functions
	if(CaptureDriverListenerFunctions() != PROXY_LISTENERS_OK)
		return E_FAIL;

	return S_OK;
}


/////

HMODULE	CProxyRef11::GetProxyDriverHandle()
{
	return m_hD3DProxyDLL;
}

/////

template <typename _T> 
_T CProxyRef11::SetDDIHelper(_T callBackFn, _T* pfn10, _T* pfn10_1, _T* pfn11, _T* pfn11_1)
{
	// Check the device version
	m_deviceVersion = PD_GetDeviceVersion();

	if(m_deviceVersion == 0)
		return NULL;

	// Create a temp variable for storing the current value
	_T p_oldPfn = NULL;

	switch(m_deviceVersion)
	{
	default:
		// unknown version
		return NULL;

	case D3D11_1_DDI_INTERFACE_VERSION:
		// Record the previous value
		p_oldPfn = *pfn11_1;
		// Set the new callBack
		*pfn11_1 = callBackFn;
		break;

	case D3D11_0_vista_DDI_INTERFACE_VERSION:
	case D3D11_0_DDI_INTERFACE_VERSION:
	case D3D11_0_7_DDI_INTERFACE_VERSION:
		// Record the previous value
		p_oldPfn = *pfn11;
		// Set the new callBack
		*pfn11 = callBackFn;
		break;
	
	case D3D10_1_DDI_INTERFACE_VERSION:
	case D3D10_1_x_vista_DDI_INTERFACE_VERSION:
	case D3D10_1_x_DDI_INTERFACE_VERSION:
	case D3D10_1_7_DDI_INTERFACE_VERSION:
		// Record the previous value
		p_oldPfn = *pfn10_1;
		// Set the new callBack
		*pfn10_1 = callBackFn;
		break;
	
	case D3D10_0_DDI_INTERFACE_VERSION:
	case D3D10_0_x_vista_DDI_INTERFACE_VERSION:
	case D3D10_0_x_DDI_INTERFACE_VERSION:
	case D3D10_0_7_DDI_INTERFACE_VERSION:
		// Record the previous value
		p_oldPfn = *pfn10;
		// Set the new callBack
		*pfn10 = callBackFn;
		break;
	}

	// Return the previous value of the function pointer
	return p_oldPfn;
}

template <typename _T> 
_T CProxyRef11::SetOADDIHelper(_T callBackFn, _T* pfn10, _T* pfn10_2)
{
	// Check the device version
	eOpenAdapterVersion eOAV = PD_GetOpenAdapterVersion();

	if(eOAV == OAV_Unknown)
		return NULL;

	// Create a temp variable for storing the current value
	_T p_oldPfn = NULL;

	if (eOAV == OAV_OpenAdapter10_2)
	{
		// Record the previous value
		p_oldPfn = *pfn10_2;

		// Set the new callBack
		*pfn10_2 = callBackFn;
	}
	else if (eOAV == OAV_OpenAdapter10)
	{
		// Record the previous value
		p_oldPfn = *pfn10;

		// Set the new callBack
		*pfn10 = callBackFn;
	}

	// Return the previous value of the function pointer
	return p_oldPfn;
}

template <typename _T> 
_T CProxyRef11::SetDXGIDDIHelper(_T callBackFn, _T* pfnDxgi)
{
	// Create a temp variable for storing the current value
	_T p_oldPfn = *pfnDxgi;

	// Set the new callBack
	*pfnDxgi = callBackFn;

	// Return the previous value of the function pointer
	return p_oldPfn;
}

////////////////////////
/* Below here are all the definitions for the SetDDI functions */
/* They utilize the SetDDIHelper function above */

PFND3D10DDI_CREATEDEVICE CProxyRef11::SetOADDI_CreateDevice( PFND3D10DDI_CREATEDEVICE callBackFn )
{
	return SetOADDIHelper<PFND3D10DDI_CREATEDEVICE>(callBackFn, 
		&(m_pD3D10DDI_ADAPTERFUNCS->pfnCreateDevice), 
		&(m_pD3D10_2DDI_ADAPTERFUNCS->pfnCreateDevice));
}
PFND3D10_2DDI_GETCAPS CProxyRef11::SetOADDI_GetCaps( PFND3D10_2DDI_GETCAPS callBackFn )
{
	return SetOADDIHelper<PFND3D10_2DDI_GETCAPS>(callBackFn, 
		NULL, 
		&(m_pD3D10_2DDI_ADAPTERFUNCS->pfnGetCaps));
}
PFND3D10_2DDI_GETSUPPORTEDVERSIONS CProxyRef11::SetOADDI_GetSupportedVersions( PFND3D10_2DDI_GETSUPPORTEDVERSIONS callBackFn )
{
	return SetOADDIHelper<PFND3D10_2DDI_GETSUPPORTEDVERSIONS>(callBackFn, 
		NULL, 
		&(m_pD3D10_2DDI_ADAPTERFUNCS->pfnGetSupportedVersions));
}

/* These functions use the macros defined above to simplify maintenance */
/* The parameters include: 
	The type (signature) for the pointer function
	The name of the function to hook the DDI (this is what a test app will call)
	The name of the entry in the device function table */
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETSHADERRESOURCES, SetDDI_PSSetShaderResources, pfnPsSetShaderResources);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETSHADER, SetDDI_PsSetShader, pfnPsSetShader);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETSAMPLERS, SetDDI_PSSetSamplers, pfnPsSetSamplers);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETSHADER, SetDDI_VsSetShader, pfnVsSetShader);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DRAWINDEXED, SetDDI_DrawIndexed, pfnDrawIndexed);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DRAW, SetDDI_Draw, pfnDraw);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEMAP, SetDDI_DynamicIABufferMapNoOverwrite, pfnDynamicIABufferMapNoOverwrite);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEUNMAP, SetDDI_DynamicIABufferUnmap, pfnDynamicIABufferUnmap);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEMAP, SetDDI_DynamicConstantBufferMapDiscard, pfnDynamicConstantBufferMapDiscard);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEMAP, SetDDI_DynamicIABufferMapDiscard, pfnDynamicIABufferMapDiscard);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEUNMAP, SetDDI_DynamicConstantBufferUnmap, pfnDynamicConstantBufferUnmap);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETINPUTLAYOUT, SetDDI_IaSetInputLayout, pfnIaSetInputLayout);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_IA_SETVERTEXBUFFERS, SetDDI_IaSetVertexBuffers, pfnIaSetVertexBuffers);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_IA_SETINDEXBUFFER, SetDDI_IaSetIndexBuffer, pfnIaSetIndexBuffer);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DRAWINDEXEDINSTANCED, SetDDI_DrawIndexedInstanced, pfnDrawIndexedInstanced);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DRAWINSTANCED, SetDDI_DrawInstanced, pfnDrawInstanced);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEMAP, SetDDI_DynamicResourceMapDiscard, pfnDynamicResourceMapDiscard);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEUNMAP, SetDDI_DynamicResourceUnmap, pfnDynamicResourceUnmap);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETSHADER, SetDDI_GsSetShader, pfnGsSetShader);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_IA_SETTOPOLOGY, SetDDI_IaSetTopology, pfnIaSetTopology);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEMAP, SetDDI_StagingResourceMap, pfnStagingResourceMap);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEUNMAP, SetDDI_StagingResourceUnmap, pfnStagingResourceUnmap);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETSHADERRESOURCES, SetDDI_VSSetShaderResources, pfnVsSetShaderResources);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETSAMPLERS, SetDDI_VSSetSamplers, pfnVsSetSamplers);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETSHADERRESOURCES, SetDDI_GSSetShaderResources, pfnGsSetShaderResources);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETSAMPLERS, SetDDI_GSSetSamplers, pfnGsSetSamplers);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SHADERRESOURCEVIEWREADAFTERWRITEHAZARD, SetDDI_ShaderResourceViewReadAfterWriteHazard, pfnShaderResourceViewReadAfterWriteHazard);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEREADAFTERWRITEHAZARD, SetDDI_ResourceReadAfterWriteHazard, pfnResourceReadAfterWriteHazard);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETBLENDSTATE, SetDDI_SetBlendState, pfnSetBlendState);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETDEPTHSTENCILSTATE, SetDDI_SetDepthStencilState, pfnSetDepthStencilState);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETRASTERIZERSTATE, SetDDI_SetRasterizerState, pfnSetRasterizerState);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_QUERYEND, SetDDI_QueryEnd, pfnQueryEnd);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_QUERYBEGIN, SetDDI_QueryBegin, pfnQueryBegin);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SO_SETTARGETS, SetDDI_SOSetTargets, pfnSoSetTargets);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DRAWAUTO, SetDDI_DrawAuto, pfnDrawAuto);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETVIEWPORTS, SetDDI_SetViewports, pfnSetViewports);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETSCISSORRECTS, SetDDI_SetScissorRects, pfnSetScissorRects);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CLEARRENDERTARGETVIEW, SetDDI_ClearRenderTargetView, pfnClearRenderTargetView);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CLEARDEPTHSTENCILVIEW, SetDDI_ClearDepthStencilView, pfnClearDepthStencilView);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETPREDICATION, SetDDI_SetPredication, pfnSetPredication);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_QUERYGETDATA, SetDDI_QueryGetData, pfnQueryGetData);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_GENMIPS, SetDDI_GenerateMips, pfnGenMips);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCECOPY, SetDDI_ResourceCopy, pfnResourceCopy);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCERESOLVESUBRESOURCE, SetDDI_ResourceResolveSubresource, pfnResourceResolveSubresource);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEMAP, SetDDI_ResourceMap, pfnResourceMap);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEUNMAP, SetDDI_ResourceUnmap, pfnResourceUnmap);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_RESOURCEISSTAGINGBUSY, SetDDI_ResourceIsStagingBusy, pfnResourceIsStagingBusy);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CALCPRIVATEOPENEDRESOURCESIZE, SetDDI_CalcPrivateOpenedResourceSize, pfnCalcPrivateOpenedResourceSize);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_OPENRESOURCE, SetDDI_OpenResource, pfnOpenResource);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYRESOURCE, SetDDI_DestroyResource, pfnDestroyResource);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYSHADERRESOURCEVIEW, SetDDI_DestroyShaderResourceView, pfnDestroyShaderResourceView);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CALCPRIVATERENDERTARGETVIEWSIZE, SetDDI_CalcPrivateRenderTargetViewSize, pfnCalcPrivateRenderTargetViewSize);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CREATERENDERTARGETVIEW, SetDDI_CreateRenderTargetView, pfnCreateRenderTargetView);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYRENDERTARGETVIEW, SetDDI_DestroyRenderTargetView, pfnDestroyRenderTargetView);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYDEPTHSTENCILVIEW, SetDDI_DestroyDepthStencilView, pfnDestroyDepthStencilView);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CALCPRIVATEELEMENTLAYOUTSIZE, SetDDI_CalcPrivateElementLayoutSize, pfnCalcPrivateElementLayoutSize);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CREATEELEMENTLAYOUT, SetDDI_CreateElementLayout, pfnCreateElementLayout);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYELEMENTLAYOUT, SetDDI_DestroyElementLayout, pfnDestroyElementLayout);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYBLENDSTATE, SetDDI_DestroyBlendState, pfnDestroyBlendState);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CALCPRIVATEDEPTHSTENCILSTATESIZE, SetDDI_CalcPrivateDepthStencilStateSize, pfnCalcPrivateDepthStencilStateSize);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CREATEDEPTHSTENCILSTATE, SetDDI_CreateDepthStencilState, pfnCreateDepthStencilState);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYDEPTHSTENCILSTATE, SetDDI_DestroyDepthStencilState, pfnDestroyDepthStencilState);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYRASTERIZERSTATE, SetDDI_DestroyRasterizerState, pfnDestroyRasterizerState);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYSHADER, SetDDI_DestroyShader, pfnDestroyShader);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CALCPRIVATESAMPLERSIZE, SetDDI_CalcPrivateSamplerSize, pfnCalcPrivateSamplerSize);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CREATESAMPLER, SetDDI_CreateSampler, pfnCreateSampler);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYSAMPLER, SetDDI_DestroySampler, pfnDestroySampler);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CALCPRIVATEQUERYSIZE, SetDDI_CalcPrivateQuerySize, pfnCalcPrivateQuerySize);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CREATEQUERY, SetDDI_CreateQuery, pfnCreateQuery);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYQUERY, SetDDI_DestroyQuery, pfnDestroyQuery);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CHECKFORMATSUPPORT, SetDDI_CheckFormatSupport, pfnCheckFormatSupport);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CHECKMULTISAMPLEQUALITYLEVELS, SetDDI_CheckMultisampleQualityLevels, pfnCheckMultisampleQualityLevels);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CHECKCOUNTERINFO, SetDDI_CheckCounterInfo, pfnCheckCounterInfo);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_CHECKCOUNTER, SetDDI_CheckCounter, pfnCheckCounter);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_DESTROYDEVICE, SetDDI_DestroyDevice, pfnDestroyDevice);
SETDDI_FUNC_ALLDEVFUNCTABLES(PFND3D10DDI_SETTEXTFILTERSIZE, SetDDI_SetTextFilter, pfnSetTextFilterSize);

//
// These functions have different signatures depending on the function table
//

// *SetConstantBuffers
SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_SETCONSTANTBUFFERS, SetDDI_VsSetConstantBuffers, pfnVsSetConstantBuffers);	// Only sets 10.0, 10.1, and 11.0 tables; others are NULL
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_SETCONSTANTBUFFERS, SetDDI_VsSetConstantBuffers11_1, pfnVsSetConstantBuffers);	// Only sets 11.1 tables; others are NULL

SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_SETCONSTANTBUFFERS, SetDDI_PsSetConstantBuffers, pfnPsSetConstantBuffers);	// Only sets 10.0, 10.1, and 11.0 tables; others are NULL
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_SETCONSTANTBUFFERS, SetDDI_PsSetConstantBuffers11_1, pfnPsSetConstantBuffers);	// Only sets 11.1 tables; others are NULL

SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_SETCONSTANTBUFFERS, SetDDI_GsSetConstantBuffers, pfnGsSetConstantBuffers);	// Only sets 10.0, 10.1, and 11.0 tables; others are NULL
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_SETCONSTANTBUFFERS, SetDDI_GsSetConstantBuffers11_1, pfnGsSetConstantBuffers);	// Only sets 11.1 tables; others are NULL

// SetRenderTargets
SETDDI_FUNC_10_XDEVFUNCTABLES(PFND3D10DDI_SETRENDERTARGETS, SetDDI_SetRenderTargets10, pfnSetRenderTargets);	// Only sets 10.0 and 10.1 tables; others are NULL
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_SETRENDERTARGETS, SetDDI_SetRenderTargets11, pfnSetRenderTargets);	// Only sets 11.0 and 11.1 tables; others are NULL

// ResourceUpdateSubresourceUP
SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP, SetDDI_ResourceUpdateSubresourceUP, pfnResourceUpdateSubresourceUP);	// Only sets 10.0, 10.1, and 11.0 tables; others are NULL
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_RESOURCEUPDATESUBRESOURCEUP, SetDDI_ResourceUpdateSubresourceUP11_1, pfnResourceUpdateSubresourceUP);	// Only sets 11.1 tables; others are NULL

// ConstantBufferUpdateSubresourceUP
SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP, SetDDI_ConstantBufferUpdateSubresourceUP, pfnDefaultConstantBufferUpdateSubresourceUP);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_RESOURCEUPDATESUBRESOURCEUP, SetDDI_ConstantBufferUpdateSubresourceUP11_1, pfnDefaultConstantBufferUpdateSubresourceUP);

// Flush
SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_FLUSH, SetDDI_Flush, pfnFlush);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_FLUSH, SetDDI_Flush11_1, pfnFlush);

// Relocate
PFND3D10DDI_RELOCATEDEVICEFUNCS CProxyRef11::SetDDI_RelocateDeviceFuncs10(PFND3D10DDI_RELOCATEDEVICEFUNCS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RELOCATEDEVICEFUNCS>(callBackFn, 
		&(m_pD3D10DDI_DEVFUNCS->pfnRelocateDeviceFuncs), 
		NULL, 
		NULL,
		NULL);
}

PFND3D10_1DDI_RELOCATEDEVICEFUNCS CProxyRef11::SetDDI_RelocateDeviceFuncs10_1(PFND3D10_1DDI_RELOCATEDEVICEFUNCS callBackFn)
{
	return SetDDIHelper<PFND3D10_1DDI_RELOCATEDEVICEFUNCS>(callBackFn, 
		NULL, 
		&(m_pD3D10_1DDI_DEVFUNCS->pfnRelocateDeviceFuncs), 
		NULL,
		NULL);
}

PFND3D11DDI_RELOCATEDEVICEFUNCS CProxyRef11::SetDDI_RelocateDeviceFuncs11(PFND3D11DDI_RELOCATEDEVICEFUNCS callBackFn)
{
	return SetDDIHelper<PFND3D11DDI_RELOCATEDEVICEFUNCS>(callBackFn, 
		NULL,
		NULL,
		&(m_pD3D11DDI_DEVFUNCS->pfnRelocateDeviceFuncs),
		NULL);
}

PFND3D11_1DDI_RELOCATEDEVICEFUNCS CProxyRef11::SetDDI_RelocateDeviceFuncs11_1(PFND3D11_1DDI_RELOCATEDEVICEFUNCS callBackFn)
{
	return SetDDIHelper<PFND3D11_1DDI_RELOCATEDEVICEFUNCS>(callBackFn, 
		NULL,
		NULL,
		NULL,
		&(m_pD3D11_1DDI_DEVFUNCS->pfnRelocateDeviceFuncs));
}

// CalcPrivateResourceSize
SETDDI_FUNC_10_XDEVFUNCTABLES(PFND3D10DDI_CALCPRIVATERESOURCESIZE, SetDDI_CalcPrivateResourceSize10, pfnCalcPrivateResourceSize);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CALCPRIVATERESOURCESIZE, SetDDI_CalcPrivateResourceSize11, pfnCalcPrivateResourceSize);

// CreateResource
SETDDI_FUNC_10_XDEVFUNCTABLES(PFND3D10DDI_CREATERESOURCE, SetDDI_CreateResource10, pfnCreateResource);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CREATERESOURCE, SetDDI_CreateResource11, pfnCreateResource);

// CalcPrivateShaderResourceViewSize
PFND3D10DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE CProxyRef11::SetDDI_CalcPrivateShaderResourceViewSize10(PFND3D10DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE>(callBackFn, 
		&(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateShaderResourceViewSize), 
		NULL, 
		NULL,
		NULL);
}

PFND3D10_1DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE CProxyRef11::SetDDI_CalcPrivateShaderResourceViewSize10_1(PFND3D10_1DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE callBackFn)
{
	return SetDDIHelper<PFND3D10_1DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE>(callBackFn, 
		NULL, 
		&(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateShaderResourceViewSize), 
		NULL,
		NULL);
}

SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE, SetDDI_CalcPrivateShaderResourceViewSize11, pfnCalcPrivateShaderResourceViewSize);

// CreateShaderResourceView
PFND3D10DDI_CREATESHADERRESOURCEVIEW CProxyRef11::SetDDI_CreateShaderResourceView10(PFND3D10DDI_CREATESHADERRESOURCEVIEW callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATESHADERRESOURCEVIEW>(callBackFn, 
		&(m_pD3D10DDI_DEVFUNCS->pfnCreateShaderResourceView), 
		NULL, 
		NULL,
		NULL);
}

PFND3D10_1DDI_CREATESHADERRESOURCEVIEW CProxyRef11::SetDDI_CreateShaderResourceView10_1(PFND3D10_1DDI_CREATESHADERRESOURCEVIEW callBackFn)
{
	return SetDDIHelper<PFND3D10_1DDI_CREATESHADERRESOURCEVIEW>(callBackFn, 
		NULL, 
		&(m_pD3D10_1DDI_DEVFUNCS->pfnCreateShaderResourceView), 
		NULL,
		NULL);
}

SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CREATESHADERRESOURCEVIEW, SetDDI_CreateShaderResourceView11, pfnCreateShaderResourceView);

// CalcPrivateDepthStencilViewSize
SETDDI_FUNC_10_XDEVFUNCTABLES(PFND3D10DDI_CALCPRIVATEDEPTHSTENCILVIEWSIZE, SetDDI_CalcPrivateDepthStencilViewSize, pfnCalcPrivateDepthStencilViewSize);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CALCPRIVATEDEPTHSTENCILVIEWSIZE, SetDDI_CalcPrivateDepthStencilViewSize11, pfnCalcPrivateDepthStencilViewSize);

// CreateDepthStencilView
SETDDI_FUNC_10_XDEVFUNCTABLES(PFND3D10DDI_CREATEDEPTHSTENCILVIEW, SetDDI_CreateDepthStencilView, pfnCreateDepthStencilView);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CREATEDEPTHSTENCILVIEW, SetDDI_CreateDepthStencilView11, pfnCreateDepthStencilView);

// CalcPrivateBlendStateSize
PFND3D10DDI_CALCPRIVATEBLENDSTATESIZE CProxyRef11::SetDDI_CalcPrivateBlendStateSize10(PFND3D10DDI_CALCPRIVATEBLENDSTATESIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATEBLENDSTATESIZE>(callBackFn, 
		&(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateBlendStateSize), 
		NULL, 
		NULL,
		NULL);
}

PFND3D10_1DDI_CALCPRIVATEBLENDSTATESIZE CProxyRef11::SetDDI_CalcPrivateBlendStateSize10_1(PFND3D10_1DDI_CALCPRIVATEBLENDSTATESIZE callBackFn)
{
	return SetDDIHelper<PFND3D10_1DDI_CALCPRIVATEBLENDSTATESIZE>(callBackFn, 
		NULL, 
		&(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateBlendStateSize), 
		&(m_pD3D11DDI_DEVFUNCS->pfnCalcPrivateBlendStateSize),
		NULL);
}

SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CALCPRIVATEBLENDSTATESIZE, SetDDI_CalcPrivateBlendStateSize11_1, pfnCalcPrivateBlendStateSize);

// CreateBlendState
PFND3D10DDI_CREATEBLENDSTATE CProxyRef11::SetDDI_CreateBlendState10(PFND3D10DDI_CREATEBLENDSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATEBLENDSTATE>(callBackFn, 
		&(m_pD3D10DDI_DEVFUNCS->pfnCreateBlendState), 
		NULL, 
		NULL,
		NULL);
}

PFND3D10_1DDI_CREATEBLENDSTATE CProxyRef11::SetDDI_CreateBlendState10_1(PFND3D10_1DDI_CREATEBLENDSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10_1DDI_CREATEBLENDSTATE>(callBackFn, 
		NULL, 
		&(m_pD3D10_1DDI_DEVFUNCS->pfnCreateBlendState), 
		&(m_pD3D11DDI_DEVFUNCS->pfnCreateBlendState),
		NULL);
}

SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CREATEBLENDSTATE, SetDDI_CreateBlendState11_1, pfnCreateBlendState);

// CalcPrivateShaderSize
SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_CALCPRIVATESHADERSIZE, SetDDI_CalcPrivateShaderSize, pfnCalcPrivateShaderSize);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CALCPRIVATESHADERSIZE, SetDDI_CalcPrivateShaderSize11_1, pfnCalcPrivateShaderSize);

// CreateVertexShader
SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_CREATEVERTEXSHADER, SetDDI_CreateVertexShader, pfnCreateVertexShader);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CREATEVERTEXSHADER, SetDDI_CreateVertexShader11_1, pfnCreateVertexShader);

// CreateHullShader
SETDDI_FUNC_11_0DEVFUNCTABLES(PFND3D11DDI_CREATEHULLSHADER, SetDDI_CreateHullShader, pfnCreateHullShader);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CREATEHULLSHADER, SetDDI_CreateHullShader11_1, pfnCreateHullShader);

// CreateDomainShader
SETDDI_FUNC_11_0DEVFUNCTABLES(PFND3D11DDI_CREATEDOMAINSHADER, SetDDI_CreateDomainShader, pfnCreateDomainShader);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CREATEDOMAINSHADER, SetDDI_CreateDomainShader11_1, pfnCreateDomainShader);

// CreateGeometryShader
SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_CREATEGEOMETRYSHADER, SetDDI_CreateGeometryShader, pfnCreateGeometryShader);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CREATEGEOMETRYSHADER, SetDDI_CreateGeometryShader11_1, pfnCreateGeometryShader);

// CreatePixelShader
SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_CREATEPIXELSHADER, SetDDI_CreatePixelShader, pfnCreatePixelShader);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CREATEPIXELSHADER, SetDDI_CreatePixelShader11_1, pfnCreatePixelShader);

// CalcPrivateGeometryShaderWithStreamOutputSize
SETDDI_FUNC_10_XDEVFUNCTABLES(PFND3D10DDI_CALCPRIVATEGEOMETRYSHADERWITHSTREAMOUTPUT, SetDDI_CalcPrivateGeometryShaderWithStreamOutputSize10, pfnCalcPrivateGeometryShaderWithStreamOutput);
SETDDI_FUNC_11_0DEVFUNCTABLES(PFND3D11DDI_CALCPRIVATEGEOMETRYSHADERWITHSTREAMOUTPUT, SetDDI_CalcPrivateGeometryShaderWithStreamOutputSize11, pfnCalcPrivateGeometryShaderWithStreamOutput);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CALCPRIVATEGEOMETRYSHADERWITHSTREAMOUTPUT, SetDDI_CalcPrivateGeometryShaderWithStreamOutputSize11_1, pfnCalcPrivateGeometryShaderWithStreamOutput);

// CreateGeometryShaderWithStreamOutput
SETDDI_FUNC_10_XDEVFUNCTABLES(PFND3D10DDI_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT, SetDDI_CreateGeometryShaderWithStreamOutput10, pfnCreateGeometryShaderWithStreamOutput);
SETDDI_FUNC_11_0DEVFUNCTABLES(PFND3D11DDI_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT, SetDDI_CreateGeometryShaderWithStreamOutput11, pfnCreateGeometryShaderWithStreamOutput);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT, SetDDI_CreateGeometryShaderWithStreamOutput11_1, pfnCreateGeometryShaderWithStreamOutput);

// CalcPrivateRasterizerStateSize
SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_CALCPRIVATERASTERIZERSTATESIZE, SetDDI_CalcPrivateRasterizerStateSize, pfnCalcPrivateRasterizerStateSize);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CALCPRIVATERASTERIZERSTATESIZE, SetDDI_CalcPrivateRasterizerStateSize11_1, pfnCalcPrivateRasterizerStateSize);

// CreateRasterizerState
SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_CREATERASTERIZERSTATE, SetDDI_CreateRasterizerState, pfnCreateRasterizerState);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CREATERASTERIZERSTATE, SetDDI_CreateRasterizerState11_1, pfnCreateRasterizerState);

// ResourceCopyRegion
SETDDI_FUNC_10_X_11_0DEVFUNCTABLES(PFND3D10DDI_RESOURCECOPYREGION, SetDDI_ResourceCopyRegion, pfnResourceCopyRegion);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_RESOURCECOPYREGION, SetDDI_ResourceCopyRegion11_1, pfnResourceCopyRegion);

// ResourceConvert
PFND3D10DDI_RESOURCECOPY CProxyRef11::SetDDI_ResourceConvert(PFND3D10DDI_RESOURCECOPY callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCECOPY>(callBackFn, 
		NULL, 
		&(m_pD3D10_1DDI_DEVFUNCS->pfnResourceConvert), 
		&(m_pD3D11DDI_DEVFUNCS->pfnResourceConvert),
		&(m_pD3D11_1DDI_DEVFUNCS->pfnResourceConvert));
}

// ResourceConvertRegion
PFND3D10DDI_RESOURCECOPYREGION CProxyRef11::SetDDI_ResourceConvertRegion(PFND3D10DDI_RESOURCECOPYREGION callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCECOPYREGION>(callBackFn, 
		NULL, 
		&(m_pD3D10_1DDI_DEVFUNCS->pfnResourceConvertRegion), 
		&(m_pD3D11DDI_DEVFUNCS->pfnResourceConvertRegion),
		NULL);
}
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_RESOURCECOPYREGION, SetDDI_ResourceConvertRegion11_1, pfnResourceConvertRegion);

#if !defined( REF10_LDK_BUILD )
#ifdef D3D10PSGP
PFND3D10DDI_RESETPRIMITIVEID CProxyRef11::SetDDI_IaResetPrimitiveID(PFND3D10DDI_RESETPRIMITIVEID callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESETPRIMITIVEID>(callBackFn, 
		&(m_pD3D10DDI_DEVFUNCS->pfnResetPrimitiveID), 
		&(m_pD3D10_1DDI_DEVFUNCS->pfnResetPrimitiveID), 
		&(m_pD3D11DDI_DEVFUNCS->pfnResetPrimitiveID),
		&(m_pD3D11_1DDI_DEVFUNCS->pfnResetPrimitiveID))
}

PFND3D10DDI_SETVERTEXPIPELINEOUTPUT CProxyRef11::SetDDI_IaSetVertexPipelineOutput(PFND3D10DDI_SETVERTEXPIPELINEOUTPUT callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETVERTEXPIPELINEOUTPUT>(callBackFn, 
		&(m_pD3D10DDI_DEVFUNCS->pfnSetVertexPipelineOutput), 
		&(m_pD3D10_1DDI_DEVFUNCS->pfnSetVertexPipelineOutput), 
		&(m_pD3D11DDI_DEVFUNCS->pfnSetVertexPipelineOutput),
		&(m_pD3D11_1DDI_DEVFUNCS->pfnSetVertexPipelineOutput));
}
#endif
#endif

SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_DRAWINDEXEDINSTANCEDINDIRECT, SetDDI_DrawIndexedInstancedIndirect, pfnDrawIndexedInstancedIndirect);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_DRAWINSTANCEDINDIRECT, SetDDI_DrawInstancedIndirect, pfnDrawInstancedIndirect);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_COMMANDLISTEXECUTE, SetDDI_CommandListExecute, pfnCommandListExecute);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CHECKDEFERREDCONTEXTHANDLESIZES, SetDDI_CheckDeferredContextHandleSizes, pfnCheckDeferredContextHandleSizes);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CALCDEFERREDCONTEXTHANDLESIZE, SetDDI_CalcDeferredContextHandleSize, pfnCalcDeferredContextHandleSize);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CALCPRIVATECOMMANDLISTSIZE, SetDDI_CalcPrivateCommandListSize, pfnCalcPrivateCommandListSize);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CREATECOMMANDLIST, SetDDI_CreateCommandList, pfnCreateCommandList);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_DESTROYCOMMANDLIST, SetDDI_DestroyCommandList, pfnDestroyCommandList);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D10DDI_SETSHADERRESOURCES, SetDDI_HsSetShaderResources, pfnHsSetShaderResources);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D10DDI_SETSHADER, SetDDI_HsSetShader, pfnHsSetShader);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D10DDI_SETSAMPLERS, SetDDI_HsSetSamplers, pfnHsSetSamplers);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D10DDI_SETSHADERRESOURCES, SetDDI_DsSetShaderResources, pfnDsSetShaderResources);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D10DDI_SETSHADER, SetDDI_DsSetShader, pfnDsSetShader);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D10DDI_SETSAMPLERS, SetDDI_DsSetSamplers, pfnDsSetSamplers);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_SETSHADER_WITH_IFACES, SetDDI_PsSetShaderWithIfaces, pfnPsSetShaderWithIfaces);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_SETSHADER_WITH_IFACES, SetDDI_VsSetShaderWithIfaces, pfnVsSetShaderWithIfaces);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_SETSHADER_WITH_IFACES, SetDDI_GsSetShaderWithIfaces, pfnGsSetShaderWithIfaces);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_SETSHADER_WITH_IFACES, SetDDI_HsSetShaderWithIfaces, pfnHsSetShaderWithIfaces);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_SETSHADER_WITH_IFACES, SetDDI_DsSetShaderWithIfaces, pfnDsSetShaderWithIfaces);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_SETSHADER_WITH_IFACES, SetDDI_CsSetShaderWithIfaces, pfnCsSetShaderWithIfaces);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CREATECOMPUTESHADER, SetDDI_CreateComputeShader, pfnCreateComputeShader);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D10DDI_SETSHADERRESOURCES, SetDDI_CsSetShaderResources, pfnCsSetShaderResources);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D10DDI_SETSHADER, SetDDI_CsSetShader, pfnCsSetShader);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D10DDI_SETSAMPLERS, SetDDI_CsSetSamplers, pfnCsSetSamplers);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CALCPRIVATEUNORDEREDACCESSVIEWSIZE, SetDDI_CalcPrivateUnorderedAccessViewSize, pfnCalcPrivateUnorderedAccessViewSize);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CREATEUNORDEREDACCESSVIEW, SetDDI_CreateUnorderedAccessView, pfnCreateUnorderedAccessView);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_DESTROYUNORDEREDACCESSVIEW, SetDDI_DestroyUnorderedAccessView, pfnDestroyUnorderedAccessView);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CLEARUNORDEREDACCESSVIEWUINT, SetDDI_ClearUnorderedAccessViewUint, pfnClearUnorderedAccessViewUint);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_CLEARUNORDEREDACCESSVIEWFLOAT, SetDDI_ClearUnorderedAccessViewFloat, pfnClearUnorderedAccessViewFloat);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_SETUNORDEREDACCESSVIEWS, SetDDI_CsSetUnorderedAccessViews, pfnCsSetUnorderedAccessViews);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_DISPATCH, SetDDI_Dispatch, pfnDispatch);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_DISPATCHINDIRECT, SetDDI_DispatchIndirect, pfnDispatchIndirect);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_SETRESOURCEMINLOD, SetDDI_SetResourceMinLOD, pfnSetResourceMinLOD);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_COPYSTRUCTURECOUNT, SetDDI_CopyStructureCount, pfnCopyStructureCount);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_RECYCLECOMMANDLIST, SetDDI_RecycleCommandList, pfnRecycleCommandList);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_RECYCLECREATECOMMANDLIST, SetDDI_RecycleCreateCommandList, pfnRecycleCreateCommandList);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_RECYCLECREATEDEFERREDCONTEXT, SetDDI_RecycleCreateDeferredContext, pfnRecycleCreateDeferredContext);
SETDDI_FUNC_11_XDEVFUNCTABLES(PFND3D11DDI_DESTROYCOMMANDLIST, SetDDI_RecycleDestroyCommandList, pfnRecycleDestroyCommandList);

SETDDI_FUNC_11_0DEVFUNCTABLES(PFND3D10DDI_SETCONSTANTBUFFERS, SetDDI_HsSetConstantBuffers, pfnHsSetConstantBuffers);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_SETCONSTANTBUFFERS, SetDDI_HsSetConstantBuffers11_1, pfnHsSetConstantBuffers);

SETDDI_FUNC_11_0DEVFUNCTABLES(PFND3D10DDI_SETCONSTANTBUFFERS, SetDDI_DsSetConstantBuffers, pfnDsSetConstantBuffers);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_SETCONSTANTBUFFERS, SetDDI_DsSetConstantBuffers11_1, pfnDsSetConstantBuffers);

SETDDI_FUNC_11_0DEVFUNCTABLES(PFND3D10DDI_SETCONSTANTBUFFERS, SetDDI_CsSetConstantBuffers, pfnCsSetConstantBuffers);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_SETCONSTANTBUFFERS, SetDDI_CsSetConstantBuffers11_1, pfnCsSetConstantBuffers);

SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_DISCARD, SetDDI_Discard, pfnDiscard);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_ASSIGNDEBUGBINARY, SetDDI_AssignDebugBinary, pfnAssignDebugBinary);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D10DDI_RESOURCEMAP, SetDDI_DynamicConstantBufferMapNoOverwrite, pfnDynamicConstantBufferMapNoOverwrite);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CHECKDIRECTFLIPSUPPORT, SetDDI_CheckDirectFlipSupport, pfnCheckDirectFlipSupport);
SETDDI_FUNC_11_1DEVFUNCTABLES(PFND3D11_1DDI_CLEARVIEW, SetDDI_ClearView, pfnClearView);

#define MAKE_PROXYREF_DXGI_CALLBACK_METHOD(funcPtrType, funcName) \
funcPtrType CProxyRef11::SetDXGIDDI_##funcName(funcPtrType callBackFn)\
{ return SetDXGIDDIHelper<funcPtrType>(callBackFn, &(m_pDXGIDDI_BASECALLBACKS->pfn##funcName));}\

MAKE_PROXYREF_DXGI_CALLBACK_METHOD(PFNDDXGIDDI_PRESENTCB,PresentCb)


#define MAKE_PROXYREF_DXGI_METHOD(funcPtrType, funcName) \
funcPtrType CProxyRef11::SetDXGIDDI_##funcName(funcPtrType callBackFn)\
{ return SetDXGIDDIHelper<funcPtrType>(callBackFn, &(m_pDXGI11DDI_BASEFUNCS->pfn##funcName));}\

MAKE_PROXYREF_DXGI_METHOD(PFNDXGIDDI_PRESENT,Present)
MAKE_PROXYREF_DXGI_METHOD(PFNDXGIDDI_GETGAMMACAPS,GetGammaCaps)
MAKE_PROXYREF_DXGI_METHOD(PFNDXGIDDI_SETDISPLAYMODE,SetDisplayMode)
MAKE_PROXYREF_DXGI_METHOD(PFNDXGIDDI_SETRESOURCEPRIORITY,SetResourcePriority)
MAKE_PROXYREF_DXGI_METHOD(PFNDXGIDDI_QUERYRESOURCERESIDENCY,QueryResourceResidency)
MAKE_PROXYREF_DXGI_METHOD(PFNDXGIDDI_ROTATERESOURCEIDENTITIES,RotateResourceIdentities)
MAKE_PROXYREF_DXGI_METHOD(PFNDXGIDDI_BLT,Blt)
MAKE_PROXYREF_DXGI_METHOD(PFNDXGIDDI_RESOLVESHAREDRESOURCE,ResolveSharedResource)

#define MAKE_PROXYREF_DXGI_KMT_METHOD(funcPtrType, funcName) \
funcPtrType CProxyRef11::SetDXGIKMT_##funcName(funcPtrType callBackFn)\
{ return SetDXGIDDIHelper<funcPtrType>(callBackFn, &(m_pDXGI_KMTHUNKS->m_pfnOsThunkDDI##funcName));}\

MAKE_PROXYREF_DXGI_KMT_METHOD(PFND3DKMT_OPENADAPTERFROMGDIDISPLAYNAME,OpenAdapterFromGdiDisplayName)
MAKE_PROXYREF_DXGI_KMT_METHOD(PFND3DKMT_OPENADAPTERFROMDEVICENAME,OpenAdapterFromDeviceName)
MAKE_PROXYREF_DXGI_KMT_METHOD(PFND3DKMT_GETDISPLAYMODELIST,GetDisplayModeList)
MAKE_PROXYREF_DXGI_KMT_METHOD(PFND3DKMT_SETVIDPNSOURCEOWNER,SetVIDPnSourceOwner)
MAKE_PROXYREF_DXGI_KMT_METHOD(PFND3DKMT_SETDISPLAYMODE,SetDisplayMode)
MAKE_PROXYREF_DXGI_KMT_METHOD(PFND3DKMT_CLOSEADAPTER,CloseAdapter)
MAKE_PROXYREF_DXGI_KMT_METHOD(PFND3DKMT_SETGAMMARAMP,SetGammaRamp)
MAKE_PROXYREF_DXGI_KMT_METHOD(PFND3DKMT_GETDEVICESTATE,GetDeviceState)
MAKE_PROXYREF_DXGI_KMT_METHOD(PFND3DKMT_QUERYADAPTERINFO,QueryAdapterInfo)
MAKE_PROXYREF_DXGI_KMT_METHOD(PFND3DKMT_WAITFORVERTICALBLANKEVENT,WaitForVerticalBlankEvent)

