#include "include.fx"

Technique
<
	String	Name = "ps2_x add r2, c0, r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.490000, 1.250000, -0.400000, 1.450000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.990000, -0.750000, 0.900000, -0.950000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	-0.490000, 	1.250000, 	-0.400000, 	1.450000
			mov 	r0, 	c1
			mov 	r2, 	c2
			add 	r2, 	c0, 	r0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r3, c1.x, r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.900000, 0.980000, -0.500000, 0.800000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[1] = {1.400000, -0.480000, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c2, 	-0.900000, 	0.980000, 	-0.500000, 	0.800000
			mov 	r1, 	c2
			mov 	r3, 	c3
			add 	r3, 	c1.x, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps2_x add r4, c2, r2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.200000, 1.350000, -0.470000, 0.750000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[2] = {0.700000, -0.850000, 0.970000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c3, 	-0.200000, 	1.350000, 	-0.470000, 	0.750000
			mov 	r2, 	c3
			mov 	r4, 	c4
			add 	r4, 	c2, 	r2.xyzw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x add r5, c3.xy, r3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.000000, 0.600000, -0.800000, 0.960000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[3] = {0.500000, -0.100000, 1.300000, -0.460000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c4, 	0.000000, 	0.600000, 	-0.800000, 	0.960000
			mov 	r3, 	c4
			mov 	r5, 	c5
			add 	r5, 	c3.xy, 	r3.y
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps2_x add r6, c4.yzxw, r4.xz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.450000, 0.250000, -0.000000, 1.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[4] = {0.950000, 0.250000, 0.500000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c5, 	-0.450000, 	0.250000, 	-0.000000, 	1.250000
			mov 	r4, 	c5
			mov 	r6, 	c6
			add 	r6, 	c4.yzxw, 	r4.xz
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps2_x r7, add.x c5, r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.700000, 0.940000, 0.500000, 0.400000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[5] = {1.200000, -0.440000, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c6, 	-0.700000, 	0.940000, 	0.500000, 	0.400000
			mov 	r5, 	c6
			mov 	r7, 	c7
			add 	r7.x, 	c5, 	r5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps2_x add r8, -c6, r6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.200000, 1.150000, -0.430000, -0.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[6] = {0.300000, -0.650000, 0.930000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c7, 	0.200000, 	1.150000, 	-0.430000, 	-0.250000
			mov 	r6, 	c7
			mov 	r8, 	c8
			add 	r8, 	-c6, 	r6
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps2_x add_sat r9, c7, r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.000000, 0.200000, -0.600000, 0.920000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[7] = {-0.500000, 0.300000, 1.100000, -0.420000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c8, 	1.000000, 	0.200000, 	-0.600000, 	0.920000
			mov 	r7, 	c8
			mov 	r9, 	c9
			add_sat 	r9, 	c7, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, c8, v0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.410000, -0.250000, 0.400000, 1.050000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[8] = {0.913725, 0.752941, 0.101961, -0.550000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r8, 	c8
			mov 	r10, 	c11
			add 	r10, 	c8, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, c9.z, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.500000, 0.901961, 0.501961, -0.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[9] = {1.000000, -0.400000, 0.000000, 0.501961};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r8, 	c9
			mov 	r10, 	c11
			add 	r10, 	c9.z, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, c11, v0.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.600000, 0.952941, -0.390000, 0.250980
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[11] = {-0.100000, -0.450000, 0.890196, 0.250980};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r8, 	c11
			mov 	r10, 	c12
			add 	r10, 	c11, 	v0.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, c11.xw, v1.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.000000, -0.200000, -0.400000, 0.882353
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[11] = {0.501961, 0.701961, 0.901961, -0.380000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r8, 	c11
			mov 	r10, 	c13
			add 	r10, 	c11.xw, 	v1.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, c12.wzyx, v0.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.370000, 0.752941, 0.800000, 0.850980
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[12] = {0.870588, -0.250000, -0.300000, -0.350000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r8, 	c12
			mov 	r10, 	c14
			add 	r10, 	c12.wzyx, 	v0.yx
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10.y, c13, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.300000, 0.862745, -0.500000, -0.400000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[13] = {0.803922, -0.360000, 1.000000, 0.901961};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r8, 	c13
			mov 	r10, 	c15
			add 	r10.y, 	c13, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, c14, -v0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.000000, 0.752941, -0.350000, 1.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[14] = {-0.500000, -0.250000, 0.850980, -0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r8, 	c14
			mov 	r10, 	c16
			add 	r10, 	c14, 	-v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add_pp r10, c15, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -1.000000, -0.600000, -0.200000, 0.843137
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[15] = {1.500000, 1.100000, 0.701961, -0.340000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r8, 	c15
			mov 	r10, 	c17
			add_pp 	r10, 	c15, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, c16, t0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.330000, 1.250000, 1.200000, 0.650000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[16] = {0.830000, -0.750000, -0.700000, -0.150000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	c16, 	t0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, c17.x, t1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.100000, 0.820000, -0.500000, -0.800000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT1, 	c4			
		};
		
		PixelShaderConstant[17] = {0.600000, -0.320000, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t1
			mov 	r8, 	c17
			mov 	r10, 	c19
			add 	r10, 	c17.x, 	t1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, c18, t2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.400000, 0.550000, -0.310000, 0.750000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT2, 	c4			
		};
		
		PixelShaderConstant[18] = {-0.900000, -0.050000, 0.810000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t2
			mov 	r8, 	c18
			mov 	r10, 	c20
			add 	r10, 	c18, 	t2.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, c19.yz, t3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.000000, -1.000000, 0.000000, 0.800000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT3, 	c4			
		};
		
		PixelShaderConstant[19] = {0.500000, 1.500000, 0.500000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t3
			mov 	r8, 	c19
			mov 	r10, 	c21
			add 	r10, 	c19.yz, 	t3.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, c20.yzxw, t4.yw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.290000, 0.250000, 1.400000, 0.450000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT4, 	c4			
		};
		
		PixelShaderConstant[20] = {0.790000, 0.250000, -0.900000, 0.050000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t4
			mov 	r8, 	c20
			mov 	r10, 	c22
			add 	r10, 	c20.yzxw, 	t4.yw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10.z, c21, t5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.100000, 0.780000, 0.500000, -0.800000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT5, 	c4			
		};
		
		PixelShaderConstant[21] = {0.400000, -0.280000, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t5
			mov 	r8, 	c21
			mov 	r10, 	c23
			add 	r10.z, 	c21, 	t5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, -c22, -t6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.200000, 0.350000, -0.270000, -0.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT6, 	c4			
		};
		
		PixelShaderConstant[22] = {-0.700000, 0.150000, 0.770000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t6
			mov 	r8, 	c22
			mov 	r10, 	c24
			add 	r10, 	-c22, 	-t6
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add_sat r10, c23, t7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.000000, -0.600000, 0.200000, 0.760000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT7, 	c4			
		};
		
		PixelShaderConstant[23] = {-0.500000, 1.100000, 0.300000, -0.260000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t7
			mov 	r8, 	c23
			mov 	r10, 	c25
			add_sat 	r10, 	c23, 	t7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, r8, c24";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.250000, -0.250000, 1.000000, 0.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[24] = {0.750000, 0.750000, -0.500000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c25, 	-0.250000, 	-0.250000, 	1.000000, 	0.250000
			mov 	r8, 	c25
			mov 	r10, 	c26
			add 	r10, 	r8, 	c24
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r11, r9.z, c25";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.300000, 0.740000, 0.500000, -0.400000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[25] = {0.200000, -0.240000, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c26, 	0.300000, 	0.740000, 	0.500000, 	-0.400000
			mov 	r9, 	c26
			mov 	r11, 	c27
			add 	r11, 	r9.z, 	c25
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps2_x add r0, r10, c26.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.800000, 0.150000, -0.230000, 0.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[26] = {-0.300000, 0.350000, 0.730000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c27, 	0.800000, 	0.150000, 	-0.230000, 	0.250000
			mov 	r10, 	c27
			mov 	r0, 	c28
			add 	r0, 	r10, 	c26.zxyw
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps2_x add r1, r11.zx, c27.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.000000, -0.200000, 0.400000, 0.720000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[27] = {0.500000, 0.700000, 0.100000, -0.220000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c28, 	0.000000, 	-0.200000, 	0.400000, 	0.720000
			mov 	r11, 	c28
			mov 	r1, 	c29
			add 	r1, 	r11.zx, 	c27.w
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2, r0.wzyx, c28.zy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.210000, 0.750000, 0.600000, 0.050000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[28] = {0.710000, -0.250000, -0.100000, 0.450000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c29, 	-0.210000, 	0.750000, 	0.600000, 	0.050000
			mov 	r0, 	c29
			mov 	r2, 	c30
			add 	r2, 	r0.wzyx, 	c28.zy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r3.w, r1, c29";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.500000, 0.700000, -0.500000, -0.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[29] = {0.000000, -0.200000, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c30, 	0.500000, 	0.700000, 	-0.500000, 	-0.000000
			mov 	r1, 	c30
			mov 	r3, 	c31
			add 	r3.w, 	r1, 	c29
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps2_x add r4, -r2, c30";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.400000, -0.050000, -0.190000, 1.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[30] = {0.100000, 0.550000, 0.690000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c31, 	0.400000, 	-0.050000, 	-0.190000, 	1.250000
			mov 	r2, 	c31
			mov 	r4, 	c0
			add 	r4, 	-r2, 	c30
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x add_pp r5, r3, c31";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -1.000000, 0.200000, 0.600000, 0.680000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[31] = {1.500000, 0.300000, -0.100000, -0.180000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c0, 	-1.000000, 	0.200000, 	0.600000, 	0.680000
			mov 	r3, 	c0
			mov 	r5, 	c1
			add_pp 	r5, 	r3, 	c31
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps2_x add r6, r4, r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.170000, 1.250000, 0.200000, -0.150000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.670000, -0.750000, 0.300000, 0.650000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	-0.170000, 	1.250000, 	0.200000, 	-0.150000
			mov 	r5, 	c1
			mov 	r4, 	c0
			mov 	r6, 	c2
			add 	r6, 	r4, 	r5
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps2_x add r8, r6.x, r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.700000, 0.660000, -0.500000, 0.400000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {-0.200000, -0.160000, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	0.700000, 	0.660000, 	-0.500000, 	0.400000
			mov 	r7, 	c1
			mov 	r6, 	c0
			mov 	r8, 	c2
			add 	r8, 	r6.x, 	r7
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, r8, r9.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.000000, -0.250000, -0.150000, 0.750000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.500000, 0.750000, 0.650000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	-0.000000, 	-0.250000, 	-0.150000, 	0.750000
			mov 	r9, 	c1
			mov 	r8, 	c0
			mov 	r10, 	c2
			add 	r10, 	r8, 	r9.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r0, r10.zw, r11.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.000000, 0.600000, 0.800000, 0.640000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.500000, -0.100000, -0.300000, -0.140000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	0.000000, 	0.600000, 	0.800000, 	0.640000
			mov 	r11, 	c1
			mov 	r10, 	c0
			mov 	r0, 	c2
			add 	r0, 	r10.zw, 	r11.y
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2, r0.yzxw, r1.wx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.130000, 0.250000, -0.200000, -0.350000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.630000, 0.250000, 0.700000, 0.850000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	-0.130000, 	0.250000, 	-0.200000, 	-0.350000
			mov 	r1, 	c1
			mov 	r0, 	c0
			mov 	r2, 	c2
			add 	r2, 	r0.yzxw, 	r1.wx
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r4.xy, r2, r3";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.900000, 0.620000, 0.500000, 0.800000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {-0.400000, -0.120000, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	0.900000, 	0.620000, 	0.500000, 	0.800000
			mov 	r3, 	c1
			mov 	r2, 	c0
			mov 	r4, 	c2
			add 	r4.xy, 	r2, 	r3
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x add r6, r4, -r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.400000, -0.450000, -0.110000, -0.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.900000, 0.950000, 0.610000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	-0.400000, 	-0.450000, 	-0.110000, 	-0.250000
			mov 	r5, 	c1
			mov 	r4, 	c0
			mov 	r6, 	c2
			add 	r6, 	r4, 	-r5
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps2_x add_sat r8, r6, r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.000000, 1.000000, 1.000000, 0.600000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {-0.500000, -0.500000, -0.500000, -0.100000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			def 	c1, 	1.000000, 	1.000000, 	1.000000, 	0.600000
			mov 	r7, 	c1
			mov 	r6, 	c0
			mov 	r8, 	c2
			add_sat 	r8, 	r6, 	r7
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, r8, v0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.090000, -0.250000, -0.400000, -0.450000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.592157, 0.752941, 0.901961, 0.952941};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r8, 	c0
			mov 	r10, 	c2
			add 	r10, 	r8, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r11, r9.z, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.901961, 0.580392, 0.501961, 0.800000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[0] = {-0.400000, -0.080000, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	r9.z, 	v1
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps2_x add r0, r10, v0.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.200000, -0.350000, -0.070000, 0.250980
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.701961, 0.850980, 0.572549, 0.250980};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r10, 	c0
			mov 	r0, 	c2
			add 	r0, 	r10, 	v0.zxyw
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps2_x add r1, r11.wy, v1.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.000000, 0.600000, 0.803922, 0.560784
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[0] = {0.501961, -0.100000, -0.300000, -0.060000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r11, 	c0
			mov 	r1, 	c2
			add 	r1, 	r11.wy, 	v1.w
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2, r0.wzyx, v0.wz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.050000, 0.752941, -0.000000, -0.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.552941, -0.250000, 0.501961, 0.752941};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c0
			mov 	r2, 	c2
			add 	r2, 	r0.wzyx, 	v0.wz
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r3.xz, r1, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.701961, 0.541176, -0.500000, 0.400000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[0] = {-0.200000, -0.040000, 1.000000, 0.101961};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r1, 	c0
			mov 	r3, 	c2
			add 	r3.xz, 	r1, 	v1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps2_x add r4, -r2, -v0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.200000, -0.150000, -0.030000, 1.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.301961, 0.650980, 0.533333, -0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r2, 	c0
			mov 	r4, 	c2
			add 	r4, 	-r2, 	-v0
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x add_pp r5, r3, v1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -1.000000, 0.200000, 0.603922, 0.521569
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[0] = {1.500000, 0.301961, -0.100000, -0.020000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r3, 	c0
			mov 	r5, 	c2
			add_pp 	r5, 	r3, 	v1
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps2_x add r6, r4, t0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.010000, 1.250000, 0.400000, -0.050000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.510000, -0.750000, 0.100000, 0.550000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			mov 	r4, 	c0
			mov 	r6, 	c2
			add 	r6, 	r4, 	t0
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps2_x add r7, r5.x, t1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.500000, 0.500000, -0.500000, -0.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT1, 	c4			
		};
		
		PixelShaderConstant[0] = {0.000000, -0.000000, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t1
			mov 	r5, 	c0
			mov 	r7, 	c2
			add 	r7, 	r5.x, 	t1
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps2_x add r8, r6, t2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.600000, 0.050000, 0.010000, 0.750000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT2, 	c4			
		};
		
		PixelShaderConstant[0] = {-0.100000, 0.450000, 0.490000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t2
			mov 	r6, 	c0
			mov 	r8, 	c2
			add 	r8, 	r6, 	t2.xyzw
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps2_x add r9, r7.xyz, t3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.000000, -0.200000, 0.400000, 0.480000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT3, 	c4			
		};
		
		PixelShaderConstant[0] = {0.500000, 0.700000, 0.100000, 0.020000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t3
			mov 	r7, 	c0
			mov 	r9, 	c2
			add 	r9, 	r7.xyz, 	t3.y
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, r8.yzxw, t4.xyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.030000, 0.250000, 0.800000, 0.150000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT4, 	c4			
		};
		
		PixelShaderConstant[0] = {0.470001, 0.250000, -0.300000, 0.350000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t4
			mov 	r8, 	c0
			mov 	r10, 	c2
			add 	r10, 	r8.yzxw, 	t4.xyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r11.xw, r9, t5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.300000, 0.460001, 0.500000, -0.400000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT5, 	c4			
		};
		
		PixelShaderConstant[0] = {0.200000, 0.040000, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t5
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11.xw, 	r9, 	t5
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps2_x add r0, -r10, t6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.000000, 0.250000, 0.050000, -0.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT6, 	c4			
		};
		
		PixelShaderConstant[0] = {-0.500000, 0.250000, 0.450001, 0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t6
			mov 	r10, 	c0
			mov 	r0, 	c2
			add 	r0, 	-r10, 	t6
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps2_x add_sat r1, r11, t7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.000000, -0.600000, 0.200000, 0.440001
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT7, 	c4			
		};
		
		PixelShaderConstant[0] = {-0.500000, 1.100000, 0.300000, 0.060000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t7
			mov 	r11, 	c0
			mov 	r1, 	c2
			add_sat 	r1, 	r11, 	t7
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2, v0, c0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.070588, -0.250000, 1.200000, 0.352941
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[0] = {0.431373, 0.752941, -0.700000, 0.152941};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c0
			mov 	r2, 	c2
			add 	r2, 	v0, 	c0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2, v1.z, c1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.101961, 0.423529, 0.501961, -0.800000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[1] = {0.403922, 0.082353, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r0, 	c1
			mov 	r2, 	c3
			add 	r2, 	v1.z, 	c1
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2, v0, c2.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.400000, 0.450980, 0.090196, 0.250980
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[2] = {-0.900000, 0.050980, 0.411765, 0.250980};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c2
			mov 	r2, 	c4
			add 	r2, 	v0, 	c2.zxyw
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2, v1.xzy, c3.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.000000, -1.000000, 0.003922, 0.403922
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[3] = {0.501961, 1.500000, 0.501961, 0.101961};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r0, 	c3
			mov 	r2, 	c5
			add 	r2, 	v1.xzy, 	c3.w
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2, v0.wzyx, c4.xzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.113725, 0.752941, 1.400000, 0.552941
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[4] = {0.392157, -0.250000, -0.900000, -0.050000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c4
			mov 	r2, 	c6
			add 	r2, 	v0.wzyx, 	c4.xzw
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2.yz, v1, c5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.100000, 0.380392, -0.500000, -0.800000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[5] = {0.603922, 0.121569, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r0, 	c5
			mov 	r2, 	c7
			add 	r2.yz, 	v1, 	c5
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2, v0, -c6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.200000, 0.650980, 0.133333, 1.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[6] = {-0.700000, -0.150000, 0.372549, -0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c6
			mov 	r2, 	c8
			add 	r2, 	v0, 	-c6
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add_pp r2, v1, c7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -1.000000, -0.600000, -0.200000, 0.360784
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[7] = {1.500000, 1.100000, 0.701961, 0.141176};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r0, 	c7
			mov 	r2, 	c9
			add_pp 	r2, 	v1, 	c7
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2, v0, r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.152941, 1.250000, 1.000000, 0.752941
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[8] = {0.352941, -0.750000, -0.500000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r0, 	c8
			mov 	r2, 	c11
			add 	r2, 	v0, 	r0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r3, v1.x, r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.300000, 0.341176, -0.500000, -0.400000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[8] = {0.803922, 0.160784, 1.000000, 0.901961};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r1, 	c8
			mov 	r3, 	c11
			add 	r3, 	v1.x, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps2_x add r4, v0, r2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.800000, 0.850980, 0.172549, 0.752941
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[8] = {-0.300000, -0.350000, 0.333333, -0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r2, 	c8
			mov 	r4, 	c11
			add 	r4, 	v0, 	r2.xyzw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x add r5, v1.xwy, r3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.000000, -0.200000, -0.400000, 0.321569
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[8] = {0.501961, 0.701961, 0.901961, 0.180392};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r3, 	c8
			mov 	r5, 	c11
			add 	r5, 	v1.xwy, 	r3.y
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps2_x add r6, v0.yzxw, r4.xwz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.192157, 0.250980, 0.600000, 0.952941
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[8] = {0.313726, 0.250980, -0.100000, -0.450000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r4, 	c8
			mov 	r6, 	c11
			add 	r6, 	v0.yzxw, 	r4.xwz
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps2_x add r7.yw, v1, r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.500000, 0.301961, 0.501961, -0.000000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[8] = {1.000000, 0.200000, 0.000000, 0.501961};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r5, 	c8
			mov 	r7, 	c11
			add 	r7.yw, 	v1, 	r5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps2_x add r8, -v0, -r6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.400000, 1.050000, 0.211765, -0.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD0, 	c4			
		};
		
		PixelShaderConstant[8] = {0.101961, -0.550000, 0.290196, 0.752941};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v0
			mov 	r6, 	c8
			mov 	r8, 	c11
			add 	r8, 	-v0, 	-r6
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps2_x add_sat r9, v1, r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.000000, 0.200000, -0.600000, 0.282353
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oD1, 	c4			
		};
		
		PixelShaderConstant[8] = {-0.500000, 0.301961, 1.100000, 0.223529};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	v1
			mov 	r7, 	c8
			mov 	r9, 	c11
			add_sat 	r9, 	v1, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, t0, c8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.230000, -0.250000, 0.200000, 1.150000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[8] = {0.270001, 0.750000, 0.300000, -0.650000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			mov 	r8, 	c8
			mov 	r10, 	c11
			add 	r10, 	t0, 	c8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, t1.z, c9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.700000, 0.260001, 0.500000, 0.400000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT1, 	c4			
		};
		
		PixelShaderConstant[9] = {1.200000, 0.240000, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t1
			mov 	r8, 	c9
			mov 	r10, 	c11
			add 	r10, 	t1.z, 	c9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, t2, c11.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.000000, 1.250000, 0.250000, 0.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT2, 	c4			
		};
		
		PixelShaderConstant[11] = {0.500000, -0.750000, 0.250001, 0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t2
			mov 	r8, 	c11
			mov 	r10, 	c12
			add 	r10, 	t2, 	c11.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, t3.yxz, c11.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.000000, 0.600000, -0.800000, 0.240001
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT3, 	c4			
		};
		
		PixelShaderConstant[11] = {0.500000, -0.100000, 1.300000, 0.260000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t3
			mov 	r8, 	c11
			mov 	r10, 	c13
			add 	r10, 	t3.yxz, 	c11.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, t4.wzyx, c12.yxw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.270000, 0.750000, -0.200000, 1.350000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT4, 	c4			
		};
		
		PixelShaderConstant[12] = {0.230001, -0.250000, 0.700000, -0.850000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t4
			mov 	r8, 	c12
			mov 	r10, 	c14
			add 	r10, 	t4.wzyx, 	c12.yxw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10.zw, t5, c13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.900000, 0.220001, -0.500000, 0.800000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT5, 	c4			
		};
		
		PixelShaderConstant[13] = {1.400000, 0.280000, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t5
			mov 	r8, 	c13
			mov 	r10, 	c15
			add 	r10.zw, 	t5, 	c13
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, -t6, c14";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.400000, 1.450000, 0.290000, 1.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT6, 	c4			
		};
		
		PixelShaderConstant[14] = {0.900000, -0.950000, 0.210001, -0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t6
			mov 	r8, 	c14
			mov 	r10, 	c16
			add 	r10, 	-t6, 	c14
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add_pp r10, t7, c15";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -1.000000, 1.000000, -1.000000, 0.200001
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT7, 	c4			
		};
		
		PixelShaderConstant[15] = {1.500000, -0.500000, 1.500000, 0.300000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t7
			mov 	r8, 	c15
			mov 	r10, 	c17
			add_pp 	r10, 	t7, 	c15
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, t0, r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.310000, 1.250000, -0.400000, 1.450000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT0, 	c4			
		};
		
		PixelShaderConstant[16] = {0.190001, -0.750000, 0.900000, -0.950000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t0
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	t0, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, t1.x, r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.900000, 0.180001, -0.500000, 0.800000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT1, 	c4			
		};
		
		PixelShaderConstant[16] = {1.400000, 0.320000, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t1
			mov 	r9, 	c16
			mov 	r11, 	c18
			add 	r11, 	t1.x, 	r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps2_x add r10, t2, r10.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.200000, 1.350000, 0.330000, 0.750000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT2, 	c4			
		};
		
		PixelShaderConstant[16] = {0.700000, -0.850000, 0.170001, -0.250000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t2
			mov 	r10, 	c16
			mov 	r0, 	c18
			add 	r0, 	t2, 	r10.xyzw
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps2_x add r1, t3.yzx, r11.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.000000, 0.600000, -0.800000, 0.160001
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT3, 	c4			
		};
		
		PixelShaderConstant[16] = {0.500000, -0.100000, 1.300000, 0.340000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t3
			mov 	r11, 	c16
			mov 	r1, 	c18
			add 	r1, 	t3.yzx, 	r11.y
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps2_x add r2, t4.yzxw, r0.yzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.350000, 0.250000, -0.000000, 1.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT4, 	c4			
		};
		
		PixelShaderConstant[16] = {0.150001, 0.250000, 0.500000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t4
			mov 	r0, 	c16
			mov 	r2, 	c18
			add 	r2, 	t4.yzxw, 	r0.yzw
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps2_x add r3.xyz, t5, r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, -0.700000, 0.140001, 0.500000, 0.400000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT5, 	c4			
		};
		
		PixelShaderConstant[16] = {1.200000, 0.360000, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t5
			mov 	r1, 	c16
			mov 	r3, 	c18
			add 	r3.xyz, 	t5, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps2_x add r4, t6, -r2";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 0.200000, 1.150000, 0.370000, -0.250000
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT6, 	c4			
		};
		
		PixelShaderConstant[16] = {0.300000, -0.650000, 0.130001, 0.750000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t6
			mov 	r2, 	c16
			mov 	r4, 	c18
			add 	r4, 	t6, 	-r2
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps2_x add_sat r5, t7, r3";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
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
			def c4, 1.000000, 0.200000, -0.600000, 0.120001
			//input registers
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			//output registers
			
			m4x4 	oPos, 	v0, 	c0
			mov 	oT7, 	c4			
		};
		
		PixelShaderConstant[16] = {-0.500000, 0.300000, 1.100000, 0.380000};
		
		PixelShader =
		asm
		{
			ps_2_x
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl 	t7
			mov 	r3, 	c16
			mov 	r5, 	c18
			add_sat 	r5, 	t7, 	r3
			mov 	oC0, 	r5

		};
	}
}
//cases with predicates
Technique
<
	String	Name = "Inst: (p0) add r4, r4, c1";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {0.0,	1.0,	0.0,	1.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x

			//def			c0,		0.0,	1.0,	0.0,	1.0
			def			c1,		1.0,	1.0,	1.0,	1.0

			mov			r0,		c0
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0

			mov			r4,		c0.x		// r4 = ( 0, 0, 0, 0 )
	(p0)	add			r4,		r4,		c1

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: (p0.x) add r4, r4, c0";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {1.0,	1.0,	1.0,	1.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x

			//def			c0,		1.0,	1.0,	1.0,	1.0
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0

			mov			r4,		c1.y		// r4 = ( 0, 0, 0, 0 )
	(p0.x)	add			r4,		r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: (p0(masked)) add r4, r4, c0";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {1.0,	1.0,	1.0,	1.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x

			//def			c0,		1.0,	1.0,	1.0,	1.0
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4.xw,	c1.y
			mov			r4.yz,	c1.x			// r4 = ( 0, 1, 1, 0 )
	(p0)	add			r4,		r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: (p0(masked).x) add r4, r4, c0";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {1.0,	1.0,	1.0,	1.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x

			//def			c0,		1.0,	1.0,	1.0,	1.0
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4,		c1.y			// r4 = ( 0, 0, 0, 0 )
	(p0.x)	add			r4,		r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: (!p0(masked)) add r4, r4, c0";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {1.0,	1.0,	1.0,	1.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x

			//def			c0,		1.0,	1.0,	1.0,	1.0
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4.xw,	c1.x
			mov			r4.yz,	c1.y			// r4 = ( 1, 0, 0, 1 )
	(!p0)	add			r4,		r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: (!p0(masked).y) add r4, r4, c0";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {1.0,	1.0,	1.0,	1.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x

			//def			c0,		1.0,	1.0,	1.0,	1.0
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4,		c1.y			// r4 = ( 0, 0, 0, 0 )
	(!p0.y)	add			r4,		r4,		c0

			mov			oC0,	r4
		};
	}
}

Technique
<
	String	Name = "Inst: (!p0(masked).y) add r4.yz, r4, c0";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
        PixelShaderConstant[0] = {1.0,	1.0,	1.0,	1.0};
		VertexShader = <StandardVS>;

		PixelShader =
		asm
		{
			ps_2_x

			//def			c0,		1.0,	1.0,	1.0,	1.0
			def			c1,		1.0,	0.0,	0.0,	1.0

			mov			r0,		c1
			mov			r1,		c1
			
			setp_eq		p0,		c0,		r0		// p0 = ( t, f, f, t )

			mov			r4.yz,	r1.y			// r4 = ( x, 0, 0, x )
			mov			r4.xw,	c1.x			// r4 = ( 1, 0, 0, 1 )
	(!p0.y)	add			r4.yz,	r4,		c0

			mov			oC0,	r4
		};
	}
}

