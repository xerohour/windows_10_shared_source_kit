# common.mk - Common sources file for all classes in the common directory

!include $(GRAPHICS_TST_ROOT)\DirectX\DirectX.mk

TARGETTYPE=LIBRARY


USE_MSVCRT=1

USE_NATIVE_EH=1

USE_STL=1
STL_VER=100

TEST_CODE=1
MSC_WARNING_LEVEL=/W3 /WX

INCLUDES=\
	.\; \
	$(DIRECTX_TST_ROOT)\Common\Include; \
	$(DIRECTX_TST_ROOT)\Common\Regex; \
	$(DIRECTX_TST_ROOT)\Common\TraceListenerThread; \
	$(INCLUDES)

