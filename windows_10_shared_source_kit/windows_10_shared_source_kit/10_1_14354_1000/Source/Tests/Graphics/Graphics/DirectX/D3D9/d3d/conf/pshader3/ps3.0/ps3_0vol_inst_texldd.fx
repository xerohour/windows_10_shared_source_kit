#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texldd : r0, VolTex5, dsx(c2)=(0.09192, 0.09192, 0, 0), dsy(c2)=(0.09192, 0.09192, 0, 0) -> level = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.09192, 0.09192, 0.0, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_volume s5
			
			mov r0, v0
			texldd r0, r0, s5, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, VolTex5(SliceX), dsx(c2)=(0, 0.09192, 0.18384, 0), dsy(c2)=(0, 0.09192, 0.18384, 0) -> level = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 0.09192, 0.18384, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_volume s5
			
			mov r0, v0
			texldd r0, r0, s5, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, VolTex5, dsx(r0)=(0, 0, 0.6, 1), dsy(r1)=(0.3, 0, 0, 1) -> level = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.3, 0.0, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.6, 1.0
            dcl_volume s5
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			mov r1, c2
			texldd r0, v0, s5, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, VolTex5(SliceY), dsx(r0)=(0.3, 0, 0, 1), dsy(r1)=(0, 0.3, 0, 1) -> level = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.3, 0.0, 0.0, 1.0
            dcl_volume s5
			
			mov r0, c2
			mov r1, c2
			mov r1.x, c2.y
			mov r1.y, c2.x
			texldd r0, v0, s5, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, VolTex5, dsx=v0, dsy(r1)=(0.7778, 0.7778, 0, 1) -> level = NA(5)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.7778, 0.7778, 0.0, 1.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s5
			
			mov r1, c2
			mov r0, v0
			texldd r0, r0, s5, v1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, VolTex5(SliceY), dsx=v0, dsy(r1)=(0.7778, 0, 1.5556, 0) -> level = NA(5)";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.7778, 0.0, 1.5556, 0.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s5
			
			mov r1, c2
			mov r0, v0
			texldd r0, r0, s5, v1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, VolTex5, dsx=v0, dsy(c2)=(0, 0.04, 0, 1) -> level = NA(0)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 0.04, 0.0, 1.0
            dcl_volume s5
			
			texldd r0, c2, s5, v0, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, VolTex5(SliceX), dsx=v0, dsy(c2)=(0, 0.0, 0.08, 1) -> level = NA(0)";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 0.0, 0.08, 1.0
            dcl_volume s5
			
			texldd r0, c2, s5, v0, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, VolTex5, dsx(r1)=(0, 0.232535, 0.46507, 1), dsy=v1 -> level = NA(1)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.232535, 0.46507, 1.0	// 0.232535=.07/sqrt(2)
            dcl_volume s5
			
			mov r0, v0
			mov r1, c2
			texldd r2, r0, s5, r1, v1
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, VolTex5, dsx(r0)=(0.13, 0, 0, 1), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 0.13, 0.0, 0.0
            dcl_volume s5
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			texldd r2, v0, s5, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, VolTex5(SliceX), dsx(r0)=(0, 0, 0.26, 0), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 0.13, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.26, 0.0
            dcl_volume s5
			
			mov r0, c3
			texldd r2, v0, s5, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, VolTex5, dsx(c2)=(0, 0.3, 0, 1), dsy=v0 -> level = NA(3)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 0.3, 0.0, 1.0
            dcl_volume s5
			
			texldd r0, c2, s5, c2, v0
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : r0, VolTex5, dsx=(1.1, 0, 0, 1), dsy=(1.1, 0, 0, 1) -> level = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 1.1, 0.0, 0.0, 1.0
            dcl_volume s5
			
			mov r0, v0
			texldd_pp r0, r0, s5, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : r0, VolTex5(SliceY), dsx=(0, 0, 2.2, 1), dsy=(0, 0, 2.2, 1) -> level = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c2, 0.0, 0.0, 2.2, 1.0
            dcl_volume s5
			
			mov r0, v0
			texldd_pp r0, r0, s5, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=0.5 => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0.f, 1.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/w+1/4w, dsy=0/h+1/4h; repeat=0.5 => lev=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 0.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=0.5 => lev=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0.f, 1.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=0.5 => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 1.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/w+1/4w, dsy=2/h+1/4h; repeat=0.5 => lev=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 2.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=0.5 => lev=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 1.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=0.5 => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 4.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=4/w+1/4w, dsy=2/h+1/4h; repeat=0.5 => lev=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 2.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=0.5 => lev=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 4.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=0.5 => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 4.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=4/w+1/4w, dsy=8/h+1/4h; repeat=0.5 => lev=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 8.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=0.5 => lev=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 4.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=0.5 => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 16.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=16/w+1/4w, dsy=8/h+1/4h; repeat=0.5 => lev=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 8.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=0.5 => lev=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 16.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=0.5 => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 16.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=16/w+1/4w, dsy=32/h+1/4h; repeat=0.5 => lev=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 32.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=0.5 => lev=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 16.f, 0.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 1;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 2;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 3;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 4;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[5] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
		MipFilter[5] = Point;
        MaxMipLevel[5] = 5;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[5] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol5(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[5] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[5] = <TexVol_0>;
 		MipFilter[5] = Point;
        MaxMipLevel[5] = 0;
        MipMapLODBias[5] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s5
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s5, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r1, VolTex10, dsx(c2)=(0.09192, 0.09192, 0, 0), dsy(c2)=(0.09192, 0.09192, 0, 0) -> level = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.09192, 0.09192, 0.0, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_volume s10
			
			mov r1, v1
			texldd r0, r1, s10, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r1, VolTex10(SliceX), dsx(c2)=(0, 0.09192, 0.18384, 0), dsy(c2)=(0, 0.09192, 0.18384, 0) -> level = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.09192, 0.18384, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_volume s10
			
			mov r1, v1
			texldd r0, r1, s10, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v1, VolTex10, dsx(r0)=(0, 0, 0.6, 1), dsy(r1)=(0.3, 0, 0, 1) -> level = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.3, 0.0, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.6, 1.0
            dcl_volume s10
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			mov r1, c2
			texldd r0, v1, s10, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v1, VolTex10(SliceY), dsx(r0)=(0.3, 0, 0, 1), dsy(r1)=(0, 0.3, 0, 1) -> level = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.3, 0.0, 0.0, 1.0
            dcl_volume s10
			
			mov r0, c2
			mov r1, c2
			mov r1.x, c2.y
			mov r1.y, c2.x
			texldd r0, v1, s10, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, VolTex10, dsx=v1, dsy(r1)=(0.7778, 0.7778, 0, 1) -> level = NA(5)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.7778, 0.7778, 0.0, 1.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s10
			
			mov r1, c2
			mov r0, v0
			texldd r0, r0, s10, v1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, VolTex10(SliceY), dsx=v1, dsy(r1)=(0.7778, 0, 1.5556, 0) -> level = NA(5)";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.7778, 0.0, 1.5556, 0.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s10
			
			mov r1, c2
			mov r0, v0
			texldd r0, r0, s10, v1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, VolTex10, dsx=v1, dsy(c2)=(0, 0.04, 0, 1) -> level = NA(0)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.04, 0.0, 1.0
            dcl_volume s10
			
			texldd r0, c2, s10, v1, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, VolTex10(SliceX), dsx=v1, dsy(c2)=(0, 0.0, 0.08, 1) -> level = NA(0)";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.0, 0.08, 1.0
            dcl_volume s10
			
			texldd r0, c2, s10, v1, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, VolTex10, dsx(r1)=(0, 0.232535, 0.46507, 1), dsy=v1 -> level = NA(1)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.232535, 0.46507, 1.0	// 0.232535=.07/sqrt(2)
            dcl_volume s10
			
			mov r0, v0
			mov r1, c2
			texldd r2, r0, s10, r1, v1
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v1, VolTex10, dsx(r0)=(0.13, 0, 0, 1), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.13, 0.0, 0.0
            dcl_volume s10
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			texldd r2, v1, s10, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v1, VolTex10(SliceX), dsx(r0)=(0, 0, 0.26, 0), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.13, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.26, 0.0
            dcl_volume s10
			
			mov r0, c3
			texldd r2, v1, s10, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, VolTex10, dsx(c2)=(0, 0.3, 0, 1), dsy=v1 -> level = NA(3)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.3, 0.0, 1.0
            dcl_volume s10
			
			texldd r0, c2, s10, c2, v1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : r1, VolTex10, dsx=(1.1, 0, 0, 1), dsy=(1.1, 0, 0, 1) -> level = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 1.1, 0.0, 0.0, 1.0
            dcl_volume s10
			
			mov r1, v1
			texldd_pp r0, r1, s10, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : r1, VolTex10(SliceY), dsx=(0, 0, 2.2, 1), dsy=(0, 0, 2.2, 1) -> level = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.0, 2.2, 1.0
            dcl_volume s10
			
			mov r1, v1
			texldd_pp r0, r1, s10, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=1.5 => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0.f, 1.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/w+1/4w, dsy=0/h+1/4h; repeat=1.5 => lev=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 0.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=1.5 => lev=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0.f, 1.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=1.5 => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 1.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/w+1/4w, dsy=2/h+1/4h; repeat=1.5 => lev=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 2.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=1.5 => lev=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 1.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=1.5 => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 4.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=4/w+1/4w, dsy=2/h+1/4h; repeat=1.5 => lev=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 2.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=1.5 => lev=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 4.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=1.5 => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 4.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=4/w+1/4w, dsy=8/h+1/4h; repeat=1.5 => lev=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 8.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=1.5 => lev=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 4.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=1.5 => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 16.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=16/w+1/4w, dsy=8/h+1/4h; repeat=1.5 => lev=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 8.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=1.5 => lev=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 16.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=1.5 => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 16.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=16/w+1/4w, dsy=32/h+1/4h; repeat=1.5 => lev=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 32.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=1.5 => lev=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 16.f, 1.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 1;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 1;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 1;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 3;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 3;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 3;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 5;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 5;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 5;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 1;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 1;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 1;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 2;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 3;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 3;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 3;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 4;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 5;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 5;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[10] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
		MipFilter[10] = Point;
        MaxMipLevel[10] = 5;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[10] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol10(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[10] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[10] = <TexVol_0>;
 		MipFilter[10] = Point;
        MaxMipLevel[10] = 0;
        MipMapLODBias[10] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s10
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s10, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r2, VolTex15, dsx(c2)=(0.09192, 0.09192, 0, 0), dsy(c2)=(0.09192, 0.09192, 0, 0) -> level = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.09192, 0.09192, 0.0, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_volume s15
			
			mov r2, v2
			texldd r0, r2, s15, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r2, VolTex15(SliceX), dsx(c2)=(0, 0.09192, 0.18384, 0), dsy(c2)=(0, 0.09192, 0.18384, 0) -> level = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.0, 0.09192, 0.18384, 0.0		// 0.09192 = 0.13 / sqrt(2)
            dcl_volume s15
			
			mov r2, v2
			texldd r0, r2, s15, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v2, VolTex15, dsx(r0)=(0, 0, 0.6, 1), dsy(r1)=(0.3, 0, 0, 1) -> level = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.3, 0.0, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.6, 1.0
            dcl_volume s15
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			mov r1, c2
			texldd r0, v2, s15, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v2, VolTex15(SliceY), dsx(r0)=(0.3, 0, 0, 1), dsy(r1)=(0, 0.3, 0, 1) -> level = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.3, 0.0, 0.0, 1.0
            dcl_volume s15
			
			mov r0, c2
			mov r1, c2
			mov r1.x, c2.y
			mov r1.y, c2.x
			texldd r0, v2, s15, r0, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, VolTex15, dsx=v2, dsy(r1)=(0.7778, 0.7778, 0, 1) -> level = NA(5)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.7778, 0.7778, 0.0, 1.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s15
			
			mov r1, c2
			mov r0, v0
			texldd r0, r0, s15, v1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, VolTex15(SliceY), dsx=v2, dsy(r1)=(0.7778, 0, 1.5556, 0) -> level = NA(5)";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.7778, 0.0, 1.5556, 0.0	// 0.7778 = 1.1 / sqrt(2)
            dcl_volume s15
			
			mov r1, c2
			mov r0, v0
			texldd r0, r0, s15, v1, r1
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, VolTex15, dsx=v2, dsy(c2)=(0, 0.04, 0, 1) -> level = NA(0)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.0, 0.04, 0.0, 1.0
            dcl_volume s15
			
			texldd r0, c2, s15, v2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, VolTex15(SliceX), dsx=v2, dsy(c2)=(0, 0.0, 0.08, 1) -> level = NA(0)";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.0, 0.0, 0.08, 1.0
            dcl_volume s15
			
			texldd r0, c2, s15, v2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, VolTex15, dsx(r1)=(0, 0.232535, 0.46507, 1), dsy=v1 -> level = NA(1)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
        
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_texcoord1 v1
			def  c2, 0.0, 0.232535, 0.46507, 1.0	// 0.232535=.07/sqrt(2)
            dcl_volume s15
			
			mov r0, v0
			mov r1, c2
			texldd r2, r0, s15, r1, v1
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v2, VolTex15, dsx(r0)=(0.13, 0, 0, 1), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.0, 0.13, 0.0, 0.0
            dcl_volume s15
			
			mov r0, c2
			mov r0.x, c2.y
			mov r0.y, c2.x
			texldd r2, v2, s15, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v2, VolTex15(SliceX), dsx(r0)=(0, 0, 0.26, 0), dsy(c2)=(0, 0.13, 0, 1) -> level = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.0, 0.13, 0.0, 1.0
            def  c3, 0.0, 0.0, 0.26, 0.0
            dcl_volume s15
			
			mov r0, c3
			texldd r2, v2, s15, r0, c2
			mov oC0, r2
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : c2, VolTex15, dsx(c2)=(0, 0.3, 0, 1), dsy=v2 -> level = NA(3)";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.0, 0.3, 0.0, 1.0
            dcl_volume s15
			
			texldd r0, c2, s15, c2, v2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : r2, VolTex15, dsx=(1.1, 0, 0, 1), dsy=(1.1, 0, 0, 1) -> level = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 1.1, 0.0, 0.0, 1.0
            dcl_volume s15
			
			mov r2, v2
			texldd_pp r0, r2, s15, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd_pp : r2, VolTex15(SliceY), dsx=(0, 0, 2.2, 1), dsy=(0, 0, 2.2, 1) -> level = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def  c2, 0.0, 0.0, 2.2, 1.0
            dcl_volume s15
			
			mov r2, v2
			texldd_pp r0, r2, s15, c2, c2
			mov oC0, r0
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/w+1/4w, dsy=1/h+1/4h => level=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 1.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=2/w+1/4w, dsy=2/h+1/4h => level=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 2.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=4/w+1/4w, dsy=4/h+1/4h => level=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 4.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=8/w+1/4w, dsy=8/h+1/4h => level=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 8.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=16/w+1/4w, dsy=16/h+1/4h => level=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=32/w+1/4w, dsy=32/h+1/4h => level=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 32.f, 1.f, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=2.5 => level=0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0.f, 1.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/w+1/4w, dsy=0/h+1/4h; repeat=2.5 => lev=0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 0.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=0/w+1/4w, dsy=1/h+1/4h; repeat=2.5 => lev=0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0.f, 1.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=2.5 => level=1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 1.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/w+1/4w, dsy=2/h+1/4h; repeat=2.5 => lev=1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 1.f, 2.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=2/w+1/4w, dsy=1/h+1/4h; repeat=2.5 => lev=1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 1.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=2.5 => level=2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 4.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=4/w+1/4w, dsy=2/h+1/4h; repeat=2.5 => lev=2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 2.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=2/w+1/4w, dsy=4/h+1/4h; repeat=2.5 => lev=2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 2.f, 4.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=2.5 => level=3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 4.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=4/w+1/4w, dsy=8/h+1/4h; repeat=2.5 => lev=3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 4.f, 8.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=8/w+1/4w, dsy=4/h+1/4h; repeat=2.5 => lev=3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 4.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=2.5 => level=4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 16.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=16/w+1/4w, dsy=8/h+1/4h; repeat=2.5 => lev=4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 8.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=8/w+1/4w, dsy=16/h+1/4h; repeat=2.5 => lev=4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 8.f, 16.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=2.5 => level=5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 16.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=16/w+1/4w, dsy=32/h+1/4h; repeat=2.5 => lev=5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 16.f, 32.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=32/w+1/4w, dsy=16/h+1/4h; repeat=2.5 => lev=5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 32.f, 16.f, 2.5, 4.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mul r0.xy, r0.xy, c0.z
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=0, writemask=xyzw";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=1, writemask=xyzw";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=2, writemask=xyzw";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=3, writemask=xyzw";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=4, writemask=xyzw";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.z, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; level=5, writemask=xyzw";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.z, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4.xyzw, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 2;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 2;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 2;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 4;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 4;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 4;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0 
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 1;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 2;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 2;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 2;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 3;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 4;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 4;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 4;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MaxMipLevel[15] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
		MipFilter[15] = Point;
        MaxMipLevel[15] = 5;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1/2w, dsy=1/2h; MipMapLODBias[15] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = 5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 0, 0, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : v0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, v0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -0;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -1;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -2;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -3;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -4;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15, dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceX), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texldd : r0, TexVol15(SliceY), dsx=1+1/2w, dsy=1+1/2h; MipMapLODBias[15] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[15] = <TexVol_0>;
 		MipFilter[15] = Point;
        MaxMipLevel[15] = 0;
        MipMapLODBias[15] = -5;
 		CullMode = None;
 		PixelShaderConstant[1] = <fVolSizes>;        
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def  c0, 16.f, 16.f, 1.f, 2.f
			def  c2, 0.0, 0.0, 0.0, 0.0
            dcl_volume s15
			
			mov r0, v0
			mov r1.xy, c1.zw
			mov r2.x, c0.w
			mul r1.zw, c1, r2.x
			rcp r1.x, r1.x
			rcp r1.y, r1.y
			rcp r1.z, r1.z
			rcp r1.w, r1.w
			mov r2, c2
			mov r3, c2
			mad r2.x, c0.x, r1.x, r1.z
			mad r3.y, c0.y, r1.y, r1.w
			texldd r4, r0, s15, r2, r3
			mov oC0, r4
        };
		VertexShader = <VolTLVS>;
   }
}
