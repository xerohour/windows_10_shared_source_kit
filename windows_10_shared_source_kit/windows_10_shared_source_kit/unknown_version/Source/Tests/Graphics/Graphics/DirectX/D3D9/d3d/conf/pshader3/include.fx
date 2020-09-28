float4x4  mFinal;
float4x4  mWorld;
float4	  fViewerPos = {0,0,0,0};
float4	  fViewerNormal = {0,0,1,0};
float4	  fvPosLimits;
float4    fMaxTexRepeat;
float4    fTexSizes;
float4    fCubeSizes;
float4    fVolSizes;

texture  Tex2D_0;
texture  Tex2D_1;
texture  Tex2D_2;
texture	 TexCube_0;
texture	 TexCube_1;
texture	 TexVol_0;
texture	 TexVol_1;

texture  TexSpecDiffLookup;
texture	 TexNormalMap;

static const VertexShader StandardVS_3_0=
	asm
	
	{
		vs_3_0
		dcl_position	v0
		dcl_normal		v1
		dcl_texcoord0	v2
		
		dcl_position	o0
		dcl_texcoord0	o1
		
		m4x4	        o0, v0, c0
		mov		o1.xy, v2.xy
	};
	
static const VertexShader TexTLVS_3_0=
asm
{
	vs_3_0
	dcl_position	v0
	dcl_color0    	v1
	dcl_texcoord0	v2
	dcl_texcoord1	v3
	dcl_texcoord2	v4
	
	dcl_position	o0
	dcl_color0   	o1
	dcl_texcoord0   o2
	dcl_texcoord1   o3
	dcl_texcoord2   o4
	
	mov	o0, v0
	mov o1, v1
	mov o2, v2
	mov o3, v3
	mov o4, v4
};

static const VertexShader VolTLVS_3_0=
	asm
	{
		vs_3_0
		dcl_position	v0
		dcl_color0    	v1
		dcl_texcoord0	v2
		dcl_texcoord1	v3
		dcl_texcoord2	v4
		
		dcl_position	o0
		dcl_color0   	o1
		dcl_texcoord0	o2
		dcl_texcoord1   o3
		dcl_texcoord2   o4
		
		mov	o0, v0
		mov o1, v1
		mov o2, v2
		mov o3, v3
		mov o4, v4
	};

static const VertexShader StandardVS_2_0=
	asm
	{
		vs_2_0
		dcl_position	v0
		dcl_normal		v1
		dcl_texcoord0	v2
		
		m4x4	        oPos, v0, c0
		mov		oT0.xy, v2.xy
	};

static const VertexShader StandardVS_1_0=
	asm
	{
		vs_1_1
		dcl_position	v0
		dcl_normal		v1
		dcl_texcoord0	v2
		
		m4x4	        oPos, v0, c0
		mov		oT0.xy, v2.xy
	};

static const VertexShader TexTLVS_1_0=
	asm
	{
		vs_1_1
		dcl_position	v0
		dcl_color0    	v1
		dcl_texcoord0	v2
		dcl_texcoord1	v3
		dcl_texcoord2	v4
		
		mov	oPos, v0
		mov oD0,  v1
		mov	oT0,  v2
		mov	oT1,  v3
		mov	oT2,  v4
	};

	
static const VertexShader TexTLVS_2_0=
	asm
	{
		vs_2_0
		dcl_position	v0
		dcl_color0    	v1
		dcl_texcoord0	v2
		dcl_texcoord1	v3
		dcl_texcoord2	v4
		
		mov	oPos, v0
		mov oD0,  v1
		mov	oT0,  v2
		mov	oT1,  v3
		mov	oT2,  v4
	};
	
static const VertexShader VolTLVS_2_X=
	asm
	{
		vs_2_0
		dcl_position	v0
		dcl_color0    	v1
		dcl_texcoord0	v2
		dcl_texcoord1	v3
		dcl_texcoord2	v4
		
		mov	oPos, v0
		mov oD0,  v1
		mov	oT0,  v2
		mov	oT1,  v3
		mov	oT2,  v4
	};

static const VertexShader VolTLVS_2_0=
	asm
	{
		vs_2_0
		dcl_position	v0
		dcl_color0    	v1
		dcl_texcoord0	v2
		
		mov	oPos,   v0
		mov     oD0,    v1
		mov	oT0,    v2
	};

static const VertexShader VolTLVS_1_0=
	asm
	{
		vs_1_1
		dcl_position	v0
		dcl_color0    	v1
		dcl_texcoord0	v2
		
		mov	oPos,   v0
		mov     oD0,    v1
		mov	oT0,    v2
	};


VertexShader StandardVS=
#ifdef USING_PS_3_0
	StandardVS_3_0;
#endif
#ifdef USING_PS_2_X
	StandardVS_2_0;
#endif
#ifdef USING_PS_2_0
	StandardVS_2_0;
#endif
#ifdef USING_PS_1_X
        StandardVS_1_0;
#endif

VertexShader TexTLVS=
#ifdef USING_PS_3_0
	TexTLVS_3_0;
#endif
#ifdef USING_PS_2_X
	TexTLVS_2_0;
#endif
#ifdef USING_PS_2_0
	TexTLVS_2_0;
#endif
#ifdef USING_PS_1_X
        TexTLVS_1_0;
#endif

VertexShader VolTLVS=
#ifdef USING_PS_3_0
	VolTLVS_3_0;
#endif
#ifdef USING_PS_2_X
	VolTLVS_2_X;
#endif
#ifdef USING_PS_2_0
	VolTLVS_2_0;
#endif
#ifdef USING_PS_1_X
        VolTLVS_1_0;
#endif

struct VS_OUTPUT
{
    float4 Position : POSITION;
    float4 Diffuse : COLOR0;
    float4 Specular : COLOR1;
    float2 TexCoord0 : TEXCOORD0;
};

VS_OUTPUT VertexLighting
    (
    float4 vPos : POSITION, 
    float3 vNormal : NORMAL,
    float2 vTexCoord0 : TEXCOORD0
    )
{
    VS_OUTPUT Output;
    float4 vTransNormal;
	float4 vNormalPad;
	
    Output.Position = mul(vPos, mFinal);
    
	vNormalPad.xyz = vNormal;
	vNormalPad.w = 1.0f;
    vTransNormal = mul(vNormalPad, mWorld);
    Output.Diffuse = dot(vTransNormal, -fViewerNormal);
    
    Output.Specular = 0;
    Output.TexCoord0 = vTexCoord0;
    return Output;
}