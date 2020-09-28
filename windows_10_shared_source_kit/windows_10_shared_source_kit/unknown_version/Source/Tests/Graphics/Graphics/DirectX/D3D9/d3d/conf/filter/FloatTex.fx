
texture Tex0;

Technique FloatTextureScale
< 
	String	Name = "Scale float texture";
>
{
    Pass P0
    {        
        Texture[0] = <Tex0>;
		       
        PixelShader =
        asm
        {
		ps_2_0
		dcl t0.xy
                dcl_2d  s0
                def c1,         0.0625, 0.0625, 0.0625, 0.0625
			
		texld   r0, t0, s0
                mul     r0, r0, c1
		mov     oC0, r0
        };
   }
}

Technique FloatTextureScaleNegative
< 
	String	Name = "Scale float texture (negative)";
>
{
    Pass P0
    {        
        Texture[0] = <Tex0>;
		       
        PixelShader =
        asm
        {
		ps_2_0
		dcl t0.xy
                dcl_2d  s0
                def c1,         0.0625, 0.0625, 0.0625, 0.0625

		texld   r0, t0, s0
                mul     r0, r0, c1
                mov     r0, -r0
                mov     oC0, r0
        };
   }
}
