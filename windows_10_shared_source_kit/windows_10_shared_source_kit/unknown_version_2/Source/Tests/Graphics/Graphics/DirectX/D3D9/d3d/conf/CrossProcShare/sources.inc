!include ..\..\conf.mk

INCLUDES=\
	$(INCLUDES); \
	..\; \
	..\CrossProcShare_Lib; \

SOURCES=\
	..\CrossProcShareTest.rc \

TARGETLIBS=\
	$(TARGETLIBS) \
	$(GRAPHICS_TST_OBJ_ROOT)\DirectX\D3D9\D3D\Conf\CrossProcShare\CrossProcShare_Lib\$O\CrossprocShare.lib \
