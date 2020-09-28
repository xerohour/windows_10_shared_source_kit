$(OBJ_PATH)\$(O)\SimplePixelShader.cso:
    fxc.exe /Tps_4_0 /Fo"$(OBJ_PATH)\$(O)\SimplePixelShader.cso" /E main SimplePixelShader.hlsl

$(OBJ_PATH)\$(O)\SimpleVertexShader.cso:
    fxc.exe /Tvs_4_0 /Fo"$(OBJ_PATH)\$(O)\SimpleVertexShader.cso" /E main SimpleVertexShader.hlsl

