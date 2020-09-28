#include "include.fx"

//-----------------------------------------------------------------------------
// Operate on R0, R1 register
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add (r1 = r0 + c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
			
		        add	r1,	r0,	c1      // r1 = ( 1, 1, 1, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add: neg (r1 = r0 + c0)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0

			mov	r0,	c1

	                add     r1,	r0,	c0      // r1 = (0,1,1,0)
                        mov     r1.a,   c0.a            // r1 = (0,1,1,1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add sat (r1 = sat(r0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r1,	r0,	c1      // r1 = ( 1, 1, 1, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg (r1 = sat(r0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r1,	r0,	c1      // r1 = ( 1, 1, 1, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add -r1 (r1 = r0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
                        mov	r1,	c1
			
		        add	r1,	r0,	-r1      // r1 = ( 0, 1, 0, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add neg -r1 (r1 = r0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0

			mov	r0,	c0
                        mov     r1,     c1
			
		        add	r1,	r0,	-r1      // r1 = ( 0, 1, 1, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add sat -r1 (r1 = sat(r0 + -r1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     4.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
                        mov     r1,     c1
			
		        add_sat	r1,	r0,	-r1      // r1 = ( 1, 0, 0, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg -r1 (r1 = sat(r0 + -r1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0

			mov	r0,	c0
                        mov	r1,	c1
			
		        add_sat	r1,	r0,	-r1      // r1 = ( 1, 1, 0, 1)

			mov	r0,	r1
		};
	}
}

Technique
<
	String	Name = "Inst: add (t) (r0 = t0 + c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.0,	1.0,	0.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };


		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	1.0
                        
                        tex     t0

                        add	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: add: neg (t) (r0 = t0 + c0)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	-1.0,	0.0,	0.0,	-1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };

		PixelShader =
		asm
		{
			ps_1_1

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0
                        tex     t0


	                add     r0,	t0,	c0      // r0 = (0,1,1,0)
                        mov     r0.a,   c0.a            // r0 = (0,1,1,1)
		};
	}
}

Technique
<
	String	Name = "Inst: add sat (t) (r0 = sat(t0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;                

                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.0,	1.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        tex     t0

		        add_sat	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg (t) (r0 = sat(t0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.0,	-1.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0
                        tex     t0

		        add_sat	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: add -r1 (t) (r0 = t0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	1.0,	1.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0
                        tex     t0                        

		        mov     r1,     c1
                        add	r0,	t0,	-r1      // r4 = ( 0, 1, 0, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: add neg -r1 (t) (r0 = t0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.5,	1.0,	0.5,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0
                        tex     t0                        

		        mov     r1,     c1
                        add	r0,	t0,	-r1      // r0 = ( 0, 1, 1, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: add sat -r1 (t) (r0 = sat(t0 + -r1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	1.0,	0.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };

		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     1.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        tex     t0                        

		        mov     r1,     c1
                        add_sat	r0,	t0,	-r1      // r0 = ( 1, 0, 0, 1)
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg -r1 (t) (r0 = sat(t0 + -r1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                Texture[0] = <Tex2D_0>;
                VertexShader = 
                asm
                {
        			vs_1_1
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	0.0,	1.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };
                
		PixelShader =
		asm
		{
			ps_1_1

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0
                        tex     t0                        

		        mov     r1,     c1
                        add_sat	r0,	t0,	-r1      // r0 = ( 1, 1, 0, 1)
		};
	}
}


//-----------------------------------------------------------------------------
// Operate on R2, R3 register
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add (r3 = r0 + c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
			
		        add	r3,	r0,	c1      // r3 = ( 1, 1, 1, 1)

			mov	r0,	r3
		};
	}
}

Technique
<
	String	Name = "Inst: add: neg (r3 = r0 + c0)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0

			mov	r0,	c1

	                add     r3,	r0,	c0      // r3 = (0,1,1,0)
                        mov     r3.a,   c0.a            // r3 = (0,1,1,1)

			mov	r0,	r3
		};
	}
}

Technique
<
	String	Name = "Inst: add sat (r3 = sat(r0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r3,	r0,	c1      // r3 = ( 1, 1, 1, 1)

			mov	r0,	r3
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg (r3 = sat(r0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r3,	r0,	c1      // r3 = ( 1, 1, 1, 1)

			mov	r0,	r3
		};
	}
}

Technique
<
	String	Name = "Inst: add -r3 (r3 = r0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
                        mov	r1,	c1
			
		        add	r3,	r0,	-r1      // r3 = ( 0, 1, 0, 1)

			mov	r0,	r3
		};
	}
}

Technique
<
	String	Name = "Inst: add neg -r2 (r2 = r0 + -r1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0

			mov	r0,	c0
                        mov     r2,     c1
			
		        add	r2,	r0,	-r2      // r2 = ( 0, 1, 1, 1)

			mov	r0,	r2
		};
	}
}

Technique
<
	String	Name = "Inst: add sat -r2 (r1 = sat(r0 + -r2))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     1.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
                        mov     r2,     c1
			
		        add_sat	r2,	r0,	-r2      // r2 = ( 1, 0, 0, 1)

			mov	r0,	r2
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg -r3 (r1 = sat(r0 + -r3))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_1_4

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0

			mov	r0,	c0
                        mov	r3,	c1
			
		        add_sat	r3,	r0,	-r3      // r3 = ( 1, 1, 0, 1)

			mov	r0,	r3
		};
	}
}

