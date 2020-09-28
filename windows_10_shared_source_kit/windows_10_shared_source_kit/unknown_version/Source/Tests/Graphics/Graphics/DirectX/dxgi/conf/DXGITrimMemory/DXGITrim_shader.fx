matrix g_WorldViewProj;
Texture2D  g_Texture;
SamplerState g_Sampler
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

void Transform(
   in float4 inPos   : POSITION,
   in float2 inTex   : TEXCOORD0,
   out float4 outPos : SV_POSITION,
   out float2 outTex : TEXCOORD0
) {
   //Position transform
   outPos = mul(inPos,g_WorldViewProj);
   
   //Pass tex coord through
   outTex = inTex;
}

void Color(
   in float4 pos     : SV_POSITION,
   in float2 tex     : TEXCOORD0,
   out float4 color  : SV_TARGET
) {
   color = g_Texture.Sample(g_Sampler,tex);
}

technique10 TransformColor
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0_level_9_1, Transform() ) );
        SetPixelShader( CompileShader( ps_4_0_level_9_1, Color() ) );
    }
}