matrix g_WorldViewProj;

void Transform(
   in float4 inPos     : POSITION,
   in float4 inColor   : COLOR0,
   out float4 outPos   : SV_POSITION,
   out float4 outColor : COLOR0
)
{
   //Position transform
   outPos = mul(inPos,g_WorldViewProj);
   
   //Pass tex coord through
   outColor = inColor;
}

void Color(
   in float4 pos       : SV_POSITION,
   in float4 inColor   : COLOR0,
   out float4 outColor : SV_TARGET
)
{
   outColor = inColor;
}

technique10 TransformColor
{
    pass P0
    {
        SetVertexShader( CompileShader( vs_4_0_level_9_1, Transform() ) );
        SetPixelShader( CompileShader( ps_4_0_level_9_1, Color() ) );
    }
}