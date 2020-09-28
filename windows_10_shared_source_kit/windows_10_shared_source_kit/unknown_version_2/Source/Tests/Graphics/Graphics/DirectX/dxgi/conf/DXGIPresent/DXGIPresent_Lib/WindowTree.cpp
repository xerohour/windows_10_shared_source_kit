#include <WindowTree.h>
#include <TCHAR.h>
#include <D3d9.h>
#include <StrSafe.h>




//-----------------------------------------------------------------------------
//
//  Constructor/Destructor
//
//-----------------------------------------------------------------------------
WindowTree::WindowTree() :
numberOfWindows(0)
{
}

WindowTree::~WindowTree()
{

    CleanUpWindowTree(&m_Root);
    m_vecWindowTree.clear();
}

//////////////////////////////////////////////////////////////////////
// Name:	InsertChild
//////////////////////////////////////////////////////////////////////
HRESULT WindowTree::InsertChild(Node<WindowData>* parent, WindowData windowData, __out Node<WindowData>** child)
{
    HRESULT hResult = S_OK;
    Node <WindowData> localChild;


    localChild.Data.Style = windowData.Style;
    localChild.Data.ExStyle = windowData.ExStyle;
    localChild.Data.IsTestWindow =windowData.IsTestWindow;
    localChild.Data.IsRegionWindow =windowData.IsRegionWindow;
    localChild.Data.IsLayeredWindow =windowData.IsLayeredWindow;
    localChild.Data.CreateDevice = windowData.CreateDevice;
    localChild.Data.SwapEffect = windowData.SwapEffect;
    localChild.Data.Hwnd = windowData.Hwnd;
    localChild.Data.X = windowData.X;
    localChild.Data.Y = windowData.Y;
    localChild.Data.Color = windowData.Color;
    localChild.Data.Width = windowData.Width;
    localChild.Data.Height = windowData.Height;

    if (parent == NULL)
    {
        hResult = InitializeNode(NULL, &localChild);
        if (SUCCEEDED(hResult))
        {
            m_Root.Children.push_back(localChild);
            UINT current = (m_Root.Children.size()) -1;
            *child = &m_Root.Children[current];
        }
    }
    else
    {
        hResult = InitializeNode(parent->Data.Hwnd, &localChild);
        if (SUCCEEDED(hResult))
        {
            parent->Children.push_back(localChild); 
            UINT current = (parent->Children.size()) -1;
            *child = &parent->Children[current];
        }
    }

    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name:	InitializeNode
//////////////////////////////////////////////////////////////////////
HRESULT WindowTree::InitializeNode(HWND root, Node<WindowData>* pNode)
{
    HRESULT hResult = S_OK;
    if (pNode->Data.IsTestWindow)
    {
        pNode->Data.RestoreStyle = GetWindowLong(pNode->Data.Hwnd, GWL_STYLE);
        if (pNode->Data.RestoreStyle == 0)
        {
            DWORD e = GetLastError();
            hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
        }

        if (SUCCEEDED(hResult))
        {
            if (!SetWindowLong(pNode->Data.Hwnd, GWL_STYLE, pNode->Data.Style))
            {
                DWORD e = GetLastError();
                hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
            }
        } 

        if (SUCCEEDED(hResult))
        {
            pNode->Data.RestoreExStyle = GetWindowLong(pNode->Data.Hwnd, GWL_EXSTYLE);

            // SetWindowLong:
            // If the function succeeds, the return value is the previous value of the specified 32-bit integer.
            // If the function fails, the return value is zero. To get extended error information, call GetLastError. 
            // If the previous value of the specified 32-bit integer is zero, and the function succeeds, the return value is zero, but the function does not clear the last error information. This makes it difficult to determine success or failure. To deal with this, you should clear the last error information by calling SetLastError(0) before calling SetWindowLong. Then, function failure will be indicated by a return value of zero and a GetLastError result that is nonzero.

            if (pNode->Data.RestoreExStyle != 0 )
            {
                if (!SetWindowLong(pNode->Data.Hwnd, GWL_EXSTYLE, pNode->Data.ExStyle))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }
        }

        if (SUCCEEDED(hResult))
        {
            if (!GetWindowRect( pNode->Data.Hwnd, &pNode->Data.RestoreRect))
            {
                DWORD e = GetLastError();
                hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
            }
        }

        if (SUCCEEDED(hResult) && pNode->Data.Style & WS_CHILD)
        {
            if (SUCCEEDED(hResult))
            {
                if (!SetParent(pNode->Data.Hwnd, root))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                }
            }

            if (SUCCEEDED(hResult))
            {
                //
                // Move to (0,0) in parents
                //
                if (!SetWindowPos( pNode->Data.Hwnd, NULL, 0, 0, 0, 0, SWP_NOSIZE))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
                }
            }
        }
    }
    else
    {
        RECT rect = { 0 };

        if ( pNode->Data.Style & WS_CHILD)
        {
            rect.left = 0;
            rect.top = 0;
        }
        else
        {
            rect.left = pNode->Data.X;
            rect.top = pNode->Data.Y;
        }

        StringCchPrintf(
            pNode->Data.Name,
            ARRAYSIZE( pNode->Data.Name),
            _T( "WindowTree_%u"),
            numberOfWindows);

        // create hwnds
        // Register the windows class for the clipping windows
        WNDCLASS wndClass = 
        {
            0, 
            MyWindowProc,
            0, 
            0, 
            NULL, 
            NULL, 
            LoadCursor( NULL, IDC_ARROW),
            CreateSolidBrush(pNode->Data.Color), 
            NULL,
            pNode->Data.Name 
        };
        if ( 0 == RegisterClass( &wndClass))
        {
            DWORD dwErr = GetLastError();
            if ( (dwErr != 0) && (dwErr != ERROR_CLASS_ALREADY_EXISTS))
            {
                hResult = E_FAIL;
            }
        } 

        numberOfWindows++;


        if (SUCCEEDED(hResult))
        {
            pNode->Data.Hwnd = CreateWindowEx(
                pNode->Data.ExStyle,
                pNode->Data.Name, 
                pNode->Data.Name,
                pNode->Data.Style,
                rect.left,
                rect.top,
                pNode->Data.Width,
                pNode->Data.Height,
                root,
                0L,
                0L, 
                0L); 

            if (pNode->Data.Hwnd == NULL)
            {
                DWORD e = GetLastError();
                hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
            }
        }
    }

    //
    // Apply the window region
    //
    if (SUCCEEDED(hResult) && pNode->Data.IsRegionWindow)
    {
        RECT rect = { 0 };
        if (SUCCEEDED(hResult))
        {
            if (!GetClientRect( pNode->Data.Hwnd, &rect))
            {
                DWORD e = GetLastError();
                hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
            }
        }

        HRGN hrgn = NULL;
        if (SUCCEEDED(hResult))
        {
            pNode->Data.RestoreHrgn = NULL;
            GetWindowRgn(pNode->Data.Hwnd, pNode->Data.RestoreHrgn);

            hrgn = CreateEllipticRgn(20, 20, rect.right - 20, rect.bottom - 20);
            if (hrgn == NULL)
            {
                DWORD e = GetLastError();
                hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
            }
        }

        if (SUCCEEDED(hResult))
        {
            if (!SetWindowRgn(pNode->Data.Hwnd, hrgn, true))
            {
                DWORD e = GetLastError();
                hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
            }
        }
    }

    if (SUCCEEDED(hResult) && pNode->Data.CreateDevice)
    {
        hResult = E_NOTIMPL;
        //
        // Render some content to the window
        //
        if (SUCCEEDED(hResult))
        {
            // pNode->Data.pDevice->Clear( 0L, NULL, D3DCLEAR_TARGET, 0x0000ff, 1.0f, 0L );
            // pNode->Data.pDevice->Present(NULL,NULL,NULL,NULL);
        }
    } 

    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name:	ResetWindowTreeRegions
//////////////////////////////////////////////////////////////////////
HRESULT WindowTree::ResetWindowTreeRegions(Node<WindowData>* pNode)
{
    return E_NOTIMPL;
}

//////////////////////////////////////////////////////////////////////
// Name:	MoveWindowTree
//////////////////////////////////////////////////////////////////////
HRESULT WindowTree::MoveWindowTree(
                                   Node<WindowData>* pNode,
                                   int x,
                                   int y)
{

    HRESULT hResult = S_OK;
    for(UINT i = 0; i < pNode->Children.size(); i++)
    {
        if (!SetWindowPos( pNode->Children[i].Data.Hwnd, NULL, x, y, 0, 0, SWP_NOZORDER | SWP_NOSIZE))
        {
            DWORD e = GetLastError();
            hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e);
        }

        if(SUCCEEDED(hResult))
        {
            hResult = MoveWindowTree(&pNode->Children[i], x, y);
        }

        if(FAILED(hResult))
        {
            break;
        }

    }

    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name:	FindTestWindowInWindowTree
//////////////////////////////////////////////////////////////////////
Node<WindowData>* WindowTree::FindTestWindowInWindowTree(
    Node<WindowData>* pNode,
    bool *pFound)
{

    Node<WindowData>* returnNode = NULL;
    for(UINT i = 0; i < pNode->Children.size(); i++)
    {
        if(pNode->Children[i].Data.IsTestWindow)
        {
            returnNode = &pNode->Children[i];
            *pFound = true;
        }

        if(*pFound == false)
        {
            returnNode = FindTestWindowInWindowTree(&pNode->Children[i], pFound);
        }
        else
        {
            break;
        }
    }
    return returnNode;
}

//////////////////////////////////////////////////////////////////////
// Name:	PumpMessagesWindowTree
//////////////////////////////////////////////////////////////////////
HRESULT WindowTree::PumpMessagesWindowTree()
{
    return PumpMessagesWindowTree(&m_Root);
}

//////////////////////////////////////////////////////////////////////
// Name:	PumpMessagesWindowTree
//////////////////////////////////////////////////////////////////////
HRESULT WindowTree::PumpMessagesWindowTree(
    Node<WindowData>* pNode)
{

    HRESULT hResult = S_OK;
    for(UINT i = 0; i < pNode->Children.size(); i++)
    {
        MSG msg;
        while( ::PeekMessage( &msg, pNode->Children[i].Data.Hwnd, 0, 0, PM_NOREMOVE ) && msg.message != WM_QUIT )
        {
            if( ::PeekMessage( &msg, pNode->Children[i].Data.Hwnd, 0, 0, PM_REMOVE ) )
            {
                ::TranslateMessage( &msg );
                ::DispatchMessage( &msg );
            }
        }

        if(SUCCEEDED(hResult))
        {
            hResult = PumpMessagesWindowTree(&pNode->Children[i]);
        }

        if(FAILED(hResult))
        {
            break;
        }

    }

    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name:	CleanUpWindowTree
//////////////////////////////////////////////////////////////////////
HRESULT WindowTree::CleanUpWindowTree()
{
    return CleanUpWindowTree(&m_Root);
}

//////////////////////////////////////////////////////////////////////
// Name:	CleanUpWindowTree
//////////////////////////////////////////////////////////////////////
HRESULT WindowTree::CleanUpWindowTree(Node<WindowData>* pNode)
{

    HRESULT hresult = S_OK;
    HRESULT tempHresult = S_OK;
    //
    // Reset the TestWindow
    //
    tempHresult = ResetTestWindow(pNode);
    if(FAILED(tempHresult))
    {
        hresult = tempHresult;    
    }

    //
    // Restore the testwindow's position
    //
    tempHresult = ResetPosition(pNode);
    if(FAILED(tempHresult))
    {
        hresult = tempHresult;    
    }

    //
    // Clean up devices and destroy windows 
    //
    hresult = CleanUpDevicesAndWindows(pNode);
    if(FAILED(tempHresult))
    {
        hresult = tempHresult;    
    }

    return hresult;
}

//////////////////////////////////////////////////////////////////////
// Name:	ResetTestWindow
// 
// Reset the style and reparent to the desktop
//
//////////////////////////////////////////////////////////////////////
HRESULT WindowTree::ResetTestWindow(Node<WindowData>* pNode)
{  
    HRESULT hResult = S_OK;

    for(UINT i = 0; i < pNode->Children.size(); i++)
    {
        if(pNode->Children[i].Data.IsTestWindow)
        {
            if(SUCCEEDED(hResult))
            {
                if(!SetParent(pNode->Children[i].Data.Hwnd, NULL))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }

            if(SUCCEEDED(hResult))
            {
                if(!SetWindowLong(pNode->Children[i].Data.Hwnd, GWL_STYLE, pNode->Children[i].Data.RestoreStyle))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }

            if(SUCCEEDED(hResult))
            {
                if(!SetWindowLong(pNode->Children[i].Data.Hwnd, GWL_EXSTYLE, pNode->Children[i].Data.RestoreExStyle))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }

            if(SUCCEEDED(hResult))
            {
                if(!SetWindowRgn(pNode->Children[i].Data.Hwnd, pNode->Children[i].Data.RestoreHrgn, true))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }

        }

        if(SUCCEEDED(hResult))
        {
            hResult = ResetTestWindow(&pNode->Children[i]);
        }

        if(FAILED(hResult))
        {
            break;
        }
    }

    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name:	CleanUpDevicesAndWindows
//////////////////////////////////////////////////////////////////////
HRESULT WindowTree::CleanUpDevicesAndWindows(Node<WindowData>* pNode)
{  

    for(UINT i = 0; i < pNode->Children.size(); i++)
    {
        if(pNode->Children[i].Data.CreateDevice)
        {           
            //SAFE_RELEASE(pNode->Children[i].Data.pDevice);
            // pNode->Children[i].Data.pDevice = NULL;
        }

        if(!pNode->Children[i].Data.IsTestWindow)
        {
            DestroyWindow(pNode->Children[i].Data.Hwnd);
            pNode->Children[i].Data.Hwnd = NULL;
            UnregisterClass( 
                pNode->Children[i].Data.Name, 
                NULL); 
        }

        CleanUpDevicesAndWindows(&pNode->Children[i]);
    }

    return S_OK;
}

//////////////////////////////////////////////////////////////////////
// Name:	ResetPosition
//
// Try to get the test window to move back to its original postion.
// InvalidateRect, UpdateWindow and Message pumping may not be 
//  necessary
//
//////////////////////////////////////////////////////////////////////
HRESULT WindowTree::ResetPosition(Node<WindowData>* pNode)
{    
    HRESULT hResult = S_OK;

    for(UINT i = 0; i < pNode->Children.size(); i++)
    {
        if(pNode->Children[i].Data.IsTestWindow)
        {
            RECT rect = pNode->Children[i].Data.RestoreRect;
            if(!MoveWindow( pNode->Children[i].Data.Hwnd,
                rect.left,
                rect.top,
                rect.right - rect.left,
                rect.bottom - rect.top,
                TRUE ))
            {
                DWORD e = GetLastError();
                hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
            }
            if(SUCCEEDED(hResult))
            {
                if(!InvalidateRect( pNode->Children[i].Data.Hwnd, NULL, TRUE ))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }

            if(SUCCEEDED(hResult))
            {
                if(!UpdateWindow(  pNode->Children[i].Data.Hwnd ))
                {
                    DWORD e = GetLastError();
                    hResult = (e == ERROR_SUCCESS) ? E_FAIL : HRESULT_FROM_WIN32(e); 
                }
            }

            if(SUCCEEDED(hResult))
            {
                MSG msg;
                while( ::PeekMessage( &msg, pNode->Children[i].Data.Hwnd, 0, 0, PM_NOREMOVE ) && msg.message != WM_QUIT )
                {
                    if( ::PeekMessage( &msg, pNode->Children[i].Data.Hwnd, 0, 0, PM_REMOVE ) )
                    {
                        ::TranslateMessage( &msg );
                        ::DispatchMessage( &msg );
                    }
                }
            }
        }

        if(SUCCEEDED(hResult))
        {
            hResult = ResetPosition(&pNode->Children[i]);
        }

        if(FAILED(hResult))
        {
            break;
        }
    }

    return hResult;
}

//////////////////////////////////////////////////////////////////////
// Name: MyWindowProc
// Desc: window message processing function, used to prevent screensavers
//////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WindowTree::MyWindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam )
{
    switch( Msg )
    {
    case WM_SYSCOMMAND:
        switch( wParam )
        {
        case SC_SCREENSAVE:
            return FALSE;
        }
        break;
    case WM_ERASEBKGND:
        break;
    }

    return DefWindowProc( hWnd, Msg, wParam, lParam );
}
