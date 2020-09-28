#include "include.fx"

Technique
<
	String	Name = "ps3_0 mul c0, r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c1, 	0.026664, 	0.010000, 	0.050000, 	0.016667
			mov 	r0, 	c1
			mov 	r2, 	c2
			mul 	r2, 	c0, 	r0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c1.z, r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[1] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c2, 	100.000000, 	75.000000, 	499.999969, 	50.000004
			mov 	r1, 	c2
			mov 	r3, 	c3
			mul 	r3, 	c1.z, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c2, r2.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[2] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c3, 	0.050000, 	0.016667, 	0.026664, 	0.010000
			mov 	r2, 	c3
			mov 	r4, 	c4
			mul 	r4, 	c2, 	r2.zxyw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c3.ywx, r3.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[3] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c4, 	5.000000, 	2.000000, 	9.000001, 	4.687500
			mov 	r3, 	c4
			mov 	r5, 	c5
			mul 	r5, 	c3.ywx, 	r3.w
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c4.wzyx, r4.ywz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[4] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c5, 	0.426624, 	1.000000, 	1.250000, 	0.185185
			mov 	r4, 	c5
			mov 	r6, 	c6
			mul 	r6, 	c4.wzyx, 	r4.ywz
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyw c5, r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[5] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c6, 	0.810000, 	0.292969, 	0.050000, 	0.080000
			mov 	r5, 	c6
			mov 	r7, 	c7
			mul 	r7.xyw, 	c5, 	r5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c6, r6_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[6] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c7, 	31.249998, 	2.057613, 	6.825984, 	100.000008
			mov 	r6, 	c7
			mov 	r8, 	c8
			mul 	r8, 	c6, 	r6_abs
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp c7, r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[7] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c8, 	0.050000, 	0.080000, 	0.810000, 	0.292969
			mov 	r7, 	c8
			mov 	r9, 	c9
			mul_pp 	r9, 	c7, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c8, v0(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			mov 	r8, 	c8
			mov 	r10, 	c11
			mul 	r10, 	c8, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c9.x, v1(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[9] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord1 	v1
			mov 	r8, 	c9
			mov 	r10, 	c11
			mul 	r10, 	c9.x, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c11, v2.xyzw(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			mov 	r8, 	c11
			mov 	r10, 	c12
			mul 	r10, 	c11, 	v2.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c11.zxy, v3.y(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord3 	v3
			mov 	r8, 	c11
			mov 	r10, 	c13
			mul 	r10, 	c11.zxy, 	v3.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c12.yzxw, v4.zxw(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[12] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			mov 	r8, 	c12
			mov 	r10, 	c14
			mul 	r10, 	c12.yzxw, 	v4.zxw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xzw c13, v5(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[13] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			mov 	r8, 	c13
			mov 	r10, 	c15
			mul 	r10.xzw, 	c13, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c14_abs, -v6(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[14] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			mov 	r8, 	c14
			mov 	r10, 	c16
			mul 	r10, 	c14_abs, 	-v6
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat c15, v7(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[15] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			mov 	r8, 	c15
			mov 	r10, 	c17
			mul_sat 	r10, 	c15, 	v7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c16, v8(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight8 	v8
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	c16, 	v8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c17.z, v9(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[17] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight9 	v9
			mov 	r8, 	c17
			mov 	r10, 	c19
			mul 	r10, 	c17.z, 	v9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c18, v0.zxyw(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[18] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			mov 	r8, 	c18
			mov 	r10, 	c20
			mul 	r10, 	c18, 	v0.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c19.zyx, v1.w(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[19] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			mov 	r8, 	c19
			mov 	r10, 	c21
			mul 	r10, 	c19.zyx, 	v1.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c20.wzyx, v2.zyw(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[20] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight2 	v2
			mov 	r8, 	c20
			mov 	r10, 	c22
			mul 	r10, 	c20.wzyx, 	v2.zyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yzw c21, v3(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[21] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight3 	v3
			mov 	r8, 	c21
			mov 	r10, 	c23
			mul 	r10.yzw, 	c21, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -c22, -v4(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[22] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight4 	v4
			mov 	r8, 	c22
			mov 	r10, 	c24
			mul 	r10, 	-c22, 	-v4
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp c23, v5(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[23] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight5 	v5
			mov 	r8, 	c23
			mov 	r10, 	c25
			mul_pp 	r10, 	c23, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c24, v6(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices6 	v6
			mov 	r8, 	c24
			mov 	r10, 	c26
			mul 	r10, 	c24, 	v6
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c25.x, v7(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[25] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices7 	v7
			mov 	r8, 	c25
			mov 	r10, 	c27
			mul 	r10, 	c25.x, 	v7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c26, v8.xyzw(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[26] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices8 	v8
			mov 	r8, 	c26
			mov 	r10, 	c28
			mul 	r10, 	c26, 	v8.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c27.zwx, v9.y(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[27] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices9 	v9
			mov 	r8, 	c27
			mov 	r10, 	c29
			mul 	r10, 	c27.zwx, 	v9.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c28.yzxw, v0.zwy(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[28] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices0 	v0
			mov 	r8, 	c28
			mov 	r10, 	c30
			mul 	r10, 	c28.yzxw, 	v0.zwy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyzw c29, v1(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[29] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices1 	v1
			mov 	r8, 	c29
			mov 	r10, 	c31
			mul 	r10.xyzw, 	c29, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c30_abs, v2_abs(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[30] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			mov 	r8, 	c30
			mov 	r10, 	c0
			mul 	r10, 	c30_abs, 	v2_abs
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat c31, v3(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[31] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			mov 	r8, 	c31
			mov 	r10, 	c1
			mul_sat 	r10, 	c31, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c0, v4(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal4 	v4
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul 	r10, 	c0, 	v4
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c1.z, v5(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[1] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal5 	v5
			mov 	r8, 	c1
			mov 	r10, 	c3
			mul 	r10, 	c1.z, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c2, v6.zxyw(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[2] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			mov 	r8, 	c2
			mov 	r10, 	c4
			mul 	r10, 	c2, 	v6.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c3.wxy, v7.w(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[3] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			mov 	r8, 	c3
			mov 	r10, 	c5
			mul 	r10, 	c3.wxy, 	v7.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c4.wzyx, v8.wxz(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[4] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			mov 	r8, 	c4
			mov 	r10, 	c6
			mul 	r10, 	c4.wzyx, 	v8.wxz
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.x c5, v9(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[5] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			mov 	r8, 	c5
			mov 	r10, 	c7
			mul 	r10.x, 	c5, 	v9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c6_abs, v0(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[6] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal0 	v0
			mov 	r8, 	c6
			mov 	r10, 	c8
			mul 	r10, 	c6_abs, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp c7, v1(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[7] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal1 	v1
			mov 	r8, 	c7
			mov 	r10, 	c9
			mul_pp 	r10, 	c7, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c8, v2(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent2 	v2
			mov 	r8, 	c8
			mov 	r10, 	c11
			mul 	r10, 	c8, 	v2
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c9.x, v3(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[9] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent3 	v3
			mov 	r8, 	c9
			mov 	r10, 	c11
			mul 	r10, 	c9.x, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c11, v4.xyzw(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent4 	v4
			mov 	r8, 	c11
			mov 	r10, 	c12
			mul 	r10, 	c11, 	v4.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c11.wyx, v5.y(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent5 	v5
			mov 	r8, 	c11
			mov 	r10, 	c13
			mul 	r10, 	c11.wyx, 	v5.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c12.yzxw, v6.wyz(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[12] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent6 	v6
			mov 	r8, 	c12
			mov 	r10, 	c14
			mul 	r10, 	c12.yzxw, 	v6.wyz
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.y c13, v7(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[13] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent7 	v7
			mov 	r8, 	c13
			mov 	r10, 	c15
			mul 	r10.y, 	c13, 	v7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c14, -v8(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[14] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent8 	v8
			mov 	r8, 	c14
			mov 	r10, 	c16
			mul 	r10, 	c14, 	-v8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat c15, v9(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[15] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent9 	v9
			mov 	r8, 	c15
			mov 	r10, 	c17
			mul_sat 	r10, 	c15, 	v9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c16, v0(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal0 	v0
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	c16, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c17.z, v1(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[17] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal1 	v1
			mov 	r8, 	c17
			mov 	r10, 	c19
			mul 	r10, 	c17.z, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c18, v2.zxyw(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[18] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal2 	v2
			mov 	r8, 	c18
			mov 	r10, 	c20
			mul 	r10, 	c18, 	v2.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c19.wzx, v3.w(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[19] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal3 	v3
			mov 	r8, 	c19
			mov 	r10, 	c21
			mul 	r10, 	c19.wzx, 	v3.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c20.wzyx, v4.wzy(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[20] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			mov 	r8, 	c20
			mov 	r10, 	c22
			mul 	r10, 	c20.wzyx, 	v4.wzy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.z c21, v5(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[21] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			mov 	r8, 	c21
			mov 	r10, 	c23
			mul 	r10.z, 	c21, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -c22, v6_abs(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[22] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			mov 	r8, 	c22
			mov 	r10, 	c24
			mul 	r10, 	-c22, 	v6_abs
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp c23, v7(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[23] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			mov 	r8, 	c23
			mov 	r10, 	c25
			mul_pp 	r10, 	c23, 	v7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c24, v8(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color8 	v8
			mov 	r8, 	c24
			mov 	r10, 	c26
			mul 	r10, 	c24, 	v8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c25.x, v9(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[25] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color9 	v9
			mov 	r8, 	c25
			mov 	r10, 	c27
			mul 	r10, 	c25.x, 	v9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c26, v0.xyzw(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[26] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			mov 	r8, 	c26
			mov 	r10, 	c28
			mul 	r10, 	c26, 	v0.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c27.xywz, v1.y(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[27] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			mov 	r8, 	c27
			mov 	r10, 	c29
			mul 	r10, 	c27.xywz, 	v1.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c28.yzxw, v2.xzyw(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[28] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color2 	v2
			mov 	r8, 	c28
			mov 	r10, 	c30
			mul 	r10, 	c28.yzxw, 	v2.xzyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.w c29, v3(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[29] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color3 	v3
			mov 	r8, 	c29
			mov 	r10, 	c31
			mul 	r10.w, 	c29, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -c30, -v4(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[30] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color4 	v4
			mov 	r8, 	c30
			mov 	r10, 	c0
			mul 	r10, 	-c30, 	-v4
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat c31, v5(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[31] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color5 	v5
			mov 	r8, 	c31
			mov 	r10, 	c1
			mul_sat 	r10, 	c31, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c0, v6(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog6 	v6
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul 	r10, 	c0, 	v6
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c1.z, v7(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[1] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog7 	v7
			mov 	r8, 	c1
			mov 	r10, 	c3
			mul 	r10, 	c1.z, 	v7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c2, v8.zxyw(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[2] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog8 	v8
			mov 	r8, 	c2
			mov 	r10, 	c4
			mul 	r10, 	c2, 	v8.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c3.xzwy, v9.w(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[3] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog9 	v9
			mov 	r8, 	c3
			mov 	r10, 	c5
			mul 	r10, 	c3.xzwy, 	v9.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c4.wzyx, v0.xwyz(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[4] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog0 	v0
			mov 	r8, 	c4
			mov 	r10, 	c6
			mul 	r10, 	c4.wzyx, 	v0.xwyz
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xy c5, v1(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[5] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog1 	v1
			mov 	r8, 	c5
			mov 	r10, 	c7
			mul 	r10.xy, 	c5, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c6_abs, v2_abs(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[6] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			mov 	r8, 	c6
			mov 	r10, 	c8
			mul 	r10, 	c6_abs, 	v2_abs
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp c7, v3(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[7] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			mov 	r8, 	c7
			mov 	r10, 	c9
			mul_pp 	r10, 	c7, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c8, v4(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth4 	v4
			mov 	r8, 	c8
			mov 	r10, 	c11
			mul 	r10, 	c8, 	v4
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c9.x, v5(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[9] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth5 	v5
			mov 	r8, 	c9
			mov 	r10, 	c11
			mul 	r10, 	c9.x, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c11, v6.xyzw(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			mov 	r8, 	c11
			mov 	r10, 	c12
			mul 	r10, 	c11, 	v6.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c11.xwzy, v7.y(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			mov 	r8, 	c11
			mov 	r10, 	c13
			mul 	r10, 	c11.xwzy, 	v7.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c12.yzxw, v8.yxzw(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[12] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			mov 	r8, 	c12
			mov 	r10, 	c14
			mul 	r10, 	c12.yzxw, 	v8.yxzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xz c13, v9(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[13] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			mov 	r8, 	c13
			mov 	r10, 	c15
			mul 	r10.xz, 	c13, 	v9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -c14, v0(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[14] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth0 	v0
			mov 	r8, 	c14
			mov 	r10, 	c16
			mul 	r10, 	-c14, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat c15, v1(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[15] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth1 	v1
			mov 	r8, 	c15
			mov 	r10, 	c17
			mul_sat 	r10, 	c15, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c16, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	c16, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c17.z, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[17] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c17
			mov 	r10, 	c19
			mul 	r10, 	c17.z, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c18, vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[18] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c18
			mov 	r10, 	c20
			mul 	r10, 	c18, 	vPos.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c19.yxwz, vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[19] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c19
			mov 	r10, 	c21
			mul 	r10, 	c19.yxwz, 	vPos.x
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c20.wzyx, vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[20] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c20
			mov 	r10, 	c22
			mul 	r10, 	c20.wzyx, 	vPos.yx
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xw c21, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[21] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c21
			mov 	r10, 	c23
			mul 	r10.xw, 	c21, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul c22, vPos_abs.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[22] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c22
			mov 	r10, 	c24
			mul 	r10, 	c22, 	vPos_abs.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp c23, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[23] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c23
			mov 	r10, 	c25
			mul_pp 	r10, 	c23, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r8, c24";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c25, 	0.426624, 	1.000000, 	1.250000, 	0.185185
			mov 	r8, 	c25
			mov 	r10, 	c26
			mul 	r10, 	r8, 	c24
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r9.x, c25";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[25] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c26, 	9.000000, 	4.687500, 	5.000000, 	2.000000
			mov 	r9, 	c26
			mov 	r11, 	c27
			mul 	r11, 	r9.x, 	c25
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r10, c26.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[26] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c27, 	0.050000, 	0.016667, 	0.026664, 	0.010000
			mov 	r10, 	c27
			mov 	r12, 	c28
			mul 	r12, 	r10, 	c26.xyzw
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r11.ywxz, c27.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[27] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c28, 	499.999969, 	50.000004, 	100.000000, 	75.000000
			mov 	r11, 	c28
			mov 	r13, 	c29
			mul 	r13, 	r11.ywxz, 	c27.y
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r12.yzxw, c28.ywzx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[28] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c29, 	0.026664, 	0.010000, 	0.050000, 	0.016667
			mov 	r12, 	c29
			mov 	r14, 	c30
			mul 	r14, 	r12.yzxw, 	c28.ywzx
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yz r13, c29";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[29] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c30, 	9.000001, 	4.687500, 	5.000000, 	2.000000
			mov 	r13, 	c30
			mov 	r0, 	c31
			mul 	r0.yz, 	r13, 	c29
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r14_abs, -c30";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[30] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c31, 	1.250000, 	0.185185, 	0.426624, 	1.000000
			mov 	r14, 	c31
			mov 	r1, 	c0
			mul 	r1, 	r14_abs, 	-c30
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat r0, c31";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[31] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c0, 	0.050000, 	0.080000, 	0.810000, 	0.292969
			mov 	r0, 	c0
			mov 	r2, 	c1
			mul_sat 	r2, 	r0, 	c31
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r1, r2";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	6.825984, 	100.000008, 	31.249998, 	2.057613
			mov 	r2, 	c1
			mov 	r1, 	c0
			mov 	r3, 	c2
			mul 	r3, 	r1, 	r2
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r3.z, r4";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	0.810000, 	0.292969, 	0.050000, 	0.080000
			mov 	r4, 	c1
			mov 	r3, 	c0
			mov 	r5, 	c2
			mul 	r5, 	r3.z, 	r4
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r5, r6.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	1.250000, 	0.185185, 	0.426624, 	1.000000
			mov 	r6, 	c1
			mov 	r5, 	c0
			mov 	r7, 	c2
			mul 	r7, 	r5, 	r6.zxyw
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r7.zxwy, r8.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	5.000000, 	2.000000, 	9.000000, 	4.687500
			mov 	r8, 	c1
			mov 	r7, 	c0
			mov 	r9, 	c2
			mul 	r9, 	r7.zxwy, 	r8.w
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r9.wzyx, r10.zyxw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	0.026664, 	0.010000, 	0.050000, 	0.016667
			mov 	r10, 	c1
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	r9.wzyx, 	r10.zyxw
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yw r11, r12";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	100.000000, 	75.000000, 	499.999969, 	50.000004
			mov 	r12, 	c1
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13.yw, 	r11, 	r12
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -r13, -r14";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	0.050000, 	0.016667, 	0.026664, 	0.010000
			mov 	r14, 	c1
			mov 	r13, 	c0
			mov 	r0, 	c2
			mul 	r0, 	-r13, 	-r14
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp r0, r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	5.000000, 	2.000000, 	9.000001, 	4.687500
			mov 	r1, 	c1
			mov 	r0, 	c0
			mov 	r2, 	c2
			mul_pp 	r2, 	r0, 	r1
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r2, v2(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			mov 	r2, 	c0
			mov 	r4, 	c2
			mul 	r4, 	r2, 	v2
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r3.x, v3(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord3 	v3
			mov 	r3, 	c0
			mov 	r5, 	c2
			mul 	r5, 	r3.x, 	v3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r4, v4.xyzw(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			mov 	r4, 	c0
			mov 	r6, 	c2
			mul 	r6, 	r4, 	v4.xyzw
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r5.zywx, v5.y(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			mov 	r5, 	c0
			mov 	r7, 	c2
			mul 	r7, 	r5.zywx, 	v5.y
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r6.yzxw, v6.zwyx(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			mov 	r6, 	c0
			mov 	r8, 	c2
			mul 	r8, 	r6.yzxw, 	v6.zwyx
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.zw r7, v7(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			mov 	r7, 	c0
			mov 	r9, 	c2
			mul 	r9.zw, 	r7, 	v7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r8_abs, v8_abs(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord8 	v8
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul 	r10, 	r8_abs, 	v8_abs
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat r9, v9(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord9 	v9
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul_sat 	r11, 	r9, 	v9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r10, v0(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			mov 	r10, 	c0
			mov 	r12, 	c2
			mul 	r12, 	r10, 	v0
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r11.z, v1(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13, 	r11.z, 	v1
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r12, v2.zxyw(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight2 	v2
			mov 	r12, 	c0
			mov 	r14, 	c2
			mul 	r14, 	r12, 	v2.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r13.wxyz, v3.w(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight3 	v3
			mov 	r13, 	c0
			mov 	r0, 	c2
			mul 	r0, 	r13.wxyz, 	v3.w
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r14.wzyx, v4.wxzy(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight4 	v4
			mov 	r14, 	c0
			mov 	r1, 	c2
			mul 	r1, 	r14.wzyx, 	v4.wxzy
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyz r0, v5(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight5 	v5
			mov 	r0, 	c0
			mov 	r2, 	c2
			mul 	r2.xyz, 	r0, 	v5
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r1_abs, v6(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight6 	v6
			mov 	r1, 	c0
			mov 	r3, 	c2
			mul 	r3, 	r1_abs, 	v6
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp r2, v7(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight7 	v7
			mov 	r2, 	c0
			mov 	r4, 	c2
			mul_pp 	r4, 	r2, 	v7
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r3, v8(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices8 	v8
			mov 	r3, 	c0
			mov 	r5, 	c2
			mul 	r5, 	r3, 	v8
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r4.x, v9(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices9 	v9
			mov 	r4, 	c0
			mov 	r6, 	c2
			mul 	r6, 	r4.x, 	v9
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r5, v0.xyzw(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices0 	v0
			mov 	r5, 	c0
			mov 	r7, 	c2
			mul 	r7, 	r5, 	v0.xyzw
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r6.wyxz, v1.y(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices1 	v1
			mov 	r6, 	c0
			mov 	r8, 	c2
			mul 	r8, 	r6.wyxz, 	v1.y
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r7.yzxw, v2.wyzx(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			mov 	r7, 	c0
			mov 	r9, 	c2
			mul 	r9, 	r7.yzxw, 	v2.wyzx
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyw r8, v3(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul 	r10.xyw, 	r8, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r9, -v4(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices4 	v4
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	r9, 	-v4
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat r10, v5(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices5 	v5
			mov 	r10, 	c0
			mov 	r12, 	c2
			mul_sat 	r12, 	r10, 	v5
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r11, v6(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13, 	r11, 	v6
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r12.z, v7(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			mov 	r12, 	c0
			mov 	r14, 	c2
			mul 	r14, 	r12.z, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r13, v8.zxyw(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			mov 	r13, 	c0
			mov 	r0, 	c2
			mul 	r0, 	r13, 	v8.zxyw
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r14.wzxy, v9.w(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			mov 	r14, 	c0
			mov 	r1, 	c2
			mul 	r1, 	r14.wzxy, 	v9.w
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r0.wzyx, v0.xy(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal0 	v0
			mov 	r0, 	c0
			mov 	r2, 	c2
			mul 	r2, 	r0.wzyx, 	v0.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xzw r1, v1(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal1 	v1
			mov 	r1, 	c0
			mov 	r3, 	c2
			mul 	r3.xzw, 	r1, 	v1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -r2, v2_abs(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal2 	v2
			mov 	r2, 	c0
			mov 	r4, 	c2
			mul 	r4, 	-r2, 	v2_abs
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp r3, v3(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal3 	v3
			mov 	r3, 	c0
			mov 	r5, 	c2
			mul_pp 	r5, 	r3, 	v3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r4, v4(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent4 	v4
			mov 	r4, 	c0
			mov 	r6, 	c2
			mul 	r6, 	r4, 	v4
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r5.x, v5(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent5 	v5
			mov 	r5, 	c0
			mov 	r7, 	c2
			mul 	r7, 	r5.x, 	v5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r6, v6.xyzw(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent6 	v6
			mov 	r6, 	c0
			mov 	r8, 	c2
			mul 	r8, 	r6, 	v6.xyzw
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r7.xz, v7.y(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent7 	v7
			mov 	r7, 	c0
			mov 	r9, 	c2
			mul 	r9, 	r7.xz, 	v7.y
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r8.yzxw, v8.xw(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent8 	v8
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul 	r10, 	r8.yzxw, 	v8.xw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yzw r9, v9(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent9 	v9
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11.yzw, 	r9, 	v9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -r10, -v0(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent0 	v0
			mov 	r10, 	c0
			mov 	r12, 	c2
			mul 	r12, 	-r10, 	-v0
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat r11, v1(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent1 	v1
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul_sat 	r13, 	r11, 	v1
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r12, v2(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal2 	v2
			mov 	r12, 	c0
			mov 	r14, 	c2
			mul 	r14, 	r12, 	v2
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r13.z, v3(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal3 	v3
			mov 	r13, 	c0
			mov 	r0, 	c2
			mul 	r0, 	r13.z, 	v3
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r14, v4.zxyw(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			mov 	r14, 	c0
			mov 	r1, 	c2
			mul 	r1, 	r14, 	v4.zxyw
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r0.yx, v5.w(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			mov 	r0, 	c0
			mov 	r2, 	c2
			mul 	r2, 	r0.yx, 	v5.w
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r1.wzyx, v6.yz(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			mov 	r1, 	c0
			mov 	r3, 	c2
			mul 	r3, 	r1.wzyx, 	v6.yz
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyzw r2, v7(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			mov 	r2, 	c0
			mov 	r4, 	c2
			mul 	r4.xyzw, 	r2, 	v7
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r3_abs, v8_abs(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal8 	v8
			mov 	r3, 	c0
			mov 	r5, 	c2
			mul 	r5, 	r3_abs, 	v8_abs
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp r4, v9(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal9 	v9
			mov 	r4, 	c0
			mov 	r6, 	c2
			mul_pp 	r6, 	r4, 	v9
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r5, v0(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			mov 	r5, 	c0
			mov 	r7, 	c2
			mul 	r7, 	r5, 	v0
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r6.x, v1(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			mov 	r6, 	c0
			mov 	r8, 	c2
			mul 	r8, 	r6.x, 	v1
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r7, v2.xyzw(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color2 	v2
			mov 	r7, 	c0
			mov 	r9, 	c2
			mul 	r9, 	r7, 	v2.xyzw
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r8.yw, v3.y(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color3 	v3
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul 	r10, 	r8.yw, 	v3.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r9.yzxw, v4.zx(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color4 	v4
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	r9.yzxw, 	v4.zx
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.x r10, v5(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color5 	v5
			mov 	r10, 	c0
			mov 	r12, 	c2
			mul 	r12.x, 	r10, 	v5
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -r11, v6(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color6 	v6
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13, 	-r11, 	v6
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat r12, v7(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color7 	v7
			mov 	r12, 	c0
			mov 	r14, 	c2
			mul_sat 	r14, 	r12, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r13, v8(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog8 	v8
			mov 	r13, 	c0
			mov 	r0, 	c2
			mul 	r0, 	r13, 	v8
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r14.z, v9(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog9 	v9
			mov 	r14, 	c0
			mov 	r1, 	c2
			mul 	r1, 	r14.z, 	v9
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r0, v0.zxyw(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog0 	v0
			mov 	r0, 	c0
			mov 	r2, 	c2
			mul 	r2, 	r0, 	v0.zxyw
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r1.zy, v1.w(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog1 	v1
			mov 	r1, 	c0
			mov 	r3, 	c2
			mul 	r3, 	r1.zy, 	v1.w
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r2.wzyx, v2.zw(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			mov 	r2, 	c0
			mov 	r4, 	c2
			mul 	r4, 	r2.wzyx, 	v2.zw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.y r3, v3(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			mov 	r3, 	c0
			mov 	r5, 	c2
			mul 	r5.y, 	r3, 	v3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r4, v4_abs(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog4 	v4
			mov 	r4, 	c0
			mov 	r6, 	c2
			mul 	r6, 	r4, 	v4_abs
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp r5, v5(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog5 	v5
			mov 	r5, 	c0
			mov 	r7, 	c2
			mul_pp 	r7, 	r5, 	v5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r6, v6(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			mov 	r6, 	c0
			mov 	r8, 	c2
			mul 	r8, 	r6, 	v6
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r7.x, v7(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			mov 	r7, 	c0
			mov 	r9, 	c2
			mul 	r9, 	r7.x, 	v7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r8, v8.xyzw(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul 	r10, 	r8, 	v8.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r9.wx, v9.y(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	r9.wx, 	v9.y
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r10.yzxw, v0.wy(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth0 	v0
			mov 	r10, 	c0
			mov 	r12, 	c2
			mul 	r12, 	r10.yzxw, 	v0.wy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.z r11, v1(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth1 	v1
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13.z, 	r11, 	v1
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r12_abs, -v2(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth2 	v2
			mov 	r12, 	c0
			mov 	r14, 	c2
			mul 	r14, 	r12_abs, 	-v2
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat r13, v3(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth3 	v3
			mov 	r13, 	c0
			mov 	r0, 	c2
			mul_sat 	r0, 	r13, 	v3
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r14, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r14, 	c0
			mov 	r1, 	c2
			mul 	r1, 	r14, 	vPos.xy
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r0.z, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r0, 	c0
			mov 	r2, 	c2
			mul 	r2, 	r0.z, 	vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r1, vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r1, 	c0
			mov 	r3, 	c2
			mul 	r3, 	r1, 	vPos.y
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r2.wz, vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r2, 	c0
			mov 	r4, 	c2
			mul 	r4, 	r2.wz, 	vPos.x
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul r3.wzyx, vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r3, 	c0
			mov 	r5, 	c2
			mul 	r5, 	r3.wzyx, 	vPos.yx
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.w r4, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r4, 	c0
			mov 	r6, 	c2
			mul 	r6.w, 	r4, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -r5, -vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r5, 	c0
			mov 	r7, 	c2
			mul 	r7, 	-r5, 	-vPos.xy
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp r6, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r6, 	c0
			mov 	r8, 	c2
			mul_pp 	r8, 	r6, 	vPos.xy
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(texcoord), c0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			mov 	r7, 	c0
			mov 	r9, 	c2
			mul 	r9, 	v4, 	c0
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.x(texcoord), c1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[1] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			mov 	r7, 	c1
			mov 	r9, 	c3
			mul 	r9, 	v5.x, 	c1
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(texcoord), c2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[2] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			mov 	r7, 	c2
			mov 	r9, 	c4
			mul 	r9, 	v6, 	c2.xyzw
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v7.xyw(texcoord), c3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[3] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			mov 	r7, 	c3
			mov 	r9, 	c5
			mul 	r9, 	v7.xyw, 	c3.y
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8.yzxw(texcoord), c4.xzy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[4] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord8 	v8
			mov 	r7, 	c4
			mov 	r9, 	c6
			mul 	r9, 	v8.yzxw, 	c4.xzy
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xy v9(texcoord), c5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[5] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord9 	v9
			mov 	r7, 	c5
			mov 	r9, 	c7
			mul 	r9.xy, 	v9, 	c5
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0_abs(texcoord), c6_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[6] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			mov 	r7, 	c6
			mov 	r9, 	c8
			mul 	r9, 	v0_abs, 	c6_abs
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v1(texcoord), c7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[7] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord1 	v1
			mov 	r7, 	c7
			mov 	r9, 	c9
			mul_sat 	r9, 	v1, 	c7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2(texcoord), r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			mov 	r7, 	c8
			mov 	r9, 	c11
			mul 	r9, 	v2, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v3.z(texcoord), r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord3 	v3
			mov 	r8, 	c8
			mov 	r10, 	c11
			mul 	r10, 	v3.z, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(texcoord), r9.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			mov 	r9, 	c8
			mov 	r11, 	c11
			mul 	r11, 	v4, 	r9.zxyw
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.xzw(texcoord), r10.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			mov 	r10, 	c8
			mov 	r12, 	c11
			mul 	r12, 	v5.xzw, 	r10.w
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6.wzyx(texcoord), r11.xwy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			mov 	r11, 	c8
			mov 	r13, 	c11
			mul 	r13, 	v6.wzyx, 	r11.xwy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xz v7(texcoord), r12";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			mov 	r12, 	c8
			mov 	r14, 	c11
			mul 	r14.xz, 	v7, 	r12
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8_abs(texcoord), r13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord8 	v8
			mov 	r13, 	c8
			mov 	r0, 	c11
			mul 	r0, 	v8_abs, 	r13
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v9(texcoord), r14";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord9 	v9
			mov 	r14, 	c8
			mov 	r1, 	c11
			mul_pp 	r1, 	v9, 	r14
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0(texcoord), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul 	r2, 	v0, 	vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v1.x(texcoord), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord1 	v1
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul 	r2, 	v1.x, 	vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2(texcoord), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul 	r2, 	v2, 	vPos.y
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v3.xwz(texcoord), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord3 	v3
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul 	r2, 	v3.xwz, 	vPos.x
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4.yzxw(texcoord), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul 	r2, 	v4.yzxw, 	vPos.yx
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xw v5(texcoord), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul 	r2.xw, 	v5, 	vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(texcoord), -vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul 	r2, 	v6, 	-vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v7(texcoord), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul_sat 	r2, 	v7, 	vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8(blendweight), c8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight8 	v8
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul 	r2, 	v8, 	c8
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v9.z(blendweight), c9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[9] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight9 	v9
			mov 	r0, 	c9
			mov 	r2, 	c11
			mul 	r2, 	v9.z, 	c9
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0(blendweight), c11.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			mov 	r0, 	c11
			mov 	r2, 	c12
			mul 	r2, 	v0, 	c11.zxyw
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v1.yxw(blendweight), c11.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			mov 	r0, 	c11
			mov 	r2, 	c13
			mul 	r2, 	v1.yxw, 	c11.w
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2.wzyx(blendweight), c12.yzx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[12] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight2 	v2
			mov 	r0, 	c12
			mov 	r2, 	c14
			mul 	r2, 	v2.wzyx, 	c12.yzx
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yz v3(blendweight), c13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[13] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight3 	v3
			mov 	r0, 	c13
			mov 	r2, 	c15
			mul 	r2.yz, 	v3, 	c13
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -v4(blendweight), c14_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[14] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight4 	v4
			mov 	r0, 	c14
			mov 	r2, 	c16
			mul 	r2, 	-v4, 	c14_abs
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v5(blendweight), c15";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[15] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight5 	v5
			mov 	r0, 	c15
			mov 	r2, 	c17
			mul_pp 	r2, 	v5, 	c15
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(blendweight), r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight6 	v6
			mov 	r0, 	c16
			mov 	r2, 	c18
			mul 	r2, 	v6, 	r0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v7.x(blendweight), r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight7 	v7
			mov 	r1, 	c16
			mov 	r3, 	c18
			mul 	r3, 	v7.x, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8(blendweight), r2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight8 	v8
			mov 	r2, 	c16
			mov 	r4, 	c18
			mul 	r4, 	v8, 	r2.xyzw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v9.yzw(blendweight), r3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight9 	v9
			mov 	r3, 	c16
			mov 	r5, 	c18
			mul 	r5, 	v9.yzw, 	r3.y
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0.yzxw(blendweight), r4.ywx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			mov 	r4, 	c16
			mov 	r6, 	c18
			mul 	r6, 	v0.yzxw, 	r4.ywx
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yw v1(blendweight), r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			mov 	r5, 	c16
			mov 	r7, 	c18
			mul 	r7.yw, 	v1, 	r5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -v2(blendweight), -r6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight2 	v2
			mov 	r6, 	c16
			mov 	r8, 	c18
			mul 	r8, 	-v2, 	-r6
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v3(blendweight), r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight3 	v3
			mov 	r7, 	c16
			mov 	r9, 	c18
			mul_sat 	r9, 	v3, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(blendweight), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight4 	v4
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	v4, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.z(blendweight), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight5 	v5
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	v5.z, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(blendweight), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight6 	v6
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	v6, 	vPos.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v7.ywz(blendweight), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight7 	v7
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	v7.ywz, 	vPos.x
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8.wzyx(blendweight), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight8 	v8
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	v8.wzyx, 	vPos.yx
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.zw v9(blendweight), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight9 	v9
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10.zw, 	v9, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0_abs(blendweight), vPos_abs.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	v0_abs, 	vPos_abs.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v1(blendweight), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul_pp 	r10, 	v1, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2(blendindices), c16";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	v2, 	c16
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v3.x(blendindices), c17";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[17] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			mov 	r8, 	c17
			mov 	r10, 	c19
			mul 	r10, 	v3.x, 	c17
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(blendindices), c18.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[18] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices4 	v4
			mov 	r8, 	c18
			mov 	r10, 	c20
			mul 	r10, 	v4, 	c18.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.zxw(blendindices), c19.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[19] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices5 	v5
			mov 	r8, 	c19
			mov 	r10, 	c21
			mul 	r10, 	v5.zxw, 	c19.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6.yzxw(blendindices), c20.zyx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[20] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices6 	v6
			mov 	r8, 	c20
			mov 	r10, 	c22
			mul 	r10, 	v6.yzxw, 	c20.zyx
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyz v7(blendindices), c21";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[21] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices7 	v7
			mov 	r8, 	c21
			mov 	r10, 	c23
			mul 	r10.xyz, 	v7, 	c21
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -v8(blendindices), c22";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[22] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices8 	v8
			mov 	r8, 	c22
			mov 	r10, 	c24
			mul 	r10, 	-v8, 	c22
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v9(blendindices), c23";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[23] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices9 	v9
			mov 	r8, 	c23
			mov 	r10, 	c25
			mul_sat 	r10, 	v9, 	c23
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0(blendindices), r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices0 	v0
			mov 	r8, 	c24
			mov 	r10, 	c26
			mul 	r10, 	v0, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v1.z(blendindices), r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices1 	v1
			mov 	r9, 	c24
			mov 	r11, 	c26
			mul 	r11, 	v1.z, 	r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2(blendindices), r10.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			mov 	r10, 	c24
			mov 	r12, 	c26
			mul 	r12, 	v2, 	r10.zxyw
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v3.zyw(blendindices), r11.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			mov 	r11, 	c24
			mov 	r13, 	c26
			mul 	r13, 	v3.zyw, 	r11.w
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4.wzyx(blendindices), r12.zwx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices4 	v4
			mov 	r12, 	c24
			mov 	r14, 	c26
			mul 	r14, 	v4.wzyx, 	r12.zwx
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyw v5(blendindices), r13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices5 	v5
			mov 	r13, 	c24
			mov 	r0, 	c26
			mul 	r0.xyw, 	v5, 	r13
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(blendindices), r14_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices6 	v6
			mov 	r14, 	c24
			mov 	r1, 	c26
			mul 	r1, 	v6, 	r14_abs
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v7(blendindices), r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices7 	v7
			mov 	r0, 	c24
			mov 	r2, 	c26
			mul_pp 	r2, 	v7, 	r0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8(blendindices), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices8 	v8
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			mul 	r3, 	v8, 	vPos.xy
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v9.x(blendindices), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices9 	v9
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			mul 	r3, 	v9.x, 	vPos.xy
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0(blendindices), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices0 	v0
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			mul 	r3, 	v0, 	vPos.y
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v1.zwy(blendindices), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices1 	v1
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			mul 	r3, 	v1.zwy, 	vPos.x
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2.yzxw(blendindices), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			mul 	r3, 	v2.yzxw, 	vPos.yx
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xzw v3(blendindices), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			mul 	r3.xzw, 	v3, 	vPos.xy
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4_abs(blendindices), -vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices4 	v4
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			mul 	r3, 	v4_abs, 	-vPos.xy
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v5(blendindices), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices5 	v5
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			mul_sat 	r3, 	v5, 	vPos.xy
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(normal), c24";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			mov 	r1, 	c24
			mov 	r3, 	c26
			mul 	r3, 	v6, 	c24
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v7.z(normal), c25";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[25] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			mov 	r1, 	c25
			mov 	r3, 	c27
			mul 	r3, 	v7.z, 	c25
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8(normal), c26.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[26] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			mov 	r1, 	c26
			mov 	r3, 	c28
			mul 	r3, 	v8, 	c26.zxyw
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v9.wxz(normal), c27.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[27] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			mov 	r1, 	c27
			mov 	r3, 	c29
			mul 	r3, 	v9.wxz, 	c27.w
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0.wzyx(normal), c28.wyx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[28] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal0 	v0
			mov 	r1, 	c28
			mov 	r3, 	c30
			mul 	r3, 	v0.wzyx, 	c28.wyx
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yzw v1(normal), c29";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[29] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal1 	v1
			mov 	r1, 	c29
			mov 	r3, 	c31
			mul 	r3.yzw, 	v1, 	c29
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -v2(normal), -c30";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[30] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal2 	v2
			mov 	r1, 	c30
			mov 	r3, 	c0
			mul 	r3, 	-v2, 	-c30
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v3(normal), c31";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[31] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal3 	v3
			mov 	r1, 	c31
			mov 	r3, 	c1
			mul_pp 	r3, 	v3, 	c31
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(normal), r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal4 	v4
			mov 	r1, 	c0
			mov 	r3, 	c2
			mul 	r3, 	v4, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.x(normal), r2";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal5 	v5
			mov 	r2, 	c0
			mov 	r4, 	c2
			mul 	r4, 	v5.x, 	r2
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(normal), r3.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			mov 	r3, 	c0
			mov 	r5, 	c2
			mul 	r5, 	v6, 	r3.xyzw
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v7.wyz(normal), r4.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			mov 	r4, 	c0
			mov 	r6, 	c2
			mul 	r6, 	v7.wyz, 	r4.y
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8.yzxw(normal), r5.wzx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			mov 	r5, 	c0
			mov 	r7, 	c2
			mul 	r7, 	v8.yzxw, 	r5.wzx
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyzw v9(normal), r6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			mov 	r6, 	c0
			mov 	r8, 	c2
			mul 	r8.xyzw, 	v9, 	r6
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0_abs(normal), r7_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal0 	v0
			mov 	r7, 	c0
			mov 	r9, 	c2
			mul 	r9, 	v0_abs, 	r7_abs
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v1(normal), r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal1 	v1
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul_sat 	r10, 	v1, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2(normal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal2 	v2
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	v2, 	vPos.xy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v3.z(normal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal3 	v3
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	v3.z, 	vPos.xy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(normal), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal4 	v4
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	v4, 	vPos.y
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.wzy(normal), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal5 	v5
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	v5.wzy, 	vPos.x
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6.wzyx(normal), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	v6.wzyx, 	vPos.yx
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.x v7(normal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11.x, 	v7, 	vPos.xy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8_abs(normal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	v8_abs, 	vPos.xy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v9(normal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul_pp 	r11, 	v9, 	vPos.xy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0(tangent), c0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent0 	v0
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	v0, 	c0
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v1.x(tangent), c1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[1] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent1 	v1
			mov 	r9, 	c1
			mov 	r11, 	c3
			mul 	r11, 	v1.x, 	c1
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2(tangent), c2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[2] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent2 	v2
			mov 	r9, 	c2
			mov 	r11, 	c4
			mul 	r11, 	v2, 	c2.xyzw
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v3.xzyw(tangent), c3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[3] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent3 	v3
			mov 	r9, 	c3
			mov 	r11, 	c5
			mul 	r11, 	v3.xzyw, 	c3.y
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4.yzxw(tangent), c4.xzwy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[4] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent4 	v4
			mov 	r9, 	c4
			mov 	r11, 	c6
			mul 	r11, 	v4.yzxw, 	c4.xzwy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.y v5(tangent), c5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[5] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent5 	v5
			mov 	r9, 	c5
			mov 	r11, 	c7
			mul 	r11.y, 	v5, 	c5
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(tangent), -c6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[6] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent6 	v6
			mov 	r9, 	c6
			mov 	r11, 	c8
			mul 	r11, 	v6, 	-c6
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v7(tangent), c7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[7] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent7 	v7
			mov 	r9, 	c7
			mov 	r11, 	c9
			mul_sat 	r11, 	v7, 	c7
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8(tangent), r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent8 	v8
			mov 	r9, 	c8
			mov 	r11, 	c11
			mul 	r11, 	v8, 	r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v9.z(tangent), r10";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent9 	v9
			mov 	r10, 	c8
			mov 	r12, 	c11
			mul 	r12, 	v9.z, 	r10
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0(tangent), r11.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent0 	v0
			mov 	r11, 	c8
			mov 	r13, 	c11
			mul 	r13, 	v0, 	r11.zxyw
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v1.xwyz(tangent), r12.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent1 	v1
			mov 	r12, 	c8
			mov 	r14, 	c11
			mul 	r14, 	v1.xwyz, 	r12.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2.wzyx(tangent), r13.xwzy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent2 	v2
			mov 	r13, 	c8
			mov 	r0, 	c11
			mul 	r0, 	v2.wzyx, 	r13.xwzy
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.z v3(tangent), r14";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent3 	v3
			mov 	r14, 	c8
			mov 	r1, 	c11
			mul 	r1.z, 	v3, 	r14
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -v4(tangent), r0_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent4 	v4
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul 	r2, 	-v4, 	r0_abs
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v5(tangent), r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent5 	v5
			mov 	r1, 	c8
			mov 	r3, 	c11
			mul_pp 	r3, 	v5, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(tangent), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent6 	v6
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			mul 	r4, 	v6, 	vPos.xy
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v7.x(tangent), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent7 	v7
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			mul 	r4, 	v7.x, 	vPos.xy
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8(tangent), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent8 	v8
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			mul 	r4, 	v8, 	vPos.y
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v9.yxzw(tangent), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent9 	v9
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			mul 	r4, 	v9.yxzw, 	vPos.x
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0.yzxw(tangent), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent0 	v0
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			mul 	r4, 	v0.yzxw, 	vPos.yx
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.w v1(tangent), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent1 	v1
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			mul 	r4.w, 	v1, 	vPos.xy
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -v2(tangent), -vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent2 	v2
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			mul 	r4, 	-v2, 	-vPos.xy
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v3(tangent), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent3 	v3
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			mul_sat 	r4, 	v3, 	vPos.xy
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(binormal), c8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			mov 	r2, 	c8
			mov 	r4, 	c11
			mul 	r4, 	v4, 	c8
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.z(binormal), c9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[9] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			mov 	r2, 	c9
			mov 	r4, 	c11
			mul 	r4, 	v5.z, 	c9
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(binormal), c11.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			mov 	r2, 	c11
			mov 	r4, 	c12
			mul 	r4, 	v6, 	c11.zxyw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v7.yzwx(binormal), c11.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			mov 	r2, 	c11
			mov 	r4, 	c13
			mul 	r4, 	v7.yzwx, 	c11.w
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8.wzyx(binormal), c12.ywxz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[12] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal8 	v8
			mov 	r2, 	c12
			mov 	r4, 	c14
			mul 	r4, 	v8.wzyx, 	c12.ywxz
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xy v9(binormal), c13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[13] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal9 	v9
			mov 	r2, 	c13
			mov 	r4, 	c15
			mul 	r4.xy, 	v9, 	c13
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0_abs(binormal), c14_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[14] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal0 	v0
			mov 	r2, 	c14
			mov 	r4, 	c16
			mul 	r4, 	v0_abs, 	c14_abs
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v1(binormal), c15";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[15] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal1 	v1
			mov 	r2, 	c15
			mov 	r4, 	c17
			mul_pp 	r4, 	v1, 	c15
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2(binormal), r2";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal2 	v2
			mov 	r2, 	c16
			mov 	r4, 	c18
			mul 	r4, 	v2, 	r2
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v3.x(binormal), r3";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal3 	v3
			mov 	r3, 	c16
			mov 	r5, 	c18
			mul 	r5, 	v3.x, 	r3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(binormal), r4.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			mov 	r4, 	c16
			mov 	r6, 	c18
			mul 	r6, 	v4, 	r4.xyzw
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.ywzx(binormal), r5.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			mov 	r5, 	c16
			mov 	r7, 	c18
			mul 	r7, 	v5.ywzx, 	r5.y
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6.yzxw(binormal), r6.zxwy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			mov 	r6, 	c16
			mov 	r8, 	c18
			mul 	r8, 	v6.yzxw, 	r6.zxwy
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xz v7(binormal), r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			mov 	r7, 	c16
			mov 	r9, 	c18
			mul 	r9.xz, 	v7, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -v8(binormal), r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal8 	v8
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	-v8, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v9(binormal), r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal9 	v9
			mov 	r9, 	c16
			mov 	r11, 	c18
			mul_sat 	r11, 	v9, 	r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0(binormal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal0 	v0
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			mul 	r12, 	v0, 	vPos.xy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v1.z(binormal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal1 	v1
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			mul 	r12, 	v1.z, 	vPos.xy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2(binormal), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal2 	v2
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			mul 	r12, 	v2, 	vPos.y
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v3.zyxw(binormal), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal3 	v3
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			mul 	r12, 	v3.zyxw, 	vPos.x
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4.wzyx(binormal), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			mul 	r12, 	v4.wzyx, 	vPos.yx
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xw v5(binormal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			mul 	r12.xw, 	v5, 	vPos.xy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(binormal), vPos_abs.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			mul 	r12, 	v6, 	vPos_abs.xy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v7(binormal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			mul_pp 	r12, 	v7, 	vPos.xy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8(color), c16";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color8 	v8
			mov 	r10, 	c16
			mov 	r12, 	c18
			mul 	r12, 	v8, 	c16
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v9.x(color), c17";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[17] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color9 	v9
			mov 	r10, 	c17
			mov 	r12, 	c19
			mul 	r12, 	v9.x, 	c17
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0(color), c18.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[18] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			mov 	r10, 	c18
			mov 	r12, 	c20
			mul 	r12, 	v0, 	c18.xyzw
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v1.zwyx(color), c19.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[19] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			mov 	r10, 	c19
			mov 	r12, 	c21
			mul 	r12, 	v1.zwyx, 	c19.y
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2.yzxw(color), c20.wxyz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[20] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color2 	v2
			mov 	r10, 	c20
			mov 	r12, 	c22
			mul 	r12, 	v2.yzxw, 	c20.wxyz
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yz v3(color), c21";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[21] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color3 	v3
			mov 	r10, 	c21
			mov 	r12, 	c23
			mul 	r12.yz, 	v3, 	c21
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4_abs(color), -c22";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[22] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color4 	v4
			mov 	r10, 	c22
			mov 	r12, 	c24
			mul 	r12, 	v4_abs, 	-c22
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v5(color), c23";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[23] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color5 	v5
			mov 	r10, 	c23
			mov 	r12, 	c25
			mul_sat 	r12, 	v5, 	c23
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(color), r10";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color6 	v6
			mov 	r10, 	c24
			mov 	r12, 	c26
			mul 	r12, 	v6, 	r10
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v7.z(color), r11";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color7 	v7
			mov 	r11, 	c24
			mov 	r13, 	c26
			mul 	r13, 	v7.z, 	r11
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8(color), r12.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color8 	v8
			mov 	r12, 	c24
			mov 	r14, 	c26
			mul 	r14, 	v8, 	r12.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v9.wxzy(color), r13.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color9 	v9
			mov 	r13, 	c24
			mov 	r0, 	c26
			mul 	r0, 	v9.wxzy, 	r13.w
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0.wzyx(color), r14.wyxz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			mov 	r14, 	c24
			mov 	r1, 	c26
			mul 	r1, 	v0.wzyx, 	r14.wyxz
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yw v1(color), r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			mov 	r0, 	c24
			mov 	r2, 	c26
			mul 	r2.yw, 	v1, 	r0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -v2(color), -r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color2 	v2
			mov 	r1, 	c24
			mov 	r3, 	c26
			mul 	r3, 	-v2, 	-r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v3(color), r2";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color3 	v3
			mov 	r2, 	c24
			mov 	r4, 	c26
			mul_pp 	r4, 	v3, 	r2
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(color), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color4 	v4
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			mul 	r5, 	v4, 	vPos.xy
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.x(color), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color5 	v5
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			mul 	r5, 	v5.x, 	vPos.xy
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(color), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color6 	v6
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			mul 	r5, 	v6, 	vPos.y
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v7.wyzx(color), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color7 	v7
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			mul 	r5, 	v7.wyzx, 	vPos.x
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8.yzxw(color), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color8 	v8
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			mul 	r5, 	v8.yzxw, 	vPos.yx
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.zw v9(color), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color9 	v9
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			mul 	r5.zw, 	v9, 	vPos.xy
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0_abs(color), vPos_abs.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			mul 	r5, 	v0_abs, 	vPos_abs.xy
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v1(color), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			mul_sat 	r5, 	v1, 	vPos.xy
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2(fog), c24";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[24] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			mov 	r3, 	c24
			mov 	r5, 	c26
			mul 	r5, 	v2, 	c24
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v3.z(fog), c25";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[25] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			mov 	r3, 	c25
			mov 	r5, 	c27
			mul 	r5, 	v3.z, 	c25
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(fog), c26.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[26] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog4 	v4
			mov 	r3, 	c26
			mov 	r5, 	c28
			mul 	r5, 	v4, 	c26.zxyw
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.xy(fog), c27.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[27] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog5 	v5
			mov 	r3, 	c27
			mov 	r5, 	c29
			mul 	r5, 	v5.xy, 	c27.w
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6.wzyx(fog), c28.xz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[28] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog6 	v6
			mov 	r3, 	c28
			mov 	r5, 	c30
			mul 	r5, 	v6.wzyx, 	c28.xz
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyz v7(fog), c29";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[29] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog7 	v7
			mov 	r3, 	c29
			mov 	r5, 	c31
			mul 	r5.xyz, 	v7, 	c29
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8_abs(fog), c30";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[30] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog8 	v8
			mov 	r3, 	c30
			mov 	r5, 	c0
			mul 	r5, 	v8_abs, 	c30
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v9(fog), c31";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[31] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog9 	v9
			mov 	r3, 	c31
			mov 	r5, 	c1
			mul_pp 	r5, 	v9, 	c31
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0(fog), r3";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog0 	v0
			mov 	r3, 	c0
			mov 	r5, 	c2
			mul 	r5, 	v0, 	r3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v1.x(fog), r4";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog1 	v1
			mov 	r4, 	c0
			mov 	r6, 	c2
			mul 	r6, 	v1.x, 	r4
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2(fog), r5.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			mov 	r5, 	c0
			mov 	r7, 	c2
			mul 	r7, 	v2, 	r5.xyzw
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v3.xw(fog), r6.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			mov 	r6, 	c0
			mov 	r8, 	c2
			mul 	r8, 	v3.xw, 	r6.y
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4.yzxw(fog), r7.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog4 	v4
			mov 	r7, 	c0
			mov 	r9, 	c2
			mul 	r9, 	v4.yzxw, 	r7.yx
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyw v5(fog), r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog5 	v5
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul 	r10.xyw, 	v5, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(fog), -r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog6 	v6
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	v6, 	-r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v7(fog), r10";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog7 	v7
			mov 	r10, 	c0
			mov 	r12, 	c2
			mul_sat 	r12, 	v7, 	r10
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8(fog), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog8 	v8
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13, 	v8, 	vPos.xy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v9.z(fog), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog9 	v9
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13, 	v9.z, 	vPos.xy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0(fog), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog0 	v0
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13, 	v0, 	vPos.y
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v1.yz(fog), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog1 	v1
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13, 	v1.yz, 	vPos.x
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2.wzyx(fog), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13, 	v2.wzyx, 	vPos.yx
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xzw v3(fog), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13.xzw, 	v3, 	vPos.xy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -v4(fog), vPos_abs.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog4 	v4
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13, 	-v4, 	vPos_abs.xy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v5(fog), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog5 	v5
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul_pp 	r13, 	v5, 	vPos.xy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(depth), c0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[0] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			mov 	r11, 	c0
			mov 	r13, 	c2
			mul 	r13, 	v6, 	c0
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v7.x(depth), c1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[1] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			mov 	r11, 	c1
			mov 	r13, 	c3
			mul 	r13, 	v7.x, 	c1
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8(depth), c2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[2] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			mov 	r11, 	c2
			mov 	r13, 	c4
			mul 	r13, 	v8, 	c2.xyzw
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v9.zx(depth), c3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[3] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			mov 	r11, 	c3
			mov 	r13, 	c5
			mul 	r13, 	v9.zx, 	c3.y
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0.yzxw(depth), c4.zy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[4] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth0 	v0
			mov 	r11, 	c4
			mov 	r13, 	c6
			mul 	r13, 	v0.yzxw, 	c4.zy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yzw v1(depth), c5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[5] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth1 	v1
			mov 	r11, 	c5
			mov 	r13, 	c7
			mul 	r13.yzw, 	v1, 	c5
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -v2(depth), -c6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[6] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth2 	v2
			mov 	r11, 	c6
			mov 	r13, 	c8
			mul 	r13, 	-v2, 	-c6
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v3(depth), c7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[7] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth3 	v3
			mov 	r11, 	c7
			mov 	r13, 	c9
			mul_sat 	r13, 	v3, 	c7
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(depth), r11";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth4 	v4
			mov 	r11, 	c8
			mov 	r13, 	c11
			mul 	r13, 	v4, 	r11
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.z(depth), r12";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth5 	v5
			mov 	r12, 	c8
			mov 	r14, 	c11
			mul 	r14, 	v5.z, 	r12
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6(depth), r13.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			mov 	r13, 	c8
			mov 	r0, 	c11
			mul 	r0, 	v6, 	r13.zxyw
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v7.zw(depth), r14.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			mov 	r14, 	c8
			mov 	r1, 	c11
			mul 	r1, 	v7.zw, 	r14.w
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v8.wzyx(depth), r0.wx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul 	r2, 	v8.wzyx, 	r0.wx
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyzw v9(depth), r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			mov 	r1, 	c8
			mov 	r3, 	c11
			mul 	r3.xyzw, 	v9, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v0_abs(depth), r2_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth0 	v0
			mov 	r2, 	c8
			mov 	r4, 	c11
			mul 	r4, 	v0_abs, 	r2_abs
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp v1(depth), r3";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth1 	v1
			mov 	r3, 	c8
			mov 	r5, 	c11
			mul_pp 	r5, 	v1, 	r3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v2(depth), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth2 	v2
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			mul 	r6, 	v2, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v3.x(depth), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth3 	v3
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			mul 	r6, 	v3.x, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v4(depth), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth4 	v4
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			mul 	r6, 	v4, 	vPos.y
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v5.wy(depth), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth5 	v5
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			mul 	r6, 	v5.wy, 	vPos.x
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul v6.yzxw(depth), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			mul 	r6, 	v6.yzxw, 	vPos.yx
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.x v7(depth), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			mul 	r6.x, 	v7, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -v8(depth), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			mul 	r6, 	-v8, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat v9(depth), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			mul_sat 	r6, 	v9, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, c8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[8] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			mul 	r6, 	vPos.xy, 	c8
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.x, c9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[9] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c9
			mov 	r6, 	c11
			mul 	r6, 	vPos.x, 	c9
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, c11.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c11
			mov 	r6, 	c12
			mul 	r6, 	vPos.xy, 	c11.zxyw
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.yx, c11.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[11] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c11
			mov 	r6, 	c13
			mul 	r6, 	vPos.yx, 	c11.w
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.y, c12.xyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[12] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c12
			mov 	r6, 	c14
			mul 	r6, 	vPos.y, 	c12.xyw
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.y vPos.xy, c13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[13] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c13
			mov 	r6, 	c15
			mul 	r6.y, 	vPos.xy, 	c13
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, c14_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[14] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c14
			mov 	r6, 	c16
			mul 	r6, 	vPos.xy, 	c14_abs
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp vPos.xy, c15";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[15] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c15
			mov 	r6, 	c17
			mul_pp 	r6, 	vPos.xy, 	c15
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, r4";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c16
			mov 	r6, 	c18
			mul 	r6, 	vPos.xy, 	r4
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.x, r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r5, 	c16
			mov 	r7, 	c18
			mul 	r7, 	vPos.x, 	r5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, r6.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r6, 	c16
			mov 	r8, 	c18
			mul 	r8, 	vPos.xy, 	r6.xyzw
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.yx, r7.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r7, 	c16
			mov 	r9, 	c18
			mul 	r9, 	vPos.yx, 	r7.y
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.y, r8.xzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	vPos.y, 	r8.xzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.z vPos.xy, r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r9, 	c16
			mov 	r11, 	c18
			mul 	r11.z, 	vPos.xy, 	r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos_abs.xy, -r10";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			mul 	r12, 	vPos_abs.xy, 	-r10
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat vPos.xy, r11";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r11, 	c16
			mov 	r13, 	c18
			mul_sat 	r13, 	vPos.xy, 	r11
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v0(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v0
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.x, v1(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.x, 	v1
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v2.zxyw(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v2.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.yx, v3.w(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.yx, 	v3.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.y, v4.xwz(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.y, 	v4.xwz
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.w vPos.xy, v5(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14.w, 	vPos.xy, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -vPos.xy, -v6(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	-vPos.xy, 	-v6
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp vPos.xy, v7(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_texcoord7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul_pp 	r14, 	vPos.xy, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v8(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v8
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.x, v9(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.x, 	v9
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v0.xyzw(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v0.xyzw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.yx, v1.y(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.yx, 	v1.y
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.y, v2.yxw(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.y, 	v2.yxw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xy vPos.xy, v3(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14.xy, 	vPos.xy, 	v3
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos_abs.xy, v4_abs(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos_abs.xy, 	v4_abs
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat vPos.xy, v5(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendweight5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul_sat 	r14, 	vPos.xy, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v6(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v6
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.x, v7(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.x, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v8.zxyw(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v8.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.yx, v9.w(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.yx, 	v9.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.y, v0.yzw(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.y, 	v0.yzw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xz vPos.xy, v1(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14.xz, 	vPos.xy, 	v1
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos_abs.xy, v2(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos_abs.xy, 	v2
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp vPos.xy, v3(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_blendindices3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul_pp 	r14, 	vPos.xy, 	v3
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v4(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v4
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.x, v5(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.x, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v6.xyzw(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v6.xyzw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.yx, v7.y(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.yx, 	v7.y
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.y, v8.ywz(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.y, 	v8.ywz
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xw vPos.xy, v9(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14.xw, 	vPos.xy, 	v9
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, -v0(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	-v0
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat vPos.xy, v1(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_normal1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul_sat 	r14, 	vPos.xy, 	v1
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v2(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v2
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.x, v3(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.x, 	v3
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v4.zxyw(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v4.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.yx, v5.w(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.yx, 	v5.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.y, v6.zxw(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.y, 	v6.zxw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yz vPos.xy, v7(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14.yz, 	vPos.xy, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -vPos.xy, v8_abs(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	-vPos.xy, 	v8_abs
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp vPos.xy, v9(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_tangent9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul_pp 	r14, 	vPos.xy, 	v9
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v0(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v0
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.x, v1(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.x, 	v1
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v2.xyzw(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v2.xyzw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.yx, v3.y(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.yx, 	v3.y
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.y, v4.zyw(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.y, 	v4.zyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.yw vPos.xy, v5(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14.yw, 	vPos.xy, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -vPos.xy, -v6(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	-vPos.xy, 	-v6
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat vPos.xy, v7(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_binormal7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul_sat 	r14, 	vPos.xy, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v8(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v8
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.x, v9(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.x, 	v9
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v0.zxyw(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v0.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.yx, v1.w(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.yx, 	v1.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.y, v2.zwy(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.y, 	v2.zwy
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.zw vPos.xy, v3(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14.zw, 	vPos.xy, 	v3
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos_abs.xy, v4_abs(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos_abs.xy, 	v4_abs
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp vPos.xy, v5(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_color5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul_pp 	r14, 	vPos.xy, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v6(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v6
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.x, v7(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.x, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v8.xyzw(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v8.xyzw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.yx, v9.y(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.yx, 	v9.y
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.y, v0.wxz(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.y, 	v0.wxz
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyz vPos.xy, v1(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14.xyz, 	vPos.xy, 	v1
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul -vPos.xy, v2(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog2 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	-vPos.xy, 	v2
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_sat vPos.xy, v3(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_fog3 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul_sat 	r14, 	vPos.xy, 	v3
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v4(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth4 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v4
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.x, v5(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth5 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.x, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v6.zxyw(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth6 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v6.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.yx, v7.w(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth7 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.yx, 	v7.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.y, v8.wyz(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth8 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.y, 	v8.wyz
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul.xyw vPos.xy, v9(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth9 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14.xyw, 	vPos.xy, 	v9
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul vPos.xy, v0_abs(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth0 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul 	r14, 	vPos.xy, 	v0_abs
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 mul_pp vPos.xy, v1(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			dcl_position 	o0
			dcl_depth1 	o2

			m4x4 	o0, 	v0, 	c0
			mov 	o2, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			mul_pp 	r14, 	vPos.xy, 	v1
			mov 	oC0, 	r14

		};
	}
}
//cases with predicates
Technique
<
	String	Name = "Inst: mul: (p)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;
		PixelShaderConstant[0] = {0.5, 1.0,	0.0, 1.0};

		PixelShader =
		asm
		{
			ps_3_0

			//def			c0,		0.5,	1.0,	0.0,	1.0
			def			c1,		2,		2,		2,		2

			mov			r0,		c0
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0

			mov			r4,		c0.x		// r4 = ( 0.5, 0.5, 0.5, 0.5 )
	(p0)	mul			r4,		r4,		c1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mul: (p).x";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[0] = {2.0,	2.0,	2.0,	2.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_3_0

			//def			c0,		2.0,	2.0,	2.0,	2.0
			def			c1,		2.0,	0.5,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0

			mov			r4,		c1.y		// r4 = ( 0.5, 0.5, 0.5, 0.5 )
	(p0.x)	mul			r4,		r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mul: (p(masked))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[0] = {2.0, 0.0,	0.5, 2.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_3_0

			//def			c0,		2.0,	0.0,	0.5,	2.0
			def			c1,		2.0,	0.5,	1.0,	2.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4.xw,	c1.y			// r4 = ( 0.5, x, x, 0.5 )
			mov			r4.yz,	c1.z			// r4 = ( 0.5, 1, 1, 0.5 )
	(p0)	mul			r4,		r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mul: (p(masked).x)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[0] = {2.0,	2.0,	2.0,	2.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_3_0

			//def			c0,		2.0,	2.0,	2.0,	2.0
			def			c1,		2.0,	0.5,	1.0,	2.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4,		c1.y			// r4 = ( 0.5, 0.5, 0.5, 0.5 )
	(p0.x)	mul			r4,		r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mul: (!p(masked))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[0] = {2.0,	2.0,	2.0,	2.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_3_0

			//def			c0,		2.0,	2.0,	2.0,	2.0
			def			c1,		2.0,	0.5,	1.0,	2.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4.xw,	c1.z
			mov			r4.yz,	c1.y			// r4 = ( 1, 0.5, 0.5, 1 )
	(!p0)	mul			r4,		r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mul: (!p(masked).x)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[0] = {2.0,	2.0,	2.0,	2.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_3_0

			//def			c0,		2.0,	2.0,	2.0,	2.0
			def			c1,		2.0,	0.5,	0.0,	2.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4,		c1.y			// r4 = ( 0.5, 0.5, 0.5, 0.5 )
	(!p0.y)	mul			r4,		r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: mul: (!p(masked).x), mask";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		PixelShaderConstant[0] = {2.0,	2.0,	2.0,	2.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_3_0

			//def			c0,		2.0,	2.0,	2.0,	2.0
			def			c1,		2.0,	0.5,	1.0,	2.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4.yz,	r1.y			// r4 = ( x, 0.5, 0.5, x )
			mov			r4.xw,	c1.w			// r4 = ( 1, 0.5, 0.5, 1 )
	(!p0.y)	mul			r4.yz,	r4,		c0

			mov			oC0,	r4
		};
	}
}

