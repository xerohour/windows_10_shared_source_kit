//
// Starburst Shader
// Copyright (c) Microsoft Corporation. All rights reserved.
//

// transformations
float4x4 Projection;

float3 MidPoint = {0.5f, 0.5f, 0.0f};
float3 Ray1 = {0.0f, 1.0f, 0.0f};
float3 Ray2 = {1.0f, 0.0f, 0.0f};
float3 Ray3 = {1.0f, 1.0f, 0.0f};
float3 Ray4 = {-1.0f, 1.0f, 0.0f};

float3 Ray5 = {0.0f, 1.0f, 0.0f};
float3 Ray6 = {1.0f, 0.0f, 0.0f};

float  Angle1 = 0.2f;
float  Angle2 = 0.3f;
float  Angle3 = 0.1f;
float  Time;

float4 FlareColor = {0.6f, 0.4f, 0.0f, 1.0f};

struct VS_OUTPUT
{
    float4 Pos  : POSITION;
    float2 Tex1 : TexCoord0;
    float2 Tex2 : TexCoord1;
    float2 Tex3 : TexCoord2;
};

VS_OUTPUT VS(    
    float3 Pos  : POSITION)
{
    VS_OUTPUT Out = (VS_OUTPUT)0;

    //float3 P = mul(float4(Pos, 1), (float4x3)WorldView);    // position (view space)

    Out.Pos  = mul(float4(Pos, 1), Projection);             // position (projected)

    float3 Coord;
    float3 Coord2;
    float  sangle = sin(Angle1*Time);
    float  cangle = cos(Angle1*Time);

    Coord2.x = (Pos.x / 10);
    Coord2.y = (Pos.y / 10);
    Coord.z = 0.0;

    Coord.x = ((cangle * Coord2.x) + ((-sangle) * Coord2.y));
    Coord.y = ((sangle * Coord2.x) + (cangle * Coord2.y));

    Out.Tex1.x = Coord.x + 0.5;
    Out.Tex1.y = Coord.y + 0.5;

    sangle = sin(-Angle2 * Time);
    cangle = cos(-Angle2 * Time);

    Coord2.x = (Pos.x / 10);
    Coord2.y = (Pos.y / 10);
    Coord.z = 0.0;

    Coord.x = ((cangle * Coord2.x) + ((-sangle) * Coord2.y));
    Coord.y = ((sangle * Coord2.x) + (cangle * Coord2.y));

    Out.Tex2.x = Coord.x + 0.5;
    Out.Tex2.y = Coord.y + 0.5;

    sangle = sin(Angle3 * Time);
    cangle = cos(Angle3 * Time);

    Coord2.x = (Pos.x / 10);
    Coord2.y = (Pos.y / 10);
    Coord.z = 0.0;

    Coord.x = ((cangle * Coord2.x) + ((-sangle) * Coord2.y));
    Coord.y = ((sangle * Coord2.x) + (cangle * Coord2.y));

    Out.Tex3.x = Coord.x + 0.5;
    Out.Tex3.y = Coord.y + 0.5;

    return Out;
}

// This technique uses abs, dot, max, lerp, and 3 texture
// coordinates without any textures.

float4 PS20(VS_OUTPUT In) : COLOR
{   
    float4 Color;
    float3 Coord;
    float3 Coord2;
    float3 Coord3;

    Coord.x = In.Tex1.x - 0.5;
    Coord.y = In.Tex1.y - 0.5;
    Coord.z = 0.0;

    Coord2.x = In.Tex2.x - 0.5;
    Coord2.y = In.Tex2.y - 0.5;
    Coord2.z = 0.0;

    Coord3.x = In.Tex3.x - 0.5;
    Coord3.y = In.Tex3.y - 0.5;
    Coord3.z = 0.0;

    float len = 1.0 - length(In.Tex1 - MidPoint);

    // Apply initial burst

    Color.rgb = lerp(0.0, 4.0, 1.0 * pow(len,6) - 0.05);

    float PrimaryIntens = lerp(0.0, 1.0, len - 0.4);
    float SecondaryIntens = lerp(0.0, 1.0, len - 0.7);
    float tSecondaryIntens = lerp(0.0, 1.0, len - 0.6);

    // Apply primary and secondary rays

    Color.rgb += (max(0.0,1.0 - abs(dot(Ray1,Coord)) * 64) * PrimaryIntens);
    Color.rgb += (max(0.0,1.0 - abs(dot(Ray2,Coord)) * 64)  * PrimaryIntens);

    Color.rgb += (max(0.0,1.0 - abs(dot(Ray3,Coord3)) * 64)  * SecondaryIntens);
    Color.rgb += (max(0.0,1.0 - abs(dot(Ray4,Coord3)) * 64)  * SecondaryIntens);

    Color.rgb += (max(0.0,1.0 - abs(dot(Ray5,Coord2)) * 64)  * tSecondaryIntens);
    Color.rgb += (max(0.0,1.0 - abs(dot(Ray6,Coord2)) * 64)  * tSecondaryIntens);

    Color.rgb *= FlareColor;

    Color.w   = 1;

    return Color;
}  

technique StarBurst
{
    pass P0
    {
        VertexShader = compile vs_1_1 VS();
        PixelShader  = compile ps_2_0 PS20();
    }
}
