#pragma once

#include <Windows.h>
#include <d3d11_1.h>
#include <memory>

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
	CResourceViewerWindow(void);
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
		_In_opt_ HWND hWndParent, 
		_In_ ID3D11Device1 *pDevice,
		DWORD windowStyle = WS_OVERLAPPED,
		DWORD extendedWindowStyle = 0,
		DXGI_FORMAT swapChainFormat = DXGI_FORMAT_R8G8B8A8_UNORM
		);

	////////////////////////////////////////////////////////////////////////////
	// Destroy()
	// Destroys the window and releases all memory associated with this
	////////////////////////////////////////////////////////////////////////////
	void Destroy();

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
		float resizeScale = 1.0f,
		DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN
		);

	////////////////////////////////////////////////////////////////////////////
	// GetWindow()
	//
	// Returns the window handle
	////////////////////////////////////////////////////////////////////////////
	HWND GetWindowHandle() const;

	////////////////////////////////////////////////////////////////////////////
	// SetTitle()
	//
	// Sets the title text for the ResourceViewerWindow window
	////////////////////////////////////////////////////////////////////////////
	void SetTitle(_In_ PCWSTR title);

	////////////////////////////////////////////////////////////////////////////
	// ShowWindow()
	//
	// Shows the window associated with this
	////////////////////////////////////////////////////////////////////////////
	void ShowWindow();

	////////////////////////////////////////////////////////////////////////////
	// HideWindow()
	//
	// Hides the window associated with this
	////////////////////////////////////////////////////////////////////////////
	void HideWindow();

	////////////////////////////////////////////////////////////////////////////
	// Present()
	//
	// Have the ResourceViewerWindow window present
	////////////////////////////////////////////////////////////////////////////
	HRESULT Present();

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

	// Destroy CResourceViewerWindow object by name
	void DestroyWindow(_In_ PCWSTR name);

	// Destroy CResourceViewerWindow object by pointer
	void DestroyWindow(_In_ CResourceViewerWindow *pResourceViewerWindow);

	// Destroy all CResourceViewerWindow objects
	void DestroyAll();

	// Hide CResourceViewerWindow object by name
	void HideWindow(_In_ PCWSTR name);

	// Hide all CResourceViewerWindow objects
	void HideAll();

	// Create a new resource viewer (initially hidden) and assign the name provided
	CResourceViewerWindow *NewResourceViewerWindow(_In_ PCWSTR name);

	// Returns pointer to CResourceViewerWindow object with the specified name.
	// If no window exists the method resturns nullptr
	CResourceViewerWindow *FindResourceViewerWindow(_In_ PCWSTR name) const;

	PCWSTR GetResourceViewerWindowName(_In_ CResourceViewerWindow *pWindow) const;

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