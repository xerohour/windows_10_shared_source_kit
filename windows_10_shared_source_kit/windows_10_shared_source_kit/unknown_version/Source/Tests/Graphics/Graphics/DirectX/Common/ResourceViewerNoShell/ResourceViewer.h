#pragma once

#include <Windows.h>
#include <d3d11_1.h>
#include <memory>
#include <string>

class CResourceViewerWindowImpl;
class CResourceViewerImpl;

////////////////////////////////////////////////////////////////////////////////
//
// class CResourceViewerWindow
// 
// This class manages a Window that displays the contents of a D3D resource.  
//
// One simple use might look like:
//	{
//		myViewer.Create(parentHWnd, myD3DDevice)
//		myViewer.ShowResource(myD3DResource);
//	}
//
// It is possible to use any extended windows styles and assign the window
// as a child of an application window.  This makes this a flexible
// presentation utility for a wide variety of testing purposes.
////////////////////////////////////////////////////////////////////////////////
class CResourceViewerWindow
{
public:
    CResourceViewerWindow();
    ~CResourceViewerWindow();

    ////////////////////////////////////////////////////////////////////////////
    // Create()
    // Creates the resource viewer
    //
    //  hWndParent - Parent window handle (may be NULL)
    //	pDevice - Pointer to the ID3D11Device1 object associated with the resources being viewed
    //	windowStyle - Style of window (default is WS_OVERLAPPED)
    //	extendedWindowStyle - Extended style of window (default is 0)
    ////////////////////////////////////////////////////////////////////////////
    HRESULT Create(
        std::wstring const& filename,
        _In_ ID3D11Device1 *pDevice
        );

    ////////////////////////////////////////////////////////////////////////////
    // ShowResource()
    //
    // Presents a copy of the resource data converted to R8G8B8A8. Modifications
    // to the resource data after calling ShowResource() do not change the
    // contents of the resource viewer window.
    //
    // pResource - Pointer to the texture resource to draw
    // mipLevel - Mip level to draw
    // arraySlice - Array slice to draw
    // depthSlice - Depth slice to draw (use 0 for Texture1D and Texture2D)
    // resizeScale - Amount to scale the window as a factor of subresource size (set to 0 to prevent window resize)
    // format - Format used to interpret resource data (Use DXGI_FORMAT_UNKNOWN to let ResourceViewer decide)
    ////////////////////////////////////////////////////////////////////////////
    HRESULT ShowResource(
        _In_ ID3D11Resource *pResource, 
        UINT mipLevel = 0,
        UINT arraySlice = 0,
        UINT depthSlice = 0,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN
        );

protected:
    std::unique_ptr<CResourceViewerWindowImpl> m_pimpl;
};

////////////////////////////////////////////////////////////////////////////////
// class CResourceViewer
// 
// This class manages a collection of CResourceViewWindow objects.  This provides
// a simple way to use multiple CResourceViewerWindow objects without the need
// for complicated code to create, destroy, hide, and cleanup viewer windows.
//
// By default all CResourceViewerWindow windows use style WS_OVERLAPPED and are created without
// a parent window.  If different windows properties are needed use the CResourceViewerWindow class directly.
// Future updates to this class could provide more flexibility if needed.
////////////////////////////////////////////////////////////////////////////////
class CResourceViewer
{
public:
    CResourceViewer();
    ~CResourceViewer();

    // Initialize the CViewerManager infrastructure
    HRESULT Setup(_In_ ID3D11Device1 *pDevice);

    // Destroys all of the existing resource viewer windows and free CResourceViewerWindow container objects
    void Cleanup();

    // Destroy all CResourceViewerWindow objects
    void DestroyAll();

    // Hide all CResourceViewerWindow objects
    void HideAll();

    // Returns pointer to CResourceViewerWindow object with the specified name.
    // If no window exists the method resturns nullptr
    CResourceViewerWindow *FindResourceViewerWindow(_In_ PCWSTR name) const;

    ////////////////////////////////////////////////////////////////////////////
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
    CResourceViewerWindow *ShowResource(
        _In_ PCWSTR name,
        _In_ ID3D11Resource *pResource, 
        UINT mipLevel = 0,
        UINT arraySlice = 0,
        UINT depthSlice = 0,
        float resizeScale = 1.0f,
        DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN
        );

private:
    std::unique_ptr<CResourceViewerImpl> m_pimpl;
};