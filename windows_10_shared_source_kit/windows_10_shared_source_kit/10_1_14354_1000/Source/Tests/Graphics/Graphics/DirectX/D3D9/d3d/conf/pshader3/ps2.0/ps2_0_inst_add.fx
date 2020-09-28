#include "include.fx"

//-----------------------------------------------------------------------------
// Operate on R register
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add (r4 = r0 + c1)";
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
			ps_2_0

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
			
		        add	r4,	r0,	c1      // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add: neg (r4 = r0 + c0)";
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
			ps_2_0

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0

			mov	r0,	c1

	                add     r4,	r0,	c0      // r4 = (0,1,1,0)
                        mov     r4.a,   c0.a            // r4 = (0,1,1,1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add sat (r4 = sat(r0 + c1))";
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r4,	r0,	c1      // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg (r4 = sat(r0 + c1))";
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
			ps_2_0

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r4,	r0,	c1      // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add -c1 (r4 = r0 + -c1)";
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
			ps_2_0

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
			
		        add	r4,	r0,	-c1      // r4 = ( 0, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add neg -c1 (r4 = r0 + -c1)";
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
			ps_2_0

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0

			mov	r0,	c0
			
		        add	r4,	r0,	-c1      // r4 = ( 0, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add sat -c1 (r4 = sat(r0 + -c1))";
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
			ps_2_0

			def     c0,     4.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r4,	r0,	-c1      // r4 = ( 1, 0, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg -c1 (r4 = sat(r0 + -c1))";
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r4,	r0,	-c1      // r4 = ( 1, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}

//-----------------------------------------------------------------------------
// Operate on R register Mask
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add masked (r4 = r0 + c0)";
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
			ps_2_0

			def     c0,     0.0,	0.0,	0.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	1.0

			mov	r0,	c1              // r0 = (1,1,1,1)
			
		        add	r4.r,	r0.r,	c0.r    // r4 = ( 1, 0, 0, 0)
                        add	r4.g,	r0.g,	c0.g    // r4 = ( 1, 1, 0, 0)
                        add	r4.b,	r0.b,	c0.b    // r4 = ( 1, 1, 1, 0)
                        add	r4.a,	r0.a,	c0.a    // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add: neg masked (r4 = r0 + c0)";
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
			ps_2_0

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0

			mov	r0,	c1

	                add     r4.r,	r0.r,	c0.r    // r4 = (0,0,0,0)
                        add     r4.g,	r0.g,	c0.g    // r4 = (0,1,0,0)
                        add     r4.b,	r0.b,	c0.b    // r4 = (0,1,1,0)
                        mov     r4.a,   c0.a            // r4 = (0,1,1,1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add sat masked (r4 = sat(r0 + c1))";
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r4,	r0,	c1      // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg masked (r4 = sat(r0 + c1))";
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
			ps_2_0

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r4.r,	r0.r,	c1.r      // r4 = ( 1, 0, 0, 0)
                        add_sat	r4.g,	r0.g,	c1.g      // r4 = ( 1, 1, 0, 0)
                        add_sat	r4.b,	r0.b,	c1.b      // r4 = ( 1, 1, 1, 0)
                        add_sat	r4.a,	r0.a,	c1.a      // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add -c1 masked (r4 = r0 + -c1)";
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
			ps_2_0

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
			
		        add	r4.r,	r0.r,	-c1.r      // r4 = ( 0, 0, 0, 0)
                        add	r4.g,	r0.g,	-c1.g      // r4 = ( 0, 1, 0, 0)
                        add	r4.b,	r0.b,	-c1.b      // r4 = ( 0, 1, 0, 0)
                        add	r4.a,	r0.a,	-c1.a      // r4 = ( 0, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add neg -c1 masked (r4 = r0 + -c1)";
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
			ps_2_0

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0

			mov	r0,	c0
			
		        add	r4.r,	r0.r,	-c1.r      // r4 = ( 0, 0, 0, 0)
                        add	r4.g,	r0.g,	-c1.g      // r4 = ( 0, 1, 0, 0)
                        add	r4.b,	r0.b,	-c1.b      // r4 = ( 0, 1, 1, 0)
                        add	r4.a,	r0.a,	-c1.a      // r4 = ( 0, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add sat -c1 masked (r4 = sat(r0 + -c1))";
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
			ps_2_0

			def     c0,     4.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r4.r,	r0.r,	-c1.r      // r4 = ( 1, 0, 0, 0)
                        add_sat	r4.g,	r0.g,	-c1.g      // r4 = ( 1, 0, 0, 0)
                        add_sat	r4.b,	r0.b,	-c1.b      // r4 = ( 1, 0, 0, 0)
                        add_sat	r4.a,	r0.a,	-c1.a      // r4 = ( 1, 0, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg -c1 (r4 = sat(r0 + -c1))";
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat	r4.r,	r0.r,	-c1.r      // r4 = ( 1, 0, 0, 0)
                        add_sat	r4.g,	r0.g,	-c1.g      // r4 = ( 1, 1, 0, 0)
                        add_sat	r4.b,	r0.b,	-c1.b      // r4 = ( 1, 1, 0, 0)
                        add_sat	r4.a,	r0.a,	-c1.a      // r4 = ( 1, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}


//-----------------------------------------------------------------------------
// Operate on T register
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add (t) (r0 = t0 + c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0
                        dcl     t0

                        add	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
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
                
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0
                        dcl     t0                        


	                add     r0,	t0,	c0      // r0 = (0,1,1,0)
                        mov     r0.a,   c0.a            // r0 = (0,1,1,1)

			mov	oC0,	r0
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
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        dcl     t0

		        add_sat	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
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
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0
                        dcl     t0

		        add_sat	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add -c1 (t) (r4 = t0 + -c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0
                        dcl     t0                        

		        add	r4,	t0,	-c1      // r4 = ( 0, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add neg -c1 (t) (r0 = t0 + -c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0
                        dcl     t0                        

		        add	r0,	t0,	-c1      // r0 = ( 0, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add sat -c1 (t) (r0 = sat(t0 + -c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	4.0,	0.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };

		PixelShader =
		asm
		{
			ps_2_0

			def     c0,     4.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        dcl     t0                        

		        add_sat	r0,	t0,	-c1      // r0 = ( 1, 0, 0, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg -c1 (t) (r0 = sat(t0 + -c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0
                        dcl     t0                        

		        add_sat	r0,	t0,	-c1      // r0 = ( 1, 1, 0, 1)

			mov	oC0,	r0
		};
	}
}

//-----------------------------------------------------------------------------
// Operate on T register, masked
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add masked (t) (r0 = t0 + c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0
                        dcl     t0

                        add	r0.x,	t0.x,	c1.x      // r0 = ( 1, 0, 0, 0)
                        add	r0.y,	t0.y,	c1.y      // r0 = ( 1, 1, 0, 0)
                        add	r0.z,	t0.z,	c1.z      // r0 = ( 1, 1, 1, 0)
                        add	r0.w,	t0.w,	c1.w      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add: neg masked (t) (r0 = t0 + c0)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0
                        dcl     t0                        


	                add     r0.x,	t0.x,	c0.x    // r0 = (0,0,0,0)
                        add     r0.y,	t0.y,	c0.y    // r0 = (0,1,0,0)
                        add     r0.z,	t0.z,	c0.z    // r0 = (0,1,1,0)
                        mov     r0.a,   c0.a            // r0 = (0,1,1,1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add sat masked (t) (r0 = sat(t0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        dcl     t0

		        add_sat	r0.x,	t0.x,	c1.x      // r0 = ( 1, 0, 0, 0)
                        add_sat	r0.y,	t0.y,	c1.y      // r0 = ( 1, 1, 0, 0)
                        add_sat	r0.z,	t0.z,	c1.z      // r0 = ( 1, 1, 1, 0)
                        add_sat	r0.w,	t0.w,	c1.w      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg masked (t) (r0 = sat(t0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0
                        dcl     t0

		        add_sat	r0.r,	t0.r,	c1.x      // r0 = ( 1, 0, 0, 0)
                        add_sat	r0.g,	t0.g,	c1.y      // r0 = ( 1, 1, 0, 0)
                        add_sat	r0.b,	t0.b,	c1.z      // r0 = ( 1, 1, 1, 0)
                        add_sat	r0.w,	t0.w,	c1.a      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add -c1 masked (t) (r4 = t0 + -c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0
                        dcl     t0                        

		        add	r4.x,	t0.x,	-c1.x      // r4 = ( 0, 0, 0, 0)
                        add	r4.y,	t0.y,	-c1.y      // r4 = ( 0, 1, 0, 0)
                        add	r4.z,	t0.z,	-c1.z      // r4 = ( 0, 1, 0, 0)
                        add	r4.w,	t0.w,	-c1.w      // r4 = ( 0, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add neg masked -c1 (t) (r0 = t0 + -c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0
                        dcl     t0                        

		        add	r0.x,	t0.r,	-c1.r      // r0 = ( 0, 0, 0, 0)
                        add	r0.y,	t0.g,	-c1.g      // r0 = ( 0, 1, 0, 0)
                        add	r0.z,	t0.b,	-c1.b      // r0 = ( 0, 1, 1, 0)
                        add	r0.w,	t0.a,	-c1.a      // r0 = ( 0, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add sat masked -c1 (t) (r0 = sat(t0 + -c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	4.0,	0.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };

		PixelShader =
		asm
		{
			ps_2_0

			def     c0,     4.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        dcl     t0                        

		        add_sat	r0.r,	t0.r,	-c1.r      // r0 = ( 1, 0, 0, 0)
                        add_sat	r0.g,	t0.g,	-c1.g      // r0 = ( 1, 0, 0, 0)
                        add_sat	r0.b,	t0.b,	-c1.b      // r0 = ( 1, 0, 0, 0)
                        add_sat	r0.a,	t0.a,	-c1.a      // r0 = ( 1, 0, 0, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add sat neg masked -c1 (t) (r0 = sat(t0 + -c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0
                        dcl     t0                        

		        add_sat	r0.r,	t0.r,	-c1.r      // r0 = ( 1, 1, 0, 1)
                        add_sat	r0.g,	t0.g,	-c1.g      // r0 = ( 1, 1, 0, 1)
                        add_sat	r0.b,	t0.b,	-c1.b      // r0 = ( 1, 1, 0, 1)
                        add_sat	r0.a,	t0.a,	-c1.a      // r0 = ( 1, 1, 0, 1)

			mov	oC0,	r0
		};
	}
}


//-----------------------------------------------------------------------------
// Operate on R register
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add_pp (r4 = r0 + c1)";
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
			ps_2_0

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_pp	r4,	r0,	c1      // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp: neg (r4 = r0 + c0)";
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
			ps_2_0

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0

			mov	r0,	c1

	                add_pp     r4,	r0,	c0      // r4 = (0,1,1,0)
                        mov     r4.a,   c0.a            // r4 = (0,1,1,1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_sat_pp (r4 = sat(r0 + c1))";
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_pp_sat	r4,	r0,	c1      // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_sat_pp neg (r4 = sat(r0 + c1))";
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
			ps_2_0

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0

			mov	r0,	c0
			
		        add_pp_sat	r4,	r0,	c1      // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp -c1 (r4 = r0 + -c1)";
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
			ps_2_0

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_pp	r4,	r0,	-c1      // r4 = ( 0, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add neg -c1 (r4 = r0 + -c1)";
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
			ps_2_0

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0

			mov	r0,	c0
			
		        add	r4,	r0,	-c1      // r4 = ( 0, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_sat_pp -c1 (r4 = sat(r0 + -c1))";
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
			ps_2_0

			def     c0,     4.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat_pp	r4,	r0,	-c1      // r4 = ( 1, 0, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_sat_pp neg -c1 (r4 = sat(r0 + -c1))";
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat_pp	r4,	r0,	-c1      // r4 = ( 1, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}

//-----------------------------------------------------------------------------
// Operate on R register Mask
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add_pp masked (r4 = r0 + c0)";
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
			ps_2_0

			def     c0,     0.0,	0.0,	0.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	1.0

			mov	r0,	c1              // r0 = (1,1,1,1)
			
		        add_pp	r4.r,	r0.r,	c0.r    // r4 = ( 1, 0, 0, 0)
                        add_pp	r4.g,	r0.g,	c0.g    // r4 = ( 1, 1, 0, 0)
                        add_pp	r4.b,	r0.b,	c0.b    // r4 = ( 1, 1, 1, 0)
                        add_pp	r4.a,	r0.a,	c0.a    // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp: neg masked (r4 = r0 + c0)";
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
			ps_2_0

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0

			mov	r0,	c1

	                add_pp     r4.r,	r0.r,	c0.r    // r4 = (0,0,0,0)
                        add_pp     r4.g,	r0.g,	c0.g    // r4 = (0,1,0,0)
                        add_pp     r4.b,	r0.b,	c0.b    // r4 = (0,1,1,0)
                        mov     r4.a,   c0.a            // r4 = (0,1,1,1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp sat masked (r4 = sat(r0 + c1))";
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat_pp	r4,	r0,	c1      // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_sat_pp neg masked (r4 = sat(r0 + c1))";
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
			ps_2_0

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0

			mov	r0,	c0
			
		        add_sat_pp	r4.r,	r0.r,	c1.r      // r4 = ( 1, 0, 0, 0)
                        add_sat_pp	r4.g,	r0.g,	c1.g      // r4 = ( 1, 1, 0, 0)
                        add_sat_pp	r4.b,	r0.b,	c1.b      // r4 = ( 1, 1, 1, 0)
                        add_sat_pp	r4.a,	r0.a,	c1.a      // r4 = ( 1, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp -c1 masked (r4 = r0 + -c1)";
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
			ps_2_0

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_pp	r4.r,	r0.r,	-c1.r      // r4 = ( 0, 0, 0, 0)
                        add_pp	r4.g,	r0.g,	-c1.g      // r4 = ( 0, 1, 0, 0)
                        add_pp	r4.b,	r0.b,	-c1.b      // r4 = ( 0, 1, 0, 0)
                        add_pp	r4.a,	r0.a,	-c1.a      // r4 = ( 0, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp neg -c1 masked (r4 = r0 + -c1)";
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
			ps_2_0

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0

			mov	r0,	c0
			
		        add_pp	r4.r,	r0.r,	-c1.r      // r4 = ( 0, 0, 0, 0)
                        add_pp	r4.g,	r0.g,	-c1.g      // r4 = ( 0, 1, 0, 0)
                        add_pp	r4.b,	r0.b,	-c1.b      // r4 = ( 0, 1, 1, 0)
                        add_pp	r4.a,	r0.a,	-c1.a      // r4 = ( 0, 1, 1, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp sat -c1 masked (r4 = sat(r0 + -c1))";
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
			ps_2_0

			def     c0,     4.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat_pp	r4.r,	r0.r,	-c1.r      // r4 = ( 1, 0, 0, 0)
                        add_sat_pp	r4.g,	r0.g,	-c1.g      // r4 = ( 1, 0, 0, 0)
                        add_sat_pp	r4.b,	r0.b,	-c1.b      // r4 = ( 1, 0, 0, 0)
                        add_sat_pp	r4.a,	r0.a,	-c1.a      // r4 = ( 1, 0, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp sat neg -c1 (r4 = sat(r0 + -c1))";
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0

			mov	r0,	c0
			
		        add_sat_pp	r4.r,	r0.r,	-c1.r      // r4 = ( 1, 0, 0, 0)
                        add_sat_pp	r4.g,	r0.g,	-c1.g      // r4 = ( 1, 1, 0, 0)
                        add_sat_pp	r4.b,	r0.b,	-c1.b      // r4 = ( 1, 1, 0, 0)
                        add_sat_pp	r4.a,	r0.a,	-c1.a      // r4 = ( 1, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}


//-----------------------------------------------------------------------------
// Operate on T register
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add_pp (t) (r0 = t0 + c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0
                        dcl     t0

                        add_pp	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp: neg (t) (r0 = t0 + c0)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0
                        dcl     t0                        


	                add_pp     r0,	t0,	c0      // r0 = (0,1,1,0)
                        mov     r0.a,   c0.a            // r0 = (0,1,1,1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_sat_pp (t) (r0 = sat(t0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        dcl     t0

		        add_sat_pp	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_sat_pp neg (t) (r0 = sat(t0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0
                        dcl     t0

		        add_sat_pp	r0,	t0,	c1      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp -c1 (t) (r4 = t0 + -c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0
                        dcl     t0                        

		        add_pp	r4,	t0,	-c1      // r4 = ( 0, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp neg -c1 (t) (r0 = t0 + -c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0
                        dcl     t0                        

		        add_pp	r0,	t0,	-c1      // r0 = ( 0, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp sat -c1 (t) (r0 = sat(t0 + -c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	4.0,	0.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };

		PixelShader =
		asm
		{
			ps_2_0

			def     c0,     4.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        dcl     t0                        

		        add_sat_pp	r0,	t0,	-c1      // r0 = ( 1, 0, 0, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp sat neg -c1 (t) (r0 = sat(t0 + -c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0
                        dcl     t0                        

		        add_sat_pp	r0,	t0,	-c1      // r0 = ( 1, 1, 0, 1)

			mov	oC0,	r0
		};
	}
}

//-----------------------------------------------------------------------------
// Operate on T register, masked
//-----------------------------------------------------------------------------

Technique
<
	String	Name = "Inst: add_pp masked (t) (r0 = t0 + c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	0.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0
                        dcl     t0

                        add_pp	r0.x,	t0.x,	c1.x      // r0 = ( 1, 0, 0, 0)
                        add_pp	r0.y,	t0.y,	c1.y      // r0 = ( 1, 1, 0, 0)
                        add_pp	r0.z,	t0.z,	c1.z      // r0 = ( 1, 1, 1, 0)
                        add_pp	r0.w,	t0.w,	c1.w      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp: neg masked (t) (r0 = t0 + c0)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def	c0,	1.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	0.0,	0.0,	-1.0
                        dcl     t0                        


	                add_pp     r0.x,	t0.x,	c0.x    // r0 = (0,0,0,0)
                        add_pp     r0.y,	t0.y,	c0.y    // r0 = (0,1,0,0)
                        add_pp     r0.z,	t0.z,	c0.z    // r0 = (0,1,1,0)
                        mov     r0.a,   c0.a            // r0 = (0,1,1,1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp sat masked (t) (r0 = sat(t0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        dcl     t0

		        add_sat_pp	r0.x,	t0.x,	c1.x      // r0 = ( 1, 0, 0, 0)
                        add_sat_pp	r0.y,	t0.y,	c1.y      // r0 = ( 1, 1, 0, 0)
                        add_sat_pp	r0.z,	t0.z,	c1.z      // r0 = ( 1, 1, 1, 0)
                        add_sat_pp	r0.w,	t0.w,	c1.w      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp sat neg masked (t) (r0 = sat(t0 + c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	-1.0,	1.0,	1.0
			def	c1,	1.0,	-1.0,	-1.0,	0.0
                        dcl     t0

		        add_sat_pp	r0.r,	t0.r,	c1.x      // r0 = ( 1, 0, 0, 0)
                        add_sat_pp	r0.g,	t0.g,	c1.y      // r0 = ( 1, 1, 0, 0)
                        add_sat_pp	r0.b,	t0.b,	c1.z      // r0 = ( 1, 1, 1, 0)
                        add_sat_pp	r0.w,	t0.w,	c1.a      // r0 = ( 1, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp -c1 masked (t) (r4 = t0 + -c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     1.0,	1.0,	1.0,	1.0
			def	c1,	1.0,	0.0,	1.0,	0.0
                        dcl     t0                        

		        add_pp	r4.x,	t0.x,	-c1.x      // r4 = ( 0, 0, 0, 0)
                        add_pp	r4.y,	t0.y,	-c1.y      // r4 = ( 0, 1, 0, 0)
                        add_pp	r4.z,	t0.z,	-c1.z      // r4 = ( 0, 1, 0, 0)
                        add_pp	r4.w,	t0.w,	-c1.w      // r4 = ( 0, 1, 0, 1)

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: add_pp neg masked -c1 (t) (r0 = t0 + -c1)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.5,	1.0,	0.5,	1.0
			def	c1,	0.5,	0.0,	-0.5,	0.0
                        dcl     t0                        

		        add_pp	r0.x,	t0.r,	-c1.r      // r0 = ( 0, 0, 0, 0)
                        add_pp	r0.y,	t0.g,	-c1.g      // r0 = ( 0, 1, 0, 0)
                        add_pp	r0.z,	t0.b,	-c1.b      // r0 = ( 0, 1, 1, 0)
                        add_pp	r0.w,	t0.a,	-c1.a      // r0 = ( 0, 1, 1, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_sat_pp masked -c1 (t) (r0 = sat(t0 + -c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
        			dcl_position	v0
        			dcl_normal	v1
        			dcl_texcoord0	v2
        
        			def	c4,	4.0,	0.0,	1.0,	1.0
        
        			m4x4	oPos,	v0,     c0
        			mov	oT0,	c4
                };

		PixelShader =
		asm
		{
			ps_2_0

			def     c0,     4.0,	0.0,	1.0,	1.0
			def	c1,	1.0,	1.0,	1.0,	0.0
                        dcl     t0                        

		        add_sat_pp	r0.r,	t0.r,	-c1.r      // r0 = ( 1, 0, 0, 0)
                        add_sat_pp	r0.g,	t0.g,	-c1.g      // r0 = ( 1, 0, 0, 0)
                        add_sat_pp	r0.b,	t0.b,	-c1.b      // r0 = ( 1, 0, 0, 0)
                        add_sat_pp	r0.a,	t0.a,	-c1.a      // r0 = ( 1, 0, 0, 1)

			mov	oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: add_sat_pp neg masked -c1 (t) (r0 = sat(t0 + -c1))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
                VertexShader = 
                asm
                {
        			vs_2_0
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
			ps_2_0

			def     c0,     0.0,	1.0,	1.0,	1.0
			def	c1,	-1.0,	-1.0,	1.0,	0.0
                        dcl     t0                        

		        add_sat_pp	r0.r,	t0.r,	-c1.r      // r0 = ( 1, 1, 0, 1)
                        add_sat_pp	r0.g,	t0.g,	-c1.g      // r0 = ( 1, 1, 0, 1)
                        add_sat_pp	r0.b,	t0.b,	-c1.b      // r0 = ( 1, 1, 0, 1)
                        add_sat_pp	r0.a,	t0.a,	-c1.a      // r0 = ( 1, 1, 0, 1)

			mov	oC0,	r0
		};
	}
}

