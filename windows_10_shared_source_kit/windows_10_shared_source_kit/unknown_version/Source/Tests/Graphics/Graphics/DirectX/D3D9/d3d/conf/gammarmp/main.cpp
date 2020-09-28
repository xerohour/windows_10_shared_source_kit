#include <windows.h>

#include"d3dalias.h"

#include <stdio.h>

#include <ctype.h>

#include "dxfmt.h"

#include "resource.h"

// Class for Gamma Ramp controls.
class clsGradient
{
private:
    const char *szLabel;
    RECT rectLocation;
    long lLeftIntercept, lRightIntercept;
    WORD *arrayGammaFunction;
    // Points to the WORDs in the D3DGAMMARAMP structure that represent the function
    // manipulated by this control.
                                
public:
    clsGradient();
    ~clsGradient();
    void Init(const char * const szAssignLabel, WORD *arrayTargetFunction);
    void SetLinear(HWND hwnd, const int iInterceptRadius);
    void SetGammaFunction();
    void SetPos(const long lLeft, const long lTop, const long lRight, const long lBottom);
    void Paint(HDC hdc, const int iInterceptRadius);
    bool CheckLeftInterceptClick(const long lMouseX, const long lMouseY, const int iInterceptRadius);
    bool CheckRightInterceptClick(const long lMouseX, const long lMouseY, const int iInterceptRadius);
    void MoveLeftIntercept(HWND hwnd, const int iInterceptRadius, const long lNewYCoordinate);
    void MoveRightIntercept(HWND hwnd, const int iInterceptRadius, const long lNewYCoordinate);
};

typedef enum {msNotDragging, msDraggingLeft, msDraggingRight} MOUSE_ACTION;

typedef
struct
{
    MOUSE_ACTION mouseaction;
    clsGradient *pgradientDragged;
}
MOUSE_STATE;

static char szMainFocusWindowClassName[] = "MainFocusWindowClass";

static HINSTANCE hinstProg;
static HWND      hwndMainFocusWindow = NULL;

static LPDIRECT3D            lpd3d = NULL;
static D3DPRESENT_PARAMETERS d3dppMainSwapChain;
static LPDIRECT3DDEVICE      lpd3ddevice=NULL;

static const int iNoOfVerticalGridUnits = 64;
static const int iBorderWidthGridUnits = 1;
static const int iControlSideGridUnits = (iNoOfVerticalGridUnits - 4) / 3;
static int iInterceptRadius;

static D3DGAMMARAMP ddgammaramp;

static clsGradient gradRed;
static clsGradient gradGreen;
static clsGradient gradBlue;

static MOUSE_STATE mousestate;

D3DFORMAT g_d3dfmtMode = D3DFMT_UNKNOWN;
static char szModeDescription[255];

static BITMAPINFO *pbmi;
static BYTE *pBits;

static int cxDib, cyDib;

static int iDisplayWidth, iDisplayHeight;


//****************************************************************************
//* Function: dprintf
//* Description:
//*     For debugging use.  Output's information to a debugger.
//
//****************************************************************************
void cdecl dprintf(LPSTR szFormat,...)
{
    char ach[256];
    va_list va;

    va_start(va, szFormat);
    wvsprintf (ach,szFormat,va);

    OutputDebugString(ach);

    va_end(va);
}

clsGradient::clsGradient()
{
    rectLocation.left = rectLocation.top = rectLocation.right = rectLocation.bottom = 0;
    lLeftIntercept = lRightIntercept = 0;
}

clsGradient::~clsGradient()
{
}

void clsGradient::Init(const char * const szAssignLabel, WORD *arrayTargetFunction)
{
    szLabel = szAssignLabel;
    arrayGammaFunction = arrayTargetFunction;
}

// Adjusts the relevant portion of the D3DGAMMARAMP structure based on the positions
// of the control's intercepts.  The adjusted gamma ramp is then used to update the
// adapter's gamma settings.
void clsGradient::SetGammaFunction()
{
    int i;
    DWORD dwControlHeight = (DWORD)(rectLocation.bottom - rectLocation.top);
    // Compute a gamma value (0 -- 65535) based on the left intercept.
    WORD wFunctionLeftIntercept = (WORD)((((DWORD)(rectLocation.bottom - lLeftIntercept)) *
    65535) / dwControlHeight);
    // Compute the difference between the gamma values for the right and left intercepts.
    DWORD dwFunctionInterceptDifference = ((((DWORD)(rectLocation.bottom - lRightIntercept)) *
    65535) / dwControlHeight) - (DWORD)wFunctionLeftIntercept;
    WORD *arrayTargetFunction = arrayGammaFunction;

    // Assign gamma values (0 -- 65535) to each of the 256 WORDs representing the gamma function
    // for this control.  The points between the left and right intercepts are computed simply
    // by a linear interpolation.
    for (i = 0; i < 256; ++i)
        arrayTargetFunction[i] = (WORD)((((DWORD)i) * dwFunctionInterceptDifference) / 255) +
        wFunctionLeftIntercept;

    // Set the new gamma ramp.
    lpd3ddevice -> SetGammaRamp(0, &ddgammaramp);
}

void clsGradient::SetLinear(HWND hwnd, const int iInterceptRadius)
{
    MoveLeftIntercept(hwnd, iInterceptRadius, rectLocation.bottom);
    MoveRightIntercept(hwnd, iInterceptRadius, rectLocation.top);
}

void clsGradient::SetPos(const long lLeft, const long lTop, const long lRight, const long lBottom)
{
    rectLocation.left = lLeft;
    lRightIntercept = rectLocation.top = lTop;
    rectLocation.right = lRight;
    lLeftIntercept = rectLocation.bottom = lBottom;
}

void clsGradient::Paint(HDC hdc, const int iInterceptRadius)
{
    MoveToEx(hdc, rectLocation.left, rectLocation.top, NULL);
    LineTo(hdc, rectLocation.right, rectLocation.top);
    LineTo(hdc, rectLocation.right, rectLocation.bottom);
    LineTo(hdc, rectLocation.left, rectLocation.bottom);
    LineTo(hdc, rectLocation.left, rectLocation.top);

    DrawText(hdc, szLabel, -1, &rectLocation, DT_SINGLELINE | DT_CENTER | DT_VCENTER);

    MoveToEx(hdc, rectLocation.left, lLeftIntercept, NULL);
    LineTo(hdc, rectLocation.right, lRightIntercept);

    Ellipse(hdc, rectLocation.left - iInterceptRadius, lLeftIntercept - iInterceptRadius,
    rectLocation.left + iInterceptRadius, lLeftIntercept + iInterceptRadius);
    Ellipse(hdc, rectLocation.right - iInterceptRadius, lRightIntercept - iInterceptRadius,
    rectLocation.right + iInterceptRadius, lRightIntercept + iInterceptRadius);
}

bool clsGradient::CheckLeftInterceptClick(const long lMouseX, const long lMouseY, const int iInterceptRadius)
{
    if
    (
        (lMouseX >= rectLocation.left - iInterceptRadius) &&
        (lMouseX <= rectLocation.left + iInterceptRadius) &&
        (lMouseY >= lLeftIntercept - iInterceptRadius) &&
        (lMouseY <= lLeftIntercept + iInterceptRadius)
    )
        return true;

    return false;
}

bool clsGradient::CheckRightInterceptClick(const long lMouseX, const long lMouseY, const int iInterceptRadius)
{
    if
    (
        (lMouseX >= rectLocation.right - iInterceptRadius) &&
        (lMouseX <= rectLocation.right + iInterceptRadius) &&
        (lMouseY >= lRightIntercept - iInterceptRadius) &&
        (lMouseY <= lRightIntercept + iInterceptRadius)
    )
        return true;

    return false;
}

void clsGradient::MoveLeftIntercept(HWND hwnd, const int iInterceptRadius, long lNewYCoordinate)
{
    HDC hdc;

    hdc = GetDC(hwnd);

    SelectObject(hdc, GetStockObject(WHITE_PEN));

    MoveToEx(hdc, rectLocation.left, lLeftIntercept, NULL);
    LineTo(hdc, rectLocation.right, lRightIntercept);
    Ellipse(hdc, rectLocation.left - iInterceptRadius, lLeftIntercept - iInterceptRadius,
    rectLocation.left + iInterceptRadius, lLeftIntercept + iInterceptRadius);

    if (lNewYCoordinate < rectLocation.top)
        lNewYCoordinate = rectLocation.top;
    else
        if (lNewYCoordinate > rectLocation.bottom)
            lNewYCoordinate = rectLocation.bottom;

    lLeftIntercept = lNewYCoordinate;

    SelectObject(hdc, GetStockObject(BLACK_PEN));
    Paint(hdc, iInterceptRadius);

    ReleaseDC(hwnd, hdc);
}

void clsGradient::MoveRightIntercept(HWND hwnd, const int iInterceptRadius, long lNewYCoordinate)
{
    HDC hdc;

    hdc = GetDC(hwnd);

    SelectObject(hdc, GetStockObject(WHITE_PEN));

    MoveToEx(hdc, rectLocation.left, lLeftIntercept, NULL);
    LineTo(hdc, rectLocation.right, lRightIntercept);
    Ellipse(hdc, rectLocation.right - iInterceptRadius, lRightIntercept - iInterceptRadius,
    rectLocation.right + iInterceptRadius, lRightIntercept + iInterceptRadius);

    if (lNewYCoordinate < rectLocation.top)
        lNewYCoordinate = rectLocation.top;
    else
        if (lNewYCoordinate > rectLocation.bottom)
            lNewYCoordinate = rectLocation.bottom;

    lRightIntercept = lNewYCoordinate;

    SelectObject(hdc, GetStockObject(BLACK_PEN));
    Paint(hdc, iInterceptRadius);

    ReleaseDC(hwnd, hdc);
}

static void ReportError(const char * const lpszErrorMessage, const char * const lpszErrorCaption)
{
    MessageBox(NULL, lpszErrorMessage, lpszErrorCaption, MB_OK |MB_ICONEXCLAMATION);
}

static void ReportDDError(const char * const lpszErrorMessage, const HRESULT hresultDD)
{
    ReportError(lpszErrorMessage, "DirectDraw Error");
}

static void ReportInfo(const char * const lpszInfoMessage, const char * const lpszInfoCaption)
{
    MessageBox(NULL, lpszInfoMessage, lpszInfoCaption, MB_OK | MB_ICONINFORMATION);
}

static LRESULT CALLBACK MainWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    int iViewBitmapX, iViewBitmapY, iViewBitmapWidth, iViewBitmapHeight;

    TEXTMETRIC textmetric;
    RECT rectModeDescription;

    HDC hdc;
    PAINTSTRUCT ps;

    switch (message)
    {
    case WM_PAINT:
        InvalidateRect(hwnd, NULL, TRUE);
        hdc = BeginPaint(hwnd, &ps);

        GetTextMetrics(hdc, &textmetric);

        rectModeDescription.left = (rectModeDescription.top = 0);
        rectModeDescription.right = iDisplayWidth - 1;
        rectModeDescription.bottom = textmetric.tmHeight * 2 - 1;

        iViewBitmapHeight = iDisplayHeight - rectModeDescription.bottom - 1;
        iViewBitmapWidth = (iViewBitmapHeight * cxDib) / cyDib;
        iViewBitmapX = (iDisplayWidth - iViewBitmapWidth) / 2;
        iViewBitmapY = rectModeDescription.bottom + 1;

        DrawText(hdc, szModeDescription, -1, &rectModeDescription,
        DT_SINGLELINE | DT_CENTER | DT_VCENTER);

        gradRed.Paint(hdc, iInterceptRadius);
        gradGreen.Paint(hdc, iInterceptRadius);
        gradBlue.Paint(hdc, iInterceptRadius);

        StretchDIBits
        (
            hdc,
            iViewBitmapX,
            iViewBitmapY,
            iViewBitmapWidth,
            iViewBitmapHeight,
            0,
            0,
            cxDib,
            cyDib,
            pBits,
            pbmi,
            DIB_RGB_COLORS,
            SRCCOPY
        );

        EndPaint(hwnd, &ps);
        break;

    case WM_KEYDOWN:
        DestroyWindow(hwnd);
        break;

    case WM_RBUTTONDOWN:
        gradRed.SetLinear(hwnd, iInterceptRadius);
        gradRed.SetGammaFunction();
        gradGreen.SetLinear(hwnd, iInterceptRadius);
        gradGreen.SetGammaFunction();
        gradBlue.SetLinear(hwnd, iInterceptRadius);
        gradBlue.SetGammaFunction();
        break;
    case WM_LBUTTONDOWN:

        if (gradRed.CheckLeftInterceptClick(LOWORD(lParam), HIWORD(lParam), iInterceptRadius))
        {
            mousestate.mouseaction = msDraggingLeft;
            mousestate.pgradientDragged = &gradRed;
            break;
        }

        if (gradRed.CheckRightInterceptClick(LOWORD(lParam), HIWORD(lParam), iInterceptRadius))
        {
            mousestate.mouseaction = msDraggingRight;
            mousestate.pgradientDragged = &gradRed;
            break;
        }

        if (gradGreen.CheckLeftInterceptClick(LOWORD(lParam), HIWORD(lParam), iInterceptRadius))
        {
            mousestate.mouseaction = msDraggingLeft;
            mousestate.pgradientDragged = &gradGreen;
            break;
        }

        if (gradGreen.CheckRightInterceptClick(LOWORD(lParam), HIWORD(lParam), iInterceptRadius))
        {
            mousestate.mouseaction = msDraggingRight;
            mousestate.pgradientDragged = &gradGreen;
            break;
        }

        if (gradBlue.CheckLeftInterceptClick(LOWORD(lParam), HIWORD(lParam), iInterceptRadius))
        {
            mousestate.mouseaction = msDraggingLeft;
            mousestate.pgradientDragged = &gradBlue;
            break;
        }

        if (gradBlue.CheckRightInterceptClick(LOWORD(lParam), HIWORD(lParam), iInterceptRadius))
        {
            mousestate.mouseaction = msDraggingRight;
            mousestate.pgradientDragged = &gradBlue;
            break;
        }

        break;
    case WM_LBUTTONUP:
        if (mousestate.mouseaction != msNotDragging)
        {
            mousestate.mouseaction = msNotDragging;
            (mousestate.pgradientDragged) -> SetGammaFunction();
        }
        break;
    case WM_MOUSEMOVE:
        if (mousestate.mouseaction == msDraggingLeft)
            (mousestate.pgradientDragged) -> MoveLeftIntercept(hwnd, iInterceptRadius, HIWORD(lParam));
        else
            if (mousestate.mouseaction == msDraggingRight)
                (mousestate.pgradientDragged) -> MoveRightIntercept(hwnd, iInterceptRadius, HIWORD(lParam));
        break;
    case WM_DESTROY:

        if (lpd3ddevice)
        {
            lpd3ddevice -> Release();
            lpd3ddevice = NULL;
        }

        hwndMainFocusWindow = NULL;
        break;
    default:
        return DefWindowProc(hwnd, message, wParam, lParam);
    }

    return 0;
}

static bool InitializeD3D(HINSTANCE hInstance)
{
    /* Create the "factory". */
    if (!(lpd3d = Direct3DCreate(D3D_SDK_VERSION)))
        MessageBox(NULL, "The application could not create the Direct3D object.",
                   "Testing Aborted", MB_OK | MB_ICONERROR);
    else
    {
        WNDCLASS wndclass;
        BITMAPFILEHEADER *pbmfh;
        HRSRC FindResourceResult;
        HGLOBAL LoadResourceResult;

        hinstProg = hInstance;

        // Register a class for the main window.
        wndclass.style = CS_HREDRAW | CS_VREDRAW;
        wndclass.lpfnWndProc = (WNDPROC)(&MainWndProc);
        wndclass.cbClsExtra = 0;
        wndclass.cbWndExtra = 0;
        wndclass.hInstance = hinstProg;
        wndclass.hIcon = LoadIcon(NULL, IDI_APPLICATION);
        wndclass.hCursor = LoadCursor(NULL, IDC_ARROW);
        wndclass.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
        wndclass.lpszMenuName = NULL;
        wndclass.lpszClassName = szMainFocusWindowClassName;
        RegisterClass(&wndclass);

        FindResourceResult = FindResource(hinstProg, MAKEINTRESOURCE(IDR_DIBFILETYPE1),
        "DIBFILETYPE");
        LoadResourceResult = LoadResource(hinstProg, FindResourceResult);
        pbmfh = (BITMAPFILEHEADER *)LockResource(LoadResourceResult);

        pbmi = (BITMAPINFO *)(pbmfh + 1);
        pBits = ((BYTE *)pbmfh) + (pbmfh -> bfOffBits);

        if ((pbmi -> bmiHeader).biSize == sizeof(BITMAPCOREHEADER))
        {
            cxDib = ((BITMAPCOREHEADER *)pbmi) -> bcWidth;
            cyDib = ((BITMAPCOREHEADER *)pbmi) -> bcHeight;
        }
        else
        {
            cxDib = (pbmi -> bmiHeader).biWidth;
            cyDib = abs((pbmi -> bmiHeader).biHeight);
        }

        return true;
    }

    return false;
}

static void CleanupD3D()
{
    if (lpd3d)
    {
        lpd3d -> Release();
        lpd3d = NULL;
    }
}

static bool InitializeD3DDevice(const char *szFocusWindowTitle, UINT uiAdapterOrdinal, D3DDEVTYPE d3ddevtype, const D3DPRESENT_PARAMETERS *pd3dpresentparam)
{
    HRESULT hresult;

    //Create the main device window.
    hwndMainFocusWindow =
    CreateWindow
    (
        szMainFocusWindowClassName,
        szFocusWindowTitle,
        WS_POPUP,//WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        CW_USEDEFAULT,
        NULL,
        NULL,
        hinstProg,
        NULL
    );
    ShowWindow(hwndMainFocusWindow, SW_SHOWNORMAL);
    UpdateWindow(hwndMainFocusWindow);

    // Create the Direct3DDevice object.
    d3dppMainSwapChain = *pd3dpresentparam;

    hresult =
    lpd3d -> CreateDevice
    (
        uiAdapterOrdinal,
        d3ddevtype,
        hwndMainFocusWindow,
        D3DCREATE_SOFTWARE_VERTEXPROCESSING,
        &d3dppMainSwapChain,
        &lpd3ddevice
    );

    if (FAILED(hresult))
    {
        switch (hresult)
        {
        case D3DERR_INVALIDCALL:
            MessageBox(hwndMainFocusWindow, "D3DERR_INVALIDCALL", "Device Creation Failure", MB_OK | MB_ICONERROR);
            break;
        case D3DERR_NOTAVAILABLE:
            MessageBox(hwndMainFocusWindow, "D3DERR_NOTAVAILABLE", "Device Creation Failure", MB_OK | MB_ICONERROR);
            break;
        case D3DERR_OUTOFVIDEOMEMORY:
            MessageBox(hwndMainFocusWindow, "D3DERR_OUTOFVIDEOMEMORY", "Device Creation Failure", MB_OK | MB_ICONERROR);
            break;
        }
    }
    else
    {
        RECT rect;
        int iClientWidth, iClientHeight, iBorderWidthPixels, iControlSidePixels;

        mousestate.mouseaction = msNotDragging;

        //
        // DX9 Supports the concept of DialogBoxMode.  It's primary purpose is
        // to allow application dialogs to be displayed over fullscreen DX 
        // content.  However, GDI draw operations to the primary are also
        // visible.   This test app depends on it.   Prior to Vista.   As long
        // as you never Presented, the default buffer was the GDI Primary and
        // anything drawn was visible.   With Vista, you must call 
        // SetDialogBox mode.
        //
#if BUILD_DX9
        //
        // Set Dialog Box Mode so that GDI draws will be visible.
        // This is being added for Vista (And later) compatibility.
        //
        // Unfortunately SetDialogBoxMode will fail for 2:10:10:10 modes.
        // we'll have to skip this step, which means you can no longer
        // test 2:10:10:10 in Vista (or later) OSes.
        if (g_d3dfmtMode != D3DFMT_A2R10G10B10)
        {            
            if (FAILED(hresult = lpd3ddevice->SetDialogBoxMode(TRUE)))
            {
                //
                // Since dialogboxMode failed, we have no means of displaying an error message to the user.  
                // Log some debug spew.
                dprintf("GammaRmp:  SetDialogBoxMode(TRUE) failed with 0x%x.   All GDI draws this app makes may not be visible!\n", hresult);
                //
                // On vista this failure will result in GDI calls being drawn to a non-visible buffer.
                // however on legacy OSes, the default buffer is shared with the desktop, so this
                // failure is benign.  Let's continue and hope for the best.
                //
            }
        }
        else  // Running in 2:10:10:10 mode.  Can't set DialogBoxMode.
        {
                dprintf("GammaRmp:  Running in 2:10:10:10 mode.   SetDialogBoxMode is not supported.  All GDI draws this app makes may not be visible on Vista and later OSes!\n", hresult);
        }
#endif // End BUILD_DX9 check

        GetClientRect(hwndMainFocusWindow, &rect);
        iClientWidth = rect.right;
        iClientHeight = rect.bottom;
        iBorderWidthPixels = (iClientHeight * iBorderWidthGridUnits) / iNoOfVerticalGridUnits;
        iControlSidePixels = (iClientHeight * iControlSideGridUnits) / iNoOfVerticalGridUnits;

        iInterceptRadius = ((iBorderWidthPixels + 1) / 2) - 1;

        gradRed.Init("Red", ddgammaramp.red);
        gradGreen.Init("Green", ddgammaramp.green);
        gradBlue.Init("Blue", ddgammaramp.blue);

        gradRed.SetPos
        (
            iClientWidth - iBorderWidthPixels - iControlSidePixels,
            iBorderWidthPixels,
            iClientWidth - iBorderWidthPixels - 1,
            iBorderWidthPixels + iControlSidePixels - 1
        );
        gradRed.SetGammaFunction();

        gradGreen.SetPos
        (
            iClientWidth - iBorderWidthPixels - iControlSidePixels,
            (iBorderWidthPixels * 2) + iControlSidePixels,
            iClientWidth - iBorderWidthPixels - 1,
            (iBorderWidthPixels + iControlSidePixels) * 2 - 1
        );
        gradGreen.SetGammaFunction();

        gradBlue.SetPos
        (
            iClientWidth - iBorderWidthPixels - iControlSidePixels,
            iBorderWidthPixels * 3 + iControlSidePixels * 2,
            iClientWidth - iBorderWidthPixels - 1,
            (iBorderWidthPixels + iControlSidePixels) * 3 - 1
        );
        gradBlue.SetGammaFunction();

        iDisplayWidth = iClientWidth - (iBorderWidthPixels * 2) - iControlSidePixels;
        iDisplayHeight = iClientHeight;

        return true;
    }

    return false;
}

static void CleanupDevice()
{
    if (hwndMainFocusWindow)
        DestroyWindow(hwndMainFocusWindow);
}

int ParseCmdLine(LPSTR lpCmdLine)
{
#if BUILD_DX9
    if (strstr(lpCmdLine, "A2R10G10B10"))
        g_d3dfmtMode = D3DFMT_A2R10G10B10;
#endif

    return 0; // Might want to add error reporting later
}

UINT FindAdapterOrdinal(LPSTR lpCmdLine)
{
    UINT uiAdapterOrdinal = 0;
    const char *szHeadSelectionString = NULL;

    if ((szHeadSelectionString = strstr(lpCmdLine, "-m:")) != NULL)
    {
        const char *szIntegerSearch = szHeadSelectionString + 3;
        char chCurrent = *szIntegerSearch;

        while (isspace(chCurrent))
        {
            chCurrent = *(++szIntegerSearch);
        }

        while (isdigit(chCurrent))
        {
            uiAdapterOrdinal = uiAdapterOrdinal * 10 + (int)chCurrent - (int)'0';
            chCurrent = *(++szIntegerSearch);
        }
    }

    return uiAdapterOrdinal;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
    UINT uiAdapterOrdinalFromCmdLine;

    ParseCmdLine(lpCmdLine);
    uiAdapterOrdinalFromCmdLine = FindAdapterOrdinal(lpCmdLine);

    if (InitializeD3D(hInstance))
    {
        D3DDISPLAYMODE d3ddispmodeDesktop;
        D3DPRESENT_PARAMETERS d3dpresentparam;

        lpd3d -> GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddispmodeDesktop);

        ZeroMemory(&d3dpresentparam, sizeof(D3DPRESENT_PARAMETERS));
        d3dpresentparam.Windowed = false;
        d3dpresentparam.BackBufferWidth = d3ddispmodeDesktop.Width;
        d3dpresentparam.BackBufferHeight = d3ddispmodeDesktop.Height;
        d3dpresentparam.BackBufferFormat = ((g_d3dfmtMode != D3DFMT_UNKNOWN) ? g_d3dfmtMode : d3ddispmodeDesktop.Format);
        d3dpresentparam.FullScreen_RefreshRateInHz = d3ddispmodeDesktop.RefreshRate;
        d3dpresentparam.BackBufferCount = 1;
        d3dpresentparam.SwapEffect       = D3DSWAPEFFECT_DISCARD;
        d3dpresentparam.MultiSampleType = D3DMULTISAMPLE_NONE;
        d3dpresentparam.Flags = D3DPRESENTFLAG_LOCKABLE_BACKBUFFER;

        if (InitializeD3DDevice("Gamma Ramp Tool", uiAdapterOrdinalFromCmdLine, D3DDEVTYPE_HAL, &d3dpresentparam))
        {
            D3DCAPS d3dcaps;
            lpd3d -> GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &d3dcaps);
            if (!((d3dcaps.Caps2) & D3DCAPS2_FULLSCREENGAMMA))
                MessageBox(hwndMainFocusWindow, "The driver cannot adjust gamma ramps.", "Capability Not Supported", MB_OK | MB_ICONEXCLAMATION);

            lpd3d -> GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &d3ddispmodeDesktop);

            sprintf
            (
                szModeDescription,
                "Mode: %d x %d x %s at %dHz",
                d3ddispmodeDesktop.Width,
                d3ddispmodeDesktop.Height,
                D3DFormatName(d3ddispmodeDesktop.Format),
                d3ddispmodeDesktop.RefreshRate
            );

            do
            {
                MSG msg;

                if (lpd3ddevice -> TestCooperativeLevel() == D3DERR_DEVICENOTRESET)
                {
                    lpd3ddevice -> Reset(&d3dppMainSwapChain);
                    lpd3ddevice -> SetGammaRamp(0, &ddgammaramp);
                }

                while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
                {
                    TranslateMessage(&msg);
                    DispatchMessage(&msg);
                }
            }
            while (lpd3ddevice);
        }

        CleanupDevice();

        CleanupD3D();
    }

    return 0;
}
