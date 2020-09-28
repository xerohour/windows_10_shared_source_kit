#include "include.fx"

Technique
<
	String	Name = "Inst: abs r0, c0";
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
			
			def			c0,		1.0,	-1.0,	-1.0,	1.0
			
			abs			r0,		c0

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs r0, -c0";
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
			
			def			c0,		1.0,	-1.0,	-1.0,	1.0
			
			abs			r0,		-c0

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs r0, c.x";
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
			
			def			c0,		1.0,	0,		0,		0
			
			mov			r0.yzw,	c0.x		// r0 = ( x, 1, 1, 1 )
			abs			r0,		c0.x		// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs r0, c.xyxx";
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
			
			def			c0,		1.0,	1.0,	-1.0,	1.0
			
			mov			r0.zw,	c0.x		// r0 = ( x, x, 1, 1 )
			abs			r0,		c0.xyxx		// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs r0.xz, c0";
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
			
			def			c0,		1.0,	-1.0,	-1.0,	1.0
			
			mov			r0.yw,		c0.x	// r0 = ( x, 1, x, 1 )
			abs			r0.xz,		c0		// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs_sat r0, c0";
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
			
			def			c0,		4.0,	-3.0,	-4.0,	200.0
			
			abs_sat		r0,		c0

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs_sat r0.xy, c0";
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
			
			def			c0,		4.0,	-3.0,	-4.0,	1.0
			
			mov			r0.zw,	c0.w		// r0 = ( x, x, 1, 1 )
			abs_sat		r0.xy,	c0			// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: (p0) abs r0, c0";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {1.0,	-1.0,	-1.0,	1.0};

        PixelShader =
        asm
        {
			ps_2_x
			
			//def			c0,		1.0,	-1.0,	-1.0,	1.0

			mov			r0,		c0
			setp_eq		p0,		c0,		r0			// p0 = ( t, t, t, t )
			
	(p0)	abs			r0,		c0

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: (!p0) abs r0, c0";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {1.0,	-1.0,	-1.0,	1.0};

        PixelShader =
        asm
        {
			ps_2_x
			
			//def			c0,		1.0,	-1.0,	-1.0,	1.0

			mov			r0,		c0
			setp_ne		p0,		c0,		r0			// p0 = ( f, f, f, f )
			
	(!p0)	abs			r0,		c0

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: (p0-tftf) abs r0, c0";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {1.0,	-1.0,	-1.0,	1.0};

        PixelShader =
        asm
        {
			ps_2_x
			
			//def			c0,		1.0,	-1.0,	-1.0,	1.0

			mov			r0,		c0
			setp_eq		p0.xz,	c0,		r0
			setp_ne		p0.yw,	c0,		r0			// p0 = ( t, f, t, f )
			
			mov			r0.yw,	c0.x				// r0 = ( x, 1, x, 1 )
	(p0)	abs			r0,		c0					// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: (p0.x) abs r0.xz, c0";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {-10.0,	0.0,	-10.0,	0.0};

        PixelShader =
        asm
        {
			ps_2_x
			
			//def			c0,		-10.0,	0.0,	-10.0,	0.0
			def			c1,		1.0,	0.0,	0.0,	0.0

			mov			r0,		c0
			setp_eq		p0.xz,	c0,		r0
			setp_ne		p0.yw,	c0,		r0			// p0 = ( t, f, t, f )
			
			mov			r0.yzw,	c1.x				// r0 = ( x, 1, 1, 1 )
	(p0.x)	abs			r0.xz,	c0					// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: (p0.x) abs_sat r0.xz, c0";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {-10.0,	0.0,	-10.0,	0.0};

        PixelShader =
        asm
        {
			ps_2_x
			
			//def			c0,		-10.0,	0.0,	-10.0,	0.0
			def			c1,		1.0,	1.0,	0.0,	1.0

			mov			r0,		c0
			setp_eq		p0.xz,	c0,		r0
			setp_ne		p0.yw,	c0,		r0			// p0 = ( t, f, t, f )
			
			mov			r0.yzw,	c1.x				// r0 = ( x, 1, 1, 1 )
	(p0.x)	abs_sat		r0.xz,	c0					// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

// r#

Technique
<
	String	Name = "Inst: abs r0, r1";
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
			
			def			c0,		1.0,	-1.0,	-1.0,	1.0
			
			mov			r1,		c0
			abs			r0,		r1

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs r0, -r1";
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
			
			def			c0,		1.0,	-1.0,	-1.0,	1.0
			
			mov			r1,		c0
			abs			r0,		-r1

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs r0, r1.x";
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
			
			def			c0,		1.0,	0,		0,		0
			
			mov			r1,		c0
			mov			r0.yzw,	c0.x		// r0 = ( x, 1, 1, 1 )
			abs			r0,		r1.x		// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs r0, r1.xyxx";
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
			
			def			c0,		1.0,	1.0,	-1.0,	1.0
			
			mov			r1,		c0
			mov			r0.zw,	c0.x		// r0 = ( x, x, 1, 1 )
			abs			r0,		r1.xyxx		// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

// t#

Technique
<
	String	Name = "Inst: abs r0, t0";
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

			def			c4,		1.0,	-1.0,	-1.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	-1.0,	-1.0,	1.0
			
			mov			r1,		c0
			abs			r0,		t0

			mov			oC0,	r0
        };
    }
}

Technique
<
	String	Name = "Inst: abs r0, -t0";
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

			def			c4,		1.0,	-1.0,	-1.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	-1.0,	-1.0,	1.0
			
			mov			r1,		c0
			abs			r0,		-t0

			mov			oC0,	r0
        };
   }
}

Technique
<
	String	Name = "Inst: abs r0, t0.x";
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

			def			c4,		1.0,	0,		0,		0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	0,		0,		0
			
			mov			r1,		c0
			mov			r0.yzw,	c0.x		// r0 = ( x, 1, 1, 1 )
			abs			r0,		t0.x		// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r0
        };
   }
}

Technique
<
	String	Name = "Inst: abs r0, t0.xyxx";
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

			def			c4,		1.0,	1.0,	-1.0,	1.0

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			
			def			c0,		1.0,	1.0,	-1.0,	1.0
			
			mov			r1,		c0
			mov			r0.zw,	c0.x		// r0 = ( x, x, 1, 1 )
			abs			r0,		t0.xyxx		// r0 = ( 1, 1, 1, 1 )

			mov			oC0,	r0
        };
   }
}
