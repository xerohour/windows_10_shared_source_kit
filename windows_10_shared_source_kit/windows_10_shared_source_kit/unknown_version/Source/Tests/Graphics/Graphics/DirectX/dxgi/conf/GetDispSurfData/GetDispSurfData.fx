//--------------------------------------------------------------------------------------
// File: GetDispSurfData.fx
//
// The effect file for the GetDispSurfData test app.  
// 
// Copyright (c) 2007 Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------


//--------------------------------------------------------------------------------------
// Vertex shader output structure
//--------------------------------------------------------------------------------------
struct VS_OUTPUT
{
    float4 Pos : SV_POSITION;
    float4 Color : COLOR0;
};

//--------------------------------------------------------------------------------------
// Vertex Shader 
//--------------------------------------------------------------------------------------
VS_OUTPUT  VS( float4 Pos : POSITION,  float4 Color : COLOR )
{
    VS_OUTPUT  output = (VS_OUTPUT)0;
    output.Pos = Pos;    
    output.Color = Color;

    return output;    
}

//--------------------------------------------------------------------------------------
// Pixel Shader 
//--------------------------------------------------------------------------------------
float4  PS( VS_OUTPUT input ) : SV_Target
{ 
    return input.Color;
}


//--------------------------------------------------------------------------------------
// Renders scene 
//--------------------------------------------------------------------------------------
technique10 Render
{
    pass P0
    {       
        SetVertexShader( CompileShader( vs_4_0, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0, PS() ) );
    }
}

technique10 Render_fl9
{
    pass P0
    {       
        SetVertexShader( CompileShader( vs_4_0_level_9_1, VS() ) );
        SetGeometryShader( NULL );
        SetPixelShader( CompileShader( ps_4_0_level_9_1, PS() ) );
    }
}
