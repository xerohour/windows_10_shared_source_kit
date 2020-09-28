#include "include.fx"

Technique
<
	String	Name = "Inst: mad r0, r1, c2";
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

			def			c0,	0.5,	0.5,	0.5,	0.5
			def			c1,	1,	1,	1,	0.5
                        def                     c2,     0,      0,      0,      0

			mov			r0,	c0
                        mov			r1,	c1
                        add                     r1,     r1, c1  // 2,2,2.1

		        mad			r0,	r0,	r1, c2
		};
	}
}

Technique
<
	String	Name = "Inst: mad r0, c1, c2";
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

			def			c0,	0.5,	0.5,	0.5,	0.5
			def			c1,	1,	1,	1,	1
                        def                     c2,     0.5,    0,      0.5,    0.5

			mov			r0,	c0

		        mad			r0,	r0,	c1, c2
		};
	}
}

Technique
<
	String	Name = "Inst: mad r0, c1, c2 (neg add)";
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

			def			c0,	0.5,	0.5,	0.5,	0.5
			def			c1,	1,	1,	1,	1
                        def                     c2,     -0.5,   -0.5,   -0.5,   -0.5

			mov			r0,	c0

		        mad			r0,	r0,	c1, c2
		};
	}
}

Technique
<
	String	Name = "Inst: mad c0, r0, -r1";
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

			def			c0,	0.5,	0.5,	0.5,	0.5
			def			c1,	1,	1,	1,	1
                        def                     c2,     0.5,    0.5,    0.5,    0.5

			mov			r0,	c1
                        add                     r0,     r0, c1  // 2,2,2,2
                        mov			r1,	c2

		        mad			r0,	c0,	r0, -r1
		};
	}
}

Technique
<
	String	Name = "Inst: mad r0, -r1, c2";
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

			def			c0,	0.5,	0.5,	0.5,	0.5
			def			c1,	-1,	-1,	-1,	-1
                        def                     c2,     0.5,    0.5,    0.5,    0.5

			mov			r0,	c0
                        mov			r1,	c1
                        add                     r1, r1, c1      // -2, -2, -2, -2

		        mad			r0,	r0,	-r1, c2
		};
	}
}

Technique
<
	String	Name = "Inst: mad_sat r0, r1, c2";
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

			def			c0,	1.0,	1.0,	1.0,	1.0
			def			c1,	1,	1,	1,	1
                        def                     c2,     0.5,    0.5,    0.5,    0.5

			mov			r0,	c0
                        mov			r1,	c1
                        add                     r1, r1, c1      // 2,2,2,2

		        mad_sat			r0,	r0,	r1, c2
		};
	}
}

Technique
<
	String	Name = "Inst: mad_sat r0, -r1, c2";
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

			def			c0,	1.0,	1.0,	1.0,	1.0
			def			c1,	-1,	-1,	-1,	-1
                        def                     c2,     0.5,    0.5,    0.5,    0.5

			mov			r0,	c0
                        mov			r1,	c1
                        add                     r1, r1, c1      // -2, -2, -2, -2

		        mad_sat			r0,	r0,	r1, c2
		};
	}
}

Technique
<
	String	Name = "Inst: mad_sat r0, c1, c2";
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

			def			c0,	1.0,	1.0,	1.0,	1.0
			def			c1,	1,	1,	1,	1
                        def                     c2,     0.5,    0.5,    0.5,    0.5

			mov			r0,	c0
                        add                     r0, r0, c0      // 2,2,2,2

		        mad_sat			r0,	r0,	c1, c2
		};
	}
}

