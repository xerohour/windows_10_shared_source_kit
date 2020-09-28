#include "include.fx"

Technique
<
	String	Name = "ps2_x mul c0, r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
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
	String	Name = "ps2_x mul c1.x, r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[1] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c2, 	9.000001, 	4.687500, 	5.000000, 	2.000000
			mov 	r1, 	c2
			mov 	r3, 	c3
			mul 	r3, 	c1.x, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c2, r2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[2] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c3, 	1.250000, 	0.185185, 	0.426624, 	1.000000
			mov 	r2, 	c3
			mov 	r4, 	c4
			mul 	r4, 	c2, 	r2.xyzw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c3.xy, r3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[3] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c4, 	0.050000, 	0.080000, 	0.810000, 	0.292969
			mov 	r3, 	c4
			mov 	r5, 	c5
			mul 	r5, 	c3.xy, 	r3.y
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c4.yzxw, r4.xz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[4] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c5, 	6.825984, 	100.000008, 	31.249998, 	2.057613
			mov 	r4, 	c5
			mov 	r6, 	c6
			mul 	r6, 	c4.yzxw, 	r4.xz
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps2_x mul.x c5, r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[5] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c6, 	0.810000, 	0.292969, 	0.050000, 	0.080000
			mov 	r5, 	c6
			mov 	r7, 	c7
			mul 	r7.x, 	c5, 	r5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps2_x mul -c6, r6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[6] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c7, 	1.250000, 	0.185185, 	0.426624, 	1.000000
			mov 	r6, 	c7
			mov 	r8, 	c8
			mul 	r8, 	-c6, 	r6
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps2_x mul_sat c7, r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[7] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c8, 	5.000000, 	2.000000, 	9.000000, 	4.687500
			mov 	r7, 	c8
			mov 	r9, 	c9
			mul_sat 	r9, 	c7, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c8, v0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.027451, 0.011765, 0.050980, 0.019608
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[8] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r8, 	c8
			mov 	r10, 	c11
			mul 	r10, 	c8, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c9.z, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[9] = {0.007843, 0.007843, 0.003922, 0.011765};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r8, 	c9
			mov 	r10, 	c11
			mul 	r10, 	c9.z, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c11, v0.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050980, 0.019608, 0.027451, 0.011765
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[11] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r8, 	c11
			mov 	r10, 	c12
			mul 	r10, 	c11, 	v0.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c11.xw, v1.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[11] = {0.101961, 0.250980, 0.058824, 0.109804};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r8, 	c11
			mov 	r10, 	c13
			mul 	r10, 	c11.xw, 	v1.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c12.wzyx, v0.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.427451, 1.000000, 1.250000, 0.188235
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[12] = {1.171875, 0.501961, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r8, 	c12
			mov 	r10, 	c14
			mul 	r10, 	c12.wzyx, 	v0.yx
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul.y c13, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.811765, 0.294118, 0.050980, 0.082353
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[13] = {0.619608, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r8, 	c13
			mov 	r10, 	c15
			mul 	r10.y, 	c13, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c14, -v0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[14] = {0.019608, 0.243137, 0.074510, 0.007843};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r8, 	c14
			mov 	r10, 	c16
			mul 	r10, 	c14, 	-v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul_pp c15, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050980, 0.082353, 0.811765, 0.294118
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[15] = {10.000000, 6.250000, 0.619608, 1.706496};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r8, 	c15
			mov 	r10, 	c17
			mul_pp 	r10, 	c15, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c16, t0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[16] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	c16, 	t0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c17.x, t1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT1, 	c4			
		};
		
		PixelShaderConstant[17] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t1
			mov 	r8, 	c17
			mov 	r10, 	c19
			mul 	r10, 	c17.x, 	t1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c18, t2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT2, 	c4			
		};
		
		PixelShaderConstant[18] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t2
			mov 	r8, 	c18
			mov 	r10, 	c20
			mul 	r10, 	c18, 	t2.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c19.yz, t3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT3, 	c4			
		};
		
		PixelShaderConstant[19] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t3
			mov 	r8, 	c19
			mov 	r10, 	c21
			mul 	r10, 	c19.yz, 	t3.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul c20.yzxw, t4.yw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT4, 	c4			
		};
		
		PixelShaderConstant[20] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t4
			mov 	r8, 	c20
			mov 	r10, 	c22
			mul 	r10, 	c20.yzxw, 	t4.yw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul.z c21, t5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT5, 	c4			
		};
		
		PixelShaderConstant[21] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t5
			mov 	r8, 	c21
			mov 	r10, 	c23
			mul 	r10.z, 	c21, 	t5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul -c22, -t6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT6, 	c4			
		};
		
		PixelShaderConstant[22] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t6
			mov 	r8, 	c22
			mov 	r10, 	c24
			mul 	r10, 	-c22, 	-t6
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul_sat c23, t7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT7, 	c4			
		};
		
		PixelShaderConstant[23] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t7
			mov 	r8, 	c23
			mov 	r10, 	c25
			mul_sat 	r10, 	c23, 	t7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r8, c24";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[24] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c25, 	6.825984, 	100.000008, 	31.249998, 	2.057613
			mov 	r8, 	c25
			mov 	r10, 	c26
			mul 	r10, 	r8, 	c24
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r9.z, c25";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[25] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c26, 	0.810000, 	0.292969, 	0.050000, 	0.080000
			mov 	r9, 	c26
			mov 	r11, 	c27
			mul 	r11, 	r9.z, 	c25
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r10, c26.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[26] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c27, 	1.250000, 	0.185185, 	0.426624, 	1.000000
			mov 	r10, 	c27
			mov 	r0, 	c28
			mul 	r0, 	r10, 	c26.zxyw
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r11.zx, c27.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[27] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c28, 	5.000000, 	2.000000, 	9.000000, 	4.687500
			mov 	r11, 	c28
			mov 	r1, 	c29
			mul 	r1, 	r11.zx, 	c27.w
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r0.wzyx, c28.zy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[28] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c29, 	0.026664, 	0.010000, 	0.050000, 	0.016667
			mov 	r0, 	c29
			mov 	r2, 	c30
			mul 	r2, 	r0.wzyx, 	c28.zy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul.w r1, c29";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[29] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c30, 	100.000000, 	75.000000, 	499.999969, 	50.000004
			mov 	r1, 	c30
			mov 	r3, 	c31
			mul 	r3.w, 	r1, 	c29
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps2_x mul -r2, c30";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[30] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c31, 	0.050000, 	0.016667, 	0.026664, 	0.010000
			mov 	r2, 	c31
			mov 	r4, 	c0
			mul 	r4, 	-r2, 	c30
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x mul_pp r3, c31";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[31] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c0, 	5.000000, 	2.000000, 	9.000001, 	4.687500
			mov 	r3, 	c0
			mov 	r5, 	c1
			mul_pp 	r5, 	r3, 	c31
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r4, r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	0.426624, 	1.000000, 	1.250000, 	0.185185
			mov 	r5, 	c1
			mov 	r4, 	c0
			mov 	r6, 	c2
			mul 	r6, 	r4, 	r5
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r6.x, r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	0.810000, 	0.292969, 	0.050000, 	0.080000
			mov 	r7, 	c1
			mov 	r6, 	c0
			mov 	r8, 	c2
			mul 	r8, 	r6.x, 	r7
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r8, r9.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	31.249998, 	2.057613, 	6.825984, 	100.000008
			mov 	r9, 	c1
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul 	r10, 	r8, 	r9.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r10.zw, r11.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	0.050000, 	0.080000, 	0.810000, 	0.292969
			mov 	r11, 	c1
			mov 	r10, 	c0
			mov 	r0, 	c2
			mul 	r0, 	r10.zw, 	r11.y
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r0.yzxw, r1.wx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	0.426624, 	1.000000, 	1.250000, 	0.185185
			mov 	r1, 	c1
			mov 	r0, 	c0
			mov 	r2, 	c2
			mul 	r2, 	r0.yzxw, 	r1.wx
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul.xy r2, r3";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	9.000000, 	4.687500, 	5.000000, 	2.000000
			mov 	r3, 	c1
			mov 	r2, 	c0
			mov 	r4, 	c2
			mul 	r4.xy, 	r2, 	r3
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r4, -r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	0.050000, 	0.016667, 	0.026664, 	0.010000
			mov 	r5, 	c1
			mov 	r4, 	c0
			mov 	r6, 	c2
			mul 	r6, 	r4, 	-r5
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps2_x mul_sat r6, r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	499.999969, 	50.000004, 	100.000000, 	75.000000
			mov 	r7, 	c1
			mov 	r6, 	c0
			mov 	r8, 	c2
			mul_sat 	r8, 	r6, 	r7
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r8, v0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.027451, 0.011765, 0.050980, 0.019608
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul 	r10, 	r8, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r9.z, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[0] = {0.058824, 0.109804, 0.101961, 0.250980};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11, 	r9.z, 	v1
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r10, v0.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 1.250000, 0.188235, 0.427451, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.400000, 2.700000, 1.171875, 0.501961};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r10, 	c0
			mov 	r0, 	c2
			mul 	r0, 	r10, 	v0.zxyw
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r11.wy, v1.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050980, 0.082353, 0.811765, 0.294118
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000001, 6.250000, 0.619608, 1.706496};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r11, 	c0
			mov 	r1, 	c2
			mul 	r1, 	r11.wy, 	v1.w
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r0.wzyx, v0.wz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.074510, 0.007843, 0.019608, 0.243137};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c0
			mov 	r2, 	c2
			mul 	r2, 	r0.wzyx, 	v0.wz
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul.xz r1, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.811765, 0.294118, 0.050980, 0.082353
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[0] = {0.619608, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r1, 	c0
			mov 	r3, 	c2
			mul 	r3.xz, 	r1, 	v1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps2_x mul -r2, -v0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 1.250000, 0.188235, 0.427451, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.403922, 2.700000, 1.171875, 0.501961};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r2, 	c0
			mov 	r4, 	c2
			mul 	r4, 	-r2, 	-v0
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x mul_pp r3, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[0] = {0.101961, 0.250980, 0.058824, 0.109804};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r3, 	c0
			mov 	r5, 	c2
			mul_pp 	r5, 	r3, 	v1
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r4, t0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			mov 	r4, 	c0
			mov 	r6, 	c2
			mul 	r6, 	r4, 	t0
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r5.x, t1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT1, 	c4			
		};
		
		PixelShaderConstant[0] = {0.005000, 0.006666, 0.001000, 0.010000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t1
			mov 	r5, 	c0
			mov 	r7, 	c2
			mul 	r7, 	r5.x, 	t1
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r6, t2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT2, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t2
			mov 	r6, 	c0
			mov 	r8, 	c2
			mul 	r8, 	r6, 	t2.xyzw
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r7.xyz, t3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT3, 	c4			
		};
		
		PixelShaderConstant[0] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t3
			mov 	r7, 	c0
			mov 	r9, 	c2
			mul 	r9, 	r7.xyz, 	t3.y
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps2_x mul r8.yzxw, t4.xyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT4, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t4
			mov 	r8, 	c0
			mov 	r10, 	c2
			mul 	r10, 	r8.yzxw, 	t4.xyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul.xw r9, t5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT5, 	c4			
		};
		
		PixelShaderConstant[0] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t5
			mov 	r9, 	c0
			mov 	r11, 	c2
			mul 	r11.xw, 	r9, 	t5
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps2_x mul -r10, t6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT6, 	c4			
		};
		
		PixelShaderConstant[0] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t6
			mov 	r10, 	c0
			mov 	r0, 	c2
			mul 	r0, 	-r10, 	t6
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps2_x mul_sat r11, t7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT7, 	c4			
		};
		
		PixelShaderConstant[0] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t7
			mov 	r11, 	c0
			mov 	r1, 	c2
			mul_sat 	r1, 	r11, 	t7
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps2_x mul v0, c0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.427451, 1.000000, 1.250000, 0.188235
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[0] = {1.171875, 0.501961, 0.403922, 2.700000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c0
			mov 	r2, 	c2
			mul 	r2, 	v0, 	c0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul v1.z, c1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[1] = {0.058824, 0.109804, 0.101961, 0.250980};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r0, 	c1
			mov 	r2, 	c3
			mul 	r2, 	v1.z, 	c1
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul v0, c2.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050980, 0.019608, 0.027451, 0.011765
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[2] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c2
			mov 	r2, 	c4
			mul 	r2, 	v0, 	c2.zxyw
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul v1.xzy, c3.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[3] = {0.003922, 0.011765, 0.007843, 0.007843};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r0, 	c3
			mov 	r2, 	c5
			mul 	r2, 	v1.xzy, 	c3.w
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul v0.wzyx, c4.xzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.027451, 0.011765, 0.050980, 0.019608
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[4] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c4
			mov 	r2, 	c6
			mul 	r2, 	v0.wzyx, 	c4.xzw
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul.yz v1, c5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[5] = {0.058824, 0.109804, 0.101961, 0.250980};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r0, 	c5
			mov 	r2, 	c7
			mul 	r2.yz, 	v1, 	c5
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul v0, -c6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 1.250000, 0.188235, 0.427451, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[6] = {0.400000, 2.700000, 1.171875, 0.501961};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c6
			mov 	r2, 	c8
			mul 	r2, 	v0, 	-c6
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul_pp v1, c7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050980, 0.082353, 0.811765, 0.294118
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[7] = {10.000001, 6.250000, 0.619608, 1.706496};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r0, 	c7
			mov 	r2, 	c9
			mul_pp 	r2, 	v1, 	c7
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul v0, r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[8] = {0.074510, 0.007843, 0.019608, 0.243137};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c8
			mov 	r2, 	c11
			mul 	r2, 	v0, 	r0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul v1.x, r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.811765, 0.294118, 0.050980, 0.082353
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[8] = {0.619608, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r1, 	c8
			mov 	r3, 	c11
			mul 	r3, 	v1.x, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps2_x mul v0, r2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 1.250000, 0.188235, 0.427451, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[8] = {0.403922, 2.700000, 1.171875, 0.501961};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r2, 	c8
			mov 	r4, 	c11
			mul 	r4, 	v0, 	r2.xyzw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x mul v1.xwy, r3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[8] = {0.101961, 0.250980, 0.058824, 0.109804};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r3, 	c8
			mov 	r5, 	c11
			mul 	r5, 	v1.xwy, 	r3.y
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps2_x mul v0.yzxw, r4.xwz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.027451, 0.011765, 0.050980, 0.019608
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[8] = {18.750000, 49.999996, 10.000001, 30.000000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r4, 	c8
			mov 	r6, 	c11
			mul 	r6, 	v0.yzxw, 	r4.xwz
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps2_x mul.yw v1, r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 100.000000, 75.000000, 499.999969, 50.000004
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[8] = {0.007843, 0.007843, 0.003922, 0.011765};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r5, 	c8
			mov 	r7, 	c11
			mul 	r7.yw, 	v1, 	r5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps2_x mul -v0, -r6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050980, 0.019608, 0.027451, 0.011765
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[8] = {10.000000, 30.000002, 18.750000, 50.000000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r6, 	c8
			mov 	r8, 	c11
			mul 	r8, 	-v0, 	-r6
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps2_x mul_sat v1, r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 5.000000, 2.000000, 9.000001, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[8] = {0.101961, 0.250980, 0.058824, 0.109804};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r7, 	c8
			mov 	r9, 	c11
			mul_sat 	r9, 	v1, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps2_x mul t0, c8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[8] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			mov 	r8, 	c8
			mov 	r10, 	c11
			mul 	r10, 	t0, 	c8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul t1.z, c9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT1, 	c4			
		};
		
		PixelShaderConstant[9] = {0.617284, 1.706496, 10.000001, 6.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t1
			mov 	r8, 	c9
			mov 	r10, 	c11
			mul 	r10, 	t1.z, 	c9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul t2, c11.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 31.249998, 2.057613, 6.825984, 100.000008
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT2, 	c4			
		};
		
		PixelShaderConstant[11] = {0.016000, 0.243000, 0.073242, 0.005000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t2
			mov 	r8, 	c11
			mov 	r10, 	c12
			mul 	r10, 	t2, 	c11.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul t3.yxz, c11.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT3, 	c4			
		};
		
		PixelShaderConstant[11] = {10.000000, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t3
			mov 	r8, 	c11
			mov 	r10, 	c13
			mul 	r10, 	t3.yxz, 	c11.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul t4.wzyx, c12.yxw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.426624, 1.000000, 1.250000, 0.185185
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT4, 	c4			
		};
		
		PixelShaderConstant[12] = {1.171875, 0.500000, 0.400000, 2.700000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t4
			mov 	r8, 	c12
			mov 	r10, 	c14
			mul 	r10, 	t4.wzyx, 	c12.yxw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul.zw t5, c13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 9.000000, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT5, 	c4			
		};
		
		PixelShaderConstant[13] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t5
			mov 	r8, 	c13
			mov 	r10, 	c15
			mul 	r10.zw, 	t5, 	c13
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul -t6, c14";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050000, 0.016667, 0.026664, 0.010000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT6, 	c4			
		};
		
		PixelShaderConstant[14] = {10.000001, 30.000000, 18.750000, 49.999996};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t6
			mov 	r8, 	c14
			mov 	r10, 	c16
			mul 	r10, 	-t6, 	c14
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul_pp t7, c15";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 499.999969, 50.000004, 100.000000, 75.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT7, 	c4			
		};
		
		PixelShaderConstant[15] = {0.001000, 0.010000, 0.005000, 0.006666};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t7
			mov 	r8, 	c15
			mov 	r10, 	c17
			mul_pp 	r10, 	t7, 	c15
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul t0, r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.026664, 0.010000, 0.050000, 0.016667
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[16] = {18.750000, 50.000000, 10.000000, 30.000002};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			mov 	r8, 	c16
			mov 	r10, 	c18
			mul 	r10, 	t0, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x mul t1.x, r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 9.000001, 4.687500, 5.000000, 2.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT1, 	c4			
		};
		
		PixelShaderConstant[16] = {0.055556, 0.106656, 0.100000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t1
			mov 	r9, 	c16
			mov 	r11, 	c18
			mul 	r11, 	t1.x, 	r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps2_x mul t2, r10.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t2
			mov 	r10, 	c16
			mov 	r0, 	c18
			mul 	r0, 	t2, 	r10.xyzw
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps2_x mul t3.yzx, r11.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.050000, 0.080000, 0.810000, 0.292969
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT3, 	c4			
		};
		
		PixelShaderConstant[16] = {10.000001, 6.250000, 0.617284, 1.706496};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t3
			mov 	r11, 	c16
			mov 	r1, 	c18
			mul 	r1, 	t3.yzx, 	r11.y
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps2_x mul t4.yzxw, r0.yzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 6.825984, 100.000008, 31.249998, 2.057613
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT4, 	c4			
		};
		
		PixelShaderConstant[16] = {0.073242, 0.005000, 0.016000, 0.243000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t4
			mov 	r0, 	c16
			mov 	r2, 	c18
			mul 	r2, 	t4.yzxw, 	r0.yzw
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x mul.xyz t5, r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 0.810000, 0.292969, 0.050000, 0.080000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT5, 	c4			
		};
		
		PixelShaderConstant[16] = {0.617284, 1.706496, 10.000000, 6.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t5
			mov 	r1, 	c16
			mov 	r3, 	c18
			mul 	r3.xyz, 	t5, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps2_x mul t6, -r2";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 1.250000, 0.185185, 0.426624, 1.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT6, 	c4			
		};
		
		PixelShaderConstant[16] = {0.400000, 2.700000, 1.171875, 0.500000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t6
			mov 	r2, 	c16
			mov 	r4, 	c18
			mul 	r4, 	t6, 	-r2
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x mul_sat t7, r3";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_2_x
			def c4, 5.000000, 2.000000, 9.000000, 4.687500
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT7, 	c4			
		};
		
		PixelShaderConstant[16] = {0.100000, 0.250000, 0.055556, 0.106656};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t7
			mov 	r3, 	c16
			mov 	r5, 	c18
			mul_sat 	r5, 	t7, 	r3
			mov 	oC0, 	r5

		};
	}
}
//cases with predicates
Technique
<
	String	Name = "Inst: mul: (p0)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0.5,	1.0,	0.0,	1.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x

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
	String	Name = "Inst: mul: (p0).x";
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
			ps_2_x

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
	String	Name = "Inst: mul: (p0(masked))";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {2.0,	2.0,	0.5,	2.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x

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
	String	Name = "Inst: mul: (p0(masked).x)";
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
			ps_2_x

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
	String	Name = "Inst: mul: (!p0(masked))";
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
			ps_2_x

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
	String	Name = "Inst: mul: (!p0(masked).x)";
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
			ps_2_x

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
	String	Name = "Inst: mul: (!p0(masked).x), mask";
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
			ps_2_x

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

