//-----------------------------------------------------------------------------
//  WindowTree 
//
//
//  History
//      11/23/05 - jbarr - Created 
//
//-----------------------------------------------------------------------------
#pragma once
#include <windows.h>

#include <node.h>
#include <vector>

//--------------------------------------------------------------------------------------
//
//  Macros
//
//--------------------------------------------------------------------------------------

#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif


using namespace std;

#define DEFAULT_WINDOW_WIDTH    256
#define DEFAULT_WINDOW_HEIGHT    256
    typedef struct
    {
        DWORD Style;
        DWORD RestoreStyle;

        DWORD ExStyle;
        DWORD RestoreExStyle;
        int X;
        int Y;
        int Width;
        int Height;
        COLORREF Color;
        RECT RestoreRect;
        HWND Hwnd;
        bool IsTestWindow;
        bool IsRegionWindow;
        HRGN RestoreHrgn;
        bool IsLayeredWindow;
        bool CreateDevice;
        DWORD SwapEffect;
        TCHAR Name[MAX_PATH];
    } WindowData;

class WindowTree
{
public:
    WindowTree();
    virtual ~WindowTree();

    HRESULT InsertChild(
        Node<WindowData>* parent,
        WindowData windowData,
        __out Node<WindowData>** child);

    HRESULT ResetWindowTreeRegions(Node<WindowData>* pNode);
    HRESULT MoveWindowTree(Node<WindowData>* pNode,int, int);
    HRESULT PumpMessagesWindowTree();

    Node<WindowData>* FindTestWindowInWindowTree(Node<WindowData>* pNode, bool *pFound);
    HRESULT CleanUpWindowTree();

private:

    HRESULT BuildWindowTree(HWND root, HWND testWindow, Node<WindowData>*);
    HRESULT CleanUpWindowTree(Node<WindowData>*);
    HRESULT CleanUpDevicesAndWindows(Node<WindowData>*);
    HRESULT ResetTestWindow(Node<WindowData>*);
    HRESULT ResetTestWindowPosition(Node<WindowData>*);
    HRESULT WindowTree::ResetPosition(Node<WindowData>* pNode);
    HRESULT PumpMessagesWindowTree(Node<WindowData>*);
    HRESULT InitializeNode(HWND root, Node<WindowData>* pNode);
    static LRESULT CALLBACK MyWindowProc( HWND hWnd, UINT Msg, WPARAM wParam, LPARAM lParam );

    vector<Node<WindowData>>    m_vecWindowTree;
    Node<WindowData> m_Root;
    UINT numberOfWindows;


};