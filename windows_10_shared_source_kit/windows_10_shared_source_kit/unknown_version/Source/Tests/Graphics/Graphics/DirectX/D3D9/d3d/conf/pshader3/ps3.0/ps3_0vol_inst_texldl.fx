#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			mov r0, v0
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			mov r0, v0
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			mov r0, v0
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.0f, 0.0f, 0.0f, 0.f
            dcl_volume s3
			
			texldl r1, c0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol3; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.0f, 0.0f, 0.48f, 0.f
            dcl_volume s3
			
			texldl r1, c0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol3; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.0f, 0.0f, 0.98f, 0.f
            dcl_volume s3
			
			texldl r1, c0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			texldl r1.xyzw, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			texldl r1.xyzw, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			texldl r1.xyzw, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			mov r0, v0
			texldl r1.xyzw, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			mov r0, v0
			texldl r1.xyzw, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceY writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			mov r0, v0
			texldl r1.xyzw, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol3; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.98f, 0.98f, 0.0f, 0.f
            dcl_volume s3
			
			texldl r1.xyzw, c0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol3; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.98f, 0.98f, 0.48f, 0.f
            dcl_volume s3
			
			texldl r1.xyzw, c0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol3; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.98f, 0.98f, 0.98f, 0.f
            dcl_volume s3
			
			texldl r1.xyzw, c0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v0, TexVol3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			texldl_pp r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v0, TexVol3; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			texldl_pp r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v0, TexVol3; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			texldl_pp r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r0, TexVol3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			mov r0, v0
			texldl_pp r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r0, TexVol3; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			mov r0, v0
			texldl_pp r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r0, TexVol3; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			dcl_volume s3
			
			mov r0, v0
			texldl_pp r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c0, TexVol3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.48f, 0.48f, 0.0f, 0.f
            dcl_volume s3
			
			texldl_pp r1, c0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c0, TexVol3; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.48f, 0.48f, 0.48f, 0.f
            dcl_volume s3
			
			texldl_pp r1, c0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c0, TexVol3; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
			def c0, 0.48f, 0.48f, 0.98f, 0.f
            dcl_volume s3
			
			texldl_pp r1, c0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; w = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceX, w = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceY, w = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s3
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; w = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s3
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; sliceX, w = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s3
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; sliceY, w = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s3
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; w = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s3
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; sliceX, w = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s3
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; sliceY, w = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s3
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; w = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s3
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; sliceX, w = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s3
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; sliceY, w = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s3
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; w = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s3
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; sliceX, w = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s3
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; sliceY, w = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s3
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; w = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s3
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; sliceX, w = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s3
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; sliceY, w = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s3
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; w = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s3
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; sliceX, w = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s3
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; sliceY, w = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s3
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MaxMipLevel[3] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MaxMipLevel[3] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MaxMipLevel[3] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 2;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 2;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 2;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MaxMipLevel[3] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MaxMipLevel[3] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 4;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 4;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 4;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MaxMipLevel[3] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MaxMipLevel[3] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 6;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 6;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 6;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; w = 0, MaxMipLevel[3] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r0, v0
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r0, v0
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r0, v0
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; w = 0, MaxMipLevel[3] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r1, v0
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r1, v0
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 1;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r1, v0
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; w = 0, MaxMipLevel[3] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 2;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r2, v0
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 2;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r2, v0
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 2;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r2, v0
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; w = 0, MaxMipLevel[3] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r3, v0
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r3, v0
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 3;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r3, v0
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; w = 0, MaxMipLevel[3] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 4;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r4, v0
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 4;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r4, v0
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 4;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r4, v0
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; w = 0, MaxMipLevel[3] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r5, v0
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r5, v0
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 5;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r5, v0
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; w = 0, MaxMipLevel[3] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 6;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r6, v0
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; sliceX, w = 0, MaxMipLevel[3] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 6;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r6, v0
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; sliceY, w = 0, MaxMipLevel[3] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 6;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r6, v0
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MipMapLODBias[3] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MipMapLODBias[3] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MipMapLODBias[3] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MipMapLODBias[3] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MipMapLODBias[3] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MipMapLODBias[3] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 0, MipMapLODBias[3] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; w = 0, MipMapLODBias[3] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r0, v0
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r0, v0
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r0, v0
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; w = 0, MipMapLODBias[3] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r1, v0
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r1, v0
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r1, v0
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; w = 0, MipMapLODBias[3] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r2, v0
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r2, v0
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r2, v0
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; w = 0, MipMapLODBias[3] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r3, v0
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r3, v0
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r3, v0
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; w = 0, MipMapLODBias[3] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r4, v0
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r4, v0
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r4, v0
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; w = 0, MipMapLODBias[3] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r5, v0
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r5, v0
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r5, v0
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; w = 0, MipMapLODBias[3] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r6, v0
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; sliceX, w = 0, MipMapLODBias[3] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r6, v0
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; sliceY, w = 0, MipMapLODBias[3] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s3
			
			mov r6, v0
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 5, MipMapLODBias[3] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 5, MipMapLODBias[3] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 5, MipMapLODBias[3] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 5, MipMapLODBias[3] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 5, MipMapLODBias[3] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 5, MipMapLODBias[3] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol3; w = 5, MipMapLODBias[3] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s3
			
			texldl r1, v0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			texldl r1, v2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; w = 5, MipMapLODBias[3] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r0, v2
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r0, v2
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r0, v2
			texldl r1, r0, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; w = 5, MipMapLODBias[3] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r1, v2
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r1, v2
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r1, v2
			texldl r1, r1, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; w = 5, MipMapLODBias[3] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r2, v2
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r2, v2
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r2, v2
			texldl r1, r2, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; w = 5, MipMapLODBias[3] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r3, v2
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r3, v2
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r3, v2
			texldl r1, r3, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; w = 5, MipMapLODBias[3] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r4, v2
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r4, v2
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r4, v2
			texldl r1, r4, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; w = 5, MipMapLODBias[3] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r5, v2
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r5, v2
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r5, v2
			texldl r1, r5, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; w = 5, MipMapLODBias[3] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r6, v2
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; sliceX, w = 5, MipMapLODBias[3] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r6, v2
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol3; sliceY, w = 5, MipMapLODBias[3] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[3] = <TexVol_0>;
 		MipFilter[3] = Point;
        MaxMipLevel[3] = 0;
        MipMapLODBias[3] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s3
			
			mov r6, v2
			texldl r1, r6, s3
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol7";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			texldl r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol7; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			texldl r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol7; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			texldl r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			mov r1, v1
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			mov r1, v1
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			mov r1, v1
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol7";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.0f, 0.0f, 0.0f, 1.f
            dcl_volume s7
			
			texldl r1, c1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol7; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.0f, 0.0f, 0.48f, 1.f
            dcl_volume s7
			
			texldl r1, c1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol7; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.0f, 0.0f, 0.98f, 1.f
            dcl_volume s7
			
			texldl r1, c1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol7; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			texldl r1.xyzw, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol7; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			texldl r1.xyzw, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol7; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			texldl r1.xyzw, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			mov r1, v1
			texldl r1.xyzw, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			mov r1, v1
			texldl r1.xyzw, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceY writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			mov r1, v1
			texldl r1.xyzw, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol7; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.98f, 0.98f, 0.0f, 1.f
            dcl_volume s7
			
			texldl r1.xyzw, c1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol7; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.98f, 0.98f, 0.48f, 1.f
            dcl_volume s7
			
			texldl r1.xyzw, c1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol7; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.98f, 0.98f, 0.98f, 1.f
            dcl_volume s7
			
			texldl r1.xyzw, c1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v1, TexVol7";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			texldl_pp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v1, TexVol7; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			texldl_pp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v1, TexVol7; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			texldl_pp r1, v1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r1, TexVol7";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			mov r1, v1
			texldl_pp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r1, TexVol7; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			mov r1, v1
			texldl_pp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r1, TexVol7; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			dcl_volume s7
			
			mov r1, v1
			texldl_pp r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c1, TexVol7";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.48f, 0.48f, 0.0f, 1.f
            dcl_volume s7
			
			texldl_pp r1, c1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c1, TexVol7; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.48f, 0.48f, 0.48f, 1.f
            dcl_volume s7
			
			texldl_pp r1, c1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c1, TexVol7; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
			def c1, 0.48f, 0.48f, 0.98f, 1.f
            dcl_volume s7
			
			texldl_pp r1, c1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; w = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; sliceX, w = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; sliceY, w = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s7
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; w = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s7
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceX, w = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s7
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceY, w = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s7
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; w = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s7
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; sliceX, w = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s7
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; sliceY, w = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s7
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; w = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s7
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; sliceX, w = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s7
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; sliceY, w = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s7
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; w = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s7
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; sliceX, w = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s7
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; sliceY, w = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s7
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; w = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s7
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; sliceX, w = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s7
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; sliceY, w = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s7
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; w = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s7
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; sliceX, w = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s7
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; sliceY, w = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s7
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MaxMipLevel[7] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MaxMipLevel[7] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MaxMipLevel[7] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MaxMipLevel[7] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MaxMipLevel[7] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MaxMipLevel[7] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MaxMipLevel[7] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; w = 0, MaxMipLevel[7] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r0, v0
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r0, v0
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r0, v0
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; w = 0, MaxMipLevel[7] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r1, v0
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r1, v0
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 1;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r1, v0
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; w = 0, MaxMipLevel[7] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r2, v0
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r2, v0
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 2;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r2, v0
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; w = 0, MaxMipLevel[7] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r3, v0
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r3, v0
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 3;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r3, v0
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; w = 0, MaxMipLevel[7] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r4, v0
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r4, v0
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 4;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r4, v0
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; w = 0, MaxMipLevel[7] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r5, v0
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r5, v0
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 5;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r5, v0
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; w = 0, MaxMipLevel[7] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r6, v0
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; sliceX, w = 0, MaxMipLevel[7] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r6, v0
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; sliceY, w = 0, MaxMipLevel[7] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 6;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r6, v0
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MipMapLODBias[7] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MipMapLODBias[7] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MipMapLODBias[7] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MipMapLODBias[7] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MipMapLODBias[7] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MipMapLODBias[7] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 0, MipMapLODBias[7] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; w = 0, MipMapLODBias[7] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r0, v0
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r0, v0
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r0, v0
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; w = 0, MipMapLODBias[7] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r1, v0
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r1, v0
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r1, v0
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; w = 0, MipMapLODBias[7] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r2, v0
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r2, v0
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r2, v0
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; w = 0, MipMapLODBias[7] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r3, v0
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r3, v0
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r3, v0
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; w = 0, MipMapLODBias[7] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r4, v0
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r4, v0
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r4, v0
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; w = 0, MipMapLODBias[7] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r5, v0
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r5, v0
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r5, v0
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; w = 0, MipMapLODBias[7] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r6, v0
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; sliceX, w = 0, MipMapLODBias[7] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r6, v0
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; sliceY, w = 0, MipMapLODBias[7] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s7
			
			mov r6, v0
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 5, MipMapLODBias[7] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 5, MipMapLODBias[7] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 5, MipMapLODBias[7] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 5, MipMapLODBias[7] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 5, MipMapLODBias[7] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 5, MipMapLODBias[7] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol7; w = 5, MipMapLODBias[7] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s7
			
			texldl r1, v0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			texldl r1, v2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; w = 5, MipMapLODBias[7] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r0, v2
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r0, v2
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r0, v2
			texldl r1, r0, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; w = 5, MipMapLODBias[7] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r1, v2
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r1, v2
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r1, v2
			texldl r1, r1, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; w = 5, MipMapLODBias[7] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r2, v2
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r2, v2
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r2, v2
			texldl r1, r2, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; w = 5, MipMapLODBias[7] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r3, v2
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r3, v2
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r3, v2
			texldl r1, r3, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; w = 5, MipMapLODBias[7] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r4, v2
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r4, v2
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r4, v2
			texldl r1, r4, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; w = 5, MipMapLODBias[7] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r5, v2
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r5, v2
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r5, v2
			texldl r1, r5, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; w = 5, MipMapLODBias[7] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r6, v2
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; sliceX, w = 5, MipMapLODBias[7] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r6, v2
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol7; sliceY, w = 5, MipMapLODBias[7] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[7] = <TexVol_0>;
 		MipFilter[7] = Point;
        MaxMipLevel[7] = 0;
        MipMapLODBias[7] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s7
			
			mov r6, v2
			texldl r1, r6, s7
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			mov r2, v2
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			mov r2, v2
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			mov r2, v2
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol12";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.0f, 0.0f, 0.0f, 2.f
            dcl_volume s12
			
			texldl r1, c2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol12; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.0f, 0.0f, 0.48f, 2.f
            dcl_volume s12
			
			texldl r1, c2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol12; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.0f, 0.0f, 0.98f, 2.f
            dcl_volume s12
			
			texldl r1, c2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			texldl r1.xyzw, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			texldl r1.xyzw, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			texldl r1.xyzw, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			mov r2, v2
			texldl r1.xyzw, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			mov r2, v2
			texldl r1.xyzw, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceY writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			mov r2, v2
			texldl r1.xyzw, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol12; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.98f, 0.98f, 0.0f, 2.f
            dcl_volume s12
			
			texldl r1.xyzw, c2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol12; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.98f, 0.98f, 0.48f, 2.f
            dcl_volume s12
			
			texldl r1.xyzw, c2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol12; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.98f, 0.98f, 0.98f, 2.f
            dcl_volume s12
			
			texldl r1.xyzw, c2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v2, TexVol12";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			texldl_pp r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v2, TexVol12; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			texldl_pp r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v2, TexVol12; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			texldl_pp r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r2, TexVol12";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			mov r2, v2
			texldl_pp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r2, TexVol12; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			mov r2, v2
			texldl_pp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r2, TexVol12; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			dcl_volume s12
			
			mov r2, v2
			texldl_pp r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c2, TexVol12";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.48f, 0.48f, 0.0f, 2.f
            dcl_volume s12
			
			texldl_pp r1, c2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c2, TexVol12; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.48f, 0.48f, 0.48f, 2.f
            dcl_volume s12
			
			texldl_pp r1, c2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c2, TexVol12; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
			def c2, 0.48f, 0.48f, 0.98f, 2.f
            dcl_volume s12
			
			texldl_pp r1, c2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; w = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; sliceX, w = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; sliceY, w = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 0.0, 0.0, 0.0, 0.0
            dcl_volume s12
			
			mov r0, v0.xyz
			mov r0.w, c0.x
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; w = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s12
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; sliceX, w = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s12
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; sliceY, w = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 1.0, 1.0, 1.0, 1.0
            dcl_volume s12
			
			mov r1, v0.xyz
			mov r1.w, c0.x
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; w = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s12
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceX, w = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s12
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceY, w = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 2.0, 2.0, 2.0, 2.0
            dcl_volume s12
			
			mov r2, v0.xyz
			mov r2.w, c0.x
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; w = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s12
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; sliceX, w = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s12
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; sliceY, w = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 3.0, 3.0, 3.0, 3.0
            dcl_volume s12
			
			mov r3, v0.xyz
			mov r3.w, c0.x
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; w = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s12
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; sliceX, w = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s12
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; sliceY, w = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 4.0, 4.0, 4.0, 4.0
            dcl_volume s12
			
			mov r4, v0.xyz
			mov r4.w, c0.x
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; w = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s12
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; sliceX, w = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s12
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; sliceY, w = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 5.0, 5.0, 5.0, 5.0
            dcl_volume s12
			
			mov r5, v0.xyz
			mov r5.w, c0.x
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; w = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s12
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; sliceX, w = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s12
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; sliceY, w = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def  c0, 6.0, 6.0, 6.0, 6.0
            dcl_volume s12
			
			mov r6, v0.xyz
			mov r6.w, c0.x
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MaxMipLevel[12] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MaxMipLevel[12] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MaxMipLevel[12] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MaxMipLevel[12] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MaxMipLevel[12] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MaxMipLevel[12] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MaxMipLevel[12] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; w = 0, MaxMipLevel[12] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r0, v0
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r0, v0
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r0, v0
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; w = 0, MaxMipLevel[12] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r1, v0
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r1, v0
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 1;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r1, v0
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; w = 0, MaxMipLevel[12] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r2, v0
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r2, v0
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 2;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r2, v0
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; w = 0, MaxMipLevel[12] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r3, v0
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r3, v0
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 3;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r3, v0
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; w = 0, MaxMipLevel[12] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r4, v0
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r4, v0
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 4;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r4, v0
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; w = 0, MaxMipLevel[12] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r5, v0
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r5, v0
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 5;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r5, v0
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; w = 0, MaxMipLevel[12] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r6, v0
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; sliceX, w = 0, MaxMipLevel[12] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r6, v0
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; sliceY, w = 0, MaxMipLevel[12] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 6;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r6, v0
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MipMapLODBias[12] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MipMapLODBias[12] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MipMapLODBias[12] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MipMapLODBias[12] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MipMapLODBias[12] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MipMapLODBias[12] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 0, MipMapLODBias[12] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; w = 0, MipMapLODBias[12] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r0, v0
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r0, v0
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r0, v0
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; w = 0, MipMapLODBias[12] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r1, v0
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r1, v0
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r1, v0
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; w = 0, MipMapLODBias[12] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r2, v0
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r2, v0
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r2, v0
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; w = 0, MipMapLODBias[12] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r3, v0
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r3, v0
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r3, v0
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; w = 0, MipMapLODBias[12] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r4, v0
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r4, v0
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r4, v0
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; w = 0, MipMapLODBias[12] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r5, v0
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r5, v0
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r5, v0
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; w = 0, MipMapLODBias[12] = 6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r6, v0
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; sliceX, w = 0, MipMapLODBias[12] = 6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r6, v0
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; sliceY, w = 0, MipMapLODBias[12] = 6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = 6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s12
			
			mov r6, v0
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 5, MipMapLODBias[12] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 5, MipMapLODBias[12] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 5, MipMapLODBias[12] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 5, MipMapLODBias[12] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 5, MipMapLODBias[12] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 5, MipMapLODBias[12] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol12; w = 5, MipMapLODBias[12] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v0
            dcl_volume s12
			
			texldl r1, v0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			texldl r1, v2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; w = 5, MipMapLODBias[12] = -0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r0, v2
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r0, v2
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r0, v2
			texldl r1, r0, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; w = 5, MipMapLODBias[12] = -1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r1, v2
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r1, v2
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r1, v2
			texldl r1, r1, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; w = 5, MipMapLODBias[12] = -2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r2, v2
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r2, v2
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r2, v2
			texldl r1, r2, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; w = 5, MipMapLODBias[12] = -3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r3, v2
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r3, v2
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r3, v2
			texldl r1, r3, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; w = 5, MipMapLODBias[12] = -4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r4, v2
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r4, v2
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r4, v2
			texldl r1, r4, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; w = 5, MipMapLODBias[12] = -5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r5, v2
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r5, v2
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r5, v2
			texldl r1, r5, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; w = 5, MipMapLODBias[12] = -6";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r6, v2
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; sliceX, w = 5, MipMapLODBias[12] = -6";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r6, v2
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r6, TexVol12; sliceY, w = 5, MipMapLODBias[12] = -6";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[12] = <TexVol_0>;
 		MipFilter[12] = Point;
        MaxMipLevel[12] = 0;
        MipMapLODBias[12] = -6;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s12
			
			mov r6, v2
			texldl r1, r6, s12
			mov oC0, r1
        };
         
		VertexShader = <VolTLVS>;
   }
}
