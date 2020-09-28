!include $(WINDOWS_TST_ROOT)\Graphics\DirectX\DXGI\DXGI.mk


TARGETNAME=Microsoft.DXGI.PresentDuration.Tests
TARGETTYPE=DYNLINK
DLLDEF=

TARGET_DESTINATION=Graphics\D3D\Conf


_NT_TARGET_VERSION=$(_NT_TARGET_VERSION_WINBLUE)


NT_SIGNCODE=1


#C_DEFINES=$(C_DEFINES) /DUNICODE /D_UNICODE

!if $(FREEBUILD)
!else
C_DEFINES=$(C_DEFINES) -DDEBUG
!endif


INCLUDES=\
	$(INCLUDES); \
	$(WINDOWS_TST_ROOT)\Common\SimpleLog\DXGLog; \
	$(WINDOWS_TST_ROOT)\Common\SimpleLog\TAEFLog; \
	$(WINDOWS_TST_ROOT)\Common\StringConverters; \
	$(SDKTOOLS_INC_PATH)\Detours\v3.0; \
	$(SDKTOOLS_INC_PATH)\WEXTest\CuE; \
	$(WINDOWS_INC_PATH); \


SOURCES=\
	..\CustomPresentDurationEventProcessingThread.cpp \
	..\ETWEventProcessingThread.cpp \
	..\GraphicsTAEFTest.cpp \
	..\PresentDurationTest.cpp \


TARGETLIBS=\
	$(TARGETLIBS) \
	$(WINDOWS_TST_OBJ_ROOT)\Common\SimpleLog\DXGLog\$O\SimpleDXGLog.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Common\SimpleLog\TAEFLog\$O\SimpleTAEFLog.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Common\TraceListenerThread\$O\TraceListener.lib \
	$(WINDOWS_TST_OBJ_ROOT)\Graphics\DirectX\DXGI\Common\source\DCOMPisolation\$O\DCOMPisolation.lib \
	$(SDK_LIB_PATH)\D3D11.lib \
	$(SDK_LIB_PATH)\DXGUID.lib \
	$(SDK_LIB_PATH)\TDH.lib \
	$(SDKTOOLS_LIB_PATH)\Detours\V3.0\Detours.lib \
	$(SDKTOOLS_LIB_PATH)\WexTest\CuE\TE.Common.lib \
	$(SDKTOOLS_LIB_PATH)\WexTest\CuE\Wex.Common.lib \
	$(SDKTOOLS_LIB_PATH)\WexTest\CuE\Wex.Logger.lib \
