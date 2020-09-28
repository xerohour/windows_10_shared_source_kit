#include "include.fx"


Technique
<
	String	Name = "Inst: max: c, r";
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
			ps_3_0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		1.0,	0.0,	1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		c0,		r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: -c, r";
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
			ps_3_0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		-1.0,	0.0,	-1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		-c0,	r1			// r4 = ( 0, 0, 0, 0 )
			add			r4,		r4,		c0.y		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: c.x, r";
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
			ps_3_0

			def			c0,		1.0,	0.0,	0.0,	1.0
			def			c1,		0.0,	0.5,	0.5,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		c0.x,	r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: c.xyxx, r";
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
			ps_3_0

			def			c0,		1.0,	1.0,	0.0,	0.0
			def			c1,		0.0,	0.5,	0.1,	0.1

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		c0.xyxx,	r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: -c.x, r";
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
			ps_3_0

			def			c0,		1.0,	0.0,	0.0,	0.0
			def			c1,		-2.0,	0.0,	-1.0,	-2.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		-c0.x,	r1			// r4 = ( -1, 0, -1, -1 )
			sub			r4.y,	r4,		c0.x		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, c";
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
			ps_3_0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		1.0,	0.0,	1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, -c";
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
			ps_3_0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		-1.0,	0.0,	-1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		-c0			// r4 = ( 0, 0, 0, 0 )
			add			r4,		r4,		c0.y		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, c.x";
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
			ps_3_0

			def			c0,		1.0,	0.0,	0.0,	1.0
			def			c1,		0.0,	0.5,	0.5,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		c0.x

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, c.xyxx";
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
			ps_3_0

			def			c0,		1.0,	1.0,	0.0,	0.0
			def			c1,		0.0,	0.5,	0.1,	0.1

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		c0.xyxx

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, -c.x";
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
			ps_3_0

			def			c0,		1.0,	0.0,	0.0,	0.0
			def			c1,		-2.0,	0.0,	-1.0,	-2.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		-c0.x
			sub			r4.y,	r4,		c0.x		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x

			mov			oC0,	r4
		};
	}
}

//////////////////////

Technique
<
	String	Name = "Inst: max: r, r";
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
			ps_3_0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		1.0,	0.0,	1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r0,		r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: -r, r";
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
			ps_3_0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		-1.0,	0.0,	-1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		-r0,	r1			// r4 = ( 0, 0, 0, 0 )
			add			r4,		r4,		c0.y		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r.x, r";
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
			ps_3_0

			def			c0,		1.0,	0.0,	0.0,	1.0
			def			c1,		0.0,	0.5,	0.5,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r0.x,	r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r.xyxx, r";
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
			ps_3_0

			def			c0,		1.0,	1.0,	0.0,	0.0
			def			c1,		0.0,	0.5,	0.1,	0.1

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r0.xyxx,	r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: -r.x, r";
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
			ps_3_0

			def			c0,		1.0,	0.0,	0.0,	0.0
			def			c1,		-2.0,	0.0,	-1.0,	-2.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		-r0.x,	r1
			sub			r4.y,	r4,		c0.x		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, r";
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
			ps_3_0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		1.0,	0.0,	1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		r0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, -r";
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
			ps_3_0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		-1.0,	0.0,	-1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		-r0			// r4 = ( 0, 0, 0, 0 )
			add			r4,		r4,		c0.y		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, r.x";
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
			ps_3_0

			def			c0,		1.0,	0.0,	0.0,	1.0
			def			c1,		0.0,	0.5,	0.5,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		r0.x

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, r.xyxx";
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
			ps_3_0

			def			c0,		1.0,	1.0,	0.0,	0.0
			def			c1,		0.0,	0.5,	0.1,	0.1

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		r0.xyxx

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, -r.x";
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
			ps_3_0

			def			c0,		1.0,	0.0,	0.0,	0.0
			def			c1,		-2.0,	0.0,	-1.0,	-2.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		-r0.x
			sub			r4.y,	r4,		c0.x		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x

			mov			oC0,	r4
		};
	}
}

////////

Technique
<
	String	Name = "Inst: max: t, r";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_texcoord	o1
			
			def			c4,		0.0,	1.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_texcoord0	v0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		1.0,	0.0,	1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		v0,		r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: -t, r";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_texcoord	o1

			def			c4,		0.0,	1.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_texcoord0	v0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		-1.0,	0.0,	-1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		-v0,	r1			// r4 = ( 0, 0, 0, 0 )
			add			r4,		r4,		c0.y		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: t.x, r";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_texcoord	o1

			def			c4,		1.0,	0.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_texcoord0	v0

			def			c0,		1.0,	0.0,	0.0,	1.0
			def			c1,		0.0,	0.5,	0.5,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		v0.x,	r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: t.xyxx, r";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_texcoord	o1

			def			c4,		1.0,	1.0,	0.0,	0.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_texcoord0	v0

			def			c0,		1.0,	1.0,	0.0,	0.0
			def			c1,		0.0,	0.5,	0.1,	0.1

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		v0.xyxx,	r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: -t.x, r";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_texcoord	o1

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_texcoord0	v0

			def			c0,		1.0,	0.0,	0.0,	0.0
			def			c1,		-2.0,	0.0,	-1.0,	-2.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		-v0.x,	r1
			sub			r4.y,	r4,		c0.x		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, t";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_texcoord	o1

			def			c4,		0.0,	1.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_texcoord0	v0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		1.0,	0.0,	1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		v0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, -t";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_texcoord	o1

			def			c4,		0.0,	1.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_texcoord0	v0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		-1.0,	0.0,	-1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		-v0			// r4 = ( 0, 0, 0, 0 )
			add			r4,		r4,		c0.y		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, t.x";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_texcoord	o1

			def			c4,		1.0,	0.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_texcoord0	v0

			def			c0,		1.0,	0.0,	0.0,	1.0
			def			c1,		0.0,	0.5,	0.5,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		v0.x

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, t.xyxx";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_texcoord	o1

			def			c4,		1.0,	1.0,	0.0,	0.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_texcoord0	v0

			def			c0,		1.0,	1.0,	0.0,	0.0
			def			c1,		0.0,	0.5,	0.1,	0.1

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		v0.xyxx

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, -t.x";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_texcoord	o1

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_texcoord0	v0

			def			c0,		1.0,	0.0,	0.0,	0.0
			def			c1,		-2.0,	0.0,	-1.0,	-2.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		-v0.x
			sub			r4.y,	r4,		c0.x		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x

			mov			oC0,	r4
		};
	}
}

////////

Technique
<
	String	Name = "Inst: max: v, r";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_color		o1

			def			c4,		0.0,	1.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_color	v0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		1.0,	0.0,	1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		v0,		r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: -v, r";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_color		o1

			def			c4,		0.0,	1.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_color	v0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		-1.0,	0.0,	-1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		-v0,	r1			// r4 = ( 0, 0, 0, 0 )
			add			r4,		r4,		c0.y		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: v.x, r";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_color		o1

			def			c4,		1.0,	0.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_color	v0

			def			c0,		1.0,	0.0,	0.0,	1.0
			def			c1,		0.0,	0.5,	0.5,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		v0.x,	r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: v.xyxx, r";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_color		o1

			def			c4,		1.0,	1.0,	0.0,	0.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_color	v0

			def			c0,		1.0,	1.0,	0.0,	0.0
			def			c1,		0.0,	0.5,	0.1,	0.1

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		v0.xyxx,	r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: -v.x, r";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_color		o1

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_color	v0

			def			c0,		1.0,	0.0,	0.0,	0.0
			def			c1,		-2.0,	0.0,	-1.0,	-2.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		-v0.x,	r1
			sub			r4.y,	r4,		c0.x		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, v";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_color		o1

			def			c4,		0.0,	1.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_color	v0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		1.0,	0.0,	1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		v0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, -v";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_color		o1

			def			c4,		0.0,	1.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_color	v0

			def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		-1.0,	0.0,	-1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		-v0			// r4 = ( 0, 0, 0, 0 )
			add			r4,		r4,		c0.y		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, v.x";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_color		o1

			def			c4,		1.0,	0.0,	0.0,	1.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_color	v0

			def			c0,		1.0,	0.0,	0.0,	1.0
			def			c1,		0.0,	0.5,	0.5,	0.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		v0.x

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, v.xyxx";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_color		o1

			def			c4,		1.0,	1.0,	0.0,	0.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_color	v0

			def			c0,		1.0,	1.0,	0.0,	0.0
			def			c1,		0.0,	0.5,	0.1,	0.1

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		v0.xyxx

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: r, -v.x";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;

        VertexShader = 
        asm
        {
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			dcl_position	o0
			dcl_color		o1

			def			c4,		1.0,	0.0,	0.0,	0.0

			m4x4		o0,		v0,		c0
			mov			o1,		c4
        };

		PixelShader =
		asm
		{
			ps_3_0
			dcl_color	v0

			def			c0,		1.0,	0.0,	0.0,	0.0
			def			c1,		-2.0,	0.0,	-1.0,	-2.0

			mov			r0,		c0
			mov			r1,		c1

			max			r4,		r1,		-v0.x
			sub			r4.y,	r4,		c0.x		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x

			mov			oC0,	r4
		};
	}
}

//////////

Technique
<
	String	Name = "Inst: max: mask, c, r";
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
			ps_3_0

			def			c0,		0.0,	0.0,	0.0,	0.0
			def			c1,		1.0,	0.0,	1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1
			
			mov			r4.yw,	c1.x
			max			r4.xz,	c0,		r1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: _sat, c, r";
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
			ps_3_0

			def			c0,		-2.0,	2.0,	-2.0,	2.0
			def			c1,		-1.0,	0.0,	-1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1
			
			max_sat		r4,		c0,		r1			// r4 = ( 0, 1, 0, 1 )			or ( -1, 2, -2, 0 )
			add			r4.xz,	r4,		-c1.x		// r4 = ( 1, 1, 1, 1 )			or ( 0, 2, -1, 0 )
			mul			r4.yw,	r4,		c1.x		// r4 = ( 1, -1, 1, -1 )		or ( 0, -2, -1, 0 )
			add			r4.yw,	r4,		-c0.x		// r4 = ( 1, 1, 1, 1 )			or ( 0, 0, -1, 0 )

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: max: _sat mask, c, r";
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
			ps_3_0

			def			c0,		-2.0,	2.0,	-2.0,	1.0
			def			c1,		-1.0,	0.0,	-1.0,	0.0

			mov			r0,		c0
			mov			r1,		c1

			mov			r4.yw,	-c0.x				// r4 = ( x, 2, x, 2 )
			mov			r4.z,	c1.y				// r4 = ( x, 2, 0, 2 )
			max_sat		r4.xy,	c0,		r1			// r4 = ( 0, 1, 0, 2 )			or ( -1, 2, -1, 0 )		or ( -2, 0, -2, 1 )
			sub			r4.w,	r4,		c0.w		// r4 = ( 0, 1, 0, 1 )			or ( -1, 2, -1, -1 )	or ( -2, 0, -2, 0 )
			add			r4.xz,	r4,		c0.w		// r4 = ( 1, 1, 1, 1 )			or ( 0, 2, 0, -1 )		or( -1, 0, -1, 0 )
			sub			r4.y,	r4,		c0.w		// r4 = ( 1, 0, 1, 1 )			or ( 0, 1, 0, -1 )		or ( -1, -1, -1, 0 )
			abs			r4.y,	r4					// r4 = ( 1, 0, 1, 1 )			or ( 0, 1, 0, -1 )		or ( -1, 1, -1, 0 )
			mul			r4.y,	r4,		c1.x		// r4 = ( 1, 0, 1, 1 )			or ( 0, -1, 0, -1 )		or ( -1, -1, -1, 0 )
			add			r4.y,	r4,		c0.w		// r4 = ( 1, 1, 1, 1 )			or ( 0, 0, 0, -1 )		or ( -1, 0, -1, 0 )

			mov			oC0,	r4
		};
	}
}

