  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MaxMipLevel[9] = 6";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
		MipFilter[9] = Point;
        MaxMipLevel[9] = 6;
        MipMapLODBias[9] = 0;
 		CullMode = None;               
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_2_0
			dcl t0.xy
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_2d s9
			
			mov r0.x, t0.x  mov r0.y, t0.y
			mov r0.w, c0.x
			texldp r1, r0, s9
			mov oC0, r1
        };
         
		VertexShader = <TexTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_2_0 texldp : Tex9, w = 1, MipMapLODBias[9] = 0";
	String	Shape = "TexTL";
>
{
    Pass P0
    {        
		Texture[9] = <Tex2D_0>;
 		MipFilter[9] = Point;
        MaxMipLevel[9] = 0;
        MipMapLODBias[9] = 0;
 		CullMode = None; 
        VertexShaderConstant[0] = <mFi