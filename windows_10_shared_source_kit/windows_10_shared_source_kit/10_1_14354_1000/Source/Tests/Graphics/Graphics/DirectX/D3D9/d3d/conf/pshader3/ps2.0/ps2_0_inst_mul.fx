#include "include.fx"

Technique
<
	String	Name = "Inst: mul";
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

			def			c0,		0.5,	1.0,	0.0,	1.0
			def			c1,		2,		2,		2,		2

			mov			r0,		c0
			mov			r1,		c1
			
                        mov			r4,		c0.x		// r4 = ( 0.5, 0.5, 0.5, 0.5 )
		        mul			r4,		r4,		c1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mul masked";
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

			def	c0,	0.5,	1.0,	0.0,	1.0
			def	c1,	2,	2,	2,	2

			mov	r0,	c0
			mov	r1,	c1
			
                        mov	r4,	c0.x		// r4 = ( 0.5, 0.5, 0.5, 0.5 )
		        mul	r4.x,     r4.x,	c1.x
                        mul	r4.y,     r4.y,	c1.y
                        mul	r4.z,     r4.z,	c1.z
                        mul	r4.w,     r4.w,	c1.w

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mul sat masked";
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

			def	c0,	0.75,	1.0,	0.25,	1.0
			def	c1,	2,	2,	2,	2

			mov	r0,	c0
			mov	r1,	c1
			
                        mov	r4,	c0		// r4 = ( 0.5, 0.5, 0.5, 0.5 )
		        mul_sat	r4.x,     r4.x,	c1.x
                        mul_sat	r4.y,     r4.y,	c1.y
                        mul_sat	r4.z,     r4.z,	c1.z
                        mul_sat	r4.w,     r4.w,	c1.w

			mov	oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mul _pp";
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

			def			c0,		0.45985,	1.0,	0.0,	1.0
			def			c1,		2,		2,		2,		2

			mov			r0,		c0
			mov			r1,		c1
			
                        mov			r4,		c0.x		// r4 = ( 0.5, 0.5, 0.5, 0.5 )
		        mul_pp			r4,		r4,		c1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mul sat _pp";
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

			def			c0,		0.45985,	1.123,	0.157,	1.0
			def			c1,		2,		2.25,	2.534,	2

			mov			r0,		c0
			mov			r1,		c1
			
                        mov			r4,		c0.x		// r4 = ( 0.5, 0.5, 0.5, 0.5 )
		        mul_sat_pp		r4,		r4,		c1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mul sat pp masked";
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

			def	c0,	0.75234,1.234,	0.23425,	1.0
			def	c1,	2.2234,	2,	2.2342,	        2

			mov	        r0,	c0
			mov	        r1,	c1
			
                        mov	        r4,	c0		// r4 = ( 0.5, 0.5, 0.5, 0.5 )
		        mul_sat_pp	r4.x,     r4.x,	c1.x
                        mul_sat_pp	r4.y,     r4.y,	c1.y
                        mul_sat_pp	r4.z,     r4.z,	c1.z
                        mul_sat_pp	r4.w,     r4.w,	c1.w

			mov	        oC0,	r4
		};
	}
}

