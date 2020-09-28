#include "include.fx"

Technique
<
	String	Name = "Inst: frc - c# - positive src";
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
			def			c0,	 0.25,   0.0,	 0.0,	  0.0
			def			c1,	 1.0,   -1.0,	 1e-007,  0.0
			def			c2,  20.25, -20.875, 20.5,	 -20.125
			
			mov			r0.yzw,	c1.w			// r0 = ( x, 0, 0, 0 )
			frc			r0.x,	c2.x      		// r0 = ( .25, .0, .0, .0 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: frc - c# - negative src";
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
			def			c0,		0.75,	0.0,	0.0,	 0.0
			def			c1,		1.0,   -1.0,	1e-007,	 0.0
			def         c2,    -20.25, -20.875, -20.5,	-20.125
			
			mov			r0.yzw,	c1.w			// r0 = ( x, 0, 0, 0 )
			frc			r0.x,	c2.x      		// r0 = ( .75, .0, .0, .0 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: frc - c# - positive src, unmasked, unswizzled";
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
			def			c0,		0.25,	0.875,	0.5,	0.125
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    20.25,	20.875, 20.5,	20.125
			
			frc			r0, 	c2      		// r0 = ( 0.25,	0.875, 0.5, 0.125 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: frc - c# - negative src, unmasked, unswizzled";
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
			def			c0,		0.75,	0.125,	0.5,	0.875
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    -20.25,	-20.875,-20.5,	-20.125
			
			frc			r0, 	c2      		// r0 = ( 0.75,	0.125, 0.5, 0.875 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: frc - c# - positive src, unmasked, swizzled";
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
			def			c0,		0.125,	0.5,	0.875,	0.25
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    20.25,	20.875,	20.5,	20.125
			
			frc			r0, 	c2.wzyx     	// r0 = ( 0.125, 0.5, 0.875, 0.25 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: frc - c# - positive src, masked, unswizzled";
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
			def			c0,		0.25,	0.0,	0.5,	0.0
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    20.25,	20.9,	20.5,	20.1
			
			mov			r0.yw,	c1.w			// set r1 to black
			frc			r0.xz, 	c2          	// r0 = ( .25, .0, .5, .0 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: frc - c# - positive src, masked, swizzled";
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
			def			c0,		0.125,	0.0,	0.875,	0.0
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    20.2,	20.875,	20.5,	20.125
			
			mov			r0.yw,	c1.w			// set r1 to black
			frc			r0.xz, 	c2.wzyx        	// r0 = ( .125, .0, .875, .0 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: frc - c# - negated positive src";
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
			def			c0,		0.75,	0.125,	0.5,	0.875
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    20.25,	20.875,	20.5,	20.125
			
			frc			r0,    -c2           	// r0 = ( .75, .125, .5, .875 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: frc - c# - negated negative src";
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
			def			c0,		0.25,	0.875,	0.5,	0.125
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    -20.25,	-20.875,-20.5,	-20.125
			
			frc			r0,    -c2           	// r0 = ( 0.25,	0.875, 0.5, 0.125 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}

// *** r# tests

Technique
<
	String	Name = "Inst: frc - r# - positive src";
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
			def			c0,		0.25,	0.0,	0.0,	0.0
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    20.25,	-20.875,-20.5,	-20.125
						
			mov			r1,		c2
			mov			r0.yzw,	c1.w			// r0 = ( x, 0, 0, 0 )
			frc			r0.x,   r1.x           	// r0 = ( 0.25,	0.0, 0.0, 0.0 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
			
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: frc - r# - negative src";
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
			def			c0,		0.75,	0.0,	0.0,	0.0
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    -20.25,	-20.875,-20.5,	-20.125
						
			mov			r1,		c2
			mov			r0.yzw,	c1.w			// r0 = ( x, 0, 0, 0 )
			frc			r0.x,   r1.x           	// r0 = ( 0.75,	0.0, 0.0, 0.0 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: frc - r# - positive src, unmasked, unswizzled";
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
			def			c0,		0.25,	0.875,	0.5,	0.125
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    20.25,	20.875, 20.5,	20.125
						
			mov			r1,		c2
			frc			r0,     r1           	// r0 = ( 0.25,	0.875, 0.5, 0.125 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: frc - r# - negative src, unmasked, unswizzled";
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
			def			c0,		0.75,	0.125,	0.5,	0.875
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    -20.25,	-20.875,-20.5,	-20.125
						
			mov			r1,		c2
			frc			r0,     r1           	// r0 = ( 0.75,	0.125, 0.5, 0.875 )
						
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: frc - r# - positive src, unmasked, swizzled";
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
			def			c0,		0.125,	0.5,	0.875,	0.25
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    20.25,	20.875,	20.5,	20.125
						
			mov			r1,		c2
			frc			r0,     r1 .wzyx        // r0 = ( .125, .5, .875, .25 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: frc - r# - positive src, masked, unswizzled";
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
			def			c0,		0.25,	0.0,	0.5,	0.0
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    20.25,	20.9,	20.5,	20.1
						
			mov			r1,		c2
			mov			r0.yw,	c1.w			// set r1 to black
			frc			r0.xz,  r1              // r0 = ( 0.25,	0.0, 0.5, 0.0 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}


Technique
<
	String	Name = "Inst: frc - r# - positive src, masked, swizzled";
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
			def			c0,		0.125,	0.0,	0.875,	0.0
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    20.2,	20.875,	20.5,	20.125
						
			mov			r1,		c2
			mov			r0.yw,	c1.w			// set r1 to black
			frc			r0.xz,  r1.wzyx         // r0 = ( 0.125, 0.0, 0.875, 0.0 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: frc - r# - negated positive src";
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
			def			c0,		0.75,	0.125,	0.5,	0.875
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    20.25,	20.875,	20.5,	20.125
						
			mov			r1,		c2
			frc			r0,    -r1              // r0 = ( 0.75,	0.125, 0.5, 0.875 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: frc - r# - negated negative src";
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
			def			c0,		0.25,	0.875,	0.5,	0.125
			def			c1,		1.0,   -1.0,	1e-007,	0.0
			def         c2,    -20.25,	-20.875,-20.5,	-20.125
						
			mov			r1,		c2
			frc			r0,    -r1              // r0 = ( 0.25,	0.875, 0.5, 0.125 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}

// ** t# tests

Technique
<
	String	Name = "Inst: frc - t# - positive src";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[2]  = <fMaxTexRepeat>;

		VertexShader =
		asm
		{
			vs_2_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		2.25,	-2.875,	-2.5,	-2.125

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

		PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			def			c0,		0.25,	0.0,	0.0,	0.0
			def			c1,		1.0,	-1.0,	1e-007,	0.0
			
			mov			r0.yzw,	c1.w			// r0 = (  x, 0, 0, 0 )
			frc			r0.x,	t0.x			// r0 = ( .25, .0, .0, .0 )
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c2.w,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };
   }
}

Technique
<
	String	Name = "Inst: frc - t# - negative src";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[2]  = <fMaxTexRepeat>;

		VertexShader =
		asm
		{
			vs_2_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		-2.25,	-2.875,	-2.5,	-2.125

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			def			c0,		0.75,	0.0,	0.0,	0.0
			def			c1,		1.0,	-1.0,	0.00001,	0.0
			
			mov			r0.yzw,	c1.w			// r0 = (  x, 0, 0, 0 )
			frc			r0.x,	t0.x			// r0 = ( .75, .0, .0, .0 )
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c2.w,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };
   }
}

Technique
<
	String	Name = "Inst: frc - t# - positive src, unmasked, unswizzled";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[2]  = <fMaxTexRepeat>;

		VertexShader =
		asm
		{
			vs_2_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		2.25,	2.875,	2.5,	2.125

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			def			c0,		0.25,	0.875,	0.5,	0.125
			def			c1,		1.0,	-1.0,	1e-007,	0.0
			
			frc			r0,	    t0			    // r0 = ( 0.25,	0.875, 0.5, 0.125 )
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c2.w,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };
   }
}


Technique
<
	String	Name = "Inst: frc - t# - negative src, unmasked, unswizzled";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[2]  = <fMaxTexRepeat>;

		VertexShader =
		asm
		{
			vs_2_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		-2.25,	-2.875,	-2.5,	-2.125

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			def			c0,		0.75,	0.125,	0.5,	0.875
			def			c1,		1.0,	-1.0,	1e-007,	0.0
			
			frc			r0,	    t0			    // r0 = ( 0.75,	0.125,	0.5,	0.875 )
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c2.w,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };
   }
}


Technique
<
	String	Name = "Inst: frc - t# - positive src, unmasked, swizzled";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[2]  = <fMaxTexRepeat>;

		VertexShader =
		asm
		{
			vs_2_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		2.25,	2.875,	2.5,	2.125

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			def			c0,		0.125,	0.5,	0.875,	0.25
			def			c1,		1.0,	-1.0,	1e-007,	0.0
			
			frc			r0,	    t0.wzyx			// r0 = ( 0.125, 0.5, 0.875, 0.25 )
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c2.w,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };
   }
}


Technique
<
	String	Name = "Inst: frc - t# - positive src, masked, unswizzled";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[2]  = <fMaxTexRepeat>;

		VertexShader =
		asm
		{
			vs_2_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		2.25,	2.9,	2.5,	2.1

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			def			c0,		0.25,	0.0,	0.5,	0.0
			def			c1,		1.0,	-1.0,	1e-007,	0.0
			
			mov			r0.yw,	c1.w			// r0 = (  x, .0,  x, .0 )
			frc			r0.xz,	t0  			// r0 = ( 0.25,	0.0, 0.5, 0.0 )
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c2.w,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };
   }
}


Technique
<
	String	Name = "Inst: frc - t# - positive src, masked, swizzled";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[2]  = <fMaxTexRepeat>;

		VertexShader =
		asm
		{
			vs_2_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		2.2,	2.875,	2.5,	2.125

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			def			c0,		0.125,	0.0,	0.875,	0.0
			def			c1,		1.0,	-1.0,	1e-007,	0.0
			
			mov			r0.yw,	c1.w			// r0 = (  x, .0,  x, .0 )
			frc			r0.xz,	t0.wzyx  		// r0 = ( 0.125, 0.0, 0.875, 0.0 )
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c2.w,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };
   }
}


Technique
<
	String	Name = "Inst: frc - t# - negated positive src";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[2]  = <fMaxTexRepeat>;

		VertexShader =
		asm
		{
			vs_2_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		2.25,	2.875,	2.5,	2.125

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			def			c0,		0.75,	0.125,	0.5,	0.875
			def			c1,		1.0,	-1.0,	1e-007,	0.0
			
			frc			r0,	   -t0      		// r0 = ( .75, .125, .5, .875 )
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c2.w,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };
   }
}

Technique
<
	String	Name = "Inst: frc - t# - negated negative src";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[2]  = <fMaxTexRepeat>;

		VertexShader =
		asm
		{
			vs_2_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2

			def			c4,		-2.25,	-2.875,	-2.5,	-2.125

			m4x4		oPos,	v0,		c0
			mov			oT0,	c4
		};

        PixelShader =
        asm
        {
			ps_2_x
			dcl			t0
			def			c0,		0.25,	0.875,	0.5,	0.125
			def			c1,		1.0,	-1.0,	1e-007,	0.0
			
			frc			r0,	   -t0      		// r0 = ( .25, .875, .5, .125 )
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c2.w,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y  
			texkill     r2
			mov			oC0,	r2
        };
   }
}

// pred

Technique
<
	String	Name = "Inst: (p0) frc - r# - positive src, unmasked, unswizzled (unmasked predicate)";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {20.25,	20.875, 20.5,	20.125};

        PixelShader =
        asm
        {
			ps_2_x
			def			c0,		1.0,	4.0,	2.5,	0.0
			//def			c1,		20.25,	20.875, 20.5,	20.125
			def			c2,		0.25,	0.875,	0.5,	0.125
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-1.0,	-1.0,	-1.0,	1.0

			mov			r1,		c1
			mov			r3,		c3
			mov			r4,		c4
			
			setp_eq		p0,		r1,		c1		// p0 = ( t, t, t, t )
			
	(p0)	frc			r0,		r1				// r0 = { .25, .875, .5, .125 }
			sub			r0,		r0,		c2		// r0 = (  0,  0,  0,  0 )
			cmp			r1,		r0,		r3,		r4		// choose r3
			cmp			r2,		-r0,	r3,		r4		// choose r3
			add_sat		r0,		r1,		r2		// r0 = ( 0, 1, 0 ) if valid, else ( 0, 0, 0 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: (p0.x) frc - r# - positive src, unmasked, unswizzled";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {20.25,	20.875, 20.5,	20.125};

        PixelShader =
        asm
        {
			ps_2_x
			def			c0,		1.0,	4.0,	2.5,	0.0
			//def			c1,		20.25,	20.875, 20.5,	20.125
			def			c2,		0.25,	0.875,	0.5,	0.125
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-1.0,	-1.0,	-1.0,	1.0

			mov			r1,		c1
			mov			r3,		c3
			mov			r4,		c4
			
			setp_eq		p0.x,	r1,		c1
			setp_ne		p0.yzw,	r1,		c1		// p0 = ( t, f, f, f )
			
	(p0.x)	frc			r0,		r1				// r0 = { .25, .875, .5, .125 }
			sub			r0,		r0,		c2		// r0 = (  0,  0,  0,  0 )
			cmp			r1,		r0,		r3,		r4		// choose r3
			cmp			r2,		-r0,	r3,		r4		// choose r3
			add_sat		r0,		r1,		r2		// r0 = ( 0, 1, 0 ) if valid, else ( 0, 0, 0 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: (!p0.x) frc - r# - positive src, unmasked, unswizzled";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {20.25,	20.875, 20.5,	20.125};

        PixelShader =
        asm
        {
			ps_2_x
			def			c0,		1.0,	4.0,	2.5,	0.0
			//def			c1,		20.25,	20.875, 20.5,	20.125
			def			c2,		0.25,	0.875,	0.5,	0.125
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-1.0,	-1.0,	-1.0,	1.0

			mov			r1,		c1
			mov			r3,		c3
			mov			r4,		c4
			
			setp_eq		p0.yzw,	r1,		c1
			setp_ne		p0.x,	r1,		c1		// p0 = ( f, t, t, t )
			
	(!p0.x)	frc			r0,		r1				// r0 = { .25, .875, .5, .125 }
			sub			r0,		r0,		c2		// r0 = (  0,  0,  0,  0 )
			cmp			r1,		r0,		r3,		r4		// choose r3
			cmp			r2,		-r0,	r3,		r4		// choose r3
			add_sat		r0,		r1,		r2		// r0 = ( 0, 1, 0 ) if valid, else ( 0, 0, 0 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: (p0) frc - r# - positive src, unmasked, unswizzled (masked predicate)";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {20.25,	20.9,	20.5,	20.1};

        PixelShader =
        asm
        {
			ps_2_x
			def			c0,		1.0,	4.0,	2.5,	0.0
			//def			c1,		20.25,	20.9,	20.5,	20.1
			def			c2,		0.25,	0.0,	0.5,	0.0
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-1.0,	-1.0,	-1.0,	1.0

			mov			r1,		c1
			mov			r3,		c3
			mov			r4,		c4
			
			setp_eq		p0.xz,	r1,		c1
			setp_ne		p0.yw,	r1,		c1		// p0 = ( t, f, t, f )
			
			mov			r0.yw,	c0.w			// set r1 to black
	(p0)	frc			r0,		r1				// r0 = { .25, .0, .5, .0 }
			sub			r0,		r0,		c2		// r0 = ( 0, 0, 0, 0 )
			cmp			r1,		r0,		r3,		r4		// choose r3
			cmp			r2,		-r0,	r3,		r4		// choose r3
			add_sat		r0,		r1,		r2		// r0 = ( 0, 1, 0 ) if valid, else ( 0, 0, 0 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: (p0) frc - r# - positive src, masked, unswizzled (masked predicate)";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {20.25,	20.9,	20.5,	20.1};

        PixelShader =
        asm
        {
			ps_2_x
			def			c0,		1.0,	4.0,	2.5,	0.0
			//def			c1,		20.25,	20.9,	20.5,	20.1
			def			c2,		0.25,	0.0,	0.0,	0.0
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-1.0,	-1.0,	-1.0,	1.0

			mov			r1,		c1
			mov			r3,		c3
			mov			r4,		c4
			
			setp_eq		p0.xz,	r1,		c1
			setp_ne		p0.yw,	r1,		c1		// p0 = ( t, f, t, f )
			
			mov			r0.yzw,	c0.w			// set r1 to black
	(p0)	frc			r0.xyw,	r1				// r0 = { .25, .0, .0, .0 }
			sub			r0,		r0,		c2		// r0 = ( 0, 0, 0, 0 )
			cmp			r1,		r0,		r3,		r4		// choose r3
			cmp			r2,		-r0,	r3,		r4		// choose r3
			add_sat		r0,		r1,		r2		// r0 = ( 0, 1, 0 ) if valid, else ( 0, 0, 0 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}

Technique
<
	String	Name = "Inst: (!p0) frc - r# - positive src, masked, unswizzled";
	String	Shape = "TinyQuad";
>
{
    Pass P0
    {
        VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[1] = {20.25,	20.9,	20.5,	20.1};

        PixelShader =
        asm
        {
			ps_2_x
			def			c0,		1.0,	4.0,	2.5,	0.0
			//def			c1,		20.25,	20.9,	20.5,	20.1
			def			c2,		0.25,	0.0,	0.5,	0.0
			def			c3,		1.0,	1.0,	1.0,	1.0
			def			c4,		-1.0,	-1.0,	-1.0,	1.0

			mov			r1,		c1
			mov			r3,		c3
			mov			r4,		c4
			
			setp_eq		p0.yw,	r1,		c1
			setp_ne		p0.xz,	r1,		c1		// p0 = ( f, t, f, t )
			
			mov			r0.yw,	c0.w			// set r1 to black
	(!p0)	frc			r0,		r1				// r0 = { .25, .0, .5, .0 }
			sub			r0,		r0,		c2		// r0 = ( 0, 0, 0, 0 )
			cmp			r1,		r0,		r3,		r4		// choose r3
			cmp			r2,		-r0,	r3,		r4		// choose r3
			add_sat		r0,		r1,		r2		// r0 = ( 0, 1, 0 ) if valid, else ( 0, 0, 0 )

			mov			oC0,	r0
        };

		VertexShader = <StandardVS>;
   }
}
