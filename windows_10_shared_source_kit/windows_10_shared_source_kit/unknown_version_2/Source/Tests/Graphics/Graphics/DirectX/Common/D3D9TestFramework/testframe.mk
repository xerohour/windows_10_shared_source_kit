# TestFrame.mk - Common sources file for all classes in the test framework

USE_LEGACY_TEST_FRAMEWORK=1

!include $(GRAPHICS_TST_ROOT)\DirectX\DirectX.mk

TARGETTYPE=LIBRARY

INCLUDES=.\; \
         $(DIRECTX_TST_ROOT)\D3D9\Common; \
         $(WINDOWS_TST_COMMON)\Include; \
         $(DDK_INC_PATH) \
         $(INCLUDES)

TEST_CODE=1
MSC_WARNING_LEVEL=/W3 /WX