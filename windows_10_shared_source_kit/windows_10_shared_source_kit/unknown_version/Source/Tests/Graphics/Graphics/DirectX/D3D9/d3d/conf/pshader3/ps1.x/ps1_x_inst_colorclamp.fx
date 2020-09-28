#include "include.fx"

Technique
<
	String	Name = "Color clamp red channel, positive";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.5,	1.0,	0.5,	1.0
                                def	c5,	1.0,	0.0,	0.0,	1.0
        
        			mov     r0, c5
                                add     r0, r0, c5
                                add     r0, r0, c5
                                
                                m4x4	oPos,	v0,     c0
                                mov     oD0,    r0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     -0.5,	0.0,	0.0,	1.0
                        
                        mov     r1, v0
                        
                        add	r0,	r1,	c0      // r0 = ( 0, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Color clamp red channel, negative";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.5,	1.0,	0.5,	1.0
                                def	c5,	-1.0,	0.0,	0.0,	1.0
        
        			mov     r0, c5
                                add     r0, r0, c5
                                add     r0, r0, c5
                                
                                m4x4	oPos,	v0,     c0
                                mov     oD0,    r0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.5,	0.0,	0.0,	1.0
                        
                        mov     r1, v0
                        
                        add	r0,	r1,	c0      // r0 = ( 0, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Color clamp green channel, positive";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.5,	1.0,	0.5,	1.0
                                def	c5,	0.0,	1.0,	0.0,	1.0
        
        			mov     r0, c5
                                add     r0, r0, c5
                                add     r0, r0, c5
                                
                                m4x4	oPos,	v0,     c0
                                mov     oD0,    r0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	-0.5,	0.0,	1.0
                        
                        mov     r1, v0
                        
                        add	r0,	r1,	c0      // r0 = ( 0, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Color clamp green channel, negative";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.5,	1.0,	0.5,	1.0
                                def	c5,	0.0,	-1.0,	0.0,	1.0
        
        			mov     r0, c5
                                add     r0, r0, c5
                                add     r0, r0, c5
                                
                                m4x4	oPos,	v0,     c0
                                mov     oD0,    r0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	0.5,	0.0,	1.0
                        
                        mov     r1, v0
                        
                        add	r0,	r1,	c0      // r0 = ( 0, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Color clamp blue channel, positive";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.5,	1.0,	0.5,	1.0
                                def	c5,	0.0,	0.0,	1.0,	1.0
        
        			mov     r0, c5
                                add     r0, r0, c5
                                add     r0, r0, c5
                                
                                m4x4	oPos,	v0,     c0
                                mov     oD0,    r0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	0.0,	-0.5,	1.0
                        
                        mov     r1, v0
                        
                        add	r0,	r1,	c0      // r0 = ( 0, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Color clamp blue channel, negative";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.5,	1.0,	0.5,	1.0
                                def	c5,	0.0,	0.0,	-1.0,	1.0
        
        			mov     r0, c5
                                add     r0, r0, c5
                                add     r0, r0, c5
                                
                                m4x4	oPos,	v0,     c0
                                mov     oD0,    r0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	0.0,	0.5,	1.0
                        
                        mov     r1, v0
                        
                        add	r0,	r1,	c0      // r0 = ( 0, 1, 1, 1)
		};
	}
}
