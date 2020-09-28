#include "include.fx"

Technique
<
	String	Name = "Inst: mul r0, c1";
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
			ps_1_1

			def			c0,		0.5,	0.5,	0.5,	1
			def			c1,		1,	1,	1,	0.5

			mov			r1,		c1
                        add                     r1,             r1, c1      // 2,2,2,1

		        mul			r0,		r1,		c0
		};
	}
}

Technique
<
	String	Name = "Inst: mul c0, c1";
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
			ps_1_1

			def			c0,		0.95,	0.75,	0.5,	1
			def			c1,		1,	1,	1,	1

		        mul			r0,		c0,		c1
		};
	}
}

Technique
<
	String	Name = "Inst: mul r0, -r1";
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
			ps_1_1

			def			c0,		0.5,	0.5,	0.5,	1
			def			c1,		-1,	-1,	-1,	-0.5

			mov			r0,		c0
                        mov			r1,		c1
                        add                     r1,             r1, c1      // 2,2,2,1

		        mul			r0,		r0,		-r1
		};
	}
}

Technique
<
	String	Name = "Inst: mul sat r0, c1";
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
			ps_1_1

			def			c0,		1,	1,	1,	1
			def			c1,		1,	1,	1,	1

			mov			r0,		c0
                        add                     r0, r0, c0      // 2,2,2,2

		        mul_sat			r0,		r0,		c1
		};
	}
}

Technique
<
	String	Name = "Inst: mul sat r0, -r1";
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
			ps_1_1

                        def			c0,		1,	1,	1,	1
                        def			c1,		-1,	-1,	-1,	-1                        

			mov			r0,		c0
                        add                     r0, r0, c0      // 2,2,2,2
                        mov			r1,		c1
                        add                     r1, r1, c1      // -2, -2, -2, -2

		        mul_sat			r0,		r0,		-r1
		};
	}
}




