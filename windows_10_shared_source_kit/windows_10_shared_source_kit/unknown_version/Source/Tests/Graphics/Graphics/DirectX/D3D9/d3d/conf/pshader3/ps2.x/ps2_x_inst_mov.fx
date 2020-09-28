#include "include.fx"


Technique
<
	String	Name = "Inst: mov: (p)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0.0,	1.0,	0.0,	1.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x

			//def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		1.0,	1.0,	1.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0

	(p0)	mov			r4,		c1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mov: (p0).x";
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
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0

			mov			r4,		c1.y		// r4 = ( 0, 0, 0, 0 )
	(p0.x)	mov			r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mov: (p0(masked))";
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
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4.xw,	c1.y
			mov			r4.yz,	c1.x			// r4 = ( 0, 1, 1, 0 )
	(p0)	mov			r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mov: (p0(masked).x)";
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
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4,		c1.y			// r4 = ( 0, 0, 0, 0 )
	(p0.x)	mov			r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mov: (!p0(masked))";
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
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4.xw,	c1.x
			mov			r4.yz,	c1.y			// r4 = ( 1, 0, 0, 1 )
	(!p0)	mov			r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mov: (!p0(masked).x)";
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
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4,		c1.y			// r4 = ( 0, 0, 0, 0 )
	(!p0.y)	mov			r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mov: (!p0(masked).x), mask";
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
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4.yz,	r1.y			// r4 = ( x, 0, 0, x )
			mov			r4.xw,	c1.x			// r4 = ( 1, 0, 0, 1 )
	(!p0.y)	mov			r4.yz,	c0

			mov			oC0,	r4
		};
	}
}

