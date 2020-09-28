#include "include.fx"

Technique
<
	String	Name = "ps_3_0 precison";
	String	Shape = "Precision";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        MipFilter    [0] = Point;
        MaxMipLevel  [0] = 0;
        MipMapLODBias[0] = 0;
        
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def			c0,	 0.5f,   -1.f,	 0.f,  -2.f
			
			rcp r0, v0.x
			mov r1, v0.y
			mul r2, r0, r1
			add r3, r2, c0.w
			
			mov r4, c0.z
			cmp r4.x, r3, c0.x, c0.z
			
			mov r5, c0.z
			mul r3, r3, c0.y
			cmp r5.x, r3, c0.x, c0.z
						
			mov r0, v0.z
			mov r1, v0.w
			mul r0, r0, c0.y
			add r2, r0, r1
			
			cmp r4.y, r2, c0.x, c0.z
			
			mul r2, r2, c0.y
			cmp r5.y, r2, c0.z, c0.x
			
			add r6, r4, r5
			mov oC0, r6
        };
   }
}