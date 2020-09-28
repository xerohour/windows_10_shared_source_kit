$(OBJ_PATH)\$(O)\CopyTexCSTiled.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /ECopyTexCSTiled -Fh$(OBJ_PATH)\$(O)\CopyTexCSTiled.hpp TiledResourcesCS.hlsl

$(OBJ_PATH)\$(O)\CopyTexCSNormal.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /ECopyTexCSNormal -Fh$(OBJ_PATH)\$(O)\CopyTexCSNormal.hpp TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessRawBufferTiled.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessRawBufferTiled -Fh$(OBJ_PATH)\$(O)\ProcessRawBufferTiled.hpp TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessStructuredBufferTiled.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessStructuredBufferTiled -Fh$(OBJ_PATH)\$(O)\ProcessStructuredBufferTiled.hpp TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessRawBufferNormal.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessRawBufferNormal -Fh$(OBJ_PATH)\$(O)\ProcessRawBufferNormal.hpp TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessStructuredBufferNormal.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessStructuredBufferNormal -Fh$(OBJ_PATH)\$(O)\ProcessStructuredBufferNormal.hpp TiledResourcesCS.hlsl

$(OBJ_PATH)\$(O)\CopyTexCSTiledTex3D.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /ECopyTexCSTiled -Fh$(OBJ_PATH)\$(O)\CopyTexCSTiledTex3D.hpp /DUSE_TEXTURE3D /Vng_CopyTexCSTiledTex3D TiledResourcesCS.hlsl

$(OBJ_PATH)\$(O)\CopyTexCSNormalTex3D.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /ECopyTexCSNormal -Fh$(OBJ_PATH)\$(O)\CopyTexCSNormalTex3D.hpp /DUSE_TEXTURE3D /Vng_CopyTexCSNormalTex3D TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessRawBufferTiledTex3D.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessRawBufferTiled -Fh$(OBJ_PATH)\$(O)\ProcessRawBufferTiledTex3D.hpp /DUSE_TEXTURE3D /Vng_ProcessRawBufferTiledTex3D TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessStructuredBufferTiledTex3D.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessStructuredBufferTiled -Fh$(OBJ_PATH)\$(O)\ProcessStructuredBufferTiledTex3D.hpp /DUSE_TEXTURE3D /Vng_ProcessStructuredBufferTiledTex3D TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessRawBufferNormalTex3D.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessRawBufferNormal -Fh$(OBJ_PATH)\$(O)\ProcessRawBufferNormalTex3D.hpp /DUSE_TEXTURE3D /Vng_ProcessRawBufferNormalTex3D TiledResourcesCS.hlsl
										 
$(OBJ_PATH)\$(O)\ProcessStructuredBufferNormalTex3D.hpp : TiledResourcesCS.hlsl
    fxc /Tcs_5_0 /EProcessStructuredBufferNormal -Fh$(OBJ_PATH)\$(O)\ProcessStructuredBufferNormalTex3D.hpp /DUSE_TEXTURE3D /Vng_ProcessStructuredBufferNormalTex3D TiledResourcesCS.hlsl

$(OBJ_PATH)\$(O)\CopyTexTier1PS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /ECopyTexTier1PS -Fh$(OBJ_PATH)\$(O)\CopyTexTier1PS.hpp TiledResourcesPS.hlsl
										 
$(OBJ_PATH)\$(O)\CopyTexGSTex3D.hpp : TiledResourcesPS.hlsl
    fxc /Tgs_5_0 /ECopyTexGSTex3D -Fh$(OBJ_PATH)\$(O)\CopyTexGSTex3D.hpp /DUSE_TEXTURE3D TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\CopyTexTier2PS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /ECopyTexTier2PS -Fh$(OBJ_PATH)\$(O)\CopyTexTier2PS.hpp TiledResourcesPS.hlsl
										 
$(OBJ_PATH)\$(O)\RenderDepthPS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /ERenderDepthPS -Fh$(OBJ_PATH)\$(O)\RenderDepthPS.hpp TiledResourcesPS.hlsl
										 
$(OBJ_PATH)\$(O)\RenderColorUsingDepthPS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /ERenderColorUsingDepthPS -Fh$(OBJ_PATH)\$(O)\RenderColorUsingDepthPS.hpp TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\CopyTexTier2PSTex3D.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /ECopyTexTier2PS -Fh$(OBJ_PATH)\$(O)\CopyTexTier2PSTex3D.hpp /DUSE_TEXTURE3D /Vng_CopyTexTier2PSTex3D TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\CopyTexVS.hpp : TiledResourcesVS.hlsl
    fxc /Tvs_5_0 /ECopyTexVS -Fh$(OBJ_PATH)\$(O)\CopyTexVS.hpp TiledResourcesVS.hlsl

$(OBJ_PATH)\$(O)\CopyTexVSTex3D.hpp : TiledResourcesVS.hlsl
    fxc /Tvs_5_0 /ECopyTexVS -Fh$(OBJ_PATH)\$(O)\CopyTexVSTex3D.hpp /DUSE_TEXTURE3D /Vng_CopyTexVSTex3D TiledResourcesVS.hlsl

$(OBJ_PATH)\$(O)\FilterNormalTier1PS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /EFilterNormalTier1PS -Fh$(OBJ_PATH)\$(O)\FilterNormalTier1PS.hpp TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\FilterTiledTier1PS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /EFilterTiledTier1PS -Fh$(OBJ_PATH)\$(O)\FilterTiledTier1PS.hpp TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\FilterNormalTier2PS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /EFilterNormalTier2PS -Fh$(OBJ_PATH)\$(O)\FilterNormalTier2PS.hpp TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\FilterTiledTier2PS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /EFilterTiledTier2PS -Fh$(OBJ_PATH)\$(O)\FilterTiledTier2PS.hpp TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\FilterNormalTier2PSTex3D.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /EFilterNormalTier2PS -Fh$(OBJ_PATH)\$(O)\FilterNormalTier2PSTex3D.hpp /DUSE_TEXTURE3D /Vng_FilterNormalTier2PSTex3D TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\FilterTiledTier2PSTex3D.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /EFilterTiledTier2PS -Fh$(OBJ_PATH)\$(O)\FilterTiledTier2PSTex3D.hpp /DUSE_TEXTURE3D /Vng_FilterTiledTier2PSTex3D TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\GatherNormalTier1PS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /EGatherNormalTier1PS -Fh$(OBJ_PATH)\$(O)\GatherNormalTier1PS.hpp TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\GatherTiledTier1PS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /EGatherTiledTier1PS -Fh$(OBJ_PATH)\$(O)\GatherTiledTier1PS.hpp TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\GatherNormalTier2PS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /EGatherNormalTier2PS -Fh$(OBJ_PATH)\$(O)\GatherNormalTier2PS.hpp TiledResourcesPS.hlsl

$(OBJ_PATH)\$(O)\GatherTiledTier2PS.hpp : TiledResourcesPS.hlsl
    fxc /Tps_5_0 /EGatherTiledTier2PS -Fh$(OBJ_PATH)\$(O)\GatherTiledTier2PS.hpp TiledResourcesPS.hlsl
