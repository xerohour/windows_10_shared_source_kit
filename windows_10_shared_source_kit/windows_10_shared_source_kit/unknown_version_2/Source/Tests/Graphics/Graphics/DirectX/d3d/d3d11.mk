!include $(GRAPHICS_TST_ROOT)\DirectX\DirectXDM.mk

USE_RTTI=1

TARGETLIBS=\
    $(TARGETLIBS) \
    $(DIRECTX_TST_OBJ_ROOT)\Common\TiledResourceHelper\$(O)\TiledResourceHelper.lib \
!ifdef DX_BETA_NAMING
    $(OBJECT_ROOT)\windows\directx\dxg\d3d11\d3dcore\linkdll\$(O)\d3d11$(DX_BETA_NAMING).lib \
!else
    $(SDK_LIB_PATH)\d3d11.lib \
!endif
    $(SDK_LIB_PATH)\DXGUID.lib \
!ifdef UNIVERSAL_WGF_TEST
    $(DIRECTX_TST_OBJ_ROOT)\common\ResourceViewerNoShell\$O\ResourceViewer.lib \
!else
    $(DIRECTX_TST_OBJ_ROOT)\common\ResourceViewer\$O\ResourceViewer.lib \
!endif

INCLUDES=\
    $(GRAPHICS_TST_ROOT)\DirectX\tools\D3D11ProxyDriver; \
    $(INCLUDES) 
