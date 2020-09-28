// Pixel shader source for WGF11TiledResources

// Use this shader for Texture3D resources
#ifdef USE_TEXTURE3D
#   define UV_COORD_TYPE float3
#   define TEXTURE_TYPE  Texture3D<float4>
#   define RESOURCE_OFFSET int3(0,0,0)
#   define GET_LOAD_LOCATION(inLoc, numSlices) int4(inLoc.xy, inLoc.z*numSlices /*slice (integer)*/, 0 /*mip level 0*/)
#   define GET_SAMPLE_UV(inUV) UV_COORD_TYPE(inUV.xy, inUV.z /*slice (float)*/)
#   define MAX_TEXTURE_WH_DIM 128                       // maximum texture width & height = tileWidth (or tileHeight) * 4
#   define PS_INPUT_STRUCT GS_OUTPUT

// Use this shader for Texture2D resources
#else
#   define UV_COORD_TYPE float2
#   define TEXTURE_TYPE  Texture2D<float4>
#   define RESOURCE_OFFSET int2(0,0)
#   define GET_LOAD_LOCATION(inLoc, unused) int3(inLoc.xyz)
#   define GET_SAMPLE_UV(inUV) UV_COORD_TYPE(inUV)
#   define MAX_TEXTURE_WH_DIM 512                       // maximum texture width & height = tileWidth (or tileHeight) * 4
#   define PS_INPUT_STRUCT VS_OUTPUT
#endif

struct SETTINGS
{
    uint useClamp;
    uint useFeedback;
};

cbuffer mybuf : register(b0)
{
    SETTINGS settings;
}

struct VS_OUTPUT
{
    float4               position      : SV_POSITION;
    UV_COORD_TYPE        uv            : TEXCOORD0;
    nointerpolation uint RTSlice       : RenderTargetArrayIndex;
    nointerpolation uint numSlicesSRV  : NumSlicesSRV;
    nointerpolation uint numSlicesRTV  : NumSlicesRTV;
};

struct GS_OUTPUT
{
    float4               position      : SV_POSITION;
    UV_COORD_TYPE        uv            : TEXCOORD0;
    nointerpolation uint RTSlice       : SV_RenderTargetArrayIndex;
    nointerpolation uint numSlicesSRV  : NumSlicesSRV;
    nointerpolation uint numSlicesRTV  : NumSlicesRTV;
};

TEXTURE_TYPE sourceTexture;
SamplerState filterSampler;

float4 CopyTexTier1PS(float4 position : SV_POSITION) : SV_Target
{
    float4 color = float4(0, 0, 0, 0);
    uint feedback = 0;
    color = sourceTexture.Load(GET_LOAD_LOCATION(position, 0/*slice*/));
    return color;
}

[maxvertexcount(3)]
void CopyTexGSTex3D(triangle VS_OUTPUT input[3], inout TriangleStream<GS_OUTPUT> Stream)
{
    for (uint i = 0; i < 3; ++i)
    {
        GS_OUTPUT output;
        output.position     = input[i].position;
        output.uv           = input[i].uv;
        output.RTSlice      = input[i].position.z * input[i].numSlicesRTV;
        output.numSlicesRTV = input[i].numSlicesRTV;
        output.numSlicesSRV = input[i].numSlicesSRV;
        Stream.Append(output);
    }
    Stream.RestartStrip();
}

float4 CopyTexTier2PS(PS_INPUT_STRUCT input) : SV_Target
{
    float4 color = float4(0, 0, 0, 0);
    uint feedback = 0;
    color = sourceTexture.Load(GET_LOAD_LOCATION(input.position, input.numSlicesSRV), RESOURCE_OFFSET, feedback);
    if (CheckAccessFullyMapped(feedback))
    {
        // feedback indicates tile is resident
        if (color.a == 0)
        {
            // Alpha should be >0 for resident reads.  Set color to red to indicate failure
            color.g = 1;
        }
    }
    else
    {
        // feedback indicates non-resident
        if (color.r != 0 && color.g != 0 && color.b != 0 && color.a != 0)
        {
            // Make output color red if the feedback value was correctly set for non-resident reads
            color.r = 1;
        }
    }
    return color;
}

// Can't use Texture3D for depth views
#ifndef USE_TEXTURE3D
float RenderDepthPS(VS_OUTPUT input) : SV_Depth
{
    float depth;
    float a = 3.14159 * (input.position.x + input.position.y) / 100.0;
    float b = 3.14159 * (input.position.x - input.position.y) / 100.0;
    depth = (sin(a) + 1.0) / 4.0;
    depth = depth + (cos(b) + 1.0) / 4.0;
    return depth;
}

struct COLOR_USING_DEPTH_OUT
{
    float4 color	: SV_Target;
    float depth : SV_Depth;
};

COLOR_USING_DEPTH_OUT RenderColorUsingDepthPS(VS_OUTPUT input)
{
    COLOR_USING_DEPTH_OUT output;
    float depth;
    int x = uint(input.position.x) % 512;
    int y = uint(input.position.y) % 512;
    output.color = sourceTexture.Load(int3(x, y, 0));
    float a = 3.14159 * (input.position.x + input.position.y) / 50.0;
    float b = 3.14159 * (input.position.x - input.position.y) / 50.0;
    depth = (sin(a) + 1.0) / 4.0;
    depth = depth + (cos(b) + 1.0) / 4.0;
    output.depth = depth;
    return output;
}
#endif

float4 FilterTiledTier1PS(PS_INPUT_STRUCT input) : SV_Target
{
    float4 color = float4(0, 0, 0, 0);
    color = sourceTexture.Sample(filterSampler, input.uv);
    return color;
}

float4 FilterNormalTier1PS(PS_INPUT_STRUCT input) : SV_Target
{
    float4 color = float4(0, 0, 0, 0);
    color = sourceTexture.Sample(filterSampler, input.uv);
    return color;
}

float4 FilterTiledTier2PS(PS_INPUT_STRUCT input) : SV_Target
{
    uint feedback;
    float4 color = float4(0, 0, 0, 0);
        float clamp = 0;

    if (settings.useClamp)
    {
        // Calculate clamp value based on groovy sin wave
        int x = uint(input.position.x) % MAX_TEXTURE_WH_DIM;
        int y = uint(input.position.y) % MAX_TEXTURE_WH_DIM;
        float a = 3.14159 * (input.position.x + input.position.y) / 100.0;
        float b = 3.14159 * (input.position.x - input.position.y) / 100.0;
        clamp = (sin(a) + 1.0) / 4.0;
        clamp = clamp + (cos(b) + 1.0) / 4.0;

        // Ensure clamp is never outside the bounds of [0.0, 1.0] due to unexpected floating point error
        if (clamp > 1.0)
            clamp = 1.0;
        if (clamp < 0.0)
            clamp = 0.0;
    }

    if (settings.useFeedback)
    {
        color = sourceTexture.Sample(filterSampler, GET_SAMPLE_UV(input.uv), 0, clamp, feedback);

        if (CheckAccessFullyMapped(feedback))
        {
            // feedback indicates tile is resident
            if (color.a != 1)
            {
                // Alpha should be 1 for fully resident reads.  Set color to red to indicate failure
                color.r = 1;
            }
        }
        else
        {
            // feedback indicates at least one read was non-resident
            if (color.a == 1)
            {
                // Make output color red if the feedback value was incorrectly set for non-resident reads
                color.r = 1;
            }
        }
    }
    else
    {
        color = sourceTexture.Sample(filterSampler, GET_SAMPLE_UV(input.uv), 0, clamp);
    }

    if (color.a < 1 && color.a > 0 && color.r != 1)
    {
        // Alpha values between 0 and 1 exclusive indicate partialy resident reads.
        // In order to help identify pixels with partial residency invert
        // the blue and green channels
        color.b = 1.0 - color.b;
        color.g = 1.0 - color.g;
    }

    return color;
}

float4 FilterNormalTier2PS(PS_INPUT_STRUCT input) : SV_Target
{
    float clamp = 0;
    float4 color = float4(0, 0, 0, 0);

        if (settings.useClamp)
        {
            // Calculate clamp value based on groovy sin wave
            int x = uint(input.position.x) % MAX_TEXTURE_WH_DIM;
            int y = uint(input.position.y) % MAX_TEXTURE_WH_DIM;
            float a = 3.14159 * (input.position.x + input.position.y) / 100.0;
            float b = 3.14159 * (input.position.x - input.position.y) / 100.0;
            clamp = (sin(a) + 1.0) / 4.0;
            clamp = clamp + (cos(b) + 1.0) / 4.0;

            // Ensure clamp is never outside the bounds of [0.0, 1.0] due to unexpected floating point error
            if (clamp > 1.0)
                clamp = 1.0;
            if (clamp < 0.0)
                clamp = 0.0;

            // Emulate the clamp by first computing LOD then implementing the clamp and using Sample
            float lod = sourceTexture.CalculateLevelOfDetail(filterSampler, input.uv);
            if (lod < clamp)
                lod = clamp;

            color = sourceTexture.SampleLevel(filterSampler, GET_SAMPLE_UV(input.uv), lod);
        }
        else
        {
            color = sourceTexture.Sample(filterSampler, GET_SAMPLE_UV(input.uv));
        }

    if (color.a < 1 && color.a > 0 && color.r != 1)
    {
        // Alpha values between 0 and 1 exclusive indicate partialy resident reads.
        // In order to help identify pixels with partial residency invert
        // the blue and green channels
        color.b = 1.0 - color.b;
        color.g = 1.0 - color.g;
    }

    return color;
}

#ifndef USE_TEXTURE3D

float4 GatherAllWithFeedback(Texture2D<float4> t, SamplerState s, float2 location, uint2 offset, out uint feedback)
{
    float4 color = float4(0, 0, 0, 0);

        feedback = 0;

    uint feedbackR;
    uint feedbackG;
    uint feedbackB;
    uint feedbackA;

    float4 r = sourceTexture.GatherRed(filterSampler, location, offset, feedbackR);
    float4 g = sourceTexture.GatherGreen(filterSampler, location, offset, feedbackG);
    float4 b = sourceTexture.GatherBlue(filterSampler, location, offset, feedbackB);
    float4 a = sourceTexture.GatherAlpha(filterSampler, location, offset, feedbackA);

    bool haveR = CheckAccessFullyMapped(feedbackR);
    bool haveG = CheckAccessFullyMapped(feedbackG);
    bool haveB = CheckAccessFullyMapped(feedbackB);
    bool haveA = CheckAccessFullyMapped(feedbackA);

    if (haveR != haveG || haveR != haveB || haveR != haveA)
    {
        // This isn't good since the residency feedback is expected to be the same for each Gather call.
        // Just draw red.
        color.r = 1;
        color.g = 0;
        color.b = 0;
        color.a = 0;
    }
    else
    {
        // Just blend all gathered values equally
        float4 scale = float4(.25, .25, .25, .25);
            color.r = dot(scale, r);
        color.g = dot(scale, g);
        color.b = dot(scale, b);
        color.a = dot(scale, a);
    }

    feedback = feedbackR;

    return color;
}

float4 GatherAll(in Texture2D<float4> t, in SamplerState s, in float2 location, in uint2 offset)
{
    float4 color = float4(0, 0, 0, 0);

        float4 r = sourceTexture.GatherRed(filterSampler, location, offset);
        float4 g = sourceTexture.GatherGreen(filterSampler, location, offset);
        float4 b = sourceTexture.GatherBlue(filterSampler, location, offset);
        float4 a = sourceTexture.GatherAlpha(filterSampler, location, offset);

        // Just blend all gathered values equally
        float4 scale = float4(.25, .25, .25, .25);
        color.r = dot(scale, r);
    color.g = dot(scale, g);
    color.b = dot(scale, b);
    color.a = dot(scale, a);

    return color;
}

float4 GatherTiledTier2PS(VS_OUTPUT input) : SV_Target
{
    uint feedbackR;
    uint feedbackG;
    uint feedbackB;
    uint feedbackA;
    float4 color = float4(0, 0, 0, 0);

        if (settings.useFeedback)
        {
        uint feedback = 0;

        color = GatherAllWithFeedback(sourceTexture, filterSampler, input.uv, int2(0, 0), feedback);

        if (CheckAccessFullyMapped(feedback))
        {
            // See if data was actually read
            if (color.a == 0)
            {
                // Alpha should be 1 for resident reads.  Set color to red to indicate failure
                color.r = 1;
                color.g = 0;
                color.b = 0;
            }
        }
        else
        {
            // feedback indicates non-resident
            if (color.r == 0 && color.g == 0 && color.b == 0 && color.a == 0)
            {
                // Make output color green if the feedback value was correctly set for non-resident reads
                color.g = 0;
            }
        }
        }
        else
        {
            color = GatherAll(sourceTexture, filterSampler, input.uv, 0);
        }

    if (color.a < 1 && color.a > 0 && color.r != 1)
    {
        // Alpha values between 0 and 1 exclusive indicate partialy resident reads.
        // In order to help identify pixels with partial residency invert
        // the blue and green channels
        color.b = 1.0 - color.b;
        color.g = 1.0 - color.g;
    }

    return color;
}

float4 GatherNormalTier2PS(VS_OUTPUT input) : SV_Target
{
    float4 color = float4(0, 0, 0, 0);

    color = GatherAll(sourceTexture, filterSampler, input.uv, 0);

    if (color.a < 1 && color.a > 0 && color.r != 1)
    {
        // Alpha values between 0 and 1 exclusive indicate partialy resident reads.
        // In order to help identify pixels with partial residency invert
        // the blue and green channels
        color.b = 1.0 - color.b;
        color.g = 1.0 - color.g;
    }

    return color;
}

float4 GatherTiledTier1PS(VS_OUTPUT input) : SV_Target
{
    float4 color = float4(0, 0, 0, 0);

    color = GatherAll(sourceTexture, filterSampler, input.uv, 0);

    return color;
}

float4 GatherNormalTier1PS(VS_OUTPUT input) : SV_Target
{
    float4 color = float4(0, 0, 0, 0);

    color = GatherAll(sourceTexture, filterSampler, input.uv, 0);

    return color;
}

#endif
