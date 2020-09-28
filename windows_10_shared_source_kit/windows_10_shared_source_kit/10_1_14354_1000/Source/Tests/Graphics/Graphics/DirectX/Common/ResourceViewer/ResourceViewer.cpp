#include "ResourceViewerImpl.h"
#include "FormatHelper.h"
#include "D3DFormatConverter.h"
#include "Handle.h"

using namespace WindowsTest;

#include <exception>

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(ptr) { if(nullptr != ptr) { ptr->Release(); ptr = nullptr; } }
#endif

#ifndef SAFE_DELETE
#define SAFE_DELETE(ptr) { if(nullptr != ptr) { delete(ptr); ptr = nullptr; } }
#endif

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(ptr) { if(nullptr != ptr) { delete[](ptr); ptr = nullptr; } }
#endif

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::CResourceViewerWindow()
////////////////////////////////////////////////////////////////////////////////
CResourceViewerWindow::CResourceViewerWindow(void) :
	m_pimpl(new CResourceViewerWindowImpl)
{
}

CResourceViewerWindowImpl::CResourceViewerWindowImpl(void) :
	m_hWnd(NULL),
	m_pDevice(nullptr),
	m_pSwapChain(nullptr),
	m_pFormattedCopy(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::~CResourceViewerWindow()
////////////////////////////////////////////////////////////////////////////////
CResourceViewerWindow::~CResourceViewerWindow()
{
}

CResourceViewerWindowImpl::~CResourceViewerWindowImpl(void)
{
	SAFE_RELEASE(m_pFormattedCopy);
	SAFE_RELEASE(m_pSwapChain);
	SAFE_RELEASE(m_pDevice);

	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::GetWindowHandle()
////////////////////////////////////////////////////////////////////////////////
HWND CResourceViewerWindow::GetWindowHandle() const { return m_pimpl->GetWindowHandle(); }

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::PrivateWindowProc()
////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK CResourceViewerWindowImpl::PrivateWindowProc(
	HWND hwnd,
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	)
{
	CResourceViewerWindowImpl *pResourceViewer = CResourceViewerWindowImpl::GetResourceViewerClassPointer(hwnd);

	switch(uMsg)
	{
	case WM_CREATE:
		{
			// Store the pointer to the owner CResourceViewerWindowImpl object in windows user data
			LPCREATESTRUCT lpcs = (LPCREATESTRUCT) lParam;
			SetWindowLongPtr( hwnd, GWLP_USERDATA, (LONG_PTR) lpcs->lpCreateParams );
			pResourceViewer = reinterpret_cast<CResourceViewerWindowImpl *>(lpcs->lpCreateParams);
		}
		break;
	}

	if(nullptr != pResourceViewer)
		return pResourceViewer->WindowProc(uMsg, wParam, lParam);

	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::WindowProc()
////////////////////////////////////////////////////////////////////////////////
LRESULT CResourceViewerWindowImpl::WindowProc(
	UINT uMsg,
	WPARAM wParam,
	LPARAM lParam
	)
{
	switch(uMsg)
	{
	case WM_SIZE:
		break;

	case WM_PAINT:
		if(FAILED(Present()))
			throw(std::exception("Present() failed"));
		break;

	case WM_CLOSE:
		Destroy();
		break;

	default:
		return DefWindowProc(m_hWnd, uMsg, wParam, lParam);
	}

	return 0;
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::Present()
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindow::Present() { return m_pimpl->Present(); }
HRESULT CResourceViewerWindowImpl::Present()
{
	HRESULT hr = S_OK;

	RECT rect;
	if(TRUE == GetUpdateRect(m_hWnd, &rect, FALSE))
	{
		// Present data from the swap chain that needs updating
		if(m_pSwapChain && m_pFormattedCopy)
		{
			// Copy the formatted data to the back buffer
			CHandle<ID3D11DeviceContext> pContext;
			m_pDevice->GetImmediateContext(&pContext);

			CHandle<ID3D11Texture2D> pBackBuffer;
			hr = m_pSwapChain->GetBuffer(0, __uuidof(pBackBuffer), (void **) &pBackBuffer);
			if(SUCCEEDED(hr))
			{
				pContext->CopyResource(pBackBuffer, m_pFormattedCopy);

				hr = m_pSwapChain->Present(0, DXGI_PRESENT_DO_NOT_WAIT);
			}
		}

		ValidateRect(m_hWnd, &rect);
	}

	return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::Create()
//
// Used to keep track of windows using the ResourceViewer window class.
// When all windows are closed this unregisters the class.
////////////////////////////////////////////////////////////////////////////////
static class CAtom *s_pAtom = nullptr;

class CAtom : public CRefObject
{
public:
	CAtom() : m_atom(NULL) 
	{
	}

	~CAtom()
	{ 
		UnregisterClass(reinterpret_cast<PCWSTR>(m_atom), NULL);
		s_pAtom = nullptr;
	}

	ATOM m_atom;
};

HRESULT CResourceViewerWindow::Create(
	HWND hWndParent, 
	_In_ ID3D11Device1 *pDevice, 
	DWORD windowStyle,
	DWORD extendedWindowStyle,
	DXGI_FORMAT swapChainFormat
	)
{
	return m_pimpl->Create(hWndParent, pDevice, windowStyle, extendedWindowStyle, swapChainFormat);
}

HRESULT CResourceViewerWindowImpl::Create(
	HWND hWndParent, 
	_In_ ID3D11Device1 *pDevice, 
	DWORD windowStyle,
	DWORD extendedWindowStyle,
	DXGI_FORMAT swapChainFormat
	)
{
	//
	// Create the window class (if needed)
	//
	if(NULL == s_pAtom)
	{
		s_pAtom = new CAtom;

		WNDCLASSEX wndClass;
		wndClass.cbSize = sizeof(wndClass);
		wndClass.style = CS_VREDRAW|CS_HREDRAW;
		wndClass.lpfnWndProc = PrivateWindowProc;
		wndClass.cbClsExtra = 0;
		wndClass.cbWndExtra = sizeof(void *); // Set aside enough bytes for a pointer to the CResourceViewerWindow class
		wndClass.hInstance = NULL;
		wndClass.hIcon = NULL;
		wndClass.hCursor = NULL;
		wndClass.hbrBackground = (HBRUSH) GetStockObject(BLACK_BRUSH);
		wndClass.lpszMenuName = nullptr;
		wndClass.lpszClassName = L"ResourceViewer";
		wndClass.hIconSm = NULL;

		s_pAtom->m_atom = RegisterClassEx(&wndClass);
		if(0 == s_pAtom->m_atom)
			return E_FAIL;
	}

	s_pAtom->AddRef();

	//
	// Create the window
	//
	m_hWnd = CreateWindowEx(
		extendedWindowStyle, 
		L"ResourceViewer", 
		L"ResourceViewer", 
		windowStyle, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		CW_USEDEFAULT, 
		hWndParent, 
		NULL, 
		NULL, 
		this
		); 

	assert(this == GetResourceViewerClassPointer(m_hWnd));

	if(m_hWnd == NULL)
		return E_FAIL;

	//
	// Create the swapchain
	//

	// Get the DXGI adapter
	HRESULT hr = S_OK;
	CHandle<IDXGIDevice> pDXGIDevice = nullptr;
	hr = pDevice->QueryInterface(__uuidof(IDXGIDevice), (void **) &pDXGIDevice);
	if(FAILED(hr))
		return hr;

	CHandle<IDXGIAdapter> pDXGIAdapter = nullptr;
	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void **)&pDXGIAdapter);
	if(FAILED(hr))
		return hr;

	CHandle<IDXGIFactory2> pDXGIFactory = nullptr;
	hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory2), (void **) &pDXGIFactory);
	if(FAILED(hr))
		return hr;

	RECT clientRect;
	if(FALSE == GetClientRect(m_hWnd, &clientRect))
		return E_FAIL;

	DXGI_SWAP_CHAIN_DESC1 swapChainDesc;
	swapChainDesc.Width = clientRect.right - clientRect.left;
	swapChainDesc.Height = clientRect.bottom - clientRect.top;
	swapChainDesc.Format = swapChainFormat;
	swapChainDesc.Stereo = FALSE;
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapChainDesc.BufferCount = 1;
	swapChainDesc.Scaling = DXGI_SCALING_STRETCH;
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_SEQUENTIAL;
	swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_IGNORE;
	swapChainDesc.Flags = 0;

	hr = pDXGIFactory->CreateSwapChainForHwnd(pDevice, m_hWnd, &swapChainDesc, nullptr, nullptr, &m_pSwapChain);
	if(FAILED(hr))
		return hr;

	// Add a reference to the D3D device
	m_pDevice = pDevice;
	m_pDevice->AddRef();

	return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::Destroy()
////////////////////////////////////////////////////////////////////////////////
void CResourceViewerWindow::Destroy() { m_pimpl->Destroy(); }
void CResourceViewerWindowImpl::Destroy()
{
	// Release the swap chain
	SAFE_RELEASE(m_pSwapChain);

	// Destroy the window
	if(m_hWnd)
	{
		DestroyWindow(m_hWnd);
		m_hWnd = NULL;
	}

	s_pAtom->Release();
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::SetTitle()
////////////////////////////////////////////////////////////////////////////////
void CResourceViewerWindow::SetTitle(_In_ PCWSTR title) { m_pimpl->SetTitle(title); }
void CResourceViewerWindowImpl::SetTitle(_In_ PCWSTR title)
{
	if(NULL != m_hWnd)
		SetWindowText(m_hWnd, title);
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::ShowWindow()
////////////////////////////////////////////////////////////////////////////////
void CResourceViewerWindow::ShowWindow() { m_pimpl->ShowWindow(); }
void CResourceViewerWindowImpl::ShowWindow()
{
	if(NULL != m_hWnd)
	{
		if(!IsWindowVisible(m_hWnd))
		{
			::ShowWindow(m_hWnd, SW_NORMAL);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::HideWindow()
////////////////////////////////////////////////////////////////////////////////
void CResourceViewerWindow::HideWindow() { m_pimpl->HideWindow(); }
void CResourceViewerWindowImpl::HideWindow()
{
	if(NULL != m_hWnd)
		::ShowWindow(m_hWnd, SW_HIDE);
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::GetResourceViewerClassPointer()
////////////////////////////////////////////////////////////////////////////////
CResourceViewerWindowImpl *CResourceViewerWindowImpl::GetResourceViewerClassPointer(HWND hWnd)
{
	return reinterpret_cast<CResourceViewerWindowImpl *>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::ResizeBuffersToWindowClient()
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindowImpl::ResizeBuffersToWindowClient()
{
	if(NULL == m_hWnd || nullptr == m_pSwapChain)
		return S_FALSE;

	RECT clientRect;

	GetClientRect(m_hWnd, &clientRect);

	return m_pSwapChain->ResizeBuffers(1, clientRect.right - clientRect.left, clientRect.bottom - clientRect.top, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::ShowResource()
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindow::ShowResource(
	_In_ ID3D11Resource *pResource, 
	UINT mipLevel, 
	UINT arraySlice,
	UINT depthSlice,
	float resizeScale,
	DXGI_FORMAT format
	)
{
	return m_pimpl->ShowResource(pResource, mipLevel, arraySlice, depthSlice, resizeScale, format);
}

HRESULT CResourceViewerWindowImpl::ShowResource(
	_In_ ID3D11Resource *pResource, 
	UINT mipLevel, 
	UINT arraySlice,
	UINT depthSlice,
	float resizeScale,
	DXGI_FORMAT format
	)
{
	HRESULT hr = S_OK;
	D3D11_RESOURCE_DIMENSION dimension;
	pResource->GetType(&dimension);

	ShowWindow();

	switch(dimension)
	{
	case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
		hr = ShowTexture1D(reinterpret_cast<ID3D11Texture1D *>(pResource), mipLevel, arraySlice, resizeScale, format);
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
		hr = ShowTexture2D(reinterpret_cast<ID3D11Texture2D *>(pResource), mipLevel, arraySlice, resizeScale, format);
		break;
	case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
		hr = ShowTexture3D(reinterpret_cast<ID3D11Texture3D *>(pResource), depthSlice, mipLevel, arraySlice, resizeScale, format);
		break;
		 
	case D3D11_RESOURCE_DIMENSION_UNKNOWN:
	case D3D11_RESOURCE_DIMENSION_BUFFER:
	default:
		hr = E_NOTIMPL;
		break;
	};

	return hr;
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::ShowTexture1D()
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindowImpl::ShowTexture1D(_In_ ID3D11Texture1D *pTexture1D, UINT mipLevel, UINT arraySlice, float resizeScale, DXGI_FORMAT format)
{
	if(nullptr == m_pDevice)
		return E_FAIL;

	D3D11_TEXTURE1D_DESC srcDesc;

	pTexture1D->GetDesc(&srcDesc);
	UINT subresourceWidth = srcDesc.Width;
	CFormatHelper::GetMipDimensions(mipLevel, &subresourceWidth);
	UINT subresource = arraySlice * srcDesc.MipLevels + mipLevel;

	if(DXGI_FORMAT_UNKNOWN == format)
		format = srcDesc.Format;

	D3D11_BOX box;
	box.top = 0;
	box.left = 0;
	box.right = subresourceWidth;
	box.bottom = 1;
	box.front = 0;
	box.back = 1;

	return ShowSubresource(pTexture1D, subresource, &box, resizeScale, format);
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::ShowTexture2D()
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindowImpl::ShowTexture2D(_In_ ID3D11Texture2D *pTexture2D, UINT mipLevel, UINT arraySlice, float resizeScale, DXGI_FORMAT format)
{
	if(nullptr == m_pDevice)
		return E_FAIL;

	D3D11_TEXTURE2D_DESC srcDesc;

	pTexture2D->GetDesc(&srcDesc);
	UINT subresourceWidth = srcDesc.Width;
	UINT subresourceHeight = srcDesc.Height;
	CFormatHelper::GetMipDimensions(mipLevel, &subresourceWidth, &subresourceHeight);
	UINT subresource = arraySlice * srcDesc.MipLevels + mipLevel;

	if(DXGI_FORMAT_UNKNOWN == format)
		format = srcDesc.Format;

	D3D11_BOX box;
	box.top = 0;
	box.left = 0;
	box.right = subresourceWidth;
	box.bottom = subresourceHeight;
	box.front = 0;
	box.back = 1;

	return ShowSubresource(pTexture2D, subresource, &box, resizeScale, format);
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::ShowTexture3D
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindowImpl::ShowTexture3D(_In_ ID3D11Texture3D *pTexture3D, UINT depthSlice, UINT mipLevel, UINT arraySlice, float resizeScale, DXGI_FORMAT format)
{
	if(nullptr == m_pDevice)
		return E_FAIL;

	D3D11_TEXTURE3D_DESC srcDesc;

	pTexture3D->GetDesc(&srcDesc);
	UINT subresourceWidth = srcDesc.Width;
	UINT subresourceHeight = srcDesc.Height;
	CFormatHelper::GetMipDimensions(mipLevel, &subresourceWidth, &subresourceHeight);
	UINT subresource = arraySlice * srcDesc.MipLevels + mipLevel;

	if(DXGI_FORMAT_UNKNOWN == format)
		format = srcDesc.Format;

	D3D11_BOX box;
	box.top = 0;
	box.left = 0;
	box.right = subresourceWidth;
	box.bottom = subresourceHeight;
	box.front = depthSlice;
	box.back = box.front + 1;

	return ShowSubresource(pTexture3D, subresource, &box, resizeScale, format);
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::ShowSubresource
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindowImpl::ShowSubresource(_In_ ID3D11Resource *pResource, UINT subresource, D3D11_BOX *pBox, float resizeScale, DXGI_FORMAT format)
{
	HRESULT hr = S_OK;
	CHandle<ID3D11DeviceContext1> pContext = nullptr;
	CHandle<ID3D11Texture2D> pStagedCopy = nullptr;
	CHandle<ID3D11Texture2D> pFormattedCopy = nullptr;

	UINT subresourceWidth = pBox->right;
	UINT subresourceHeight = pBox->bottom;

	// Make sure the format can be displayed
	switch(format)
	{
	default:
		return E_NOTIMPL;
		break;
	case DXGI_FORMAT_R16G16B16A16_UNORM:
	case DXGI_FORMAT_R10G10B10A2_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM:
	case DXGI_FORMAT_R8G8B8A8_UNORM_SRGB:
	case DXGI_FORMAT_R16G16_UNORM:
	case DXGI_FORMAT_D24_UNORM_S8_UINT:
	case DXGI_FORMAT_R24_UNORM_X8_TYPELESS:
	case DXGI_FORMAT_R8G8_UNORM:
	case DXGI_FORMAT_D16_UNORM:
	case DXGI_FORMAT_R16_UNORM:
	case DXGI_FORMAT_R8_UNORM:
	case DXGI_FORMAT_A8_UNORM:
	case DXGI_FORMAT_R8G8_B8G8_UNORM:
	case DXGI_FORMAT_G8R8_G8B8_UNORM:
	case DXGI_FORMAT_BC1_UNORM:
	case DXGI_FORMAT_BC1_UNORM_SRGB:
	case DXGI_FORMAT_BC2_UNORM:
	case DXGI_FORMAT_BC2_UNORM_SRGB:
	case DXGI_FORMAT_BC3_UNORM:
	case DXGI_FORMAT_BC3_UNORM_SRGB:
	case DXGI_FORMAT_BC4_UNORM:
	case DXGI_FORMAT_BC5_UNORM:
	case DXGI_FORMAT_B5G6R5_UNORM:
	case DXGI_FORMAT_B5G5R5A1_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM:
	case DXGI_FORMAT_B8G8R8X8_UNORM:
	case DXGI_FORMAT_R10G10B10_XR_BIAS_A2_UNORM:
	case DXGI_FORMAT_B8G8R8A8_UNORM_SRGB:
	case DXGI_FORMAT_B8G8R8X8_UNORM_SRGB:
	case DXGI_FORMAT_BC7_UNORM:
	case DXGI_FORMAT_BC7_UNORM_SRGB:
	case DXGI_FORMAT_B4G4R4A4_UNORM:
	case DXGI_FORMAT_R32G32B32A32_FLOAT:
	case DXGI_FORMAT_R32G32B32_FLOAT:
	case DXGI_FORMAT_R16G16B16A16_FLOAT:
	case DXGI_FORMAT_R32G32_FLOAT:
	case DXGI_FORMAT_D32_FLOAT_S8X24_UINT:
	case DXGI_FORMAT_R32_FLOAT_X8X24_TYPELESS:
	case DXGI_FORMAT_R11G11B10_FLOAT:
	case DXGI_FORMAT_R16G16_FLOAT:
	case DXGI_FORMAT_D32_FLOAT:
	case DXGI_FORMAT_R32_FLOAT:
	case DXGI_FORMAT_R16_FLOAT:
		break;
	}

	// Resize the window to fit the resource
	DWORD wndStyle = GetWindowLong(m_hWnd, GWL_STYLE);
	RECT rc;
	rc.left = 0;
	rc.right = (UINT) (resizeScale * subresourceWidth);
	rc.top = 0;
	rc.bottom = (UINT) (resizeScale * subresourceHeight);
	AdjustWindowRect(&rc, wndStyle, false);
	int windowWidth = rc.right - rc.left;
	int windowHeight = rc.bottom - rc.top;

	RECT rcCur;
	GetWindowRect(m_hWnd, &rcCur);
	
	// Only reposition the window if the size differs from the current size
	if(rcCur.right - rcCur.left != rc.right - rc.left || rcCur.bottom - rcCur.top != rc.bottom - rc.top)
	{
		if(FALSE == SetWindowPos(m_hWnd, HWND_NOTOPMOST, 0, 0, windowWidth, windowHeight, SWP_NOACTIVATE|SWP_NOZORDER|SWP_NOMOVE))
			return E_FAIL;
	}

	// Resize the swap chain to match the subresource dimensions
	hr = m_pSwapChain->ResizeBuffers(1, subresourceWidth, subresourceHeight, DXGI_FORMAT_R8G8B8A8_UNORM, 0);
	if(FAILED(hr))
	{
		return hr;
	}

	// Get back buffer texture
	CHandle<ID3D11Texture2D> pBackBuffer;
	hr = m_pSwapChain->GetBuffer(0, __uuidof(pBackBuffer), (void **) &pBackBuffer);
	if(FAILED(hr))
	{
		return hr;
	}

	// Create the formatted copy texture to match back buffer
	CD3D11_TEXTURE2D_DESC formattedCopyDesc;
	pBackBuffer->GetDesc(&formattedCopyDesc);

	if(format == formattedCopyDesc.Format)
	{
		// Back buffer format matches the source texture format so optimize
		// resources since we do not need to convert pixel data
		formattedCopyDesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
		formattedCopyDesc.CPUAccessFlags = 0;
		formattedCopyDesc.Usage = D3D11_USAGE_DEFAULT;
	}
	else
	{
		formattedCopyDesc.BindFlags = 0;
		formattedCopyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		formattedCopyDesc.Usage = D3D11_USAGE_STAGING;
	}

	// Formatted copy dimensions must match subresource dimensions
	formattedCopyDesc.Width = subresourceWidth;
	formattedCopyDesc.Height = subresourceHeight;

	if(nullptr != m_pFormattedCopy)
	{
		// We already have a formatted copy texture so see if it matches our current needs
		CD3D11_TEXTURE2D_DESC oldFormattedCopyDesc;
		m_pFormattedCopy->GetDesc(&oldFormattedCopyDesc);

		if(0 == memcmp(&oldFormattedCopyDesc, &formattedCopyDesc, sizeof(formattedCopyDesc)))
		{
			// Don't bother creating a new formatted copy since the 
			// existing texture matches what we need
			pFormattedCopy.Attach(m_pFormattedCopy);
		}
	}

	if(nullptr == pFormattedCopy)
	{
		hr = m_pDevice->CreateTexture2D(&formattedCopyDesc, NULL, &pFormattedCopy);
		if(FAILED(hr))
			return hr;
	}

	m_pDevice->GetImmediateContext1(&pContext);

	// If the source texture format matches the back buffer
	// then just do a direct CopySubresourceRegion() directly to the back buffer and save
	// the hit of doing a pixel conversion
	if(formattedCopyDesc.Format == format)
	{
		// Copy the subresource directly to the formatted texture
		pContext->CopySubresourceRegion(pFormattedCopy, 0, 0, 0, 0, pResource, subresource, pBox);
	}
	else
	{
		D3D11_TEXTURE2D_DESC copyDesc;

		// Create a cpu-readable staging texture 2d to copy the texture subresource
		copyDesc.Format = format;
		copyDesc.ArraySize = 1;
		copyDesc.BindFlags = 0;
		copyDesc.MipLevels = 1;
		copyDesc.Usage = D3D11_USAGE_STAGING;
		copyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_READ;
		copyDesc.SampleDesc.Count = 1;
		copyDesc.SampleDesc.Quality = 0;
		copyDesc.MiscFlags = 0;
		copyDesc.Width = subresourceWidth;
		copyDesc.Height = subresourceHeight;

		hr = m_pDevice->CreateTexture2D(&copyDesc, nullptr, &pStagedCopy);
		if(FAILED(hr))
			return hr;

		// Copy the entire subresource to the staged texture2d
		pContext->CopySubresourceRegion(pStagedCopy, 0, 0, 0, 0, pResource, subresource, pBox);

		D3D11_MAPPED_SUBRESOURCE mapDataSrc;
		D3D11_MAPPED_SUBRESOURCE mapDataDest;
		hr = pContext->Map(pStagedCopy, 0, D3D11_MAP_READ, 0, &mapDataSrc);
		if(FAILED(hr))
			return hr;
		hr = pContext->Map(pFormattedCopy, 0, D3D11_MAP_WRITE, 0, &mapDataDest);
		if(FAILED(hr))
			return hr;

		// Copy converted pixels to pFormattedCopy
		BYTE *pDestRow;
		UINT srcPixelSize = CFormatHelper::ElementBytes(copyDesc.Format);
		UINT destPixelSize = CFormatHelper::ElementBytes(formattedCopyDesc.Format);
		pDestRow = reinterpret_cast<BYTE *>(mapDataDest.pData);
		UINT srcWidthInBytes = srcPixelSize * copyDesc.Width;
		UINT destWidthInBytes = destPixelSize * formattedCopyDesc.Width;

		//
		// Convert the data to the target format
		//

		hr = CD3DFormatConverter::Convert2D(
			mapDataSrc.pData,
			mapDataDest.pData,
			format,
			formattedCopyDesc.Format,
			0,
			0,
			copyDesc.Width,
			copyDesc.Height,
			0,
			0,
			mapDataSrc.RowPitch,
			mapDataDest.RowPitch
			);

		pContext->Unmap(pStagedCopy, 0);
		pContext->Unmap(pFormattedCopy, 0);

		if(FAILED(hr))
		{
			return hr;
		}
	}

	// Free previous copy
	if(pFormattedCopy != m_pFormattedCopy)
	{
		SAFE_RELEASE(m_pFormattedCopy);
	}

	// Set the new copy
	m_pFormattedCopy = pFormattedCopy;
	pFormattedCopy.Detach();

	InvalidateRect(m_hWnd, NULL, FALSE);

	return hr;
}


////////////////////////////////////////////////////////////////////////////////
//
// class CResourceViewer
//
////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::CResourceViewer()
////////////////////////////////////////////////////////////////////////////
CResourceViewer::CResourceViewer() :
	m_pimpl(new CResourceViewerImpl)
{
}

CResourceViewerImpl::CResourceViewerImpl() :
	m_pDevice(nullptr)
{
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::~CResourceViewer()
////////////////////////////////////////////////////////////////////////////
CResourceViewer::~CResourceViewer()
{
}

CResourceViewerImpl::~CResourceViewerImpl()
{
	Cleanup();
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::Setup()
//
// Initialize the CViewerManager infrastructure
////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewer::Setup(_In_ ID3D11Device1 *pDevice) { return m_pimpl->Setup(pDevice); }
HRESULT CResourceViewerImpl::Setup(_In_ ID3D11Device1 *pDevice)
{
	// Hold pointer to the device and AddRef()
	m_pDevice = pDevice;
	m_pDevice->AddRef();

	// May need additional setup in the future so returning an HRESULT
	// to prepare for possible failure conditions.
	return S_OK;
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::Cleanup()
//
// Destroys all of the existing resource viewer windows and free CResourceViewerWindow container objects
////////////////////////////////////////////////////////////////////////////
void CResourceViewer::Cleanup() { m_pimpl->Cleanup(); }
void CResourceViewerImpl::Cleanup()
{
	// Destroy all contained windows
	DestroyAll();

	// Release the D3D device
	SAFE_RELEASE(m_pDevice);
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::DestroyWindow()
//
// Destroy CResourceViewerWindow object by name
////////////////////////////////////////////////////////////////////////////
void CResourceViewer::DestroyWindow(_In_ PCWSTR name) { m_pimpl->DestroyWindow(name); }
void CResourceViewerImpl::DestroyWindow(_In_ PCWSTR name)
{
	CResourceViewerWindow *pWindow = FindResourceViewerWindow(name);

	if(nullptr != pWindow)
	{
		DestroyWindow(pWindow);
	}
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::GetResourceViewerWindowName()
////////////////////////////////////////////////////////////////////////////
PCWSTR CResourceViewer::GetResourceViewerWindowName(_In_ CResourceViewerWindow *pWindow) const
{
	return m_pimpl->GetResourceViewerWindowName(pWindow);
}
PCWSTR CResourceViewerImpl::GetResourceViewerWindowName(_In_ CResourceViewerWindow *pWindow) const
{
	PCWSTR pName = nullptr;
	ResourceViewerWindowMapByPointer::const_iterator it = m_resourceViewerMapByPointer.find(pWindow);
	if(it != m_resourceViewerMapByPointer.end())
	{
		pName = it->second;
	}

	return pName;
}



////////////////////////////////////////////////////////////////////////////
// CResourceViewer::DestroyWindow()
//
// Destroy CResourceViewerWindow object by pointer
////////////////////////////////////////////////////////////////////////////
void CResourceViewer::DestroyWindow(_In_ CResourceViewerWindow *pResourceViewerWindow) { m_pimpl->DestroyWindow(pResourceViewerWindow); }
void CResourceViewerImpl::DestroyWindow(_In_ CResourceViewerWindow *pResourceViewerWindow)
{
	PCWSTR pName = GetResourceViewerWindowName(pResourceViewerWindow);
	
	if(nullptr != pName)
	{
		m_resourceViewerMapByName.erase(pName);
		m_resourceViewerMapByPointer.erase(pResourceViewerWindow);
		pResourceViewerWindow->Destroy();
		delete(pResourceViewerWindow);
	}
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::DestroyAll()
//
// Destroy all CResourceViewerWindow objects
////////////////////////////////////////////////////////////////////////////
void CResourceViewer::DestroyAll() { m_pimpl->DestroyAll(); }
void CResourceViewerImpl::DestroyAll()
{
	// Clear the maps
	for(ResourceViewerWindowMapByPointer::iterator it = m_resourceViewerMapByPointer.begin(); it != m_resourceViewerMapByPointer.end();)
	{
		CResourceViewerWindow *pWindow = it->first;
		PCWSTR name = it->second;
		m_resourceViewerMapByName.erase(name);
		it = m_resourceViewerMapByPointer.erase(it);
		pWindow->Destroy();
		delete(pWindow);
	}
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::HideWindow()
// 
// Hide CResourceViewerWindow object by name
////////////////////////////////////////////////////////////////////////////
void CResourceViewer::HideWindow(_In_ PCWSTR name) { m_pimpl->HideWindow(name); }
void CResourceViewerImpl::HideWindow(_In_ PCWSTR name)
{
	CResourceViewerWindow *pWindow = FindResourceViewerWindow(name);
	if(nullptr != pWindow)
	{
		pWindow->HideWindow();
	}
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::HideAll()
//
// Hide all CResourceViewerWindow objects
////////////////////////////////////////////////////////////////////////////
void CResourceViewer::HideAll() { m_pimpl->HideAll(); }
void CResourceViewerImpl::HideAll()
{
	for(ResourceViewerWindowMapByPointer::iterator it = m_resourceViewerMapByPointer.begin(); it != m_resourceViewerMapByPointer.end();++it)
	{
		CResourceViewerWindow *pWindow = it->first;
		pWindow->HideWindow();
	}
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::NewResourceViewerWindow()
//
// Create a new resource viewer (initially hidden) and assign the name provided
////////////////////////////////////////////////////////////////////////////
CResourceViewerWindow *CResourceViewer::NewResourceViewerWindow(_In_ PCWSTR name) { return m_pimpl->NewResourceViewerWindow(name); }
CResourceViewerWindow *CResourceViewerImpl::NewResourceViewerWindow(_In_ PCWSTR name)
{
	CResourceViewerWindow *pWindow = nullptr;

	// Only create a resource viewer window if this has been Setup
	if(m_pDevice)
	{
		// Create the CResourceViewerWindow object
		pWindow = new CResourceViewerWindow();
		if(FAILED(pWindow->Create(NULL, m_pDevice)))
		{
			delete(pWindow);
			return nullptr;
		}

		// Add the window to the maps
		m_resourceViewerMapByName.insert(ResourceViewerWindowMapByName::value_type(name, pWindow));
		m_resourceViewerMapByPointer.insert(ResourceViewerWindowMapByPointer::value_type(pWindow, name));
	}

	return pWindow;
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::FindResourceViewerWindow()
//
// Returns pointer to CResourceViewerWindow object with the specified name.
// If no window exists the method resturns nullptr
////////////////////////////////////////////////////////////////////////////
CResourceViewerWindow *CResourceViewer::FindResourceViewerWindow(_In_ PCWSTR name) const { return m_pimpl->FindResourceViewerWindow(name); }
CResourceViewerWindow *CResourceViewerImpl::FindResourceViewerWindow(_In_ PCWSTR name) const
{
	CResourceViewerWindow *pWindow = nullptr;
	ResourceViewerWindowMapByName::const_iterator it = m_resourceViewerMapByName.find(name);

	if(it != m_resourceViewerMapByName.end())
	{
		pWindow = it->second;
	}

	return pWindow;
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::ShowResource()
//
// Shows a resource in the CResourceViewerWindow window with the name provided.
// If no window exists with the provided name a viewer window will be 
// created.
//
// name - Name of the CResourceViewerWindow object
// pResource - Pointer to the texture resource to draw
// mipLevel - Mip level to draw
// arraySlice - Array slice to draw
// depthSlice - Depth slice to draw (use 0 for Texture1D and Texture2D)
// resizeScale - Amount to scale the window as a factor of subresource size (set to 0 to prevent window resize)
// format - Format used to interpret resource data (Use DXGI_FORMAT_UNKNOWN to let ResourceViewer decide)
////////////////////////////////////////////////////////////////////////////
CResourceViewerWindow *CResourceViewer::ShowResource(
	_In_ PCWSTR name,
	_In_ ID3D11Resource *pResource, 
	UINT mipLevel,
	UINT arraySlice,
	UINT depthSlice,
	float resizeScale,
	DXGI_FORMAT format
	)
{
	return m_pimpl->ShowResource(name, pResource, mipLevel, arraySlice, depthSlice, resizeScale, format);
}

CResourceViewerWindow *CResourceViewerImpl::ShowResource(
	_In_ PCWSTR name,
	_In_ ID3D11Resource *pResource, 
	UINT mipLevel,
	UINT arraySlice,
	UINT depthSlice,
	float resizeScale,
	DXGI_FORMAT format
	)
{
	CResourceViewerWindow *pWindow = nullptr;
	
	if(m_pDevice)
	{
		pWindow = FindResourceViewerWindow(name);

		if(nullptr == pWindow)
		{
			// Create new window
			pWindow = NewResourceViewerWindow(name);
		}

		if(nullptr != pWindow)
		{
			// Show the resource
			pWindow->SetTitle(name);

			HRESULT hr = pWindow->ShowResource(
				pResource,
				mipLevel,
				arraySlice,
				depthSlice,
				resizeScale,
				format
				);

			if(FAILED(hr))
			{
				return nullptr;
			}
		}
	}

	return pWindow;
}
