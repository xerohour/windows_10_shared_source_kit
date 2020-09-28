!include $(GRAPHICS_TST_ROOT)\DirectX\D3D\D3D11.mk

TARGET_DESTINATION=graphics\d3d\conf

TARGETLIBS=\
	$(TARGETLIBS) \
	$(SDK_LIB_PATH)\d3dx10_test.lib \
	$(WINDOWS_LIB_PATH)\d3dx11_test.lib \
	$(SDK_LIB_PATH)\d3d10.lib \
	