#include "include.fx"

//cases with src0 = c#
Technique
<
	String	Name = "Inst: add c0, r1";
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
			ps_3_0

			def			c0,		0.5,	1.0,	0.25,	-1.0
			def			c1,		0.0,	-0.5,	0.25,	0.0

			mov			r1,		c1
			add			r2,		c0,		r1
			mov			oC0,	r2
		};
	}
}
Technique
<
	String	Name = "Inst: add c1, v0 (texcoord)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		
		VertexShader =
		asm
		
		{
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			
			dcl_position	o0
			dcl_texcoord0	o1
			
			//we want t.x=[-0.5, 0.5], t.y=[1,2], t.z=[0,1], t.w=[-1,0]
			//t.z=(x+y)/2, t.w=(x+y)/2-1
			//we write out t.xyzw
			def c4, -0.5, 1.0, 0, -1.0
			mov r0, v2.xy
			add r0.zw, r0,  v2.x
			mul r0.zw, r0, -c4.x
			add r0, r0, c4
			m4x4 o0, v0, c0
			mov  o1, r0
		};

		PixelShader =
		asm
		{
			ps_3_0
			dcl_texcoord0 v0
			def			c1,		0.5,	-1.0,	0.0,	1.0
			
			add			r1,		c1,		v0
			mov			oC0,	r1
		};
	}
}
Technique
<
	String	Name = "Inst: add c2, v1 (blendweight)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		
		VertexShader =
		asm
		
		{
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			
			dcl_position	 o0
			dcl_texcoord0	 o1
			dcl_blendweight0 o2
			
			//we want t.x=[-0.5, 0.5], t.y=[1,2], t.z=[0,1], t.w=[-1,0]
			//t.z=(x+y)/2, t.w=(x+y)/2-1
			//we write out t.yzwx
			def c4, 1.0, 0, -1.0, -0.5
			mov r0, v2.yyyx
			add r0.yz, r0,  v2.x
			mul r0.yz, r0, -c4.w
			add r0, r0, c4
			m4x4 o0, v0, c0
			mov	 o1, v2.xy
			mov  o2, r0
		};

		PixelShader =
		asm
		{
			ps_3_0
			dcl_blendweight v1
			def			c2,		-1.0,	0.0,	1.0, 0.5
			
			add			r1,		c2,		v1
			mov			oC0,	r1
		};
	}
}
Technique
<
	String	Name = "Inst: add c3, v2 (blendindices)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		
		VertexShader =
		asm
		
		{
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			
			dcl_position	o0
			dcl_texcoord0	o1
			dcl_blendindices o2
			
			//we want t.x=[-0.5, 0.5], t.y=[1,2], t.z=[0,1], t.w=[-1,0]
			//t.z=(x+y)/2, t.w=(x+y)/2-1
			//we write out t.zwxy
			def c4, 0, -1.0, -0.5, 1.0
			mov r0, v2.yyxy
			add r0.xy, r0,  v2.x
			mul r0.xy, r0, -c4.z
			add r0, r0, c4
			m4x4 o0, v0, c0
			mov	 o1, v2.xy
			mov  o2, r0
		};

		PixelShader =
		asm
		{
			ps_3_0
			dcl_blendindices v2
			def			c3,		0.0, 1.0, 0.5, -1.0
			
			add			r1,		c3,		v2
			mov			oC0,	r1
		};
	}
}
Technique
<
	String	Name = "Inst: add c4, vPos";
	String	Shape = "TexTL";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;
		PixelShaderConstant[4] = <fvPosLimits>;

		PixelShader =
		asm
		{
			ps_3_0
			dcl vPos.xy
			def	c1,		256.0,	0.0,	20,		0.0

			add			r2,		-c4,	vPos.xy
			mov			r2.zw,	c1.y
			mul			r2,		r2,		c1.z
			rcp			r1,		c1.x
			mul			r1,		r1,		r2
			frc			r2,		r1
			mov			oC0,	r2
		};
	}
}

//cases with src0 = r#
Technique
<
	String	Name = "Inst: add r0, c0";
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
			ps_3_0

			def			c0,		-0.833,	5.725,	0.27778,	1.00005
			def			c1,		1.333,	-5.225,	0.22222,	-0.050005

			mov			r0,		c1
			add			r2,		r0,		c0
			mov			oC0,	r2
		};
	}
}
Technique
<
	String	Name = "Inst: add r1, r2";
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
			ps_3_0

			def			c0,		0.12345,	-0.775768,	0.25,	-1.0
			def			c1,		0.37655,	1.275768,	0.25,	0.0

			mov			r1,		c0
			mov			r2,		c1
			add			r2,		r1,		r2
			mov			oC0,	r2
		};
	}
}
Technique
<
	String	Name = "Inst: add r4, v3 (normal)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		
		VertexShader =
		asm
		
		{
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			
			dcl_position	o0
			dcl_texcoord0	o1
			dcl_normal      o2
			
			//we want t.x=[-0.5, 0.5], t.y=[1,2], t.z=[0,1], t.w=[-1,0]
			//t.z=(x+y)/2, t.w=(x+y)/2-1
			//we write out t.wxyz
			def c4, -1.0, -0.5, 1.0, 0
			mov r0, v2.yxyy
			add r0.xw, r0,  v2.x
			mul r0.xw, r0, -c4.y
			add r0, r0, c4
			m4x4 o0, v0, c0
			mov	 o1, v2.xy
			mov  o2, r0
		};

		PixelShader =
		asm
		{
			ps_3_0
			dcl_normal v3
			def			c4,		1.0, 0.5, -1.0, 0.0
			
			mov			r4,		c4
			add			r1,		r4,		v3
			mov			oC0,	r1
		};
	}
}
Technique
<
	String	Name = "Inst: add r5, v4 (tangent)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		
		VertexShader =
		asm
		
		{
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			
			dcl_position	o0
			dcl_texcoord0   o1
			dcl_tangent     o2
			
			//we want t.x=[-0.5, 0.5], t.y=[1,2], t.z=[0,1], t.w=[-1,0]
			//t.z=(x+y)/2, t.w=(x+y)/2-1
			//we write out t.yxwz
			def c4, 1.0, -0.5, -1.0, 0
			mov r0, v2.yxyy
			add r0.zw, r0,  v2.x
			mul r0.zw, r0, -c4.y
			add r0, r0, c4
			m4x4 o0, v0, c0
			mov	 o1, v2.xy
			mov  o2, r0
		};

		PixelShader =
		asm
		{
			ps_3_0
			dcl_tangent v4
			def			c5,		-1.0, 0.5, 1.0, 0.0
			
			mov			r5,		c5
			add			r1,		r5,		v4
			mov			oC0,	r1
		};
	}
}
Technique
<
	String	Name = "Inst: add r6, v5 (binormal)";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		
		VertexShader =
		asm
		
		{
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			
			dcl_position	o0
			dcl_texcoord0	o1
			dcl_binormal    o2
			
			//we want t.x=[-0.5, 0.5], t.y=[1,2], t.z=[0,1], t.w=[-1,0]
			//t.z=(x+y)/2, t.w=(x+y)/2-1
			//we write out t.zyxw
			def c4, 0.0, 1.0, -0.5, -1.0
			mov r0, v2.yyxy
			add r0.xw, r0,  v2.x
			mul r0.xw, r0, -c4.z
			add r0, r0, c4
			m4x4 o0, v0, c0
			mov	 o1, v2.xy
			mov  o2, r0
		};

		PixelShader =
		asm
		{
			ps_3_0
			dcl_binormal v5
			def			c6,		0.0, -1.0, 0.5, 1.0
			
			mov			r6,		c6
			add			r1,		r6,		v5
			mov			oC0,	r1
		};
	}
}
Technique
<
	String	Name = "Inst: add r0, vPos";
	String	Shape = "TexTL";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = <StandardVS>;
		PixelShaderConstant[0] = <fvPosLimits>;

		PixelShader =
		asm
		{
			ps_3_0
			dcl vPos.xy
			def	c1,		256.0,	0.0,	20,	0.0

			mov			r0,		-c0
			add			r2,		r0,		vPos.xy
			mov			r2.zw,	c1.y
			mul			r2,		r2,		c1.z
			rcp			r1,		c1.x
			mul			r1,		r1,		r2
			frc			r2,		r1
			mov			oC0,	r2
		};
	}
}

//cases with src0 = v#
Technique
<
	String	Name = "Inst: add v0 (color), c0";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		
		VertexShader =
		asm
		
		{
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			
			dcl_position	o0
			dcl_color0	o1
			
			//we want t.x=[0.25, 1.25], t.y=[2.3, 3.3], t.z=[1/3, 2/3], t.w=[-0.5, 0]
			//t.z=(x+y)/3, t.w=(x+y)/4-0.5
			//we write out t.xyzw
			def		c4,		0.25,	2.3,	0,			-0.5
			def		c5,		1,		1,		0.333333,	0.25
			mov		r0,		v2.xy
			add		r0.zw,	r0,		v2.x	//mov (x+y) to .zw
			mul		r0,		r0,		c5
			add		r0,		r0,		c4
			m4x4	o0,		v0,		c0
			mov		o1,		r0
		};

		PixelShader =
		asm
		{
			ps_3_0
			dcl_color0 v0
			def		c0,		-0.25,	-2.3,	0.33333,	0.75
			
			add		r2,		v0,		c0
			mov		oC0,	r2
		};
	}
}
Technique
<
	String	Name = "Inst: add v1 (fog), r0";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		
		VertexShader =
		asm
		
		{
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			
			dcl_position	o0
			dcl_fog0	o1
			
			//we want t.x=[0.25, 1.25], t.y=[2.3, 3.3], t.z=[1/3, 2/3], t.w=[-0.5, 0]
			//t.z=(x+y)/3, t.w=(x+y)/4-0.5
			//we write out t.yzwx
			def		c4,		2.3,	0,			-0.5,	0.25
			def		c5,		1,		0.333333,	0.25,	1
			mov		r0,		v2.yyyx
			mov		r0,		v2.xy
			add		r0.zw,	r0,		v2.x	//mov (x+y) to .zw
			mul		r0,		r0,		c5
			add		r0,		r0,		c4
			m4x4	o0,		v0,		c0
			mov		o1,		r0
		};

		PixelShader =
		asm
		{
			ps_3_0
			dcl_fog v1
			def		c0,		-2.3,	0.33333,	0.75,	-0.25
			
			mov		r0,		c0
			add		r2,		v1,		r0
			mov		oC0,	r2
		};
	}
}
Technique
<
	String	Name = "Inst: add v3 (depth), vPos";
	String	Shape = "TinyQuad";
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShaderConstant[4] = <fvPosLimits>;

		VertexShader =
		asm
		
		{
			vs_3_0
			dcl_position	v0
			dcl_normal		v1
			dcl_texcoord0	v2
			
			dcl_position	o0
			dcl_depth0		o1
			
			mov		r0,		-c4
			m4x4	o0,		v0,		c0
			mov		o1,		r0
		};
		
		PixelShader =
		asm
		{
			ps_3_0
			dcl_depth v3
			dcl vPos.xy
			def	c1,		256.0,	0.0,	20,		0.0

			add			r2,		v3,		vPos.xy
			mov			r2.zw,	c1.y
			mul			r2,		r2,		c1.z
			rcp			r1,		c1.x
			mul			r1,		r1,		r2
			frc			r2,		r1
			mov			oC0,	r2
		};
	}
}
Technique
<
	String	Name = "ps3_0 add c0, r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.390000, -0.250000, 0.400000, 1.050000
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
		
		PixelShaderConstant[0] = {0.110001, 0.750000, 0.100000, -0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c1, 	0.390000, 	-0.250000, 	0.400000, 	1.050000
			mov 	r0, 	c1
			mov 	r2, 	c2
			add 	r2, 	c0, 	r0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c1.z, r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, 0.100001, 0.500000, -0.000000
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
		
		PixelShaderConstant[1] = {1.000000, 0.400000, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c2, 	-0.500000, 	0.100001, 	0.500000, 	-0.000000
			mov 	r1, 	c2
			mov 	r3, 	c3
			add 	r3, 	c1.z, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c2, r2.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.950000, 0.410000, 0.250000
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
		
		PixelShaderConstant[2] = {-0.100000, -0.450000, 0.090001, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c3, 	0.600000, 	0.950000, 	0.410000, 	0.250000
			mov 	r2, 	c3
			mov 	r4, 	c4
			add 	r4, 	c2, 	r2.zxyw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c3.ywx, r3.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, 0.080001
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
		
		PixelShaderConstant[3] = {0.500000, 0.700000, 0.900000, 0.420000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c4, 	0.000000, 	-0.200000, 	-0.400000, 	0.080001
			mov 	r3, 	c4
			mov 	r5, 	c5
			add 	r5, 	c3.ywx, 	r3.w
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c4.wzyx, r4.ywz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.430000, 0.750000, 0.800000, 0.850000
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
		
		PixelShaderConstant[4] = {0.070001, -0.250000, -0.300000, -0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c5, 	0.430000, 	0.750000, 	0.800000, 	0.850000
			mov 	r4, 	c5
			mov 	r6, 	c6
			add 	r6, 	c4.wzyx, 	r4.ywz
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyw c5, r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, 0.060001, -0.500000, -0.400000
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
		
		PixelShaderConstant[5] = {0.800000, 0.440000, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c6, 	-0.300000, 	0.060001, 	-0.500000, 	-0.400000
			mov 	r5, 	c6
			mov 	r7, 	c7
			add 	r7.xyw, 	c5, 	r5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c6, r6_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.750000, 0.450000, 1.250000
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
		
		PixelShaderConstant[6] = {-0.500000, -0.250000, 0.050001, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c7, 	1.000000, 	0.750000, 	0.450000, 	1.250000
			mov 	r6, 	c7
			mov 	r8, 	c8
			add 	r8, 	c6, 	r6_abs
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp c7, r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, 0.040001
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
		
		PixelShaderConstant[7] = {1.500000, 1.100000, 0.700000, 0.460000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			def 	c8, 	-1.000000, 	-0.600000, 	-0.200000, 	0.040001
			mov 	r7, 	c8
			mov 	r9, 	c9
			add_pp 	r9, 	c7, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c8, v0(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.470000, 1.250000, 1.200000, 0.650000
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
		
		PixelShaderConstant[8] = {0.030001, -0.750000, -0.700000, -0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			mov 	r8, 	c8
			mov 	r10, 	c11
			add 	r10, 	c8, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c9.x, v1(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, 0.020001, -0.500000, -0.800000
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
		
		PixelShaderConstant[9] = {0.600000, 0.480000, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord1 	v1
			mov 	r8, 	c9
			mov 	r10, 	c11
			add 	r10, 	c9.x, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c11, v2.xyzw(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.550000, 0.490000, 0.750000
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
		
		PixelShaderConstant[11] = {-0.900000, -0.050000, 0.010001, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			mov 	r8, 	c11
			mov 	r10, 	c12
			add 	r10, 	c11, 	v2.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c11.zxy, v3.y(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, 0.000001
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
		
		PixelShaderConstant[11] = {0.500000, 1.500000, 0.500000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord3 	v3
			mov 	r8, 	c11
			mov 	r10, 	c13
			add 	r10, 	c11.zxy, 	v3.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c12.yzxw, v4.zxw(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.510000, 0.250000, 1.400000, 0.450000
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
		
		PixelShaderConstant[12] = {-0.009999, 0.250000, -0.900000, 0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			mov 	r8, 	c12
			mov 	r10, 	c14
			add 	r10, 	c12.yzxw, 	v4.zxw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xzw c13, v5(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, -0.019999, 0.500000, -0.800000
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
		
		PixelShaderConstant[13] = {0.400000, 0.520000, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			mov 	r8, 	c13
			mov 	r10, 	c15
			add 	r10.xzw, 	c13, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c14_abs, -v6(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.350000, 0.530000, -0.250000
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
		
		PixelShaderConstant[14] = {-0.700000, 0.150000, -0.029999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			mov 	r8, 	c14
			mov 	r10, 	c16
			add 	r10, 	c14_abs, 	-v6
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat c15, v7(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, -0.039999
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
		
		PixelShaderConstant[15] = {-0.500000, 1.100000, 0.300000, 0.540000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			mov 	r8, 	c15
			mov 	r10, 	c17
			add_sat 	r10, 	c15, 	v7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c16, v8(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.550000, -0.250000, 1.000000, 0.250000
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
		
		PixelShaderConstant[16] = {-0.049999, 0.750000, -0.500000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight8 	v8
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	c16, 	v8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c17.z, v9(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, -0.059999, 0.500000, -0.400000
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
		
		PixelShaderConstant[17] = {0.200000, 0.560000, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight9 	v9
			mov 	r8, 	c17
			mov 	r10, 	c19
			add 	r10, 	c17.z, 	v9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c18, v0.zxyw(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.150000, 0.570000, 0.250000
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
		
		PixelShaderConstant[18] = {-0.300000, 0.350000, -0.069999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			mov 	r8, 	c18
			mov 	r10, 	c20
			add 	r10, 	c18, 	v0.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c19.zyx, v1.w(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, -0.079999
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
		
		PixelShaderConstant[19] = {0.500000, 0.700000, 0.100000, 0.580000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			mov 	r8, 	c19
			mov 	r10, 	c21
			add 	r10, 	c19.zyx, 	v1.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c20.wzyx, v2.zyw(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.590000, 0.750000, 0.600000, 0.050000
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
		
		PixelShaderConstant[20] = {-0.089999, -0.250000, -0.100000, 0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight2 	v2
			mov 	r8, 	c20
			mov 	r10, 	c22
			add 	r10, 	c20.wzyx, 	v2.zyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yzw c21, v3(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, -0.099999, -0.500000, -0.000000
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
		
		PixelShaderConstant[21] = {0.000000, 0.600000, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight3 	v3
			mov 	r8, 	c21
			mov 	r10, 	c23
			add 	r10.yzw, 	c21, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -c22, -v4(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, -0.050000, 0.610000, 1.250000
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
		
		PixelShaderConstant[22] = {0.100000, 0.550000, -0.109999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight4 	v4
			mov 	r8, 	c22
			mov 	r10, 	c24
			add 	r10, 	-c22, 	-v4
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp c23, v5(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, -0.119999
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
		
		PixelShaderConstant[23] = {1.500000, 0.300000, -0.100000, 0.620000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight5 	v5
			mov 	r8, 	c23
			mov 	r10, 	c25
			add_pp 	r10, 	c23, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c24, v6(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.630000, 1.250000, 0.200000, -0.150000
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
		
		PixelShaderConstant[24] = {-0.129999, -0.750000, 0.300000, 0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices6 	v6
			mov 	r8, 	c24
			mov 	r10, 	c26
			add 	r10, 	c24, 	v6
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c25.x, v7(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, -0.139999, -0.500000, 0.400000
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
		
		PixelShaderConstant[25] = {-0.200000, 0.640000, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices7 	v7
			mov 	r8, 	c25
			mov 	r10, 	c27
			add 	r10, 	c25.x, 	v7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c26, v8.xyzw(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, -0.250000, 0.649999, 0.750000
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
		
		PixelShaderConstant[26] = {0.500000, 0.750000, -0.149999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices8 	v8
			mov 	r8, 	c26
			mov 	r10, 	c28
			add 	r10, 	c26, 	v8.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c27.zwx, v9.y(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, -0.159999
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
		
		PixelShaderConstant[27] = {0.500000, -0.100000, -0.300000, 0.659999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices9 	v9
			mov 	r8, 	c27
			mov 	r10, 	c29
			add 	r10, 	c27.zwx, 	v9.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c28.yzxw, v0.zwy(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.669999, 0.250000, -0.200000, -0.350000
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
		
		PixelShaderConstant[28] = {-0.169999, 0.250000, 0.700000, 0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices0 	v0
			mov 	r8, 	c28
			mov 	r10, 	c30
			add 	r10, 	c28.yzxw, 	v0.zwy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyzw c29, v1(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, -0.179999, 0.500000, 0.800000
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
		
		PixelShaderConstant[29] = {-0.400000, 0.679999, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices1 	v1
			mov 	r8, 	c29
			mov 	r10, 	c31
			add 	r10.xyzw, 	c29, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c30_abs, v2_abs(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, -0.450000, 0.689999, -0.250000
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
		
		PixelShaderConstant[30] = {0.900000, 0.950000, -0.189999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			mov 	r8, 	c30
			mov 	r10, 	c0
			add 	r10, 	c30_abs, 	v2_abs
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat c31, v3(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 1.000000, 1.000000, -0.199999
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
		
		PixelShaderConstant[31] = {-0.500000, -0.500000, -0.500000, 0.699999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			mov 	r8, 	c31
			mov 	r10, 	c1
			add_sat 	r10, 	c31, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c0, v4(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.709999, -0.250000, -0.400000, -0.450000
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
		
		PixelShaderConstant[0] = {-0.209999, 0.750000, 0.900000, 0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal4 	v4
			mov 	r8, 	c0
			mov 	r10, 	c2
			add 	r10, 	c0, 	v4
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c1.z, v5(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, -0.219999, 0.500000, 0.800000
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
		
		PixelShaderConstant[1] = {-0.400000, 0.719999, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal5 	v5
			mov 	r8, 	c1
			mov 	r10, 	c3
			add 	r10, 	c1.z, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c2, v6.zxyw(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, -0.350000, 0.729999, 0.250000
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
		
		PixelShaderConstant[2] = {0.700000, 0.850000, -0.229999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			mov 	r8, 	c2
			mov 	r10, 	c4
			add 	r10, 	c2, 	v6.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c3.wxy, v7.w(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, -0.239999
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
		
		PixelShaderConstant[3] = {0.500000, -0.100000, -0.300000, 0.739999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			mov 	r8, 	c3
			mov 	r10, 	c5
			add 	r10, 	c3.wxy, 	v7.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c4.wzyx, v8.wxz(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.749999, 0.750000, -0.000000, -0.250000
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
		
		PixelShaderConstant[4] = {-0.249999, -0.250000, 0.500000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			mov 	r8, 	c4
			mov 	r10, 	c6
			add 	r10, 	c4.wzyx, 	v8.wxz
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.x c5, v9(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, -0.259999, -0.500000, 0.400000
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
		
		PixelShaderConstant[5] = {-0.200000, 0.759999, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			mov 	r8, 	c5
			mov 	r10, 	c7
			add 	r10.x, 	c5, 	v9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c6_abs, v0(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, -0.150000, 0.769999, 1.250000
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
		
		PixelShaderConstant[6] = {0.300000, 0.650000, -0.269999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal0 	v0
			mov 	r8, 	c6
			mov 	r10, 	c8
			add 	r10, 	c6_abs, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp c7, v1(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, -0.279999
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
		
		PixelShaderConstant[7] = {1.500000, 0.300000, -0.100000, 0.779999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal1 	v1
			mov 	r8, 	c7
			mov 	r10, 	c9
			add_pp 	r10, 	c7, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c8, v2(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.789999, 1.250000, 0.400000, -0.050000
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
		
		PixelShaderConstant[8] = {-0.289999, -0.750000, 0.100000, 0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent2 	v2
			mov 	r8, 	c8
			mov 	r10, 	c11
			add 	r10, 	c8, 	v2
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c9.x, v3(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, -0.299999, -0.500000, -0.000000
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
		
		PixelShaderConstant[9] = {0.000000, 0.799999, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent3 	v3
			mov 	r8, 	c9
			mov 	r10, 	c11
			add 	r10, 	c9.x, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c11, v4.xyzw(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.050000, 0.809999, 0.750000
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
		
		PixelShaderConstant[11] = {-0.100000, 0.450000, -0.309999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent4 	v4
			mov 	r8, 	c11
			mov 	r10, 	c12
			add 	r10, 	c11, 	v4.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c11.wyx, v5.y(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, -0.319999
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
		
		PixelShaderConstant[11] = {0.500000, 0.700000, 0.100000, 0.819999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent5 	v5
			mov 	r8, 	c11
			mov 	r10, 	c13
			add 	r10, 	c11.wyx, 	v5.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c12.yzxw, v6.wyz(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.829999, 0.250000, 0.800000, 0.150000
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
		
		PixelShaderConstant[12] = {-0.329999, 0.250000, -0.300000, 0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent6 	v6
			mov 	r8, 	c12
			mov 	r10, 	c14
			add 	r10, 	c12.yzxw, 	v6.wyz
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.y c13, v7(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, -0.339999, 0.500000, -0.400000
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
		
		PixelShaderConstant[13] = {0.200000, 0.839999, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent7 	v7
			mov 	r8, 	c13
			mov 	r10, 	c15
			add 	r10.y, 	c13, 	v7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c14, -v8(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.250000, 0.849999, -0.250000
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
		
		PixelShaderConstant[14] = {-0.500000, 0.250000, -0.349999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent8 	v8
			mov 	r8, 	c14
			mov 	r10, 	c16
			add 	r10, 	c14, 	-v8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat c15, v9(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, -0.359999
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
		
		PixelShaderConstant[15] = {-0.500000, 1.100000, 0.300000, 0.859999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent9 	v9
			mov 	r8, 	c15
			mov 	r10, 	c17
			add_sat 	r10, 	c15, 	v9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c16, v0(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.869999, -0.250000, 1.200000, 0.350000
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
		
		PixelShaderConstant[16] = {-0.369999, 0.750000, -0.700000, 0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal0 	v0
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	c16, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c17.z, v1(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, -0.379999, 0.500000, -0.800000
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
		
		PixelShaderConstant[17] = {0.400000, 0.879999, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal1 	v1
			mov 	r8, 	c17
			mov 	r10, 	c19
			add 	r10, 	c17.z, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c18, v2.zxyw(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.450000, 0.889999, 0.250000
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
		
		PixelShaderConstant[18] = {-0.900000, 0.050000, -0.389999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal2 	v2
			mov 	r8, 	c18
			mov 	r10, 	c20
			add 	r10, 	c18, 	v2.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c19.wzx, v3.w(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, -0.399999
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
		
		PixelShaderConstant[19] = {0.500000, 1.500000, 0.500000, 0.899999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal3 	v3
			mov 	r8, 	c19
			mov 	r10, 	c21
			add 	r10, 	c19.wzx, 	v3.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c20.wzyx, v4.wzy(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.909999, 0.750000, 1.400000, 0.550000
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
		
		PixelShaderConstant[20] = {-0.409999, -0.250000, -0.900000, -0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			mov 	r8, 	c20
			mov 	r10, 	c22
			add 	r10, 	c20.wzyx, 	v4.wzy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.z c21, v5(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, -0.419999, -0.500000, -0.800000
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
		
		PixelShaderConstant[21] = {0.600000, 0.919999, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			mov 	r8, 	c21
			mov 	r10, 	c23
			add 	r10.z, 	c21, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -c22, v6_abs(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.650000, 0.929999, 1.250000
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
		
		PixelShaderConstant[22] = {-0.700000, -0.150000, -0.429999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			mov 	r8, 	c22
			mov 	r10, 	c24
			add 	r10, 	-c22, 	v6_abs
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp c23, v7(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, -0.439999
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
		
		PixelShaderConstant[23] = {1.500000, 1.100000, 0.700000, 0.939999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			mov 	r8, 	c23
			mov 	r10, 	c25
			add_pp 	r10, 	c23, 	v7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c24, v8(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.949999, 1.250000, 1.000000, 0.750000
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
		
		PixelShaderConstant[24] = {-0.449999, -0.750000, -0.500000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color8 	v8
			mov 	r8, 	c24
			mov 	r10, 	c26
			add 	r10, 	c24, 	v8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c25.x, v9(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, -0.459999, -0.500000, -0.400000
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
		
		PixelShaderConstant[25] = {0.800000, 0.959999, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color9 	v9
			mov 	r8, 	c25
			mov 	r10, 	c27
			add 	r10, 	c25.x, 	v9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c26, v0.xyzw(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.850000, 0.969999, 0.750000
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
		
		PixelShaderConstant[26] = {-0.300000, -0.350000, -0.469999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			mov 	r8, 	c26
			mov 	r10, 	c28
			add 	r10, 	c26, 	v0.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c27.xywz, v1.y(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, -0.479999
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
		
		PixelShaderConstant[27] = {0.500000, 0.700000, 0.900000, 0.979999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			mov 	r8, 	c27
			mov 	r10, 	c29
			add 	r10, 	c27.xywz, 	v1.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c28.yzxw, v2.xzyw(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.989999, 0.250000, 0.600000, 0.950000
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
		
		PixelShaderConstant[28] = {-0.489999, 0.250000, -0.100000, -0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color2 	v2
			mov 	r8, 	c28
			mov 	r10, 	c30
			add 	r10, 	c28.yzxw, 	v2.xzyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.w c29, v3(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, -0.499999, 0.500000, -0.000000
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
		
		PixelShaderConstant[29] = {1.000000, 0.999999, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color3 	v3
			mov 	r8, 	c29
			mov 	r10, 	c31
			add 	r10.w, 	c29, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -c30, -v4(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, 1.050000, 1.009999, -0.250000
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
		
		PixelShaderConstant[30] = {0.100000, -0.550000, -0.509999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color4 	v4
			mov 	r8, 	c30
			mov 	r10, 	c0
			add 	r10, 	-c30, 	-v4
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat c31, v5(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, -0.519999
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
		
		PixelShaderConstant[31] = {-0.500000, 0.300000, 1.100000, 1.019999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color5 	v5
			mov 	r8, 	c31
			mov 	r10, 	c1
			add_sat 	r10, 	c31, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c0, v6(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.029999, -0.250000, 0.200000, 1.150000
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
		
		PixelShaderConstant[0] = {-0.529999, 0.750000, 0.300000, -0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog6 	v6
			mov 	r8, 	c0
			mov 	r10, 	c2
			add 	r10, 	c0, 	v6
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c1.z, v7(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, -0.539999, 0.500000, 0.400000
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
		
		PixelShaderConstant[1] = {1.200000, 1.039999, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog7 	v7
			mov 	r8, 	c1
			mov 	r10, 	c3
			add 	r10, 	c1.z, 	v7
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c2, v8.zxyw(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, 1.250000, 1.049999, 0.250000
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
		
		PixelShaderConstant[2] = {0.500000, -0.750000, -0.549999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog8 	v8
			mov 	r8, 	c2
			mov 	r10, 	c4
			add 	r10, 	c2, 	v8.zxyw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c3.xzwy, v9.w(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, -0.559999
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
		
		PixelShaderConstant[3] = {0.500000, -0.100000, 1.300000, 1.059999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog9 	v9
			mov 	r8, 	c3
			mov 	r10, 	c5
			add 	r10, 	c3.xzwy, 	v9.w
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c4.wzyx, v0.xwyz(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.069999, 0.750000, -0.200000, 1.350000
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
		
		PixelShaderConstant[4] = {-0.569999, -0.250000, 0.700000, -0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog0 	v0
			mov 	r8, 	c4
			mov 	r10, 	c6
			add 	r10, 	c4.wzyx, 	v0.xwyz
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xy c5, v1(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, -0.579999, -0.500000, 0.800000
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
		
		PixelShaderConstant[5] = {1.400000, 1.079999, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog1 	v1
			mov 	r8, 	c5
			mov 	r10, 	c7
			add 	r10.xy, 	c5, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c6_abs, v2_abs(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, 1.450000, 1.089999, 1.250000
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
		
		PixelShaderConstant[6] = {0.900000, -0.950000, -0.589999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			mov 	r8, 	c6
			mov 	r10, 	c8
			add 	r10, 	c6_abs, 	v2_abs
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp c7, v3(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 1.000000, -1.000000, -0.599999
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
		
		PixelShaderConstant[7] = {1.500000, -0.500000, 1.500000, 1.099999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			mov 	r8, 	c7
			mov 	r10, 	c9
			add_pp 	r10, 	c7, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c8, v4(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.109999, 1.250000, -0.400000, 1.450000
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
		
		PixelShaderConstant[8] = {-0.609999, -0.750000, 0.900000, -0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth4 	v4
			mov 	r8, 	c8
			mov 	r10, 	c11
			add 	r10, 	c8, 	v4
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c9.x, v5(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, -0.619999, -0.500000, 0.800000
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
		
		PixelShaderConstant[9] = {1.400000, 1.119999, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth5 	v5
			mov 	r8, 	c9
			mov 	r10, 	c11
			add 	r10, 	c9.x, 	v5
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c11, v6.xyzw(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, 1.350000, 1.129999, 0.750000
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
		
		PixelShaderConstant[11] = {0.700000, -0.850000, -0.629999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			mov 	r8, 	c11
			mov 	r10, 	c12
			add 	r10, 	c11, 	v6.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c11.xwzy, v7.y(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, -0.639999
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
		
		PixelShaderConstant[11] = {0.500000, -0.100000, 1.300000, 1.139999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			mov 	r8, 	c11
			mov 	r10, 	c13
			add 	r10, 	c11.xwzy, 	v7.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c12.yzxw, v8.yxzw(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.149999, 0.250000, -0.000000, 1.250000
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
		
		PixelShaderConstant[12] = {-0.649999, 0.250000, 0.500000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			mov 	r8, 	c12
			mov 	r10, 	c14
			add 	r10, 	c12.yzxw, 	v8.yxzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xz c13, v9(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, -0.659999, 0.500000, 0.400000
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
		
		PixelShaderConstant[13] = {1.200000, 1.159999, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			mov 	r8, 	c13
			mov 	r10, 	c15
			add 	r10.xz, 	c13, 	v9
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -c14, v0(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, 1.150000, 1.169999, -0.250000
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
		
		PixelShaderConstant[14] = {0.300000, -0.650000, -0.669999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth0 	v0
			mov 	r8, 	c14
			mov 	r10, 	c16
			add 	r10, 	-c14, 	v0
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat c15, v1(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, -0.679999
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
		
		PixelShaderConstant[15] = {-0.500000, 0.300000, 1.100000, 1.179999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth1 	v1
			mov 	r8, 	c15
			mov 	r10, 	c17
			add_sat 	r10, 	c15, 	v1
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c16, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.189999, -0.250000, 0.400000, 1.050000
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
		
		PixelShaderConstant[16] = {-0.689999, 0.750000, 0.100000, -0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	c16, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c17.z, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, -0.699999, 0.500000, -0.000000
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
		
		PixelShaderConstant[17] = {1.000000, 1.199999, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c17
			mov 	r10, 	c19
			add 	r10, 	c17.z, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c18, vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.950000, 1.209999, 0.250000
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
		
		PixelShaderConstant[18] = {-0.100000, -0.450000, -0.709999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c18
			mov 	r10, 	c20
			add 	r10, 	c18, 	vPos.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c19.yxwz, vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, -0.719999
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
		
		PixelShaderConstant[19] = {0.500000, 0.700000, 0.900000, 1.219999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c19
			mov 	r10, 	c21
			add 	r10, 	c19.yxwz, 	vPos.x
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c20.wzyx, vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.229999, 0.750000, 0.800000, 0.850000
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
		
		PixelShaderConstant[20] = {-0.729999, -0.250000, -0.300000, -0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c20
			mov 	r10, 	c22
			add 	r10, 	c20.wzyx, 	vPos.yx
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xw c21, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, -0.739999, -0.500000, -0.400000
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
		
		PixelShaderConstant[21] = {0.800000, 1.239999, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c21
			mov 	r10, 	c23
			add 	r10.xw, 	c21, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add c22, vPos_abs.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.750000, 1.249999, 1.250000
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
		
		PixelShaderConstant[22] = {-0.500000, -0.250000, -0.749999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c22
			mov 	r10, 	c24
			add 	r10, 	c22, 	vPos_abs.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp c23, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, -0.759999
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
		
		PixelShaderConstant[23] = {1.500000, 1.100000, 0.700000, 1.259999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r8, 	c23
			mov 	r10, 	c25
			add_pp 	r10, 	c23, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r8, c24";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.269999, 1.250000, 1.200000, 0.650000
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
		
		PixelShaderConstant[24] = {-0.769999, -0.750000, -0.700000, -0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c25, 	1.269999, 	1.250000, 	1.200000, 	0.650000
			mov 	r8, 	c25
			mov 	r10, 	c26
			add 	r10, 	r8, 	c24
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r9.x, c25";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, -0.779999, -0.500000, -0.800000
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
		
		PixelShaderConstant[25] = {0.600000, 1.279999, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c26, 	-0.100000, 	-0.779999, 	-0.500000, 	-0.800000
			mov 	r9, 	c26
			mov 	r11, 	c27
			add 	r11, 	r9.x, 	c25
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r10, c26.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.550000, 1.289999, 0.750000
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
		
		PixelShaderConstant[26] = {-0.900000, -0.050000, -0.789999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c27, 	1.400000, 	0.550000, 	1.289999, 	0.750000
			mov 	r10, 	c27
			mov 	r12, 	c28
			add 	r12, 	r10, 	c26.xyzw
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r11.ywxz, c27.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, -0.799999
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
		
		PixelShaderConstant[27] = {0.500000, 1.500000, 0.500000, 1.299999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c28, 	0.000000, 	-1.000000, 	0.000000, 	-0.799999
			mov 	r11, 	c28
			mov 	r13, 	c29
			add 	r13, 	r11.ywxz, 	c27.y
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r12.yzxw, c28.ywzx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.309999, 0.250000, 1.400000, 0.450000
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
		
		PixelShaderConstant[28] = {-0.809999, 0.250000, -0.900000, 0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c29, 	1.309999, 	0.250000, 	1.400000, 	0.450000
			mov 	r12, 	c29
			mov 	r14, 	c30
			add 	r14, 	r12.yzxw, 	c28.ywzx
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yz r13, c29";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, -0.819999, 0.500000, -0.800000
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
		
		PixelShaderConstant[29] = {0.400000, 1.319999, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c30, 	0.100000, 	-0.819999, 	0.500000, 	-0.800000
			mov 	r13, 	c30
			mov 	r0, 	c31
			add 	r0.yz, 	r13, 	c29
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r14_abs, -c30";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.350000, 1.329999, -0.250000
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
		
		PixelShaderConstant[30] = {-0.700000, 0.150000, -0.829999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c31, 	1.200000, 	0.350000, 	1.329999, 	-0.250000
			mov 	r14, 	c31
			mov 	r1, 	c0
			add 	r1, 	r14_abs, 	-c30
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat r0, c31";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, -0.839999
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
		
		PixelShaderConstant[31] = {-0.500000, 1.100000, 0.300000, 1.339999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c0, 	1.000000, 	-0.600000, 	0.200000, 	-0.839999
			mov 	r0, 	c0
			mov 	r2, 	c1
			add_sat 	r2, 	r0, 	c31
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r1, r2";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.349999, -0.250000, 1.000000, 0.250000
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
		
		PixelShaderConstant[0] = {-0.849999, 0.750000, -0.500000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	1.349999, 	-0.250000, 	1.000000, 	0.250000
			mov 	r2, 	c1
			mov 	r1, 	c0
			mov 	r3, 	c2
			add 	r3, 	r1, 	r2
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r3.z, r4";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, -0.859999, 0.500000, -0.400000
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
		
		PixelShaderConstant[0] = {0.200000, 1.359999, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	0.300000, 	-0.859999, 	0.500000, 	-0.400000
			mov 	r4, 	c1
			mov 	r3, 	c0
			mov 	r5, 	c2
			add 	r5, 	r3.z, 	r4
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r5, r6.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.150000, 1.369999, 0.250000
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
		
		PixelShaderConstant[0] = {-0.300000, 0.350000, -0.869999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	0.800000, 	0.150000, 	1.369999, 	0.250000
			mov 	r6, 	c1
			mov 	r5, 	c0
			mov 	r7, 	c2
			add 	r7, 	r5, 	r6.zxyw
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r7.zxwy, r8.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, -0.879999
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
		
		PixelShaderConstant[0] = {0.500000, 0.700000, 0.100000, 1.379999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	0.000000, 	-0.200000, 	0.400000, 	-0.879999
			mov 	r8, 	c1
			mov 	r7, 	c0
			mov 	r9, 	c2
			add 	r9, 	r7.zxwy, 	r8.w
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r9.wzyx, r10.zyxw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.389999, 0.750000, 0.600000, 0.050000
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
		
		PixelShaderConstant[0] = {-0.889999, -0.250000, -0.100000, 0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	1.389999, 	0.750000, 	0.600000, 	0.050000
			mov 	r10, 	c1
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	r9.wzyx, 	r10.zyxw
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yw r11, r12";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, -0.899999, -0.500000, -0.000000
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
		
		PixelShaderConstant[0] = {0.000000, 1.399999, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	0.500000, 	-0.899999, 	-0.500000, 	-0.000000
			mov 	r12, 	c1
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13.yw, 	r11, 	r12
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -r13, -r14";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, -0.050000, 1.409999, 1.250000
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
		
		PixelShaderConstant[0] = {0.100000, 0.550000, -0.909999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	0.400000, 	-0.050000, 	1.409999, 	1.250000
			mov 	r14, 	c1
			mov 	r13, 	c0
			mov 	r0, 	c2
			add 	r0, 	-r13, 	-r14
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp r0, r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, -0.919999
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
		
		PixelShaderConstant[0] = {1.500000, 0.300000, -0.100000, 1.419999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			def 	c1, 	-1.000000, 	0.200000, 	0.600000, 	-0.919999
			mov 	r1, 	c1
			mov 	r0, 	c0
			mov 	r2, 	c2
			add_pp 	r2, 	r0, 	r1
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r2, v2(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.429999, 1.250000, 0.200000, -0.150000
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
		
		PixelShaderConstant[0] = {-0.929999, -0.750000, 0.300000, 0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			mov 	r2, 	c0
			mov 	r4, 	c2
			add 	r4, 	r2, 	v2
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r3.x, v3(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, -0.939999, -0.500000, 0.400000
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
		
		PixelShaderConstant[0] = {-0.200000, 1.439999, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord3 	v3
			mov 	r3, 	c0
			mov 	r5, 	c2
			add 	r5, 	r3.x, 	v3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r4, v4.xyzw(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, -0.250000, 1.449999, 0.750000
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
		
		PixelShaderConstant[0] = {0.500000, 0.750000, -0.949999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			mov 	r4, 	c0
			mov 	r6, 	c2
			add 	r6, 	r4, 	v4.xyzw
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r5.zywx, v5.y(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, -0.959999
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
		
		PixelShaderConstant[0] = {0.500000, -0.100000, -0.300000, 1.459999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			mov 	r5, 	c0
			mov 	r7, 	c2
			add 	r7, 	r5.zywx, 	v5.y
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r6.yzxw, v6.zwyx(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.469999, 0.250000, -0.200000, -0.350000
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
		
		PixelShaderConstant[0] = {-0.969999, 0.250000, 0.700000, 0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			mov 	r6, 	c0
			mov 	r8, 	c2
			add 	r8, 	r6.yzxw, 	v6.zwyx
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.zw r7, v7(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, -0.979999, 0.500000, 0.800000
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
		
		PixelShaderConstant[0] = {-0.400000, 1.479999, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			mov 	r7, 	c0
			mov 	r9, 	c2
			add 	r9.zw, 	r7, 	v7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r8_abs, v8_abs(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, -0.450000, 1.489999, -0.250000
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
		
		PixelShaderConstant[0] = {0.900000, 0.950000, -0.989999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord8 	v8
			mov 	r8, 	c0
			mov 	r10, 	c2
			add 	r10, 	r8_abs, 	v8_abs
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat r9, v9(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 1.000000, 1.000000, -0.999999
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
		
		PixelShaderConstant[0] = {-0.500000, -0.500000, -0.500000, 1.499999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord9 	v9
			mov 	r9, 	c0
			mov 	r11, 	c2
			add_sat 	r11, 	r9, 	v9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r10, v0(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.489999, -0.250000, -0.400000, -0.450000
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
		
		PixelShaderConstant[0] = {-0.989999, 0.750000, 0.900000, 0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			mov 	r10, 	c0
			mov 	r12, 	c2
			add 	r12, 	r10, 	v0
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r11.z, v1(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, -0.979999, 0.500000, 0.800000
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
		
		PixelShaderConstant[0] = {-0.400000, 1.479999, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13, 	r11.z, 	v1
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r12, v2.zxyw(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, -0.350000, 1.469999, 0.250000
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
		
		PixelShaderConstant[0] = {0.700000, 0.850000, -0.969999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight2 	v2
			mov 	r12, 	c0
			mov 	r14, 	c2
			add 	r14, 	r12, 	v2.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r13.wxyz, v3.w(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, -0.959999
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
		
		PixelShaderConstant[0] = {0.500000, -0.100000, -0.300000, 1.459999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight3 	v3
			mov 	r13, 	c0
			mov 	r0, 	c2
			add 	r0, 	r13.wxyz, 	v3.w
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r14.wzyx, v4.wxzy(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.449999, 0.750000, -0.000000, -0.250000
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
		
		PixelShaderConstant[0] = {-0.949999, -0.250000, 0.500000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight4 	v4
			mov 	r14, 	c0
			mov 	r1, 	c2
			add 	r1, 	r14.wzyx, 	v4.wxzy
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyz r0, v5(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, -0.939999, -0.500000, 0.400000
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
		
		PixelShaderConstant[0] = {-0.200000, 1.439999, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight5 	v5
			mov 	r0, 	c0
			mov 	r2, 	c2
			add 	r2.xyz, 	r0, 	v5
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r1_abs, v6(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, -0.150000, 1.429999, 1.250000
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
		
		PixelShaderConstant[0] = {0.300000, 0.650000, -0.929999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight6 	v6
			mov 	r1, 	c0
			mov 	r3, 	c2
			add 	r3, 	r1_abs, 	v6
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp r2, v7(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, -0.919999
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
		
		PixelShaderConstant[0] = {1.500000, 0.300000, -0.100000, 1.419999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight7 	v7
			mov 	r2, 	c0
			mov 	r4, 	c2
			add_pp 	r4, 	r2, 	v7
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r3, v8(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.409999, 1.250000, 0.400000, -0.050000
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
		
		PixelShaderConstant[0] = {-0.909999, -0.750000, 0.100000, 0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices8 	v8
			mov 	r3, 	c0
			mov 	r5, 	c2
			add 	r5, 	r3, 	v8
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r4.x, v9(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, -0.899999, -0.500000, -0.000000
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
		
		PixelShaderConstant[0] = {0.000000, 1.399999, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices9 	v9
			mov 	r4, 	c0
			mov 	r6, 	c2
			add 	r6, 	r4.x, 	v9
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r5, v0.xyzw(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.050000, 1.389999, 0.750000
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
		
		PixelShaderConstant[0] = {-0.100000, 0.450000, -0.889999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices0 	v0
			mov 	r5, 	c0
			mov 	r7, 	c2
			add 	r7, 	r5, 	v0.xyzw
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r6.wyxz, v1.y(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, -0.879999
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
		
		PixelShaderConstant[0] = {0.500000, 0.700000, 0.100000, 1.379999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices1 	v1
			mov 	r6, 	c0
			mov 	r8, 	c2
			add 	r8, 	r6.wyxz, 	v1.y
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r7.yzxw, v2.wyzx(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.369999, 0.250000, 0.800000, 0.150000
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
		
		PixelShaderConstant[0] = {-0.869999, 0.250000, -0.300000, 0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			mov 	r7, 	c0
			mov 	r9, 	c2
			add 	r9, 	r7.yzxw, 	v2.wyzx
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyw r8, v3(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, -0.859999, 0.500000, -0.400000
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
		
		PixelShaderConstant[0] = {0.200000, 1.359999, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			mov 	r8, 	c0
			mov 	r10, 	c2
			add 	r10.xyw, 	r8, 	v3
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r9, -v4(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.250000, 1.349999, -0.250000
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
		
		PixelShaderConstant[0] = {-0.500000, 0.250000, -0.849999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices4 	v4
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	r9, 	-v4
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat r10, v5(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, -0.839999
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
		
		PixelShaderConstant[0] = {-0.500000, 1.100000, 0.300000, 1.339999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices5 	v5
			mov 	r10, 	c0
			mov 	r12, 	c2
			add_sat 	r12, 	r10, 	v5
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r11, v6(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.329999, -0.250000, 1.200000, 0.350000
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
		
		PixelShaderConstant[0] = {-0.829999, 0.750000, -0.700000, 0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13, 	r11, 	v6
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r12.z, v7(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, -0.819999, 0.500000, -0.800000
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
		
		PixelShaderConstant[0] = {0.400000, 1.319999, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			mov 	r12, 	c0
			mov 	r14, 	c2
			add 	r14, 	r12.z, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r13, v8.zxyw(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.450000, 1.309999, 0.250000
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
		
		PixelShaderConstant[0] = {-0.900000, 0.050000, -0.809999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			mov 	r13, 	c0
			mov 	r0, 	c2
			add 	r0, 	r13, 	v8.zxyw
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r14.wzxy, v9.w(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, -0.799999
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
		
		PixelShaderConstant[0] = {0.500000, 1.500000, 0.500000, 1.299999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			mov 	r14, 	c0
			mov 	r1, 	c2
			add 	r1, 	r14.wzxy, 	v9.w
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r0.wzyx, v0.xy(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.289999, 0.750000, 1.400000, 0.550000
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
		
		PixelShaderConstant[0] = {-0.789999, -0.250000, -0.900000, -0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal0 	v0
			mov 	r0, 	c0
			mov 	r2, 	c2
			add 	r2, 	r0.wzyx, 	v0.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xzw r1, v1(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, -0.779999, -0.500000, -0.800000
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
		
		PixelShaderConstant[0] = {0.600000, 1.279999, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal1 	v1
			mov 	r1, 	c0
			mov 	r3, 	c2
			add 	r3.xzw, 	r1, 	v1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -r2, v2_abs(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.650000, 1.269999, 1.250000
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
		
		PixelShaderConstant[0] = {-0.700000, -0.150000, -0.769999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal2 	v2
			mov 	r2, 	c0
			mov 	r4, 	c2
			add 	r4, 	-r2, 	v2_abs
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp r3, v3(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, -0.759999
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
		
		PixelShaderConstant[0] = {1.500000, 1.100000, 0.700000, 1.259999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal3 	v3
			mov 	r3, 	c0
			mov 	r5, 	c2
			add_pp 	r5, 	r3, 	v3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r4, v4(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.249999, 1.250000, 1.000000, 0.750000
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
		
		PixelShaderConstant[0] = {-0.749999, -0.750000, -0.500000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent4 	v4
			mov 	r4, 	c0
			mov 	r6, 	c2
			add 	r6, 	r4, 	v4
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r5.x, v5(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, -0.739999, -0.500000, -0.400000
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
		
		PixelShaderConstant[0] = {0.800000, 1.239999, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent5 	v5
			mov 	r5, 	c0
			mov 	r7, 	c2
			add 	r7, 	r5.x, 	v5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r6, v6.xyzw(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.850000, 1.229999, 0.750000
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
		
		PixelShaderConstant[0] = {-0.300000, -0.350000, -0.729999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent6 	v6
			mov 	r6, 	c0
			mov 	r8, 	c2
			add 	r8, 	r6, 	v6.xyzw
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r7.xz, v7.y(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, -0.719999
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
		
		PixelShaderConstant[0] = {0.500000, 0.700000, 0.900000, 1.219999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent7 	v7
			mov 	r7, 	c0
			mov 	r9, 	c2
			add 	r9, 	r7.xz, 	v7.y
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r8.yzxw, v8.xw(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.209999, 0.250000, 0.600000, 0.950000
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
		
		PixelShaderConstant[0] = {-0.709999, 0.250000, -0.100000, -0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent8 	v8
			mov 	r8, 	c0
			mov 	r10, 	c2
			add 	r10, 	r8.yzxw, 	v8.xw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yzw r9, v9(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, -0.699999, 0.500000, -0.000000
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
		
		PixelShaderConstant[0] = {1.000000, 1.199999, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent9 	v9
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11.yzw, 	r9, 	v9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -r10, -v0(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, 1.050000, 1.189999, -0.250000
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
		
		PixelShaderConstant[0] = {0.100000, -0.550000, -0.689999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent0 	v0
			mov 	r10, 	c0
			mov 	r12, 	c2
			add 	r12, 	-r10, 	-v0
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat r11, v1(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, -0.679999
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
		
		PixelShaderConstant[0] = {-0.500000, 0.300000, 1.100000, 1.179999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent1 	v1
			mov 	r11, 	c0
			mov 	r13, 	c2
			add_sat 	r13, 	r11, 	v1
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r12, v2(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.169999, -0.250000, 0.200000, 1.150000
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
		
		PixelShaderConstant[0] = {-0.669999, 0.750000, 0.300000, -0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal2 	v2
			mov 	r12, 	c0
			mov 	r14, 	c2
			add 	r14, 	r12, 	v2
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r13.z, v3(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, -0.659999, 0.500000, 0.400000
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
		
		PixelShaderConstant[0] = {1.200000, 1.159999, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal3 	v3
			mov 	r13, 	c0
			mov 	r0, 	c2
			add 	r0, 	r13.z, 	v3
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r14, v4.zxyw(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, 1.250000, 1.149999, 0.250000
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
		
		PixelShaderConstant[0] = {0.500000, -0.750000, -0.649999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			mov 	r14, 	c0
			mov 	r1, 	c2
			add 	r1, 	r14, 	v4.zxyw
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r0.yx, v5.w(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, -0.639999
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
		
		PixelShaderConstant[0] = {0.500000, -0.100000, 1.300000, 1.139999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			mov 	r0, 	c0
			mov 	r2, 	c2
			add 	r2, 	r0.yx, 	v5.w
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r1.wzyx, v6.yz(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.129999, 0.750000, -0.200000, 1.350000
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
		
		PixelShaderConstant[0] = {-0.629999, -0.250000, 0.700000, -0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			mov 	r1, 	c0
			mov 	r3, 	c2
			add 	r3, 	r1.wzyx, 	v6.yz
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyzw r2, v7(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, -0.619999, -0.500000, 0.800000
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
		
		PixelShaderConstant[0] = {1.400000, 1.119999, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			mov 	r2, 	c0
			mov 	r4, 	c2
			add 	r4.xyzw, 	r2, 	v7
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r3_abs, v8_abs(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, 1.450000, 1.109999, 1.250000
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
		
		PixelShaderConstant[0] = {0.900000, -0.950000, -0.609999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal8 	v8
			mov 	r3, 	c0
			mov 	r5, 	c2
			add 	r5, 	r3_abs, 	v8_abs
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp r4, v9(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 1.000000, -1.000000, -0.599999
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
		
		PixelShaderConstant[0] = {1.500000, -0.500000, 1.500000, 1.099999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal9 	v9
			mov 	r4, 	c0
			mov 	r6, 	c2
			add_pp 	r6, 	r4, 	v9
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r5, v0(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.089999, 1.250000, -0.400000, 1.450000
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
		
		PixelShaderConstant[0] = {-0.589999, -0.750000, 0.900000, -0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			mov 	r5, 	c0
			mov 	r7, 	c2
			add 	r7, 	r5, 	v0
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r6.x, v1(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, -0.579999, -0.500000, 0.800000
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
		
		PixelShaderConstant[0] = {1.400000, 1.079999, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			mov 	r6, 	c0
			mov 	r8, 	c2
			add 	r8, 	r6.x, 	v1
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r7, v2.xyzw(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, 1.350000, 1.069999, 0.750000
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
		
		PixelShaderConstant[0] = {0.700000, -0.850000, -0.569999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color2 	v2
			mov 	r7, 	c0
			mov 	r9, 	c2
			add 	r9, 	r7, 	v2.xyzw
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r8.yw, v3.y(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, -0.559999
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
		
		PixelShaderConstant[0] = {0.500000, -0.100000, 1.300000, 1.059999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color3 	v3
			mov 	r8, 	c0
			mov 	r10, 	c2
			add 	r10, 	r8.yw, 	v3.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r9.yzxw, v4.zx(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.049999, 0.250000, -0.000000, 1.250000
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
		
		PixelShaderConstant[0] = {-0.549999, 0.250000, 0.500000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color4 	v4
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	r9.yzxw, 	v4.zx
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.x r10, v5(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, -0.539999, 0.500000, 0.400000
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
		
		PixelShaderConstant[0] = {1.200000, 1.039999, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color5 	v5
			mov 	r10, 	c0
			mov 	r12, 	c2
			add 	r12.x, 	r10, 	v5
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -r11, v6(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, 1.150000, 1.029999, -0.250000
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
		
		PixelShaderConstant[0] = {0.300000, -0.650000, -0.529999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color6 	v6
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13, 	-r11, 	v6
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat r12, v7(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, -0.519999
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
		
		PixelShaderConstant[0] = {-0.500000, 0.300000, 1.100000, 1.019999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color7 	v7
			mov 	r12, 	c0
			mov 	r14, 	c2
			add_sat 	r14, 	r12, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r13, v8(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.009999, -0.250000, 0.400000, 1.050000
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
		
		PixelShaderConstant[0] = {-0.509999, 0.750000, 0.100000, -0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog8 	v8
			mov 	r13, 	c0
			mov 	r0, 	c2
			add 	r0, 	r13, 	v8
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r14.z, v9(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, -0.499999, 0.500000, -0.000000
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
		
		PixelShaderConstant[0] = {1.000000, 0.999999, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog9 	v9
			mov 	r14, 	c0
			mov 	r1, 	c2
			add 	r1, 	r14.z, 	v9
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r0, v0.zxyw(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.950000, 0.989999, 0.250000
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
		
		PixelShaderConstant[0] = {-0.100000, -0.450000, -0.489999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog0 	v0
			mov 	r0, 	c0
			mov 	r2, 	c2
			add 	r2, 	r0, 	v0.zxyw
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r1.zy, v1.w(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, -0.479999
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
		
		PixelShaderConstant[0] = {0.500000, 0.700000, 0.900000, 0.979999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog1 	v1
			mov 	r1, 	c0
			mov 	r3, 	c2
			add 	r3, 	r1.zy, 	v1.w
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r2.wzyx, v2.zw(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.969999, 0.750000, 0.800000, 0.850000
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
		
		PixelShaderConstant[0] = {-0.469999, -0.250000, -0.300000, -0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			mov 	r2, 	c0
			mov 	r4, 	c2
			add 	r4, 	r2.wzyx, 	v2.zw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.y r3, v3(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, -0.459999, -0.500000, -0.400000
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
		
		PixelShaderConstant[0] = {0.800000, 0.959999, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			mov 	r3, 	c0
			mov 	r5, 	c2
			add 	r5.y, 	r3, 	v3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r4, v4_abs(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.750000, 0.949999, 1.250000
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
		
		PixelShaderConstant[0] = {-0.500000, -0.250000, -0.449999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog4 	v4
			mov 	r4, 	c0
			mov 	r6, 	c2
			add 	r6, 	r4, 	v4_abs
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp r5, v5(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, -0.439999
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
		
		PixelShaderConstant[0] = {1.500000, 1.100000, 0.700000, 0.939999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog5 	v5
			mov 	r5, 	c0
			mov 	r7, 	c2
			add_pp 	r7, 	r5, 	v5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r6, v6(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.929999, 1.250000, 1.200000, 0.650000
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
		
		PixelShaderConstant[0] = {-0.429999, -0.750000, -0.700000, -0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			mov 	r6, 	c0
			mov 	r8, 	c2
			add 	r8, 	r6, 	v6
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r7.x, v7(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, -0.419999, -0.500000, -0.800000
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
		
		PixelShaderConstant[0] = {0.600000, 0.919999, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			mov 	r7, 	c0
			mov 	r9, 	c2
			add 	r9, 	r7.x, 	v7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r8, v8.xyzw(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.550000, 0.909999, 0.750000
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
		
		PixelShaderConstant[0] = {-0.900000, -0.050000, -0.409999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			mov 	r8, 	c0
			mov 	r10, 	c2
			add 	r10, 	r8, 	v8.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r9.wx, v9.y(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, -0.399999
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
		
		PixelShaderConstant[0] = {0.500000, 1.500000, 0.500000, 0.899999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	r9.wx, 	v9.y
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r10.yzxw, v0.wy(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.889999, 0.250000, 1.400000, 0.450000
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
		
		PixelShaderConstant[0] = {-0.389999, 0.250000, -0.900000, 0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth0 	v0
			mov 	r10, 	c0
			mov 	r12, 	c2
			add 	r12, 	r10.yzxw, 	v0.wy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.z r11, v1(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, -0.379999, 0.500000, -0.800000
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
		
		PixelShaderConstant[0] = {0.400000, 0.879999, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth1 	v1
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13.z, 	r11, 	v1
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r12_abs, -v2(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.350000, 0.869999, -0.250000
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
		
		PixelShaderConstant[0] = {-0.700000, 0.150000, -0.369999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth2 	v2
			mov 	r12, 	c0
			mov 	r14, 	c2
			add 	r14, 	r12_abs, 	-v2
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat r13, v3(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, -0.359999
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
		
		PixelShaderConstant[0] = {-0.500000, 1.100000, 0.300000, 0.859999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth3 	v3
			mov 	r13, 	c0
			mov 	r0, 	c2
			add_sat 	r0, 	r13, 	v3
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r14, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.849999, -0.250000, 1.000000, 0.250000
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
		
		PixelShaderConstant[0] = {-0.349999, 0.750000, -0.500000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r14, 	c0
			mov 	r1, 	c2
			add 	r1, 	r14, 	vPos.xy
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r0.z, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, -0.339999, 0.500000, -0.400000
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
		
		PixelShaderConstant[0] = {0.200000, 0.839999, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r0, 	c0
			mov 	r2, 	c2
			add 	r2, 	r0.z, 	vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r1, vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.150000, 0.829999, 0.250000
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
		
		PixelShaderConstant[0] = {-0.300000, 0.350000, -0.329999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r1, 	c0
			mov 	r3, 	c2
			add 	r3, 	r1, 	vPos.y
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r2.wz, vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, -0.319999
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
		
		PixelShaderConstant[0] = {0.500000, 0.700000, 0.100000, 0.819999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r2, 	c0
			mov 	r4, 	c2
			add 	r4, 	r2.wz, 	vPos.x
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add r3.wzyx, vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.809999, 0.750000, 0.600000, 0.050000
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
		
		PixelShaderConstant[0] = {-0.309999, -0.250000, -0.100000, 0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r3, 	c0
			mov 	r5, 	c2
			add 	r5, 	r3.wzyx, 	vPos.yx
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.w r4, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, -0.299999, -0.500000, -0.000000
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
		
		PixelShaderConstant[0] = {0.000000, 0.799999, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r4, 	c0
			mov 	r6, 	c2
			add 	r6.w, 	r4, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -r5, -vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, -0.050000, 0.789999, 1.250000
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
		
		PixelShaderConstant[0] = {0.100000, 0.550000, -0.289999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r5, 	c0
			mov 	r7, 	c2
			add 	r7, 	-r5, 	-vPos.xy
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp r6, vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, -0.279999
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
		
		PixelShaderConstant[0] = {1.500000, 0.300000, -0.100000, 0.779999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r6, 	c0
			mov 	r8, 	c2
			add_pp 	r8, 	r6, 	vPos.xy
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(texcoord), c0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.769999, 1.250000, 0.200000, -0.150000
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
		
		PixelShaderConstant[0] = {-0.269999, -0.750000, 0.300000, 0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			mov 	r7, 	c0
			mov 	r9, 	c2
			add 	r9, 	v4, 	c0
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.x(texcoord), c1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, -0.259999, -0.500000, 0.400000
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
		
		PixelShaderConstant[1] = {-0.200000, 0.759999, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			mov 	r7, 	c1
			mov 	r9, 	c3
			add 	r9, 	v5.x, 	c1
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(texcoord), c2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, -0.250000, 0.749999, 0.750000
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
		
		PixelShaderConstant[2] = {0.500000, 0.750000, -0.249999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			mov 	r7, 	c2
			mov 	r9, 	c4
			add 	r9, 	v6, 	c2.xyzw
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v7.xyw(texcoord), c3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, -0.239999
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
		
		PixelShaderConstant[3] = {0.500000, -0.100000, -0.300000, 0.739999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			mov 	r7, 	c3
			mov 	r9, 	c5
			add 	r9, 	v7.xyw, 	c3.y
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8.yzxw(texcoord), c4.xzy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.729999, 0.250000, -0.200000, -0.350000
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
		
		PixelShaderConstant[4] = {-0.229999, 0.250000, 0.700000, 0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord8 	v8
			mov 	r7, 	c4
			mov 	r9, 	c6
			add 	r9, 	v8.yzxw, 	c4.xzy
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xy v9(texcoord), c5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, -0.219999, 0.500000, 0.800000
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
		
		PixelShaderConstant[5] = {-0.400000, 0.719999, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord9 	v9
			mov 	r7, 	c5
			mov 	r9, 	c7
			add 	r9.xy, 	v9, 	c5
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0_abs(texcoord), c6_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, -0.450000, 0.709999, -0.250000
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
		
		PixelShaderConstant[6] = {0.900000, 0.950000, -0.209999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			mov 	r7, 	c6
			mov 	r9, 	c8
			add 	r9, 	v0_abs, 	c6_abs
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v1(texcoord), c7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 1.000000, 1.000000, -0.199999
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
		
		PixelShaderConstant[7] = {-0.500000, -0.500000, -0.500000, 0.699999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord1 	v1
			mov 	r7, 	c7
			mov 	r9, 	c9
			add_sat 	r9, 	v1, 	c7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2(texcoord), r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.689999, -0.250000, -0.400000, -0.450000
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
		
		PixelShaderConstant[8] = {-0.189999, 0.750000, 0.900000, 0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			mov 	r7, 	c8
			mov 	r9, 	c11
			add 	r9, 	v2, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v3.z(texcoord), r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, -0.179999, 0.500000, 0.800000
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
		
		PixelShaderConstant[8] = {-0.400000, 0.679999, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord3 	v3
			mov 	r8, 	c8
			mov 	r10, 	c11
			add 	r10, 	v3.z, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(texcoord), r9.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, -0.350000, 0.669999, 0.250000
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
		
		PixelShaderConstant[8] = {0.700000, 0.850000, -0.169999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			mov 	r9, 	c8
			mov 	r11, 	c11
			add 	r11, 	v4, 	r9.zxyw
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.xzw(texcoord), r10.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, -0.159999
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
		
		PixelShaderConstant[8] = {0.500000, -0.100000, -0.300000, 0.659999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			mov 	r10, 	c8
			mov 	r12, 	c11
			add 	r12, 	v5.xzw, 	r10.w
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6.wzyx(texcoord), r11.xwy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.649999, 0.750000, -0.000000, -0.250000
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
		
		PixelShaderConstant[8] = {-0.149999, -0.250000, 0.500000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			mov 	r11, 	c8
			mov 	r13, 	c11
			add 	r13, 	v6.wzyx, 	r11.xwy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xz v7(texcoord), r12";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, -0.139999, -0.500000, 0.400000
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
		
		PixelShaderConstant[8] = {-0.200000, 0.640000, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			mov 	r12, 	c8
			mov 	r14, 	c11
			add 	r14.xz, 	v7, 	r12
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8_abs(texcoord), r13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, -0.150000, 0.630000, 1.250000
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
		
		PixelShaderConstant[8] = {0.300000, 0.650000, -0.129999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord8 	v8
			mov 	r13, 	c8
			mov 	r0, 	c11
			add 	r0, 	v8_abs, 	r13
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v9(texcoord), r14";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, -0.119999
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
		
		PixelShaderConstant[8] = {1.500000, 0.300000, -0.100000, 0.620000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord9 	v9
			mov 	r14, 	c8
			mov 	r1, 	c11
			add_pp 	r1, 	v9, 	r14
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0(texcoord), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.610000, 1.250000, 0.400000, -0.050000
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
		
		PixelShaderConstant[8] = {-0.109999, -0.750000, 0.100000, 0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			add 	r2, 	v0, 	vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v1.x(texcoord), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, -0.099999, -0.500000, -0.000000
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
		
		PixelShaderConstant[8] = {0.000000, 0.600000, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord1 	v1
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			add 	r2, 	v1.x, 	vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2(texcoord), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.050000, 0.590000, 0.750000
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
		
		PixelShaderConstant[8] = {-0.100000, 0.450000, -0.089999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			add 	r2, 	v2, 	vPos.y
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v3.xwz(texcoord), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, -0.079999
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
		
		PixelShaderConstant[8] = {0.500000, 0.700000, 0.100000, 0.580000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord3 	v3
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			add 	r2, 	v3.xwz, 	vPos.x
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4.yzxw(texcoord), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.570000, 0.250000, 0.800000, 0.150000
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
		
		PixelShaderConstant[8] = {-0.069999, 0.250000, -0.300000, 0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			add 	r2, 	v4.yzxw, 	vPos.yx
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xw v5(texcoord), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, -0.059999, 0.500000, -0.400000
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
		
		PixelShaderConstant[8] = {0.200000, 0.560000, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			add 	r2.xw, 	v5, 	vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(texcoord), -vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.250000, 0.550000, -0.250000
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
		
		PixelShaderConstant[8] = {-0.500000, 0.250000, -0.049999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			add 	r2, 	v6, 	-vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v7(texcoord), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, -0.039999
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
		
		PixelShaderConstant[8] = {-0.500000, 1.100000, 0.300000, 0.540000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			dcl 	vPos.xy
			mov 	r0, 	c8
			mov 	r2, 	c11
			add_sat 	r2, 	v7, 	vPos.xy
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8(blendweight), c8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.530000, -0.250000, 1.200000, 0.350000
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
		
		PixelShaderConstant[8] = {-0.029999, 0.750000, -0.700000, 0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight8 	v8
			mov 	r0, 	c8
			mov 	r2, 	c11
			add 	r2, 	v8, 	c8
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v9.z(blendweight), c9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, -0.019999, 0.500000, -0.800000
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
		
		PixelShaderConstant[9] = {0.400000, 0.520000, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight9 	v9
			mov 	r0, 	c9
			mov 	r2, 	c11
			add 	r2, 	v9.z, 	c9
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0(blendweight), c11.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.450000, 0.510000, 0.250000
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
		
		PixelShaderConstant[11] = {-0.900000, 0.050000, -0.009999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			mov 	r0, 	c11
			mov 	r2, 	c12
			add 	r2, 	v0, 	c11.zxyw
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v1.yxw(blendweight), c11.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, 0.000001
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
		
		PixelShaderConstant[11] = {0.500000, 1.500000, 0.500000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			mov 	r0, 	c11
			mov 	r2, 	c13
			add 	r2, 	v1.yxw, 	c11.w
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2.wzyx(blendweight), c12.yzx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.490000, 0.750000, 1.400000, 0.550000
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
		
		PixelShaderConstant[12] = {0.010001, -0.250000, -0.900000, -0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight2 	v2
			mov 	r0, 	c12
			mov 	r2, 	c14
			add 	r2, 	v2.wzyx, 	c12.yzx
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yz v3(blendweight), c13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, 0.020001, -0.500000, -0.800000
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
		
		PixelShaderConstant[13] = {0.600000, 0.480000, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight3 	v3
			mov 	r0, 	c13
			mov 	r2, 	c15
			add 	r2.yz, 	v3, 	c13
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -v4(blendweight), c14_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.650000, 0.470000, 1.250000
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
		
		PixelShaderConstant[14] = {-0.700000, -0.150000, 0.030001, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight4 	v4
			mov 	r0, 	c14
			mov 	r2, 	c16
			add 	r2, 	-v4, 	c14_abs
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v5(blendweight), c15";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, 0.040001
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
		
		PixelShaderConstant[15] = {1.500000, 1.100000, 0.700000, 0.460000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight5 	v5
			mov 	r0, 	c15
			mov 	r2, 	c17
			add_pp 	r2, 	v5, 	c15
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(blendweight), r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.450000, 1.250000, 1.000000, 0.750000
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
		
		PixelShaderConstant[16] = {0.050001, -0.750000, -0.500000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight6 	v6
			mov 	r0, 	c16
			mov 	r2, 	c18
			add 	r2, 	v6, 	r0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v7.x(blendweight), r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, 0.060001, -0.500000, -0.400000
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
		
		PixelShaderConstant[16] = {0.800000, 0.440000, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight7 	v7
			mov 	r1, 	c16
			mov 	r3, 	c18
			add 	r3, 	v7.x, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8(blendweight), r2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.850000, 0.430000, 0.750000
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
		
		PixelShaderConstant[16] = {-0.300000, -0.350000, 0.070001, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight8 	v8
			mov 	r2, 	c16
			mov 	r4, 	c18
			add 	r4, 	v8, 	r2.xyzw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v9.yzw(blendweight), r3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, 0.080001
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
		
		PixelShaderConstant[16] = {0.500000, 0.700000, 0.900000, 0.420000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight9 	v9
			mov 	r3, 	c16
			mov 	r5, 	c18
			add 	r5, 	v9.yzw, 	r3.y
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0.yzxw(blendweight), r4.ywx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.410000, 0.250000, 0.600000, 0.950000
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
		
		PixelShaderConstant[16] = {0.090001, 0.250000, -0.100000, -0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			mov 	r4, 	c16
			mov 	r6, 	c18
			add 	r6, 	v0.yzxw, 	r4.ywx
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yw v1(blendweight), r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, 0.100001, 0.500000, -0.000000
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
		
		PixelShaderConstant[16] = {1.000000, 0.400000, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			mov 	r5, 	c16
			mov 	r7, 	c18
			add 	r7.yw, 	v1, 	r5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -v2(blendweight), -r6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, 1.050000, 0.390000, -0.250000
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
		
		PixelShaderConstant[16] = {0.100000, -0.550000, 0.110001, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight2 	v2
			mov 	r6, 	c16
			mov 	r8, 	c18
			add 	r8, 	-v2, 	-r6
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v3(blendweight), r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, 0.120001
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
		
		PixelShaderConstant[16] = {-0.500000, 0.300000, 1.100000, 0.380000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight3 	v3
			mov 	r7, 	c16
			mov 	r9, 	c18
			add_sat 	r9, 	v3, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(blendweight), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.370000, -0.250000, 0.200000, 1.150000
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
		
		PixelShaderConstant[16] = {0.130001, 0.750000, 0.300000, -0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight4 	v4
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	v4, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.z(blendweight), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, 0.140001, 0.500000, 0.400000
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
		
		PixelShaderConstant[16] = {1.200000, 0.360000, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight5 	v5
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	v5.z, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(blendweight), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, 1.250000, 0.350000, 0.250000
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
		
		PixelShaderConstant[16] = {0.500000, -0.750000, 0.150001, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight6 	v6
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	v6, 	vPos.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v7.ywz(blendweight), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, 0.160001
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
		
		PixelShaderConstant[16] = {0.500000, -0.100000, 1.300000, 0.340000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight7 	v7
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	v7.ywz, 	vPos.x
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8.wzyx(blendweight), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.330000, 0.750000, -0.200000, 1.350000
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
		
		PixelShaderConstant[16] = {0.170001, -0.250000, 0.700000, -0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight8 	v8
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	v8.wzyx, 	vPos.yx
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.zw v9(blendweight), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, 0.180001, -0.500000, 0.800000
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
		
		PixelShaderConstant[16] = {1.400000, 0.320000, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight9 	v9
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10.zw, 	v9, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0_abs(blendweight), vPos_abs.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, 1.450000, 0.310000, 1.250000
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
		
		PixelShaderConstant[16] = {0.900000, -0.950000, 0.190001, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	v0_abs, 	vPos_abs.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v1(blendweight), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 1.000000, -1.000000, 0.200001
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
		
		PixelShaderConstant[16] = {1.500000, -0.500000, 1.500000, 0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			add_pp 	r10, 	v1, 	vPos.xy
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2(blendindices), c16";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.290000, 1.250000, -0.400000, 1.450000
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
		
		PixelShaderConstant[16] = {0.210001, -0.750000, 0.900000, -0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	v2, 	c16
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v3.x(blendindices), c17";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, 0.220001, -0.500000, 0.800000
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
		
		PixelShaderConstant[17] = {1.400000, 0.280000, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			mov 	r8, 	c17
			mov 	r10, 	c19
			add 	r10, 	v3.x, 	c17
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(blendindices), c18.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, 1.350000, 0.270000, 0.750000
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
		
		PixelShaderConstant[18] = {0.700000, -0.850000, 0.230001, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices4 	v4
			mov 	r8, 	c18
			mov 	r10, 	c20
			add 	r10, 	v4, 	c18.xyzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.zxw(blendindices), c19.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, 0.240001
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
		
		PixelShaderConstant[19] = {0.500000, -0.100000, 1.300000, 0.260000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices5 	v5
			mov 	r8, 	c19
			mov 	r10, 	c21
			add 	r10, 	v5.zxw, 	c19.y
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6.yzxw(blendindices), c20.zyx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.250000, 0.250000, -0.000000, 1.250000
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
		
		PixelShaderConstant[20] = {0.250001, 0.250000, 0.500000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices6 	v6
			mov 	r8, 	c20
			mov 	r10, 	c22
			add 	r10, 	v6.yzxw, 	c20.zyx
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyz v7(blendindices), c21";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, 0.260001, 0.500000, 0.400000
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
		
		PixelShaderConstant[21] = {1.200000, 0.240000, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices7 	v7
			mov 	r8, 	c21
			mov 	r10, 	c23
			add 	r10.xyz, 	v7, 	c21
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -v8(blendindices), c22";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, 1.150000, 0.230000, -0.250000
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
		
		PixelShaderConstant[22] = {0.300000, -0.650000, 0.270001, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices8 	v8
			mov 	r8, 	c22
			mov 	r10, 	c24
			add 	r10, 	-v8, 	c22
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v9(blendindices), c23";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, 0.280001
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
		
		PixelShaderConstant[23] = {-0.500000, 0.300000, 1.100000, 0.220000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices9 	v9
			mov 	r8, 	c23
			mov 	r10, 	c25
			add_sat 	r10, 	v9, 	c23
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0(blendindices), r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.210000, -0.250000, 0.400000, 1.050000
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
		
		PixelShaderConstant[24] = {0.290001, 0.750000, 0.100000, -0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices0 	v0
			mov 	r8, 	c24
			mov 	r10, 	c26
			add 	r10, 	v0, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v1.z(blendindices), r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, 0.300001, 0.500000, -0.000000
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
		
		PixelShaderConstant[24] = {1.000000, 0.200000, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices1 	v1
			mov 	r9, 	c24
			mov 	r11, 	c26
			add 	r11, 	v1.z, 	r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2(blendindices), r10.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.950000, 0.190000, 0.250000
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
		
		PixelShaderConstant[24] = {-0.100000, -0.450000, 0.310001, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			mov 	r10, 	c24
			mov 	r12, 	c26
			add 	r12, 	v2, 	r10.zxyw
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v3.zyw(blendindices), r11.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, 0.320001
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
		
		PixelShaderConstant[24] = {0.500000, 0.700000, 0.900000, 0.180000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			mov 	r11, 	c24
			mov 	r13, 	c26
			add 	r13, 	v3.zyw, 	r11.w
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4.wzyx(blendindices), r12.zwx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.170000, 0.750000, 0.800000, 0.850000
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
		
		PixelShaderConstant[24] = {0.330001, -0.250000, -0.300000, -0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices4 	v4
			mov 	r12, 	c24
			mov 	r14, 	c26
			add 	r14, 	v4.wzyx, 	r12.zwx
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyw v5(blendindices), r13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, 0.340001, -0.500000, -0.400000
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
		
		PixelShaderConstant[24] = {0.800000, 0.160000, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices5 	v5
			mov 	r13, 	c24
			mov 	r0, 	c26
			add 	r0.xyw, 	v5, 	r13
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(blendindices), r14_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.750000, 0.150000, 1.250000
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
		
		PixelShaderConstant[24] = {-0.500000, -0.250000, 0.350001, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices6 	v6
			mov 	r14, 	c24
			mov 	r1, 	c26
			add 	r1, 	v6, 	r14_abs
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v7(blendindices), r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, 0.360001
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
		
		PixelShaderConstant[24] = {1.500000, 1.100000, 0.700000, 0.140000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices7 	v7
			mov 	r0, 	c24
			mov 	r2, 	c26
			add_pp 	r2, 	v7, 	r0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8(blendindices), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.130000, 1.250000, 1.200000, 0.650000
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
		
		PixelShaderConstant[24] = {0.370001, -0.750000, -0.700000, -0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices8 	v8
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			add 	r3, 	v8, 	vPos.xy
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v9.x(blendindices), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, 0.380001, -0.500000, -0.800000
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
		
		PixelShaderConstant[24] = {0.600000, 0.120000, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices9 	v9
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			add 	r3, 	v9.x, 	vPos.xy
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0(blendindices), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.550000, 0.110000, 0.750000
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
		
		PixelShaderConstant[24] = {-0.900000, -0.050000, 0.390001, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices0 	v0
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			add 	r3, 	v0, 	vPos.y
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v1.zwy(blendindices), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, 0.400001
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
		
		PixelShaderConstant[24] = {0.500000, 1.500000, 0.500000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices1 	v1
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			add 	r3, 	v1.zwy, 	vPos.x
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2.yzxw(blendindices), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.090000, 0.250000, 1.400000, 0.450000
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
		
		PixelShaderConstant[24] = {0.410001, 0.250000, -0.900000, 0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			add 	r3, 	v2.yzxw, 	vPos.yx
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xzw v3(blendindices), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, 0.420001, 0.500000, -0.800000
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
		
		PixelShaderConstant[24] = {0.400000, 0.080000, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			add 	r3.xzw, 	v3, 	vPos.xy
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4_abs(blendindices), -vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.350000, 0.070000, -0.250000
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
		
		PixelShaderConstant[24] = {-0.700000, 0.150000, 0.430001, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices4 	v4
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			add 	r3, 	v4_abs, 	-vPos.xy
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v5(blendindices), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, 0.440001
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
		
		PixelShaderConstant[24] = {-0.500000, 1.100000, 0.300000, 0.060000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices5 	v5
			dcl 	vPos.xy
			mov 	r1, 	c24
			mov 	r3, 	c26
			add_sat 	r3, 	v5, 	vPos.xy
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(normal), c24";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.050000, -0.250000, 1.000000, 0.250000
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
		
		PixelShaderConstant[24] = {0.450000, 0.750000, -0.500000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			mov 	r1, 	c24
			mov 	r3, 	c26
			add 	r3, 	v6, 	c24
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v7.z(normal), c25";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, 0.460000, 0.500000, -0.400000
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
		
		PixelShaderConstant[25] = {0.200000, 0.040000, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			mov 	r1, 	c25
			mov 	r3, 	c27
			add 	r3, 	v7.z, 	c25
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8(normal), c26.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.150000, 0.030000, 0.250000
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
		
		PixelShaderConstant[26] = {-0.300000, 0.350000, 0.470000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			mov 	r1, 	c26
			mov 	r3, 	c28
			add 	r3, 	v8, 	c26.zxyw
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v9.wxz(normal), c27.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, 0.480000
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
		
		PixelShaderConstant[27] = {0.500000, 0.700000, 0.100000, 0.020000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			mov 	r1, 	c27
			mov 	r3, 	c29
			add 	r3, 	v9.wxz, 	c27.w
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0.wzyx(normal), c28.wyx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.010000, 0.750000, 0.600000, 0.050000
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
		
		PixelShaderConstant[28] = {0.490000, -0.250000, -0.100000, 0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal0 	v0
			mov 	r1, 	c28
			mov 	r3, 	c30
			add 	r3, 	v0.wzyx, 	c28.wyx
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yzw v1(normal), c29";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, 0.500000, -0.500000, -0.000000
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
		
		PixelShaderConstant[29] = {0.000000, -0.000000, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal1 	v1
			mov 	r1, 	c29
			mov 	r3, 	c31
			add 	r3.yzw, 	v1, 	c29
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -v2(normal), -c30";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, -0.050000, -0.010000, 1.250000
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
		
		PixelShaderConstant[30] = {0.100000, 0.550000, 0.510000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal2 	v2
			mov 	r1, 	c30
			mov 	r3, 	c0
			add 	r3, 	-v2, 	-c30
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v3(normal), c31";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, 0.520000
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
		
		PixelShaderConstant[31] = {1.500000, 0.300000, -0.100000, -0.020000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal3 	v3
			mov 	r1, 	c31
			mov 	r3, 	c1
			add_pp 	r3, 	v3, 	c31
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(normal), r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.030000, 1.250000, 0.200000, -0.150000
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
		
		PixelShaderConstant[0] = {0.530000, -0.750000, 0.300000, 0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal4 	v4
			mov 	r1, 	c0
			mov 	r3, 	c2
			add 	r3, 	v4, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.x(normal), r2";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, 0.540000, -0.500000, 0.400000
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
		
		PixelShaderConstant[0] = {-0.200000, -0.040000, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal5 	v5
			mov 	r2, 	c0
			mov 	r4, 	c2
			add 	r4, 	v5.x, 	r2
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(normal), r3.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, -0.250000, -0.050000, 0.750000
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
		
		PixelShaderConstant[0] = {0.500000, 0.750000, 0.550000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			mov 	r3, 	c0
			mov 	r5, 	c2
			add 	r5, 	v6, 	r3.xyzw
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v7.wyz(normal), r4.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, 0.560000
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
		
		PixelShaderConstant[0] = {0.500000, -0.100000, -0.300000, -0.060000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			mov 	r4, 	c0
			mov 	r6, 	c2
			add 	r6, 	v7.wyz, 	r4.y
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8.yzxw(normal), r5.wzx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.070000, 0.250000, -0.200000, -0.350000
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
		
		PixelShaderConstant[0] = {0.570000, 0.250000, 0.700000, 0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			mov 	r5, 	c0
			mov 	r7, 	c2
			add 	r7, 	v8.yzxw, 	r5.wzx
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyzw v9(normal), r6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, 0.580000, 0.500000, 0.800000
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
		
		PixelShaderConstant[0] = {-0.400000, -0.080000, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			mov 	r6, 	c0
			mov 	r8, 	c2
			add 	r8.xyzw, 	v9, 	r6
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0_abs(normal), r7_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, -0.450000, -0.090000, -0.250000
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
		
		PixelShaderConstant[0] = {0.900000, 0.950000, 0.590000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal0 	v0
			mov 	r7, 	c0
			mov 	r9, 	c2
			add 	r9, 	v0_abs, 	r7_abs
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v1(normal), r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 1.000000, 1.000000, 0.600000
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
		
		PixelShaderConstant[0] = {-0.500000, -0.500000, -0.500000, -0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal1 	v1
			mov 	r8, 	c0
			mov 	r10, 	c2
			add_sat 	r10, 	v1, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2(normal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.110000, -0.250000, -0.400000, -0.450000
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
		
		PixelShaderConstant[0] = {0.610000, 0.750000, 0.900000, 0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal2 	v2
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	v2, 	vPos.xy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v3.z(normal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, 0.620000, 0.500000, 0.800000
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
		
		PixelShaderConstant[0] = {-0.400000, -0.120000, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal3 	v3
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	v3.z, 	vPos.xy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(normal), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, -0.350000, -0.130000, 0.250000
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
		
		PixelShaderConstant[0] = {0.700000, 0.850000, 0.630000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal4 	v4
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	v4, 	vPos.y
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.wzy(normal), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, 0.640000
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
		
		PixelShaderConstant[0] = {0.500000, -0.100000, -0.300000, -0.140000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal5 	v5
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	v5.wzy, 	vPos.x
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6.wzyx(normal), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.150000, 0.750000, -0.000000, -0.250000
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
		
		PixelShaderConstant[0] = {0.650000, -0.250000, 0.500000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	v6.wzyx, 	vPos.yx
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.x v7(normal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, 0.660000, -0.500000, 0.400000
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
		
		PixelShaderConstant[0] = {-0.200000, -0.160000, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11.x, 	v7, 	vPos.xy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8_abs(normal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, -0.150000, -0.170000, 1.250000
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
		
		PixelShaderConstant[0] = {0.300000, 0.650000, 0.670000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	v8_abs, 	vPos.xy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v9(normal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, 0.680000
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
		
		PixelShaderConstant[0] = {1.500000, 0.300000, -0.100000, -0.180000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			dcl 	vPos.xy
			mov 	r9, 	c0
			mov 	r11, 	c2
			add_pp 	r11, 	v9, 	vPos.xy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0(tangent), c0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.190000, 1.250000, 0.400000, -0.050000
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
		
		PixelShaderConstant[0] = {0.690000, -0.750000, 0.100000, 0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent0 	v0
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	v0, 	c0
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v1.x(tangent), c1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, 0.700000, -0.500000, -0.000000
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
		
		PixelShaderConstant[1] = {0.000000, -0.200000, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent1 	v1
			mov 	r9, 	c1
			mov 	r11, 	c3
			add 	r11, 	v1.x, 	c1
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2(tangent), c2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.050000, -0.210000, 0.750000
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
		
		PixelShaderConstant[2] = {-0.100000, 0.450000, 0.710000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent2 	v2
			mov 	r9, 	c2
			mov 	r11, 	c4
			add 	r11, 	v2, 	c2.xyzw
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v3.xzyw(tangent), c3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, 0.720000
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
		
		PixelShaderConstant[3] = {0.500000, 0.700000, 0.100000, -0.220000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent3 	v3
			mov 	r9, 	c3
			mov 	r11, 	c5
			add 	r11, 	v3.xzyw, 	c3.y
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4.yzxw(tangent), c4.xzwy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.230000, 0.250000, 0.800000, 0.150000
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
		
		PixelShaderConstant[4] = {0.730000, 0.250000, -0.300000, 0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent4 	v4
			mov 	r9, 	c4
			mov 	r11, 	c6
			add 	r11, 	v4.yzxw, 	c4.xzwy
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.y v5(tangent), c5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, 0.740000, 0.500000, -0.400000
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
		
		PixelShaderConstant[5] = {0.200000, -0.240000, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent5 	v5
			mov 	r9, 	c5
			mov 	r11, 	c7
			add 	r11.y, 	v5, 	c5
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(tangent), -c6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.250000, -0.250000, -0.250000
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
		
		PixelShaderConstant[6] = {-0.500000, 0.250000, 0.750000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent6 	v6
			mov 	r9, 	c6
			mov 	r11, 	c8
			add 	r11, 	v6, 	-c6
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v7(tangent), c7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, 0.760000
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
		
		PixelShaderConstant[7] = {-0.500000, 1.100000, 0.300000, -0.260000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent7 	v7
			mov 	r9, 	c7
			mov 	r11, 	c9
			add_sat 	r11, 	v7, 	c7
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8(tangent), r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.270000, -0.250000, 1.200000, 0.350000
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
		
		PixelShaderConstant[8] = {0.770000, 0.750000, -0.700000, 0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent8 	v8
			mov 	r9, 	c8
			mov 	r11, 	c11
			add 	r11, 	v8, 	r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v9.z(tangent), r10";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, 0.780000, 0.500000, -0.800000
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
		
		PixelShaderConstant[8] = {0.400000, -0.280000, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent9 	v9
			mov 	r10, 	c8
			mov 	r12, 	c11
			add 	r12, 	v9.z, 	r10
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0(tangent), r11.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.450000, -0.290000, 0.250000
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
		
		PixelShaderConstant[8] = {-0.900000, 0.050000, 0.790000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent0 	v0
			mov 	r11, 	c8
			mov 	r13, 	c11
			add 	r13, 	v0, 	r11.zxyw
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v1.xwyz(tangent), r12.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, 0.800000
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
		
		PixelShaderConstant[8] = {0.500000, 1.500000, 0.500000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent1 	v1
			mov 	r12, 	c8
			mov 	r14, 	c11
			add 	r14, 	v1.xwyz, 	r12.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2.wzyx(tangent), r13.xwzy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.310000, 0.750000, 1.400000, 0.550000
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
		
		PixelShaderConstant[8] = {0.810000, -0.250000, -0.900000, -0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent2 	v2
			mov 	r13, 	c8
			mov 	r0, 	c11
			add 	r0, 	v2.wzyx, 	r13.xwzy
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.z v3(tangent), r14";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, 0.820000, -0.500000, -0.800000
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
		
		PixelShaderConstant[8] = {0.600000, -0.320000, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent3 	v3
			mov 	r14, 	c8
			mov 	r1, 	c11
			add 	r1.z, 	v3, 	r14
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -v4(tangent), r0_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.650000, -0.330000, 1.250000
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
		
		PixelShaderConstant[8] = {-0.700000, -0.150000, 0.830000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent4 	v4
			mov 	r0, 	c8
			mov 	r2, 	c11
			add 	r2, 	-v4, 	r0_abs
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v5(tangent), r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, 0.840000
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
		
		PixelShaderConstant[8] = {1.500000, 1.100000, 0.700000, -0.340000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent5 	v5
			mov 	r1, 	c8
			mov 	r3, 	c11
			add_pp 	r3, 	v5, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(tangent), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.350000, 1.250000, 1.000000, 0.750000
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
		
		PixelShaderConstant[8] = {0.850000, -0.750000, -0.500000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent6 	v6
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			add 	r4, 	v6, 	vPos.xy
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v7.x(tangent), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, 0.860000, -0.500000, -0.400000
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
		
		PixelShaderConstant[8] = {0.800000, -0.360000, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent7 	v7
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			add 	r4, 	v7.x, 	vPos.xy
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8(tangent), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.850000, -0.370000, 0.750000
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
		
		PixelShaderConstant[8] = {-0.300000, -0.350000, 0.870000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent8 	v8
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			add 	r4, 	v8, 	vPos.y
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v9.yxzw(tangent), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, 0.880000
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
		
		PixelShaderConstant[8] = {0.500000, 0.700000, 0.900000, -0.380000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent9 	v9
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			add 	r4, 	v9.yxzw, 	vPos.x
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0.yzxw(tangent), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.390000, 0.250000, 0.600000, 0.950000
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
		
		PixelShaderConstant[8] = {0.890000, 0.250000, -0.100000, -0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent0 	v0
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			add 	r4, 	v0.yzxw, 	vPos.yx
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.w v1(tangent), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, 0.900000, 0.500000, -0.000000
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
		
		PixelShaderConstant[8] = {1.000000, -0.400000, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent1 	v1
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			add 	r4.w, 	v1, 	vPos.xy
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -v2(tangent), -vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, 1.050000, -0.410000, -0.250000
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
		
		PixelShaderConstant[8] = {0.100000, -0.550000, 0.910000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent2 	v2
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			add 	r4, 	-v2, 	-vPos.xy
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v3(tangent), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, 0.920000
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
		
		PixelShaderConstant[8] = {-0.500000, 0.300000, 1.100000, -0.420000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent3 	v3
			dcl 	vPos.xy
			mov 	r2, 	c8
			mov 	r4, 	c11
			add_sat 	r4, 	v3, 	vPos.xy
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(binormal), c8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.430000, -0.250000, 0.200000, 1.150000
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
		
		PixelShaderConstant[8] = {0.930000, 0.750000, 0.300000, -0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			mov 	r2, 	c8
			mov 	r4, 	c11
			add 	r4, 	v4, 	c8
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.z(binormal), c9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, 0.940000, 0.500000, 0.400000
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
		
		PixelShaderConstant[9] = {1.200000, -0.440000, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			mov 	r2, 	c9
			mov 	r4, 	c11
			add 	r4, 	v5.z, 	c9
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(binormal), c11.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, 1.250000, -0.450000, 0.250000
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
		
		PixelShaderConstant[11] = {0.500000, -0.750000, 0.950000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			mov 	r2, 	c11
			mov 	r4, 	c12
			add 	r4, 	v6, 	c11.zxyw
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v7.yzwx(binormal), c11.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, 0.960000
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
		
		PixelShaderConstant[11] = {0.500000, -0.100000, 1.300000, -0.460000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			mov 	r2, 	c11
			mov 	r4, 	c13
			add 	r4, 	v7.yzwx, 	c11.w
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8.wzyx(binormal), c12.ywxz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.470000, 0.750000, -0.200000, 1.350000
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
		
		PixelShaderConstant[12] = {0.970000, -0.250000, 0.700000, -0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal8 	v8
			mov 	r2, 	c12
			mov 	r4, 	c14
			add 	r4, 	v8.wzyx, 	c12.ywxz
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xy v9(binormal), c13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, 0.980000, -0.500000, 0.800000
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
		
		PixelShaderConstant[13] = {1.400000, -0.480000, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal9 	v9
			mov 	r2, 	c13
			mov 	r4, 	c15
			add 	r4.xy, 	v9, 	c13
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0_abs(binormal), c14_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, 1.450000, -0.490000, 1.250000
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
		
		PixelShaderConstant[14] = {0.900000, -0.950000, 0.990000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal0 	v0
			mov 	r2, 	c14
			mov 	r4, 	c16
			add 	r4, 	v0_abs, 	c14_abs
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v1(binormal), c15";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 1.000000, -1.000000, 1.000000
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
		
		PixelShaderConstant[15] = {1.500000, -0.500000, 1.500000, -0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal1 	v1
			mov 	r2, 	c15
			mov 	r4, 	c17
			add_pp 	r4, 	v1, 	c15
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2(binormal), r2";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.490000, 1.250000, -0.400000, 1.450000
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
		
		PixelShaderConstant[16] = {0.990000, -0.750000, 0.900000, -0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal2 	v2
			mov 	r2, 	c16
			mov 	r4, 	c18
			add 	r4, 	v2, 	r2
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v3.x(binormal), r3";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, 0.980000, -0.500000, 0.800000
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
		
		PixelShaderConstant[16] = {1.400000, -0.480000, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal3 	v3
			mov 	r3, 	c16
			mov 	r5, 	c18
			add 	r5, 	v3.x, 	r3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(binormal), r4.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, 1.350000, -0.470000, 0.750000
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
		
		PixelShaderConstant[16] = {0.700000, -0.850000, 0.970000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			mov 	r4, 	c16
			mov 	r6, 	c18
			add 	r6, 	v4, 	r4.xyzw
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.ywzx(binormal), r5.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, 0.960000
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
		
		PixelShaderConstant[16] = {0.500000, -0.100000, 1.300000, -0.460000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			mov 	r5, 	c16
			mov 	r7, 	c18
			add 	r7, 	v5.ywzx, 	r5.y
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6.yzxw(binormal), r6.zxwy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.450000, 0.250000, -0.000000, 1.250000
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
		
		PixelShaderConstant[16] = {0.950000, 0.250000, 0.500000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			mov 	r6, 	c16
			mov 	r8, 	c18
			add 	r8, 	v6.yzxw, 	r6.zxwy
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xz v7(binormal), r7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, 0.940000, 0.500000, 0.400000
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
		
		PixelShaderConstant[16] = {1.200000, -0.440000, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			mov 	r7, 	c16
			mov 	r9, 	c18
			add 	r9.xz, 	v7, 	r7
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -v8(binormal), r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, 1.150000, -0.430000, -0.250000
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
		
		PixelShaderConstant[16] = {0.300000, -0.650000, 0.930000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal8 	v8
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	-v8, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v9(binormal), r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, 0.920000
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
		
		PixelShaderConstant[16] = {-0.500000, 0.300000, 1.100000, -0.420000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal9 	v9
			mov 	r9, 	c16
			mov 	r11, 	c18
			add_sat 	r11, 	v9, 	r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0(binormal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.410000, -0.250000, 0.400000, 1.050000
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
		
		PixelShaderConstant[16] = {0.910000, 0.750000, 0.100000, -0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal0 	v0
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			add 	r12, 	v0, 	vPos.xy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v1.z(binormal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, 0.900000, 0.500000, -0.000000
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
		
		PixelShaderConstant[16] = {1.000000, -0.400000, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal1 	v1
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			add 	r12, 	v1.z, 	vPos.xy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2(binormal), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.950000, -0.390000, 0.250000
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
		
		PixelShaderConstant[16] = {-0.100000, -0.450000, 0.890000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal2 	v2
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			add 	r12, 	v2, 	vPos.y
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v3.zyxw(binormal), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, 0.880000
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
		
		PixelShaderConstant[16] = {0.500000, 0.700000, 0.900000, -0.380000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal3 	v3
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			add 	r12, 	v3.zyxw, 	vPos.x
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4.wzyx(binormal), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.370000, 0.750000, 0.800000, 0.850000
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
		
		PixelShaderConstant[16] = {0.870000, -0.250000, -0.300000, -0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			add 	r12, 	v4.wzyx, 	vPos.yx
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xw v5(binormal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, 0.860000, -0.500000, -0.400000
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
		
		PixelShaderConstant[16] = {0.800000, -0.360000, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			add 	r12.xw, 	v5, 	vPos.xy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(binormal), vPos_abs.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.750000, -0.350000, 1.250000
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
		
		PixelShaderConstant[16] = {-0.500000, -0.250000, 0.850000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			add 	r12, 	v6, 	vPos_abs.xy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v7(binormal), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, 0.840000
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
		
		PixelShaderConstant[16] = {1.500000, 1.100000, 0.700000, -0.340000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			add_pp 	r12, 	v7, 	vPos.xy
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8(color), c16";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.330000, 1.250000, 1.200000, 0.650000
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
		
		PixelShaderConstant[16] = {0.830000, -0.750000, -0.700000, -0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color8 	v8
			mov 	r10, 	c16
			mov 	r12, 	c18
			add 	r12, 	v8, 	c16
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v9.x(color), c17";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, 0.820000, -0.500000, -0.800000
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
		
		PixelShaderConstant[17] = {0.600000, -0.320000, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c19, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color9 	v9
			mov 	r10, 	c17
			mov 	r12, 	c19
			add 	r12, 	v9.x, 	c17
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0(color), c18.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.550000, -0.310000, 0.750000
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
		
		PixelShaderConstant[18] = {-0.900000, -0.050000, 0.810000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c20, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			mov 	r10, 	c18
			mov 	r12, 	c20
			add 	r12, 	v0, 	c18.xyzw
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v1.zwyx(color), c19.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, 0.800000
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
		
		PixelShaderConstant[19] = {0.500000, 1.500000, 0.500000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c21, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			mov 	r10, 	c19
			mov 	r12, 	c21
			add 	r12, 	v1.zwyx, 	c19.y
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2.yzxw(color), c20.wxyz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.290000, 0.250000, 1.400000, 0.450000
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
		
		PixelShaderConstant[20] = {0.790000, 0.250000, -0.900000, 0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c22, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color2 	v2
			mov 	r10, 	c20
			mov 	r12, 	c22
			add 	r12, 	v2.yzxw, 	c20.wxyz
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yz v3(color), c21";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, 0.780000, 0.500000, -0.800000
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
		
		PixelShaderConstant[21] = {0.400000, -0.280000, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c23, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color3 	v3
			mov 	r10, 	c21
			mov 	r12, 	c23
			add 	r12.yz, 	v3, 	c21
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4_abs(color), -c22";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.350000, -0.270000, -0.250000
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
		
		PixelShaderConstant[22] = {-0.700000, 0.150000, 0.770000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c24, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color4 	v4
			mov 	r10, 	c22
			mov 	r12, 	c24
			add 	r12, 	v4_abs, 	-c22
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v5(color), c23";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, 0.760000
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
		
		PixelShaderConstant[23] = {-0.500000, 1.100000, 0.300000, -0.260000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c25, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color5 	v5
			mov 	r10, 	c23
			mov 	r12, 	c25
			add_sat 	r12, 	v5, 	c23
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(color), r10";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.250000, -0.250000, 1.000000, 0.250000
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
		
		PixelShaderConstant[24] = {0.750000, 0.750000, -0.500000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color6 	v6
			mov 	r10, 	c24
			mov 	r12, 	c26
			add 	r12, 	v6, 	r10
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v7.z(color), r11";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, 0.740000, 0.500000, -0.400000
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
		
		PixelShaderConstant[24] = {0.200000, -0.240000, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color7 	v7
			mov 	r11, 	c24
			mov 	r13, 	c26
			add 	r13, 	v7.z, 	r11
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8(color), r12.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.150000, -0.230000, 0.250000
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
		
		PixelShaderConstant[24] = {-0.300000, 0.350000, 0.730000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color8 	v8
			mov 	r12, 	c24
			mov 	r14, 	c26
			add 	r14, 	v8, 	r12.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v9.wxzy(color), r13.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, 0.720000
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
		
		PixelShaderConstant[24] = {0.500000, 0.700000, 0.100000, -0.220000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color9 	v9
			mov 	r13, 	c24
			mov 	r0, 	c26
			add 	r0, 	v9.wxzy, 	r13.w
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0.wzyx(color), r14.wyxz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.210000, 0.750000, 0.600000, 0.050000
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
		
		PixelShaderConstant[24] = {0.710000, -0.250000, -0.100000, 0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			mov 	r14, 	c24
			mov 	r1, 	c26
			add 	r1, 	v0.wzyx, 	r14.wyxz
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yw v1(color), r0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, 0.700000, -0.500000, -0.000000
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
		
		PixelShaderConstant[24] = {0.000000, -0.200000, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			mov 	r0, 	c24
			mov 	r2, 	c26
			add 	r2.yw, 	v1, 	r0
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -v2(color), -r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, -0.050000, -0.190000, 1.250000
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
		
		PixelShaderConstant[24] = {0.100000, 0.550000, 0.690000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color2 	v2
			mov 	r1, 	c24
			mov 	r3, 	c26
			add 	r3, 	-v2, 	-r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v3(color), r2";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, 0.680000
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
		
		PixelShaderConstant[24] = {1.500000, 0.300000, -0.100000, -0.180000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color3 	v3
			mov 	r2, 	c24
			mov 	r4, 	c26
			add_pp 	r4, 	v3, 	r2
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(color), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.170000, 1.250000, 0.200000, -0.150000
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
		
		PixelShaderConstant[24] = {0.670000, -0.750000, 0.300000, 0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color4 	v4
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			add 	r5, 	v4, 	vPos.xy
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.x(color), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, 0.660000, -0.500000, 0.400000
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
		
		PixelShaderConstant[24] = {-0.200000, -0.160000, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color5 	v5
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			add 	r5, 	v5.x, 	vPos.xy
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(color), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, -0.250000, -0.150000, 0.750000
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
		
		PixelShaderConstant[24] = {0.500000, 0.750000, 0.650000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color6 	v6
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			add 	r5, 	v6, 	vPos.y
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v7.wyzx(color), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, 0.640000
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
		
		PixelShaderConstant[24] = {0.500000, -0.100000, -0.300000, -0.140000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color7 	v7
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			add 	r5, 	v7.wyzx, 	vPos.x
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8.yzxw(color), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.130000, 0.250000, -0.200000, -0.350000
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
		
		PixelShaderConstant[24] = {0.630000, 0.250000, 0.700000, 0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color8 	v8
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			add 	r5, 	v8.yzxw, 	vPos.yx
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.zw v9(color), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, 0.620000, 0.500000, 0.800000
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
		
		PixelShaderConstant[24] = {-0.400000, -0.120000, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color9 	v9
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			add 	r5.zw, 	v9, 	vPos.xy
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0_abs(color), vPos_abs.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, -0.450000, -0.110000, -0.250000
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
		
		PixelShaderConstant[24] = {0.900000, 0.950000, 0.610000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			add 	r5, 	v0_abs, 	vPos_abs.xy
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v1(color), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 1.000000, 1.000000, 0.600000
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
		
		PixelShaderConstant[24] = {-0.500000, -0.500000, -0.500000, -0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			dcl 	vPos.xy
			mov 	r3, 	c24
			mov 	r5, 	c26
			add_sat 	r5, 	v1, 	vPos.xy
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2(fog), c24";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.090000, -0.250000, -0.400000, -0.450000
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
		
		PixelShaderConstant[24] = {0.590000, 0.750000, 0.900000, 0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c26, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			mov 	r3, 	c24
			mov 	r5, 	c26
			add 	r5, 	v2, 	c24
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v3.z(fog), c25";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, 0.580000, 0.500000, 0.800000
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
		
		PixelShaderConstant[25] = {-0.400000, -0.080000, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c27, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			mov 	r3, 	c25
			mov 	r5, 	c27
			add 	r5, 	v3.z, 	c25
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(fog), c26.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, -0.350000, -0.070000, 0.250000
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
		
		PixelShaderConstant[26] = {0.700000, 0.850000, 0.570000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c28, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog4 	v4
			mov 	r3, 	c26
			mov 	r5, 	c28
			add 	r5, 	v4, 	c26.zxyw
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.xy(fog), c27.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, 0.560000
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
		
		PixelShaderConstant[27] = {0.500000, -0.100000, -0.300000, -0.060000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c29, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog5 	v5
			mov 	r3, 	c27
			mov 	r5, 	c29
			add 	r5, 	v5.xy, 	c27.w
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6.wzyx(fog), c28.xz";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.050000, 0.750000, -0.000000, -0.250000
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
		
		PixelShaderConstant[28] = {0.550000, -0.250000, 0.500000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c30, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog6 	v6
			mov 	r3, 	c28
			mov 	r5, 	c30
			add 	r5, 	v6.wzyx, 	c28.xz
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyz v7(fog), c29";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, 0.540000, -0.500000, 0.400000
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
		
		PixelShaderConstant[29] = {-0.200000, -0.040000, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c31, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog7 	v7
			mov 	r3, 	c29
			mov 	r5, 	c31
			add 	r5.xyz, 	v7, 	c29
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8_abs(fog), c30";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, -0.150000, -0.030000, 1.250000
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
		
		PixelShaderConstant[30] = {0.300000, 0.650000, 0.530000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c0, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog8 	v8
			mov 	r3, 	c30
			mov 	r5, 	c0
			add 	r5, 	v8_abs, 	c30
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v9(fog), c31";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, 0.520000
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
		
		PixelShaderConstant[31] = {1.500000, 0.300000, -0.100000, -0.020000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c1, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog9 	v9
			mov 	r3, 	c31
			mov 	r5, 	c1
			add_pp 	r5, 	v9, 	c31
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0(fog), r3";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.010000, 1.250000, 0.400000, -0.050000
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
		
		PixelShaderConstant[0] = {0.510000, -0.750000, 0.100000, 0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog0 	v0
			mov 	r3, 	c0
			mov 	r5, 	c2
			add 	r5, 	v0, 	r3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v1.x(fog), r4";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, 0.500000, -0.500000, -0.000000
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
		
		PixelShaderConstant[0] = {0.000000, -0.000000, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog1 	v1
			mov 	r4, 	c0
			mov 	r6, 	c2
			add 	r6, 	v1.x, 	r4
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2(fog), r5.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.050000, 0.010000, 0.750000
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
		
		PixelShaderConstant[0] = {-0.100000, 0.450000, 0.490000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			mov 	r5, 	c0
			mov 	r7, 	c2
			add 	r7, 	v2, 	r5.xyzw
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v3.xw(fog), r6.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, 0.480000
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
		
		PixelShaderConstant[0] = {0.500000, 0.700000, 0.100000, 0.020000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			mov 	r6, 	c0
			mov 	r8, 	c2
			add 	r8, 	v3.xw, 	r6.y
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4.yzxw(fog), r7.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.030000, 0.250000, 0.800000, 0.150000
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
		
		PixelShaderConstant[0] = {0.470001, 0.250000, -0.300000, 0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog4 	v4
			mov 	r7, 	c0
			mov 	r9, 	c2
			add 	r9, 	v4.yzxw, 	r7.yx
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyw v5(fog), r8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, 0.460001, 0.500000, -0.400000
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
		
		PixelShaderConstant[0] = {0.200000, 0.040000, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog5 	v5
			mov 	r8, 	c0
			mov 	r10, 	c2
			add 	r10.xyw, 	v5, 	r8
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(fog), -r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.250000, 0.050000, -0.250000
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
		
		PixelShaderConstant[0] = {-0.500000, 0.250000, 0.450001, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog6 	v6
			mov 	r9, 	c0
			mov 	r11, 	c2
			add 	r11, 	v6, 	-r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v7(fog), r10";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, 0.440001
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
		
		PixelShaderConstant[0] = {-0.500000, 1.100000, 0.300000, 0.060000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog7 	v7
			mov 	r10, 	c0
			mov 	r12, 	c2
			add_sat 	r12, 	v7, 	r10
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8(fog), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.070000, -0.250000, 1.200000, 0.350000
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
		
		PixelShaderConstant[0] = {0.430001, 0.750000, -0.700000, 0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog8 	v8
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13, 	v8, 	vPos.xy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v9.z(fog), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, 0.420001, 0.500000, -0.800000
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
		
		PixelShaderConstant[0] = {0.400000, 0.080000, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog9 	v9
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13, 	v9.z, 	vPos.xy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0(fog), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.450000, 0.090000, 0.250000
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
		
		PixelShaderConstant[0] = {-0.900000, 0.050000, 0.410001, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog0 	v0
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13, 	v0, 	vPos.y
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v1.yz(fog), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, 0.400001
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
		
		PixelShaderConstant[0] = {0.500000, 1.500000, 0.500000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog1 	v1
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13, 	v1.yz, 	vPos.x
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2.wzyx(fog), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.110000, 0.750000, 1.400000, 0.550000
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
		
		PixelShaderConstant[0] = {0.390001, -0.250000, -0.900000, -0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13, 	v2.wzyx, 	vPos.yx
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xzw v3(fog), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, 0.380001, -0.500000, -0.800000
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
		
		PixelShaderConstant[0] = {0.600000, 0.120000, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13.xzw, 	v3, 	vPos.xy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -v4(fog), vPos_abs.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.650000, 0.130000, 1.250000
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
		
		PixelShaderConstant[0] = {-0.700000, -0.150000, 0.370001, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog4 	v4
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13, 	-v4, 	vPos_abs.xy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v5(fog), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, 0.360001
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
		
		PixelShaderConstant[0] = {1.500000, 1.100000, 0.700000, 0.140000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog5 	v5
			dcl 	vPos.xy
			mov 	r11, 	c0
			mov 	r13, 	c2
			add_pp 	r13, 	v5, 	vPos.xy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(depth), c0";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.150000, 1.250000, 1.000000, 0.750000
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
		
		PixelShaderConstant[0] = {0.350001, -0.750000, -0.500000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c2, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			mov 	r11, 	c0
			mov 	r13, 	c2
			add 	r13, 	v6, 	c0
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v7.x(depth), c1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, 0.340001, -0.500000, -0.400000
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
		
		PixelShaderConstant[1] = {0.800000, 0.160000, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c3, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			mov 	r11, 	c1
			mov 	r13, 	c3
			add 	r13, 	v7.x, 	c1
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8(depth), c2.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.850000, 0.170000, 0.750000
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
		
		PixelShaderConstant[2] = {-0.300000, -0.350000, 0.330001, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c4, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			mov 	r11, 	c2
			mov 	r13, 	c4
			add 	r13, 	v8, 	c2.xyzw
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v9.zx(depth), c3.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, 0.320001
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
		
		PixelShaderConstant[3] = {0.500000, 0.700000, 0.900000, 0.180000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c5, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			mov 	r11, 	c3
			mov 	r13, 	c5
			add 	r13, 	v9.zx, 	c3.y
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0.yzxw(depth), c4.zy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.190000, 0.250000, 0.600000, 0.950000
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
		
		PixelShaderConstant[4] = {0.310001, 0.250000, -0.100000, -0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c6, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth0 	v0
			mov 	r11, 	c4
			mov 	r13, 	c6
			add 	r13, 	v0.yzxw, 	c4.zy
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yzw v1(depth), c5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, 0.300001, 0.500000, -0.000000
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
		
		PixelShaderConstant[5] = {1.000000, 0.200000, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c7, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth1 	v1
			mov 	r11, 	c5
			mov 	r13, 	c7
			add 	r13.yzw, 	v1, 	c5
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -v2(depth), -c6";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, 1.050000, 0.210000, -0.250000
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
		
		PixelShaderConstant[6] = {0.100000, -0.550000, 0.290001, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c8, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth2 	v2
			mov 	r11, 	c6
			mov 	r13, 	c8
			add 	r13, 	-v2, 	-c6
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v3(depth), c7";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, 0.280001
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
		
		PixelShaderConstant[7] = {-0.500000, 0.300000, 1.100000, 0.220000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c9, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth3 	v3
			mov 	r11, 	c7
			mov 	r13, 	c9
			add_sat 	r13, 	v3, 	c7
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(depth), r11";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.230000, -0.250000, 0.200000, 1.150000
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
		
		PixelShaderConstant[8] = {0.270001, 0.750000, 0.300000, -0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth4 	v4
			mov 	r11, 	c8
			mov 	r13, 	c11
			add 	r13, 	v4, 	r11
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.z(depth), r12";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, 0.260001, 0.500000, 0.400000
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
		
		PixelShaderConstant[8] = {1.200000, 0.240000, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth5 	v5
			mov 	r12, 	c8
			mov 	r14, 	c11
			add 	r14, 	v5.z, 	r12
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6(depth), r13.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, 1.250000, 0.250000, 0.250000
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
		
		PixelShaderConstant[8] = {0.500000, -0.750000, 0.250001, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			mov 	r13, 	c8
			mov 	r0, 	c11
			add 	r0, 	v6, 	r13.zxyw
			mov 	oC0, 	r0

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v7.zw(depth), r14.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, 0.240001
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
		
		PixelShaderConstant[8] = {0.500000, -0.100000, 1.300000, 0.260000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			mov 	r14, 	c8
			mov 	r1, 	c11
			add 	r1, 	v7.zw, 	r14.w
			mov 	oC0, 	r1

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v8.wzyx(depth), r0.wx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.270000, 0.750000, -0.200000, 1.350000
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
		
		PixelShaderConstant[8] = {0.230001, -0.250000, 0.700000, -0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			mov 	r0, 	c8
			mov 	r2, 	c11
			add 	r2, 	v8.wzyx, 	r0.wx
			mov 	oC0, 	r2

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyzw v9(depth), r1";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, 0.220001, -0.500000, 0.800000
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
		
		PixelShaderConstant[8] = {1.400000, 0.280000, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			mov 	r1, 	c8
			mov 	r3, 	c11
			add 	r3.xyzw, 	v9, 	r1
			mov 	oC0, 	r3

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v0_abs(depth), r2_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, 1.450000, 0.290000, 1.250000
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
		
		PixelShaderConstant[8] = {0.900000, -0.950000, 0.210001, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth0 	v0
			mov 	r2, 	c8
			mov 	r4, 	c11
			add 	r4, 	v0_abs, 	r2_abs
			mov 	oC0, 	r4

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp v1(depth), r3";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 1.000000, -1.000000, 0.200001
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
		
		PixelShaderConstant[8] = {1.500000, -0.500000, 1.500000, 0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth1 	v1
			mov 	r3, 	c8
			mov 	r5, 	c11
			add_pp 	r5, 	v1, 	r3
			mov 	oC0, 	r5

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v2(depth), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.310000, 1.250000, -0.400000, 1.450000
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
		
		PixelShaderConstant[8] = {0.190001, -0.750000, 0.900000, -0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth2 	v2
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			add 	r6, 	v2, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v3.x(depth), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, 0.180001, -0.500000, 0.800000
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
		
		PixelShaderConstant[8] = {1.400000, 0.320000, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth3 	v3
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			add 	r6, 	v3.x, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v4(depth), vPos.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, 1.350000, 0.330000, 0.750000
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
		
		PixelShaderConstant[8] = {0.700000, -0.850000, 0.170001, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth4 	v4
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			add 	r6, 	v4, 	vPos.y
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v5.wy(depth), vPos.x";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, 0.160001
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
		
		PixelShaderConstant[8] = {0.500000, -0.100000, 1.300000, 0.340000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth5 	v5
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			add 	r6, 	v5.wy, 	vPos.x
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add v6.yzxw(depth), vPos.yx";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.350000, 0.250000, -0.000000, 1.250000
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
		
		PixelShaderConstant[8] = {0.150001, 0.250000, 0.500000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			add 	r6, 	v6.yzxw, 	vPos.yx
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.x v7(depth), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, 0.140001, 0.500000, 0.400000
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
		
		PixelShaderConstant[8] = {1.200000, 0.360000, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			add 	r6.x, 	v7, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -v8(depth), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, 1.150000, 0.370000, -0.250000
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
		
		PixelShaderConstant[8] = {0.300000, -0.650000, 0.130001, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			add 	r6, 	-v8, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat v9(depth), vPos.xy";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, 0.120001
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
		
		PixelShaderConstant[8] = {-0.500000, 0.300000, 1.100000, 0.380000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			add_sat 	r6, 	v9, 	vPos.xy
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, c8";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.390000, -0.250000, 0.400000, 1.050000
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
		
		PixelShaderConstant[8] = {0.110001, 0.750000, 0.100000, -0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c8
			mov 	r6, 	c11
			add 	r6, 	vPos.xy, 	c8
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.x, c9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, 0.100001, 0.500000, -0.000000
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
		
		PixelShaderConstant[9] = {1.000000, 0.400000, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c11, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c9
			mov 	r6, 	c11
			add 	r6, 	vPos.x, 	c9
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, c11.zxyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.950000, 0.410000, 0.250000
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
		
		PixelShaderConstant[11] = {-0.100000, -0.450000, 0.090001, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c12, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c11
			mov 	r6, 	c12
			add 	r6, 	vPos.xy, 	c11.zxyw
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.yx, c11.w";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, 0.080001
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
		
		PixelShaderConstant[11] = {0.500000, 0.700000, 0.900000, 0.420000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c13, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c11
			mov 	r6, 	c13
			add 	r6, 	vPos.yx, 	c11.w
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.y, c12.xyw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.430000, 0.750000, 0.800000, 0.850000
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
		
		PixelShaderConstant[12] = {0.070001, -0.250000, -0.300000, -0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c14, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c12
			mov 	r6, 	c14
			add 	r6, 	vPos.y, 	c12.xyw
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.y vPos.xy, c13";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, 0.060001, -0.500000, -0.400000
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
		
		PixelShaderConstant[13] = {0.800000, 0.440000, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c15, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c13
			mov 	r6, 	c15
			add 	r6.y, 	vPos.xy, 	c13
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, c14_abs";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.750000, 0.450000, 1.250000
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
		
		PixelShaderConstant[14] = {-0.500000, -0.250000, 0.050001, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c16, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c14
			mov 	r6, 	c16
			add 	r6, 	vPos.xy, 	c14_abs
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp vPos.xy, c15";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, 0.040001
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
		
		PixelShaderConstant[15] = {1.500000, 1.100000, 0.700000, 0.460000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c17, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c15
			mov 	r6, 	c17
			add_pp 	r6, 	vPos.xy, 	c15
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, r4";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.470000, 1.250000, 1.200000, 0.650000
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
		
		PixelShaderConstant[16] = {0.030001, -0.750000, -0.700000, -0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r4, 	c16
			mov 	r6, 	c18
			add 	r6, 	vPos.xy, 	r4
			mov 	oC0, 	r6

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.x, r5";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, 0.020001, -0.500000, -0.800000
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
		
		PixelShaderConstant[16] = {0.600000, 0.480000, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r5, 	c16
			mov 	r7, 	c18
			add 	r7, 	vPos.x, 	r5
			mov 	oC0, 	r7

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, r6.xyzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.550000, 0.490000, 0.750000
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
		
		PixelShaderConstant[16] = {-0.900000, -0.050000, 0.010001, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r6, 	c16
			mov 	r8, 	c18
			add 	r8, 	vPos.xy, 	r6.xyzw
			mov 	oC0, 	r8

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.yx, r7.y";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, 0.000001
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
		
		PixelShaderConstant[16] = {0.500000, 1.500000, 0.500000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r7, 	c16
			mov 	r9, 	c18
			add 	r9, 	vPos.yx, 	r7.y
			mov 	oC0, 	r9

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.y, r8.xzw";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.510000, 0.250000, 1.400000, 0.450000
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
		
		PixelShaderConstant[16] = {-0.009999, 0.250000, -0.900000, 0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r8, 	c16
			mov 	r10, 	c18
			add 	r10, 	vPos.y, 	r8.xzw
			mov 	oC0, 	r10

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.z vPos.xy, r9";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, -0.019999, 0.500000, -0.800000
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
		
		PixelShaderConstant[16] = {0.400000, 0.520000, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r9, 	c16
			mov 	r11, 	c18
			add 	r11.z, 	vPos.xy, 	r9
			mov 	oC0, 	r11

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos_abs.xy, -r10";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.350000, 0.530000, -0.250000
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
		
		PixelShaderConstant[16] = {-0.700000, 0.150000, -0.029999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r10, 	c16
			mov 	r12, 	c18
			add 	r12, 	vPos_abs.xy, 	-r10
			mov 	oC0, 	r12

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat vPos.xy, r11";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, -0.039999
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
		
		PixelShaderConstant[16] = {-0.500000, 1.100000, 0.300000, 0.540000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r11, 	c16
			mov 	r13, 	c18
			add_sat 	r13, 	vPos.xy, 	r11
			mov 	oC0, 	r13

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v0(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.550000, -0.250000, 1.000000, 0.250000
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
		
		PixelShaderConstant[16] = {-0.049999, 0.750000, -0.500000, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v0
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.x, v1(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, -0.059999, 0.500000, -0.400000
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
		
		PixelShaderConstant[16] = {0.200000, 0.560000, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.x, 	v1
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v2.zxyw(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.150000, 0.570000, 0.250000
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
		
		PixelShaderConstant[16] = {-0.300000, 0.350000, -0.069999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v2.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.yx, v3.w(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, -0.079999
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
		
		PixelShaderConstant[16] = {0.500000, 0.700000, 0.100000, 0.580000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.yx, 	v3.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.y, v4.xwz(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.590000, 0.750000, 0.600000, 0.050000
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
		
		PixelShaderConstant[16] = {-0.089999, -0.250000, -0.100000, 0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.y, 	v4.xwz
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.w vPos.xy, v5(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, -0.099999, -0.500000, -0.000000
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
		
		PixelShaderConstant[16] = {0.000000, 0.600000, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14.w, 	vPos.xy, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -vPos.xy, -v6(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, -0.050000, 0.610000, 1.250000
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
		
		PixelShaderConstant[16] = {0.100000, 0.550000, -0.109999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	-vPos.xy, 	-v6
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp vPos.xy, v7(texcoord)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, -0.119999
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
		
		PixelShaderConstant[16] = {1.500000, 0.300000, -0.100000, 0.620000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_texcoord7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add_pp 	r14, 	vPos.xy, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v8(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.630000, 1.250000, 0.200000, -0.150000
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
		
		PixelShaderConstant[16] = {-0.129999, -0.750000, 0.300000, 0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v8
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.x, v9(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, -0.139999, -0.500000, 0.400000
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
		
		PixelShaderConstant[16] = {-0.200000, 0.640000, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.x, 	v9
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v0.xyzw(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, -0.250000, 0.649999, 0.750000
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
		
		PixelShaderConstant[16] = {0.500000, 0.750000, -0.149999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v0.xyzw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.yx, v1.y(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, -0.159999
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
		
		PixelShaderConstant[16] = {0.500000, -0.100000, -0.300000, 0.659999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.yx, 	v1.y
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.y, v2.yxw(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.669999, 0.250000, -0.200000, -0.350000
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
		
		PixelShaderConstant[16] = {-0.169999, 0.250000, 0.700000, 0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.y, 	v2.yxw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xy vPos.xy, v3(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, -0.179999, 0.500000, 0.800000
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
		
		PixelShaderConstant[16] = {-0.400000, 0.679999, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14.xy, 	vPos.xy, 	v3
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos_abs.xy, v4_abs(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, -0.450000, 0.689999, -0.250000
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
		
		PixelShaderConstant[16] = {0.900000, 0.950000, -0.189999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos_abs.xy, 	v4_abs
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat vPos.xy, v5(blendweight)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 1.000000, 1.000000, -0.199999
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
		
		PixelShaderConstant[16] = {-0.500000, -0.500000, -0.500000, 0.699999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendweight5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add_sat 	r14, 	vPos.xy, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v6(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.709999, -0.250000, -0.400000, -0.450000
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
		
		PixelShaderConstant[16] = {-0.209999, 0.750000, 0.900000, 0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v6
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.x, v7(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.900000, -0.219999, 0.500000, 0.800000
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
		
		PixelShaderConstant[16] = {-0.400000, 0.719999, 0.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.x, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v8.zxyw(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, -0.350000, 0.729999, 0.250000
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
		
		PixelShaderConstant[16] = {0.700000, 0.850000, -0.229999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v8.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.yx, v9.w(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, 0.800000, -0.239999
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
		
		PixelShaderConstant[16] = {0.500000, -0.100000, -0.300000, 0.739999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.yx, 	v9.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.y, v0.yzw(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.749999, 0.750000, -0.000000, -0.250000
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
		
		PixelShaderConstant[16] = {-0.249999, -0.250000, 0.500000, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.y, 	v0.yzw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xz vPos.xy, v1(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.700000, -0.259999, -0.500000, 0.400000
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
		
		PixelShaderConstant[16] = {-0.200000, 0.759999, 1.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14.xz, 	vPos.xy, 	v1
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos_abs.xy, v2(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, -0.150000, 0.769999, 1.250000
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
		
		PixelShaderConstant[16] = {0.300000, 0.650000, -0.269999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos_abs.xy, 	v2
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp vPos.xy, v3(blendindices)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 0.200000, 0.600000, -0.279999
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
		
		PixelShaderConstant[16] = {1.500000, 0.300000, -0.100000, 0.779999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_blendindices3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add_pp 	r14, 	vPos.xy, 	v3
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v4(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.789999, 1.250000, 0.400000, -0.050000
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
		
		PixelShaderConstant[16] = {-0.289999, -0.750000, 0.100000, 0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v4
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.x, v5(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.500000, -0.299999, -0.500000, -0.000000
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
		
		PixelShaderConstant[16] = {0.000000, 0.799999, 1.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.x, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v6.xyzw(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.050000, 0.809999, 0.750000
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
		
		PixelShaderConstant[16] = {-0.100000, 0.450000, -0.309999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v6.xyzw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.yx, v7.y(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, 0.400000, -0.319999
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
		
		PixelShaderConstant[16] = {0.500000, 0.700000, 0.100000, 0.819999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.yx, 	v7.y
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.y, v8.ywz(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.829999, 0.250000, 0.800000, 0.150000
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
		
		PixelShaderConstant[16] = {-0.329999, 0.250000, -0.300000, 0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.y, 	v8.ywz
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xw vPos.xy, v9(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.300000, -0.339999, 0.500000, -0.400000
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
		
		PixelShaderConstant[16] = {0.200000, 0.839999, 0.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14.xw, 	vPos.xy, 	v9
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, -v0(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.250000, 0.849999, -0.250000
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
		
		PixelShaderConstant[16] = {-0.500000, 0.250000, -0.349999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	-v0
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat vPos.xy, v1(normal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, -0.600000, 0.200000, -0.359999
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
		
		PixelShaderConstant[16] = {-0.500000, 1.100000, 0.300000, 0.859999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_normal1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add_sat 	r14, 	vPos.xy, 	v1
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v2(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.869999, -0.250000, 1.200000, 0.350000
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
		
		PixelShaderConstant[16] = {-0.369999, 0.750000, -0.700000, 0.150000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v2
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.x, v3(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.100000, -0.379999, 0.500000, -0.800000
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
		
		PixelShaderConstant[16] = {0.400000, 0.879999, 0.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.x, 	v3
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v4.zxyw(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.400000, 0.450000, 0.889999, 0.250000
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
		
		PixelShaderConstant[16] = {-0.900000, 0.050000, -0.389999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v4.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.yx, v5.w(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -1.000000, 0.000000, -0.399999
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
		
		PixelShaderConstant[16] = {0.500000, 1.500000, 0.500000, 0.899999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.yx, 	v5.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.y, v6.zxw(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.909999, 0.750000, 1.400000, 0.550000
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
		
		PixelShaderConstant[16] = {-0.409999, -0.250000, -0.900000, -0.050000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.y, 	v6.zxw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yz vPos.xy, v7(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.100000, -0.419999, -0.500000, -0.800000
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
		
		PixelShaderConstant[16] = {0.600000, 0.919999, 1.000000, 1.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14.yz, 	vPos.xy, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -vPos.xy, v8_abs(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.200000, 0.650000, 0.929999, 1.250000
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
		
		PixelShaderConstant[16] = {-0.700000, -0.150000, -0.429999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	-vPos.xy, 	v8_abs
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp vPos.xy, v9(tangent)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, -0.439999
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
		
		PixelShaderConstant[16] = {1.500000, 1.100000, 0.700000, 0.939999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_tangent9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add_pp 	r14, 	vPos.xy, 	v9
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v0(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.949999, 1.250000, 1.000000, 0.750000
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
		
		PixelShaderConstant[16] = {-0.449999, -0.750000, -0.500000, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v0
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.x, v1(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, -0.459999, -0.500000, -0.400000
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
		
		PixelShaderConstant[16] = {0.800000, 0.959999, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.x, 	v1
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v2.xyzw(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.800000, 0.850000, 0.969999, 0.750000
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
		
		PixelShaderConstant[16] = {-0.300000, -0.350000, -0.469999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v2.xyzw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.yx, v3.y(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, -0.479999
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
		
		PixelShaderConstant[16] = {0.500000, 0.700000, 0.900000, 0.979999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.yx, 	v3.y
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.y, v4.zyw(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.989999, 0.250000, 0.600000, 0.950000
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
		
		PixelShaderConstant[16] = {-0.489999, 0.250000, -0.100000, -0.450000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.y, 	v4.zyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.yw vPos.xy, v5(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, -0.499999, 0.500000, -0.000000
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
		
		PixelShaderConstant[16] = {1.000000, 0.999999, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14.yw, 	vPos.xy, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -vPos.xy, -v6(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.400000, 1.050000, 1.009999, -0.250000
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
		
		PixelShaderConstant[16] = {0.100000, -0.550000, -0.509999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	-vPos.xy, 	-v6
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat vPos.xy, v7(binormal)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, -0.519999
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
		
		PixelShaderConstant[16] = {-0.500000, 0.300000, 1.100000, 1.019999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_binormal7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add_sat 	r14, 	vPos.xy, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v8(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.029999, -0.250000, 0.200000, 1.150000
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
		
		PixelShaderConstant[16] = {-0.529999, 0.750000, 0.300000, -0.650000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v8
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.x, v9(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, -0.539999, 0.500000, 0.400000
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
		
		PixelShaderConstant[16] = {1.200000, 1.039999, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.x, 	v9
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v0.zxyw(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.000000, 1.250000, 1.049999, 0.250000
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
		
		PixelShaderConstant[16] = {0.500000, -0.750000, -0.549999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v0.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.yx, v1.w(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, -0.559999
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
		
		PixelShaderConstant[16] = {0.500000, -0.100000, 1.300000, 1.059999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.yx, 	v1.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.y, v2.zwy(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.069999, 0.750000, -0.200000, 1.350000
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
		
		PixelShaderConstant[16] = {-0.569999, -0.250000, 0.700000, -0.850000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.y, 	v2.zwy
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.zw vPos.xy, v3(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, -0.579999, -0.500000, 0.800000
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
		
		PixelShaderConstant[16] = {1.400000, 1.079999, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14.zw, 	vPos.xy, 	v3
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos_abs.xy, v4_abs(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.400000, 1.450000, 1.089999, 1.250000
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
		
		PixelShaderConstant[16] = {0.900000, -0.950000, -0.589999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos_abs.xy, 	v4_abs
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp vPos.xy, v5(color)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, 1.000000, -1.000000, -0.599999
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
		
		PixelShaderConstant[16] = {1.500000, -0.500000, 1.500000, 1.099999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_color5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add_pp 	r14, 	vPos.xy, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v6(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.109999, 1.250000, -0.400000, 1.450000
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
		
		PixelShaderConstant[16] = {-0.609999, -0.750000, 0.900000, -0.950000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v6
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.x, v7(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.900000, -0.619999, -0.500000, 0.800000
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
		
		PixelShaderConstant[16] = {1.400000, 1.119999, 1.000000, -0.300000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.x, 	v7
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v8.xyzw(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.200000, 1.350000, 1.129999, 0.750000
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
		
		PixelShaderConstant[16] = {0.700000, -0.850000, -0.629999, -0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v8.xyzw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.yx, v9.y(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, 0.600000, -0.800000, -0.639999
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
		
		PixelShaderConstant[16] = {0.500000, -0.100000, 1.300000, 1.139999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.yx, 	v9.y
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.y, v0.wxz(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.149999, 0.250000, -0.000000, 1.250000
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
		
		PixelShaderConstant[16] = {-0.649999, 0.250000, 0.500000, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.y, 	v0.wxz
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyz vPos.xy, v1(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.700000, -0.659999, 0.500000, 0.400000
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
		
		PixelShaderConstant[16] = {1.200000, 1.159999, 0.000000, 0.100000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14.xyz, 	vPos.xy, 	v1
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add -vPos.xy, v2(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.200000, 1.150000, 1.169999, -0.250000
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
		
		PixelShaderConstant[16] = {0.300000, -0.650000, -0.669999, 0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog2 	v2
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	-vPos.xy, 	v2
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_sat vPos.xy, v3(fog)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.200000, -0.600000, -0.679999
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
		
		PixelShaderConstant[16] = {-0.500000, 0.300000, 1.100000, 1.179999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_fog3 	v3
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add_sat 	r14, 	vPos.xy, 	v3
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v4(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.189999, -0.250000, 0.400000, 1.050000
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
		
		PixelShaderConstant[16] = {-0.689999, 0.750000, 0.100000, -0.550000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth4 	v4
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v4
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.x, v5(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.500000, -0.699999, 0.500000, -0.000000
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
		
		PixelShaderConstant[16] = {1.000000, 1.199999, 0.000000, 0.500000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth5 	v5
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.x, 	v5
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v6.zxyw(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.600000, 0.950000, 1.209999, 0.250000
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
		
		PixelShaderConstant[16] = {-0.100000, -0.450000, -0.709999, 0.250000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth6 	v6
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v6.zxyw
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.yx, v7.w(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 0.000000, -0.200000, -0.400000, -0.719999
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
		
		PixelShaderConstant[16] = {0.500000, 0.700000, 0.900000, 1.219999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth7 	v7
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.yx, 	v7.w
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.y, v8.wyz(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.229999, 0.750000, 0.800000, 0.850000
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
		
		PixelShaderConstant[16] = {-0.729999, -0.250000, -0.300000, -0.350000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth8 	v8
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.y, 	v8.wyz
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add.xyw vPos.xy, v9(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -0.300000, -0.739999, -0.500000, -0.400000
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
		
		PixelShaderConstant[16] = {0.800000, 1.239999, 1.000000, 0.900000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth9 	v9
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14.xyw, 	vPos.xy, 	v9
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add vPos.xy, v0_abs(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, 1.000000, 0.750000, 1.249999, 1.250000
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
		
		PixelShaderConstant[16] = {-0.500000, -0.250000, -0.749999, -0.750000};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth0 	v0
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add 	r14, 	vPos.xy, 	v0_abs
			mov 	oC0, 	r14

		};
	}
}
Technique
<
	String	Name = "ps3_0 add_pp vPos.xy, v1(depth)";
	String	Shape = "TinyQuad";
	float   TolPerPixel = 0.02f;
	float   TolPerPixelPerChannel = 0.02f;
>
{
	Pass P0
	{
		VertexShaderConstant[0] = <mFinal>;
		VertexShader = 
		asm
		{
			vs_3_0
			def c4, -1.000000, -0.600000, -0.200000, -0.759999
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
		
		PixelShaderConstant[16] = {1.500000, 1.100000, 0.700000, 1.259999};
		
		PixelShader =
		asm
		{
			ps_3_0
			def 	c18, 	0.f, 	0.f, 	0.f, 	0.f
			dcl_depth1 	v1
			dcl 	vPos.xy
			mov 	r12, 	c16
			mov 	r14, 	c18
			add_pp 	r14, 	vPos.xy, 	v1
			mov 	oC0, 	r14

		};
	}
}

//cases with predicates
Technique
<
	String	Name = "Inst: add: (p)";
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
			ps_3_0

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
	String	Name = "Inst: add: (p).x";
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
			ps_3_0

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
	String	Name = "Inst: add: (p(masked))";
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
			ps_3_0

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
	String	Name = "Inst: add: (p(masked).x)";
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
			ps_3_0

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
	String	Name = "Inst: add: (!p(masked))";
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
			ps_3_0

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
	String	Name = "Inst: add: (!p(masked).x)";
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
			ps_3_0

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
	String	Name = "Inst: add: (!p(masked).x), mask";
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
			ps_3_0

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