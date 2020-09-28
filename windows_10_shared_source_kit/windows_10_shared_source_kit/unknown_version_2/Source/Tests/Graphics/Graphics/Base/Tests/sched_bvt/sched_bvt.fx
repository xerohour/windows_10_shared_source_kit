//-----------------------------------------------------------------------------
// File: sched_bvt.fx
// Desc: Effect File for Scheduler BVT Test
// Copyright (c) Microsoft Corporation.  All rights reserved.
//-----------------------------------------------------------------------------

// light direction (view space)
float3 LightDirection  = {0.f, -0.003, -0.999};

// light intensity
float4 I_a = { 0.1f, 0.1f, 0.1f, 1.0f };    // ambient
float4 I_d = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse
float4 I_s = { 1.0f, 1.0f, 1.0f, 1.0f };    // specular

// material reflectivity
float4 k_a : MATERIALAMBIENT = { 0.5f, 0.5f, 0.5f, 1.0f };    // ambient
float4 k_d : MATERIALDIFFUSE = { 0.5f, 0.5f, 0.5f, 1.0f };    // diffuse
float4 k_s : MATERIALSPECULAR= { 1.0f, 1.0f, 1.0f, 1.0f };    // specular
float  n   : MATERIALPOWER = 32.0f;                           // power

// transformations
float4x4 matView  : VIEW;
float4x4 matProj  : PROJECTION;
float4x4 matWorld : WORLD;

texture skybox_tex;
sampler skybox_sampler = sampler_state
{
	Texture = (skybox_tex);
	MinFilter = LINEAR;
	MipFilter = LINEAR;
	MagFilter = LINEAR;
};

texture Texture0;
sampler Texture0_sampler = sampler_state
{
	Texture = (Texture0);
	MinFilter = LINEAR;
	MipFilter = LINEAR;
	MagFilter = LINEAR;
};

texture Texture1;
sampler Texture1_sampler = sampler_state
{
	Texture = (Texture1);
	MinFilter = LINEAR;
	MipFilter = LINEAR;
	MagFilter = LINEAR;
};

#define MATVIEWPROJ      mul(matView, matProj)
#define MATWORLDVIEWPROJ mul( mul( matWorld, matView), matProj )
#define MATWORLDVIEW     mul(matWorld, matView)


//-----------------------------------------------------------------------------
// Name: ptWorldToClipspace()
// Desc: Moves a point between world and clipspace coordinate systems
//-----------------------------------------------------------------------------
float4 ptWorldToClipspace( float4 vecPosition )
{
    return mul( vecPosition, MATVIEWPROJ );
}




//-----------------------------------------------------------------------------
// Name: ptLocalToClipspace()
// Desc: Moves a point between local model space, clipspace coordinate systems
//-----------------------------------------------------------------------------
float4 ptLocalToClipspace( float4 vecPosition )
{
    return mul( vecPosition, MATWORLDVIEWPROJ );
}




//-----------------------------------------------------------------------------
// Name: ptLocalToView()
// Desc: Moves a point between local model space, view coordinate systems
//-----------------------------------------------------------------------------
float4 ptLocalToView( float4 vecPosition )
{
    return mul( vecPosition, MATWORLDVIEW );
}




//-----------------------------------------------------------------------------
// Name: ptLocalToWorld()
// Desc: Moves a point between local model space, world coordinate systems
//-----------------------------------------------------------------------------
float4 ptLocalToWorld( float4 vecPosition )
{
    return mul( vecPosition, matWorld );
}




//-----------------------------------------------------------------------------
// Name: ptSkyBoxToClipspace()
// Desc: Applies a skybox transform into ClipSpace (no translations applied)
//-----------------------------------------------------------------------------
float4 ptSkyBoxToClipspace( float4 vecPosition, float fYOffset )
{
    // strip any translation off of the view matrix
    // use only rotations & the projection matrix
    float4x4 matViewNoTrans =
    {
        matView[0],
        matView[1],
        matView[2],
        float4( 0.f, fYOffset, 0.f, 1.f )
    };

    // output the position
    return mul( vecPosition, mul( matViewNoTrans, matProj ) );
}




//-----------------------------------------------------------------------------
// Name: ptViewBillboardToClipspace()
// Desc: Applies a viewbillboard transform into ClipSpace
//-----------------------------------------------------------------------------
float4 ptViewBillboardToClipspace( float4 vecPosition,
								   float2 vecViewBasis,
								   float fDistance )
{
	// Transform the position into view space
	float4 vecPosition_view = ptLocalToView( vecPosition );
	
	// calculate view-space displacement
	float2 fDisplacement = fDistance * (vecViewBasis - float2( 0.5f, 0.5f ));
	
	// add to position, to billboard the face
	vecPosition_view += float4( fDisplacement, 0.f, 0.f );
	
	// now, output to clip space
	return mul( vecPosition_view, matProj );
}




//-----------------------------------------------------------------------------
// Name: vcLocalToWorld()
// Desc: Moves a directional unit vector (normals, tangents, etc -- NOT positions)
//       ASSUMES:  matWorld^-1 == matWorld^T
//       ASSUMES:  input (directional) vector does not translate
//-----------------------------------------------------------------------------
float3 vcLocalToWorld( float3 vecDirectionLocal )
{
	float3 vecXform = mul( vecDirectionLocal, matWorld );
	return normalize(vecXform);
}




//-----------------------------------------------------------------------------
// Name: vcLocalToView()
// Desc: Moves a directional unit vector (normals, tangents, etc -- NOT positions)
//       ASSUMES:  matWorld^-1 == matWorld^T
//       ASSUMES:  matView^-1  == matView^T
//       ASSUMES:  input (direction) vectors do not translate
//-----------------------------------------------------------------------------
float3 vcLocalToView( float3 vecDirectionLocal )
{
	float3 vecXform = mul( vecDirectionLocal, MATWORLDVIEW );
	return normalize(vecXform);
}




//-----------------------------------------------------------------------------
// Name: vcWorldToView()
// Desc: Moves a directional unit vector (normals, tangents, etc -- NOT positions)
//       ASSUMES:  matView^-1  == matView^T
//       ASSUMES:  input (direction) vectors do not translate
//-----------------------------------------------------------------------------
float3 vcWorldToView( float3 vecDirectionWorld )
{
	float3 vecXform = mul( vecDirectionWorld, matView );
	return normalize(vecXform);
}



//-----------------------------------------------------------------------------
// Name: NormalToTangent()
// Desc: Calculate an arbitrary tangent-space basis from a normal vector
//-----------------------------------------------------------------------------
void NormalToTangent ( in  float3 normal,
                       out float3 tangent,
                       out float3 binormal )
{

    tangent = float3( normal.z, 0.f, normal.x );
    if( dot(tangent, tangent) < 0.2f )
        tangent = float3( -normal.y, normal.x, 0.f );
    tangent = normalize( tangent );
    binormal = cross( normal, tangent );
}




//-----------------------------------------------------------------------------
// Name: Reflect()
// Desc: Calculate an arbitrary tangent-space basis from a normal vector
//-----------------------------------------------------------------------------
float3 Reflect ( float3 normal, float3 direction )
{
	return normalize( 2.f * dot( normal, direction) * normal - direction); 
}




//-----------------------------------------------------------------------------
// Name: FresnelApprox()
// Desc: Calculate a fresnel term approximation
//-----------------------------------------------------------------------------
float FresnelApprox ( float3 normal, float3 ToViewer )
{
	float fFresnel = 0.5f - dot( ToViewer, normal );
	return saturate(1.f - 4.f*fFresnel*fFresnel); 
}




//-----------------------------------------------------------------------------
// Name: DiffuseTerm()
// Desc: Calculate diffuse term - directional light
//-----------------------------------------------------------------------------
float DiffuseTerm( float3 normal, float3 ToLight )
{
	return max( 0, dot( normal, ToLight ) );
}




//-----------------------------------------------------------------------------
// Name: matDiffuse()
// Desc: Calculate diffuse term - directional light
//-----------------------------------------------------------------------------
float4 matDiffuse( float4 material, float i_d, float3 normal, float3 ToLight )
{
	return i_d * material * DiffuseTerm( normal, ToLight );
}




//-----------------------------------------------------------------------------
// Name: Specular()
// Desc: Calculate a specular term - directional light
//-----------------------------------------------------------------------------
float SpecularTerm( float3 normal, float3 ToLight, float3 ToViewer )
{
    float3 vecHalf   = normalize( ToViewer + ToLight );
    float specular = max( 0, dot( vecHalf, normal ) );

    // raise the power of specular coefficient
    specular *= specular;
    specular *= specular;
    specular *= specular;
    specular *= specular;
    specular *= specular;
    specular *= specular;
    specular *= specular;
    
    return specular;
}




//-----------------------------------------------------------------------------
// Name: matSpecular()
// Desc: Calculate diffuse term - directional light
//-----------------------------------------------------------------------------
float4 matSpecular( float4 material,
                    float i_s,
                    float3 normal,
                    float3 ToLight,
                    float3 ToViewer )
{
	return i_s * material * SpecularTerm( normal, ToLight, ToViewer );
}


//-----------------------------------------------------------------------------


void VS ( in  float4 v0   : POSITION,
          in  float3 v1   : NORMAL,
          in  float2 v2   : TEXCOORD0,
          out float4 oPos : POSITION,
          out float4 oD0  : COLOR0,
          out float4 oD1  : COLOR1,
          out float2 oT0  : TEXCOORD0,
          out float3 oT1  : TEXCOORD1,
          out float  oT2  : TEXCOORD2,
          out float2 oT3  : TEXCOORD3 )
{
    // immediately output the transformedposition
    oPos = ptLocalToClipspace( v0 );

    // work in view-space
    float3 vecNormal  = vcLocalToView( v1 );
    float3 vecToLight = vcWorldToView( -LightDirection );

    // calculate environment, specular vectors
    float3 vecToViewer = normalize( ptLocalToView( -v0 ) );
    
    // pass the mesh texture coordinate through
    oT0 = oT3 = v2;

    // store the glance vector for environment mapping
    float3 vecGlanceView = Reflect( vecNormal, vecToViewer );
    matrix matInvView = transpose( matView );
    matInvView[3] = float4( 0.f, 0.f, 0.f, 0.f );
    oT1 = mul( float4( vecGlanceView, 0.f), matInvView );
    

	// output the fresnel term
	oT2 = FresnelApprox( vecNormal, vecToViewer );

    // smaller ambient == illusion of self-shadowing
    // (do not use I_a)
    const float4 ambient_bias = float4( 0.1f, 0.1f, 0.1f, 0.f );

    // color calculations
    float4 diffuse  = matDiffuse( k_d, I_d, vecNormal, vecToLight );
    float4 ambient  = k_a + ambient_bias;
	float4 specular = matSpecular( k_s, I_s, vecNormal, vecToLight, vecToViewer );
    
    // output non-specular terms
    oD0 = ambient + diffuse;

    // output specular
    oD1 = specular;
}




//-----------------------------------------------------------------------------
// Name: Mesh_PS()
// Desc: Pixel Shader for mesh objects
//-----------------------------------------------------------------------------
void PS (  in  float4 diffuse   : COLOR0,
                in  float4 specular  : COLOR1,
                in  float2 texcoord0 : TEXCOORD0,
                in  float3 texcoord1 : TEXCOORD1,
                in  float  fresnel   : TEXCOORD2,
                in  float2 lightmap  : TEXCOORD3,
                out float4 r0        : COLOR0 )
{
	// controls the amount of environment mapping
	const float emap_factor = 0.20f;
	
    // sample the mesh texture
    float4 t0 = tex2D( Texture0_sampler, texcoord0 );

    // sample the environment map
    float4 t1 = texCUBE( skybox_sampler, texcoord1 );

	// get the mask for blur of lighting
	float4 t2 = tex2D( Texture1_sampler, lightmap );
	
    // establish any arbitrary amount of reflection to blend in
    float4 environment_weight = saturate(emap_factor * float4(fresnel, fresnel, fresnel, 0.f));

	const float alpha_threshold = 0.9f;
	
    // blend the textures, color, and specular lighting
	r0 = saturate( t0 * diffuse + t1 * environment_weight + specular);

	// set the blur factor
    r0.a = t2.r;
}


technique TVertexAndPixelShader
{
    pass P0
    {
        // shaders
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_1_1 PS();

        ZEnable = TRUE;
        ZWriteEnable = TRUE;
        AlphaBlendEnable = FALSE;
    }  
}

//-----------------------------------------------------------------------------

float2 TapOffset0 = float2( 0.f, 0.f );
float2 TapOffset1 = float2( 0.f, 0.f );
float2 TapOffset2 = float2( 0.f, 0.f );
float2 TapOffset3 = float2( 0.f, 0.f );

void VS_Blur(
    in  float3 Pos  : POSITION, 
    out float4 oPos  : POSITION,
    out float2 oTexCoord0 : TEXCOORD0,
    out float2 oTexCoord1 : TEXCOORD1,
    out float2 oTexCoord2 : TEXCOORD2,
    out float2 oTexCoord3 : TEXCOORD3)
{
	const matrix xform =
	{
		1.f,  0.f,  0.f,  0.f,
		0.f,  1.f,  0.f,  0.f,
		0.f,  0.f, 0.5f,  0.f,
		0.f,  0.f, 0.5f,  1.f
	};
	
	oPos = mul( float4( Pos, 1.f), xform );
	
	oTexCoord0 = float2( 0.5f * Pos.x + 0.5f,
	                    0.5f * Pos.y + 0.5f );
	oTexCoord1 = oTexCoord0;
	oTexCoord2 = oTexCoord0;
	oTexCoord3 = oTexCoord0;
	
	oTexCoord0 += TapOffset0;
	oTexCoord1 += TapOffset1;
	oTexCoord2 += TapOffset2;
	oTexCoord3 += TapOffset3;
}

texture rt_tex;

sampler blur_sampler = sampler_state
{
	Texture = (rt_tex);
	MipFilter = LINEAR;
	MagFilter = LINEAR;
	MinFilter = LINEAR;
};

float4 PS_Blur( in float2  TC0 : TEXCOORD0,
                  in float2  TC1 : TEXCOORD1,
                  in float2  TC2 : TEXCOORD2,
                  in float2  TC3 : TEXCOORD3) : COLOR
{
	float4 Sample0 = tex2D( blur_sampler, TC0 );
	float4 Sample1 = tex2D( blur_sampler, TC1 );
	float4 Sample2 = tex2D( blur_sampler, TC2 );
	float4 Sample3 = tex2D( blur_sampler, TC3 );
	float4 Sample;
	Sample = 0.35f * Sample0;
	Sample += 0.535f * Sample1;
	Sample += 0.535f * Sample2;
	Sample += 0.35f * Sample3;
	return Sample;
}

technique Blur
{
    pass P0
    {
        // shaders
        VertexShader = compile vs_1_1 VS_Blur();
        PixelShader  = compile ps_1_1 PS_Blur();
        ZEnable = FALSE;
        ZWriteEnable = FALSE;
        AlphaBlendEnable = TRUE;
        SrcBlend = SRCALPHA;
        DestBlend = ONE;
    }  
}


//-----------------------------------------------------------------------------

void VS_Skybox(
    in  float3 Pos  : POSITION,
    out float4 oPos : POSITION,
    out float3 oTexCoords : TEXCOORD )
{
    matrix xform = matrix( matView[0],
						   matView[1],
						   matView[2],
						   float4( 0.f, 0.f, 0.f, 1.f )
						 );

    float4 P = mul(float4(Pos, 1), xform);    // position (view space)
    oPos  = mul(P, matProj);             // position (projected)
	oTexCoords = float3( Pos.x, Pos.y, Pos.z );
}

float4 PS_Skybox( in float3 TexCoords : TEXCOORD ) : COLOR
{
	return tex3D( skybox_sampler, TexCoords );
}

technique Skybox
{
    pass P0
    {
        // shaders
        VertexShader = compile vs_1_1 VS_Skybox();
        PixelShader  = compile ps_1_1 PS_Skybox();
        ZEnable = FALSE;
        ZWriteEnable = FALSE;
        AlphaBlendEnable = FALSE;
    }  
}

//-----------------------------------------------------------------------------
