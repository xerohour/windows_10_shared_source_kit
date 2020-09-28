// Includes
#include "ProxyRef10.h"

// Defines
#define	D3D10_PROXYDRIVER_DLLNAME	"D3D10ProxyDriver.dll"
#define D3D10_DEVICE_10_1NULLREF	"d3d10_1core.dll"
#define D3D10_DEVICE_10_0NULLREF	"d3d10core.dll"

////////////////////////

CProxyRef10::CProxyRef10( ) :	m_hD3D10ProxyDLL( NULL ), 
								m_pD3D10_1DDI_DEVFUNCS( NULL ), 
								m_pD3D10DDI_DEVFUNCS( NULL ),
								m_proxySetup( PROXY_SETUP_OK )
{
}

/////

CProxyRef10::~CProxyRef10(void)
{
}

/////

PROXY_SETUP CProxyRef10::Setup( tstring d3d10DeviceDLL, PD_CREATE_DEVICE_FP* createDeviceCallback )
{
	// Load the Proxy Driver DLL and set its device dll
	m_proxySetup = LoadProxyDriverDLL( d3d10DeviceDLL );

	// Check to see if the CreateDevice should be hooked
	if(createDeviceCallback)
		SetCreateDeviceCallback(createDeviceCallback);

	// Return the status of the proxy driver initialization process
	return m_proxySetup;
}

/////

PROXY_SETUP CProxyRef10::LoadProxyDriverDLL( tstring d3d10DeviceDLL )
{
	// Check to see if null ref should be hooked
	if(d3d10DeviceDLL == "")
		d3d10DeviceDLL = D3D10_DEVICE_10_1NULLREF;

	// Load the proxy driver dll
	m_hD3D10ProxyDLL = LoadLibrary( D3D10_PROXYDRIVER_DLLNAME );

	// Check the proxy driver dll was loaded
	if( !m_hD3D10ProxyDLL )
		return PROXY_SETUP_PROXY_DLL_FAILED;

	// Capture the dll name and the device version
	PD_GetDeviceDLL = (PD_GET_DEVICE_DLL_FP)GetProcAddress(m_hD3D10ProxyDLL, "PD_GetDeviceDLL");
	PD_GetDeviceVersion = (PD_GET_DEVICE_VERSION_FP)GetProcAddress(m_hD3D10ProxyDLL, "PD_GetDeviceVersion");

	// Set the Proxy driver's device dll, for example: "d3d10ref.DLL"
	PD_SET_DEVICE_DLL_FP PD_SpecifyDeviceDLL = (PD_SET_DEVICE_DLL_FP)GetProcAddress(m_hD3D10ProxyDLL, "PD_SetDeviceDLL");
	PD_SpecifyDeviceDLL( d3d10DeviceDLL.c_str() );

	// Capture the dll name (Global variable in CD3D10ProxyDriver)
	PD_GetDeviceDLL(m_dllName);

	// Check the device dll was set in the proxy driver 
	if( !m_dllName )
		return PROXY_SETUP_DEVICE_DLL_FAILED;

	// Successfully loaded the proxy driver dll and set its device driver dll
	return PROXY_SETUP_OK;
}

/////

void CProxyRef10::SetCreateDeviceCallback(PD_CREATE_DEVICE_FP* callBack)
{
	// Setup create device callback
	PD_REGISTER_CREATE_DEVICE_FP PD_RegisterCreateDeviceCallback = (PD_REGISTER_CREATE_DEVICE_FP)GetProcAddress(m_hD3D10ProxyDLL, "PD_RegisterCreateDeviceCallback");
	PD_RegisterCreateDeviceCallback(callBack);
}

/////

PROXY_LISTENERS CProxyRef10::CaptureDriverListenerFunctions()
{
	// Register Device functions
	PD_REGISTER_DRIVER_FUNCS_10_1_FP PD_RegisterDriverListenerFunctions10_1 = (PD_REGISTER_DRIVER_FUNCS_10_1_FP)GetProcAddress(m_hD3D10ProxyDLL, "PD_RegisterDriverListenerFunctions10_1");
	PD_REGISTER_DRIVER_FUNCS_10_0_FP PD_RegisterDriverListenerFunctions10_0 = (PD_REGISTER_DRIVER_FUNCS_10_0_FP)GetProcAddress(m_hD3D10ProxyDLL, "PD_RegisterDriverListenerFunctions10_0");

	// Capture the driver listener functions
	m_pD3D10_1DDI_DEVFUNCS = PD_RegisterDriverListenerFunctions10_1();
	m_pD3D10DDI_DEVFUNCS = PD_RegisterDriverListenerFunctions10_0();
	
	// Check to see if the device functions were loaded
	if( !m_pD3D10_1DDI_DEVFUNCS )
		return PROXY_LISTENERS_10_1DDI_DEVFUNCTS_FAILED;

	if( !m_pD3D10DDI_DEVFUNCS )
		return PROXY_LISTENERS_10_DDI_DEVFUNCTS_FAILED;

	// Successfully caqptured the driver listener functions
	return PROXY_LISTENERS_OK;
}

/////

HRESULT CProxyRef10::CreateDevice1(ID3D10Device1** ppDevice, D3D10_FEATURE_LEVEL1 fl, UINT createDeviceFlags)
{
	// Local variables
	HRESULT hr;

	// Create the device
	hr = D3D10CreateDevice1( NULL, D3D10_DRIVER_TYPE_SOFTWARE, m_hD3D10ProxyDLL, createDeviceFlags, 
                                        fl, D3D10_1_SDK_VERSION, ppDevice );
	if(FAILED(hr))
		return hr;

	// Capture the device listener functions
	if(CaptureDriverListenerFunctions() != PROXY_LISTENERS_OK)
		return E_FAIL;

	return S_OK;
}

/////

HRESULT CProxyRef10::CreateDevice(ID3D10Device** ppDevice, UINT createDeviceFlags)
{
	// Local variables
	HRESULT hr;

	// Create the device
	hr = D3D10CreateDevice( NULL, D3D10_DRIVER_TYPE_SOFTWARE, m_hD3D10ProxyDLL, createDeviceFlags, D3D10_SDK_VERSION, ppDevice );

	if(FAILED(hr))
		return hr;

	// Capture the device listener functions
	if(CaptureDriverListenerFunctions() != PROXY_LISTENERS_OK)
		return E_FAIL;

	return S_OK;
}

/////

HMODULE	CProxyRef10::GetProxyDriverHandle()
{
	return m_hD3D10ProxyDLL;
}

/////

template <typename _T> _T CProxyRef10::SetDDIHelper(_T callBackFn, _T* pfn10, _T* pfn10_1)
{
	// Check the device version
	m_deviceVersion = PD_GetDeviceVersion();

	if(m_deviceVersion == 0)
		return NULL;

	// Create a temp variable for storing the current value
	_T p_oldPfn = NULL;

	if (m_deviceVersion == D3D10_1_DDI_INTERFACE_VERSION)
	{
		// Record the previous value
		p_oldPfn = *pfn10_1;

		// Set the new callBack
		*pfn10_1 = callBackFn;
	}
	else
	{
		// Record the previous value
		p_oldPfn = *pfn10;

		// Set the new callBack
		*pfn10 = callBackFn;
	}

	// Return the previous value of the function pointer
	return p_oldPfn;
}

////////////////////////
/* Below here are all the definitions for the SetDDI functions */
/* They utilize the SetDDIHelper function above */

PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP CProxyRef10::SetDDI_ConstantBufferUpdateSubresourceUP(PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDefaultConstantBufferUpdateSubresourceUP), &(m_pD3D10_1DDI_DEVFUNCS->pfnDefaultConstantBufferUpdateSubresourceUP));
}

PFND3D10DDI_SETCONSTANTBUFFERS CProxyRef10::SetDDI_VsSetConstantBuffers(PFND3D10DDI_SETCONSTANTBUFFERS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETCONSTANTBUFFERS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnVsSetConstantBuffers), &(m_pD3D10_1DDI_DEVFUNCS->pfnVsSetConstantBuffers));
}

PFND3D10DDI_SETSHADERRESOURCES CProxyRef10::SetDDI_PSSetShaderResources(PFND3D10DDI_SETSHADERRESOURCES callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETSHADERRESOURCES>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnPsSetShaderResources), &(m_pD3D10_1DDI_DEVFUNCS->pfnPsSetShaderResources));
}

PFND3D10DDI_SETSHADER CProxyRef10::SetDDI_PsSetShader(PFND3D10DDI_SETSHADER callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETSHADER>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnPsSetShader), &(m_pD3D10_1DDI_DEVFUNCS->pfnPsSetShader));
}

PFND3D10DDI_SETSAMPLERS CProxyRef10::SetDDI_PSSetSamplers(PFND3D10DDI_SETSAMPLERS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETSAMPLERS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnPsSetSamplers), &(m_pD3D10_1DDI_DEVFUNCS->pfnPsSetSamplers));
}

PFND3D10DDI_SETSHADER CProxyRef10::SetDDI_VsSetShader(PFND3D10DDI_SETSHADER callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETSHADER>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnVsSetShader), &(m_pD3D10_1DDI_DEVFUNCS->pfnVsSetShader));
}

PFND3D10DDI_DRAWINDEXED CProxyRef10::SetDDI_DrawIndexed(PFND3D10DDI_DRAWINDEXED callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DRAWINDEXED>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDrawIndexed), &(m_pD3D10_1DDI_DEVFUNCS->pfnDrawIndexed));
}

PFND3D10DDI_DRAW CProxyRef10::SetDDI_Draw(PFND3D10DDI_DRAW callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DRAW>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDraw), &(m_pD3D10_1DDI_DEVFUNCS->pfnDraw));
}

PFND3D10DDI_RESOURCEMAP CProxyRef10::SetDDI_DynamicIABufferMapNoOverwrite(PFND3D10DDI_RESOURCEMAP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEMAP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDynamicIABufferMapNoOverwrite), &(m_pD3D10_1DDI_DEVFUNCS->pfnDynamicIABufferMapNoOverwrite));
}

PFND3D10DDI_RESOURCEUNMAP CProxyRef10::SetDDI_DynamicIABufferUnmap(PFND3D10DDI_RESOURCEUNMAP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEUNMAP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDynamicIABufferUnmap), &(m_pD3D10_1DDI_DEVFUNCS->pfnDynamicIABufferUnmap));
}

PFND3D10DDI_RESOURCEMAP CProxyRef10::SetDDI_DynamicConstantBufferMapDiscard(PFND3D10DDI_RESOURCEMAP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEMAP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDynamicConstantBufferMapDiscard), &(m_pD3D10_1DDI_DEVFUNCS->pfnDynamicConstantBufferMapDiscard));
}

PFND3D10DDI_RESOURCEMAP CProxyRef10::SetDDI_DynamicIABufferMapDiscard(PFND3D10DDI_RESOURCEMAP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEMAP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDynamicIABufferMapDiscard), &(m_pD3D10_1DDI_DEVFUNCS->pfnDynamicIABufferMapDiscard));
}

PFND3D10DDI_RESOURCEUNMAP CProxyRef10::SetDDI_DynamicConstantBufferUnmap(PFND3D10DDI_RESOURCEUNMAP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEUNMAP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDynamicConstantBufferUnmap), &(m_pD3D10_1DDI_DEVFUNCS->pfnDynamicConstantBufferUnmap));
}

PFND3D10DDI_SETCONSTANTBUFFERS CProxyRef10::SetDDI_PsSetConstantBuffers(PFND3D10DDI_SETCONSTANTBUFFERS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETCONSTANTBUFFERS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnPsSetConstantBuffers), &(m_pD3D10_1DDI_DEVFUNCS->pfnPsSetConstantBuffers));
}

PFND3D10DDI_SETINPUTLAYOUT CProxyRef10::SetDDI_IaSetInputLayout(PFND3D10DDI_SETINPUTLAYOUT callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETINPUTLAYOUT>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnIaSetInputLayout), &(m_pD3D10_1DDI_DEVFUNCS->pfnIaSetInputLayout));
}

PFND3D10DDI_IA_SETVERTEXBUFFERS CProxyRef10::SetDDI_IaSetVertexBuffers(PFND3D10DDI_IA_SETVERTEXBUFFERS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_IA_SETVERTEXBUFFERS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnIaSetVertexBuffers), &(m_pD3D10_1DDI_DEVFUNCS->pfnIaSetVertexBuffers));
}

PFND3D10DDI_IA_SETINDEXBUFFER CProxyRef10::SetDDI_IaSetIndexBuffer(PFND3D10DDI_IA_SETINDEXBUFFER callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_IA_SETINDEXBUFFER>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnIaSetIndexBuffer), &(m_pD3D10_1DDI_DEVFUNCS->pfnIaSetIndexBuffer));
}

PFND3D10DDI_DRAWINDEXEDINSTANCED CProxyRef10::SetDDI_DrawIndexedInstanced(PFND3D10DDI_DRAWINDEXEDINSTANCED callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DRAWINDEXEDINSTANCED>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDrawIndexedInstanced), &(m_pD3D10_1DDI_DEVFUNCS->pfnDrawIndexedInstanced));
}

PFND3D10DDI_DRAWINSTANCED CProxyRef10::SetDDI_DrawInstanced(PFND3D10DDI_DRAWINSTANCED callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DRAWINSTANCED>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDrawInstanced), &(m_pD3D10_1DDI_DEVFUNCS->pfnDrawInstanced));
}

PFND3D10DDI_RESOURCEMAP CProxyRef10::SetDDI_DynamicResourceMapDiscard(PFND3D10DDI_RESOURCEMAP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEMAP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDynamicResourceMapDiscard), &(m_pD3D10_1DDI_DEVFUNCS->pfnDynamicResourceMapDiscard));
}

PFND3D10DDI_RESOURCEUNMAP CProxyRef10::SetDDI_DynamicResourceUnmap(PFND3D10DDI_RESOURCEUNMAP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEUNMAP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDynamicResourceUnmap), &(m_pD3D10_1DDI_DEVFUNCS->pfnDynamicResourceUnmap));
}

PFND3D10DDI_SETCONSTANTBUFFERS CProxyRef10::SetDDI_GsSetConstantBuffers(PFND3D10DDI_SETCONSTANTBUFFERS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETCONSTANTBUFFERS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnGsSetConstantBuffers), &(m_pD3D10_1DDI_DEVFUNCS->pfnGsSetConstantBuffers));
}

PFND3D10DDI_SETSHADER CProxyRef10::SetDDI_GsSetShader(PFND3D10DDI_SETSHADER callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETSHADER>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnGsSetShader), &(m_pD3D10_1DDI_DEVFUNCS->pfnGsSetShader));
}

PFND3D10DDI_IA_SETTOPOLOGY CProxyRef10::SetDDI_IaSetTopology(PFND3D10DDI_IA_SETTOPOLOGY callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_IA_SETTOPOLOGY>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnIaSetTopology), &(m_pD3D10_1DDI_DEVFUNCS->pfnIaSetTopology));
}

PFND3D10DDI_RESOURCEMAP CProxyRef10::SetDDI_StagingResourceMap(PFND3D10DDI_RESOURCEMAP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEMAP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnStagingResourceMap), &(m_pD3D10_1DDI_DEVFUNCS->pfnStagingResourceMap));
}

PFND3D10DDI_RESOURCEUNMAP CProxyRef10::SetDDI_StagingResourceUnmap(PFND3D10DDI_RESOURCEUNMAP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEUNMAP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnStagingResourceUnmap), &(m_pD3D10_1DDI_DEVFUNCS->pfnStagingResourceUnmap));
}

PFND3D10DDI_SETSHADERRESOURCES CProxyRef10::SetDDI_VSSetShaderResources(PFND3D10DDI_SETSHADERRESOURCES callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETSHADERRESOURCES>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnVsSetShaderResources), &(m_pD3D10_1DDI_DEVFUNCS->pfnVsSetShaderResources));
}

PFND3D10DDI_SETSAMPLERS CProxyRef10::SetDDI_VSSetSamplers(PFND3D10DDI_SETSAMPLERS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETSAMPLERS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnVsSetSamplers), &(m_pD3D10_1DDI_DEVFUNCS->pfnVsSetSamplers));
}

PFND3D10DDI_SETSHADERRESOURCES CProxyRef10::SetDDI_GSSetShaderResources(PFND3D10DDI_SETSHADERRESOURCES callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETSHADERRESOURCES>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnGsSetShaderResources), &(m_pD3D10_1DDI_DEVFUNCS->pfnGsSetShaderResources));
}

PFND3D10DDI_SETSAMPLERS CProxyRef10::SetDDI_GSSetSamplers(PFND3D10DDI_SETSAMPLERS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETSAMPLERS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnGsSetSamplers), &(m_pD3D10_1DDI_DEVFUNCS->pfnGsSetSamplers));
}

PFND3D10DDI_SETRENDERTARGETS CProxyRef10::SetDDI_SetRenderTargets(PFND3D10DDI_SETRENDERTARGETS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETRENDERTARGETS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnSetRenderTargets), &(m_pD3D10_1DDI_DEVFUNCS->pfnSetRenderTargets));
}

PFND3D10DDI_SHADERRESOURCEVIEWREADAFTERWRITEHAZARD CProxyRef10::SetDDI_ShaderResourceViewReadAfterWriteHazard(PFND3D10DDI_SHADERRESOURCEVIEWREADAFTERWRITEHAZARD callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SHADERRESOURCEVIEWREADAFTERWRITEHAZARD>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnShaderResourceViewReadAfterWriteHazard), &(m_pD3D10_1DDI_DEVFUNCS->pfnShaderResourceViewReadAfterWriteHazard));
}

PFND3D10DDI_RESOURCEREADAFTERWRITEHAZARD CProxyRef10::SetDDI_ResourceReadAfterWriteHazard(PFND3D10DDI_RESOURCEREADAFTERWRITEHAZARD callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEREADAFTERWRITEHAZARD>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnResourceReadAfterWriteHazard), &(m_pD3D10_1DDI_DEVFUNCS->pfnResourceReadAfterWriteHazard));
}

PFND3D10DDI_SETBLENDSTATE CProxyRef10::SetDDI_SetBlendState(PFND3D10DDI_SETBLENDSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETBLENDSTATE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnSetBlendState), &(m_pD3D10_1DDI_DEVFUNCS->pfnSetBlendState));
}

PFND3D10DDI_SETDEPTHSTENCILSTATE CProxyRef10::SetDDI_SetDepthStencilState(PFND3D10DDI_SETDEPTHSTENCILSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETDEPTHSTENCILSTATE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnSetDepthStencilState), &(m_pD3D10_1DDI_DEVFUNCS->pfnSetDepthStencilState));
}

PFND3D10DDI_SETRASTERIZERSTATE CProxyRef10::SetDDI_SetRasterizerState(PFND3D10DDI_SETRASTERIZERSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETRASTERIZERSTATE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnSetRasterizerState), &(m_pD3D10_1DDI_DEVFUNCS->pfnSetRasterizerState));
}

PFND3D10DDI_QUERYEND CProxyRef10::SetDDI_QueryEnd(PFND3D10DDI_QUERYEND callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_QUERYEND>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnQueryEnd), &(m_pD3D10_1DDI_DEVFUNCS->pfnQueryEnd));
}

PFND3D10DDI_QUERYBEGIN CProxyRef10::SetDDI_QueryBegin(PFND3D10DDI_QUERYBEGIN callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_QUERYBEGIN>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnQueryBegin), &(m_pD3D10_1DDI_DEVFUNCS->pfnQueryBegin));
}

PFND3D10DDI_RESOURCECOPYREGION CProxyRef10::SetDDI_ResourceCopyRegion(PFND3D10DDI_RESOURCECOPYREGION callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCECOPYREGION>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnResourceCopyRegion), &(m_pD3D10_1DDI_DEVFUNCS->pfnResourceCopyRegion));
}

PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP CProxyRef10::SetDDI_ResourceUpdateSubresourceUP(PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEUPDATESUBRESOURCEUP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnResourceUpdateSubresourceUP), &(m_pD3D10_1DDI_DEVFUNCS->pfnResourceUpdateSubresourceUP));
}

PFND3D10DDI_SO_SETTARGETS CProxyRef10::SetDDI_SOSetTargets(PFND3D10DDI_SO_SETTARGETS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SO_SETTARGETS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnSoSetTargets), &(m_pD3D10_1DDI_DEVFUNCS->pfnSoSetTargets));
}

PFND3D10DDI_DRAWAUTO CProxyRef10::SetDDI_DrawAuto(PFND3D10DDI_DRAWAUTO callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DRAWAUTO>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDrawAuto), &(m_pD3D10_1DDI_DEVFUNCS->pfnDrawAuto));
}

PFND3D10DDI_SETVIEWPORTS CProxyRef10::SetDDI_SetViewports(PFND3D10DDI_SETVIEWPORTS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETVIEWPORTS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnSetViewports), &(m_pD3D10_1DDI_DEVFUNCS->pfnSetViewports));
}

PFND3D10DDI_SETSCISSORRECTS CProxyRef10::SetDDI_SetScissorRects(PFND3D10DDI_SETSCISSORRECTS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETSCISSORRECTS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnSetScissorRects), &(m_pD3D10_1DDI_DEVFUNCS->pfnSetScissorRects));
}

PFND3D10DDI_CLEARRENDERTARGETVIEW CProxyRef10::SetDDI_ClearRenderTargetView(PFND3D10DDI_CLEARRENDERTARGETVIEW callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CLEARRENDERTARGETVIEW>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnClearRenderTargetView), &(m_pD3D10_1DDI_DEVFUNCS->pfnClearRenderTargetView));
}

PFND3D10DDI_CLEARDEPTHSTENCILVIEW CProxyRef10::SetDDI_ClearDepthStencilView(PFND3D10DDI_CLEARDEPTHSTENCILVIEW callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CLEARDEPTHSTENCILVIEW>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnClearDepthStencilView), &(m_pD3D10_1DDI_DEVFUNCS->pfnClearDepthStencilView));
}

PFND3D10DDI_SETPREDICATION CProxyRef10::SetDDI_SetPredication(PFND3D10DDI_SETPREDICATION callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETPREDICATION>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnSetPredication), &(m_pD3D10_1DDI_DEVFUNCS->pfnSetPredication));
}

PFND3D10DDI_QUERYGETDATA CProxyRef10::SetDDI_QueryGetData(PFND3D10DDI_QUERYGETDATA callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_QUERYGETDATA>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnQueryGetData), &(m_pD3D10_1DDI_DEVFUNCS->pfnQueryGetData));
}

PFND3D10DDI_FLUSH CProxyRef10::SetDDI_Flush(PFND3D10DDI_FLUSH callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_FLUSH>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnFlush), &(m_pD3D10_1DDI_DEVFUNCS->pfnFlush));
}

PFND3D10DDI_GENMIPS CProxyRef10::SetDDI_GenerateMips(PFND3D10DDI_GENMIPS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_GENMIPS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnGenMips), &(m_pD3D10_1DDI_DEVFUNCS->pfnGenMips));
}

PFND3D10DDI_RESOURCECOPY CProxyRef10::SetDDI_ResourceCopy(PFND3D10DDI_RESOURCECOPY callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCECOPY>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnResourceCopy), &(m_pD3D10_1DDI_DEVFUNCS->pfnResourceCopy));
}

PFND3D10DDI_RESOURCERESOLVESUBRESOURCE CProxyRef10::SetDDI_ResourceResolveSubresource(PFND3D10DDI_RESOURCERESOLVESUBRESOURCE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCERESOLVESUBRESOURCE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnResourceResolveSubresource), &(m_pD3D10_1DDI_DEVFUNCS->pfnResourceResolveSubresource));
}

PFND3D10DDI_RESOURCEMAP CProxyRef10::SetDDI_ResourceMap(PFND3D10DDI_RESOURCEMAP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEMAP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnResourceMap), &(m_pD3D10_1DDI_DEVFUNCS->pfnResourceMap));
}

PFND3D10DDI_RESOURCEUNMAP CProxyRef10::SetDDI_ResourceUnmap(PFND3D10DDI_RESOURCEUNMAP callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEUNMAP>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnResourceUnmap), &(m_pD3D10_1DDI_DEVFUNCS->pfnResourceUnmap));
}

PFND3D10DDI_RESOURCEISSTAGINGBUSY CProxyRef10::SetDDI_ResourceIsStagingBusy(PFND3D10DDI_RESOURCEISSTAGINGBUSY callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESOURCEISSTAGINGBUSY>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnResourceIsStagingBusy), &(m_pD3D10_1DDI_DEVFUNCS->pfnResourceIsStagingBusy));
}

PFND3D10_1DDI_RELOCATEDEVICEFUNCS CProxyRef10::SetDDI_RelocateDeviceFuncs10_1(PFND3D10_1DDI_RELOCATEDEVICEFUNCS callBackFn)
{
	return SetDDIHelper<PFND3D10_1DDI_RELOCATEDEVICEFUNCS>(callBackFn, NULL, &(m_pD3D10_1DDI_DEVFUNCS->pfnRelocateDeviceFuncs));
}

PFND3D10DDI_RELOCATEDEVICEFUNCS CProxyRef10::SetDDI_RelocateDeviceFuncs10(PFND3D10DDI_RELOCATEDEVICEFUNCS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RELOCATEDEVICEFUNCS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnRelocateDeviceFuncs), NULL);
}

PFND3D10DDI_CALCPRIVATERESOURCESIZE CProxyRef10::SetDDI_CalcPrivateResourceSize(PFND3D10DDI_CALCPRIVATERESOURCESIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATERESOURCESIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateResourceSize), &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateResourceSize));
}

PFND3D10DDI_CALCPRIVATEOPENEDRESOURCESIZE CProxyRef10::SetDDI_CalcPrivateOpenedResourceSize(PFND3D10DDI_CALCPRIVATEOPENEDRESOURCESIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATEOPENEDRESOURCESIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateOpenedResourceSize), &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateOpenedResourceSize));
}

PFND3D10DDI_CREATERESOURCE CProxyRef10::SetDDI_CreateResource(PFND3D10DDI_CREATERESOURCE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATERESOURCE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateResource), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateResource));
}

PFND3D10DDI_OPENRESOURCE CProxyRef10::SetDDI_OpenResource(PFND3D10DDI_OPENRESOURCE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_OPENRESOURCE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnOpenResource), &(m_pD3D10_1DDI_DEVFUNCS->pfnOpenResource));
}

PFND3D10DDI_DESTROYRESOURCE CProxyRef10::SetDDI_DestroyResource(PFND3D10DDI_DESTROYRESOURCE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYRESOURCE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroyResource), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroyResource));
}

PFND3D10_1DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE CProxyRef10::SetDDI_CalcPrivateShaderResourceViewSize10_1(PFND3D10_1DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE callBackFn)
{
	return SetDDIHelper<PFND3D10_1DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE>(callBackFn, NULL, &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateShaderResourceViewSize));
}

PFND3D10DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE CProxyRef10::SetDDI_CalcPrivateShaderResourceViewSize10(PFND3D10DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATESHADERRESOURCEVIEWSIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateShaderResourceViewSize), NULL );
}

PFND3D10DDI_CREATESHADERRESOURCEVIEW CProxyRef10::SetDDI_CreateShaderResourceView10(PFND3D10DDI_CREATESHADERRESOURCEVIEW callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATESHADERRESOURCEVIEW>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateShaderResourceView), NULL);
}

PFND3D10_1DDI_CREATESHADERRESOURCEVIEW CProxyRef10::SetDDI_CreateShaderResourceView10_1(PFND3D10_1DDI_CREATESHADERRESOURCEVIEW callBackFn)
{
	return SetDDIHelper<PFND3D10_1DDI_CREATESHADERRESOURCEVIEW>(callBackFn, NULL, &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateShaderResourceView));
}

PFND3D10DDI_DESTROYSHADERRESOURCEVIEW CProxyRef10::SetDDI_DestroyShaderResourceView(PFND3D10DDI_DESTROYSHADERRESOURCEVIEW callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYSHADERRESOURCEVIEW>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroyShaderResourceView), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroyShaderResourceView));
}

PFND3D10DDI_CALCPRIVATERENDERTARGETVIEWSIZE CProxyRef10::SetDDI_CalcPrivateRenderTargetViewSize(PFND3D10DDI_CALCPRIVATERENDERTARGETVIEWSIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATERENDERTARGETVIEWSIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateRenderTargetViewSize), &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateRenderTargetViewSize));
}

PFND3D10DDI_CREATERENDERTARGETVIEW CProxyRef10::SetDDI_CreateRenderTargetView(PFND3D10DDI_CREATERENDERTARGETVIEW callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATERENDERTARGETVIEW>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateRenderTargetView), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateRenderTargetView));
}

PFND3D10DDI_DESTROYRENDERTARGETVIEW CProxyRef10::SetDDI_DestroyRenderTargetView(PFND3D10DDI_DESTROYRENDERTARGETVIEW callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYRENDERTARGETVIEW>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroyRenderTargetView), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroyRenderTargetView));
}

PFND3D10DDI_CALCPRIVATEDEPTHSTENCILVIEWSIZE CProxyRef10::SetDDI_CalcPrivateDepthStencilViewSize(PFND3D10DDI_CALCPRIVATEDEPTHSTENCILVIEWSIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATEDEPTHSTENCILVIEWSIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateDepthStencilViewSize), &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateDepthStencilViewSize));
}

PFND3D10DDI_CREATEDEPTHSTENCILVIEW CProxyRef10::SetDDI_CreateDepthStencilView(PFND3D10DDI_CREATEDEPTHSTENCILVIEW callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATEDEPTHSTENCILVIEW>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateDepthStencilView), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateDepthStencilView));
}

PFND3D10DDI_DESTROYDEPTHSTENCILVIEW CProxyRef10::SetDDI_DestroyDepthStencilView(PFND3D10DDI_DESTROYDEPTHSTENCILVIEW callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYDEPTHSTENCILVIEW>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroyDepthStencilView), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroyDepthStencilView));
}

PFND3D10DDI_CALCPRIVATEELEMENTLAYOUTSIZE CProxyRef10::SetDDI_CalcPrivateElementLayoutSize(PFND3D10DDI_CALCPRIVATEELEMENTLAYOUTSIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATEELEMENTLAYOUTSIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateElementLayoutSize), &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateElementLayoutSize));
}

PFND3D10DDI_CREATEELEMENTLAYOUT CProxyRef10::SetDDI_CreateElementLayout(PFND3D10DDI_CREATEELEMENTLAYOUT callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATEELEMENTLAYOUT>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateElementLayout), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateElementLayout));
}

PFND3D10DDI_DESTROYELEMENTLAYOUT CProxyRef10::SetDDI_DestroyElementLayout(PFND3D10DDI_DESTROYELEMENTLAYOUT callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYELEMENTLAYOUT>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroyElementLayout), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroyElementLayout));
}

PFND3D10DDI_CALCPRIVATEBLENDSTATESIZE CProxyRef10::SetDDI_CalcPrivateBlendStateSize10(PFND3D10DDI_CALCPRIVATEBLENDSTATESIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATEBLENDSTATESIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateBlendStateSize), NULL);
}

PFND3D10_1DDI_CALCPRIVATEBLENDSTATESIZE CProxyRef10::SetDDI_CalcPrivateBlendStateSize10_1(PFND3D10_1DDI_CALCPRIVATEBLENDSTATESIZE callBackFn)
{
	return SetDDIHelper<PFND3D10_1DDI_CALCPRIVATEBLENDSTATESIZE>(callBackFn, NULL, &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateBlendStateSize));
}

PFND3D10DDI_CREATEBLENDSTATE CProxyRef10::SetDDI_CreateBlendState10(PFND3D10DDI_CREATEBLENDSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATEBLENDSTATE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateBlendState), NULL);
}

PFND3D10_1DDI_CREATEBLENDSTATE CProxyRef10::SetDDI_CreateBlendState10_1(PFND3D10_1DDI_CREATEBLENDSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10_1DDI_CREATEBLENDSTATE>(callBackFn, NULL, &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateBlendState));
}

PFND3D10DDI_DESTROYBLENDSTATE CProxyRef10::SetDDI_DestroyBlendState(PFND3D10DDI_DESTROYBLENDSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYBLENDSTATE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroyBlendState), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroyBlendState));
}

PFND3D10DDI_CALCPRIVATEDEPTHSTENCILSTATESIZE CProxyRef10::SetDDI_CalcPrivateDepthStencilStateSize(PFND3D10DDI_CALCPRIVATEDEPTHSTENCILSTATESIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATEDEPTHSTENCILSTATESIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateDepthStencilStateSize), &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateDepthStencilStateSize));
}

PFND3D10DDI_CREATEDEPTHSTENCILSTATE CProxyRef10::SetDDI_CreateDepthStencilState(PFND3D10DDI_CREATEDEPTHSTENCILSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATEDEPTHSTENCILSTATE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateDepthStencilState), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateDepthStencilState));
}

PFND3D10DDI_DESTROYDEPTHSTENCILSTATE CProxyRef10::SetDDI_DestroyDepthStencilState(PFND3D10DDI_DESTROYDEPTHSTENCILSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYDEPTHSTENCILSTATE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroyDepthStencilState), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroyDepthStencilState));
}

PFND3D10DDI_CALCPRIVATERASTERIZERSTATESIZE CProxyRef10::SetDDI_CalcPrivateRasterizerStateSize(PFND3D10DDI_CALCPRIVATERASTERIZERSTATESIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATERASTERIZERSTATESIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateRasterizerStateSize), &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateRasterizerStateSize));
}

PFND3D10DDI_CREATERASTERIZERSTATE CProxyRef10::SetDDI_CreateRasterizerState(PFND3D10DDI_CREATERASTERIZERSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATERASTERIZERSTATE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateRasterizerState), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateRasterizerState));
}

PFND3D10DDI_DESTROYRASTERIZERSTATE CProxyRef10::SetDDI_DestroyRasterizerState(PFND3D10DDI_DESTROYRASTERIZERSTATE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYRASTERIZERSTATE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroyRasterizerState), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroyRasterizerState));
}

PFND3D10DDI_CALCPRIVATESHADERSIZE CProxyRef10::SetDDI_CalcPrivateShaderSize(PFND3D10DDI_CALCPRIVATESHADERSIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATESHADERSIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateShaderSize), &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateShaderSize));
}

PFND3D10DDI_CREATEVERTEXSHADER CProxyRef10::SetDDI_CreateVertexShader(PFND3D10DDI_CREATEVERTEXSHADER callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATEVERTEXSHADER>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateVertexShader), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateVertexShader));
}

PFND3D10DDI_CREATEGEOMETRYSHADER CProxyRef10::SetDDI_CreateGeometryShader(PFND3D10DDI_CREATEGEOMETRYSHADER callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATEGEOMETRYSHADER>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateGeometryShader), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateGeometryShader));
}

PFND3D10DDI_CREATEPIXELSHADER CProxyRef10::SetDDI_CreatePixelShader(PFND3D10DDI_CREATEPIXELSHADER callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATEPIXELSHADER>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreatePixelShader), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreatePixelShader));
}

PFND3D10DDI_CALCPRIVATEGEOMETRYSHADERWITHSTREAMOUTPUT CProxyRef10::SetDDI_CalcPrivateGeometryShaderWithStreamOutputSize(PFND3D10DDI_CALCPRIVATEGEOMETRYSHADERWITHSTREAMOUTPUT callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATEGEOMETRYSHADERWITHSTREAMOUTPUT>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateGeometryShaderWithStreamOutput), &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateGeometryShaderWithStreamOutput));
}

PFND3D10DDI_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT CProxyRef10::SetDDI_CreateGeometryShaderWithStreamOutput(PFND3D10DDI_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATEGEOMETRYSHADERWITHSTREAMOUTPUT>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateGeometryShaderWithStreamOutput), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateGeometryShaderWithStreamOutput));
}

PFND3D10DDI_DESTROYSHADER CProxyRef10::SetDDI_DestroyShader(PFND3D10DDI_DESTROYSHADER callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYSHADER>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroyShader), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroyShader));
}

PFND3D10DDI_CALCPRIVATESAMPLERSIZE CProxyRef10::SetDDI_CalcPrivateSamplerSize(PFND3D10DDI_CALCPRIVATESAMPLERSIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATESAMPLERSIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateSamplerSize), &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateSamplerSize));
}

PFND3D10DDI_CREATESAMPLER CProxyRef10::SetDDI_CreateSampler(PFND3D10DDI_CREATESAMPLER callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATESAMPLER>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateSampler), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateSampler));
}

PFND3D10DDI_DESTROYSAMPLER CProxyRef10::SetDDI_DestroySampler(PFND3D10DDI_DESTROYSAMPLER callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYSAMPLER>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroySampler), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroySampler));
}

PFND3D10DDI_CALCPRIVATEQUERYSIZE CProxyRef10::SetDDI_CalcPrivateQuerySize(PFND3D10DDI_CALCPRIVATEQUERYSIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CALCPRIVATEQUERYSIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCalcPrivateQuerySize), &(m_pD3D10_1DDI_DEVFUNCS->pfnCalcPrivateQuerySize));
}

PFND3D10DDI_CREATEQUERY CProxyRef10::SetDDI_CreateQuery(PFND3D10DDI_CREATEQUERY callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CREATEQUERY>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCreateQuery), &(m_pD3D10_1DDI_DEVFUNCS->pfnCreateQuery));
}

PFND3D10DDI_DESTROYQUERY CProxyRef10::SetDDI_DestroyQuery(PFND3D10DDI_DESTROYQUERY callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYQUERY>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroyQuery), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroyQuery));
}

PFND3D10DDI_CHECKFORMATSUPPORT CProxyRef10::SetDDI_CheckFormatSupport(PFND3D10DDI_CHECKFORMATSUPPORT callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CHECKFORMATSUPPORT>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCheckFormatSupport), &(m_pD3D10_1DDI_DEVFUNCS->pfnCheckFormatSupport));
}

PFND3D10DDI_CHECKMULTISAMPLEQUALITYLEVELS CProxyRef10::SetDDI_CheckMultisampleQualityLevels(PFND3D10DDI_CHECKMULTISAMPLEQUALITYLEVELS callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CHECKMULTISAMPLEQUALITYLEVELS>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCheckMultisampleQualityLevels), &(m_pD3D10_1DDI_DEVFUNCS->pfnCheckMultisampleQualityLevels));
}

PFND3D10DDI_CHECKCOUNTERINFO CProxyRef10::SetDDI_CheckCounterInfo(PFND3D10DDI_CHECKCOUNTERINFO callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CHECKCOUNTERINFO>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCheckCounterInfo), &(m_pD3D10_1DDI_DEVFUNCS->pfnCheckCounterInfo));
}

PFND3D10DDI_CHECKCOUNTER CProxyRef10::SetDDI_CheckCounter(PFND3D10DDI_CHECKCOUNTER callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_CHECKCOUNTER>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnCheckCounter), &(m_pD3D10_1DDI_DEVFUNCS->pfnCheckCounter));
}

PFND3D10DDI_DESTROYDEVICE CProxyRef10::SetDDI_DestroyDevice(PFND3D10DDI_DESTROYDEVICE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_DESTROYDEVICE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnDestroyDevice), &(m_pD3D10_1DDI_DEVFUNCS->pfnDestroyDevice));
}

PFND3D10DDI_SETTEXTFILTERSIZE CProxyRef10::SetDDI_SetTextFilter(PFND3D10DDI_SETTEXTFILTERSIZE callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETTEXTFILTERSIZE>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnSetTextFilterSize), &(m_pD3D10_1DDI_DEVFUNCS->pfnSetTextFilterSize));
}

#if !defined( REF10_LDK_BUILD )
#ifdef D3D10PSGP
PFND3D10DDI_RESETPRIMITIVEID CProxyRef10::SetDDI_IaResetPrimitiveID(PFND3D10DDI_RESETPRIMITIVEID callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_RESETPRIMITIVEID>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnResetPrimitiveID), &(m_pD3D10_1DDI_DEVFUNCS->pfnResetPrimitiveID))
}

PFND3D10DDI_SETVERTEXPIPELINEOUTPUT CProxyRef10::SetDDI_IaSetVertexPipelineOutput(PFND3D10DDI_SETVERTEXPIPELINEOUTPUT callBackFn)
{
	return SetDDIHelper<PFND3D10DDI_SETVERTEXPIPELINEOUTPUT>(callBackFn, &(m_pD3D10DDI_DEVFUNCS->pfnSetVertexPipelineOutput), &(m_pD3D10_1DDI_DEVFUNCS->pfnSetVertexPipelineOutput));
}
#endif
#endif