#include "ResourceViewerImpl.h"
#include "FormatHelper.h"
#include "D3DFormatConverter.h"

using namespace WindowsTest;

#include <exception>

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::CResourceViewerWindow()
////////////////////////////////////////////////////////////////////////////////
CResourceViewerWindow::CResourceViewerWindow() :
    m_pimpl(new CResourceViewerWindowImpl)
{
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::~CResourceViewerWindow()
////////////////////////////////////////////////////////////////////////////////
CResourceViewerWindow::~CResourceViewerWindow()
{
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::Create()
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindow::Create(
    std::wstring const& filename, 
    _In_ ID3D11Device1 *pDevice
    )
{
    return m_pimpl->Create(filename, pDevice);
}

HRESULT CResourceViewerWindowImpl::Create(
    std::wstring const& filename, 
    _In_ ID3D11Device1 *pDevice
    )
{
    m_filename = filename + L".bmp";
    m_pDevice = pDevice;

    return S_OK;
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::ShowResource()
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindow::ShowResource(
    _In_ ID3D11Resource *pResource, 
    UINT mipLevel, 
    UINT arraySlice,
    UINT depthSlice,
    DXGI_FORMAT format
    )
{
    return m_pimpl->ShowResource(pResource, mipLevel, arraySlice, depthSlice, format);
}

HRESULT CResourceViewerWindowImpl::ShowResource(
    _In_ ID3D11Resource *pResource, 
    UINT mipLevel, 
    UINT arraySlice,
    UINT depthSlice,
    DXGI_FORMAT format
    )
{
    HRESULT hr = S_OK;
    D3D11_RESOURCE_DIMENSION dimension;
    pResource->GetType(&dimension);

    switch(dimension)
    {
    case D3D11_RESOURCE_DIMENSION_TEXTURE1D:
        hr = ShowTexture1D(reinterpret_cast<ID3D11Texture1D *>(pResource), mipLevel, arraySlice, format);
        break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE2D:
        hr = ShowTexture2D(reinterpret_cast<ID3D11Texture2D *>(pResource), mipLevel, arraySlice, format);
        break;
    case D3D11_RESOURCE_DIMENSION_TEXTURE3D:
        hr = ShowTexture3D(reinterpret_cast<ID3D11Texture3D *>(pResource), depthSlice, mipLevel, arraySlice, format);
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
HRESULT CResourceViewerWindowImpl::ShowTexture1D(_In_ ID3D11Texture1D *pTexture1D, UINT mipLevel, UINT arraySlice, DXGI_FORMAT format)
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

    return ShowSubresource(pTexture1D, subresource, &box, format);
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::ShowTexture2D()
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindowImpl::ShowTexture2D(_In_ ID3D11Texture2D *pTexture2D, UINT mipLevel, UINT arraySlice, DXGI_FORMAT format)
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

    return ShowSubresource(pTexture2D, subresource, &box, format);
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::ShowTexture3D
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindowImpl::ShowTexture3D(_In_ ID3D11Texture3D *pTexture3D, UINT depthSlice, UINT mipLevel, UINT arraySlice, DXGI_FORMAT format)
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

    return ShowSubresource(pTexture3D, subresource, &box, format);
}

////////////////////////////////////////////////////////////////////////////////
// CResourceViewerWindow::ShowSubresource
////////////////////////////////////////////////////////////////////////////////
HRESULT CResourceViewerWindowImpl::ShowSubresource(_In_ ID3D11Resource *pResource, UINT subresource, D3D11_BOX *pBox, DXGI_FORMAT format)
{
    HRESULT hr = S_OK;
    CHandle<ID3D11DeviceContext1> pContext;
    CHandle<ID3D11Texture2D> pStagedCopy;
    CHandle<ID3D11Texture2D> pFormattedCopy;

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

    // Create the formatted copy texture to match back buffer
    D3D11_TEXTURE2D_DESC formattedCopyDesc = {};

    formattedCopyDesc.MipLevels = 1;
    formattedCopyDesc.ArraySize = 1;
    formattedCopyDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
    formattedCopyDesc.SampleDesc.Count = 1;
    formattedCopyDesc.SampleDesc.Quality = 0;

    formattedCopyDesc.BindFlags = 0;
    formattedCopyDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
    formattedCopyDesc.Usage = D3D11_USAGE_STAGING;

    // Formatted copy dimensions must match subresource dimensions
    formattedCopyDesc.Width = subresourceWidth;
    formattedCopyDesc.Height = subresourceHeight;

    hr = m_pDevice->CreateTexture2D(&formattedCopyDesc, NULL, &pFormattedCopy);
    if(FAILED(hr))
        return hr;

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

    hr = D3DX11SaveTextureToFileW(pContext,
                                  pFormattedCopy,
                                  D3DX11_IFF_BMP,
                                  m_filename.c_str());

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

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::~CResourceViewer()
////////////////////////////////////////////////////////////////////////////
CResourceViewer::~CResourceViewer()
{
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
    m_pDevice = nullptr;
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::DestroyAll()
//
// Destroy all CResourceViewerWindow objects
////////////////////////////////////////////////////////////////////////////
void CResourceViewer::DestroyAll() { m_pimpl->DestroyAll(); }
void CResourceViewerImpl::DestroyAll()
{
    m_resourceViewerWindows.clear();
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::HideAll()
//
// Hide all CResourceViewerWindow objects
////////////////////////////////////////////////////////////////////////////
void CResourceViewer::HideAll() { m_pimpl->HideAll(); }
void CResourceViewerImpl::HideAll()
{
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
    auto it = m_resourceViewerWindows.find(name);

    if(it != m_resourceViewerWindows.end())
    {
        pWindow = it->second.get();
    }

    return pWindow;
}

////////////////////////////////////////////////////////////////////////////
// CResourceViewer::NewResourceViewerWindow()
//
// Create a new resource viewer (initially hidden) and assign the name provided
////////////////////////////////////////////////////////////////////////////
CResourceViewerWindow *CResourceViewerImpl::NewResourceViewerWindow(_In_ PCWSTR name)
{
    std::unique_ptr<CResourceViewerWindow> pWindow = nullptr;

    // Only create a resource viewer window if this has been Setup
    if(m_pDevice)
    {
        // Create the CResourceViewerWindow object
        pWindow.reset(new CResourceViewerWindow());
        if(FAILED(pWindow->Create(name, m_pDevice)))
        {
            return nullptr;
        }

        return m_resourceViewerWindows.emplace(std::make_pair(name, std::move(pWindow))).first->second.get();
    }

    return nullptr;
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
            HRESULT hr = pWindow->ShowResource(
                pResource,
                mipLevel,
                arraySlice,
                depthSlice,
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
