#include "include.fx"

Technique 
<
	String	Name = "Inst: cmp r0, c2, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, c0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		c0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, c1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r2,		c2
			
			cmp			r0,		r2,		r0,		c1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -c2, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		-c2,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2.x, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2.x,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2.xyxx, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2.xyxx,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -c2.x, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		-2.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		-c2.x,	r0,		r1			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -c2.xyxx, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		-2.0,	-2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		-c2.xyxx,	r0,		r1			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -r2, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		-r2,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: cmp r0, r2.x, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2.x,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique
<
	String	Name = "Inst: cmp r0, r2.xyxx, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2.xyxx,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -r2.x, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		-2.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		-r2.x,	r0,		r1			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -r2.xyxx, r0, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		-2.0,	-2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		-r2.xyxx,	r0,		r1			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, -c0, r1";
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
			ps_2_x
			def			c0,		-1.0,	-1.0,	-1.0,	-1.0	// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		-c0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, c0.x, r1";
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
			ps_2_x
			def			c0,		1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		c0.x,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, c0.xyxx, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		c0.xyxx,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, -c0.x, r1";
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
			ps_2_x
			def			c0,		-1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		-c0.x,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, -c0.xyxx, r1";
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
			ps_2_x
			def			c0,		-1.0,	-1.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		-c0.xyxx,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, -r0, r1";
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
			ps_2_x
			def			c0,		-1.0,	-1.0,	-1.0,	-1.0	// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		-r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, r0.x, r1";
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
			ps_2_x
			def			c0,		1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		r0.x,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, r0.xyxx, r1";
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
			ps_2_x
			def			c0,		1.0,	1.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		r0.xyxx,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, -r0.x, r1";
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
			ps_2_x
			def			c0,		-1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		-r0.x,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, -r0.xyxx, r1";
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
			ps_2_x
			def			c0,		-1.0,	-1.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		-r0.xyxx,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, -c1";
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
			ps_2_x
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	-1.0,	-1.0,	-1.0	// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		-c1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, r0, c1.x";
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
			ps_2_x
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		1.0,	0.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		c1.x			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, c1.xyxx";
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
			ps_2_x
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		1.0,	1.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		c1.xyxx			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, -c1.x";
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
			ps_2_x
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	0.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		-c1.x			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, -c1.xyxx";
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
			ps_2_x
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	-1.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		-c1.xyxx			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, r0, -r1";
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
			ps_2_x
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	-1.0,	-1.0,	-1.0	// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		r0,		-r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, r0, r1.x";
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
			ps_2_x
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		1.0,	0.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		r0,		r1.x			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, r0, r1.xyxx";
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
			ps_2_x
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		1.0,	1.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		r0,		r1.xyxx			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, r0, -r1.x";
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
			ps_2_x
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	0.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		r0,		-r1.x			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, c2, r0, -r1.xyxx";
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
			ps_2_x
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	-1.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		c2,		r0,		-r1.xyxx			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp_sat r0, r2, r0, r1 > 1.f";
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
			ps_2_x
			def			c0,		2.0,	2.0,	2.0,	1.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0
			def			c3,		1.0,	1.0,	1.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp_sat		r0,		r2,		r0,		r1				// r0 = ( 1, 1, 1, 1 ); this is the test pivot
			sub			r0,		r0,		c3						// r0 = ( 0, 0, 0, 0 )
			cmp			r0,		r0,		c3,		r0				// pass or fail, yo

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp_sat r0, r2, r0, r1 < 0.f";
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
			ps_2_x
			def			c0,		-2.0,	-2.0,	-2.0,	-1.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0
			def			c3,		1.0,	1.0,	1.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp_sat		r0,		r2,		r0,		r1				// r0 = ( 0, 0, 0, 0 ); this is the test pivot
			cmp			r0,		r0,		c3,		r0				// pass or fail, yo
			
			mov			r0.w,	c1.w							// set the alpha either way

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r3.x, r2, r0, r1";
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
			ps_2_x
			def			c0,		-2.0,	-2.0,	-2.0,	-2.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-2.0,	0.0,	0.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp			r3.x,	r2,		r0,		r1				// r3 = ( -2, x, x, x ); this is the test pivot
			mov			r3.yzw,	c1								// r3 = ( -2, 0, 0, 1 )
			sub			r3,		r3,		c4						// r3 = ( 0, 0, 0, 0 )
			cmp			r1,		r3,		c3,		r0				// r1 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			cmp			r2,		-r3,	c3,		r0				// r2 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			add_sat		r0,		r1,		r2						// r0 = ( 1, 1, 1, 1 ) or ( 0, 0, 0, 0 )
			
			mov			r0.w,	c1.w							// set the alpha either way

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp_sat r3.x, r2, r0, r1";
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
			ps_2_x
			def			c0,		2.0,	2.0,	2.0,	2.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-2.0,	0.0,	0.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2
			mov			r4,		c4.x

			cmp_sat		r3.x,	r2,		r0,		r1				// r3 = ( 1, x, x, x ); this is the test pivot
			mov			r3.yzw,	c1.x							// r3 = ( 1, 0, 0, 0 )
			sub			r3.x,	r3,		c3.x					// r3 = ( 0, 0, 0, 0 )
			cmp			r1,		r3,		c3,		r4				// r1 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			cmp			r2,		-r3,	c3,		r4				// r2 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			add_sat		r0,		r1,		r2						// r0 = ( 1, 1, 1, 1 ) or ( 0, 0, 0, 0 )
			
			mov			r0.w,	c1.w							// set the alpha either way

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp_sat r3.x, r2, r0, r1";
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
			ps_2_x
			def			c0,		-2.0,	-2.0,	-2.0,	-2.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-2.0,	0.0,	0.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			cmp_sat		r3.x,	r2,		r0,		r1				// r3 = ( 0, x, x, x ); this is the test pivot
			mov			r3.yzw,	c1.x							// r3 = ( 0, 0, 0, 0 )
			cmp			r1,		r3,		c3,		r0				// r1 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			cmp			r2,		-r3,	c3,		r0				// r2 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			add_sat		r0,		r1,		r2						// r0 = ( 1, 1, 1, 1 ) or ( 0, 0, 0, 0 )
			
			mov			r0.w,	c1.w							// set the alpha either way

			mov			oC0,	r0
		};
	}
}

// pred

Technique 
<
	String	Name = "Inst: (p0) cmp r3, r2, r0, r1";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {-2.0,	-2.0,	-2.0,	-2.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x
			//def			c0,		-2.0,	-2.0,	-2.0,	-2.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c2,		0.0,	2.0,	7.125,	1.0
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-2.0,	0.0,	0.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2
			
			setp_eq		p0.x,	r0,		c0
			setp_ne		p0.yzw,	r0,		c0						// p0 = ( t, f, f, f )

	(p0)	cmp			r3,		r2,		r0,		r1				// r3 = ( -2, x, x, x ); this is the test pivot
			mov			r3.yzw,	c1								// r3 = ( -2, 0, 0, 1 )
			sub			r3,		r3,		c4						// r3 = ( 0, 0, 0, 0 )
			cmp			r1,		r3,		c3,		r0				// r1 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			cmp			r2,		-r3,	c3,		r0				// r2 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			add_sat		r0,		r1,		r2						// r0 = ( 1, 1, 1, 1 ) or ( 0, 0, 0, 0 )
			
			mov			r0.w,	c1.w							// set the alpha either way

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: (!p0) cmp r3, r2, r0, r1";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {-2.0,	-2.0,	-2.0,	-2.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x
			//def			c0,		-2.0,	-2.0,	-2.0,	-2.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c2,		0.0,	2.0,	7.125,	1.0
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-2.0,	0.0,	0.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			setp_eq		p0.yzw,	r0,		c0
			setp_ne		p0.x,	r0,		c0						// p0 = ( f, t, t, t )

	(!p0)	cmp			r3,		r2,		r0,		r1				// r3 = ( -2, x, x, x ); this is the test pivot
			mov			r3.yzw,	c1								// r3 = ( -2, 0, 0, 1 )
			sub			r3,		r3,		c4						// r3 = ( 0, 0, 0, 0 )
			cmp			r1,		r3,		c3,		r0				// r1 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			cmp			r2,		-r3,	c3,		r0				// r2 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			add_sat		r0,		r1,		r2						// r0 = ( 1, 1, 1, 1 ) or ( 0, 0, 0, 0 )
			
			mov			r0.w,	c1.w							// set the alpha either way

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: (p0.y) cmp r1, r2, r0, r1";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {1.0,	1.0,	1.0,	1.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x
			//def			c0,		1.0,	1.0,	1.0,	1.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			setp_eq		p0.yzw,	r0,		c0
			setp_ne		p0.x,	r0,		c0						// p0 = ( f, t, t, t )

	(p0.y)	cmp			r1,		r2,		r0,		r1				// r3 = ( 1, 1, 1, 1 ); this is the test pivot

			mov			oC0,	r1
		};
	}
}

Technique 
<
	String	Name = "Inst: (!p0.x) cmp r1, r2, r0, r1";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {1.0,	1.0,	1.0,	1.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x
			//def			c0,		1.0,	1.0,	1.0,	1.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			setp_eq		p0.yzw,	r0,		c0
			setp_ne		p0.x,	r0,		c0						// p0 = ( f, t, t, t )

	(!p0.x)	cmp			r1,		r2,		r0,		r1				// r3 = ( 1, 1, 1, 1 ); this is the test pivot

			mov			oC0,	r1
		};
	}
}

Technique 
<
	String	Name = "Inst: (p0) cmp r3.xyw, r2, r0, r1";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {-2.0,	-2.0,	-2.0,	-2.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x
			//def			c0,		-2.0,	-2.0,	-2.0,	-2.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c2,		0.0,	2.0,	7.125,	1.0
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-2.0,	0.0,	0.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2
			
			setp_eq		p0.xz,	r0,		c0
			setp_ne		p0.yw,	r0,		c0						// p0 = ( t, f, t, f )

	(p0)	cmp			r3.xyw,	r2,		r0,		r1				// r3 = ( -2, x, x, x ); this is the test pivot
			mov			r3.yzw,	c1								// r3 = ( -2, 0, 0, 1 )
			sub			r3,		r3,		c4						// r3 = ( 0, 0, 0, 0 )
			cmp			r1,		r3,		c3,		r0				// r1 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			cmp			r2,		-r3,	c3,		r0				// r2 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			add_sat		r0,		r1,		r2						// r0 = ( 1, 1, 1, 1 ) or ( 0, 0, 0, 0 )
			
			mov			r0.w,	c1.w							// set the alpha either way

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: (p0) cmp_sat r3.xzw, r2, r0, r1";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {-2.0,	-2.0,	-2.0,	-2.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x
			//def			c0,		-2.0,	-2.0,	-2.0,	-2.0
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c2,		0.0,	2.0,	7.125,	1.0
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-2.0,	0.0,	0.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			mov			r2,		c2

			setp_eq		p0.xz,	r0,		c0
			setp_ne		p0.yw,	r0,		c0						// p0 = ( t, f, t, f )

	(p0)	cmp_sat		r3.xzw,	r2,		r0,		r1				// r3 = ( 0, x, x, x ); this is the test pivot
			mov			r3.yzw,	c1.x							// r3 = ( 0, 0, 0, 0 )
			cmp			r1,		r3,		c3,		r0				// r1 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			cmp			r2,		-r3,	c3,		r0				// r2 = ( 1, 1, 1, 1 ) or ( -2, -2, -2, -2 )
			add_sat		r0,		r1,		r2						// r0 = ( 1, 1, 1, 1 ) or ( 0, 0, 0, 0 )
			
			mov			r0.w,	c1.w							// set the alpha either way

			mov			oC0,	r0
		};
	}
}

// t# tests

Technique 
<
	String	Name = "Inst: cmp r0, t0, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	2.0,	7.125,	1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0

			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		t0,		r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, t0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	1.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0

			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		t0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, t0";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	0.0,	0.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r2,		c2
			
			cmp			r0,		r2,		r0,		t0				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -t0, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	-2.0,	-7.125,	-1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		-t0,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, t0.x, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	-2.0,	-7.125,	-1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		t0.x,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, t0.xyxx, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	2.0,	-7.125,	-1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		t0.xyxx,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -t0.x, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		-2.0,	2.0,	7.125,	1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		-2.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		-t0.x,	r0,		r1			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -t0.xyxx, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		-2.0,	-2.0,	7.125,	1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		-2.0,	-2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		-t0.xyxx,	r0,		r1			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, -t0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		-1.0,	-1.0,	-1.0,	-1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		-1.0,	-1.0,	-1.0,	-1.0	// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		-t0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, t0.x, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		t0.x,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, t0.xyxx, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	1.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		t0.xyxx,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, -t0.x, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		-1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		-1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		-t0.x,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, -t0.xyxx, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		-1.0,	-1.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		-1.0,	-1.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		-t0.xyxx,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, -t0";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		-1.0,	-1.0,	-1.0,	-1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	-1.0,	-1.0,	-1.0	// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		-t0				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, t0.x";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		1.0,	0.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		t0.x			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, t0.xyxx";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		1.0,	1.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		t0.xyxx			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, -t0.x";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		-1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	0.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		-t0.x			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, -t0.xyxx";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		-1.0,	-1.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			t0
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	-1.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		-t0.xyxx			// set pass or fail

			mov			oC0,	r0
		};
	}
}

// v# tests

Technique 
<
	String	Name = "Inst: cmp r0, v0, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	2.0,	7.125,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0

			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		v0,		r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, v0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	1.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0

			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		v0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, v0";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	0.0,	0.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r2,		c2
			
			cmp			r0,		r2,		r0,		v0				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -v0, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	2.0,	7.125,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		0.0,	0.0,	0.0,	1.0
			def			c1,		1.0,	1.0,	1.0,	1.0

			def			c2,		0.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		-v0,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, v0.x, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	2.0,	7.125,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		v0.x,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, v0.xyxx, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		0.0,	2.0,	7.125,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		1.0,	1.0,	1.0,	1.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		v0.xyxx,	r0,		r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -v0.x, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		2.0,	2.0,	7.125,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		0.0,	0.0,	0.0,	1.0
			def			c1,		1.0,	1.0,	1.0,	1.0

			def			c2,		-2.0,	2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		-v0.x,	r0,		r1			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, -v0.xyxx, r0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		2.0,	2.0,	7.125,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		0.0,	0.0,	0.0,	1.0
			def			c1,		1.0,	1.0,	1.0,	1.0

			def			c2,		-2.0,	-2.0,	7.125,	1.0

			mov			r0,		c0
			mov			r1,		c1

			cmp			r0,		-v0.xyxx,	r0,		r1

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, -v0, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	1.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		-1.0,	-1.0,	-1.0,	-1.0	// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		-v0,		r1			// r0 = ( -1, -1, -1, -1 )
			mul			r0,		r0,		c0.x

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, v0.x, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		v0.x,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, v0.xyxx, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		1.0,	1.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		v0.xyxx,	r1				// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, -v0.x, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		-1.0,	0.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		-v0.x,	r1				// r0 = ( -1, -1, -1, -1 )
			mul			r0,		r0,		c0.x

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, -v0.xyxx, r1";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		-1.0,	-1.0,	0.0,	0.0		// pass color
			def			c1,		0.0,	0.0,	0.0,	1.0		// failure color

			def			c2,		0.0,	2.0,	7.125,	1.0

			mov			r1,		c1
			mov			r2,		c2

			cmp			r0,		r2,		-v0.xyxx,	r1				// r0 = ( -1, -1, -1, -1 )
			mul			r0,		r0,		c0.x

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, -v0";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	1.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	-1.0,	-1.0,	-1.0	// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		-v0				// r0 = ( -1, -1, -1, -1 )
			mul			r0,		r0,		c1.x					// r0 = - r0

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, v0.x";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		1.0,	0.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		v0.x			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, v0.xyxx";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		1.0,	1.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		v0.xyxx			// set pass or fail

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, -v0.x";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	0.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		-v0.x			// r0 = ( -1, -1, -1, -1 )
			mul			r0,		r0,		c1.x					// r0 = - r0

			mov			oC0,	r0
		};
	}
}

Technique 
<
	String	Name = "Inst: cmp r0, r2, r0, -v0.xyxx";
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
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		1.0,	1.0,	0.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
		};

		PixelShader =
		asm
		{
			ps_2_x
			dcl			v0
			
			def			c0,		0.0,	0.0,	0.0,	1.0		// failure color
			def			c1,		-1.0,	-1.0,	0.0,	0.0		// pass color

			def			c2,		-2.0,	-2.0,	-7.125,	-1.0

			mov			r0,		c0
			mov			r2,		c2

			cmp			r0,		r2,		r0,		-v0.xyxx			// r0 = ( -1, -1, -1, -1 )
			mul			r0,		r0,		c1.x						// r0 = - r0

			mov			oC0,	r0
		};
	}
}
