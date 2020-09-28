######################################################################
#
# Target
#
######################################################################
TARGETNAME=Microsoft.DXGI.SwapChain.Rendering.Tests
TARGETTYPE=DYNLINK
DLLDEF=

TARGET_DESTINATION=Graphics\D3D\Conf


######################################################################
#
# Package dependencies
#
######################################################################
TEST_CODE=1

WANT_UNICODE=1
C_DEFINES=$(C_DEFINES) /DUNICODE /D_UNICODE
!include $(WINDOWS_TST_ROOT)\Common\Common.mk


######################################################################
#
# Build flags
#
######################################################################
MSC_WARNING_LEVEL=/W3 /WX

_NT_TARGET_VERSION=$(_NT_TARGET_VERSION_WINBLUE)

!if $(FREEBUILD)
!else
C_DEFINES=$(C_DEFINES) -DDEBUG
!endif


USE_MSVCRT=1

USE_STL=1
STL_VER=100

USE_NATIVE_EH=1

USE_TEST_LOGGING=1


# Which library will be used for screen capture?
USE_SCREENCAPTURE=1
#USE_DWMSLICER=

!ifdef USE_SCREENCAPTURE
C_DEFINES=$(C_DEFINES) /DUSE_SCREENCAPTURE=1
!endif

!ifdef USE_DWMSLICER
C_DEFINES=$(C_DEFINES) /DUSE_DWMSLICER=1

USE_STATIC_ATL=1
ATL_VER=70
!endif


######################################################################
#
# Includes
#
######################################################################
INCLUDES=\
	$(INCLUDES); \
	$(WINDOWS_TST_ROOT)\Common\Include; \
	$(WINDOWS_TST_ROOT)\Common\SimpleLog; \
	$(WINDOWS_TST_ROOT)\Common\SimpleLog\DXGLog; \
	$(WINDOWS_TST_ROOT)\Common\SimpleLog\TAEFLog; \
	$(WINDOWS_TST_ROOT)\Common\StringConverters; \
	$(WINDOWS_TST_ROOT)\Common\StringUtils; \
	$(WINDOWS_TST_ROOT)\Common\TraceListenerThread; \
	$(WINDOWS_TST_ROOT)\Graphics\Common\SmartPtr; \
	$(WINDOWS_TST_ROOT)\Graphics\Common\DWMSlicer; \
	$(WINDOWS_TST_ROOT)\Graphics\DirectX\Common\Include; \
	$(WINDOWS_TST_ROOT)\Graphics\DirectX\DXGI\Common\Include; \
	$(SDKTOOLS_INC_PATH); \
	$(SDKTOOLS_INC_PATH)\Detours\v3.0; \
	$(SDKTOOLS_INC_PATH)\WEXTest\CuE; \
	$(COM_INC_PATH);\
	$(WINDOWS_INC_PATH); \
	$(WINDOWS_INC_PATH)\D3DPri; \

!ifdef USE_DWMSLICER
INCLUDES=\
	$(INCLUDES); \
	$(WINDOWS_TST_ROOT)\Graphics\DirectX\Common\FormatHelper;
!endif


######################################################################
#
# Sources
#
######################################################################
SOURCES=\
	..\CD3D11ClearRenderingTechnique.cpp \
	..\CD3D11ColorSwatchPatternRenderingTechnique.cpp \
	..\CD3D11RenderingTechnique.cpp \
	..\CD3D11SceneRenderer.cpp \
	..\CD3D11ShadedCubeRenderingTechnique.cpp \
	..\CD3D11TexturedPlaneRenderingTechnique.cpp \
	..\CD3D12ClearRenderingTechnique.cpp \
	..\CD3D12ColorSwatchPatternRenderingTechnique.cpp \
	..\CD3D12RenderingTechnique.cpp \
	..\CD3D12SceneRenderer.cpp \
	..\CD3D12ShadedCubeRenderingTechnique.cpp \
	..\CD3D12TexturedPlaneRenderingTechnique.cpp \
	..\CImmersiveWindowWrapper.cpp \
	..\CRenderingTechnique.cpp \
	..\CSwapChainsTest.cpp \
	..\CSystemWindowWrapper.cpp \
	..\GraphicsTAEFTest.cpp \


!ifdef USE_SCREENCAPTURE
SOURCES=\
	$(SOURCES) \
	..\CScreenCapture.cpp \

!endif


######################################################################
#
# Libraries
#
######################################################################
TARGETLIBS=\
	$(TARGETLIBS) \
	$(WINDOWS_TST_OBJ_ROOT)\Common\DebugOut\Unicode\$O\CDebugOutU.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Common\DXGLogger\Unicode\$O\DXGLoggerU.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Common\HelperFunctions\$O\HelperFunctions.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Common\SimpleLog\DXGLog\$O\SimpleDXGLog.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Common\SimpleLog\TAEFLog\$O\SimpleTAEFLog.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Common\StringConverters\Unicode\$O\StringConverters_Unicode.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Common\StringUtils\$O\StringUtils.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Common\SysInfo\Unicode\$O\CSysInfoU.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Common\TraceListenerThread\$O\TraceListener.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Graphics\DirectX\DXGI\Common\source\DCompIsolation\$O\DCompIsolation.lib \
	$(SDK_LIB_PATH)\D3D11.lib \
	$(SDK_LIB_PATH)\D3D12.lib \
	$(SDK_LIB_PATH)\D3DCompiler.lib \
	$(SDK_LIB_PATH)\D3DX10_Test.lib \
	$(SDK_LIB_PATH)\DXGI.lib \
	$(SDK_LIB_PATH)\DXGUID.lib \
	$(SDK_LIB_PATH)\SHCore.lib \
	$(SDKTOOLS_LIB_PATH)\Detours\V3.0\Detours.lib \
	$(SDKTOOLS_LIB_PATH)\WexTest\CuE\TE.Common.lib \
	$(SDKTOOLS_LIB_PATH)\WexTest\CuE\Wex.Common.lib \
	$(SDKTOOLS_LIB_PATH)\WexTest\CuE\Wex.Logger.lib \
	$(WINDOWS_LIB_PATH)\D3DX11_Test.lib \
	$(WINDOWS_LIB_PATH)\RegEx_one.lib \
	$(WINDOWS_LIB_PATH)\RegEx_two.lib \
	$(WINDOWS_LIB_PATH)\RegEx_three.lib \
	$(MINCORE_SDK_LIB_VPATH)\api-ms-win-core-winrt-l1.lib \
	$(MINCORE_SDK_LIB_VPATH)\api-ms-win-core-winrt-string-l1.lib \


!ifdef USE_DWMSLICER
TARGETLIBS=\
	$(TARGETLIBS) \
	$(WINDOWS_TST_OBJ_ROOT)\Graphics\Common\SmartPtr\$(O)\SmartPtr.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Graphics\Common\DwmSlicer\$(O)\Rad.ImagePipeline.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Graphics\DirectX\Common\FormatHelper\$(O)\FormatHelper.lib \
	$(WINDOWS_LIB_PATH)\User32p.lib \

!endif

!ifdef USE_SCREENCAPTURE
TARGETLIBS=\
	$(TARGETLIBS) \
	$(SDK_LIB_PATH)\WindowsCodecs.lib \

!endif
