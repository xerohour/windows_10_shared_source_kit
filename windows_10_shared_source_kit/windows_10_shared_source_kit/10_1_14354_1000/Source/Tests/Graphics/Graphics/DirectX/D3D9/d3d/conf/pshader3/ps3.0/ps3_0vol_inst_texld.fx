#include "include.fx"

Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0(.xyz), TexVol2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0(.xyz), TexVol2; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0(.xyz), TexVol2; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0.xyz
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.0f, 0.0f, 0.0f, 0.f
            dcl_volume s2
			
			texld r0, c0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol2; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.0f, 0.0f, 0.48f, 0.f
            dcl_volume s2
			
			texld r0, c0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol2; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.0f, 0.0f, 0.98f, 0.f
            dcl_volume s2
			
			texld r0, c0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0.xyzw, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0.xyzw, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0.xyzw, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0.xyzw, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0.xyzw, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0.xyzw, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol2; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.98f, 0.98f, 0.0f, 0.f
            dcl_volume s2
			
			texld r0.xyzw, c0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol2; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.98f, 0.98f, 0.48f, 0.f
            dcl_volume s2
			
			texld r0.xyzw, c0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c0, TexVol2; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.98f, 0.98f, 0.98f, 0.f
            dcl_volume s2
			
			texld r0.xyzw, c0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v0, TexVol2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld_pp r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v0, TexVol2; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld_pp r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v0, TexVol2; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld_pp r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r0, TexVol2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld_pp r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r0, TexVol2; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld_pp r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r0, TexVol2; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld_pp r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c0, TexVol2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.48f, 0.48f, 0.0f, 0.f
            dcl_volume s2
			
			texld_pp r0, c0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c0, TexVol2; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.48f, 0.48f, 0.48f, 0.f
            dcl_volume s2
			
			texld_pp r0, c0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c0, TexVol2; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            def c0, 0.48f, 0.48f, 0.98f, 0.f
            dcl_volume s2
			
			texld_pp r0, c0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 0";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 0";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 0";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 1";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 1";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 1";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 2";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 2";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 2";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 3";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 3";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 3";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 4";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 4";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 4";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; level = 0";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceX, level = 0";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceY, level = 0";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; level = 1";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; sliceX, level = 1";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; sliceY, level = 1";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; level = 2";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; sliceX, level = 2";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; sliceY, level = 2";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; level = 3";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; sliceX, level = 3";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; sliceY, level = 3";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; level = 4";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; sliceX, level = 4";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; sliceY, level = 4";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; level = 5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; sliceX, level = 5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; sliceY, level = 5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MaxMipLevel[2] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MaxMipLevel[2] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MaxMipLevel[2] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MaxMipLevel[2] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 1;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MaxMipLevel[2] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 1;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MaxMipLevel[2] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 1;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MaxMipLevel[2] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MaxMipLevel[2] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MaxMipLevel[2] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MaxMipLevel[2] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 3;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MaxMipLevel[2] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 3;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MaxMipLevel[2] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 3;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MaxMipLevel[2] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MaxMipLevel[2] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MaxMipLevel[2] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MaxMipLevel[2] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 5;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MaxMipLevel[2] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 5;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MaxMipLevel[2] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 5;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; MaxMipLevel[2] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceX, MaxMipLevel[2] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceY, MaxMipLevel[2] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; MaxMipLevel[2] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 1;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; sliceX, MaxMipLevel[2] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 1;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; sliceY, MaxMipLevel[2] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 1;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; MaxMipLevel[2] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; sliceX, MaxMipLevel[2] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; sliceY, MaxMipLevel[2] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 2;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; MaxMipLevel[2] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 3;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; sliceX, MaxMipLevel[2] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 3;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; sliceY, MaxMipLevel[2] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 3;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; MaxMipLevel[2] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; sliceX, MaxMipLevel[2] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; sliceY, MaxMipLevel[2] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 4;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; MaxMipLevel[2] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 5;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; sliceX, MaxMipLevel[2] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 5;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; sliceY, MaxMipLevel[2] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 5;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MipMapLODBias[2] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MipMapLODBias[2] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MipMapLODBias[2] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MipMapLODBias[2] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MipMapLODBias[2] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MipMapLODBias[2] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MipMapLODBias[2] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MipMapLODBias[2] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MipMapLODBias[2] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MipMapLODBias[2] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MipMapLODBias[2] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MipMapLODBias[2] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MipMapLODBias[2] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MipMapLODBias[2] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MipMapLODBias[2] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; MipMapLODBias[2] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, MipMapLODBias[2] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, MipMapLODBias[2] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; MipMapLODBias[2] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceX, MipMapLODBias[2] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceY, MipMapLODBias[2] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; MipMapLODBias[2] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; sliceX, MipMapLODBias[2] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; sliceY, MipMapLODBias[2] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; MipMapLODBias[2] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; sliceX, MipMapLODBias[2] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; sliceY, MipMapLODBias[2] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; MipMapLODBias[2] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; sliceX, MipMapLODBias[2] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; sliceY, MipMapLODBias[2] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; MipMapLODBias[2] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; sliceX, MipMapLODBias[2] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; sliceY, MipMapLODBias[2] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; MipMapLODBias[2] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; sliceX, MipMapLODBias[2] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; sliceY, MipMapLODBias[2] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_0>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 5, MipMapLODBias[2] = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 5, MipMapLODBias[2] = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 5, MipMapLODBias[2] = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 5, MipMapLODBias[2] = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 5, MipMapLODBias[2] = -4";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -4";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -4";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; level = 5, MipMapLODBias[2] = -5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			texld r0, v0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; level = 5, MipMapLODBias[2] = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r0, v0
			texld r0, r0, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; level = 5, MipMapLODBias[2] = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r1, v0
			texld r0, r1, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; level = 5, MipMapLODBias[2] = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r2, v0
			texld r0, r2, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; level = 5, MipMapLODBias[2] = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r3, v0
			texld r0, r3, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; level = 5, MipMapLODBias[2] = -4";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -4";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -4";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r4, v0
			texld r0, r4, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; level = 5, MipMapLODBias[2] = -5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; sliceX, level = 5, MipMapLODBias[2] = -5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol2; sliceY, level = 5, MipMapLODBias[2] = -5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[2] = <TexVol_1>;
 		MipFilter[2] = Point;
        MaxMipLevel[2] = 0;
        MipMapLODBias[2] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s2
			
			mov r5, v0
			texld r0, r5, s2
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			texld r0, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol8; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			texld r0, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol8; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			texld r0, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1(.xyz), TexVol8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1.xyz
            dcl_volume s8
			
			texld r0, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1(.xyz), TexVol8; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1.xyz
            dcl_volume s8
			
			texld r0, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1(.xyz), TexVol8; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1.xyz
            dcl_volume s8
			
			texld r0, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			mov r1, v1
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			mov r1, v1
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			mov r1, v1
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.0f, 0.0f, 0.0f, 1.f
            dcl_volume s8
			
			texld r0, c1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol8; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.0f, 0.0f, 0.48f, 1.f
            dcl_volume s8
			
			texld r0, c1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol8; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.0f, 0.0f, 0.98f, 1.f
            dcl_volume s8
			
			texld r0, c1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol8; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			texld r0.xyzw, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol8; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			texld r0.xyzw, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v1, TexVol8; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			texld r0.xyzw, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			mov r1, v1
			texld r0.xyzw, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			mov r1, v1
			texld r0.xyzw, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			mov r1, v1
			texld r0.xyzw, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol8; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.98f, 0.98f, 0.0f, 1.f
            dcl_volume s8
			
			texld r0.xyzw, c1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol8; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.98f, 0.98f, 0.48f, 1.f
            dcl_volume s8
			
			texld r0.xyzw, c1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c1, TexVol8; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.98f, 0.98f, 0.98f, 1.f
            dcl_volume s8
			
			texld r0.xyzw, c1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v1, TexVol8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			texld_pp r0, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v1, TexVol8; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			texld_pp r0, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v1, TexVol8; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			texld_pp r0, v1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r1, TexVol8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			mov r1, v1
			texld_pp r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r1, TexVol8; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			mov r1, v1
			texld_pp r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r1, TexVol8; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            dcl_volume s8
			
			mov r1, v1
			texld_pp r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c1, TexVol8";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.48f, 0.48f, 0.0f, 1.f
            dcl_volume s8
			
			texld_pp r0, c1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c1, TexVol8; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.48f, 0.48f, 0.48f, 1.f
            dcl_volume s8
			
			texld_pp r0, c1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c1, TexVol8; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord1 v1
            def c1, 0.48f, 0.48f, 0.98f, 1.f
            dcl_volume s8
			
			texld_pp r0, c1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 0";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 0";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 0";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 1";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 1";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 1";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 2";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 2";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 2";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 3";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 3";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 3";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 4";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 4";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 4";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; level = 0";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; sliceX, level = 0";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; sliceY, level = 0";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; level = 1";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceX, level = 1";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceY, level = 1";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; level = 2";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; sliceX, level = 2";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; sliceY, level = 2";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; level = 3";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; sliceX, level = 3";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; sliceY, level = 3";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; level = 4";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; sliceX, level = 4";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; sliceY, level = 4";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; level = 5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; sliceX, level = 5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; sliceY, level = 5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MaxMipLevel[8] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MaxMipLevel[8] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MaxMipLevel[8] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MaxMipLevel[8] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 1;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MaxMipLevel[8] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 1;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MaxMipLevel[8] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 1;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MaxMipLevel[8] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MaxMipLevel[8] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MaxMipLevel[8] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MaxMipLevel[8] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 3;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MaxMipLevel[8] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 3;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MaxMipLevel[8] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 3;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MaxMipLevel[8] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MaxMipLevel[8] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MaxMipLevel[8] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MaxMipLevel[8] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 5;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MaxMipLevel[8] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 5;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MaxMipLevel[8] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 5;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; MaxMipLevel[8] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; sliceX, MaxMipLevel[8] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; sliceY, MaxMipLevel[8] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; MaxMipLevel[8] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 1;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceX, MaxMipLevel[8] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 1;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceY, MaxMipLevel[8] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 1;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; MaxMipLevel[8] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; sliceX, MaxMipLevel[8] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; sliceY, MaxMipLevel[8] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 2;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; MaxMipLevel[8] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 3;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; sliceX, MaxMipLevel[8] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 3;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; sliceY, MaxMipLevel[8] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 3;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; MaxMipLevel[8] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; sliceX, MaxMipLevel[8] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; sliceY, MaxMipLevel[8] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 4;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; MaxMipLevel[8] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 5;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; sliceX, MaxMipLevel[8] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 5;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; sliceY, MaxMipLevel[8] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 5;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MipMapLODBias[8] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MipMapLODBias[8] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MipMapLODBias[8] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MipMapLODBias[8] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MipMapLODBias[8] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MipMapLODBias[8] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MipMapLODBias[8] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MipMapLODBias[8] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MipMapLODBias[8] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MipMapLODBias[8] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MipMapLODBias[8] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MipMapLODBias[8] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MipMapLODBias[8] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MipMapLODBias[8] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MipMapLODBias[8] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; MipMapLODBias[8] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, MipMapLODBias[8] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, MipMapLODBias[8] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; MipMapLODBias[8] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; sliceX, MipMapLODBias[8] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; sliceY, MipMapLODBias[8] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; MipMapLODBias[8] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceX, MipMapLODBias[8] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceY, MipMapLODBias[8] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; MipMapLODBias[8] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; sliceX, MipMapLODBias[8] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; sliceY, MipMapLODBias[8] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; MipMapLODBias[8] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; sliceX, MipMapLODBias[8] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; sliceY, MipMapLODBias[8] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; MipMapLODBias[8] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; sliceX, MipMapLODBias[8] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; sliceY, MipMapLODBias[8] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; MipMapLODBias[8] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; sliceX, MipMapLODBias[8] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; sliceY, MipMapLODBias[8] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_0>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 5, MipMapLODBias[8] = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 5, MipMapLODBias[8] = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 5, MipMapLODBias[8] = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 5, MipMapLODBias[8] = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 5, MipMapLODBias[8] = -4";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -4";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -4";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; level = 5, MipMapLODBias[8] = -5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			texld r0, v0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; level = 5, MipMapLODBias[8] = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r0, v0
			texld r0, r0, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; level = 5, MipMapLODBias[8] = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r1, v0
			texld r0, r1, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; level = 5, MipMapLODBias[8] = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r2, v0
			texld r0, r2, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; level = 5, MipMapLODBias[8] = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r3, v0
			texld r0, r3, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; level = 5, MipMapLODBias[8] = -4";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -4";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -4";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r4, v0
			texld r0, r4, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; level = 5, MipMapLODBias[8] = -5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; sliceX, level = 5, MipMapLODBias[8] = -5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol8; sliceY, level = 5, MipMapLODBias[8] = -5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[8] = <TexVol_1>;
 		MipFilter[8] = Point;
        MaxMipLevel[8] = 0;
        MipMapLODBias[8] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s8
			
			mov r5, v0
			texld r0, r5, s8
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol13";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			texld r0, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol13; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			texld r0, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol13; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			texld r0, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2(.xyz), TexVol13";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2.xyz
            dcl_volume s13
			
			texld r0, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2(.xyz), TexVol13; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2.xyz
            dcl_volume s13
			
			texld r0, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2(.xyz), TexVol13; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2.xyz
            dcl_volume s13
			
			texld r0, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			mov r2, v2
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			mov r2, v2
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			mov r2, v2
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol13";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.0f, 0.0f, 0.0f, 2.f
            dcl_volume s13
			
			texld r0, c2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol13; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.0f, 0.0f, 0.48f, 2.f
            dcl_volume s13
			
			texld r0, c2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol13; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.0f, 0.0f, 0.98f, 2.f
            dcl_volume s13
			
			texld r0, c2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol13; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			texld r0.xyzw, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol13; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			texld r0.xyzw, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v2, TexVol13; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			texld r0.xyzw, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			mov r2, v2
			texld r0.xyzw, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			mov r2, v2
			texld r0.xyzw, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			mov r2, v2
			texld r0.xyzw, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol13; writemask=xyzw";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.98f, 0.98f, 0.0f, 2.f
            dcl_volume s13
			
			texld r0.xyzw, c2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol13; sliceX, writemask=xyzw";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.98f, 0.98f, 0.48f, 2.f
            dcl_volume s13
			
			texld r0.xyzw, c2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : c2, TexVol13; sliceY, writemask=xyzw";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.98f, 0.98f, 0.98f, 2.f
            dcl_volume s13
			
			texld r0.xyzw, c2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v2, TexVol13";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			texld_pp r0, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v2, TexVol13; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			texld_pp r0, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : v2, TexVol13; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			texld_pp r0, v2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r2, TexVol13";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			mov r2, v2
			texld_pp r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r2, TexVol13; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			mov r2, v2
			texld_pp r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : r2, TexVol13; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            dcl_volume s13
			
			mov r2, v2
			texld_pp r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c2, TexVol13";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.48f, 0.48f, 0.0f, 2.f
            dcl_volume s13
			
			texld_pp r0, c2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c2, TexVol13; sliceX";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.48f, 0.48f, 0.48f, 2.f
            dcl_volume s13
			
			texld_pp r0, c2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld_pp : c2, TexVol13; sliceY";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord2 v2
            def c2, 0.48f, 0.48f, 0.98f, 2.f
            dcl_volume s13
			
			texld_pp r0, c2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 0";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 0";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 0";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 1";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 1";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 1";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 2";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 2";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 2";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 3";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 3";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 3";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 4";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 4";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 4";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; level = 0";
	String	Shape = "VolTLZ0";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; sliceX, level = 0";
	String	Shape = "VolTLX0";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; sliceY, level = 0";
	String	Shape = "VolTLY0";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; level = 1";
	String	Shape = "VolTLZ1";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; sliceX, level = 1";
	String	Shape = "VolTLX1";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; sliceY, level = 1";
	String	Shape = "VolTLY1";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; level = 2";
	String	Shape = "VolTLZ2";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceX, level = 2";
	String	Shape = "VolTLX2";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceY, level = 2";
	String	Shape = "VolTLY2";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; level = 3";
	String	Shape = "VolTLZ3";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; sliceX, level = 3";
	String	Shape = "VolTLX3";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; sliceY, level = 3";
	String	Shape = "VolTLY3";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; level = 4";
	String	Shape = "VolTLZ4";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; sliceX, level = 4";
	String	Shape = "VolTLX4";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; sliceY, level = 4";
	String	Shape = "VolTLY4";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; level = 5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; sliceX, level = 5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; sliceY, level = 5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MaxMipLevel[13] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MaxMipLevel[13] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MaxMipLevel[13] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MaxMipLevel[13] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MaxMipLevel[13] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MaxMipLevel[13] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MaxMipLevel[13] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 2;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MaxMipLevel[13] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 2;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MaxMipLevel[13] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 2;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MaxMipLevel[13] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MaxMipLevel[13] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MaxMipLevel[13] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MaxMipLevel[13] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 4;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MaxMipLevel[13] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 4;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MaxMipLevel[13] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 4;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MaxMipLevel[13] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MaxMipLevel[13] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MaxMipLevel[13] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; MaxMipLevel[13] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; sliceX, MaxMipLevel[13] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; sliceY, MaxMipLevel[13] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; MaxMipLevel[13] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; sliceX, MaxMipLevel[13] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; sliceY, MaxMipLevel[13] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 1;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; MaxMipLevel[13] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 2;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceX, MaxMipLevel[13] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 2;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceY, MaxMipLevel[13] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 2;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; MaxMipLevel[13] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; sliceX, MaxMipLevel[13] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; sliceY, MaxMipLevel[13] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 3;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; MaxMipLevel[13] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 4;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; sliceX, MaxMipLevel[13] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 4;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; sliceY, MaxMipLevel[13] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 4;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; MaxMipLevel[13] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; sliceX, MaxMipLevel[13] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; sliceY, MaxMipLevel[13] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 5;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MipMapLODBias[13] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MipMapLODBias[13] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MipMapLODBias[13] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MipMapLODBias[13] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MipMapLODBias[13] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MipMapLODBias[13] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MipMapLODBias[13] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MipMapLODBias[13] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MipMapLODBias[13] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MipMapLODBias[13] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MipMapLODBias[13] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MipMapLODBias[13] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MipMapLODBias[13] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MipMapLODBias[13] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MipMapLODBias[13] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; MipMapLODBias[13] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, MipMapLODBias[13] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, MipMapLODBias[13] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; MipMapLODBias[13] = 0";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; sliceX, MipMapLODBias[13] = 0";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; sliceY, MipMapLODBias[13] = 0";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; MipMapLODBias[13] = 1";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; sliceX, MipMapLODBias[13] = 1";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; sliceY, MipMapLODBias[13] = 1";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; MipMapLODBias[13] = 2";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceX, MipMapLODBias[13] = 2";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceY, MipMapLODBias[13] = 2";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; MipMapLODBias[13] = 3";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; sliceX, MipMapLODBias[13] = 3";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; sliceY, MipMapLODBias[13] = 3";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; MipMapLODBias[13] = 4";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; sliceX, MipMapLODBias[13] = 4";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; sliceY, MipMapLODBias[13] = 4";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; MipMapLODBias[13] = 5";
	String	Shape = "VolTLZ";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; sliceX, MipMapLODBias[13] = 5";
	String	Shape = "VolTLX";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; sliceY, MipMapLODBias[13] = 5";
	String	Shape = "VolTLY";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_0>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = 5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 5, MipMapLODBias[13] = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 5, MipMapLODBias[13] = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 5, MipMapLODBias[13] = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 5, MipMapLODBias[13] = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 5, MipMapLODBias[13] = -4";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -4";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -4";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; level = 5, MipMapLODBias[13] = -5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : v0, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			texld r0, v0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; level = 5, MipMapLODBias[13] = -0";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -0";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r0, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -0";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -0;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r0, v0
			texld r0, r0, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; level = 5, MipMapLODBias[13] = -1";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -1";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r1, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -1";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -1;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r1, v0
			texld r0, r1, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; level = 5, MipMapLODBias[13] = -2";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -2";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r2, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -2";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -2;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r2, v0
			texld r0, r2, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; level = 5, MipMapLODBias[13] = -3";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -3";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r3, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -3";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -3;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r3, v0
			texld r0, r3, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; level = 5, MipMapLODBias[13] = -4";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -4";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r4, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -4";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -4;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r4, v0
			texld r0, r4, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; level = 5, MipMapLODBias[13] = -5";
	String	Shape = "VolTLZ5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; sliceX, level = 5, MipMapLODBias[13] = -5";
	String	Shape = "VolTLX5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}
Technique 
< 
	String	Name = "ps_3_0 texld : r5, TexVol13; sliceY, level = 5, MipMapLODBias[13] = -5";
	String	Shape = "VolTLY5";
>
{
    Pass P0
    {        
		Texture[13] = <TexVol_1>;
 		MipFilter[13] = Point;
        MaxMipLevel[13] = 0;
        MipMapLODBias[13] = -5;
 		CullMode = None;
        VertexShaderConstant[0] = <mFinal>;
		       
        PixelShader =
        asm
        {
			ps_3_0
			dcl_texcoord0 v0
            dcl_volume s13
			
			mov r5, v0
			texld r0, r5, s13
			mov oC0, r0
			
        };
         
		VertexShader = <VolTLVS>;
   }
}