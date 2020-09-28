//
// Landscape Shader
// Copyright (c) Microsoft Corporation. All rights reserved.
//

// transformations
float4x4 Projection;

texture Tex0;
texture Tex1;
texture Tex2;
texture Tex3;
texture Tex4;
texture Tex5;

// light direction (view space)
float3 lightDir <  string UIDirectional = "Light Direction"; > = {0.577, -0.577, 0.577};

// light intensity
float4 I_a = { 0.3f, 0.3f, 0.3f, 1.0f };    // ambient
float4 I_d = { 1.0f, 1.0f, 1.0f, 1.0f };    // diffuse
float4 I_s = { 0.6f, 0.6f, 0.6f, 1.0f };    // specular

// Constants

float MaxAltitude = 55.0;
float GridSize = 200.0;
float HalfGridSize = 100.0;
float CompressAngle = 0.43;
float ColorMaxTexCoord = 1.0 - 2.0 * (1.0/64.0);
float MagnifyFinalColors = 2.5;
float CloudShadowSize = 0.1;
float DetailMultiplier = 32;
float DirtMultiplier = 4;
float RockMultiplier = 8;
float WaterMultiplier = 1.5;
float WaterColorIntensity = 1.8;
float4 WaterOffset;
float4 CloudOffset;

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float4 Diff : COLOR0;
    float4 Spec : COLOR1;
    float4 Tex0 : TEXCOORD0;               
};

VS_OUTPUT VS2(    
    float3 Pos  : POSITION,
    float3 Norm : NORMAL)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    float3 L = lightDir;
//    float3 P = mul(float4(Pos, 1), WorldView);    // position (view space)

    // Compress view normal vector into RGB diffuse value.

    Out.Diff.a = (Pos.z / MaxAltitude);
    Out.Diff.r = ((Norm.x * 0.5) + 0.5);
    Out.Diff.g = ((Norm.y * 0.5) + 0.5);
    Out.Diff.b = ((Norm.z * 0.5) + 0.5);

    // Compress light direction into RGB specular value

    Out.Spec.r = (L.x * 0.5) + 0.5;
    Out.Spec.g = (L.y * 0.5) + 0.5;
    Out.Spec.b = (L.z * 0.5) + 0.5;

    Out.Pos  = mul(float4(Pos, 1), Projection);  // position (projected)
    Out.Tex0.x = (Pos.x + HalfGridSize) / GridSize;
    Out.Tex0.y = (Pos.y + HalfGridSize) / GridSize;

    return Out;
}

// Samplers

sampler Sampler = sampler_state
{
    Texture   = (Tex0);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler Dirt = sampler_state
{
    Texture   = (Tex1);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler Dirt2 = sampler_state
{
    Texture   = (Tex3);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler RockMask = sampler_state
{
    Texture   = (Tex4);
    MipFilter = POINT;
    MinFilter = POINT;
};

sampler Water = sampler_state
{
    Texture   = (Tex5);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};

sampler Detail = sampler_state
{
    Texture   = (Tex2);
    MipFilter = LINEAR;
    MinFilter = LINEAR;
    MagFilter = LINEAR;
};


float4 LandscapeHLSLProc(VS_OUTPUT In) : COLOR
{   
    float4 Color;

    float3 N;
    float3 L;

    // Unpack normals

    N.x = ((In.Diff.r - 0.5) * 2.0);
    N.y = ((In.Diff.g - 0.5) * 2.0);
    N.z = ((In.Diff.b - 0.5) * 2.0);

    // Unpack light direction

    //L.x = ((In.Spec.r - 0.5) * 2.0);
    //L.y = ((In.Spec.g - 0.5) * 2.0);
    //L.z = ((In.Spec.b - 0.5) * 2.0);

    // Read the textures

    float2 ColorTex;
    float3 UpDir;

    UpDir.x = 0.0;
    UpDir.y = 0.0;
    UpDir.z = 1.0;

    float angle = dot(UpDir,N) * CompressAngle;

    ColorTex.y = In.Diff.a;
    ColorTex.x = angle;

    // Randomize the color texture

    ColorTex.x = max(0.0000,min(ColorMaxTexCoord ,ColorTex.x));
    ColorTex.y = max(0.0000,min(ColorMaxTexCoord ,ColorTex.y));

    float3 Mask = tex2D(RockMask,ColorTex);
    float3 SampleColor;

    SampleColor = tex2D(Dirt,In.Tex0 * RockMultiplier) * Mask;
    SampleColor += abs(1.0 - Mask);

    float DirtColor = tex2D(Dirt2,In.Tex0 * DirtMultiplier) * Mask;
    DirtColor += tex2D(Water,(In.Tex0 * WaterMultiplier) + WaterOffset)/WaterColorIntensity * abs(1.0 - Mask);

    SampleColor *= DirtColor;
    SampleColor *= tex2D(Detail,In.Tex0 * DetailMultiplier);
    SampleColor *= tex2D(Sampler,ColorTex) * MagnifyFinalColors;

    // Add some cloud shadow effects

    SampleColor *= tex2D(Water,(In.Tex0 * CloudShadowSize) + CloudOffset);

    //float Intens = max(0,dot(N,L));

    // Add lighting computation

    Color.rgb = 1.0;//I_d * Intens;

    Color.r = Color.r * SampleColor.r;
    Color.g = Color.g * SampleColor.g;
    Color.b = Color.b * SampleColor.b;

    Color.w   = 1;

    return Color;
}  

technique Landscape
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS2();

        Texture[0] = <Tex0>;
        Texture[1] = <Tex1>;
        Texture[2] = <Tex2>;
        Texture[3] = <Tex3>;
        Texture[4] = <Tex4>;
        Texture[5] = <Tex5>;
        MipFilter[0] = LINEAR;
        MipFilter[1] = LINEAR;
        MipFilter[2] = LINEAR;
        MipFilter[3] = LINEAR;
        MipFilter[4] = POINT;
        MipFilter[5] = LINEAR;
        MinFilter[0] = LINEAR;
        MinFilter[1] = LINEAR;
        MinFilter[2] = LINEAR;
        MinFilter[3] = LINEAR;
        MinFilter[4] = POINT;
        MinFilter[5] = LINEAR;
        MagFilter[0] = LINEAR;
        MagFilter[1] = LINEAR;
        MagFilter[2] = LINEAR;
        MagFilter[3] = LINEAR;
        MagFilter[4] = POINT;
        MagFilter[5] = LINEAR;

        PixelShader = 
        asm
        {
            ps_2_0

            def    c0, 0.0, 0.0, 1.0, 0.0
            def    c1, 0.5, 0.5, 0.5, 0.5
            def    c2, 2.0, 2.0, 2.0, 1.0
            def    c3, 0.01, 0.01, 0.0, 0.0
            def    c4, 1.0, 1.0, 1.0, 1.0
            def    c5, 8.0, 8.0, 0.0, 0.0
            def    c6, 4.0, 4.0, 0.0, 0.0
            def    c7, 1.5, 1.5, 1.5, 1.0
            def    c8, 0.43, 0.43, 0.43, 0.43
            def    c9, 0.93, 0.93, 0.93, 0.93
            def    c10, 0.3, 0.3, 0.0, 0.0
            dcl    v0
            dcl    v1
            dcl    t0
            dcl_2d s0
            dcl_2d s1
            dcl_2d s2
            dcl_2d s3
            dcl_2d s4
            dcl_2d s5
            
            // Preinitialize R2 and R5
            
            mov    r2,c0
            mov    r5,c0

            // Unpack normal into R0

            mov    r0,v0
            sub    r0,r0,c1
            mul    r0,r0,c2

            // Unpack light normal into R1            

            mov    r1,v1
            sub    r1,r1,c1
            mul    r1,r1,c2

            // determine angle value of the pixel
            // It goes in r2.w

            dp3    r2, c0, r0
            mul    r2, r2, c8

            // texture coordinates go in r2.x/r2.y

            mov    r2.y,v0.a
            mov    r2.x,r2.w            

            // r2.z is uninitialized. This should be valid for 2D textures.

            // Clamp the UV coordinates of r2

            min    r2, r2, c9
            max    r2, r2, c0

            // Read in the sample color into register r3

            texld   r3, r2, s0
            texld   r4, r2, s4    // Load land mask
            sub     r7, c4, r4

            mov     r5,t0
            mul     r5,r5,c5      // Rock multiplier
            
            // R5 contains z and w components, uninitialized.

            texld   r6, r5, s1    // Load Rock
            mul     r6,r6,r4      // Mask it off
            add     r6,r6,r7

            mov     r5,t0
            mul     r5,r5,c6      // Dirt multiplier
            
            // R5 contains Z and W components uninitialized.

            texld   r8, r5, s3    // Load dirt
            mul     r8, r8, r4
            add     r8, r8, r7

            mul     r6, r6, r8

            mov     r5,t0
            mul     r5,r5,c7      // Water multiplier
            add     r5,r5,c12
            
            // R5 contains Z and W components uninitialized.

            texld   r8, r5, s5    // Load water and apply it
            mul     r7, r7, c1
            mul     r8, r8, r7
            add     r8, r8, r4

            mul     r5,t0,c5     // Detail texture mag
            mul     r5, r5, c6

            // R5 Z and W still uninitialized.
            
            texld   r4,r5,s2      // Load up detail texture
            mul     r6,r6,r4

            mul     r6, r6, c2    // Magnify image
            mul     r6, r6, c2    // Magnify image
            mul     r6, r6, r8

            mul     r5,t0,c10     // water scale details
            add     r5,r5,c11
            
            // R5 contains Z and W uninitialized.

            texld   r4,r5,s5      // Load up water texture
            mul     r6,r6,r4

            dp3    r0, r0, r1    // Perform basic lighting
            mov    r0.x,r0.w
            mov    r0.y,r0.w
            mov    r0.z,r0.w

            max    r0, c1, r0     // Clamp lighting

            mul    r6,r6,r3        // Apply final coloring

            mul    r6, r6, r0
            
            mov    oC0,r6
        };
    }
}

technique LandscapeHLSL
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS2();

        PixelShader = compile ps_2_0 LandscapeHLSLProc();
    }
}
