!include $(GRAPHICS_TST_ROOT)\DirectX\DirectX.mk

USE_RTTI=1

TARGETLIBS=\
	$(TARGETLIBS) \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\Common\WGFTestFramework\D3DTest\$O\D3DTest.lib \
	$(SDK_LIB_PATH)\DXGUID.lib \

INCLUDES=\
	.\; \
	$(GRAPHICS_TST_ROOT)\DirectX\Common\WGFTestFramework\D3DTest; \
	$(INCLUDES)
