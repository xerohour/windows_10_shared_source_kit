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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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

/*Technique
<
	String	Name = "Inst: frc - c# - extreme values";
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
			def			c0,	 2.168404E-18,  2.168404E-18, 2.168404E-18,  2.168404E-18
			def			c1,	 1.0,    -1.0,	     2.168404E-18,     0.0
			def			c2,  2.168404E-18, -2.168404E-18,   2.168404E-18, -2.168404E-18
			
			mov r3, c2
			sub r3.z, c1.x, r3.z
			sub r3.w, c1.y, r3.w
			
			mov r4, c0
			mov r4.y, r3.z
			mov r4.z, r3.z
			
			frc			r0,		r3    		// r0 = ( .25, .0, .0, .0 )
			
			sub			r1,		r0,		r4		// r1 = (  0,  0,  0,  0 )
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
	String	Name = "Inst: frc - c# - extreme values";
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
			def			c0,	 1e-007,  0.9999999, 0.9999999,  1e-004
			def			c1,	 1.0,    -1.0,	     1e-007,     0.0
			def			c2,  1e-007, -1e-007,   0.9999999, -0.9999
			
			mov r0.yzw, c1.w
			frc			r0,		c2    		// r0 = ( .25, .0, .0, .0 )
			
			sub			r1,		r0,		c0		// r1 = (  0,  0,  0,  0 )
			abs         r1,     r1
			sub         r1,     c1.z,     r1
			
			cmp         r2,     r1,     c1.x,    c1.y
			texkill     r2
			mov			oC0,	r2
        };

		VertexShader = <StandardVS>;
   }
}*/

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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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
			ps_2_0
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

