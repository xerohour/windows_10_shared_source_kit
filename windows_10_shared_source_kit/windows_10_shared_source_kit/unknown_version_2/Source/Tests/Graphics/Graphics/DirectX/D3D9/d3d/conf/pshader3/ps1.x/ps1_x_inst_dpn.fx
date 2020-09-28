#include "include.fx"

Technique
<
	String	Name = "Inst: dp3 r0, c0, c2 (neg result)";
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

			def			c0,	0,	1,	0,	0
			def			c1,	0,	-1,	0,	0

                        dp3     r0, c0, c1      // r0.w = -1.0
                        mov     r0.rgb,r0.a

                        mov     r0, -r0
                };
	}
}

Technique
<
	String	Name = "Inst: dp3 r0, c0, c2 (pos result)";
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

			def			c0,	0,	1,	0,	0
			def			c1,	0,     1,	0,	0

                        dp3     r0, c0, c1      // r0.w = 1.0
                        mov     r0.rgb,r0.a
		};
	}
}

Technique
<
	String	Name = "Inst: dp3_sat r0, c0, c2 (neg result)";
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

			def			c0,	0,	1,	0,	0
			def			c1,	0,	-1,	0,	0
                        def			c2,	0.5,	0.5,	0.5,	1

                        dp3_sat r0, c0, c1      // r0.w = 0.0
                        mov     r0.rgb,r0.a

                        add     r0, r0, c2
                };
	}
}

Technique
<
	String	Name = "Inst: dp3_sat r0, r1, c2 (pos result)";
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

			def			c0,	0,	1,	0,	0
			def			c1,	0,	1,	0,	0
                        def			c2,	-0.5,	-0.5,	-0.5,	1

                        mov     r1, c1
                        add     r1, r1, c1      // 0, 2, 0, 0, non-unit length vector
                        
                        dp3_sat r0, r1, c1      // r0.w = 1.0
                        mov     r0.rgb,r0.a

                        add     r0, r0, c2
                };
	}
}

//*****************************************************************************

Technique
<
	String	Name = "Inst: dp4 r0, c0, c2 (neg result)";
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
			ps_1_2

			def			c0,	0,	0,	0,	1
			def			c1,	0,	0,	0,	-1

                        dp4     r0, c0, c1      // r0.w = -1.0
                        mov     r0.rgb,r0.a
                        
                        mov     r0, -r0
                        
		};
	}
}

Technique
<
	String	Name = "Inst: dp4 r0, c0, c2 (pos result)";
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
			ps_1_2

			def			c0,	0,	0,	0,	1
			def			c1,	0,      0,	0,	1

                        dp4     r0, c0, c1      // r0.w = 1.0
                        mov     r0.rgb,r0.a
		};
	}
}

Technique
<
	String	Name = "Inst: dp4_sat r0, c0, c2 (neg result)";
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
			ps_1_2

			def			c0,	0,	0,	0,	1
			def			c1,	0,	0,	0,	-1
                        def			c2,	0.5,	0.5,	0.5,	1

                        dp4_sat r0, c0, c1      // r0.w = 0.0
                        mov     r0.rgb,r0.a

                        add     r0, r0, c2
                };
	}
}

Technique
<
	String	Name = "Inst: dp4_sat r0, r1, c2 (pos result)";
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
			ps_1_2

			def			c0,	0,	0,	0,	1
			def			c1,	0,	0,	0,	1
                        def			c2,	-0.5,	-0.5,	-0.5,	1

                        mov     r1, c1
                        add     r1, r1, c1      // 0, 0, 0, 2, non-unit length vector
                        
                        dp4_sat r0, r1, c1      // r0.w = 1.0
                        mov     r0.rgb,r0.a

                        add     r0, r0, c2
                };
	}
}

