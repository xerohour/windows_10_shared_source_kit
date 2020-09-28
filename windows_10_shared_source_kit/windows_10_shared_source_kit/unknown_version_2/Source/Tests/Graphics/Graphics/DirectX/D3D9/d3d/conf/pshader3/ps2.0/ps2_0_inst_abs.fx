#include "include.fx"

Technique
<
	String	Name = "Inst: abs - c";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def	c0,	1.0,	-1.0,	-1.0,	1.0
			
			abs	r0,	c0

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs - -c";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def	c0,	1.0,	-1.0,	-1.0,	1.0
			
			abs	r0,	-c0

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs - c.x";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def    	c0,	1.0,    0,      0,      0
			
			mov    	r0.yzw,	c0.x		// r0 = ( x, 1, 1, 1 )
			abs    	r0,	c0.x		// r0 = ( 1, 1, 1, 1 )

			mov     oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: abs - mask";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def	c0,    	1.0,	-1.0,	-1.0,	1.0
			
			mov	r0.yw,	c0.x	// r0 = ( x, 1, x, 1 )
			abs	r0.xz,  c0	// r0 = ( 1, 1, 1, 1 )

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs - _sat";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def   	c0,  4.0,       -3.0,   -4.0,   200.0
			
			abs_sat	r0,  c0

			mov    	oC0, r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs - _sat mask";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def	c0,	4.0,	-3.0,	-4.0,	1.0
			
			mov    	r0.zw,	c0.w		// r0 = ( x, x, 1, 1 )
			abs_sat	r0.xy,	c0		// r0 = ( 1, 1, 1, 1 )

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

// r#

Technique
<
	String	Name = "Inst: abs - r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def     c0,    	1.0,	-1.0,	-1.0,	1.0
			
			mov	r1,    	c0
			abs	r0,    	r1

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs - -r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def     c0,	1.0,	-1.0,	-1.0,	1.0
			
			mov	r1,     c0
			abs	r0,     -r1

			mov	oC0,    r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs - r.x";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def	c0,    	1.0,	0,		0,		0
			
			mov	r1,    	c0
			mov	r0.yzw,	c0.x		// r0 = ( x, 1, 1, 1 )
			abs	r0,    	r1.x		// r0 = ( 1, 1, 1, 1 )

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

// t#

Technique
<
	String	Name = "Inst: abs - t";
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

			def	c4,	1.0,	-1.0,	-1.0,	1.0

			m4x4	oPos,	v0,	c0
			mov	oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_0
                        
			dcl	t0
			
			def	c0,	1.0,	-1.0,	-1.0,	1.0
			
			mov	r1,	c0
			abs	r0,     t0

			mov	oC0,    r0
        };
    }
}

Technique
<
	String	Name = "Inst: abs - -t";
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


			def    	c4,		1.0,	-1.0,	-1.0,	1.0

			m4x4	oPos,	v0,	c0
			mov 	oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_0
			dcl	t0
			
			def	c0,	1.0,	-1.0,	-1.0,	1.0
			
			mov	r1,	c0
			abs	r0,	-t0

			mov	oC0,	r0
        };
   }
}

Technique
<
	String	Name = "Inst: abs - t.x";
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

			def	c4,	1.0,	0,	0,	0

			m4x4	oPos,	v0,	c0
			mov	oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_0
			dcl	t0
			
			def	c0,	1.0,	0,      0,	0
			
			mov	r1,	c0
			mov	r0.yzw,	c0.x		// r0 = ( x, 1, 1, 1 )
			abs	r0,	t0.x		// r0 = ( 1, 1, 1, 1 )

			mov	oC0,	r0
        };
   }
}

Technique
<
	String	Name = "Inst: abs_pp - c";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def	c0,	1.0,	-1.0,	-1.0,	1.0
			
			abs_pp	r0,	c0

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs_pp - -c";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def	c0,	1.0,	-1.0,	-1.0,	1.0
			
			abs_pp	r0,	-c0

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs_pp - c.x";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def    	c0,	1.0,    0,      0,      0
			
			mov    	r0.yzw,	c0.x		// r0 = ( x, 1, 1, 1 )
			abs_pp 	r0,	c0.x		// r0 = ( 1, 1, 1, 1 )

			mov     oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: abs_pp - mask";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def	c0,    	1.0,	-1.0,	-1.0,	1.0
			
			mov	r0.yw,	c0.x	// r0 = ( x, 1, x, 1 )
			abs_pp	r0.xz,  c0	// r0 = ( 1, 1, 1, 1 )

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs - _sat_pp";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def   	c0,  4.0,       -3.0,   -4.0,   200.0
			
			abs_sat_pp	r0,  c0

			mov    	oC0, r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs - _sat_pp mask";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def	c0,	4.0,	-3.0,	-4.0,	1.0
			
			mov    	r0.zw,	c0.w		// r0 = ( x, x, 1, 1 )
			abs_sat_pp	r0.xy,	c0		// r0 = ( 1, 1, 1, 1 )

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

// r#

Technique
<
	String	Name = "Inst: abs_pp - r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def     c0,    	1.0,	-1.0,	-1.0,	1.0
			
			mov	r1,    	c0
			abs_pp	r0,    	r1

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs_pp - -r";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def     c0,	1.0,	-1.0,	-1.0,	1.0
			
			mov	r1,     c0
			abs_pp	r0,     -r1

			mov	oC0,    r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: abs_pp - r.x";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;

        PixelShader =
        asm
        {
			ps_2_0
			
			def	c0,    	1.0,	0,		0,		0
			
			mov	r1,    	c0
			mov	r0.yzw,	c0.x		// r0 = ( x, 1, 1, 1 )
			abs_pp	r0,    	r1.x		// r0 = ( 1, 1, 1, 1 )

			mov	oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

// t#

Technique
<
	String	Name = "Inst: abs_pp - t";
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

			def	c4,	1.0,	-1.0,	-1.0,	1.0

			m4x4	oPos,	v0,	c0
			mov	oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_0
                        
			dcl	t0
			
			def	c0,	1.0,	-1.0,	-1.0,	1.0
			
			mov	r1,	c0
			abs_pp	r0,     t0

			mov	oC0,    r0
        };
    }
}

Technique
<
	String	Name = "Inst: abs_pp - -t";
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


			def    	c4,		1.0,	-1.0,	-1.0,	1.0

			m4x4	oPos,	v0,	c0
			mov 	oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_0
			dcl	t0
			
			def	c0,	1.0,	-1.0,	-1.0,	1.0
			
			mov	r1,	c0
			abs_pp	r0,	-t0

			mov	oC0,	r0
        };
   }
}

Technique
<
	String	Name = "Inst: abs_pp - t.x";
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

			def	c4,	1.0,	0,	0,	0

			m4x4	oPos,	v0,	c0
			mov	oT0,	c4
        };

        PixelShader =
        asm
        {
			ps_2_0
			dcl	t0
			
			def	c0,	1.0,	0,      0,	0
			
			mov	r1,	c0
			mov	r0.yzw,	c0.x		// r0 = ( x, 1, 1, 1 )
			abs_pp	r0,	t0.x		// r0 = ( 1, 1, 1, 1 )

			mov	oC0,	r0
        };
   }
}

