.
IDI_D3DICON             ICON                    "d3d.ico"

/////////////////////////////////////////////////////////////////////////////
//
// Accelerator
//

IDR_ACCELERATOR ACCELERATORS 
BEGIN
    VK_ESCAPE,      IDM_CLOSE,              VIRTKEY, NOINVERT
    VK_F9,          IDM_CMPIMGMODE,         VIRTKEY, NOINVERT
    VK_NEXT,        IDM_EXECUTION_NEXT,     VIRTKEY, NOINVERT
    "R",            IDM_EXECUTION_REPLAY,   VIRTKEY, NOINVERT
    "g",            IDM_EXECUTION_RUN,      ASCII,  NOINVERT
    "s",            IDM_EXECUTION_STEP,     ASCII,  NOINVERT
    VK_F8,          IDM_IMAGECOMPARE,       VIRTKEY, NOINVERT
    VK_F7,          IDM_SIZETODEVICE,       VIRTKEY, NOINVERT
    VK_PRIOR,       IDM_EXECUTION_PREVIOUS, VIRTKEY, NOINVERT
END


/////////////////////////////////////////////////////////////////////////////
//
// Dialog
//

IDD_DEVICEOPTIONS DIALOG  0, 0, 229, 113
STYLE DS_SETFONT | DS_MODALFRAME | DS_NOFAILCREATE | WS_POPUP | WS_VISIBLE | 
    WS_CAPTION
CAPTION "Device Options"
FONT 8, "MS Shell Dlg"
BEGIN
    DEFPUSHBUTTON   "OK",IDOK,172,7,50,14
    PUSHBUTTON      "Cancel",IDCANCEL,172,25,50,14
    COMBOBOX        IDC_SELDEVICE,81,7,83,42,CBS_DROPDOWN | WS_VSCROLL | 
                    WS_TABSTOP
    COMBOBOX        IDC_SELVP,81,55,83,51,CBS_DROPDOWN | WS_VSCROLL | 
                    WS_TABSTOP
    LTEXT           "Vertex Processing",IDC_STATIC,17,59,58,8
    COMBOBOX        IDC_SELBEHAVIORS,81,71,83,57,CBS_DROPDOWN | WS_VSCROLL | 
                    WS_TABSTOP
    LTEXT           "Behaviors",IDC_STATIC,17,75,32,8
    COMBOBOX        IDC_SELDEVTYPE,81,39,83,50,CBS_DROPDOWN | WS_VSCROLL | 
                    WS_TABSTOP
    LTEXT           "Device Type",IDC_STATIC,17,43,42,8
    COMBOBOX        IDC_SELADAPTER,81,23,83,49,CBS_DROPDOWN | WS_VSCROLL | 
                    WS_TABSTOP
    LTEXT           "Adapter",IDC_STATIC,17,27,42,8
    CONTROL         "PSGP",IDC_PSGP,"Button",BS_AUTOCHECKBOX | WS_TABSTOP,60,
                    95,35,10
    CONTROL         "Fullscreen",IDC_FULLSCREEN,"Button",BS_AUTOCHECKBOX | 
                    WS_TABSTOP,122,95,48,10
    LTEXT           "Device",IDC_STATIC,17,11,24,8
END

IDD_COMPAREOPTIONS DIALOGEX 0, 0, 245, 158
STYLE DS_SETFONT | DS_MODALFRAME | DS_FIXEDSYS | DS_NOFAILCREATE | WS_POPUP | 
    WS_VISIBLE | WS_CAPTION | WS_SYSMENU
CAPTION "Image