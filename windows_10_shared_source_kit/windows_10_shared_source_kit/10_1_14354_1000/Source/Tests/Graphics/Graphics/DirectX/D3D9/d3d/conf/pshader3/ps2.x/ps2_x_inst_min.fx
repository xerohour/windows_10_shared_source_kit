#include "include.fx"


Technique
<
	String	Name = "Inst: min: c, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		1.0,	10.0,	1.0,	10.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		c0,		r1

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: -c, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	-10.0,	-1.0,	-10.0
			def			c1,		-10.0,	-1.0,	-10.0,	-1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-c0,	-r1

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: c.x, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	0.0,	0.0,	0.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		c0.x,	r1			// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: c.xyxx, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	-1.0,	0.0,	0.0
			def			c1,		10.0,	10.0,	10.0,	10.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		c0.xyxx,	r1		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: -c.x, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		1.0,	-2.0,	-2.0,	-2.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-c0.x,	r1		// r0 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x	// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: r, -c";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	-10.0,	-1.0,	-10.0
			def			c1,		-10.0,	-1.0,	-10.0,	-1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-r1,	-c0

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: r, c.x";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	0.0,	0.0,	0.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		c0.x		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: r, c.xyxx";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	-1.0,	0.0,	0.0
			def			c1,		10.0,	10.0,	10.0,	10.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		c0.xyxx		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: r, -c.x";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		1.0,	-2.0,	-2.0,	-2.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		-c0.x	// r0 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x	// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: r, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		1.0,	10.0,	1.0,	10.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r0,		r1

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: -r, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	-10.0,	-1.0,	-10.0
			def			c1,		-10.0,	-1.0,	-10.0,	-1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-r0,	-r1

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: r.x, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	0.0,	0.0,	0.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r0.x,	r1			// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: r.xyxx, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	-1.0,	0.0,	0.0
			def			c1,		10.0,	10.0,	10.0,	10.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r0.xyxx,	r1		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: -r.x, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		1.0,	-2.0,	-2.0,	-2.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-r0.x,	r1		// r0 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x	// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: r, -r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	-10.0,	-1.0,	-10.0
			def			c1,		-10.0,	-1.0,	-10.0,	-1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-r1,	-r0

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: r, r.x";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	0.0,	0.0,	0.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		r0.x		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: r, r.xyxx";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		-1.0,	-1.0,	0.0,	0.0
			def			c1,		10.0,	10.0,	10.0,	10.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		r0.xyxx		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: r, -r.x";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		1.0,	-2.0,	-2.0,	-2.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		-r0.x	// r0 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x	// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

/////////////

Technique
<
	String	Name = "Inst: min: t, r";
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

			def			c4,		1.0,	10.0,	1.0,	10.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	10.0,	1.0,	10.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		t0,		r1

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: -t, r";
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

			def			c4,		-1.0,	-10.0,	-1.0,	-10.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			
			def			c0,		-1.0,	-10.0,	-1.0,	-10.0
			def			c1,		-10.0,	-1.0,	-10.0,	-1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-t0,	-r1

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: t.x, r";
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
			
			def			c0,		-1.0,	0.0,	0.0,	0.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		t0.x,	r1			// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: t.xyxx, r";
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
			
			def			c0,		-1.0,	-1.0,	0.0,	0.0
			def			c1,		10.0,	10.0,	10.0,	10.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		t0.xyxx,	r1		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: -t.x, r";
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

			def			c4,		1.0,	-2.0,	-2.0,	-2.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	-2.0,	-2.0,	-2.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-t0.x,	r1		// r0 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x	// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: r, -t";
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

			def			c4,		-1.0,	-10.0,	-1.0,	-10.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			
			def			c0,		-1.0,	-10.0,	-1.0,	-10.0
			def			c1,		-10.0,	-1.0,	-10.0,	-1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-r1,	-t0

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: r, t.x";
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
			
			def			c0,		-1.0,	0.0,	0.0,	0.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		t0.x		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: r, t.xyxx";
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
			
			def			c0,		-1.0,	-1.0,	0.0,	0.0
			def			c1,		10.0,	10.0,	10.0,	10.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		t0.xyxx		// r4 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: r, -t.x";
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

			def			c4,		1.0,	-2.0,	-2.0,	-2.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	-2.0,	-2.0,	-2.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		-t0.x	// r0 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x	// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};
	}
}

/////////////

Technique
<
	String	Name = "Inst: min: v, r";
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

			def			c4,		1.0,	10.0,	1.0,	10.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			v0
			
			def			c0,		1.0,	10.0,	1.0,	10.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		v0,		r1

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: -v, r";
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

			def			c4,		1.0,	0.0,	1.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			v0
			
			def			c0,		1.0,	0.0,	1.0,	0.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-v0,	-r1			// r4 = ( -10, 0, -10, 0 )		or ( -10, -1, -10, -1 )
			mul			r4.yw,	r4,		-c1.x		// r4 =	( -10, 0, -10, 0 )		or ( -10, 10, -10, 10 )
			sub			r4.yw,	r4,		c1.x		// r4 = ( -10, -10, -10, -10 )	or ( -10, 0, -10, 0 )
			mul			r4,		r4,		-c0.x		// r4 = ( 10, 10, 10, 10 )		or ( 10, 0, 10, 0 )
			sub			r4,		r4,		c0.y		// r4 = ( 0, 0, 0, 0 )			or ( 0, -10, 0, -10 )
			add			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )			or ( 1, -9, 1, -9 )

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: v.x, r";
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
			
			def			c0,		1.0,	0.0,	0.0,	0.0
			def			c1,		10.0,	2.0,	10.0,	2.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		v0.x,	r1			// r4 = ( 1, 1, 1, 1 )
			mul			r4,		r4,		-c0.x		// r4 = ( -1, -1, -1, -1 )
			add			r4,		r4,		c1.y		// r4 = ( 1, 1, 1, 1 )			

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: v.xyxx, r";
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
			
			def			c0,		1.0,	1.0,	0.0,	0.0
			def			c1,		10.0,	2.0,	10.0,	2.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		v0.xyxx,	r1		// r4 = ( 1, 1, 1, 1 )
			mul			r4,		r4,		-c0.x		// r4 = ( -1, -1, -1, -1 )
			add			r4,		r4,		c1.y		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: -v.x, r";
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

			def			c4,		1.0,	0.0,	0.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			v0
			
			def			c0,		1.0,	0.0,	0.0,	1.0
			def			c1,		10.0,	-.5,	-.5,	2.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-v0.x,	r1		// r0 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x	// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: r, -v";
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

			def			c4,		1.0,	0.0,	1.0,	0.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			v0
			
			def			c0,		1.0,	0.0,	1.0,	0.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		-r1,	-v0			// r4 = ( -10, 0, -10, 0 )		or ( -10, -1, -10, -1 )
			mul			r4.yw,	r4,		-c1.x		// r4 =	( -10, 0, -10, 0 )		or ( -10, 10, -10, 10 )
			sub			r4.yw,	r4,		c1.x		// r4 = ( -10, -10, -10, -10 )	or ( -10, 0, -10, 0 )
			mul			r4,		r4,		-c0.x		// r4 = ( 10, 10, 10, 10 )		or ( 10, 0, 10, 0 )
			sub			r4,		r4,		c0.y		// r4 = ( 0, 0, 0, 0 )			or ( 0, -10, 0, -10 )
			add			r4,		r4,		c0.x		// r4 = ( 1, 1, 1, 1 )			or ( 1, -9, 1, -9 )

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: r, v.x";
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
			
			def			c0,		1.0,	0.0,	0.0,	0.0
			def			c1,		10.0,	2.0,	10.0,	2.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		v0.x		// r4 = ( 1, 1, 1, 1 )
			mul			r4,		r4,		-c0.x		// r4 = ( -1, -1, -1, -1 )
			add			r4,		r4,		c1.y		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: r, v.xyxx";
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
			
			def			c0,		1.0,	1.0,	0.0,	0.0
			def			c1,		10.0,	2.0,	10.0,	2.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		v0.xyxx		// r4 = ( 1, 1, 1, 1 )
			mul			r4,		r4,		-c0.x		// r4 = ( -1, -1, -1, -1 )
			add			r4,		r4,		c1.y		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};
	}
}

Technique
<
	String	Name = "Inst: min: r, -v.x";
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

			def			c4,		1.0,	0.0,	0.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oD0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			v0
			
			def			c0,		1.0,	0.0,	0.0,	1.0
			def			c1,		10.0,	-.5,	-.5,	2.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min			r4,		r1,		-v0.x	// r0 = ( -1, -1, -1, -1 )
			mul			r4,		r4,		-c0.x	// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};
	}
}

/////////////

Technique
<
	String	Name = "Inst: min: mask, c, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		1.0,	10.0,	1.0,	10.0
			def			c1,		10.0,	1.0,	10.0,	1.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			mov			r4.zw,	c0.x
			min			r4.xy,	c0,		r1

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: _sat, c, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		2.0,	10.0,	2.0,	10.0
			def			c1,		10.0,	2.0,	10.0,	2.0
			
			mov			r0,		c0
			mov			r1,		c1
			
			min_sat		r4,		c0,		r1

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}

Technique
<
	String	Name = "Inst: min: _sat, mask, c, r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_x
			
			def			c0,		2.0,	10.0,	1.0,	10.0
			def			c1,		10.0,	2.0,	10.0,	1.0
			def			c2,		.5,		0.0,	0.0,	0.0
			
			mov			r0,		c0
			mov			r1,		c1

			mov			r4.zw,	c0.x			// r4 = ( x, x, 2, 2 )
			mul			r4.zw,	r4,		c2.x	// r4 = ( x, x, 1, 1 )
			min_sat		r4.xy,	c0,		r1		// r4 = ( 1, 1, 1, 1 )

			mov			oC0,	r4
     	};

		VertexShader = <StandardVS>;
	}
}
