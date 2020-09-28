#include "include.fx"

Technique 
<
	String	Name = "Inst: cmp, c, r, r";
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
			ps_1_2
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		c0,		r1				// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, r, c, r";
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
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		c0,		r1				// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, r, r, c";
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
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r0,		c0
			mov			r2,		c2
			
			cmp			r0,		r2,		r0,		c1				// set pass or fail

		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, r, r, r";
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
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		r0,		r1				// set pass or fail

		};
	}
}


Technique 
<
	String	Name = "Inst: cmp, c.x, r, r";
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
			ps_1_2
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.0,	0.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,	c0,		r1				// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, -r, r, r";
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
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	-0.5,	-0.75,	-1.0

			mov			r3,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		-r2,	r3,		r1				// set pass or fail
		};
	}
}

Technique
<
	String	Name = "Inst: cmp, r.x, r, r";
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
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	-0.5,	-0.75,	-1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2.x,	r0,		r1				// set pass or fail
		};
	}
}


Technique 
<
	String	Name = "Inst: cmp, -r.x, r, r";
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
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		-0.5,	0.5,	0.75,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		-r2.x,	r0,		r1			// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, r, c.x, r";
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
			def			c0,		1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		c0.x,	r1				// set pass or fail
		};
	}
}


Technique 
<
	String	Name = "Inst: cmp, c, -r, c";
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
			ps_1_2
			def			c0,		-1.0,	-1.0,	-1.0,	-1.0	// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r1,		c0

			cmp			r0,		c2,		-r1,		c0				// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, c, c, r";
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
			ps_1_2
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

                        mov			r1,		c1

			cmp			r0,		c2,		c0,	r1				// set pass or fail
		};
	}
}


Technique 
<
	String	Name = "Inst: cmp, c, -r, r";
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
			ps_1_2
			def			c0,		-1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r1,		c0

			cmp			r0,		c2,		-r1,	c1				// set pass or fail
		};
	}
}


Technique 
<
	String	Name = "Inst: cmp, r, r, c.x";
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
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		1.0,	0.0,	0.0,	0.0		// pass color

			def			c2,		-0.5,	-0.5,	-0.75,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		c1.x			// set pass or fail
		};
	}
}


Technique 
<
	String	Name = "Inst: cmp, c, r, -r";
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
			ps_1_2
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	-1.0,	-1.0,	-1.0	// pass color

			def			c2,		-0.5,	-0.5,	-0.75,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		c0,		-r1				// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, c, r, r.x";
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
			ps_1_2
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		1.0,	1.0,	1.0,	1.0		// pass color

			def			c2,		-0.5,	-0.5,	-0.75,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		c0,		r1			// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, c, r, -r.x";
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
			ps_1_2
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	-1.0,	-1.0,	-1.0		// pass color

			def			c2,		-0.5,	-0.5,	-0.75,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		c0,		-r1			// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp_sat, r, r, r, > 1.f";
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
			def			c0,		0.5,	0.5,	0.5,	1.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0
			def			c3,		1.0,	1.0,	1.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp_sat		        r0,		r2,		r0,		r1				// r0 = ( 1, 1, 1, 1 ); this is the test pivot
			sub			r0,		r0,		c3						// r0 = ( 0, 0, 0, 0 )
			cmp			r0,		r0,		c3,		r0				// pass or fail, yo
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp_sat, r, r, r, < 0.f";
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
			def			c0,		-0.5,	-0.5,	-0.5,	-1.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0
			def			c3,		1.0,	1.0,	1.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp_sat		r0,		r2,		r0,		r1				// r0 = ( 0, 0, 0, 0 ); this is the test pivot
			cmp			r0,		r0,		c3,		r0				// pass or fail, yo
			
			mov			r0.w,	c1.w							// set the alpha either way
		};
	}
}

// v# tests

Technique 
<
	String	Name = "Inst: cmp, v, c, r";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	0.5,	0.75,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_2

			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		v0,		c0,		r1				// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, r, v, r";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	1.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_4

			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		v0,		r1				// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, r, r, v";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	0.0,	0.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_4
			
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r0,		c0
			mov			r2,		c2
			
			cmp			r0,		r2,		r0,		v0				// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, -v, c, r";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	0.5,	0.75,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_2
			
			def			c0,		0.0,	0.0,	0.0,	1.0
			def			c1,		1.0,	1.0,	1.0,	1.0

			def			c2,		0.0,	-0.5,	-0.75,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		-v0,	c0,		r1				// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, v.x, r, r";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	0.5,	0.75,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_2
			
                        def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	-0.5,	-0.75,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		v0,	c0,		r1				// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, -v.x, r, r";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.5,	0.5,	0.75,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_2
			
			def			c0,		0.0,	0.0,	0.0,	1.0
			def			c1,		1.0,	1.0,	1.0,	1.0

			def			c2,		-0.5,	0.5,	0.75,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		-v0,	c0,		r1			// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, r, -v, r";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	1.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_4
			
			def			c0,		-1.0,	-1.0,	-1.0,	-1.0	// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		-v0,		r1			// r0 = ( -1, -1, -1, -1 )
			mul			r0,		r0,		c0.x
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, r, v.x, r";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_4
			
			def			c0,		1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		v0.x,	r1				// set pass or fail
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, r, -v.x, r";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_4
			
			def			c0,		-1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	0.5,	0.75,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		-v0.x,	r1				// r0 = ( -1, -1, -1, -1 )
			mul			r0,		r0,		c0.x
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, r, r, -v";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	1.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_4
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	-1.0,	-1.0,	-1.0	// pass color

			def			c2,		-0.5,	-0.5,	-0.75,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		-v0				// r0 = ( -1, -1, -1, -1 )
			mul			r0,		r0,		c1.x					// r0 = - r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp, r, r, v.x";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_4
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		1.0,	0.0,	0.0,	0.0		// pass color

			def			c2,		-0.5,	-0.5,	-0.75,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		v0.x			// set pass or fail
		};
	}
}


Technique 
<
	String	Name = "Inst: cmp, r, r, -v.x";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_1_4
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	0.0,	0.0,	0.0		// pass color

			def			c2,		-0.5,	-0.5,	-0.75,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		-v0.x			// r0 = ( -1, -1, -1, -1 )
			mul			r0,		r0,		c1.x					// r0 = - r0
		};
	}
}

