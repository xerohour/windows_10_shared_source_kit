#include "include.fx"

Technique
<
	String	Name = "Inst: lrp c0, c1, r1";
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

			def 	c0,	0.5,	0.5,	0.5,	1.0
			def 	c1,	0,	0,	0,	1
                        def     c2,     1,      1,      1,      0.5

			mov 	r1,	c2
                        add     r1, r1, c2      // 2,2,2,1

		        lrp     r0,	c0,	c1, r1
		};
	}
}

Technique
<
	String	Name = "Inst: lrp c0, r1, r0 (neg t)";
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

			def 	c0,	-0.5,	-0.5,	-0.5,	1.0
			def 	c1,	1,	1,	1,	1

			mov 	r1,	c1
                        add     r1,     r1, c1  // 2,2,2,2
                        mov     r0,     c1
                        add     r0,     r0, c1
                        add     r0,     r0, c1  // 3,3,3,3

		        lrp 	r0,	c0,	r1, r0
		};
	}
}

Technique
<
	String	Name = "Inst: lrp sat c0, r1, r0";
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

			def 	c0,	0.5,	0.5,	0.5,	1.0
			def 	c1,	1,	1,	1,	1

			mov 	r1,	c1
                        add     r1,     r1, c1  // 2,2,2,2
                        mov     r0,     c1
                        add     r0,     r0, c1
                        add     r0,     r0, c1  // 3,3,3,3

		        lrp_sat	r0,	c0,	r1, r0
		};
	}
}

Technique
<
	String	Name = "Inst: lrp sat c0, r1, r0 (neg)";
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

			def 	c0,	0.5,	0.5,	0.5,	1.0
			def 	c1,	-1,	-1,	-1,	1

			mov 	r1,	c1
                        add     r1,     r1, c1  // -2,-2,-2,-2
                        mov     r0,     c1
                        add     r0,     r0, c1
                        add     r0,     r0, c1  // -3,-3,-3,-3

		        lrp_sat	r0,	c0,	r1, r0
		};
	}
}



