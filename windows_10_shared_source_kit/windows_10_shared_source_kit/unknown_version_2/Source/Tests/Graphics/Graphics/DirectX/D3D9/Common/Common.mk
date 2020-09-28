# Common.mk - Build settings for D3D9 common code

!include $(GRAPHICS_TST_ROOT)\DirectX\D3D9\D3D9.mk

TARGETTYPE=LIBRARY


USE_NATIVE_EH=1

USE_STL=1
STL_VER=100


INCLUDES=\
	.\; \
	$(WINDOWS_TST_ROOT)\Common\TraceListenerThread; \
	$(GRAPHICS_TST_ROOT)\DirectX\D3D9\Common\Include; \
	$(GRAPHICS_TST_ROOT)\DirectX\Common\D3D9TestFramework\Include; \
	$(INCLUDES)

