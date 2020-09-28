const char* CSCONST_SHADER_TDR20K ="                \
struct BufType                                      \
{                                                   \
    int i;                                          \
    float f;                                        \
};                                                  \
                                                    \
StructuredBuffer<BufType> Buffer0 : register(t0);   \
StructuredBuffer<BufType> Buffer1 : register(t1);   \
RWStructuredBuffer<BufType> BufferOut : register(u0);\
                                                        \
[numthreads(1, 1, 1)]                                   \
void CSMain( uint3 DTid : SV_DispatchThreadID )         \
{                                                       \
    for(int c = 0; c < 20000; c++)                       \
    {                                                   \
        BufferOut[DTid.x].i = Buffer0[DTid.x].i + Buffer1[DTid.x].i;    \
        BufferOut[DTid.x].f = Buffer0[DTid.x].f + Buffer1[DTid.x].f;    \
    }                                                                   \
}";

const char* CSCONST_SHADER_TDR_LITE ="              \
struct BufType                                      \
{                                                   \
    int i;                                          \
    float f;                                        \
};                                                  \
                                                    \
StructuredBuffer<BufType> Buffer0 : register(t0);   \
StructuredBuffer<BufType> Buffer1 : register(t1);   \
RWStructuredBuffer<BufType> BufferOut : register(u0);\
                                                        \
[numthreads(1, 1, 1)]                                   \
void CSMain( uint3 DTid : SV_DispatchThreadID )         \
{                                                       \
    for(int c = 0; c < 20; c++)                         \
    {                                                   \
        BufferOut[DTid.x].i = Buffer0[DTid.x].i + Buffer1[DTid.x].i;    \
        BufferOut[DTid.x].f = Buffer0[DTid.x].f + Buffer1[DTid.x].f;    \
    }                                                                   \
}";

// This string has a %d in the loop location that must be set before it can be used.
char* SHADER_TDR ="  \
struct BufType       \
{                    \
    int i;           \
    float f;         \
};                   \
                     \
StructuredBuffer<BufType> Buffer0 : register(t0);    \
StructuredBuffer<BufType> Buffer1 : register(t1);    \
RWStructuredBuffer<BufType> BufferOut : register(u0);\
                                                     \
[numthreads(1, 1, 1)]                                \
void CSMain( uint3 DTid : SV_DispatchThreadID )      \
{                                                    \
    for(int c = 0; c < %d; c++)                      \
    {                                                \
        BufferOut[DTid.x].i = Buffer0[DTid.x].i + Buffer1[DTid.x].i;    \
        BufferOut[DTid.x].f = Buffer0[DTid.x].f + Buffer1[DTid.x].f;    \
    }                                                                   \
}";

const char* CSCONST_SHADER ="                       \
struct BufType                                      \
{                                                   \
    int i;                                          \
    float f;                                        \
};                                                  \
                                                    \
StructuredBuffer<BufType> Buffer0 : register(t0);   \
StructuredBuffer<BufType> Buffer1 : register(t1);   \
RWStructuredBuffer<BufType> BufferOut : register(u0);\
                                                        \
[numthreads(1, 1, 1)]                                   \
void CSMain( uint3 DTid : SV_DispatchThreadID )         \
{                                                       \
        BufferOut[DTid.x].i = Buffer0[DTid.x].i + Buffer1[DTid.x].i;    \
        BufferOut[DTid.x].f = Buffer0[DTid.x].f + Buffer1[DTid.x].f;    \
}";

const char* CSCONST_SHADER_DX11 ="                       \
RWTexture2D<float4> Output; \
void Plot(int x, int y) \
{\
   Output[uint2(x, y)] = float4(0, 0, 1, 1);\
}\
void DrawLine(int2 start, int2 end)\
{\
     bool steep = abs(end.y - start.y) > abs(end.x - start.x);\
     if (steep)\
     {\
         int temp = start.x;\
         start.x = start.y;\
         start.y = temp;\
         temp = end.x;\
         end.x = end.y;\
         end.y = temp;\
     }\
     if (start.x > end.x)\
     {\
         int temp = start.x;\
         start.x = end.x;\
         end.x = temp;\
         temp = start.y;\
         start.y = end.y;\
         end.y = temp;\
     }\
     int deltax = end.x - start.x;\
     int deltay = abs(end.y - start.y);\
     float error = 0;\
     float deltaerr = deltay / deltax;\
     int ystep;\
     int y = start.y;\
     if (start.y < end.y)\
        ystep = 1; \
     else \
        ystep = -1;\
     for (int x = start.x; x < end.x; x++)\
     {\
         if (steep)\
            Plot(y, x);\
         else \
            Plot(x, y);\
         error = error + deltaerr;\
         if (error >= 0.5)\
         {\
             y = y + ystep;\
             error = error - 1.0;\
         }\
    }\
}  \
void DrawCircle(int x0, int y0, int radius)\
{\
  int f = 1 - radius;\
  int ddF_x = 1;\
  int ddF_y = -2 * radius;\
  int x = 0;\
  int y = radius;\
  Plot(x0, y0 + radius);\
  Plot(x0, y0 - radius);\
  Plot(x0 + radius, y0);\
  Plot(x0 - radius, y0);\
  while(x < y)\
  {\
    if(f >= 0) \
    {\
      y--;\
      ddF_y += 2;\
      f += ddF_y;\
    }\
    x++;\
    ddF_x += 2;\
    f += ddF_x; \
    Plot(x0 + x, y0 + y);\
    Plot(x0 - x, y0 + y);\
    Plot(x0 + x, y0 - y);\
    Plot(x0 - x, y0 - y);\
    Plot(x0 + y, y0 + x);\
    Plot(x0 - y, y0 + x);\
    Plot(x0 + y, y0 - x);\
    Plot(x0 - y, y0 - x);\
  }\
}\
void DDA(float2 start, float2 end)\
{\
    float dydx = (end.y - start.y) / (end.x - start.x);\
    float y = start.y;\
    for (int x = start.x; x <= end.x; x++) \
    {\
        Plot(x, round(y));\
        y = y + dydx;\
    }\
}\
[numthreads(1, 1, 1)]\
void main( uint3 threadID : SV_DispatchThreadID )\
{\
    Output[threadID.xy] = float4(threadID.xy / 1024.0f, 0, 1);\
    if (threadID.x == 1023 && threadID.y == 1023)\
    {\
		    DDA(float2(0, 0), float2(1024, 1024));\
		    DDA(float2(0, 1023), float2(1023, 0));\
      DrawCircle(512, 512, 250);\
		    DrawCircle(0, 512, 250);\
    }\
}";


const char* PSCONST_SHADER ="                    \
sampler TextureOne : register(s0);               \
sampler TextureTwo : register(s1);               \
sampler TextureThree : register(s2);             \
sampler TextureFour : register(s3);              \
                                                 \
                                                 \
struct VS_OUTPUT                                 \
{                                                \
    float4   vDiffuse : COLOR0;                  \
    float2   vTex0 : TEXCOORD0;                  \
    float2   vTex1 : TEXCOORD1;                  \
    float2   vTex2 : TEXCOORD2;                  \
    float2   vTex3 : TEXCOORD3;                  \
};                                               \
                                                 \
float4 CSMain(VS_OUTPUT v) : COLOR                 \
{                                                \
    float4 tex1 = 0;                             \
    float4 tex2 = 0;                             \
    float4 tex3 = 0;                             \
    float4 tex4 = 0;                             \
    int i = 0;                                   \
    float2 offset = 0 ;                          \
    for ( i = 27 ; i < ( 27 + %d ); i++ )        \
    {                                            \
        offset = float(i * 17 % 13) / 0.023;     \
        tex1 += tex2D(TextureOne, (v.vTex0 + offset )); \
        tex2 += tex2D(TextureTwo, (v.vTex1 + offset));   \
        tex3 += tex2D(TextureThree, (v.vTex2 + offset)); \
        tex4 += tex2D(TextureFour, (v.vTex3 + offset));  \
    }                                             \
    return tex1 + tex2 + tex3 + tex4;             \
}";
