    StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_OUTOFMEMORY"));
        break;

    case DDERR_OUTOFVIDEOMEMORY :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_OUTOFVIDEOMEMORY"));
        break;

    case DDERR_PRIMARYSURFACEALREADYEXISTS :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_PRIMARYSURFACEALREADYEXISTS"));
        break;

    case DDERR_SURFACEBUSY :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_SURFACEBUSY"));
        break;

    case DDERR_SURFACELOST :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_SURFACELOST"));
        break;

    case DDERR_UNSUPPORTED :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_UNSUPPORTED"));
        break;

    case DDERR_UNSUPPORTEDFORMAT :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_UNSUPPORTED"));
        break;

    case DDERR_UNSUPPORTEDMODE :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_UNSUPPORTEDMODE"));
        break;

    case DDERR_WASSTILLDRAWING :
        StringCchPrintf(szErrorCode, MAX_PATH, _T("DDERR_UNSUPPORTED"));
        break;

    case E_NOINTERFACE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("E_NOINTERFACE"));
        break;

    case E_POINTER:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("E_POINTER"));
        break;

    case CO_E_NOTINITIALIZED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("CO_E_NOTINITIALIZED"));
        break;

    case REGDB_E_CLASSNOTREG:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("REGDB_E_CLASSNOTREG"));
        break;

    case E_HANDLE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("E_HANDLE"));
        break;

    case D3DERR_DRIVERINTERNALERROR:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("D3DERR_DRIVERINTERNALERROR"));
        break;

    case D3DERR_INVALIDCALL:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("D3DERR_INVALIDCALL"));
        break;

    case D3DERR_NOTAVAILABLE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("D3DERR_NOTAVAILABLE"));
        break;

    case D3DERR_CANNOTPROTECTCONTENT:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("D3DERR_CANNOTPROTECTCONTENT"));
        break;

    case E_UNEXPECTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("E_UNEXPECTED"));
        break;


   // OPM - PVP etc
   case ERROR_GRAPHICS_OPM_NOT_SUPPORTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_NOT_SUPPORTED"));
        break;

   case ERROR_GRAPHICS_COPP_NOT_SUPPORTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_COPP_NOT_SUPPORTED"));
        break;

   case ERROR_GRAPHICS_UAB_NOT_SUPPORTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_UAB_NOT_SUPPORTED"));
        break;

   case ERROR_GRAPHICS_OPM_INVALID_ENCRYPTED_PARAMETERS:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INVALID_ENCRYPTED_PARAMETERS"));
        break;

   case ERROR_GRAPHICS_PARAMETER_ARRAY_TOO_SMALL:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_PARAMETER_ARRAY_TOO_SMALL"));
        break;

   case ERROR_GRAPHICS_OPM_NO_VIDEO_OUTPUTS_EXIST:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_NO_VIDEO_OUTPUTS_EXIST"));
        break;

   case ERROR_GRAPHICS_NO_DISPLAY_DEVICE_CORRESPONDS_TO_NAME:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_NO_DISPLAY_DEVICE_CORRESPONDS_TO_NAME"));
        break;

   case ERROR_GRAPHICS_DISPLAY_DEVICE_NOT_ATTACHED_TO_DESKTOP:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_DISPLAY_DEVICE_NOT_ATTACHED_TO_DESKTOP"));
        break;

   case ERROR_GRAPHICS_MIRRORING_DEVICES_NOT_SUPPORTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_MIRRORING_DEVICES_NOT_SUPPORTED"));
        break;

   case ERROR_GRAPHICS_INVALID_POINTER:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_INVALID_POINTER"));
        break;

   case ERROR_GRAPHICS_OPM_INTERNAL_ERROR:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INTERNAL_ERROR"));
        break;

   case ERROR_GRAPHICS_OPM_INVALID_HANDLE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INVALID_HANDLE"));
        break;
        
   case ERROR_GRAPHICS_NO_MONITORS_CORRESPOND_TO_DISPLAY_DEVICE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_NO_MONITORS_CORRESPOND_TO_DISPLAY_DEVICE"));
        break;

   case ERROR_GRAPHICS_PVP_INVALID_CERTIFICATE_LENGTH:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_PVP_INVALID_CERTIFICATE_LENGTH"));
        break;
   
   case ERROR_GRAPHICS_OPM_SPANNING_MODE_ENABLED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_SPANNING_MODE_ENABLED"));
        break;
   
   case ERROR_GRAPHICS_OPM_THEATER_MODE_ENABLED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_THEATER_MODE_ENABLED"));
        break;
   
   case ERROR_GRAPHICS_PVP_HFS_FAILED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_PVP_HFS_FAILED"));
        break;

   case ERROR_GRAPHICS_OPM_INVALID_SRM:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INVALID_SRM"));
        break;

   case ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_HDCP:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_HDCP"));
        break;

   case ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_ACP:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_ACP"));
        break;

   case ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_CGMSA:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_OUTPUT_DOES_NOT_SUPPORT_CGMSA"));
        break;

   case ERROR_GRAPHICS_OPM_HDCP_SRM_NEVER_SET:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_HDCP_SRM_NEVER_SET"));
        break;

   case ERROR_GRAPHICS_OPM_RESOLUTION_TOO_HIGH:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_RESOLUTION_TOO_HIGH"));
        break;

   case ERROR_GRAPHICS_OPM_ALL_HDCP_HARDWARE_ALREADY_IN_USE:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_ALL_HDCP_HARDWARE_ALREADY_IN_USE"));
        break;

   case ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_NO_LONGER_EXISTS:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_NO_LONGER_EXISTS"));
        break;

   case ERROR_GRAPHICS_OPM_SESSION_TYPE_CHANGE_IN_PROGRESS:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_SESSION_TYPE_CHANGE_IN_PROGRESS"));
        break;

   case ERROR_GRAPHICS_OPM_DRIVER_INTERNAL_ERROR:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_DRIVER_INTERNAL_ERROR"));
        break;

   case ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_DOES_NOT_HAVE_COPP_SEMANTICS:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_DOES_NOT_HAVE_COPP_SEMANTICS"));
        break;

   case ERROR_GRAPHICS_OPM_INVALID_INFORMATION_REQUEST:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INVALID_INFORMATION_REQUEST"));
        break;

   case ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_DOES_NOT_HAVE_OPM_SEMANTICS:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_VIDEO_OUTPUT_DOES_NOT_HAVE_OPM_SEMANTICS"));
        break;

   case ERROR_GRAPHICS_OPM_SIGNALING_NOT_SUPPORTED:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_SIGNALING_NOT_SUPPORTED"));
        break;

   case ERROR_GRAPHICS_OPM_INVALID_CONFIGURATION_REQUEST:
        StringCchPrintf(szErrorCode, MAX_PATH, _T("ERROR_GRAPHICS_OPM_INVALID_CONFIGURATION_REQUEST"));
        break;

   default :
       //  attempt to get a message description from the system
       //  ugly solution, but works in a pinch; the error string has a \n
       //  at the end, which ruins our formatting. FormatMessage takes an
       //  escape sequence but the mechanism is very cumbersome and not
       //  platform independent (no 64 bit builds).
       DWORD dwcch = FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM, NULL, hr, 
                                    MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                    szErrorCode, MAX_PATH, NULL);

       if (0 == dwcch)
           StringCchPrintf(szErrorCode, MAX_PATH, _T("unrecognized error code (0x%x)"), hr);
       else
       {
           //  dwcch is the number of chars, not including \0
           if(MAX_PATH - dwcch > 15)
               StringCchPrintf(szErrorCode+dwcch, MAX_PATH, _T("(0x%x)"), hr);
           else
               StringCchPrintf(szErrorCode+dwcch-15, MAX_PATH, _T("(0x%x)"), hr);

       }
        
    }   //  switch

    return hr;
}   //  ErrorCodeFromHResult

ULONG
ProtTypeIdxFromType
(
    const ULONG ProtType 
)
{
    ULONG   idx = 0;

    switch (ProtType)
    {
        case OPM_PROTECTION_TYPE_HDCP:
        {
            idx = OPM_PROTTYPE_IDX_HDCP;
            break;
        }
        case OPM_PROTECTION_TYPE_ACP:  
        {
            idx = OPM_PROTTYPE_IDX_ACP;
            break;
        }
        case OPM_PROTECTION_TYPE_CGMSA:
        {
            idx = OPM_PROTTYPE_IDX_CGMSA;
            break;
        }
        case OPM_PROTECTION_TYPE_COPP_COMPATIBLE_HDCP:
        {
            idx = OPM_PROTTYPE_IDX_COPP_COMPATIBLE_HDCP;
            break;
        }
        default:
        {
            idx = OPM_PROTTYPE_IDX_Invalid;
            break;