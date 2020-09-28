!include ..\..\conf.mk


INCLUDES=\
	$(INCLUDES); \
	..\; \
	..\Present2_Lib; \

SOURCES=\
	..\PresentII.rc \

TARGETLIBS=\
	$(TARGETLIBS) \
        $(GRAPHICS_TST_OBJ_ROOT)\DirectX\D3D9\D3D\Conf\Present2\Present2_Lib\$O\Present2.lib \
	$(SDK_LIB_PATH)\shell32.lib \
	$(SDK_LIB_PATH)\dwmapi.lib \